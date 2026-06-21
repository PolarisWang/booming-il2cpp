#!/usr/bin/env python3
"""Batch manifest — generate namespace-partition.json for all foundation-dll families.

Scans testing/foundation-dll/ for family directories, finds their DLLs,
and runs the manifest stage to generate dll-method-manifest.json and
namespace-partition.json for each assembly that lacks them.

Usage:
    # Generate for all families
    python testing/foundation-dll/batch_manifest.py --all

    # Generate for specific families
    python testing/foundation-dll/batch_manifest.py --families System.Collections.NonGeneric,System.Text.Json

    # Dry run (show what would be done)
    python testing/foundation-dll/batch_manifest.py --all --dry-run
"""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path


_FOUNDATION_DLL = Path(__file__).resolve().parent  # testing/foundation-dll/
_REPO_ROOT = _FOUNDATION_DLL.parent.parent


# ── Families that are known to have resolvable DLLs ──
_KNOWN_FAMILIES: dict[str, str] = {
    # Key: directory name, Value: DLL name (usually same)
    "System.Collections.NonGeneric": "System.Collections.NonGeneric",
    "System.Collections.Immutable": "System.Collections.Immutable",
    "System.ComponentModel.TypeConverter": "System.ComponentModel.TypeConverter",
    "System.Data.Common": "System.Data.Common",
    "System.Diagnostics.DiagnosticSource": "System.Diagnostics.DiagnosticSource",
    "System.Formats.Asn1": "System.Formats.Asn1",
    "System.IO.Compression.Brotli": "System.IO.Compression.Brotli",
    "System.IO.Pipelines": "System.IO.Pipelines",
    "System.Linq": "System.Linq",
    "System.Linq.Expressions": "System.Linq.Expressions",
    "System.Net.Http": "System.Net.Http",
    "System.Net.ServerSentEvents": "System.Net.ServerSentEvents",
    "System.Net.Sockets": "System.Net.Sockets",
    "System.ObjectModel": "System.ObjectModel",
    "System.Private.CoreLib": "System.Private.CoreLib",
    "System.Private.Xml": "System.Private.Xml",
    "System.Reflection.Metadata": "System.Reflection.Metadata",
    "System.Runtime": "System.Runtime",
    "System.Runtime.InteropServices": "System.Runtime.InteropServices",
    "System.Runtime.Intrinsics": "System.Runtime.Intrinsics",
    "System.Runtime.Serialization.Formatters": "System.Runtime.Serialization.Formatters",
    "System.Security.Claims": "System.Security.Claims",
    "System.Security.Cryptography": "System.Security.Cryptography",
    "System.Security.Principal.Windows": "System.Security.Principal.Windows",
    "System.Text.Json": "System.Text.Json",
    "System.Threading.Tasks.Parallel": "System.Threading.Tasks.Parallel",
    "System.Xml.ReaderWriter": "System.Xml.ReaderWriter",
}


def find_assembly_dll(dll_name: str) -> Path | None:
    """Find the DLL for a given assembly name.

    Search order:
      1. Local copy in foundation-dll/{assembly}/{assembly}.dll
      2. DOTNET_ROOT/shared/Microsoft.NETCore.App/**/{assembly}.dll
      3. NuGet cache (~/.nuget/packages/{assembly_lower}/**/{assembly}.dll)
    """
    # Local copy
    local = _FOUNDATION_DLL / dll_name / f"{dll_name}.dll"
    if local.exists():
        return local

    # DOTNET_ROOT shared framework
    dotnet_root = Path(os.environ.get("DOTNET_ROOT", "C:/Program Files/dotnet/shared"))
    for runtime_dir in sorted(dotnet_root.rglob(f"**/{dll_name}.dll")):
        return runtime_dir

    # DOTNET_ROOT for .NET core framework assemblies
    if dll_name.startswith("System."):
        for runtime_dir in sorted(dotnet_root.rglob(f"**/{dll_name}.dll")):
            return runtime_dir

    # NuGet cache
    nuget_cache = Path(os.environ.get("NUGET_PACKAGES",
                                       Path.home() / ".nuget" / "packages"))
    dll_lower = dll_name.lower()
    for pkg_dir in sorted(nuget_cache.rglob(f"{dll_lower}/**/{dll_name}.dll")):
        return pkg_dir

    return None


def has_partition(assembly_dir: Path) -> bool:
    """Check if the assembly already has namespace-partition.json."""
    partition_path = assembly_dir / "_dll" / "namespace-partition.json"
    return partition_path.exists()


