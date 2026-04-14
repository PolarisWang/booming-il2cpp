from __future__ import annotations

import json
from typing import Any, Iterable


_FRAMEWORK_PROJECT_REFERENCE = "../../../../src/reference/Chaos.TestFramework/Chaos.TestFramework.csproj"


def assign_entry_indexes(entries: Iterable[dict[str, Any]]) -> list[dict[str, Any]]:
    normalized_entries = [dict(entry) for entry in entries]
    indexed_entries: list[dict[str, Any]] = []
    for entry_index, entry in enumerate(sorted(normalized_entries, key=lambda item: str(item.get("stableId") or ""))):
        indexed_entry = dict(entry)
        indexed_entry["entryIndex"] = entry_index
        indexed_entries.append(indexed_entry)
    return indexed_entries


def render_declared_test_host_source(
    *,
    subject_id: str,
    host_kind: str,
    entries: Iterable[dict[str, Any]],
) -> str:
    indexed_entries = _prepare_entries(entries)
    class_name = _host_class_name(subject_id=subject_id, host_kind=host_kind)
    namespace_name = f"Chaos.Generated.ManagedTests.{_sanitize_identifier(subject_id)}"
    if host_kind == "proof-host":
        return _render_proof_host_source(namespace_name=namespace_name, class_name=class_name, entries=indexed_entries)
    if host_kind == "benchmark-host":
        return _render_benchmark_host_source(namespace_name=namespace_name, class_name=class_name, entries=indexed_entries)
    raise ValueError(f"unsupported host kind: {host_kind}")


def render_declared_test_host_project(
    *,
    subject_id: str,
    host_kind: str,
    project_references: Iterable[str],
    assembly_name: str | None = None,
) -> str:
    project_name = assembly_name or _host_class_name(subject_id=subject_id, host_kind=host_kind)
    references = [_FRAMEWORK_PROJECT_REFERENCE, *list(project_references)]
    lines = [
        '<Project Sdk="Microsoft.NET.Sdk">',
        "  <PropertyGroup>",
        "    <TargetFramework>net8.0</TargetFramework>",
        "    <ImplicitUsings>enable</ImplicitUsings>",
        "    <Nullable>enable</Nullable>",
        f"    <AssemblyName>{project_name}</AssemblyName>",
        f"    <RootNamespace>{project_name}</RootNamespace>",
        "  </PropertyGroup>",
        "  <ItemGroup>",
    ]
    for reference in references:
        lines.append(f'    <ProjectReference Include="{_xml_escape(str(reference))}" />')
    lines.extend(
        [
            "  </ItemGroup>",
            "</Project>",
            "",
        ]
    )
    return "\n".join(lines)


def _render_proof_host_source(
    *,
    namespace_name: str,
    class_name: str,
    entries: list[dict[str, Any]],
) -> str:
    lines = [
        "using System;",
        "",
        f"namespace {namespace_name};",
        "",
        f"public static class {class_name}",
        "{",
        "    public static int Execute(int entryIndex)",
        "    {",
        "        switch (entryIndex)",
        "        {",
    ]
    for entry in entries:
        entry_index = int(entry.get("entryIndex", -1))
        declaring_type = str(entry.get("declaringType") or "")
        method_name = str(entry.get("methodName") or "")
        lines.extend(
            [
                f"            case {entry_index}:",
                f"                global::{declaring_type}.{method_name}();",
                "                return 0;",
            ]
        )
    lines.extend(
        [
            '            default:',
            '                throw new ArgumentOutOfRangeException(nameof(entryIndex), entryIndex, "Unknown declared proof entry index.");',
            "        }",
            "    }",
            "}",
            "",
        ]
    )
    return "\n".join(lines)


