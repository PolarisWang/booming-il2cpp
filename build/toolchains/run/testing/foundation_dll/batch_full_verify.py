"""Batch: regenerate codegen, rebuild entry, and fact-verify all families.
Handles GBK encoding on Windows, module path issues, and SEH injection.

Usage:
    python batch_full_verify.py                    # full rebuild + verify all
    python batch_full_verify.py --quick            # just run existing entry.exe
    python batch_full_verify.py --families a,b     # specific families only
    python batch_full_verify.py --quick --families a,b  # quick scan specific
    python batch_full_verify.py reflection-type    # resume from family
"""
import subprocess, sys, os, time, json
from pathlib import Path

REPO = Path(r"D:\agent\booming-il2cpp")
VERIFICATION = REPO / "testing" / "foundation-dll" / "System.Private.CoreLib"
DRIVER = REPO / "src" / "managed" / "Chaos.IL2CPP.Driver"
SKIP_FAMILIES = {"reports", "System.Private.CoreLib"}

def get_families():
    return sorted(d.name for d in VERIFICATION.iterdir() if d.is_dir() and d.name not in SKIP_FAMILIES)

def find_dll(family_slug):
    candidates = list(VERIFICATION.glob(f"{family_slug}/managed/**/build-output/*.dll"))
    for c in candidates:
        if family_slug.replace("-", "").replace("_", "") in c.stem.replace("_", "").replace("-", "").lower():
            return c
    return candidates[0] if candidates else None

def run_codegen(family_slug, dll_path):
    print(f"    codegen...", flush=True)
    out_dir = VERIFICATION / family_slug / "codegen"
    out_dir.mkdir(parents=True, exist_ok=True)
    try:
        r = subprocess.run([
            "dotnet", "run", "--no-build",
            "--project", str(DRIVER),
            "--", "convert-to-cpp",
            "--assembly", str(dll_path),
            "--assembly-dir", str(dll_path.parent),
            "--output", str(out_dir),
        ], capture_output=True, text=True, errors='replace', timeout=300)
        if r.returncode != 0:
            print(f"    CODEGEN FAILED rc={r.returncode}")
            for line in r.stderr.splitlines()[-5:]:
                print(f"      {line}")
            return False
        return True
    except subprocess.TimeoutExpired:
        print(f"    CODEGEN TIMEOUT (>300s)")
        return False
    except Exception as e:
        print(f"    CODEGEN EXCEPTION: {e}")
        return False

def inject_seh(cmakelists):
    if not cmakelists.exists():
        return False
    text = cmakelists.read_text(encoding="utf-8")
    if "CHAOS_IL2CPP_EH_WIN32_SEH" in text:
        return False
    marker = "# chaos-il2cpp SEH (Windows)"
    line = f"add_compile_definitions(CHAOS_IL2CPP_EH_WIN32_SEH)  {marker}"
    text = text.replace(
        "add_compile_options(/utf-8 /GS-)",
        f"add_compile_options(/utf-8 /GS-)\n{line}",
    )
    cmakelists.write_text(text, encoding="utf-8")
    return True

def build_entry(family_slug, retries=3):
    native_dir = VERIFICATION / family_slug / "native"
    cmakelists = native_dir / "CMakeLists.txt"
    if not cmakelists.exists():
        return "no-cmakelists"

    inject_seh(cmakelists)

    build_dir = native_dir / "build"

    # Remove stale build directory to force fresh cmake configure
    # (file(GLOB) results are cached across cmake runs, so new codegen
    #  files won't be picked up without a clean configure.)
    import shutil
    if build_dir.exists():
        shutil.rmtree(build_dir, ignore_errors=True)

    build_dir.mkdir(parents=True, exist_ok=True)

    for attempt in range(1, retries + 1):
        try:
            cfg = subprocess.run(
                ["cmake", "-S", str(native_dir), "-B", str(build_dir),
                 "-G", "Visual Studio 17 2022", "-A", "x64",
                 "-DCHAOS_NATIVE_BUILD=" + str(REPO / "build" / "native")],
                capture_output=True, text=True, errors='replace', timeout=120)
            if cfg.returncode != 0:
                err = cfg.stderr[-200:] if cfg.stderr else "unknown"
                if attempt < retries and "locked" in err.lower():
                    print(f"      cmake configure locked, retry {attempt}/{retries}")
                    time.sleep(5)
                    continue
                return f"cmake-fail: {err}"
        except Exception as e:
            if attempt < retries:
                print(f"      cmake configure exception, retry {attempt}/{retries}: {e}")
                time.sleep(5)
                continue
            return f"cmake-exception: {e}"

        try:
            bld = subprocess.run(
                ["cmake", "--build", str(build_dir), "--config", "RelWithDebInfo",
                 "--target", "entry"],
                capture_output=True, text=True, errors='replace', timeout=300)
            if bld.returncode != 0:
                err = bld.stderr[-200:] if bld.stderr else bld.stdout[-200:]
                if attempt < retries and ("locked" in err.lower() or "LNK" in err):
                    print(f"      cmake build locked/contention, retry {attempt}/{retries}")
                    time.sleep(5)
                    continue
                return f"build-fail: {err}"
        except Exception as e:
            if attempt < retries:
                print(f"      cmake build exception, retry {attempt}/{retries}: {e}")
                time.sleep(5)
                continue
            return f"build-exception: {e}"

        # Success — break out of retry loop
        break

    src_exe = build_dir / "RelWithDebInfo" / "entry.exe"
    if not src_exe.exists():
        src_exe = build_dir / "Release" / "entry.exe"
    if src_exe.exists():
        dst_exe = native_dir / "entry.exe"
        import shutil
        shutil.copy2(str(src_exe), str(dst_exe))
    return "ok"

