"""Generate per-family D3 hotupdate C++ test from CodeGen-generated host C++.

For each family, this script:
  1. Reads the host (genuine) native-aot.generated.cpp to discover MethodN symbols
  2. Fixes the genuine TU by injecting missing structs/type-ids (→ genuine-fixed)
  3. Embeds .patchdata as a C++ byte array in HotUpdateTest.cpp
  4. Generates HotUpdateTest.cpp that:
     - Bootstraps the runtime through the ABI bridge
     - Calls ApplyPatchFromMemory to apply D3 dispatch hotpatch
     - For each method: verifies dispatch entry flags, calls InterpreterEntryDirect
     - Calls Unpatch, verifies dispatch entry flags are cleared
  5. Creates/updates CMakeLists.txt with proper linkage (no patch C++ TU)

The test verifies the D3 dual-layer dispatch lifecycle:
  ApplyPatchFromMemory → dispatch table patching → InterpreterEntryDirect → Unpatch
"""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION = _REPO_ROOT / "verification" / "foundation-dll" / "System.Private.CoreLib"

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
    # New reflection families
    "reflection-type",
    "reflection-assembly",
    "reflection-generics",
    "reflection-member-complete",
    "reflection-parameters",
    "reflection-module",
    "reflection-binding",
    "reflection-activation",
]


def _slug_from_family_id(family_id: str) -> str:
    value = str(family_id or "")
    if not value:
        return ""
    parts = value.split("/")
    if len(parts) >= 4:
        return "-".join(part.replace("_", "-") for part in parts[2:])
    return parts[-1].replace("_", "-")


def _ns_slug_from_family_id(family_id: str) -> str:
    return _slug_from_family_id(family_id).replace("-", "_")


def _class_name(family_slug: str, variant: str) -> str:
    """Derive C++ class name from family slug and variant (host or patch)."""
    base = "".join(part.capitalize() for part in family_slug.split("-"))
    return f"{base}NativeEntry" if variant == "host" else f"{base}PatchEntry"


def _discover_host_symbols(family_slug: str) -> list[str]:
    """Scan the genuine (host) generated C++ to discover MethodN symbols.

    Returns host_symbols list.
    """
    host_cpp = _VERIFICATION / family_slug / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
    return _extract_method_symbols(host_cpp)


def _extract_method_symbols(cpp_path: Path) -> list[str]:
    """Extract extern C method declarations (Method0..MethodN) from generated C++."""
    if not cpp_path.exists():
        return []

    symbols: list[str] = []
    content = cpp_path.read_text(encoding="utf-8")
    for line in content.splitlines():
        m = re.match(
            r'extern\s+"C"\s+CHAOS_IL2CPP_INT32\s+(\w+_Method\d+)\(void\)',
            line.strip(),
        )
        if m:
            sym = m.group(1)
            if sym not in symbols:
                symbols.append(sym)
    return symbols


def _load_contract(family_slug: str) -> list[str]:
    """Load method subject IDs from contract."""
    contract_path = _VERIFICATION / family_slug / "capability-family-contract.json"
    if not contract_path.exists():
        return []
    with open(contract_path, encoding="utf-8") as f:
        contract = json.load(f)
    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]
    return mids


def _generate_hotupdate_test(
    family_slug: str,
    host_symbols: list[str],
    method_subject_ids: list[str],
) -> str:
    """Generate the per-family D3 hotupdate C++ test source."""
    family_id = f"family/System.Private.CoreLib/{family_slug.replace('-', '/')}"
    ns_slug = _ns_slug_from_family_id(family_id)
    method_count = len(host_symbols)

    # Embed .patchdata for D3 dispatch hotpatch
    host_class_name = _class_name(family_slug, "host")
    patch_data_cxx, _ = _embed_patch_data(family_slug)

    lines = []
    if patch_data_cxx:
        lines.append(patch_data_cxx)
    _emit_header(lines, family_id, method_count)
    _emit_forward_decls(lines, host_symbols)
    _emit_namespace_block(lines, host_symbols, method_count)
    _emit_main_function(lines, method_count, family_id, bool(patch_data_cxx), host_class_name)

    return "\n".join(lines)


def _embed_patch_data(family_slug: str) -> tuple[str, int]:
    """Read .patchdata binary and generate C++ byte array for embedding.

    Returns (cxx_array_source, byte_count).
    Returns empty string if .patchdata not found.
    """
    patchdata_path = _VERIFICATION / family_slug / "il2cpp_dist" / "patch" / "patchdata" / f"{family_slug}.patchdata"
    if not patchdata_path.exists():
        return "", 0

    data = patchdata_path.read_bytes()
    lines = [
        "// ── Embedded .patchdata for D3 dispatch hotpatch ──────────────",
        "// Generated from: " + str(patchdata_path.relative_to(_VERIFICATION)),
        f"// Size: {len(data)} bytes",
        "",
        "static const uint8_t kPatchData[] = {",
    ]

    # Emit as 16-byte wide hex dump
    for i in range(0, len(data), 16):
        chunk = data[i:i+16]
        hex_bytes = ", ".join(f"0x{b:02X}" for b in chunk)
        lines.append(f"    {hex_bytes},")

    lines.append("};")
    lines.append(f"static constexpr size_t kPatchDataSize = {len(data)}u;")
    lines.append("")

    return "\n".join(lines), len(data)
