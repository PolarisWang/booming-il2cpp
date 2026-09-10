#!/usr/bin/env python3
"""
manifest stage — Scan DLL → dll-method-manifest.json + namespace-partition.json.

Usage:
    python -m verification.stages.manifest --dll <path> [--output <dir>] [--max-methods-per-chunk 500]
"""

import argparse
import json
import os
import subprocess
import sys
from collections import OrderedDict
from datetime import datetime, timezone
from pathlib import Path


# ── Paths ──
_SCRIPT_DIR = Path(__file__).resolve().parent
_VERIFICATION_ROOT = _SCRIPT_DIR.parent  # verification/
_FOUNDATION_DLL = _VERIFICATION_ROOT.parent  # testing/foundation-dll/
_TOOLS_DIR = Path(__file__).resolve().parents[4] / "src" / "tools"
_TOOL_PROJECT = _TOOLS_DIR / "Chaos.IL2CPP.Tools.DllManifest" / "Chaos.IL2CPP.Tools.DllManifest.csproj"
_TOOL_DLL = _TOOLS_DIR / "Chaos.IL2CPP.Tools.DllManifest" / "bin" / "Debug" / "net8.0" / "Chaos.IL2CPP.Tools.DllManifest.dll"

# Default namespace → chunk slug mapping for well-known .NET namespaces.
# Used for deterministic, human-readable chunk names.
_DEFAULT_NAMESPACE_CHUNKS: dict[str, str] = {
    # Global non-System namespaces
    "": "global-ns",
    "Internal.Runtime.InteropServices": "global-ns",
    "Internal.Win32": "global-ns",
    "Microsoft.Win32.SafeHandles": "global-ns",
    # Core system
    "System": "system",
    "System.CodeDom.Compiler": "system",
    "System.StubHelpers": "system",
    # Collections
    "System.Collections.NonGeneric": "collections",
    "System.Collections.Generic": "collections-generic",
    "System.Collections.ObjectModel": "collections-generic",
    "System.Collections.Concurrent": "collections-concurrent",
    "System.Collections.Specialized": "collections-specialized",
    # Component model / Configuration
    "System.ComponentModel": "component-model",
    "System.Configuration": "configuration",
    # Diagnostics
    "System.Diagnostics": "diagnostics",
    "System.Diagnostics.CodeAnalysis": "diagnostics",
    "System.Diagnostics.Contracts": "diagnostics",
    "System.Diagnostics.Tracing": "diagnostics",
    # Globalization
    "System.Globalization": "globalization",
    # IO
    "System.IO": "io",
    "System.IO.Compression": "io-compression",
    "System.IO.Enumeration": "io",
    "System.IO.MemoryMappedFiles": "io-memory",
    "System.IO.Pipes": "io-pipes",
    "System.IO.Strategies": "io",
    # Linq
    "System.Linq": "linq",
    "System.Linq.Expressions": "linq-expressions",
    # Net
    "System.Net": "net",
    # Numerics
    "System.Numerics": "numerics",
    # Reflection
    "System.Reflection": "reflection",
    "System.Reflection.Emit": "reflection-emit",
    # Resources
    "System.Resources": "resources",
    # Runtime
    "System.Runtime": "runtime",
    "System.Runtime.CompilerServices": "runtime-compiler",
    "System.Runtime.ExceptionServices": "runtime",
    "System.Runtime.InteropServices": "runtime-interop",
    "System.Runtime.InteropServices.CustomMarshalers": "runtime-interop",
    "System.Runtime.InteropServices.Marshalling": "runtime-interop",
    "System.Runtime.InteropServices.ObjectiveC": "runtime-interop",
    "System.Runtime.Intrinsics": "runtime-intrinsics",
    "System.Runtime.Loader": "runtime",
    "System.Runtime.Remoting": "runtime",
    "System.Runtime.Serialization": "runtime-serialization",
    "System.Runtime.Versioning": "runtime",
    # Security
    "System.Security": "security",
    "System.Security.Claims": "security-claims",
    "System.Security.Cryptography": "security-cryptography",
    "System.Security.Permissions": "security",
    "System.Security.Principal": "security",
    # Stub helpers (tiny, group with system)
    "System.StubHelpers": "system",
    # Text
    "System.Text": "text",
    "System.Text.Json": "text-json",
    "System.Text.RegularExpressions": "text-regex",
    "System.Text.Unicode": "text",
    # Threading
    "System.Threading": "threading",
    "System.Threading.Tasks": "threading-tasks",
    "System.Threading.Channels": "threading-channels",
    "System.Threading.RateLimiting": "threading-rate",
    # Transactions
    "System.Transactions": "transactions",
    # Buffers
    "System.Buffers": "buffers",
    "System.Buffers.Binary": "buffers",
    "System.Buffers.Text": "buffers",
    # Xml
    "System.Xml": "xml",
    "System.Xml.Linq": "xml-linq",
}


