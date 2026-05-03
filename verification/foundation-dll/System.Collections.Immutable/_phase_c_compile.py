#!/usr/bin/env python3
"""Phase C: Compile all 10 System.Collections.Immutable generated native-aot.generated.cpp files with MSVC.

A compat header (_compat.h) provides global aliases for namespace-scoped
constants and inline helpers that the emitter references.
"""

from __future__ import annotations

import shutil
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path("D:/agent/booming-il2cpp")
IMMUTABLE_DIR = REPO_ROOT / "verification" / "foundation-dll" / "System.Collections.Immutable"

VC_ROOT = Path("C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/MSVC/14.42.34433")
WIN_KITS = Path("C:/Program Files (x86)/Windows Kits/10")

FAMILIES = [
    "immutable-array",
    "immutable-list",
    "immutable-dictionary",
    "immutable-sorted-dictionary",
    "immutable-hash-set",
    "immutable-sorted-set",
    "immutable-queue-stack",
    "immutable-interlocked",
    "frozen-collections",
    "extensions-marshal",
]

COMPAT_HEADER = r"""// Phase C compatibility header — force-included to provide symbols the C# emitter
// references that are missing from or namespaced differently in native headers.

#ifndef CHAOS_IL2CPP_VERIFICATION_COMPAT_H_
#define CHAOS_IL2CPP_VERIFICATION_COMPAT_H_

#include <chaos/native_types.h>
#include <chaos/ptr_tag.h>

// -- Global aliases for namespace-scoped constants ---------------------------
constexpr auto chaos_managed_pointer_local_slot_tag =
    ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;

constexpr auto chaos_raw_int32_pointer_tag =
    ChaosIl2cpp::Common::k_raw_int32_pointer_tag;

// -- StringId helpers --------------------------------------------------------
// StringIds use the upper 16 bits of a 64-bit value as a non-canonical tag.
inline bool chaos_is_string_id(CHAOS_IL2CPP_INTPTR value) noexcept
{
    auto raw = static_cast<CHAOS_IL2CPP_UINT64>(value);
    auto upper = static_cast<CHAOS_IL2CPP_UINT16>(raw >> 48);
    return upper != 0 && upper != 0xFFFF;
}

inline CHAOS_IL2CPP_UINT64 chaos_extract_string_id(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return static_cast<CHAOS_IL2CPP_UINT64>(value);
}

// CHAOS_IL2CPP_STRING_TYPE is defined per-TU after the String struct decl.
#ifndef CHAOS_IL2CPP_STRING_TYPE
#define CHAOS_IL2CPP_STRING_TYPE chaos_type_System_Private_CoreLib_System_String
#endif

#endif
"""


def find_generated_cpp(family: str) -> Path | None:
    candidates = [
        IMMUTABLE_DIR / family / "native" / "genuine" / "generated" / "native-aot.generated.cpp",
        IMMUTABLE_DIR / f"family-System.Collections.Immutable-{family}" / "native" / "genuine" / "generated" / "native-aot.generated.cpp",
    ]
    for c in candidates:
        if c.exists():
            return c
    return None


def fix_source(source: Path, build_dir: Path) -> Path:
    """Apply source-level fixes for MSVC compatibility.

    The compat header (_compat.h) provides namespace-scoped aliases and
    inline helpers that the C# emitter references but aren't declared in
    native headers.  No per-TU source patching is needed.
    """
    fixed_path = build_dir / f"{source.stem}_fixed.cpp"
    fixed_path.write_text(source.read_text(encoding="utf-8"), encoding="utf-8")
    return fixed_path


