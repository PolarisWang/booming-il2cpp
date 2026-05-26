"""Batch-run verification pipelines for all foundation-dll families.
Simple sequential runner with progress tracking."""
import subprocess, sys, time, json
from pathlib import Path

testing_root = Path(r'D:\agent\booming-il2cpp\testing\foundation-dll')
families = sorted([d.name for d in (testing_root / 'System.Private.CoreLib').iterdir()
                   if d.is_dir() and d.name not in ('reports', 'System.Private.CoreLib')])

# Known-valid families (have contract.json)
valid_families = [
    'array-indexing-copy', 'attributes-custom-metadata', 'boxing-unboxing-casts',
    'buffer-memory', 'ccw-basic', 'codegen-edge-cases', 'collections-generic-core',
    'convert-char', 'custom-generics', 'delegate-core-invocation', 'dispatch-basic',
    'eh-stress', 'enum-parsing', 'error-info-basic', 'exception-throw-diagnostics',
    'garbage-collection', 'generic-nullable-value', 'generics-stress',
    'globalization-culture', 'guid-random-hashcode', 'interface-dispatch',
    'interface-dispatch,pinvoke-dllimport,primitive-numeric-conversions-core',
    'io-streams-basics', 'math-numerics', 'object-equality-identity',
    'pinvoke-dllimport', 'pinvoke-stress', 'primitive-numeric-conversions',
    'primitive-numeric-conversions-core', 'rcw-basic', 'reflection-activation',
    'reflection-assembly', 'reflection-binding', 'reflection-field-property',
    'reflection-generics', 'reflection-member-basics', 'reflection-member-complete',
    'reflection-module', 'reflection-parameters', 'reflection-type',
    'runtime-compiler-services', 'snapshot-prover', 'span-memory-buffers',
    'string-char-text-core', 'string-construction', 'threading-monitor-interlocked',
    'threading-sync', 'threading-tasks-primitives', 'threading-thread-basics',
    'time-date-time-timespan', 'type-runtime-handles',
]

already_done = {'enum-parsing', 'array-indexing-copy'}
to_run = [f for f in valid_families if f not in already_done]

concurrency = int(sys.argv[1]) if len(sys.argv) > 1 else 2
log_dir = testing_root / 'results'
log_dir.mkdir(parents=True, exist_ok=True)

print(f"Running {len(to_run)} families with concurrency={concurrency}")
start_all = time.time()
passed = failed = 0

# Simple batch loop
for i in range(0, len(to_run), concurrency):
    batch = to_run[i:i+concurrency]
    procs = {}
    for f in batch:
        log = log_dir / f'{f}-pipeline.log'
        print(f"[{i//concurrency+1}/{(len(to_run)+concurrency-1)//concurrency}] Starting: {f}")
        log_file = open(log, 'w')
        p = subprocess.Popen(
            [sys.executable, str(testing_root / 'run_pipeline.py'), f],
            stdout=log_file, stderr=subprocess.STDOUT,
            cwd=r'D:\agent\booming-il2cpp')
        procs[f] = (p, log_file)

    for f, (p, lf) in procs.items():
        p.wait()
        lf.close()
        if p.returncode == 0:
            passed += 1
            print(f"  PASSED: {f}")
        else:
            # Check if it's a "stages_failed" but had hotupdate data
            log = log_dir / f'{f}-pipeline.log'
            log_content = log.read_text()
            if 'hotupdate' in log_content and 'Result: failed' not in log_content:
                # Pipeline might have failed on non-critical stage
                print(f"  PARTIAL: {f} (check log)")
                failed += 1
            else:
                failed += 1
                print(f"  FAILED: {f} (exit {p.returncode})")

elapsed = time.time() - start_all
print(f"\n{'='*60}")
print(f"Total: {passed} passed, {failed} failed, {elapsed:.0f}s ({elapsed/60:.1f} min)")
print(f"{'='*60}")
