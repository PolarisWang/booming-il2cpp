#!/usr/bin/env bash
# build-tool-package.sh — build chaos-il2cpp as an installable dotnet tool (nupkg).
#
# Produces: artifacts/release/tool/chaos-il2cpp.<version>.nupkg
#
# The nupkg embeds the native SDK (libs + headers + stubs) so that a user can
#   dotnet tool install --global chaos-il2cpp
#   chaos-il2cpp publish MyApp.csproj
# and the tool finds its SDK at AppContext.BaseDirectory/sdk/ (no repo needed).
#
# Steps:
#   1. Build Generator + Driver (Release)
#   2. Build the native SDK preset (libs) if not already built
#   3. Assemble a self-contained sdk/ dir (libs + headers + runtime stubs)
#   4. dotnet pack the Driver → baseline .nupkg
#   5. Inject sdk/ into the nupkg (we append the sdk folder before the tool content)
#
# Usage:
#   ./scripts/build-tool-package.sh [version]
#
# Shared constants from release-config.sh (RC_*).

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"
source "$REPO_ROOT/scripts/release-config.sh"

VERSION_OVERRIDE="${1:-}"
VERSION="$VERSION_OVERRIDE"
if [ -z "$VERSION" ]; then
    VERSION=$(cat "$RC_VERSION_FILE" 2>/dev/null || echo "0.0.0")
fi

PRESET="${RC_PRESET_WIN_X64:-windows-x64-reference}"
TOOL_OUT="$RC_ARTIFACTS_BASE/release/tool"
SDK_STAGE="$TOOL_OUT/sdk"
NUPKG_OUT="$TOOL_OUT"

echo "=== build-tool-package: version=$VERSION preset=$PRESET ==="

# ── 1. Build Generator + Driver ─────────────────────────────────────────
echo "[1/5] Build Generator + Driver (Release)"
dotnet build src/managed/Chaos.IL2CPP.Generator/ --configuration Release --nologo -v q >/dev/null 2>&1 || {
    echo "  ERROR: Generator build failed" >&2; exit 1; }
dotnet build src/managed/Chaos.IL2CPP.Driver/ --configuration Release --nologo -v q >/dev/null 2>&1 || {
    echo "  ERROR: Driver build failed" >&2; exit 1; }

# ── 2. Locate or build the native SDK libs ───────────────────────────────
echo "[2/5] Locate native SDK libs (preset=${PRESET})"
SDK_LIB_SRC="tests/e2e/translation/sdk/$PRESET/lib"
if [ ! -d "$SDK_LIB_SRC" ] || [ -z "$(ls -A "$SDK_LIB_SRC" 2>/dev/null)" ]; then
    echo "  building SDK preset via build_presets.py..."
    python tests/e2e/translation/artifacts/build_presets.py --preset "$PRESET" >/dev/null 2>&1 || {
        echo "  ⚠️  build_presets failed; trying raw cmake preset"; }
fi
if [ ! -d "$SDK_LIB_SRC" ]; then
    echo "  ERROR: SDK libs not found at $SDK_LIB_SRC" >&2
    exit 1
fi

# ── 3. Assemble self-contained sdk/ dir ─────────────────────────────────
echo "[3/5] Assemble sdk/ (libs + headers + stubs)"
rm -rf "$SDK_STAGE"
mkdir -p "$SDK_STAGE/lib" "$SDK_STAGE/include" "$SDK_STAGE/runtime_stubs"

