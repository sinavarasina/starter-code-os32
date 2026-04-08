# ============================================================
#  Dockerfile – IF-OS Development Environment
#
#  Multi-arch: linux/amd64 + linux/arm64 (Apple M-series)
#  Base: debian:bookworm-slim (~30 MB sebelum package)
#
#  Build:
#    docker build -t ifos-env .
#
#  Build multi-arch & push ke registry:
#    docker buildx build --platform linux/amd64,linux/arm64 \
#      -t <username>/ifos-env:latest --push .
#
#  Run (mount direktori kerja mahasiswa):
#    docker run --rm -it -v $(pwd):/workspace ifos-env
# ============================================================
FROM debian:bookworm-slim

# Hindari prompt interaktif saat apt
ARG DEBIAN_FRONTEND=noninteractive

# Deteksi arsitektur target (diisi otomatis oleh Docker buildx)
ARG TARGETARCH

# ── Install package dasar ────────────────────────────────────
RUN apt-get update && apt-get install -y --no-install-recommends \
        make \
        nasm \
        xorriso \
        grub-pc-bin \
        grub-common \
        qemu-system-x86 \
        qemu-utils \
        libncurses5 \
    && rm -rf /var/lib/apt/lists/*

# ── Install cross-compiler sesuai arsitektur host ───────────
#   amd64 : gcc -m32  (butuh gcc-multilib)
#   arm64 : i686-linux-gnu-gcc  (cross-compiler, tanpa -m32)
RUN if [ "$TARGETARCH" = "arm64" ]; then \
        apt-get update && apt-get install -y --no-install-recommends \
            gcc-i686-linux-gnu \
            binutils-i686-linux-gnu \
        && rm -rf /var/lib/apt/lists/*; \
    else \
        apt-get update && apt-get install -y --no-install-recommends \
            gcc \
            gcc-multilib \
        && rm -rf /var/lib/apt/lists/*; \
    fi

# ── Set env agar Makefile tahu arsitektur host ───────────────
ENV IFOS_ARCH=${TARGETARCH}

# ── Working directory mahasiswa ──────────────────────────────
WORKDIR /workspace

# Tampilkan versi tools saat container pertama kali jalan
CMD ["bash", "-c", \
  "echo '================================================' && \
   echo '  IF-OS Development Environment' && \
   echo '  Arch: '$(uname -m) && \
   echo '================================================' && \
   echo '' && \
   echo 'Perintah:' && \
   echo '  make       -> compile + buat ISO' && \
   echo '  make run   -> jalankan di QEMU' && \
   echo '  make clean -> hapus build' && \
   echo '' && \
   exec bash"]
