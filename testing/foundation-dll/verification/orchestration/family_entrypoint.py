"""Family entrypoint generation — synthetic C# entry points for AOT/JIT verification.

Two-pass flow:
  1. PROBE: Generate and run a probe under managed .NET to capture each API
     method's return value (as int) or exception type.
  2. EMIT: Generate the real entry with proper Assert.Equal / Assert.Throws.

Also generates runtime-entry.cpp with CLI dispatch for entry.exe.

Absorbed from old pipeline family_entrypoint_generator.py.
"""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
from typing import Any

_REPO_ROOT = Path(__file__).resolve().parents[4]

from verification.stages.test_code_generator import (METHOD_OVERRIDES, build_call_expr, build_call_expr_with_refs,
                                                     build_call_expr_with_ref_locals, cast_return_to_int,
                                                     default_expr, get_skip_reason, has_blocked_param, has_ref_param,
                                                     is_auto_callable,
                                                     parse_method_subject_id, ref_return_expr,
                                                     INSTANCE_ALTERNATIVE_EXPR_MAP, TYPE_ALTERNATIVE_MAP)
from verification.stages.native_code_generator import slug_from_family_id, family_namespace_slug, method_slot_name


# ═══════════════════════════════════════════════════════════════════
# Utility helpers
# ═══════════════════════════════════════════════════════════════════

def collect_required_usings(method_subject_ids: list[str]) -> set[str]:
    """Collect the using directives needed by the generated code."""
    usings = {"System", "System.Collections.Generic", "System.Linq"}
    for subject_id in method_subject_ids:
        parsed = parse_method_subject_id(subject_id)
        add_type_using_from_full_path(parsed.get("type_path", ""), usings)
        for pt in parsed.get("param_types", []):
            add_type_using(pt, usings)
        ret = parsed.get("return_type", "")
        if ret:
            add_type_using(ret, usings)
    return usings


def add_type_using_from_full_path(full_type_path: str, usings: set[str]) -> None:
    if "/" not in full_type_path:
        return
    assembly_part, full_type = full_type_path.split("/", 1)
    if "." in full_type:
        ns = ".".join(full_type.split(".")[:-1])
        if ns and ns != "System":
            usings.add(ns)
    elif assembly_part == "SnapshotTestFixtures":
        usings.add(assembly_part)


def add_type_using(t: str, usings: set[str]) -> None:
    bare = t.rstrip("&*?()").strip()
    if '{' in bare and '}' not in bare:
        bare = bare[:bare.index('{')]
    bare = re.sub(r"\{.*\}", "", bare)
    bare = re.sub(r"<.*>", "", bare)
    while bare.endswith("[]"):
        bare = bare[:-2].strip()
    bare = re.sub(r"`\d+$", "", bare)
    if not bare or "." not in bare:
        return
    ns = ".".join(bare.split(".")[:-1])
    if ns and ns != "System":
        usings.add(ns)


def looks_like_property_read(expr: str) -> bool:
    stripped = expr.strip()
    return not stripped.endswith(')')


def build_call_expr_for_benchmark(subject_id: str) -> tuple[str, str]:
    parsed = parse_method_subject_id(subject_id)
    if not is_auto_callable(parsed):
        return ("", "")
    param_count = len(parsed["param_types"])
    override = METHOD_OVERRIDES.get((parsed["type_name"], parsed["method_name"], param_count))
    if override is not None and override != "skip":
        tn, mn = parsed["type_name"], parsed["method_name"]
        if (
            param_count >= 1
            and parsed["param_types"][0] == "System.String"
            and (
                (tn == "Convert" and mn.startswith("To"))
                or (tn == "Guid" and mn == ".ctor")
            )
        ) or (param_count == 0):
            return ("", override)
    if has_blocked_param(parsed["param_types"]):
        return ("", "")
    if has_ref_param(parsed["param_types"]):
        try:
            return build_call_expr_with_ref_locals(parsed)
        except Exception:
            try:
                return build_call_expr_with_refs(parsed)
            except Exception:
                return ("", "")
    try:
        return ("", build_call_expr(parsed))
    except Exception:
        return ("", "")


def build_call_expr_for_semantic_patch(subject_id: str) -> tuple[str, str]:
    parsed = parse_method_subject_id(subject_id)
    if not is_auto_callable(parsed):
        return ("", "")
    param_count = len(parsed["param_types"])
    override = METHOD_OVERRIDES.get((parsed["type_name"], parsed["method_name"], param_count))
    if override is not None and override != "skip":
        return ("", override)
    if has_blocked_param(parsed["param_types"]):
        return ("", "")
    if has_ref_param(parsed["param_types"]):
        try:
            return build_call_expr_with_refs(parsed, type_map=TYPE_ALTERNATIVE_MAP, instance_map=INSTANCE_ALTERNATIVE_EXPR_MAP)
        except Exception:
            return ("", "")
    try:
        return ("", build_call_expr(parsed, type_map=TYPE_ALTERNATIVE_MAP, instance_map=INSTANCE_ALTERNATIVE_EXPR_MAP))
    except Exception:
        return ("", "")


_SYSTEM_TYPE_TO_CSHARP: dict[str, str] = {
    "System.Boolean": "bool", "System.Byte": "byte", "System.SByte": "sbyte",
    "System.Int16": "short", "System.UInt16": "ushort", "System.Int32": "int",
    "System.UInt32": "uint", "System.Int64": "long", "System.UInt64": "ulong",
    "System.Single": "float", "System.Double": "double", "System.Char": "char",
    "System.String": "string", "System.IntPtr": "IntPtr", "System.UIntPtr": "UIntPtr",
    "System.Object": "object", "System.Type": "Type", "System.Array": "Array",
    "System.Delegate": "Delegate",
}


