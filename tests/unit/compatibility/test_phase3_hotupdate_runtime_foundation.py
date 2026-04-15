from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
HOT_UPDATE_ROOT = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate"
HOT_UPDATE_PACKAGE_PATH = HOT_UPDATE_ROOT / "HotUpdatePackage.cs"
HOT_UPDATE_ASSEMBLY_LOADER_PATH = HOT_UPDATE_ROOT / "HotUpdateAssemblyLoader.cs"
SUPPLEMENTAL_METADATA_REGISTRY_PATH = HOT_UPDATE_ROOT / "SupplementalMetadataRegistry.cs"
RUNTIME_MANAGER_PATH = HOT_UPDATE_ROOT / "RuntimeManager.cs"
METADATA_SUPPLEMENT_PROOF_PATH = (
    REPO_ROOT
    / "subjects"
    / "HotUpdateHostPack"
    / "source"
    / "Host"
    / "Proofs"
    / "MetadataSupplementProofEntry.cs"
)


class Phase3HotUpdateRuntimeFoundationTests(unittest.TestCase):
    def test_hot_update_package_model_and_loader_expose_runtime_foundation_contract(self) -> None:
        package_source = HOT_UPDATE_PACKAGE_PATH.read_text(encoding="utf-8")
        loader_source = HOT_UPDATE_ASSEMBLY_LOADER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "record LoadedHotUpdatePackage",
            "LoadedSupplementalMetadata SupplementalMetadata",
        ]:
            self.assertIn(required_fragment, package_source)

        for required_fragment in [
            "class HotUpdateAssemblyLoader",
            "LoadFromDirectory(",
            "PackageReader.ManifestFileName",
            "SupplementalMetadataLoader",
            "LoadedHotUpdatePackage",
        ]:
            self.assertIn(required_fragment, loader_source)

    def test_runtime_manager_and_metadata_proof_consume_registry_backed_runtime_foundation(self) -> None:
        registry_source = SUPPLEMENTAL_METADATA_REGISTRY_PATH.read_text(encoding="utf-8")
        runtime_source = RUNTIME_MANAGER_PATH.read_text(encoding="utf-8")
        proof_source = METADATA_SUPPLEMENT_PROOF_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "class SupplementalMetadataRegistry",
            "LoadedSupplementalMetadata? ActiveMetadata",
            "Activate(LoadedHotUpdatePackage package)",
            "Clear()",
            "TryGetMethod(ManagedMethodIdentityArtifact identity",
            "TryGetMethodByToken(",
            "TryGetTypeBySubjectId(",
            "HasGenericInstantiation(",
        ]:
            self.assertIn(required_fragment, registry_source)

        for required_fragment in [
            "HotUpdateAssemblyLoader _assemblyLoader = new();",
            "public SupplementalMetadataRegistry SupplementalMetadata { get; } = new();",
            "_assemblyLoader.LoadFromDirectory(",
            "SupplementalMetadata.Activate(package);",
            "SupplementalMetadata.Clear();",
        ]:
            self.assertIn(required_fragment, runtime_source)

        self.assertNotIn("_supplementalMetadataLoader.LoadFromBytes", runtime_source)

        for required_fragment in [
            "runtimeManager.SupplementalMetadata",
            "TryGetTypeBySubjectId(",
            "TryGetMethod(PackageIdentity",
            "TryGetMethodByToken(",
        ]:
            self.assertIn(required_fragment, proof_source)

        self.assertNotIn("new SupplementalMetadataLoader().LoadFromBytes", proof_source)


if __name__ == "__main__":
    unittest.main()
