"""Generate synthetic C# entry point for a capability family.

This script creates a standalone C# file + project that serves as the input
for the Chaos.IL2CPP.CodeGen pipeline. The generated code contains one static
``Run(int entryIndex)`` method that switch-dispatches to N per-method stubs,
each calling one real API method (e.g. ``Convert.ToChar(true)``).

The resulting DLL is fed to ``chaos-il2cpp convert`` which produces closure
artifacts, then ``chaos-il2cpp emit-native-aot`` generates real C++ code for
every reachable method in the family.

Usage:
    python family_entrypoint_generator.py ^
        --assembly-name System.Private.CoreLib ^
        --family-id family/System.Private.CoreLib/convert/char
"""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(_HERE))

from test_code_generator import (_build_call_expr, _build_call_expr_with_refs, _cast_return_to_int,
                                 _default_expr, _has_blocked_param, _has_ref_param, _is_auto_callable,
                                 _parse_method_subject_id, _ref_return_expr,
                                 INSTANCE_ALTERNATIVE_EXPR_MAP, TYPE_ALTERNATIVE_MAP)
from native_codegen_generator import _slug_from_family_id, _family_namespace_slug, _method_slot_name



def _collect_required_usings(method_subject_ids: list[str]) -> set[str]:
    """Collect the using directives needed by the generated code."""
    usings = set()

    # Always needed
    usings.add("System")

    for subject_id in method_subject_ids:
        parsed = _parse_method_subject_id(subject_id)

        # Extract namespace from the method's own type path
        _add_type_using_from_full_path(parsed.get("type_path", ""), usings)

        # Extract namespace from parameter types
        for pt in parsed.get("param_types", []):
            _add_type_using(pt, usings)

        # Extract namespace from return type
        ret = parsed.get("return_type", "")
        if ret:
            _add_type_using(ret, usings)

    return usings


def _add_type_using_from_full_path(full_type_path: str, usings: set[str]) -> None:
    """Extract and add using from a full type path like 'System.Private.CoreLib/System.Reflection.MemberInfo'."""
    if "/" not in full_type_path:
        return
    _, full_type = full_type_path.split("/", 1)
    if "." in full_type:
        ns = ".".join(full_type.split(".")[:-1])
        if ns and ns != "System":
            usings.add(ns)


def _add_type_using(t: str, usings: set[str]) -> None:
    """Add using directive based on a CLR type string (parameter or return type)."""
    bare = t.rstrip("&*?").strip()
    # Strip array brackets
    while bare.endswith("[]"):
        bare = bare[:-2].strip()
    # Strip generic arity suffix (e.g. List`1 -> List)
    bare = re.sub(r"`\d+$", "", bare)
    if not bare or "." not in bare:
        return
    ns = ".".join(bare.split(".")[:-1])
    if ns and ns != "System":
        usings.add(ns)


def _build_call_expr_for_benchmark(subject_id: str) -> tuple[str, str]:
    """Build a C# call expression for a benchmark method subject ID.

    Returns (prelude, call_expr) where prelude contains local variable
    declarations for ref params (empty string if none).
    """
    parsed = _parse_method_subject_id(subject_id)

    if not _is_auto_callable(parsed):
        return ("", "")

    if _has_blocked_param(parsed["param_types"]):
        return ("", "")

    if _has_ref_param(parsed["param_types"]):
        try:
            return _build_call_expr_with_refs(parsed)
        except Exception:
            return ("", "")

    try:
        return ("", _build_call_expr(parsed))
    except Exception:
        return ("", "")


def _build_call_expr_for_semantic_patch(subject_id: str) -> tuple[str, str]:
    """Build a C# call expression for a semantic-patch variant.

    Uses TYPE_ALTERNATIVE_MAP and INSTANCE_ALTERNATIVE_EXPR_MAP to produce
    different parameter values than the benchmark variant, so the same method
    call with different inputs yields different results.

    Returns (prelude, call_expr).
    """
    parsed = _parse_method_subject_id(subject_id)

    if not _is_auto_callable(parsed):
        return ("", "")

    if _has_blocked_param(parsed["param_types"]):
        return ("", "")

    if _has_ref_param(parsed["param_types"]):
        try:
            return _build_call_expr_with_refs(parsed, type_map=TYPE_ALTERNATIVE_MAP, instance_map=INSTANCE_ALTERNATIVE_EXPR_MAP)
        except Exception:
            return ("", "")

    try:
        return ("", _build_call_expr(parsed, type_map=TYPE_ALTERNATIVE_MAP, instance_map=INSTANCE_ALTERNATIVE_EXPR_MAP))
    except Exception:
        return ("", "")