# libs
cp "$SDK_LIB_SRC"/*.lib "$SDK_STAGE/lib/" 2>/dev/null || true
cp "$SDK_LIB_SRC"/*.a "$SDK_STAGE/lib/" 2>/dev/null || true
echo "  libs: $(ls -1 "$SDK_STAGE/lib" | wc -l) files ($(du -sh "$SDK_STAGE/lib" | cut -f1))"

# runtime stubs (block .h + .cpp needed by consumer build)
if [ -d "src/native/runtime-core/runtime_stubs" ]; then
    cp src/native/runtime-core/runtime_stubs/*.h "$SDK_STAGE/runtime_stubs/" 2>/dev/null || true
    cp src/native/runtime-core/runtime_stubs/*.cpp "$SDK_STAGE/runtime_stubs/" 2>/dev/null || true
fi

# Runtime headers — use the SdkEmitter's own CopyRuntimeHeaders logic by
# invoking it via a tiny codegen invocation, or copy the same set of headers
# directly from the repo tree.  This mirrors what SdkEmitter.CopyRuntimeHeaders
# does: src/native/{common,runtime-core,codegen,contracts} → include/.
echo "  copying runtime headers..."
INCLUDE="$SDK_STAGE/include"
mkdir -p "$INCLUDE/chaos/pal" "$INCLUDE/gc" "$INCLUDE/runtime_stubs" "$INCLUDE/reflection" "$INCLUDE/fmt"

# chaos/*.h (common public API surface)
if [ -d "src/native/common/chaos" ]; then
    cp src/native/common/chaos/*.h "$INCLUDE/chaos/" 2>/dev/null || true
fi
# chaos/pal/*.h (platform abstraction)
if [ -d "src/native/pal/chaos/pal" ]; then
    cp src/native/pal/chaos/pal/*.h "$INCLUDE/chaos/pal/" 2>/dev/null || true
fi
# runtime-core top-level headers (chaos_runtime_host.h etc.)
if [ -d "src/native/runtime-core" ]; then
    for h in chaos_runtime_host.h runtime_core.h com_ccw.h module_registry.h \
             abi_manifest.h hotpatch_table.h runtime_vtable.h \
             runtime_instantiation.h reflection_query_model.h \
             load_store_chaos_bridge.h interpreter_entry.h exception_helpers.h \
             thread_state.h forbid_suspend.h memory_domain.h convert.h \
             enum_stubs.h patch_loader.h jit_registration.h \
             ChaosGeneratedRuntimePrelude.h generated_code_compat.h \
             string_table.h reflection_api.h reflection_metadata_impl.h \
             arithmetic_chaos_bridge.h numerics_carriers.h \
             chaos_pch.h; do
        [ -f "src/native/runtime-core/$h" ] && cp "src/native/runtime-core/$h" "$INCLUDE/" 2>/dev/null || true
    done
fi
# gc/*.h
if [ -d "src/native/runtime-core/gc" ]; then
    cp src/native/runtime-core/gc/*.h "$INCLUDE/gc/" 2>/dev/null || true
fi
# codegen bridge headers
if [ -d "src/native/codegen" ]; then
    cp src/native/codegen/*.h "$INCLUDE/" 2>/dev/null || true
fi
# reflection headers
if [ -d "src/native/runtime-core/reflection" ]; then
    cp src/native/runtime-core/reflection/*.h "$INCLUDE/reflection/" 2>/dev/null || true
fi
# fmt headers
if [ -d "third_party/fmt/include/fmt" ]; then
    cp third_party/fmt/include/fmt/*.h "$INCLUDE/fmt/" 2>/dev/null || true
fi
echo "  include: $(find "$INCLUDE" -type f | wc -l) files"

# version marker
echo "$VERSION" > "$SDK_STAGE/version.txt"
echo "  sdk stage size: $(du -sh "$SDK_STAGE" | cut -f1)"

# ── 4. dotnet pack the Driver → nupkg ───────────────────────────────────
echo "[4/5] dotnet pack Driver (version=$VERSION)"
mkdir -p "$NUPKG_OUT"
MSBUILD_PROP="-p:Version=$VERSION -p:PackageVersion=$VERSION"
dotnet pack src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj \
    --configuration Release --output "$NUPKG_OUT" --no-build $MSBUILD_PROP >/dev/null 2>&1 || {
    echo "  ERROR: dotnet pack failed" >&2; exit 1; }

NUPKG="$NUPKG_OUT/chaos-il2cpp.$VERSION.nupkg"
if [ ! -f "$NUPKG" ]; then
    echo "  ERROR: expected nupkg at $NUPKG" >&2
    ls -la "$NUPKG_OUT" >&2
    exit 1
fi
echo "  nupkg: $NUPKG ($(du -h "$NUPKG" | cut -f1))"

# ── 5. Inject sdk/ into the nupkg ────────────────────────────────────────
# A nupkg is a zip. dotnet tool looks for the tool DLL under tools/net8.0/any/.
# We tack sdk/ into the archive root so AppContext.BaseDirectory points at the
# unpacked store dir which contains sdk/ next to the tool.
echo "[5/5] Inject sdk/ into nupkg"
STAGED_BASE="$TOOL_OUT/_stage"
rm -rf "$STAGED_BASE"
mkdir -p "$STAGED_BASE"
# unzip the nupkg (python for cross-platform zip)
python - "$NUPKG" "$STAGED_BASE" "$SDK_STAGE" <<'PYEOF'
import shutil, sys, zipfile, os
nupkg, staged, sdk = sys.argv[1], sys.argv[2], sys.argv[3]
with zipfile.ZipFile(nupkg, 'r') as z:
    z.extractall(staged)
# copy sdk/ into staged root
dst = os.path.join(staged, 'sdk')
if os.path.exists(dst):
    shutil.rmtree(dst)
shutil.copytree(sdk, dst)
# rewrite nupkg from staged
os.remove(nupkg)
with zipfile.ZipFile(nupkg, 'w', zipfile.ZIP_DEFLATED) as z:
    for root, dirs, files in os.walk(staged):
        for f in files:
            full = os.path.join(root, f)
            rel = os.path.relpath(full, staged).replace('\\', '/')
            z.write(full, rel)
print(f"  injected sdk/ into {nupkg} ({os.path.getsize(nupkg)//1024//1024} MB)")
PYEOF

echo ""
echo "== Tool package ready:"
echo "   $NUPKG"
echo "   Install:  dotnet tool install --global --add-source $NUPKG_OUT chaos-il2cpp"
echo "   Run:      chaos-il2cpp publish <app.csproj>"
exit 0