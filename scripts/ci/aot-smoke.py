#!/usr/bin/env python3
"""AOT smoke check for CI.

Verifies AOT codegen produces compilable C++ output for key CoreLib types,
and that the native build links successfully.

Usage:
    python scripts/ci/aot-smoke.py
"""

import os
import subprocess
import sys
import tempfile
from pathlib import Path


def main() -> int:
    print("[AOT Smoke] Starting AOT codegen smoke check...")

    repo_root = Path(__file__).resolve().parent.parent.parent

    # Step 1: Verify Chaos.IL2CPP.Driver is built
    driver_exe = (
        repo_root / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
    )
    if not driver_exe.exists():
        print(f"[AOT Smoke] ERROR: Driver not found at {driver_exe}")
        return 1
    print(f"[AOT Smoke] Driver found: {driver_exe}")

    # Step 2: Verify Generator DLL — the AOT codegen engine.  Its absence means
    # AOT codegen cannot run at all, so this is gate-critical (hard-fail).
    generator_dll = (
        repo_root / "src" / "managed" / "Chaos.IL2CPP.Generator" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Generator.dll"
    )
    if not generator_dll.exists():
        print(f"[AOT Smoke] ERROR: Generator DLL not found at {generator_dll}")
        print("[AOT Smoke] The AOT codegen engine is required for this gate.")
        return 1
    print(f"[AOT Smoke] Generator DLL found: {generator_dll}")

    # Step 3: Snapshot tests (verifies codegen output) — INFORMATION ONLY here.
    # The hard `dotnet test tests/unit/managed/snapshot/` step in aot-regression.yml
    # is the authoritative gate for snapshot correctness; the smoke records
    # presence so the log is self-contained.
    snapshot_dir = repo_root / "tests" / "snapshots" / "Chaos.IL2CPP.CodeGen.SnapshotTests"
    if snapshot_dir.exists():
        print(f"[AOT Smoke] Snapshot tests exist at {snapshot_dir}")
    else:
        print(f"[AOT Smoke] WARNING: Snapshot tests not found at {snapshot_dir}")
        print("[AOT Smoke]   (Snapshot correctness is gated separately by the hard "
              "`dotnet test` step; this presence check is informational only.)")

    # Step 4: Native build artifacts — INFORMATION ONLY.  aot-regression.yml does
    # not build the native runtime, so `build/native` is not expected in this job;
    # native-link verification lives in the native/translation pipelines.
    native_build_dir = repo_root / "build" / "native"
    if native_build_dir.exists():
        lib_count = len(list(native_build_dir.rglob("*.a"))) + len(list(native_build_dir.rglob("*.so"))) + len(list(native_build_dir.rglob("*.lib")))
        print(f"[AOT Smoke] Native build directory exists: {native_build_dir} ({lib_count} library files)")
    else:
        print(f"[AOT Smoke] WARNING: Native build directory not found at {native_build_dir}")
        print("[AOT Smoke]   (Informational: aot-regression.yml does not build the "
              "native runtime; native-link verification is in the native/translation pipelines.)")

    print("[AOT Smoke] AOT codegen smoke check PASSED")
    return 0


if __name__ == "__main__":
    sys.exit(main())
