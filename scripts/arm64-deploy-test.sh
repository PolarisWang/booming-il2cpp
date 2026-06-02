#!/usr/bin/env bash
# ── ARM64 Deploy & Test Script ──────────────────────────────────────────
# Cross-compile (or native-compile) the ARM64 JIT test binary and deploy
# it to a remote ARM64 device for hardware verification.
#
# Usage:
#   ./scripts/arm64-deploy-test.sh <user@host> [optional port]
#
# Environment variables:
#   BUILD_PRESET   CMake preset (default: linux-arm64-smoke)
#                   Use "linux-arm64-native" when running on ARM64 host
#   BINARY_NAME    Binary to deploy (default: chaos_arm64_jit_exec)
#   SSH_PORT       SSH port (default: 22, or second positional arg)
#   SSH_KEY        Path to SSH identity file (optional)
#   REMOTE_DIR     Remote directory (default: ~/chaos-arm64-test)
#   NO_BUILD       Set to 1 to skip the cmake --build step
#   EXTRA_TESTS    Additional test binaries to deploy (space-separated)
#
# Examples:
#   ./scripts/arm64-deploy-test.sh pi@raspberrypi5.local
#   BUILD_PRESET=linux-arm64-native ./scripts/arm64-deploy-test.sh localhost
#   NO_BUILD=1 ./scripts/arm64-deploy-test.sh pi@10.0.0.42
#
# Requirements (host):
#   - cmake 3.25+, rsync, ssh
#   - For cross-compile: aarch64-linux-gnu-g++ (GCC cross toolchain)
#   - For native: gcc-14 (or host native compiler)
#
# Requirements (remote):
#   - Linux aarch64 (Raspberry Pi 5, Rockchip, etc.)
#   - C++ standard libraries (libstdc++ or libc++)
#
# On the remote device, use BUILD_PRESET=linux-arm64-native to compile
# directly instead of cross-compiling + deploying.
# =========================================================================

set -euo pipefail

