"""Compile and run the native AOT benchmark for a single family.

Compiles the generated AOT C++ code together with the benchmark host
(native_aot_main.cpp) and runtime stubs, then runs timing measurements
for each entry point method.

Usage:
  python native_benchmark_runner.py <family-slug> [--iterations N] [--methods IDX,IDX]
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION = _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib"

from testing.trace import trace_init, trace


def _find_msvc_env() -> dict[str, str]:
    """Find MSVC environment via vcvarsall.bat."""
    candidates = [
        Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat"),
        Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/Build/vcvarsall.bat"),
        Path("C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Auxiliary/Build/vcvarsall.bat"),
    ]
    vcvars = None
    for c in candidates:
        if c.exists():
            vcvars = c
            break
    if not vcvars:
        return {}

    try:
        result = subprocess.run(
            f'"{vcvars}" x64 && set',
            shell=True, capture_output=True, text=True, timeout=30,
        )
        env = {}
        for line in result.stdout.splitlines():
            if "=" in line:
                k, v = line.split("=", 1)
                env[k.upper()] = v
        return env
    except (subprocess.TimeoutExpired, OSError):
        return {}


def _find_latest_msvc_cl() -> Path | None:
    base = Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC")
    if not base.exists():
        base = Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC")
    if not base.exists():
        return None
    versions = sorted([d for d in base.iterdir() if d.is_dir() and d.name[0].isdigit()])
    if not versions:
        return None
    cl_path = versions[-1] / "bin" / "Hostx64" / "x64" / "cl.exe"
    return cl_path if cl_path.exists() else None


def _find_link_exe() -> Path | None:
    cl = _find_latest_msvc_cl()
    if cl:
        link = cl.parent / "link.exe"
        return link if link.exists() else None
    return None


def build_native_benchmark(
    family_slug: str,
    *,
    output_dir: Path | None = None,
) -> dict:
    """Build the native benchmark executable for a family.

    Compiles: native_aot_main.cpp + native-aot.generated.cpp + runtime_stubs.cpp
    """
    family_dir = _VERIFICATION / family_slug
    generated_cpp = family_dir / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
    runtime_stubs = _REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_stubs.cpp"
    benchmark_host = _REPO_ROOT / "src" / "native" / "benchmark-host" / "native_aot_main.cpp"

    if not generated_cpp.exists():
        return {"success": False, "error": f"Generated C++ not found: {generated_cpp}"}

    include_dirs = [
        _REPO_ROOT / "src" / "native" / "common",
        _REPO_ROOT / "src" / "native" / "common" / "chaos",
        _REPO_ROOT / "src" / "native" / "runtime-core",
        _REPO_ROOT / "contracts" / "native" / "v0",
        _REPO_ROOT / "third_party" / "fmt" / "include",
    ]
    include_flags = " ".join(f'-I"{d}"' for d in include_dirs)

    output_dir = output_dir or (family_dir / "native_test" / "benchmark" / "build")
    output_dir.mkdir(parents=True, exist_ok=True)
    exe_path = output_dir / f"benchmark_{family_slug}.exe"

    cl_exe = _find_latest_msvc_cl()
    msvc_env = _find_msvc_env()
    if not cl_exe or not msvc_env:
        return {"success": False, "error": "MSVC not found"}

    source_files = [benchmark_host, generated_cpp, runtime_stubs]
    obj_files = []

    compile_flags = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2"
    defines = "-DCHAOS_IL2CPP_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED"

    print(f"Compiling native benchmark for {family_slug} ({len(source_files)} source files)...")
    t0 = time.time()

    # Step 1: Compile each source file to object
    for src in source_files:
        obj = output_dir / f"{src.stem}.obj"
        obj_files.append(obj)
        cmd = (
            f'"{cl_exe}" {compile_flags} {include_flags} {defines} '
            f'-Fo"{obj}" '
            f'"{src}"'
        )
        r = subprocess.run(
            cmd, shell=True, capture_output=True, text=True, timeout=120,
            env={**os.environ, **msvc_env},
        )
        if r.returncode != 0:
            err = [l for l in (r.stdout + r.stderr).splitlines() if l.strip() and "Microsoft" not in l]
            return {
                "success": False,
                "error": f"Compile failed for {src.name}:\n" + "\n".join(err[-10:]),
                "elapsed": time.time() - t0,
            }

    # Step 2: Link objects into executable
    link_exe = _find_link_exe()
    if not link_exe:
        return {"success": False, "error": "MSVC linker not found"}

    obj_list = " ".join(f'"{o}"' for o in obj_files)
    link_cmd = f'"{link_exe}" /nologo /out:"{exe_path}" {obj_list}'
    r = subprocess.run(
        link_cmd, shell=True, capture_output=True, text=True, timeout=60,
        env={**os.environ, **msvc_env},
    )
    elapsed = time.time() - t0
    if r.returncode != 0:
        err = [l for l in r.stderr.splitlines() if l.strip() and "Microsoft" not in l]
        return {
            "success": False,
            "error": f"Link failed:\n" + "\n".join(err[-10:]),
            "elapsed": elapsed,
        }
    print(f"  Build OK ({elapsed:.1f}s)")
    return {"success": True, "exe_path": str(exe_path), "elapsed": elapsed}


def run_benchmark(exe_path: str, method_count: int, iterations: int = 10000) -> list[dict]:
    """Run the benchmark executable for each method index."""
    results = []
    print(f"\nRunning native benchmark ({iterations} iterations per method)...")
    for idx in range(method_count):
        cmd = [exe_path, "--iterations", str(iterations), "--entry-index", str(idx)]
        t0 = time.time()
        try:
            r = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
            elapsed = time.time() - t0
            if r.returncode == 0:
                # Parse JSON output
                try:
                    data = json.loads(r.stdout.strip())
                    data["methodIndex"] = idx
                    data["totalWallClock"] = round(elapsed, 3)
                    results.append(data)
                    print(f"  [{idx}] {data.get('opsPerSecond', 0):.0f} ops/s  ({data.get('elapsedMilliseconds', 0):.2f} ms)")
                except json.JSONDecodeError:
                    results.append({"methodIndex": idx, "error": "JSON parse failed", "raw": r.stdout[:200]})
                    print(f"  [{idx}] JSON parse error")
            else:
                results.append({"methodIndex": idx, "error": f"exit code {r.returncode}", "stderr": r.stderr[:200]})
                print(f"  [{idx}] FAILED (rc={r.returncode})")
        except subprocess.TimeoutExpired:
            results.append({"methodIndex": idx, "error": "timeout"})
            print(f"  [{idx}] TIMEOUT")
    return results


def _load_method_count(family_slug: str) -> int:
    """Load method count from capability family contract."""
    contract_path = _VERIFICATION / family_slug / "capability-family-contract.json"
    if not contract_path.exists():
        return 0
    with open(contract_path, encoding="utf-8") as f:
        contract = json.load(f)
    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]
    return len(mids)


def main() -> None:
    parser = argparse.ArgumentParser(description="Native benchmark runner")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--iterations", type=int, default=10000, help="Iterations per method")
    parser.add_argument("--methods", help="Comma-separated method indices to run (default: all)")
    parser.add_argument("--no-trace", action="store_true", help="Disable JSONL trace logging")
    args = parser.parse_args()

    trace_init(_REPO_ROOT, stage="native-benchmark")
    trace("benchmark_start", family=args.family_slug, iterations=args.iterations)

    # Load method count
    method_count = _load_method_count(args.family_slug)
    if method_count == 0:
        print(f"No method subject IDs found for {args.family_slug}")
        sys.exit(1)
    print(f"Family: {args.family_slug} ({method_count} methods)")

    # Parse method filter
    method_indices = None
    if args.methods:
        method_indices = [int(x.strip()) for x in args.methods.split(",")]
        method_indices = [i for i in method_indices if 0 <= i < method_count]
        print(f"Running methods: {method_indices}")

    # Build
    build_result = build_native_benchmark(args.family_slug)
    if not build_result["success"]:
        print(f"\nBuild FAILED:")
        print(build_result.get("error", "unknown error"))
        sys.exit(1)

    # Run
    run_results = run_benchmark(
        build_result["exe_path"],
        method_count if not method_indices else max(method_indices) + 1,
        iterations=args.iterations,
    )

    # Filter results if specific methods requested
    if method_indices:
        run_results = [r for r in run_results if r.get("methodIndex") in method_indices]

    # Summary
    passes = sum(1 for r in run_results if "elapsedMilliseconds" in r)
    fails = sum(1 for r in run_results if "error" in r)
    trace("benchmark_done", family=args.family_slug, passed=passes, failed=fails, total=len(run_results))
    print(f"\n{'='*50}")
    print(f"Results: {passes} passed, {fails} failed, {len(run_results)} total")

    # Print JSON summary
    output = {
        "family": args.family_slug,
        "iterations": args.iterations,
        "results": run_results,
    }
    print(f"\nJSON summary:")
    print(json.dumps(output, indent=2))

    if fails > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
