#!/usr/bin/env bash
# ============================================================================
# run-autotest-generator.sh
# Batch runner for Chaos.IL2CPP AutoTestGenerator
#
# Usage:
#   ./run-autotest-generator.sh                    # Run all known DLLs
#   ./run-autotest-generator.sh --dll <path>       # Run a single DLL
#   ./run-autotest-generator.sh --list             # List known DLLs
# ============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
TOOL_DIR="$SCRIPT_DIR/src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator"
OUTPUT_DIR="$SCRIPT_DIR/output_test/verify"
DOTNET_DIR="/c/Program Files/dotnet/shared/Microsoft.NETCore.App/10.0.6"

# Known high-value BCL DLLs for batch generation
declare -A KNOWN_DLLS
KNOWN_DLLS["SystemSecurityCryptography"]="$DOTNET_DIR/System.Security.Cryptography.dll"
KNOWN_DLLS["SystemTextRegularExpressions"]="$DOTNET_DIR/System.Text.RegularExpressions.dll"
KNOWN_DLLS["SystemIOCompression"]="$DOTNET_DIR/System.IO.Compression.dll"
KNOWN_DLLS["SystemDataCommon"]="$DOTNET_DIR/System.Data.Common.dll"

# Proven high-yield DLLs
KNOWN_DLLS["SystemTextJson"]="$DOTNET_DIR/System.Text.Json.dll"
KNOWN_DLLS["SystemNetHttp"]="$DOTNET_DIR/System.Net.Http.dll"
KNOWN_DLLS["SystemCollectionsImmutable"]="$DOTNET_DIR/System.Collections.Immutable.dll"
KNOWN_DLLS["SystemLinq"]="$DOTNET_DIR/System.Linq.dll"
KNOWN_DLLS["SystemCollectionsConcurrent"]="$DOTNET_DIR/System.Collections.Concurrent.dll"

# Tier 1: Pure math/data processing DLLs (high expected deterministic yield)
KNOWN_DLLS["SystemRuntimeNumerics"]="$DOTNET_DIR/System.Runtime.Numerics.dll"
KNOWN_DLLS["SystemFormatsAsn1"]="$DOTNET_DIR/System.Formats.Asn1.dll"
KNOWN_DLLS["SystemFormatsTar"]="$DOTNET_DIR/System.Formats.Tar.dll"
KNOWN_DLLS["SystemThreadingChannels"]="$DOTNET_DIR/System.Threading.Channels.dll"
KNOWN_DLLS["SystemReflectionMetadata"]="$DOTNET_DIR/System.Reflection.Metadata.dll"

# Tier 2: Moderate expected yield
KNOWN_DLLS["SystemIOPipelines"]="$DOTNET_DIR/System.IO.Pipelines.dll"
KNOWN_DLLS["SystemLinqAsyncEnumerable"]="$DOTNET_DIR/System.Linq.AsyncEnumerable.dll"
KNOWN_DLLS["SystemSecurityAccessControl"]="$DOTNET_DIR/System.Security.AccessControl.dll"
KNOWN_DLLS["SystemDiagnosticsDiagnosticSource"]="$DOTNET_DIR/System.Diagnostics.DiagnosticSource.dll"

# Tier 3: Specialized BCL DLLs (larger, more complex types)
KNOWN_DLLS["SystemPrivateXml"]="$DOTNET_DIR/System.Private.Xml.dll"
KNOWN_DLLS["SystemLinqExpressions"]="$DOTNET_DIR/System.Linq.Expressions.dll"
KNOWN_DLLS["SystemComponentModelTypeConverter"]="$DOTNET_DIR/System.ComponentModel.TypeConverter.dll"
KNOWN_DLLS["SystemReflectionEmit"]="$DOTNET_DIR/System.Reflection.Emit.dll"
KNOWN_DLLS["SystemThreadingTasksDataflow"]="$DOTNET_DIR/System.Threading.Tasks.Dataflow.dll"

