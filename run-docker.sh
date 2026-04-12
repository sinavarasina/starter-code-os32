#!/bin/bash
# =============================================================
#  run-docker.sh – Jalankan IF-OS Development Environment
#
#  File ini sudah disiapkan oleh asisten praktikum.
#
#  Cara pakai:
#    chmod +x run-docker.sh
#    ./run-docker.sh
# =============================================================

IMAGE_LOCAL="ifos-env"
IMAGE_REMOTE="sinanonym/ifos-env:latest"
CONTAINER_NAME="ifos-dev"

# ── 1. Deteksi Container Engine ───────────────────────────────
if command -v podman &> /dev/null; then
    CE="podman"
elif command -v docker &> /dev/null; then
    CE="docker"
else
    echo "Error: Container engine tidak ditemukan."
    echo "Silakan install salah satu dari:"
    echo " - Docker Desktop: https://www.docker.com/products/docker-desktop/"
    echo " - Podman Desktop: https://podman-desktop.io/"
    exit 1
fi

echo ">>> Terdeteksi container engine: [$CE]"

# ── 2. Cek & Siapkan Image ───────────────────────────────────
# Cek image lokal, kalau tidak ada pull dari Hub, kalau gagal build lokal
if ! $CE image inspect "$IMAGE_LOCAL" &> /dev/null; then
    echo ">>> Image lokal tidak ditemukan. Mencoba pull dari registry..."
    
    if $CE pull "docker.io/$IMAGE_REMOTE" &> /dev/null || $CE pull "$IMAGE_REMOTE" &> /dev/null; then
        $CE tag "$IMAGE_REMOTE" "$IMAGE_LOCAL" 2>/dev/null || $CE tag "docker.io/$IMAGE_REMOTE" "$IMAGE_LOCAL"
        echo ">>> Image berhasil di-pull."
    else
        echo ">>> Pull gagal. Membangun image dari Dockerfile (bisa memakan waktu 5-15 menit)..."
        $CE build -t "$IMAGE_LOCAL" "$(dirname "$0")"
        if [ $? -ne 0 ]; then
            echo "Error: Gagal membangun image."
            exit 1
        fi
    fi
fi

echo "================================================"
echo "  IF-OS Development Environment"
echo "  Working directory: $(pwd)"
echo "================================================"
echo ""
echo "Gunakan 'make' dan 'make run' di dalam container."
echo "Ketik 'exit' untuk keluar."
echo ""

# Jalankan container dengan volume mount ke direktori saat ini
$CE run --rm -it \
    --name "$CONTAINER_NAME" \
    -v "$(pwd):/workspace" \
    "$IMAGE_LOCAL"
