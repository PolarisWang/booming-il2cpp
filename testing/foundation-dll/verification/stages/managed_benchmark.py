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
import time
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from verification.orchestration.context import ChunkContext, StageResult

_REPO_ROOT = Path(__file__).resolve().parents[4]
_RUNTIME_PROJECT = _REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Runtime"
_RESULTS_BASE = Path(__file__).resolve().parent.parent / "results" / "foundation-dll"
_ITERATIONS = 1000
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


def _sanitize_for_net8_retry(combined_src: Path, error_lines: set[int]) -> bool:
    """Retry net8.0 build by commenting out error lines.

    Sanitizes lines that have net9.0+ API errors, plus cascading undefined
    variable references. Handles CS0161 by adding 'return default;'.
    """
    if not error_lines:
        return False

    src_text = combined_src.read_text(encoding="utf-8")
    lines = src_text.splitlines(keepends=True)

    # Separate CS0161 lines (method declaration) from other error lines
    cs0161_lines: set[int] = set()
    comment_lines: set[int] = set()
    for lineno in error_lines:
        idx = lineno - 1
        if idx < 0 or idx >= len(lines):
            continue
        # Check if this specific line might be a CS0161 (method declaration)
        stripped = lines[idx].strip()
        if any(stripped.startswith(kw) for kw in ("public ", "private ", "internal ", "protected ")):
            cs0161_lines.add(lineno)
        else:
            comment_lines.add(lineno)

    sanitized_count = 0
    for lineno in sorted(comment_lines, reverse=True):
        idx = lineno - 1
        if idx < 0 or idx >= len(lines):
            continue
        stripped = lines[idx].strip()
        if not stripped or stripped.startswith("//"):
            continue
        lines[idx] = f"// [net8.0-safe] {lines[idx]}"
        sanitized_count += 1

    # Handle CS0161: add 'return default;' before closing brace of affected methods
    # We process these AFTER comment_lines because the comment-outs may create CS0161
    for lineno in sorted(cs0161_lines, reverse=True):
        idx = lineno - 1
        if idx < 0 or idx >= len(lines):
            continue
        # Find the closing brace of this method
        i = lineno  # 1-based, start from method declaration
        while i < len(lines):
            line = lines[i]
            if "{" in line:
                depth = line.count("{") - line.count("}")
                j = i + 1
                while j < len(lines) and depth > 0:
                    depth += lines[j].count("{") - lines[j].count("}")
                    j += 1
                # j-1 is the closing brace line
                indent = " " * (len(lines[j - 1]) - len(lines[j - 1].lstrip()))
                lines.insert(j - 1, f"{indent}return default;\n")
                sanitized_count += 1
                break
            i += 1

    if sanitized_count > 0:
        combined_src.write_text("".join(lines), encoding="utf-8")
        print(f"  [managed-benchmark] Sanitized {sanitized_count} lines for net8.0 build")
    return sanitized_count > 0


def _ensure_multitarget_csproj(combined_csproj: Path) -> str | None:
    """Patch csproj to use TargetFrameworks (multi-target). Returns original TFM or None."""
    csproj_text = combined_csproj.read_text(encoding="utf-8")
    if "<TargetFramework>" not in csproj_text:
        return None  # already multi-target

    csproj_text = csproj_text.replace(
        "<TargetFramework>", "<TargetFrameworks>")
    csproj_text = csproj_text.replace(
        "</TargetFramework>", "</TargetFrameworks>")
    net9_matches = list(re.finditer(r"net\d+\.\d+", csproj_text))
    current_tfm = net9_matches[0].group() if net9_matches else "net9.0"
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
# overrun / null-pointer deref) and must be excluded from managed benchmarks.
_UNSAFE_BENCHMARK_PATTERNS: list[str] = [
    "StoreUnsafe",                     # writes 32B vector past a 4B stack slot — STATUS_ACCESS_VIOLATION
    "ArgIterator",                     # System.ArgIterator.GetNextArgType() — Internal CLR error (0x80131506)
    "RuntimeHelpers.CreateSpan",       # .NET runtime bug — CLR crash on all TFMs
    "Environment.Exit",                # Environment.Exit(N) terminates the runner process immediately
    "Environment.FailFast",            # Environment.FailFast terminates the runner process immediately
    "Contract.Assert",                 # Contract.Assert(false) triggers FailFast — "Process terminated. Assumption failed."
    "Contract.Assume",                 # Contract.Assume(false) also triggers FailFast
    "Contract.Requires",               # Contract.Requires(false) throws ArgumentException at runtime
    "Contract.Ensures",                # Contract.Ensures postconditions can fail at runtime
    "Contract.Invariant",              # Contract.Invariant(false) can terminate the process
    "Debug.Assert",                    # Debug.Assert(false) terminates on .NET Core in some configurations
    "Debug.Fail",                      # Debug.Fail() always terminates the process with "Assertion failed."
]


