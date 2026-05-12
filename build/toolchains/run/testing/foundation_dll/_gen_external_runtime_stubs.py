"""Generate stub definitions for missing chaos_external_runtime_* functions.

The codegen emits calls to chaos_external_runtime_* helper functions but
sometimes skips emitting their definitions. This script detects those
missing functions and generates minimal stub definitions.

The stub return type is inferred from the function name suffix:
  System_Int32_  -> CHAOS_IL2CPP_INT32
  System_String_ -> CHAOS_IL2CPP_INTPTR
  System_Object_ -> CHAOS_IL2CPP_INTPTR
  System_Boolean_ -> CHAOS_IL2CPP_INT32
  System_DateTime_ -> CHAOS_IL2CPP_INTPTR
  System_TimeSpan_ -> CHAOS_IL2CPP_INTPTR
  System_Void (ctor) -> void
  System_Int64_ -> CHAOS_IL2CPP_INT64
  System_Char_ -> CHAOS_IL2CPP_UINT16
  System_Double_ -> CHAOS_IL2CPP_INT64
  default -> CHAOS_IL2CPP_INTPTR (safe fallback)

Stub bodies return 0 (int types) or nullptr (pointer types) and suppress
unused parameter warnings.

Usage:
    python _gen_external_runtime_stubs.py <path-to-generated.cpp>
"""

from __future__ import annotations

import re
import sys
from pathlib import Path


# Map from .NET type suffix in function name to C++ return type
RETURN_TYPE_MAP: dict[str, str] = {
    "System_Int32_": "CHAOS_IL2CPP_INT32",
    "System_Int64_": "CHAOS_IL2CPP_INT64",
    "System_UInt16_": "CHAOS_IL2CPP_UINT16",
    "System_Byte_": "CHAOS_IL2CPP_UINT8",
    "System_Boolean_": "CHAOS_IL2CPP_INT32",
    "System_Char_": "CHAOS_IL2CPP_UINT16",
    "System_Single_": "CHAOS_IL2CPP_FLOAT",
    "System_Double_": "CHAOS_IL2CPP_DOUBLE",
    "System_String_": "CHAOS_IL2CPP_INTPTR",
    "System_Object_": "CHAOS_IL2CPP_INTPTR",
    "System_Type_": "CHAOS_IL2CPP_INTPTR",
    "System_Array_": "CHAOS_IL2CPP_INTPTR",
    "System_DateTime_": "CHAOS_IL2CPP_INTPTR",
    "System_TimeSpan_": "CHAOS_IL2CPP_INTPTR",
    "System_Decimal_": "CHAOS_IL2CPP_INTPTR",
    "System_Enum_": "CHAOS_IL2CPP_INTPTR",
    "System_ValueType_": "CHAOS_IL2CPP_INTPTR",
    "System_Void": "void",
}

DEFAULT_RETURN_TYPE = "CHAOS_IL2CPP_INTPTR"


def _infer_return_type(fn_name: str) -> str:
    """Infer C++ return type from the function name's encoded return type suffix.

    The function name pattern is:
      chaos_external_runtime_<Namespace>_<Class>__<Method>_<ReturnType>_<ParamTypes>_
    We extract the return type by looking at the segment right after the method
    name's closing '__' and before the last '__' or end.
    """
    # For constructors (contains ___ctor), return void
    if "___ctor" in fn_name or "___cctor" in fn_name:
        return "void"

    # Find the return type suffix: after the method name and its __ separator.
    # The last __X__ or __X_ pattern before the final trailing underscore holds the return type.
    # Strategy: split on __ and check the last meaningful segment.
    # Actually, let's match the suffix patterns directly.
    for suffix, ret_type in RETURN_TYPE_MAP.items():
        if fn_name.endswith(suffix) or (suffix == "System_Void" and "__System_Void_" in fn_name):
            return ret_type

    return DEFAULT_RETURN_TYPE


def _is_void_return(ret_type: str) -> bool:
    return ret_type == "void"


def _generate_stub(fn_name: str, arg_count: int) -> str:
    """Generate a stub definition for a missing external runtime function."""
    ret_type = _infer_return_type(fn_name)
    is_void = _is_void_return(ret_type)

    # Build parameter list
    if arg_count == 0:
        params = ""
        param_names = ""
        void_kw = "void"
    else:
        params_list = [f"CHAOS_IL2CPP_INTPTR chaos_arg_{i}" for i in range(arg_count)]
        params = ", ".join(params_list)
        param_names = ", ".join(f"(void)chaos_arg_{i}" for i in range(arg_count))
        void_kw = ""

    # Build body
    if is_void:
        body = f"    {param_names};\n" if param_names else ""
    else:
        null_val = "static_cast<CHAOS_IL2CPP_INTPTR>(0)" if ret_type == "CHAOS_IL2CPP_INTPTR" else "0"
        body = f"    {param_names};\n    return {null_val};\n" if param_names else f"    return {null_val};\n"

    lines = [
        f"static {ret_type} {fn_name}({params if params else void_kw})",
        "{",
        body,
        "}",
    ]
    return "\n".join(lines)