def _compile_cpp(source: Path, build_dir: Path) -> tuple[bool, str]:
    """Compile a single .cpp with MSVC.

    Returns (success, log).
    """
    # Write compat header
    compat_path = build_dir / "_compat.h"
    compat_path.write_text(COMPAT_HEADER, encoding="utf-8")

    # Apply source fixes
    fixed_src = fix_source(source, build_dir)

    # Build env
    env = {
        "PATH": f"{VC_ROOT / 'bin' / 'Hostx64' / 'x64'};{__import__('os').environ.get('PATH', '')}",
        "INCLUDE": (
            f"{VC_ROOT / 'include'};"
            f"{VC_ROOT / 'atlmfc' / 'include'};"
            f"{WIN_KITS / 'Include/10.0.22621.0/ucrt'};"
            f"{WIN_KITS / 'Include/10.0.22621.0/um'};"
            f"{WIN_KITS / 'Include/10.0.22621.0/shared'};"
            f"{WIN_KITS / 'Include/10.0.22621.0/winrt'};"
            f"{REPO_ROOT / 'src' / 'native' / 'common'};"
            f"{REPO_ROOT / 'src' / 'native' / 'runtime-core'};"
            f"{REPO_ROOT / 'contracts' / 'native' / 'v0'};"
            f"{REPO_ROOT / 'third_party' / 'fmt' / 'include'};"
            f"{build_dir}"
        ),
        "LIB": (
            f"{VC_ROOT / 'lib' / 'x64'};"
            f"{WIN_KITS / 'Lib/10.0.22621.0/ucrt/x64'};"
            f"{WIN_KITS / 'Lib/10.0.22621.0/um/x64'}"
        ),
    }

    obj_path = build_dir / f"{source.stem}.obj"
    cmd = [
        str(VC_ROOT / "bin" / "Hostx64" / "x64" / "cl.exe"),
        "/nologo",
        "/std:c++17",
        "/utf-8",
        f"/FI{build_dir / '_compat.h'}",
        "/c",
        f"/Fo{obj_path}",
        str(fixed_src),
    ]

    log_lines = [f"$ {' '.join(str(c) for c in cmd)}"]

    # PowerShell to set env and run
    ps_script = (
        "$env:PATH='" + env["PATH"] + "'; "
        "$env:INCLUDE='" + env["INCLUDE"] + "'; "
        "$env:LIB='" + env["LIB"] + "'; "
        "& '" + str(cmd[0]) + "' " + " ".join(f"'{c}'" for c in cmd[1:]) + " 2>&1"
    )

    try:
        result = subprocess.run(
            ["powershell", "-Command", ps_script],
            capture_output=True, text=True, timeout=120,
        )
        log_lines.append(result.stdout)
        log_lines.append(result.stderr)
        success = result.returncode == 0
        return success, "\n".join(log_lines)
    except subprocess.TimeoutExpired:
        return False, "TIMEOUT"
    except Exception as e:
        return False, str(e)


def main():
    build_root = IMMUTABLE_DIR / "_phase_c_build"
    build_root.mkdir(parents=True, exist_ok=True)

    print(f"Phase C: Compiling {len(FAMILIES)} Immutable families with MSVC")
    print(f"MSVC: {VC_ROOT}")
    print(f"Build: {build_root}")
    print()

    results: dict[str, bool] = {}
    logs: dict[str, str] = {}

    for family in FAMILIES:
        print(f"  [{family}] ", end="", flush=True)
        src = find_generated_cpp(family)
        if src is None:
            print("SKIP (no generated cpp)")
            results[family] = False
            logs[family] = "No generated cpp found"
            continue

        ok, log = _compile_cpp(src, build_root)
        results[family] = ok
        logs[family] = log

        if ok:
            print("OK")
        else:
            print("FAIL")
            # Show error summary
            for line in log.split("\n"):
                if "error" in line.lower() or "fatal" in line.lower():
                    print(f"    {line.strip()}")

    print()
    print("=" * 60)
    passed = sum(1 for v in results.values() if v)
    failed = sum(1 for v in results.values() if not v)
    print(f"Results: {passed}/{len(results)} passed, {failed} failed")
    for family, ok in results.items():
        print(f"  {family}: {'OK' if ok else 'FAIL'}")

    # Write log
    log_file = build_root / "phase_c_results.log"
    with open(log_file, "w", encoding="utf-8") as f:
        for family in FAMILIES:
            f.write(f"\n{'='*60}\nFamily: {family} {'OK' if results.get(family) else 'FAIL'}\n{'='*60}\n")
            f.write(logs.get(family, "No log"))
            f.write("\n")
    print(f"\nFull log: {log_file}")


if __name__ == "__main__":
    main()
