#!/usr/bin/env bash
# Usage: ./decode_crash.sh 0x08007D04
# Decodes a crash PC address from the Nucleo-H723ZG debug ELF.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ELF="$SCRIPT_DIR/cmake-build-debug/nucleo-h723zg.elf"

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <PC_address> [LR_address ...]"
    echo "Example: $0 0x08007D04 0x08008047"
    exit 1
fi

if [[ ! -f "$ELF" ]]; then
    echo "ERROR: ELF not found at $ELF"
    echo "Build the project first (cmake-build-debug configuration)."
    exit 1
fi

for addr in "$@"; do
    echo "=== $addr ==="
    arm-none-eabi-addr2line -e "$ELF" -f -p -i "$addr"
done

