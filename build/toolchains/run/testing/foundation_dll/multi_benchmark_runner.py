"""Multi-Runtime Benchmark Runner — orchestrates benchmark execution across
.NET JIT (net8.0/net10.0), Mono, Chaos IL2CPP AOT/JIT/HotUpdate, and MS Native AOT.

Usage:
    python multi_benchmark_runner.py <family-dir> --runtimes net8-jit,chaos-aot,chaos-hu-post
    python multi_benchmark_runner.py <family-dir> --runtimes all

Output: <family-dir>/multi-run/multi-run-report.json
         <family-dir>/multi-run/per-runtime/    (raw per-runtime results)
"""

from __future__ import annotations

import json
import math
import os
import re
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any

# ═══════════════════════════════════════════════════════════════════════════════
# Runtime Definitions
# ═══════════════════════════════════════════════════════════════════════════════

@dataclass
class RuntimeSpec:
    """Specification for a single benchmark runtime."""
    alias: str                        # short label: "net8-jit", "chaos-aot", etc.
    display_name: str                 # human-readable: ".NET 8.0 JIT"
    runner_type: str                  # "dotnet-run" | "mono" | "native" | "native-hotupdate"
    target_framework: str | None      # "net8.0", "net10.0", None for native
    required: bool = False            # fail if unavailable
    kind: str = "managed"             # "managed" | "aot" | "jit" | "hotupdate"

RUNTIME_SPECS: list[RuntimeSpec] = [
    # ── Primary: run every cycle ────────────────────────────────────────
    RuntimeSpec("net8-jit",    ".NET 8.0 JIT",          "dotnet-run", "net8.0",  required=True,  kind="managed"),
    RuntimeSpec("chaos-aot",   "Chaos IL2CPP AOT",      "native",     None,      required=True,  kind="aot"),

    # ── Extension: run weekly or on-demand ──────────────────────────────
    RuntimeSpec("net10-jit",   ".NET 10.0 JIT",         "dotnet-run", "net10.0", required=False, kind="managed"),
    RuntimeSpec("net8-aot",    ".NET 8.0 Native AOT",   "dotnet-aot", "net8.0",  required=False, kind="aot"),
    RuntimeSpec("net10-aot",   ".NET 10.0 Native AOT",  "dotnet-aot", "net10.0", required=False, kind="aot"),
    RuntimeSpec("chaos-jit",   "Chaos IL2CPP JIT",      "native",     None,      required=False, kind="jit"),
    RuntimeSpec("mono",        "Mono JIT",              "mono",       "net8.0",  required=False, kind="managed"),

    # ── HotUpdate: post-patch interpreter path (always runs after patch) ─
    RuntimeSpec("chaos-hu-aot",  "Chaos HotUpdate AOT (post-patch)",  "native-hotupdate", None, required=False, kind="hotupdate"),
    RuntimeSpec("chaos-hu-jit",  "Chaos HotUpdate JIT (post-patch)",  "native-hotupdate", None, required=False, kind="hotupdate"),
]

RUNTIME_ALIASES = {s.alias: s for s in RUNTIME_SPECS}

# ═══════════════════════════════════════════════════════════════════════════════
# Data Models
# ═══════════════════════════════════════════════════════════════════════════════

@dataclass
class PerfSample:
    """Benchmark measurement for a single method on a single runtime."""
    runtime: str
    mean_ns: float
    stddev_ns: float = 0.0
    samples: int = 0
    status: str = "ok"               # "ok" | "stub" | "throws" | "unsupported" | "error"
    error: str = ""

@dataclass
class MethodBenchResult:
    """All measurements for a single method across all runtimes."""
    subject_id: str
    method_index: int
    label: str = ""
    samples: dict[str, PerfSample] = field(default_factory=dict)  # runtime_alias -> sample

@dataclass
class MultiRunSummary:
    """Per-runtime aggregate metrics for a domain/family."""
    runtime: str
    geometric_mean_ns: float = 0.0
    min_ns: float = 0.0
    max_ns: float = 0.0
    median_ns: float = 0.0
    method_count: int = 0
    ok_count: int = 0
    stub_count: int = 0
    throws_count: int = 0
    unsupported_count: int = 0

@dataclass
class RatioSummary:
    """Ratio comparisons between runtimes."""
    label: str                       # e.g. "chaos-aot / net8-jit"
    numerator: str
    denominator: str
    geometric_mean_ratio: float = 0.0
    min_ratio: float = 0.0
    max_ratio: float = 0.0
    faster_count: int = 0            # numerator faster than denominator
    slower_count: int = 0
    equal_count: int = 0
    chaos_classification: str = ""   # for chaos-aot specifically

@dataclass
class RegressionAlert:
    """Detected regression between current and baseline run."""
    method_index: int
    subject_id: str
    runtime: str
    metric: str                      # "mean_ns" | "ratio"
    before_value: float
    after_value: float
    delta_pct: float
    threshold_pct: float = 10.0
    alert: bool = False