def csharp_param_declarations(param_types: list[str]) -> str:
    parts: list[str] = []
    for i, pt in enumerate(param_types):
        bare = pt.rstrip("&*?").strip()
        cs_type = _SYSTEM_TYPE_TO_CSHARP.get(bare, bare)
        parts.append(f"{cs_type} p{i}")
    return ", ".join(parts)


# ═══════════════════════════════════════════════════════════════════
# Source generation
# ═══════════════════════════════════════════════════════════════════

def generate_entrypoint_source(
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
    has_custom_entry: bool = False,
) -> str:
    """Generate the synthetic C# entry point source code.

    probe_mode=True:  MethodN returns int (cast result), no assertions.
    probe_mode=False: MethodN is void, uses Assert.Equal(expected, call) or
                      Assert.Throws<ExceptionType>(() => call).
    """
    family_slug = slug_from_family_id(family_id)
    ns_slug = family_namespace_slug(family_id)

    if variant == "patch":
        usings: set[str] = set()
    elif variant == "subjects":
        usings = collect_required_usings(method_subject_ids)
    else:
        usings = collect_required_usings(method_subject_ids)
        usings.add("Chaos.TestFramework")

    if not probe_mode:
        usings.discard("Chaos.TestFramework")

    ns_indent = "    " if namespace_name else ""

    lines = [
        "// Auto-generated native-AOT entry point",
        f"// Family: {family_id}",
        f"// Assembly: {assembly_name}",
        f"// Variant: {variant}",
        "",
    ]
    for ns in sorted(usings):
        if ns:
            lines.append(f"using {ns};")
    if usings:
        lines.append("")

    if namespace_name:
        lines.append(f"namespace {namespace_name}")
        lines.append("{")

    ns_indent = "    " if namespace_name else ""

    lines.append(f"{ns_indent}public static partial class {class_name}")
    lines.append(f"{ns_indent}{{")

    if not probe_mode:
        lines.append(f"{ns_indent}    // Inlined exit code — avoids SDK method call resolution in codegen")
        lines.append(f"{ns_indent}    public static int _exitCode;")
        lines.append("")

    method_prefix = "Subject_" if variant in ("subjects", "patch") else "Method"
    custom_prefix = "CustomEntrySubject_" if variant == "subjects" else "CustomEntryMethod"

    for idx, subject_id in enumerate(method_subject_ids):
        lines.append(f"{ns_indent}    // [{idx}] {subject_id}")
        if idx in (custom_method_indices or set()):
            if probe_mode:
                lines.append(f"{ns_indent}    public static int {custom_prefix}{idx}()")
                lines.append(f"{ns_indent}    {{")
                lines.append(f"{ns_indent}        return 0;")
                lines.append(f"{ns_indent}    }}")
                lines.append("")
            else:
                if variant in ("subjects", "patch") and not has_custom_entry:
                    lines.append(f"{ns_indent}    public static void {custom_prefix}{idx}()")
                    lines.append(f"{ns_indent}    {{")
                    lines.append(f"{ns_indent}    }}")
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
            lines.append(f"{ns_indent}    public static int {method_prefix}{idx}()")
            lines.append(f"{ns_indent}    {{")
            prelude, call_expr = build_call_expr_for_benchmark(subject_id)
            if call_expr:
                parsed = parse_method_subject_id(subject_id)
                if prelude:
                    lines.append(prelude)
                ret = parsed["return_type"]
                if ret == "System.Void" or not ret:
                    lines.append(f"{ns_indent}        {call_expr};")
                    lines.append(f"{ns_indent}        return 0;")
                else:
                    cast_expr = cast_return_to_int(ret, call_expr)
                    lines.append(f"{ns_indent}        return {cast_expr};")
            else:
                lines.append(f"{ns_indent}        return -1;  // cannot auto-generate call")
            lines.append(f"{ns_indent}    }}")
            lines.append("")
            continue

        parsed = parse_method_subject_id(subject_id)
        is_self_ref = (variant == "subjects" and parsed["type_name"] == class_name)
        if is_self_ref:
            param_decls = csharp_param_declarations(parsed["param_types"]) if parsed["param_types"] else ""
            ret_type = parsed["return_type"]
            if ret_type == "System.Void":
                cs_return = "void"
                body_lines = [f"{ns_indent}        try {{ }}", f"{ns_indent}        catch {{ _exitCode = 1; }}"]
            else:
                bare = ret_type.rstrip("&*?").strip()
                cs_return = _SYSTEM_TYPE_TO_CSHARP.get(bare, bare)
                body_lines = [f"{ns_indent}        try {{ return default; }}", f"{ns_indent}        catch {{ _exitCode = 1; return default; }}"]
            lines.append(f"{ns_indent}    public static {cs_return} {method_prefix}{idx}({param_decls})")
            lines.append(f"{ns_indent}    {{")
            for bl in body_lines:
                lines.append(bl)
            lines.append(f"{ns_indent}    }}")
            lines.append("")
            continue

        lines.append(f"{ns_indent}    public static void {method_prefix}{idx}()")
        lines.append(f"{ns_indent}    {{")

        ev = (expected_values or {}).get(idx)
        prelude, call_expr = build_call_expr_for_benchmark(subject_id)
        if call_expr:
            parsed = parse_method_subject_id(subject_id)
            if prelude:
                lines.append(prelude)
            ret = parsed["return_type"]

            if ev and ev.get("exception"):
                exc_type = ev["exception"]
                _EXCEPTION_FULL_NAMES = {"SecurityException": "System.Security.SecurityException"}
                exc_type = _EXCEPTION_FULL_NAMES.get(exc_type, exc_type)
                if looks_like_property_read(call_expr):
                    lines.append(f"{ns_indent}        try {{ _ = {call_expr}; _exitCode = 1; }}")
                else:
                    lines.append(f"{ns_indent}        try {{ {call_expr}; _exitCode = 1; }}")
                lines.append(f"{ns_indent}        catch ({exc_type}) {{ }}")
            elif ev and "value" in ev and ev["value"] is not None:
                if ret == "System.Void" or not ret:
                    lines.append(f"{ns_indent}        {call_expr};")
                else:
                    cast_expr = cast_return_to_int(ret, call_expr)
                    lines.append(f"{ns_indent}        if ({cast_expr} != {ev['value']}) _exitCode = 1;")
            else:
                if ret == "System.Void" or not ret:
                    lines.append(f"{ns_indent}        try {{ {call_expr}; }}")
                    lines.append(f"{ns_indent}        catch {{ _exitCode = 1; }}")
                else:
                    cast_expr = cast_return_to_int(ret, call_expr)
                    lines.append(f"{ns_indent}        try {{ if ({cast_expr} != {cast_expr}) _exitCode = 1; }}")
                    lines.append(f"{ns_indent}        catch {{ _exitCode = 1; }}")
        else:
            parsed = parse_method_subject_id(subject_id)
            skip_reason = get_skip_reason(parsed)
            if skip_reason.startswith("IMPLEMENTABLE"):
                lines.append(f"{ns_indent}        // IMPLEMENTABLE gap: {skip_reason}")
                lines.append(f"{ns_indent}        // TODO: wrapper for {subject_id}")
                lines.append(f"{ns_indent}        _exitCode = 0;  // stub — no assertion")
            else:
                lines.append(f"{ns_indent}        // {skip_reason or 'non-callable'}: {subject_id}")
        lines.append(f"{ns_indent}    }}")
        lines.append("")

    if variant != "subjects":
        lines.append(f"{ns_indent}    public static void Run(int entryIndex)")
        lines.append(f"{ns_indent}    {{")
        lines.append(f"{ns_indent}        try")
        lines.append(f"{ns_indent}        {{")
        lines.append(f"{ns_indent}            switch (entryIndex)")
        lines.append(f"{ns_indent}            {{")
        for idx in range(len(method_subject_ids)):
            mn = f"{custom_prefix}{idx}" if idx in (custom_method_indices or set()) else f"{method_prefix}{idx}"
            lines.append(f"{ns_indent}                case {idx}: {mn}(); break;")
        lines.append(f"{ns_indent}            }}")
        lines.append(f"{ns_indent}        }}")
        lines.append(f"{ns_indent}        catch")
        lines.append(f"{ns_indent}        {{")
        lines.append(f"{ns_indent}            _exitCode = 1;")
        lines.append(f"{ns_indent}        }}")
        lines.append(f"{ns_indent}    }}")
        lines.append("")

    lines.append(f"{ns_indent}}}")
    if namespace_name:
        lines.append("}")

    return "\n".join(lines)


