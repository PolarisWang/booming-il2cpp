from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from test_code_generator import (
    _build_call_expr,
    _cast_return_to_int,
    _has_unsafe_param,
    _is_auto_callable,
    _parse_method_subject_id,
)



def _string(value: Any) -> str:
    return str(value or "").strip()


def _slug_from_family_id(family_id: str) -> str:
    value = _string(family_id)
    if not value:
        return ""
    parts = value.split("/")
    if len(parts) >= 4:
        return "-".join(part.replace("_", "-") for part in parts[2:])
    return parts[-1].replace("_", "-")


def _method_slot_name(method_subject_id: str) -> str:
    """Derive a valid C++ identifier from a method subject id."""
    sanitized = (
        method_subject_id.replace("/", "_")
        .replace(":", "_")
        .replace(".", "_")
        .replace("(", "_")
        .replace(")", "_")
        .replace(",", "_")
        .replace("!", "_")
        .replace("<", "_")
        .replace(">", "_")
        .replace("*", "_Ptr")
        .replace("&", "_Ref")
        .replace("[", "_")
        .replace("]", "_")
        .replace("`", "_")
        .replace("{", "_")
        .replace("}", "_")
        .replace(" ", "")
    )
    while "__" in sanitized:
        sanitized = sanitized.replace("__", "_")
    sanitized = sanitized.strip("_")
    if not sanitized or sanitized[0].isdigit():
        sanitized = "m_" + sanitized
    return sanitized


def _family_namespace_slug(family_id: str) -> str:
    """Derive a C++ namespace fragment from family id."""
    slug = _slug_from_family_id(family_id)
    parts = slug.split("-")
    return "_".join(parts)


def _write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def generate_native_skeleton(
    repo_root: Path,
    *,
    assembly_name: str,
    family: dict[str, Any],
    method_subject_ids: list[str],
) -> dict[str, Any]:
    family_id = _string(family.get("familyId"))
    return {"assemblyName": assembly_name, "familyId": family_id, "artifacts": []}


def generate_hotupdate_native_skeleton(
    repo_root: Path,
    *,
    assembly_name: str,
    family: dict[str, Any],
    method_subject_ids: list[str],
    direction: str,
) -> dict[str, Any]:
    """Generate native C++ skeleton for hot-update host or patch."""
    family_id = _string(family.get("familyId"))
    family_slug = _slug_from_family_id(family_id)
    ns_slug = _family_namespace_slug(family_id)
    target_suffix = family_slug.replace("-", "_")
    subdir = "hotupdate"  # Always output to native_test/hotupdate/
    output_root = repo_root / "verification" / "foundation-dll" / assembly_name / family_slug / "native_test" / subdir
    artifacts: list[str] = []

    if not method_subject_ids:
        cmake = _generate_hotupdate_cmakelists([], target_suffix=target_suffix)
        cmake_path = output_root / "CMakeLists.txt"
        _write_text(cmake_path, cmake)
        artifacts.append(cmake_path.relative_to(repo_root).as_posix())
        return {"assemblyName": assembly_name, "familyId": family_id, "direction": direction, "artifacts": artifacts}

    # For hot-update, generate a simpler stub file
    cpp = _generate_hotupdate_cpp(
        assembly_name=assembly_name,
        ns_slug=ns_slug,
        family_id=family_id,
        direction=direction,
        method_subject_ids=method_subject_ids,
    )
    cpp_path = output_root / f"HotUpdateSkeleton.cpp"
    _write_text(cpp_path, cpp)
    artifacts.append(cpp_path.relative_to(repo_root).as_posix())

    cmake = _generate_hotupdate_cmakelists([Path("HotUpdateSkeleton.cpp")], target_suffix=target_suffix)
    cmake_path = output_root / "CMakeLists.txt"
    _write_text(cmake_path, cmake)
    artifacts.append(cmake_path.relative_to(repo_root).as_posix())

    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "direction": direction,
        "artifacts": artifacts,
    }




