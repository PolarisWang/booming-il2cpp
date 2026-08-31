"""Managed benchmark stage — runs CombinedSubjects.dll under .NET 8 and .NET 10 JIT.

Reads the chunk's CombinedSubjects.dll (built during the `build` stage),
runs all [Benchmark] methods via Chaos.TestFramework.Runtime under each
target runtime, and writes dashboard-compatible benchmark-history.jsonl
with technology tags "net8-jit" and "net10-jit".

Output directory:
    results/foundation-dll/{assembly}/{slug}/perf/benchmark-history.jsonl
"""
from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import sys
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from verification.orchestration.context import ChunkContext, StageResult
from verification._path import results_base

_REPO_ROOT = Path(__file__).resolve().parents[4]
_RUNTIME_PROJECT = _REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Runtime"
_RESULTS_BASE = results_base()
_ITERATIONS = 200
_TIMEOUT = 1800

# TFM → technology tag mapping
_TFM_TECH: dict[str, str] = {
    "net8.0": "net8-jit",
    "net10.0": "net10-jit",
}


def _ensure_runner_built() -> bool:
    """Build Chaos.TestFramework.Runtime for both net8.0 and net10.0."""
    for tfm in ("net8.0", "net10.0"):
        dll = _RUNTIME_PROJECT / "bin" / "Debug" / tfm / "Chaos.TestFramework.Runtime.dll"
        if dll.exists():
            continue
        print(f"  [managed-benchmark] Building runner ({tfm})...")
        result = subprocess.run(
            ["dotnet", "build", str(_RUNTIME_PROJECT), "-f", tfm, "--nologo", "-v", "q"],
            capture_output=True, text=True, timeout=180)
        if result.returncode != 0:
            print(f"  [managed-benchmark] ERROR: runner build failed for {tfm}")
            for line in (result.stderr.splitlines() + result.stdout.splitlines())[-10:]:
                print(f"      {line}")
            return False
    return True


def _runner_dll(tfm: str) -> Path:
    return _RUNTIME_PROJECT / "bin" / "Debug" / tfm / "Chaos.TestFramework.Runtime.dll"


def _ensure_multitarget_csproj(combined_csproj: Path) -> str | None:
    """Patch csproj to use TargetFrameworks (multi-target). Returns original TFM or None."""
    csproj_text = combined_csproj.read_text(encoding="utf-8")
    if "<TargetFramework>" not in csproj_text:
        return None  # already multi-target

    csproj_text = csproj_text.replace(
        "<TargetFramework>", "<TargetFrameworks>")
    csproj_text = csproj_text.replace(
        "</TargetFramework>", "</TargetFrameworks>")
    net_matches = list(re.finditer(r"net\d+\.\d+", csproj_text))
    current_tfm = net_matches[0].group() if net_matches else "net9.0"
    csproj_text = csproj_text.replace(
        current_tfm, f"net8.0;{current_tfm};net10.0")
    combined_csproj.write_text(csproj_text, encoding="utf-8")
    return current_tfm


_NET8_REPLACEMENTS = [
    ("default(System.ReadOnlySpan<object>)", "default(object[])"),
    ("default(System.ReadOnlySpan<string>)", "default(string[])"),
    ("default(System.ReadOnlySpan<char>)", "default(string)"),
    ("default(System.ReadOnlySpan<byte>)", "default(byte[])"),
    ("default(System.ReadOnlyMemory<char>)", "default(string)"),
    ("default(System.ReadOnlyMemory<byte>)", "default(byte[])"),
]