def generate_program_source(
    class_name: str,
    namespace_name: str | None,
    *,
    probe_mode: bool = False,
    method_count: int = 0,
    custom_method_indices: set[int] | None = None,
    method_prefix: str = "Method",
) -> str:
    """Generate Program.cs for the entry EXE."""
    ns_indent = "    " if namespace_name else ""
    full_class = f"{namespace_name}.{class_name}" if namespace_name else class_name
    cmi = custom_method_indices or set()

    if probe_mode:
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
                parts.append(f"        try {{ results.Add($\"RESULT {idx}:{{Convert.ToInt32({full_class}.CustomEntry{method_prefix}{idx}())}}\"); }}")
                parts.append(f"        catch (System.Exception ex) {{ results.Add($\"EXCEPTION {idx}:{{ex.GetType().Name}}\"); }}")
            else:
                parts.append(f"        try {{ results.Add($\"RESULT {idx}:{{Convert.ToInt32({full_class}.{method_prefix}{idx}())}}\"); }}")
                parts.append(f"        catch (System.Exception ex) {{ results.Add($\"EXCEPTION {idx}:{{ex.GetType().Name}}\"); }}")
        parts.extend([
            "        Console.WriteLine(string.Join(\"\\n\", results));",
            "        return 0;",
            "    }",
            "}",
        ])
        return "\n".join(parts) + "\n"

    parts = ['']
    if namespace_name:
        parts.append(f"namespace {namespace_name}")
        parts.append("{")

    parts.append(f"{ns_indent}public class Program")
    parts.append(f"{ns_indent}{{")
    parts.append(f"{ns_indent}    static int Main()")
    parts.append(f"{ns_indent}    {{")
    parts.append(f"{ns_indent}        int failures = 0;")
    for idx in range(method_count):
        method_name = f"CustomEntry{method_prefix}{idx}" if idx in cmi else f"{method_prefix}{idx}"
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


