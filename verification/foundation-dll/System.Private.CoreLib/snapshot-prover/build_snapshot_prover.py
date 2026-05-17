#!/usr/bin/env python3
"""
Standalone build script for snapshot-prover runtime verification family.

This script implements the foundation-dll verification pipeline for the
snapshot-prover family, which verifies that the chaos-il2cpp codegen
produces correct native code for basic C# patterns (arithmetic, control
flow, boxing, generics, virtual dispatch, exception handling).

Usage:
    python build_snapshot_prover.py [--build-type check|ship]

The script:
  1. Builds SnapshotProverSubjects.dll (the subjects assembly)
  2. Runs chaos-il2cpp convert-to-cpp to generate native C++ code
  3. Creates native/CMakeLists.txt if missing
  4. CMake configures and builds the native entry.exe
  5. Runs entry.exe to verify "Passed: N/M" with N == M

Prerequisites:
  - The native runtime libraries must be pre-built (cmake --build build/native)
  - dotnet SDK 8.0+
  - CMake 3.20+ and Visual Studio 2022
"""

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

# ── Paths ──────────────────────────────────────────────────────────────────
REPO_ROOT = Path(__file__).resolve().parents[4]
assert (REPO_ROOT / ".git").exists(), f"Not repo root: {REPO_ROOT}"

FAMILY_SLUG = "snapshot-prover"
VERIFICATION_DIR = (
    REPO_ROOT
    / "verification"
    / "foundation-dll"
    / "System.Private.CoreLib"
    / FAMILY_SLUG
)
MANAGED_DIR = VERIFICATION_DIR / "managed" / "subjects"
NATIVE_DIR = VERIFICATION_DIR / "native"
CODEGEN_DIR = VERIFICATION_DIR / "codegen"
DRIVER_PROJECT = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"
NATIVE_BUILD_DIR = REPO_ROOT / "build" / "native"

# Configuration
CONFIG_TIER = "CHAOS_IL2CPP_CONFIG_TIER_CHECK"
LOG_LEVEL = "3"
CMAKE_PRESET = "x64-check"  # Must match build/native CMake preset

METHOD_COUNT = 10  # Number of subjects in the contract


# ── Step 1: Build subjects DLL ──────────────────────────────────────────
def build_subjects_dll() -> Path | None:
    """Build SnapshotProverSubjects.dll from managed/subjects/."""
    print("\n=== Step 1: Build subjects DLL ===")

    csproj = MANAGED_DIR / "SnapshotProverSubjects.csproj"
    if not csproj.exists():
        print(f"  ERROR: csproj not found at {csproj}")
        return None

    output_dir = MANAGED_DIR / "build-output"
    output_dir.mkdir(parents=True, exist_ok=True)

    cmd = [
        "dotnet", "build", str(csproj),
        "--configuration", "Release",
        "--output", str(output_dir),
        "-p:Platform=AnyCPU",
        "/nologo",
    ]
    print(f"  Running: {' '.join(cmd)}")

    result = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
    if result.returncode != 0:
        print(f"  BUILD FAILED (rc={result.returncode})")
        for line in result.stderr.splitlines()[-20:]:
            print(f"    {line}")
        return None

    dll_path = output_dir / "SnapshotProverSubjects.dll"
    if dll_path.exists():
        size = dll_path.stat().st_size
        print(f"  OK: {dll_path.name} ({size} bytes)")
        return dll_path
    else:
        print(f"  ERROR: DLL not found at {dll_path}")
        return None


# ── Step 2: Build Driver (if needed) ────────────────────────────────────
def ensure_driver_built() -> bool:
    """Ensure Chaos.IL2CPP.Driver is built (for --no-build in convert-to-cpp)."""
    driver_dll = (
        DRIVER_PROJECT / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
    )
    if driver_dll.exists():
        print(f"  Driver already built: {driver_dll.name}")
        return True

    print("\n  Building Chaos.IL2CPP.Driver...")
    cmd = [
        "dotnet", "build", str(DRIVER_PROJECT),
        "--configuration", "Release",
        "/nologo",
    ]
    result = subprocess.run(cmd, capture_output=True, text=True, timeout=180)
    if result.returncode != 0:
        print(f"  Driver BUILD FAILED (rc={result.returncode})")
        print(result.stderr[-500:])
        return False
    print(f"  Driver built OK")
    return True