# Benchmark methods using these APIs crash at runtime (stack overflow / buffer
# overrun / null-pointer deref) or THROW on real .NET (default(this).Method()
# auto-generated probes on null/uninit objects), and must be excluded from
# managed benchmarks.
_UNSAFE_BENCHMARK_PATTERNS: list[str] = [
    "StoreUnsafe",                     # writes 32B vector past a 4B stack slot — STATUS_ACCESS_VIOLATION
    "GetNextArgType",                  # System.ArgIterator.GetNextArgType() — Internal CLR error (0x80131506)
    "RuntimeHelpers.CreateSpan",       # .NET runtime bug — CLR crash on all TFMs
    "Environment.Exit",                # Environment.Exit(N) terminates the runner process immediately
    "Environment.FailFast",            # Environment.FailFast terminates the runner process immediately
    "Environment.GetEnvironmentVariable",  # per-process, non-deterministic; probes throw on default target
    "Environment.SetEnvironmentVariable",  # mutates OS env — side-effect hazard across benchmark TFs
    "Environment.ExpandEnvironmentVariables",  # probes throw on default(null) input
    "GetHexString",                    # Random.GetHexString — net9+; throws on net8/absent API
    "NextInt64",                       # Random.NextInt64(long) — probes throw on default boundaries
    "NextSingle",                      # Random.NextSingle() — probes throw on default state
    "AggregateException.GetBaseException",   # default(AggregateException) throws NullReference
    "AggregateException.Handle",             # default(AggregateException) throws NullReference
    "AggregateException.Flatten",            # default(AggregateException) throws NullReference
    "System.IntPtr.Parse",             # default(IntPtr).Parse(null) throws ArgumentNull
    "System.UIntPtr.Parse",            # default(UIntPtr).Parse(null) throws ArgumentNull
    "System.IntPtr.DivRem",            # default(IntPtr).DivRem — divide-by-zero on default operands
    "System.UIntPtr.DivRem",           # default(UIntPtr).DivRem — divide-by-zero on default operands
    "Contract.Assert",                 # Contract.Assert(false) triggers FailFast — "Process terminated. Assumption failed."
    "Contract.Assume",                 # Contract.Assume(false) also triggers FailFast
    "Contract.Requires",               # Contract.Requires(false) throws ArgumentException at runtime
    "Contract.Ensures",                # Contract.Ensures postconditions can fail at runtime
    "Contract.Invariant",              # Contract.Invariant(false) can terminate the process
    "Debug.Assert",                    # Debug.Assert(false) terminates on .NET Core in some configurations
    "Debug.Fail",                      # Debug.Fail() always terminates the process with "Assertion failed."
    "SpinWait.SpinUntil",              # SpinUntil(() => default(bool)) spins forever since condition always returns false
]


_DISPATCH_ATTR_PREFIXES = ("[Fact", "[HotUpdate", "[Benchmark")

def _remove_unsafe_benchmarks(combined_src: Path) -> bool:
    """Neutralize dispatch attributes on methods matching _UNSAFE_BENCHMARK_PATTERNS
    so the runner never dispatches them (they crash / hang / terminate the .NET
    runtime).

    The Chaos.TestFramework.Runtime runner discovers methods by the [Fact] /
    [HotUpdate] attributes (BenchmarkRunner.cs:17), NOT [Benchmark]. For each
    dispatch attribute that sits ABOVE a method-body referencing an unsafe
    pattern, comment out the attribute line. The method body stays intact so the
    file still compiles — the method simply has no dispatch attribute and is
    invisible to the runner. Returns True if any modification was made.
    """
    src_text = combined_src.read_text(encoding="utf-8")
    lines = src_text.splitlines(keepends=True)
    modified = False

    for pattern in _UNSAFE_BENCHMARK_PATTERNS:
        i = 0
        while i < len(lines):
            stripped = lines[i].strip()
            # Skip comments and non-dispatch-attribute lines
            if stripped.startswith(("//", "/*")) or not any(
                stripped.startswith(p) for p in _DISPATCH_ATTR_PREFIXES
            ):
                i += 1
                continue

            # This line is a dispatch attribute. Find the method signature.
            sig_idx = i + 1
            while sig_idx < len(lines):
                sj = lines[sig_idx].strip()
                if not sj or sj.startswith("["):
                    sig_idx += 1
                    continue
                if sj.startswith("public ") and "(" in sj:
                    break
                sig_idx += 1
            else:
                i += 1
                continue

            # Scan the method body for the unsafe pattern (first 20 lines)
            body = "".join(lines[sig_idx: min(sig_idx + 20, len(lines))])
            if pattern in body:
                indent = lines[i][:len(lines[i]) - len(lines[i].lstrip())]
                lines[i] = f"{indent}// [SAFE] {lines[i].lstrip()}"
                modified = True
                print(f"  [managed-benchmark] Neutralized '{pattern}' dispatch at line {i + 1}")

            i += 1

    if modified:
        combined_src.write_text("".join(lines), encoding="utf-8")

    return modified


