from __future__ import annotations

import json
import shutil
import unittest
from pathlib import Path

from tests._support.fs import make_temp_repo_root
from tests._support.module_loading import load_module


REPO_ROOT = Path(__file__).resolve().parents[4]
CONTRACTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "verification_contracts.py"
MERGE_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "verification_merge.py"
REPORTING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "verification_reporting.py"
CODEGEN_STUBS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "verification_codegen_stubs.py"


class TestVerificationFormalContracts(unittest.TestCase):
    def test_build_records_and_payload_wrappers_follow_verification_v1_contract(self) -> None:
        contracts = load_module(CONTRACTS_MODULE_PATH, "chaos_verification_contracts_records")

        closure = contracts.build_closure_record(
            closure_id="closure/completed-solution-core",
            closure_kind="completed",
            scope_code="solution-core",
            template_id="template/completed-default",
            overlay_id="overlay/solution-core",
            target_capabilities=["capability.generic-sharing"],
            required_routes=["managed", "aot"],
            required_platforms=["windows-x64"],
            required_device_profiles=["desktop-amd64"],
            required_evidence_kinds=["semantic", "contract"],
            mandatory_lanes=["aot", "il2cpp", "contracts", "test_governance"],
            verification_state="blocked",
            support_state="partial",
            blockers=[
                {
                    "code": "toolchain_gap",
                    "lane": "il2cpp",
                    "message": "IL2CPP route is not wired yet.",
                }
            ],
            obligation_count=2,
            passed_count=1,
            blocked_count=1,
        )
        obligation = contracts.build_obligation_claim_record(
            obligation_claim_id="obligation/generic-sharing-aot",
            closure_id=closure["closureId"],
            capability_id="capability.generic-sharing",
            owner_subject_id="SolutionCorePack",
            required_evidence_kinds=["semantic", "contract"],
            required_routes=["aot"],
            required_platforms=["windows-x64"],
            required_device_profiles=["desktop-amd64"],
            mandatory_lanes=["aot", "il2cpp", "contracts"],
            support_state="unsupported",
            verification_state="blocked",
            blockers=[
                {
                    "code": "toolchain_gap",
                    "lane": "il2cpp",
                    "message": "Generic native route is still blocked.",
                }
            ],
            evidence_claim_ids=["evidence-claim/generic-sharing-aot-windows"],
            latest_stage_id="authority",
        )
        evidence_claim = contracts.build_evidence_claim_record(
            evidence_claim_id="evidence-claim/generic-sharing-aot-windows",
            obligation_claim_id=obligation["obligationClaimId"],
            scenario_id="scenario/generic-sharing",
            scenario_kind="unit-test",
            route_code="aot",
            platform_code="windows-x64",
            device_profile_code="desktop-amd64",
            evidence_kind="semantic",
            execution_contract_id="execution/native-proof-host",
            verification_state="planned",
            support_state="supported",
        )
        evidence = contracts.build_evidence_record(
            evidence_id="evidence/generic-sharing-aot-windows-001",
            evidence_claim_id=evidence_claim["evidenceClaimId"],
            obligation_claim_id=obligation["obligationClaimId"],
            closure_id=closure["closureId"],
            stage_id="semantic",
            route_code="aot",
            platform_code="windows-x64",
            device_profile_code="desktop-amd64",
            actual_device_id="lab-amd64-01",
            verification_state="failed",
            support_state="unsupported",
            blockers=[
                {
                    "code": "runtime_gap",
                    "lane": "aot",
                    "message": "Semantic assertion did not close.",
                }
            ],
            source_refs=[
                {
                    "kind": "report",
                    "path": "verification/archive/reports/completed/solution-core/semantic.md",
                    "label": "semantic report",
                }
            ],
            payload_kind="semantic",
            payload={
                "summary": "assertion mismatch",
                "assertions": [{"name": "ResultEqualsManaged", "status": "failed"}],
            },
        )
        stage_report = contracts.build_stage_report_record(
            stage_id="semantic",
            closure_id=closure["closureId"],
            scope_code="solution-core",
            stage_kind="semantic",
            verification_state="failed",
            support_state="unsupported",
            covered_obligation_claim_ids=[obligation["obligationClaimId"]],
            success_evidence_ids=["evidence/passed-001"],
            failed_evidence_ids=[evidence["evidenceId"]],
            blocked_evidence_ids=["evidence/blocked-001"],
            missing_evidence_claim_ids=["evidence-claim/missing-001"],
            blockers=[
                {
                    "code": "runtime_gap",
                    "lane": "aot",
                    "message": "Semantic mismatch is still open.",
                }
            ],
        )

        self.assertEqual("closure/completed-solution-core", closure["closureId"])
        self.assertEqual("partial", closure["supportState"])
        self.assertEqual("obligation/generic-sharing-aot", obligation["obligationClaimId"])
        self.assertEqual("evidence-claim/generic-sharing-aot-windows", evidence_claim["evidenceClaimId"])
        self.assertEqual("lab-amd64-01", evidence["kernel"]["actualDeviceId"])
        self.assertEqual("semantic", evidence["payloadKind"])
        self.assertEqual("semantic", stage_report["stageKind"])

        snapshot = contracts.build_snapshot(
            [closure, obligation],
            closure_id=closure["closureId"],
            producer_id="fixture-producer",
            source_spec_version="verification-v1",
        )
        master = contracts.build_master(
            [obligation],
            merge_key="obligationClaimId",
            producer_id="fixture-producer",
            source_spec_version="verification-v1",
        )

        self.assertEqual(1, snapshot["schemaVersion"])
        self.assertEqual("closure/completed-solution-core", snapshot["closureId"])
        self.assertEqual("fixture-producer", snapshot["producerId"])
        self.assertEqual(2, len(snapshot["items"]))
        self.assertEqual("obligationClaimId", master["mergeKey"])
        self.assertEqual("fixture-producer", master["producerId"])
        self.assertEqual(1, len(master["items"]))

    def test_merge_master_payload_overwrites_existing_items_by_stable_key(self) -> None:
        merge_module = load_module(MERGE_MODULE_PATH, "chaos_verification_merge_payload")

        existing = {
            "schemaVersion": 1,
            "generatedAtUtc": "2026-04-23T10:00:00Z",
            "producerId": "fixture",
            "sourceSpecVersion": "verification-v1",
            "mergeKey": "obligationClaimId",
            "items": [
                {
                    "obligationClaimId": "obligation/a",
                    "verificationState": "planned",
                    "supportState": "supported",
                },
                {
                    "obligationClaimId": "obligation/b",
                    "verificationState": "planned",
                    "supportState": "supported",
                },
            ],
        }
        incoming = {
            "schemaVersion": 1,
            "generatedAtUtc": "2026-04-23T10:05:00Z",
            "producerId": "fixture",
            "sourceSpecVersion": "verification-v1",
            "mergeKey": "obligationClaimId",
            "items": [
                {
                    "obligationClaimId": "obligation/b",
                    "verificationState": "passed",
                    "supportState": "supported",
                    "latestStageId": "semantic",
                },
                {
                    "obligationClaimId": "obligation/c",
                    "verificationState": "blocked",
                    "supportState": "unsupported",
                },
            ],
        }

        merged = merge_module.merge_result_master(existing, incoming)

        self.assertEqual("obligationClaimId", merged["mergeKey"])
        self.assertEqual(3, len(merged["items"]))
        by_id = {item["obligationClaimId"]: item for item in merged["items"]}
        self.assertEqual("planned", by_id["obligation/a"]["verificationState"])
        self.assertEqual("passed", by_id["obligation/b"]["verificationState"])
        self.assertEqual("semantic", by_id["obligation/b"]["latestStageId"])
        self.assertEqual("blocked", by_id["obligation/c"]["verificationState"])

    def test_write_stage_reports_materializes_summary_and_stage_markdown(self) -> None:
        reporting = load_module(REPORTING_MODULE_PATH, "chaos_verification_reporting_markdown")
        repo_root = make_temp_repo_root("verification", "stage-reports")

        try:
            bundle = reporting.write_stage_reports(
                repo_root=repo_root,
                closure_kind="completed",
                scope_code="solution-core",
                stage_reports=[
                    {
                        "stageId": "authority",
                        "closureId": "closure/completed-solution-core",
                        "scopeCode": "solution-core",
                        "stageKind": "authority",
                        "verificationState": "blocked",
                        "supportState": "unsupported",
                        "coveredObligationClaimIds": ["obligation/generic-sharing-aot"],
                        "successEvidenceIds": ["evidence/authority-ok-001"],
                        "failedEvidenceIds": ["evidence/authority-failed-001"],
                        "blockedEvidenceIds": ["evidence/authority-blocked-001"],
                        "missingEvidenceClaimIds": ["evidence-claim/authority-missing-001"],
                        "blockers": [
                            {
                                "code": "toolchain_gap",
                                "lane": "il2cpp",
                                "message": "Native route is not connected.",
                            }
                        ],
                        "reportRefs": [],
                    },
                    {
                        "stageId": "semantic",
                        "closureId": "closure/completed-solution-core",
                        "scopeCode": "solution-core",
                        "stageKind": "semantic",
                        "verificationState": "passed",
                        "supportState": "supported",
                        "coveredObligationClaimIds": ["obligation/generic-sharing-managed"],
                        "successEvidenceIds": ["evidence/semantic-ok-001"],
                        "failedEvidenceIds": [],
                        "blockedEvidenceIds": [],
                        "missingEvidenceClaimIds": [],
                        "blockers": [],
                        "reportRefs": [],
                    },
                ],
            )

            summary_path = repo_root / Path(bundle["summaryPath"])
            authority_path = repo_root / Path(bundle["stagePaths"]["authority"])
            semantic_path = repo_root / Path(bundle["stagePaths"]["semantic"])

            self.assertTrue(summary_path.is_file())
            self.assertTrue(authority_path.is_file())
            self.assertTrue(semantic_path.is_file())

            summary_text = summary_path.read_text(encoding="utf-8")
            authority_text = authority_path.read_text(encoding="utf-8")

            self.assertIn("Verification Stage Summary", summary_text)
            self.assertIn("authority", summary_text)
            self.assertIn("semantic", summary_text)
            self.assertIn("Blocked Evidence", authority_text)
            self.assertIn("Failed Evidence", authority_text)
            self.assertIn("Missing Claims", authority_text)
            self.assertIn("toolchain_gap", authority_text)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_write_stub_index_materializes_owner_subject_verification_path(self) -> None:
        codegen_stubs = load_module(CODEGEN_STUBS_MODULE_PATH, "chaos_verification_codegen_stubs")
        repo_root = make_temp_repo_root("verification", "codegen-stub-index")

        try:
            stub_index = codegen_stubs.build_stub_index(
                owner_subject_id="SolutionCorePack",
                capability_id="capability.generic-sharing",
                route_code="aot",
                profile_code="windows-x64",
                managed_source_refs=[
                    {
                        "kind": "managed-source",
                "path": "verification/catalog/owners/SolutionCorePack/proofs/GenericSharingProof.cs",
                        "label": "managed proof",
                    }
                ],
                stub_refs=[
                    {
                        "kind": "stub",
                        "path": "generated/aot/generic-sharing/stub.cpp",
                        "label": "native stub",
                    }
                ],
                generated_from_evidence_ids=["evidence/generic-sharing-aot-001"],
            )
            index_path = codegen_stubs.write_stub_index(repo_root, stub_index)

            self.assertEqual(
                repo_root
                / "artifact"
                / "verification-catalog"
                / "evidence"
                / "owners"
                / "SolutionCorePack"
                / "codegen-stubs"
                / "capability.generic-sharing"
                / "aot-windows-x64"
                / "stub-index.json",
                index_path,
            )
            self.assertTrue(index_path.is_file())

            payload = json.loads(index_path.read_text(encoding="utf-8"))
            self.assertEqual("SolutionCorePack", payload["ownerSubjectId"])
            self.assertEqual(
                "SolutionCorePack|capability.generic-sharing|aot|windows-x64",
                payload["mergeKey"],
            )
            self.assertNotIn("run", index_path.as_posix())
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