def _emit_header(
    lines: list[str],
    family_id: str,
    method_count: int,
) -> None:
    lines.extend([
        "// Per-family hotupdate verification test",
        f"// Family: {family_id}",
        f"// {method_count} methods",
        "//",
        "// Uses the codegen-emitted dispatch table + .patchdata for D3 verification:",
        "//   ApplyPatchFromMemory -> dispatch table patching -> InterpreterEntryDirect -> Unpatch",
        "//",
    ])
    lines.extend([
        '#include "bootstrap.h"',
        '#include "codegen_bridge.h"',
        '#include "patch_loader.h"',
        '#include "dispatch_table.h"',
        '#include "runtime_core.h"',
        '#include "interpreter_entry.h"',
        "",
        "#include <cstdio>",
        "#include <cstdint>",
        "#include <cstring>",
        "",
    ])


def _emit_forward_decls(
    lines: list[str],
    host_symbols: list[str],
) -> None:
    lines.append("// ---------------------------------------------------------------")
    lines.append("// Forward declarations: host methods (real API calls via CodeGen)")
    lines.append("// ---------------------------------------------------------------")
    for sym in host_symbols:
        lines.append(f'extern "C" CHAOS_IL2CPP_INT32 {sym}(void);')


def _emit_namespace_block(
    lines: list[str],
    host_symbols: list[str],
    method_count: int,
) -> None:
    lines.extend([
        "",
        "namespace {",
        "",
        "// Method pointer table for bootstrap registration.",
        "// Each entry maps a method token to the host method pointer.",
        "struct MethodPointerEntry {",
        "    uint32_t method_token;",
        "    void* method_pointer;",
        "};",
        "",
        f"constexpr uint32_t kMethodCount = {method_count}u;",
        "constexpr uint32_t kBaseToken = 0x06000001u;",
        "constexpr uintptr_t kSentinelPatchBase = 0xB0000000u;",
        "",
        "// Host method pointer array (auto-indexed by MethodN ordering)",
        "void* (*kHostThunks[])() = {",
    ])
    for sym in host_symbols:
        lines.append(f"    reinterpret_cast<void* (*)()>(&{sym}),")
    lines.extend([
        "};",
        "",
        "}  // namespace",
    ])