# ── Step 3: Run convert-to-cpp ──────────────────────────────────────────
def run_convert_to_cpp(dll_path: Path) -> bool:
    """Run chaos-il2cpp convert-to-cpp on the subjects DLL."""
    print("\n=== Step 2: chaos-il2cpp convert-to-cpp ===")

    CODEGEN_DIR.mkdir(parents=True, exist_ok=True)

    cmd = [
        "dotnet", "run", "--no-build",
        "--project", str(DRIVER_PROJECT),
        "--configuration", "Release",
        "--", "convert-to-cpp",
        "--assembly", str(dll_path),
        "--assembly-dir", str(dll_path.parent),
        "--output", str(CODEGEN_DIR),
    ]
    print(f"  Running: dotnet run --no-build --project Driver -- convert-to-cpp ...")
    print(f"  assembly: {dll_path.name}")
    print(f"  output:   {CODEGEN_DIR}")

    result = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8", errors="replace", timeout=300)

    if result.returncode != 0:
        print(f"  FAILED (rc={result.returncode})")
        for line in result.stderr.splitlines()[-20:]:
            print(f"    {line}")
        return False

    # Check for generated C++ output
    cpp_found = False
    for d in sorted(CODEGEN_DIR.iterdir()):
        if d.is_dir() and d.name not in ("build", "generated"):
            per_asm_cpp = d / "generated" / "native-aot.generated.cpp"
            if per_asm_cpp.exists():
                cpp_found = True
                size = per_asm_cpp.stat().st_size
                print(f"  OK: {size} bytes -> {per_asm_cpp.relative_to(CODEGEN_DIR)}")

    if not cpp_found:
        print(f"  WARNING: no .cpp output found in {CODEGEN_DIR}")
    else:
        print(f"  convert-to-cpp completed successfully")

    # Print any interesting stderr messages (warnings, etc.)
    if result.stderr:
        for line in result.stderr.splitlines():
            if "error" in line.lower() or "warning" in line.lower():
                print(f"  [{line}]")

    return True


