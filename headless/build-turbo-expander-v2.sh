#!/usr/bin/env bash
# Собирает Linux-библиотеку один раз и сохраняет её для быстрых правок тестов.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
TESTER_DIR="$ROOT_DIR/lib_tester_new"
CACHE_DIR="$TESTER_DIR/headless/cache/turbo-expander-v2"
IMAGE_NAME="mma-headless-tester:bookworm"

docker build --platform linux/amd64 -t "$IMAGE_NAME" "$TESTER_DIR/headless"
mkdir -p "$CACHE_DIR"

docker run --rm --platform linux/amd64 \
  --mount "type=bind,src=$ROOT_DIR,dst=/work,readonly" \
  --mount "type=bind,src=$CACHE_DIR,dst=/cache" \
  "$IMAGE_NAME" '
    set -eu
    mkdir -p /tmp/source /tmp/build
    tar -C /work/del_new2 --exclude=build --exclude=build-mac --exclude=build-vs-release \
      --exclude=linux-build --exclude=linux-build-bookworm -cf - . | tar -C /tmp/source -xf -
    cmake -S /tmp/source -B /tmp/build -G Ninja -DCMAKE_BUILD_TYPE=Release
    cmake --build /tmp/build --target perm-devices --parallel 2
    cp /tmp/build/del/libperm-devices.so /cache/libperm-devices.so
  '

echo "Кэш обновлён: $CACHE_DIR/libperm-devices.so"
