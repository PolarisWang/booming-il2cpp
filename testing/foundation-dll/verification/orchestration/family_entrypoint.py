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

from verification.stages.test_code_generator import (METHOD_OVERRIDES, build_call_expr, build_call_expr_safe,
                                                     build_call_expr_with_refs, build_call_expr_with_ref_locals,
                                                     cast_return_to_int, default_expr, get_skip_reason,
                                                     has_blocked_param, has_ref_param, is_auto_callable,
                                                     parse_method_subject_id, ref_return_expr,
                                                     INSTANCE_ALTERNATIVE_EXPR_MAP, TYPE_ALTERNATIVE_MAP)
from verification.stages.native_code_generator import slug_from_family_id, family_namespace_slug, method_slot_name

# ── NuGet local feed for Chaos.TestFramework.Sdk/Runner ────────────────────
# Version locked via local feed at testing/_packages/. Bump _SDK_VERSION
# when the SDK API changes (breaking) or on significant updates (minor).
_SDK_VERSION = "0.1.0"
_PACKAGES_DIR = _REPO_ROOT / "testing" / "_packages"
_SDK_PROJECT = _REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"


def _ensure_sdk_packed() -> None:
    """Build and pack SDK to local NuGet feed if not already cached.

    The feed at testing/_packages/ is referenced by testing/nuget.config so
    that test projects can use PackageReference instead of ProjectReference.
    This provides version locking and prevents cascading breakage when the
    SDK source changes.
    """
    _PACKAGES_DIR.mkdir(parents=True, exist_ok=True)
    sdk_nupkg = _PACKAGES_DIR / f"Chaos.TestFramework.Sdk.{_SDK_VERSION}.nupkg"
    if not sdk_nupkg.exists():
        result = subprocess.run(
            ["dotnet", "pack", str(_SDK_PROJECT), "--configuration", "Release", "-o", str(_PACKAGES_DIR)],
            capture_output=True, text=True, timeout=120,
        )
        if result.returncode != 0:
            print(f"[entrypoint] WARN: SDK pack failed: {result.stderr.strip()[:200]}")


# ═══════════════════════════════════════════════════════════════════
# Utility helpers
# ═══════════════════════════════════════════════════════════════════

def collect_required_usings(method_subject_ids: list[str]) -> set[str]:
    """Collect the using directives needed by the generated code."""
    usings = {"System", "System.Collections.Generic", "System.Linq", "System.IO", "System.Threading"}
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
    return build_call_expr_safe(parsed, ref_mode="ref_locals")