def _emit_main_function(lines: list[str], method_count: int, family_id: str,
                         has_patch_data: bool,
                         host_class_name: str) -> None:
    lines.extend([
        "",
        "int main() {",
        "    using chaos::il2cpp::bootstrap::PeekBootstrapState;",
        "    using chaos::il2cpp::runtime_core::ApplyPatchFromMemory;",
        "    using chaos::il2cpp::runtime_core::Unpatch;",
        "    using chaos::il2cpp::runtime_core::RuntimeDispatchLookupBySlot;",
        "    using chaos::il2cpp::runtime_core::PatchContext;",
        "    using chaos::il2cpp::runtime_core::InterpreterEntryDirect;",
        "",
        "    // Build synthetic method pointer table with host methods.",
        "    MethodPointerEntry entries[kMethodCount];",
        "    for (uint32_t i = 0u; i < kMethodCount; i++) {",
        "        entries[i].method_token = kBaseToken + i;",
        "        entries[i].method_pointer = reinterpret_cast<void*>(kHostThunks[i]);",
        "    }",
        "",
        "    // Synthetic code registrations.",
        "    CodeRegistrationV0 code_reg = {};",
        "    code_reg.struct_size = sizeof(CodeRegistrationV0);",
        "    code_reg.method_pointers = static_cast<const void*>(entries);",
        "    code_reg.method_pointer_count = kMethodCount;",
        "",
        "    MetadataRegistrationV0 meta_reg = {};",
        "    meta_reg.struct_size = sizeof(MetadataRegistrationV0);",
        "",
        "    CodegenRegistrationOptionsV0 options = {};",
        "    options.struct_size = sizeof(CodegenRegistrationOptionsV0);",
        '    options.image_name_utf8 = "System.Private.CoreLib";',
        "",
        "    // Bootstrap the runtime through the ABI bridge.",
        "    const CodegenBridgeV0* bridge = chaos_codegen_get_bridge_v0();",
        "    if (bridge == nullptr) {",
        '        std::fprintf(stderr, "FATAL: codegen bridge is null\\n");',
        "        return 1;",
        "    }",
        "    if (bridge->register_codegen(&code_reg, &meta_reg, &options) != CHAOS_BRIDGE_STATUS_OK) {",
        '        std::fprintf(stderr, "FATAL: register_codegen failed\\n");',
        "        return 1;",
        "    }",
        "    if (bridge->bootstrap_runtime() != CHAOS_BRIDGE_STATUS_OK) {",
        '        std::fprintf(stderr, "FATAL: bootstrap_runtime failed\\n");',
        "        return 1;",
        "    }",
        "",
        "    if (PeekBootstrapState() == nullptr) {",
        '        std::fprintf(stderr, "FATAL: bootstrap state is null after bootstrap\\n");',
        "        return 1;",
        "    }",
        "",
        "    // Force-reference to ensure runtime_core.obj is pulled in on MSVC",
        "    // before reflection_api.obj (both in chaos_runtime_core.lib).",
        "    (void)chaos::il2cpp::runtime_core::GetCurrentRuntimeState();",
        "",
        "    // Verify dispatch table is registered (codegen static initializer).",
        "    auto* first_entry = RuntimeDispatchLookupBySlot(0u, 0u);",
        "    if (first_entry == nullptr) {",
        '        std::fprintf(stderr, "FATAL: no dispatch table registered by codegen TU\\n");',
        "        return 1;",
        "    }",
        "    (void)first_entry;",
    ])

    # Apply D3 patch
    if has_patch_data:
        lines.extend([
            "",
            "    // ── D3 dispatch hotpatch via ApplyPatchFromMemory ────────────────",
            f'    PatchContext* patch_ctx = ApplyPatchFromMemory(kPatchData, kPatchDataSize, "{host_class_name}");',
            "    if (patch_ctx == nullptr) {",
            '        std::fprintf(stderr, "FATAL: ApplyPatchFromMemory failed\\n");',
            "        return 1;",
            "    }",
            "    uint32_t d3_patched_count = patch_ctx->method_count;",
            "    if (d3_patched_count == 0u) {",
            f'        std::fprintf(stderr, "WARN: ApplyPatchFromMemory patched 0 methods (host_type_name=\\"{host_class_name}\\")\\n");',
            "    }",
            "",
        ])
    else:
        lines.extend([
            "",
            "    // ── D3 dispatch hotpatch disabled (no .patchdata) ──────────────",
            "    uint32_t d3_patched_count = 0u;",
            "    PatchContext* patch_ctx = nullptr;",
            "",
        ])

    lines.extend([
        "    // Seed the JSON output array.",
        '    std::printf("{\\n");',
        '    std::printf("  \\"schemaVersion\\": 1,\\n");',
        '    std::printf("  \\"assemblyName\\": \\"System.Private.CoreLib\\",\\n");',
        f'    std::printf("  \\"familyId\\": \\"{family_id}\\",\\n");',
        '    std::printf("  \\"verificationKind\\": \\"hotupdate-proof\\",\\n");',
        '    std::printf("  \\"totalMethods\\": %u,\\n", kMethodCount);',
        '    std::printf("  \\"d3PatchApplied\\": %s,\\n", (d3_patched_count > 0u) ? "true" : "false");',
        '    std::printf("  \\"d3PatchedCount\\": %u,\\n", d3_patched_count);',
        '    std::printf("  \\"results\\": [\\n");',
        "",
        "    uint32_t passed_count = 0u;",
        "    uint32_t failed_count = 0u;",
        "",
        "    for (uint32_t i = 0u; i < kMethodCount; i++) {",
        "        bool step_ok = true;",
        "        const uintptr_t expected_patched = kSentinelPatchBase + i;",
        "",
        "        // Step 1: Get dispatch entry via RuntimeDispatchLookupBySlot.",
        "        auto* entry = RuntimeDispatchLookupBySlot(0u, i);",
        "        if (entry == nullptr) {",
        '            std::fprintf(stderr, "FAIL[%u]: RuntimeDispatchLookupBySlot returned null\\n", i);',
        "            step_ok = false;",
        "        }",
        "",
        "        // Step 2: Verify direct_ptr is non-null (AOT codegen function).",
        "        if (entry != nullptr && entry->direct_ptr == nullptr) {",
        '            std::fprintf(stderr, "FAIL[%u]: dispatch entry direct_ptr is null\\n", i);',
        "            step_ok = false;",
        "        }",
        "",
    ])
    if has_patch_data:
        lines.extend([
            "        // Step 3: Verify dispatch entry has kDispatchPatched flag set.",
            "        bool patched_flag = (entry != nullptr) && (entry->flags & kDispatchPatched);",
            "        if (!patched_flag) {",
            '            std::fprintf(stderr, "FAIL[%u]: dispatch entry not patched (flags=0x%08x)\\n",',
            "                i, entry ? entry->flags : 0u);",
            "            step_ok = false;",
            "        }",
            "",
            "        // Step 4: Verify method_key is set (PatchMethod*).",
            "        uintptr_t method_key = (entry != nullptr) ? entry->method_key : 0u;",
            "        if (method_key == 0u) {",
            '            std::fprintf(stderr, "FAIL[%u]: method_key is null (PatchMethod* not set)\\n", i);',
            "            step_ok = false;",
            "        }",
            "",
            "        // Step 5: Call via InterpreterEntryDirect and verify sentinel return.",
            "        CHAOS_IL2CPP_INT32 patched_value = 0;",
            "        if (method_key != 0u) {",
            "            InterpreterEntryDirect(method_key, nullptr, &patched_value);",
            "        }",
            "        if (static_cast<uintptr_t>(patched_value) != expected_patched) {",
            '            std::fprintf(stderr, "FAIL[%u]: InterpreterEntryDirect returned 0x%08x, expected 0x%08zx\\n",',
            "                i, static_cast<unsigned>(patched_value), static_cast<size_t>(expected_patched));",
            "            step_ok = false;",
            "        }",
        ])
    else:
        lines.extend([
            "        // No .patchdata — skip patched-flag and interpreter verification.",
        ])

    lines.extend([
        "",
        "        if (step_ok) {",
        "            passed_count++;",
        "        } else {",
        "            failed_count++;",
        "        }",
        "",
        "        // Emit JSON result for this method.",
        '        const char* comma = (i + 1u < kMethodCount) ? "," : "";',
        "        std::printf(",
        '            "    {\\n"',
        '            "      \\"methodToken\\": %u,\\n"',
        '            "      \\"status\\": \\"%s\\",\\n"',
        '            "      \\"d3Patched\\": %s,\\n"',
        '            "      \\"revertVerified\\": false,\\n"',
        '            "      \\"semanticVerified\\": false\\n"',
        '            "    }%s\\n",',
    ])
    lines.extend([
        "            static_cast<unsigned>(kBaseToken + i),",
        "            step_ok ? \"passed\" : \"failed\",",
        "            (d3_patched_count > 0u) ? \"true\" : \"false\",",
        "            comma);",
        "    }",
        "",
        '    std::printf("  ],\\n");',
        '    std::printf("  \\"passedMethods\\": %u,\\n", static_cast<unsigned>(passed_count));',
        '    std::printf("  \\"failedMethods\\": %u\\n", static_cast<unsigned>(failed_count));',
        '    std::printf("}\\n");',
        "",
        "    // ── D3 Unpatch ────────────────────────────────────────────────────",
    ])
    if has_patch_data:
        lines.extend([
            "    if (patch_ctx != nullptr) {",
            "        if (!Unpatch(patch_ctx)) {",
            '            std::fprintf(stderr, "FATAL: Unpatch failed\\n");',
            "            return 1;",
            "        }",
            "",
            "        // Verify all dispatch entry flags are cleared after Unpatch.",
            "        for (uint32_t j = 0u; j < kMethodCount; j++) {",
            "            auto* e = RuntimeDispatchLookupBySlot(0u, j);",
            "            if (e != nullptr && (e->flags & kDispatchPatched)) {",
            '                std::fprintf(stderr, "FAIL[unpatch]: entry[%u] still patched after Unpatch (flags=0x%08x)\\n",',
            "                    j, e->flags);",
            "                failed_count++;",
            "            }",
            "        }",
            "    }",
        ])
    lines.extend([
        "    return (failed_count == 0u) ? 0 : 1;",
        "}",
    ])


