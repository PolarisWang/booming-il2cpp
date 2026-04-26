from __future__ import annotations

from pathlib import Path

from tests._support.module_loading import load_module


REPO_ROOT = Path(__file__).resolve().parents[5]
EXTRACTOR_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "candidate_extractor.py"


def test_extract_candidate_features_collects_core_surface_shape() -> None:
    extractor_module = load_module(EXTRACTOR_MODULE_PATH, "chaos_foundation_dll_candidate_extractor")

    assembly_payload = {
        "publicTypeDefinitionCount": 80,
        "publicMemberCount": 640,
        "publicTypes": {
            "System.Text.Json.JsonSerializer": {},
            "System.Text.Json.JsonSerializerOptions": {},
            "System.Text.Json.JsonDocument": {},
            "System.Text.Json.Nodes.JsonObject": {},
            "System.Text.Json.Schema.JsonSchemaExporter": {},
        },
    }

    features = extractor_module.extract_candidate_features(
        "System.Text.Json",
        assembly_payload,
        semantic_family_count=0,
    )

    assert features["assemblyName"] == "System.Text.Json"
    assert features["publicTypeCount"] == 80
    assert features["publicMemberCount"] == 640
    assert "System.Text.Json.JsonSerializer" in features["surfaceTypes"]
    assert "System.Text.Json" in features["namespaceBuckets"]
    assert "System.Text.Json.Nodes" in features["namespaceBuckets"]
    assert "JsonSerializer" in features["typeNameTokens"]
    assert "JsonSchemaExporter" in features["typeNameTokens"]
    assert "System" in features["rootNamespaceBuckets"]
    assert "JsonObject" in features["rootTypeAnchors"]
    assert "Attribute" not in features["attributeTypeTokens"]
    assert features["interopNamespacePresence"] is False


def test_extract_candidate_features_carries_semantic_hint_for_corelib() -> None:
    extractor_module = load_module(EXTRACTOR_MODULE_PATH, "chaos_foundation_dll_candidate_extractor")

    features = extractor_module.extract_candidate_features(
        "System.Private.CoreLib",
        {"publicTypeDefinitionCount": 2801, "publicMemberCount": 32489, "publicTypes": {}},
        semantic_family_count=6,
    )

    assert features["assemblyName"] == "System.Private.CoreLib"
    assert features["semanticFamilyCount"] == 6


def test_extract_candidate_features_captures_attribute_and_interop_hints() -> None:
    extractor_module = load_module(EXTRACTOR_MODULE_PATH, "chaos_foundation_dll_candidate_extractor_interop")

    features = extractor_module.extract_candidate_features(
        "System.Runtime.InteropServices",
        {
            "publicTypeDefinitionCount": 211,
            "publicMemberCount": 1603,
            "publicTypes": {
                "System.Runtime.InteropServices.StructLayoutAttribute": {},
                "System.Runtime.InteropServices.DllImportAttribute": {},
                "System.Runtime.InteropServices.ComWrappers": {},
                "System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute": {},
                "System.Runtime.InteropServices.ComTypes.IStream": {},
            },
        },
        semantic_family_count=0,
    )

    assert features["interopNamespacePresence"] is True
    assert "StructLayoutAttribute" in features["attributeTypeTokens"]
    assert "DllImportAttribute" in features["attributeTypeTokens"]
    assert "ComWrappers" in features["rootTypeAnchors"]
