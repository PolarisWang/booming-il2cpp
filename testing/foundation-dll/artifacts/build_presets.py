#!/usr/bin/env python3
"""Build native runtime SDK from source.

Builds all native C++ runtime libraries via cmake presets, then collects
the resulting .lib files into testing/foundation-dll/sdk/<preset-name>/.

Usage:
    python testing/foundation-dll/artifacts/build_presets.py
    python testing/foundation-dll/artifacts/build_presets.py --preset windows-x64-reference
    python testing/foundation-dll/artifacts/build_presets.py --check  # dry-run: check if up-to-date

Output:
    testing/foundation-dll/sdk/windows-x64-reference/
        ├── chaos_runtime_core.lib
        ├── chaos_bootstrap.lib
        ├── chaos_common.lib
        ├── chaos_pal.lib
        ├── chaos_jit.lib
        ├── chaos_support.lib
        └── ...
"""

from __future__ import annotations

import hashlib
import os
import shutil
import subprocess
import sys
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parents[3]
_PRESETS_DIR = _REPO_ROOT / "artifacts" / "presets"
_SDK_ROOT = _REPO_ROOT / "testing" / "foundation-dll" / "sdk"
_NATIVE_ROOT = _REPO_ROOT / "src" / "native"

# ── Presets that need to be built ──
PRESETS = {
    "windows-x64-reference": {
        "cmake_preset": "windows-x64-reference",
        "config": "RelWithDebInfo",
        "sdk_subdir": "windows-x64-reference",
    },
}


def _get_source_hash() -> str:
    """Compute a hash of all native source files to detect changes."""
    hasher = hashlib.sha256()
    for root, dirs, files in os.walk(_NATIVE_ROOT):
        # Skip build artifacts, cmake output, git dirs
        dirs[:] = [d for d in dirs if not d.startswith((".", "__")) and d not in ("build", "out")]
        for f in sorted(files):
            if f.endswith((".cpp", ".h", ".hpp", ".cmake", ".txt", ".scriban")):
                path = Path(root) / f
                rel = path.relative_to(_REPO_ROOT)
                hasher.update(str(rel).encode())
                try:
                    hasher.update(path.read_bytes()[:4096])  # first 4KB
                except OSError:
                    pass
    return hasher.hexdigest()[:16]


def _sdk_needs_rebuild(preset_name: str) -> bool:
    """Check if the SDK is missing or source files have changed."""
    sdk_dir = _SDK_ROOT / preset_name
    hash_file = sdk_dir / ".source_hash"
    if not sdk_dir.is_dir():
        return True
    if not hash_file.exists():
        return True
    prev_hash = hash_file.read_text(encoding="utf-8").strip()
    current_hash = _get_source_hash()
    return prev_hash != current_hash


def _write_source_hash(preset_name: str) -> None:
    """Write source hash to mark SDK as up-to-date."""
    sdk_dir = _SDK_ROOT / preset_name
    sdk_dir.mkdir(parents=True, exist_ok=True)
    (sdk_dir / ".source_hash").write_text(_get_source_hash(), encoding="utf-8")


def build_preset(preset_name: str, force: bool = False) -> bool:
    """Configure and build a single preset. Returns True on success."""
    info = PRESETS.get(preset_name)
    if not info:
        print(f"[build-presets] Unknown preset: {preset_name}", file=sys.stderr)
        return False

    cmake_preset = info["cmake_preset"]
    config = info["config"]
    sdk_subdir = info["sdk_subdir"]

    if not force and not _sdk_needs_rebuild(preset_name):
        print(f"[build-presets] {preset_name}: SDK is up-to-date (source hash matches)")
        return True

    print(f"[build-presets] Building {preset_name}...")

    # Step 1: cmake --preset (configure)
    result = subprocess.run(
        ["cmake", "--preset", cmake_preset],
        cwd=_REPO_ROOT, capture_output=True, text=True, timeout=120,
    )
    if result.returncode != 0:
        print(f"[build-presets] cmake configure FAILED for {preset_name}")
        print(result.stderr[-500:])
        return False

    # Step 2: cmake --build all targets
    preset_dir = _PRESETS_DIR / preset_name
    result = subprocess.run(
        ["cmake", "--build", str(preset_dir), "--config", config],
        capture_output=True, text=True, timeout=1200,
    )
    if result.returncode != 0:
        print(f"[build-presets] cmake build FAILED for {preset_name}")
        # Print only the last 30 lines of errors
        errors = [l for l in (result.stderr + result.stdout).split("\n") if "error" in l.lower()]
        for e in errors[-20:]:
            print(f"  {e.strip()}")
        return False

    # Step 3: Collect .lib files into SDK directory
    sdk_target = _SDK_ROOT / sdk_subdir
    sdk_lib_dir = sdk_target / "lib"
    sdk_lib_dir.mkdir(parents=True, exist_ok=True)

    lib_count = 0
    for lib in preset_dir.rglob("*.lib"):
        if "RelWithDebInfo" in str(lib) or config in str(lib):
            target = sdk_lib_dir / lib.name
            shutil.copy2(lib, target)
            lib_count += 1

    _write_source_hash(preset_name)
    print(f"[build-presets] {preset_name}: {lib_count} libs -> {sdk_lib_dir}")
    return True


def main() -> int:
    import argparse

    parser = argparse.ArgumentParser(description="Build native runtime SDK from source")
    parser.add_argument("--preset", choices=list(PRESETS.keys()) + ["all"], default="all",
                        help="Preset to build (default: all)")
    parser.add_argument("--force", action="store_true",
                        help="Force rebuild even if source hash matches")
    parser.add_argument("--check", action="store_true",
                        help="Check if SDK needs rebuild without building")
    args = parser.parse_args()

    presets = list(PRESETS.keys()) if args.preset == "all" else [args.preset]

    if args.check:
        for p in presets:
            needs = _sdk_needs_rebuild(p)
            sdk_dir = _SDK_ROOT / PRESETS[p]["sdk_subdir"]
            print(f"{p}: {'NEEDS REBUILD' if needs else 'up-to-date'} ({sdk_dir})")
        return 0

    ok = True
    for p in presets:
        if not build_preset(p, force=args.force):
            ok = False

    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
