from __future__ import annotations

from typing import Any

try:
    from . import candidate_extractor as candidate_extractor_module
    from . import rule_registry as rule_registry_module
except ImportError:
    import sys
    from pathlib import Path

    sys.path.insert(0, str(Path(__file__).resolve().parent))
    import candidate_extractor as candidate_extractor_module
    import rule_registry as rule_registry_module


def _family(assembly_name: str, slug: str, display_name: str, description: str, matched_types: list[str]) -> dict[str, Any]:
    return {
        "familyId": f"family/{assembly_name}/{slug}",
        "displayName": display_name,
        "description": description,
        "matchedTypes": matched_types,
    }


def _synthesize_anchor_bundle_families(
    assembly_name: str,
    *,
    available_type_names: set[str],
    anchor_types: set[str],
    family_bundle: list[tuple[str, str, str]],
) -> list[dict[str, Any]]:
    matched_anchors = sorted(type_name for type_name in available_type_names if type_name in anchor_types)
    if not matched_anchors:
        return []
    return [_family(assembly_name, slug, display_name, description, matched_anchors) for slug, display_name, description in family_bundle]


def _synthesize_marker_rule_families(
    assembly_name: str,
    *,
    available_type_names: set[str],
    rules: list[tuple[str, str, str, tuple[str, ...]]],
) -> list[dict[str, Any]]:
    families: list[dict[str, Any]] = []
    for slug, display_name, description, markers in rules:
        matched = [type_name for type_name in available_type_names if any(type_name.startswith(marker) for marker in markers)]
        if matched:
            families.append(_family(assembly_name, slug, display_name, description, sorted(matched)))
    return families


def synthesize_families_for_assembly(assembly_name: str, surface_types: dict[str, Any]) -> list[dict[str, Any]]:
    features = candidate_extractor_module.extract_candidate_features(
        assembly_name,
        {"publicTypes": surface_types},
    )
    available_type_names = set(features.get("surfaceTypes") or [])
    rule_config = rule_registry_module.get_rule_config(assembly_name)
    if rule_config is None:
        return []

    mode = str(rule_config.get("mode") or "")
    if mode == "marker-rules":
        return _synthesize_marker_rule_families(
            assembly_name,
            available_type_names=available_type_names,
            rules=list(rule_config.get("rules") or []),
        )
    if mode == "anchor-bundle":
        return _synthesize_anchor_bundle_families(
            assembly_name,
            available_type_names=available_type_names,
            anchor_types=set(rule_config.get("anchorTypes") or []),
            family_bundle=list(rule_config.get("familyBundle") or []),
        )
    return []
