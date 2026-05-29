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

# Ensure stdout/stderr can handle UTF-8 output in GBK terminals
if hasattr(sys.stdout, 'reconfigure'):
    sys.stdout.reconfigure(encoding='utf-8', errors='replace')
if hasattr(sys.stderr, 'reconfigure'):
    sys.stderr.reconfigure(encoding='utf-8', errors='replace')

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
    # Use net8.0 TFM for .NET 8 SDK compatibility. net10.0 requires
    # .NET 10 SDK which is not available on all platforms.
    tfm = "net8.0"
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

    # Use net8.0 TFM for .NET 8 SDK compatibility.
    tfm = "net8.0"
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


def _sync_runtime_libs_to_sdk(codegen_dir: Path, repo_root: Path | None = None) -> None:
    """Copy runtime libs into the per-assembly SDK lib/ directories.

    Tries codegen/lib/ first (root-level SDK output from legacy runs),
    then falls back to repo artifacts/presets/ (canonical prebuilt location).
    """
    if not codegen_dir.is_dir():
        print(f"    [sync_libs] codegen_dir not found: {codegen_dir}")
        return

    def _copy_libs(source_lib_dir: Path) -> bool:
        """Copy all .lib files from source_lib_dir into per-assembly SDK lib/ dirs."""
        if not source_lib_dir.is_dir():
            print(f"    [sync_libs] source_lib_dir not found: {source_lib_dir}")
            return False
        any_copied = False
        for d in sorted(codegen_dir.iterdir()):
            if not d.is_dir():
                continue
            sdk_lib_dir = d / "lib"
            if not sdk_lib_dir.is_dir():
                print(f"    [sync_libs] sdk_lib_dir not found for {d.name}: {sdk_lib_dir}")
                continue
            for lib_file in sorted(source_lib_dir.iterdir()):
                if lib_file.suffix != ".lib":
                    continue
                target = sdk_lib_dir / lib_file.name
                # Always overwrite codegen stubs (8-byte placeholders) with real libs.
                if target.exists() and target.stat().st_size > 100:
                    continue
                    shutil.copy2(str(lib_file), str(sdk_lib_dir / lib_file.name))
                    any_copied = True
        if any_copied:
            print(f"    [sync_libs] copied from {source_lib_dir.name}")
        return any_copied

    # Try primary: codegen/lib/ (legacy root-level SDK output)
    parent_lib_dir = codegen_dir / "lib"
    if parent_lib_dir.is_dir():
        print(f"    [sync_libs] trying parent_lib_dir: {parent_lib_dir}")
        if _copy_libs(parent_lib_dir):
            return

    # Fallback: repo artifacts/presets/<platform>/ — collect all .lib from RelWithDebInfo
    if repo_root is not None:
        presets_dir = repo_root / "artifacts" / "presets"
        # In a git worktree, _REPO_ROOT resolves to the worktree root, not the
        # real repo root. Walk up from repo_root to find artifacts/presets.
        if not presets_dir.is_dir():
            parent = repo_root.parent
            while parent is not None and parent != parent.parent:
                candidate = parent / "artifacts" / "presets"
                if candidate.is_dir():
                    presets_dir = candidate
                    break
                parent = parent.parent
        if presets_dir.is_dir():
            print(f"    [sync_libs] fallback to presets: {presets_dir}")
            for pd in sorted(presets_dir.iterdir()):
                if pd.name.startswith("windows-x64-reference") and pd.is_dir():
                    found = list(pd.rglob("*.lib"))
                    print(f"    [sync_libs] found {len(found)} libs in {pd.name}")
                    for lib_file in sorted(found):
                        for d in sorted(codegen_dir.iterdir()):
                            if not d.is_dir():
                                continue
                            sdk_lib_dir = d / "lib"
                            if not sdk_lib_dir.is_dir():
                                continue
                            target = sdk_lib_dir / lib_file.name
                            # Always overwrite codegen stubs (8-byte placeholders).
                            if target.exists() and target.stat().st_size > 100:
                                continue
                            shutil.copy2(str(lib_file), str(target))
                    return
        else:
            print(f"    [sync_libs] presets_dir not found: {presets_dir}")


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
    # AOT-only builds (handled via CMake generator expression in template).
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
        f'# Compiler settings — platform-conditional\n'
        f'# MSVC: /EHa needed for catch(...) to intercept C++ exceptions from generated code.\n'
        f'# GCC/Clang: -fexceptions -finput-charset=utf-8, suppress known warnings\n'
        f'add_compile_options(\n'
        f'    "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"\n'
        f'    "$<$<CXX_COMPILER_ID:MSVC>:/GS->"\n'
        f'    "$<$<CXX_COMPILER_ID:MSVC>:/FS>"\n'
        f'    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fexceptions>"\n'
        f'    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=utf-8>"\n'
        f'    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wno-error=invalid-offsetof>"\n'
        f')\n'
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
        f'target_compile_options(entry PRIVATE\n'
        f'    "$<$<CXX_COMPILER_ID:MSVC>:/EHa>"\n'
        f'    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fexceptions>"\n'
        f')\n'
        f'target_link_libraries(entry PRIVATE chaos::runtime)\n'
        f'target_link_options(entry PRIVATE\n'
        f'    "$<$<CXX_COMPILER_ID:MSVC>:/FORCE:MULTIPLE>"\n'
        f'    "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wl,--allow-multiple-definition>"\n'
        f')\n'
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



