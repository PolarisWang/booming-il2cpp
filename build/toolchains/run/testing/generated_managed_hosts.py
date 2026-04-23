from __future__ import annotations

from pathlib import Path
from typing import Any, Iterable
import sys

try:
    from . import template_assets as template_assets_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing import template_assets as template_assets_module


_FRAMEWORK_PROJECT_REFERENCE = "../../../../../src/reference/Chaos.TestFramework.Sdk/Chaos.TestFramework.Sdk.csproj"
_RUNTIME_PROJECT_REFERENCE = "../../../../../src/reference/Chaos.TestFramework.Runtime/Chaos.TestFramework.Runtime.csproj"
_PROOF_HOST_TEMPLATE = "templates/managed-proof-host.cs.tmpl"
_BENCHMARK_HOST_TEMPLATE = "templates/managed-benchmark-host.cs.tmpl"
_HOST_PROJECT_TEMPLATE = "templates/managed-declared-host.csproj.tmpl"


def assign_entry_indexes(entries: Iterable[dict[str, Any]]) -> list[dict[str, Any]]:
    normalized_entries = [dict(entry) for entry in entries]
    sorted_entries = [
        dict(entry)
        for entry in sorted(normalized_entries, key=lambda item: str(item.get("stableId") or ""))
    ]
    explicit_entry_indexes = [
        entry.get("entryIndex")
        for entry in sorted_entries
    ]
    if explicit_entry_indexes and all(
        isinstance(entry_index, int) and not isinstance(entry_index, bool) and entry_index >= 0
        for entry_index in explicit_entry_indexes
    ):
        if len({int(entry_index) for entry_index in explicit_entry_indexes}) != len(explicit_entry_indexes):
            raise ValueError("declared entries require unique explicit entryIndex values")
        return sorted_entries

    indexed_entries: list[dict[str, Any]] = []
    for entry_index, entry in enumerate(sorted_entries):
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
    generated_source_path: str,
    assembly_name: str | None = None,
) -> str:
    project_name = assembly_name or _host_class_name(subject_id=subject_id, host_kind=host_kind)
    references = [_FRAMEWORK_PROJECT_REFERENCE, _RUNTIME_PROJECT_REFERENCE, *list(project_references)]
    project_reference_items = "\n".join(
        f'    <ProjectReference Include="{_xml_escape(str(reference))}" />'
        for reference in references
    )
    return template_assets_module.render_template(
        owner_file=__file__,
        relative_template_path=_HOST_PROJECT_TEMPLATE,
        replacements={
            "ASSEMBLY_NAME": _xml_escape(project_name),
            "PROJECT_REFERENCES": project_reference_items,
            "GENERATED_SOURCE_PATH": _xml_escape(str(generated_source_path)),
        },
    )


def _render_proof_host_source(
    *,
    namespace_name: str,
    class_name: str,
    entries: list[dict[str, Any]],
) -> str:
    return template_assets_module.render_template(
        owner_file=__file__,
        relative_template_path=_PROOF_HOST_TEMPLATE,
        replacements={
            "NAMESPACE": namespace_name,
            "CLASS_NAME": class_name,
            "SWITCH_CASES": _render_switch_cases(entries),
        },
    )


def _render_benchmark_host_source(
    *,
    namespace_name: str,
    class_name: str,
    entries: list[dict[str, Any]],
) -> str:
    return template_assets_module.render_template(
        owner_file=__file__,
        relative_template_path=_BENCHMARK_HOST_TEMPLATE,
        replacements={
            "NAMESPACE": namespace_name,
            "CLASS_NAME": class_name,
            "SWITCH_CASES": _render_switch_cases(entries),
        },
    )


def _render_switch_cases(entries: list[dict[str, Any]]) -> str:
    lines: list[str] = []
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

def _xml_escape(value: str) -> str:
    return (
        value.replace("&", "&amp;")
        .replace('"', "&quot;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
    )
