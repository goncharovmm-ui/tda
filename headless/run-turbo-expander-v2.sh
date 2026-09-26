#!/usr/bin/env bash
# Запускает только тесты, используя кэшированную Linux-библиотеку.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
TESTER_DIR="$ROOT_DIR/lib_tester_new"
TEST_PATH="${1:-$TESTER_DIR/Tests/perm-devices/turbo-expander-v2}"
CACHE_DIR="$TESTER_DIR/headless/cache/turbo-expander-v2"
IMAGE_NAME="mma-headless-tester:bookworm"

test -f "$CACHE_DIR/libperm-devices.so" || {
  echo "Нет кэшированной библиотеки. Сначала выполните ./headless/build-turbo-expander-v2.sh" >&2
  exit 2
}
TEST_PATH="$(cd "$TEST_PATH" && pwd)"
case "$TEST_PATH" in "$ROOT_DIR"/*) TEST_PATH="${TEST_PATH#"$ROOT_DIR"/}" ;; *) exit 2 ;; esac

docker run --rm --platform linux/amd64 \
  --mount "type=bind,src=$ROOT_DIR,dst=/work,readonly" \
  --mount "type=bind,src=$CACHE_DIR,dst=/cache,readonly" \
  "$IMAGE_NAME" '
    set -eu
    cp -a /work/lib_tester_new/linux64/libs /tmp/libs
    cp -a /work/lib_tester_new/win64/libs/dbTDA /tmp/libs/dbTDA
    cp /cache/libperm-devices.so /tmp/libs/libperm-devices.so
    cp /work/lib_tester_new/calculation-tester/tester/bin/console-calculation-tester /tmp/tester
    chmod 755 /tmp/tester
    export LD_LIBRARY_PATH=/tmp/libs
    cd /tmp
    exec /tmp/tester --libdir /tmp/libs --mode recursive "/work/'"$TEST_PATH"'" --threads 1 --verbose
  '