def find_missing_external_runtime_fns(content: str) -> dict[str, int]:
    """Find chaos_external_runtime_* or chaos_stub_definition_* functions
    called but not defined.

    Returns dict mapping function name -> argument count at call site.
    """
    # All chaos_external_runtime_* and chaos_stub_definition_* names referenced in the file
    all_refs: set[str] = set()
    for m in re.finditer(r'(?:chaos_external_runtime_|chaos_stub_definition_)\w+', content):
        all_refs.add(m.group(0))

    # Those with definitions: static RET_TYPE fn_name(param_list) {
    defined: set[str] = set()
    for m in re.finditer(
        r'(?:static\s+)?(?:CHAOS_IL2CPP_\w+|void|chaos_valuetype_\w+|double|float)\s+'
        r'(chaos_external_runtime_\w+|chaos_stub_definition_\w+)\s*\(',
        content
    ):
        defined.add(m.group(1))

    missing = all_refs - defined

    # For each missing function, find the arg count at its call site
    result: dict[str, int] = {}
    for name in missing:
        for m in re.finditer(re.escape(name), content):
            # Find the opening paren after the function name
            paren = content.find('(', m.end())
            if paren == -1:
                continue
            # Count args between parens
            depth = 1
            pos = paren + 1
            commas = 0
            while depth > 0 and pos < len(content):
                c = content[pos]
                if c == '(':
                    depth += 1
                elif c == ')':
                    depth -= 1
                elif c == ',' and depth == 1:
                    commas += 1
                pos += 1
            arg_count = commas + 1 if (commas > 0 or content[paren+1:pos-1].strip()) else 0
            result[name] = arg_count
            break  # Only need one call site

    return result


def generate_stubs_cpp(content: str, missing: dict[str, int], family_slug: str) -> str:
    """Generate the complete stubs C++ code as a string."""
    if not missing:
        return ""

    lines = [
        "// Auto-generated stubs for missing chaos_external_runtime_* functions",
        f"// Family: {family_slug}",
        "// These functions are called by generated code but their definitions",
        "// were not emitted by the codegen.",
        "// CHAOS_IL2CPP_* types come from the included headers above.",
        "",
    ]

    for fn_name in sorted(missing.keys()):
        arg_count = missing[fn_name]
        stub = _generate_stub(fn_name, arg_count)
        lines.append(stub)
        lines.append("")
        lines.append("")

    return "\n".join(lines)


def _subject_id_to_fn_name(subject_id: str) -> str:
    """Convert a subjectId to the corresponding chaos_external_runtime_* function name.

    Must match the codegen's SanitizeSubjectId (NativeAotLoweringPlanner.ObjectModelUtilities.cs):
    replaces ALL non-alphanumeric characters with '_' (no rstrip, no suffix).

    "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)"
    → "chaos_external_runtime_System_Private_CoreLib_System_Convert__ToChar_System_Char_System_Byte_"
    """
    result = ''.join(c if c.isalnum() else '_' for c in subject_id)
    return f'chaos_external_runtime_{result}'