def _remove_unsafe_benchmarks(combined_src: Path) -> bool:
    """Remove [Benchmark] attributes from methods matching _UNSAFE_BENCHMARK_PATTERNS.

    The method body stays valid C# so the file still compiles, but the runner
    won't discover these methods. Returns True if any modifications were made.
    """
    src_text = combined_src.read_text(encoding="utf-8")
    lines = src_text.splitlines(keepends=True)
    modified = False

    for pattern in _UNSAFE_BENCHMARK_PATTERNS:
        i = 0
        while i < len(lines):
            stripped = lines[i].strip()
            if stripped == "[Benchmark]":
                # Look ahead to see if this benchmark uses the unsafe pattern
                j = i + 1
                while j < len(lines) and not lines[j].strip().startswith("["):
                    if pattern in lines[j]:
                        # Comment out the [Benchmark] attribute
                        indent = lines[i][:len(lines[i]) - len(lines[i].lstrip())]
                        lines[i] = f"{indent}// [SAFE] [Benchmark]\n"
                        modified = True
                        print(f"  [managed-benchmark] Removed benchmark using '{pattern}' (line {i + 1})")
                        break
                    j += 1
            i += 1

    if modified:
        combined_src.write_text("".join(lines), encoding="utf-8")

    return modified


def _build_combined_for_tfm(combined_csproj: Path, tfm: str, out_dir: Path) -> bool:
    """Rebuild CombinedSubjects.csproj for the given TFM.

    For net8.0, applies targeted source replacements for net9.0+ APIs
    (ReadOnlySpan<T>/ReadOnlyMemory<T> overloads) then retries with
    comment-out sanitization as fallback.
    """
    if not combined_csproj.exists():
        return False

    out_dir.mkdir(parents=True, exist_ok=True)
    combined_src = combined_csproj.parent / "CombinedSubjects.cs"

    # Patch csproj to multi-target
    _ensure_multitarget_csproj(combined_csproj)

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

        # Build failed. For net8.0, try comment-out retry as fallback.
        if tfm != "net8.0":
            errors = [l for l in (result.stderr + result.stdout).splitlines()
                      if "error CS" in l]
            for e in errors:
                print(f"      {e}")
            return False

        max_retries = 3
        for attempt in range(max_retries):
            error_lines: set[int] = set()
            for line in (result.stderr + result.stdout).splitlines():
                m = re.match(r"^.*CombinedSubjects\.cs\((\d+),\d+\):\s*(error\s+(CS\d{4})).*", line)
                if m:
                    code = m.group(3)
                    if code in ("CS1503", "CS0117", "CS1061", "CS0103",
                                "CS0234", "CS0426", "CS0305", "CS0161",
                                "CS1501", "CS0452"):
                        error_lines.add(int(m.group(1)))

            if not error_lines:
                if attempt == 0:
                    errors = [l for l in (result.stderr + result.stdout).splitlines()
                              if "error" in l.lower()]
                    for e in errors:
                        print(f"      {e}")
                break

            ok = _sanitize_for_net8_retry(combined_src, error_lines)
            if not ok:
                break

            result = subprocess.run(
                ["dotnet", "build", str(combined_csproj), "-f", tfm,
                 f"-p:OutDir={out_dir}", "--nologo", "-v", "q"],
                capture_output=True, text=True, timeout=120)
            if result.returncode == 0:
                return True

        remaining = [l for l in (result.stderr + result.stdout).splitlines()
                     if "error CS" in l]
        for e in remaining:
            print(f"      {e}")
        return False
    finally:
        # Restore original source if modified
        if src_was_modified and orig_src is not None:
            combined_src.write_text(orig_src, encoding="utf-8")