def _generate_hotupdate_cmake_full(family_slug: str) -> str:
    """Generate the complete CMakeLists.txt with D3 hotupdate target."""
    ns_slug = _ns_slug_from_family_id(family_id=f"family/System.Private.CoreLib/{family_slug.replace('-', '/')}")
    target = f"chaos_hotupdate_{ns_slug}"

    host_cpp = "hotupdate/genuine-fixed/native-aot.generated.cpp"
    test_cpp = "hotupdate/HotUpdateTest.cpp"

    cmake_sources = (
        f"    {test_cpp}\n"
        f"    {host_cpp}\n"
    )

    dummy_lib = f"family_{ns_slug}_dummy"
    return (
        f"add_library({dummy_lib} INTERFACE)\n"
        "\n"
        "# Per-family D3 hotupdate test.\n"
        "# The genuine AOT TU provides dispatch table + NameIndex via static init;\n"
        "# HotUpdateTest.cpp embeds .patchdata and calls ApplyPatchFromMemory.\n"
        "# No patch C++ TU needed — the interpreter runs the patched IL directly.\n"
        "\n"
        f"if(EXISTS ${{CMAKE_CURRENT_SOURCE_DIR}}/{test_cpp})\n"
        f"add_executable({target}\n"
        f"{cmake_sources}"
        ")\n"
        f"target_compile_features({target} PRIVATE cxx_std_17)\n"
        f"target_compile_definitions({target} PRIVATE CHAOS_RUNTIME_ABI_STATIC)\n"
        f"target_include_directories({target} PRIVATE\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/common\n"
        "    ${CMAKE_SOURCE_DIR}/contracts/native/v0\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/runtime-core\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/bootstrap\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/interpreter\n"
        "    ${CMAKE_SOURCE_DIR}/verification/foundation-dll/System.Private.CoreLib\n"
        ")\n"
        "# Force-include hotupdate config so CodeGen-generated code can find\n"
        "# chaos_managed_pointer_local_slot_tag, chaos_is_string_id, etc.\n"
        f"target_compile_options({target} PRIVATE\n"
        '    $<$<CXX_COMPILER_ID:MSVC>:/FI"${CMAKE_SOURCE_DIR}/verification/foundation-dll/System.Private.CoreLib/native_hotupdate_config.h">\n'
        '    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-include${CMAKE_SOURCE_DIR}/verification/foundation-dll/System.Private.CoreLib/native_hotupdate_config.h>\n'
        ")\n"
        "# fmt requires /utf-8 on MSVC\n"
        f"target_compile_options({target} PRIVATE\n"
        "    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>\n"
        ")\n"
        f"target_link_libraries({target} PRIVATE\n"
        "    chaos_hot_update\n"
        "    chaos_bootstrap\n"
        "    chaos_runtime_core\n"
        "    chaos_interpreter\n"
        "    chaos_bdwgc\n"
        "    chaos_common\n"
        # Note: force-reference in HotUpdateTest.cpp (GetCurrentRuntimeState())
        # resolves MSVC intra-lib circular deps — no WHOLEARCHIVE needed.
        ")\n"
        "endif()\n"
    )