def _generate_entrypoint_source(
    assembly_name: str,
    family_id: str,
    method_subject_ids: list[str],
    class_name: str,
    namespace_name: str | None = None,
    *,
    variant: str = "benchmark",
) -> str:
    """Generate the synthetic C# entry point source code.

    Args:
        variant: "benchmark" (default) — MethodN calls real API (current behavior).
                 "patch" — each MethodN returns sentinel 0xB0000000 + N for hotupdate
                           verification.
                 "semantic-patch" — MethodN calls the same real API as benchmark, but
                           with different parameter values (from TYPE_ALTERNATIVE_MAP),
                           producing different but valid results for semantic verification.
    """
    family_slug = _slug_from_family_id(family_id)
    ns_slug = _family_namespace_slug(family_id)

    if variant == "patch":
        usings: set[str] = set()
    else:
        usings = _collect_required_usings(method_subject_ids)

    lines = [
        "// Auto-generated native-AOT entry point",
        f"// Family: {family_id}",
        f"// Assembly: {assembly_name}",
        f"// Variant: {variant}",
        "",
    ]

    for ns in sorted(usings):
        lines.append(f"using {ns};")
    if usings:
        lines.append("")

    if namespace_name:
        lines.append(f"namespace {namespace_name}")
        lines.append("{")

    ns_indent = "    " if namespace_name else ""

    lines.append(f"{ns_indent}public static class {class_name}")
    lines.append(f"{ns_indent}{{")

    # --- Run(int entryIndex) dispatcher ---
    lines.append(f"{ns_indent}    public static int Run(int entryIndex)")
    lines.append(f"{ns_indent}    {{")
    lines.append(f"{ns_indent}        switch (entryIndex)")
    lines.append(f"{ns_indent}        {{")

    for idx, subject_id in enumerate(method_subject_ids):
        lines.append(f"{ns_indent}            case {idx}: return Method{idx}();")

    lines.append(f"{ns_indent}            default: return -1;")
    lines.append(f"{ns_indent}        }}")
    lines.append(f"{ns_indent}    }}")
    lines.append("")

    # --- Per-method stubs ---
    for idx, subject_id in enumerate(method_subject_ids):
        lines.append(f"{ns_indent}    // [{idx}] {subject_id}")
        lines.append(f"{ns_indent}    static int Method{idx}()")
        lines.append(f"{ns_indent}    {{")

        if variant == "patch":
            # Patch variant: return sentinel 0xB0000000 + index
            # Use unchecked to suppress CS0221 (value exceeds int.MaxValue)
            lines.append(f"{ns_indent}        return unchecked((int)(0xB0000000u + {idx}));")
        elif variant == "semantic-patch":
            # Semantic-patch variant: call real API with different params, return checksum
            prelude, call_expr = _build_call_expr_for_semantic_patch(subject_id)
            if call_expr:
                parsed = _parse_method_subject_id(subject_id)
                if prelude:
                    lines.append(prelude)
                ret = parsed["return_type"]
                if ret == "System.Void" or not ret:
                    lines.append(f"{ns_indent}        {call_expr};")
                    ret_expr = _ref_return_expr(parsed)
                    lines.append(f"{ns_indent}        return (int){ret_expr};")
                else:
                    cast_expr = _cast_return_to_int(ret, call_expr)
                    lines.append(f"{ns_indent}        return {cast_expr};")
            else:
                lines.append(f"{ns_indent}        // TODO: {subject_id} could not be auto-generated for semantic-patch")
                lines.append(f"{ns_indent}        return 0;")
        else:
            # Benchmark variant: call real API, capture and return result as checksum
            prelude, call_expr = _build_call_expr_for_benchmark(subject_id)
            if call_expr:
                parsed = _parse_method_subject_id(subject_id)
                if prelude:
                    lines.append(prelude)
                ret = parsed["return_type"]
                if ret == "System.Void" or not ret:
                    lines.append(f"{ns_indent}        {call_expr};")
                    ret_expr = _ref_return_expr(parsed)
                    lines.append(f"{ns_indent}        return (int){ret_expr};")
                else:
                    cast_expr = _cast_return_to_int(ret, call_expr)
                    lines.append(f"{ns_indent}        return {cast_expr};")
            else:
                lines.append(f"{ns_indent}        // TODO: {subject_id} could not be auto-generated")
                lines.append(f"{ns_indent}        return 0;")

        lines.append(f"{ns_indent}    }}")
        lines.append("")

    lines.append(f"{ns_indent}}}")
    if namespace_name:
        lines.append("}")

    return "\n".join(lines)