# ── Config ─────────────────────────────────────────────────────────────
REMOTE_HOST="${1:?Usage: $0 <user@host> [port]}"
SSH_PORT="${2:-${SSH_PORT:-22}}"
BUILD_PRESET="${BUILD_PRESET:-linux-arm64-smoke}"
BINARY_NAME="${BINARY_NAME:-chaos_arm64_jit_exec}"
REMOTE_DIR="${REMOTE_DIR:-~/chaos-arm64-test}"
SSH_KEY="${SSH_KEY:-}"
NO_BUILD="${NO_BUILD:-0}"
EXTRA_TESTS="${EXTRA_TESTS:-}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# ── Locate CMake binary dir from preset ────────────────────────────────
# Parse CMakePresets.json to get the binaryDir for our preset.
get_preset_binary_dir() {
    local preset="$1"
    python3 -c "
import json, sys
with open('$PROJECT_ROOT/CMakePresets.json') as f:
    presets = json.load(f)
for p in presets.get('buildPresets', []):
    if p['name'] == '$preset':
        print(p.get('binaryDir', ''))
        sys.exit(0)
for p in presets.get('configurePresets', []):
    if p['name'] == '$preset':
        print(p.get('binaryDir', ''))
        sys.exit(0)
print('')
sys.exit(1)
" 2>/dev/null || echo "artifacts/presets/${preset#linux-}"
}

BUILD_DIR="$(get_preset_binary_dir "$BUILD_PRESET")"
if [ -z "$BUILD_DIR" ]; then
    echo "ERROR: Cannot determine build directory for preset '$BUILD_PRESET'"
    exit 1
fi
# Resolve relative to project root
if [[ "$BUILD_DIR" != /* ]]; then
    BUILD_DIR="$PROJECT_ROOT/$BUILD_DIR"
fi
BINARY_DIR="$BUILD_DIR/../bin"
BINARY_PATH="$BINARY_DIR/$BINARY_NAME"

# ── Build ──────────────────────────────────────────────────────────────
if [ "$NO_BUILD" != "1" ]; then
    echo "=== Building with preset: $BUILD_PRESET ==="
    echo "    Build dir: $BUILD_DIR"
    cd "$PROJECT_ROOT"
    cmake --preset "$BUILD_PRESET" 2>&1 | tail -3
    cmake --build "$BUILD_DIR" --target "$BINARY_NAME" -j"$(nproc)" 2>&1
    # Build extra targets too
    for extra in $EXTRA_TESTS; do
        cmake --build "$BUILD_DIR" --target "$extra" -j"$(nproc)" 2>&1
    done
    echo "=== Build complete ==="
else
    echo "=== Skipping build (NO_BUILD=1) ==="
fi

# ── Verify binary exists ──────────────────────────────────────────────
if [ ! -f "$BINARY_PATH" ]; then
    echo "ERROR: Binary not found at $BINARY_PATH"
    echo "  Looked in: $BINARY_DIR/"
    ls "$BINARY_DIR" 2>/dev/null || echo "  (directory does not exist)"
    echo "Try rebuilding with NO_BUILD=0 or specify BUILD_PRESET"
    exit 1
fi

BINARY_SIZE=$(stat -c%s "$BINARY_PATH" 2>/dev/null || stat -f%z "$BINARY_PATH" 2>/dev/null)
echo "=== Binary: $BINARY_PATH ($BINARY_SIZE bytes) ==="
file "$BINARY_PATH"

# ── Build rsync file list ─────────────────────────────────────────────
RSYNC_FILES=("$BINARY_PATH")
for extra in $EXTRA_TESTS; do
    extra_path="$BINARY_DIR/$extra"
    if [ -f "$extra_path" ]; then
        RSYNC_FILES+=("$extra_path")
    fi
done

# ── Deploy ─────────────────────────────────────────────────────────────
echo "=== Deploying to $REMOTE_HOST:$REMOTE_DIR ==="
SSH_BASE=()
[ -n "$SSH_KEY" ] && SSH_BASE+=(-i "$SSH_KEY")
SSH_BASE+=(-p "$SSH_PORT")

ssh "${SSH_BASE[@]}" "$REMOTE_HOST" "mkdir -p $REMOTE_DIR"
rsync -avz "${SSH_BASE[@]}" "${RSYNC_FILES[@]}" "$REMOTE_HOST:$REMOTE_DIR/"
echo "=== Deploy complete ==="

# ── Check remote system info ──────────────────────────────────────────
echo "=== Remote system ==="
ssh "${SSH_BASE[@]}" "$REMOTE_HOST" \
    "echo '  Kernel: \$(uname -a)' && \
     echo '  CPU: \$(cat /proc/cpuinfo | grep 'model name' | head -1)' && \
     echo '  CPU cores: \$(nproc)' && \
     echo '  Memory: \$(free -h | grep Mem | awk {print\$2})' && \
     echo '--- ldd ---' && \
     ldd $REMOTE_DIR/$BINARY_NAME 2>&1 || true"

# ── Run test ───────────────────────────────────────────────────────────
echo ""
echo "=== Running $BINARY_NAME on $REMOTE_HOST ==="
echo "    (remote dir: $REMOTE_DIR)"
echo ""

ssh "${SSH_BASE[@]}" "$REMOTE_HOST" \
    "cd $REMOTE_DIR && ./$BINARY_NAME 2>&1" \
    | tee "$PROJECT_ROOT/artifacts/arm64-remote-result.txt"

# Extract summary line
SUMMARY=$(grep -E "^══ Results:" "$PROJECT_ROOT/artifacts/arm64-remote-result.txt" 2>/dev/null || echo "")
if [ -n "$SUMMARY" ]; then
    echo ""
    echo "$SUMMARY"
fi

echo ""
echo "=== Full result saved to artifacts/arm64-remote-result.txt ==="