@dataclass
class MultiRunReport:
    """Top-level multi-runtime benchmark report."""
    family: str
    assembly: str
    timestamp: str = ""
    duration_ms: int = 0
    config: dict[str, Any] = field(default_factory=dict)

    # Per-method raw data
    methods: list[MethodBenchResult] = field(default_factory=list)

    # Per-runtime aggregates
    summaries: list[MultiRunSummary] = field(default_factory=list)

    # Ratio comparisons
    ratios: list[RatioSummary] = field(default_factory=list)

    # Regression
    regression_alerts: list[RegressionAlert] = field(default_factory=list)

    # Runtime availability
    runtimes_available: list[str] = field(default_factory=list)
    runtimes_unavailable: list[str] = field(default_factory=list)

    # Classification (for chaos-aot specifically)
    chaos_classification_breakdown: dict[str, int] = field(default_factory=dict)


# ═══════════════════════════════════════════════════════════════════════════════
# Capabilities & Prerequisites
# ═══════════════════════════════════════════════════════════════════════════════

class DotNetSdkDetector:
    """Detect available .NET SDKs, Mono, and MS AOT compilers."""

    @staticmethod
    def list_frameworks() -> list[str]:
        """Return list of available TFMs: ['net8.0', 'net9.0', 'net10.0']."""
        try:
            r = subprocess.run(
                ["dotnet", "--list-sdks"],
                capture_output=True, text=True, timeout=15,
            )
            versions = []
            for line in r.stdout.strip().splitlines():
                m = re.match(r'(\d+\.\d+)\.', line.strip())
                if m:
                    v = m.group(1)
                    versions.append(f"net{v}")
            return sorted(set(versions))
        except Exception:
            return []

    @staticmethod
    def mono_available() -> bool:
        return shutil.which("mono") is not None

    @staticmethod
    def check_aot_pack(tfm: str) -> bool:
        """Check if the .NET AOT publishing pack is installed for a given TFM."""
        try:
            r = subprocess.run(
                ["dotnet", "--info"],
                capture_output=True, text=True, timeout=15,
            )
            # AOT packs are listed under "Runtimes" in dotnet --info
            aot_indicator = f"Microsoft.NETCore.App.Ref/{tfm}" in r.stdout
            return aot_indicator
        except Exception:
            return False


def detect_available_runtimes(requested: list[str]) -> tuple[list[str], list[str]]:
    """Given a list of runtime aliases, return (available, unavailable).

    If requested == ['all'], use all RUNTIME_SPECS.
    """
    if not requested or requested == ["all"]:
        specs = RUNTIME_SPECS
    else:
        specs = [RUNTIME_ALIASES[a] for a in requested if a in RUNTIME_ALIASES]

    frameworks = DotNetSdkDetector.list_frameworks()
    mono_ok = DotNetSdkDetector.mono_available()

    available: list[str] = []
    unavailable: list[str] = []

    for spec in specs:
        if spec.runner_type == "dotnet-run":
            if spec.target_framework and spec.target_framework in frameworks:
                available.append(spec.alias)
            else:
                unavailable.append(spec.alias)
        elif spec.runner_type == "dotnet-aot":
            if (spec.target_framework and spec.target_framework in frameworks
                    and DotNetSdkDetector.check_aot_pack(spec.target_framework)):
                available.append(spec.alias)
            else:
                unavailable.append(spec.alias)
        elif spec.runner_type == "mono":
            if mono_ok:
                available.append(spec.alias)
            else:
                unavailable.append(spec.alias)
        elif spec.runner_type in ("native", "native-hotupdate"):
            # These are always available if the entry exe exists
            available.append(spec.alias)
        else:
            unavailable.append(spec.alias)

    return available, unavailable


# ═══════════════════════════════════════════════════════════════════════════════
# Managed Benchmark Harness
# ═══════════════════════════════════════════════════════════════════════════════

def adapt_csproj_for_multitarget(csproj_path: Path, target_frameworks: list[str]) -> None:
    """Modify an existing benchmark harness .csproj to use multi-targeting.

    Reads the csproj, replaces single <TargetFramework> with <TargetFrameworks>,
    and writes it back.
    """
    content = csproj_path.read_text(encoding="utf-8")
    tfms = ";".join(target_frameworks)
    content = re.sub(
        r'<TargetFramework>([^<]+)</TargetFramework>',
        f'<TargetFrameworks>{tfms}</TargetFrameworks>',
        content,
    )
    # Suppress auto-generated Program.Main — harness has explicit Main()
    if '<GenerateProgramFile>' not in content:
        content = content.replace("  </PropertyGroup>",
            "    <GenerateProgramFile>false</GenerateProgramFile>\n  </PropertyGroup>")
    csproj_path.write_text(content, encoding="utf-8")