def _generate_csproj(
    assembly_name: str,
    class_name: str,
    cs_file_name: str,
    target_framework: str = "net10.0",
) -> str:
    """Generate the .csproj for the synthetic entry point assembly."""
    namespace_part = f"<RootNamespace>{class_name}</RootNamespace>"

    return (
        '<Project Sdk="Microsoft.NET.Sdk">\n'
        "  <PropertyGroup>\n"
        "    <OutputType>Library</OutputType>\n"
        f"    <TargetFramework>{target_framework}</TargetFramework>\n"
        "    <Nullable>enable</Nullable>\n"
        "    <ImplicitUsings>enable</ImplicitUsings>\n"
        f"    <AssemblyName>{class_name}</AssemblyName>\n"
        f"    {namespace_part}\n"
        "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>\n"
        "  </PropertyGroup>\n"
        "  <ItemGroup>\n"
        f'    <Compile Include="{cs_file_name}" />\n'
        "  </ItemGroup>\n"
        "</Project>\n"
    )


def _generate_subject_manifest(
    class_name: str,
    entry_point_subject_id: str,
    output_dir: str,
) -> dict[str, Any]:
    """Generate a subject.manifest.json for the chaos-il2cpp convert command."""
    return {
        "source": {
            "type": "dotnet-project",
            "path": str(Path(output_dir) / f"{class_name}.csproj"),
            "primaryProjectPath": str(Path(output_dir) / f"{class_name}.csproj"),
            "entry": entry_point_subject_id,
        }
    }


def _compute_entry_point_subject_id(
    class_name: str,
    namespace_name: str | None,
) -> str:
    """Compute the subject ID for the Run(int) entry point.

    Format: <AssemblyName>/<FullTypeName>::<MethodName>:<ReturnType>(<ParamTypes>)
    """
    full_type = f"{namespace_name}.{class_name}" if namespace_name else class_name
    return f"{class_name}/{full_type}::Run:System.Int32(System.Int32)"