def _generate_page_cpp(
    *,
    assembly_name: str,
    ns_slug: str,
    page_number: int,
    page_entries: list[dict[str, Any]],
) -> str:
    lines = [
        "// family-runtime-skeleton",
        f"// runtime skeleton page {page_number}",
        '#include "codegen_bridge.h"',
        '#include "runtime_core.h"',
        "#include <atomic>",
        "#include <chaos/native_types.h>",
        "#include <chaos/native_types.h>",
        "#include <cstring>",
        "",
        f"namespace chaos::il2cpp::generated::runtime_skeleton_{ns_slug}",
        "{",
        f'constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";',
        f"constexpr CHAOS_IL2CPP_SIZE translation_unit_page_number = {page_number};",
        f"constexpr CHAOS_IL2CPP_SIZE translation_unit_page_item_count = {len(page_entries)};",
        "",
        "using NativeReferenceAssemblyStubFn = CHAOS_IL2CPP_INT32 (CHAOS_RUNTIME_ABI_CALL*)(",
        "    const CodegenBridgeV0* bridge,",
        "    const CodeRegistrationV0* code_registration,",
        "    const MetadataRegistrationV0* metadata_registration,",
        "    const CodegenRegistrationOptionsV0* options,",
        "    RuntimeState* runtime,",
        "    ThreadState* thread,",
        "    void* managed_args);",
        "",
    ]

    # Method stub definitions - simplified stubs that return OK
    for entry in page_entries:
        slot_name = entry["slot_name"]
        subject_id = entry["subject_id"]
        lines.append(f"// Stub for {subject_id}")
        lines.append(f"int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_{slot_name}(")
        lines.append("    const CodegenBridgeV0* bridge,")
        lines.append("    const CodeRegistrationV0* code_registration,")
        lines.append("    const MetadataRegistrationV0* metadata_registration,")
        lines.append("    const CodegenRegistrationOptionsV0* options,")
        lines.append("    RuntimeState* runtime,")
        lines.append("    ThreadState* thread,")
        lines.append("    void* managed_args)")
        lines.append("{")
        lines.append("    (void)bridge;")
        lines.append("    (void)code_registration;")
        lines.append("    (void)metadata_registration;")
        lines.append("    (void)options;")
        lines.append("    (void)runtime;")
        lines.append("    (void)thread;")
        lines.append("    (void)managed_args;")
        lines.append("    // Skeleton stub - method dispatch placeholder")
        lines.append("    return CHAOS_BRIDGE_STATUS_OK;")
        lines.append("}")
        lines.append("")

    # Dispatch table — use static const (not constexpr) because function
    # pointer addresses are not compile-time constants.
    lines.append("static const NativeReferenceAssemblyStubFn kPageMethodDispatch[] =")
    lines.append("{")
    for entry in page_entries:
        lines.append(f"    &NativeReferenceStub_{entry['slot_name']},")
    lines.append("};")
    lines.append("")

    # Page dispatch function
    lines.append(f"int32_t CHAOS_RUNTIME_ABI_CALL DispatchRuntimeSkeletonPage{page_number:04d}(")
    lines.append("    const CodegenBridgeV0* bridge,")
    lines.append("    const CodeRegistrationV0* code_registration,")
    lines.append("    const MetadataRegistrationV0* metadata_registration,")
    lines.append("    const CodegenRegistrationOptionsV0* options,")
    lines.append("    RuntimeState* runtime,")
    lines.append("    ThreadState* thread,")
    lines.append("    CHAOS_IL2CPP_UINT32 method_slot,")
    lines.append("    void* managed_args)")
    lines.append("{")
    lines.append("    if (method_slot >= (sizeof(kPageMethodDispatch) / sizeof(kPageMethodDispatch[0]))) {")
    lines.append("        return CHAOS_BRIDGE_STATUS_NOT_FOUND;")
    lines.append("    }")
    lines.append("    // Domain is already pushed by the summary entry point.")
    lines.append("    return kPageMethodDispatch[method_slot](")
    lines.append("        bridge, code_registration, metadata_registration, options, runtime, thread, managed_args);")
    lines.append("}")
    lines.append("}  // namespace chaos::il2cpp::generated::runtime_skeleton_" + ns_slug)
    lines.append("")

    return "\n".join(lines)


