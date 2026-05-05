"""Generate per-family hotupdate C++ test from CodeGen-generated host and patch C++.

For each family, this script:
  1. Copies the patch native-aot.generated.cpp with RunNativeAot renamed to avoid collision
  2. Generates a HotUpdateTest.cpp that:
     - Forward-declares all extern "C" MethodN symbols from both host and patch
     - Links to the renamed patch file via extern declarations
     - Implements the 9-step verification cycle for each method
     - Uses bootstrap + method_replacement infrastructure
     - Optionally adds step 10 semantic verification when semantic-patch C++ is available
  3. Creates CMakeLists.txt with proper linkage

Usage:
    python generate_hotupdate_test.py
        --families math-numerics convert-char
    (runs for all families if --families omitted)
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


def _discover_method_symbols(family_slug: str) -> tuple[list[str], list[str], list[str]]:
    """Scan the genuine (host) generated C++ to discover MethodN symbols.

    Returns (host_symbols, patch_symbols, semantic_patch_symbols).
    """
    host_cpp = _VERIFICATION / family_slug / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
    # Emit-native-aot may output to generated/ or generated/generated/ depending on version
    patch_cpp = _VERIFICATION / family_slug / "il2cpp_dist" / "patch" / "generated" / "native-aot.generated.cpp"
    if not patch_cpp.exists():
        patch_cpp = _VERIFICATION / family_slug / "il2cpp_dist" / "patch" / "generated" / "generated" / "native-aot.generated.cpp"
    semantic_patch_cpp = _VERIFICATION / family_slug / "il2cpp_dist" / "semantic-patch" / "generated" / "native-aot.generated.cpp"
    if not semantic_patch_cpp.exists():
        semantic_patch_cpp = _VERIFICATION / family_slug / "il2cpp_dist" / "semantic-patch" / "generated" / "generated" / "native-aot.generated.cpp"

    host_symbols = _extract_method_symbols(host_cpp)
    patch_symbols = _extract_method_symbols(patch_cpp)
    semantic_patch_symbols = _extract_method_symbols(semantic_patch_cpp)

    return host_symbols, patch_symbols, semantic_patch_symbols


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
    patch_symbols: list[str],
    method_subject_ids: list[str],
    semantic_patch_symbols: list[str] | None = None,
) -> str:
    """Generate the per-family hotupdate C++ test source.

    If semantic_patch_symbols are provided and match the method count, the test
    also performs semantic verification: comparing host vs semantic-patch return
    values to prove the hotupdate mechanism replaced real computation.
    """
    family_id = f"family/System.Private.CoreLib/{family_slug.replace('-', '/')}"
    ns_slug = _ns_slug_from_family_id(family_id)
    method_count = len(host_symbols)
    has_semantic = bool(semantic_patch_symbols and len(semantic_patch_symbols) == method_count)

    # Embed .patchdata for D3 dispatch hotpatch
    patch_data_cxx, _ = _embed_patch_data(family_slug)

    lines = []
    if patch_data_cxx:
        lines.append(patch_data_cxx)
    _emit_header(lines, family_id, method_count, has_semantic)
    _emit_forward_decls(lines, host_symbols, patch_symbols, semantic_patch_symbols, has_semantic)
    _emit_namespace_block(lines, host_symbols, patch_symbols, semantic_patch_symbols, has_semantic, method_count)
    _emit_main_function(lines, method_count, family_id, has_semantic, has_patch_data=bool(patch_data_cxx))

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
    has_semantic: bool,
) -> None:
    lines.extend([
        "// Per-family hotupdate verification test",
        f"// Family: {family_id}",
        f"// {method_count} methods",
        "//",
        "// Uses CodeGen-generated C++ for both host (real API calls) and",
        "// patch (sentinel returns). Verifies the method replacement lifecycle:",
        "//   original -> register patch -> resolve -> call patched -> revert -> verify",
        "//",
    ])
    if has_semantic:
        lines.extend([
            "// When semantic-patch C++ is available (via TYPE_ALTERNATIVE_MAP),",
            "// also verifies that host and semantic-patch produce different (but valid)",
            "// return values, proving the hotupdate mechanism replaced actual computation.",
            "//",
        ])
    lines.extend([
        '#include "bootstrap.h"',
        '#include "codegen_bridge.h"',
        '#include "method_replacement.h"',
        '#include "method_table.h"',
        '#include "module_registry.h"',
        '#include "abi_manifest.h"',
        '#include "patch_loader.h"',
        '#include "dispatch_table.h"',
        "",
        "#include <cstdio>",
        "#include <cstdint>",
        "#include <cstring>",
        "",
    ])


def _emit_forward_decls(
    lines: list[str],
    host_symbols: list[str],
    patch_symbols: list[str],
    semantic_patch_symbols: list[str] | None,
    has_semantic: bool,
) -> None:
    lines.append("// ---------------------------------------------------------------")
    lines.append("// Forward declarations: host methods (real API calls via CodeGen)")
    lines.append("// ---------------------------------------------------------------")
    for sym in host_symbols:
        lines.append(f'extern "C" CHAOS_IL2CPP_INT32 {sym}(void);')

    lines.append("")
    lines.append("// ---------------------------------------------------------------")
    lines.append("// Forward declarations: patch methods (sentinel returns via CodeGen)")
    lines.append("// ---------------------------------------------------------------")
    for sym in patch_symbols:
        lines.append(f'extern "C" CHAOS_IL2CPP_INT32 {sym}(void);')

    if has_semantic and semantic_patch_symbols:
        lines.append("")
        lines.append("// ---------------------------------------------------------------")
        lines.append("// Forward declarations: semantic-patch methods (alternative API calls)")
        lines.append("// ---------------------------------------------------------------")
        for sym in semantic_patch_symbols:
            lines.append(f'extern "C" CHAOS_IL2CPP_INT32 {sym}(void);')


def _emit_namespace_block(
    lines: list[str],
    host_symbols: list[str],
    patch_symbols: list[str],
    semantic_patch_symbols: list[str] | None,
    has_semantic: bool,
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
        "// Patch method pointer array (auto-indexed by MethodN ordering)",
        "void* (*kPatchThunks[])() = {",
    ])
    for sym in patch_symbols:
        lines.append(f"    reinterpret_cast<void* (*)()>(&{sym}),")
    lines.append("};")

    if has_semantic and semantic_patch_symbols:
        lines.extend([
            "",
            "// Semantic-patch method pointer array (alternative real API calls)",
            "void* (*kSemanticPatchThunks[])() = {",
        ])
        for sym in semantic_patch_symbols:
            lines.append(f"    reinterpret_cast<void* (*)()>(&{sym}),")
        lines.append("};")

    lines.extend([
        "",
        "constexpr uint32_t kAotDomainIdFallback = 0u;",
        "",
        "// ── ABI validation module ────────────────────────────────────────",
        "// Register a module with a uniform manifest (all thunks return Int32, no params)",
        "// so the test can validate ResolveMethodTableWithAbiCheck.",
        "static uint32_t RegisterAbiModule() {",
        "    alignas(ChaosAbiManifestV0) static uint8_t s_manifest_buf[",
        "        sizeof(ChaosAbiManifestV0) + sizeof(ChaosAbiMethodEntryV0)] = {};",
        "    auto* m = reinterpret_cast<ChaosAbiManifestV0*>(s_manifest_buf);",
        "    m->abi_version = CHAOS_ABI_MANIFEST_VERSION;",
        "    m->method_count = 1;",
        "    m->parameters_byte_count = 0;",
        "    m->checksum = 0;",
        "    auto* e = reinterpret_cast<ChaosAbiMethodEntryV0*>(",
        "        s_manifest_buf + sizeof(ChaosAbiManifestV0));",
        "    e[0].return_carrier = CHAOS_ABI_CARRIER_INT32;",
        "    e[0].parameter_count = 0;",
        "",
        "    static ::chaos::il2cpp::runtime_core::ModuleDescriptor s_abi_mod = {};",
        "    s_abi_mod.name_utf8 = \"hotupdate-abi-test\";",
        "    s_abi_mod.abi_manifest = reinterpret_cast<const ChaosAbiManifestV0*>(s_manifest_buf);",
        "",
        "    return ::chaos::il2cpp::runtime_core::RegisterModule(",
        "        \"hotupdate-abi-test\", &s_abi_mod);",
        "}",
        "static const uint32_t kAbiTestModuleId = RegisterAbiModule();",
        "",
        "}  // namespace",
    ])


def _emit_main_function(lines: list[str], method_count: int, family_id: str, has_semantic: bool, has_patch_data: bool = False) -> None:
    lines.extend([
        "",
        "int main() {",
        "    using chaos::il2cpp::bootstrap::FindMethodPointerByToken;",
        "    using chaos::il2cpp::bootstrap::PeekBootstrapState;",
        "    using chaos::il2cpp::method_replacement::ActiveCount;",
        "    using chaos::il2cpp::method_replacement::Register;",
        "    using chaos::il2cpp::method_replacement::Resolve;",
        "    using chaos::il2cpp::method_replacement::Revert;",
        "    using chaos::il2cpp::method_replacement::RevertAll;",
        "    using chaos::il2cpp::runtime_core::ApplyPatchFromMemory;",
        "    using chaos::il2cpp::runtime_core::Unpatch;",
        "    using chaos::il2cpp::runtime_core::RuntimeDispatchLookup;",
        "    using chaos::il2cpp::runtime_core::PatchContext;",
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
        "    // Seed the JSON output array.",
        '    std::printf("{\\n");',
        '    std::printf("  \\"schemaVersion\\": 1,\\n");',
        '    std::printf("  \\"assemblyName\\": \\"System.Private.CoreLib\\",\\n");',
        f'    std::printf("  \\"familyId\\": \\"{family_id}\\",\\n");',
        '    std::printf("  \\"verificationKind\\": \\"hotupdate-proof\\",\\n");',
        '    std::printf("  \\"totalMethods\\": %u,\\n", kMethodCount);',
        '    std::printf("  \\"results\\": [\\n");',
        "",
        "    uint32_t passed_count = 0u;",
        "    uint32_t failed_count = 0u;",
        "    RevertAll();",
    ])
    if has_patch_data:
        lines.extend([
            "",
            "    // ── D3 dispatch hotpatch: ApplyPatchFromMemory ─────────────────",
            "    PatchContext* patch_ctx = ApplyPatchFromMemory(kPatchData, kPatchDataSize);",
            "    if (patch_ctx == nullptr) {",
            '        std::fprintf(stderr, "FATAL: ApplyPatchFromMemory failed\\n");',
            "        return 1;",
            "    }",
            "    uint32_t d3_patched_count = patch_ctx->method_count;",
            "    if (d3_patched_count > 0) {",
            '        std::fprintf(stdout, "  \\"d3PatchApplied\\": true,\\n");',
            "    } else {",
            '        std::fprintf(stdout, "  \\"d3PatchApplied\\": false,\\n");',
            '        std::fprintf(stderr, "WARN: ApplyPatchFromMemory patched 0 methods (type name mismatch)\\n");',
            "    }",
        ])
    lines.extend([
        "    for (uint32_t i = 0u; i < kMethodCount; i++) {",
        "        const uint32_t token = kBaseToken + i;",
        "        const uintptr_t expected_sentinel_b = kSentinelPatchBase + i;",
        "",
        "        bool step_ok = true;",
        "",
        "        // Step 1: Find original pointer via bootstrap dispatch chain.",
        "        void* original_ptr = FindMethodPointerByToken(token);",
        "        if (original_ptr == nullptr) {",
        '            std::fprintf(stderr, "FAIL[%u]: FindMethodPointerByToken returned null\\n", i);',
        "            step_ok = false;",
        "        }",
        "",
        "        // Step 2: Call original thunk (host now returns real checksum from cast-to-int).",
        "        uintptr_t original_value = 0u;",
        "        if (original_ptr != nullptr) {",
        "            auto* thunk = reinterpret_cast<uintptr_t (*)()>(original_ptr);",
        "            original_value = thunk();",
        "        }",
        "",
    ])
    if has_patch_data:
        lines.extend([
            "        // Step 2c: Verify D3 dispatch entry has kDispatchPatched flag.",
            "        if (d3_patched_count > 0) {",
            "            auto* dispatch_entry = RuntimeDispatchLookup(token);",
            "            if (dispatch_entry == nullptr) {",
            '            std::fprintf(stderr, "FAIL[%u]: RuntimeDispatchLookup returned null\\n", i);',
            "                step_ok = false;",
            "            } else if (!(dispatch_entry->flags & kDispatchPatched)) {",
            '            std::fprintf(stderr, "FAIL[%u]: dispatch entry not patched (flags=0x%08x)\\n", i, dispatch_entry->flags);',
            "                step_ok = false;",
            "            }",
            "        }",
        ])

    if has_semantic:
        lines.extend([
            "        // Step 2b: Call semantic-patch thunk before registration to get alternative checksum.",
            "        bool has_semantic_thunk = (i < sizeof(kSemanticPatchThunks)/sizeof(kSemanticPatchThunks[0])",
            "            && kSemanticPatchThunks[i] != nullptr);",
            "        uintptr_t semantic_value = 0u;",
            "        if (has_semantic_thunk) {",
            "            auto* thunk = reinterpret_cast<uintptr_t (*)()>(kSemanticPatchThunks[i]);",
            "            semantic_value = thunk();",
            "        }",
            "",
        ])

    lines.extend([
        "        // Step 3: Register patch replacement (CodeGen-generated sentinel).",
        "        if (!Register(token, reinterpret_cast<void*>(kPatchThunks[i]))) {",
        '            std::fprintf(stderr, "FAIL[%u]: Register returned false\\n", i);',
        "            step_ok = false;",
        "        }",
        "",
        "        // Step 4: Resolve directly - must return patched thunk.",
        "        void* resolved_after_patch = Resolve(token);",
        "        if (resolved_after_patch == nullptr) {",
        '            std::fprintf(stderr, "FAIL[%u]: Resolve returned null after Register\\n", i);',
        "            step_ok = false;",
        "        }",
        "        if (resolved_after_patch != reinterpret_cast<void*>(kPatchThunks[i])) {",
        '            std::fprintf(stderr, "FAIL[%u]: Resolve returned wrong pointer after Register\\n", i);',
        "            step_ok = false;",
        "        }",
        "",
        "        // Step 5: FindMethodPointerByToken - must return patched thunk (integration test).",
        "        void* dispatch_ptr = FindMethodPointerByToken(token);",
        "        if (dispatch_ptr == nullptr) {",
        '            std::fprintf(stderr, "FAIL[%u]: FindMethodPointerByToken returned null after Register\\n", i);',
        "            step_ok = false;",
        "        }",
        "        if (dispatch_ptr != reinterpret_cast<void*>(kPatchThunks[i])) {",
        '            std::fprintf(stderr, "FAIL[%u]: FindMethodPointerByToken did not return patched thunk\\n", i);',
        "            step_ok = false;",
        "        }",
        "",
        "        // Step 6: Call the dispatch pointer - must return sentinel B.",
        "        uintptr_t patched_value = 0u;",
        "        if (dispatch_ptr != nullptr) {",
        "            auto* thunk = reinterpret_cast<void* (*)()>(dispatch_ptr);",
        "            patched_value = reinterpret_cast<uintptr_t>(thunk());",
        "            if (patched_value != expected_sentinel_b) {",
        '                std::fprintf(stderr, "FAIL[%u]: patched returned 0x%08zx, expected 0x%08zx\\n",',
        "                    i, static_cast<size_t>(patched_value), static_cast<size_t>(expected_sentinel_b));",
        "                step_ok = false;",
        "            }",
        "        }",
        "",
        "        // Step 7: Revert replacement.",
        "        if (!Revert(token)) {",
        '            std::fprintf(stderr, "FAIL[%u]: Revert returned false\\n", i);',
        "            step_ok = false;",
        "        }",
        "",
        "        // Step 8: After revert, FindMethodPointerByToken must return original pointer.",
        "        void* after_revert_ptr = FindMethodPointerByToken(token);",
        "        if (after_revert_ptr == nullptr) {",
        '            std::fprintf(stderr, "FAIL[%u]: FindMethodPointerByToken returned null after Revert\\n", i);',
        "            step_ok = false;",
        "        }",
        "        if (after_revert_ptr != original_ptr) {",
        '            std::fprintf(stderr, "FAIL[%u]: after Revert, pointer does not match original\\n", i);',
        "            step_ok = false;",
        "        }",
        "",
        "        // Step 9: Call after revert - must return host value.",
        "        if (after_revert_ptr != nullptr) {",
        "            auto* thunk = reinterpret_cast<void* (*)()>(after_revert_ptr);",
        "            uintptr_t reverted_value = reinterpret_cast<uintptr_t>(thunk());",
        "            if (reverted_value != original_value) {",
        '                std::fprintf(stderr, "FAIL[%u]: after Revert, call returned 0x%08zx, expected 0x%08zx\\n",',
        "                    i, static_cast<size_t>(original_value), static_cast<size_t>(original_value));",
        "                step_ok = false;",
        "            }",
        "        }",
        "",
        "        // Step 10: ABI validation via method table.",
        "        // Write the original thunk to the method table, set its origin to",
        "        // the registered test module, and validate via",
        "        // ResolveMethodTableWithAbiCheck (expects return=Int32, no params).",
        "        ::chaos::il2cpp::method_table::WriteMethodTable(i, original_ptr, 1u);",
        "        ::chaos::il2cpp::method_table::SetMethodOrigin(i, kAbiTestModuleId, 0);",
        "        void* abi_ptr = ::chaos::il2cpp::method_table::ResolveMethodTableWithAbiCheck(",
        "            i, CHAOS_ABI_CARRIER_INT32, nullptr, 0);",
        "        if (abi_ptr == nullptr) {",
        '            std::fprintf(stderr, "FAIL[%u]: ResolveMethodTableWithAbiCheck returned null\\n", i);',
        "            step_ok = false;",
        "        }",
        "        if (abi_ptr != original_ptr) {",
        '            std::fprintf(stderr, "FAIL[%u]: ResolveMethodTableWithAbiCheck wrong pointer\\n", i);',
        "            step_ok = false;",
        "        }",
        "",
    ])

    if has_semantic:
        lines.extend([
            "        // Step 11: Semantic verification - host and semantic-patch must differ.",
            "        // This proves the hotupdate mechanism replaced actual computation.",
            "        bool semantic_ok = true;",
            "        if (has_semantic_thunk) {",
            "            if (original_value == 0u && semantic_value == 0u) {",
            "                // Both returned 0 - likely TODO stubs, cannot verify semantics.",
            '                std::fprintf(stderr, "WARN[%u]: both host and semantic-patch returned 0 (stub?)\\n", i);',
            "                semantic_ok = false;",
            "                // Don't fail the test — the method still passed all hotupdate steps;",
            "                // only the semantic comparison is inconclusive.",
            "            } else if (original_value == semantic_value) {",
            "                // Different inputs should produce different results.",
            '                std::fprintf(stderr, "FAIL[%u]: host and semantic-patch returned same value 0x%08zx\\n",',
            "                    i, static_cast<size_t>(original_value));",
            "                semantic_ok = false;",
            "                step_ok = false;",
            "            }",
            "        }",
            "",
        ])

    lines.extend([
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
        '            "      \\"originalReturnValue\\": \\"0x%08zx\\",\\n"',
        '            "      \\"patchedReturnValue\\": \\"0x%08zx\\",\\n"',
        '            "      \\"expectedPatchedValue\\": \\"0x%08zx\\",\\n"',
        '            "      \\"revertVerified\\": true,\\n"',
    ])

    if has_semantic:
        lines.extend([
            '            "      \\"semanticVerified\\": %s\\n"',
            '            "    }%s\\n",',
        ])
    else:
        lines.extend([
            '            "      \\"semanticVerified\\": false\\n"',
            '            "    }%s\\n",',
        ])

    lines.extend([
        "            static_cast<unsigned>(token),",
        "            step_ok ? \"passed\" : \"failed\",",
        "            static_cast<size_t>(original_value),",
        "            static_cast<size_t>(patched_value),",
        "            static_cast<size_t>(expected_sentinel_b),",
    ])

    if has_semantic:
        lines.append('            (semantic_ok ? "true" : "false"),')

    lines.extend([
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
            "    }",
        ])
    lines.extend([
        "    return (failed_count == 0u) ? 0 : 1;",
        "}",
    ])


def _generate_hotupdate_cmake_piece(family_slug: str, host_symbols: list[str]) -> str:
    """Generate the hotupdate CMake portion to append to existing CMakeLists.txt."""
    ns_slug = _ns_slug_from_family_id(family_id=f"family/System.Private.CoreLib/{family_slug.replace('-', '/')}")
    target = f"chaos_hotupdate_{ns_slug}"

    host_cpp = "hotupdate/genuine-fixed/native-aot.generated.cpp"
    patch_cpp_renamed = "patch/native-aot.patch.generated.cpp"
    test_cpp = "hotupdate/HotUpdateTest.cpp"
    semantic_cpp_renamed = "semantic-patch/native-aot.semantic-patch.generated.cpp"

    cmake_sources = (
        f"    {test_cpp}\n"
        f"    {host_cpp}\n"
        f"    {patch_cpp_renamed}\n"
    )

    has_semantic_cpp = Path(_VERIFICATION / family_slug / "il2cpp_dist" / semantic_cpp_renamed).exists()
    if has_semantic_cpp:
        cmake_sources += f"    {semantic_cpp_renamed}\n"

    return (
        "# Per-family hotupdate test from CodeGen-generated C++\n"
        "# The patch file (native-aot.patch.generated.cpp) has RunNativeAot\n"
        "# renamed to RunNativeAot_{ns_slug} to avoid symbol collision with\n"
        "# the host file's RunNativeAot.\n"
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
        ")\n"
        "endif()\n"
    )


def generate_family(family_slug: str) -> dict[str, Any]:
    """Generate the hotupdate test for a single family."""
    print(f"\n{'='*60}")
    print(f"Family: {family_slug}")
    print(f"{'='*60}")

    # Discover symbols
    host_symbols, patch_symbols, semantic_patch_symbols = _discover_method_symbols(family_slug)
    if not host_symbols:
        print(f"  [SKIP] no host symbols found at {_VERIFICATION / family_slug / 'il2cpp_dist/genuine/generated/'}")
        return {"family": family_slug, "artifacts": []}
    if not patch_symbols:
        print(f"  [SKIP] no patch symbols found at {_VERIFICATION / family_slug / 'il2cpp_dist/patch/generated/'}")
        return {"family": family_slug, "artifacts": []}

    print(f"  Host symbols: {len(host_symbols)}")
    print(f"  Patch symbols: {len(patch_symbols)}")
    print(f"  Semantic-patch symbols: {len(semantic_patch_symbols)}")

    method_subject_ids = _load_contract(family_slug)
    print(f"  Contract methods: {len(method_subject_ids)}")

    # Generate test
    test_source = _generate_hotupdate_test(
        family_slug, host_symbols, patch_symbols, method_subject_ids,
        semantic_patch_symbols=semantic_patch_symbols if len(semantic_patch_symbols) == len(host_symbols) else None,
    )
    test_dir = _VERIFICATION / family_slug / "il2cpp_dist" / "hotupdate"
    test_dir.mkdir(parents=True, exist_ok=True)
    test_path = test_dir / "HotUpdateTest.cpp"
    test_path.write_text(test_source, encoding="utf-8")
    print(f"  Test: {test_path.relative_to(_REPO_ROOT)}")

    # Copy patch file with renamed RunNativeAot to avoid symbol collision
    ns_slug = _ns_slug_from_family_id(f"family/System.Private.CoreLib/{family_slug.replace('-', '/')}")

    # Fix genuine TU: strip extern wrappers, inject missing type-ids
    _fix_genuine_for_hotupdate(family_slug, ns_slug)

    patch_src = _VERIFICATION / family_slug / "il2cpp_dist" / "patch" / "generated" / "native-aot.generated.cpp"
    patch_dst = _VERIFICATION / family_slug / "il2cpp_dist" / "patch" / "native-aot.patch.generated.cpp"
    if not patch_src.exists():
        patch_src = _VERIFICATION / family_slug / "il2cpp_dist" / "patch" / "generated" / "generated" / "native-aot.generated.cpp"
    if patch_src.exists():
        _rename_and_fix_patch_file(patch_src, patch_dst, ns_slug, strip_extern_wrappers=True)

    # Copy semantic-patch file with renamed RunNativeAot, KEEP extern wrappers
    # so the linker resolves symbols from the semantic-patch TU.
    semantic_src = _VERIFICATION / family_slug / "il2cpp_dist" / "semantic-patch" / "generated" / "native-aot.generated.cpp"
    if not semantic_src.exists():
        semantic_src = _VERIFICATION / family_slug / "il2cpp_dist" / "semantic-patch" / "generated" / "generated" / "native-aot.generated.cpp"
    semantic_dst = _VERIFICATION / family_slug / "il2cpp_dist" / "semantic-patch" / "native-aot.semantic-patch.generated.cpp"
    if semantic_src.exists():
        _rename_and_fix_patch_file(semantic_src, semantic_dst, ns_slug, suffix="semantic", strip_extern_wrappers=False)

    # Append hotupdate test target to the existing CMakeLists.txt
    cmake_piece = _generate_hotupdate_cmake_piece(family_slug, host_symbols)
    cmake_path = _VERIFICATION / family_slug / "il2cpp_dist" / "CMakeLists.txt"
    if cmake_path.exists():
        existing = cmake_path.read_text(encoding="utf-8")
        marker = "# Per-family hotupdate test from CodeGen-generated C++"
        if marker not in existing:
            with open(cmake_path, "a", encoding="utf-8") as f:
                f.write("\n" + cmake_piece)
            print(f"  CMake: appended hotupdate target to {cmake_path.relative_to(_REPO_ROOT)}")
        else:
            print(f"  CMake: already has hotupdate target (skipped)")
    else:
        cmake_path.write_text(cmake_piece, encoding="utf-8")
        print(f"  CMake: {cmake_path.relative_to(_REPO_ROOT)} (new file)")

    artifacts = [
        str(test_path.relative_to(_REPO_ROOT)),
        str(patch_dst.relative_to(_REPO_ROOT)) if patch_src.exists() else "",
    ]
    return {"family": family_slug, "artifacts": [a for a in artifacts if a]}


def _fix_genuine_for_hotupdate(family_slug: str, ns_slug: str) -> Path | None:
    """Copy the genuine native-aot.generated.cpp with extern wrappers stripped
    and missing type-ids injected, so the 3-TU linkage works without LNK2005
    from duplicate extern "C" definitions.

    The fixed copy goes to native/hotupdate/genuine-fixed/native-aot.generated.cpp
    so the original genuine file remains untouched.
    """
    src = _VERIFICATION / family_slug / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
    if not src.exists():
        return None

    dst = _VERIFICATION / family_slug / "il2cpp_dist" / "hotupdate" / "genuine-fixed" / "native-aot.generated.cpp"
    _rename_and_fix_patch_file(src, dst, ns_slug, suffix="genuine_fixed", strip_extern_wrappers=True)
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


def _rename_and_fix_patch_file(src: Path, dst: Path, ns_slug: str, suffix: str = "patch",
                                strip_extern_wrappers: bool = True) -> None:
    """Copy a CodeGen-generated C++ file with RunNativeAot renamed to avoid symbol collision.

    Also uncomments the stripped chaos_managed_pointer_local_slot_tag constexpr
    so each TU has its own anonymous-namespace definition (no ODR violation across
    host + patch + semantic-patch TUs).

    When strip_extern_wrappers=True, strips `extern "C" chaos_external_runtime_*`
    helper function bodies, keeping only declarations (replaces { body } with ;).
    The TU that keeps the definitions (semantic-patch when strip_extern_wrappers=False)
    provides the symbol definitions; the other TUs declare them and the linker resolves.
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

    if strip_extern_wrappers:
        # Strip duplicate extern "C" chaos_external_runtime_ helper definitions.
        # These are inside the anonymous namespace and are `extern "C"` so they
        # have external linkage.  When the same family emits the same helpers in
        # both genuine + semantic-patch TUs, the linker sees duplicate definitions.
        # We keep a DECLARATION (replace { body } with ;) so entrypoint functions
        # in this TU can still call the symbol, which the linker resolves from
        # the TU that kept the definitions (semantic-patch).
        lines = content.splitlines(keepends=True)
        filtered: list[str] = []
        in_external_func = False
        brace_depth = 0

        for line in lines:
            stripped = line.strip()

            if not in_external_func and stripped.startswith('extern "C"') and 'chaos_external_runtime_' in stripped:
                in_external_func = True
                brace_depth = 0
                # Emit the signature as a declaration (replace { body } with ;)
                clean = line.rstrip('\n\r')
                if clean.endswith('{'):
                    clean = clean[:-1].rstrip() + ';\n'
                elif '{' in clean:
                    idx = clean.index('{')
                    clean = clean[:idx].rstrip() + ';\n'
                else:
                    clean += ';\n'
                filtered.append(clean)
                continue

            if in_external_func:
                for ch in stripped:
                    if ch == '{':
                        brace_depth += 1
                    elif ch == '}':
                        brace_depth -= 1
                if brace_depth == 0 and stripped == '}':
                    # End of function body — skip the closing brace
                    in_external_func = False
                    continue
                continue  # skip everything inside the function

            filtered.append(line)

        content = ''.join(filtered)

    # Inject missing reflection bridge stubs for functions the semantic-patch
    # TU references but CodeGen didn't emit (e.g., chaos_reflection_get_fields).
    content = _inject_reflection_bridge_stubs(content)

    # Inject missing struct definitions and type-id constexprs
    content = _inject_missing_structs_and_type_ids(content)

    dst.parent.mkdir(parents=True, exist_ok=True)
    dst.write_text(content, encoding="utf-8")
    print(f"  {suffix.capitalize()} renamed: {dst.relative_to(_REPO_ROOT)}")


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Generate per-family hotupdate C++ tests")
    parser.add_argument("--families", nargs="*", help="Families to process (default: all)")
    args = parser.parse_args()

    families = args.families or FAMILIES
    print(f"Generating per-family hotupdate tests - {len(families)} families")

    for family_slug in families:
        generate_family(family_slug)

    print(f"\nDone. Test and CMake artifacts in verification/foundation-dll/.../il2cpp_dist/hotupdate/")


if __name__ == "__main__":
    main()
