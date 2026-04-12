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
        build-essential \
        make \
        nasm \
        xorriso \
        qemu-system-x86 \
        qemu-utils \
        seabios \
        libncurses6 \
    && rm -rf /var/lib/apt/lists/*

# ── Install cross-compiler sesuai arsitektur host ───────────
#   amd64 : gcc -m32  (butuh gcc-multilib)
#   arm64 : i686-linux-gnu-gcc  (cross-compiler, tanpa -m32)
RUN if [ "$TARGETARCH" = "arm64" ]; then \
        dpkg --add-architecture amd64 && \
        apt-get update && apt-get install -y --no-install-recommends \
            gcc-i686-linux-gnu \
            binutils-i686-linux-gnu \
            libc6-dev-i386-cross \
            grub-pc-bin:amd64 \
            grub-common:amd64 \
        && rm -rf /var/lib/apt/lists/*; \
    else \
        apt-get update && apt-get install -y --no-install-recommends \
            gcc \
            gcc-multilib \
            grub-pc-bin \
            grub-common \
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
   echo '  make                                -> compile + buat ISO' && \
   echo '  make DEMO=-DSYNCHRONIZATION         -> compile + buat ISO untuk tugas Sinkronisasi' && \
   echo '  make DEMO=-DPRIORIY_SCHEDULING      -> compile + buat ISO untuk tugas Priority Scheduling' && \
   echo '  make run                            -> jalankan di QEMU' && \
   echo '  make clean                          -> hapus build' && \
   echo '' && \
   exec bash"]
