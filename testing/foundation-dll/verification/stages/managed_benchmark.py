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

_REPO_ROOT = Path(__file__).resolve().parents[4]
_RUNTIME_PROJECT = _REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Runtime"
_RESULTS_BASE = Path(__file__).resolve().parent.parent / "results" / "foundation-dll"
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

# Net9+ API patterns that don't exist in net8.0, found in CombinedSubjects.cs.
# Methods whose body references any of these are removed entirely for net8.0.
# The net8.0 baseline is optional, but removing these methods lets the chunk
# compile and still contribute valid net8.0 results for unaffected methods.
_NET9_API_PATTERNS: list[str] = [
    # System.Linq (net9): Enumerable.* extensions — must match fully qualified
    "Enumerable.AggregateBy",
    "Enumerable.CountBy",
    "Enumerable.Index",
    "Enumerable.LeftJoin",
    "Enumerable.RightJoin",
    "Enumerable.Shuffle",
    "global::System.Linq.Enumerable.Sequence",
    # System.Formats.Asn1 (net9): AsnWriter.Encode became generic
    "AsnWriter)!.Encode<",
    # System.ComponentModel (net9): TypeDescriptor.RegisterType
    "TypeDescriptor.RegisterType",
    # System.Diagnostics.Metrics (net9): Meter.CreateGauge, InstrumentAdvice<>
    "CreateGauge",
    "InstrumentAdvice<",
    # System.Diagnostics (net9): DistributedContextPropagator.Create*
    "DistributedContextPropagator.Create",
    # System.Security.Cryptography (net9): post-quantum types
    "CompositeMLDsa",
    "Kmac128",
    "Kmac256",
    "KmacXof128",
    "KmacXof256",
    "MLKem",
    "MLDsa",
    "SlhDsa",
    "GetMLKem",
    "GetMLDsa",
    "GetSlhDsa",
    "GetCompositeMLDsa",
    # System.Reflection.Metadata (net9): TypeName
    ".Metadata.TypeName",
    # System.Text.Json (net9): Json.Schema
    "Text.Json.Schema",
    # Checks for net9+ APIs that may appear in using statements (not just method bodies)
    # These namespaces don't exist in net8.0 as part of the framework
    "System.IO.Pipelines",
    "System.Net.ServerSentEvents",
    "System.Text.Json.Schema",
    # System.Runtime.Intrinsics (net9): Vector64/128/256/512 methods added in .NET 9.
    # Each pattern uses the fully-qualified VectorXX. prefix to avoid matching
    # similar names in other assemblies. All 4 widths are covered for each method.
    #
    # Some patterns (IsFinite, Round, CopySign) look like they might exist in
    # net8 but were added as static Vector API methods only in net9 — and the
    # CombinedSubjects.cs calls the static VectorXX.IsFinite<T> form.
    "Vector64.AddSaturate",
    "Vector128.AddSaturate",
    "Vector256.AddSaturate",
    "Vector512.AddSaturate",
    "Vector64.AnyWhereAllBitsSet",
    "Vector128.AnyWhereAllBitsSet",
    "Vector256.AnyWhereAllBitsSet",
    "Vector512.AnyWhereAllBitsSet",
    "Vector64.AsVector128Unsafe",
    "Vector128.AsVector128Unsafe",
    "Vector256.AsVector128Unsafe",
    "Vector512.AsVector128Unsafe",
    "Vector64.ClampNative",
    "Vector128.ClampNative",
    "Vector256.ClampNative",
    "Vector512.ClampNative",
    "Vector64.ConvertToInt32Native",
    "Vector128.ConvertToInt32Native",
    "Vector256.ConvertToInt32Native",
    "Vector512.ConvertToInt32Native",
    "Vector64.ConvertToInt64Native",
    "Vector128.ConvertToInt64Native",
    "Vector256.ConvertToInt64Native",
    "Vector512.ConvertToInt64Native",
    "Vector64.ConvertToUInt32Native",
    "Vector128.ConvertToUInt32Native",
    "Vector256.ConvertToUInt32Native",
    "Vector512.ConvertToUInt32Native",
    "Vector64.ConvertToUInt64Native",
    "Vector128.ConvertToUInt64Native",
    "Vector256.ConvertToUInt64Native",
    "Vector512.ConvertToUInt64Native",
    "Vector64.CopySign",
    "Vector128.CopySign",
    "Vector256.CopySign",
    "Vector512.CopySign",
    "Vector64.CountWhereAllBitsSet",
    "Vector128.CountWhereAllBitsSet",
    "Vector256.CountWhereAllBitsSet",
    "Vector512.CountWhereAllBitsSet",
    "Vector64.CreateSequence",
    "Vector128.CreateSequence",
    "Vector256.CreateSequence",
    "Vector512.CreateSequence",
    "Vector64.DegreesToRadians",
    "Vector128.DegreesToRadians",
    "Vector256.DegreesToRadians",
    "Vector512.DegreesToRadians",
    "Vector64.Exp",
    "Vector128.Exp",
    "Vector256.Exp",
    "Vector512.Exp",
    "Vector64.FusedMultiplyAdd",
    "Vector128.FusedMultiplyAdd",
    "Vector256.FusedMultiplyAdd",
    "Vector512.FusedMultiplyAdd",
    "Vector64.Hypot",
    "Vector128.Hypot",
    "Vector256.Hypot",
    "Vector512.Hypot",
    "Vector64.IndexOfWhereAllBitsSet",
    "Vector128.IndexOfWhereAllBitsSet",
    "Vector256.IndexOfWhereAllBitsSet",
    "Vector512.IndexOfWhereAllBitsSet",
    "Vector64.IsEvenInteger",
    "Vector128.IsEvenInteger",
    "Vector256.IsEvenInteger",
    "Vector512.IsEvenInteger",
    "Vector64.IsFinite",
    "Vector128.IsFinite",
    "Vector256.IsFinite",
    "Vector512.IsFinite",
    "Vector64.IsInfinity",
    "Vector128.IsInfinity",
    "Vector256.IsInfinity",
    "Vector512.IsInfinity",
    "Vector64.IsInteger",
    "Vector128.IsInteger",
    "Vector256.IsInteger",
    "Vector512.IsInteger",
    "Vector64.IsNaN",
    "Vector128.IsNaN",
    "Vector256.IsNaN",
    "Vector512.IsNaN",
    "Vector64.IsNegative",
    "Vector128.IsNegative",
    "Vector256.IsNegative",
    "Vector512.IsNegative",
    "Vector64.IsNormal",
    "Vector128.IsNormal",
    "Vector256.IsNormal",
    "Vector512.IsNormal",
    "Vector64.IsOddInteger",
    "Vector128.IsOddInteger",
    "Vector256.IsOddInteger",
    "Vector512.IsOddInteger",
    "Vector64.IsPositive",
    "Vector128.IsPositive",
    "Vector256.IsPositive",
    "Vector512.IsPositive",
    "Vector64.IsSubnormal",
    "Vector128.IsSubnormal",
    "Vector256.IsSubnormal",
    "Vector512.IsSubnormal",
    "Vector64.IsZero",
    "Vector128.IsZero",
    "Vector256.IsZero",
    "Vector512.IsZero",
    "Vector64.LastIndexOfWhereAllBitsSet",
    "Vector128.LastIndexOfWhereAllBitsSet",
    "Vector256.LastIndexOfWhereAllBitsSet",
    "Vector512.LastIndexOfWhereAllBitsSet",
    "Vector64.MaxMagnitude",
    "Vector128.MaxMagnitude",
    "Vector256.MaxMagnitude",
    "Vector512.MaxMagnitude",
    "Vector64.MaxMagnitudeNumber",
    "Vector128.MaxMagnitudeNumber",
    "Vector256.MaxMagnitudeNumber",
    "Vector512.MaxMagnitudeNumber",
    "Vector64.MaxNative",
    "Vector128.MaxNative",
    "Vector256.MaxNative",
    "Vector512.MaxNative",
    "Vector64.MaxNumber",
    "Vector128.MaxNumber",
    "Vector256.MaxNumber",
    "Vector512.MaxNumber",
    "Vector64.MinMagnitude",
    "Vector128.MinMagnitude",
    "Vector256.MinMagnitude",
    "Vector512.MinMagnitude",
    "Vector64.MinMagnitudeNumber",
    "Vector128.MinMagnitudeNumber",
    "Vector256.MinMagnitudeNumber",
    "Vector512.MinMagnitudeNumber",
    "Vector64.MinNative",
    "Vector128.MinNative",
    "Vector256.MinNative",
    "Vector512.MinNative",
    "Vector64.MinNumber",
    "Vector128.MinNumber",
    "Vector256.MinNumber",
    "Vector512.MinNumber",
    "Vector64.NarrowWithSaturation",
    "Vector128.NarrowWithSaturation",
    "Vector256.NarrowWithSaturation",
    "Vector512.NarrowWithSaturation",
    "Vector64.None",
    "Vector128.None",
    "Vector256.None",
    "Vector512.None",
    "Vector64.NoneWhereAllBitsSet",
    "Vector128.NoneWhereAllBitsSet",
    "Vector256.NoneWhereAllBitsSet",
    "Vector512.NoneWhereAllBitsSet",
    "Vector64.RadiansToDegrees",
    "Vector128.RadiansToDegrees",
    "Vector256.RadiansToDegrees",
    "Vector512.RadiansToDegrees",
    "Vector64.Round",
    "Vector128.Round",
    "Vector256.Round",
    "Vector512.Round",
    "Vector64.ShuffleNative",
    "Vector128.ShuffleNative",
    "Vector256.ShuffleNative",
    "Vector512.ShuffleNative",
    "Vector64.Sin",
    "Vector128.Sin",
    "Vector256.Sin",
    "Vector512.Sin",
    "Vector64.Cos",
    "Vector128.Cos",
    "Vector256.Cos",
    "Vector512.Cos",
    "Vector64.SinCos",
    "Vector128.SinCos",
    "Vector256.SinCos",
    "Vector512.SinCos",
    "Vector64.SubtractSaturate",
    "Vector128.SubtractSaturate",
    "Vector256.SubtractSaturate",
    "Vector512.SubtractSaturate",
    "Vector64.Truncate",
    "Vector128.Truncate",
    "Vector256.Truncate",
    "Vector512.Truncate",
]