def generate_and_build(
    output_dir: Path,
    *,
    assembly_name: str,
    family_id: str,
    method_subject_ids: list[str],
    class_name: str | None = None,
    namespace_name: str | None = None,
    target_framework: str = "net10.0",
    variant: str = "benchmark",
) -> dict[str, Any]:
    """Generate the synthetic entry point and build it into a DLL.

    Args:
        variant: "benchmark" (default) — MethodN calls real API.
                 "patch" — each MethodN returns sentinel 0xB0000000 + N.
                 "semantic-patch" — MethodN calls real API with different params.

    Returns:
        Dict with keys:
          - dll_path: Path to the compiled DLL
          - csproj_path: Path to the .csproj
          - source_path: Path to the .cs file
          - entry_point_subject_id: The subject ID of the Run(int) method
    """
    if class_name is None:
        class_name = f"{_family_namespace_slug(family_id).title().replace('_', '')}NativeEntry"

    if variant == "patch":
        class_name = class_name.replace("NativeEntry", "PatchEntry")
    elif variant == "semantic-patch":
        class_name = class_name.replace("NativeEntry", "SemanticPatchEntry")

    cs_file_name = f"{class_name}.cs"
    csproj_name = f"{class_name}.csproj"

    # Generate source
    source = _generate_entrypoint_source(
        assembly_name=assembly_name,
        family_id=family_id,
        method_subject_ids=method_subject_ids,
        class_name=class_name,
        namespace_name=namespace_name,
        variant=variant,
    )

    # Generate csproj
    csproj = _generate_csproj(
        assembly_name=assembly_name,
        class_name=class_name,
        cs_file_name=cs_file_name,
        target_framework=target_framework,
    )

    # Write files
    output_dir.mkdir(parents=True, exist_ok=True)
    source_path = output_dir / cs_file_name
    csproj_path = output_dir / csproj_name
    source_path.write_text(source, encoding="utf-8")
    csproj_path.write_text(csproj, encoding="utf-8")

    # Build
    entry_point_subject_id = _compute_entry_point_subject_id(class_name, namespace_name)

    print(f"[entrypoint] Building {class_name}...")
    build_out = output_dir / "build-output"
    result = subprocess.run(
        ["dotnet", "build", str(csproj_path), "-o", str(build_out), "--nologo", "-v", "quiet"],
        capture_output=True, text=True,
    )
    if result.returncode != 0:
        print(f"[entrypoint] Build FAILED for {class_name}:")
        if result.stdout:
            print(result.stdout)
        if result.stderr:
            print(result.stderr)
        return {
            "success": False,
            "error": result.stderr or result.stdout,
            "dll_path": None,
            "csproj_path": str(csproj_path),
            "source_path": str(source_path),
            "entry_point_subject_id": entry_point_subject_id,
        }

    # Locate the DLL (it should be named {class_name}.dll)
    dll_path = build_out / f"{class_name}.dll"
    if not dll_path.exists():
        # Fallback: search in build output
        dlls = list(build_out.glob("*.dll"))
        if dlls:
            dll_path = dlls[0]
        else:
            print(f"[entrypoint] DLL not found in {build_out}")
            return {
                "success": False,
                "error": "DLL not found after build",
                "dll_path": None,
                "csproj_path": str(csproj_path),
                "source_path": str(source_path),
                "entry_point_subject_id": entry_point_subject_id,
            }

    print(f"[entrypoint] Built {dll_path}")
    return {
        "success": True,
        "dll_path": str(dll_path.resolve()),
        "build_output_dir": str(build_out.resolve()),
        "csproj_path": str(csproj_path),
        "source_path": str(source_path),
        "entry_point_subject_id": entry_point_subject_id,
    }


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate synthetic C# entry point for a capability family")
    parser.add_argument("--assembly-name", default="System.Private.CoreLib", help="Assembly name")
    parser.add_argument("--family-id", default="family/System.Private.CoreLib/convert/char", help="Family ID")
    parser.add_argument("--output-dir", type=Path, help="Output directory (default: verification/<assembly>/<slug>/entrypoint)")
    parser.add_argument("--class-name", help="Custom class name (default: auto-derived from family slug)")
    parser.add_argument("--namespace", dest="namespace_name", help="Optional namespace")
    parser.add_argument("--repo-root", type=Path, default=Path.cwd(), help="Repository root")
    parser.add_argument("--variant", default="benchmark", choices=["benchmark", "patch", "semantic-patch"],
                        help="Entrypoint variant (default: benchmark)")
    parser.add_argument("--method-subject-ids", nargs="*", help="Method subject IDs (read from contract if not specified)")
    args = parser.parse_args()

    repo_root = args.repo_root.resolve()

    # Collect method subject IDs
    if args.method_subject_ids:
        method_subject_ids = args.method_subject_ids
    else:
        # Read from capability family contract
        family_slug = _slug_from_family_id(args.family_id)
        contract_path = repo_root / "verification" / "foundation-dll" / args.assembly_name / family_slug / "capability-family-contract.json"
        if not contract_path.exists():
            print(f"FATAL: contract not found at {contract_path}", file=sys.stderr)
            sys.exit(1)
        with open(contract_path, encoding="utf-8") as f:
            contract = json.load(f)
        method_subject_ids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if "methodSubjectId" in m]
        if not method_subject_ids:
            method_subject_ids = [str(s) for s in contract.get("methodSubjectIds", []) if str(s)]

    if not method_subject_ids:
        print(f"FATAL: no method subject IDs found for {args.family_id}", file=sys.stderr)
        sys.exit(1)

    if args.output_dir:
        output_dir = args.output_dir
    else:
        family_slug = _slug_from_family_id(args.family_id)
        output_dir = repo_root / "verification" / "foundation-dll" / args.assembly_name / family_slug / "entrypoint"

    result = generate_and_build(
        output_dir,
        assembly_name=args.assembly_name,
        family_id=args.family_id,
        method_subject_ids=method_subject_ids,
        class_name=args.class_name,
        namespace_name=args.namespace_name,
        variant=args.variant,
    )

    if result["success"]:
        print(f"\nEntry point subject ID: {result['entry_point_subject_id']}")
        print(f"DLL path: {result['dll_path']}")
        print("\nNext steps:")
        print(f"  chaos-il2cpp {result['dll_path']} <output-root> --entry-point-subject-id \"{result['entry_point_subject_id']}\"")
        print(f"  chaos-il2cpp emit-native-aot <output-root>/analysis <native-out-dir>")
    else:
        print(f"\nFAILED: {result.get('error', 'unknown error')}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
