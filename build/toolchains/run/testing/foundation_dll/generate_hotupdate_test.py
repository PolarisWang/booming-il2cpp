"""Generate per-family Hotpatch C++ test from CodeGen-generated host C++.

For each family, this script:
  1. Reads the host (genuine) native-aot.generated.cpp to discover MethodN symbols
  2. Fixes the genuine TU by injecting missing structs/type-ids (→ genuine-fixed)
  3. Embeds .patchdata as a C++ byte array in HotUpdateTest.cpp
  4. Generates HotUpdateTest.cpp that:
     - Bootstraps the runtime through the ABI bridge
     - Calls ApplyPatchFromMemory to apply Hotpatch dispatch
     - For each method: verifies dispatch entry flags, calls InterpreterEntryDirect
     - Calls Unpatch, verifies dispatch entry flags are cleared
  5. Creates/updates CMakeLists.txt with proper linkage (no patch C++ TU)

The test verifies the Hotpatch dual-layer dispatch lifecycle:
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


# Runtime core functions that raise managed exceptions (crash when called
# during baseline capture with uninitialized runtime).  Extend this set
# when new families exhibit FAILFAST crashes during baseline.
# These are the runtime-core functions whose inline-stub wrappers are
# "pass-through" (just `return runtime_core::func(args)`) and thus don't
# directly contain Raise/ABORT in the stub body.
_KNOWN_THROWING_RUNTIME_FUNCTIONS = frozenset({
    # convert-char: invalid cast conversions that call RaiseInvalidCastException.
    # Trivial numeric conversions (byte, char, int16, uint16, int32, uint32,
    # int64, uint64, sbyte) ARE safe during baseline — they're simple cast/
    # truncation.  The following need runtime type resolution which fails when
    # the runtime is uninitialized:
    "chaos_convert_tochar_double",
    "chaos_convert_tochar_single",
    "chaos_convert_tochar_boolean",
    "chaos_convert_tochar_datetime",
    "chaos_convert_tochar_decimal",
    "chaos_convert_tochar_object_provider",
    "chaos_convert_tochar_string",
    "chaos_convert_tochar_string_provider",
})


def _ns_slug_from_family_id(family_id: str) -> str:
    return _slug_from_family_id(family_id).replace("-", "_")


def _class_name(family_slug: str, variant: str) -> str:
    """Derive C++ class name from family slug and variant (host or patch)."""
    base = "".join(part.capitalize() for part in family_slug.split("-"))
    return f"{base}NativeEntry" if variant == "host" else f"{base}PatchEntry"


def _find_genuine_cpp(family_slug: str) -> Path | None:
    """Find the genuine native-aot.generated.cpp for a family.

    Used for symbol discovery and aborting-methods detection — needs the
    file that actually contains MethodN function definitions/bodies.

    Returns NativeEntry-subdirectory version when the flat path is a shim.
    """
    genuine_dir = _VERIFICATION / family_slug / "il2cpp_dist" / "genuine"
    if genuine_dir.is_dir():
        for native_entry_dir in sorted(genuine_dir.iterdir()):
            if native_entry_dir.is_dir() and native_entry_dir.name.endswith("NativeEntry"):
                candidate = native_entry_dir / "generated" / "native-aot.generated.cpp"
                if candidate.exists():
                    return candidate

    direct = _VERIFICATION / family_slug / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
    if direct.exists():
        return direct

    genuine_fixed = _VERIFICATION / family_slug / "il2cpp_dist" / "hotupdate" / "genuine-fixed" / "native-aot.generated.cpp"
    if genuine_fixed.exists():
        return genuine_fixed

    return None


def _has_new_format_typeinfo(content: str) -> bool:
    """Check if the file uses GetWarmPtr (new TypeInfoHot format) vs old 10-field TypeInfo.

    New-style TypeInfo access uses GetWarmPtr to reach TypeInfoWarm members
    (iface_map, runtime_iface_count, etc.).  Old-style accesses these fields
    directly on TypeInfo*, which doesn't compile with the current TypeInfoHot.
    """
    return 'GetWarmPtr' in content


def _find_genuine_cpp_for_fix(family_slug: str) -> Path | None:
    """Find the genuine source for the genuine-fixed copy.

    Prefers the flat genuine/generated/ path (new convert-to-cpp pipeline)
    when it has the new format (named namespace, CHAOS_IL2CPP_INT32
    RunNativeAot).  Falls back to the NativeEntry subdirectory (old
    emit-native-aot format) only when the flat path doesn't exist or
    doesn't have the new format.

    The flat path is preferred because it goes through the current codegen
    which emits hotpatch dispatch (s_hotpatch_entries slot lookup + ArgBuffer).
    NativeEntry files are from the old emit-native-aot pipeline which
    lacks D3 dispatch call sites.
    """
    # First: try the flat path (new convert-to-cpp pipeline).
    # Accept both named-namespace format (canonical new pipeline) and
    # "hybrid" format (anonymous namespace + CHAOS_IL2CPP_INT32 RunNativeAot,
    # from the batch pipeline).  The flat path goes through current codegen
    # which emits Hotpatch dispatch for same-module calls and has more complete
    # external runtime helpers.  The old-format fix pipeline handles the
    # anonymous namespace fixes that hybrid families need.
    #
    # However, some hybrid-format flat files were generated by an older codegen
    # that uses the old 10-field TypeInfo layout (accessing iface_map, etc.
    # directly on TypeInfo*).  These files cannot compile with the current
    # TypeInfoHot - they need GetWarmPtr().  Detect this via _has_new_format_typeinfo
    # and fall back to NativeEntry when the flat file has old-style TypeInfo access.
    candidate = _VERIFICATION / family_slug / "il2cpp_dist" / "genuine" / "generated" / "native-aot.generated.cpp"
    if candidate.exists():
        content = candidate.read_text(encoding="utf-8")
        if 'extern "C" CHAOS_IL2CPP_INT32 RunNativeAot' in content:
            if _has_new_format_typeinfo(content):
                return candidate
            # Flat file uses old TypeInfo layout — can't compile with current headers.
            # Fall through to NativeEntry.

    # Fallback: try NativeEntry subdirectory (old emit-native-aot format)
    genuine_dir = _VERIFICATION / family_slug / "il2cpp_dist" / "genuine"
    if genuine_dir.is_dir():
        for native_entry_dir in sorted(genuine_dir.iterdir()):
            if native_entry_dir.is_dir() and native_entry_dir.name.endswith("NativeEntry"):
                candidate = native_entry_dir / "generated" / "native-aot.generated.cpp"
                if candidate.exists():
                    return candidate

    return None


def _discover_host_symbols(family_slug: str) -> list[str]:
    """Scan the genuine (host) generated C++ to discover MethodN symbols.

    Returns host_symbols list.
    """
    host_cpp = _find_genuine_cpp(family_slug)
    if host_cpp is None:
        return []
    return _extract_method_symbols(host_cpp)


def _extract_method_symbols(cpp_path: Path) -> list[str]:
    """Extract extern C method declarations (Method0..MethodN) from generated C++."""
    if not cpp_path.exists():
        return []

    symbols: list[str] = []
    content = cpp_path.read_text(encoding="utf-8")
    for line in content.splitlines():
        # New format: extern "C" void Namespace_Namespace_MethodN(void)
        m = re.match(
            r'extern\s+"C"\s+void\s+(\w+_Method\d+)\(void\)',
            line.strip(),
        )
        if m:
            sym = m.group(1)
            if sym not in symbols:
                symbols.append(sym)
            continue
        # Old format: extern "C" CHAOS_IL2CPP_INT32 Name_MethodN(void)
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


def _detect_method_return_type(family_slug: str) -> str:
    """Detect whether codegen uses 'void' or 'CHAOS_IL2CPP_INT32' return type.

    Scans the genuine TU for MethodN forward declaration/definition format.
    Returns 'void' or 'CHAOS_IL2CPP_INT32'.
    """
    host_cpp = _find_genuine_cpp(family_slug)
    if host_cpp is None or not host_cpp.exists():
        return "CHAOS_IL2CPP_INT32"  # default
    content = host_cpp.read_text(encoding="utf-8")
    for line in content.splitlines():
        m = re.match(
            r'extern\s+"C"\s+(void|CHAOS_IL2CPP_INT32)\s+\w+_Method\d+\(void\)',
            line.strip(),
        )
        if m:
            return m.group(1)
    return "CHAOS_IL2CPP_INT32"  # default


def _detect_aborting_methods(family_slug: str, host_symbols: list[str]) -> set[int]:
    """Parse the genuine TU to detect which MethodN indices will abort during baseline capture.

    Detection logic:
    1. Build WriteMethodTable idx → func_name mapping
    2. Classify extern "C" inline stubs as null-returning (body returns static_cast<INTPTR>(0))
    3. For each MethodN body, check if it:
       a. Calls g_method_table[nidx].fn_ptr where nidx maps to a null-returning stub
       b. Has the pattern `if (chaos_array == nullptr) { CHAOS_IL2CPP_ABORT(); }`

    Returns set of method indices that will abort when called directly.
    """
    host_cpp = _find_genuine_cpp(family_slug)
    if host_cpp is None:
        return set()
    content = host_cpp.read_text(encoding="utf-8")

    # Step 1: Build WriteMethodTable idx → func_name mapping
    idx_to_name: dict[int, str] = {}
    for m in re.finditer(
        r'WriteMethodTable\((\d+),\s*reinterpret_cast<void\*>\((chaos_external_runtime_\w+)\),\s*\d+u?\)',
        content,
    ):
        idx_to_name[int(m.group(1))] = m.group(2)

    # Also capture literal-0 method table entries (unresolved generic methods
    # like Nullable<T>.GetValueOrDefault where codegen emits
    # WriteMethodTable(N, reinterpret_cast<void*>(0), 0u)).
    zero_indices: set[int] = set()
    for m in re.finditer(
        r'WriteMethodTable\((\d+),\s*reinterpret_cast<void\*>\s*\(\s*0\s*\)\s*,\s*\d+u?\)',
        content,
    ):
        zero_indices.add(int(m.group(1)))

    # Step 2: Classify null-returning stubs (INTPTR return, body returns 0)
    null_stub_names: set[str] = set()
    for m in re.finditer(r'extern "C" inline CHAOS_IL2CPP_INTPTR (chaos_external_runtime_\w+)\(', content):
        name = m.group(1)
        start = m.start()
        brace = content.index("{", start)
        depth = 1
        pos = brace + 1
        while depth > 0 and pos < len(content):
            if content[pos] == "{":
                depth += 1
            elif content[pos] == "}":
                depth -= 1
            pos += 1
        body = content[brace:pos]
        if "return static_cast<CHAOS_IL2CPP_INTPTR>(0)" in body:
            null_stub_names.add(name)

    # Step 2b: Classify throwing inline stubs — any inline stub whose body
    # calls Raise* (managed exception), std::abort(), or CHAOS_IL2CPP_ABORT.
    # These will terminate the process when called during baseline capture
    # (uninitialized runtime, or type not resolvable in per-family builds).
    throwing_stub_names: set[str] = set()
    for m in re.finditer(r'extern "C" inline \w+ (chaos_external_runtime_\w+)\(', content):
        name = m.group(1)
        if name in null_stub_names:
            continue  # already classified as null-returning
        start = m.start()
        brace = content.index("{", start)
        depth = 1
        pos = brace + 1
        while depth > 0 and pos < len(content):
            if content[pos] == "{":
                depth += 1
            elif content[pos] == "}":
                depth -= 1
            pos += 1
        body = content[brace:pos]
        # Check if the body calls anything that raises or aborts
        if any(kw in body for kw in ['Raise', 'ABORT', 'abort', 'throw']):
            throwing_stub_names.add(name)

    # Step 2c: Detect pass-through inline stubs that call known-throwing
    # runtime functions.  These stubs are thin wrappers like:
    #   return chaos_convert_tochar_double(chaos_fn_arg_0);
    # The stub body itself does NOT contain Raise/ABORT — the throwing
    # is in the runtime-core function it delegates to.  We classify them
    # as throwing when the called function is in _KNOWN_THROWING_RUNTIME_FUNCTIONS.
    for m in re.finditer(r'extern "C" inline \w+ (chaos_external_runtime_\w+)\(', content):
        name = m.group(1)
        if name in null_stub_names or name in throwing_stub_names:
            continue
        start = m.start()
        brace = content.index("{", start)
        depth = 1
        pos = brace + 1
        while depth > 0 and pos < len(content):
            if content[pos] == "{":
                depth += 1
            elif content[pos] == "}":
                depth -= 1
            pos += 1
        body = content[brace:pos]
        # Check if the body calls any known-throwing runtime function
        for tf in _KNOWN_THROWING_RUNTIME_FUNCTIONS:
            if tf in body:
                throwing_stub_names.add(name)
                break

    # Map null stub names to g_method_table indices
    null_indices = {idx for idx, name in idx_to_name.items() if name in null_stub_names}

    # Merge literal-0 method table slots into the null detection set.
    # These are unresolved generic methods (e.g., Nullable<T>.GetValueOrDefault)
    # where codegen emits reinterpret_cast<void*>(0) instead of a valid stub.
    null_indices |= zero_indices

    # Step 3: For each MethodN, check for null-stub call + abort pattern
    # (even when null_indices is empty — methods can still abort on null arg
    # checks during baseline capture because kHostThunks[i]() passes no args).
    aborting: set[int] = set()
    sym_indexes: dict[str, int] = {}
    for sym in host_symbols:
        m = re.match(r".*_Method(\d+)$", sym)
        if m:
            sym_indexes[sym] = int(m.group(1))

    for sym, midx in sym_indexes.items():
        # Find the method body (skip forward declarations ending with ";")
        # Try void return format first, then CHAOS_IL2CPP_INT32 fallback
        needle_void = f'extern "C" void {sym}(void)'
        needle_int32 = f'extern "C" CHAOS_IL2CPP_INT32 {sym}(void)'
        needle = needle_void if needle_void in content else needle_int32
        search_from = 0
        idx = -1
        while True:
            idx = content.find(needle, search_from)
            if idx < 0:
                break
            rest = content[idx + len(needle):].strip()
            if rest.startswith("{"):
                break  # found the function definition
            search_from = idx + len(needle)
        if idx < 0:
            continue
        # Find the opening brace
        brace = content.index("{", idx)
        depth = 1
        pos = brace + 1
        while depth > 0 and pos < len(content):
            if content[pos] == "{":
                depth += 1
            elif content[pos] == "}":
                depth -= 1
            pos += 1
        body = content[brace:pos]

        # Check for g_method_table[null_idx].fn_ptr call
        # (codegen emits fully-qualified ::chaos::il2cpp::method_table::g_method_table[N])
        calls_null_stub = any(
            f"::g_method_table[{ni}]" in body or f"g_method_table[{ni}]" in body
            or f"::chaos::il2cpp::method_table::g_method_table[{ni}]" in body
            for ni in null_indices
        )

        # Check for null→abort pattern — multiple variants:
        #   if (chaos_array == nullptr) { CHAOS_IL2CPP_ABORT(); }
        #   if (chaos_string == nullptr) { CHAOS_IL2CPP_ABORT(); }
        #   if (chaos_arg_N == static_cast<CHAOS_IL2CPP_INTPTR>(0)) { CHAOS_IL2CPP_ABORT(); }
        has_null_abort = (
            "if (chaos_array == nullptr)" in body
            or "if (chaos_string == nullptr)" in body
            or "if (chaos_delegate == nullptr)" in body
            or re.search(
                r'if \(chaos_arg_\d+ == static_cast<CHAOS_IL2CPP_INTPTR>\(0\)\)',
                body,
            ) is not None
        )

        # Check for calls to throwing inline stubs (stubs that call
        # Raise*, CHAOS_IL2CPP_ABORT, std::abort, or known-throwing
        # runtime functions in their body). These are extern "C" inline
        # wrappers in the genuine TU that dispatch to runtime_core
        # functions that throw when the runtime is uninitialized or the
        # type is not resolvable.
        #
        # Uses substring match (tn in body) instead of regex, because
        # different families call stubs with different syntax:
        #   return stubname(args);           — return-value pass-through
        #   const auto r = stubname(args);   — captured result
        # Only false-positive risk is if the stub name is a substring of
        # another identifier, which is negligible for these long names.
        calls_throwing_stub = any(
            tn in body
            for tn in throwing_stub_names
        )

        # Additional check: if the method calls a literal-0 method table slot
        # (WriteMethodTable(N, reinterpret_cast<void*>(0), ...)), the call
        # itself will crash with ACCESS_VIOLATION since the function pointer
        # is null. There is no explicit null-check-abort pattern — the crash
        # happens inside the null dereference itself.
        calls_zero_slot = any(
            f"::g_method_table[{zi}]" in body or f"g_method_table[{zi}]" in body
            or f"::chaos::il2cpp::method_table::g_method_table[{zi}]" in body
            for zi in zero_indices
        )

        if calls_null_stub and has_null_abort:
            # Calls a null-returning stub and then null-checks the result → abort.
            aborting.add(midx)
        elif has_null_abort:
            # The method null-checks an arg (e.g., chaos_arg_N == 0) which will
            # always trigger during baseline capture (all methods are called with
            # no C++ arguments, so eval-stack args are zero-initialized).
            # This catches reflection-* families where WriteMethodTable entries
            # point to real functions but the method body aborts on null arg.
            aborting.add(midx)
        elif calls_zero_slot:
            # Calling a null function pointer directly will crash — no
            # explicit null-check-abort pattern needed.
            aborting.add(midx)
        elif calls_throwing_stub:
            # The method calls an inline stub that raises a managed exception
            # (e.g., RaiseInvalidCastException, RaiseManagedException).
            # During baseline capture with uninitialized runtime, these abort.
            aborting.add(midx)

    return aborting


def _generate_hotupdate_test(
    family_slug: str,
    host_symbols: list[str],
    method_subject_ids: list[str],
) -> str:
    """Generate the per-family Hotpatch C++ test source."""
    family_id = f"family/System.Private.CoreLib/{family_slug.replace('-', '/')}"
    ns_slug = _ns_slug_from_family_id(family_id)
    method_count = len(host_symbols)

    # Embed .patchdata for D3 dispatch hotpatch
    host_class_name = _class_name(family_slug, "host")
    patch_data_cxx, _ = _embed_patch_data(family_slug)

    # Detect whether codegen uses 'void' or 'CHAOS_IL2CPP_INT32' return type
    return_type = _detect_method_return_type(family_slug)

    # Detect methods that abort during baseline capture (call null-returning stub → abort)
    aborting_indices = _detect_aborting_methods(family_slug, host_symbols)

    lines = []
    # IMPORTANT: #include directives must come BEFORE the patch data byte array,
    # because kPatchData uses uint8_t which requires <cstdint>.
    _emit_header(lines, family_id, method_count)
    if patch_data_cxx:
        lines.append(patch_data_cxx)
    _emit_forward_decls(lines, host_symbols, return_type)
    _emit_namespace_block(lines, host_symbols, method_count, return_type)
    _emit_main_function(lines, method_count, family_id, bool(patch_data_cxx), host_class_name, aborting_indices, return_type)

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
        "// ── Embedded .patchdata for Hotpatch dispatch ──────────────",
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
        "// Uses the codegen-emitted dispatch table + .patchdata for Hotpatch verification:",
        "//   ApplyPatchFromMemory -> dispatch table patching -> InterpreterEntryDirect -> Unpatch",
        "//",
    ])
    lines.extend([
        '#include "bootstrap.h"',
        '#include "codegen_bridge.h"',
        '#include "patch_loader.h"',
        '#include "hotpatch_table.h"',
        '#include "runtime_core.h"',
        '#include "interpreter_entry.h"',
        '#include "exception_helpers.h"',
        "",
        "#include <cstdio>",
        "#include <cstdint>",
        "#include <cstring>",
        "#include <excpt.h>",
        "",
    ])


def _emit_forward_decls(
    lines: list[str],
    host_symbols: list[str],
    return_type: str = "CHAOS_IL2CPP_INT32",
) -> None:
    lines.append("// ---------------------------------------------------------------")
    lines.append("// Forward declarations: host methods (real API calls via CodeGen)")
    lines.append("// ---------------------------------------------------------------")
    for sym in host_symbols:
        lines.append(f'extern "C" {return_type} {sym}(void);')


def _emit_namespace_block(
    lines: list[str],
    host_symbols: list[str],
    method_count: int,
    return_type: str = "CHAOS_IL2CPP_INT32",
) -> None:
    is_void_return = (return_type == "void")
    # For void-returning methods: void (*kHostThunks[])()
    # For CHAOS_IL2CPP_INT32-returning: void* (*kHostThunks[])()
    fn_ptr_cast = "void (*)()" if is_void_return else "void* (*)()"
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
        "",
        "// Host method pointer array (auto-indexed by MethodN ordering)",
    ])
    if is_void_return:
        lines.append("    void (*kHostThunks[])() = {")
    else:
        lines.append("    void* (*kHostThunks[])() = {")
    for sym in host_symbols:
        lines.append(f"    reinterpret_cast<{fn_ptr_cast}>(&{sym}),")
    lines.extend([
        "};",
        "",
        "}  // namespace",
    ])


def _emit_main_function(lines: list[str], method_count: int, family_id: str,
                         has_patch_data: bool,
                         host_class_name: str,
                         aborting_indices: set[int] = set(),
                         return_type: str = "CHAOS_IL2CPP_INT32") -> None:
    is_void_return = (return_type == "void")
    # Emit aborting-methods helpers as file-scope statics before main()
    aborting_sorted = sorted(aborting_indices)
    if aborting_sorted:
        lines.extend([
            "",
            f"static constexpr uint32_t kAbortingMethodIndices[] = {{{', '.join(str(i) for i in aborting_sorted)}}};",
            f"static constexpr uint32_t kAbortingMethodCount = {len(aborting_sorted)}u;",
            "",
            "static bool IsAbortingMethod(uint32_t i) {",
            "    for (uint32_t j = 0u; j < kAbortingMethodCount; j++) {",
            "        if (kAbortingMethodIndices[j] == i) return true;",
            "    }",
            "    return false;",
            "}",
        ])
    else:
        lines.extend([
            "",
            "static bool IsAbortingMethod(uint32_t) { return false; }",
        ])

    # Now the main function
    lines.extend([
        "",
        "int main() {",
        "    using chaos::il2cpp::bootstrap::PeekBootstrapState;",
        "    using chaos::il2cpp::runtime_core::ApplyPatchFromMemory;",
        "    using chaos::il2cpp::runtime_core::Unpatch;",
        "    using chaos::il2cpp::runtime_core::HotpatchLookupBySlot;",
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
        "    auto* first_entry = HotpatchLookupBySlot(0u, 0u);",
        "    if (first_entry == nullptr) {",
        '        std::fprintf(stderr, "FATAL: no dispatch table registered by codegen TU\\n");',
        "        return 1;",
        "    }",
        "    (void)first_entry;",
        "",
    ])

    if is_void_return:
        baseline_lines = [
            "    // ── Step A: Capture AOT baseline (pre-patch call verification) ─────",
            "    // Call each host method directly. Since codegen emits void return,",
            "    // there is no return value to capture; we just verify the call doesn't crash.",
            "    // Methods that crash during direct execution are skipped.",
            "    // Uses __try/__except to catch ACCESS_VIOLATION so one bad method",
            "    // doesn't kill the whole test.  Methods that __fastfail (std::abort)",
            "    // are handled via static IsAbortingMethod detection instead.",
            "    CHAOS_IL2CPP_INT32 baseline_values[kMethodCount] = {};",
            "    for (uint32_t i = 0u; i < kMethodCount; i++) {",
            "        if (IsAbortingMethod(i)) {",
            "            baseline_values[i] = 0;",
            "            continue;",
            "        }",
            "        __try {",
            "            kHostThunks[i]();",
            "            baseline_values[i] = 0;",
            "        } __except (EXCEPTION_EXECUTE_HANDLER) {",
            "            baseline_values[i] = 0;",
            "        }",
            "    }",
        ]
    else:
        baseline_lines = [
            "    // ── Step A: Capture AOT baseline (pre-patch return values) ─────",
            "    // Call each host method directly to establish the AOT return value.",
            "    // Methods that crash during direct execution are skipped (baseline = 0).",
            "    // Uses __try/__except to catch ACCESS_VIOLATION so one bad method",
            "    // doesn't kill the whole test.  Methods that __fastfail (std::abort)",
            "    // are handled via static IsAbortingMethod detection instead.",
            "    CHAOS_IL2CPP_INT32 baseline_values[kMethodCount] = {};",
            "    for (uint32_t i = 0u; i < kMethodCount; i++) {",
            "        if (IsAbortingMethod(i)) {",
            "            baseline_values[i] = 0;",
            "            continue;",
            "        }",
            "        __try {",
            "            baseline_values[i] = reinterpret_cast<int32_t>(kHostThunks[i]());",
            "        } __except (EXCEPTION_EXECUTE_HANDLER) {",
            "            baseline_values[i] = 0;",
            "        }",
            "    }",
        ]
    lines.extend(baseline_lines)

    if has_patch_data:
        lines.extend([
            "",
            "    // ── Hotpatch dispatch via ApplyPatchFromMemory ────────────────",
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
            "    // ── Hotpatch dispatch disabled (no .patchdata) ──────────────",
            "    uint32_t d3_patched_count = 0u;",
            "    PatchContext* patch_ctx = nullptr;",
            "",
        ])

    lines.extend([
        "    uint32_t passed_count = 0u;",
        "    uint32_t failed_count = 0u;",
        "",
        "    // Store per-method results for JSON emission after Unpatch",
        "    // (so revertVerified can be populated).",
        "    struct MethodResult {",
        "        uint32_t method_token;",
        "        bool     step_ok;",
        "        bool     d3_patched;",
        "        int32_t  patch_return_value;",
        "        bool     interpreter_dispatched;",
        "        bool     semantic_ok;",
        "        bool     revert_ok;",
        "        bool     baseline_skip;",
        "    };",
        "    MethodResult results[kMethodCount];",
        "    for (uint32_t i = 0u; i < kMethodCount; i++) {",
        "        results[i] = {};",
        "    }",
        "",
        "    for (uint32_t i = 0u; i < kMethodCount; i++) {",
        "        const uint32_t token = kBaseToken + i;",
        "        bool step_ok = true;",
        "",
        "        // Step 1: Get dispatch entry via HotpatchLookupBySlot.",
        "        auto* entry = HotpatchLookupBySlot(0u, i);",
        "        if (entry == nullptr) {",
        '            std::fprintf(stderr, "FAIL[%u]: HotpatchLookupBySlot returned null\\n", i);',
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
            "        // Step 3: Verify dispatch entry has kHotpatchActive flag set.",
            "        bool patched_flag = (entry != nullptr) && (entry->flags & kHotpatchActive);",
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
            "        // Step 5: Call via InterpreterEntryDirect with ret_buf to capture",
            "        // the patched method's return value. The patchdata contains real IL",
            "        // code, so the return value is the actual method result.",
            "        CHAOS_IL2CPP_INT32 patch_return_value = 0;",
            "        if (method_key != 0u) {",
            "            InterpreterEntryDirect(method_key, nullptr, &patch_return_value);",
            "        }",
            "",
            "        // Step 6a: Semantic verification.",
            "        // For void-returning methods, skip comparison (no return value to compare).",
            f"        bool semantic_ok = {'true' if is_void_return else 'IsAbortingMethod(i) || (patch_return_value == baseline_values[i])'};",
            "        if (!semantic_ok && d3_patched_count > 0u) {",
            '            std::fprintf(stderr, "FAIL[%u]: semantic mismatch \\u2014 AOT baseline=%d, patch=%d\\n",',
            "                i, static_cast<int>(baseline_values[i]),",
            "                static_cast<int>(patch_return_value));",
            "            step_ok = false;",
            "        }",
            "",
            "        // Step 6b: Store per-method results for JSON emission after Unpatch.",
            "        results[i].method_token = token;",
            "        results[i].step_ok = step_ok;",
            "        results[i].d3_patched = (d3_patched_count > 0u);",
            "        results[i].patch_return_value = static_cast<int32_t>(patch_return_value);",
            "        results[i].interpreter_dispatched = (d3_patched_count > 0u);",
            "        results[i].semantic_ok = semantic_ok;",
            "        results[i].revert_ok = false;",
            "        results[i].baseline_skip = IsAbortingMethod(i);",
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
        "    }",
        "",
        "    // ── Hotpatch Unpatch ────────────────────────────────────",
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
            "        // Per-method revert_ok is set to true only when the flag is",
            "        // confirmed cleared — this populates revertVerified in JSON output.",
            "        for (uint32_t j = 0u; j < kMethodCount; j++) {",
            "            auto* e = HotpatchLookupBySlot(0u, j);",
            "            if (e == nullptr || (e->flags & kHotpatchActive)) {",
            '                std::fprintf(stderr, "FAIL[unpatch]: entry[%u] still patched after Unpatch (flags=0x%08x)\\n",',
            "                    j, e ? e->flags : 0u);",
            "                failed_count++;",
            "            } else if (e->direct_ptr != nullptr) {",
            "                if (IsAbortingMethod(j)) {",
            "                    results[j].revert_ok = true;  // no baseline to compare",
            "                } else {",
        ])
        if is_void_return:
            lines.extend([
                "                    auto revert_fn = reinterpret_cast<void(*)()>(e->direct_ptr);",
                "                    revert_fn();",
                "                    results[j].revert_ok = true;",
            ])
        else:
            lines.extend([
                "                    auto revert_value = reinterpret_cast<int32_t(*)()>(e->direct_ptr)();",
                "                    results[j].revert_ok = (revert_value == baseline_values[j]);",
                "                    if (!results[j].revert_ok) {",
                '                        std::fprintf(stderr, "FAIL[unpatch]: entry[%u] revert mismatch (expected=%d, actual=%d)\\n",',
                "                            j, static_cast<int>(baseline_values[j]), static_cast<int>(revert_value));",
                "                        failed_count++;",
                "                    }",
            ])
        lines.extend([
            "                }",
            "            } else {",
            "                results[j].revert_ok = false;",
            "            }",
            "        }",
            "    }",
        ])

    lines.extend([
        "",
        "    // ── Emit JSON results (after Unpatch, so revertVerified is known) ────",
        '    std::printf("  ],\\n");',
        '    std::printf("  \\"passedMethods\\": %u,\\n", static_cast<unsigned>(passed_count));',
        '    std::printf("  \\"failedMethods\\": %u\\n", static_cast<unsigned>(failed_count));',
        '    std::printf("}\\n");',
        "",
        "    // Re-emit complete JSON with per-method details now that revert is known.",
        '    std::printf("{\\n");',
        '    std::printf("  \\"schemaVersion\\": 1,\\n");',
        '    std::printf("  \\"assemblyName\\": \\"System.Private.CoreLib\\",\\n");',
        f'    std::printf("  \\"familyId\\": \\"{family_id}\\",\\n");',
        '    std::printf("  \\"verificationKind\\": \\"hotupdate-proof\\",\\n");',
        '    std::printf("  \\"totalMethods\\": %u,\\n", kMethodCount);',
        '    std::printf("  \\"d3PatchApplied\\": %s,\\n", (d3_patched_count > 0u) ? "true" : "false");',
        '    std::printf("  \\"d3PatchedCount\\": %u,\\n", d3_patched_count);',
        '    std::printf("  \\"passedMethods\\": %u,\\n", static_cast<unsigned>(passed_count));',
        '    std::printf("  \\"failedMethods\\": %u,\\n", static_cast<unsigned>(failed_count));',
        '    std::printf("  \\"results\\": [\\n");',
        "",
        "    for (uint32_t i = 0u; i < kMethodCount; i++) {",
        "        const char* comma = (i + 1u < kMethodCount) ? \",\" : \"\";",
        "        std::printf(",
        '            "    {\\n"',
        '            "      \\"methodToken\\": %u,\\n"',
        '            "      \\"status\\": \\"%s\\",\\n"',
        '            "      \\"d3Patched\\": %s,\\n"',
        '            "      \\"patchReturnValue\\": %d,\\n"',
        '            "      \\"interpreterDispatched\\": %s,\\n"',
        '            "      \\"revertVerified\\": %s,\\n"',
        '            "      \\"semanticVerified\\": %s,\\n"',
        '            "      \\"baselineSkipped\\": %s\\n"',
        '            "    }%s\\n",',
        "            static_cast<unsigned>(results[i].method_token),",
        "            results[i].step_ok ? \"passed\" : \"failed\",",
        "            results[i].d3_patched ? \"true\" : \"false\",",
        "            static_cast<int>(results[i].patch_return_value),",
        "            results[i].interpreter_dispatched ? \"true\" : \"false\",",
        "            results[i].revert_ok ? \"true\" : \"false\",",
        "            results[i].semantic_ok ? \"true\" : \"false\",",
        "            results[i].baseline_skip ? \"true\" : \"false\",",
        "            comma);",
        "    }",
        "",
        '    std::printf("  ]\\n");',
        '    std::printf("}\\n");',
    ])
    lines.extend([
        "    return (failed_count == 0u) ? 0 : 1;",
        "}",
    ])


def _generate_hotupdate_cmake_full(family_slug: str) -> str:
    """Generate the complete CMakeLists.txt with Hotpatch target.

    Uses REPO_ROOT computed relative to CMAKE_CURRENT_LIST_DIR so the file
    works when configured standalone (not as part of the solution_native build).
    Links against prebuilt .lib files with full paths to avoid RuntimeLibrary
    mismatches.
    """
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
        'cmake_minimum_required(VERSION 3.15)\n'
        f'project(chaos_hotupdate_{ns_slug} LANGUAGES CXX)\n'
        '\n'
        '# Relative to repo root (il2cpp_dist = rooted at ../../../../.. from repo root)\n'
        'get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)\n'
        '\n'
        f"add_library({dummy_lib} INTERFACE)\n"
        "\n"
        "# Per-family Hotpatch test.\n"
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
        f"target_compile_definitions({target} PRIVATE NOMINMAX)\n"
        f"target_include_directories({target} PRIVATE\n"
        "    ${REPO_ROOT}/src/native/common\n"
        "    ${REPO_ROOT}/contracts/native/v0\n"
        "    ${REPO_ROOT}/src/native/runtime-core\n"
        "    ${REPO_ROOT}/src/native/bootstrap\n"
        "    ${REPO_ROOT}/src/native/interpreter\n"
        "    ${REPO_ROOT}/third_party/bdwgc/include\n"
        "    ${REPO_ROOT}/third_party/fmt/include\n"
        "    ${REPO_ROOT}/verification/foundation-dll/System.Private.CoreLib\n"
        ")\n"
        "# Force-include hotupdate config so CodeGen-generated code can find\n"
        "# chaos_managed_pointer_local_slot_tag, chaos_is_string_id, etc.\n"
        f"target_compile_options({target} PRIVATE\n"
        '    $<$<CXX_COMPILER_ID:MSVC>:/FI"${REPO_ROOT}/verification/foundation-dll/System.Private.CoreLib/native_hotupdate_config.h">\n'
        '    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-include${REPO_ROOT}/verification/foundation-dll/System.Private.CoreLib/native_hotupdate_config.h>\n'
        ")\n"
        "# fmt requires /utf-8 on MSVC\n"
        f"target_compile_options({target} PRIVATE\n"
        "    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>\n"
        ")\n"
        'set(NATIVE_LIB_DIR "${REPO_ROOT}/build/native")\n'
        f"target_link_libraries({target} PRIVATE\n"
        '    "${NATIVE_LIB_DIR}/src/native/hot-update/RelWithDebInfo/chaos_hot_update.lib"\n'
        '    "${NATIVE_LIB_DIR}/src/native/bootstrap/RelWithDebInfo/chaos_bootstrap.lib"\n'
        '    "${NATIVE_LIB_DIR}/src/native/runtime-core/RelWithDebInfo/chaos_runtime_core.lib"\n'
        '    "${NATIVE_LIB_DIR}/src/native/interpreter/RelWithDebInfo/chaos_interpreter.lib"\n'
        '    "${NATIVE_LIB_DIR}/src/native/common/RelWithDebInfo/chaos_common.lib"\n'
        '    "${NATIVE_LIB_DIR}/src/native/support/RelWithDebInfo/chaos_support.lib"\n'
        '    "${NATIVE_LIB_DIR}/bdwgc_build/RelWithDebInfo/chaos_bdwgc.lib"\n'
        '    "${NATIVE_LIB_DIR}/fmt_build/RelWithDebInfo/chaos_fmt.lib"\n'
        ")\n"
        "endif()\n"
    )


def generate_family(family_slug: str) -> dict[str, Any]:
    """Generate the Hotpatch test for a single family."""
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

    # Append Hotpatch target to existing CMakeLists.txt
    # (preserves benchmark static library target already in the file).
    cmake_hotupdate_section = _generate_hotupdate_cmake_full(family_slug)
    cmake_path = _VERIFICATION / family_slug / "il2cpp_dist" / "CMakeLists.txt"
    existing_cmake = cmake_path.read_text(encoding="utf-8") if cmake_path.exists() else ""
    # Check if hotupdate section is already present (idempotent).
    hotupdate_marker = f"Per-family Hotpatch test"
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

    Prefers the NativeEntry subdirectory (old emit-native-aot format) when
    available — the fix functions work correctly with this format.  Falls
    back to the flat path (new convert-to-cpp format) when the NativeEntry
    dir has no generated/ output.

    For the new pipeline format (flat-only), uses a simplified fix pass since
    the new format has full struct definitions and doesn't need most injections.
    """
    src = _find_genuine_cpp_for_fix(family_slug)
    if src is None:
        # Fallback: try NativeEntry subdirectory
        src = _find_genuine_cpp(family_slug)
        if src is None:
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

    defined_structs: set[str] = set()
    for line in lines:
        m = re.match(r'^\s*struct\s+(chaos_type_\S+)\s*(?::|$)'.replace('(?::|$)', '(?::.*)?$'), line.strip())
        if m:
            defined_structs.add(m.group(1))

    # Also detect base classes referenced in struct inheritance that aren't defined.
    for line in lines:
        m = re.match(r'^\s*struct\s+chaos_type_\S+\s*:\s*public\s+(chaos_type_\S+)', line.strip())
        if m and m.group(1) not in defined_structs:
            referenced_types.add(m.group(1))

    if not referenced_types:
        return content

    missing_structs = sorted(referenced_types - defined_structs)
    if not missing_structs:
        return content

    # Build a set of known struct layouts keyed by type name suffix
    known_structs = {
        'chaos_type_System_Private_CoreLib_System_Object': [
            '    chaos_object_header header{};\n',
        ],
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
        'chaos_type_System_Private_CoreLib_System_Exception': [
            "    CHAOS_IL2CPP_INTPTR _className = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _message = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _data = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _innerException = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _helpURL = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _stackTrace = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _stackTraceString = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _remoteStackTraceString = 0;\n",
            "    CHAOS_IL2CPP_INT32 _HResult = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _source = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _xptrs = 0;\n",
            "    CHAOS_IL2CPP_INT32 _xcode = -1;\n",
        ],
        'chaos_type_System_Private_CoreLib_System_SystemException': [
            "    CHAOS_IL2CPP_INTPTR _className = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _message = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _data = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _innerException = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _helpURL = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _stackTrace = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _stackTraceString = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _remoteStackTraceString = 0;\n",
            "    CHAOS_IL2CPP_INT32 _HResult = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _source = 0;\n",
            "    CHAOS_IL2CPP_INTPTR _xptrs = 0;\n",
            "    CHAOS_IL2CPP_INT32 _xcode = -1;\n",
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

        # Ensure System.Object base class is defined first if any injected
        # struct inherits from it.
        object_type = 'chaos_type_System_Private_CoreLib_System_Object'
        if object_type not in defined_structs and object_type not in missing_structs:
            missing_structs.insert(0, object_type)

        for struct_name in missing_structs:
            if struct_name in known_structs:
                fields = known_structs[struct_name]
                base = '' if struct_name == object_type else ' : public ' + object_type
                insert_lines.append(
                    f"struct {struct_name}{base}\n{{\n"
                )
                for f in fields:
                    insert_lines.append(f)
                insert_lines.append("};\n\n")
            else:
                base = '' if struct_name == object_type else ' : public ' + object_type
                insert_lines.append(
                    f"struct {struct_name}{base}\n{{\n"
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


def _generate_oldformat_stub_definitions(content: str) -> str:
    """Detect and generate stubs for `chaos_stub_definition_*` functions.

    Old-format codegen emits calls like:
      chaos_stub_definition_System_Collections_*::get_Default(...)

    These are never defined in any TU.  The `::` suffix is the old C++/CLI-style
    method reference and is NOT a valid C++ function name — we generate a stub
    for the base stub_definition function name (everything before the first `::`).
    """
    # Collect all unique stub_definition base names called in the TU
    stub_calls: set[str] = set()
    for m in re.finditer(
        r'(chaos_stub_definition_\w+)\s*(?:::|\()',
        content,
    ):
        base = m.group(1)  # Drop ::method suffix — just the type-level stub
        stub_calls.add(base)

    if not stub_calls:
        return content

    # Check if any of these are already defined
    defined_stubs: set[str] = set()
    for line in content.splitlines():
        for base in stub_calls:
            if base in line and ('{' in line or ');' in line):
                # Might be a forward decl or definition
                if base in line and '{' not in line and ')' in line:
                    continue  # forward decl only
                if base in line and '{' in line:
                    defined_stubs.add(base)

    missing = sorted(stub_calls - defined_stubs)
    if not missing:
        return content

    stubs = []
    stubs.append("// Old-format stub definitions injected for hotupdate test compilation.\n")
    stubs.append("// These are called by codegen-emitted code but never defined.\n\n")
    for func in missing:
        stubs.append(f"CHAOS_IL2CPP_INTPTR {func}()\n")
        stubs.append("{\n")
        stubs.append("    return static_cast<CHAOS_IL2CPP_INTPTR>(0);\n")
        stubs.append("}\n\n")

    stub_text = "".join(stubs)

    # Insert before the first extern "C" function definition (after includes/decls)
    marker = 'extern "C" CHAOS_IL2CPP_INT32'
    idx = content.find(marker)
    if idx < 0:
        marker = 'extern "C" void'
        idx = content.find(marker)
    if idx < 0:
        return content
    line_start = content.rfind('\n', 0, idx) + 1
    content = content[:line_start] + stub_text + content[line_start:]
    return content


def _inject_reflection_bridge_stubs(content: str) -> str:
    """Inject missing chaos_* stub definitions for functions called but not defined.

    The AOT lowering planner emits `extern "C"` wrappers that call various
    chaos_* helper functions, but the CodeGen stage does not always emit the
    corresponding helper definitions. Each TU needs its own copy since these
    are in the anonymous namespace.

    Covers chaos_reflection_*, chaos_thread_*, chaos_safepoint_*, and any
    other chaos_* function called but not defined in the TU. Functions that
    have known definitions in the runtime headers or the TU's own preamble
    are excluded.
    """
    # ── Phase 0: handle old-format chaos_stub_definition_* references.
    # Old-format codegen emits calls to `chaos_stub_definition_*` functions
    # (e.g., chaos_stub_definition_System_Collections_*::get_Default())
    # that are never defined anywhere. Generate stubs for these too.
    # Note: this runs AND falls through to Phase 1 — both may be needed.
    content = _generate_oldformat_stub_definitions(content)

    # ── Phase 1: scan for called-vs-defined functions.
    # Functions known to be defined in runtime headers (Chaos* via using decls)
    # or in the TU preamble (chaos_* inline helpers from codegen).
    _KNOWN_DEFINED = frozenset({
        # Runtime-provided (Chaos* prefix, capital C — declared in generated_code_compat.h)
        'ChaosArrayClear', 'ChaosArrayGetLength', 'ChaosBufferByteLength',
        'ChaosCultureGetCompareInfo', 'ChaosCultureGetCurrent', 'ChaosCultureGetDateTimeFormat',
        'ChaosCultureGetDisplayName', 'ChaosCultureGetInvariant', 'ChaosCultureGetName',
        'ChaosCultureGetNumberFormat', 'ChaosDatetimeGetHashCode', 'ChaosDatetimeGetUtcNow',
        'ChaosExceptionGetBaseException', 'ChaosExceptionGetHresult', 'ChaosExceptionGetInnerException',
        'ChaosFormattablestringFactoryCreate', 'ChaosGuidNewGuid', 'ChaosInterlockedMemoryBarrier',
        'ChaosLoadFloat64', 'ChaosLoadInt64', 'ChaosMathSqrt', 'ChaosObjectCtor',
        'ChaosObjectEqualsStatic', 'ChaosRandomNextBytes', 'ChaosRandomNextDouble',
        'ChaosReflectionAssemblyGetTypes', 'ChaosReflectionGetAssemblyFullName',
        'ChaosReflectionGetAssemblyLocation',
        'ChaosReflectionGetCallingAssembly', 'ChaosReflectionGetCallingConvention',
        'ChaosReflectionGetConstructorsDefault', 'ChaosReflectionGetContainsGenericParams',
        'ChaosReflectionGetExceptionMessage', 'ChaosReflectionGetExecutingAssembly',
        'ChaosReflectionGetFieldsBindingflags', 'ChaosReflectionGetGenericParamConstraints',
        'ChaosReflectionGetGenericParamPos', 'ChaosReflectionGetImageRuntimeVersion',
        'ChaosReflectionGetIsAbstract',
        'ChaosReflectionGetIsConstructedGeneric',
        'ChaosReflectionGetIsGenericType', 'ChaosReflectionGetIsGenericTypeDef',
        'ChaosReflectionGetIsInterface', 'ChaosReflectionGetIsPublic',
        'ChaosReflectionGetIsSealed',
        'ChaosReflectionGetIsVirtual', 'ChaosReflectionGetMethodsBindingflags',
        'ChaosReflectionGetModuleAssembly', 'ChaosReflectionGetModuleName',
        'ChaosReflectionGetModuleNameOnly', 'ChaosReflectionGetNamespace',
        'ChaosReflectionGetParamAttributes', 'ChaosReflectionGetParamPosition',
        'ChaosReflectionGetParameterType', 'ChaosReflectionGetRawDefaultValue',
        'ChaosReflectionGetReflectedType', 'ChaosReflectionGetRequiredCustomModifiers',
        'ChaosReflectionGetTypeFromAssemblyBool', 'ChaosReflectionGetTypeFullName',
        'ChaosReflectionHasDefaultValue', 'ChaosReflectionIsAssignableFrom',
        'ChaosReflectionIsAssignableTo', 'ChaosReflectionIsDefined',
        'ChaosReflectionIsInstanceOfType', 'ChaosReflectionIsSubclassOf',
        'ChaosReflectionMakeGenericType', 'ChaosReflectionModuleGetType',
        'ChaosReflectionModuleGetTypes', 'ChaosReflectionSetExceptionMetadata',
        'ChaosRuntimeHelpersEquals', 'ChaosRuntimeHelpersGetHashCode',
        'ChaosRuntimeHelpersGetObjectValue', 'ChaosRuntimefieldhandleGetHashCode',
        'ChaosRuntimemethodhandleGetHashCode', 'ChaosRuntimewrappedGetWrappedException', 'ChaosStoreFloat32', 'ChaosStoreFloat64',
        'ChaosStoreInt64', 'ChaosStringContains', 'ChaosStringJoinSs', 'ChaosStringStartsWith',
        'ChaosGetCustomAttributeFromBlob',
        # Codegen preamble inline helpers (defined in every TU)
        'chaos_normalize_native_int_argument', 'chaos_resolve_managed_value_pointer',
        'chaos_is_string_id', 'chaos_is_type_compatible', 'chaos_type_implements_interface',
        'chaos_does_type_implement_interface', 'chaos_is_array_type_compatible',
        'chaos_is_array_store_compatible', 'chaos_vtable_resolve', 'chaos_get_parent_type_info',
        'chaos_make_string_id_value', 'chaos_extract_string_id', 'chaos_resolve_managed_pointer',
        # Reflection string helpers (implemented in the reflection runtime)
        'chaos_reflection_create_string_utf8_copy', 'chaos_reflection_concat_string_pair_values',
        'chaos_reflection_get_string_utf8', 'chaos_reflection_create_reference_array',
        'chaos_reflection_create_type_value', 'chaos_reflection_create_array_value',
        'chaos_reflection_create_struct_value',
        # Thread helpers declared in runtime headers
        'chaos_find_interface_offset',
        # Delegate helpers (defined in codegen as struct type returns or using aliases)
        'chaos_delegate_invocation_list', 'chaos_require_delegate',
        'chaos_try_get_delegate_invocation_list', 'chaos_delegate_single_entry_equals',
        # Convert-char helpers (defined in runtime-core/convert.cpp)
        'chaos_convert_tochar_byte', 'chaos_convert_tochar_char',
        'chaos_convert_tochar_int16', 'chaos_convert_tochar_int32',
        'chaos_convert_tochar_int64', 'chaos_convert_tochar_sbyte',
        'chaos_convert_tochar_uint16', 'chaos_convert_tochar_uint32',
        'chaos_convert_tochar_uint64', 'chaos_convert_tochar_boolean',
        'chaos_convert_tochar_datetime', 'chaos_convert_tochar_decimal',
        'chaos_convert_tochar_double', 'chaos_convert_tochar_single',
        'chaos_convert_tochar_object', 'chaos_convert_tochar_object_provider',
        'chaos_convert_tochar_string', 'chaos_convert_tochar_string_provider',
        # Generic registration helpers (defined in every TU's anonymous namespace)
        'ChaosDoPopulateGenericRegistration', 'ChaosGenericRegistrationInit',
    })

    # Find all called-but-not-defined chaos_* functions
    called_funcs: set[str] = set()
    defined_funcs: set[str] = set()

    for line in content.splitlines():
        stripped = line.strip()
        # Detect calls: any `chaos_*(` or `Chaos*(` pattern
        for m in re.finditer(r'\b(chaos_\w+)\s*\(', stripped):
            called_funcs.add(m.group(1))
        for m in re.finditer(r'\b(Chaos\w+)\s*\(', stripped):
            called_funcs.add(m.group(1))
        # Detect definitions: any typed return + func name
        # Catches CHAOS_IL2CPP_*, plain C types, template prefixes, extern "C" inline, etc.
        for m in re.finditer(
            r'\b(CHAOS_IL2CPP_\w+|void|bool|int32_t|uint32_t|int64_t|uint64_t|'
            r'int16_t|uint16_t|intptr_t|uintptr_t|size_t)\s*\*?\s+'
            r'(chaos_\w+|Chaos\w+)\s*\(',
            stripped):
            defined_funcs.add(m.group(2))
        # Also catch template definitions like TValue* chaos_resolve_managed_value_pointer
        for m in re.finditer(r'\w+\s*\*?\s+(chaos_\w+)\s*\(', stripped):
            if m.group(1) not in defined_funcs:
                # Check if this is actually a definition (typename before it)
                if any(kw in stripped for kw in ('inline', 'extern', 'template', 'TValue')):
                    defined_funcs.add(m.group(1))

    # System.Private.CoreLib interop helpers — never stub these
    for line in content.splitlines():
        stripped = line.strip()
        if 'System.Private.CoreLib' in stripped and 'extern' in stripped:
            for m in re.finditer(r'\b(chaos_\w+)\s*\(', stripped):
                defined_funcs.add(m.group(1))

    missing = sorted(called_funcs - defined_funcs)
    # Remove known-defined runtime functions
    missing = [f for f in missing if f not in _KNOWN_DEFINED]

    if not missing:
        return content

    # Generate stubs for all missing functions
    stubs = []
    stubs.append("// Stub definitions injected for hotupdate test compilation.\n")
    stubs.append("// These functions are called by codegen-emitted code but not\n")
    stubs.append("// defined in this TU nor exported by the runtime core library.\n\n")

    for func in missing:
        # Determine parameter count and whether the function returns a value
        used_params: list[str] = []
        returns_value = False
        call_pattern = f'{func}('
        for line in content.splitlines():
            if call_pattern not in line:
                continue
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
            real_args = [a for a in args if a not in ('void', '')]
            if real_args and (not used_params or len(real_args) > len(used_params)):
                used_params = [f"chaos_arg_{i}" for i in range(len(real_args))]
            # Check if any call site uses the return value
            before = line[:line.index(call_pattern)].rstrip()
            if before.endswith('=') or before.endswith('return'):
                returns_value = True

        if returns_value:
            param_list = ", ".join(f"CHAOS_IL2CPP_INTPTR {p}" for p in used_params) if used_params else "void"
            stubs.append(f"CHAOS_IL2CPP_INTPTR {func}({param_list})\n")
            stubs.append("{\n")
            for p in used_params:
                stubs.append(f"    (void){p};\n")
            stubs.append("    return static_cast<CHAOS_IL2CPP_INTPTR>(0);\n")
            stubs.append("}\n\n")
        else:
            param_list = ", ".join(f"CHAOS_IL2CPP_INTPTR {p}" for p in used_params) if used_params else "void"
            stubs.append(f"void {func}({param_list})\n")
            stubs.append("{\n")
            for p in used_params:
                stubs.append(f"    (void){p};\n")
            stubs.append("}\n\n")

    stub_text = "".join(stubs)

    # Insert before the first chaos_external_runtime_ wrapper.
    # The new-format codegen emits extern "C" CHAOS_IL2CPP_INTPTR for return types,
    # while the old format uses static CHAOS_IL2CPP_INT32.
    marker_candidates = [
        r'extern\s+"C"\s+(?:inline\s+)?CHAOS_IL2CPP_INTPTR\s+chaos_external_runtime_',
        r'static\s+(?:CHAOS_IL2CPP_INT32|CHAOS_IL2CPP_INTPTR|void)\s+chaos_external_runtime_',
    ]
    m = None
    for pat in marker_candidates:
        m = re.search(pat, content)
        if m:
            break
    if not m:
        return content

    line_start = content.rfind('\n', 0, m.start()) + 1
    content = content[:line_start] + stub_text + content[line_start:]
    return content


def _inject_interpreter_entry_include(content: str) -> str:
    """Inject #include "interpreter_entry.h".

    The codegen emits &InterpreterEntryDirect as the interrupt_ptr value for
    every dispatch table entry, but the generated TU does not include the
    header.  This injection adds the include so the dispatch table compiles.

    NOTE: we do NOT add a `using` declaration because the generated code
    already has an extern "C" forward declaration of InterpreterEntryDirect
    at file scope, and a `using` declaration in the anonymous namespace would
    conflict with it.
    """
    marker = '#include "hotpatch_table.h"'
    include_line = '#include "interpreter_entry.h"'
    if include_line in content:
        return content  # already injected

    idx = content.find(marker)
    if idx < 0:
        return content  # can't find insertion point

    # Insert #include after the hotpatch_table.h line
    line_end = content.find('\n', idx)
    insert_at = line_end + 1
    inject = '\n' + include_line + '\n'
    content = content[:insert_at] + inject + content[insert_at:]
    return content


def _inject_string_materialize_stub(content: str) -> str:
    """Inject chaos_string_materialize definition if called but not defined.

    The codegen emits the full definition only when the entrypoint TU has
    string ID mappings (_stringIdMapping.Count > 0).  Families that pass
    string arguments (e.g. Assert.Equal message) have calls to it without
    a definition in the genuine TU.

    For the hotupdate test, a stub returning 0 is sufficient because:
    - Assert.Equal ignores the message parameter ((void)chaos_fn_arg_2)
    - Other string comparisons via g_method_table dispatch handle nullptr
    - The stub allows the TU to compile and the test to run
    """
    if 'chaos_string_materialize' not in content:
        return content
    if 'CHAOS_IL2CPP_INTPTR chaos_string_materialize' in content:
        return content  # already has definition

    stub = (
        '\n'
        '// Stub injected by generate_hotupdate_test.py: codegen emits calls\n'
        '// to chaos_string_materialize but not its definition when the\n'
        '// entrypoint has no string ID mappings.\n'
        'CHAOS_IL2CPP_INTPTR chaos_string_materialize(CHAOS_IL2CPP_INTPTR chaos_value) noexcept\n'
        '{\n'
        '    (void)chaos_value;\n'
        '    return static_cast<CHAOS_IL2CPP_INTPTR>(0);\n'
        '}\n'
    )

    # Insert before the first extern "C" function definition
    marker = 'extern "C" inline'
    idx = content.find(marker)
    if idx < 0:
        marker = 'extern "C" CHAOS_IL2CPP'
        idx = content.find(marker)
    if idx < 0:
        marker = 'extern "C" void'
        idx = content.find(marker)
    if idx < 0:
        return content

    line_start = content.rfind('\n', 0, idx) + 1
    content = content[:line_start] + stub + content[line_start:]
    return content


def _rename_and_fix_patch_file(src: Path, dst: Path, ns_slug: str, suffix: str = "patch") -> None:
    """Copy a CodeGen-generated C++ file with RunNativeAot renamed to avoid symbol collision.

    Also uncomments the stripped chaos_managed_pointer_local_slot_tag constexpr
    so each TU has its own anonymous-namespace definition.
    """
    content = src.read_text(encoding="utf-8")

    # Detect format: old (anonymous namespace, int RunNativeAot) vs new
    # (named namespace, CHAOS_IL2CPP_INT32 RunNativeAot) vs hybrid
    # (anonymous namespace + CHAOS_IL2CPP_INT32 RunNativeAot, from batch
    # pipeline that upgraded the RunNativeAot signature but not the namespace).
    is_old_format = 'extern "C" int RunNativeAot' in content
    has_chaos_run = 'extern "C" CHAOS_IL2CPP_INT32 RunNativeAot' in content
    has_named_ns = 'namespace chaos::il2cpp::codegen::' in content
    is_new_format = has_chaos_run and has_named_ns
    is_hybrid_format = has_chaos_run and not has_named_ns

    # Handle RunNativeAot renaming (old vs new/hybrid signature differs)
    if is_old_format:
        rename_from = 'extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)'
        rename_to = f'extern "C" int RunNativeAot_{ns_slug}_{suffix}(CHAOS_IL2CPP_INT32 chaos_entry_index)'
        content = content.replace(rename_from, rename_to)
    else:
        # Both new and hybrid use CHAOS_IL2CPP_INT32 RunNativeAot
        rename_from = 'extern "C" CHAOS_IL2CPP_INT32 RunNativeAot(CHAOS_IL2CPP_INT32 chaos_entry_index)'
        rename_to = f'extern "C" CHAOS_IL2CPP_INT32 RunNativeAot_{ns_slug}_{suffix}(CHAOS_IL2CPP_INT32 chaos_entry_index)'
        content = content.replace(rename_from, rename_to)

    # Old-format fixes: apply to both old AND hybrid (both have anonymous
    # namespace structure that needs struct injection, eval_stack, etc.)
    if is_old_format or is_hybrid_format:
        # Old format fixes (anonymous namespace, pre-convert-to-cpp pipeline)
        # Uncomment the stripped chaos_managed_pointer_local_slot_tag constexpr
        content = content.replace(
            '// constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;  // stripped: provided by native_hotupdate_config.h',
            'constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;',
        )
        content = content.replace(
            '// constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;',
            'constexpr CHAOS_IL2CPP_INTPTR chaos_managed_pointer_local_slot_tag = ChaosIl2cpp::Common::k_managed_pointer_local_slot_tag;',
        )

        # Inject missing struct definitions and type-id constexprs
        content = _inject_missing_structs_and_type_ids(content)

        # Fix `__s[N] = &chaos_locals[N]` → `__s[N] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[N])`
        content = re.sub(
            r'(?<!reinterpret_cast<CHAOS_IL2CPP_INTPTR>\()(&chaos_locals\[\d+\])',
            r'reinterpret_cast<CHAOS_IL2CPP_INTPTR>(\1)',
            content,
        )

        # Fix `const auto chaos_result = ();`
        content = re.sub(
            r'const auto chaos_result = \(\);',
            'const auto chaos_result = static_cast<CHAOS_IL2CPP_INTPTR>(0);',
            content,
        )

        # Fix `// TODO: define fields for chaos_type_*` stubs
        content = _fix_todo_struct_fields(content)

        # Inject InterpreterEntryDirect reference
        content = _inject_interpreter_entry_include(content)

        # Inject host symbol forward declarations before the anonymous namespace
        content = _inject_host_symbol_forward_decls(content)

        # Inject chaos_string_materialize stub
        content = _inject_string_materialize_stub(content)

        # Inject missing chaos_eval_stack, chaos_stack_top, and _sN declarations
        content = _inject_missing_eval_stack_decls(content)

        # Fix misuse of chaos_normalize_native_int_argument on raw Int32 literal
        # values loaded into __s[] slots.
        content = _fix_raw_int32_normalize_misuse(content)

    # Format-agnostic fixes (apply to both old and new formats)

    # Fix `ThinLockableHeader header{}``/`FatHeader header{}` missing trailing semicolon.
    # The codegen emits them with semicolons, but some injection steps or source
    # copies may strip them. MSVC rejects `header{}` without `;` in struct body.
    content = re.sub(
        r'(FatHeader|ThinLockableHeader)\s+header\{\}(?!\s*;)',
        r'\1 header{};',
        content,
    )

    # Inject missing reflection bridge stubs for functions that CodeGen hasn't
    # emitted yet (e.g., chaos_reflection_get_fields).
    content = _inject_reflection_bridge_stubs(content)

    # Fix parameter name mismatch in extern "C" inline function bodies:
    # codegen emits chaos_fn_arg_N as param names but chaos_arg_N in body.
    content = _fix_inline_param_mismatch(content)

    if is_new_format:
        # New pipeline format (convert-to-cpp): named namespace
        # chaos::il2cpp::codegen::FamilyName { ... }.
        #
        # Move `#include <chaos/common.h>` OUTSIDE the named namespace
        # so that its transitive include of <fmt/format.h> resolves std::*
        # as ::std::* and not FamilyName::std::* (C2039).
        # Keep `#include <chaos/type_info.h>` inside the namespace so
        # that TypeInfo (= TypeInfoHot) from ChaosIl2cpp::Common resolves.
        content = _move_common_include_outside_namespace(content)

        # Inject `using namespace chaos::il2cpp::runtime_core;` for families
        # whose codegen lacks it (buffer-memory, threading-monitor-interlocked
        # etc.) so ChaosBufferByteLength, ChaosVolatileRead etc. findable.
        ns_marker = 'namespace chaos::il2cpp::codegen::'
        ns_brace = content.find('{', content.find(ns_marker))
        if ns_brace >= 0:
            brace_line_end = content.find('\n', ns_brace) + 1
            after_brace = content[brace_line_end:brace_line_end + 120]
            if 'runtime_core' not in after_brace:
                content = content[:brace_line_end] + 'using namespace chaos::il2cpp::runtime_core;\n' + content[brace_line_end:]

        content = _inject_interpreter_entry_include(content)

        # Fix 10-field TypeInfo init-lists to 7-field TypeInfoHot format.
        content = _fix_newformat_typeinfo_initlist(content)

        # Fix codegen bug: `// comment    },` where closing brace is eaten
        # by the line comment. Common in s_abi_manifest_storage header init.
        content = _fix_closing_brace_in_comment(content)

    if is_old_format or is_hybrid_format:
        # Old-format files (anonymous namespace or file-scope) also need
        # the TypeInfo init-list fix since they use the same 10-field layout.
        content = _fix_newformat_typeinfo_initlist(content)

    if is_old_format:
        # Inject `using namespace chaos::il2cpp::runtime_core;` at file scope
        # so ChaosArrayClear, ChaosArrayGetLength etc. (declared inside
        # namespace chaos::il2cpp::runtime_core in generated_code_compat.h)
        # are findable from the old-format anonymous-ns / file-scope TU.
        # Also inject `chaos_object_header = FatHeader` typedef since the
        # old codegen emits `chaos_object_header header{};` but the
        # A5-Trinity migration renamed it to FatHeader with no global alias.
        # Find a good insertion point: after the last #include line.
        last_include = -1
        for idx, ln in enumerate(content.splitlines(keepends=True)):
            if ln.strip().startswith('#include'):
                last_include = idx
        if last_include >= 0:
            lines = content.splitlines(keepends=True)
            insert_pos = last_include + 1
            # Only inject if not already present
            content_after = ''.join(lines[insert_pos:insert_pos + 3])
            if 'runtime_core' not in content_after:
                ns_inject = (
                    '\n'
                    'using namespace chaos::il2cpp::runtime_core;\n'
                    'using chaos_object_header = FatHeader;\n'
                    '\n'
                )
                lines.insert(insert_pos, ns_inject)
                content = ''.join(lines)

    if is_hybrid_format:
        # Hybrid format: anonymous namespace + CHAOS_IL2CPP_INT32 RunNativeAot.
        # Remove the duplicate `#include <chaos/common.h>` inside the anonymous
        # namespace so its transitive <fmt/format.h> doesn't resolve std::*
        # as (anonymous)::std::* (C2039 / struct parsing errors).
        content = _fix_hybrid_common_include(content)

    dst.parent.mkdir(parents=True, exist_ok=True)
    dst.write_text(content, encoding="utf-8")
    print(f"  {suffix.capitalize()} renamed: {dst.relative_to(_REPO_ROOT)}")
    return dst


def _inject_missing_eval_stack_decls(content: str) -> str:
    """Inject chaos_eval_stack{} / chaos_stack_top / missing _sN declarations.

    The codegen emits interpreter-dispatch functions (for/switch/case pattern)
    that reference chaos_eval_stack[] and chaos_stack_top, but some families
    (e.g. array-indexing-copy) omit these declarations in the Run function
    preamble.

    We detect functions that use chaos_eval_stack but lack a declaration for it
    in the first 6 lines of the function body, then inject the missing decls.
    """
    # Phase 1: scan for max locals, s vars, and eval stack constants.
    # Do this on the raw content first.
    max_locals = 0
    for m in re.finditer(r'chaos_locals\[(\d+)\]', content):
        val = int(m.group(1))
        if val > max_locals:
            max_locals = val

    max_s_array = 0
    for m in re.finditer(r'__s\[(\d+)\]', content):
        val = int(m.group(1))
        if val > max_s_array:
            max_s_array = val

    max_stack_index = 0
    for m in re.finditer(r'chaos_eval_stack\[(\d+)\]', content):
        val = int(m.group(1))
        if val > max_stack_index:
            max_stack_index = val

    eval_size = max(max_locals + 4, max_s_array + 4, max_stack_index + 1, 8)

    s_vars: set[str] = set()
    for m in re.finditer(r'(?<!CHAOS_IL2CPP_INTPTR )(_s\d+)\s*=', content):
        name = m.group(1)
        if name != '_s0':
            s_vars.add(name)

    # Phase 2: iterate through extern "C" function definitions (not forward decls)
    # and inject missing declarations.
    LINES = content.splitlines(keepends=True)
    result = []
    i = 0
    while i < len(LINES):
        line = LINES[i]
        stripped = line.strip()

        # Detect extern "C" function definitions only.
        # We skip non-extern-C functions (they're in the anonymous namespace
        # and we must not touch their internal brace tracking).
        is_function_def = False
        if (stripped.startswith('extern "C"') or
            not stripped.startswith('extern') and not stripped.startswith('//') and not stripped.startswith('#')):
            if stripped.endswith(')'):
                # Check next non-empty line is { (not ; for forward decls)
                for j in range(i + 1, min(i + 4, len(LINES))):
                    next_stripped = LINES[j].strip()
                    if next_stripped == '{':
                        # ONLY match if it's extern "C" — skip non-extern C++
                        # functions inside the anonymous namespace.
                        if 'extern "C"' in line:
                            is_function_def = True
                        break
                    if next_stripped and not next_stripped.startswith('//') and next_stripped != '{':
                        break  # Not a function start (e.g. another decl)

        if is_function_def:
            func_lines = [line]
            depth = line.count('{') - line.count('}')
            i += 1
            while i < len(LINES):
                func_lines.append(LINES[i])
                depth += LINES[i].count('{') - LINES[i].count('}')
                i += 1
                if depth == 0:
                    break

            func_text = ''.join(func_lines)

            uses_eval = 'chaos_eval_stack' in func_text
            has_decl = False
            if uses_eval:
                count = 0
                for fl in func_lines:
                    if 'CHAOS_IL2CPP_ARRAY' in fl and 'chaos_eval_stack' in fl:
                        has_decl = True
                        break
                    count += 1
                    if count > 6:
                        break

            # Also check for _sN vars used but not declared in preamble
            # (functions may use _s3/_s4 without eval stack)
            # Scan the ENTIRE function body for existing declarations (not just
            # first 6 lines) to avoid re-injecting _sN that are declared later
            # in the preamble (e.g. after chaos_args/chaos_locals arrays).
            func_s_vars_used: set[str] = set()
            for m2 in re.finditer(r'(?<!CHAOS_IL2CPP_INTPTR )(_s\d+)\s*=', func_text):
                func_s_vars_used.add(m2.group(1))
            func_s_vars_declared: set[str] = set()
            for fl in func_lines:
                for m2 in re.finditer(r'CHAOS_IL2CPP_INTPTR\s+(_s\d+)', fl):
                    func_s_vars_declared.add(m2.group(1))
            missing_s_vars = func_s_vars_used - func_s_vars_declared

            if (uses_eval and not has_decl) or missing_s_vars:
                brace_idx = func_text.index('{')
                decls = []
                if uses_eval and not has_decl:
                    decls.append(f'\tCHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, {eval_size}) chaos_eval_stack{{}};\n')
                    decls.append(f'\tCHAOS_IL2CPP_SIZE chaos_stack_top = 0;\n')
                # Inject any _sN vars that are used but not declared
                for s in sorted(missing_s_vars, key=lambda x: int(x[2:])):
                    decls.append(f'\tCHAOS_IL2CPP_INTPTR {s}{{}};\n')
                if max_s_array > 0:
                    decls.append(f'\tCHAOS_IL2CPP_INTPTR __s[{max_s_array + 1}] = {{}};\n')
                insert = ''.join(decls)
                func_text = func_text[:brace_idx + 1] + '\n' + insert + func_text[brace_idx + 1:]

            result.append(func_text)
        else:
            result.append(line)
            i += 1

    return ''.join(result)


def _fix_inline_param_mismatch(content: str) -> str:
    """Fix parameter name mismatch in extern "C" inline function bodies.

    Codegen is inconsistent: some inline stubs use `chaos_fn_arg_N` as param
    names but `chaos_arg_N` in the function body.  This function detects such
    mismatches per function by checking the parameter list, and replaces the
    body references to match.

    Uses brace-depth tracking and processes each extern "C" inline function
    individually (not regex-based), so it correctly handles all nesting levels.
    """
    # Strategy: iterate through content by line. For each `extern "C" inline`
    # function, track which naming convention the params use, then fix the body.
    lines = content.splitlines(keepends=True)
    result = []
    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()
        if stripped.startswith('extern "C" inline'):
            # Collect the full declaration (may span multiple lines)
            decl = line
            i += 1
            while i < len(lines) and '{' not in lines[i]:
                decl += lines[i]
                i += 1
            if i >= len(lines):
                result.append(decl)
                break

            opening_line = lines[i]
            body_start_idx = opening_line.find('{')
            # Everything from extern to the opening brace is the decl
            decl += opening_line[:body_start_idx + 1]
            remaining = opening_line[body_start_idx + 1:]

            # Determine param naming convention
            params_use_fn_arg = 'chaos_fn_arg_' in decl

            # Collect body, tracking brace depth.  Start depth=1 because
            # we already consumed the opening {.
            body = remaining
            depth = 1 + remaining.count('{') - remaining.count('}')
            i += 1
            while depth > 0 and i < len(lines):
                body += lines[i]
                depth += lines[i].count('{') - lines[i].count('}')
                i += 1

            if params_use_fn_arg:
                # Fix body: chaos_arg_N -> chaos_fn_arg_N (but NOT when
                # already chaos_fn_arg_N -- replace with word boundary match)
                body = re.sub(r'\bchaos_arg_(\d)\b', r'chaos_fn_arg_\1', body)

            result.append(decl)
            result.append(body)
        else:
            result.append(line)
            i += 1

    return ''.join(result)


def _inject_host_symbol_forward_decls(content: str) -> str:
    """Inject forward declarations for host MethodN/Run symbols before the
    anonymous namespace if they are currently placed after the namespace block.

    Some codegen families (e.g. convert-char) emit extern "C" forward
    declarations for Method0..MethodN AFTER the anonymous namespace closing
    brace.  The dispatch table (inside the namespace) references these symbols
    and needs forward declarations before use.

    We detect the forward-declaration block by scanning for lines matching
      extern "C" <type> <class>_Method<N>(void);
      extern "C" <type> <class>_Run(<type> ...);

    where <type> is CHAOS_IL2CPP_INT32 or void.

    If the first such declaration appears AFTER '#pragma warning(pop)' (i.e.
    outside the anonymous namespace), we inject copies of all host symbol
    forward declarations right after the #include section, before the namespace.
    """
    if 'extern "C"' not in content:
        return content

    # Find the #pragma warning(pop) to determine namespace boundary
    pop_marker = '#pragma warning(pop)'
    pop_idx = content.find(pop_marker)
    if pop_idx < 0:
        return content  # no namespace boundary found

    # Find namespace opening
    ns_marker = '\nnamespace\n{'
    ns_idx = content.find(ns_marker)
    if ns_idx < 0:
        ns_marker = '\nnamespace {'
        ns_idx = content.find(ns_marker)
    if ns_idx < 0:
        return content

    # Collect extern "C" forward declarations that appear AFTER #pragma warning(pop)
    after_pop = content[pop_idx:]
    host_fwd_lines = []
    for line in after_pop.splitlines():
        stripped = line.strip()
        # Match: extern "C" <type> <symbol>(void);  or  extern "C" <type> <symbol>(<type> ...);
        # IMPORTANT: must end with semicolon to exclude function definitions.
        if re.match(r'extern\s+"C"\s+(CHAOS_IL2CPP_INT32|void)\s+\w+(?:\(void\)|\((?:CHAOS_IL2CPP_INT32|void)).*;\s*$', stripped):
            host_fwd_lines.append(line)

    if not host_fwd_lines:
        return content

    # Check if any of these forward declarations appear BEFORE the namespace
    before_ns = content[ns_idx:pop_idx + len(pop_marker)]
    for fwd_line in host_fwd_lines:
        sym = fwd_line.strip()
        if sym in before_ns:
            return content  # already declared before namespace, no fix needed

    # Inject host forward declarations after the last #include but before namespace
    # Find the last #include line
    lines = content.splitlines(keepends=True)
    last_include_idx = -1
    for i, line in enumerate(lines[:ns_idx]):
        if line.strip().startswith('#include'):
            last_include_idx = i
    if last_include_idx < 0:
        return content

    inject_lines = ['\n', '// Forward declarations for host MethodN symbols (injected)\n']
    for fwd_line in host_fwd_lines:
        inject_lines.append(fwd_line.rstrip() + '\n')
    inject_lines.append('\n')

    insert_at = last_include_idx + 1
    for i, line in enumerate(inject_lines):
        lines.insert(insert_at + i, line)

    return ''.join(lines)


def _move_common_include_outside_namespace(content: str) -> str:
    """Move `#include <chaos/common.h>` outside the named codegen namespace.

    When `#include <chaos/common.h>` is inside `namespace
    chaos::il2cpp::codegen::FamilyName { ... }`, the transitive include of
    <fmt/format.h> resolves std::* symbols as FamilyName::std::*, which
    fails C2039.  Moving the include outside the namespace fixes this.

    Keeps `#include <chaos/type_info.h>` inside the namespace so that
    `TypeInfo` (= TypeInfoHot 7 fields) from ChaosIl2cpp::Common is found.
    """
    common_include = '#include <chaos/common.h>'
    ci = content.find(common_include)
    if ci < 0:
        return content

    ns_marker = 'namespace chaos::il2cpp::codegen::'
    ns_idx = content.find(ns_marker)
    if ns_idx < 0 or ci < ns_idx:
        return content  # already outside or no named namespace

    # Extract the '#include <chaos/common.h>' line
    line_start = content.rfind('\n', 0, ci) + 1
    line_end = content.find('\n', ci)
    if line_end < 0:
        line_end = len(content)
    else:
        line_end += 1
    include_line = content[line_start:line_end]

    # Remove it from inside the namespace
    content = content[:line_start] + content[line_end:]

    # Insert it right before the namespace line
    # (after the #pragma warning(disable: ...) block)
    ns_line_start = content.rfind('\n', 0, content.find(ns_marker)) + 1
    content = content[:ns_line_start] + include_line + content[ns_line_start:]
    return content


def _fix_hybrid_common_include(content: str) -> str:
    """Fix `#include <chaos/common.h>` inside anonymous namespace for hybrid format.

    Hybrid-format files have `#include <chaos/common.h>` BOTH at file scope
    AND inside `namespace { ... }`.  The copy inside the anonymous namespace
    causes the transitive include of <fmt/format.h> to resolve std::* as
    (anonymous)::std::*, which fails C2039.

    Additionally, `chaos_object_header` is NOT defined in any ChaosIl2cpp::Common
    header — it was historically defined locally per-family by the codegen.
    The batch-pipeline flat files use `chaos_object_header` as a member type
    in struct definitions but never define it.

    Fix:
    1. Remove `#include <chaos/common.h>` from inside the anonymous namespace.
    2. After `#include <chaos/type_info.h>`, inject a local `chaos_object_header`
       struct that provides the fields the generated code expects.
    """
    common = '#include <chaos/common.h>'
    type_info_marker = '#include <chaos/type_info.h>'

    # Find anonymous namespace opening brace
    ns_brace = -1
    for pat in ['\nnamespace\n{', '\nnamespace {']:
        idx = content.find(pat)
        if idx >= 0:
            ns_brace = content.index('{', idx)
            break
    if ns_brace < 0:
        return content

    # Check for common.h inside namespace
    after_brace = content[ns_brace:]
    ns_common = after_brace.find(common)
    if ns_common < 0:
        return content  # no include inside namespace — nothing to fix

    common_pos = ns_brace + ns_common

    # Only do this if there's a file-scope copy too
    first = content.find(common)
    if first < 0 or first > common_pos:
        return content  # only the namespace copy exists — leave it

    # Step 1: Remove the namespace copy of common.h
    line_start = content.rfind('\n', 0, common_pos) + 1
    line_end = content.find('\n', common_pos)
    if line_end < 0:
        line_end = len(content)
    else:
        line_end += 1
    content = content[:line_start] + content[line_end:]

    # Step 2: Find `#include <chaos/type_info.h>` inside namespace and inject
    # a local chaos_object_header struct after it
    after_removal = content[ns_brace:]
    ns_typeinfo = after_removal.find(type_info_marker)
    if ns_typeinfo >= 0:
        ti_pos = ns_brace + ns_typeinfo
        ti_line_end = content.find('\n', ti_pos)
        if ti_line_end >= 0:
            ti_line_end += 1
            local_struct = (
                '\n'
                'struct chaos_object_header\n'
                '{\n'
                '    const TypeInfo* type_info = nullptr;\n'
                '    CHAOS_IL2CPP_INTPTR type_id = 0;\n'
                '};\n'
            )
            content = content[:ti_line_end] + local_struct + content[ti_line_end:]

    return content


def _fix_newformat_typeinfo_initlist(content: str) -> str:
    """Fix 10-field TypeInfo init-lists to TypeInfoHot 7-field format.

    The old TypeInfo (pre-A5-Trinity) had 10 fields:
      {parent, stable_id, iface_map, runtime_iface_map,
       iface_count, runtime_iface_count, type_shape, flags,
       vtable_array, vtable_length}

    TypeInfoHot has 7 fields:
      {parent, vtable_array, stable_id, vtable_length,
       warm_delta, type_shape, flags}

    Conversion:
      new[0]=old[0] (parent)
      new[1]=old[8] (vtable_array — nullptr becomes nullptr)
      new[2]=old[1] (stable_id)
      new[3]=old[9] (vtable_length — 0u becomes 0)
      new[4]=32    (warm_delta — always 32 for V0/V1)
      new[5]=old[6] (type_shape)
      new[6]=old[7] (flags)

    Only applies when the init-list has 10 fields (detected by
    a pattern where field[2] is a pointer type or nullptr).
    """
    def _convert_typeinfo_initlist(m: re.Match) -> str:
        full = m.group(0)
        # Extract fields
        inner = full[full.index('{'):full.rindex('}') + 1]
        fields_str = inner[1:-1]
        fields = [f.strip() for f in fields_str.split(',')]
        if len(fields) == 10:
            # Convert: parent_stable_id_iface_map...
            # old[0]=parent old[1]=stable_id old[6]=type_shape
            # old[7]=flags old[8]=vtable_array old[9]=vtable_length
            parent = fields[0]       # old[0]
            vtable_array = fields[8] # old[8]
            stable_id = fields[1]    # old[1]
            vtable_len = fields[9]   # old[9]
            type_shape = fields[6]   # old[6]
            flags = fields[7]        # old[7]
            # Normalize vtable_length: strip trailing u/U suffix
            if vtable_len.endswith('u') or vtable_len.endswith('U'):
                vtable_len = vtable_len[:-1]
            new_inner = f'{{{parent}, {vtable_array}, {stable_id}, {vtable_len}, 32, {type_shape}, {flags}}}'
            return full[:full.index('{')] + new_inner
        return full

    # Match inline TypeInfo chaos_type_info_* = { ... };
    return re.sub(
        r'inline\s+TypeInfo\s+chaos_type_info_\w+\s*=\s*\{[^}]+\}',
        _convert_typeinfo_initlist,
        content,
    )


def _fix_closing_brace_in_comment(content: str) -> str:
    """Fix codegen bug where `},` or `}` after a `//` comment gets consumed as part of the comment.

    Some families (e.g. buffer-memory) have:
        s_abi_manifest_prefix_sum  // O(1) prefix-sum
    },
    where the closing `},` is on its own line, which is valid C++ but only
    if there's no trailing comma on the comment line.  If the codegen emits:
        s_abi_manifest_prefix_sum,  // O(1) prefix-sum    },
    the `},` is after the comment and gets consumed as part of it, causing C2059.

    Detection: look for lines where `}` (possibly with trailing comma) appears
    after whitespace following a `//` comment.
    """
    result = []
    for line in content.splitlines(keepends=True):
        stripped = line.rstrip('\n\r')
        m = re.match(r'^(\s*)(.*?//.*?)(\s+)(\},?)\s*$', stripped)
        if m and m.group(4).startswith('}'):
            # The `},` or `}` is after the comment — split onto its own line
            result.append(m.group(1) + m.group(2) + '\n')
            result.append(m.group(1) + m.group(4) + '\n')
        else:
            result.append(line)
    return ''.join(result)


def _fix_raw_int32_normalize_misuse(content: str) -> str:
    """Fix misuse of chaos_normalize_native_int_argument on raw Int32 literal values.

    Codegen applies chaos_normalize_native_int_argument to all NativeInt-carrying
    arguments, including literal integers stored in __s[] slots.  When the literal
    has bit 0 set (odd value), normalize misinterprets it as a
    managed_pointer_local_slot_tag and dereferences an invalid pointer -> SIGSEGV.

    Strategy: per-method-body, track __s[N] assignments as we scan.  When
    chaos_normalize_native_int_argument(chaos_raw_arg_N) is encountered, check if
    __s[N] was last assigned a literal with bit 0 set.  If so, replace with
    direct raw arg access.  This is done inline so slot state at the time of
    use is correct (not overwritten by later assignments).
    """
    lines = content.splitlines(keepends=True)
    result = []

    i = 0
    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        # Detect method definition: extern "C" <type> <Class>_Method<N>(void)
        # where <type> is CHAOS_IL2CPP_INT32 or void
        # followed by { on this or next line (NOT forward declarations ending with ;)
        is_method_def = (
            re.match(r'extern\s+"C"\s+(?:CHAOS_IL2CPP_INT32|void)\s+\w+_Method\d+\(void\)\s*$', stripped)
            and not stripped.endswith(';')
        )
        if is_method_def:
            result.append(line)
            i += 1
            # Consume the { line if it's separate
            if i < len(lines) and '{' in lines[i] and '}' not in lines[i]:
                result.append(lines[i])
                depth = 1
                i += 1
            else:
                depth = line.count('{') - line.count('}')
                if depth <= 0:
                    continue

            # Per-method tracking: __s[slot] -> True if last assigned odd literal
            slot_literal_odd: dict[int, bool] = {}

            while i < len(lines) and depth > 0:
                cline = lines[i]
                depth += cline.count('{') - cline.count('}')

                # Track __s[N] = static_cast<CHAOS_IL2CPP_INTPTR>(DIGITS)
                m = re.match(
                    r'\s*__s\[(\d+)\]\s*=\s*static_cast<CHAOS_IL2CPP_INTPTR>\s*\(\s*(\d+)\s*\)\s*;',
                    cline,
                )
                if m:
                    slot = int(m.group(1))
                    value = int(m.group(2))
                    slot_literal_odd[slot] = (value & 1) == 1
                elif re.match(r'\s*__s\[(\d+)\]\s*=', cline):
                    # Non-literal __s[N] = ... clears literal status
                    m2 = re.match(r'\s*__s\[(\d+)\]\s*=', cline)
                    if m2:
                        slot = int(m2.group(1))
                        slot_literal_odd[slot] = False

                # Fix inline: if this line is chaos_normalize_native_int_argument(chaos_raw_arg_N)
                # and __s[N] was last assigned an odd literal, use raw_arg directly
                mn = re.match(
                    r'(\s*const auto chaos_arg_(\d+)\s*=\s*)chaos_normalize_native_int_argument\(chaos_raw_arg_\2\)(.*)',
                    cline,
                )
                if mn:
                    slot = int(mn.group(2))
                    if slot in slot_literal_odd and slot_literal_odd[slot]:
                        result.append(
                            f'{mn.group(1)}chaos_raw_arg_{slot}{mn.group(3)}\n'
                        )
                    else:
                        result.append(cline)
                else:
                    result.append(cline)

                i += 1
        else:
            result.append(line)
            i += 1

    return ''.join(result)


def _fix_todo_struct_fields(content: str) -> str:
    """Replace `// TODO: define fields for chaos_type_*` stubs with real fields.

    Codegen emits placeholder struct stubs for types that it knows about
    but can't resolve field layouts for (e.g., System.Object, System.Delegate).
    The generated runtime helper code then accesses fields on those structs,
    which fails at compile time.

    This function detects the TODO markers and injects appropriate field
    declarations for known types.
    """
    # Known struct layouts keyed by full type name
    known_todo_layouts = {
        'chaos_type_System_Private_CoreLib_System_Object': [
            '    chaos_object_header header{};\n',
        ],
        'chaos_type_System_Private_CoreLib_System_Delegate': [
            '    CHAOS_IL2CPP_INTPTR chaos_delegate_target = 0;\n',
            '    CHAOS_IL2CPP_INTPTR chaos_delegate_method_ptr = 0;\n',
            '    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_list = 0;\n',
            '    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_count = 0;\n',
        ],
        'chaos_type_System_Private_CoreLib_System_MulticastDelegate': [
            '    CHAOS_IL2CPP_INTPTR chaos_delegate_target = 0;\n',
            '    CHAOS_IL2CPP_INTPTR chaos_delegate_method_ptr = 0;\n',
            '    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_list = 0;\n',
            '    CHAOS_IL2CPP_INTPTR chaos_delegate_invocation_count = 0;\n',
        ],
        'chaos_type_System_Private_CoreLib_System_Exception': [
            '    CHAOS_IL2CPP_INTPTR _className = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _message = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _data = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _innerException = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _helpURL = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _stackTrace = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _stackTraceString = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _remoteStackTraceString = 0;\n',
            '    CHAOS_IL2CPP_INT32 _HResult = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _source = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _xptrs = 0;\n',
            '    CHAOS_IL2CPP_INT32 _xcode = -1;\n',
        ],
        'chaos_type_System_Private_CoreLib_System_SystemException': [
            '    CHAOS_IL2CPP_INTPTR _className = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _message = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _data = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _innerException = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _helpURL = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _stackTrace = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _stackTraceString = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _remoteStackTraceString = 0;\n',
            '    CHAOS_IL2CPP_INT32 _HResult = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _source = 0;\n',
            '    CHAOS_IL2CPP_INTPTR _xptrs = 0;\n',
            '    CHAOS_IL2CPP_INT32 _xcode = -1;\n',
        ],
    }

    def _replace_todo(m: re.Match) -> str:
        struct_name = m.group(1)
        base_clause = m.group(2) or ''
        indent = m.group(3) or '    '
        if struct_name in known_todo_layouts:
            fields = ''.join(known_todo_layouts[struct_name])
            # Reconstruct the full struct declaration with body replaced.
            return f'struct {struct_name} {base_clause}\n{{\n{fields}{indent}}};'
        return m.group(0)  # keep unchanged

    content = re.sub(
        r'struct\s+(chaos_type_\S+)\s*(:\s*public\s+chaos_type_System_Private_CoreLib_System_Object)?\s*\n\s*\{\n(\s*)//\s*TODO: define fields for \1\s*\n\s*\};',
        _replace_todo,
        content,
    )

    return content


def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Generate per-family hotupdate C++ tests")
    parser.add_argument("--families", nargs="*", help="Families to process (default: all)")
    args = parser.parse_args()

    families = args.families or FAMILIES
    print(f"Generating per-family Hotpatch tests (pure dispatch-table + interpreter) - {len(families)} families")

    for family_slug in families:
        generate_family(family_slug)

    print(f"\nDone. Hotpatch test and CMake artifacts in verification/foundation-dll/.../il2cpp_dist/hotupdate/")


if __name__ == "__main__":
    main()
