#!/usr/bin/env python3
"""Batch rebuild all foundation-dll chunks that had CMakeLists.txt fixes."""
import subprocess, sys, os, glob
from pathlib import Path

FD = Path("/home/debian/agent/booming-il2cpp/testing/foundation-dll")
os.chdir(str(FD))

# Find all chunks with native/CMakeLists.txt
chunks = sorted(Path(f).parent.parent for f in glob.glob("*/chunks/*/native/CMakeLists.txt"))

results = {"ok": 0, "fail": 0, "skipped": 0}

for cmake_dir in chunks:
    native_dir = cmake_dir / "native"
    build_dir = native_dir / "build"

    # Check if this chunk already has a working entry.exe
    entry_exe = native_dir / "entry.exe"
    if entry_exe.exists():
        # Quick smoke test
        try:
            r = subprocess.run([str(entry_exe), "--fact-json"], capture_output=True, timeout=30, cwd=str(native_dir))
            if r.returncode == 0:
                print(f"[SKIP] {cmake_dir.parent.name}/{cmake_dir.name} — already OK")
                results["skipped"] += 1
                continue
        except:
            pass

    print(f"[BUILD] {cmake_dir.parent.name}/{cmake_dir.name}")

    # Create build dir
    build_dir.mkdir(parents=True, exist_ok=True)

    # Cmake configure
    r = subprocess.run(["cmake", ".."], capture_output=True, timeout=120, cwd=str(build_dir))
    if r.returncode != 0:
        print(f"  CMAKE FAIL: {r.stderr.decode()[-500:]}")
        results["fail"] += 1
        continue

    # Make
    r = subprocess.run(["make", "-j{}".format(os.cpu_count())], capture_output=True, timeout=300, cwd=str(build_dir))
    if r.returncode != 0:
        print(f"  BUILD FAIL: {r.stderr.decode()[-500:]}")
        results["fail"] += 1
        continue

    # Copy binary (use abs paths to avoid cwd confusion)
    subprocess.run(["cp", str(FD / build_dir / "chaos_entry"), str(FD / native_dir / "entry.exe")])

    # Fact test
    try:
        r = subprocess.run([str(native_dir / "entry.exe"), "--fact-json"], capture_output=True, timeout=60, cwd=str(native_dir))
        if r.returncode == 0:
            print(f"  FACT OK (exit=0)")
            results["ok"] += 1
        else:
            print(f"  FACT FAIL (exit={r.returncode})")
            results["fail"] += 1
    except Exception as e:
        print(f"  FACT CRASH: {e}")
        results["fail"] += 1

print(f"\n=== Results: {results['ok']} OK, {results['fail']} FAIL, {results['skipped']} SKIPPED ===")