def _build_combined_for_tfm(combined_csproj: Path, tfm: str, out_dir: Path) -> bool:
    """Rebuild CombinedSubjects.csproj for the given TFM.

    For net8.0, applies targeted source replacements for net9.0+ APIs
    (ReadOnlySpan<T>/ReadOnlyMemory<T> overloads). If the build still
    fails after deterministic replacements, net8.0 is skipped (it's an
    optional baseline, not a blocking requirement).
    """
    if not combined_csproj.exists():
        return False

    out_dir.mkdir(parents=True, exist_ok=True)
    combined_src = combined_csproj.parent / "CombinedSubjects.cs"

    # Pre-build: remove [Benchmark] from methods known to crash at runtime
    # (e.g. Vector.StoreUnsafe writes past stack slots). Applied for all TFMs.
    if combined_src.exists():
        _remove_unsafe_benchmarks(combined_src)

    # For net8.0, apply targeted source replacements first
    src_was_modified = False
    orig_src = None
    if tfm == "net8.0" and combined_src.exists():
        orig_src = combined_src.read_text(encoding="utf-8")
        for old, new in _NET8_REPLACEMENTS:
            if old in orig_src:
                orig_src = orig_src.replace(old, new)
                src_was_modified = True
        if src_was_modified:
            combined_src.write_text(orig_src, encoding="utf-8")

    try:
        # Restore
        result = subprocess.run(
            ["dotnet", "restore", str(combined_csproj), "--nologo"],
            capture_output=True, text=True, timeout=120)
        if result.returncode != 0:
            print(f"  [managed-benchmark] restore failed for {tfm}")
            return False

        # Build for target TFM
        result = subprocess.run(
            ["dotnet", "build", str(combined_csproj), "-f", tfm,
             f"-p:OutDir={out_dir}", "--nologo", "-v", "q"],
            capture_output=True, text=True, timeout=120)

        if result.returncode == 0:
            return True

        # Build failed after deterministic replacements.
        # For net8.0, this is non-fatal — report the first error and skip.
        # The net8.0 baseline is optional; missing it means "no net8 baseline"
        # for comparison, which benchmark_report handles gracefully.
        if tfm == "net8.0":
            errors = [l for l in (result.stderr + result.stdout).splitlines()
                      if "error CS" in l]
            print(f"  [managed-benchmark] net8.0 build failed with {len(errors)} error(s) — skipping (non-fatal)")
            for e in errors[:5]:
                print(f"      {e}")
            if len(errors) > 5:
                print(f"      ... and {len(errors) - 5} more")
            return False

        # For net10.0, errors are unexpected — report them
        errors = [l for l in (result.stderr + result.stdout).splitlines()
                  if "error CS" in l]
        for e in errors:
            print(f"      {e}")
        return False
    finally:
        # Restore original source if modified
        if src_was_modified and orig_src is not None:
            combined_src.write_text(orig_src, encoding="utf-8")


def _read_benchmark_metadata(metadata_path: Path) -> list[dict]:
    """Read subjects.metadata.json and return benchmark method entries in index order.

    Returns a list of dicts with keys: index, methodSubjectId.
    When benchmarkMethodIndices is missing or empty, returns EMPTY — meaning this
    chunk has no BenchmarkDotNet benchmarks and should not produce managed data.
    """
    metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
    methods = metadata.get("methods", [])
    benchmark_indices = set(metadata.get("benchmarkMethodIndices") or [])
    if benchmark_indices:
        return [
            m for m in sorted(methods, key=lambda x: x["index"])
            if m["index"] in benchmark_indices
        ]
    # No benchmark indices defined — this chunk has no BenchmarkDotNet methods.
    # Returning empty here means the managed benchmark stage produces no data,
    # and benchmark_report will correctly report "no net8 baseline" for this chunk.
    return []


def _parse_runner_output(stdout: str) -> list[dict]:
    """Parse JSON output from Chaos.TestFramework.Runtime.

    Expected format: {"results": [{"label": "...", "elapsedMs": ..., "opsPerSecond": ..., "threw": ...}]}
    """
    try:
        parsed = json.loads(stdout)
        return parsed.get("results", [])
    except json.JSONDecodeError:
        return []


