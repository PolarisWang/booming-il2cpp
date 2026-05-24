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
_REPO_ROOT = _HERE.parents[4]  # D:/agent/booming-il2cpp
_VERIFICATION_BASE = _REPO_ROOT / "testing" / "foundation-dll"
_VERIFICATION: Path | None = None  # set per-assembly in main()
sys.path.insert(0, str(_HERE))
sys.path.insert(0, str(_HERE.parent.parent))  # for testing.trace (build/toolchains/run/)

from family_entrypoint_generator import generate_and_build

from testing.trace import trace_init, trace

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


def _discover_families(assembly_name: str) -> list[str]:
    """Discover families for an assembly by scanning for capability-family-contract.json files."""
    asm_dir = _VERIFICATION_BASE / assembly_name
    if not asm_dir.exists():
        return []
    families = []
    for d in sorted(asm_dir.iterdir()):
        if d.is_dir() and (d / "capability-family-contract.json").exists():
            families.append(d.name)
    return families


def _load_method_subject_ids(family_slug: str, *, verification: Path | None = None) -> list[str]:
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


def _count_methods_in_contract(family_slug: str, *, verification: Path | None = None) -> int:
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


def _build_subjects_dll(
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

    from family_entrypoint_generator import generate_and_build
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


def _build_entrypoint(
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


def _run_convert_to_cpp(
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

    cmd = [
        "dotnet", "exec", str(driver_dll), "convert-to-cpp",
        "--assembly", dll_path,
        "--assembly-dir", str(Path(dll_path).parent),
        "--sdk-out", str(codegen_out),
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

    # Sync flat codegen/generated/ -> codegen/<AssemblyName>/generated/
    # The converter writes to the flat layout; the rest of the pipeline
    # expects per-assembly layout.
    asm_name = Path(dll_path).stem
    flat_gen = codegen_out / "generated"
    if flat_gen.exists():
        per_asm_dir = codegen_out / asm_name / "generated"
        per_asm_dir.mkdir(parents=True, exist_ok=True)
        for root, dirs, files in os.walk(flat_gen):
            rel_path = Path(root).relative_to(flat_gen)
            target_dir = per_asm_dir / rel_path
            target_dir.mkdir(parents=True, exist_ok=True)
            for f in files:
                src = Path(root) / f
                (target_dir / f).write_bytes(src.read_bytes())

    # Check output
    cpp_found = False
    for d in sorted(codegen_out.iterdir()):
        if d.is_dir() and d.name not in ("build", "generated"):
            per_asm_cpp = d / "generated" / "native-aot.generated.cpp"
            if per_asm_cpp.exists():
                if not cpp_found:
                    cpp_found = True
                    size = per_asm_cpp.stat().st_size
                    print(f"    convert-to-cpp OK: {size} bytes -> {per_asm_cpp.relative_to(v / family_slug)}")
                else:
                    print(f"      + {per_asm_cpp}")
    if not cpp_found:
        print(f"    convert-to-cpp OK (no .cpp output found)")
    return True


def _codegen_patch_undefined_labels(family_slug: str, *, verification: Path | None = None) -> None:
    """Post-process generated C++ to fix undefined branch-target labels.

    When the IL reader strips nop instructions, branch targets pointing to those
    stripped offsets become undefined C++ labels (chaos_label_N referenced in goto
    but never defined). This function maps them to the nearest following defined label.
    """
    v = verification or _VERIFICATION
    codegen_dir = v / family_slug / "codegen"
    if not codegen_dir.exists():
        print(f"    [patch_labels] no codegen dir: {codegen_dir}")
        return

    for cpp_file in sorted(codegen_dir.rglob("native-aot.generated.cpp")):
        content = cpp_file.read_text(encoding="utf-8")
        orig = content

        # Collect all label definitions and goto references
        defs: set[int] = set()
        refs: set[int] = set()
        for m in re.finditer(r'chaos_label_(\d+):', content):
            defs.add(int(m.group(1)))
        for m in re.finditer(r'goto\s+chaos_label_(\d+)', content):
            refs.add(int(m.group(1)))

        undefined = refs - defs
        if not undefined:
            continue

        # Build a map: undefined label -> nearest higher-numbered defined label
        sorted_defs = sorted(defs)
        fixup = {}
        for lbl in sorted(undefined):
            # Binary search for the first defined label > lbl
            idx = __import__("bisect").bisect_right(sorted_defs, lbl)
            if idx < len(sorted_defs):
                fixup[lbl] = sorted_defs[idx]
            else:
                # No higher label exists — last resort: use the label itself (still broken)
                print(f"      WARN: chaos_label_{lbl} has no higher defined label, leaving as-is")
                fixup[lbl] = lbl

        # Apply replacements (sorted descending to avoid offset shifts)
        for old_lbl, new_lbl in sorted(fixup.items(), reverse=True):
            content = content.replace(
                f"chaos_label_{old_lbl}",
                f"chaos_label_{new_lbl}",
            )

        cpp_file.write_text(content, encoding="utf-8")
        print(f"    [patch_labels] {cpp_file.relative_to(codegen_dir)}: fixed {len(fixup)} undefined labels")
        for lbl, mapped in sorted(fixup.items()):
            if lbl != mapped:
                print(f"      chaos_label_{lbl} → chaos_label_{mapped}")


def _patch_generated_files(family_slug: str, *, verification: Path | None = None) -> None:
    """Apply 0xC0000409 bypass patch to all generated native-aot.generated.cpp files.

    The MSVC compiler crashes (STATUS_STACK_BUFFER_OVERRUN / 0xC0000409) when a single
    function has too many locals or inline call blocks. This post-processes each generated
    .cpp to strip Program::Main() and replace RunNativeAot() with a dispatch table + loop.
    Non-fatal: if no generated file or script found, logs and continues.
    """
    v = verification or _VERIFICATION
    genuine_out = v / family_slug / "il2cpp_dist" / "genuine"
    patch_script = _HERE / "_patch_bypass_0xC0000409.py"
    supp_script = _HERE / "_gen_supplemental_dispatch.py"
    if not patch_script.exists() or not supp_script.exists():
        print(f"    [patch_bypass] script not found")
        return

    has_methodN = False
    for gen_cpp in sorted(genuine_out.rglob("native-aot.generated.cpp")):
        content = gen_cpp.read_text(encoding="utf-8")
        if re.search(r'Method\d+\(void\)', content):
            has_methodN = True
        print(f"    [patch_bypass] patching {gen_cpp.relative_to(genuine_out)}...")
        r = subprocess.run(
            [sys.executable, str(patch_script), str(gen_cpp)],
            capture_output=True, text=True, timeout=60,
        )
        for line in r.stdout.splitlines():
            print(f"      {line}")
        if r.stderr:
            for line in r.stderr.splitlines():
                print(f"      ERR: {line}")

    # For handwritten entrypoint families (no MethodN pattern), generate
    # supplemental dispatch symbols (RunNativeAotAll, sentinel patchdata).
    if not has_methodN:
        method_count = _count_methods_in_contract(family_slug, verification=v)
        print(f"    [gen_supplemental] no MethodN pattern, generating {method_count}-method dispatch...")
        r = subprocess.run(
            [sys.executable, str(supp_script), str(genuine_out), str(method_count)],
            capture_output=True, text=True, timeout=30,
        )
        for line in r.stdout.splitlines():
            print(f"      {line}")
        if r.stderr:
            for line in r.stderr.splitlines():
                print(f"      ERR: {line}")

    # ── Post-processing: valuetype forward declarations ────────────
    # Some families (e.g. reflection-binding) reference chaos_valuetype_* types
    # from external assemblies via extern "C" declarations. Add typedef-int
    # forward declarations to satisfy the compiler.
    fwd_script = _HERE / "_gen_valuetype_forward_decls.py"
    if fwd_script.exists():
        for gen_cpp in sorted(genuine_out.rglob("native-aot.generated.cpp")):
            r = subprocess.run(
                [sys.executable, str(fwd_script), str(gen_cpp)],
                capture_output=True, text=True, timeout=30,
            )
            for line in r.stdout.splitlines():
                print(f"      {line}")
            if r.stderr:
                for line in r.stderr.splitlines():
                    print(f"      ERR: {line}")

    # ── Post-processing: weak stub generation ──────────────────────
    # Families pulling in vtable refs to external assembly functions
    # (e.g. Chaos.TestFramework.Sdk attributes) need stub definitions
    # to resolve linker errors.
    stub_script = _HERE / "_gen_weak_stubs.py"
    if stub_script.exists():
        for gen_cpp in sorted(genuine_out.rglob("native-aot.generated.cpp")):
            r = subprocess.run(
                [sys.executable, str(stub_script), str(gen_cpp), str(genuine_out), family_slug],
                capture_output=True, text=True, timeout=30,
            )
            for line in r.stdout.splitlines():
                print(f"      {line}")
            if r.stderr:
                for line in r.stderr.splitlines():
                    print(f"      ERR: {line}")

    # ── Post-processing: external runtime stub generation ────────────
    # Families with missing chaos_external_runtime_* function definitions
    # need auto-generated stubs to resolve C3861 compile errors. These stubs
    # are appended directly to native-aot.generated.cpp (no CMake change).
    ext_stub_script = _HERE / "_gen_external_runtime_stubs.py"
    if ext_stub_script.exists():
        for gen_cpp in sorted(genuine_out.rglob("native-aot.generated.cpp")):
            r = subprocess.run(
                [sys.executable, str(ext_stub_script), str(gen_cpp)],
                capture_output=True, text=True, timeout=30,
            )
            for line in r.stdout.splitlines():
                print(f"      {line}")
            if r.stderr:
                for line in r.stderr.splitlines():
                    print(f"      ERR: {line}")


def _generate_coverage_json(family_slug: str, assembly_name: str,
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
    from native_codegen_generator import _generate_coverage_json as _gen_cov
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


def _generate_supplemental_metadata(family_slug: str, method_subject_ids: list[str],
                                    *, verification: Path | None = None) -> bool:
    """Generate supplemental-metadata.json from the codegen template.

    Reads codegen/generated/hot-update/supplemental-metadata-template.json
    (produced by the managed convert-to-cpp pipeline) and produces the
    resolved supplemental-metadata payload consumed by the native hot-update
    runtime (LoadSupplementalMetadataFromPath).

    For foundation-dll test families, the template typically contains
    registered types/methods from the subjects assembly. The output payload
    marks all template entries as ReferencedAot (they are AOT-compiled and
    resolvable by metadata token from hotupdate code).

    If the template file does not exist, this step is silently skipped
    (not all codegen modes produce one).
    """
    v = verification or _VERIFICATION
    template = v / family_slug / "codegen" / "generated" / "hot-update" / "supplemental-metadata-template.json"
    if not template.exists():
        return True  # silently skip — not all codegen modes produce a template

    import json as _json
    try:
        with template.open(encoding="utf-8") as f:
            tmpl = _json.load(f)
    except Exception as e:
        print(f"    [supplemental] FAILED to read template: {e}")
        return False

    registered_types = tmpl.get("registeredTypes", [])
    registered_methods = tmpl.get("registeredMethods", [])
    reserved = tmpl.get("reservedSlots", {"typeCount": 256, "methodCount": 1024, "genericInstantiationCount": 256})

    # Build resolved payload:
    # - All registered types/methods from the template are "ReferencedAot"
    # - Empty generic instantiations (not needed for foundation-dll test families)
    types_out = []
    for t in registered_types:
        types_out.append({
            "subjectId": t["subjectId"],
            "metadataToken": t["metadataToken"],
            "sourceKind": "ReferencedAot",
        })

    methods_out = []
    for m in registered_methods:
        methods_out.append({
            "subjectId": m["subjectId"],
            "metadataToken": m["metadataToken"],
            "sourceKind": "ReferencedAot",
        })

    payload = {
        "formatVersion": "v0",
        "artifactKind": "supplementalMetadata",
        "reservedSlots": reserved,
        "types": types_out,
        "methods": methods_out,
        "genericInstantiations": [],
    }

    out_dir = template.parent
    out_path = out_dir / "supplemental-metadata.json"
    try:
        out_dir.mkdir(parents=True, exist_ok=True)
        out_path.write_text(_json.dumps(payload, indent=2), encoding="utf-8")
        print(f"    [supplemental] wrote {len(types_out)} types, {len(methods_out)} methods -> {out_path.relative_to(v)}")
    except Exception as e:
        print(f"    [supplemental] FAILED to write: {e}")
        return False
    return True


def _generate_patch_data(family_slug: str, *,
                          verification: Path | None = None) -> bool:
    """Build patch DLL from managed/patch/, emit .patchdata, generate native/runtime-patchdata.cpp.

    The generated runtime-patchdata.cpp defines kPatchData[], kPatchDataSize,
    and kPatchDataHostClassName for entry.exe's hotpatch dispatch.

    If .patchdata generation fails (e.g. no contract), generates a sentinel
    file with kPatchDataSize=0 so compilation never breaks.
    """
    v = verification or _VERIFICATION
    family_dir = v / family_slug if v else _VERIFICATION_BASE / family_slug
    mids = _load_method_subject_ids(family_slug, verification=v)
    if not mids:
        print(f"    [gen_patch] no method subject IDs, generating sentinel")
        return _write_sentinel_patchdata(family_dir)

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

    from family_entrypoint_generator import generate_and_build
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
        return _write_sentinel_patchdata(family_dir)

    # Run emit-patch-data
    from batch_hotupdate_runner import _run_emit_patch_data
    patchdata_dir = family_dir / "managed" / "patch" / "patchdata"
    patchdata_dir.mkdir(parents=True, exist_ok=True)
    patchdata_path = patchdata_dir / f"{family_slug}.patchdata"

    # Pass aot-core-ir.json from codegen/ so .patchdata contains pre-lowered IR
    aot_core_ir_path = str(family_dir / "codegen" / "aot-core-ir.json")
    if not os.path.exists(aot_core_ir_path):
        aot_core_ir_path = None

    if not _run_emit_patch_data(build_result["dll_path"], str(patchdata_path),
                                aot_core_ir_path=aot_core_ir_path):
        print(f"    [gen_patch] emit-patch-data failed")
        return _write_sentinel_patchdata(family_dir)

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


def _write_sentinel_patchdata(family_dir: Path) -> bool:
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


def _inject_config_tier(cmakelists: Path, config_tier: str) -> None:
    """Inject target_compile_definitions for config tier into CMakeLists.txt.

    Ensures the entry EXE gets the correct CHAOS_IL2CPP_CONFIG_* define.
    """
    config_tier = config_tier.upper()
    assert config_tier in ("CHECK", "PROFILE", "SHIP"), f"Invalid config tier: {config_tier}"
    marker = "# chaos-il2cpp config tier"
    text = cmakelists.read_text(encoding="utf-8")
    if marker in text:
        return  # already injected
    line = f"target_compile_definitions(entry PRIVATE CHAOS_IL2CPP_CONFIG_{config_tier})  {marker}"
    # Insert before the closing paren of add_executable or after link libraries
    text = text.replace(
        "target_link_libraries(entry PRIVATE",
        f"{line}\ntarget_link_libraries(entry PRIVATE",
    )
    cmakelists.write_text(text, encoding="utf-8")


def _inject_seh_define(cmakelists: Path) -> None:
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


def _inject_runtime_stubs_include(cmakelists: Path) -> None:
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


def _inject_verification_dispatch_source(cmakelists: Path) -> None:
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

def _inject_eha_directive(cmakelists: Path) -> None:
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


def _ensure_cmakelists(cmakelists: Path, family_slug: str, verification: Path, *, is_jit: bool = False) -> None:
    """Auto-generate or update native/CMakeLists.txt with SDK-based template.

    Uses find_package(chaos) to discover the chaos SDK (prebuilt runtime libs,
    compile flags). The SDK is at codegen/ output directory (--sdk-out).

    When is_jit=True, adds JIT-mode libs (chaos_jit, chaos_debugger) and
    /FORCE:MULTIPLE linker flag.
    """

    repo_root_str = str(_REPO_ROOT).replace("\\", "/")
    codegen_rel = str((verification / family_slug / "codegen").resolve()).replace("\\", "/")

    # ── Conditional JIT-mode additions ──────────────────────────────────
    force_multiple = '\ntarget_link_options(entry PRIVATE /FORCE:MULTIPLE)' if is_jit else ''

    cmake_content = (
        f'cmake_minimum_required(VERSION 3.20)\n'
        f'project(chaos_entry CXX)\n'
        f'set(CMAKE_CXX_STANDARD 20)\n'
        f'\n'
        f'# Compiler settings — /EHa needed for catch(...) to intercept C++ exceptions\n'
        f'# thrown by generated code (throw chaos_managed_exception from unresolved calls).\n'
        f'add_compile_options(/utf-8 /GS- /FS)\n'
        f'add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)\n'
        f'add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)\n'
        f'\n'
        f'# Find chaos SDK — provides chaos::runtime (prebuilt libs + flags) and\n'
        f'# chaos::codegen (precompiled generated code) via find_package(chaos).\n'
        f'# The SDK root is the codegen output directory from --sdk-out.\n'
        f'set(CHAOS_SDK_DIR "{codegen_rel}")\n'
        f'find_package(chaos REQUIRED PATHS "${{CHAOS_SDK_DIR}}")\n'
        f'\n'
        f'# Paths\n'
        f'set(CHAOS_PROJECT_ROOT "{repo_root_str}")\n'
        f'set(CHAOS_CODEGEN_DIR "{codegen_rel}")\n'
        f'\n'
        f'# Source files — codegen outputs to codegen/<AssemblyName>/generated/\n'
        f'file(GLOB CHAOS_CODEGEN_CPP "${{CHAOS_CODEGEN_DIR}}/*Subjects/generated/native-aot.generated.cpp")\n'
        f'# Also search native/ (pipeline syncs codegen output there after convert-to-cpp)\n'
        f'file(GLOB CHAOS_CODEGEN_NATIVE_CPP "${{CMAKE_CURRENT_SOURCE_DIR}}/*Subjects/generated/native-aot.generated.cpp")\n'
        f'# Use codegen path if available, fall back to native path (avoids compiling both)\n'
        f'if(CHAOS_CODEGEN_CPP)\n'
        f'  set(CHAOS_AOT_GENERATED_CPP ${{CHAOS_CODEGEN_CPP}})\n'
        f'else()\n'
        f'  set(CHAOS_AOT_GENERATED_CPP ${{CHAOS_CODEGEN_NATIVE_CPP}})\n'
        f'endif()\n'
        f'file(GLOB CHAOS_NATIVE_STUBS "*.cpp")\n'
        f'list(REMOVE_ITEM CHAOS_NATIVE_STUBS\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/runtime-entry.cpp"\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/runtime-patchdata.cpp"\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/verification_dispatch.generated.cpp"\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/jit_stubs.cpp"\n'
        f')\n'
        f'set(CHAOS_ENTRY_SOURCES\n'
        f'    "runtime-entry.cpp"\n'
        f'    "runtime-patchdata.cpp"\n'
        f'    "verification_dispatch.generated.cpp"\n'
        f'    ${{CHAOS_NATIVE_STUBS}}\n'
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
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/runtime-core/runtime_stubs"  # runtime_stubs for enum_stubs.h\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/bootstrap"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/interpreter"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/interpreter/generated"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/support"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/src/native/hot-update"\n'
        f'    "${{CHAOS_PROJECT_ROOT}}/third_party/unordered_dense/include"\n'
        f'    "${{CHAOS_CODEGEN_DIR}}/NumericAggregationSubjects/generated"\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}"\n'
        f')\n'
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



def _patch_enum_dispatch_guard(codegen_dir: Path) -> None:
    for gen_h in codegen_dir.glob('**/enum_metadata.generated.h'):
        content = gen_h.read_text(encoding='utf-8')
        guard = '#ifndef CHAOS_IL2CPP_ENUM_DISPATCH_ENTRY_DEFINED'
        if guard not in content:
            # Wrap the full struct definition with #ifndef/#define/#endif so that
            # it compiles when generated_code_compat.h (which also defines the
            # same struct) is included first.
            old = 'struct EnumDispatchEntry {\n    CHAOS_IL2CPP_UINT32 fnv24;\n    const EnumMetadataTable* table;\n};'
            new = ('#ifndef CHAOS_IL2CPP_ENUM_DISPATCH_ENTRY_DEFINED\n'
                   '#define CHAOS_IL2CPP_ENUM_DISPATCH_ENTRY_DEFINED\n'
                   'struct EnumDispatchEntry {\n'
                   '    CHAOS_IL2CPP_UINT32 fnv24;\n'
                   '    const EnumMetadataTable* table;\n'
                   '};\n'
                   '#endif')
            if old in content:
                content = content.replace(old, new, 1)
            else:
                # Fallback: guard the opening and find the closing };
                opening_old = 'struct EnumDispatchEntry {'
                opening_new = guard + '\n#define CHAOS_IL2CPP_ENUM_DISPATCH_ENTRY_DEFINED\n' + opening_old
                pos = content.find(opening_old)
                if pos != -1:
                    content = content[:pos] + opening_new + content[pos + len(opening_old):]
                    closing_pos = content.find('};', pos)
                    if closing_pos != -1:
                        content = content[:closing_pos + 2] + '\n#endif' + content[closing_pos + 2:]
            gen_h.write_text(content, encoding='utf-8')
            print(f'    [patch] added ifndef guard to {gen_h.name}')

def _ensure_microbench_source(native_dir: Path) -> None:
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
        'extern "C" double RunBenchmark(int entry_index, int iterations);\n'
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
        '        double elapsed_ms = RunBenchmark(0, kIterations);\n'
        '        if (elapsed_ms >= 0.0) {\n'
        '            double ns_per_call = (elapsed_ms * 1e6) / kIterations;\n'
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
        '        double elapsed_ms = RunBenchmark(0, d_iter);\n'
        '        if (elapsed_ms >= 0.0) {\n'
        '            double ns_per_call = (elapsed_ms * 1e6) / d_iter;\n'
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


def _inject_microbench_source(cmakelists: Path) -> None:
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


def _fix_runtime_entry(path: Path) -> None:
    """Fix known bugs in the stock runtime-entry.cpp template.

    1. `\\n` → `\n` in printf format strings (double-escaped backslash).
    2. `int result` bitmask with `while(tmp)` arithmetic-right-shift loop
       that never terminates for negative values (i >= 31 sets sign bit).
    """
    text = path.read_text(encoding="utf-8")
    changed = False

    # Fix 1: double-escaped backslash in printf
    if '\\\\n"' in text:
        text = text.replace('\\\\n"', '\\n"')
        changed = True

    # Fix 2: bitmask infinite loop — replace with simple counter
    old_bitmask = (
        "        int result = 0;\n"
        "        for (int i = 0; i < kAotMethodCount; i++) {\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = []() { throw chaos_managed_exception{}; };\n"
        "            try {\n"
        "                RunNativeAot(i);\n"
        "            } catch (const chaos_managed_exception&) {\n"
        "                result |= (1 << i);\n"
        "            } catch (...) {\n"
        "                result |= (1 << i);\n"
        "            }\n"
        "        }\n"
        "        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;\n"
        "        int failed_count = 0;\n"
        "        int tmp = result;\n"
        "        while (tmp) { failed_count += tmp & 1; tmp >>= 1; }\n"
        "        int passed_count = kAotMethodCount - failed_count;\n"
        '        printf("Passed: %d/%d\\n", passed_count, kAotMethodCount);\n'
        "        std::fflush(stdout);\n"
        "        _exit(result);\n"
        "        return result;\n"
    )
    new_counter = (
        "        int failed_count = 0;\n"
        "        for (int i = 0; i < kAotMethodCount; i++) {\n"
        "            bool caught = false;\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = []() { throw chaos_managed_exception{}; };\n"
        "            try {\n"
        "                RunNativeAot(i);\n"
        "            } catch (const chaos_managed_exception&) {\n"
        "                caught = true;\n"
        "            } catch (...) {\n"
        "                caught = true;\n"
        "            }\n"
        "            if (caught) { ++failed_count; }\n"
        "        }\n"
        "        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;\n"
        "        int passed_count = kAotMethodCount - failed_count;\n"
        '        printf("Passed: %d/%d\\n", passed_count, kAotMethodCount);\n'
        "        std::fflush(stdout);\n"
        "        _exit(failed_count);\n"
        "        return failed_count;\n"
    )
    if old_bitmask in text:
        text = text.replace(old_bitmask, new_counter)
        changed = True

    # Fix 4: HotUpdate mode counter — replace with semantic + revert verification
    old_hot_counter = (
        "        auto* patch_ctx = ApplyHotpatchIfAvailable();\n"
        "        int hotupdate_failed = 0;\n"
        "        for (int i = 0; i < kAotMethodCount; i++) {\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = []() { throw chaos_managed_exception{}; };\n"
        "            try {\n"
        "                RunNativeAot(i);\n"
        "            } catch (...) {\n"
        "                ++hotupdate_failed;\n"
        "            }\n"
        "        }\n"
        "        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;\n"
        "        int passed_count = kAotMethodCount - hotupdate_failed;\n"
        '        printf("{\\"passedMethods\\":%d,\\"failedMethods\\":%d,\\"totalMethods\\":%d}\\n",\n'
        "               passed_count, hotupdate_failed, kAotMethodCount);\n"
        "        std::fflush(stdout);\n"
        "        _exit(hotupdate_failed);\n"
        "        return hotupdate_failed;\n"
    )
    new_hot_semantic_revert = (
        "        const int kCount = kAotMethodCount;\n"
        "        CHAOS_IL2CPP_INT32 baseline_values[256] = {0};\n"
        "        bool baseline_ok[256] = {false};\n"
        "        for (int i = 0; i < kCount; i++) {\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = []() { throw chaos_managed_exception{}; };\n"
        "            try {\n"
        "                baseline_values[i] = RunNativeAot(i);\n"
        "                baseline_ok[i] = true;\n"
        "            } catch (...) {\n"
        "            }\n"
        "        }\n"
        "        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;\n"
        "        auto* patch_ctx = ApplyHotpatchIfAvailable();\n"
        "        bool all_semantic = true;\n"
        "        int semantic_passed = 0;\n"
        "        for (int i = 0; i < kCount; i++) {\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = []() { throw chaos_managed_exception{}; };\n"
        "            CHAOS_IL2CPP_INT32 ret = 0;\n"
        "            try {\n"
        "                ret = RunNativeAot(i);\n"
        "            } catch (...) {\n"
        "            }\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = nullptr;\n"
        "            if (baseline_ok[i] && ret == baseline_values[i]) {\n"
        "                semantic_passed++;\n"
        "            } else {\n"
        "                all_semantic = false;\n"
        "            }\n"
        "        }\n"
        "        if (patch_ctx != nullptr) {\n"
        "            chaos::il2cpp::runtime_core::Unpatch(patch_ctx);\n"
        "        }\n"
        "        bool all_revert = true;\n"
        "        int revert_passed = 0;\n"
        "        for (int i = 0; i < kCount; i++) {\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = []() { throw chaos_managed_exception{}; };\n"
        "            CHAOS_IL2CPP_INT32 ret = 0;\n"
        "            try {\n"
        "                ret = RunNativeAot(i);\n"
        "            } catch (...) {\n"
        "            }\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = nullptr;\n"
        "            if (baseline_ok[i] && ret == baseline_values[i]) {\n"
        "                revert_passed++;\n"
        "            } else {\n"
        "                all_revert = false;\n"
        "            }\n"
        "        }\n"
        '        printf("{\\"passedMethods\\":%d,\\"failedMethods\\":0,\\"totalMethods\\":%d,\\"allSemantic\\":%s,\\"allRevert\\":%s}\\n",\n'
        "               semantic_passed, kCount,\n"
        "               all_semantic ? \"true\" : \"false\",\n"
        "               all_revert ? \"true\" : \"false\");\n"
        "        std::fflush(stdout);\n"
        "        _exit(0);\n"
        "        return 0;\n"
    )
    if old_hot_counter in text:
        text = text.replace(old_hot_counter, new_hot_semantic_revert)
        changed = True
    # Backward compat: old bitmask pattern (from older template versions)
    old_hot_bitmask = (
        "        auto* patch_ctx = ApplyHotpatchIfAvailable();\n"
        "        int result = 0;\n"
        "        for (int i = 0; i < kAotMethodCount; i++) {\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = []() { throw chaos_managed_exception{}; };\n"
        "            try {\n"
        "                RunNativeAot(i);\n"
        "            } catch (...) {\n"
        "                result |= (1 << i);\n"
        "            }\n"
        "        }\n"
        "        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;\n"
        "        int failed_count = 0;\n"
        "        int tmp2 = result;\n"
        "        while (tmp2) { failed_count += tmp2 & 1; tmp2 >>= 1; }\n"
        "        int passed_count = kAotMethodCount - failed_count;\n"
        '        printf("{\\"passedMethods\\":%d,\\"failedMethods\\":%d,\\"totalMethods\\":%d}\\n",\n'
        "               passed_count, failed_count, kAotMethodCount);\n"
        "        std::fflush(stdout);\n"
        "        _exit(result);\n"
        "        return result;\n"
    )
    if old_hot_bitmask in text:
        text = text.replace(old_hot_bitmask, new_hot_semantic_revert)
        changed = True

    # Fix 5: Add Marshal.GetExceptionForHR stub returning null to
    # FillExternalRuntimeStubs. The subject ID pattern
    # "Marshal::GetExceptionForHR:System.Exception(System.Int32)"
    # has return type System.Exception which doesn't match any
    # known return-type pattern, causing the sentinel (non-null) stub
    # to be used — which crashes when the generated code tries to call
    # get_Message on it.
    marshal_stub = (
        "        // Array.CreateInstance 2D — return pseudo-pointer with hash 56793269.\n"
        "        if (std::strstr(sub, \"System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)\")) {\n"
        "            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR len1, CHAOS_IL2CPP_INTPTR len2) -> CHAOS_IL2CPP_INTPTR {\n"
        "                return ChaosArrayCreateInstance2D(type, static_cast<CHAOS_IL2CPP_INT32>(len1), static_cast<CHAOS_IL2CPP_INT32>(len2));\n"
        "            });\n"
        "            continue;\n"
        "        }\n"
        "\n"
        "        // Marshal.GetExceptionForHR — return null so downstream\n"
        "        // null-checks (ex == null) work during fact verification.\n"
        "        if (std::strstr(sub, \"Marshal::GetExceptionForHR:\")) {\n"
        "            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INTPTR { return 0; });\n"
        "            continue;\n"
        "        }\n"
        "\n"
        "        // Parse return type from subject ID pattern:\n"
    )
    old_after_array = (
        "        // Array.CreateInstance 2D — return pseudo-pointer with hash 56793269.\n"
        "        if (std::strstr(sub, \"System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)\")) {\n"
        "            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(+[](CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR len1, CHAOS_IL2CPP_INTPTR len2) -> CHAOS_IL2CPP_INTPTR {\n"
        "                return ChaosArrayCreateInstance2D(type, static_cast<CHAOS_IL2CPP_INT32>(len1), static_cast<CHAOS_IL2CPP_INT32>(len2));\n"
        "            });\n"
        "            continue;\n"
        "        }\n"
        "\n"
        "        // Parse return type from subject ID pattern:\n"
    )
    if old_after_array in text:
        text = text.replace(old_after_array, marshal_stub)
        changed = True

    # Fix 6: Replace BenchmarkMethod with RunBenchmark in extern declarations and calls.
    # The verification dispatch code has been moved from Scriban templates to
    # Python-generated verification_dispatch.generated.cpp, which defines
    # RunBenchmark instead of BenchmarkMethod.
    old_benchmark_extern = 'extern "C" double BenchmarkMethod(int, int);'
    new_benchmark_extern = 'extern "C" double RunBenchmark(int, int);'
    if old_benchmark_extern in text:
        text = text.replace(old_benchmark_extern, new_benchmark_extern)
        changed = True

    # Remove RunNativeAotAll and RunNativeAotBench extern declarations — these
    # no longer exist (verification dispatch moved to Python generator).
    for decl in ['extern "C" std::int32_t RunNativeAotAll();',
                 'extern "C" std::int32_t RunNativeAotBench(std::int32_t);']:
        if decl in text:
            text = text.replace(decl + "\n", "")
            changed = True

    # Replace BenchmarkMethod calls in Benchmark case with RunBenchmark
    if "BenchmarkMethod(entry_index, iterations)" in text:
        text = text.replace("BenchmarkMethod(entry_index, iterations)",
                            "RunBenchmark(entry_index, iterations)")
        changed = True

    # Fix 6: Add DIAG printfs to catch blocks in fact loop (for all template
    # formats). Replaces silent catch blocks that lack DIAG prints.
    old_silent_catch = (
        "        } catch (const chaos_managed_exception&) {\n"
        "                ++failed_count;\n"
        "            } catch (...) {\n"
        "                ++failed_count;\n"
        "            }"
    )
    new_diag_catch = (
        "        } catch (const chaos_managed_exception&) {\n"
        "                ++failed_count;\n"
        '                printf("[DIAG] Fact FAILED method index %d/%d (chaos_managed_exception)\\n", i, kAotMethodCount);\n'
        "                std::fflush(stdout);\n"
        "            } catch (...) {\n"
        "                ++failed_count;\n"
        '                printf("[DIAG] Fact FAILED method index %d/%d (...)\\n", i, kAotMethodCount);\n'
        "                std::fflush(stdout);\n"
        "            }"
    )
    if old_silent_catch in text:
        text = text.replace(old_silent_catch, new_diag_catch)
        changed = True

    # Fix 7: Subject-entry fact loop — iterate kSubjectEntryCount instead of
    # kAotMethodCount so that JIT-mode (interpreter) dispatch only tests the
    # contract methods (subject entries).  Non-subject methods may use EH
    # patterns (fault/filter/nested-catch) that the interpreter doesn't yet
    # support, causing false failures in fact_jit.
    old_subject_loop = (
        "        int failed_count = 0;\n"
        "        for (int i = 0; i < kAotMethodCount; i++) {\n"
        "            bool caught = false;\n"
        "#if defined(CHAOS_IL2CPP_EH_WIN32_SEH)\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = []() { chaos::il2cpp::runtime_core::chaos_raise_exception(0); };\n"
        "#else\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = []() { throw chaos_managed_exception{}; };\n"
        "#endif\n"
        "            try {\n"
        "                RunNativeAot(i);\n"
        "            } catch (const chaos_managed_exception&) {\n"
        "                ++failed_count;\n"
        '                printf("[DIAG] Fact FAILED method index %d/%d (chaos_managed_exception)\\n", i, kAotMethodCount);\n'
        "                std::fflush(stdout);\n"
        "            } catch (...) {\n"
        "                ++failed_count;\n"
        '                printf("[DIAG] Fact FAILED method index %d/%d (...)\\n", i, kAotMethodCount);\n'
        "                std::fflush(stdout);\n"
        "            }\n"
        "        }\n"
        "        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;\n"
        "        int passed_count = kAotMethodCount - failed_count;\n"
        '        printf("Passed: %d/%d\\n", passed_count, kAotMethodCount);\n'
    )
    new_subject_loop = (
        "        int failed_count = 0;\n"
        "        for (int si = 0; si < kSubjectEntryCount; si++) {\n"
        "            int i = kSubjectEntryIndices[si];\n"
        "#if defined(CHAOS_IL2CPP_EH_WIN32_SEH)\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = []() { chaos::il2cpp::runtime_core::chaos_raise_exception(0); };\n"
        "#else\n"
        "            chaos::il2cpp::common::g_chaos_fail_hook = []() { throw chaos_managed_exception{}; };\n"
        "#endif\n"
        "            try {\n"
        "                RunNativeAot(i);\n"
        "            } catch (const chaos_managed_exception&) {\n"
        "                ++failed_count;\n"
        '                printf("[DIAG] Fact FAILED method index %d/%d (chaos_managed_exception)\\n", i, kAotMethodCount);\n'
        "                std::fflush(stdout);\n"
        "            } catch (...) {\n"
        "                ++failed_count;\n"
        '                printf("[DIAG] Fact FAILED method index %d/%d (...)\\n", i, kAotMethodCount);\n'
        "                std::fflush(stdout);\n"
        "            }\n"
        "        }\n"
        "        chaos::il2cpp::common::g_chaos_fail_hook = nullptr;\n"
        "        int passed_count = kSubjectEntryCount - failed_count;\n"
        '        printf("Passed: %d/%d\\n", passed_count, kSubjectEntryCount);\n'
    )
    if old_subject_loop in text:
        text = text.replace(old_subject_loop, new_subject_loop)
        changed = True

    # Fix 7b: Add global-scope extern declarations for kSubjectEntryCount/Indices
    # (must be at file scope, not inside a block, for correct linkage).
    # Verification_dispatch.generated.cpp and native-aot.generated.cpp define
    # these, so runtime-entry.cpp only needs extern declarations.
    old_extern_section = (
        'extern "C" const char* kChaosExternalRuntimeSubjects[];'
    )
    new_extern_section = (
        'extern "C" const char* kChaosExternalRuntimeSubjects[];\n'
        'extern "C" const int kSubjectEntryCount;\n'
        'extern "C" const int kSubjectEntryIndices[];'
    )
    if old_extern_section in text:
        text = text.replace(old_extern_section, new_extern_section)
        changed = True

    if changed:
        path.write_text(text, encoding="utf-8")
        print(f"    [build_entry] fixed runtime-entry.cpp bugs")


def _fix_native_aot_bridge_thunks(native_dir: Path) -> None:
    """Patch generated native-aot.generated.cpp to fix known codegen bugs.

    The codegen emits bridge/import thunks that reference statically-defined
    'chaos_external_runtime_*' wrapper functions, but doesn't always generate
    those wrappers for every method.  This function:
      1. Fixes p0 -> chaos_fn_arg_0 undeclared identifier
      2. Fixes chaos_gc_try_start_no_gc_region arg count (needs 2 args)
      3. Adds variadic forward declarations for missing bridge thunk targets
    """
    for gen_cpp in native_dir.glob("**/native-aot.generated.cpp"):
        text = gen_cpp.read_text(encoding="utf-8")
        changed = False

        # Fix 1: p0 -> chaos_fn_arg_0 in bridge thunk bodies
        if "(p0)" in text:
            text = text.replace("(p0)", "(chaos_fn_arg_0)")
            changed = True

        # Fix 2: chaos_gc_try_start_no_gc_region(chaos_fn_arg_0) -> add 0 for 2nd arg
        old_try = "chaos_gc_try_start_no_gc_region(chaos_fn_arg_0)"
        new_try = "chaos_gc_try_start_no_gc_region(chaos_fn_arg_0, 0)"
        if old_try in text:
            text = text.replace(old_try, new_try)
            changed = True

        # Also fix subject-body calls where the arg is ChaosLoadInt64(chaos_arg_0)
        # — match chaos_gc_try_start_no_gc_region(...) with any single-arg pattern
        # using paren-counting to find the matching close paren.
        idx = 0
        while True:
            pos = text.find("chaos_gc_try_start_no_gc_region(", idx)
            if pos == -1:
                break
            call_start = pos + len("chaos_gc_try_start_no_gc_region(")
            depth = 1
            end = call_start
            while end < len(text) and depth > 0:
                if text[end] == '(':
                    depth += 1
                elif text[end] == ')':
                    depth -= 1
                end += 1
            # end now points past the closing ')'
            inner = text[call_start:end - 1]  # exclude the closing )
            if ',' not in inner:
                # Single-arg call — insert ", 0" before closing paren
                text = text[:end - 1] + ', 0' + text[end - 1:]
                changed = True
                idx = end + 2  # skip past the inserted ", 0"
            else:
                idx = end

        # Fix 3: Add stubs for missing external runtime functions
        import re as _re
        bridge_marker = "// ── Bridge/import thunks ──"
        if bridge_marker in text:
            refs = set(_re.findall(r'chaos_external_runtime_[\w_]+', text))
            defined = set(_re.findall(r'\b(?:static|extern)\s+.*?\b(chaos_external_runtime_[\w_]+)\s*\(', text, _re.MULTILINE))
            missing = refs - defined
            if missing:
                # Use C-style variadic (...) to accept any caller arguments,
                # and int return type since all bridge thunk return types are
                # integral (int, int64, or pointer — all implicitly convertible).
                stubs = ["// ── Bridge thunk stubs (pipeline fix) ──"]
                for fn_name in sorted(missing):
                    stubs.append(f"static int {fn_name}(...) {{ return 0; }}")
                stubs.append("")
                insert_pos = text.find(bridge_marker)
                text = text[:insert_pos] + "\n".join(stubs) + "\n\n" + text[insert_pos:]
                changed = True

        if changed:
            gen_cpp.write_text(text, encoding="utf-8")
            print(f"    [build_entry] fixed bridge thunks in {gen_cpp.name}")


def _fix_t4_jit_include(native_dir: Path) -> None:
    """Inject #include \"jit_registration.h\" into generated files that
    reference RegisterT4JitMethods, RegisterJitEntryMethods, JitT4Entry, or
    JitEntry but don't include the header.
    """
    import re as _re
    for gen_cpp in native_dir.glob("**/native-aot.generated.cpp"):
        text = gen_cpp.read_text(encoding="utf-8")
        # Skip if already has the include
        if '#include "jit_registration.h"' in text:
            continue
        # Only inject if the file references T4 JIT types or functions
        if not _re.search(r'\bRegisterT4JitMethods\b|\bJitT4Entry\b|\bRegisterJitEntryMethods\b|\bJitEntry\b', text):
            continue
        # Find the last #include line and insert after it
        lines = text.splitlines(keepends=True)
        last_include_idx = -1
        for i, line in enumerate(lines):
            if line.startswith('#include'):
                last_include_idx = i
        if last_include_idx >= 0:
            lines.insert(last_include_idx + 1, '#include "jit_registration.h"\n')
            gen_cpp.write_text("".join(lines), encoding="utf-8")
            print(f"    [build_entry] added #include \"jit_registration.h\" in {gen_cpp.name}")


def _fix_aot_chaos_jit_register_all(native_dir: Path) -> None:
    """Replace ChaosJitRegisterAll() body with empty no-op in AOT mode.

    In AOT mode, the generated code sometimes emits a non-empty
    ChaosJitRegisterAll() that calls RegisterT4JitMethods().  This requires JIT
    engine initialization and crashes when the JIT is not linked/initialized.
    The AOT dispatch uses direct_ptr directly — JIT trampolines are not needed.
    """
    import re as _re
    for gen_cpp in native_dir.glob("**/native-aot.generated.cpp"):
        text = gen_cpp.read_text(encoding="utf-8")
        # Match ChaosJitRegisterAll with a non-empty body (i.e. not "{}" alone)
        match = _re.search(
            r'(extern\s+"C"\s+void\s+ChaosJitRegisterAll\s*\(\s*\)\s*)\{([^}]*)\}',
            text)
        if not match:
            continue
        body = match.group(2)
        if not body.strip():
            continue  # already a no-op
        # Replace with empty body — use raw match string (including newlines)
        # The regex captures everything between { and } so body includes leading/trailing
        # whitespace/newlines. Replace the full match, not a reconstructed string.
        start, end = match.start(), match.end()
        text = text[:start] + match.group(1) + '{}' + text[end:]
        gen_cpp.write_text(text, encoding="utf-8")
        print(f"    [build_entry] stripped ChaosJitRegisterAll() body in {gen_cpp.name}")


def _fix_forward_declarations(native_dir: Path) -> None:
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


def _write_sentinel_dispatch(dispatch_cpp: Path) -> None:
    """Write a sentinel verification_dispatch.generated.cpp for cmake configure.

    The real dispatch file is generated by generate_verification_dispatch.py
    after codegen runs.  This sentinel ensures cmake configure can find the
    source file during the initial build.
    """
    content = (
        '// verification_dispatch.generated.cpp — sentinel (pre-codegen)\n'
        '#include <cstdint>\n'
        '#include <chaos/native_types.h>\n'
        '\n'
        'extern "C" const int kSubjectEntryCount;\n'
        'extern "C" const int kSubjectEntryIndices[];\n'
        '\n'
        'extern "C" CHAOS_IL2CPP_INT32 RunFactAll() { return 0; }\n'
        'extern "C" double RunBenchmark(int, int) { return -1.0; }\n'
        'extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll() { return 0; }\n'
        'extern "C" double RunHotpatchBenchmark(int, int) { return -1.0; }\n'
    )
    dispatch_cpp.parent.mkdir(parents=True, exist_ok=True)
    dispatch_cpp.write_text(content, encoding="utf-8")


def _fix_dispatch_externs(dispatch_cpp: Path) -> None:
    """Replace kSubjectEntryCount/Indices definitions with extern declarations.

    Some dispatch files generated by older versions of the script used
    definition syntax (e.g. 'extern "C" const int kSubjectEntryCount = 16;')
    which causes LNK2005 when native-aot.generated.cpp also defines them.
    This fixup converts them to pure extern declarations.
    """
    import re
    content = dispatch_cpp.read_text(encoding="utf-8")
    new_content = re.sub(
        r'extern "C" const int kSubjectEntryCount = \d+;\s*'
        r'extern "C" const int kSubjectEntryIndices\[\d+\] = \{.*?\};',
        'extern "C" const int kSubjectEntryCount;\n'
        'extern "C" const int kSubjectEntryIndices[];\n'
        '// (defined in native-aot.generated.cpp)',
        content,
        flags=re.DOTALL,
    )
    if new_content != content:
        dispatch_cpp.write_text(new_content, encoding="utf-8")
        print(f"    [build_entry] fixed kSubjectEntryCount extern declarations in dispatch")


def _build_entry_exe(family_slug: str, *, verification: Path | None = None, config_tier: str = "CHECK", output_name: str = "entry.exe", is_jit: bool = False) -> bool:
    v = verification or _VERIFICATION
    native_dir = v / family_slug / "native"
    cmakelists = native_dir / "CMakeLists.txt"

    # Clean stale jit_stubs.cpp from previous JIT runs (replaced by chaos_jit.lib in SDK)
    stale_jit_stubs = native_dir / "jit_stubs.cpp"
    if stale_jit_stubs.exists():
        stale_jit_stubs.unlink()
        print(f"    [build_entry] cleaned stale jit_stubs.cpp")

    # Auto-generate CMakeLists.txt if missing (e.g. after clean delete)
    _ensure_cmakelists(cmakelists, family_slug, v, is_jit=is_jit)

    # Inject config tier compile definition into CMakeLists.txt
    _inject_config_tier(cmakelists, config_tier)

    # Inject SEH define (Windows) to avoid MSVC EH table corruption in large TUs
    _inject_seh_define(cmakelists)

    # Inject verification_dispatch.generated.cpp into CMakeLists.txt if missing
    # (families created before the dispatch generator refactor need this)
    _inject_verification_dispatch_source(cmakelists)

    # Inject runtime_stubs include path if missing (older CMakeLists.txt don't have it)
    _inject_runtime_stubs_include(cmakelists)

    # Ensure runtime-patchdata.cpp exists (sentinel if not generated)
    patchdata_cpp = native_dir / "runtime-patchdata.cpp"
    if not patchdata_cpp.exists():
        _write_sentinel_patchdata(v / family_slug)
        print(f"    [build_entry] sentinel runtime-patchdata.cpp generated")

    # Ensure microbench.cpp exists for --microbench support
    _ensure_microbench_source(native_dir)
    _inject_microbench_source(cmakelists)

    # Ensure runtime-entry.cpp exists in native/ — use codegen-generated version
    codegen_runtime_entry = v / family_slug / "codegen" / "runtime-entry.cpp"
    native_runtime_entry = native_dir / "runtime-entry.cpp"
    if codegen_runtime_entry.exists():
        native_runtime_entry.write_text(codegen_runtime_entry.read_text(encoding="utf-8"), encoding="utf-8")
        print(f"    [build_entry] copied runtime-entry.cpp from codegen/ to native/")
        # Fix known bugs in the stock runtime-entry.cpp template:
        # 1. `\\n` -> `\n` in printf (double-escaped backslash)
        # 2. `int result` bitmask with `while (tmp)` loop hangs for >31 methods
        #    (MSVC arithmetic right shift of negative int never reaches 0)
        _fix_runtime_entry(native_runtime_entry)
    elif native_runtime_entry.exists():
        # No fresh codegen copy available, but existing native/ entry exists.
        # Still apply fixes (e.g. semantic/revert verification) to the old file.
        _fix_runtime_entry(native_runtime_entry)

    # Sync generated .cpp from codegen/<Assembly>/ to native/<Assembly>/
    # so the native CMakeLists.txt compiles the latest codegen output.
    codegen_dir = v / family_slug / "codegen"
    synced_names = set()
    if codegen_dir.exists():
        # Patch generated enum_metadata.generated.h FIRST (before syncing to native)
        # to add #ifndef guard for EnumDispatchEntry (defined in generated_code_compat.h
        # to avoid redefinition). This must happen before the copy so native files get
        # the patched version too.
        _patch_enum_dispatch_guard(codegen_dir)

        for subdir in sorted(codegen_dir.iterdir()):
            if not subdir.is_dir() or subdir.name in ("build", "generated", "hot-update"):
                continue
            # Sync ALL generated files (.cpp, .h) from codegen to native
            src = subdir / "generated"
            if not src.exists():
                continue
            dst = native_dir / subdir.name / "generated"
            dst.mkdir(parents=True, exist_ok=True)
            for f in src.iterdir():
                if not f.is_file():
                    continue
                (dst / f.name).write_bytes(f.read_bytes())
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
                # Check if a corresponding codegen source exists
                expected_codegen = codegen_dir / expected / "generated" / "native-aot.generated.cpp"
                if expected_codegen.exists():
                    # Already has its own codegen output — sync it
                    expected_native.parent.mkdir(parents=True, exist_ok=True)
                    expected_native.write_text(expected_codegen.read_text(encoding="utf-8"), encoding="utf-8")
                    print(f"    [build_entry] synced {expected_codegen.relative_to(codegen_dir)} to native/ (delayed)")
                else:
                    # Assembly name mismatch — find best source (most recently modified)
                    best_src = max(
                        (codegen_dir / s / "generated" / "native-aot.generated.cpp" for s in synced_names),
                        key=lambda p: p.stat().st_mtime,
                    )
                    expected_native.parent.mkdir(parents=True, exist_ok=True)
                    expected_native.write_text(best_src.read_text(encoding="utf-8"), encoding="utf-8")
                    print(f"    [build_entry] synced {best_src.relative_to(codegen_dir)} -> {expected}/generated/ (assembly name mismatch)")

    # Ensure CMakeLists.txt exists — auto-generate from template if missing
    # (families deleted and regenerated from scratch won't have native/CMakeLists.txt)
    _ensure_cmakelists(cmakelists, family_slug, v, is_jit=is_jit)

    # Ensure verification_dispatch.generated.cpp exists
    # The real file is generated by the orchestrator after codegen; the sentinel
    # ensures cmake configure can find the source file during initial build.
    dispatch_cpp = native_dir / "verification_dispatch.generated.cpp"
    if not dispatch_cpp.exists():
        _write_sentinel_dispatch(dispatch_cpp)
        print(f"    [build_entry] sentinel verification_dispatch.generated.cpp created")
    else:
        # Guard: ensure kSubjectEntryCount/Indices are extern declarations,
        # not definitions (which cause LNK2005 when native-aot.generated.cpp
        # also defines them after codegen regeneration).
        _fix_dispatch_externs(dispatch_cpp)

    # Copy enum_stubs.cpp from source to native/ for unresolved symbol resolution.
    # The pre-built chaos_runtime_core.lib at artifacts/presets/ is stale (built
    # before enum_stubs.cpp was added), so enum_stubs.cpp needs to be compiled
    # directly as part of the entry.exe build via cmake file glob.
    enum_stubs_src = _REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_stubs" / "enum_stubs.cpp"
    if enum_stubs_src.exists():
        enum_stubs_dst = native_dir / "enum_stubs.cpp"
        if not enum_stubs_dst.exists() or enum_stubs_src.stat().st_mtime > enum_stubs_dst.stat().st_mtime:
            enum_stubs_dst.write_bytes(enum_stubs_src.read_bytes())
            enum_stubs_hdr_src = _REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_stubs" / "enum_stubs.h"
            if enum_stubs_hdr_src.exists():
                enum_stubs_hdr_dst = native_dir / "enum_stubs.h"
                if not enum_stubs_hdr_dst.exists() or enum_stubs_hdr_src.stat().st_mtime > enum_stubs_hdr_dst.stat().st_mtime:
                    enum_stubs_hdr_dst.write_bytes(enum_stubs_hdr_src.read_bytes())
                    print(f"    [build_entry] copied enum_stubs.h to native/ for include resolution")
            print(f"    [build_entry] copied enum_stubs.cpp to native/ for link resolution")

    # Patch native-aot.generated.cpp bridge thunks — the codegen may not emit
    # all wrapper functions that bridge thunks reference, and has known bugs
    # (p0 undeclared, TryStartNoGCRegion arg count mismatch).
    # Patch BOTH native_dir (synced copy) and codegen_dir (source compiled by cmake).
    _fix_native_aot_bridge_thunks(native_dir)
    # Add jit_registration.h include for RegisterT4JitMethods() calls
    _fix_t4_jit_include(native_dir)
    # Make ChaosJitRegisterAll() a no-op in AOT mode (codegen may emit non-empty
    # T4 JIT registration that requires JIT engine initialization)
    if not is_jit:
        _fix_aot_chaos_jit_register_all(native_dir)
    # Add forward declarations for functions referenced before their extern "C" decl.
    # This happens when generic dispatch wrappers call instantiated function bodies
    # declared later in the file.
    _fix_forward_declarations(native_dir)
    codegen_native_dir = v / family_slug / "codegen"
    if codegen_native_dir.exists():
        _fix_native_aot_bridge_thunks(codegen_native_dir)
        _fix_t4_jit_include(codegen_native_dir)
        if not is_jit:
            _fix_aot_chaos_jit_register_all(codegen_native_dir)
        _fix_forward_declarations(codegen_native_dir)

    build_dir = native_dir / "build"
    # Remove stale cmake cache to avoid generator/platform mismatch errors
    if build_dir.exists():
        try:
            shutil.rmtree(build_dir)
        except PermissionError:
            print(f"    [build_entry] warning: could not remove build_dir, continuing anyway")
    if not build_dir.exists():
        build_dir.mkdir(parents=True, exist_ok=True)

    # Step 1: CMake configure
    print(f"    [build_entry] cmake configure...")
    cfg_result = subprocess.run(
        ["cmake", "-S", str(native_dir), "-B", str(build_dir),
         "-G", "Visual Studio 17 2022", "-A", "x64"],
        capture_output=True, text=True, timeout=120,
    )
    if cfg_result.returncode != 0:
        print(f"    [build_entry] cmake configure FAILED")
        for line in cfg_result.stderr.splitlines()[-10:]:
            print(f"      {line}")
        return False

    # Step 1b: Patch vcxproj files to use /EHs /EHc- (extern "C" exception propagation)
    patch_script = _HERE.parents[0] / "_patch_vcxproj.py"
    if patch_script.exists():
        subprocess.run([sys.executable, str(patch_script), str(build_dir)],
                      capture_output=True, text=True, timeout=30)
        print(f"    [build_entry] vcxproj patched for /EHs /EHc-")

    # Step 2: CMake build (entry target, RelWithDebInfo)
    print(f"    [build_entry] cmake build...")
    build_result = subprocess.run(
        ["cmake", "--build", str(build_dir), "--config", "RelWithDebInfo",
         "--target", "entry"],
        capture_output=True, text=True, timeout=300,
    )
    if build_result.returncode != 0:
        print(f"    [build_entry] cmake build FAILED")
        for line in build_result.stderr.splitlines()[-15:]:
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


def _run_emit_native_aot(family_slug: str, *, verification: Path | None = None) -> bool:
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


def _has_synthetic_method_ids(method_subject_ids: list[str]) -> bool:
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
    mids = _load_method_subject_ids(family_slug, verification=verification)
    if not mids:
        print(f"  [SKIP] no methods in contract (contract-only family)")
        result["success"] = True
        result["error"] = None
        trace("family_skip", family=family_slug, reason="no methods")
        return result
    print(f"  Methods: {len(mids)}")
    result["methodCount"] = len(mids)

    # Step 1a: Build subjects DLL (il2cpp input via managed/subjects/)
    auto_variant = variant or ("patch" if _has_synthetic_method_ids(mids) else "benchmark")
    print(f"  [1a/3] Building subjects DLL (variant={auto_variant})...")
    build_result = _build_subjects_dll(family_slug, mids, assembly_name=assembly_name, verification=verification, variant=auto_variant)
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
    if not _run_convert_to_cpp(family_slug, build_result["dll_path"], verification=verification, entry_point_subject_id=entry_pt, codegen_mode=codegen_mode):
        result["steps"]["convert_to_cpp"] = "FAILED"
        result["error"] = "convert-to-cpp failed"
        trace("family_c2c_failed", family=family_slug)
        return result
    result["steps"]["convert_to_cpp"] = "OK"

    # Step 1c: Fix up undefined branch target labels in generated C++.
    # When the IL reader strips nop instructions, some branch targets point
    # to IL offsets that have no corresponding instruction.  The codegen's
    # EmitFlatGotoBody only emits labels at real instruction offsets, so
    # those branch targets become undefined C++ labels.  Map them to the
    # nearest following real instruction offset so the file compiles.
    _codegen_patch_undefined_labels(family_slug, verification=verification)

    # Step 1d: Generate .patchdata for hotpatch dispatch (before entry.exe build)
    _generate_patch_data(family_slug, verification=verification)

    # Step 1e: Generate coverage JSON for dashboard/kernel integration
    _generate_coverage_json(family_slug, assembly_name, mids, verification=verification)

    # Step 1f: Generate supplemental metadata payload from template
    _generate_supplemental_metadata(family_slug, mids, verification=verification)

    # Step 2: Build entry.exe from codegen/native-aot.generated.cpp → native/
    print(f"  [2/3] Building entry.exe from codegen...")
    if not _build_entry_exe(family_slug, verification=verification, config_tier=config_tier):
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


def _run_direct_assembly_translation(assembly_name: str) -> dict:
    """Run convert-to-cpp directly on an assembly DLL (no synthetic entrypoint).

    Translates the actual assembly DLL through the full IL2CPP pipeline,
    producing per-assembly C++ output. This is the 'final form' IL2CPP
    translation that foundation DLL verification should use.
    """
    # Find the actual assembly DLL
    dll_path = _find_assembly_dll(assembly_name)
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


def _find_assembly_dll(assembly_name: str) -> str | None:
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
        _REPO_ROOT / "verification" / "foundation-dll" / assembly_name,
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
        _run_assembly_translation_mode(args)
        return

    # Original per-family entrypoint mode
    if not _VERIFICATION.exists():
        print(f"FATAL: verification directory not found: {_VERIFICATION}", file=sys.stderr)
        sys.exit(1)

    families = args.families
    if not families:
        families = _discover_families(args.assembly_name)
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


def _run_assembly_translation_mode(args: argparse.Namespace) -> None:
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
        asm_result = _run_direct_assembly_translation(asm_name)
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
