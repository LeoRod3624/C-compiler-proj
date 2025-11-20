FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y \
      clang \
      llvm-14-dev \
      cmake \
      ninja-build && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY docker-entrypoint.sh /usr/local/bin/leocc-entry
RUN chmod +x /usr/local/bin/leocc-entry

ENTRYPOINT ["leocc-entry"]