def _write_perf_records(
    perf_path: Path,
    slug: str,
    technology: str,
    records: list[dict],
    metadata_methods: list[dict],
    now: str,
    append: bool = True,
) -> int:
    """Write benchmark records to perf store in dashboard-compatible JSONL format.

    Returns the count of successful (non-throwing) records.
    """
    perf_path.parent.mkdir(parents=True, exist_ok=True)

    mode = "a" if append else "w"
    completed_count = 0
    with open(perf_path, mode, encoding="utf-8") as f:
        for i, record in enumerate(records):
            elapsed_ms = record.get("elapsedMs", 0)
            threw = record.get("threw", False)
            ops = record.get("opsPerSecond", 0)

            # Map throwing methods to error status with -1 elapsed
            is_error = bool(threw)
            metrics_elapsed = -1.0 if is_error else (elapsed_ms if elapsed_ms > 0 else 0.001)
            metrics_ops = 0.0 if is_error else ops
            if not is_error:
                completed_count += 1

            method_subject_id = ""
            if i < len(metadata_methods):
                method_subject_id = metadata_methods[i].get("methodSubjectId", "")

            jsonl_record = {
                "timestamp": now,
                "slug": slug,
                "technology": technology,
                "methodSubjectId": method_subject_id,
                "methodIndex": i,
                "metrics": {
                    "elapsedMilliseconds": metrics_elapsed,
                    "opsPerSecond": metrics_ops,
                },
                "gcInfo": record.get("gcInfo", {}),
                "iterations": record.get("iterations", _ITERATIONS),
                "status": "completed" if not is_error else "error",
            }
            # Skip error records (elapsed < 0) — they have no meaningful timing data
            # and only pollute the comparison report with noise.
            if is_error or metrics_elapsed < 0:
                continue
            f.write(json.dumps(jsonl_record, ensure_ascii=False, separators=(",", ":")) + "\n")
    return completed_count


