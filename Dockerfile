FROM ubuntu:22.04

RUN apt-get update && \
    apt update && \
    apt-get install -y \
      file binutils \
      cmake \
      ninja-build \
      wget \
      gnupg2 \
      build-essential \
      gdb \
      qemu-user \
      gcc-aarch64-linux-gnu \
      binutils-aarch64-linux-gnu
      

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add && \
      echo "deb https://apt.llvm.org/jammy/ llvm-toolchain-jammy-21 main" >> /etc/apt/sources.list.d/llvm.list

RUN apt update && \
      apt install -y llvm-21-dev clang-21
    

WORKDIR /app