def _find_or_generate_managed_harness(
    family_dir: Path,
    family_slug: str,
    assembly: str,
    method_subject_ids: list[str],
) -> Path | None:
    """Find the managed benchmark harness dir; generate if missing.

    Returns the path to the harness directory, or None if generation failed.
    """
    harness_dir = family_dir / "managed_test" / "benchmarks"
    harness_cs = harness_dir / "ManagedBenchmarkHarness.cs"
    harness_csproj = harness_dir / "ManagedBenchmarkHarness.csproj"

    if harness_csproj.exists() and harness_cs.exists():
        return harness_dir

    # If not exist, we need to delegate to the existing auto-generation.
    # Since _auto_generate_managed_benchmark is in family_verification_orchestrator.py,
    # we import it dynamically.
    print(f"  [multi-runner] Managed harness not found at {harness_dir}, generating...")
    # The harness will be generated by the parent pipeline stage.
    # If we're running standalone, we need to do it here.
    # For now, return None so callers can fallback.
    return None


# ═══════════════════════════════════════════════════════════════════════════════
# Per-Runtime Runners
# ═══════════════════════════════════════════════════════════════════════════════

def _run_net_benchmark(
    harness_dir: Path,
    tfm: str,
    iterations: int = 100000,
) -> dict[str, Any]:
    """Run the managed benchmark harness under a specific .NET TFM via dotnet run.

    Returns: {method_results: [{method_index, elapsed_ms, iterations, status}],
              runtime_info: {sdk_version}}
    """
    csproj = harness_dir / "ManagedBenchmarkHarness.csproj"
    if not csproj.exists():
        return {"method_results": [], "runtime_info": {}, "error": "harness not found"}

    print(f"  [multi-runner] Running .NET {tfm} benchmark...")
    start = time.perf_counter()
    try:
        r = subprocess.run(
            ["dotnet", "run", "--project", str(csproj), "-f", tfm,
             "--configuration", "Release", "--no-build"],
            capture_output=True, text=True, timeout=300,
        )
        elapsed = time.perf_counter() - start
        if r.returncode != 0:
            # First run may need build
            r = subprocess.run(
                ["dotnet", "run", "--project", str(csproj), "-f", tfm,
                 "--configuration", "Release"],
                capture_output=True, text=True, timeout=300,
            )
            elapsed = time.perf_counter() - start
            if r.returncode != 0:
                return {
                    "method_results": [],
                    "runtime_info": {"tfm": tfm},
                    "error": f"exit_code={r.returncode}: {r.stderr[:200]}",
                }

        data = json.loads(r.stdout)
        method_results_raw = data.get("results", [])
        # Normalize camelCase keys from managed harness to snake_case
        method_results = []
        for item in method_results_raw:
            method_results.append({
                "method_index": item.get("methodIndex", -1),
                "elapsed_ms": item.get("elapsedMilliseconds", 0),
                "iterations": item.get("iterations", iterations),
                "status": "ok" if item.get("isBodyReal", False) else "stub",
            })

        # Get SDK version
        sdk_r = subprocess.run(
            ["dotnet", "--version"], capture_output=True, text=True, timeout=10
        )

        return {
            "method_results": method_results,
            "runtime_info": {"tfm": tfm, "sdk_version": sdk_r.stdout.strip()},
            "duration_s": round(elapsed, 2),
        }
    except subprocess.TimeoutExpired:
        return {"method_results": [], "runtime_info": {"tfm": tfm},
                "error": "timeout (300s)"}
    except Exception as e:
        return {"method_results": [], "runtime_info": {"tfm": tfm},
                "error": str(e)}


def _run_mono_benchmark(
    harness_dir: Path,
    tfm: str,
    iterations: int = 100000,
) -> dict[str, Any]:
    """Build the harness for net8.0, then run with Mono."""
    csproj = harness_dir / "ManagedBenchmarkHarness.csproj"
    build_dir = harness_dir / "bin" / "Release" / tfm
    exe_path = build_dir / "ManagedBenchmarkHarness.exe"

    print(f"  [multi-runner] Running Mono benchmark...")
    try:
        # Build for the target TFM
        r = subprocess.run(
            ["dotnet", "build", str(csproj), "-f", tfm, "--configuration", "Release"],
            capture_output=True, text=True, timeout=120,
        )
        if r.returncode != 0:
            return {"method_results": [], "runtime_info": {}, "error": f"build failed: {r.stderr[:200]}"}

        if not exe_path.exists():
            return {"method_results": [], "runtime_info": {}, "error": f"exe not found at {exe_path}"}

        start = time.perf_counter()
        r = subprocess.run(
            ["mono", str(exe_path)],
            capture_output=True, text=True, timeout=300,
        )
        elapsed = time.perf_counter() - start
        if r.returncode != 0:
            return {"method_results": [], "runtime_info": {},
                    "error": f"mono exit_code={r.returncode}: {r.stderr[:200]}"}

        data = json.loads(r.stdout)
        method_results_raw = data.get("results", [])
        method_results = []
        for item in method_results_raw:
            method_results.append({
                "method_index": item.get("methodIndex", -1),
                "elapsed_ms": item.get("elapsedMilliseconds", 0),
                "iterations": item.get("iterations", iterations),
                "status": "ok" if item.get("isBodyReal", False) else "stub",
            })
        return {
            "method_results": method_results,
            "runtime_info": {"tfm": tfm, "runner": "mono"},
            "duration_s": round(elapsed, 2),
        }
    except subprocess.TimeoutExpired:
        return {"method_results": [], "runtime_info": {}, "error": "timeout (300s)"}
    except Exception as e:
        return {"method_results": [], "runtime_info": {}, "error": str(e)}