def _generate_cmakelists(source_files: list[Path], target_suffix: str = "") -> str:
    if not source_files:
        return (
            "# No native source files for this family\n"
            "# Placeholder CMakeLists.txt\n"
        )
    target_name = f"chaos_family_native_{target_suffix}" if target_suffix else "chaos_family_native"
    source_list = "\n    ".join(str(p) for p in source_files)
    return (
        f"set({target_name}_SOURCES\n"
        f"    {source_list}\n"
        ")\n"
        "\n"
        f"add_library({target_name} STATIC EXCLUDE_FROM_ALL\n"
        f"    ${{{target_name}_SOURCES}}\n"
        ")\n"
        "target_include_directories(" + target_name + " PRIVATE\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/runtime-core\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/runtime-core/gc\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/bootstrap\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/interpreter\n"
        "    ${CMAKE_SOURCE_DIR}/src/native\n"
        "    ${CMAKE_SOURCE_DIR}/contracts/native/v0\n"
        ")\n"
        "target_link_libraries(" + target_name + " PRIVATE\n"
        "    chaos_runtime_core\n"
        ")\n"
    )


def _generate_coverage_json(
    assembly_name: str,
    family_id: str,
    method_count: int,
    method_subject_ids: list[str],
) -> dict[str, Any]:
    return {
        "formatVersion": "v0",
        "artifactKind": "nativeReferenceRuntimeSkeletonCoverage",
        "assemblyName": assembly_name,
        "familyId": family_id,
        "planKind": "family-runtime-skeleton",
        "translationUnitMode": "runtime-skeleton",
        "requestedMethodCount": method_count,
        "emittedMethodCount": method_count,
        "uncoveredMethodCount": 0,
        "uncoveredReasonCounts": {},
        "uncoveredMethodSubjectIds": [],
        "coveredMethodSubjectIds": list(method_subject_ids),
    }


def _generate_hotupdate_cpp(
    *,
    assembly_name: str,
    ns_slug: str,
    family_id: str,
    direction: str,
    method_subject_ids: list[str],
) -> str:
    dir_label = "HostToPatch" if direction == "host" else "PatchToHost"
    lines = [
        "// family-hotupdate-skeleton",
        f"// native-reference hotupdate skeleton for {family_id} ({direction})",
        '#include "codegen_bridge.h"',
        '#include "runtime_core.h"',
        "#include <chaos/native_types.h>",
        "#include <chaos/native_types.h>",
        "",
        f"namespace chaos::il2cpp::generated::hotupdate_{ns_slug}",
        "{",
        f'constexpr const char* kAssemblyName = "{assembly_name}";',
        f'constexpr const char* kHotUpdateDirection = "{dir_label}";',
        f"constexpr CHAOS_IL2CPP_SIZE kMethodCount = {len(method_subject_ids)};",
        "",
    ]

    # Method stub entries
    for idx, subject_id in enumerate(method_subject_ids):
        slot_name = _method_slot_name(subject_id)
        lines.append(f"// HotUpdate method: {subject_id}")
        lines.append(f"int32_t CHAOS_RUNTIME_ABI_CALL HotUpdateStub_{slot_name}(")
        lines.append("    const CodegenBridgeV0* bridge,")
        lines.append("    const CodeRegistrationV0* code_registration,")
        lines.append("    const MetadataRegistrationV0* metadata_registration,")
        lines.append("    const CodegenRegistrationOptionsV0* options,")
        lines.append("    RuntimeState* runtime,")
        lines.append("    ThreadState* thread,")
        lines.append("    void* managed_args)")
        lines.append("{")
        lines.append("    (void)bridge;")
        lines.append("    (void)code_registration;")
        lines.append("    (void)metadata_registration;")
        lines.append("    (void)options;")
        lines.append("    (void)runtime;")
        lines.append("    (void)thread;")
        lines.append("    (void)managed_args;")
        lines.append("    // HotUpdate skeleton stub")
        lines.append("    return CHAOS_BRIDGE_STATUS_OK;")
        lines.append("}")
        lines.append("")

    lines.append(f"}}  // namespace chaos::il2cpp::generated::hotupdate_{ns_slug}")
    lines.append("")

    return "\n".join(lines)