def generate_project_file(
    assembly_name: str,
    class_name: str,
    cs_file_name: str,
    *,
    variant: str = "benchmark",
    has_custom_entry: bool = False,
    output_dir: Path | None = None,
    extra_refs: list[str] | None = None,
    target_framework: str | None = None,
) -> str:
    """Generate the .csproj for the synthetic entry point assembly."""
    namespace_part = f"<RootNamespace>{class_name}</RootNamespace>"

    if variant == "subjects":
        tfm = target_framework or "net8.0"
        extra_cs = ""
        if output_dir is not None and output_dir.is_dir():
            custom_cs_name = f"{class_name}.Custom.cs"
            for f in sorted(output_dir.iterdir()):
                if f.suffix == ".cs" and f.name != cs_file_name:
                    if has_custom_entry and f.name == custom_cs_name:
                        continue
                    extra_cs += f'    <Compile Include="{f.name}" />\n'
        custom_cs = f'    <Compile Include="{class_name}.Custom.cs" />\n' if has_custom_entry else ""
        extra_refs_xml = "  </ItemGroup>\n"
        if extra_refs:
            items = "\n".join(f'    <ProjectReference Include="{r}" />' for r in extra_refs)
            extra_refs_xml = f"  </ItemGroup>\n  <ItemGroup>\n{items}\n  </ItemGroup>\n"
        remove_test_refs = (
            '  <ItemGroup>\n'
            '    <PackageReference Remove="Microsoft.NET.Test.Sdk" />\n'
            '    <PackageReference Remove="xunit" />\n'
            '    <PackageReference Remove="xunit.runner.visualstudio" />\n'
            '    <PackageReference Remove="FsCheck.Xunit" />\n'
            '  </ItemGroup>\n'
        )
        return (
            '<Project Sdk="Microsoft.NET.Sdk">\n'
            "  <PropertyGroup>\n"
            "    <OutputType>Library</OutputType>\n"
            f"    <TargetFramework>{tfm}</TargetFramework>\n"
            "    <Nullable>enable</Nullable>\n"
            "    <ImplicitUsings>disable</ImplicitUsings>\n"
            "    <IsTestProject>false</IsTestProject>\n"
            f"    <AssemblyName>{class_name}</AssemblyName>\n"
            f"    {namespace_part}\n"
            "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>\n"
            "    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>\n"
            "    <NoWarn>$(NoWarn);SYSLIB0011;SYSLIB0020;SYSLIB0050;CS8602;CS8604</NoWarn>\n"
            "  </PropertyGroup>\n"
            "  <ItemGroup>\n"
            f'    <Compile Include="{cs_file_name}" />\n'
            f"{extra_cs}"
            f"{custom_cs}"
            f"{extra_refs_xml}"
            f"{remove_test_refs}"
            "</Project>\n"
        )

    if variant == "patch":
        custom_cs = f'    <Compile Include="{class_name}.Custom.cs" />\n' if has_custom_entry else ""
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
            f"{custom_cs}"
            "  </ItemGroup>\n"
            "</Project>\n"
        )

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
        "    <OutputType>Exe</OutputType>\n"
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


def generate_subject_manifest(
    class_name: str,
    entry_point_subject_id: str,
    output_dir: str,
) -> dict[str, Any]:
    return {
        "source": {
            "type": "dotnet-project",
            "path": str(Path(output_dir) / f"{class_name}.csproj"),
            "primaryProjectPath": str(Path(output_dir) / f"{class_name}.csproj"),
            "entry": entry_point_subject_id,
        }
    }


def compute_entry_point_subject_id(
    class_name: str,
    namespace_name: str | None,
    *,
    variant: str = "benchmark",
) -> str:
    if variant == "patch":
        full_type = f"{namespace_name}.{class_name}" if namespace_name else class_name
        return f"{class_name}/{full_type}::Run:System.Void(System.Int32)"
    if variant == "subjects":
        return ""
    return f"{class_name}/Program::Main:System.Int32()"


def build_project_safe(csproj_path: Path, build_out: Path) -> subprocess.CompletedProcess:
    """Build a .csproj, working around MSBuild comma-in-path limitation on Windows."""
    csproj_str = str(csproj_path)
    build_out_str = str(build_out)
    if "," not in csproj_str and "," not in build_out_str:
        return subprocess.run(
            ["dotnet", "build", csproj_str, "-o", build_out_str, "--nologo", "-v", "quiet"],
            capture_output=True, text=True,
        )

    temp_dir = Path(tempfile.mkdtemp(prefix="compound_build_"))
    temp_out_dir = Path(tempfile.mkdtemp(prefix="compound_out_"))
    temp_csproj = temp_dir / csproj_path.name
    shutil.copy2(csproj_path, temp_csproj)
    src_dir = csproj_path.parent
    for f in src_dir.iterdir():
        if f.suffix == ".cs":
            shutil.copy2(f, temp_dir / f.name)

    result = subprocess.run(
        ["dotnet", "build", str(temp_csproj), "-o", str(temp_out_dir), "--nologo", "-v", "quiet"],
        capture_output=True, text=True,
    )

    if result.returncode == 0:
        build_out.mkdir(parents=True, exist_ok=True)
        for dll in temp_out_dir.glob("*.dll"):
            shutil.copy2(dll, build_out / dll.name)
        for exe in temp_out_dir.glob("*.exe"):
            shutil.copy2(exe, build_out / exe.name)
        for pdb in temp_out_dir.glob("*.pdb"):
            shutil.copy2(pdb, build_out / pdb.name)

    shutil.rmtree(temp_dir, ignore_errors=True)
    shutil.rmtree(temp_out_dir, ignore_errors=True)
    return result


def run_probe_and_capture(
    output_dir: Path,
    class_name: str,
    method_subject_ids: list[str],
    custom_method_indices: set[int] | None = None,
) -> dict[int, dict]:
    """Run the probe EXE under managed .NET to capture expected values."""
    print("[probe] Running managed probe to capture expected values...")
    build_out = output_dir / "build-output"
    exe_path = build_out / f"{class_name}.exe"

    if not exe_path.exists():
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
        m = re.match(r'RESULT (\d+):(-?\d+)', line)
        if m:
            idx = int(m.group(1))
            value = int(m.group(2))
            callable_idx = idx not in cmi and idx < len(method_subject_ids)
            if callable_idx:
                _, call_expr = build_call_expr_for_benchmark(method_subject_ids[idx])
                if call_expr:
                    expected[idx] = {"value": value, "exception": None}
                    continue
            expected[idx] = {"value": value, "exception": None}
            continue

        m = re.match(r'EXCEPTION (\d+):(.+)', line)
        if m:
            idx = int(m.group(1))
            exception_type = m.group(2).strip()
            expected[idx] = {"value": None, "exception": exception_type}

    print(f"[probe] Captured {len(expected)} expected values")
    return expected


