#!/bin/bash
# Simple batch JIT rebuild v3
# Runs builds in batches, waits for all before starting next batch

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
    local timestamp="$(date +%H:%M:%S)"

    rm -rf "$jit_output"
    echo "[$timestamp] BUILD $asm/$slug (${dll_sz}b)..."
    echo "  log: $log_file"

    dotnet exec "$TPG_DLL" generate-dll --jit \
        --dll "$chunk_dir/managed/subjects/CombinedSubjects.dll" \
        --metadata "$chunk_dir/managed/subjects/subjects.metadata.json" \
        --output "$jit_output" \
        --config-tier check --clean \
        > "$log_file" 2>&1

    local rc=$?
    if [ $rc -ne 0 ]; then
        echo "[$(date +%H:%M:%S)] FAIL $asm/$slug (rc=$rc)" | tee -a "$log_file"
        return $rc
    fi

    if [ ! -f "$jit_output/entry-jit.exe" ]; then
        echo "[$(date +%H:%M:%S)] FAIL $asm/$slug: no entry-jit.exe" | tee -a "$log_file"
        return 1
    fi

    cp "$jit_output/entry-jit.exe" "$native_dir/entry-jit.exe"

    local jdata=""
    if [ -f "$jit_output/codegen/generated/aot-core-ir.jdata" ]; then
        jdata="$jit_output/codegen/generated/aot-core-ir.jdata"
    elif [ -f "$jit_output/aot-core-ir.jdata" ]; then
        jdata="$jit_output/aot-core-ir.jdata"
    fi

    if [ -n "$jdata" ]; then
        cp "$jdata" "$native_dir/aot-core-ir.jdata"
        local jsize=$(stat --format=%s "$native_dir/aot-core-ir.jdata")
        echo "[$(date +%H:%M:%S)] OK  $asm/$slug (exe + jdata ${jsize}b)" | tee -a "$log_file"
    else
        echo "[$(date +%H:%M:%S)] WARN $asm/$slug: exe OK, no jdata" | tee -a "$log_file"
    fi
    return 0
}

# Read list and process in batches
batch_num=0
success=0
fail=0
total=$(wc -l < "$LIST")
echo "=== Building $total chunks in batches of $MAX_JOBS ==="
echo ""

while IFS='|' read -r asm slug chunk_dir dll_sz rest; do
    [ -z "$asm" ] && continue

    # Start build in background
    build_one "$asm" "$slug" "$chunk_dir" "$dll_sz" &
    local pid=$!

    # Track PIDs for this batch
    batch_pids+=($pid)
    batch_names+=("$asm/$slug")

    # If batch is full, wait for all
    if [ ${#batch_pids[@]} -ge $MAX_JOBS ]; then
        echo "  [batch] Waiting for ${#batch_pids[@]} builds..."
        local i=0
        for pid in "${batch_pids[@]}"; do
            wait $pid 2>/dev/null
            if [ $? -eq 0 ]; then
                success=$((success + 1))
            else
                fail=$((fail + 1))
            fi
        done
        echo "  [batch] Done: ${success}succeeded, ${fail}failed"
        batch_pids=()
        batch_names=()
    fi
done < "$LIST"

# Wait for remaining
if [ ${#batch_pids[@]} -gt 0 ]; then
    echo "  [final] Waiting for ${#batch_pids[@]} builds..."
    for pid in "${batch_pids[@]}"; do
        wait $pid 2>/dev/null
        if [ $? -eq 0 ]; then
            success=$((success + 1))
        else
            fail=$((fail + 1))
        fi
    done
fi

echo ""
echo "====================================="
echo "DONE: $success OK, $fail FAIL"
echo "====================================="
