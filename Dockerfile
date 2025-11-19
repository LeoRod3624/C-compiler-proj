# Always the same base image name; the *architecture* is chosen at build time
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
      build-essential \
      clang \
      llvm-14-dev \
      cmake \
      ninja-build && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the whole repo into the image
COPY . .

# Configure + build with CMake/Ninja
RUN mkdir -p build && \
    cd build && \
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. && \
    ninja && \
    # copy built leocc next to tests at /app/leocc
    cp ./leocc ..

# Set a default backend for people who forget the env var.
ENV LEO_BACKEND=llvm

# Default command when you just `docker run -it leocc-arm64`
CMD ["/bin/bash"]

