"""Generate synthetic C# entry point for a capability family.

Two-pass flow:
  1. PROBE: Generate and run a probe under managed .NET to capture each API
     method's return value (as int) or exception type. Methods that work with
     the existing call-expression builder are auto-probed; custom entries
     (from Custom.cs + contract customEntryIndices) bypass the probe.
  2. EMIT: Generate the real entry with proper Assert.Equal(<expected>, <call>)
     and Assert.Throws<ExceptionType>(() => <call>) using captured values.

The resulting EXE is translated by il2cpp to a native executable and run
directly — no C++ host, no checksums, no CMake.
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
_REPO_ROOT = _HERE.parents[4]
sys.path.insert(0, str(_HERE))

from test_code_generator import (_METHOD_OVERRIDES, _build_call_expr, _build_call_expr_with_refs, _cast_return_to_int,
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
    # Strip CLR generic argument braces: Action{System.Threading.Tasks.Task} -> Action
    bare = re.sub(r"\{.*\}", "", bare)
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


def _looks_like_property_read(expr: str) -> bool:
    """Check if a call_expr is a property read (not a method call).

    Property reads end with a name/chain like '.Name' or '.Value',
    while method calls end with ')'.
    """
    stripped = expr.strip()
    if stripped.endswith(')'):
        return False
    # It's a bare property/field access or cast expression
    return True


def _build_call_expr_for_benchmark(subject_id: str) -> tuple[str, str]:
    """Build a C# call expression for a benchmark method subject ID.

    Returns (prelude, call_expr) where prelude contains local variable
    declarations for ref params (empty string if none).
    """
    parsed = _parse_method_subject_id(subject_id)

    if not _is_auto_callable(parsed):
        return ("", "")

    # Check override map first — known problematic signatures bypass param checks
    param_count = len(parsed["param_types"])
    override = _METHOD_OVERRIDES.get((parsed["type_name"], parsed["method_name"], param_count))
    if override is not None and override != "skip":
        return ("", override)

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

    # Check override map first — known problematic signatures bypass param checks
    param_count = len(parsed["param_types"])
    override = _METHOD_OVERRIDES.get((parsed["type_name"], parsed["method_name"], param_count))
    if override is not None and override != "skip":
        return ("", override)

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
    custom_method_indices: set[int] | None = None,
    probe_mode: bool = False,
    expected_values: dict[int, dict] | None = None,
) -> str:
    """Generate the synthetic C# entry point source code.

    Two modes:
      probe_mode=True:  MethodN returns int (cast result), no assertions.
                         Used to run under managed .NET and capture expected values.
      probe_mode=False: MethodN is void, uses Assert.Equal(expected, call) or
                         Assert.Throws<ExceptionType>(() => call) based on probe data.

    Args:
        probe_mode: If True, generate int-returning methods for the probe run.
        expected_values: Dict mapping method index -> {"value": int|None, "exception": str|None}.
                         Only used when probe_mode=False.
    """
    family_slug = _slug_from_family_id(family_id)
    ns_slug = _family_namespace_slug(family_id)

    if variant == "patch":
        usings: set[str] = set()
    elif variant == "subjects":
        usings = _collect_required_usings(method_subject_ids)
        # subjects DLL is pure il2cpp input — no Chaos.TestFramework dependency
    else:
        usings = _collect_required_usings(method_subject_ids)
        usings.add("Chaos.TestFramework")

    # Normal mode: void entry with inlined assertion logic.
    # Does NOT call Assert.Equal/RecordFailure from SDK — codegen can't
    # resolve external assembly method symbols. Instead, uses a static
    # _exitCode field directly in the entry class.
    if not probe_mode:
        usings.discard("Chaos.TestFramework")  # don't need SDK in entry methods

    ns_indent = "    " if namespace_name else ""

    lines = [
        "// Auto-generated native-AOT entry point",
        f"// Family: {family_id}",
        f"// Assembly: {assembly_name}",
        f"// Variant: {variant}",
        "",
    ]

    for ns in sorted(usings):
        if ns:  # skip empty
            lines.append(f"using {ns};")
    if usings:
        lines.append("")

    if namespace_name:
        lines.append(f"namespace {namespace_name}")
        lines.append("{")

    ns_indent = "    " if namespace_name else ""

    # Use partial class to allow custom entry file extension
    lines.append(f"{ns_indent}public static partial class {class_name}")
    lines.append(f"{ns_indent}{{")

    if not probe_mode:
        lines.append(f"{ns_indent}    // Inlined exit code — avoids SDK method call resolution in codegen")
        lines.append(f"{ns_indent}    public static int _exitCode;")
        lines.append("")

    # --- Per-method stubs and Run dispatcher ---
    # No Action[] MethodTable — avoids delegate lowering issues in codegen.
    # Instead generates a Run(int entryIndex) switch dispatcher for runtime-entry.cpp
    # to call via --benchmark N or --hotupdate.
    method_prefix = "Subject_" if variant == "subjects" else "Method"
    custom_prefix = "CustomEntrySubject_" if variant == "subjects" else "CustomEntryMethod"

    for idx, subject_id in enumerate(method_subject_ids):
        lines.append(f"{ns_indent}    // [{idx}] {subject_id}")
        if idx in (custom_method_indices or set()):
            if probe_mode:
                # Generate int-returning probe stub inline (don't use Custom.cs which has void methods)
                lines.append(f"{ns_indent}    public static int {custom_prefix}{idx}()")
                lines.append(f"{ns_indent}    {{")
                prelude, call_expr = _build_call_expr_for_benchmark(subject_id)
                if call_expr:
                    if prelude:
                        lines.append(prelude)
                    lines.append(f"{ns_indent}        {call_expr};")
                lines.append(f"{ns_indent}        return 0;  // unreachable if throws")
                lines.append(f"{ns_indent}    }}")
                lines.append("")
            else:
                # Normal mode: void stub defined in Custom.cs
                lines.append("")
            continue

        if variant == "patch":
            lines.append(f"{ns_indent}    public static int {method_prefix}{idx}()")
            lines.append(f"{ns_indent}    {{")
            lines.append(f"{ns_indent}        return unchecked((int)(0xB0000000u + {idx}));")
            lines.append(f"{ns_indent}    }}")
            lines.append("")
            continue

        if probe_mode:
            # Probe mode: return int, capture value under managed .NET
            # Must be public so Program.cs can call it
            lines.append(f"{ns_indent}    public static int {method_prefix}{idx}()")
            lines.append(f"{ns_indent}    {{")
            prelude, call_expr = _build_call_expr_for_benchmark(subject_id)
            if call_expr:
                parsed = _parse_method_subject_id(subject_id)
                if prelude:
                    lines.append(prelude)
                ret = parsed["return_type"]
                if ret == "System.Void" or not ret:
                    lines.append(f"{ns_indent}        {call_expr};")
                    lines.append(f"{ns_indent}        return 0;")
                else:
                    cast_expr = _cast_return_to_int(ret, call_expr)
                    lines.append(f"{ns_indent}        return {cast_expr};")
            else:
                lines.append(f"{ns_indent}        return -1;  // cannot auto-generate call")
            lines.append(f"{ns_indent}    }}")
            lines.append("")
            continue

        # Normal mode: void entry with proper Assert.Equal(expected, actual)
        # Must be public so Program.cs (different class) can call it
        lines.append(f"{ns_indent}    public static void {method_prefix}{idx}()")
        lines.append(f"{ns_indent}    {{")

        ev = (expected_values or {}).get(idx)
        prelude, call_expr = _build_call_expr_for_benchmark(subject_id)
        if call_expr:
            parsed = _parse_method_subject_id(subject_id)
            if prelude:
                lines.append(prelude)
            ret = parsed["return_type"]

            if ev and ev.get("exception"):
                # Known-throwing method: emit try/catch. The stub throws directly
                # via C++ throw and /EHs allows propagation through extern "C" frames.
                exc_type = ev["exception"]
                if _looks_like_property_read(call_expr):
                    lines.append(f"{ns_indent}        try {{ _ = {call_expr}; _exitCode = 1; }}")
                else:
                    lines.append(f"{ns_indent}        try {{ {call_expr}; _exitCode = 1; }}")
                lines.append(f"{ns_indent}        catch ({exc_type}) {{ }}")
            elif ev and "value" in ev and ev["value"] is not None:
                # Known value: compare with expected, set _exitCode on mismatch
                # Use Convert.ToInt32 for type-safe comparison (handles object, bool, etc.)
                if ret == "System.Void" or not ret:
                    lines.append(f"{ns_indent}        {call_expr};")
                else:
                    cast_expr = _cast_return_to_int(ret, call_expr)
                    lines.append(f"{ns_indent}        if ({cast_expr} != {ev['value']}) _exitCode = 1;")
            else:
                # Fallback: void or unknown
                if ret == "System.Void" or not ret:
                    lines.append(f"{ns_indent}        {call_expr};")
                else:
                    cast_expr = _cast_return_to_int(ret, call_expr)
                    lines.append(f"{ns_indent}        if ({cast_expr} != {cast_expr}) _exitCode = 1;")
        else:
            lines.append(f"{ns_indent}        // TODO: {subject_id} could not be auto-generated")
        lines.append(f"{ns_indent}    }}")
        lines.append("")

    # --- Generate Run(int entryIndex) switch dispatcher ---
    # Allows runtime-entry.cpp to invoke individual methods by index
    # for --benchmark N and --hotupdate modes.
    lines.append(f"{ns_indent}    public static void Run(int entryIndex)")
    lines.append(f"{ns_indent}    {{")
    lines.append(f"{ns_indent}        switch (entryIndex)")
    lines.append(f"{ns_indent}        {{")
    for idx in range(len(method_subject_ids)):
        mn = f"{custom_prefix}{idx}" if idx in (custom_method_indices or set()) else f"{method_prefix}{idx}"
        lines.append(f"{ns_indent}            case {idx}: {mn}(); break;")
    lines.append(f"{ns_indent}        }}")
    lines.append(f"{ns_indent}    }}")
    lines.append("")

    lines.append(f"{ns_indent}}}")
    if namespace_name:
        lines.append("}")

    return "\n".join(lines)


def _generate_program_source(
    class_name: str,
    namespace_name: str | None,
    *,
    probe_mode: bool = False,
    method_count: int = 0,
    custom_method_indices: set[int] | None = None,
) -> str:
    """Generate Program.cs.

    probe_mode=True:  Run each MethodN() directly, print RESULT N: <value>
                      or EXCEPTION N: <ExceptionType> for each. Handles custom
                      entries (throwing methods) with try/catch.
    probe_mode=False: Call each MethodN() directly (no MethodTable to avoid
                      delegate lowering issues in codegen). Custom entries use
                      CustomEntryMethodN naming.
    """
    ns_indent = "    " if namespace_name else ""
    full_class = f"{namespace_name}.{class_name}" if namespace_name else class_name
    cmi = custom_method_indices or set()

    if probe_mode:
        # Probe Program.cs: call each method, capture result or exception
        parts = [
            "using System;",
            "using System.Collections.Generic;",
            "",
            "public static class Program",
            "{",
            "    static int Main()",
            "    {",
            "        var results = new List<string>();",
        ]

        for idx in range(method_count):
            if idx in cmi:
                parts.append(f"        try {{ results.Add($\"RESULT {idx}:{{Convert.ToInt32({full_class}.CustomEntryMethod{idx}())}}\"); }}")
                parts.append(f"        catch (System.Exception ex) {{ results.Add($\"EXCEPTION {idx}:{{ex.GetType().Name}}\"); }}")
            else:
                parts.append(f"        try {{ results.Add($\"RESULT {idx}:{{Convert.ToInt32({full_class}.Method{idx}())}}\"); }}")
                parts.append(f"        catch (System.Exception ex) {{ results.Add($\"EXCEPTION {idx}:{{ex.GetType().Name}}\"); }}")

        parts.extend([
            "        Console.WriteLine(string.Join(\"\\n\", results));",
            "        return 0;",
            "    }",
            "}",
        ])
        return "\n".join(parts) + "\n"

    # Normal mode: inline runner, fully unrolled (no loops — avoids IL flat-goto)
    # Uses inlined _exitCode field on the entry class (not ChaosAssertState.ExitCode)
    # to avoid cross-assembly method resolution issues in codegen.
    parts = [
        '',
    ]
    if namespace_name:
        parts.append(f"namespace {namespace_name}")
        parts.append("{")

    parts.append(f"{ns_indent}public class Program")
    parts.append(f"{ns_indent}{{")
    parts.append(f"{ns_indent}    static int Main()")
    parts.append(f"{ns_indent}    {{")
    parts.append(f"{ns_indent}        int failures = 0;")
    for idx in range(method_count):
        method_name = f"CustomEntryMethod{idx}" if idx in cmi else f"Method{idx}"
        parts.append(
            f"{ns_indent}        {full_class}._exitCode = 0; {full_class}.{method_name}(); "
            f"failures += {full_class}._exitCode << {idx};"
        )
    parts.append(f"{ns_indent}        return failures;")
    parts.append(f"{ns_indent}    }}")
    parts.append(f"{ns_indent}}}")
    if namespace_name:
        parts.append("}")

    return "\n".join(parts) + "\n"


def _generate_csproj(
    assembly_name: str,
    class_name: str,
    cs_file_name: str,
    *,
    variant: str = "benchmark",
    has_custom_entry: bool = False,
) -> str:
    """Generate the .csproj for the synthetic entry point assembly.

    For subjects variant: OutputType is Library, no TestFramework refs, no Program.cs.
    For other variants: OutputType is Exe so il2cpp translates it to a native executable.
    """
    namespace_part = f"<RootNamespace>{class_name}</RootNamespace>"

    if variant == "subjects":
        # Subjects DLL: pure il2cpp input, no TestFramework, no Program.cs, net8.0
        return (
            '<Project Sdk="Microsoft.NET.Sdk">\n'
            "  <PropertyGroup>\n"
            "    <OutputType>Library</OutputType>\n"
            f"    <TargetFramework>net8.0</TargetFramework>\n"
            "    <Nullable>enable</Nullable>\n"
            "    <ImplicitUsings>disable</ImplicitUsings>\n"
            f"    <AssemblyName>{class_name}</AssemblyName>\n"
            f"    {namespace_part}\n"
            "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>\n"
            "  </PropertyGroup>\n"
            "  <ItemGroup>\n"
            f'    <Compile Include="{cs_file_name}" />\n'
            "  </ItemGroup>\n"
            "</Project>\n"
        )

    if variant == "patch":
        # Patch DLL: Library, no TestFramework, no Program.cs, net8.0
        return (
            '<Project Sdk="Microsoft.NET.Sdk">\n'
            "  <PropertyGroup>\n"
            "    <OutputType>Library</OutputType>\n"
            f"    <TargetFramework>net8.0</TargetFramework>\n"
            "    <Nullable>enable</Nullable>\n"
            "    <ImplicitUsings>disable</ImplicitUsings>\n"
            f"    <AssemblyName>{class_name}</AssemblyName>\n"
            f"    {namespace_part}\n"
            "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>\n"
            "  </PropertyGroup>\n"
            "  <ItemGroup>\n"
            f'    <Compile Include="{cs_file_name}" />\n'
            "  </ItemGroup>\n"
            "</Project>\n"
        )

    # References
    sdk_csproj = _REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
    runner_csproj = _REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Runner" / "Chaos.TestFramework.Runner.csproj"

    chaos_tf_ref = (
        "  <ItemGroup>\n"
        f'    <ProjectReference Include="{sdk_csproj}" />\n'
        f'    <ProjectReference Include="{runner_csproj}" />\n'
        "  </ItemGroup>\n"
    )

    custom_cs = f'    <Compile Include="{class_name}.Custom.cs" />\n' if has_custom_entry else ""

    return (
        '<Project Sdk="Microsoft.NET.Sdk">\n'
        "  <PropertyGroup>\n"
        "    <OutputType>Exe</OutputType>\n"  # Exe so il2cpp -> native exe
        f"    <TargetFramework>net10.0</TargetFramework>\n"
        "    <Nullable>enable</Nullable>\n"
        "    <ImplicitUsings>enable</ImplicitUsings>\n"
        f"    <AssemblyName>{class_name}</AssemblyName>\n"
        f"    {namespace_part}\n"
        "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>\n"
        "  </PropertyGroup>\n"
        f"{chaos_tf_ref}"
        "  <ItemGroup>\n"
        f'    <Compile Include="{cs_file_name}" />\n'
        f'    <Compile Include="Program.cs" />\n'
        f"{custom_cs}"
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
    *,
    variant: str = "benchmark",
) -> str:
    """Compute the subject ID for the entry point.

    For non-patch, non-subjects variants:
        Entry is Program.Main() which returns int, takes no args.
        ABI: int()
        Format: <AssemblyName>/Program::Main:System.Int32()
    For patch and subjects variants:
        Entry is Run(int) which returns int, takes int32.
        ABI: int(int32)
        Format: <AssemblyName>/<FullTypeName>::Run:System.Int32(System.Int32)
    """
    if variant == "patch":
        full_type = f"{namespace_name}.{class_name}" if namespace_name else class_name
        return f"{class_name}/{full_type}::Run:System.Int32(System.Int32)"
    if variant == "subjects":
        full_type = f"{namespace_name}.{class_name}" if namespace_name else class_name
        return f"{class_name}/{full_type}::Run:System.Void(System.Int32)"
    # Non-patch: entry is Program.Main()
    return f"{class_name}/Program::Main:System.Int32()"


def _run_probe_and_capture(
    output_dir: Path,
    class_name: str,
    method_subject_ids: list[str],
    custom_method_indices: set[int] | None = None,
) -> dict[int, dict]:
    """Run the probe EXE under managed .NET to capture expected values.

    Returns dict mapping method index -> {"value": int|None, "exception": str|None}.
    Methods with auto-generated calls get their return value captured.
    Custom entry methods (throwing) get their exception type captured.
    Methods that can't be auto-generated get {"skip": True}.
    """
    print("[probe] Running managed probe to capture expected values...")
    build_out = output_dir / "build-output"
    # Probe was built as Exe, so it's {class_name}.exe in build-output
    exe_path = build_out / f"{class_name}.exe"

    if not exe_path.exists():
        # Maybe it was renamed from a previous run
        alt = build_out / f"{class_name}.Probe.exe"
        if alt.exists():
            exe_path = alt
        else:
            print(f"[probe] Probe EXE not found at {exe_path}")
            return {}

    r = subprocess.run([str(exe_path)], capture_output=True, text=True, timeout=60)
    if r.returncode != 0:
        print(f"[probe] Probe failed (rc={r.returncode}): {r.stderr[:200]}")
        return {}

    expected = {}
    cmi = custom_method_indices or set()
    for line in r.stdout.splitlines():
        line = line.strip()
        # Parse "RESULT N:<value>" or "EXCEPTION N:<type>"
        m = re.match(r'RESULT (\d+):(-?\d+)', line)
        if m:
            idx = int(m.group(1))
            value = int(m.group(2))
            callable_idx = idx not in cmi and idx < len(method_subject_ids)
            if callable_idx:
                # Check if this method was auto-callable in the probe build
                _, call_expr = _build_call_expr_for_benchmark(method_subject_ids[idx])
                if call_expr:
                    expected[idx] = {"value": value, "exception": None}
                    continue
            # Custom entries or non-callable: keep as exception probe data
            expected[idx] = {"value": value, "exception": None}
            continue

        m = re.match(r'EXCEPTION (\d+):(.+)', line)
        if m:
            idx = int(m.group(1))
            exception_type = m.group(2).strip()
            callable_idx = idx not in cmi and idx < len(method_subject_ids)
            if not callable_idx:
                # Custom entry (expected throwing): record exception type
                expected[idx] = {"value": None, "exception": exception_type}
            else:
                # Auto-generated call that threw — likely a blocked method
                expected[idx] = {"value": None, "exception": exception_type}

    print(f"[probe] Captured {len(expected)} expected values")
    return expected


def generate_and_build(
    output_dir: Path,
    *,
    assembly_name: str,
    family_id: str,
    method_subject_ids: list[str],
    class_name: str | None = None,
    namespace_name: str | None = None,
    variant: str = "benchmark",
) -> dict[str, Any]:
    """Generate the synthetic entry point and build it into an EXE.

    Two-pass flow:
      1. Probe: Generate int-returning entry, build as Library (no Main),
         run via reflection or probe EXE to capture expected values.
      2. Emit: Generate void entry with Assert.Equal(expected, call)
         and Assert.Throws<ExceptionType>(() => call).

    Auto-detects custom entry file at {output_dir}/{class_name}.Custom.cs and
    custom method indices from contract customEntryIndices.
    """
    if class_name is None:
        class_name = f"{_family_namespace_slug(family_id).title().replace('_', '')}NativeEntry"

    if variant == "subjects":
        class_name = class_name.replace("NativeEntry", "Subjects")
    elif variant == "patch":
        class_name = class_name.replace("NativeEntry", "PatchEntry")
    elif variant == "semantic-patch":
        class_name = class_name.replace("NativeEntry", "SemanticPatchEntry")

    cs_file_name = f"{class_name}.cs"
    csproj_name = f"{class_name}.csproj"
    source_path = output_dir / cs_file_name
    csproj_path = output_dir / csproj_name

    # Auto-detect custom entry file
    custom_cs_path = output_dir / f"{class_name}.Custom.cs"
    has_custom_entry = custom_cs_path.exists()

    # Auto-detect custom method indices: check contract for customEntryIndices,
    # or fall back to custom per-method contracts with "customEntry": true
    custom_method_indices: set[int] | None = None
    if has_custom_entry:
        # Build custom_method_indices from contract if available
        family_slug = _slug_from_family_id(family_id)
        contract_path = _REPO_ROOT / "verification" / "foundation-dll" / assembly_name / family_slug / "capability-family-contract.json"
        if contract_path.exists():
            with open(contract_path, encoding="utf-8") as f:
                contract = json.load(f)
            indices = contract.get("customEntryIndices")
            if indices is not None:
                custom_method_indices = set(indices)
            else:
                # Fallback: look for per-method "customEntry": true in methodContracts
                custom_mids = set()
                for mc in contract.get("methodContracts", []):
                    if mc.get("customEntry") and mc.get("methodSubjectId") in method_subject_ids:
                        custom_mids.add(method_subject_ids.index(mc["methodSubjectId"]))
                if custom_mids:
                    custom_method_indices = custom_mids

    # ── Pass 1: Probe ──
    if variant == "benchmark":
        probe_source = _generate_entrypoint_source(
            assembly_name=assembly_name,
            family_id=family_id,
            method_subject_ids=method_subject_ids,
            class_name=class_name,
            namespace_name=namespace_name,
            variant=variant,
            custom_method_indices=custom_method_indices,
            probe_mode=True,
        )
        probe_program = _generate_program_source(
            class_name, namespace_name,
            probe_mode=True,
            method_count=len(method_subject_ids),
            custom_method_indices=custom_method_indices,
        )
        probe_csproj = _generate_csproj(
            assembly_name=assembly_name,
            class_name=class_name,
            cs_file_name=cs_file_name,
            variant=variant,
            has_custom_entry=False,  # probe generates its own int stubs inline
        )

        output_dir.mkdir(parents=True, exist_ok=True)
        (output_dir / cs_file_name).write_text(probe_source, encoding="utf-8")
        (output_dir / csproj_name).write_text(probe_csproj, encoding="utf-8")
        (output_dir / "Program.cs").write_text(probe_program, encoding="utf-8")

        print(f"[probe] Building {class_name} probe...")
        build_out = output_dir / "build-output"
        build_result = subprocess.run(
            ["dotnet", "build", str(csproj_path := output_dir / csproj_name), "-o", str(build_out), "--nologo", "-v", "quiet"],
            capture_output=True, text=True,
        )
        if build_result.returncode != 0:
            print(f"[probe] Build FAILED: {build_result.stderr[:200]}")
            # Fall through: proceed with no expected values (generates self-comparison)
            expected_values = {}
        else:
            expected_values = _run_probe_and_capture(
                output_dir,
                class_name,
                method_subject_ids,
                custom_method_indices=custom_method_indices,
            )
    else:
        expected_values = {}

    # ── Pass 2: Emit final entry EXE with proper assertions ──
    final_source = _generate_entrypoint_source(
        assembly_name=assembly_name,
        family_id=family_id,
        method_subject_ids=method_subject_ids,
        class_name=class_name,
        namespace_name=namespace_name,
        variant=variant,
        custom_method_indices=custom_method_indices,
        expected_values=expected_values,
    )

    source_path = output_dir / cs_file_name
    csproj_path = output_dir / csproj_name
    output_dir.mkdir(parents=True, exist_ok=True)
    source_path.write_text(final_source, encoding="utf-8")

    entry_point_subject_id = _compute_entry_point_subject_id(class_name, namespace_name, variant=variant)

    # ── Subjects variant: Library, no Program.cs, build as DLL ──
    if variant == "subjects":
        (output_dir / csproj_name).write_text(
            _generate_csproj(
                assembly_name=assembly_name,
                class_name=class_name,
                cs_file_name=cs_file_name,
                variant=variant,
            ),
            encoding="utf-8",
        )
        print(f"[subjects] Building {class_name} DLL...")
        build_out = output_dir / "build-output"
        result = subprocess.run(
            ["dotnet", "build", str(csproj_path), "-o", str(build_out), "--nologo", "-v", "quiet"],
            capture_output=True, text=True,
        )
        if result.returncode != 0:
            print(f"[subjects] Build FAILED for {class_name}:")
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
        dll_path = build_out / f"{class_name}.dll"
        if not dll_path.exists():
            dlls = list(build_out.glob("*.dll"))
            if dlls:
                dll_path = dlls[0]
            else:
                return {
                    "success": False,
                    "error": "DLL not found after build",
                    "dll_path": None,
                    "csproj_path": str(csproj_path),
                    "source_path": str(source_path),
                    "entry_point_subject_id": entry_point_subject_id,
                }
        print(f"[subjects] Built {dll_path}")
        return {
            "success": True,
            "dll_path": str(dll_path.resolve()),
            "build_output_dir": str(build_out.resolve()),
            "csproj_path": str(csproj_path),
            "source_path": str(source_path),
            "entry_point_subject_id": entry_point_subject_id,
        }

    # ── Non-subjects variants: Exe with Program.cs ──
    final_program = _generate_program_source(
        class_name, namespace_name,
        method_count=len(method_subject_ids),
        custom_method_indices=custom_method_indices,
    )
    (output_dir / "Program.cs").write_text(final_program, encoding="utf-8")
    (output_dir / csproj_name).write_text(
        _generate_csproj(
            assembly_name=assembly_name,
            class_name=class_name,
            cs_file_name=cs_file_name,
            variant=variant,
            has_custom_entry=has_custom_entry,
        ),
        encoding="utf-8",
    )

    print(f"[entrypoint] Building {class_name} (final)...")
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
    parser.add_argument("--output-dir", type=Path, help="Output directory (default: verification/<assembly>/<slug>/il2cpp_dist/entrypoint)")
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
        output_dir = repo_root / "verification" / "foundation-dll" / args.assembly_name / family_slug / "il2cpp_dist" / "entrypoint"

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