def generate_family(family_slug: str) -> dict[str, Any]:
    """Generate the D3 hotupdate test for a single family."""
    print(f"\n{'='*60}")
    print(f"Family: {family_slug}")
    print(f"{'='*60}")

    # Discover symbols
    host_symbols = _discover_host_symbols(family_slug)
    if not host_symbols:
        print(f"  [SKIP] no host symbols found at {_VERIFICATION / family_slug / 'il2cpp_dist/genuine/generated/'}")
        return {"family": family_slug, "artifacts": []}

    print(f"  Host symbols: {len(host_symbols)}")

    method_subject_ids = _load_contract(family_slug)
    print(f"  Contract methods: {len(method_subject_ids)}")

    # Generate test
    test_source = _generate_hotupdate_test(
        family_slug, host_symbols, method_subject_ids,
    )
    test_dir = _VERIFICATION / family_slug / "il2cpp_dist" / "hotupdate"
    test_dir.mkdir(parents=True, exist_ok=True)
    test_path = test_dir / "HotUpdateTest.cpp"
    test_path.write_text(test_source, encoding="utf-8")
    print(f"  Test: {test_path.relative_to(_REPO_ROOT)}")

    # Fix genuine TU: inject missing structs/type-ids so it compiles standalone.
    ns_slug = _ns_slug_from_family_id(f"family/System.Private.CoreLib/{family_slug.replace('-', '/')}")
    _fix_genuine_for_hotupdate_keep_extern(family_slug, ns_slug)

    # Append D3 hotupdate target to existing CMakeLists.txt
    # (preserves benchmark static library target already in the file).
    cmake_hotupdate_section = _generate_hotupdate_cmake_full(family_slug)
    cmake_path = _VERIFICATION / family_slug / "il2cpp_dist" / "CMakeLists.txt"
    existing_cmake = cmake_path.read_text(encoding="utf-8") if cmake_path.exists() else ""
    # Check if hotupdate section is already present (idempotent).
    hotupdate_marker = f"Per-family D3 hotupdate test"
    if hotupdate_marker not in existing_cmake:
        combined = existing_cmake.rstrip() + "\n\n" + cmake_hotupdate_section
        cmake_path.write_text(combined, encoding="utf-8")
        print(f"  CMake: {cmake_path.relative_to(_REPO_ROOT)} (appended hotupdate target)")
    else:
        print(f"  CMake: {cmake_path.relative_to(_REPO_ROOT)} (hotupdate target already present, skipped)")

    artifacts = [
        str(test_path.relative_to(_REPO_ROOT)),
    ]
    return {"family": family_slug, "artifacts": [a for a in artifacts if a]}


def _fix_genuine_for_hotupdate_keep_extern(family_slug: str, ns_slug: str) -> Path | None:
    """Copy the genuine native-aot.generated.cpp with extern wrappers KEPT
    and missing type-ids injected.

    The fixed copy goes to native/hotupdate/genuine-fixed/native-aot.generated.cpp
    so the original genuine file remains untouched.
    """
    src = _VERIFICATION / family_slug / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
    if not src.exists():
        return None

    dst = _VERIFICATION / family_slug / "il2cpp_dist" / "hotupdate" / "genuine-fixed" / "native-aot.generated.cpp"
    _rename_and_fix_patch_file(src, dst, ns_slug, suffix="genuine_fixed")
    return dst


