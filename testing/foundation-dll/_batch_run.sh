#!/bin/bash
# Batch-run verification pipelines for all valid foundation-dll families.
# Runs CONCURRENCY families at a time (default 2).
CONCURRENCY=${1:-2}
BASE="D:/agent/booming-il2cpp"
LOG_DIR="$BASE/testing/foundation-dll/results"
mkdir -p "$LOG_DIR"

# Valid families with contracts (sorted)
FAMILIES=(
    attributes-custom-metadata
    boxing-unboxing-casts
    buffer-memory
    ccw-basic
    codegen-edge-cases
    collections-generic-core
    convert-char
    custom-generics
    delegate-core-invocation
    dispatch-basic
    eh-stress
    error-info-basic
    exception-throw-diagnostics
    garbage-collection
    generic-nullable-value
    generics-stress
    globalization-culture
    guid-random-hashcode
    interface-dispatch
    interface-dispatch,pinvoke-dllimport,primitive-numeric-conversions-core
    io-streams-basics
    math-numerics
    object-equality-identity
    pinvoke-dllimport
    pinvoke-stress
    primitive-numeric-conversions
    primitive-numeric-conversions-core
    rcw-basic
    reflection-activation
    reflection-assembly
    reflection-binding
    reflection-field-property
    reflection-generics
    reflection-member-basics
    reflection-member-complete
    reflection-module
    reflection-parameters
    reflection-type
    runtime-compiler-services
    snapshot-prover
    span-memory-buffers
    string-char-text-core
    string-construction
    threading-monitor-interlocked
    threading-sync
    threading-tasks-primitives
    threading-thread-basics
    time-date-time-timespan
    type-runtime-handles
)

TOTAL=${#FAMILIES[@]}
echo "Running $TOTAL families with concurrency=$CONCURRENCY"
START=$(date +%s)
PASSED=0
FAILED=0
COUNT=0

for ((i=0; i<TOTAL; i+=CONCURRENCY)); do
    BATCH_NUM=$((i / CONCURRENCY + 1))
    TOTAL_BATCHES=$(( (TOTAL + CONCURRENCY - 1) / CONCURRENCY ))

    # Start this batch in parallel
    for ((j=i; j<i+CONCURRENCY && j<TOTAL; j++)); do
        f="${FAMILIES[$j]}"
        echo "[$BATCH_NUM/$TOTAL_BATCHES] Starting: $f"
        (cd "$BASE" && python testing/foundation-dll/run_pipeline.py "$f" > "$LOG_DIR/${f}-pipeline.log" 2>&1) &
    done

    # Wait for this batch to complete
    wait

    # Check results
    for ((j=i; j<i+CONCURRENCY && j<TOTAL; j++)); do
        f="${FAMILIES[$j]}"
        if grep -q '"overall_status": "passed"' "$LOG_DIR/${f}-pipeline.log" 2>/dev/null; then
            PASSED=$((PASSED + 1))
            echo "  PASSED: $f"
        else
            FAILED=$((FAILED + 1))
            echo "  FAILED: $f"
        fi
    done
done

ELAPSED=$(($(date +%s) - START))
echo ""
echo "============================================================"
echo "Total: $PASSED passed, $FAILED failed, ${ELAPSED}s ($((ELAPSED/60)) min)"
echo "============================================================"