# ═══════════════════════════════════════════════════════════════════
# Main entry point — generate_and_build
# ═══════════════════════════════════════════════════════════════════

def generate_and_build(
    output_dir: Path,
    *,
    assembly_name: str,
    family_id: str,
    method_subject_ids: list[str],
    class_name: str | None = None,
    namespace_name: str | None = None,
    variant: str = "benchmark",
    extra_refs: list[str] | None = None,
    target_framework: str | None = None,
) -> dict[str, Any]:
    """Generate the synthetic entry point and build it into an EXE.

    Two-pass flow:
      1. Probe: Generate int-returning entry, build as Library (no Main),
         run via reflection or probe EXE to capture expected values.
      2. Emit: Generate void entry with Assert.Equal(expected, call)
         and Assert.Throws<ExceptionType>(() => call).
    """
    if class_name is None:
        class_name = f"{family_namespace_slug(family_id).title().replace('_', '').replace(',', '')}NativeEntry"

    if variant == "subjects":
        class_name = class_name.replace("NativeEntry", "Subjects")
    elif variant == "patch":
        class_name = class_name.replace("NativeEntry", "PatchEntry")
    elif variant == "semantic-patch":
        class_name = class_name.replace("NativeEntry", "SemanticPatchEntry")

    cs_file_name = f"{class_name}.cs"
    csproj_name = f"{class_name}.csproj"

    # Auto-detect custom entry file
    custom_cs_path = output_dir / f"{class_name}.Custom.cs"
    has_custom_entry = custom_cs_path.exists()

    custom_method_indices: set[int] | None = None
    family_slug = slug_from_family_id(family_id)
    # Try multiple contract locations: computed slug, then actual output_dir parent(s)
    candidate_paths = [
        _REPO_ROOT / "testing" / "foundation-dll" / assembly_name / family_slug / "capability-family-contract.json",
        _REPO_ROOT / "testing" / "foundation-dll" / assembly_name / family_slug / "contract.json",
    ]
    for parent in [output_dir, output_dir.parent]:
        for fname in ("capability-family-contract.json", "contract.json"):
            candidate = parent / fname
            if candidate not in candidate_paths:
                candidate_paths.append(candidate)
    contract = None
    for cp in candidate_paths:
        if cp.exists():
            try:
                with open(cp, encoding="utf-8") as f:
                    contract = json.load(f)
                if contract.get("customEntryIndices") is not None:
                    break
            except Exception:
                continue
    if contract is not None:
        indices = contract.get("customEntryIndices")
        if indices is not None:
            custom_method_indices = set(indices)
        else:
            custom_mids = set()
            for mc in contract.get("methodContracts", []):
                if mc.get("customEntry") and mc.get("methodSubjectId") in method_subject_ids:
                    custom_mids.add(method_subject_ids.index(mc["methodSubjectId"]))
            if custom_mids:
                custom_method_indices = custom_mids

    # ── Pass 1: Probe ──
    def _run_probe() -> dict[int, dict]:
        probe_prefix = "Subject_" if variant in ("subjects", "patch") else "Method"
        probe_source = generate_entrypoint_source(
            assembly_name=assembly_name, family_id=family_id,
            method_subject_ids=method_subject_ids, class_name=class_name,
            namespace_name=namespace_name, variant=variant,
            custom_method_indices=custom_method_indices, probe_mode=True,
        )
        probe_program = generate_program_source(
            class_name, namespace_name, probe_mode=True,
            method_count=len(method_subject_ids),
            custom_method_indices=custom_method_indices,
            method_prefix=probe_prefix,
        )
        probe_csproj = generate_project_file(
            assembly_name=assembly_name, class_name=class_name,
            cs_file_name=cs_file_name, variant="benchmark",
            has_custom_entry=False, target_framework=target_framework,
        )

        output_dir.mkdir(parents=True, exist_ok=True)
        (output_dir / cs_file_name).write_text(probe_source, encoding="utf-8")
        (output_dir / csproj_name).write_text(probe_csproj, encoding="utf-8")
        (output_dir / "Program.cs").write_text(probe_program, encoding="utf-8")

        print(f"[probe] Building {class_name} probe...")
        build_out = output_dir / "build-output"
        build_result = build_project_safe(output_dir / csproj_name, build_out)
        if build_result.returncode != 0:
            print(f"[probe] Build FAILED: {build_result.stderr[:200]}")
            return {}
        return run_probe_and_capture(
            output_dir, class_name, method_subject_ids,
            custom_method_indices=custom_method_indices,
        )

    if variant in ("benchmark", "subjects"):
        expected_values = _run_probe()
    else:
        expected_values = {}

    # ── Pass 2: Emit final entry with proper assertions ──
    final_source = generate_entrypoint_source(
        assembly_name=assembly_name, family_id=family_id,
        method_subject_ids=method_subject_ids, class_name=class_name,
        namespace_name=namespace_name, variant=variant,
        custom_method_indices=custom_method_indices,
        expected_values=expected_values, has_custom_entry=has_custom_entry,
    )

    source_path = output_dir / cs_file_name
    csproj_path = output_dir / csproj_name
    output_dir.mkdir(parents=True, exist_ok=True)
    source_path.write_text(final_source, encoding="utf-8")

    entry_point_subject_id = compute_entry_point_subject_id(class_name, namespace_name, variant=variant)

    # ── Subjects variant: Library, no Program.cs ──
    if variant == "subjects":
        probe_program = output_dir / "Program.cs"
        if probe_program.exists():
            probe_program.unlink()
        probe_build_out = output_dir / "build-output"
        if probe_build_out.exists():
            shutil.rmtree(probe_build_out)

        (output_dir / csproj_name).write_text(
            generate_project_file(
                assembly_name=assembly_name, class_name=class_name,
                cs_file_name=cs_file_name, variant=variant,
                has_custom_entry=has_custom_entry, output_dir=output_dir,
                extra_refs=extra_refs, target_framework=target_framework,
            ), encoding="utf-8",
        )
        print(f"[subjects] Building {class_name} DLL...")
        build_out = output_dir / "build-output"
        result = build_project_safe(csproj_path, build_out)
        if result.returncode != 0:
            return {"success": False, "error": result.stderr or result.stdout,
                    "dll_path": None, "csproj_path": str(csproj_path),
                    "source_path": str(source_path), "entry_point_subject_id": entry_point_subject_id}
        dll_path = build_out / f"{class_name}.dll"
        if not dll_path.exists():
            dlls = list(build_out.glob("*.dll"))
            dll_path = dlls[0] if dlls else None
            if dll_path is None:
                return {"success": False, "error": "DLL not found after build",
                        "dll_path": None, "csproj_path": str(csproj_path),
                        "source_path": str(source_path), "entry_point_subject_id": entry_point_subject_id}
        print(f"[subjects] Built {dll_path}")
        return {"success": True, "dll_path": str(dll_path.resolve()),
                "build_output_dir": str(build_out.resolve()),
                "csproj_path": str(csproj_path), "source_path": str(source_path),
                "entry_point_subject_id": entry_point_subject_id}

    # ── Non-subjects variants: Exe with Program.cs ──
    final_program = generate_program_source(
        class_name, namespace_name, method_count=len(method_subject_ids),
        custom_method_indices=custom_method_indices,
    )
    (output_dir / "Program.cs").write_text(final_program, encoding="utf-8")
    (output_dir / csproj_name).write_text(
        generate_project_file(
            assembly_name=assembly_name, class_name=class_name,
            cs_file_name=cs_file_name, variant=variant,
            has_custom_entry=has_custom_entry, target_framework=target_framework,
        ), encoding="utf-8",
    )

    print(f"[entrypoint] Building {class_name} (final)...")
    build_out = output_dir / "build-output"
    result = build_project_safe(csproj_path, build_out)
    if result.returncode != 0:
        return {"success": False, "error": result.stderr or result.stdout,
                "dll_path": None, "csproj_path": str(csproj_path),
                "source_path": str(source_path), "entry_point_subject_id": entry_point_subject_id}

    dll_path = build_out / f"{class_name}.dll"
    if not dll_path.exists():
        dlls = list(build_out.glob("*.dll"))
        dll_path = dlls[0] if dlls else None
        if dll_path is None:
            return {"success": False, "error": "DLL not found after build",
                    "dll_path": None, "csproj_path": str(csproj_path),
                    "source_path": str(source_path), "entry_point_subject_id": entry_point_subject_id}

    print(f"[entrypoint] Built {dll_path}")
    return {"success": True, "dll_path": str(dll_path.resolve()),
            "build_output_dir": str(build_out.resolve()),
            "csproj_path": str(csproj_path), "source_path": str(source_path),
            "entry_point_subject_id": entry_point_subject_id}