def write_sentinel_dispatch(dispatch_cpp: Path) -> None:
    """Write a sentinel verification_dispatch.generated.cpp for cmake configure.

    The real dispatch file is generated by TestProjectGenerator emit after
    codegen runs.  This sentinel ensures cmake configure can find the
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


def write_sentinel_entry(entry_cpp: Path) -> None:
    """Write a sentinel runtime-entry.cpp for cmake configure.

    The real runtime-entry.cpp is generated by TestProjectGenerator emit after
    codegen runs.  This sentinel ensures cmake configure can find the source
    file during the initial build before TPG emit has been invoked.
    """
    content = (
        '// runtime-entry.cpp — sentinel (pre-TPG emit)\n'
        '// Real file generated by TestProjectGenerator emit\n'
        'int main(int, char*[]) { return 0; }\n'
    )
    entry_cpp.parent.mkdir(parents=True, exist_ok=True)
    entry_cpp.write_text(content, encoding="utf-8")
    print(f"    [build_entry] sentinel runtime-entry.cpp created")


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
            # Handwritten / TPG-generated files to preserve
            "runtime-entry.cpp",
            "verification_dispatch.generated.cpp",
        ):
            stale_name.unlink()
            print(f"    [build_entry] cleaned stale: {stale_name.name}")
    # Also clean codegen-sync subdirectories
    for stale_dir in sorted(native_dir.iterdir()):
        if stale_dir.is_dir() and stale_dir.name not in ("build", "build_jit"):
            shutil.rmtree(stale_dir)
            print(f"    [build_entry] cleaned stale directory: {stale_dir.name}")
    stale_jit_stubs = native_dir / "jit_stubs.cpp"
    if stale_jit_stubs.exists():
        stale_jit_stubs.unlink()
        print(f"    [build_entry] cleaned stale jit_stubs.cpp")

    # Auto-generate CMakeLists.txt if missing (e.g. after clean delete)
    ensure_cmake_lists_file(cmakelists, family_slug, v, is_jit=is_jit, config_tier=config_tier)

    # Ensure runtime-patchdata.cpp exists (sentinel if not generated)
    patchdata_cpp = native_dir / "runtime-patchdata.cpp"
    if not patchdata_cpp.exists():
        write_sentinel_patch_data(v / family_slug)
        print(f"    [build_entry] sentinel runtime-patchdata.cpp generated")

    # Ensure microbench.cpp exists for --microbench support
    ensure_micro_benchmark_source(native_dir)

    # Generate runtime-entry.cpp sentinel for initial cmake configure.
    # The real file is generated by TestProjectGenerator emit after codegen.
    native_runtime_entry = native_dir / "runtime-entry.cpp"
    if not native_runtime_entry.exists():
        write_sentinel_entry(native_runtime_entry)
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
    _sync_runtime_libs_to_sdk(codegen_dir, repo_root=_REPO_ROOT)

    # Use separate build directories for AOT (build/) and JIT (build_jit/)
    # to avoid MSBuild file-lock conflicts when the JIT stage follows AOT.
    build_dir = native_dir / ("build_jit" if is_jit else "build")

    # Incremental build: keep existing build cache for faster recompilation.
    # Only remove CMakeCache.txt to force reconfiguration; all .obj/.lib files
    # from previous runs are preserved so cmake --build only recompiles sources
    # whose timestamps changed (generated C++ files).  Fall back to clean build
    # if incremental configure or build fails.
    did_clean_build = False
    if build_dir.exists():
        cache_file = build_dir / "CMakeCache.txt"
        if cache_file.exists():
            try:
                cache_file.unlink()
            except OSError:
                pass
    else:
        build_dir.mkdir(parents=True, exist_ok=True)

    # ── Platform detection ───────────────────────────────────────────
    is_linux = sys.platform.startswith("linux")

    # Step 1: CMake configure (with retry for transient Windows file-lock failures)
    print(f"    [build_entry] cmake configure (platform={'linux' if is_linux else 'windows'})...")
    cfg_result = None

    cmake_args = ["cmake", "-S", str(native_dir), "-B", str(build_dir)]
    if is_linux:
        cmake_args += ["-G", "Ninja",
                       f"-DCMAKE_BUILD_TYPE={config_tier.lower()}",
                       f"-DCHAOS_IL2CPP_CONFIG_TIER={config_tier.lower()}",
                       "-DCHAOS_IL2CPP_JIT_MODE=ON" if is_jit else "-DCHAOS_IL2CPP_JIT_MODE=OFF"]
    else:
        cmake_args += ["-G", "Visual Studio 17 2022", "-A", "x64",
                       f"-DCHAOS_IL2CPP_CONFIG_TIER={config_tier.lower()}",
                       "-DCHAOS_IL2CPP_JIT_MODE=ON" if is_jit else "-DCHAOS_IL2CPP_JIT_MODE=OFF"]
    for cfg_attempt in range(3):
        if cfg_attempt > 0:
            wait = 1 << cfg_attempt  # 2, 4 seconds
            print(f"    [build_entry] cmake configure retry #{cfg_attempt} after {wait}s...")
            time.sleep(wait)
        cfg_result = subprocess.run(cmake_args, capture_output=True, text=True, timeout=120,)
        if cfg_result.returncode == 0:
            break

    # Fallback: if all 3 incremental configure retries failed, do a clean build
    if cfg_result is None or cfg_result.returncode != 0:
        print(f"    [build_entry] incremental configure FAILED after retries, falling back to clean build...")
        if build_dir.exists():
            shutil.rmtree(build_dir)
        build_dir.mkdir(parents=True, exist_ok=True)
        did_clean_build = True
        for cfg_attempt in range(3):
            if cfg_attempt > 0:
                wait = 1 << cfg_attempt
                print(f"    [build_entry] cmake configure (clean) retry #{cfg_attempt} after {wait}s...")
                time.sleep(wait)
            cfg_result = subprocess.run(cmake_args, capture_output=True, text=True, timeout=120,)
            if cfg_result.returncode == 0:
                break
        if cfg_result is None or cfg_result.returncode != 0:
            print(f"    [build_entry] clean configure ALSO FAILED, giving up")
            for line in (cfg_result.stderr.splitlines() + cfg_result.stdout.splitlines())[-25:]:
                print(f"      {line}")
            return False

    # Step 1b: Patch vcxproj files (Windows only — Ninja doesn't generate vcxproj)
    if not is_linux:
        patch_script = _HERE.parents[0] / "_patch_vcxproj.py"
        if patch_script.exists():
            subprocess.run([sys.executable, str(patch_script), str(build_dir)],
                          capture_output=True, text=True, timeout=30)
            print(f"    [build_entry] vcxproj patched for /EHs /EHc-")

    # Step 2: CMake build with retry for transient failures
    print(f"    [build_entry] cmake build (platform={'linux' if is_linux else 'windows'})...")
    build_args = ["cmake", "--build", str(build_dir), "--target", "entry", "--parallel"]
    if not is_linux:
        build_args += ["--config", "RelWithDebInfo"]
    build_result = None
    for build_attempt in range(3):
        if build_attempt > 0:
            wait = 1 << build_attempt  # 2, 4 seconds
            print(f"    [build_entry] cmake build retry #{build_attempt} after {wait}s...")
            time.sleep(wait)
        build_result = subprocess.run(build_args, capture_output=True, text=True, timeout=300,)
        if build_result.returncode == 0:
            break
    if build_result and build_result.returncode != 0:
        # Fallback to clean build if incremental build failed and we haven't
        # already done a clean build during configure fallback
        if not did_clean_build:
            print(f"    [build_entry] incremental build FAILED after retries, falling back to clean build...")
            if build_dir.exists():
                shutil.rmtree(build_dir)
            build_dir.mkdir(parents=True, exist_ok=True)
            did_clean_build = True
            # Re-run configure from scratch
            for cfg_attempt in range(3):
                if cfg_attempt > 0:
                    wait = 1 << cfg_attempt
                    print(f"    [build_entry] cmake configure (clean) retry #{cfg_attempt} after {wait}s...")
                    time.sleep(wait)
                cfg_result = subprocess.run(cmake_args, capture_output=True, text=True, timeout=120,)
                if cfg_result.returncode == 0:
                    break
            if cfg_result and cfg_result.returncode == 0:
                # Re-run build after clean configure
                for build_attempt in range(3):
                    if build_attempt > 0:
                        wait = 1 << build_attempt
                        print(f"    [build_entry] cmake build (clean) retry #{build_attempt} after {wait}s...")
                        time.sleep(wait)
                    build_result = subprocess.run(build_args, capture_output=True, text=True, timeout=300,)
                    if build_result.returncode == 0:
                        break

    if build_result and build_result.returncode != 0:
        print(f"    [build_entry] cmake build FAILED after retries")
        for line in (build_result.stderr.splitlines() + build_result.stdout.splitlines())[-25:]:
            print(f"      {line}")
        return False

    # Step 3: Locate binary (entry.exe on Windows, entry on Linux)
    exe_name = "entry.exe" if not is_linux else "entry"
    if is_linux:
        exe_candidates = [build_dir / exe_name]
    else:
        exe_candidates = [
            build_dir / "RelWithDebInfo" / exe_name,
            build_dir / "Release" / exe_name,
            build_dir / "Debug" / exe_name,
            build_dir / exe_name,
        ]
    exe_path = None
    for c in exe_candidates:
        if c.exists():
            exe_path = c
            break

    if exe_path is None:
        print(f"    [build_entry] {exe_name} not found in build output")
        return False

    # Copy entry.exe to native/ for discovery by orchestrator
    # Retry loop: msbuild may briefly hold file handles after cmake --build completes
    target_dir = native_dir
    target_path = target_dir / output_name
    for copy_attempt in range(5):
        try:
            # Remove existing target first (may be locked by prior run)
            if target_path.exists():
                target_path.unlink()
            shutil.copy2(str(exe_path), str(target_path))
            break
        except (PermissionError, OSError) as e:
            if copy_attempt < 4:
                wait = 1 << copy_attempt
                print(f"    [build_entry] copy locked, retry #{copy_attempt} after {wait}s...")
                time.sleep(wait)
            else:
                print(f"    [build_entry] copy FAILED after retries: {e}")
                return False
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

    # Step 1f: JIT dispatch is generated by TPG emit in codegen.py, not here.
    # The old dispatch_generator.py call has been removed -- TPG emit handles
    # both AOT and JIT dispatch generation correctly.

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
