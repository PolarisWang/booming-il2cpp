#!/usr/bin/env bash
# ─── ARM64 QEMU Development Environment Setup ───────────────────────────
# Installs QEMU user-mode emulation + aarch64 cross-compilation toolchain.
# Idempotent: safe to re-run.
#
# Usage:
#   sudo ./scripts/setup-qemu-arm64.sh
#
# Verifies:
#   - qemu-aarch64 --version
#   - aarch64-linux-gnu-gcc --version
# =========================================================================

set -euo pipefail

SCRIPT_NAME="$(basename "$0")"
PACKAGES=()

echo "[${SCRIPT_NAME}] Installing ARM64 QEMU + cross toolchain..."

# ── Detect package manager ──────────────────────────────────────────────
if command -v apt-get &>/dev/null; then
    PKG_MGR="apt"
elif command -v dnf &>/dev/null; then
    PKG_MGR="dnf"
elif command -v yum &>/dev/null; then
    PKG_MGR="yum"
elif command -v brew &>/dev/null; then
    PKG_MGR="brew"
else
    echo "[${SCRIPT_NAME}] ERROR: No supported package manager found (apt/dnf/yum/brew)."
    echo "  Please install manually: qemu-user + aarch64 cross-compiler."
    exit 1
fi

# ── QEMU user-mode ──────────────────────────────────────────────────────
if command -v qemu-aarch64 &>/dev/null; then
    echo "[${SCRIPT_NAME}]   qemu-aarch64 already installed: $(qemu-aarch64 --version | head -1)"
else
    echo "[${SCRIPT_NAME}]   Installing qemu-user..."
    case "${PKG_MGR}" in
        apt) PACKAGES+=(qemu-user qemu-user-static binfmt-support) ;;
        dnf|yum) PACKAGES+=(qemu-user) ;;
        brew) PACKAGES+=(qemu) ;;
    esac
fi

# ── AArch64 cross-toolchain ─────────────────────────────────────────────
if command -v aarch64-linux-gnu-gcc &>/dev/null; then
    echo "[${SCRIPT_NAME}]   Cross-compiler already installed: $(aarch64-linux-gnu-gcc --version | head -1)"
else
    echo "[${SCRIPT_NAME}]   Installing aarch64 cross-compiler..."
    case "${PKG_MGR}" in
        apt) PACKAGES+=(gcc-aarch64-linux-gnu g++-aarch64-linux-gnu) ;;
        dnf|yum) PACKAGES+=(gcc-aarch64-linux-gnu gcc-c++-aarch64-linux-gnu) ;;
        brew) PACKAGES+=(aarch64-elf-gcc) ;;
    esac
fi

# ── Install ─────────────────────────────────────────────────────────────
if [ ${#PACKAGES[@]} -gt 0 ]; then
    echo "[${SCRIPT_NAME}]   Installing packages: ${PACKAGES[*]}"
    case "${PKG_MGR}" in
        apt)
            apt-get update -qq
            apt-get install -y -qq "${PACKAGES[@]}"
            ;;
        dnf)
            dnf install -y "${PACKAGES[@]}"
            ;;
        yum)
            yum install -y "${PACKAGES[@]}"
            ;;
        brew)
            brew install "${PACKAGES[@]}"
            ;;
    esac
    echo "[${SCRIPT_NAME}]   Installation complete."
fi

# ── Final verification ──────────────────────────────────────────────────
echo ""
echo "[${SCRIPT_NAME}] === Verification ==="
echo -n "  qemu-aarch64: "
if command -v qemu-aarch64 &>/dev/null; then
    echo "$(qemu-aarch64 --version | head -1)"
else
    echo "NOT FOUND"
fi

echo -n "  aarch64-linux-gnu-gcc: "
if command -v aarch64-linux-gnu-gcc &>/dev/null; then
    echo "$(aarch64-linux-gnu-gcc --version | head -1)"
else
    echo "NOT FOUND"
fi

echo -n "  aarch64-linux-gnu-g++: "
if command -v aarch64-linux-gnu-g++ &>/dev/null; then
    echo "$(aarch64-linux-gnu-g++ --version | head -1)"
else
    echo "NOT FOUND"
fi

echo ""
echo "[${SCRIPT_NAME}] Done."
echo "  Next: cmake --preset linux-arm64-smoke"
echo "        cmake --build artifacts/presets/linux-arm64-smoke -j\$(nproc)"
echo "        ctest --test-dir artifacts/presets/linux-arm64-smoke -L arm64 -V"
