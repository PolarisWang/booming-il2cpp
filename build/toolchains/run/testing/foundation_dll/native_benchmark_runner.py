"""Compile and run the native AOT benchmark for a single family.

Build approach: generates a .bat file that calls vcvarsall.bat then
cl.exe/link.exe.  This avoids two problems with the env-based approach:
  1. Git Bash corrupts >nul redirects in shell=True commands.
  2. Captured env vars (LIB/INCLUDE/PATH) don't resolve MSVC CRT and
     Windows SDK symbols (__std_find_end_1, __imp_getenv, etc.)
     discovered through the L2 verify builds.

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

# Ensure testing.trace is importable
_RUN_DIR = _REPO_ROOT / "build" / "toolchains" / "run"
if str(_RUN_DIR) not in sys.path:
    sys.path.insert(0, str(_RUN_DIR))

from testing.trace import trace_init, trace


def _find_vcvars() -> Path | None:
    """Locate vcvarsall.bat — prefers BuildTools (MSVC 14.44+) which has
    CRT symbols (_Thrd_sleep_for, _Cnd_timedwait_for_unchecked, __std_find_end_1)
    needed by chaos_runtime_core.lib compiled with 14.44."""
    candidates = [
        # BuildTools (MSVC 14.44+, has vectorized algorithm intrinsics)
        Path("C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Auxiliary/Build/vcvarsall.bat"),
        # Professional / Community
        Path("C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat"),
        Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Auxiliary/Build/vcvarsall.bat"),
        Path("C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Auxiliary/Build/vcvarsall.bat"),
    ]
    for c in candidates:
        if c.exists():
            return c
    return None


def _build_bat(
    family_slug: str,
    *,
    vcvars: Path,
    family_dir: Path,
    benchmark_host: Path,
    generated_cpp: Path | None,
    main_obj: Path,
    gen_obj: Path,
    exe_path: Path,
    include_flags: str,
    compile_flags: str,
    defines: str,
    all_libs: str,
) -> Path:
    """Write a .bat file that builds the benchmark, return its path."""
    bat_path = (family_dir / "native_test" / "benchmark" / "build"
                / f"_build_{family_slug}.bat")
    lines = [
        "@echo off",
        f'call "{vcvars}" x64 >nul 2>nul',
        "if %ERRORLEVEL% neq 0 exit /b 1",
        "",
        f'echo Compiling benchmark host...',
        f'cl {compile_flags} {include_flags} {defines} -Fo"{main_obj}" "{benchmark_host}"',
        "if %ERRORLEVEL% neq 0 exit /b 1",
    ]
    if generated_cpp:
        lines += [
            "",
            f'echo Compiling generated C++ ({generated_cpp.name})...',
            f'cl {compile_flags} {include_flags} {defines} -Fo"{gen_obj}" "{generated_cpp}"',
            "if %ERRORLEVEL% neq 0 exit /b 1",
        ]
    lines += [
        "",
        f'echo Linking...',
        f'link /nologo /nodefaultlib:ucrt /out:"{exe_path}" "{main_obj}" "{gen_obj}" {all_libs} "%WindowsSdkDir%lib\\%UCRTVersion%\\ucrt\\x64\\ucrt.lib" ole32.lib user32.lib',
        "if %ERRORLEVEL% neq 0 exit /b 1",
        "",
        f'echo Build OK',
    ]
    bat_path.write_text("\r\n".join(lines) + "\r\n", encoding="ascii")
    return bat_path


def build_native_benchmark(
    family_slug: str,
    *,
    output_dir: Path | None = None,
) -> dict:
    """Build the native benchmark executable for a family.

    Uses the L2 verify's pre-compiled native-aot.generated.obj (which
    avoids a SIGSEGV when compiling the generated C++ via the env-based
    MSVC approach) and links against all native runtime libraries.
    """
    family_dir = _VERIFICATION / family_slug
    generated_cpp = family_dir / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
    benchmark_host = _REPO_ROOT / "src" / "native" / "benchmark-host" / "native_aot_main.cpp"

    if not generated_cpp.exists():
        return {"success": False, "error": f"Generated C++ not found: {generated_cpp}"}

    include_dirs = [
        _REPO_ROOT / "src" / "native" / "common",
        _REPO_ROOT / "src" / "native" / "common" / "chaos",
        _REPO_ROOT / "contracts" / "native" / "v0",
        _REPO_ROOT / "src" / "native" / "runtime-core",
        _REPO_ROOT / "third_party" / "fmt" / "include",
    ]
    include_flags = " ".join(f'-I"{d}"' for d in include_dirs)

    output_dir = output_dir or (family_dir / "native_test" / "benchmark" / "build")
    output_dir.mkdir(parents=True, exist_ok=True)
    exe_path = output_dir / f"benchmark_{family_slug}.exe"
    main_obj = output_dir / "native_aot_main.obj"

    vcvars = _find_vcvars()
    if vcvars is None:
        return {"success": False, "error": "MSVC vcvarsall.bat not found"}

    # Compile from source via .bat (which runs vcvarsall.bat first, avoiding
    # both the SIGSEGV from the env-based MSVC approach and the linker symbol
    # resolution issues when mixing verify's .obj with our compile flags).
    generated_cpp_path = family_dir / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
    if not generated_cpp_path.exists():
        return {"success": False, "error": f"Generated C++ not found: {generated_cpp_path}"}
    gen_obj = output_dir / "native-aot.generated.obj"

    compile_flags = "/nologo /std:c++20 /c /EHsc /W3 /utf-8 /O2 /MD"
    defines = "-DCHAOS_IL2CPP_CHECK"

    # All native runtime libs that executable code in the generated .obj
    # or in chaos_runtime_core.lib may reference.
    r = _REPO_ROOT
    all_libs = " ".join(
        f'"{p}"' for p in [
            r / "build" / "native-runtime" / "Release" / "chaos_runtime_core.lib",
            r / "build" / "native" / "src" / "native" / "interpreter" / "Release" / "chaos_interpreter.lib",
            r / "build" / "native" / "src" / "native" / "bootstrap" / "Release" / "chaos_bootstrap.lib",
            r / "build" / "native" / "src" / "native" / "support" / "Release" / "chaos_support.lib",
            r / "build" / "native" / "src" / "native" / "hot-update" / "Release" / "chaos_hot_update.lib",
            r / "build" / "native" / "fmt_build" / "Release" / "chaos_fmt.lib",
            r / "build" / "native" / "src" / "native" / "common" / "Release" / "chaos_common.lib",
            r / "build" / "native" / "bdwgc_build" / "Release" / "chaos_bdwgc.lib",
        ]
    )

    print(f"Building native benchmark for {family_slug}...")
    t0 = time.time()

    bat_path = _build_bat(
        family_slug,
        vcvars=vcvars, family_dir=family_dir,
        benchmark_host=benchmark_host, generated_cpp=generated_cpp_path,
        main_obj=main_obj, gen_obj=gen_obj, exe_path=exe_path,
        include_flags=include_flags,
        compile_flags=compile_flags, defines=defines,
        all_libs=all_libs,
    )

    r = subprocess.run(
        ["cmd.exe", "/c", str(bat_path)],
        capture_output=True, text=True, timeout=120,
    )
    elapsed = time.time() - t0
    if r.returncode != 0:
        err = [l for l in (r.stdout + r.stderr).splitlines()
               if l.strip() and "Microsoft" not in l and "vswhere" not in l.lower()]
        return {
            "success": False,
            "error": f"Build failed:\n" + "\n".join(err[-10:]),
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

    method_count = _load_method_count(args.family_slug)
    if method_count == 0:
        print(f"No method subject IDs found for {args.family_slug}")
        sys.exit(1)
    print(f"Family: {args.family_slug} ({method_count} methods)")

    method_indices = None
    if args.methods:
        method_indices = [int(x.strip()) for x in args.methods.split(",")]
        method_indices = [i for i in method_indices if 0 <= i < method_count]
        print(f"Running methods: {method_indices}")

    build_result = build_native_benchmark(args.family_slug)
    if not build_result["success"]:
        print(f"\nBuild FAILED:")
        print(build_result.get("error", "unknown error"))
        sys.exit(1)

    run_results = run_benchmark(
        build_result["exe_path"],
        method_count if not method_indices else max(method_indices) + 1,
        iterations=args.iterations,
    )

    if method_indices:
        run_results = [r for r in run_results if r.get("methodIndex") in method_indices]

    passes = sum(1 for r in run_results if "elapsedMilliseconds" in r)
    fails = sum(1 for r in run_results if "error" in r)
    trace("benchmark_done", family=args.family_slug, passed=passes, failed=fails, total=len(run_results))
    print(f"\n{'='*50}")
    print(f"Results: {passes} passed, {fails} failed, {len(run_results)} total")

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