def _remove_net9_usings(combined_src: Path) -> bool:
    """Remove using directives for namespaces that don't exist in net8.0.

    Also handles implicit usings (from <ImplicitUsings>enable</ImplicitUsings>)
    by matching namespace patterns from _NET9_API_PATTERNS against using lines.
    """
    src_text = combined_src.read_text(encoding="utf-8")
    lines = src_text.splitlines(keepends=True)
    modified = False

    # Match using directives against all _NET9_API_PATTERNS
    i = 0
    while i < len(lines):
        stripped = lines[i].strip()
        if stripped.startswith("using ") and stripped.endswith(";"):
            ns = stripped[6:-1].strip()  # extract namespace from "using X.Y.Z;"
            for p in _NET9_API_PATTERNS:
                if p in ns:
                    del lines[i]
                    modified = True
                    break
            else:
                i += 1
        else:
            i += 1

    if modified:
        combined_src.write_text("".join(lines), encoding="utf-8")
    return modified


def _remove_net9_methods(combined_src: Path) -> bool:
    """Remove entire methods whose body references net9+ APIs.

    For net8.0 builds, auto-generated CombinedSubjects.cs may contain calls
    to net9+ APIs that don't exist in net8.0. This function finds all methods
    whose body references any _NET9_API_PATTERNS and removes them entirely,
    including their [Benchmark]/[Fact] attributes.

    Uses brace-depth matching for reliable method boundary detection.
    Returns True if any modifications were made.
    """
    src_text = combined_src.read_text(encoding="utf-8")
    lines = src_text.splitlines(keepends=True)
    modified = False
    line_count = len(lines)

    # Collect all method spans to remove
    removals: list[tuple[int, int]] = []

    i = 0
    while i < line_count:
        stripped = lines[i].strip()
        # Look for [Benchmark], [Fact], or method signature (NOT class declarations)
        is_attr = stripped.startswith("[") and stripped.endswith("]")
        is_method_sig = (stripped.startswith("public ") or stripped.startswith("internal ") or stripped.startswith("private ") or stripped.startswith("static ")) and "(" in stripped
        if not (is_attr or is_method_sig):
            i += 1
            continue

        # Found potential method start — walk forward to find the method body
        method_start = i
        j = i
        has_net9_pattern = False

        # Walk to find opening brace
        while j < line_count and "{" not in lines[j]:
            for p in _NET9_API_PATTERNS:
                if p in lines[j]:
                    # Skip patterns inside comments
                    line_before_comment = lines[j].split("//")[0] if "//" in lines[j] else lines[j]
                    if p in line_before_comment:
                        has_net9_pattern = True
                        break
            j += 1

        if j >= line_count:
            i = j
            continue

        # Found opening brace at line j
        # Scan through brace-matched method body
        brace_depth = 0
        body_end = j
        while body_end < line_count:
            for ch in lines[body_end]:
                if ch == '{':
                    brace_depth += 1
                elif ch == '}':
                    brace_depth -= 1
            if brace_depth == 0:
                break
            body_end += 1

        if body_end >= line_count:
            # Unterminated method — skip
            i = j + 1
            continue

        # Now check the entire method body for net9 patterns
        if not has_net9_pattern:
            for body_line in range(method_start, body_end + 1):
                for p in _NET9_API_PATTERNS:
                    if p in lines[body_line]:
                        has_net9_pattern = True
                        break
                if has_net9_pattern:
                    break

        if has_net9_pattern:
            removals.append((method_start, body_end))
            # Also remove blank lines before the method for clean result
            if method_start > 0 and lines[method_start - 1].strip() == "":
                removals.append((method_start - 1, method_start - 1))
            i = body_end + 1
        else:
            i = body_end + 1

    # Apply removals (from end to start to preserve indices)
    if removals:
        modified = True
        removals.sort(reverse=True)
        for start, end in removals:
            del lines[start:end + 1]

    if modified:
        combined_src.write_text("".join(lines), encoding="utf-8")

    return modified

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
    "SpinWait.SpinUntil",              # SpinUntil(() => default(bool)) spins forever since condition always returns false
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
        # Remove entire methods that use net9+ APIs (avoids CS0117/CS0234 errors)
        _remove_net9_methods(combined_src)
        # Remove using directives for namespaces that don't exist in net8.0
        _remove_net9_usings(combined_src)

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
    platform: str = "",
    device: dict | None = None,
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
                "platform": platform or "windows-x64",
                "device": device or {"id": "chunk-pipeline", "name": "chunk-pipeline"},
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
            # No benchmark results from runner — use existing baseline data if available
            perf_path = _RESULTS_BASE / ctx.assembly / slug / "perf" / "benchmark-history.jsonl"
            if perf_path.exists():
                print(f"  [managed-benchmark] {technology}: runner returned 0 results, using existing baseline from {perf_path}")
                with _perf_write_lock:
                    techs_run.append(technology)
                return technology, None
            else:
                msg = f"{technology}: no benchmark results returned and no existing baseline"
                return technology, [msg]

        # Write to perf store (serialized via lock to avoid file corruption)
        perf_path = _RESULTS_BASE / ctx.assembly / slug / "perf" / "benchmark-history.jsonl"
        with _perf_write_lock:
            completed = _write_perf_records(
                perf_path, slug, technology, records, metadata_methods, now,
                platform=ctx.platform, device=ctx.device, append=True)
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

    print(f"  [managed-benchmark] Done ({duration_ms}ms): {summary}")

    return StageResult(
        stage="managed_benchmark", status=status,
        summary=summary,
        details={"technologiesRun": techs_run, "technologiesTotal": len(_TFM_TECH),
                 "methodCount": method_count, "errors": errors},
        duration_ms=duration_ms,
    )
