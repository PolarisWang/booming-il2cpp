#!/bin/bash
# Batch JIT rebuild — runs N builds in parallel, waits for all before next batch
# Usage: rebuild-jit-v2.sh [parallel_jobs]

MAX_JOBS=${1:-3}
FOUNDATION="/home/debian/agent/booming-il2cpp/testing/foundation-dll"
TPG_DLL="/home/debian/agent/booming-il2cpp/src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/bin/Release/net8.0/Chaos.IL2CPP.Tools.TestProjectGenerator.dll"
LOG_DIR="/tmp/jit-rebuild-logs"
mkdir -p "$LOG_DIR"

# ── Step 1: Build the rebuild list ──
LIST="$LOG_DIR/rebuild-list-v2.txt"
: > "$LIST"

while IFS= read -r pf; do
    [ -z "$pf" ] && continue
    asm_dir=$(dirname "$pf")
    asm_dir=$(dirname "$asm_dir")
    asm_name=$(basename "$asm_dir")

    python3 -c "
import json, sys, os
pf = '$pf'
try:
    partition = json.loads(open(pf).read())
except:
    sys.exit(0)
for chunk in partition.get('chunks', []):
    slug = chunk.get('slug', '')
    if not slug: continue
    for base in [f'{os.path.dirname(pf)}/../chunks/{slug}', f'{os.path.dirname(pf)}/../{slug}']:
        cd = os.path.normpath(base)
        if os.path.isdir(cd):
            native_jdata = cd + '/native/aot-core-ir.jdata'
            if not os.path.isfile(native_jdata):
                print(f'{chr(ord(as_cii[0]))}|{slug}|{cd}')
            break
" 2>/dev/null
} < <(find "$FOUNDATION" -name "namespace-partition.json" -path "*/_dll/*")

# Actually use python directly for list generation
python3 -c "
import json, os
foundation = '$FOUNDATION'
entries = []
for pf_file in os.popen(f'find {foundation} -name namespace-partition.json -path \"*/_dll/*\"').read().strip().split():
    if not pf_file: continue
    asm_name = os.path.basename(os.path.dirname(os.path.dirname(pf_file)))
    try:
        partition = json.loads(open(pf_file).read())
    except:
        continue
    for chunk in partition.get('chunks', []):
        slug = chunk.get('slug', '')
        if not slug: continue
        for base in [f'{os.path.dirname(pf_file)}/../chunks/{slug}', f'{os.path.dirname(pf_file)}/../{slug}']:
            cd = os.path.normpath(base)
            if os.path.isdir(cd):
                native_jdata = cd + '/native/aot-core-ir.jdata'
                if not os.path.isfile(native_jdata):
                    # Check prerequisites
                    dll = cd + '/managed/subjects/CombinedSubjects.dll'
                    meta = cd + '/managed/subjects/subjects.metadata.json'
                    if os.path.isfile(dll) and os.path.isfile(meta):
                        exe_size = os.path.getsize(dll)
                        meta_size = os.path.getsize(meta)
                        entries.append((asm_name, slug, cd, exe_size, meta_size))
                    else:
                        print(f'SKIP {asm_name}/{slug}: missing DLL or metadata')
                break

entries.sort(key=lambda x: x[3])  # sort by DLL size
with open('$LIST', 'w') as f:
    for asm, slug, cd, dll_sz, meta_sz in entries:
        f.write(f'{asm}|{slug}|{cd}|{dll_sz}|{meta_sz}\n')
print(f'Buildable chunks: {len(entries)}')
"

echo "─────────────────────────────────────"

# ── Step 2: Process in batches ──
batch=0
success=0
fail=0
skip=0

process_batch() {
    local pids=()
    local names=()

    while IFS='|' read -r asm slug chunk_dir dll_sz meta_sz; do
        [ -z "$asm" ] && continue

        # Wait if batch is full
        if [ ${#pids[@]} -ge $MAX_JOBS ]; then
            # Wait for ALL in current batch
            for pid in "${pids[@]}"; do
                wait $pid 2>/dev/null
                local rc=$?
                if [ $rc -eq 0 ]; then
                    success=$((success + 1))
                else
                    fail=$((fail + 1))
                fi
            done
            pids=()
            names=()
        fi

        # Launch build
        (
            native_dir="$chunk_dir/native"
            jit_output="$native_dir/build_jit_output"
            log_file="$LOG_DIR/${asm}__${slug}.log"
            timestamp="$(date +%H:%M:%S)"

            rm -rf "$jit_output"

            echo "[$timestamp] BUILD $asm/$slug (dll=${dll_sz}b)..."

            dotnet exec "$TPG_DLL" generate-dll --jit \
                --dll "$chunk_dir/managed/subjects/CombinedSubjects.dll" \
                --metadata "$chunk_dir/managed/subjects/subjects.metadata.json" \
                --output "$jit_output" \
                --config-tier check --clean \
                > "$log_file" 2>&1

            local rc=$?
            if [ $rc -ne 0 ]; then
                echo "[$(date +%H:%M:%S)] FAIL $asm/$slug (rc=$rc)"
                exit $rc
            fi

            # Copy exe
            if [ -f "$jit_output/entry-jit.exe" ]; then
                cp "$jit_output/entry-jit.exe" "$native_dir/entry-jit.exe"
            else
                echo "[$(date +%H:%M:%S)] FAIL $asm/$slug: no entry-jit.exe"
                exit 1
            fi

            # Copy jdata
            jdata_src="$jit_output/codegen/generated/aot-core-ir.jdata"
            if [ -f "$jdata_src" ]; then
                cp "$jdata_src" "$native_dir/aot-core-ir.jdata"
                local jsize=$(stat --format=%s "$native_dir/aot-core-ir.jdata" 2>/dev/null)
                echo "[$(date +%H:%M:%S)] OK  $asm/$slug (exe + jdata ${jsize}b)"
            else
                echo "[$(date +%H:%M:%S)] WARN $asm/$slug: no jdata"
            fi
            exit 0
        ) &
        pids+=($!)
        names+=("$asm/$slug")
    done < "$LIST"

    # Wait for remaining in final batch
    for i in "${!pids[@]}"; do
        wait ${pids[$i]} 2>/dev/null
        local rc=$?
        if [ $rc -eq 0 ]; then
            success=$((success + 1))
        else
            fail=$((fail + 1))
        fi
    done
}

process_batch

echo ""
echo "====================================="
echo "SUMMARY: $success OK, $fail FAIL, $skip SKIP"
echo "====================================="

# Verify jdata status
count_jdata=0
for pf in $(find "$FOUNDATION" -name "namespace-partition.json" -path "*/_dll/*"); do
    [ -z "$pf" ] && continue
    python3 -c "
import json, os
pf = '$pf'
try:
    partition = json.loads(open(pf).read())
except:
    exit(0)
for chunk in partition.get('chunks', []):
    slug = chunk.get('slug', '')
    if not slug: continue
    for base in [f'{os.path.dirname(pf)}/../chunks/{slug}', f'{os.path.dirname(pf)}/../{slug}']:
        cd = os.path.normpath(base)
        if os.path.isdir(cd):
            jdata = cd + '/native/aot-core-ir.jdata'
            exe = cd + '/native/entry-jit.exe'
            j = os.path.isfile(jdata)
            e = os.path.isfile(exe)
            if not j:
                m = 'no jdata'
                if not e: m = 'no exe'
                print(f'  MISSING: {os.path.basename(os.path.dirname(os.path.dirname(pf)))}/{slug}: {m}')
            break
" 2>/dev/null
done