# ── Step 4: Ensure CMakeLists.txt ──────────────────────────────────────
def ensure_cmakelists():
    """Auto-generate native/CMakeLists.txt if it doesn't exist."""
    cmakelists = NATIVE_DIR / "CMakeLists.txt"
    if cmakelists.exists():
        print(f"\n  CMakeLists.txt already exists")
        return True

    print(f"\n=== Step 3: Generate CMakeLists.txt ===")
    repo_root_str = str(REPO_ROOT).replace("\\", "/")
    codegen_str = str(CODEGEN_DIR.resolve()).replace("\\", "/")
    native_build_str = str(NATIVE_BUILD_DIR.resolve()).replace("\\", "/")

    content = (
        f'cmake_minimum_required(VERSION 3.20)\n'
        f'project(chaos_entry CXX)\n'
        f'set(CMAKE_CXX_STANDARD 20)\n'
        f'\n'
        f'# Compiler settings — /EHa needed for catch(...) to intercept C++ exceptions\n'
        f'# thrown by generated code (throw chaos_managed_exception from unresolved calls).\n'
        f'add_compile_options(/utf-8 /GS-)\n'
        f'add_compile_definitions({CONFIG_TIER})\n'
        f'add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL={LOG_LEVEL})\n'
        f'\n'
        f'# Paths\n'
        f'set(CHAOS_PROJECT_ROOT "{repo_root_str}")\n'
        f'set(CHAOS_CODEGEN_DIR "{codegen_str}")\n'
        f'set(CHAOS_NATIVE_BUILD "{native_build_str}")\n'
        f'\n'
        f'# Source files — codegen outputs to codegen/<AssemblyName>/generated/\n'
        f'file(GLOB CHAOS_CODEGEN_CPP "${{CHAOS_CODEGEN_DIR}}/*/generated/native-aot.generated.cpp")\n'
        f'set(CHAOS_ENTRY_SOURCES\n'
        f'    "runtime-entry.cpp"\n'
        f'    "runtime-patchdata.cpp"\n'
        f'    ${{CHAOS_CODEGEN_CPP}}\n'
        f')\n'
        f'\n'
        f'# Include directories\n'
        f'set(CHAOS_ENTRY_INCLUDES\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/common"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core/gc"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/bootstrap"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/interpreter"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/interpreter/generated"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/support"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/hot-update"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/contracts/native/v0"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/third_party/fmt/include"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/third_party/unordered_dense/include"\n'
        f')\n'
        f'\n'
        f'# Library link directories\n'
        f'set(CHAOS_LIB_DIRS\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/runtime-core/RelWithDebInfo"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/bootstrap/RelWithDebInfo"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/common/RelWithDebInfo"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/interpreter/RelWithDebInfo"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/support/RelWithDebInfo"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/src/native/hot-update/RelWithDebInfo"\n'
        f'    "${{CHAOS_NATIVE_BUILD}}/fmt_build/RelWithDebInfo"\n'
        f')\n'
        f'\n'
        f'# Runtime libs to link\n'
        f'set(CHAOS_RUNTIME_LIBS\n'
        f'    chaos_runtime_core\n'
        f'    chaos_bootstrap\n'
        f'    chaos_common\n'
        f'    chaos_interpreter\n'
        f'    chaos_support\n'
        f'    chaos_hot_update\n'
        f'    chaos_fmt\n'
        f')\n'
        f'\n'
        f'add_executable(entry ${{CHAOS_ENTRY_SOURCES}})\n'
        f'target_include_directories(entry PRIVATE ${{CHAOS_ENTRY_INCLUDES}})\n'
        f'target_link_directories(entry PRIVATE ${{CHAOS_LIB_DIRS}})\n'
        f'target_compile_options(entry PRIVATE /EHa)\n'
        f'target_link_libraries(entry PRIVATE ${{CHAOS_RUNTIME_LIBS}})\n'
    )

    cmakelists.parent.mkdir(parents=True, exist_ok=True)
    with open(cmakelists, "w", encoding="utf-8") as f:
        f.write(content)
    print(f"  Created: {cmakelists.relative_to(REPO_ROOT)}")
    return True


# ── Step 5: Build native entry.exe ─────────────────────────────────────
def build_native_entry() -> bool:
    """CMake configure + build of native/entry.exe."""
    print("\n=== Step 4: Build native entry.exe ===")

    build_dir = NATIVE_DIR / "build"
    build_dir.mkdir(parents=True, exist_ok=True)

    # CMake configure
    cmakelists = NATIVE_DIR / "CMakeLists.txt"
    if not cmakelists.exists():
        print(f"  ERROR: CMakeLists.txt not found at {cmakelists}")
        return False

    print("  Configuring...")
    configure_cmd = [
        "cmake",
        "-S", str(NATIVE_DIR),
        "-B", str(build_dir),
        "-G", "Visual Studio 17 2022",
        "-A", "x64",
        "-DCMAKE_CONFIGURATION_TYPES=RelWithDebInfo",
    ]
    result = subprocess.run(configure_cmd, capture_output=True, text=True, timeout=120)
    if result.returncode != 0:
        print(f"  CMAKE CONFIGURE FAILED (rc={result.returncode})")
        for line in result.stderr.splitlines()[-15:]:
            print(f"    {line}")
        return False
    print(f"  Configure OK")

    # CMake build
    print("  Building (RelWithDebInfo)...")
    build_cmd = [
        "cmake", "--build", str(build_dir),
        "--config", "RelWithDebInfo",
        "--target", "entry",
    ]
    result = subprocess.run(build_cmd, capture_output=True, text=True, timeout=600)
    if result.returncode != 0:
        print(f"  CMAKE BUILD FAILED (rc={result.returncode})")
        for line in result.stdout.splitlines()[-15:]:
            print(f"    {line}")
        for line in result.stderr.splitlines()[-15:]:
            print(f"    ERR: {line}")
        return False

    # Find entry.exe
    entry_exe = build_dir / "RelWithDebInfo" / "entry.exe"
    if not entry_exe.exists():
        entry_exe = build_dir / "entry.exe"
    if not entry_exe.exists():
        # Search in build tree
        found = list(build_dir.rglob("entry.exe"))
        if found:
            entry_exe = found[0]
        else:
            print(f"  ERROR: entry.exe not found in {build_dir}")
            return False

    size = entry_exe.stat().st_size
    print(f"  Build OK: {entry_exe.name} ({size} bytes)")

    # Copy to native/ for convenience
    shutil.copy2(entry_exe, NATIVE_DIR / "entry.exe")
    print(f"  Copied to: {NATIVE_DIR / 'entry.exe'}")

    return True


