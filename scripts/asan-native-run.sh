#!/usr/bin/env bash
# asan-native-run.sh — configure + build + run an ASan-instrumented native
# stress test from an ISOLATED output dir (no clobbering the normal Debug exes).
#
# Purposes:
#   1. Configures build/asan-native with ARTIFACT_ROOT=<repo>/artifacts/native-runtime-core-test-asan
#      so sanitizer exes NEVER overwrite the non-instrumented CTest suite's
#      artifacts/native-runtime-core-test/Debug exes (the exe-overwrite confusion
#      documented in gc-n6-liveness-findings-2026-08-25.md §十).
#   2. Copies the MSVC clang_rt.asan_dynamic-x86_64.dll next to the exe — without it
#      the binary fails to load (0xc0000135 / git-bash exit 127).
#   3. Builds and runs the requested scenario under ASan.
#
# Usage:
#   scripts/asan-native-run.sh [scenario_index] [target]
#     scenario_index  default: 2  (scenario C, aggressive young GC — the flaky one)
#     target          default: chaos_gc_stress_test
#
# Note: ASan_OPTIONS suppressions file (asan-suppressions.txt) is intentionally NOT
# set: the MSVC `interceptor_via_fun:` format fails to parse on this ASan and aborts
# the run.  See memory asan-tooling-fixed-for-gc-flakiness.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SCENARIO="${1:-2}"
TARGET="${2:-chaos_gc_stress_test}"
ASAN_BUILD_DIR="${REPO_ROOT}/build/asan-native"
ASAN_ARTIFACTS_ROOT="${REPO_ROOT}/artifacts/native-runtime-core-test-asan"

echo "[asan] repo_root        = ${REPO_ROOT}"
echo "[asan] build dir        = ${ASAN_BUILD_DIR}"
echo "[asan] isolated output  = ${ASAN_ARTIFACTS_ROOT}/Debug"

# ── 1. Configure (idempotent) ────────────────────────────────────────────
# CMake cache uses Windows drive paths (D:/...) while REPO_ROOT here is POSIX
# (/d/...); compare on the artifacts-suffix so either form matches.
if [[ ! -f "${ASAN_BUILD_DIR}/CMakeCache.txt" ]] ||
   ! grep -q "ARTIFACT_ROOT:PATH=.*native-runtime-core-test-asan" "${ASAN_BUILD_DIR}/CMakeCache.txt"; then
    echo "[asan] configuring build/asan-native with isolated ARTIFACT_ROOT ..."
    cmake -S "${REPO_ROOT}" -B "${ASAN_BUILD_DIR}" \
        -DCMAKE_CXX_FLAGS="/fsanitize=address /Zi" \
        -DARTIFACT_ROOT="${ASAN_ARTIFACTS_ROOT}"
else
    echo "[asan] build dir already configured with isolated ARTIFACT_ROOT; skipping configure"
fi

# ── 2. Locate + copy the ASan runtime DLL ────────────────────────────────
ASAN_DLL=""
for cand in \
    "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC" \
    "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC"; do
    ASAN_DLL="$(find "${cand}" -name clang_rt.asan_dynamic-x86_64.dll -path "*/Hostx64/x64/*" 2>/dev/null | sort -V | tail -1)"
    [[ -n "${ASAN_DLL}" ]] && break
done
if [[ -z "${ASAN_DLL}" ]]; then
    echo "[asan] ERROR: clang_rt.asan_dynamic-x86_64.dll not found under MSVC toolchains" >&2
    exit 1
fi
echo "[asan] asan runtime dll = ${ASAN_DLL}"
mkdir -p "${ASAN_ARTIFACTS_ROOT}/Debug"
cp -f "${ASAN_DLL}" "${ASAN_ARTIFACTS_ROOT}/Debug/"
echo "[asan] copied ASan DLL to isolated output dir"

# ── 3. Build ─────────────────────────────────────────────────────────────
echo "[asan] building ${TARGET} (Debug) ..."
cmake --build "${ASAN_BUILD_DIR}" --config Debug --target "${TARGET}"

# ── 4. Run ───────────────────────────────────────────────────────────────
EXE="${ASAN_ARTIFACTS_ROOT}/Debug/${TARGET}.exe"
echo "[asan] running: ${EXE} --scenario ${SCENARIO}"
echo "[asan] (halt_on_error=1 aborts on first memory error)"
exec env ASAN_OPTIONS="halt_on_error=1:abort_on_error=1" "${EXE}" --scenario "${SCENARIO}"
