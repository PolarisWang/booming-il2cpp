from __future__ import annotations

import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SOURCE_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "FeatureSlices" / "CoreRuntimeFeatures"
METADATA_SUPPLEMENT_SOURCE_PATH = SOURCE_ROOT / "ReflectionAndMetadata" / "MetadataSupplementProof.cs"
AOT_CLOSURE_VALIDATION_SOURCE_PATH = SOURCE_ROOT / "GenericsAndCollections" / "AotClosureValidationProof.cs"


class Phase5MetadataClosureProofTests(unittest.TestCase):
    def test_solution_core_pack_realizes_metadata_supplement_and_aot_closure_proof_slices(self) -> None:
        self.assertTrue(
            METADATA_SUPPLEMENT_SOURCE_PATH.is_file(),
            msg=f"missing metadata supplement slice: {METADATA_SUPPLEMENT_SOURCE_PATH}",
        )
        self.assertTrue(
            AOT_CLOSURE_VALIDATION_SOURCE_PATH.is_file(),
            msg=f"missing aot closure validation slice: {AOT_CLOSURE_VALIDATION_SOURCE_PATH}",
        )

        metadata_supplement_source = METADATA_SUPPLEMENT_SOURCE_PATH.read_text(encoding="utf-8")
        aot_closure_validation_source = AOT_CLOSURE_VALIDATION_SOURCE_PATH.read_text(encoding="utf-8")

        self.assertIn("internal readonly record struct MetadataSupplementRecord", metadata_supplement_source)
        self.assertIn("internal sealed class MetadataSupplementBox<T>", metadata_supplement_source)
        self.assertIn("internal static class MetadataSupplementProofEntry", metadata_supplement_source)
        self.assertIn('Alias = "metadata-supplement-proof"', metadata_supplement_source)
        self.assertIn("Capability = ChaosCapabilityItem.MetadataSupplement", metadata_supplement_source)
        self.assertIn("typeof(MetadataSupplementBox<int>)", metadata_supplement_source)
        self.assertIn("MetadataSupplementRegistry.Add", metadata_supplement_source)

        self.assertIn("internal readonly record struct AotClosureRecord", aot_closure_validation_source)
        self.assertIn("internal static class AotClosureValidationProofEntry", aot_closure_validation_source)
        self.assertIn('Alias = "aot-closure-validation-proof"', aot_closure_validation_source)
        self.assertIn("Capability = ChaosCapabilityItem.AotClosureValidation", aot_closure_validation_source)
        self.assertIn("AotClosureManifest.BuildRequiredClosure()", aot_closure_validation_source)
        self.assertIn("typeof(AotClosureBox<int>)", aot_closure_validation_source)
        self.assertIn("typeof(AotClosureBox<string>)", aot_closure_validation_source)

    def test_solution_core_pack_declared_catalog_exposes_metadata_supplement_and_aot_closure_items(self) -> None:
        sys.path.insert(0, str(REPO_ROOT / "build" / "toolchains" / "run"))
        from testing import compiled_catalog

        catalog = compiled_catalog.build_subject_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="SolutionCorePack",
        )

        declared_unit_items = {int(entry.get("capabilityItem") or 0) for entry in catalog.get("declaredUnitTests", [])}
        self.assertIn(23, declared_unit_items)
        self.assertIn(53, declared_unit_items)


if __name__ == "__main__":
    unittest.main()
