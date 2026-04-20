from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SCHEMA_ROOT = REPO_ROOT / "contracts" / "artifacts" / "v0" / "schemas"
AUDIT_PACKET_SCHEMA = SCHEMA_ROOT / "foundation-dll-audit-packet.schema.json"
CODEGEN_REVIEW_SCHEMA = SCHEMA_ROOT / "foundation-codegen-review.schema.json"
NATIVE_HOTUPDATE_AUDIT_SCHEMA = SCHEMA_ROOT / "native-hotupdate-audit.schema.json"
ANALYSIS_CONTRACT_ROOT = REPO_ROOT / "tests" / "contracts" / "analysis" / "v0"


def _read_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


class FoundationDllTranslationAuditSchemaTests(unittest.TestCase):
    def test_audit_packet_schema_freezes_required_artifacts(self) -> None:
        self.assertTrue(AUDIT_PACKET_SCHEMA.is_file(), msg=f"missing schema: {AUDIT_PACKET_SCHEMA}")

        schema = _read_json(AUDIT_PACKET_SCHEMA)
        self.assertEqual("Foundation DLL Translation Audit Packet v0", schema["title"])
        self.assertEqual("object", schema["type"])
        self.assertFalse(schema["additionalProperties"])

        required = set(schema["required"])
        for field in [
            "schemaVersion",
            "assemblyName",
            "inputManifest",
            "surfaceLedger",
            "semanticLedger",
            "dllBoundaryInventory",
            "nativeizationPlan",
            "managedProofReport",
            "nativeProofReport",
            "hotupdateProofReport",
            "benchmarkReport",
            "codegenReviewJson",
            "codegenReviewMarkdown",
            "completionReport",
            "finalStatus",
        ]:
            self.assertIn(field, required)

        final_status = schema["properties"]["finalStatus"]
        self.assertEqual(["pending", "completed", "blocked"], final_status["enum"])

    def test_codegen_review_schema_freezes_review_shape(self) -> None:
        self.assertTrue(CODEGEN_REVIEW_SCHEMA.is_file(), msg=f"missing schema: {CODEGEN_REVIEW_SCHEMA}")

        schema = _read_json(CODEGEN_REVIEW_SCHEMA)
        self.assertEqual("Foundation DLL Codegen Review v0", schema["title"])
        self.assertEqual("object", schema["type"])
        self.assertFalse(schema["additionalProperties"])

        required = set(schema["required"])
        for field in [
            "schemaVersion",
            "assemblyName",
            "sourceInputs",
            "dllBoundaryInventory",
            "generatedFiles",
            "nativeDispatchManifest",
            "hotupdateBindingManifest",
            "runtimeHelpers",
            "coverage",
            "findings",
            "finalStatus",
        ]:
            self.assertIn(field, required)

        coverage_required = set(schema["properties"]["coverage"]["required"])
        self.assertEqual(
            {"publicMembersTotal", "publicMembersCodegenCovered", "uncoveredMembers"},
            coverage_required,
        )

    def test_native_hotupdate_audit_schema_freezes_combined_chain_shape(self) -> None:
        self.assertTrue(
            NATIVE_HOTUPDATE_AUDIT_SCHEMA.is_file(),
            msg=f"missing schema: {NATIVE_HOTUPDATE_AUDIT_SCHEMA}",
        )

        schema = _read_json(NATIVE_HOTUPDATE_AUDIT_SCHEMA)
        self.assertEqual("Native HotUpdate Audit v0", schema["title"])
        self.assertEqual("object", schema["type"])
        self.assertFalse(schema["additionalProperties"])

        required = set(schema["required"])
        self.assertEqual(
            {
                "reportVersion",
                "artifactKind",
                "runId",
                "subjectId",
                "matrixId",
                "status",
                "pipelineId",
                "entrySelection",
                "hostInput",
                "nativeGeneration",
                "nativeBuild",
                "hotupdateRuntime",
                "truthBoundary",
            },
            required,
        )

        self.assertEqual("native-hotupdate-audit", schema["properties"]["artifactKind"]["const"])
        native_generation_required = set(schema["properties"]["nativeGeneration"]["required"])
        self.assertTrue(
            {
                "nativeReferencePlanKind",
                "translationUnitMode",
                "translationUnitMethodCount",
                "auditStatus",
                "auditMessage",
                "runtimeSkeletonReservedStubCount",
            }.issubset(native_generation_required)
        )
        self.assertEqual(
            "not-used-by-this-combined-proof",
            schema["properties"]["truthBoundary"]["properties"]["nativeAotScope"]["const"],
        )
        self.assertEqual(False, schema["properties"]["truthBoundary"]["properties"]["fullCoreLibTranslated"]["const"])

    def test_native_hotupdate_audit_sample_and_snapshot_are_paired(self) -> None:
        sample_path = ANALYSIS_CONTRACT_ROOT / "samples" / "native-hotupdate-audit.min.json"
        snapshot_path = ANALYSIS_CONTRACT_ROOT / "snapshots" / "native-hotupdate-audit.snapshot.json"

        self.assertTrue(sample_path.is_file(), msg=f"missing sample: {sample_path}")
        self.assertTrue(snapshot_path.is_file(), msg=f"missing snapshot: {snapshot_path}")

        sample = _read_json(sample_path)
        snapshot = _read_json(snapshot_path)

        self.assertEqual("native-hotupdate-audit", sample["artifactKind"])
        self.assertEqual("native-hotupdate-proof-output", sample["pipelineId"])
        self.assertEqual("assembly-bound-native-reference-skeleton", sample["nativeGeneration"]["runtimeExecutionKind"])
        self.assertEqual("assembly-full-closure-runtime-skeleton", sample["nativeGeneration"]["nativeReferencePlanKind"])
        self.assertEqual("runtime-skeleton", sample["nativeGeneration"]["translationUnitMode"])
        self.assertEqual(3, sample["nativeGeneration"]["translationUnitMethodCount"])
        self.assertEqual("runtime-skeleton", sample["nativeGeneration"]["auditStatus"])
        self.assertEqual(0, sample["nativeGeneration"]["runtimeSkeletonReservedStubCount"])
        self.assertEqual("native-reference", sample["nativeBuild"]["buildKind"])
        self.assertEqual(False, sample["truthBoundary"]["fullCoreLibTranslated"])
        self.assertGreaterEqual(len(snapshot["nativeGeneration"]["generatedSourcePaths"]), 2)
        self.assertEqual(3, snapshot["nativeGeneration"]["translationUnitMethodCount"])
        self.assertIn("corelib-reference-hotupdate:", snapshot["hotupdateRuntime"]["outputLines"][0])


if __name__ == "__main__":
    unittest.main()