def _run_ms_aot_benchmark(
    harness_dir: Path,
    tfm: str,
    iterations: int = 100000,
) -> dict[str, Any]:
    """Publish the harness as MS Native AOT (ILC), then run the native exe."""
    csproj = harness_dir / "ManagedBenchmarkHarness.csproj"
    publish_dir = harness_dir / "publish" / f"{tfm}-aot"
    exe_path = publish_dir / "ManagedBenchmarkHarness.exe"

    print(f"  [multi-runner] Publishing .NET {tfm} Native AOT...")
    try:
        r = subprocess.run(
            ["dotnet", "publish", str(csproj), "-f", tfm, "--configuration", "Release",
             "-p:PublishAot=true", "-p:StripSymbols=true",
             "-o", str(publish_dir)],
            capture_output=True, text=True, timeout=300,
        )
        if r.returncode != 0:
            # Try without StripSymbols
            r = subprocess.run(
                ["dotnet", "publish", str(csproj), "-f", tfm, "--configuration", "Release",
                 "-p:PublishAot=true",
                 "-o", str(publish_dir)],
                capture_output=True, text=True, timeout=300,
            )
        if r.returncode != 0:
            return {"method_results": [], "runtime_info": {"tfm": tfm},
                    "error": f"publish failed: {r.stderr[:200]}"}

        if not exe_path.exists():
            return {"method_results": [], "runtime_info": {"tfm": tfm},
                    "error": "AOT exe not found after publish"}

        print(f"  [multi-runner] Running .NET {tfm} Native AOT...")
        start = time.perf_counter()
        r = subprocess.run(
            [str(exe_path)],
            capture_output=True, text=True, timeout=300,
        )
        elapsed = time.perf_counter() - start
        if r.returncode != 0:
            return {"method_results": [], "runtime_info": {"tfm": tfm},
                    "error": f"aot exit_code={r.returncode}: {r.stderr[:200]}"}

        data = json.loads(r.stdout)
        return {
            "method_results": data.get("results", []),
            "runtime_info": {"tfm": tfm, "runner": "MS Native AOT (ILC)"},
            "duration_s": round(elapsed, 2),
        }
    except subprocess.TimeoutExpired:
        return {"method_results": [], "runtime_info": {"tfm": tfm},
                "error": "timeout (300s)"}
    except Exception as e:
        return {"method_results": [], "runtime_info": {"tfm": tfm},
                "error": str(e)}


def _run_native_benchmark(
    exe_path: Path,
    method_count: int,
    iterations: int = 100000,
    flag: str = "--benchmark",
) -> dict[str, Any]:
    """Run a native entry.exe --benchmark across all methods.

    Args:
        exe_path: path to entry-aot.exe or entry-jit.exe
        method_count: number of methods to benchmark (kAotMethodCount)
        iterations: iterations per method
        flag: "--benchmark" for normal, "--hotupdate-and-benchmark" for hotupdate

    Returns: {method_results: [{method_index, elapsed_ms, status}]}
    """
    if not exe_path.exists():
        return {"method_results": [], "error": f"exe not found: {exe_path}"}

    results = []
    total_start = time.perf_counter()

    for idx in range(method_count):
        try:
            r = subprocess.run(
                [str(exe_path), flag, str(idx), str(iterations)],
                capture_output=True, text=True, timeout=300,
            )
            if r.returncode != 0:
                results.append({
                    "method_index": idx,
                    "status": "error",
                    "error": f"exit_code={r.returncode}",
                })
                continue

            # Parse JSON from last line of stdout
            for line in reversed(r.stdout.strip().splitlines()):
                line = line.strip()
                if line.startswith("{"):
                    try:
                        data = json.loads(line)
                        # entry.exe uses camelCase keys: elapsedMilliseconds, calibratedMs
                        elapsed_ms = data.get("elapsedMilliseconds",
                                      data.get("elapsed_ms", 0))
                        calibrated_ms = data.get("calibratedMs",
                                           data.get("calibrated_ms", 0))
                        iters = data.get("iterations", iterations)
                        ns_per_op = (elapsed_ms * 1_000_000) / max(iters, 1)
                        results.append({
                            "method_index": idx,
                            "elapsed_ms": elapsed_ms,
                            "calibrated_ms": calibrated_ms,
                            "iterations": iters,
                            "ns_per_op": ns_per_op,
                            "status": "ok",
                        })
                    except json.JSONDecodeError:
                        results.append({"method_index": idx, "status": "error", "error": "json parse failed"})
                    break
            else:
                results.append({"method_index": idx, "status": "error", "error": "no JSON in stdout"})
        except subprocess.TimeoutExpired:
            results.append({"method_index": idx, "status": "error", "error": "timeout"})
        except Exception as e:
            results.append({"method_index": idx, "status": "error", "error": str(e)})

    total_elapsed = time.perf_counter() - total_start
    return {
        "method_results": results,
        "duration_s": round(total_elapsed, 2),
    }


