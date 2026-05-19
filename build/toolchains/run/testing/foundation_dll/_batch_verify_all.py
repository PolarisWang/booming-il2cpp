"""Batch: regenerate codegen, rebuild entry, and fact-verify all families.

Usage:
    python _batch_verify_all.py
    python _batch_verify_all.py --families convert-char,enum-parsing
"""
import subprocess, sys, os, time, re
from pathlib import Path

REPO = Path(r"D:\agent\booming-il2cpp")
VERIFICATION = REPO / "verification" / "foundation-dll" / "System.Private.CoreLib"
DRIVER = REPO / "src" / "managed" / "Chaos.IL2CPP.Driver"

# Families to skip (no entry.exe expected or infrastructure-only)
SKIP_FAMILIES = {"reports", "System.Private.CoreLib"}

def get_families():
    families = sorted(d.name for d in VERIFICATION.iterdir() if d.is_dir() and d.name not in SKIP_FAMILIES)
    return families

def find_dll(family_slug):
    """Find the managed subjects DLL for a family."""
    candidates = list(VERIFICATION.glob(f"{family_slug}/managed/**/build-output/*.dll"))
    # Prefer the family-named DLL (not System.Private.CoreLib.dll itself)
    for c in candidates:
        if family_slug.replace("-", "").replace("_", "") in c.stem.replace("_", "").replace("-", "").lower():
            return c
    if candidates:
        return candidates[0]
    return None

def run_codegen(family_slug, dll_path):
    out_dir = VERIFICATION / family_slug / "codegen"
    out_dir.mkdir(parents=True, exist_ok=True)
    r = subprocess.run([
        "dotnet", "run", "--no-build",
        "--project", str(DRIVER),
        "--", "convert-to-cpp",
        "--assembly", str(dll_path),
        "--assembly-dir", str(dll_path.parent),
        "--output", str(out_dir),
    ], capture_output=True, text=True, timeout=300)
    if r.returncode != 0:
        print(f"    CODEGEN FAILED rc={r.returncode}")
        for line in r.stderr.splitlines()[-5:]:
            print(f"      {line}")
        return False
    return True

def inject_seh(cmakelists):
    if not cmakelists.exists():
        return False
    text = cmakelists.read_text(encoding="utf-8")
    if "CHAOS_IL2CPP_EH_WIN32_SEH" in text:
        return False  # already has it
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

    # Inject SEH define
    inject_seh(cmakelists)

    build_dir = native_dir / "build"
    build_dir.mkdir(parents=True, exist_ok=True)

    for attempt in range(1, retries + 1):
        # CMake configure
        cfg = subprocess.run(
            ["cmake", "-S", str(native_dir), "-B", str(build_dir),
             "-G", "Visual Studio 17 2022", "-A", "x64"],
            capture_output=True, text=True, timeout=120)
        if cfg.returncode != 0:
            err = cfg.stderr[-100:]
            if attempt < retries and ("locked" in err.lower() or "LNK" in err):
                print(f"      cmake configure locked, retry {attempt}/{retries}")
                time.sleep(5)
                continue
            return f"cmake-fail: {err}"

        # CMake build
        bld = subprocess.run(
            ["cmake", "--build", str(build_dir), "--config", "Release",
             "--target", "entry"],
            capture_output=True, text=True, timeout=300)
        if bld.returncode != 0:
            err = bld.stderr[-100:]
            if attempt < retries and ("locked" in err.lower() or "LNK" in err):
                print(f"      cmake build locked/contention, retry {attempt}/{retries}")
                time.sleep(5)
                continue
            return f"build-fail: {err}"

        # Success — break out of retry loop
        break

    # Copy entry.exe to native/
    src_exe = build_dir / "Release" / "entry.exe"
    if src_exe.exists():
        dst_exe = native_dir / "entry.exe"
        import shutil
        shutil.copy2(str(src_exe), str(dst_exe))
    return "ok"

def fact_verify(family_slug):
    from build.toolchains.run.testing.foundation_dll.fact_verifier import verify_fact
    r = verify_fact(family_slug, assembly="System.Private.CoreLib")
    return r

def main():
    families = get_families()
    print(f"Found {len(families)} families to process")

    results = {}
    for i, f in enumerate(families):
        print(f"\n[{i+1}/{len(families)}] {f} ...")

        # Find DLL
        dll = find_dll(f)
        if dll is None:
            print(f"    SKIP: no managed DLL found")
            results[f] = "no-dll"
            continue

        # Codegen
        print(f"    codegen...")
        if not run_codegen(f, dll):
            results[f] = "codegen-fail"
            continue

        # Build
        print(f"    build...")
        build_status = build_entry(f)
        if build_status != "ok":
            print(f"    BUILD: {build_status}")
            results[f] = build_status
            continue

        # Fact verify
        print(f"    verify...")
        vr = fact_verify(f)
        status = vr["status"]
        passed = vr.get("passed", "?")
        total = vr.get("total", "?")
        print(f"    => {status} ({passed}/{total})")
        results[f] = {"status": status, "passed": passed, "total": total, "exit_code": vr.get("exit_code")}

    print(f"\n{'='*60}")
    print(f"RESULTS: {len(families)} families")
    print(f"{'='*60}")
    passed = [f for f, r in results.items() if isinstance(r, dict) and r.get("status") == "passed"]
    failed = [f for f, r in results.items() if isinstance(r, dict) and r.get("status") == "failed"]
    errors = [f for f, r in results.items() if not isinstance(r, dict)]
    print(f"Passed: {len(passed)}")
    print(f"Failed: {len(failed)}")
    print(f"Errors: {len(errors)}")
    if failed:
        print(f"\nFailed families:")
        for f in failed:
            r = results[f]
            print(f"  {f}: {r['status']} ({r['passed']}/{r['total']}) exit_code={r.get('exit_code','?')}")
    if errors:
        print(f"\nError families: {errors}")

if __name__ == "__main__":
    main()
