#!/usr/bin/env bash
set -e

cd /app

# Sanity check: make sure repo is mounted
if [ ! -f CMakeLists.txt ]; then
  echo "ERROR: /app does not look like the leocc repo."
  exec /bin/bash
fi

echo "[leocc-entry] Using source tree at /app"

# Configure CMake only if build dir isn't set up yet
if [ ! -d build ] || { [ ! -f build/build.ninja ] && [ ! -f build/Makefile ]; }; then
  echo "[leocc-entry] Configuring CMake..."
  mkdir -p build
  cd build
  cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
else
  cd build
fi

echo "[leocc-entry] Building with Ninja..."
ninja

# Copy the freshly built binary to /app so tests can do ./leocc
if [ -f leocc ]; then
  echo "[leocc-entry] Copying leocc to /app..."
  cp leocc /app/
else
  echo "[leocc-entry] WARNING: build finished but leocc not found in build/"
fi

cd /app
# ./test_llvm.sh
# echo "[leocc-entry] Done. Dropping you into /bin/bash in /app."

exec /bin/bash -i