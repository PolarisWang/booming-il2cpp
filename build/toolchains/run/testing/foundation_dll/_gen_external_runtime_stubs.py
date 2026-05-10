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
    """Find chaos_external_runtime_* functions called but not defined.

    Returns dict mapping function name -> argument count at call site.
    """
    # All chaos_external_runtime_* names referenced in the file
    all_refs: set[str] = set()
    for m in re.finditer(r'chaos_external_runtime_\w+', content):
        all_refs.add(m.group(0))

    # Those with definitions: static RET_TYPE fn_name(param_list) {
    defined: set[str] = set()
    for m in re.finditer(
        r'(?:static\s+)?(?:CHAOS_IL2CPP_\w+|void|chaos_valuetype_\w+|double|float)\s+'
        r'(chaos_external_runtime_\w+)\s*\(',
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

    filepath.write_text(new_content, encoding="utf-8")

    # We also need to add the stubs file to CMakeLists. For now, since we're
    # appending directly to native-aot.generated.cpp, no CMake change needed.
    print(f"  [gen_ext_stubs] generated {len(missing)} stub(s) in {filepath}")
    for fn_name in sorted(missing.keys()):
        print(f"    + {fn_name} ({missing[fn_name]} args)")


if __name__ == "__main__":
    main()