def fact_verify(family_slug):
    """Direct fact verification by running entry.exe."""
    entry = VERIFICATION / family_slug / "native" / "entry.exe"
    if not entry.exists():
        return {"status": "no-entry", "passed": 0, "total": 0, "exit_code": -1}

    env = os.environ.copy()
    try:
        r = subprocess.run([str(entry)], capture_output=True, text=True, errors='replace', timeout=30, env=env)
    except subprocess.TimeoutExpired:
        return {"status": "timeout", "passed": 0, "total": 0, "exit_code": -1}

    stdout = r.stdout if hasattr(r, 'stdout') else ""
    rc = r.returncode if hasattr(r, 'returncode') else -1

    import re
    m = re.search(r"Passed:\s*(\d+)/(\d+)", stdout)
    passed = int(m.group(1)) if m else 0
    total = int(m.group(2)) if m else 0

    status = "passed" if rc == 0 else "failed"
    return {"status": status, "passed": passed, "total": total, "exit_code": rc}

def main():
    import argparse
    parser = argparse.ArgumentParser(description="Batch verify all foundation-dll families")
    parser.add_argument("--quick", action="store_true", help="Skip codegen + build, only run existing entry.exe")
    parser.add_argument("--families", help="Comma-separated list of families to process (default: all)")
    parser.add_argument("start_from", nargs="?", help="Resume from this family (positional, ignored if --families set)")
    args = parser.parse_args()

    all_families = get_families()

    if args.families:
        requested = [f.strip() for f in args.families.split(",")]
        families = [f for f in all_families if f in requested]
        missing = set(requested) - set(families)
        if missing:
            print(f"Unknown families: {missing}")
    elif args.start_from:
        try:
            idx = all_families.index(args.start_from)
            families = all_families[idx:]
            print(f"Resuming from [{idx+1}/{len(all_families)}] {args.start_from}")
        except ValueError:
            print(f"Family '{args.start_from}' not found, starting from beginning")
            families = all_families
    else:
        families = all_families

    mode = "QUICK (entry.exe only)" if args.quick else "FULL (codegen + build + verify)"
    print(f"Batch verify — {mode}")
    print(f"Families: {len(families)}")

    results = {}
    for i, f in enumerate(families):
        print(f"\n[{i+1}/{len(families)}] {f} ...", flush=True)

        if args.quick:
            # Quick mode: just run entry.exe directly
            print(f"    quick scan...", flush=True)
            try:
                vr = fact_verify(f)
                status = vr["status"]
                passed = vr["passed"]
                total = vr["total"]
                print(f"    => {status} ({passed}/{total})", flush=True)
                results[f] = {"status": status, "passed": passed, "total": total, "exit_code": vr["exit_code"]}
            except Exception as e:
                print(f"    QUICK exception: {e}", flush=True)
                results[f] = f"quick-exception: {e}"
            continue

        # Full mode: find DLL → codegen → build → verify
        try:
            dll = find_dll(f)
            if dll is None:
                print(f"    SKIP: no managed DLL found", flush=True)
                results[f] = "no-dll"
                continue
        except Exception as e:
            print(f"    SKIP: find_dll error: {e}", flush=True)
            results[f] = f"find-dll-error: {e}"
            continue

        try:
            if not run_codegen(f, dll):
                results[f] = "codegen-fail"
                continue
        except Exception as e:
            print(f"    CODEGEN exception: {e}", flush=True)
            results[f] = f"codegen-exception: {e}"
            continue

        try:
            build_status = build_entry(f)
            if build_status != "ok":
                print(f"    BUILD: {build_status}", flush=True)
                results[f] = build_status
                continue
        except Exception as e:
            print(f"    BUILD exception: {e}", flush=True)
            results[f] = f"build-exception: {e}"
            continue

        print(f"    verify...", flush=True)
        try:
            vr = fact_verify(f)
            status = vr["status"]
            passed = vr["passed"]
            total = vr["total"]
            print(f"    => {status} ({passed}/{total})", flush=True)
            results[f] = {"status": status, "passed": passed, "total": total, "exit_code": vr["exit_code"]}
        except Exception as e:
            print(f"    VERIFY exception: {e}", flush=True)
            results[f] = f"verify-exception: {e}"

    # Summary
    print(f"\n{'='*60}")
    print(f"RESULTS: {len(families)} families")
    print(f"{'='*60}")
    passed_fams = [f for f, r in results.items() if isinstance(r, dict) and r.get("status") == "passed"]
    failed_fams = [f for f, r in results.items() if isinstance(r, dict) and r.get("status") == "failed"]
    errors = [f for f, r in results.items() if not isinstance(r, dict)]

    print(f"Passed: {len(passed_fams)}")
    print(f"Failed: {len(failed_fams)}")
    print(f"Errors: {len(errors)}")

    if failed_fams:
        print(f"\nFailed families:")
        for f in failed_fams:
            r = results[f]
            print(f"  {f}: {r['passed']}/{r['total']} exit={r['exit_code']}")
    if errors:
        print(f"\nError families:")
        for f in errors:
            print(f"  {f}: {results[f]}")

    # Save results JSON
    out = REPO / "batch_verify_results.json"
    with open(out, "w") as fh:
        json.dump(results, fh, indent=2)
    print(f"\nResults saved to {out}")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nInterrupted by user")
    except Exception as e:
        print(f"\nFATAL ERROR: {e}")
        import traceback
        traceback.print_exc()