def build_tool() -> None:
    """Build the DllManifest C# tool if not already built."""
    dll = Path(_TOOL_DLL)
    if dll.exists():
        return
    print(f"[manifest] Building DLL scanner tool...", file=sys.stderr)
    result = subprocess.run(
        ["dotnet", "build", str(_TOOL_PROJECT), "-nologo"],
        capture_output=True, text=True, timeout=120
    )
    if result.returncode != 0:
        print(f"[manifest] ERROR: Failed to build scanner tool:\n{result.stderr}", file=sys.stderr)
        sys.exit(1)


def scan_dll(dll_path: str) -> dict:
    """Run DllManifest scanner and return parsed JSON."""
    build_tool()
    result = subprocess.run(
        ["dotnet", "exec", str(_TOOL_DLL), "--dll", dll_path],
        capture_output=True, text=True, timeout=120
    )
    if result.returncode != 0:
        print(f"[manifest] ERROR: Scanner failed:\n{result.stderr}", file=sys.stderr)
        sys.exit(1)
    return json.loads(result.stdout)


def build_namespace_partition(manifest: dict, max_methods_per_chunk: int = 500) -> dict:
    """Build namespace-partition.json from the manifest data."""
    methods = manifest["methods"]
    namespaces = manifest["namespaces"]

    # Group methods by namespace
    ns_methods: dict[str, list[dict]] = {}
    for m in methods:
        ns = m["namespace"]
        ns_methods.setdefault(ns, []).append(m)

    # Sort namespaces by name for deterministic output
    sorted_ns = sorted(namespaces.keys())

    def slug_for_ns(ns: str) -> str:
        """Compute the target slug for a single namespace."""
        s = _DEFAULT_NAMESPACE_CHUNKS.get(ns)
        if s is not None:
            return s
        if not ns:
            return "global-ns"
        s = ns.lower().replace(".", "-").replace(" ", "-")
        if s and not s[0].isalpha():
            s = "ns-" + s
        return s

    # Group namespaces by slug
    slug_groups: dict[str, list[str]] = {}
    for ns in sorted_ns:
        s = slug_for_ns(ns)
        slug_groups.setdefault(s, []).append(ns)

    # Sort slug groups by their first namespace for deterministic output
    sorted_slugs = sorted(slug_groups.keys(),
                          key=lambda s: slug_groups[s][0])

    # Merge small groups (< MERGE_THRESHOLD methods) into adjacent large groups
    # to avoid hundreds of tiny chunks.
    MERGE_THRESHOLD = 50
    chunks: list[dict] = []

    def emit_chunk(slug: str, ns_list: list[str]) -> None:
        """Create one chunk from a list of namespaces (all sharing the same slug)."""
        total = sum(namespaces[ns]["methodCount"] for ns in ns_list)
        if total == 0:
            return

        # Collect all methods in order
        all_methods: list[dict] = []
        for ns in ns_list:
            for m in ns_methods.get(ns, []):
                all_methods.append({"methodSubjectId": m["methodSubjectId"]})

        # Assign indices
        for i, cm in enumerate(all_methods):
            cm["index"] = i

        if total <= max_methods_per_chunk:
            chunks.append({
                "slug": slug,
                "namespaces": list(ns_list),
                "methodCount": total,
                "methods": all_methods,
            })
            return

        # Split into batches
        for bi in range(0, len(all_methods), max_methods_per_chunk):
            batch = all_methods[bi:bi + max_methods_per_chunk]
            batch_slug = slug if bi == 0 else f"{slug}-{bi // max_methods_per_chunk + 1}"
            chunks.append({
                "slug": batch_slug,
                "namespaces": list(ns_list),
                "methodCount": len(batch),
                "methods": batch,
            })

    # Process slug groups: small groups get absorbed into the nearest large group
    slug_groups_order: list[tuple[str, list[str], int]] = []
    for slug in sorted_slugs:
        group_ns = slug_groups[slug]
        group_total = sum(namespaces[ns]["methodCount"] for ns in group_ns)
        slug_groups_order.append((slug, group_ns, group_total))

    # Two-pass: first absorb small groups into adjacent large ones
    merged_plan: list[tuple[str, list[str]]] = []
    absorb_buffer: list[tuple[str, list[str]]] = []

    for slug, group_ns, total in slug_groups_order:
        if total < MERGE_THRESHOLD:
            absorb_buffer.append((slug, group_ns))
        else:
            # Absorb any pending small groups into this large group
            for s_slug, s_ns in absorb_buffer:
                merged_plan.append((slug, s_ns))
            absorb_buffer = []
            merged_plan.append((slug, group_ns))

    # Remaining small groups at the end → absorb into global-ns
    for s_slug, s_ns in absorb_buffer:
        merged_plan.append(("global-ns", s_ns))

    # Merge adjacent entries with the same slug
    final_plan: list[tuple[str, list[str]]] = []
    for slug, ns_list in merged_plan:
        if final_plan and final_plan[-1][0] == slug:
            final_plan[-1][1].extend(ns_list)
        else:
            final_plan.append((slug, list(ns_list)))

    # Emit chunks from the final plan
    for slug, ns_list in final_plan:
        emit_chunk(slug, ns_list)

    return {
        "schemaVersion": 1,
        "assemblyName": manifest["assemblyName"],
        "generatedAt": datetime.now(timezone.utc).isoformat(),
        "partitionStrategy": "namespace",
        "maxMethodsPerChunk": max_methods_per_chunk,
        "totalChunks": len(chunks),
        "chunks": chunks,
    }