def _read_benchmark_metadata(metadata_path: Path) -> list[dict]:
    """Read subjects.metadata.json and return benchmark method entries in index order.

    Returns a list of dicts with keys: index, methodSubjectId.
    """
    metadata = json.loads(metadata_path.read_text(encoding="utf-8"))
    methods = metadata.get("methods", [])
    benchmark_indices = set(metadata.get("benchmarkMethodIndices") or [])
    return [
        m for m in sorted(methods, key=lambda x: x["index"])
        if m["index"] in benchmark_indices
    ]


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
                "iterations": record.get("iterations", _ITERATIONS),
                "status": "completed" if not is_error else "error",
            }
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

    for tfm, technology in sorted(_TFM_TECH.items()):
        is_first = (tfm == sorted(_TFM_TECH.keys())[0])
        print(f"  [managed-benchmark] Running {technology} ({tfm})...{' (first)' if is_first else ''}")

        # Build CombinedSubjects for this TFM
        build_dir = ctx.chunk_dir / "managed" / f"subjects_{tfm.replace('.', '_')}"
        if build_dir.exists():
            shutil.rmtree(build_dir)

        ok = _build_combined_for_tfm(combined_csproj, tfm, build_dir)
        if not ok:
            msg = f"{technology}: CombinedSubjects build failed for {tfm}"
            print(f"  [managed-benchmark] SKIP: {msg}")
            errors.append(msg)
            continue

        tfm_dll_path = build_dir / "CombinedSubjects.dll"
        if not tfm_dll_path.exists():
            msg = f"{technology}: CombinedSubjects.dll not produced for {tfm}"
            errors.append(msg)
            continue

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
            errors.append(msg)
            continue

        if result.returncode != 0:
            msg = f"{technology}: runner exited with code {result.returncode} (0x{result.returncode & 0xffffffff:08x})"
            errors.append(msg)
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
            continue

        # Parse output
        records = _parse_runner_output(result.stdout or "")
        if not records:
            msg = f"{technology}: no benchmark results returned"
            errors.append(msg)
            continue

        # Write to perf store (append after first to keep all technologies in one file)
        perf_path = _RESULTS_BASE / ctx.assembly / slug / "perf" / "benchmark-history.jsonl"
        completed = _write_perf_records(
            perf_path, slug, technology, records, metadata_methods, now,
            append=not is_first)

        print(f"  [managed-benchmark] {technology}: {completed}/{len(records)} methods OK -> {perf_path}")
        techs_run.append(technology)

    duration_ms = int((time.perf_counter() - start) * 1000)
    status = "error" if not techs_run else "passed" if not errors else "passed_with_errors"
    summary = f"{len(techs_run)}/{len(_TFM_TECH)} technologies: {', '.join(techs_run) or 'none'}"
    if errors:
        summary += f"; errors: {'; '.join(errors[:3])}"

    print(f"  [managed-benchmark] Done ({duration_ms}ms): {summary}")

    return StageResult(
        stage="managed_benchmark", status=status,
        summary=summary,
        details={"technologiesRun": techs_run, "technologiesTotal": len(_TFM_TECH),
                 "methodCount": method_count, "errors": errors},
        duration_ms=duration_ms,
    )