def run_manifest(assembly_dir: Path, dll_path: Path, verbose: bool = False) -> bool:
    """Run the manifest stage for an assembly.

    Returns True on success.
    """
    manifest_py = _FOUNDATION_DLL / "verification" / "stages" / "manifest.py"
    if not manifest_py.exists():
        print(f"  ERROR: manifest.py not found at {manifest_py}")
        return False

    if verbose:
        print(f"  [manifest] Running for {assembly_dir.name}...")

    result = subprocess.run(
        [sys.executable, str(manifest_py),
         "--dll", str(dll_path),
         "--output", str(assembly_dir / "_dll"),
         "--max-methods-per-chunk", "500"],
        capture_output=True, text=True, timeout=300,
    )

    if result.returncode != 0:
        print(f"  FAILED: {assembly_dir.name}")
        for line in (result.stderr.splitlines() + result.stdout.splitlines())[-10:]:
            print(f"    {line}")
        return False

    if verbose:
        for line in result.stdout.splitlines():
            print(f"    {line}")
        for line in result.stderr.splitlines():
            print(f"    {line}")

    return True


def scan_families() -> list[tuple[str, Path, Path]]:
    """Scan foundation-dll for family directories.

    Returns list of (family_name, assembly_dir, dll_path) tuples.
    """
    families: list[tuple[str, Path, Path]] = []

    for entry in sorted(_FOUNDATION_DLL.iterdir()):
        if not entry.is_dir():
            continue
        name = entry.name

        # Skip non-family directories
        if name.startswith("_") or name.startswith(".") or name in (
            "config", "output", "results", "verification",
        ):
            continue

        # Look up the DLL name
        dll_name = _KNOWN_FAMILIES.get(name, name)
        dll_path = find_assembly_dll(dll_name)
        if dll_path is None:
            continue

        families.append((name, entry, dll_path))

    return families


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Batch manifest generation for foundation-dll families",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument("--all", action="store_true",
                        help="Process all families without namespace-partition.json")
    parser.add_argument("--families", default=None,
                        help="Comma-separated list of family names to process")
    parser.add_argument("--dry-run", action="store_true",
                        help="Show what would be done without doing it")
    parser.add_argument("--verbose", action="store_true",
                        help="Verbose output")
    parser.add_argument("--force", action="store_true",
                        help="Regenerate even if partition already exists")
    args = parser.parse_args()

    if not args.all and not args.families:
        print("ERROR: Specify --all or --families")
        return 1

    # Determine target families
    if args.families:
        target_names = {f.strip() for f in args.families.split(",")}
        all_families = scan_families()
        families = [(n, d, p) for n, d, p in all_families if n in target_names]
        not_found = target_names - {f[0] for f in families}
        for nf in sorted(not_found):
            print(f"WARNING: Family '{nf}' not found or DLL not resolvable")
    else:
        families = scan_families()

    if not families:
        print("No families found to process.")
        return 0

    print(f"Found {len(families)} families")
    if args.dry_run:
        print(f"{'='*60}")
        for name, assembly_dir, dll_path in families:
            has = has_partition(assembly_dir)
            status = "✅ has partition" if has else "❌ needs partition"
            print(f"  {name:40s} {status:20s} ({dll_path.name})")
        print(f"{'='*60}")
        print(f"Dry run. Run without --dry-run to generate.")
        return 0

    # Process families
    success_count = 0
    skip_count = 0
    fail_count = 0

    for name, assembly_dir, dll_path in families:
        if not args.force and has_partition(assembly_dir):
            if args.verbose:
                print(f"  SKIP {name}: partition already exists")
            skip_count += 1
            continue

        print(f"  [{'>' if args.force else '+'}] {name}: {dll_path}")

        ok = run_manifest(assembly_dir, dll_path, verbose=args.verbose)
        if ok:
            success_count += 1
        else:
            fail_count += 1

    # Summary
    print(f"\n{'='*60}")
    print(f"Batch manifest complete: {success_count} generated, "
          f"{skip_count} skipped, {fail_count} failed")
    print(f"{'='*60}")

    # List generated partitions
    if success_count > 0:
        print(f"\nNew partitions:")
        for name, assembly_dir, _ in families:
            partition_path = assembly_dir / "_dll" / "namespace-partition.json"
            if partition_path.exists():
                try:
                    data = json.loads(partition_path.read_text(encoding="utf-8"))
                    chunks = data.get("totalChunks", 0)
                    methods = sum(c.get("methodCount", 0) for c in data.get("chunks", []))
                    print(f"  {name}: {chunks} chunks, {methods} methods")
                except (json.JSONDecodeError, OSError):
                    print(f"  {name}: partition file exists but unreadable")

    return 0 if fail_count == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