def run_managed_benchmark(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult:
    """Managed benchmark stage: run CombinedSubjects under .NET 8 and .NET 10 JIT."""
    start = time.perf_counter()

    # Pre-req: CombinedSubjects.dll must exist
    subjects_dll = ctx.subjects_dll_path
    metadata_path = ctx.subjects_metadata_path
    if not subjects_dll.exists() or not metadata_path.exists():
        return StageResult(
            stage="managed_benchmark", status="skipped",
            summary="CombinedSubjects.dll or metadata not found",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Ensure runner is built
    if not _ensure_runner_built():
        return StageResult(
            stage="managed_benchmark", status="error",
            summary="Failed to build Chaos.TestFramework.Runtime",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Read metadata for methodSubjectId mapping
    metadata_methods = _read_benchmark_metadata(metadata_path)
    method_count = len(metadata_methods)
    if method_count == 0:
        return StageResult(
            stage="managed_benchmark", status="skipped",
            summary="No benchmark methods in metadata",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    print(f"  [managed-benchmark] {method_count} benchmark methods in {ctx.slug}")
    slug = ctx.slug
    now = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    errors: list[str] = []
    techs_run: list[str] = []

    # CombinedSubjects csproj
    combined_csproj = ctx.chunk_dir / "managed" / "combined" / "CombinedSubjects.csproj"

    # Patch csproj to multi-target TFMs for net8.0/net10.0 comparison
    _ensure_multitarget_csproj(combined_csproj)

    # Phase 2: run net8 and net10 benchmarks in parallel
    _perf_write_lock: threading.Lock = threading.Lock()

    def _run_tfm(tfm: str, technology: str) -> tuple[str, list[str] | None]:
        """Build CombinedSubjects for TFM and run benchmark. Returns (technology, errors_or_None)."""
        print(f"  [managed-benchmark] Running {technology} ({tfm})...")

        # Build CombinedSubjects for this TFM
        build_dir = ctx.chunk_dir / "managed" / f"subjects_{tfm.replace('.', '_')}"
        if build_dir.exists():
            for _retry in range(3):
                try:
                    shutil.rmtree(build_dir)
                    break
                except PermissionError:
                    time.sleep(2)
            else:
                print(f"  [managed-benchmark] WARN: could not remove {build_dir} after 3 retries")

        ok = _build_combined_for_tfm(combined_csproj, tfm, build_dir)
        if not ok:
            msg = f"{technology}: CombinedSubjects build failed for {tfm}"
            print(f"  [managed-benchmark] SKIP: {msg}")
            return technology, [msg]

        tfm_dll_path = build_dir / "CombinedSubjects.dll"
        if not tfm_dll_path.exists():
            msg = f"{technology}: CombinedSubjects.dll not produced for {tfm}"
            return technology, [msg]

        # Run the managed benchmark
        runner = _runner_dll(tfm)
        try:
            result = subprocess.run(
                ["dotnet", "exec", str(runner),
                 "--assembly", str(tfm_dll_path),
                 "--kind", "benchmark",
                 "--iterations", str(_ITERATIONS)],
                capture_output=True, text=True, timeout=_TIMEOUT)
        except subprocess.TimeoutExpired:
            msg = f"{technology}: benchmark timed out"
            return technology, [msg]

        if result.returncode != 0:
            msg = f"{technology}: runner exited with code {result.returncode} (0x{result.returncode & 0xffffffff:08x})"
            print(f"  [managed-benchmark] {msg}")
            stderr_text = result.stderr or ""
            stdout_text = result.stdout or ""
            if stderr_text:
                print(f"  [managed-benchmark] Stderr ({len(stderr_text)} chars):")
                for line in stderr_text.splitlines()[-20:]:
                    print(f"      {line}")
            if stdout_text:
                print(f"  [managed-benchmark] Stdout head ({len(stdout_text)} chars):")
                for line in stdout_text.splitlines()[:10]:
                    print(f"      {line}")
            return technology, [msg]

        # Parse output
        records = _parse_runner_output(result.stdout or "")
        if not records:
            # No benchmark results from runner — this is a REAL failure, not a
            # success reusing a stale baseline. Previously it silently declared
            # success and kept the old benchmark-history.jsonl, which masked the
            # failure and let stale numbers stand as "fresh".
            msg = (f"{technology}: runner returned 0 benchmark results"
                   + (" (existing baseline NOT reused)" if (_RESULTS_BASE / ctx.assembly / slug / "perf" / "benchmark-history.jsonl").exists() else ""))
            return technology, [msg]

        # Write to perf store (serialized via lock to avoid file corruption)
        perf_path = _RESULTS_BASE / ctx.assembly / slug / "perf" / "benchmark-history.jsonl"
        with _perf_write_lock:
            completed = _write_perf_records(
                perf_path, slug, technology, records, metadata_methods, now,
                append=True)
        print(f"  [managed-benchmark] {technology}: {completed}/{len(records)} methods OK -> {perf_path}")
        with _perf_write_lock:
            techs_run.append(technology)
        return technology, None

    with ThreadPoolExecutor(max_workers=len(_TFM_TECH)) as executor:
        fut = {executor.submit(_run_tfm, tfm, tech): tech for tfm, tech in _TFM_TECH.items()}
        for f in as_completed(fut):
            tech, errs = f.result()
            if errs:
                errors.extend(errs)

    duration_ms = int((time.perf_counter() - start) * 1000)
    status = "error" if not techs_run else "passed" if not errors else "passed_with_errors"
    summary = f"{len(techs_run)}/{len(_TFM_TECH)} technologies: {', '.join(techs_run) or 'none'}"
    if errors:
        summary += f"; errors: {'; '.join(errors[:3])}"

    # P1-B fix: write managed_benchmark.json to the chunk results dir. aggregate.py
    # reads chunks/<slug>/results/managed_benchmark.json (aggregate.py:237) but no
    # stage wrote it — the branch was a dead orphan read. Actually persisting it
    # lets aggregate reflect the managed (.NET) benchmark outcome.
    ctx.results_dir.mkdir(parents=True, exist_ok=True)
    try:
        (ctx.results_dir / "managed_benchmark.json").write_text(
            json.dumps({
                "passed": len(techs_run),
                "total": len(_TFM_TECH),
                "mode": ctx.mode,
                "status": status,
                "errors": errors[:3],
                "technologiesRun": techs_run,
            }, indent=2), encoding="utf-8")
    except OSError:
        pass  # non-fatal

    print(f"  [managed-benchmark] Done ({duration_ms}ms): {summary}")

    return StageResult(
        stage="managed_benchmark", status=status,
        summary=summary,
        details={"technologiesRun": techs_run, "technologiesTotal": len(_TFM_TECH),
                 "methodCount": method_count, "errors": errors},
        duration_ms=duration_ms,
    )
