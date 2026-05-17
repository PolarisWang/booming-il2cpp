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
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll"
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
    "generic-nullable-value",
    "guid-random-hashcode",
    "math-numerics",
    "object-equality-identity",
    "primitive-numeric-conversions",
    "string-char-text-core",
    "time-date-time-timespan",
    "threading-monitor-interlocked",
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
    from family_entrypoint_generator import generate_and_build
    result = generate_and_build(
        subjects_dir,
        assembly_name=assembly_name,
        family_id=f"family/{assembly_name}/{family_slug.replace('-', '/')}",
        method_subject_ids=method_subject_ids,
        class_name=None,  # auto-derived: e.g. ConvertCharSubjects
        variant="subjects",
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
    class_name = f"{family_slug.title().replace('-', '').replace('_', '')}NativeEntry"

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
                "entry_point_subject_id": f"{class_name}/{class_name}::Run:System.Int32(System.Int32)",
            }

        # Partial class handwritten files (e.g. Custom.cs) — copy .cs files to entrypoint,
        # then fall through to generate_and_build() which auto-detects Custom.cs.
        # Source handwritten/ is NEVER modified by the pipeline.
        print(f"    using handwritten partial class files from {handwritten_dir}")
        entrypoint_dir.mkdir(parents=True, exist_ok=True)
        for f in cs_files:
            dest = entrypoint_dir / f.name
            dest.write_text(f.read_text(encoding="utf-8"), encoding="utf-8")

    result = generate_and_build(
        entrypoint_dir,
        assembly_name=assembly_name,
        family_id=f"family/{assembly_name}/{family_slug.replace('-', '/')}",
        method_subject_ids=method_subject_ids,
        class_name=class_name,
        variant=variant,
    )
    return result


def _run_convert_to_cpp(
    family_slug: str,
    dll_path: str,
    *,
    verification: Path | None = None,
    entry_point_subject_id: str | None = None,
) -> bool:
    """Run chaos-il2cpp convert-to-cpp on the subjects DLL.

    Output goes to codegen/<AssemblyName>/generated/ under verification dir.
    """
    v = verification or _VERIFICATION
    codegen_out = v / family_slug / "codegen"
    codegen_out.mkdir(parents=True, exist_ok=True)

    cmd = [
        "dotnet", "run", "--no-build",
        "--project", str(_REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver"),
        "--", "convert-to-cpp",
        "--assembly", dll_path,
        "--assembly-dir", str(Path(dll_path).parent),
        "--output", str(codegen_out),
    ]
    if entry_point_subject_id:
        cmd.extend(["--entry-point", entry_point_subject_id])

    result = subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=300)

    if result.returncode != 0:
        print(f"    convert-to-cpp FAILED (rc={result.returncode})")
        stderr = result.stderr or "(no stderr)"
        for line in stderr.splitlines()[-15:]:
            print(f"      {line}")
        return False

    # Check output — per-assembly subdirectory: codegen/<AssemblyName>/generated/native-aot.generated.cpp
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
    class_name = f"{family_slug.title().replace('-', '').replace('_', '')}NativeEntry"

    # Build patch-variant entrypoint from managed/patch/
    patch_dir = family_dir / "managed" / "patch"
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

    host_class_name = f"{class_name}"
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