def _inject_missing_structs_and_type_ids(content: str) -> str:
    """Inject missing struct definitions and type-id constexprs for types that
    CodeGen uses via reinterpret_cast/new but never defines struct layouts for.

    Detection: scan for `reinterpret_cast<chaos_type_*>` and `new chaos_type_*`
    patterns, then check if a corresponding `struct chaos_type_*` exists.
    Also scan for `chaos_type_id_*` references without corresponding constexpr.

    Each TU gets its own definitions (TU-local, no ODR violation across TUs).
    """
    lines = content.splitlines(keepends=True)

    # ---- Detect all missing struct types ----
    # Find all types referenced via reinterpret_cast or new
    referenced_types: set[str] = set()
    for line in lines:
        m = re.findall(r'reinterpret_cast<(chaos_type_\S+)\*>', line)
        for t in m:
            referenced_types.add(t)
        m = re.findall(r'new\s+(chaos_type_\S+)\s*\{', line)
        for t in m:
            referenced_types.add(t)

    if not referenced_types:
        return content

    defined_structs: set[str] = set()
    for line in lines:
        m = re.match(r'^\s*struct\s+(chaos_type_\S+)\s*(?::|$)'.replace('(?::|$)', '(?::.*)?$'), line.strip())
        if m:
            defined_structs.add(m.group(1))

    missing_structs = sorted(referenced_types - defined_structs)
    if not missing_structs:
        return content

    # Build a set of known struct layouts keyed by type name suffix
    known_structs = {
        'chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo': [
            "    CHAOS_IL2CPP_INTPTR declaring_type_handle = 0;\n",
            "    CHAOS_IL2CPP_INTPTR runtime_method_handle = 0;\n",
            "    CHAOS_IL2CPP_INTPTR generic_definition_method_handle = 0;\n",
            "    CHAOS_IL2CPP_INTPTR generic_argument_type_handle = 0;\n",
            "    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;\n",
            "    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;\n",
        ],
        'chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo': [
            "    CHAOS_IL2CPP_INTPTR runtime_field_handle = 0;\n",
            "    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;\n",
            "    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;\n",
        ],
        'chaos_type_System_Private_CoreLib_System_Reflection_ParameterInfo': [
            "    CHAOS_IL2CPP_INTPTR runtime_parameter_handle = 0;\n",
            "    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;\n",
            "    CHAOS_IL2CPP_INT32 runtime_metadata_token_value = 0;\n",
        ],
        'chaos_type_System_Private_CoreLib_System_Reflection_Assembly': [
            "    CHAOS_IL2CPP_INTPTR runtime_assembly_name_value = 0;\n",
        ],
        'chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName': [
            "    CHAOS_IL2CPP_INTPTR runtime_name_value = 0;\n",
            "    CHAOS_IL2CPP_INTPTR runtime_name = 0;\n",
        ],
        'chaos_type_System_Private_CoreLib_System_Reflection_Module': [
            "    CHAOS_IL2CPP_INTPTR runtime_module_name_value = 0;\n",
            "    CHAOS_IL2CPP_INTPTR runtime_assembly_name_value = 0;\n",
        ],
    }

    # Find the last user-defined struct (to insert after)
    last_struct_end = -1
    for idx, line in enumerate(lines):
        if re.match(r'^\s*struct\s+chaos_type_\S+\b', line.strip()):
            # Find the closing };
            for j in range(idx, min(idx + 30, len(lines))):
                if lines[j].strip() == '};':
                    last_struct_end = max(last_struct_end, j)
                    break

    if last_struct_end >= 0:
        insert_at = last_struct_end + 1
        insert_lines: list[str] = []
        for struct_name in missing_structs:
            if struct_name in known_structs:
                fields = known_structs[struct_name]
                insert_lines.append(
                    f"struct {struct_name} : public chaos_type_System_Private_CoreLib_System_Object\n{{\n"
                )
                for f in fields:
                    insert_lines.append(f)
                insert_lines.append("};\n\n")
            else:
                insert_lines.append(
                    f"struct {struct_name} : public chaos_type_System_Private_CoreLib_System_Object\n{{\n"
                    f"    // TODO: define fields for {struct_name}\n"
                    "};\n\n"
                )
        lines[insert_at:insert_at] = insert_lines
        content = ''.join(lines)
        lines = content.splitlines(keepends=True)

    # ---- Inject missing type-id constexprs ----
    # Find all chaos_type_id_ references that look like identifiers (not definitions)
    referenced_ids: set[str] = set()
    for line in lines:
        for m in re.finditer(r'\bchaos_type_id_System_Private_CoreLib_\w+\b', line):
            ref = m.group()
            # Skip if this line is a definition
            if 'constexpr' in line and f'{ref} =' in line:
                continue
            referenced_ids.add(ref)

    defined_ids: set[str] = set()
    for line in lines:
        m = re.match(
            r'^\s*constexpr\s+CHAOS_IL2CPP_INTPTR\s+(chaos_type_id_\S+)\s*=',
            line.strip(),
        )
        if m:
            defined_ids.add(m.group(1))

    missing_ids = sorted(referenced_ids - defined_ids)
    if missing_ids:
        existing_values: set[int] = set()
        last_type_id_line = -1
        for idx, line in enumerate(lines):
            m = re.match(
                r'^\s*constexpr\s+CHAOS_IL2CPP_INTPTR\s+chaos_type_id_\S+\s*=\s*(\d+)\s*;',
                line.strip(),
            )
            if m:
                existing_values.add(int(m.group(1)))
                last_type_id_line = idx

        if last_type_id_line >= 0:
            next_val = next(
                (v for v in range(1, 1000) if v not in existing_values),
                1000,
            )
            indent = ' ' * (len(lines[last_type_id_line]) - len(lines[last_type_id_line].lstrip()))
            insert_lines = []
            for tid in missing_ids:
                while next_val in existing_values:
                    next_val += 1
                insert_lines.append(
                    f"{indent}constexpr CHAOS_IL2CPP_INTPTR {tid} = {next_val};\n"
                )
                next_val += 1

            insert_at = last_type_id_line + 1
            while insert_at < len(lines) and lines[insert_at].strip() == '':
                insert_at += 1
            if insert_at > 0 and lines[insert_at - 1].strip() != '':
                insert_lines.insert(0, '\n')
            lines.insert(insert_at, ''.join(insert_lines))
            content = ''.join(lines)

    return content