# Individual high-value types from System.Private.CoreLib (non-generic)
declare -A PRIVATE_TYPES
PRIVATE_TYPES["String"]="System.String"
PRIVATE_TYPES["Convert"]="System.Convert"
PRIVATE_TYPES["Math"]="System.Math"
PRIVATE_TYPES["Type"]="System.Type"
PRIVATE_TYPES["Array"]="System.Array"
PRIVATE_TYPES["Task"]="System.Threading.Tasks.Task"
PRIVATE_TYPES["Decimal"]="System.Decimal"
PRIVATE_TYPES["DateTime"]="System.DateTime"
PRIVATE_TYPES["TimeSpan"]="System.TimeSpan"
PRIVATE_TYPES["Guid"]="System.Guid"
PRIVATE_TYPES["Enum"]="System.Enum"
PRIVATE_TYPES["BitConverter"]="System.BitConverter"
PRIVATE_TYPES["Random"]="System.Random"
PRIVATE_TYPES["HashCode"]="System.HashCode"
PRIVATE_TYPES["Version"]="System.Version"
PRIVATE_TYPES["Path"]="System.IO.Path"
PRIVATE_TYPES["Directory"]="System.IO.Directory"
PRIVATE_TYPES["Thread"]="System.Threading.Thread"
PRIVATE_TYPES["Interlocked"]="System.Threading.Interlocked"
PRIVATE_TYPES["StringBuilder"]="System.Text.StringBuilder"
PRIVATE_TYPES["Encoding"]="System.Text.Encoding"
PRIVATE_TYPES["Marshal"]="System.Runtime.InteropServices.Marshal"
PRIVATE_TYPES["StreamReader"]="System.IO.StreamReader"
PRIVATE_TYPES["StreamWriter"]="System.IO.StreamWriter"
PRIVATE_TYPES["FileStream"]="System.IO.FileStream"
PRIVATE_TYPES["BufferedStream"]="System.IO.BufferedStream"
PRIVATE_TYPES["BinaryReader"]="System.IO.BinaryReader"
PRIVATE_TYPES["BinaryWriter"]="System.IO.BinaryWriter"
PRIVATE_TYPES["CancellationToken"]="System.Threading.CancellationToken"
PRIVATE_TYPES["CancellationTokenSource"]="System.Threading.CancellationTokenSource"
PRIVATE_TYPES["Console"]="System.Console"
PRIVATE_TYPES["Environment"]="System.Environment"
PRIVATE_TYPES["Stopwatch"]="System.Diagnostics.Stopwatch"
PRIVATE_TYPES["Uri"]="System.Uri"
PRIVATE_TYPES["DateTimeOffset"]="System.DateTimeOffset"
PRIVATE_TYPES["Half"]="System.Half"
PRIVATE_TYPES["Int128"]="System.Int128"
PRIVATE_TYPES["UInt128"]="System.UInt128"
PRIVATE_TYPES["BigInteger"]="System.Numerics.BigInteger"
PRIVATE_TYPES["Complex"]="System.Numerics.Complex"
PRIVATE_TYPES["Regex"]="System.Text.RegularExpressions.Regex"
PRIVATE_TYPES["MathF"]="System.MathF"
PRIVATE_TYPES["BitArray"]="System.Collections.BitArray"

# Generic types (open definitions — tool auto-concretizes with System.Int32)
PRIVATE_TYPES["Task_T"]="System.Threading.Tasks.Task\`1"
PRIVATE_TYPES["ValueTask_T"]="System.Threading.Tasks.ValueTask\`1"
PRIVATE_TYPES["Lazy_T"]="System.Lazy\`1"
PRIVATE_TYPES["Dictionary2"]="System.Collections.Generic.Dictionary\`2"
PRIVATE_TYPES["HashSet_T"]="System.Collections.Generic.HashSet\`1"
PRIVATE_TYPES["Nullable_T"]="System.Nullable\`1"
PRIVATE_TYPES["Memory_T"]="System.Memory\`1"
PRIVATE_TYPES["ReadOnlyMemory_T"]="System.ReadOnlyMemory\`1"
PRIVATE_TYPES["Span_T"]="System.Span\`1"
PRIVATE_TYPES["ReadOnlySpan_T"]="System.ReadOnlySpan\`1"

# Additional generic types
PRIVATE_TYPES["Queue_T"]="System.Collections.Generic.Queue\`1"
PRIVATE_TYPES["Stack_T"]="System.Collections.Generic.Stack\`1"
PRIVATE_TYPES["SortedList2"]="System.Collections.Generic.SortedList\`2"
PRIVATE_TYPES["SortedDictionary2"]="System.Collections.Generic.SortedDictionary\`2"

# Types from other DLLs (handled via SPECIAL_DLL_TYPES with custom DLL)
declare -A SPECIAL_DLL_TYPES
SPECIAL_DLL_TYPES["Uri"]="$DOTNET_DIR/System.Runtime.dll"
SPECIAL_DLL_TYPES["ObservableCollection_T"]="$DOTNET_DIR/System.ObjectModel.dll"

usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  --dll <path>    Run a single DLL (all-types mode)"
    echo "  --list          List known DLLs"
    echo "  --private       Also generate core types from System.Private.CoreLib"
    echo "  --verify        Build all generated projects after generation"
    echo "  --report        Aggregate coverage from output directory into SUMMARY.md"
    echo "  --no-clean      Don't clean existing output directories"
    echo "  --help          Show this help"
    exit 0
}

list_dlls() {
    echo "Known BCL DLLs:"
    for name in "${!KNOWN_DLLS[@]}"; do
        local dll="${KNOWN_DLLS[$name]}"
        if [ -f "$dll" ]; then
            echo "  [OK]   $name  -> $dll"
        else
            echo "  [MISS] $name  -> $dll"
        fi
    done
    echo ""
    echo "Core types (from System.Private.CoreLib):"
    for name in "${!PRIVATE_TYPES[@]}"; do
        echo "  $name  -> ${PRIVATE_TYPES[$name]}"
    done
    echo ""
    echo "Special types (from other DLLs):"
    for name in "${!SPECIAL_DLL_TYPES[@]}"; do
        echo "  $name  -> ${SPECIAL_DLL_TYPES[$name]}"
    done
}

