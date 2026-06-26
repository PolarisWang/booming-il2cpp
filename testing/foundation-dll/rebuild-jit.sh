#!/bin/bash
# Batch JIT rebuild for all stale chunks
# Rebuilds entry-jit.exe + aot-core-ir.jdata for chunks that have it missing

set -e

FOUNDATION="/home/debian/agent/booming-il2cpp/testing/foundation-dll"
TPG_DLL="/home/debian/agent/booming-il2cpp/src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/bin/Release/net8.0/Chaos.IL2CPP.Tools.TestProjectGenerator.dll"
LOG_DIR="/tmp/jit-rebuild-logs"
mkdir -p "$LOG_DIR"

MAX_PARALLEL=3

# Track PIDs
pids=()
chunks_running=()

build_chunk_jit() {
    local asm_name="$1"
    local slug="$2"
    local chunk_dir="$3"
    local timestamp="$(date +%H:%M:%S)"

    local subjects_dll="$chunk_dir/managed/subjects/CombinedSubjects.dll"
    local metadata="$chunk_dir/managed/subjects/subjects.metadata.json"
    local native_dir="$chunk_dir/native"
    local jit_output="$native_dir/build_jit_output"

    local log_file="$LOG_DIR/${asm_name}__${slug}.log"

    # Check prerequisites
    if [ ! -f "$subjects_dll" ]; then
        echo "[$timestamp] SKIP $asm_name/$slug: no CombinedSubjects.dll"
        return 0
    fi
    if [ ! -f "$metadata" ]; then
        echo "[$timestamp] SKIP $asm_name/$slug: no subjects.metadata.json"
        return 0
    fi

    # Clean existing JIT output
    rm -rf "$jit_output"

    echo "[$timestamp] BUILD $asm_name/$slug..."

    # Run TPG JIT build
    dotnet exec "$TPG_DLL" generate-dll --jit \
        --dll "$subjects_dll" \
        --metadata "$metadata" \
        --output "$jit_output" \
        --config-tier check --clean \
        > "$log_file" 2>&1

    local rc=$?
    if [ $rc -ne 0 ]; then
        echo "[$timestamp] FAIL $asm_name/$slug (rc=$rc) — see $log_file"
        return $rc
    fi

    # Copy entry-jit.exe to native dir
    if [ -f "$jit_output/entry-jit.exe" ]; then
        cp "$jit_output/entry-jit.exe" "$native_dir/entry-jit.exe"
    else
        echo "[$timestamp] FAIL $asm_name/$slug: entry-jit.exe not produced"
        return 1
    fi

    # Copy aot-core-ir.jdata to native dir
    local jdata_src="$jit_output/codegen/generated/aot-core-ir.jdata"
    if [ -f "$jdata_src" ]; then
        cp "$jdata_src" "$native_dir/aot-core-ir.jdata"
        local jdata_size=$(stat --format=%s "$native_dir/aot-core-ir.jdata" 2>/dev/null)
        echo "[$timestamp] OK  $asm_name/$slug (exe + jdata ${jdata_size}b)"
    else
        # Try alternate location
        jdata_src="$jit_output/aot-core-ir.jdata"
        if [ -f "$jdata_src" ]; then
            cp "$jdata_src" "$native_dir/aot-core-ir.jdata"
            local jdata_size=$(stat --format=%s "$native_dir/aot-core-ir.jdata" 2>/dev/null)
            echo "[$timestamp] OK  $asm_name/$slug (exe + jdata ${jdata_size}b, alt path)"
        else
            echo "[$timestamp] WARN $asm_name/$slug: entry-jit.exe OK but no jdata found"
        fi
    fi

    return 0
}

# Collect all chunks from partition files
chunks=()
while IFS= read -r pf; do
    [ -z "$pf" ] && continue
    asm_dir=$(dirname "$pf")
    asm_dir=$(dirname "$asm_dir")  # parent of _dll/
    asm_name=$(basename "$asm_dir")

    python3 -c "
import json, sys, os
pf = '$pf'
asm_name = '$asm_name'
try:
    partition = json.loads(open(pf).read())
except Exception as e:
    sys.exit(0)
for chunk in partition.get('chunks', []):
    slug = chunk.get('slug', '')
    if not slug:
        continue
    # Try chunks/{slug} first, then {slug}
    for base in [f'{os.path.dirname(pf)}/../chunks/{slug}', f'{os.path.dirname(pf)}/../{slug}']:
        cd = os.path.normpath(base)
        if os.path.isdir(cd):
            native_jdata = cd + '/native/aot-core-ir.jdata'
            if not os.path.isfile(native_jdata):
                print(f'{asm_name}|{slug}|{cd}')
            break
" 2>/dev/null
done < <(find "$FOUNDATION" -name "namespace-partition.json" -path "*/_dll/*") | sort -u > "$LOG_DIR/rebuild-list.txt"

total=$(wc -l < "$LOG_DIR/rebuild-list.txt")
echo "=== Chunks needing JIT rebuild: $total ==="
echo ""

# Process in batches
batch_num=0
while IFS='|' read -r asm slug chunk_dir; do
    [ -z "$asm" ] && continue

    # Wait if we already have MAX_PARALLEL running
    if [ ${#pids[@]} -ge $MAX_PARALLEL ]; then
        # Wait for any one to finish
        wait -n 2>/dev/null || true
        # Clean up finished PIDs
        new_pids=()
        new_chunks=()
        for i in "${!pids[@]}"; do
            if kill -0 "${pids[$i]}" 2>/dev/null; then
                new_pids+=("${pids[$i]}")
                new_chunks+=("${chunks_running[$i]}")
            fi
        done
        pids=("${new_pids[@]}")
        chunks_running=("${new_chunks[@]}")
    fi

    # Start build in background
    build_chunk_jit "$asm" "$slug" "$chunk_dir" &
    pids+=($!)
    chunks_running+=("$asm/$slug")
    batch_num=$((batch_num + 1))
done < "$LOG_DIR/rebuild-list.txt"

# Wait for remaining builds
echo ""
echo "=== Waiting for ${#pids[@]} remaining builds... ==="
wait

echo ""
echo "=== All JIT builds complete ==="
echo ""

# Summary
success=0
fail=0
skip=0
while IFS='|' read -r asm slug chunk_dir; do
    [ -z "$asm" ] && continue
    native_dir="$chunk_dir/native"
    if [ -f "$native_dir/entry-jit.exe" ] && [ -f "$native_dir/aot-core-ir.jdata" ]; then
        success=$((success + 1))
    elif [ -f "$native_dir/entry-jit.exe" ]; then
        skip=$((skip + 1))
    else
        fail=$((fail + 1))
    fi
done < "$LOG_DIR/rebuild-list.txt"

echo "SUCCESS: $success"
echo "SKIP (no metadata): $skip"
echo "FAIL: $fail"