def _inject_reflection_bridge_stubs(content: str) -> str:
    """Inject missing chaos_reflection_get_* stub definitions.

    The AOT lowering planner emits `extern "C"` wrappers that call these
    helper functions, but the CodeGen stage does not always emit the
    corresponding helper definitions for reflection query functions that
    don't have concrete switch-case entries. Each TU needs its own copy
    since these are in the anonymous namespace.

    This is now generalized: ANY `chaos_reflection_*` function
    that is called but not defined in the TU will receive a stub.
    """
    # Find all called-but-not-defined chaos_reflection_* functions
    called_funcs: set[str] = set()
    defined_funcs: set[str] = set()

    for line in content.splitlines():
        stripped = line.strip()
        # Detect calls: `func(args` pattern (inside expressions)
        for m in re.finditer(r'\b(chaos_reflection_\w+)\s*\(', stripped):
            call = m.group(1)
            if call not in ('chaos_reflection_create_string_utf8_copy',
                            'chaos_reflection_concat_string_pair_values',
                            'chaos_reflection_get_string_utf8',
                            'chaos_reflection_create_reference_array',
                            'chaos_reflection_create_type_value',
                            'chaos_reflection_create_array_value',
                            'chaos_reflection_create_struct_value',
                            'chaos_make_string_id_value',
                            'chaos_extract_string_id',
                            'chaos_is_string_id',
                            'chaos_resolve_managed_pointer',
                            'chaos_resolve_managed_value_pointer'):
                called_funcs.add(call)
        # Detect definitions: any typed return + func name at line start
        for m in re.finditer(
            r'\b(CHAOS_IL2CPP_INTPTR|CHAOS_IL2CPP_INT32|CHAOS_IL2CPP_UINT8|'
            r'CHAOS_IL2CPP_SIZE|int32_t|intptr_t|bool|void)\s+'
            r'(chaos_reflection_\w+)\s*\(',
            stripped):
            defined_funcs.add(m.group(2))

    missing = sorted(called_funcs - defined_funcs)
    # Filter helpers that are definitely defined
    missing = [f for f in missing
               if not f.startswith('chaos_reflection_get_string')
               and not f.startswith('chaos_reflection_create_string')
               and not f.startswith('chaos_reflection_concat')
               and not f.startswith('chaos_reflection_create_reference')
               and not f.startswith('chaos_reflection_create_type')
               and not f.startswith('chaos_reflection_create_array')
               and not f.startswith('chaos_reflection_create_struct')]

    if not missing:
        return content

    # Generate stubs for all missing functions
    stubs = []
    stubs.append("// Reflection bridge stubs injected for hotupdate test compilation.\n")
    stubs.append("// These are TODO stubs: the runtime backend does not yet implement\n")
    stubs.append("// these specific reflection queries.\n\n")

    for func in missing:
        used_params = ["chaos_arg_0"]
        # Scan call sites to count parameters
        call_pattern = f'{func}('
        for line in content.splitlines():
            if call_pattern in line:
                idx = line.index(call_pattern) + len(call_pattern)
                depth = 0
                args: list[str] = []
                current = ""
                for ch in line[idx:]:
                    if ch == '(':
                        depth += 1
                        current += ch
                    elif ch == ')':
                        if depth == 0:
                            if current.strip():
                                args.append(current.strip())
                            break
                        depth -= 1
                        current += ch
                    elif ch == ',' and depth == 0:
                        args.append(current.strip())
                        current = ""
                    else:
                        current += ch
                if args:
                    real_args = [a for a in args if a not in ('void', '')]
                    if real_args:
                        used_params = [f"chaos_arg_{i}" for i in range(len(real_args))]
                break

        param_list = ", ".join(f"CHAOS_IL2CPP_INTPTR {p}" for p in used_params)
        stubs.append(f"CHAOS_IL2CPP_INTPTR {func}({param_list})\n")
        stubs.append("{\n")
        for p in used_params:
            stubs.append(f"    (void){p};\n")
        stubs.append("    return static_cast<CHAOS_IL2CPP_INTPTR>(0);\n")
        stubs.append("}\n\n")

    stub_text = "".join(stubs)

    # Insert before the first extern "C" chaos_external_runtime_ wrapper
    marker = 'extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_'
    idx = content.find(marker)
    if idx < 0:
        return content

    line_start = content.rfind('\n', 0, idx) + 1
    content = content[:line_start] + stub_text + content[line_start:]
    return content


