#!/bin/sh
# Compile le firmware dans ~/build-cpas : disque Linux de WSL, bien plus rapide que /mnt/c.
# Chemins par defaut : ceux installes par l'extension Raspberry Pi Pico de VS Code (~/.pico-sdk).
set -e
SDK="$HOME/.pico-sdk"
export PICO_SDK_PATH="${PICO_SDK_PATH:-$SDK/sdk/2.3.1}"
export PICO_TOOLCHAIN_PATH="${PICO_TOOLCHAIN_PATH:-$SDK/toolchain/15_2_Rel1}"
export PATH="$SDK/cmake/v4.3.4/bin:$SDK/ninja/v1.13.2:$PATH"

SOURCES="$(cd "$(dirname "$0")" && pwd)"
BUILD="$HOME/build-cpas"

cmake -S "$SOURCES" -B "$BUILD" -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -Dpicotool_DIR="$SDK/picotool/2.3.1/picotool" \
    -Dpioasm_DIR="$SDK/tools/2.3.1/pioasm"
cmake --build "$BUILD"
echo "Firmware : $BUILD/cpas_envoi.uf2"
