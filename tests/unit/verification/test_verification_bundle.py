from __future__ import annotations

import json
import shutil
import unittest
from pathlib import Path

from tests._support.fs import make_temp_repo_root
from tests._support.module_loading import load_module
from tests.unit.performance.testing_inventory_test_support import sample_inventory_source


REPO_ROOT = Path(__file__).resolve().parents[3]
VERIFICATION_BUNDLE_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "verification_bundle.py"


class TestVerificationBundle(unittest.TestCase):
    def test_build_verification_bundle_bridges_inventory_source_into_formal_snapshots(self) -> None:
        bundle_module = load_module(VERIFICATION_BUNDLE_MODULE_PATH, "chaos_verification_bundle_bridge")

        bundle = bundle_module.build_verification_bundle(
            sample_inventory_source(),
            closure_kind="completed",
            scope_code="testing-inventory",
        )

        closure_snapshot = bundle["latest"]["closure"]
        capability_snapshot = bundle["latest"]["capability"]
        evidence_claim_snapshot = bundle["latest"]["evidenceClaim"]
        stage_snapshot = bundle["latest"]["stage"]
        result_snapshot = bundle["latest"]["result"]

        self.assertEqual("closure/completed/testing-inventory", closure_snapshot["closureId"])
        self.assertEqual(1, len(closure_snapshot["items"]))
        self.assertEqual(2, len(capability_snapshot["items"]))
        self.assertEqual(8, len(evidence_claim_snapshot["items"]))
        self.assertEqual(2, len(result_snapshot["items"]))

        closure = closure_snapshot["items"][0]
        self.assertEqual("completed", closure["closureKind"])
        self.assertEqual("missing", closure["verificationState"])
        self.assertIn("semantic", closure["requiredEvidenceKinds"])
        self.assertIn("perf", closure["requiredEvidenceKinds"])
        self.assertIn("contracts", closure["mandatoryLanes"])
        self.assertIn("test_governance", closure["mandatoryLanes"])

        result_by_capability = {
            item["capabilityId"]: item
            for item in result_snapshot["items"]
        }
        self.assertEqual("missing", result_by_capability["capability/7/31"]["verificationState"])
        self.assertEqual("supported", result_by_capability["capability/7/31"]["supportState"])
        self.assertEqual("missing", result_by_capability["capability/12/52"]["verificationState"])

        stage_by_kind = {item["stageKind"]: item for item in stage_snapshot["items"]}
        self.assertEqual("passed", stage_by_kind["collector"]["verificationState"])
        self.assertEqual("passed", stage_by_kind["registry"]["verificationState"])
        self.assertEqual("passed", stage_by_kind["workspace"]["verificationState"])
        self.assertEqual("planned", stage_by_kind["managed-proof"]["verificationState"])
        self.assertEqual("planned", stage_by_kind["native-proof"]["verificationState"])
        self.assertEqual("not_required", stage_by_kind["hotupdate-proof"]["verificationState"])
        self.assertEqual("missing", stage_by_kind["perf"]["verificationState"])

        evidence_by_id = {
            item["evidenceClaimId"]: item
            for item in evidence_claim_snapshot["items"]
        }
        managed_benchmark_claim = evidence_by_id[
            "evidence-claim/benchmark/FixtureSubject::Fixture.Benchmarks::Fixture.Benchmarks.NativeInteropBenchmark::RunWorkload()/managed/bridge/windows-x64/fixture-device"
        ]
        self.assertEqual("perf", managed_benchmark_claim["stageKind"])
        self.assertEqual("benchmark", managed_benchmark_claim["projectionMeta"]["scope"])
        self.assertEqual(
            "benchmark/FixtureSubject::Fixture.Benchmarks::Fixture.Benchmarks.NativeInteropBenchmark::RunWorkload()/bridge/windows-x64/fixture-device",
            managed_benchmark_claim["projectionMeta"]["rowKey"],
        )
        self.assertEqual("native-proof", evidence_by_id[
            "evidence-claim/unit/FixtureSubject::Fixture.Tests::Fixture.Proofs.NativeInteropProof::Run()/native-proof"
        ]["stageKind"])

    def test_write_verification_bundle_materializes_latest_master_and_reports(self) -> None:
        bundle_module = load_module(VERIFICATION_BUNDLE_MODULE_PATH, "chaos_verification_bundle_write")
        repo_root = make_temp_repo_root("verification", "bundle-write")

        try:
            payload = bundle_module.write_verification_bundle(
                repo_root,
                sample_inventory_source(),
                closure_kind="completed",
                scope_code="testing-inventory",
            )

            latest_root = repo_root / "docs" / "testing-inventory" / "verification" / "latest"
            master_root = repo_root / "docs" / "testing-inventory" / "verification" / "master"
            report_root = repo_root / "docs" / "testing-inventory" / "verification" / "reports" / "completed" / "testing-inventory"
            stub_index_path = (
                repo_root
                / "subjects"
                / "FixtureSubject"
                / "verification"
                / "codegen-stubs"
                / "capability"
                / "7"
                / "31"
                / "native-windows-native-check"
                / "stub-index.json"
            )

            self.assertTrue((latest_root / "closure-snapshot.json").is_file())
            self.assertTrue((latest_root / "capability-snapshot.json").is_file())
            self.assertTrue((latest_root / "evidence-claims-snapshot.json").is_file())
            self.assertTrue((latest_root / "stage-snapshot.json").is_file())
            self.assertTrue((latest_root / "result-snapshot.json").is_file())
            self.assertTrue((master_root / "closure-master.json").is_file())
            self.assertTrue((master_root / "capability-master.json").is_file())
            self.assertTrue((master_root / "evidence-claims-master.json").is_file())
            self.assertTrue((master_root / "stage-master.json").is_file())
            self.assertTrue((master_root / "result-master.json").is_file())
            self.assertTrue((report_root / "summary.md").is_file())
            self.assertTrue((report_root / "perf.md").is_file())
            self.assertTrue(stub_index_path.is_file())

            evidence_claim_master = json.loads((master_root / "evidence-claims-master.json").read_text(encoding="utf-8"))
            result_master = json.loads((master_root / "result-master.json").read_text(encoding="utf-8"))
            stub_index = json.loads(stub_index_path.read_text(encoding="utf-8"))
            self.assertEqual("evidenceClaimId", evidence_claim_master["mergeKey"])
            self.assertEqual(8, len(evidence_claim_master["items"]))
            self.assertEqual("obligationClaimId", result_master["mergeKey"])
            self.assertEqual(2, len(result_master["items"]))
            self.assertEqual("FixtureSubject", stub_index["ownerSubjectId"])
            self.assertEqual("capability/7/31", stub_index["capabilityId"])
            self.assertEqual("native", stub_index["routeCode"])
            self.assertEqual("windows-native-check", stub_index["profileCode"])
            self.assertIn("docs/testing-inventory/verification/latest/evidence-claims-snapshot.json", payload["artifacts"])
            self.assertIn("docs/testing-inventory/verification/master/evidence-claims-master.json", payload["artifacts"])
            self.assertIn("docs/testing-inventory/verification/latest/result-snapshot.json", payload["artifacts"])
            self.assertIn("docs/testing-inventory/verification/reports/completed/testing-inventory/summary.md", payload["artifacts"])
            self.assertIn(
                "subjects/FixtureSubject/verification/codegen-stubs/capability/7/31/native-windows-native-check/stub-index.json",
                payload["artifacts"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)