def _inject_interpreter_entry_include(content: str) -> str:
    """Inject #include "interpreter_entry.h" and using declaration.

    The codegen emits &InterpreterEntryDirect as the interrupt_ptr value for
    every dispatch table entry, but the generated TU does not include the
    header.  This injection adds the include + using so the dispatch table
    compiles.
    """
    marker = '#include "dispatch_table.h"'
    include_line = '#include "interpreter_entry.h"'
    if include_line in content:
        return content  # already injected

    idx = content.find(marker)
    if idx < 0:
        return content  # can't find insertion point

    # Insert #include + using after the dispatch_table.h line
    line_end = content.find('\n', idx)
    insert_at = line_end + 1
    inject = '\n' + include_line + '\nusing chaos::il2cpp::runtime_core::InterpreterEntryDirect;\n'
    content = content[:insert_at] + inject + content[insert_at:]
    return content


def _rename_and_fix_patch_file(src: Path, dst: Path, ns_slug: str, suffix: str = "patch") -> None:
    """Copy a CodeGen-generated C++ file with RunNativeAot renamed to avoid symbol collision.

    Also uncomments the stripped chaos_managed_pointer_local_slot_tag constexpr
    so each TU has its own anonymous-namespace definition.
    """
    content = src.read_text(encoding="utf-8")
    rename_from = 'extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)'
    rename_to = f'extern "C" int RunNativeAot_{ns_slug}_{suffix}(CHAOS_IL2CPP_INT32 chaos_entry_index)'
    content = content.replace(rename_from, rename_to)

    # Uncomment the stripped chaos_managed_pointer_local_slot_tag constexpr
    content = content.replace(
        '// constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;  // stripped: provided by native_hotupdate_config.h',
        'constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;',
    )
    content = content.replace(
        '// constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;',
        'constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;',
    )

    # Inject missing reflection bridge stubs for functions that CodeGen hasn't
    # emitted yet (e.g., chaos_reflection_get_fields).
    content = _inject_reflection_bridge_stubs(content)

    # Inject missing struct definitions and type-id constexprs
    content = _inject_missing_structs_and_type_ids(content)

    # Fix `__s[N] = &chaos_locals[N]` → `__s[N] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[N])`
    # Codegen emits ldloca as bare `&chaos_locals[N]` which fails on MSVC (cannot
    # implicitly convert pointer to intptr_t).
    content = re.sub(
        r'(&chaos_locals\[\d+\])',
        r'reinterpret_cast<CHAOS_IL2CPP_INTPTR>(\1)',
        content,
    )

    # Inject InterpreterEntryDirect reference (needed by codegen-emitted dispatch table).
    # The codegen emits &InterpreterEntryDirect as the interrupt_ptr value for every
    # dispatch table entry, but the generated TU does not include interpreter_entry.h.
    content = _inject_interpreter_entry_include(content)

    dst.parent.mkdir(parents=True, exist_ok=True)
    dst.write_text(content, encoding="utf-8")
    print(f"  {suffix.capitalize()} renamed: {dst.relative_to(_REPO_ROOT)}")


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Generate per-family hotupdate C++ tests")
    parser.add_argument("--families", nargs="*", help="Families to process (default: all)")
    args = parser.parse_args()

    families = args.families or FAMILIES
    print(f"Generating per-family D3 hotupdate tests (pure dispatch-table + interpreter) - {len(families)} families")

    for family_slug in families:
        generate_family(family_slug)

    print(f"\nDone. D3 test and CMake artifacts in verification/foundation-dll/.../il2cpp_dist/hotupdate/")


if __name__ == "__main__":
    main()
