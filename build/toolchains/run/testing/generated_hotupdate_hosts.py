from __future__ import annotations

from pathlib import Path
import sys

try:
    from . import template_assets as template_assets_module
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing import template_assets as template_assets_module


_FRAMEWORK_PROJECT_REFERENCE = "../../../../src/reference/Chaos.TestFramework.Sdk/Chaos.TestFramework.Sdk.csproj"
_RUNTIME_PROJECT_REFERENCE = "../../../../src/reference/Chaos.TestFramework.Runtime/Chaos.TestFramework.Runtime.csproj"
_PROOF_HOST_TEMPLATE = "templates/hotupdate-proof-host.cs.tmpl"
_BENCHMARK_HOST_TEMPLATE = "templates/hotupdate-benchmark-host.cs.tmpl"
_HOST_PROJECT_TEMPLATE = "templates/hotupdate-host.csproj.tmpl"


def render_declared_hotupdate_host_source(*, subject_id: str, host_kind: str) -> str:
    class_name = _host_class_name(subject_id=subject_id, host_kind=host_kind)
    namespace_name = f"Chaos.Generated.HotUpdateTests.{_sanitize_identifier(subject_id)}"
    if host_kind == "proof-host":
        return template_assets_module.render_template(
            owner_file=__file__,
            relative_template_path=_PROOF_HOST_TEMPLATE,
            replacements={
                "NAMESPACE": namespace_name,
                "CLASS_NAME": class_name,
            },
        )

    if host_kind == "benchmark-host":
        return template_assets_module.render_template(
            owner_file=__file__,
            relative_template_path=_BENCHMARK_HOST_TEMPLATE,
            replacements={
                "NAMESPACE": namespace_name,
                "CLASS_NAME": class_name,
            },
        )

    raise ValueError(f"unsupported hotupdate host kind: {host_kind}")


def render_declared_hotupdate_host_project(*, assembly_name: str, generated_source_path: str) -> str:
    return template_assets_module.render_template(
        owner_file=__file__,
        relative_template_path=_HOST_PROJECT_TEMPLATE,
        replacements={
            "ASSEMBLY_NAME": _xml_escape(assembly_name),
            "FRAMEWORK_PROJECT_REFERENCE": _xml_escape(_FRAMEWORK_PROJECT_REFERENCE),
            "RUNTIME_PROJECT_REFERENCE": _xml_escape(_RUNTIME_PROJECT_REFERENCE),
            "GENERATED_SOURCE_PATH": _xml_escape(generated_source_path),
        },
    )


def _host_class_name(*, subject_id: str, host_kind: str) -> str:
    host_suffix_by_kind = {
        "proof-host": "HotUpdateProofHost",
        "benchmark-host": "HotUpdateBenchmarkHost",
    }
    try:
        host_suffix = host_suffix_by_kind[host_kind]
    except KeyError as error:
        raise ValueError(f"unsupported hotupdate host kind: {host_kind}") from error
    return f"{_sanitize_identifier(subject_id)}{host_suffix}"


def _sanitize_identifier(value: str) -> str:
    sanitized = "".join(character if character.isalnum() else "_" for character in value.strip())
    return sanitized or "GeneratedHotUpdateHost"


def _xml_escape(value: str) -> str:
    return (
        value.replace("&", "&amp;")
        .replace('"', "&quot;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
    )