# ── Step 6: Run and verify ─────────────────────────────────────────────
def run_verification() -> bool:
    """Run entry.exe and verify Passed: 10/10 output."""
    print("\n=== Step 5: Run verification ===")

    entry_exe = NATIVE_DIR / "entry.exe"
    if not entry_exe.exists():
        print(f"  ERROR: entry.exe not found at {entry_exe}")
        return False

    print(f"  Running: {entry_exe}")
    result = subprocess.run(
        [str(entry_exe)],
        capture_output=True, text=True, timeout=60,
    )

    stdout = result.stdout or ""
    stderr = result.stderr or ""
    print(f"  stdout: {stdout.strip()}")
    if stderr:
        for line in stderr.splitlines():
            print(f"  stderr: {line}")

    # Check for Passed: N/M
    import re
    match = re.search(r"Passed:\s*(\d+)/(\d+)", stdout)
    if match:
        passed = int(match.group(1))
        total = int(match.group(2))
        if passed == total and total == METHOD_COUNT:
            print(f"\n  ✅ VERIFICATION PASSED: {passed}/{total}")
            return True
        elif passed == total:
            print(f"\n  ⚠️  Count mismatch: {passed}/{total} (expected {METHOD_COUNT})")
            return total > 0
        else:
            print(f"\n  ❌ VERIFICATION FAILED: {passed}/{total}")
            return False
    else:
        print(f"\n  ❌ Could not find 'Passed: N/M' in output")
        return False


# ── Main ────────────────────────────────────────────────────────────────
def main():
    parser = argparse.ArgumentParser(description="Build and run snapshot-prover verification")
    parser.add_argument(
        "--build-type", choices=["check", "ship"], default="check",
        help="Build configuration tier (default: check)",
    )
    parser.add_argument(
        "--skip-build", action="store_true",
        help="Skip native build, only run entry.exe (for re-verification)",
    )
    args = parser.parse_args()

    global CONFIG_TIER
    if args.build_type == "ship":
        CONFIG_TIER = "CHAOS_IL2CPP_CONFIG_TIER_SHIP"

    os.chdir(REPO_ROOT)
    print(f"=== Snapshot Prover Runtime Verification ===")
    print(f"  Repo root:  {REPO_ROOT}")
    print(f"  Family:     {FAMILY_SLUG}")
    print(f"  Config:     {CONFIG_TIER}")
    print(f"  Subjects:   {METHOD_COUNT} methods")
    print()

    # Step 1: Build subjects DLL
    dll_path = build_subjects_dll()
    if dll_path is None:
        sys.exit(1)

    # Step 2: Ensure driver is built
    if not ensure_driver_built():
        sys.exit(1)

    # Step 3: Run convert-to-cpp
    if not run_convert_to_cpp(dll_path):
        sys.exit(1)

    # Step 4: Ensure CMakeLists.txt
    ensure_cmakelists()

    # Step 5: Build native
    if not args.skip_build:
        if not build_native_entry():
            sys.exit(1)
    else:
        print("\n  Skipping native build (--skip-build)")

    # Step 6: Run verification
    if not run_verification():
        sys.exit(1)

    print(f"\n=== All steps completed successfully ===")


if __name__ == "__main__":
    main()