run_dll() {
    local name="$1"
    local dll_path="$2"

    if [ ! -f "$dll_path" ]; then
        echo "[SKIP] $name: DLL not found at $dll_path"
        return 0
    fi

    echo ""
    echo "═══════════════════════════════════════════════"
    echo "  Generating: $name"
    echo "  DLL:        $dll_path"
    echo "═══════════════════════════════════════════════"

    dotnet run --project "$TOOL_DIR" --no-build -- \
        --dll "$dll_path" \
        --all-types \
        --output "$OUTPUT_DIR/$name"
}

run_single_type() {
    local name="$1"
    local type_name="$2"
    local dll_path="${3:-$SINGLE_DLL}"

    if [ ! -f "$dll_path" ]; then
        echo "[SKIP] $name: DLL not found at $dll_path"
        return 0
    fi

    echo ""
    echo "═══════════════════════════════════════════════"
    echo "  Generating: $name ($type_name)"
    echo "  DLL:        $dll_path"
    echo "═══════════════════════════════════════════════"

    # Sanitize type name for directory name
    local dir_name=$(echo "$type_name" | sed 's/[`\[\],;:=]/_/g' | sed 's/__*/_/g' | sed 's/^_//' | sed 's/_$//')

    dotnet run --project "$TOOL_DIR" --no-build -- \
        --dll "$dll_path" \
        --type "$type_name" \
        --output "$OUTPUT_DIR/$dir_name"
}

build_project() {
    local project_dir="$1"
    if [ -f "$project_dir"/*.csproj ]; then
        for csproj in "$project_dir"/*.csproj; do
            echo "  Building: $(basename "$csproj")"
            dotnet build "$csproj" -p:DefineConstants=VERIFY -nologo 2>&1 | \
                grep -E "(error|Build succeeded|Build FAILED)" || true
        done
    fi
}

# ── Main ──
DO_LIST=false
DO_PRIVATE=false
DO_VERIFY=false
DO_REPORT=false
DO_CLEAN=true
SINGLE_DLL=""

while [ $# -gt 0 ]; do
    case "$1" in
        --dll) shift; SINGLE_DLL="$1"; shift ;;
        --list) DO_LIST=true; shift ;;
        --private) DO_PRIVATE=true; shift ;;
        --verify) DO_VERIFY=true; shift ;;
        --report) DO_REPORT=true; shift ;;
        --no-clean) DO_CLEAN=false; shift ;;
        --help) usage ;;
        *) echo "Unknown option: $1"; usage ;;
    esac
done

if [ "$DO_LIST" = true ]; then
    list_dlls
    exit 0
fi

# First build the tool
echo "Building AutoTestGenerator..."
dotnet build "$TOOL_DIR" --force -nologo | tail -3

if [ -n "$SINGLE_DLL" ]; then
    # Single DLL mode
    name=$(basename "$SINGLE_DLL" .dll)
    run_dll "$name" "$SINGLE_DLL"
else
    # Batch mode: run all known DLLs
    for name in "${!KNOWN_DLLS[@]}"; do
        run_dll "$name" "${KNOWN_DLLS[$name]}"
    done

    if [ "$DO_PRIVATE" = true ]; then
        echo ""
        echo "═══════════════════════════════════════════════"
        echo "  Generating core types from System.Private.CoreLib"
        echo "═══════════════════════════════════════════════"
        SINGLE_DLL="$DOTNET_DIR/System.Private.CoreLib.dll"
        for name in "${!PRIVATE_TYPES[@]}"; do
            type_name="${PRIVATE_TYPES[$name]}"
            # Check if this type has a custom DLL mapping
            custom_dll="${SPECIAL_DLL_TYPES[$name]:-}"
            if [ -n "$custom_dll" ]; then
                run_single_type "$name" "$type_name" "$custom_dll"
            else
                run_single_type "$name" "$type_name"
            fi
        done
    fi
fi

# Optional verify: build all generated projects
if [ "$DO_VERIFY" = true ]; then
    echo ""
    echo "═══════════════════════════════════════════════"
    echo "  Verifying generated projects..."
    echo "═══════════════════════════════════════════════"
    total=0; passed=0; failed=0
    for proj_dir in "$OUTPUT_DIR"/*/; do
        build_project "$proj_dir" || true
    done
    # Also check subdirs (--all-types mode creates subdirs per type)
    for proj_dir in "$OUTPUT_DIR"/*/*/; do
        build_project "$proj_dir" || true
    done
    echo ""
    echo "Verify complete."
fi

# Generate coverage report
if [ "$DO_REPORT" = true ]; then
    echo ""
    echo "═══════════════════════════════════════════════"
    echo "  Generating coverage report..."
    echo "═══════════════════════════════════════════════"
    dotnet run --project "$TOOL_DIR" --no-build -- \
        --report "$OUTPUT_DIR"
fi

echo ""
echo "Done."