# ═══════════════════════════════════════════════════════════════════════════════
# Aggregation & Ratio Computation
# ═══════════════════════════════════════════════════════════════════════════════

def _geometric_mean(values: list[float]) -> float:
    """Compute geometric mean (more appropriate for benchmark ratios)."""
    if not values:
        return 0.0
    log_sum = sum(math.log(v) for v in values if v > 0)
    return math.exp(log_sum / len(values))


def _classify_chaos_speedup(ratio: float) -> str:
    """Classify a single method's chaos-aot / net8-jit ratio."""
    if ratio >= 2.0:
        return "chaos_faster_than_all"      # chaos is 2x+ faster than .NET JIT
    elif ratio >= 1.0:
        return "chaos_faster_than_jit"      # chaos is faster but less than 2x
    elif ratio >= 0.5:
        return "chaos_midfield"             # chaos is 0.5x-1x of .NET JIT
    else:
        return "chaos_slower_than_jit"      # chaos is significantly slower


def _compute_summaries(
    methods: list[MethodBenchResult],
    runtimes_available: list[str],
) -> list[MultiRunSummary]:
    """Compute per-runtime aggregate metrics."""
    summaries = []

    for alias in runtimes_available:
        values = []
        ok_count = stub_count = throws_count = unsupported_count = 0
        for m in methods:
            s = m.samples.get(alias)
            if s is None:
                continue
            if s.status == "ok":
                values.append(s.mean_ns)
                ok_count += 1
            elif s.status == "stub":
                stub_count += 1
            elif s.status == "throws":
                throws_count += 1
            else:
                unsupported_count += 1

        if not values:
            continue

        sorted_v = sorted(values)
        n = len(sorted_v)
        summaries.append(MultiRunSummary(
            runtime=alias,
            geometric_mean_ns=_geometric_mean(values),
            min_ns=sorted_v[0],
            max_ns=sorted_v[-1],
            median_ns=sorted_v[n // 2] if n > 0 else 0,
            method_count=n + stub_count + throws_count + unsupported_count,
            ok_count=ok_count,
            stub_count=stub_count,
            throws_count=throws_count,
            unsupported_count=unsupported_count,
        ))

    return summaries


def _compute_ratios(
    methods: list[MethodBenchResult],
    runtimes_available: list[str],
) -> list[RatioSummary]:
    """Compute pairwise ratios between important runtime pairs."""
    # Define which comparisons to make
    comparison_pairs = [
        ("chaos-aot", "net8-jit", "Chaos AOT / .NET 8 JIT"),
        ("chaos-aot", "net8-aot", "Chaos AOT / MS Native AOT (net8)"),
        ("chaos-aot", "chaos-jit", "Chaos AOT / Chaos JIT"),
        ("chaos-jit", "net8-jit", "Chaos JIT / .NET 8 JIT"),
        ("net10-jit", "net8-jit", ".NET 10 JIT / .NET 8 JIT"),
        ("net10-aot", "net8-aot", "MS AOT net10 / MS AOT net8"),
        ("chaos-hu-aot", "chaos-aot", "Chaos HotUpdate (post-patch) / Chaos AOT"),
    ]

    ratios = []
    for num, den, label in comparison_pairs:
        if num not in runtimes_available or den not in runtimes_available:
            continue

        method_ratios = []
        faster = slower = equal = 0
        for m in methods:
            n_s = m.samples.get(num)
            d_s = m.samples.get(den)
            if n_s is None or d_s is None:
                continue
            if n_s.status != "ok" or d_s.status != "ok":
                continue
            if d_s.mean_ns <= 0:
                continue
            r = n_s.mean_ns / d_s.mean_ns
            # Invert for speedup (larger = faster): ratio = den / num
            speedup = d_s.mean_ns / n_s.mean_ns if n_s.mean_ns > 0 else 0

            method_ratios.append(speedup)
            if speedup > 1.05:
                faster += 1
            elif speedup < 0.95:
                slower += 1
            else:
                equal += 1

        if not method_ratios:
            continue

        gm = _geometric_mean(method_ratios)
        chaos_class = ""
        if num == "chaos-aot" and den == "net8-jit":
            # Compute classification breakdown
            pass  # done separately

        ratios.append(RatioSummary(
            label=label,
            numerator=num,
            denominator=den,
            geometric_mean_ratio=gm,
            min_ratio=min(method_ratios),
            max_ratio=max(method_ratios),
            faster_count=faster,
            slower_count=slower,
            equal_count=equal,
        ))

    return ratios


def _compute_chaos_classification(
    methods: list[MethodBenchResult],
    runtimes_available: list[str],
) -> dict[str, int]:
    """Classify each method by chaos-aot vs net8-jit speedup."""
    if "chaos-aot" not in runtimes_available or "net8-jit" not in runtimes_available:
        return {}

    breakdown: dict[str, int] = {
        "chaos_faster_than_all": 0,
        "chaos_faster_than_jit": 0,
        "chaos_midfield": 0,
        "chaos_slower_than_jit": 0,
        "not_benchmarked": 0,
    }

    for m in methods:
        c = m.samples.get("chaos-aot")
        j = m.samples.get("net8-jit")
        if c is None or j is None or c.status != "ok" or j.status != "ok":
            breakdown["not_benchmarked"] += 1
            continue
        ratio = j.mean_ns / c.mean_ns if c.mean_ns > 0 else 0
        cls = _classify_chaos_speedup(ratio)
        breakdown[cls] = breakdown.get(cls, 0) + 1

    return breakdown


# ═══════════════════════════════════════════════════════════════════════════════
# Regression Detection
# ═══════════════════════════════════════════════════════════════════════════════

def _load_baseline_report(baseline_path: Path) -> MultiRunReport | None:
    """Load a previous multi-run-report.json for regression comparison."""
    if not baseline_path.exists():
        return None
    try:
        data = json.loads(baseline_path.read_text(encoding="utf-8"))
        # Convert dict back to structured data (we only need methods)
        # For now, we just compare raw method data
        return data  # type: ignore
    except Exception:
        return None


def _detect_regressions(
    current: MultiRunReport,
    baseline_path: Path,
    threshold_pct: float = 10.0,
) -> list[RegressionAlert]:
    """Compare current results against baseline, return regressions."""
    baseline_data = _load_baseline_report(baseline_path)
    if baseline_data is None:
        return []

    # Build lookup: (method_index, runtime) -> mean_ns
    def _build_lookup(report_data) -> dict[tuple[int, str], float]:
        lookup = {}
        # Handle both dict and MultiRunReport
        methods = report_data.get("methods", []) if isinstance(report_data, dict) else report_data.methods
        for m in methods:
            midx = m.get("method_index", -1) if isinstance(m, dict) else m.method_index
            samples = m.get("samples", {}) if isinstance(m, dict) else m.samples
            if isinstance(samples, dict):
                for alias, sample in samples.items():
                    if isinstance(sample, dict) and sample.get("status") == "ok":
                        lookup[(midx, alias)] = sample.get("mean_ns", 0)
        return lookup

    baseline_lookup = _build_lookup(baseline_data)
    alerts = []

    for m in current.methods:
        for alias, sample in m.samples.items():
            if sample.status != "ok":
                continue
            key = (m.method_index, alias)
            if key not in baseline_lookup:
                continue
            before = baseline_lookup[key]
            after = sample.mean_ns
            if before <= 0:
                continue
            delta = (after - before) / before * 100
            if abs(delta) >= threshold_pct and delta > 0:  # only alert on slowdown
                alerts.append(RegressionAlert(
                    method_index=m.method_index,
                    subject_id=m.subject_id,
                    runtime=alias,
                    metric="mean_ns",
                    before_value=before,
                    after_value=after,
                    delta_pct=round(delta, 2),
                    threshold_pct=threshold_pct,
                    alert=True,
                ))

    return alerts


# ═══════════════════════════════════════════════════════════════════════════════
# Main Entry Point
# ═══════════════════════════════════════════════════════════════════════════════

def run_multi_benchmark(
    family_dir: Path,
    family_slug: str,
    assembly: str,
    method_subject_ids: list[str],
    *,
    runtimes: list[str] | None = None,
    iterations: int = 100000,
    exe_aot: Path | None = None,
    exe_jit: Path | None = None,
    baseline_path: Path | None = None,
) -> MultiRunReport:
    """Run the multi-runtime benchmark for a single family.

    Args:
        family_dir: path to the family verification directory
        family_slug: e.g. "convert-char"
        assembly: e.g. "System.Private.CoreLib"
        method_subject_ids: list of method subject IDs
        runtimes: list of runtime aliases to run (None = default set)
        iterations: iterations per method per runtime
        exe_aot: path to entry-aot.exe (optional, auto-detected)
        exe_jit: path to entry-jit.exe (optional, auto-detected)
        baseline_path: path to previous multi-run-report.json for regression detection

    Returns: MultiRunReport
    """
    report = MultiRunReport(
        family=family_slug,
        assembly=assembly,
        timestamp=time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
    )
    overall_start = time.perf_counter()

    # ── Resolve runtime list ───────────────────────────────────────────
    if runtimes is None:
        runtimes = ["net8-jit", "chaos-aot", "chaos-hu-aot"]
    available, unavailable = detect_available_runtimes(runtimes)
    report.runtimes_available = available
    report.runtimes_unavailable = unavailable
    report.config = {
        "requested_runtimes": runtimes,
        "available_runtimes": available,
        "unavailable_runtimes": unavailable,
        "iterations": iterations,
    }

    # Check required runtimes
    for alias in runtimes:
        spec = RUNTIME_ALIASES.get(alias)
        if spec and spec.required and alias in unavailable:
            print(f"  [multi-runner] ERROR: Required runtime '{alias}' is unavailable!")
            report.overall_status = "failed"
            return report

    print(f"  [multi-runner] Available runtimes: {available}")
    print(f"  [multi-runner] Unavailable runtimes: {unavailable}")

    # ── Prepare managed harness ────────────────────────────────────────
    managed_tfms = []
    for alias in available:
        spec = RUNTIME_ALIASES.get(alias)
        if spec and spec.runner_type in ("dotnet-run", "dotnet-aot", "mono"):
            if spec.target_framework and spec.target_framework not in managed_tfms:
                managed_tfms.append(spec.target_framework)

    harness_dir = family_dir / "managed_test" / "benchmarks"
    if managed_tfms and harness_dir.exists():
        csproj = harness_dir / "ManagedBenchmarkHarness.csproj"
        if csproj.exists():
            adapt_csproj_for_multitarget(csproj, managed_tfms)
            print(f"  [multi-runner] Adapted csproj for TFMs: {managed_tfms}")

    # ── Resolve native exe paths ───────────────────────────────────────
    if exe_aot is None:
        exe_aot = family_dir / "native" / "entry-aot.exe"
    if exe_jit is None:
        exe_jit = family_dir / "native" / "entry-jit.exe"

    method_count = len(method_subject_ids)

    # ── Collect per-runtime results ────────────────────────────────────
    raw_results: dict[str, dict[str, Any]] = {}

    for alias in available:
        spec = RUNTIME_ALIASES.get(alias)
        if spec is None:
            continue

        if spec.runner_type == "dotnet-run" and spec.target_framework:
            raw_results[alias] = _run_net_benchmark(harness_dir, spec.target_framework, iterations)

        elif spec.runner_type == "mono" and spec.target_framework:
            raw_results[alias] = _run_mono_benchmark(harness_dir, spec.target_framework, iterations)

        elif spec.runner_type == "dotnet-aot" and spec.target_framework:
            raw_results[alias] = _run_ms_aot_benchmark(harness_dir, spec.target_framework, iterations)

        elif spec.runner_type == "native":
            raw_results[alias] = _run_native_benchmark(exe_aot, method_count, iterations)

        elif spec.runner_type == "native-hotupdate":
            raw_results[alias] = _run_native_benchmark(
                exe_aot, method_count, iterations,
                flag="--hotupdate-and-benchmark",
            )

    # ── Build MethodBenchResult from raw results ───────────────────────
    methods_by_idx: dict[int, MethodBenchResult] = {}
    for idx, mid in enumerate(method_subject_ids):
        methods_by_idx[idx] = MethodBenchResult(
            subject_id=mid,
            method_index=idx,
            label=mid.split("::")[-1] if "::" in mid else mid,
        )

    for alias, raw in raw_results.items():
        for mr in raw.get("method_results", []):
            idx = mr.get("method_index", -1)
            if idx < 0 or idx not in methods_by_idx:
                continue
            elapsed_ms = mr.get("elapsed_ms", 0)
            iters = mr.get("iterations", iterations)
            ns_per_op = mr.get("ns_per_op", (elapsed_ms * 1_000_000) / max(iters, 1))
            status = mr.get("status", "ok")
            methods_by_idx[idx].samples[alias] = PerfSample(
                runtime=alias,
                mean_ns=ns_per_op,
                samples=iters,
                status=status,
                error=mr.get("error", ""),
            )

    report.methods = list(methods_by_idx.values())

    # ── Compute aggregates ─────────────────────────────────────────────
    report.summaries = _compute_summaries(report.methods, available)
    report.ratios = _compute_ratios(report.methods, available)
    report.chaos_classification_breakdown = _compute_chaos_classification(report.methods, available)

    # ── Regression detection ───────────────────────────────────────────
    if baseline_path:
        report.regression_alerts = _detect_regressions(report, baseline_path)

    report.duration_ms = int((time.perf_counter() - overall_start) * 1000)
    return report


def save_report(report: MultiRunReport, output_dir: Path) -> Path:
    """Save the multi-run report to disk.

    Writes:
        output_dir/multi-run-report.json        ← main aggregate report
        output_dir/per-runtime/                 ← raw per-runtime JSON files
    """
    output_dir.mkdir(parents=True, exist_ok=True)
    per_runtime_dir = output_dir / "per-runtime"
    per_runtime_dir.mkdir(parents=True, exist_ok=True)

    # Convert report to dict
    report_dict = asdict(report)

    # Save per-runtime raw data
    for alias in report.runtimes_available:
        raw = {}
        for m in report.methods:
            s = m.samples.get(alias)
            if s:
                raw[m.subject_id] = asdict(s)
        if raw:
            (per_runtime_dir / f"{alias}.json").write_text(
                json.dumps(raw, indent=2), encoding="utf-8")

    # Save main report
    report_path = output_dir / "multi-run-report.json"
    report_path.write_text(
        json.dumps(report_dict, indent=2, default=str), encoding="utf-8")

    print(f"  [multi-runner] Report saved to {report_path}")
    return report_path


def print_report_summary(report: MultiRunReport) -> None:
    """Print a human-readable summary of the multi-run report."""
    print()
    print(f"═ Multi-Runtime Benchmark Report ═══════════════════════════════")
    print(f"  Family: {report.family} ({report.assembly})")
    print(f"  Duration: {report.duration_ms / 1000:.1f}s")
    print()
    print(f"  Runtimes available: {', '.join(report.runtimes_available)}")
    if report.runtimes_unavailable:
        print(f"  Runtimes unavailable: {', '.join(report.runtimes_unavailable)}")

    print()
    print(f"  ── Per-Runtime Summary (geometric mean ns/op) ──────────────")
    for s in report.summaries:
        bar = "█" * max(1, int(s.geometric_mean_ns / 10))
        pm = f"  ({s.ok_count}/{s.method_count} methods)"
        print(f"    {s.runtime:20s} {s.geometric_mean_ns:>10.1f} ns/op {bar}{pm}")

    print()
    print(f"  ── Ratio Comparisons (speedup = baseline / target) ─────────")
    for r in report.ratios:
        if r.faster_count + r.slower_count + r.equal_count == 0:
            continue
        pct = r.faster_count / (r.faster_count + r.slower_count + r.equal_count) * 100
        print(f"    {r.label:45s} {r.geometric_mean_ratio:>6.2f}x  "
              f"({r.faster_count}/{r.faster_count + r.slower_count} methods faster)")

    if report.chaos_classification_breakdown:
        print()
        print(f"  ── Chaos AOT Classification ─────────────────────────────")
        bd = report.chaos_classification_breakdown
        total = sum(bd.values())
        for cls, count in sorted(bd.items()):
            pct = count / total * 100 if total > 0 else 0
            print(f"    {cls:30s} {count:>4d} ({pct:5.1f}%)")

    if report.regression_alerts:
        print()
        print(f"  ── Regression Alerts ─────────────────────────────────────")
        for a in report.regression_alerts:
            print(f"    [{a.runtime}] method {a.method_index}: {a.delta_pct:+.1f}% "
                  f"({a.before_value:.1f} → {a.after_value:.1f} ns/op)")

    print()
    print(f"══════════════════════════════════════════════════════════════════")


def main() -> None:
    """CLI entry point."""
    import argparse
    parser = argparse.ArgumentParser(description="Multi-Runtime Benchmark Runner")
    parser.add_argument("family_dir", type=Path, help="Path to family verification directory")
    parser.add_argument("--family", default="unknown", help="Family slug")
    parser.add_argument("--assembly", default="System.Private.CoreLib", help="Assembly name")
    parser.add_argument("--runtimes", default="net8-jit,chaos-aot",
                        help="Comma-separated runtime aliases, or 'all'")
    parser.add_argument("--iterations", type=int, default=100000, help="Iterations per method")
    parser.add_argument("--baseline", type=Path, default=None, help="Previous multi-run-report.json")
    parser.add_argument("--output", type=Path, default=None, help="Output directory (default: family_dir/multi-run)")

    args = parser.parse_args()

    # Load method subject IDs from contract
    contract_path = args.family_dir / "capability-family-contract.json"
    if contract_path.exists():
        contract = json.loads(contract_path.read_text(encoding="utf-8"))
        mids = contract.get("methodSubjectIds", [])
    else:
        mids = []

    runtimes = [x.strip() for x in args.runtimes.split(",") if x.strip()]

    report = run_multi_benchmark(
        family_dir=args.family_dir,
        family_slug=args.family,
        assembly=args.assembly,
        method_subject_ids=mids,
        runtimes=runtimes if runtimes != ["all"] else None,
        iterations=args.iterations,
        baseline_path=args.baseline,
    )

    output_dir = args.output or (args.family_dir / "multi-run")
    save_report(report, output_dir)
    print_report_summary(report)


if __name__ == "__main__":
    main()