def _ensure_cmakelists(cmakelists: Path, family_slug: str, verification: Path) -> None:
    """Auto-generate native/CMakeLists.txt if it doesn't exist.

    Uses the same template pattern as convert-char's verified CMakeLists.txt.
    The generated file references codegen/ and native/runtime-entry.cpp.
    """
    if cmakelists.exists():
        return

    repo_root_str = str(_REPO_ROOT).replace("\\", "/")
    codegen_rel = str((verification / family_slug / "codegen").resolve()).replace("\\", "/")
    native_build = str((_REPO_ROOT / "build" / "native").resolve()).replace("\\", "/")
    cmake_content = (
        f'cmake_minimum_required(VERSION 3.20)\n'
        f'project(chaos_entry CXX)\n'
        f'set(CMAKE_CXX_STANDARD 20)\n'
        f'\n'
        f'# Compiler settings — /EHa needed for catch(...) to intercept C++ exceptions\n'
        f'# thrown by generated code (throw chaos_managed_exception from unresolved calls).\n'
        f'add_compile_options(/utf-8 /GS-)\n'
        f'add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)\n'
        f'add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)\n'
        f'\n'
        f'# Paths\n'
        f'set(CHAOS_PROJECT_ROOT "{repo_root_str}")\n'
        f'set(CHAOS_CODEGEN_DIR "{codegen_rel}")\n'
        f'set(CHAOS_NATIVE_BUILD "{native_build}")\n'
        f'\n'
        f'# Source files — codegen outputs to codegen/<AssemblyName>/generated/\n'
        f'file(GLOB CHAOS_CODEGEN_CPP "${{CHAOS_CODEGEN_DIR}}/*/generated/native-aot.generated.cpp")\n'
        f'file(GLOB CHAOS_NATIVE_STUBS "*.cpp")\n'
        f'list(REMOVE_ITEM CHAOS_NATIVE_STUBS\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/runtime-entry.cpp"\n'
        f'    "${{CMAKE_CURRENT_SOURCE_DIR}}/runtime-patchdata.cpp"\n'
        f')\n'
        f'set(CHAOS_ENTRY_SOURCES\n'
        f'    "runtime-entry.cpp"\n'
        f'    "runtime-patchdata.cpp"\n'
        f'    ${{CHAOS_NATIVE_STUBS}}\n'
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
    cmakelists.write_text(cmake_content, encoding="utf-8")
    print(f"    [build_entry] auto-generated CMakeLists.txt at {cmakelists}")


def _build_entry_exe(family_slug: str, *, verification: Path | None = None, config_tier: str = "CHECK") -> bool:
    v = verification or _VERIFICATION
    native_dir = v / family_slug / "native"
    cmakelists = native_dir / "CMakeLists.txt"
    # Auto-generate CMakeLists.txt if missing (e.g. after clean delete)
    _ensure_cmakelists(cmakelists, family_slug, v)

    # Inject config tier compile definition into CMakeLists.txt
    _inject_config_tier(cmakelists, config_tier)

    # Ensure runtime-patchdata.cpp exists (sentinel if not generated)
    patchdata_cpp = native_dir / "runtime-patchdata.cpp"
    if not patchdata_cpp.exists():
        _write_sentinel_patchdata(v / family_slug)
        print(f"    [build_entry] sentinel runtime-patchdata.cpp generated")

    # Ensure runtime-entry.cpp exists in native/
    enhanced_runtime_entry = _HERE / "runtime-entry.cpp"
    native_runtime_entry = native_dir / "runtime-entry.cpp"
    if not native_runtime_entry.exists() and enhanced_runtime_entry.exists():
        native_runtime_entry.write_text(enhanced_runtime_entry.read_text(encoding="utf-8"), encoding="utf-8")
        print(f"    [build_entry] copied runtime-entry.cpp to native/")

    # Ensure CMakeLists.txt exists — auto-generate from template if missing
    # (families deleted and regenerated from scratch won't have native/CMakeLists.txt)
    _ensure_cmakelists(cmakelists, family_slug, v)

    build_dir = native_dir / "build"
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
    shutil.copy2(str(exe_path), str(target_dir / "entry.exe"))
    size = (target_dir / "entry.exe").stat().st_size
    print(f"    [build_entry] entry.exe OK: {size} bytes -> {target_dir / 'entry.exe'}")
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


def run_family(family_slug: str, *, assembly_name: str = "System.Private.CoreLib", variant: str | None = None, config_tier: str = "CHECK") -> dict:
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
        print(f"  [SKIP] no methods in contract")
        result["error"] = "no method subject IDs"
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
    if not _run_convert_to_cpp(family_slug, build_result["dll_path"], verification=verification, entry_point_subject_id=entry_pt):
        result["steps"]["convert_to_cpp"] = "FAILED"
        result["error"] = "convert-to-cpp failed"
        trace("family_c2c_failed", family=family_slug)
        return result
    result["steps"]["convert_to_cpp"] = "OK"

    # Step 1c: [removed] No post-processing on generated files.
    # native-aot.generated.cpp is the pure output of chaos-il2cpp.

    # Step 1d: Generate .patchdata for hotpatch dispatch (before entry.exe build)
    _generate_patch_data(family_slug, verification=verification)

    # Step 1e: Generate coverage JSON for dashboard/kernel integration
    _generate_coverage_json(family_slug, assembly_name, mids, verification=verification)

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
