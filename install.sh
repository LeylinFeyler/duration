#!/usr/bin/env bash

set -e

BINARY_NAME="duration"
TARGET_BIN="/usr/local/bin/$BINARY_NAME"

MAN_SRC="./man/duration.1"
MAN_DIR="$HOME/.local/share/man/man1"
MAN_TARGET="$MAN_DIR/duration.1"

SOURCE="./duration"

if [ ! -f "$SOURCE" ]; then
    echo "build first: $SOURCE not found"
    exit 1
fi

echo "stopping old version (if running)..."
pkill -f "$BINARY_NAME" 2>/dev/null || true

echo "installing binary to $TARGET_BIN..."
sudo install -m 755 "$SOURCE" "$TARGET_BIN"

echo "installing man page to $MAN_TARGET..."

mkdir -p "$MAN_DIR"
install -m 644 "$MAN_SRC" "$MAN_TARGET"

echo "updating man database (local)..."
mandb -q "$HOME/.local/share/man" 2>/dev/null || true

echo "done: $BINARY_NAME installed"
echo "try: man duration"