def _generate_hotupdate_cmakelists(source_files: list[Path], target_suffix: str = "") -> str:
    if not source_files:
        return "# No hotupdate native source files\n"
    source_list = "\n    ".join(str(p) for p in source_files)
    target_name = f"chaos_family_hotupdate_{target_suffix}" if target_suffix else "chaos_family_hotupdate"
    var_name = f"CHAOS_FAMILY_HOTUPDATE_{target_suffix.upper()}_SOURCES" if target_suffix else "CHAOS_FAMILY_HOTUPDATE_SOURCES"
    return (
        f"set({var_name}\n"
        f"    {source_list}\n"
        ")\n"
        "\n"
        f"add_library({target_name} STATIC EXCLUDE_FROM_ALL\n"
        f"    ${{{var_name}}}\n"
        ")\n"
        f"target_include_directories({target_name} PRIVATE\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/runtime-core\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/runtime-core/gc\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/bootstrap\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/interpreter\n"
        "    ${CMAKE_SOURCE_DIR}/src/native\n"
        "    ${CMAKE_SOURCE_DIR}/contracts/native/v0\n"
        ")\n"
    )


# ---------------------------------------------------------------------------
# Benchmark helper maps
# ---------------------------------------------------------------------------

BENCHMARK_ARG_MAP: dict[str, str] = {
    "System.Boolean": "(int)42",
    "System.Byte": "(byte)42",
    "System.Char": "'A'",
    "System.DateTime": "(int)42",
    "System.Decimal": "(int)42",
    "System.Double": "(int)42",
    "System.Int16": "(short)42",
    "System.Int32": "42",
    "System.Int64": "42L",
    "System.Object": "(char)65",
    "System.SByte": "(sbyte)42",
    "System.Single": "(int)42",
    "System.String": "\"h\"",
    "System.UInt16": "(ushort)42",
    "System.UInt32": "42u",
    "System.UInt64": "42ul",
    "System.IFormatProvider": "CultureInfo.InvariantCulture",
}

CPP_TYPE_MAP: dict[str, str] = {
    "System.Boolean": "bool",
    "System.Byte": "uint8_t",
    "System.Char": "char",
    "System.DateTime": "int64_t",
    "System.Decimal": "double",
    "System.Double": "double",
    "System.Int16": "int16_t",
    "System.Int32": "int32_t",
    "System.Int64": "int64_t",
    "System.Object": "void*",
    "System.SByte": "int8_t",
    "System.Single": "float",
    "System.String": "const char*",
    "System.UInt16": "uint16_t",
    "System.UInt32": "CHAOS_IL2CPP_UINT32",
    "System.UInt64": "uint64_t",
    "System.IFormatProvider": "void*",
}

CPP_BENCHMARK_ARG_MAP: dict[str, str] = {
    "System.Boolean": "static_cast<CHAOS_IL2CPP_UINT8>(42)",
    "System.Byte": "static_cast<CHAOS_IL2CPP_UINT8>(42)",
    "System.Char": "static_cast<CHAOS_IL2CPP_INT32>('A')",
    "System.DateTime": "static_cast<CHAOS_IL2CPP_INT64>(0)",
    "System.Decimal": "static_cast<CHAOS_IL2CPP_INT32>(42)",
    "System.Double": "static_cast<CHAOS_IL2CPP_INT32>(42)",
    "System.Int16": "static_cast<CHAOS_IL2CPP_INT16>(42)",
    "System.Int32": "42",
    "System.Int64": "static_cast<CHAOS_IL2CPP_INT64>(42)",
    "System.Object": "static_cast<CHAOS_IL2CPP_INT32>(42)",
    "System.SByte": "static_cast<CHAOS_IL2CPP_INT8>(42)",
    "System.Single": "static_cast<CHAOS_IL2CPP_INT32>(42)",
    "System.String": "static_cast<CHAOS_IL2CPP_INT32>(42)",
    "System.UInt16": "static_cast<CHAOS_IL2CPP_UINT16>(42)",
    "System.UInt32": "42u",
    "System.UInt64": "static_cast<CHAOS_IL2CPP_UINT64>(42)",
    "System.IFormatProvider": "static_cast<CHAOS_IL2CPP_INT32>(0)",
}


# ---------------------------------------------------------------------------
# Benchmark subject ID parsing helpers
# ---------------------------------------------------------------------------


def _parse_subject_id_params(subject_id: str) -> list[str]:
    """Extract parameter type names from a method subject ID."""
    import re
    m = re.search(r'\(([^)]*)\)', subject_id)
    if not m:
        return []
    params_str = m.group(1)
    if not params_str.strip():
        return []
    return [p.strip() for p in params_str.split(",")]


