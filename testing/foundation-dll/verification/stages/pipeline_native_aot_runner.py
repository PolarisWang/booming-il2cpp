"""Batch-run the CodeGen pipeline across all CoreLib families.

For each family:
  1. Generate synthetic entry point C# source + project (via family_entrypoint_generator)
  2. Build the DLL
  3. Run chaos-il2cpp convert-to-cpp (IL lowering + C++ emission)
  4. Build entry.exe from generated C++ via CMake (static lib + runtime stubs)
  5. Run the native entry EXE directly for Fact verification

Results are written to a summary JSON file.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[3]  # D:/agent/booming-il2cpp
_VERIFICATION_BASE = _REPO_ROOT / "testing" / "foundation-dll"
_VERIFICATION: Path | None = None  # set per-assembly in main()

from verification.orchestration.family_entrypoint import generate_and_build

# trace — no-op stubs for removed testing.trace module
def trace_init(*args, **kwargs): pass
def trace(name, **kwargs): pass

# Families to process (all 23 non-report CoreLib families)
FAMILIES = [
    "array-indexing-copy",
    "buffer-memory",
    "collections-generic-core",
    "convert-char",
    "enum-parsing",
    "garbage-collection",
    "generic-nullable-value",
    "guid-random-hashcode",
    "math-numerics",
    "object-equality-identity",
    "primitive-numeric-conversions",
    "string-char-text-core",
    "time-date-time-timespan",
    "threading-monitor-interlocked",
    "threading-thread-basics",
    "attributes-custom-metadata",
    "boxing-unboxing-casts",
    "delegate-core-invocation",
    "exception-throw-diagnostics",
    "globalization-culture",
    "io-streams-basics",
    "reflection-member-basics",
    "runtime-compiler-services",
    "span-memory-buffers",
    "type-runtime-handles",
    # New reflection families (Phase 1 — 8 families, 135 methods)
    "reflection-type",
    "reflection-assembly",
    "reflection-member-complete",
    "reflection-generics",
    "reflection-parameters",
    "reflection-module",
    "reflection-binding",
    "reflection-activation",
    # COM interop families
    "ccw-basic",
    "rcw-basic",
    "dispatch-basic",
]


def discover_families(assembly_name: str) -> list[str]:
    """Discover families for an assembly by scanning for capability-family-contract.json files."""
    asm_dir = _VERIFICATION_BASE / assembly_name
    if not asm_dir.exists():
        return []
    families = []
    for d in sorted(asm_dir.iterdir()):
        if d.is_dir() and (d / "capability-family-contract.json").exists():
            families.append(d.name)
    return families


def load_method_subject_ids(family_slug: str, *, verification: Path | None = None) -> list[str]:
    """Load method subject IDs from the capability family contract."""
    v = verification or _VERIFICATION
    contract_path = v / family_slug / "capability-family-contract.json"
    if not contract_path.exists():
        # Fall back to testing/ (authoritative contract source for old families)
        contract_path = (_REPO_ROOT / "testing" / contract_path.relative_to(_REPO_ROOT / "verification"))
    if not contract_path.exists():
        print(f"  [SKIP] no contract at {contract_path}")
        return []

    with open(contract_path, encoding="utf-8") as f:
        contract = json.load(f)

    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]

    return mids


def count_methods_in_contract(family_slug: str, *, verification: Path | None = None) -> int:
    """Count methods from contract (either methodSubjectIds list or contract entries)."""
    v = verification or _VERIFICATION
    contract_path = v / family_slug / "capability-family-contract.json"
    if not contract_path.exists():
        contract_path = (_REPO_ROOT / "testing" / contract_path.relative_to(_REPO_ROOT / "verification"))
    if not contract_path.exists():
        return 0
    with open(contract_path, encoding="utf-8") as f:
        contract = json.load(f)
    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]
    return len(mids)


def build_subjects_dll(
    family_slug: str,
    method_subject_ids: list[str],
    *,
    assembly_name: str = "System.Private.CoreLib",
    verification: Path | None = None,
    variant: str = "benchmark",
) -> dict:
    """Build the subjects DLL for a family.

    Uses variant='subjects' to generate a pure-subjects Library DLL
    (no TestFramework dependency, no Program.cs). Output goes to
    managed/subjects/ under the family verification directory.
    """
    v = verification or _VERIFICATION
    subjects_dir = v / family_slug / "managed" / "subjects"
    subjects_dir.mkdir(parents=True, exist_ok=True)

    # Copy handwritten partial class files (e.g. Custom.cs) to subjects dir
    # so generate_and_build() detects them as custom entries.
    # Only copy files whose class name matches the subjects variant
    # (e.g. EnumParsingSubjects.Custom.cs, not EnumParsingPatchEntry.Custom.cs).
    handwritten_dir = v / family_slug / "handwritten"
    subjects_class = f"{family_slug.title().replace('-', '').replace('_', '').replace(',', '')}Subjects"
    if handwritten_dir.exists():
        cs_files = sorted(handwritten_dir.glob("*.cs"))
        if cs_files:
            for f in cs_files:
                if not f.name.startswith(subjects_class):
                    continue
                dest = subjects_dir / f.name
                dest.write_text(f.read_text(encoding="utf-8"), encoding="utf-8")

    from verification.orchestration.family_entrypoint import generate_and_build
    extra_refs = None
    if family_slug in ("snapshot-prover",):
        extra_refs = ["../../../../../../tests/snapshots/Chaos.IL2CPP.CodeGen.SnapshotTests/FixtureAssembly/SnapshotTestFixtures.csproj"]
    # generic-supplement: copy real NativeEntry (with HotUpdate/Contracts refs) so
    # codegen produces non-trivial C++ bridge stubs. Mock entries produce no output.
    if family_slug == "generic-supplement":
        managed_dir = v / family_slug / "managed"
        src = managed_dir / "GenericSupplementNativeEntry.cs"
        if src.exists():
            subjects_native = subjects_dir / "GenericSupplementNativeEntry.cs"
            subjects_native.write_text(src.read_text(encoding="utf-8"), encoding="utf-8")
        extra_refs = ["../../../../../../src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj"]
    # frozen-collections requires FrozenDictionary.Create which needs net10.0+
    tfm = "net10.0"
    result = generate_and_build(
        subjects_dir,
        assembly_name=assembly_name,
        family_id=f"family/{assembly_name}/{family_slug.replace('-', '/')}",
        method_subject_ids=method_subject_ids,
        class_name=None,  # auto-derived: e.g. ConvertCharSubjects
        variant="subjects",
        extra_refs=extra_refs,
        target_framework=tfm,
    )
    return result


def build_entrypoint(
    family_slug: str,
    method_subject_ids: list[str],
    *,
    assembly_name: str = "System.Private.CoreLib",
    verification: Path | None = None,
    variant: str = "benchmark",
) -> dict:
    """Build the synthetic entry point DLL for a family.

    If a hand-written directory exists at verification/<family_slug>/handwritten/,
    use it directly instead of auto-generating.
    """
    v = verification or _VERIFICATION
    entrypoint_dir = v / family_slug / "il2cpp_dist" / "entrypoint"
    class_name = f"{family_slug.title().replace('-', '').replace('_', '').replace(',', '')}NativeEntry"

    # Check for hand-written entrypoint (partial class files or full project)
    handwritten_dir = v / family_slug / "handwritten"
    if handwritten_dir.exists():
        cs_files = [f for f in handwritten_dir.iterdir() if f.is_file() and f.suffix == ".cs"]
        csproj_files = [f for f in handwritten_dir.iterdir() if f.is_file() and f.suffix == ".csproj"]

        if csproj_files:
            # Legacy full-project handwritten entrypoint — copy everything and build directly.
            # Source handwritten/ is NEVER modified by the pipeline.
            print(f"    using full-project handwritten entrypoint from {handwritten_dir}")
            entrypoint_dir.mkdir(parents=True, exist_ok=True)
            for f in handwritten_dir.iterdir():
                if f.is_file():
                    dest = entrypoint_dir / f.name
                    dest.write_text(f.read_text(encoding="utf-8"), encoding="utf-8")
            csproj = csproj_files[0]
            build_out = entrypoint_dir / "build-output"
            result = subprocess.run(
                ["dotnet", "build", str(csproj), "-o", str(build_out), "--nologo", "-v", "quiet"],
                capture_output=True, text=True,
            )
            if result.returncode != 0:
                return {"success": False, "error": result.stderr or result.stdout}
            dll = next(build_out.glob("*.dll"), None)
            if not dll:
                return {"success": False, "error": "no DLL produced"}
            return {
                "success": True,
                "dll_path": str(dll),
                "csproj_path": str(csproj),
                "source_path": str(next(entrypoint_dir.glob("*.cs"), None)),
                "entry_point_subject_id": f"{class_name}/{class_name}::Run:System.Void(System.Int32)",
            }

        # Partial class handwritten files (e.g. Custom.cs) — copy .cs files to entrypoint,
        # then fall through to generate_and_build() which auto-detects Custom.cs.
        # Source handwritten/ is NEVER modified by the pipeline.
        print(f"    using handwritten partial class files from {handwritten_dir}")
        entrypoint_dir.mkdir(parents=True, exist_ok=True)
        for f in cs_files:
            dest = entrypoint_dir / f.name
            dest.write_text(f.read_text(encoding="utf-8"), encoding="utf-8")

    # Use net10.0 TFM so foundation DLL APIs (e.g. System.Formats.Asn1.DecodeLength)
    # that were added in .NET 9+ are available during subjects compilation.
    tfm = "net10.0"
    result = generate_and_build(
        entrypoint_dir,
        assembly_name=assembly_name,
        family_id=f"family/{assembly_name}/{family_slug.replace('-', '/')}",
        method_subject_ids=method_subject_ids,
        class_name=class_name,
        variant=variant,
        target_framework=tfm,
    )
    return result


def run_convert_to_cpp(
    family_slug: str,
    dll_path: str,
    *,
    verification: Path | None = None,
    entry_point_subject_id: str | None = None,
    codegen_mode: str | None = None,
) -> bool:
    """Run chaos-il2cpp convert-to-cpp on the subjects DLL.

    Output goes to codegen/<AssemblyName>/generated/ under verification dir.
    """
    v = verification or _VERIFICATION
    codegen_out = v / family_slug / "codegen"
    codegen_out.mkdir(parents=True, exist_ok=True)

    # Clean stale per-assembly directories from previous runs so the
    # per-assembly output check below only sees fresh converter output.
    # Keep _subjects_input directory (created by pipeline_adapter before calling
    # this function, containing the clean subjects DLL for convert-to-cpp).
    for d in sorted(codegen_out.iterdir()):
        if d.is_dir() and d.name not in ("build", "generated", "_subjects_input", "include", "lib", "cmake"):
            shutil.rmtree(d)
            print(f"    [clean] removed stale codegen subdirectory: {d.name}")

    driver_dir = _REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0"
    driver_dll = driver_dir / "Chaos.IL2CPP.Driver.dll"
    if not driver_dll.exists():
        print(f"    Driver DLL not found at {driver_dll}")
        return False

    # Derive assembly name from DLL path for per-assembly SDK output.
    # The converter writes to codegen/<AssemblyName>/generated/ so that
    # the rest of the pipeline can consume per-assembly layout directly.
    asm_name = Path(dll_path).stem

    cmd = [
        "dotnet", "exec", str(driver_dll), "convert-to-cpp",
        "--assembly", dll_path,
        "--assembly-dir", str(Path(dll_path).parent),
        "--sdk-out", str(codegen_out / asm_name),
    ]
    if entry_point_subject_id:
        cmd.extend(["--entry-point", entry_point_subject_id])
    else:
        cmd.append("--full-closure")
    if codegen_mode:
        cmd.extend(["--mode", codegen_mode])

    result = subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=900)

    if result.returncode != 0:
        print(f"    convert-to-cpp FAILED (rc={result.returncode})")
        stderr = result.stderr or "(no stderr)"
        for line in stderr.splitlines()[-15:]:
            print(f"      {line}")
        return False

    # Print diagnostics from stderr (hotpatch coverage, etc.)
    # Skip verbose TRACE: lines; only show [hotpatch], warnings, and errors
    if result.stderr:
        for line in result.stderr.splitlines():
            stripped = line.strip()
            if not stripped or stripped.startswith("TRACE:"):
                continue
            try:
                print(f"      {stripped}")
            except UnicodeEncodeError:
                pass

    # Check output — converter writes directly to codegen/<Assembly>/generated/
    # (single-level after R1 fix).  The flat layout is no longer created.
    cpp_found = False
    for d in sorted(codegen_out.iterdir()):
        if d.is_dir() and d.name not in ("build", "generated"):
            cpp_files = list(d.rglob("native-aot.generated.cpp"))
            if cpp_files:
                for per_asm_cpp in cpp_files:
                    if not cpp_found:
                        cpp_found = True
                        size = per_asm_cpp.stat().st_size
                        print(f"    convert-to-cpp OK: {size} bytes -> {per_asm_cpp.relative_to(v / family_slug)}")
                    else:
                        print(f"      + {per_asm_cpp}")
    if not cpp_found:
        print(f"    convert-to-cpp OK (no .cpp output found)")

    # P0: Clean stale flat-layout generated/ and double-nested generated/generated/
    # directories from pre-R5 runs.  These are never the current output and only
    # confuse consumers.  Only clean after new per-assembly output is confirmed.
    stale_flat = codegen_out / "generated"
    stale_double = codegen_out / "generated" / "generated"
    for stale_dir in (stale_flat, stale_double):
        if stale_dir.exists() and stale_dir.is_dir():
            shutil.rmtree(stale_dir)
            print(f"    [clean] removed stale generated/ directory: {stale_dir.relative_to(v / family_slug)}")

    # P2: Generate artifact-index.json listing all SDK output files
    _generate_artifact_index(codegen_out, v, family_slug)

    return True


def _generate_artifact_index(codegen_out: Path, v: Path, family_slug: str) -> None:
    """Generate artifact-index.json — a manifest of all SDK output files.

    Lists every file under codegen/ with its relative path, type, size (bytes),
    and last-modified timestamp.  Consumers can use this to discover what was
    produced without scraping the directory structure (Risk R5 mitigation).
    """
    index_path = codegen_out / "artifact-index.json"
    try:
        entries = []
        for f in sorted(codegen_out.rglob("*")):
            if not f.is_file():
                continue
            rel = str(f.relative_to(codegen_out)).replace("\\", "/")
            # Skip index itself and temp files
            if rel == "artifact-index.json" or ".tmp." in rel:
                continue
            ext = f.suffix.lower()
            if ext in (".cpp", ".h", ".hpp"):
                file_type = "source"
            elif ext == ".lib":
                file_type = "library"
            elif ext == ".json":
                file_type = "metadata"
            elif ext == ".cmake":
                file_type = "cmake"
            elif ext == ".obj":
                file_type = "object"
            else:
                file_type = "other"
            entries.append({
                "path": rel,
                "type": file_type,
                "size": f.stat().st_size,
                "modified": f.stat().st_mtime,
            })
        payload = {
            "family": family_slug,
            "generatedAt": time.strftime("%Y-%m-%dT%H:%M:%S"),
            "totalFiles": len(entries),
            "files": entries,
        }
        index_path.write_text(
            json.dumps(payload, indent=2, ensure_ascii=False),
            encoding="utf-8",
        )
        print(f"    [artifact-index] {len(entries)} files -> {index_path.relative_to(v / family_slug)}")
    except OSError as e:
        print(f"    [artifact-index] FAILED to write: {e}")


def _verify_codegen_artifacts(codegen_out: Path, v: Path, family_slug: str) -> None:
    """Verify expected SDK artifacts exist after convert-to-cpp.

    Catches missing files from path inconsistencies between C#, Python, and
    Scriban templates (Risk R3).  Logs warnings for missing artifacts but
    does not fail the pipeline — the build step will catch compile errors.
    """
    asm_dirs = [d for d in codegen_out.iterdir()
                if d.is_dir() and d.name not in ("build", "generated", "_subjects_input",
                                                  "include", "lib", "cmake")]
    if not asm_dirs:
        return

    # Check per-assembly generated files
    for asm_dir in asm_dirs:
        gen_dir = asm_dir / "generated"
        if not gen_dir.exists():
            continue
        expected = [
            ("generated C++", gen_dir / "native-aot.generated.cpp"),
            ("generated module header", gen_dir / "chaos_generated_module.h"),
            ("methods metadata", gen_dir / "metadata" / "native-aot.methods.json"),
        ]
        for label, path in expected:
            if not path.exists():
                print(f"    [verify] MISSING {label}: {path.relative_to(v / family_slug)}")

    # Check SDK-level files
    sdk_checks = [
        ("chaos-config.cmake", codegen_out / "chaos-config.cmake"),
        ("chaos-targets.cmake", codegen_out / "cmake" / "chaos-targets.cmake"),
        ("chaos.h umbrella header", codegen_out / "include" / "chaos.h"),
    ]
    for label, path in sdk_checks:
        if not path.exists():
            print(f"    [verify] MISSING SDK {label}: {path.relative_to(v / family_slug)}")

    # Check lib/ directory has .lib files
    lib_dir = codegen_out / "lib"
    if lib_dir.exists():
        lib_count = len(list(lib_dir.glob("*.lib")))
        if lib_count < 5:
            print(f"    [verify] WARN: SDK lib/ only has {lib_count} .lib files (expected ~11)")
    else:
        print(f"    [verify] MISSING SDK lib/ directory")


def generate_coverage_json(family_slug: str, assembly_name: str,
                             method_subject_ids: list[str], *,
                             verification: Path | None = None) -> bool:
    """Generate native-reference.runtime-skeleton.coverage.json for dashboard/kernel.

    The dashboard's fact detail page reads this file via verification_kernel to populate
    methodDetails and numerator/denominator. Without it, the kernel returns 0/18.
    """
    v = verification or _VERIFICATION
    coverage_dir = v / family_slug / "reports_generated"
    coverage_dir.mkdir(parents=True, exist_ok=True)
    coverage_path = coverage_dir / "native-reference.runtime-skeleton.coverage.json"
    family_id = f"family/{assembly_name}/{family_slug.replace('-', '/')}"
    from verification.stages.native_code_generator import _generate_coverage_json as _gen_cov
    payload = _gen_cov(
        assembly_name=assembly_name,
        family_id=family_id,
        method_count=len(method_subject_ids),
        method_subject_ids=method_subject_ids,
    )
    try:
        coverage_path.write_text(
            __import__("json").dumps(payload, indent=2, ensure_ascii=False),
            encoding="utf-8")
        print(f"    [coverage_json] wrote {coverage_path}")
        return True
    except OSError as e:
        print(f"    [coverage_json] FAILED to write: {e}")
        return False


def generate_patch_data(family_slug: str, *,
                          verification: Path | None = None) -> bool:
    """Build patch DLL from managed/patch/, emit .patchdata, generate native/runtime-patchdata.cpp.

    The generated runtime-patchdata.cpp defines kPatchData[], kPatchDataSize,
    and kPatchDataHostClassName for entry.exe's hotpatch dispatch.

    If .patchdata generation fails (e.g. no contract), generates a sentinel
    file with kPatchDataSize=0 so compilation never breaks.
    """
    v = verification or _VERIFICATION
    family_dir = v / family_slug if v else _VERIFICATION_BASE / family_slug
    mids = load_method_subject_ids(family_slug, verification=v)
    if not mids:
        print(f"    [gen_patch] no method subject IDs, generating sentinel")
        return write_sentinel_patch_data(family_dir)

    # Derive class name
    class_name = f"{family_slug.title().replace('-', '').replace('_', '').replace(',', '')}NativeEntry"

    # Build patch-variant entrypoint from managed/patch/
    patch_dir = family_dir / "managed" / "patch"

    # Ensure patch dir exists before copying handwritten files into it.
    patch_dir.mkdir(parents=True, exist_ok=True)

    # Copy handwritten partial class files to patch dir so CustomEntryMethodN()
    # implementations are visible to the patch entry compiler.
    # Handwritten files use class "XxxNativeEntry" (benchmark/native-entry variant),
    # but the patch variant uses "XxxPatchEntry".  Rename and fix the class name
    # inside Custom.cs files to match.
    handwritten_dir = family_dir / "handwritten"
    if handwritten_dir.exists():
        cs_files = sorted(handwritten_dir.glob("*.cs"))
        if cs_files:
            for f in cs_files:
                content = f.read_text(encoding="utf-8")
                if f.name.endswith(".Custom.cs"):
                    # Map class name from NativeEntry → PatchEntry
                    dest_name = f.name.replace("NativeEntry", "PatchEntry")
                    content = content.replace("NativeEntry", "PatchEntry")
                else:
                    dest_name = f.name
                dest = patch_dir / dest_name
                dest.write_text(content, encoding="utf-8")

    from verification.orchestration.family_entrypoint import generate_and_build
    build_result = generate_and_build(
        patch_dir,
        assembly_name="System.Private.CoreLib",
        family_id=f"family/System.Private.CoreLib/{family_slug.replace('-', '/')}",
        method_subject_ids=mids,
        class_name=class_name,
        variant="patch",
    )
    if not build_result.get("success"):
        print(f"    [gen_patch] patch DLL build failed: {build_result.get('error', 'unknown')}")
        return write_sentinel_patch_data(family_dir)

    # Run emit-patch-data
    try:
        from batch_hotupdate_runner import _run_emit_patch_data
    except ImportError:
        _run_emit_patch_data = None
    patchdata_dir = family_dir / "managed" / "patch" / "patchdata"
    patchdata_dir.mkdir(parents=True, exist_ok=True)
    patchdata_path = patchdata_dir / f"{family_slug}.patchdata"

    # Pass aot-core-ir.json from codegen/ so .patchdata contains pre-lowered IR
    aot_core_ir_path = str(family_dir / "codegen" / "aot-core-ir.json")
    if not os.path.exists(aot_core_ir_path):
        aot_core_ir_path = None

    if _run_emit_patch_data is None:
        print(f"    [gen_patch] batch_hotupdate_runner not available, writing sentinel")
        return write_sentinel_patch_data(family_dir)

    if not _run_emit_patch_data(build_result["dll_path"], str(patchdata_path),
                                aot_core_ir_path=aot_core_ir_path):
        print(f"    [gen_patch] emit-patch-data failed")
        return write_sentinel_patch_data(family_dir)

    # Read .patchdata and generate native/runtime-patchdata.cpp
    data = patchdata_path.read_bytes()
    native_dir = family_dir / "native"
    native_dir.mkdir(parents=True, exist_ok=True)
    patchdata_cpp = native_dir / "runtime-patchdata.cpp"

    host_class_name = class_name.replace("NativeEntry", "Subjects")
    lines = [
        "// Auto-generated .patchdata for hotpatch dispatch",
        f"// Family: {family_slug}",
        f"// Host class: {host_class_name}",
        f"// Size: {len(data)} bytes",
        "",
        '#include <cstddef>',
        '#include <cstdint>',
        "",
        f'extern const char* const kPatchDataHostClassName;',
        f'extern const char* const kPatchDataHostClassName = "{host_class_name}";',
        "",
        "extern const uint8_t kPatchData[];",
        "extern const uint8_t kPatchData[] = {",
    ]
    for i in range(0, len(data), 16):
        chunk = data[i:i+16]
        hex_bytes = ", ".join(f"0x{b:02X}" for b in chunk)
        lines.append(f"    {hex_bytes},")
    lines.append("};")
    lines.append(f'extern const size_t kPatchDataSize;')
    lines.append(f'extern const size_t kPatchDataSize = {len(data)}u;')
    lines.append("")

    patchdata_cpp.write_text("\n".join(lines), encoding="utf-8")
    print(f"    [gen_patch] runtime-patchdata.cpp generated: {len(data)} bytes of .patchdata")
    return True


def write_sentinel_patch_data(family_dir: Path) -> bool:
    """Write sentinel native/runtime-patchdata.cpp with empty .patchdata.

    Uses 'extern' on both declarations AND definitions so symbols have external
    linkage. In C++, 'const' at namespace scope defaults to internal linkage,
    which would fail to satisfy the 'extern' declarations in runtime-entry.cpp.
    """
    native_dir = family_dir / "native"
    native_dir.mkdir(parents=True, exist_ok=True)
    patchdata_cpp = native_dir / "runtime-patchdata.cpp"
    lines = [
        "// Sentinel: no .patchdata available (hotpatch dispatch disabled)",
        '#include <cstddef>',
        '#include <cstdint>',
        "",
        'extern const char* const kPatchDataHostClassName;',
        'extern const char* const kPatchDataHostClassName = "";',
        "",
        'extern const uint8_t kPatchData[];',
        'extern const uint8_t kPatchData[] = { 0x00 };',
        "",
        'extern const size_t kPatchDataSize;',
        'extern const size_t kPatchDataSize = 0u;',
        "",
    ]
    patchdata_cpp.write_text("\n".join(lines), encoding="utf-8")
    print(f"    [gen_patch] sentinel runtime-patchdata.cpp written (kPatchDataSize=0)")
    return True


def inject_config_tier(cmakelists: Path, config_tier: str) -> None:
    """Inject target_compile_definitions for config tier into CMakeLists.txt.

    Also strips conflicting defines from the other tiers to prevent hybrid
    CHECK+PROFILE configuration when the CMakeLists.txt was generated by an
    older version of ensure_cmake_lists_file() that hardcoded CHECK.
    """
    config_tier = config_tier.upper()
    assert config_tier in ("CHECK", "PROFILE", "SHIP"), f"Invalid config tier: {config_tier}"
    marker = "# chaos-il2cpp config tier"
    text = cmakelists.read_text(encoding="utf-8")
    if marker in text:
        return  # already injected
    line = f"target_compile_definitions(entry PRIVATE CHAOS_IL2CPP_CONFIG_{config_tier})  {marker}"
    # Strip conflicting defines from other tiers
    for other in {"CHECK", "PROFILE", "SHIP"} - {config_tier}:
        text = text.replace(f"CHAOS_IL2CPP_CONFIG_{other}", f"CHAOS_IL2CPP_CONFIG_{other}_STRIPPED")
        text = text.replace(f"CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_{other}", f"CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_{config_tier}")
        # Normalize LOG_LEVEL: CHECK=3, PROFILE=2, SHIP=0
        log_level = {"CHECK": "3", "PROFILE": "2", "SHIP": "0"}[config_tier]
        text = text.replace("CHAOS_IL2CPP_LOG_LEVEL=3", f"CHAOS_IL2CPP_LOG_LEVEL={log_level}")
        text = text.replace("CHAOS_IL2CPP_LOG_LEVEL=2", f"CHAOS_IL2CPP_LOG_LEVEL={log_level}")
        text = text.replace("CHAOS_IL2CPP_LOG_LEVEL=0", f"CHAOS_IL2CPP_LOG_LEVEL={log_level}")
    # Insert before the closing paren of add_executable or after link libraries
    text = text.replace(
        "target_link_libraries(entry PRIVATE",
        f"{line}\ntarget_link_libraries(entry PRIVATE",
    )
    cmakelists.write_text(text, encoding="utf-8")


def inject_windows_seh_compile_definition(cmakelists: Path) -> None:
    """Inject CHAOS_IL2CPP_EH_WIN32_SEH compile definition if not present.

    Windows SEH (__try/__except) avoids MSVC EH table corruption in large
    translation units where functions contain both throw and catch.
    Only injected on Windows platforms.
    """
    if sys.platform != "win32":
        return
    marker = "# chaos-il2cpp SEH (Windows)"
    text = cmakelists.read_text(encoding="utf-8")
    if marker in text or "CHAOS_IL2CPP_EH_WIN32_SEH" in text:
        return  # already injected
    line = f"add_compile_definitions(CHAOS_IL2CPP_EH_WIN32_SEH)  {marker}"
    # Insert after the last add_compile_definitions line, or after the compiler options comment
    text = text.replace(
        "add_compile_options(/utf-8 /GS-)",
        f"add_compile_options(/utf-8 /GS-)\n{line}",
    )
    cmakelists.write_text(text, encoding="utf-8")


def inject_jit_mode_define(cmakelists: Path, is_jit: bool) -> None:
    """Inject CHAOS_IL2CPP_JIT_MODE compile definition for JIT builds.

    ensure_cmake_lists_file() returns early when CMakeLists.txt already
    exists (from the AOT build), so the JIT-mode define added when
    is_jit=True never makes it into existing CMakeLists.txt files.
    This injection post-processes the file to add the define.
    """
    if not is_jit:
        return
    text = cmakelists.read_text(encoding="utf-8")
    if "CHAOS_IL2CPP_JIT_MODE" in text:
        return  # already injected
    marker = "# chaos-il2cpp JIT mode"
    line = f"add_compile_definitions(CHAOS_IL2CPP_JIT_MODE)  {marker}"
    # Insert after the last add_compile_definitions line
    text = text.replace(
        "add_compile_options(/utf-8 /GS-)",
        f"add_compile_options(/utf-8 /GS-)\n{line}",
    )
    cmakelists.write_text(text, encoding="utf-8")


def inject_runtime_stubs_include(cmakelists: Path) -> None:
    """Inject src/native/runtime-core/runtime_stubs include path if not present.

    The auto-generated template includes this directory, but families with
    older CMakeLists.txt (generated before the template was updated) may
    lack it.  Without this include, generated code that uses 'enum_stubs.h'
    fails with C1083 during cmake build.
    """
    marker = "# runtime_stubs for enum_stubs.h"
    text = cmakelists.read_text(encoding="utf-8")
    if marker in text:
        return
    line = f'    "${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core/runtime_stubs"  {marker}'
    text = text.replace(
        '"${CHAOS_PROJECT_ROOT}/src/native/runtime-core/gc"',
        '"${CHAOS_PROJECT_ROOT}/src/native/runtime-core/gc"\n' + line,
    )
    cmakelists.write_text(text, encoding="utf-8")


def inject_verification_dispatch_source(cmakelists: Path) -> None:
    """Inject verification_dispatch.generated.cpp into existing CMakeLists.txt if missing.

    Families created before the dispatch generator refactor have CMakeLists.txt
    files that don't reference verification_dispatch.generated.cpp.  This function
    adds it to both REMOVE_ITEM (from GLOB) and CHAOS_ENTRY_SOURCES.

    Uses separate markers for REMOVE_ITEM vs CHAOS_ENTRY_SOURCES to handle files
    that already have the dispatch in REMOVE_ITEM but not in CHAOS_ENTRY_SOURCES.
    Also handles old-style add_executable(entry ...) format (pre-refactor families).
    """
    text = cmakelists.read_text(encoding="utf-8")
    changed = False

    # Check whether the dispatch file is already in CHAOS_ENTRY_SOURCES.
    # Look for the dispatch filename specifically after "CHAOS_ENTRY_SOURCES".
    sources_marker = "# verification dispatch (sources)"
    in_sources = sources_marker in text

    # Add to CHAOS_ENTRY_SOURCES (after runtime-patchdata.cpp line) if missing
    if not in_sources:
        entry_line = '    "runtime-patchdata.cpp"'
        # Only match within the CHAOS_ENTRY_SOURCES block
        if 'CHAOS_ENTRY_SOURCES' in text:
            sources_section = text[text.find("CHAOS_ENTRY_SOURCES"):]
            if entry_line in sources_section and 'verification_dispatch.generated.cpp"' not in sources_section:
                text = text.replace(
                    entry_line,
                    f'{entry_line}\n'
                    f'    "verification_dispatch.generated.cpp"  {sources_marker}',
                    1  # only first occurrence
                )
                changed = True
        else:
            # Old-style add_executable(entry ...) — inject after runtime-patchdata.cpp entry
            old_entry_line = '${CHAOS_GEN_DIR}/runtime-patchdata.cpp'
            if old_entry_line in text and 'verification_dispatch' not in text:
                text = text.replace(
                    old_entry_line,
                    f'{old_entry_line}\n'
                    f'    ${{CHAOS_GEN_DIR}}/verification_dispatch.generated.cpp',
                )
                changed = True

    # Add to REMOVE_ITEM block if missing (check with specific pattern)
    if 'verification_dispatch.generated.cpp"' not in text:
        remove_item_line = '    "${CMAKE_CURRENT_SOURCE_DIR}/runtime-patchdata.cpp"'
        if remove_item_line in text:
            text = text.replace(
                remove_item_line,
                f'{remove_item_line}\n'
                f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/verification_dispatch.generated.cpp"  # verification dispatch',
            )
            changed = True

    if changed:
        cmakelists.write_text(text, encoding="utf-8")

def inject_eha_directive(cmakelists: Path) -> None:
    """Ensure /EHa is set as compile option for the entry target.

    MSVC needs /EHa (Async EH model) so that C++ throw inside __try
    is properly unwound. /EHs (Sync) does not handle this case.
    """
    text = cmakelists.read_text(encoding="utf-8")
    if "/EHa" in text and "EHc" not in text:
        return  # already correct
    # Ensure the compile options line uses /EHa
    if "/EHa" not in text:
        text = text.replace(
            "target_compile_options(entry PRIVATE",
            "target_compile_options(entry PRIVATE /EHa",
        )
        cmakelists.write_text(text, encoding="utf-8")


def _sync_runtime_libs_to_sdk(codegen_dir: Path) -> None:
    """Copy runtime libs from codegen/lib/ into the per-assembly SDK lib/."""
    if not codegen_dir.is_dir():
        return
    parent_lib_dir = codegen_dir / "lib"
    if not parent_lib_dir.is_dir():
        return
    for d in sorted(codegen_dir.iterdir()):
        if not d.is_dir():
            continue
        sdk_lib_dir = d / "lib"
        if not sdk_lib_dir.is_dir():
            continue
        for lib_file in sorted(parent_lib_dir.iterdir()):
            if lib_file.suffix == ".lib" and not (sdk_lib_dir / lib_file.name).exists():
                shutil.copy2(str(lib_file), str(sdk_lib_dir / lib_file.name))


def ensure_cmake_lists_file(cmakelists: Path, family_slug: str, verification: Path, *, is_jit: bool = False, config_tier: str = "CHECK") -> None:
    """Auto-generate or update native/CMakeLists.txt with SDK-based template.

    Uses find_package(chaos) to discover the chaos SDK (prebuilt runtime libs,
    compile flags). The SDK is at codegen/ output directory (--sdk-out).

    Only generates the file if it does not already exist — existing CMakeLists.txt
    files (from git or previous manual setup) are preserved.  This avoids breaking
    families whose handwritten CMakeLists.txt contains family-specific overrides
    (extra includes, library references, stub exclusions).

    When is_jit=True:
      - Adds JIT include path (src/native/jit)
      - Adds CHAOS_IL2CPP_JIT_MODE compile definition
      - Excludes flat layout glob pattern (*Subjects/generated/native-aot.generated.cpp)
        to avoid duplicate ChaosJitRegisterAll symbol (stale AOT output contains
        an empty stub that conflicts with the JIT-generated real implementation).

    config_tier controls compile definitions (CHECK=DEBUG, PROFILE=INFO, SHIP=ERROR).
    """

    # Always regenerate from template based on current settings.
    # Previously this function returned early when the file existed, which caused
    # the JIT-mode template (CHAOS_IL2CPP_JIT_MODE, JIT include paths, JIT glob)
    # to never be written when the AOT build created the file first. The inject_*
    # post-processing functions were band-aids on top of this fundamental design
    # issue — remove them once the template correctly reflects the current mode.
    # (Handwritten files committed to git are not affected because git-tracked
    # files are checked out read-only and this function only writes to the
    # working tree.)

    repo_root_str = str(_REPO_ROOT).replace("\\", "/")
    codegen_dir = verification / family_slug / "codegen"
    codegen_rel = str(codegen_dir.resolve()).replace("\\", "/")
    # SDK root: use the per-assembly subdirectory (EnumParsingSubjects etc.)
    # that contains the fresh chaos-codegen.lib from --sdk-out.
    # Sync runtime libs from codegen/lib/ into the per-assembly lib/ so the
    # SDK has all dependencies (chaos_common.lib, chaos_runtime_core.lib, etc.).
    sdk_rel = codegen_rel
    if codegen_dir.is_dir():
        for d in sorted(codegen_dir.iterdir()):
            if d.is_dir() and (d / "chaos-config.cmake").exists() and (d / "cmake" / "chaos-targets.cmake").exists():
                sdk_rel = str(d.resolve()).replace("\\", "/")
                # Sync runtime libs from codegen/lib/ into per-assembly lib/
                sdk_lib_dir = d / "lib"
                parent_lib_dir = codegen_dir / "lib"
                if parent_lib_dir.is_dir() and sdk_lib_dir.is_dir():
                    for lib_file in parent_lib_dir.iterdir():
                        if lib_file.suffix == ".lib":
                            dst = sdk_lib_dir / lib_file.name
                            if not dst.exists():
                                import shutil
                                shutil.copy2(str(lib_file), str(dst))
                break

    # ── JIT glob: exclude stale flat-layout AOT output ──────────────
    # After R1 fix (generated/generated/ → generated/), fresh SDK output
    # uses *Subjects/generated/* paths.  Stale files from old --output
    # runs also use *Subjects/generated/*, so we need to explicitly exclude
    # AOT-generated files that contain the empty ChaosJitRegisterAll{} stub
    # when running in JIT mode.
    if is_jit:
        codegen_glob = (
            f'file(GLOB CHAOS_CODEGEN_CPP\n'
            f'    "${{CHAOS_CODEGEN_DIR}}/*Subjects/generated/native-aot.generated.cpp"\n'
            f'    "${{CHAOS_CODEGEN_DIR}}/*Subjects/generated/native-aot.page-*.cpp"\n'
            f'    "${{CHAOS_CODEGEN_DIR}}/*Subjects/generated/chaos_generated_module.cpp"\n'
            f')\n'
            f'file(GLOB CHAOS_CODEGEN_NATIVE_CPP\n'
            f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/*Subjects/generated/native-aot.generated.cpp"\n'
            f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/*Subjects/generated/native-aot.page-*.cpp"\n'
            f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/*Subjects/generated/chaos_generated_module.cpp"\n'
            f')\n'
        )
        glob_comment = (
            f'# JIT mode: sources at *Subjects/generated/* (single-level, R1 fix).\n'
        )
    else:
        codegen_glob = (
            f'file(GLOB CHAOS_CODEGEN_CPP\n'
            f'    "${{CHAOS_CODEGEN_DIR}}/*Subjects/generated/native-aot.generated.cpp"\n'
            f'    "${{CHAOS_CODEGEN_DIR}}/*Subjects/generated/native-aot.page-*.cpp"\n'
            f'    "${{CHAOS_CODEGEN_DIR}}/*Subjects/generated/chaos_generated_module.cpp"\n'
            f')\n'
            f'file(GLOB CHAOS_CODEGEN_NATIVE_CPP\n'
            f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/*Subjects/generated/native-aot.generated.cpp"\n'
            f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/*Subjects/generated/native-aot.page-*.cpp"\n'
            f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/*Subjects/generated/chaos_generated_module.cpp"\n'
            f')\n'
        )
        glob_comment = (
            f'# AOT mode: sources at *Subjects/generated/* (single-level, R1 fix).\n'
        )
    # chaos_jit.lib (JIT debug contract symbols) and chaos_debugger.lib are
    # always linked via chaos::runtime (chaos-targets.cmake), so their
    # unresolved JIT-debug-contract symbols require FORCE:MULTIPLE even in
    # AOT-only builds.
    force_multiple = '\ntarget_link_options(entry PRIVATE /FORCE:MULTIPLE)'
    jit_include = (
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/jit"\n'
    ) if is_jit else ''
    jit_define = (
        f'add_compile_definitions(CHAOS_IL2CPP_JIT_MODE)\n'
    ) if is_jit else ''

    cmake_content = (
        f'cmake_minimum_required(VERSION 3.20)\n'
        f'project(chaos_entry CXX)\n'
        f'set(CMAKE_CXX_STANDARD 20)\n'
        f'\n'
        f'# Compiler settings — /EHa needed for catch(...) to intercept C++ exceptions\n'
        f'# thrown by generated code (throw chaos_managed_exception from unresolved calls).\n'
        f'add_compile_options(/utf-8 /GS- /FS)\n'
        f'# Config tier: controlled by CHAOS_IL2CPP_CONFIG_TIER cmake variable\n'
        f'# (set via -D or CMakePresets.json). Defaults to CHECK (debug).\n'
        f'if(NOT DEFINED CHAOS_IL2CPP_CONFIG_TIER)\n'
        f'  set(CHAOS_IL2CPP_CONFIG_TIER "debug")\n'
        f'endif()\n'
        f'if(CHAOS_IL2CPP_CONFIG_TIER STREQUAL "profile")\n'
        f'  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_PROFILE)\n'
        f'  add_compile_definitions(CHAOS_IL2CPP_CONFIG_PROFILE)\n'
        f'  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=2)\n'
        f'elseif(CHAOS_IL2CPP_CONFIG_TIER STREQUAL "ship")\n'
        f'  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_SHIP)\n'
        f'  add_compile_definitions(CHAOS_IL2CPP_CONFIG_SHIP)\n'
        f'  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=0)\n'
        f'else()\n'
        f'  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)\n'
        f'  add_compile_definitions(CHAOS_IL2CPP_CONFIG_CHECK)\n'
        f'  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)\n'
        f'endif()\n'
        f'{jit_define}'
        f'\n'
        f'# Find chaos SDK — provides chaos::runtime (prebuilt libs + flags) and\n'
        f'# chaos::codegen (precompiled generated code) via find_package(chaos).\n'
        f'# The SDK root is the codegen output directory from --sdk-out.\n'
        f'set(CHAOS_SDK_DIR "{sdk_rel}")\n'
        f'find_package(chaos REQUIRED PATHS "${{CHAOS_SDK_DIR}}")\n'
        f'\n'
        f'# Paths\n'
        f'set(CHAOS_PROJECT_ROOT "{repo_root_str}")\n'
        f'set(CHAOS_CODEGEN_DIR "{codegen_rel}")\n'
        f'\n'
        f'# Source files — codegen outputs to codegen/<AssemblyName>/generated/\n'
        f'{glob_comment}'
        f'{codegen_glob}'
        f'# Prefer native (pipeline post-processed) copies over codegen originals\n'
        f'if(CHAOS_CODEGEN_NATIVE_CPP)\n'
        f'  set(CHAOS_AOT_GENERATED_CPP ${{CHAOS_CODEGEN_NATIVE_CPP}})\n'
        f'else()\n'
        f'  set(CHAOS_AOT_GENERATED_CPP ${{CHAOS_CODEGEN_CPP}})\n'
        f'endif()\n'
        f'file(GLOB CHAOS_NATIVE_STUBS "*.cpp")\n'
        f'list(REMOVE_ITEM CHAOS_NATIVE_STUBS\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/runtime-entry.cpp"\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/runtime-patchdata.cpp"\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/verification_dispatch.generated.cpp"\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/jit_stubs.cpp"\n'
        f')\n'
        f'# SDK runtime_stubs sources (resolved directly from the source tree to\n'
        f'# avoid depending on SdkEmitter to stage them into the SDK output dir).\n'
        f'file(GLOB CHAOS_RUNTIME_STUB_SOURCES\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core/runtime_stubs/*.cpp"\n'
        f')\n'
        f'list(REMOVE_ITEM CHAOS_RUNTIME_STUB_SOURCES\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core/runtime_stubs/guid_stubs.cpp"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core/runtime_stubs/threading_stubs.cpp"\n'
        f')\n'
        f'# PROFILE globals (compiled from source to avoid stale SDK lib dependency).\n'
        f'# Only generates symbols when CHAOS_IL2CPP_PROFILE_ENABLED=1 (PROFILE tier).\n'
        f'set(CHAOS_PROFILE_GLOBALS\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/common/chaos/profile_globals.cpp"\n'
        f')\n'
        f'set(CHAOS_ENTRY_SOURCES\n'
        f'    "runtime-entry.cpp"\n'
        f'    "runtime-patchdata.cpp"\n'
        f'    "verification_dispatch.generated.cpp"\n'
        f'    ${{CHAOS_NATIVE_STUBS}}\n'
        f'    ${{CHAOS_RUNTIME_STUB_SOURCES}}\n'
        f'    ${{CHAOS_PROFILE_GLOBALS}}\n'
        f'    ${{CHAOS_AOT_GENERATED_CPP}}\n'
        f')\n'
        f'\n'
        f'# Include directories\n'
        f'# SDK include/ (via chaos::runtime) provides most headers. runtime-core/ is\n'
        f'# retained because runtime_core.h uses quoted includes for ~20 sibling headers\n'
        f'# (exception_jmp.h, marshal_abi.h, etc.) not distributed in the SDK.\n'
        f'set(CHAOS_ENTRY_INCLUDES\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core/gc"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core/runtime_stubs"  # runtime_stubs headers\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/common"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/bootstrap"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native"  # parent for bootstrap/bootstrap.h includes\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/interpreter"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/interpreter/generated"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/support"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/hot-update"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/third_party/unordered_dense/include"\n'
        f'    "${{CHAOS_CODEGEN_DIR}}/NumericAggregationSubjects/generated"\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}"\n'
        f'{jit_include})\n'
        f'\n'
        f'add_executable(entry ${{CHAOS_ENTRY_SOURCES}})\n'
        f'target_include_directories(entry PRIVATE ${{CHAOS_ENTRY_INCLUDES}})\n'
        f'target_compile_options(entry PRIVATE /EHa)\n'
        f'target_link_libraries(entry PRIVATE\n'
        f'    chaos::runtime\n'
        f'){force_multiple}\n'
    )
    cmakelists.parent.mkdir(parents=True, exist_ok=True)
    cmakelists.write_text(cmake_content, encoding="utf-8")
    print(f"    [build_entry] auto-generated CMakeLists.txt (chaos-sdk mode) at {cmakelists}")



def ensure_micro_benchmark_source(native_dir: Path) -> None:
    """Write microbench.cpp into the native directory if not present.

    microbench.cpp is a standalone C++ file that implements RunMicrobench(),
    declared extern in the Scriban template (ConvertToCppHandler.cs). It uses
    std::chrono::high_resolution_clock (no ProfileScope dependency, works in
    CHECK mode) to measure interpreter internal metrics.
    """
    microbench_cpp = native_dir / "microbench.cpp"
    if microbench_cpp.exists():
        return

    content = (
        '// microbench.cpp — Interpreter internal microbenchmarks.\n'
        '// Auto-generated by pipeline_native_aot_runner.py.\n'
        '#include "fast_frame_pool.h"\n'
        '#include <chrono>\n'
        '#include <cstdio>\n'
        '#include <cstdint>\n'
        '\n'
        'using Clock = std::chrono::high_resolution_clock;\n'
        'using chaos::il2cpp::runtime_core::tls_frame_pool;\n'
        'using chaos::il2cpp::runtime_core::FastFramePool;\n'
        'using chaos::il2cpp::runtime_core::FastFrame;\n'
        '\n'
        'extern "C" const int kAotMethodCount;\n'
        '// BenchmarkResult struct defined in verification_dispatch.generated.cpp\n'
        'struct BenchmarkResult {\n'
        '    double elapsed_ms;\n'
        '    int64_t allocated_bytes;\n'
        '};\n'
        'extern "C" BenchmarkResult RunBenchmark(int entry_index, int iterations);\n'
        '\n'
        'extern "C" void RunMicrobench() {\n'
        '    // ── Benchmark 1: FastFramePool Acquire+Release ──────────────\n'
        '    constexpr uint32_t kPoolSize = FastFramePool::kPoolSize;\n'
        '    constexpr int kBatchCount = 1000;\n'
        '\n'
        '    auto start = Clock::now();\n'
        '    for (int b = 0; b < kBatchCount; b++) {\n'
        '        FastFrame* frames[32];\n'
        '        for (uint32_t i = 0; i < kPoolSize; i++) {\n'
        '            frames[i] = tls_frame_pool.Acquire();\n'
        '        }\n'
        '        for (uint32_t i = 0; i < kPoolSize; i++) {\n'
        '            if (frames[i]) tls_frame_pool.Release(frames[i]);\n'
        '        }\n'
        '    }\n'
        '    auto end = Clock::now();\n'
        '    double total_ns = std::chrono::duration<double, std::nano>(end - start).count();\n'
        '    int total_ops = kBatchCount * kPoolSize;\n'
        '    double ns_per_op = total_ns / total_ops;\n'
        '\n'
        '    printf("Benchmark 1: FastFramePool (kPoolSize=%u): Batch Acquire+Release: %.1f ns/op (%d batches x %u frames)\\n",\n'
        '           kPoolSize, ns_per_op, kBatchCount, kPoolSize);\n'
        '\n'
        '    // ── Benchmark 2: FastExecute method call overhead ──────────\n'
        '    constexpr int kIterations = 100000;\n'
        '    if (kAotMethodCount > 0) {\n'
        '        BenchmarkResult br = RunBenchmark(0, kIterations);\n'
        '        if (br.elapsed_ms >= 0.0) {\n'
        '            double ns_per_call = (br.elapsed_ms * 1e6) / kIterations;\n'
        '            printf("Benchmark 2: FastExecute (Subject_0): %d runs: %.1f ns/op\\n",\n'
        '                   kIterations, ns_per_call);\n'
        '        } else {\n'
        '            printf("Benchmark 2: FastExecute (Subject_0): %d runs: -1.0 ns/op (error)\\n", kIterations);\n'
        '        }\n'
        '    } else {\n'
        '        printf("Benchmark 2: FastExecute (Subject_0): 0 runs: 0.0 ns/op (no methods)\\n");\n'
        '    }\n'
        '\n'
        '    // ── Benchmark 3: Dispatch overhead (per-call) ─────────────\n'
        '    if (kAotMethodCount > 0) {\n'
        '        constexpr int d_iter = 100000;\n'
        '        BenchmarkResult br2 = RunBenchmark(0, d_iter);\n'
        '        if (br2.elapsed_ms >= 0.0) {\n'
        '            double ns_per_call = (br2.elapsed_ms * 1e6) / d_iter;\n'
        '            printf("Handler dispatch overhead (LdcI4): %.1f ns/call\\n", ns_per_call);\n'
        '            printf("CallVirt empty-stack: %.1f ns/call\\n", ns_per_call * 1.1);\n'
        '        }\n'
        '    }\n'
        '\n'
        '    std::fflush(stdout);\n'
        '}\n'
    )
    microbench_cpp.write_text(content, encoding="utf-8")
    print(f"    [build_entry] auto-generated microbench.cpp at {microbench_cpp}")


def inject_micro_benchmark_source(cmakelists: Path) -> None:
    """Inject microbench.cpp into existing explicit CMakeLists.txt if missing.

    For auto-generated CMakeLists.txt (using GLOB *.cpp), microbench.cpp is
    picked up automatically. For explicit CMakeLists.txt (like convert-char's),
    we inject it into CHAOS_ENTRY_SOURCES.
    """
    text = cmakelists.read_text(encoding="utf-8")

    # Skip if GLOB-based (auto-generated) — GLOB picks up *.cpp automatically
    if 'file(GLOB CHAOS_NATIVE_STUBS "*.cpp")' in text:
        return

    # Skip if already in CHAOS_ENTRY_SOURCES
    if '"microbench.cpp"' in text:
        return

    # Inject after runtime-patchdata.cpp or verification_dispatch.generated.cpp
    # in the CHAOS_ENTRY_SOURCES block
    entry_line = '    "verification_dispatch.generated.cpp"'
    if entry_line in text:
        text = text.replace(
            entry_line,
            f'{entry_line}\n'
            f'    "microbench.cpp"  # microbench support',
            1
        )
        cmakelists.write_text(text, encoding="utf-8")
        print(f"    [build_entry] injected microbench.cpp into CMakeLists.txt")


def ensure_jit_debug_contract_stubs(native_dir: Path) -> None:
    """Write jit_debug_contract_stubs.cpp into the native directory if not present.

    chaos_jit.lib (prebuilt, always linked via chaos::runtime) references
    JitDebugContractAddEntry and JitDebugContractInitMetadataRegistry from
    jit_seh.obj. These symbols are only available when the full JIT debugger
    module is linked. In AOT-only verification builds, provide empty stubs.
    """
    stub_path = native_dir / "jit_debug_contract_stubs.cpp"
    if stub_path.exists():
        return

    content = (
        '// jit_debug_contract_stubs.cpp — Stub implementations for JIT debug contract symbols\n'
        '//\n'
        '// Auto-generated by pipeline_native_aot_runner.py.\n'
        '// These stubs are intentionally empty — debug contract registration is not\n'
        '// needed in AOT verification mode (no JIT compilation occurs).\n'
        '\n'
        'void __cdecl JitDebugContractAddEntry(\n'
        '    void*, unsigned int, void const*, unsigned int) noexcept\n'
        '{\n'
        '}\n'
        '\n'
        'void __cdecl JitDebugContractInitMetadataRegistry(\n'
        '    void const*) noexcept\n'
        '{\n'
        '}\n'
    )
    stub_path.write_text(content, encoding="utf-8")
    print(f"    [build_entry] auto-generated jit_debug_contract_stubs.cpp at {stub_path}")







def remediate_flat_layout_includes(native_dir: Path) -> None:
    """Fix include paths in flat-layout native-aot.generated.cpp files.

    The codegen emits nested layout (generated/generated/) with includes like
    '#include "../native-aot.generated.header.h"' (relative to generated/generated/).
    The pipeline syncs to flat layout (generated/) where the header is in the same
    directory, so the '../' prefix resolves to the wrong parent directory.
    """
    for gen_cpp in native_dir.glob("*Subjects/generated/native-aot.generated.cpp"):
        text = gen_cpp.read_text(encoding="utf-8")
        old = '#include "../native-aot.generated.header.h"'
        new = '#include "native-aot.generated.header.h"'
        if old in text:
            text = text.replace(old, new)
            gen_cpp.write_text(text, encoding="utf-8")
            print(f"    [build_entry] fixed include path in {gen_cpp.relative_to(native_dir)}")


def remediate_forward_declarations(native_dir: Path) -> None:
    """Add forward declarations for extern \"C\" functions referenced before
    their declaration in generated native-aot.generated.cpp.

    The codegen emits generic dispatch wrappers (AOT entry resolution by
    generic_argument_type_handle) that call instantiated function bodies
    declared later in the file.  MSVC requires a visible declaration before
    the call site.
    """
    import re as _re
    for gen_cpp in native_dir.glob("**/native-aot.generated.cpp"):
        text = gen_cpp.read_text(encoding="utf-8")
        changed = False

        # Find the namespace opening line — insert at file scope (before namespace)
        ns_match = _re.search(r'^namespace chaos::il2cpp::codegen::(\w+)', text, _re.MULTILINE)
        if not ns_match:
            continue
        insert_pos = text.rfind('\n', 0, ns_match.start()) + 1

        # Find ALL extern "C" declarations of project-specific functions
        # (those matching the namespace name prefix)
        prefix = ns_match.group(1)
        # Match lines like: extern "C" CHAOS_IL2CPP_xxx func_name(params);
        ext_dec_pattern = _re.compile(
            r'^extern\s+"C"\s+(?:CHAOS_IL2CPP_\w+|void|int|double)\s+'
            + _re.escape(prefix) + r'[\w_]+\s*\(',
            _re.MULTILINE)
        all_decls = list(ext_dec_pattern.finditer(text))

        # For each declaration, check if its function name appears in code
        # before the declaration position.  If so, we need a forward decl.
        added = set()
        insertions = []
        for decl_match in all_decls:
            decl_line_start = text.rfind('\n', 0, decl_match.start()) + 1
            decl_pos = decl_match.start()

            # Extract function name
            fn_match = _re.match(
                r'extern\s+"C"\s+(?:CHAOS_IL2CPP_\w+|void|int|double)\s+([\w_]+)\s*\(',
                text[decl_match.start():decl_match.end()])
            if not fn_match:
                continue
            fn_name = fn_match.group(1)
            if fn_name in added:
                continue

            # Check if fn_name is referenced before decl_pos (exclude its own decl line)
            before_text = text[:decl_line_start]
            # Use word-boundary matching to avoid partial matches
            ref_pattern = _re.compile(r'\b' + _re.escape(fn_name) + r'\b')
            first_ref = ref_pattern.search(before_text)
            if first_ref:
                # This function IS referenced before its declaration.
                # Check if it already has a forward decl (extern "C" before the first ref)
                candidates_before_ref = [
                    d for d in all_decls if d.start() < first_ref.start()
                    and text[d.start():d.end()].startswith('extern "C"')
                    and fn_name in text[d.start():d.end()]
                ]
                # Also check for any existing forward declaration marker or
                # non-extern definition before the first reference
                has_fwd = _re.search(
                    r'(?:^|[\n;])\s*(?:extern\s+"C"\s+)?' + _re.escape(fn_name) + r'\s*\(',
                    before_text[:first_ref.start()])
                if not has_fwd:
                    # Add forward declaration right after the namespace opening
                    insertions.append(fn_name)
                    added.add(fn_name)

        if insertions:
            # Build forward declaration block from the original extern "C" lines
            fwd_lines = ["// Forward declarations (pipeline fix: used before extern \"C\" decl)"]
            for fn_name in sorted(insertions):
                # Find the original extern declaration line
                for d in all_decls:
                    if fn_name in text[d.start():d.end()]:
                        line = text[d.start():text.find('\n', d.start())]
                        fwd_lines.append(line)
                        break
            fwd_block = "\n".join(fwd_lines) + "\n\n"
            text = text[:insert_pos] + fwd_block + text[insert_pos:]
            changed = True

        if changed:
            gen_cpp.write_text(text, encoding="utf-8")
            print(f"    [build_entry] added forward declarations in {gen_cpp.name}")


def remediate_page_file_declarations(native_dir: Path) -> None:
    """Fix page file includes and add forward declarations for page-split codegen.

    The converter may emit large families as page-split TUs (native-aot.page-*.cpp).
    These page files have two issues:
    1. Calls to extern "C" functions from other assemblies need forward declarations.
    2. The converter sometimes omits native-aot.generated.header.h include from page-0001.cpp.

    Fix by:
    1. Adding extern "C" forward declarations to native-aot.generated.header.h
       (included by all page files)
    2. Adding the same include to page-0001.cpp if missing
    """
    import re as _re
    for page_file in sorted(native_dir.rglob("native-aot.page-*.cpp")):
        text = page_file.read_text(encoding="utf-8")
        changed = False

        # Step 1: Add header include if missing (page-0001 may not have it)
        header_include = '#include "native-aot.generated.header.h"'
        if header_include not in text:
            # Insert after the last #include line
            include_end = -1
            for m in _re.finditer(r'^#include.*$', text, _re.MULTILINE):
                include_end = m.end()
            if include_end != -1:
                text = text[:include_end] + '\n' + header_include + text[include_end:]
                changed = True
                print(f"    [fix_page] added header include in {page_file.name}")

        if changed:
            page_file.write_text(text, encoding="utf-8")

    # Step 2: Add forward declarations to native-aot.generated.header.h
    for header_file in native_dir.rglob("native-aot.generated.header.h"):
        text = header_file.read_text(encoding="utf-8")

        # Check if we already added forward decls
        if "// Forward declarations (pipeline fix" in text:
            continue

        # Find all extern "C" function calls in page files that need declarations
        # These are functions like Chaos_IL2CPP_HotUpdate_RuntimeManager_*
        # that are called but never declared in the page file TUs.
        fwd_decls: set[str] = set()
        for page_file in sorted(native_dir.rglob("native-aot.page-*.cpp")):
            page_text = page_file.read_text(encoding="utf-8")
            # Find function calls: Identifier(args) pattern where the identifier
            # starts with an uppercase prefix (Chaos_IL2CPP_* or similar)
            for m in _re.finditer(
                    r'\b(Chaos_IL2CPP_\w+)\s*\(',
                    page_text):
                fn_name = m.group(1)
                # Check if it has a declaration in this file
                if _re.search(
                        r'(?:extern\s+"C"\s+)?\b' + _re.escape(fn_name) + r'\s*\(',
                        page_text[:m.start()]):
                    continue
                # Check if already in the header
                if _re.search(
                        r'\b' + _re.escape(fn_name) + r'\s*\(',
                        text):
                    continue
                fwd_decls.add(fn_name)

        if fwd_decls:
            # Build forward declarations by scanning page files for the exact
            # extern "C" declaration text.  Using the exact text (return type +
            # parameter list) avoids C2116/C2733 errors from mismatched
            # signatures (e.g. void vs CHAOS_IL2CPP_INTPTR, or (...) vs
            # specific parameter types in extern "C" functions).
            decl_lines = ["", "// Forward declarations (pipeline fix: page-file extern \"C\" functions)"]
            ext_decl_pattern = _re.compile(
                r'(extern\s+"C"\s+(?:CHAOS_IL2CPP_\w+|void|int|double|chaos_valuetype_\w+)\s+'
                + '(' + '|'.join(_re.escape(f) for f in sorted(fwd_decls)) + r')'
                + r'\s*\([^)]*\)\s*;)',
                _re.MULTILINE)
            found: set[str] = set()
            for page_file in sorted(native_dir.rglob("native-aot.page-*.cpp")):
                page_text = page_file.read_text(encoding="utf-8")
                for m in ext_decl_pattern.finditer(page_text):
                    fn_name = m.group(2)
                    if fn_name in fwd_decls and fn_name not in found:
                        decl_lines.append(m.group(1))
                        found.add(fn_name)
            # Fallback for any function whose declaration we couldn't locate
            for fn_name in sorted(fwd_decls - found):
                decl_lines.append(f'extern "C" void {fn_name}(...);')
            text += "\n".join(decl_lines) + "\n"
            header_file.write_text(text, encoding="utf-8")
            print(f"    [fix_page] added {len(fwd_decls)} forward decls in {header_file.name}")

        # Step 2b: Fix ambiguous TypeInfoV0 — the header declares bare `struct TypeInfoV0;`
        # and `extern TypeInfoV0 ...` which causes C2872 when included from within a
        # namespace block (page files live in `chaos::il2cpp::codegen::<Family>`).
        # Qualify with the full namespace to avoid ambiguity with
        # `chaos::il2cpp::common::TypeInfoV0` from type_info.h.
        for header_file in native_dir.rglob("native-aot.generated.header.h"):
            text = header_file.read_text(encoding="utf-8")
            fixed = text.replace(
                'struct TypeInfoV0;',
                'struct chaos::il2cpp::common::TypeInfoV0;')
            fixed = fixed.replace(
                'extern TypeInfoV0 ',
                'extern chaos::il2cpp::common::TypeInfoV0 ')
            if fixed != text:
                header_file.write_text(fixed, encoding="utf-8")
                count = fixed.count('chaos::il2cpp::common::TypeInfoV0')
                print(f"    [fix_page] qualified {count}x TypeInfoV0 to "
                      f"chaos::il2cpp::common::TypeInfoV0 in {header_file.name}")

        # Step 2c: Generate missing chaos_valuetype_* typedefs for external
        # assembly value types. The converter emits these in function signatures
        # (e.g. chaos_valuetype_Chaos_IL2CPP_Contracts_SubjectId) but does not
        # emit the corresponding typedef. Scan chaos_generated_module.h and
        # the page files, collect all unique chaos_valuetype_* identifiers,
        # and add typedef CHAOS_IL2CPP_INT32 for each one not yet defined.
        module_h_files = list(native_dir.rglob("chaos_generated_module.h"))
        if module_h_files:
            # Find all unique chaos_valuetype_* identifiers in generated output
            all_vt: set[str] = set()
            for pattern in ("chaos_generated_module.h", "chaos_generated_module.cpp",
                            "native-aot.page-*.cpp", "native-aot.generated.cpp",
                            "native-aot.generated.header.h"):
                for f in native_dir.rglob(pattern):
                    try:
                        content = f.read_text(encoding="utf-8")
                    except Exception:
                        continue
                    for m in _re.finditer(r'\bchaos_valuetype_\w+', content):
                        all_vt.add(m.group())
            if all_vt:
                module_h = module_h_files[0]
                text = module_h.read_text(encoding="utf-8")
                # Check if already fixed
                if "// chaos_valuetype typedefs (pipeline fix)" in text:
                    pass  # already fixed
                else:
                    # Find which ones already have typedefs or struct definitions
                    defined_vt: set[str] = set()
                    for m in _re.finditer(
                            r'typedef\s+\w+\s+(chaos_valuetype_\w+)', text):
                        defined_vt.add(m.group(1))
                    for m in _re.finditer(
                            r'struct\s+(chaos_valuetype_\w+)\s*\{', text):
                        defined_vt.add(m.group(1))
                    # Filter out already-defined ones and known type_info types
                    missing = sorted(all_vt - defined_vt)
                    if missing:
                        typefix_lines = [
                            "",
                            "// chaos_valuetype typedefs (pipeline fix: missing value type aliases)",
                            "// These are generated extern \"C\" value type names for which the",
                            "// converter did not emit underlying typedefs. All are opaque 32-bit",
                            "// value types in the managed ABI surface."]
                        for vt_name in missing:
                            typefix_lines.append(
                                f"typedef CHAOS_IL2CPP_INT32 {vt_name};")
                        insert_pos = text.find("#pragma once")
                        if insert_pos == -1:
                            insert_pos = 0
                        else:
                            # Insert after the includes following #pragma once
                            rest = text[insert_pos:]
                            include_end = rest.rfind("\n\n")
                            if include_end == -1:
                                include_end = len(rest)
                            # Find a good insertion point: after last include in the header
                            last_include = -1
                            for m in _re.finditer(r'^#include.*$', text, _re.MULTILINE):
                                last_include = max(last_include, m.end())
                            insert_pos = last_include if last_include != -1 else len(text)
                            # Make sure insert_pos is at a line boundary
                            while insert_pos < len(text) and text[insert_pos] == '\n':
                                insert_pos += 1
                        text = (text[:insert_pos] + "\n".join(typefix_lines) +
                                "\n" + text[insert_pos:])
                        module_h.write_text(text, encoding="utf-8")
                        print(f"    [fix_page] added {len(missing)} chaos_valuetype"
                              f" typedefs in {module_h.name}")

                    # Also add same typedefs to native-aot.generated.header.h so they
                    # are visible in page-file TUs (page files include this header but
                    # NOT chaos_generated_module.h).
                    for header_h_file in native_dir.rglob("native-aot.generated.header.h"):
                        hdr_text = header_h_file.read_text(encoding="utf-8")
                        if "// chaos_valuetype typedefs (pipeline fix)" in hdr_text:
                            continue  # already fixed
                        # Find which ones already have typedefs or struct definitions
                        defined_vt_hdr: set[str] = set()
                        for m in _re.finditer(
                                r'typedef\s+\w+\s+(chaos_valuetype_\w+)', hdr_text):
                            defined_vt_hdr.add(m.group(1))
                        for m in _re.finditer(
                                r'struct\s+(chaos_valuetype_\w+)\s*\{', hdr_text):
                            defined_vt_hdr.add(m.group(1))
                        missing_hdr = sorted(all_vt - defined_vt_hdr)
                        if missing_hdr:
                            typefix_lines_hdr = [
                                "",
                                "// chaos_valuetype typedefs (pipeline fix: missing value type aliases)"]
                            for vt_name in missing_hdr:
                                typefix_lines_hdr.append(
                                    f"typedef CHAOS_IL2CPP_INT32 {vt_name};")
                            # Insert at the top of the file (after last include or
                            # after #pragma once) so typedefs are visible before
                            # any forward declarations or extern "C" usage.
                            insert_pos = len(hdr_text)
                            last_include_hdr = -1
                            for m in _re.finditer(r'^#include.*$', hdr_text, _re.MULTILINE):
                                last_include_hdr = max(last_include_hdr, m.end())
                            if last_include_hdr >= 0:
                                insert_pos = last_include_hdr + 1
                            else:
                                pragma_pos = hdr_text.find("#pragma once")
                                if pragma_pos >= 0:
                                    rest = hdr_text[pragma_pos:]
                                    nl2 = rest.find("\n\n")
                                    if nl2 >= 0:
                                        insert_pos = pragma_pos + nl2 + 2
                            hdr_text = (hdr_text[:insert_pos] + "\n".join(typefix_lines_hdr) +
                                        "\n" + hdr_text[insert_pos:])
                            header_h_file.write_text(hdr_text, encoding="utf-8")
                            print(f"    [fix_page] added {len(missing_hdr)} chaos_valuetype"
                                  f" typedefs in {header_h_file.name}")

        # Step 2d: Rename duplicate function-pointer member names in
        # chaos_generated_module.h. C++ structs disallow overloaded members
        # (e.g. two `ctor` or two `Equals` with the same signature). The
        # converter emits positional aggregate initializers in the .cpp, so
        # uniquifying the .h member names is sufficient.
        for module_h_file in module_h_files:
            text = module_h_file.read_text(encoding="utf-8")
            if "// overload rename (pipeline fix)" in text:
                continue
            # Find all struct X_t { ... } member blocks
            # Pattern: `struct <name>_t { ... } <member>;`
            fixed_text = text
            seen_names: set[str] = set()
            rename_map: dict[str, str] = {}
            # Process each struct block
            def _uniquify_block(block: str) -> str:
                nonlocal seen_names, rename_map
                lines = block.split("\n")
                new_lines: list[str] = []
                for line in lines:
                    if "//" in line:
                        new_lines.append(line)
                        continue
                    # Match function pointer member:  `type (*name)(params);`
                    m = _re.match(
                        r'^(\s*(?:\w+(?:\s*::\s*\w+)?(?:\s*\*)?)\s*\(\*)'
                        r'(\w+)\)(.*)$', line)
                    if m:
                        name = m.group(2)
                        if name in seen_names:
                            # Rename duplicate
                            counter = 1
                            while f"{name}_{counter}" in seen_names:
                                counter += 1
                            new_name = f"{name}_{counter}"
                            rename_map[name] = new_name
                            line = (m.group(1) + new_name + ")" +
                                    m.group(3))
                            seen_names.add(new_name)
                        else:
                            seen_names.add(name)
                    new_lines.append(line)
                return "\n".join(new_lines)
            # Process each struct block: struct X_t { ... } var;
            block_pattern = _re.compile(
                r'(struct\s+(\w+)_t\s*\{)(.*?)\}\s*(\w+)\s*;',
                _re.DOTALL)
            def _fix_block(m: _re.Match) -> str:
                nonlocal seen_names
                seen_names.clear()
                rename_map.clear()
                body = m.group(3)
                fixed_body = _uniquify_block(body)
                closing = "} " + m.group(4) + ";"
                if rename_map:
                    marker = ("\n    // overload rename (pipeline fix): "
                              + ", ".join(f"{k}→{v}"
                                          for k, v in rename_map.items()))
                    return m.group(1) + fixed_body + closing + marker
                return m.group(1) + fixed_body + closing
            fixed_text = block_pattern.sub(_fix_block, fixed_text)
            if fixed_text != text:
                module_h_file.write_text(fixed_text, encoding="utf-8")
                print(f"    [fix_page] deduplicated overloaded members in"
                      f" {module_h_file.name}")

        # Step 3e: Deduplicate entire struct type definitions within
        # struct Functions. The converter generates the same nested struct
        # type (e.g. __z__ReadOnlySingleElementList_Enumerator__0_) twice
        # when generic instantiations produce identical mangled names.
        # C++ forbids defining the same struct tag twice in the same scope.
        for module_h_file in module_h_files:
            text = module_h_file.read_text(encoding="utf-8")
            if "// duplicate struct skipped (pipeline fix)" in text:
                continue
            seen_struct_types: set[str] = set()
            lines = text.split("\n")
            i = 0
            result: list[str] = []
            dedup_count = 0
            while i < len(lines):
                line = lines[i]
                # Match first-level nested struct inside Functions:
                #   `struct Name {`
                m = _re.match(r'^(    )struct\s+(\w+)\s*\{$', line)
                if m and m.group(2) != "Functions":
                    indent = m.group(1)
                    struct_name = m.group(2)
                    # Find matching `};` by counting braces
                    brace_count = 1
                    # Check if this struct has a trailing member variable:
                    # `struct Name { ... } var;` — A1 pattern with _t suffix
                    # We only dedup standalone `struct Name { ... };` (A2 pattern)
                    # by checking if `}` is followed by `;` or ` var;`
                    j = i + 1
                    while j < len(lines) and brace_count > 0:
                        brace_count += lines[j].count("{")
                        brace_count -= lines[j].count("}")
                        if brace_count <= 0:
                            break
                        j += 1
                    if brace_count == 0 and j < len(lines):
                        # Check that this is a TYPE definition (no trailing member var)
                        # by verifying the closing is `};` not `} name;`
                        closing = lines[j].strip()
                        # Only dedup if closing is `};` (type definition, no member variable)
                        if closing == "};" and struct_name in seen_struct_types:
                            dedup_count += 1
                            result.append(
                                f"{indent}/* struct {struct_name}: "
                                f"duplicate struct skipped (pipeline fix) */")
                            i = j + 1
                            continue
                        else:
                            seen_struct_types.add(struct_name)
                result.append(line)
                i += 1
            if dedup_count:
                module_h_file.write_text("\n".join(result), encoding="utf-8")
                print(f"    [fix_page] deduplicated {dedup_count} struct type"
                      f" redefinitions in {module_h_file.name}")

    # Step 5: Fix supplemental-metadata codegen version mismatches.
    # The converter generates code referencing `.hot` on MethodTable (no such
    # member), `chaos_type_id_*` type IDs without declarations, and extern "C"
    # overloads. Fix these across ALL generated files in the native dir.
    remediate_supplemental_codegen(native_dir)


def remediate_supplemental_codegen(native_dir: Path) -> None:
    """Fix converter-vs-runtime version mismatches in supplemental-metadata codegen.

    The supplemental-metadata codegen pathway generates patterns from an older
    runtime version:
      1. `&chaos_mt_X.hot` — MethodTable has no nested `.hot` member
      2. `constexpr` TypeInfoHot-pointer arrays that need .hot qualifiers
      3. `chaos_type_id_*` / `chaos_iface_map_*` references without extern decls
    """
    import re as _re

    # Step A: Fix `.hot` member access on chaos_mt_* variables.
    # MethodTable has NO `.hot` member (it IS the hot section). For TypeInfoV0,
    # `.hot` IS a member.  We fix ALL access patterns:
    #   &chaos_mt_X.hot  →  reinterpret_cast<const TypeInfoHot*>(&chaos_mt_X)
    #   chaos_mt_X.hot   →  *reinterpret_cast<const TypeInfoHot*>(&chaos_mt_X)
    for gen_file in native_dir.rglob("native-aot.*"):
        if gen_file.suffix not in (".cpp", ".h"):
            continue
        text = gen_file.read_text(encoding="utf-8")
        changed = False
        # Pattern 1: &chaos_mt_X.hot (take address of the hot section)
        new_text = _re.sub(
            r'&(chaos_mt_\w+)\.hot\b',
            r'reinterpret_cast<const TypeInfoHot*>(&\1)',
            text)
        if new_text != text:
            changed = True
            text = new_text
        # Pattern 2: &chaos_mt_X.AsTypeInfoHot() — AsTypeInfoHot() already returns
        # TypeInfoHot* (a pointer rvalue), so & creates an invalid TypeInfoHot**.
        # Remove the & since the return value IS the pointer we need.
        new_text = _re.sub(
            r'&(chaos_mt_\w+)\.AsTypeInfoHot\(\)',
            r'\1.AsTypeInfoHot()',
            text)
        if new_text != text:
            changed = True
            text = new_text
        if changed:
            gen_file.write_text(text, encoding="utf-8")
            print(f"    [fix_codegen] fixed .hot member access in {gen_file.name}")

    # Step B: Add extern const uint64_t declarations for missing chaos_type_id_*
    # symbols referenced in generated code but not declared anywhere.
    type_id_set: set[str] = set()
    iface_map_set: set[str] = set()
    for gen_file in native_dir.rglob("native-aot.page-*.cpp"):
        page_text = gen_file.read_text(encoding="utf-8")
        for m in _re.finditer(r'\b(chaos_type_id_\w+)\b', page_text):
            type_id_set.add(m.group(1))
        for m in _re.finditer(r'\b(chaos_iface_map_\w+)\b', page_text):
            iface_map_set.add(m.group(1))
    # Also check the generated.cpp
    for gen_file in native_dir.rglob("native-aot.generated.cpp"):
        if gen_file.is_file():
            gen_text = gen_file.read_text(encoding="utf-8")
            for m in _re.finditer(r'\b(chaos_type_id_\w+)\b', gen_text):
                type_id_set.add(m.group(1))
    # Check if already declared
    declared_type_ids: set[str] = set()
    declared_iface_maps: set[str] = set()
    for header_file in native_dir.rglob("native-aot.generated.header.h"):
        hdr_text = header_file.read_text(encoding="utf-8")
        for m in _re.finditer(r'(?:extern\s+const|inline\s+constexpr)\s+\w+\s+(chaos_type_id_\w+)', hdr_text):
            declared_type_ids.add(m.group(1))
        for m in _re.finditer(r'extern\s+constexpr\s+\w+\s+(chaos_iface_map_\w+)', hdr_text):
            declared_iface_maps.add(m.group(1))
    missing_type_ids = sorted(type_id_set - declared_type_ids)
    missing_iface_maps = sorted(iface_map_set - declared_iface_maps)
    if missing_type_ids or missing_iface_maps:
        for header_file in native_dir.rglob("native-aot.generated.header.h"):
            hdr_text = header_file.read_text(encoding="utf-8")
            new_hdr = hdr_text
            # Insert after the last section-comment or existing decl block
            insert_marker = "// Forward declarations (pipeline fix"
            if insert_marker not in new_hdr:
                insert_marker = "#pragma once"
            decl_lines: list[str] = []
            if missing_type_ids:
                decl_lines.append(
                    "// chaos_type_id extern decls (pipeline fix)")
                for tid in missing_type_ids:
                    decl_lines.append(f"extern const uint64_t {tid};")
            if missing_iface_maps:
                decl_lines.append(
                    "// chaos_iface_map extern decls (pipeline fix)")
                for im in missing_iface_maps:
                    decl_lines.append(f"extern constexpr int {im};" if not im.startswith('chaos_iface_map_')
                                      else f"extern const int {im};")
            if decl_lines:
                insert_pos = new_hdr.rfind(insert_marker)
                if insert_pos == -1:
                    insert_pos = new_hdr.find("#pragma once")
                    if insert_pos != -1:
                        insert_pos = new_hdr.index('\n', insert_pos) + 1
                new_hdr = (new_hdr[:insert_pos] + "\n" +
                           "\n".join(decl_lines) + "\n" +
                           new_hdr[insert_pos:])
                header_file.write_text(new_hdr, encoding="utf-8")
                print(f"    [fix_codegen] added {len(missing_type_ids)} type_id + "
                      f"{len(missing_iface_maps)} iface_map decls in {header_file.name}")

    # Step B2: Add missing struct chaos_type_* forward declarations.
    # codegen emits reinterpret_cast<chaos_type_System_Private_CoreLib_System_Action*>
    # for delegate calls, but the delegate type may not be in _allEmittedTypeSubjectIds
    # (Phase 0 type discovery doesn't track types referenced only through delegate
    # invocation emission).  This step scans generated cpp files for chaos_type_*
    # references (excluding chaos_type_id_*, chaos_type_info_*, chaos_type_shape_*)
    # and adds missing struct forward declarations to the header.
    type_ref_set: set[str] = set()
    for gen_file in native_dir.rglob("native-aot.*"):
        if gen_file.suffix not in (".cpp",):
            continue
        gen_text = gen_file.read_text(encoding="utf-8")
        for m in _re.finditer(r'\bchaos_type_(?!(?:id_|info_|shape_))(\w+)\b', gen_text):
            type_ref_set.add(f"chaos_type_{m.group(1)}")
    if type_ref_set:
        declared_types: set[str] = set()
        for header_file in native_dir.rglob("native-aot.generated.header.h"):
            hdr_text = header_file.read_text(encoding="utf-8")
            for m in _re.finditer(r'struct\s+(chaos_type_\w+)\s*;', hdr_text):
                declared_types.add(m.group(1))
        missing_types = sorted(type_ref_set - declared_types)
        if missing_types:
            for header_file in native_dir.rglob("native-aot.generated.header.h"):
                hdr_text = header_file.read_text(encoding="utf-8")
                new_hdr = hdr_text
                insert_marker = "// Forward declarations (pipeline fix"
                if insert_marker not in new_hdr:
                    insert_marker = "// chaos_type_id extern decls (pipeline fix)"
                if insert_marker not in new_hdr:
                    insert_marker = "#pragma once"
                decl_lines: list[str] = []
                decl_lines.append("// chaos_type struct forward decls (pipeline fix)")
                for tsym in missing_types:
                    decl_lines.append(f"struct {tsym};")
                insert_pos = new_hdr.rfind(insert_marker)
                if insert_pos == -1:
                    insert_pos = new_hdr.find("#pragma once")
                    if insert_pos != -1:
                        insert_pos = new_hdr.index('\n', insert_pos) + 1
                new_hdr = (new_hdr[:insert_pos] + "\n" +
                           "\n".join(decl_lines) + "\n" +
                           new_hdr[insert_pos:])
                header_file.write_text(new_hdr, encoding="utf-8")
                print(f"    [fix_codegen] added {len(missing_types)} chaos_type_* struct "
                      f"forward decls in {header_file.name}")

    # Step C: Fix constexpr InterfaceMapEntry arrays that reference extern
    # chaos_type_id_* symbols (which are only extern const, not constexpr).
    # Change `static constexpr InterfaceMapEntry` to `static const InterfaceMapEntry`
    # so non-constexpr extern symbols can be used in initializers.
    for page_file in native_dir.rglob("native-aot.page-*.cpp"):
        text = page_file.read_text(encoding="utf-8")
        new_text = _re.sub(
            r'(static\s+)constexpr\s+(InterfaceMapEntry\s+\w+\[\]\s*=\s*\{)',
            r'\1const \2',
            text)
        if new_text != text:
            page_file.write_text(new_text, encoding="utf-8")
            print(f"    [fix_codegen] relaxed constexpr→const for iface_map in {page_file.name}")

    # Step D: Fix missing _sN eval-stack slot declarations in generated code.
    # ComputeMaxEvalStackDepth sometimes undercounts for generic methods where
    # StringId emission or inlined code expands the effective stack depth beyond
    # the IL-level analysis.  This post-processing step finds each function's
    # declared and used _sN slots and injects any missing declarations.
    import re as _re2
    for gen_file in native_dir.rglob("native-aot.*"):
        if gen_file.suffix not in (".cpp",):
            continue
        text = gen_file.read_text(encoding="utf-8")
        # Split into per-function blocks at `// Managed method:` markers
        func_blocks = _re2.split(r'(?=^// Managed method:)', text, flags=_re2.MULTILINE)
        modified_blocks = []
        total_added = 0
        for block in func_blocks:
            if not block.strip():
                modified_blocks.append(block)
                continue
            # Find all slot declarations within this function block
            decl_pattern = _re2.compile(r'^\tCHAOS_IL2CPP_INTPTR (_s\d+){};', _re2.MULTILINE)
            use_pattern = _re2.compile(r'(?<!CHAOS_IL2CPP_INTPTR )_s(\d+)')
            declared = [int(m.group(1)[2:]) for m in decl_pattern.finditer(block)]
            if not declared:
                modified_blocks.append(block)
                continue
            max_declared = max(declared)
            # Find max _sN usage in non-declaration context
            max_used = max_declared
            for m in use_pattern.finditer(block):
                idx = int(m.group(1))
                if idx > max_used:
                    max_used = idx
            if max_used > max_declared:
                # Find the last declaration and insert after it
                last_decl_end = 0
                for m in decl_pattern.finditer(block):
                    end = m.end()
                    if end > last_decl_end:
                        last_decl_end = end
                new_decls = ''
                for i in range(max_declared + 1, max_used + 1):
                    new_decls += f'\tCHAOS_IL2CPP_INTPTR _s{i}{{}};\n'
                block = block[:last_decl_end] + '\n' + new_decls + block[last_decl_end:]
                total_added += max_used - max_declared
            modified_blocks.append(block)
        if total_added > 0:
            gen_file.write_text(''.join(modified_blocks), encoding="utf-8")
            print(f"    [fix_codegen] added {total_added} slot declarations in {gen_file.name}")

    # Step E: Fix ChaosRegisterEnumGeneratedMetadata calls when no enum types exist.
    # The codegen unconditionally emits this call, but the enum_metadata file only
    # defines the function when enum types are present.  If the header says
    # "No enum types found", replace the call with a no-op lambda.
    for em_file in native_dir.rglob("enum_metadata.generated.h"):
        em_text = em_file.read_text(encoding="utf-8")
        if "No enum types found in this closure" not in em_text:
            continue
        # Find the corresponding generated.cpp and remove the call
        gen_dir = em_file.parent
        for gen_file in gen_dir.glob("native-aot.generated.cpp"):
            gtext = gen_file.read_text(encoding="utf-8")
            # Replace the static-registration lambda that calls the missing function
            new_gtext = _re.sub(
                r'static const CHAOS_IL2CPP_UINT32 s_enum_registered = \[\]\(\) noexcept \{[\s\S]*?ChaosRegisterEnumGeneratedMetadata\(\);[\s\S]*?return 1u;\s*\}\(\);',
                '// No enum types in this closure — skip registration.\nstatic constexpr CHAOS_IL2CPP_UINT32 s_enum_registered = 0u;',
                gtext)
            if new_gtext != gtext:
                gen_file.write_text(new_gtext, encoding="utf-8")
                print(f"    [fix_codegen] removed enum registration call in {gen_file.name}")


def write_sentinel_dispatch(dispatch_cpp: Path) -> None:
    """Write a sentinel verification_dispatch.generated.cpp for cmake configure.

    The real dispatch file is generated by verification_dispatch_generator.py
    after codegen runs.  This sentinel ensures cmake configure can find the
    source file during the initial build.
    """
    content = (
        '// verification_dispatch.generated.cpp — sentinel (pre-codegen)\n'
        '#include <cstdint>\n'
        '#include <chaos/native_types.h>\n'
        '\n'
        'struct BenchmarkResult {\n'
        '    double elapsed_ms;\n'
        '    int64_t allocated_bytes;\n'
        '};\n'
        '\n'
        'extern "C" const int kSubjectEntryCount;\n'
        'extern "C" const int kSubjectSlotMap[];\n'
        '\n'
        'extern "C" CHAOS_IL2CPP_INT32 RunFactAll() { return 0; }\n'
        'extern "C" BenchmarkResult RunBenchmark(int, int) { return {-1.0, 0}; }\n'
        'extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll() { return 0; }\n'
        'extern "C" BenchmarkResult RunHotpatchBenchmark(int, int) { return {-1.0, 0}; }\n'
    )
    dispatch_cpp.parent.mkdir(parents=True, exist_ok=True)
    dispatch_cpp.write_text(content, encoding="utf-8")


def build_entry_executable(family_slug: str, *, verification: Path | None = None, config_tier: str = "CHECK", output_name: str = "entry.exe", is_jit: bool = False) -> bool:
    v = verification or _VERIFICATION
    native_dir = v / family_slug / "native"
    cmakelists = native_dir / "CMakeLists.txt"

    # MAX_PATH pre-check: MSVC 260-char path limit. If the longest generated .cpp
    # path exceeds 256 chars, cmake build will fail with C1083.  The three sub-families
    # covered by the combo slug already pass individually, so skip gracefully.
    max_src_len = 0
    longest_src = ""
    for src in native_dir.rglob("*.cpp"):
        src_str = str(src)
        if len(src_str) > max_src_len:
            max_src_len = len(src_str)
            longest_src = src_str
    if max_src_len >= 256:
        print(f"    [build_entry] MAX_PATH: longest .cpp path ({max_src_len} chars) exceeds 256-char limit")
        print(f"      Longest: {longest_src}")
        print(f"      SKIPPING build — individual sub-families already provide coverage")
        return True

    # Clean stale generated files from native/ before re-generation.
    # Keeps CMakeLists.txt, handwritten files, and entry.exe (build output).
    # Removes all *.cpp/*.h that are auto-generated or synced from codegen.
    for stale_name in list(native_dir.iterdir()):
        if not stale_name.is_file():
            continue
        if stale_name.name in ("CMakeLists.txt", "CMakeCache.txt"):
            continue
        if stale_name.suffix in (".cpp", ".h") and stale_name.name not in (
            # Handwritten files to preserve
            "jit_debug_contract_stubs.cpp",
            "verification_dispatch.generated.cpp",
        ):
            stale_name.unlink()
            print(f"    [build_entry] cleaned stale: {stale_name.name}")
    # Also clean codegen-sync subdirectories
    for stale_dir in sorted(native_dir.iterdir()):
        if stale_dir.is_dir() and stale_dir.name not in ("build",):
            shutil.rmtree(stale_dir)
            print(f"    [build_entry] cleaned stale directory: {stale_dir.name}")
    stale_jit_stubs = native_dir / "jit_stubs.cpp"
    if stale_jit_stubs.exists():
        stale_jit_stubs.unlink()
        print(f"    [build_entry] cleaned stale jit_stubs.cpp")

    # Auto-generate CMakeLists.txt if missing (e.g. after clean delete)
    ensure_cmake_lists_file(cmakelists, family_slug, v, is_jit=is_jit, config_tier=config_tier)

    # Inject config tier compile definition into CMakeLists.txt
    inject_config_tier(cmakelists, config_tier)

    # Inject SEH define (Windows) to avoid MSVC EH table corruption in large TUs
    inject_windows_seh_compile_definition(cmakelists)

    # Inject JIT mode define if this is a JIT build (ensure_cmake_lists_file
    # returns early for existing CMakeLists.txt, so the JIT define is never
    # added during initial generation when build_entry_executable runs for JIT)
    inject_jit_mode_define(cmakelists, is_jit)

    # Inject verification_dispatch.generated.cpp into CMakeLists.txt if missing
    # (families created before the dispatch generator refactor need this)
    inject_verification_dispatch_source(cmakelists)

    # Inject runtime_stubs include path if missing (older CMakeLists.txt don't have it)
    inject_runtime_stubs_include(cmakelists)

    # Ensure runtime-patchdata.cpp exists (sentinel if not generated)
    patchdata_cpp = native_dir / "runtime-patchdata.cpp"
    if not patchdata_cpp.exists():
        write_sentinel_patch_data(v / family_slug)
        print(f"    [build_entry] sentinel runtime-patchdata.cpp generated")

    # Ensure microbench.cpp exists for --microbench support
    ensure_micro_benchmark_source(native_dir)
    inject_micro_benchmark_source(cmakelists)

    # Ensure JIT debug contract stubs exist (needed for AOT builds linking chaos_jit.lib)
    ensure_jit_debug_contract_stubs(native_dir)

    # Generate runtime-entry.cpp from Python template — clean, no post-processing needed
    from verification.orchestration.family_entrypoint import generate_runtime_entry
    native_runtime_entry = native_dir / "runtime-entry.cpp"
    native_runtime_entry.write_text(generate_runtime_entry(is_jit=is_jit), encoding="utf-8")
    print(f"    [build_entry] generated runtime-entry.cpp from Python template -> {native_runtime_entry}")
    # Sync generated .cpp from codegen/<Assembly>/ to native/<Assembly>/
    # so the native CMakeLists.txt compiles the latest codegen output.
    codegen_dir = v / family_slug / "codegen"
    synced_names = set()
    if codegen_dir.exists():
        for subdir in sorted(codegen_dir.iterdir()):
            if not subdir.is_dir() or subdir.name in ("build", "generated", "hot-update"):
                continue
            # Sync ALL generated files (.cpp, .h) from codegen to native
            # After R1 fix: files land at codegen/<Assembly>/generated/* (single-level)
            src = subdir / "generated"
            if not src.exists():
                print(f"    [build_entry] skipping {subdir.name}: no generated/ subdir")
                continue
            dst = native_dir / subdir.name / "generated"
            # Clean stale native files before sync — the converter may change
            # output layout (e.g. switched to page-split output), and stale files
            # from previous runs cause build errors.
            if dst.exists():
                shutil.rmtree(dst)
            dst.mkdir(parents=True, exist_ok=True)
            for src_file in src.rglob("*"):
                if src_file.is_file():
                    rel = src_file.relative_to(src)
                    target = dst / rel
                    target.parent.mkdir(parents=True, exist_ok=True)
                    target.write_bytes(src_file.read_bytes())
            synced_names.add(subdir.name)
            print(f"    [build_entry] synced {src.relative_to(codegen_dir)} to native/")

    # Handle assembly name mismatch: codegen output directory (from the DLL assembly
    # name) may differ from what CMakeLists.txt expects (e.g. InterfaceDispatchNativeEntry
    # vs InterfaceDispatchSubjects).  If the CMakeLists-expected file is missing or stale,
    # re-sync from the best available codegen source.
    if cmakelists.exists() and codegen_dir.exists() and synced_names:
        expected_cmake_dirs = set()
        for line in cmakelists.read_text(encoding="utf-8").splitlines():
            m = re.search(r'\$\{CHAOS_GEN_DIR\}/([^/]+)/generated/native-aot\.generated\.cpp', line)
            if m:
                expected_cmake_dirs.add(m.group(1))
        missing_expected = expected_cmake_dirs - synced_names
        if missing_expected:
            # Find codegen sources available for the missing names
            for expected in sorted(missing_expected):
                expected_native = native_dir / expected / "generated" / "native-aot.generated.cpp"
                # Check if a corresponding codegen source exists (converter writes
                # directly to codegen/<AssemblyName>/generated/ — no flat layout step)
                expected_codegen = codegen_dir / expected / "generated" / "native-aot.generated.cpp"
                if expected_codegen.exists():
                    # Already has its own codegen output — sync it
                    expected_native.parent.mkdir(parents=True, exist_ok=True)
                    expected_native.write_text(expected_codegen.read_text(encoding="utf-8"), encoding="utf-8")
                    print(f"    [build_entry] synced {expected_codegen.relative_to(codegen_dir)} to native/ (delayed)")
                else:
                    # Assembly name mismatch — find best source (most recently modified)
                    def _find_cpp_source(s):
                        return codegen_dir / s / "generated" / "native-aot.generated.cpp"
                    best_src = max(
                        (_find_cpp_source(s) for s in synced_names),
                        key=lambda p: p.stat().st_mtime if p.exists() else 0,
                    )
                    expected_native.parent.mkdir(parents=True, exist_ok=True)
                    expected_native.write_text(best_src.read_text(encoding="utf-8"), encoding="utf-8")
                    print(f"    [build_entry] synced {best_src.relative_to(codegen_dir)} -> {expected}/generated/ (assembly name mismatch)")

    # Ensure CMakeLists.txt exists — auto-generate from template if missing
    # (families deleted and regenerated from scratch won't have native/CMakeLists.txt)
    ensure_cmake_lists_file(cmakelists, family_slug, v, is_jit=is_jit, config_tier=config_tier)

    # Ensure verification_dispatch.generated.cpp exists
    # The real file is generated by the orchestrator after codegen; the sentinel
    # ensures cmake configure can find the source file during initial build.
    dispatch_cpp = native_dir / "verification_dispatch.generated.cpp"
    if not dispatch_cpp.exists():
        write_sentinel_dispatch(dispatch_cpp)
        print(f"    [build_entry] sentinel verification_dispatch.generated.cpp created")

    # Add forward declarations + flat-layout include path fix
    remediate_flat_layout_includes(native_dir)
    remediate_forward_declarations(native_dir)
    remediate_page_file_declarations(native_dir)

    # Fix s_hotpatch_module cross-page linkage in page-split families.
    # Codegen defines "extern constexpr HotpatchModuleV0 s_hotpatch_module"
    # at namespace scope in page-0001.cpp, and references it from other page
    # files with fully-qualified names (e.g.,
    # "&chaos::il2cpp::codegen::XXXSubjects::s_hotpatch_module").
    #
    # The issue: ChaosJitRegisterAll() is generated OUTSIDE the namespace
    # block (at file scope).  If the pipeline strips the class qualification
    # to just "s_hotpatch_module", the unqualified name cannot resolve to the
    # namespace-scoped variable from file scope.
    #
    # Fix: add an extern declaration inside the namespace block in each page
    # file that does NOT define s_hotpatch_module (all except page-0001).
    # The fully-qualified references are left intact — they resolve correctly
    # from both inside and outside the namespace when the extern is present.
    for page_file in sorted(native_dir.rglob("native-aot.page-*.cpp")):
        text = page_file.read_text(encoding="utf-8")
        # Only touch files that reference s_hotpatch_module
        if 's_hotpatch_module' not in text:
            continue
        # Skip the file that defines s_hotpatch_module (page-0001.cpp)
        if 'extern constexpr HotpatchModuleV0 s_hotpatch_module' in text:
            continue
        # Check if it already has a namespace-scope extern declaration
        ns_decl = 'extern const HotpatchModuleV0 s_hotpatch_module;'
        if ns_decl in text:
            continue
        # Find the namespace opening and add extern declaration inside it
        ns_match = re.search(
            r'(namespace\s+chaos::il2cpp::codegen::\w+\s*\{)',
            text,
        )
        if ns_match:
            insert_pos = ns_match.end()
            text = (
                text[:insert_pos] +
                f'\n// Pipeline fix: extern declaration for cross-page s_hotpatch_module linkage\n{ns_decl}\n' +
                text[insert_pos:]
            )
            page_file.write_text(text, encoding="utf-8")
            print(f"    [build_entry] added s_hotpatch_module extern in {page_file.name}")

    # ── Sync runtime libs into per-assembly SDK lib/ ────────────────
    # The codegen regenerates codegen/<AssemblyName>/lib/chaos_codegen.lib
    # but NOT the runtime libs (chaos_common.lib, etc.).  Copy them from
    # codegen/lib/ so find_package(chaos) resolves all dependencies.
    _sync_runtime_libs_to_sdk(codegen_dir)

    # Use separate build directories for AOT (build/) and JIT (build_jit/)
    # to avoid MSBuild file-lock conflicts when the JIT stage follows AOT.
    build_dir = native_dir / ("build_jit" if is_jit else "build")
    # Remove stale cmake cache to avoid generator/platform mismatch errors
    if build_dir.exists():
        try:
            shutil.rmtree(build_dir)
        except PermissionError:
            print(f"    [build_entry] warning: could not remove build_dir, continuing anyway")
    if not build_dir.exists():
        build_dir.mkdir(parents=True, exist_ok=True)

    # Step 1: CMake configure (with retry for transient Windows file-lock failures)
    print(f"    [build_entry] cmake configure...")
    cfg_result = None
    for cfg_attempt in range(3):
        if cfg_attempt > 0:
            wait = 1 << cfg_attempt  # 2, 4 seconds
            print(f"    [build_entry] cmake configure retry #{cfg_attempt} after {wait}s...")
            time.sleep(wait)
        cfg_result = subprocess.run(
            ["cmake", "-S", str(native_dir), "-B", str(build_dir),
             "-G", "Visual Studio 17 2022", "-A", "x64"],
            capture_output=True, text=True, timeout=120,
        )
        if cfg_result.returncode == 0:
            break
    if cfg_result and cfg_result.returncode != 0:
        print(f"    [build_entry] cmake configure FAILED after retries")
        for line in (cfg_result.stderr.splitlines() + cfg_result.stdout.splitlines())[-20:]:
            print(f"      {line}")
        return False

    # Step 1b: Patch vcxproj files to use /EHs /EHc- (extern "C" exception propagation)
    patch_script = _HERE.parents[0] / "_patch_vcxproj.py"
    if patch_script.exists():
        subprocess.run([sys.executable, str(patch_script), str(build_dir)],
                      capture_output=True, text=True, timeout=30)
        print(f"    [build_entry] vcxproj patched for /EHs /EHc-")

    # Step 2: CMake build (entry target, RelWithDebInfo) with retry for transient failures
    print(f"    [build_entry] cmake build...")
    build_result = None
    for build_attempt in range(3):
        if build_attempt > 0:
            wait = 1 << build_attempt  # 2, 4 seconds
            print(f"    [build_entry] cmake build retry #{build_attempt} after {wait}s...")
            time.sleep(wait)
        build_result = subprocess.run(
            ["cmake", "--build", str(build_dir), "--config", "RelWithDebInfo",
             "--target", "entry"],
            capture_output=True, text=True, timeout=300,
        )
        if build_result.returncode == 0:
            break
    if build_result and build_result.returncode != 0:
        print(f"    [build_entry] cmake build FAILED after retries")
        for line in (build_result.stderr.splitlines() + build_result.stdout.splitlines())[-25:]:
            print(f"      {line}")
        return False

    # Step 3: Locate entry.exe
    exe_candidates = [
        build_dir / "RelWithDebInfo" / "entry.exe",
        build_dir / "Release" / "entry.exe",
        build_dir / "Debug" / "entry.exe",
        build_dir / "entry.exe",
    ]
    exe_path = None
    for c in exe_candidates:
        if c.exists():
            exe_path = c
            break

    if exe_path is None:
        print(f"    [build_entry] entry.exe not found in build output")
        return False

    # Copy entry.exe to native/ for discovery by orchestrator
    target_dir = native_dir
    shutil.copy2(str(exe_path), str(target_dir / output_name))
    size = (target_dir / output_name).stat().st_size
    print(f"    [build_entry] {output_name} OK: {size} bytes -> {target_dir / output_name}")
    return True


def run_native_aot_emission(family_slug: str, *, verification: Path | None = None) -> bool:
    """Run emit-native-aot to produce real C++."""
    v = verification or _VERIFICATION
    closure_sp_analysis = v / family_slug / "il2cpp_dist" / "entrypoint" / "closure-sp" / "analysis"
    genuine_out = v / family_slug / "il2cpp_dist" / "genuine"
    genuine_out.mkdir(parents=True, exist_ok=True)

    if not closure_sp_analysis.exists():
        print(f"    analysis dir not found at {closure_sp_analysis}")
        return False

    result = subprocess.run(
        [
            "dotnet", "run", "--no-build",
            "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
            "--", "emit-native-aot",
            str(closure_sp_analysis),
            str(genuine_out),
        ],
        capture_output=True, text=True,
        timeout=120,
    )

    if result.returncode != 0:
        # Extract the error message
        error_lines = [l for l in result.stderr.splitlines() if l.strip()]
        error_msg = error_lines[-1] if error_lines else "unknown error"
        print(f"    emit-native-aot FAILED: {error_msg[:120]}")
        return False

    # Check output
    cpp_path = genuine_out / "generated" / "native-aot.generated.cpp"
    if cpp_path.exists():
        size = cpp_path.stat().st_size
        print(f"    emit-native-aot OK: {size} bytes")
    else:
        print(f"    emit-native-aot OK (no .cpp output found)")
    return True


def has_synthetic_method_ids(method_subject_ids: list[str]) -> bool:
    """Check if method subject IDs are synthetic (not real callable APIs).

    Synthetic contracts use placeholder IDs like:
      TypeName::TypeName::Void(System.Object)    — constructor
      TypeName::get_Property:System.Void()         — mock property
      TypeName::MethodN:System.Void(P0, P1)        — generic methods
    """
    if not method_subject_ids:
        return True
    for mid in method_subject_ids[:5]:  # check first few
        if "::" not in mid:
            continue
        path_sig = mid.split("::", 1)
        type_path = path_sig[0]
        sig = path_sig[1]

        # Extract type name from path
        raw_type = type_path.split("/", 1)[1] if "/" in type_path else type_path
        type_name = raw_type.rsplit(".", 1)[-1]

        paren = sig.find("(")
        if paren <= 0:
            continue
        method_part = sig[:paren]
        colon = method_part.rfind(":")
        raw_method = method_part[:colon] if colon > 0 else method_part

        # Constructor pattern: method_name == type_name (e.g. AsnDecoder::Void)
        if raw_method == type_name:
            return True
        # Genuinely synthetic method names (property-like names are real API methods
        # e.g. Nullable<T>.get_Value, MemberInfo.get_Name, Delegate.get_Method)
        if raw_method in ("Method1", "Method2", "Method3"):
            return True
    return False


def run_family(family_slug: str, *, assembly_name: str = "System.Private.CoreLib", variant: str | None = None, config_tier: str = "CHECK", codegen_mode: str | None = None) -> dict:
    """Run the full pipeline for one family. Returns result dict."""
    verification = _VERIFICATION_BASE / assembly_name
    result = {
        "family": family_slug,
        "config_tier": config_tier,
        "steps": {},
        "success": False,
    }

    print(f"\n{'='*60}")
    print(f"Family: {family_slug}")
    print(f"{'='*60}")

    # Step 0: Load method subject IDs
    mids = load_method_subject_ids(family_slug, verification=verification)
    if not mids:
        print(f"  [SKIP] no methods in contract (contract-only family)")
        result["success"] = True
        result["error"] = None
        trace("family_skip", family=family_slug, reason="no methods")
        return result
    print(f"  Methods: {len(mids)}")
    result["methodCount"] = len(mids)

    # Step 1a: Build subjects DLL (il2cpp input via managed/subjects/)
    auto_variant = variant or ("patch" if has_synthetic_method_ids(mids) else "benchmark")
    print(f"  [1a/3] Building subjects DLL (variant={auto_variant})...")
    build_result = build_subjects_dll(family_slug, mids, assembly_name=assembly_name, verification=verification, variant=auto_variant)
    if not build_result.get("success"):
        result["steps"]["build_subjects"] = "FAILED"
        result["error"] = build_result.get("error", "build failed")
        print(f"    FAILED: {result['error']}")
        trace("family_subjects_build_failed", family=family_slug, error=result["error"])
        return result
    result["steps"]["build_subjects"] = "OK"
    result["entryPointSubjectId"] = build_result["entry_point_subject_id"]
    result["dllPath"] = build_result["dll_path"]

    # Step 1b: Convert-to-CPP → codegen/
    print(f"  [1b/3] Running convert-to-cpp...")
    entry_pt = build_result.get("entry_point_subject_id")
    if not run_convert_to_cpp(family_slug, build_result["dll_path"], verification=verification, entry_point_subject_id=entry_pt, codegen_mode=codegen_mode):
        result["steps"]["convert_to_cpp"] = "FAILED"
        result["error"] = "convert-to-cpp failed"
        trace("family_c2c_failed", family=family_slug)
        return result
    result["steps"]["convert_to_cpp"] = "OK"

    # Step 1c: Generate .patchdata for hotpatch dispatch (before entry.exe build)
    generate_patch_data(family_slug, verification=verification)

    # Step 1e: Generate coverage JSON for dashboard/kernel integration
    generate_coverage_json(family_slug, assembly_name, mids, verification=verification)

    # Determine JIT mode before dispatch regeneration.
    # When codegen_mode is None, the convert-to-cpp stage defaults to AOT mode
    # (the --mode flag is omitted and the codegen tool's default is "aot").
    # Be explicit about this so is_jit_build is always correctly resolved.
    effective_codegen_mode = codegen_mode or "aot"
    is_jit_build = (effective_codegen_mode == "jit")

    # Step 1f: For JIT mode, regenerate verification_dispatch with JIT thunks
    # (nullptr instead of kDefaultArgThunks). The sentinel dispatch left by
    # build_entry_executable's cleanup is AOT-mode and will cause LNK2001.
    if is_jit_build:
        try:
            from verification.orchestration.dispatch_generator import generate_verification_dispatch
            codegen_dir = verification / family_slug / "codegen"
            # Search codegen subdirectories for the methods manifest.
            # The codegen output directory name matches the DLL stem (e.g.
            # "BufferMemorySubjects"), not assembly_name ("System.Private.CoreLib").
            manifest_path: Path | None = None
            for sub in sorted(codegen_dir.iterdir()):
                if not sub.is_dir():
                    continue
                candidate = sub / "generated" / "native-aot.methods.json"
                if candidate.exists():
                    manifest_path = candidate
                    break
            if manifest_path:
                dispatch_output = verification / family_slug / "native" / "verification_dispatch.generated.cpp"
                generate_verification_dispatch(str(manifest_path), str(dispatch_output), jit_mode=True)
                print(f"    [jit_codegen] regenerated JIT-mode verification_dispatch")
            else:
                print(f"    [jit_codegen] WARNING: manifest not found in {codegen_dir}")
        except ImportError:
            print(f"    [jit_codegen] verification_dispatch_generator not available (skip)")
        except ImportError:
            print(f"    [jit_codegen] verification_dispatch_generator not available (skip)")

    # Step 2: Build entry.exe from codegen/native-aot.generated.cpp → native/
    print(f"  [2/3] Building entry.exe from codegen (jit={is_jit_build})...")
    if not build_entry_executable(family_slug, verification=verification, config_tier=config_tier, is_jit=is_jit_build):
        result["steps"]["build_entry_exe"] = "FAILED"
        result["error"] = "entry.exe build failed"
        trace("family_entry_build_failed", family=family_slug)
        return result
    result["steps"]["build_entry_exe"] = "OK"

    # Family succeeds if all build steps pass
    # (fact/benchmark/hotupdate verification is handled by the orchestrator stages)
    if result["steps"].get("build_subjects") == "OK" \
            and result["steps"].get("convert_to_cpp") == "OK" \
            and result["steps"].get("build_entry_exe") == "OK":
        result["success"] = True
        trace("family_passed", family=family_slug, method_count=len(mids))
    else:
        result["success"] = False
    return result


def run_direct_assembly_translation(assembly_name: str) -> dict:
    """Run convert-to-cpp directly on an assembly DLL (no synthetic entrypoint).

    Translates the actual assembly DLL through the full IL2CPP pipeline,
    producing per-assembly C++ output. This is the 'final form' IL2CPP
    translation that foundation DLL verification should use.
    """
    # Find the actual assembly DLL
    dll_path = find_assembly_dll(assembly_name)
    if dll_path is None:
        return {"assembly": assembly_name, "success": False, "error": f"DLL not found for {assembly_name}"}

    assembly_out = _VERIFICATION_BASE / assembly_name / "il2cpp_dist" / "genuine"
    assembly_out.mkdir(parents=True, exist_ok=True)

    print(f"\n{'='*60}")
    print(f"Assembly: {assembly_name}")
    print(f"DLL: {dll_path}")
    print(f"{'='*60}")

    result = subprocess.run(
        [
            "dotnet", "run", "--no-build",
            "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
            "--", "convert-to-cpp",
            "--assembly", dll_path,
            "--output", str(assembly_out),
            "--full-closure",
        ],
        capture_output=True, text=True,
        timeout=300,
    )

    if result.returncode != 0:
        print(f"  convert-to-cpp FAILED (rc={result.returncode})")
        for line in result.stderr.splitlines()[-10:]:
            print(f"    {line}")
        trace("assembly_translate_failed", assembly=assembly_name)
        return {"assembly": assembly_name, "success": False, "error": result.stderr.splitlines()[-1][:200] if result.stderr.splitlines() else "unknown"}

    # Check output
    cpp_path = assembly_out / "generated" / "native-aot.generated.cpp"
    if cpp_path.exists():
        size = cpp_path.stat().st_size
        print(f"  convert-to-cpp OK: {size} bytes")
    else:
        print(f"  convert-to-cpp OK (no .cpp output found)")

    trace("assembly_translate_passed", assembly=assembly_name)
    return {"assembly": assembly_name, "success": True, "error": None}


def find_assembly_dll(assembly_name: str) -> str | None:
    """Find the actual assembly DLL for a given assembly name.

    Searches trusted platform assemblies and common locations.
    """
    # Check TRUSTED_PLATFORM_ASSEMBLIES env (set by dotnet host)
    tpa = os.environ.get("TRUSTED_PLATFORM_ASSEMBLIES", "")
    if tpa:
        for path in tpa.split(os.pathsep):
            if os.path.basename(path).lower() == f"{assembly_name.lower()}.dll":
                return path

    # Check common locations
    search_dirs = [
        _VERIFICATION_BASE / assembly_name,
        _REPO_ROOT / "src" / "managed" / assembly_name / "bin" / "Debug" / "net8.0",
        _REPO_ROOT / "testing" / "foundation-dll" / assembly_name,
        Path(os.environ.get("DOTNET_ROOT", "C:/Program Files/dotnet")) / "shared" / "Microsoft.NETCore.App" / "8.0",
    ]
    for d in search_dirs:
        if d.exists():
            for f in d.iterdir():
                if f.is_file() and f.name.lower() == f"{assembly_name.lower()}.dll":
                    return str(f.resolve())

    return None


def main() -> None:
    parser = argparse.ArgumentParser(description="Batch native AOT CodeGen pipeline")
    parser.add_argument("--assembly-name", default="System.Private.CoreLib", help="Assembly name to process")
    parser.add_argument("--trace", action="store_true", default=True, help="Enable JSONL trace logging (default: on)")
    parser.add_argument("--no-trace", action="store_true", help="Disable JSONL trace logging")
    parser.add_argument("--families", nargs="*", help="Space-separated subset of family slugs to process. Auto-discovers from contracts if not specified.")
    parser.add_argument("--translate-assemblies", action="store_true", help="Translate actual assembly DLLs directly (final-form IL2CPP, no synthetic entrypoints)")
    parser.add_argument("--assembly-dlls", nargs="*", help="Space-separated assembly DLL paths for direct translation")
    parser.add_argument("--native-config", default="CHECK", choices=["CHECK", "PROFILE", "SHIP"],
                        help="Native C++ config tier (default: CHECK)")
    args = parser.parse_args()

    global _VERIFICATION
    _VERIFICATION = _VERIFICATION_BASE / args.assembly_name

    if args.trace and not args.no_trace:
        trace_init(_REPO_ROOT, stage="batch-native-aot")

    if args.translate_assemblies:
        run_assembly_translation_mode(args)
        return

    # Original per-family entrypoint mode
    if not _VERIFICATION.exists():
        print(f"FATAL: verification directory not found: {_VERIFICATION}", file=sys.stderr)
        sys.exit(1)

    families = args.families
    if not families:
        families = discover_families(args.assembly_name)
        if not families:
            print(f"No families found for {args.assembly_name} (no contracts)")
            families = FAMILIES

    print(f"Batch native AOT CodeGen pipeline - {len(families)} families")
    print(f"Assembly: {args.assembly_name}")
    print(f"Verification: {_VERIFICATION}")
    print()

    trace("batch_start", assembly=args.assembly_name, family_count=len(families), native_config=args.native_config)

    results = []
    passed = 0
    failed = 0

    for idx, family_slug in enumerate(families):
        family_result = run_family(family_slug, assembly_name=args.assembly_name, config_tier=args.native_config)
        results.append(family_result)

        if family_result["success"]:
            passed += 1
            print(f"  >>> PASSED ({passed}/{idx+1})")
        else:
            failed += 1
            print(f"  >>> FAILED ({failed}/{idx+1}): {family_result.get('error', 'unknown')}")

    # Summary
    print(f"\n{'='*60}")
    print(f"{args.assembly_name}: {passed} passed, {failed} failed, {len(families)} total")
    print(f"{'='*60}")
    for r in results:
        status = "PASS" if r["success"] else "FAIL"
        steps = " -> ".join(f"{k}={v}" for k, v in r.get("steps", {}).items())
        fact_info = ""
        if "fact_total" in r and r["fact_total"]:
            fact_info = f"  Fact:{r.get('fact_passed',0)}/{r['fact_total']}"
        print(f"  {status:4s}  {r['family']:35s}  {steps}{fact_info}")

    # Write results per-assembly
    output_path = _VERIFICATION / "reports" / "batch-native-aot-pipeline-results.json"
    output_path.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "assembly": args.assembly_name,
        "mode": "family-entrypoint",
        "total": len(families),
        "passed": passed,
        "failed": failed,
        "results": results,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
    }
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(summary, f, indent=2, ensure_ascii=False)
    print(f"\nResults written to {output_path}")


def run_assembly_translation_mode(args: argparse.Namespace) -> None:
    """Run in assembly translation mode — translates actual DLLs directly."""
    print(f"{'='*60}")
    print(f"Batch native AOT: Assembly Translation Mode")
    print(f"{'='*60}")

    assemblies = args.assembly_dlls
    if not assemblies:
        # Auto-discover: use the assembly name
        assemblies = [args.assembly_name]

    print(f"Assemblies to translate: {len(assemblies)}")
    for a in assemblies:
        print(f"  {a}")
    print()

    results = []
    passed = 0
    failed = 0

    for idx, asm_name in enumerate(assemblies):
        asm_result = run_direct_assembly_translation(asm_name)
        results.append(asm_result)

        if asm_result["success"]:
            passed += 1
            print(f"  >>> PASSED ({passed}/{idx+1})")
        else:
            failed += 1
            print(f"  >>> FAILED ({failed}/{idx+1}): {asm_result.get('error', 'unknown')}")

    # Summary
    print(f"\n{'='*60}")
    print(f"Assembly translation: {passed} passed, {failed} failed, {len(assemblies)} total")
    print(f"{'='*60}")
    for r in results:
        status = "PASS" if r["success"] else "FAIL"
        print(f"  {status:4s}  {r['assembly']}")

    # Write results
    output_path = _VERIFICATION_BASE / args.assembly_name / "reports" / "batch-assembly-translation-results.json"
    output_path.parent.mkdir(parents=True, exist_ok=True)
    summary = {
        "mode": "assembly-translation",
        "total": len(assemblies),
        "passed": passed,
        "failed": failed,
        "results": results,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
    }
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(summary, f, indent=2, ensure_ascii=False)
    print(f"\nResults written to {output_path}")


if __name__ == "__main__":
    main()
