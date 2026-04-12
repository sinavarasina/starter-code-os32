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

# Cek apakah Docker tersedia
if ! command -v docker &> /dev/null; then
    echo "Error: Docker tidak ditemukan."
    echo "Install Docker Desktop dari https://www.docker.com/products/docker-desktop/"
    exit 1
fi

# Cek image lokal, kalau tidak ada pull dari Hub, kalau gagal build lokal
if ! docker image inspect "$IMAGE_LOCAL" &> /dev/null; then
    echo ">>> Image lokal tidak ditemukan. Mencoba pull dari Docker Hub..."
    if docker pull "$IMAGE_REMOTE" &> /dev/null; then
        docker tag "$IMAGE_REMOTE" "$IMAGE_LOCAL"
        echo ">>> Image berhasil di-pull."
    else
        echo ">>> Pull gagal. Membangun image dari Dockerfile (bisa memakan waktu 5-10 menit)..."
        docker build -t "$IMAGE_LOCAL" "$(dirname "$0")"
        if [ $? -ne 0 ]; then
            echo "Error: Gagal membangun Docker image."
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
docker run --rm -it \
    --name "$CONTAINER_NAME" \
    -v "$(pwd):/workspace" \
    "$IMAGE_LOCAL"
