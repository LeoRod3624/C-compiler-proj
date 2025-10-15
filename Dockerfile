# Use Ubuntu as the base image
FROM ubuntu:22.04

# Set environment variables to avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive
ENV TERM=xterm-256color

# Update package lists and install essential tools
RUN apt-get update && apt-get install -y \
    bash \
    clang \
    make \
    gdb \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /workspace

# Copy the project files into the container
COPY . /workspace

# Set the default command to bash
CMD ["/bin/bash"]
