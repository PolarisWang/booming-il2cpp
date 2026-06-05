"""Run benchmark pipeline in parallel for remaining chunks.

Phase 1: managed_benchmark for 19 chunks (parallel, 4 at a time)
Phase 2: benchmark for 19 chunks (parallel, 2 at a time)
Phase 3: benchmark_report + aggregate
"""
import subprocess
import sys
import time
from pathlib import Path

FOUNDATION_DIR = Path(__file__).resolve().parent
CHUNKS_TO_RUN = [
    "system-4", "system-5", "buffers", "collections",
    "collections-generic", "diagnostics", "globalization",
    "io", "io-2", "numerics", "reflection", "reflection-emit",
    "runtime-compiler", "runtime-interop", "runtime-intrinsics",
    "runtime-intrinsics-2", "text", "threading", "threading-tasks",
]

def run_batch(chunks, stage, max_parallel, label):
    """Run chunk_pipeline for chunks in parallel batches."""
    total = len(chunks)
    done = 0
    errors = []
    for i in range(0, total, max_parallel):
        batch = chunks[i:i + max_parallel]
        processes = []
        for chunk in batch:
            cmd = [
                sys.executable, "-m", "verification.chunk_pipeline",
                "--chunk", chunk, "--stages", stage
            ]
            print(f"\n[{label}] Starting {chunk} ({stage})...")
            p = subprocess.Popen(
                cmd, cwd=str(FOUNDATION_DIR),
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
            )
            processes.append((chunk, p))

        for chunk, p in processes:
            stdout, _ = p.communicate(timeout=7200)
            done += 1
            if p.returncode != 0:
                errors.append(chunk)
                print(f"[{label}] {chunk} FAILED (exit={p.returncode})")
                for line in stdout.splitlines()[-5:]:
                    print(f"  {line}")
            else:
                print(f"[{label}] {chunk} OK ({done}/{total})")

        if errors:
            print(f"[{label}] Batch errors so far: {errors}")

    return errors

def run_single(chunks, stage, label):
    """Run chunk_pipeline serially."""
    errors = []
    for i, chunk in enumerate(chunks):
        print(f"\n[{label}] {chunk} ({stage}) [{i+1}/{len(chunks)}]...")
        result = subprocess.run(
            [sys.executable, "-m", "verification.chunk_pipeline",
             "--chunk", chunk, "--stages", stage],
            cwd=str(FOUNDATION_DIR),
            capture_output=True, text=True, timeout=7200,
        )
        if result.returncode != 0:
            errors.append(chunk)
            print(f"  FAILED (exit={result.returncode})")
            for line in (result.stdout or "").splitlines()[-5:]:
                print(f"  {line}")
        else:
            print(f"  OK ({i+1}/{len(chunks)})")
    return errors


if __name__ == "__main__":
    start = time.perf_counter()
    all_errors = []

    # Phase 1: managed_benchmark (up to 4 at a time)
    print("=" * 60)
    print("PHASE 1: managed_benchmark for 19 chunks (4 parallel)")
    print("=" * 60)
    errs = run_batch(CHUNKS_TO_RUN, "managed_benchmark", 4, "MB")
    all_errors.extend(errs)

    # Phase 2: benchmark (up to 2 at a time)
    print("\n" + "=" * 60)
    print("PHASE 2: benchmark for 19 chunks (2 parallel)")
    print("=" * 60)
    errs = run_batch(CHUNKS_TO_RUN, "benchmark", 2, "BM")
    all_errors.extend(errs)

    # Phase 3: benchmark_report + aggregate
    print("\n" + "=" * 60)
    print("PHASE 3: benchmark_report + aggregate")
    print("=" * 60)
    for stage in ["benchmark_report", "aggregate"]:
        result = subprocess.run(
            [sys.executable, "-m", "verification.chunk_pipeline",
             "--assembly", "System.Private.CoreLib", "--all-chunks",
             "--stages", stage],
            cwd=str(FOUNDATION_DIR),
            capture_output=True, text=True, timeout=300,
        )
        print(result.stdout)
        if result.returncode != 0:
            all_errors.append(f"{stage}: exit={result.returncode}")

    elapsed = time.perf_counter() - start
    print(f"\n{'=' * 60}")
    print(f"Pipeline complete in {elapsed/60:.1f} min")
    if all_errors:
        print(f"ERRORS: {all_errors}")
    else:
        print("ALL OK")