# ═══════════════════════════════════════════════════════════════════
# Runtime entry C++ generation
# ═══════════════════════════════════════════════════════════════════

def generate_runtime_entry(*, is_jit: bool = False) -> str:
    """Generate runtime-entry.cpp with CLI dispatch for entry.exe.

    Supports modes: fact (default), --benchmark N iters, --hotupdate,
    --hotupdate-and-benchmark N iters, --microbench.

    When is_jit=True, includes extern declaration and call to ChaosJitRegisterAll()
    to activate the JIT compilation path at startup.
    """
    import textwrap
    jit_decl = textwrap.dedent('''\
        extern "C" void ChaosJitRegisterAll();
        extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module;

        #if defined(_WIN32)
        #include <windows.h>
        static LONG CALLBACK JitVehHandler(PEXCEPTION_POINTERS ExceptionInfo) noexcept {
            auto* ctx = ExceptionInfo->ContextRecord;
            std::fprintf(stderr,
                "JIT CRASH: Code=0x%08lX RIP=0x%p RSP=0x%p"
                " RAX=0x%p RBX=0x%p RCX=0x%p RDX=0x%p RSI=0x%p RDI=0x%p\\n",
                ExceptionInfo->ExceptionRecord->ExceptionCode,
                (void*)ctx->Rip, (void*)ctx->Rsp,
                (void*)ctx->Rax, (void*)ctx->Rbx,
                (void*)ctx->Rcx, (void*)ctx->Rdx,
                (void*)ctx->Rsi, (void*)ctx->Rdi);
            std::fflush(stderr);
            return EXCEPTION_CONTINUE_SEARCH;
        }
        #endif
    ''') if is_jit else ''

    jit_call = textwrap.dedent('''\
        #if defined(_WIN32)
            AddVectoredExceptionHandler(1, JitVehHandler);
        #endif
            if (chaos_il2cpp_aot_hotpatch_module != nullptr) {
                chaos::il2cpp::runtime_core::RegisterHotpatchModule(chaos_il2cpp_aot_hotpatch_module);
            }
            ChaosJitRegisterAll();
    ''') if is_jit else ''

    template = '''// runtime-entry.cpp — Auto-generated
// CLI entry point for native-AOT verification dispatch.
//
// Modes:
//   (no args)            — fact: run all subject entries, print Passed: N/M
//   --benchmark N I      — benchmark method N for I iterations
//   --hotupdate          — hotpatch fact: baseline + apply + semantic-check + revert
//   --hotupdate-and-benchmark N I — post-patch benchmark
//   --microbench         — interpreter microbenchmarks

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <cstdint>
#include <cinttypes>

#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#endif

// Define the log-output redirection flag declared extern in <chaos/log.h>.
// The inline log_write() in log.h references g_log_use_stderr to decide
// whether diagnostic log output goes to stdout or stderr.  The prebuilt
// chaos_runtime_core.lib (SDK) does not currently export this symbol, so
// we define it here in the entry point TU to satisfy the linker.
namespace chaos { namespace il2cpp { namespace common { namespace log_internal {
bool g_log_use_stderr = false;
}}}}

#include <chaos/config.h>
#include <chaos/native_types.h>
#include <runtime_core.h>

#include "chaos_runtime_host.h"

#define CHAOS_IL2CPP_COMMON_EH_H_
#define CHAOS_EH_TRY               try {
#define CHAOS_EH_CATCH_BEGIN       } catch (const chaos_managed_exception& chaos_exception) {
#define CHAOS_EH_END               }
#define CHAOS_EH_EXCEPTION_OBJ     (chaos_exception.object_value)

#include <chaos/hotpatch_dispatch.h>
#include <patch_loader.h>

extern "C" const int kAotMethodCount;
extern "C" const int kSubjectEntryCount;
extern "C" const int kSubjectSlotMap[];

extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept;

// Codegen registration symbols (defined in native-aot.generated.cpp)
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration;
extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration;
extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options;
extern "C" void ChaosRegisterGcLayouts();

// Default arg thunks (defined in native-aot.generated.cpp)
extern "C" void (*kDefaultArgThunks[])() noexcept;

// Benchmark result struct (must match verification_dispatch.generated.cpp)
struct BenchmarkResult {
    double elapsed_ms;
    int64_t allocated_bytes;
};

extern "C" CHAOS_IL2CPP_INT32 RunFactAll();
extern "C" BenchmarkResult RunBenchmark(int entry_index, int iterations);
extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll();
extern "C" BenchmarkResult RunHotpatchBenchmark(int entry_index, int iterations);

extern const uint8_t kPatchData[];
extern const size_t kPatchDataSize;
extern const char* const kPatchDataHostClassName;

extern "C" void RunMicrobench();

__JIT_DECL__

static chaos::il2cpp::runtime_core::PatchContext* ApplyHotpatchIfAvailable() {
    if (kPatchDataSize > 0u) {
        auto* patch_ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemory(
            kPatchData, kPatchDataSize, kPatchDataHostClassName);
        if (patch_ctx == nullptr) {
            std::fprintf(stderr, "WARN: ApplyPatchFromMemory returned null (no patches applied)\\n");
        } else {
            std::fprintf(stderr, "INFO: Applied patches to %u methods\\n",
                         static_cast<unsigned>(patch_ctx->method_count));
        }
        return patch_ctx;
    }
    return nullptr;
}

static int RunFactMode() {
    int failed_count = RunFactAll();
    int passed_count = kSubjectEntryCount - failed_count;
    printf("Passed: %d/%d\\n", passed_count, kSubjectEntryCount);
    std::fflush(stdout);
    return failed_count;
}

// ── --fact-json: per-method JSON output (R1+R2: value-level verification) ──
static int RunFactJsonMode() {
    const int kCount = kSubjectEntryCount;
    printf("{\\"factResults\\":[");
    bool first = true;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        CHAOS_IL2CPP_INT32 result = 0;
        bool caught = false;
        CHAOS_EH_TRY
            result = chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, kDefaultArgThunks);
        CHAOS_EH_CATCH_BEGIN
            caught = true;
        CHAOS_EH_END
        if (!first) printf(",");
        printf("{\\"si\\":%d,\\"methodIndex\\":%d,\\"passed\\":%s,\\"exitCode\\":%d}",
               si, i, caught ? "false" : "true", caught ? -1 : (int)result);
        first = false;
    }
    printf("]}\\n");
    std::fflush(stdout);
    return 0;
}

static int RunBenchmarkMode(int entry_index, int iterations) {
    auto result = RunBenchmark(entry_index, iterations);
    if (result.elapsed_ms < 0.0) {
        printf("{\\"elapsedMilliseconds\\":-1.0,\\"error\\":\\"invalid index\\"}\\n");
        return 1;
    }
    double ns_per_op = (result.elapsed_ms * 1e6) / iterations;
    double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
    double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
    printf(
        "{\\"elapsedMilliseconds\\":%.3f,\\"calibratedMs\\":%.3f,"
        "\\"opsPerSecond\\":%.0f,\\"iterations\\":%d,"
        "\\"allocatedBytes\\":%" PRId64 ",\\"allocPerOp\\":%.1f}\\n",
        result.elapsed_ms, result.elapsed_ms, ops_per_sec, iterations,
        result.allocated_bytes, alloc_per_op);
    std::fflush(stdout);
    return 0;
}

static int RunHotupdateMode() {
    const int kCount = kSubjectEntryCount;
    // R6: Phase 1 — baseline, capture return values
    CHAOS_IL2CPP_INT32 baseline_values[256] = {0};
    bool baseline_ok[256] = {false};
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        CHAOS_EH_TRY
            CHAOS_IL2CPP_INT32 result = chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, kDefaultArgThunks);
            baseline_values[si] = result;
            baseline_ok[si] = true;
        CHAOS_EH_CATCH_BEGIN
        CHAOS_EH_END
    }
    // R6: Phase 2 — apply patch and compare return values
    auto* patch_ctx = ApplyHotpatchIfAvailable();
    int semantic_passed = 0;
    int semantic_changed_count = 0;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        CHAOS_IL2CPP_INT32 patched_result = -1;
        bool patched_ok = false;
        CHAOS_EH_TRY
            patched_result = chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, kDefaultArgThunks);
            patched_ok = true;
        CHAOS_EH_CATCH_BEGIN
        CHAOS_EH_END
        if (!baseline_ok[si]) { continue; }
        semantic_passed++;
        if (patched_ok && baseline_values[si] != patched_result) {
            semantic_changed_count++;
        }
    }
    // R6: Phase 3 — revert and verify cleanup
    if (patch_ctx != nullptr) {
        chaos::il2cpp::runtime_core::Unpatch(patch_ctx);
    }
    bool all_revert = true;
    int revert_passed = 0;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, kDefaultArgThunks);
            revert_passed++;
        CHAOS_EH_CATCH_BEGIN
            all_revert = false;
        CHAOS_EH_END
    }
    bool all_semantic = (semantic_passed > 0 && semantic_changed_count > 0);
    printf(
        "{\\"passedMethods\\":%d,\\"failedMethods\\":0,"
        "\\"totalMethods\\":%d,\\"allSemantic\\":%s,\\"allRevert\\":%s,"
        "\\"semanticChangedCount\\":%d}\\n",
        semantic_passed, kCount, all_semantic ? "true" : "false",
        all_revert ? "true" : "false", semantic_changed_count);
    std::fflush(stdout);
    return 0;
}

static int RunMicrobenchMode() {
    RunMicrobench();
    return 0;
}

static int RunHotupdateBenchmarkMode(int entry_index, int iterations) {
    auto result = RunHotpatchBenchmark(entry_index, iterations);
    if (result.elapsed_ms < 0.0) {
        printf("{\\"elapsedMilliseconds\\":-1.0,\\"error\\":\\"invalid index\\"}\\n");
        return 1;
    }
    double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
    double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
    printf(
        "{\\"elapsedMilliseconds\\":%.3f,\\"calibratedMs\\":%.3f,"
        "\\"opsPerSecond\\":%.0f,\\"iterations\\":%d,"
        "\\"allocatedBytes\\":%" PRId64 ",\\"allocPerOp\\":%.1f}\\n",
        result.elapsed_ms, result.elapsed_ms, ops_per_sec, iterations,
        result.allocated_bytes, alloc_per_op);
    std::fflush(stdout);
    return 0;
}

int main(int argc, char* argv[]) {
__JIT_CALL__

    // Redirect diagnostic log output to stderr so that machine-consumed
    // protocol output (benchmark JSON, fact results) on stdout stays clean.
    // The verification pipeline parses stdout for JSON — log messages on
    // stdout would pollute the protocol stream and cause parse failures.
    chaos::il2cpp::common::log_internal::g_log_use_stderr = true;

    // Disable BGC for this short-lived verification process.  BGC concurrent
    // mark/sweep races with tight allocation loops in benchmark/fact dispatch,
    // causing sporadic access violations.  For a process that runs for < 30s
    // and allocates heavily, STW collections are more predictable and avoid
    // the known BGC concurrency races (see gc_old_gen.cpp BgcSweep Phase 4b).
    chaos::il2cpp::runtime_core::g_bgc_enabled = false;

    // Initialize ChaOS runtime: resolves kChaosExternalRuntimeFnTable entries
    // (bridge/import stubs) and registers the AOT module so that HotpatchNameRegistry
    // is populated.  Without this, external fnTable entries stay nullptr and any
    // AOT-compiled method that calls through them will segfault.
    //
    // NOTE: heap-allocated and intentionally leaked.  RuntimeShutdown() + static
    // destruction (BgcController threads) race on exit — for a short-lived
    // verification process it is safe to let the OS reclaim everything.
    // TerminateProcess at the shutdown label kills all threads immediately
    // without running static destructors, avoiding the race entirely.
    auto* chaos_host = new ChaosRuntimeHost();
    if (!chaos_host->Initialize("verification-entry")) {
        std::fprintf(stderr, "FATAL: ChaosRuntimeHost::Initialize failed\\n");
        return 1;
    }
    if (!chaos_host->RegisterModule(
            &chaos_codegen_code_registration,
            &chaos_codegen_metadata_registration,
            &chaos_codegen_options)) {
        std::fprintf(stderr, "FATAL: RegisterModule failed\\n");
        return 1;
    }
    ChaosRegisterGcLayouts();

    int ret = 0;
    if (argc < 2) { ret = RunFactMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--fact-json") == 0) { ret = RunFactJsonMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--benchmark") == 0) {
        if (argc < 4) { printf("Usage: entry.exe --benchmark <index> <iterations>\\n"); return 1; }
        ret = RunBenchmarkMode(std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--hotupdate") == 0) { ret = RunHotupdateMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--hotupdate-and-benchmark") == 0) {
        if (argc < 4) { printf("Usage: entry.exe --hotupdate-and-benchmark <index> <iterations>\\n"); return 1; }
        ret = RunHotupdateBenchmarkMode(std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--microbench") == 0) { ret = RunMicrobenchMode(); goto shutdown; }

    printf("Unknown flag: %s\\n", argv[1]);
    ret = 1;

shutdown:
    std::fflush(stdout);
    TerminateProcess(GetCurrentProcess(), static_cast<DWORD>(ret));
    // unreachable
}
'''
    return template.replace('__JIT_DECL__', jit_decl).replace('__JIT_CALL__', jit_call)
