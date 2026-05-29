from __future__ import annotations

from typing import Any


def _namespace_of(type_name: str) -> str:
    parts = type_name.split(".")
    if len(parts) <= 1:
        return type_name
    return ".".join(parts[:-1])


def _type_token(type_name: str) -> str:
    return type_name.split(".")[-1]


def _root_namespace(type_name: str) -> str:
    return type_name.split(".")[0] if type_name else type_name


def _is_attribute_type(type_name: str) -> bool:
    return _type_token(type_name).endswith("Attribute")


def extract_candidate_features(
    assembly_name: str,
    assembly_payload: dict[str, Any],
    *,
    semantic_family_count: int = 0,
) -> dict[str, Any]:
    public_types = dict(assembly_payload.get("publicTypes") or {})
    surface_types = sorted(public_types.keys())
    namespace_buckets = sorted({_namespace_of(type_name) for type_name in surface_types})
    type_name_tokens = sorted({_type_token(type_name) for type_name in surface_types})
    root_namespace_buckets = sorted({_root_namespace(type_name) for type_name in surface_types})
    attribute_type_tokens = sorted({_type_token(type_name) for type_name in surface_types if _is_attribute_type(type_name)})
    root_type_anchors = sorted({_type_token(type_name) for type_name in surface_types})
    interop_namespace_presence = any(type_name.startswith("System.Runtime.InteropServices") for type_name in surface_types)

    return {
        "assemblyName": assembly_name,
        "publicTypeCount": int(assembly_payload.get("publicTypeDefinitionCount") or 0),
        "publicMemberCount": int(assembly_payload.get("publicMemberCount") or 0),
        "surfaceTypes": surface_types,
        "namespaceBuckets": namespace_buckets,
        "rootNamespaceBuckets": root_namespace_buckets,
        "typeNameTokens": type_name_tokens,
        "attributeTypeTokens": attribute_type_tokens,
        "rootTypeAnchors": root_type_anchors,
        "interopNamespacePresence": interop_namespace_presence,
        "semanticFamilyCount": int(semantic_family_count or 0),
    }
