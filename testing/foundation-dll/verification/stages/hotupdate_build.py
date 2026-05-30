"""HotUpdate patch data generation — builds patch DLL, emits .patchdata, generates runtime-patchdata.cpp.

Extracted from pipeline_native_aot_runner.py to reduce pipeline runner complexity.
Callers should use generate_patch_data() before entry.exe build to ensure
hotpatch support is linked in (or sentinel fallback if unavailable).
"""

from __future__ import annotations

import os
from pathlib import Path


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
        'extern const char* const kPatchDataHostClassName = nullptr;',
        "",
        "extern const uint8_t kPatchData[];",
        "extern const uint8_t kPatchData[] = { 0 };",
        "",
        'extern const size_t kPatchDataSize;',
        'extern const size_t kPatchDataSize = 0u;',
        "",
    ]
    patchdata_cpp.write_text("\n".join(lines), encoding="utf-8")
    return True


def generate_patch_data(family_slug: str, *,
                        verification: Path | None = None) -> bool:
    """Build patch DLL from managed/patch/, emit .patchdata, generate native/runtime-patchdata.cpp.

    The generated runtime-patchdata.cpp defines kPatchData[], kPatchDataSize,
    and kPatchDataHostClassName for entry.exe's hotpatch dispatch.

    If .patchdata generation fails (e.g. no contract), generates a sentinel
    file with kPatchDataSize=0 so compilation never breaks.
    """
    # Late imports to avoid circular dependency with pipeline_native_aot_runner
    from verification.stages.pipeline_native_aot_runner import (
        _VERIFICATION, _VERIFICATION_BASE, load_method_subject_ids,
    )

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
                    # Map class name from NativeEntry to PatchEntry
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

    patchdata_dir = family_dir / "managed" / "patch" / "patchdata"
    patchdata_dir.mkdir(parents=True, exist_ok=True)
    patchdata_path = patchdata_dir / f"{family_slug}.patchdata"

    # Pass aot-core-ir.json from codegen/ so .patchdata contains pre-lowered IR
    aot_core_ir_path = str(family_dir / "codegen" / "aot-core-ir.json")
    if not os.path.exists(aot_core_ir_path):
        aot_core_ir_path = None

    # Run emit-patch-data via hotupdate.py module
    try:
        from verification.stages.hotupdate import _run_emit_patch_data
    except ImportError:
        _run_emit_patch_data = None
    if _run_emit_patch_data is None:
        print(f"    [gen_patch] emit-patch-data not available (CLI not built), writing sentinel")
        return write_sentinel_patch_data(family_dir)

    if not _run_emit_patch_data(build_result["dll_path"], str(patchdata_path),
                                aot_core_ir_path=aot_core_ir_path,
                                test_mode=True):
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