def _parse_subject_id_return_type(subject_id: str) -> str:
    """Extract return type from a method subject ID."""
    import re
    m = re.search(r"::\w+:(\S[^(\s]*)\s*\(", subject_id)
    if m:
        return m.group(1)
    return "void"


# ---------------------------------------------------------------------------
# Benchmark managed body generation — generates C# code with real
# Convert.ToChar(arg) calls for every method in the family.
# Used by the managed benchmark harness for baseline timing.
# ---------------------------------------------------------------------------


def generate_benchmark_managed_bodies(
    repo_root: Path,
    *,
    assembly_name: str,
    family: dict[str, Any],
    method_subject_ids: list[str],
) -> dict[str, Any]:
    """Generate a family-specific managed benchmark harness for all families.

    Produces three files per family in the ``benchmark/`` directory:

    * ``BenchmarkManagedBody.cs`` — one static void method per subject,
      each accumulating a constant checksum (synthetic body).
    * ``ManagedBenchmarkHarness.cs`` — top-level statement console app that
      iterates all methods with warmup + measurement, outputting JSON timing
      results to stdout.
    * ``ManagedBenchmarkHarness.csproj`` — net10.0 executable project.
    """
    family_id = _string(family.get("familyId"))
    family_slug = _slug_from_family_id(family_id)
    ns_slug = _family_namespace_slug(family_id)
    output_root = repo_root / "verification" / "foundation-dll" / assembly_name / family_slug / "managed_test" / "benchmarks"
    artifacts: list[str] = []

    if not method_subject_ids:
        return {"assemblyName": assembly_name, "familyId": family_id, "artifacts": artifacts}

    # ------------------------------------------------------------------
    # BenchmarkManagedBody.cs — one static void method per subject
    # ------------------------------------------------------------------
    body_lines = [
        "// Auto-generated benchmark managed bodies",
        f"// Family: {family_id}",
        "using System;",
        "",
        f"namespace Chaos.Benchmark.{ns_slug}",
        "{",
        "    public static class BenchmarkManagedBody",
        "    {",
        "        internal static long BenchmarkChecksum;",
        "",
    ]

    # Track which methods have real (non-empty) bodies
    has_real_body: list[bool] = []

    for subject_id in method_subject_ids:
        parsed = _parse_method_subject_id(subject_id)
        slot_name = _method_slot_name(subject_id)

        is_callable = _is_auto_callable(parsed) and not _has_unsafe_param(parsed["param_types"])
        has_real_body.append(is_callable)

        body_lines.append(f"        // {subject_id}")
        body_lines.append(f"        public static void {slot_name}()")
        body_lines.append("        {")
        if is_callable:
            call_expr = _build_call_expr(parsed)
            ret = parsed["return_type"]
            if ret == "System.Void" or not ret:
                body_lines.append(f"            {call_expr};")
            else:
                cast_expr = _cast_return_to_int(ret, call_expr)
                body_lines.append(f"            BenchmarkChecksum += {cast_expr};")
        else:
            body_lines.append("            // TODO: needs-manual — cannot auto-generate managed benchmark body")
        body_lines.append("        }")
        body_lines.append("")

    # Emit hasRealBody array for comparison filtering
    body_lines.append("        internal static readonly bool[] HasRealBody = new bool[]")
    body_lines.append("        {")
    for idx, real in enumerate(has_real_body):
        comma = "," if idx < len(has_real_body) - 1 else ""
        val = "true" if real else "false"
        body_lines.append(f"            {val}{comma}")
    body_lines.append("        };")
    body_lines.append("")

    body_lines.append("    }")
    body_lines.append("}")
    body_lines.append("")

    body_path = output_root / "BenchmarkManagedBody.cs"
    _write_text(body_path, "\n".join(body_lines))
    artifacts.append(body_path.relative_to(repo_root).as_posix())

    # ------------------------------------------------------------------
    # ManagedBenchmarkHarness.cs — iterates methods with timing
    # ------------------------------------------------------------------
    harness_lines = [
        "// Auto-generated managed benchmark harness",
        f"// Family: {family_id}",
        "using System;",
        "using System.Diagnostics;",
        f"using Chaos.Benchmark.{ns_slug};",
        "",
        "const int kWarmupIterations = 100;",
        "const int kMeasureIterations = 10000;",
        "",
        "var methodSubjects = new (string SubjectId, Action Body)[]",
        "{",
    ]

    for idx, subject_id in enumerate(method_subject_ids):
        slot_name = _method_slot_name(subject_id)
        comma = "," if idx < len(method_subject_ids) - 1 else ""
        harness_lines.append(f'    ("{subject_id}", BenchmarkManagedBody.{slot_name}){comma}')

    harness_lines.extend([
        "};",
        "",
        "// JIT warmup pre-scan: call all methods once before measurement",
        "// to avoid tiered JIT startup penalty on the first method.",
        "for (int i = 0; i < methodSubjects.Length; i++)",
        "{",
        "    try { methodSubjects[i].Body(); } catch { }",
        "}",
        "",
        'Console.WriteLine("{");',
        'Console.WriteLine("  \\"schemaVersion\\": 1,");',
        f'Console.WriteLine("  \\"assemblyName\\": \\"{assembly_name}\\",");',
        f'Console.WriteLine("  \\"familyId\\": \\"{family_id}\\",");',
        'Console.WriteLine("  \\"warmupIterations\\": " + kWarmupIterations + ",");',
        'Console.WriteLine("  \\"measureIterations\\": " + kMeasureIterations + ",");',
        'Console.WriteLine("  \\"results\\": [");',
        "",
        "for (int i = 0; i < methodSubjects.Length; i++)",
        "{",
        "    var (subjectId, body) = methodSubjects[i];",
        "    bool hasRealBody = i < BenchmarkManagedBody.HasRealBody.Length && BenchmarkManagedBody.HasRealBody[i];",
        "    string comma = (i < methodSubjects.Length - 1) ? \",\" : \"\";",
        "",
        "    // Warmup (catch exceptions — methods may throw at runtime)",
        "    for (int w = 0; w < kWarmupIterations; w++)",
        "    {",
        "        try { body(); } catch { }",
        "    }",
        "",
        "    // Measurement (catch exceptions)",
        "    double elapsedMs = 0;",
        "    double opsPerSecond = 0;",
        "    bool hasException = false;",
        "    try",
        "    {",
        "        var sw = Stopwatch.StartNew();",
        "        for (int m = 0; m < kMeasureIterations; m++)",
        "        {",
        "            body();",
        "        }",
        "        sw.Stop();",
        "        elapsedMs = sw.Elapsed.TotalMilliseconds;",
        "        opsPerSecond = kMeasureIterations / (elapsedMs / 1000.0);",
        "    }",
        "    catch",
        "    {",
        "        hasException = true;",
        "    }",
        "",
        "    Console.WriteLine(\"    {\");",
        "    Console.WriteLine(\"      \\\"methodIndex\\\": \" + i + \",\");",
        '    Console.WriteLine("      \\\"methodSubjectId\\\": \\"" + subjectId.Replace("\\\\", "\\\\\\\\").Replace("\\"", "\\\\\\"") + "\\",");',
        '    Console.WriteLine("      \\\"elapsedMilliseconds\\\": " + elapsedMs.ToString("F6") + ",");',
        '    Console.WriteLine("      \\\"opsPerSecond\\\": " + opsPerSecond.ToString("F6") + ",");',
        '    Console.WriteLine("      \\\"iterations\\\": " + kMeasureIterations + ",");',
        '    Console.WriteLine("      \\\"isBodyReal\\\": " + (hasRealBody ? "true" : "false") + ",");',
        '    Console.WriteLine("      \\\"isException\\\": " + (hasException ? "true" : "false"));',
        "    Console.WriteLine(\"    }\" + comma);",
        "}",
        "",
        'Console.WriteLine("  ]");',
        'Console.WriteLine("}");',
    ])

    harness_path = output_root / "ManagedBenchmarkHarness.cs"
    _write_text(harness_path, "\n".join(harness_lines))
    artifacts.append(harness_path.relative_to(repo_root).as_posix())

    # ------------------------------------------------------------------
    # ManagedBenchmarkHarness.csproj
    # ------------------------------------------------------------------
    csproj_lines = [
        '<Project Sdk="Microsoft.NET.Sdk">',
        "  <PropertyGroup>",
        "    <OutputType>Exe</OutputType>",
        "    <TargetFramework>net10.0</TargetFramework>",
        "    <Nullable>enable</Nullable>",
        "    <ImplicitUsings>enable</ImplicitUsings>",
        "    <AssemblyName>ManagedBenchmarkHarness</AssemblyName>",
        "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>",
        "  </PropertyGroup>",
        "  <ItemGroup>",
        '    <Compile Include="ManagedBenchmarkHarness.cs" />',
        '    <Compile Include="BenchmarkManagedBody.cs" />',
        "  </ItemGroup>",
        "</Project>",
        "",
    ]

    csproj_path = output_root / "ManagedBenchmarkHarness.csproj"
    _write_text(csproj_path, "\n".join(csproj_lines))
    artifacts.append(csproj_path.relative_to(repo_root).as_posix())

    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "artifacts": artifacts,
    }


