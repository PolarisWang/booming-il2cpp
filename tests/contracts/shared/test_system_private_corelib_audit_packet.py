from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
TASK_ROOT = REPO_ROOT / "docs" / "dev" / "in-progress" / "20260419-03-system-private-corelib-full-verification"
AUDIT_ROOT = TASK_ROOT / "audit" / "System.Private.CoreLib"


def _read_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


class SystemPrivateCoreLibAuditPacketTests(unittest.TestCase):
    def test_corelib_audit_packet_contains_required_phase0_artifacts(self) -> None:
        expected_files = [
            "input-manifest.json",
            "surface-ledger.json",
            "semantic-ledger.json",
            "corelib-dll-boundary-inventory-v1-01.json",
            "nativeization-plan.json",
            "managed-proof-report.json",
            "native-proof-report.json",
            "hotupdate-proof-report.json",
            "benchmark-report.json",
            "codegen-review.json",
            "codegen-review.md",
            "completion-report.json",
        ]

        for filename in expected_files:
            self.assertTrue((AUDIT_ROOT / filename).is_file(), msg=f"missing CoreLib audit artifact: {filename}")

    def test_corelib_completion_report_does_not_overclaim_full_verification(self) -> None:
        completion = _read_json(AUDIT_ROOT / "completion-report.json")

        self.assertEqual(0, completion["schemaVersion"])
        self.assertEqual("System.Private.CoreLib", completion["assemblyName"])
        self.assertEqual(
            "docs/dev/in-progress/20260419-03-system-private-corelib-full-verification/audit/System.Private.CoreLib/corelib-dll-boundary-inventory-v1-01.json",
            completion["auditPacket"]["dllBoundaryInventory"],
        )
        self.assertEqual("blocked", completion["finalStatus"])
        self.assertGreaterEqual(len(completion["blockers"]), 1)
        blocker_ids = {item["id"] for item in completion["blockers"]}
        self.assertIn("corelib-full-native-proof-missing", blocker_ids)
        self.assertIn("corelib-codegen-review-missing", blocker_ids)
        self.assertIn("corelib-hotupdate-proof-missing", blocker_ids)

    def test_corelib_codegen_review_is_paired_json_and_markdown(self) -> None:
        review = _read_json(AUDIT_ROOT / "codegen-review.json")
        review_markdown = (AUDIT_ROOT / "codegen-review.md").read_text(encoding="utf-8")

        self.assertEqual(0, review["schemaVersion"])
        self.assertEqual("System.Private.CoreLib", review["assemblyName"])
        self.assertEqual(
            "docs/dev/in-progress/20260419-03-system-private-corelib-full-verification/audit/System.Private.CoreLib/corelib-dll-boundary-inventory-v1-01.json",
            review["dllBoundaryInventory"],
        )
        self.assertEqual("blocked", review["finalStatus"])
        self.assertGreaterEqual(len(review["generatedFiles"]), 1)
        self.assertEqual("inspected-entry-driven-artifact", review["generatedFiles"][0]["classification"])
        self.assertIn("# System.Private.CoreLib Codegen Review", review_markdown)
        self.assertIn("DLL-boundary inventory exists", review_markdown)
        self.assertIn("Existing generated artifacts are entry-driven", review_markdown)
        self.assertIn("Final recommendation: blocked", review_markdown)

    def test_corelib_native_proof_report_lists_required_semantic_families(self) -> None:
        native_proof = _read_json(AUDIT_ROOT / "native-proof-report.json")

        self.assertEqual("blocked", native_proof["status"])
        self.assertEqual("dll-boundary", native_proof["requiredProofKind"])
        family_names = {item["familyName"] for item in native_proof["semanticFamilyRequirements"]}
        self.assertEqual(
            {
                "managed-cil",
                "internal-call",
                "runtime-special",
                "pinvoke",
                "field-metadata",
                "abstract",
            },
            family_names,
        )
        self.assertFalse(native_proof["entryDrivenArtifactsAccepted"])

    def test_corelib_hotupdate_report_distinguishes_related_consumer_validation(self) -> None:
        hotupdate = _read_json(AUDIT_ROOT / "hotupdate-proof-report.json")

        self.assertEqual("blocked", hotupdate["status"])
        self.assertEqual("Proof", hotupdate["requiredProofLevel"])
        self.assertEqual("HotUpdateHostPack", hotupdate["consumerSubjectId"])
        self.assertFalse(hotupdate["relatedConsumerValidationClosesCoreLibGate"])
        related = {item["taskId"] for item in hotupdate["relatedEvidence"]}
        self.assertIn("20260418-08-hotupdate-consumer-validation", related)


if __name__ == "__main__":
    unittest.main()
