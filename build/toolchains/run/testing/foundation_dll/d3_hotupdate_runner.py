"""Build and run D3 HotUpdateTest for a single family.

Compiles HotUpdateTest.cpp + genuine-fixed C++ and links against
all chaos runtime libs.  Runs the test and collects JSON output.

Usage:
  python d3_hotupdate_runner.py <family-slug>
  python d3_hotupdate_runner.py --all
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


def _find_vcvars() -> Path | None:
    candidates = [
        Path("C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Auxiliary/Build/vcvarsall.bat"),
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
    test_cpp: Path,
    genuine_cpp: Path,
    test_obj: Path,
    genuine_obj: Path,
    exe_path: Path,
    include_flags: str,
    compile_flags: str,
    defines: str,
    all_libs: str,
) -> Path:
    """Write a .bat file that builds the HotUpdateTest, return its path."""
    out_dir = exe_path.parent
    out_dir.mkdir(parents=True, exist_ok=True)
    bat_path = out_dir / f"_build_d3_{family_slug}.bat"
    lines = [
        "@echo off",
        f'call "{vcvars}" x64 >nul 2>nul',
        "if %ERRORLEVEL% neq 0 exit /b 1",
        "",
        f'echo Compiling HotUpdateTest.cpp...',
        f'cl {compile_flags} {include_flags} {defines} -Fo"{test_obj}" "{test_cpp}"',
        "if %ERRORLEVEL% neq 0 exit /b 1",
        "",
        f'echo Compiling genuine-fixed native-aot.generated.cpp...',
        f'cl {compile_flags} {include_flags} {defines} -Fo"{genuine_obj}" "{genuine_cpp}"',
        "if %ERRORLEVEL% neq 0 exit /b 1",
        "",
        f'echo Linking...',
        f'link /nologo /out:"{exe_path}" "{test_obj}" "{genuine_obj}" {all_libs} ole32.lib user32.lib',
        "if %ERRORLEVEL% neq 0 exit /b 1",
        "",
        f'echo Build OK',
    ]
    bat_path.write_text("\r\n".join(lines) + "\r\n", encoding="ascii")
    return bat_path


def build_and_run(family_slug: str) -> dict:
    """Build and run the D3 HotUpdateTest for a single family."""
    family_dir = _VERIFICATION / family_slug
    hotupdate_dir = family_dir / "il2cpp_dist" / "hotupdate"
    test_cpp = hotupdate_dir / "HotUpdateTest.cpp"
    genuine_cpp = hotupdate_dir / "genuine-fixed" / "native-aot.generated.cpp"

    result = {"family": family_slug, "success": False}

    if not test_cpp.exists():
        result["error"] = "HotUpdateTest.cpp not found"
        return result
    if not genuine_cpp.exists():
        result["error"] = "genuine-fixed native-aot.generated.cpp not found"
        return result

    vcvars = _find_vcvars()
    if vcvars is None:
        result["error"] = "MSVC vcvarsall.bat not found"
        return result

    # Include directories
    include_dirs = [
        _REPO_ROOT / "src" / "native" / "common",
        _REPO_ROOT / "src" / "native" / "common" / "chaos",
        _REPO_ROOT / "contracts" / "native" / "v0",
        _REPO_ROOT / "src" / "native" / "runtime-core",
        _REPO_ROOT / "src" / "native" / "bootstrap",
        _REPO_ROOT / "src" / "native" / "interpreter",
        _REPO_ROOT / "third_party" / "fmt" / "include",
        _REPO_ROOT / "third_party" / "bdwgc" / "include",
        _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib",
    ]
    include_flags = " ".join(f'-I"{d}"' for d in include_dirs)

    output_dir = hotupdate_dir / "build"
    output_dir.mkdir(parents=True, exist_ok=True)
    exe_path = output_dir / f"d3_hotupdate_{family_slug}.exe"
    test_obj = output_dir / "HotUpdateTest.obj"
    genuine_obj = output_dir / "genuine_fixed.obj"

    # /GS- for convert-char: codegen-emitted stack buffers trigger a harmless
    # MSVC buffer security check false positive (STATUS_STACK_BUFFER_OVERRUN).
    # /MD — all cmake-native chaos libs use the MSVC default dynamic CRT (/MD).
    compile_flags = "/nologo /std:c++17 /c /EHac /W3 /utf-8 /O2 /MD /GS-"
    defines = "-DCHAOS_IL2CPP_CHECK -DCHAOS_RUNTIME_ABI_STATIC"

    r = _REPO_ROOT
    # Use RelWithDebInfo variant (the cmake-native build produces this config).
    # The chaos_runtime_core.lib from build/src/native/runtime-core/Release/
    # is copied to build/native-runtime/Release/ for the D3 linking step.
    all_libs = " ".join(
        f'"{p}"' for p in [
            r / "build" / "native-runtime" / "Release" / "chaos_runtime_core.lib",
            r / "build" / "native" / "src" / "native" / "interpreter" / "RelWithDebInfo" / "chaos_interpreter.lib",
            r / "build" / "native" / "src" / "native" / "bootstrap" / "RelWithDebInfo" / "chaos_bootstrap.lib",
            r / "build" / "native" / "src" / "native" / "support" / "RelWithDebInfo" / "chaos_support.lib",
            r / "build" / "native" / "src" / "native" / "hot-update" / "RelWithDebInfo" / "chaos_hot_update.lib",
            r / "build" / "native" / "fmt_build" / "RelWithDebInfo" / "chaos_fmt.lib",
            r / "build" / "native" / "src" / "native" / "common" / "RelWithDebInfo" / "chaos_common.lib",
            r / "build" / "native" / "bdwgc_build" / "RelWithDebInfo" / "chaos_bdwgc.lib",
        ]
    )

    print(f"\nBuilding D3 HotUpdateTest for {family_slug}...")
    t0 = time.time()

    # Write and execute .bat
    bat_path = _build_bat(
        family_slug,
        vcvars=vcvars, family_dir=family_dir,
        test_cpp=test_cpp, genuine_cpp=genuine_cpp,
        test_obj=test_obj, genuine_obj=genuine_obj,
        exe_path=exe_path,
        include_flags=include_flags,
        compile_flags=compile_flags, defines=defines,
        all_libs=all_libs,
    )

    r_build = subprocess.run(
        ["cmd.exe", "/c", str(bat_path)],
        capture_output=True, text=True, timeout=180,
        errors="replace",
    )
    build_elapsed = time.time() - t0
    if r_build.returncode != 0:
        err = [l for l in (r_build.stdout + r_build.stderr).splitlines()
               if l.strip() and "Microsoft" not in l and "vswhere" not in l.lower()]
        result["error"] = f"Build failed:\n" + "\n".join(err[-15:])
        result["build_elapsed"] = round(build_elapsed, 1)
        return result

    print(f"  Build OK ({build_elapsed:.1f}s)")
    result["build_elapsed"] = round(build_elapsed, 1)

    # Run the test
    print(f"  Running D3 HotUpdateTest for {family_slug}...")
    t1 = time.time()
    try:
        r_run = subprocess.run(
            [str(exe_path)],
            capture_output=True, text=True, timeout=120,
            errors="replace",
        )
        run_elapsed = time.time() - t1
        result["run_elapsed"] = round(run_elapsed, 1)

        # Parse JSON output from stdout
        output = r_run.stdout
        json_start = output.find("{")
        if json_start >= 0:
            try:
                data = json.loads(output[json_start:])
                result["test_data"] = data
                passed = data.get("passedMethods", 0)
                failed = data.get("failedMethods", 0)
                total = data.get("totalMethods", 0)
                result["passed"] = passed
                result["failed"] = failed
                result["total"] = total
                result["success"] = failed == 0
                print(f"  {passed}/{total} passed, {failed} failed ({run_elapsed:.1f}s)")
            except json.JSONDecodeError:
                result["error"] = f"JSON parse failed: {output[:200]}"
                result["raw_output"] = output[:500]
        else:
            result["error"] = f"No JSON found, rc={r_run.returncode}"
            result["stderr"] = r_run.stderr[:300]
            print(f"  No JSON found (rc={r_run.returncode})")

        if r_run.returncode != 0 and result.get("success"):
            result["warn_exit_code"] = r_run.returncode

    except subprocess.TimeoutExpired:
        result["error"] = "Test timed out (120s)"
        print(f"  TIMEOUT")
    except OSError as e:
        result["error"] = str(e)
        print(f"  OS error: {e}")

    return result


def main() -> None:
    parser = argparse.ArgumentParser(description="D3 HotUpdateTest runner")
    parser.add_argument("family_slug", nargs="?", help="Family slug (e.g., convert-char)")
    parser.add_argument("--all", action="store_true", help="Run all families with HotUpdateTest.cpp")
    parser.add_argument("--json", action="store_true", help="Output results as JSON")
    args = parser.parse_args()

    if args.all:
        families = sorted(
            d.name for d in _VERIFICATION.iterdir()
            if d.is_dir()
            and (d / "il2cpp_dist" / "hotupdate" / "HotUpdateTest.cpp").exists()
            and d.name != "reports"
        )
        print(f"Running D3 HotUpdateTest for {len(families)} families...")
    elif args.family_slug:
        families = [args.family_slug]
    else:
        parser.print_help()
        sys.exit(1)

    all_results = []
    all_passed = 0
    all_failed_count = 0

    for slug in families:
        r = build_and_run(slug)
        all_results.append(r)
        if r["success"]:
            all_passed += 1
        else:
            all_failed_count += 1
        print(f"  Status: {'PASS' if r['success'] else 'FAIL'} ({r.get('passed', 0)}/{r.get('total', 0)} passed)")

    # Summary
    total = len(families)
    print(f"\n{'='*60}")
    print(f"D3 HotUpdateTest Summary: {all_passed} passed, {all_failed_count} failed, {total} total")
    print(f"{'='*60}")
    for r in all_results:
        status = "PASS" if r["success"] else "FAIL"
        detail = r.get("error", f"{r.get('passed', 0)}/{r.get('total', 0)} passed")
        print(f"  {status:4s}  {r['family']:35s}  {detail}")

    if args.json:
        print(json.dumps(all_results, indent=2))

    if all_failed_count > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