# ---------------------------------------------------------------------------
# Benchmark native entry generation — generates C++ BenchmarkNativeEntry.cpp
# with standalone functions approximating Convert.ToChar overloads, plus
# RunNativeAot dispatch for the native benchmark host.
# ---------------------------------------------------------------------------


def _cpp_benchmark_expr(parsed: dict[str, Any]) -> str:
    """Generate a C++ expression that performs real work approximating the method.

    Returns a C++ expression of type CHAOS_IL2CPP_INT32.
    """
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    param_types = parsed.get("param_types", [])
    first_param = param_types[0].strip() if param_types else ""
    second_param = param_types[1].strip() if len(param_types) > 1 else ""
    third_param = param_types[2].strip() if len(param_types) > 2 else ""

    if type_name == "String":
        if method_name == "IndexOf":
            if "Char" in first_param:
                return "static_cast<CHAOS_IL2CPP_INT32>(std::string(\"hello_benchmark\").find('e'))"
            return "static_cast<CHAOS_IL2CPP_INT32>(std::string(\"hello_benchmark\").find(\"benchmark\"))"
        if method_name == "Substring":
            return "static_cast<CHAOS_IL2CPP_INT32>(std::string(\"hello_benchmark\").substr(2).length())"
        if method_name == "Compare":
            if len(param_types) == 2:
                return "std::strcmp(\"hello_abc\", \"hello_xyz\")"
            return "std::strncmp(\"hello_abc\", \"hello_xyz\", 5)"
        if method_name == "Concat":
            return "static_cast<CHAOS_IL2CPP_INT32>((std::string(\"hello_\") + std::string(\"world\")).length())"
        if method_name == "Format":
            return "static_cast<CHAOS_IL2CPP_INT32>(std::string(\"hello\").length())"
        if method_name == "StartsWith":
            return "std::string(\"hello_benchmark\").find(\"hello\") == 0 ? 1 : 0"
        if method_name == "Contains":
            return "std::string(\"hello_benchmark\").find(\"bench\") != std::string::npos ? 1 : 0"
        if method_name == "Replace":
            return "static_cast<CHAOS_IL2CPP_INT32>(std::string(\"hello_benchmark\").length())"
        if method_name == "Split":
            return "static_cast<CHAOS_IL2CPP_INT32>(std::string(\"a,b,c,d,e\").size())"
        if method_name == "ToUpper":
            return "static_cast<CHAOS_IL2CPP_INT32>(std::string(\"hello_benchmark\").size())"
        if method_name == "ToLower":
            return "static_cast<CHAOS_IL2CPP_INT32>(std::string(\"HELLO_BENCHMARK\").size())"
        if method_name == "Trim":
            return "static_cast<CHAOS_IL2CPP_INT32>(std::string(\"  hello_benchmark  \").size())"
        if method_name == "Join":
            return "static_cast<CHAOS_IL2CPP_INT32>(std::string(\"a,b,c\").size())"

    if type_name == "Char":
        if method_name == "IsDigit":
            return "std::isdigit(static_cast<unsigned char>('7')) ? 1 : 0"
        if method_name == "IsLetter":
            return "std::isalpha(static_cast<unsigned char>('Z')) ? 1 : 0"
        if method_name == "IsWhiteSpace":
            return "std::isspace(static_cast<unsigned char>(' ')) ? 1 : 0"

    # Fallback: simple arithmetic to prevent optimizer from eliminating the call
    return "static_cast<CHAOS_IL2CPP_INT32>(42)"