def save_json(data: dict, path: Path) -> None:
    """Write JSON with consistent formatting."""
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
    print(f"[manifest] {path.name}: {data.get('totalMethods') or data.get('totalChunks')} entries", file=sys.stderr)


def main():
    parser = argparse.ArgumentParser(description="DLL manifest scanner")
    parser.add_argument("--dll", required=True, help="Path to the target DLL")
    parser.add_argument("--output", default=None, help="Output directory for manifest files")
    parser.add_argument("--max-methods-per-chunk", type=int, default=500,
                        help="Maximum methods per chunk (default: 500)")
    args = parser.parse_args()

    dll_path = os.path.abspath(args.dll)
    if not os.path.exists(dll_path):
        print(f"ERROR: DLL not found: {dll_path}", file=sys.stderr)
        sys.exit(1)

    # Default output: <assembly>/_dll/
    if args.output:
        output_dir = Path(os.path.abspath(args.output))
    else:
        assembly_name = Path(dll_path).stem
        output_dir = _FOUNDATION_DLL / assembly_name / "_dll"

    print(f"[manifest] Scanning {dll_path}...", file=sys.stderr)

    # Scan DLL
    manifest = scan_dll(dll_path)

    # Save dll-method-manifest.json
    manifest_path = output_dir / "dll-method-manifest.json"
    save_json(manifest, manifest_path)

    # Build and save namespace-partition.json
    partition = build_namespace_partition(manifest, args.max_methods_per_chunk)
    partition_path = output_dir / "namespace-partition.json"
    save_json(partition, partition_path)

    print(f"[manifest] Done: {manifest['totalMethods']} methods in {partition['totalChunks']} chunks", file=sys.stderr)


if __name__ == "__main__":
    main()
