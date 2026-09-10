#!/bin/bash
# Simple batch JIT rebuild v4
# Runs builds in batches of N, waits for all before starting next batch

MAX_JOBS=3
FOUNDATION="/home/debian/agent/booming-il2cpp/testing/foundation-dll"
TPG_DLL="/home/debian/agent/booming-il2cpp/src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/bin/Release/net8.0/Chaos.IL2CPP.Tools.TestProjectGenerator.dll"
LIST="/tmp/jit-rebuild-logs/rebuild-list-v3.txt"
LOG_DIR="/tmp/jit-rebuild-logs"

build_one() {
    local asm="$1" slug="$2" chunk_dir="$3" dll_sz="$4"
    local native_dir="$chunk_dir/native"
    local jit_output="$native_dir/build_jit_output"
    local log_file="$LOG_DIR/${asm}__${slug}.build.log"

    rm -rf "$jit_output"

    dotnet exec "$TPG_DLL" generate-dll --jit \
        --dll "$chunk_dir/managed/subjects/CombinedSubjects.dll" \
        --metadata "$chunk_dir/managed/subjects/subjects.metadata.json" \
        --output "$jit_output" \
        --config-tier check --clean \
        > "$log_file" 2>&1

    local rc=$?
    if [ $rc -ne 0 ]; then
        return $rc
    fi

    if [ ! -f "$jit_output/entry-jit.exe" ]; then
        return 2
    fi
    cp "$jit_output/entry-jit.exe" "$native_dir/entry-jit.exe"

    if [ -f "$jit_output/codegen/generated/aot-core-ir.jdata" ]; then
        cp "$jit_output/codegen/generated/aot-core-ir.jdata" "$native_dir/aot-core-ir.jdata"
    elif [ -f "$jit_output/aot-core-ir.jdata" ]; then
        cp "$jit_output/aot-core-ir.jdata" "$native_dir/aot-core-ir.jdata"
    fi
    return 0
}

total=$(wc -l < "$LIST")
echo "=== Building $total chunks in batches of $MAX_JOBS ==="

if [ "$total" -eq 0 ]; then
    echo "Nothing to build."
    exit 0
fi

batch_pids=()
batch_names=()
success=0
fail=0
count=0

while IFS='|' read -r asm slug chunk_dir dll_sz rest; do
    [ -z "$asm" ] && continue
    count=$((count + 1))

    build_one "$asm" "$slug" "$chunk_dir" "$dll_sz" &
    pid=$!
    batch_pids+=($pid)
    batch_names+=("$asm/$slug:${dll_sz}b")

    # If batch is full, wait
    if [ ${#batch_pids[@]} -ge $MAX_JOBS ] || [ $count -eq $total ]; then
        echo "  [batch ${count}/${total}] Waiting for ${#batch_pids[@]} builds..."
        for i in "${!batch_pids[@]}"; do
            pid="${batch_pids[$i]}"
            name="${batch_names[$i]}"
            wait $pid 2>/dev/null
            rc=$?
            if [ $rc -eq 0 ]; then
                echo "  OK  $name"
                success=$((success + 1))
            else
                echo "  FAIL $name (rc=$rc)"
                fail=$((fail + 1))
            fi
        done
        batch_pids=()
        batch_names=()
    fi
done < "$LIST"

echo ""
echo "====================================="
echo "DONE: $success OK, $fail FAIL out of $total"
echo "====================================="