def _populate_fn_table(content: str) -> str:
    """Replace nullptr entries in kChaosExternalRuntimeFnTable with addresses of
    static chaos_external_runtime_* stub functions defined in this TU.

    The codegen emits table-indexed dispatch (kChaosExternalRuntimeFnTable[i]()) but
    leaves entries as nullptr, relying on startup resolution via
    ChaosResolveExternalRuntimeFnTable. For methods not registered in the hotpatch
    registry (e.g. runtime intrinsics not in this family's generated module), the
    entries stay nullptr causing null-pointer dereference. This function fills in
    addresses of static functions whose subject IDs match the dispatch table entries.

    For entries that would otherwise be nullptr AND are directly called via
    kChaosExternalRuntimeFnTable[N]() in the generated code, this function also
    generates minimal stub definitions on-the-fly so the call sites don't crash.
    """
    # Parse kChaosExternalRuntimeSubjects array
    subjects_match = re.search(
        r'extern "C" const char\* kChaosExternalRuntimeSubjects\[(\d+)\]\s*=\s*\{(.*?)\};',
        content, re.DOTALL
    )
    if not subjects_match:
        return content

    count = int(subjects_match.group(1))
    if count == 0:
        return content

    subject_ids = re.findall(r'"([^"]*)"', subjects_match.group(2))
    if len(subject_ids) != count:
        return content

    # Find which fn table indices are called directly in the code
    # Pattern: kChaosExternalRuntimeFnTable[N]) — the closing paren after
    # the index bracket means it's a call site, not a declaration.
    called_indices: set[int] = set()
    for m in re.finditer(r'kChaosExternalRuntimeFnTable\[(\d+)\]\)', content):
        called_indices.add(int(m.group(1)))

    # Build replacement fn table entries and collect stubs to generate on-the-fly
    fn_pointers: list[str] = []
    extra_stubs: dict[int, tuple[str, str]] = {}  # index -> (fn_name, subject_id)

    for i, sid in enumerate(subject_ids):
        fn_name = _subject_id_to_fn_name(sid)
        if f' {fn_name}(' in content:
            fn_pointers.append(f'    reinterpret_cast<void*>(&{fn_name}),')
        elif i in called_indices:
            # This entry is called directly via fn table dispatch but has no
            # chaos_external_runtime_* stub. Generate one on-the-fly.
            extra_stubs[i] = (fn_name, sid)
            fn_pointers.append(f'    reinterpret_cast<void*>(&{fn_name}),')
        else:
            fn_pointers.append('    nullptr,')

    if all(p.strip() == 'nullptr,' for p in fn_pointers):
        return content  # Nothing to wire in

    # Generate extra stubs and insert them before the fn table definition
    if extra_stubs:
        stub_lines = [
            "// Auto-generated stubs for kChaosExternalRuntimeFnTable entries",
            "// called directly via fn table dispatch (no chaos_external_runtime_*",
            "// call by name in the generated code).",
        ]
        for idx, (fn_name, sid) in sorted(extra_stubs.items()):
            # 0-arg stub: the call site casts the fn pointer directly
            stub = _generate_stub(fn_name, 0)
            stub_lines.append(stub)
            stub_lines.append("")

        stub_code = "\n".join(stub_lines)

        # Insert before kChaosExternalRuntimeFnTable definition line
        lines = content.split('\n')
        fn_table_idx = None
        for j, line in enumerate(lines):
            if 'kChaosExternalRuntimeFnTable[' in line:
                fn_table_idx = j
                break

        if fn_table_idx is not None:
            lines.insert(fn_table_idx, "")
            lines.insert(fn_table_idx, stub_code)
            content = '\n'.join(lines)

    # Replace the kChaosExternalRuntimeFnTable array body (leave nullptr for
    # entries with no matching function in this TU — they'll remain deferred
    # to ChaosResolveExternalRuntimeFnTable startup resolution)
    fn_table_pattern = re.compile(
        r'(extern "C" void\* kChaosExternalRuntimeFnTable\[\d+\]\s*=\s*\{)(.*?)(\};)',
        re.DOTALL
    )

    def replace_fn_table(m):
        return m.group(1) + '\n' + '\n'.join(fn_pointers) + '\n' + m.group(3)

    new_content = fn_table_pattern.sub(replace_fn_table, content)
    wired = sum(1 for p in fn_pointers if 'reinterpret_cast' in p)
    if wired:
        print(f"  [gen_ext_stubs] wired {wired}/{count} kChaosExternalRuntimeFnTable entries to stub functions")
    return new_content


def main():
    if len(sys.argv) < 2:
        print("Usage: python _gen_external_runtime_stubs.py <path-to-generated.cpp>")
        sys.exit(1)

    filepath = Path(sys.argv[1])
    if not filepath.exists():
        print(f"File not found: {filepath}")
        sys.exit(1)

    content = filepath.read_text(encoding="utf-8")
    missing = find_missing_external_runtime_fns(content)

    if not missing:
        print("  [gen_ext_stubs] no missing chaos_external_runtime_* functions found")
        sys.exit(0)

    # Determine family slug from path
    family_slug = "unknown"
    for part in filepath.parts:
        if "NativeEntry" in part or "native-entry" in part:
            family_slug = part.replace("NativeEntry", "").lower()

    stubs_code = generate_stubs_cpp(content, missing, family_slug)

    # Insert stubs after the last #include line (before any code that uses them).
    # Important: stubs must appear BEFORE their call sites, so we cannot append
    # at the end of the file. Find the last #include line and insert after it.
    lines = content.split('\n')
    last_include_idx = -1
    for i, line in enumerate(lines):
        if line.strip().startswith('#include'):
            last_include_idx = i

    if last_include_idx >= 0:
        insert_pos = last_include_idx + 1
        lines.insert(insert_pos, "")
        lines.insert(insert_pos + 1, stubs_code)
        new_content = "\n".join(lines)
    else:
        # Fallback: prepend to file
        new_content = stubs_code + "\n" + content

    # ── Post-processing: populate kChaosExternalRuntimeFnTable entries ────────
    # The codegen emits table-indexed dispatch via kChaosExternalRuntimeFnTable[i]()
    # but leaves all entries as nullptr (filled at startup by ChaosResolveExternalRuntimeFnTable
    # via hotpatch name registry). For methods the runtime can't resolve (e.g. Object.GetHashCode
    # that lives in the core runtime assembly), the entry stays nullptr → crash.
    #
    # After generating stub definitions, wire them into the dispatch table entries
    # so table-indexed calls work correctly.
    new_content = _populate_fn_table(new_content)

    filepath.write_text(new_content, encoding="utf-8")

    print(f"  [gen_ext_stubs] generated {len(missing)} stub(s) in {filepath}")
    for fn_name in sorted(missing.keys()):
        print(f"    + {fn_name} ({missing[fn_name]} args)")


if __name__ == "__main__":
    main()