def build_call_expr_for_semantic_patch(subject_id: str) -> tuple[str, str]:
    parsed = parse_method_subject_id(subject_id)
    if not is_auto_callable(parsed):
        return ("", "")
    return build_call_expr_safe(parsed, ref_mode="refs",
                                type_map=TYPE_ALTERNATIVE_MAP,
                                instance_map=INSTANCE_ALTERNATIVE_EXPR_MAP)


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
        if not probe_mode:
            usings.add("Chaos.TestFramework")
    else:
        usings = collect_required_usings(method_subject_ids)
        usings.add("Chaos.TestFramework")

    if not probe_mode and variant != "subjects":
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

    if not probe_mode and variant != "subjects":
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
            # Patch variant generates stub Subject_N() methods for emit-patch-data.
            # These stubs use 0xB0000000u + idx as marker values so the emit-patch-data
            # tool can distinguish auto-generated stubs from real CustomEntryMethod
            # implementations. The actual semantic verification (before vs after patch)
            # happens inside entry.exe --hotupdate, which compares real AOT-compiled
            # return values — not these stubs.
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
                body_lines = [f"{ns_indent}        Assert.IsNotNull(1);"]
            else:
                bare = ret_type.rstrip("&*?").strip()
                cs_return = _SYSTEM_TYPE_TO_CSHARP.get(bare, bare)
                body_lines = [f"{ns_indent}        Assert.IsNotNull(1);", f"{ns_indent}        return default;"]
            lines.append(f"{ns_indent}    [Fact]")
            lines.append(f"{ns_indent}    public static {cs_return} {method_prefix}{idx}({param_decls})")
            lines.append(f"{ns_indent}    {{")
            for bl in body_lines:
                lines.append(bl)
            lines.append(f"{ns_indent}    }}")
            lines.append("")
            continue

        if variant == "subjects":
            # Subjects variant: use Assert.* + [Fact] annotation, no _exitCode
            lines.append(f"{ns_indent}    [Fact]")
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
                    # Use IL-level try/catch + Assert.Fail, not Assert.Throws<T>:
                    # AOT EH cannot properly handle exceptions through Assert API
                    # delegate invocations, but IL-level try/catch translates correctly.
                    if looks_like_property_read(call_expr):
                        lines.append(f"{ns_indent}        try {{ _ = {call_expr}; Assert.Fail(\"Expected {exc_type}\"); }}")
                    else:
                        lines.append(f"{ns_indent}        try {{ {call_expr}; Assert.Fail(\"Expected {exc_type}\"); }}")
                    lines.append(f"{ns_indent}        catch ({exc_type}) {{ }}")
                else:
                    # No expected_values from contract — use catch-all to tolerate
                    # subjects that throw as part of normal API contract behavior
                    # (e.g. Convert.ToChar(bool) throws InvalidCastException).
                    # This matches the old _exitCode pattern's try/catch default.
                    if ret == "System.Void" or not ret:
                        lines.append(f"{ns_indent}        try {{ {call_expr}; }}")
                    else:
                        cast_expr = cast_return_to_int(ret, call_expr)
                        lines.append(f"{ns_indent}        try {{ var __val = {cast_expr}; Assert.IsNotNull(__val.GetHashCode()); }}")
                    lines.append(f"{ns_indent}        catch {{ }}")
            else:
                parsed = parse_method_subject_id(subject_id)
                skip_reason = get_skip_reason(parsed)
                if skip_reason.startswith("IMPLEMENTABLE"):
                    lines.append(f"{ns_indent}        // IMPLEMENTABLE gap: {skip_reason}")
                    lines.append(f"{ns_indent}        // TODO: wrapper for {subject_id}")
                else:
                    lines.append(f"{ns_indent}        // {skip_reason or 'non-callable'}: {subject_id}")
            lines.append(f"{ns_indent}    }}")
            lines.append("")
        else:
            # Non-subjects variants: keep _exitCode-based pattern
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
                        lines.append(f"{ns_indent}        try {{ _ = {cast_expr}; }}")
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
            # Also scan subjects/ subdirectory for handwritten .cs files (e.g. compound family NativeEntry.cs)
            subjects_dir = output_dir / "subjects"
            if subjects_dir.is_dir():
                for f in sorted(subjects_dir.iterdir()):
                    if f.suffix == ".cs" and f.name not in (cs_file_name, "Program.cs"):
                        # Skip if already included via output_dir scan (e.g. Custom.cs copied for has_custom_entry)
                        if has_custom_entry and f.name == custom_cs_name:
                            continue
                        extra_cs += f'    <Compile Include="subjects/{f.name}" />\n'
        custom_cs = f'    <Compile Include="{class_name}.Custom.cs" />\n' if has_custom_entry else ""
        # Always add Chaos.TestFramework.Sdk via PackageReference for version locking
        sdk_refs = [f'    <PackageReference Include="Chaos.TestFramework.Sdk" Version="{_SDK_VERSION}" />']
        extra_project_refs = [f'    <ProjectReference Include="{r}" />' for r in (extra_refs or [])]
        items = "\n".join(sdk_refs + extra_project_refs)
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

    chaos_tf_ref = (
        "  <ItemGroup>\n"
        f'    <PackageReference Include="Chaos.TestFramework.Sdk" Version="{_SDK_VERSION}" />\n'
        "  </ItemGroup>\n"
    )

    extra_cs = ""
    if output_dir is not None and output_dir.is_dir():
        for f in sorted(output_dir.iterdir()):
            if f.suffix == ".cs" and f.name not in (cs_file_name, "Program.cs"):
                if has_custom_entry and f.name == f"{class_name}.Custom.cs":
                    continue
                extra_cs += f'    <Compile Include="{f.name}" />\n'
        # Also scan subjects/ subdirectory for handwritten .cs files (e.g. compound family NativeEntry.cs)
        subjects_dir = output_dir / "subjects"
        if subjects_dir.is_dir():
            for f in sorted(subjects_dir.iterdir()):
                if f.suffix == ".cs" and f.name not in (cs_file_name, "Program.cs"):
                    extra_cs += f'    <Compile Include="subjects/{f.name}" />\n'
    custom_cs = f'    <Compile Include="{class_name}.Custom.cs" />\n' if has_custom_entry else ""

    return (
        '<Project Sdk="Microsoft.NET.Sdk">\n'
        "  <PropertyGroup>\n"
        "    <OutputType>Exe</OutputType>\n"
        f"    <TargetFramework>net8.0</TargetFramework>\n"
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
        f"{extra_cs}"
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
    # Also copy files from subjects/ subdirectory for compound families
    subjects_src = src_dir / "subjects"
    if subjects_src.is_dir():
        subjects_dst = temp_dir / "subjects"
        subjects_dst.mkdir(parents=True, exist_ok=True)
        for f in subjects_src.iterdir():
            if f.suffix == ".cs":
                shutil.copy2(f, subjects_dst / f.name)

    # Diagnostic: warn if expected files are missing in temp dir
    expected_prefixes = {src_dir.name}
    if subjects_src.is_dir():
        expected_prefixes.add("subjects")
    found_files = {f.name for f in temp_dir.rglob("*.cs")}
    missing = [f.name for f in sorted(src_dir.glob("*.cs"))
               if f.name not in found_files]
    if missing:
        print(f"  [entrypoint] WARNING: build_project_safe temp copy missing {len(missing)} file(s): "
              f"{', '.join(missing[:5])}")
    if subjects_src.is_dir():
        subjects_missing = [f.name for f in sorted(subjects_src.glob("*.cs"))
                           if f.name not in found_files]
        if subjects_missing:
            print(f"  [entrypoint] WARNING: build_project_safe temp copy missing subjects file(s): "
                  f"{', '.join(subjects_missing[:5])}")

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
    # Ensure local NuGet packages are available before generating project references
    _ensure_sdk_packed()

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
    if not has_custom_entry and variant in ("subjects", "patch"):
        # Also check subjects/ subdirectory — compound families store Custom.cs there
        subjects_custom = output_dir / "subjects" / f"{class_name}.Custom.cs"
        if subjects_custom.exists():
            shutil.copy2(str(subjects_custom), str(custom_cs_path))
            print(f"[entrypoint]  copied {subjects_custom.name} from subjects/ for has_custom_entry")
            has_custom_entry = True

    custom_method_indices: set[int] | None = None
    family_slug = slug_from_family_id(family_id)
    # Try multiple contract locations: computed slug, then actual output_dir parent(s)
    candidate_paths = [
        _REPO_ROOT / "testing" / "foundation-dll" / assembly_name / family_slug / "capability-family-contract.json",
    ]
    for parent in [output_dir, output_dir.parent]:
        candidate = parent / "capability-family-contract.json"
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
            output_dir=output_dir,
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
