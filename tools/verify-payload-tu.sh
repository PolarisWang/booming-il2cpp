#!/bin/bash
# Compile one generated payload translation unit on its own.
#
# WHY THIS EXISTS
#   A full `chunk_pipeline --stages build` currently fails at the MSVC step on
#   C2712 ("Cannot use __try in functions that require object unwinding"), which
#   is a pre-existing defect under separate investigation (KNOWN-ISSUE-1). That
#   failure is in the page files that carry METHOD BODIES.
#
#   The payload translation units that carry DATA (hotpatch tables, reflection
#   dispatch parts, vtable arrays, GC slot map) contain no `__try` and are
#   unaffected. Compiling one directly therefore verifies codegen output that
#   would otherwise be blocked behind an unrelated defect — which matters
#   because payload partitioning changes only affect those data TUs.
#
#   This is NOT a substitute for a full build: it proves a TU is well-formed
#   C++, not that the program links or behaves correctly.
#
# USAGE
#   bash tools/verify-payload-tu.sh <chunk-dir> <tu-file> [<tu-file> ...]
#
#   bash tools/verify-payload-tu.sh \
#       artifacts/foundation-dll/System.Private.CoreLib/chunks/system/native \
#       subjects/native-aot.payload.page-0066.cpp
#
# Exit status is 0 only when every named TU compiles.

set -u

if [ $# -lt 2 ]; then
    echo "usage: $0 <chunk-native-dir> <tu-relative-path> [...]" >&2
    exit 2
fi

CHUNK_DIR="$(cd "$1" && pwd)" || exit 2
shift

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
R="$(cygpath -w "$REPO_ROOT")"
C="$(cygpath -w "$CHUNK_DIR")"

# Include set mirrors the generated project (chaos_entry.vcxproj
# AdditionalIncludeDirectories) — a TU is only compilable against the same
# headers the real build uses.
INC="/I $R\\src\\native\\runtime-core"
INC="$INC /I $R\\src\\native\\runtime-core\\gc"
INC="$INC /I $R\\src\\native\\runtime-core\\runtime_stubs"
INC="$INC /I $R\\src\\native\\common"
INC="$INC /I $R\\src\\native\\bootstrap"
INC="$INC /I $R\\src\\native"
INC="$INC /I $R\\src\\native\\pal"
INC="$INC /I $R\\src\\native\\interpreter"
INC="$INC /I $R\\src\\native\\support"
INC="$INC /I $R\\src\\native\\hot-update"
INC="$INC /I $R\\third_party\\unordered_dense\\include"
INC="$INC /I $R\\cmake"
INC="$INC /I $C\\codegen\\include"
INC="$INC /I $C"

# Flags mirror the project: /std:c++20 (the generated code uses designated
# initializers), /utf-8 (fmt asserts without it), /bigobj (the data TUs carry
# tens of thousands of symbols).
FLAGS="/c /nologo /std:c++20 /utf-8 /bigobj /EHsc"

VSDEVCMD='C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat'

ok=0
fail=0
for tu in "$@"; do
    tu_win="$(cygpath -w "$CHUNK_DIR/$tu")"
    out="$(cmd.exe /c "\"$VSDEVCMD\" -arch=x64 -host_arch=x64 >nul 2>&1 && cl $FLAGS $INC /Fo:\"$(cygpath -w "$CHUNK_DIR")\\tu_verify.obj\" \"$tu_win\"" 2>&1)"

    if printf '%s' "$out" | grep -qE 'error C[0-9]+'; then
        fail=$((fail + 1))
        echo "FAIL $tu"
        printf '%s\n' "$out" | grep -oE 'error C[0-9]+: [^[]*' | head -4 | sed 's/^/     /'
    else
        ok=$((ok + 1))
        echo "OK   $tu"
    fi
done

rm -f "$CHUNK_DIR/tu_verify.obj"

echo "----"
echo "compiled $ok, failed $fail"
[ "$fail" -eq 0 ]