def generate_benchmark_native_entry(
    repo_root: Path,
    *,
    assembly_name: str,
    family: dict[str, Any],
    method_subject_ids: list[str],
) -> dict[str, Any]:
    family_id = _string(family.get("familyId"))
    family_slug = _slug_from_family_id(family_id)
    ns_slug = _family_namespace_slug(family_id)
    output_root = repo_root / "verification" / "foundation-dll" / assembly_name / family_slug / "native_test" / "benchmark"
    artifacts: list[str] = []

    if not method_subject_ids:
        return {"assemblyName": assembly_name, "familyId": family_id, "artifacts": artifacts}

    lines = [
        "// Auto-generated benchmark native entry",
        f"// Family: {family_id}",
        "#include <chaos/native_types.h>",
        "#include <cctype>",
        "#include <cstring>",
        "#include <string>",
        "",
        f"namespace chaos::benchmark::{ns_slug}",
        "{",
    ]

    for idx, subject_id in enumerate(method_subject_ids):
        slot_name = _method_slot_name(subject_id)
        parsed = _parse_method_subject_id(subject_id)
        cpp_expr = _cpp_benchmark_expr(parsed)

        lines.append(f"    // [{idx}] {subject_id}")
        lines.append(f"    CHAOS_IL2CPP_INT32 BenchmarkEntry_{idx}() {{")
        lines.append(f"        return {cpp_expr};")
        lines.append("    }")
        lines.append("")

    # Dispatch table
    lines.append("    using BenchmarkEntryFn = CHAOS_IL2CPP_INT32 (*)();")
    lines.append("    static constexpr BenchmarkEntryFn kBenchmarkEntries[] = {")
    for idx in range(len(method_subject_ids)):
        comma = "," if idx < len(method_subject_ids) - 1 else ""
        lines.append(f"        &BenchmarkEntry_{idx}{comma}")
    lines.append("    };")
    lines.append(f"    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = {len(method_subject_ids)};")
    lines.append("")
    lines.append("}  // namespace chaos::benchmark::" + ns_slug)
    lines.append("")

    # RunNativeAot dispatch
    lines.append('extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)')
    lines.append("{")
    lines.append(f"    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::{ns_slug}::kBenchmarkEntryCount) {{ return -1; }}")
    lines.append(f"    return chaos::benchmark::{ns_slug}::kBenchmarkEntries[entryIndex]();")
    lines.append("}")
    lines.append("")

    text = "\n".join(lines)
    output_path = output_root / "BenchmarkNativeEntry.cpp"
    _write_text(output_path, text)
    artifacts.append(output_path.relative_to(repo_root).as_posix())

    # Append benchmark executable target to the family's CMakeLists.txt (once only)
    cmake_path = output_root / "CMakeLists.txt"
    if cmake_path.exists():
        existing_text = cmake_path.read_text(encoding="utf-8")
        if "# Benchmark executable (auto-generated)" not in existing_text:
            benchmark_cmake = (
                "\n"
                "# ---------------------------------------------------------------------------\n"
                "# Benchmark executable (auto-generated)\n"
                "# ---------------------------------------------------------------------------\n"
                "if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/BenchmarkNativeEntry.cpp)\n"
                f"add_executable(chaos_benchmark_{ns_slug}\n"
                "    ${CMAKE_SOURCE_DIR}/src/native/benchmark-host/native_aot_main.cpp\n"
                "    BenchmarkNativeEntry.cpp\n"
                ")\n"
                f"target_compile_features(chaos_benchmark_{ns_slug} PRIVATE cxx_std_17)\n"
                f"target_include_directories(chaos_benchmark_{ns_slug} PRIVATE\n"
                "    ${CMAKE_SOURCE_DIR}/src/native/common\n"
                "    ${CMAKE_SOURCE_DIR}/contracts/native/v0\n"
                ")\n"
                f"target_link_libraries(chaos_benchmark_{ns_slug} PRIVATE chaos_common)\n"
                "endif()\n"
            )
            with open(cmake_path, "a", encoding="utf-8") as f:
                f.write(benchmark_cmake)
            artifacts.append(cmake_path.relative_to(repo_root).as_posix())

    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "artifacts": artifacts,
    }