def _render_benchmark_host_source(
    *,
    namespace_name: str,
    class_name: str,
    entries: list[dict[str, Any]],
) -> str:
    lines = [
        "using System;",
        "using System.Collections.Generic;",
        "",
        f"namespace {namespace_name};",
        "",
        "public sealed record DeclaredBenchmarkEntry(",
        "    int EntryIndex,",
        "    string StableId,",
        "    string Alias,",
        "    byte Category,",
        "    byte CapabilityFamily,",
        "    ushort CapabilityItem,",
        "    byte Archetype,",
        "    ushort HotUpdateCapability,",
        "    uint Requires,",
        "    ushort Metrics,",
        "    byte Modes,",
        "    byte WarmupCount,",
        "    ushort IterationCount,",
        "    ushort InvocationCount);",
        "",
        f"public static class {class_name}",
        "{",
        "    public static IReadOnlyList<DeclaredBenchmarkEntry> Entries { get; } = new DeclaredBenchmarkEntry[]",
        "    {",
    ]
    for entry in entries:
        lines.extend(
            [
                "        new(",
                f"            entryIndex: {int(entry.get('entryIndex', -1))},",
                f"            stableId: {_csharp_string(str(entry.get('stableId') or ''))},",
                f"            alias: {_csharp_string(str(entry.get('alias') or ''))},",
                f"            category: {int(entry.get('category') or 0)},",
                f"            capabilityFamily: {int(entry.get('capabilityFamily') or 0)},",
                f"            capabilityItem: {int(entry.get('capabilityItem') or 0)},",
                f"            archetype: {int(entry.get('archetype') or 0)},",
                f"            hotUpdateCapability: {int(entry.get('hotUpdateCapability') or 0)},",
                f"            requires: {int(entry.get('requires') or 0)},",
                f"            metrics: {int(entry.get('metrics') or 0)},",
                f"            modes: {int(entry.get('modes') or 0)},",
                f"            warmupCount: {int(entry.get('warmupCount') or 0)},",
                f"            iterationCount: {int(entry.get('iterationCount') or 0)},",
                f"            invocationCount: {int(entry.get('invocationCount') or 0)}),",
            ]
        )
    lines.extend(
        [
            "    };",
            "",
            "    public static int Execute(int entryIndex)",
            "    {",
            "        switch (entryIndex)",
            "        {",
        ]
    )
    for entry in entries:
        entry_index = int(entry.get("entryIndex", -1))
        declaring_type = str(entry.get("declaringType") or "")
        method_name = str(entry.get("methodName") or "")
        lines.extend(
            [
                f"            case {entry_index}:",
                f"                global::{declaring_type}.{method_name}();",
                "                return 0;",
            ]
        )
    lines.extend(
        [
            '            default:',
            '                throw new ArgumentOutOfRangeException(nameof(entryIndex), entryIndex, "Unknown declared benchmark entry index.");',
            "        }",
            "    }",
            "}",
            "",
        ]
    )
    return "\n".join(lines)


def _host_class_name(*, subject_id: str, host_kind: str) -> str:
    host_suffix_by_kind = {
        "proof-host": "ProofHost",
        "benchmark-host": "BenchmarkHost",
    }
    try:
        host_suffix = host_suffix_by_kind[host_kind]
    except KeyError as error:
        raise ValueError(f"unsupported host kind: {host_kind}") from error
    return f"{_sanitize_identifier(subject_id)}Declared{host_suffix}"


def _prepare_entries(entries: Iterable[dict[str, Any]]) -> list[dict[str, Any]]:
    indexed_entries = assign_entry_indexes(entries)
    if not indexed_entries:
        raise ValueError("generated managed host requires at least one declared entry")

    required_fields = (
        ("stableId", "stableId"),
        ("declaringType", "declaringType"),
        ("methodName", "methodName"),
    )
    for entry in indexed_entries:
        for field_name, field_label in required_fields:
            value = str(entry.get(field_name) or "").strip()
            if value:
                continue
            raise ValueError(f"declared entry requires {field_label}")
    return indexed_entries


def _sanitize_identifier(value: str) -> str:
    sanitized = "".join(character if character.isalnum() else "_" for character in value.strip())
    return sanitized or "GeneratedHost"


def _csharp_string(value: str) -> str:
    return json.dumps(value)


def _xml_escape(value: str) -> str:
    return (
        value.replace("&", "&amp;")
        .replace('"', "&quot;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
    )
