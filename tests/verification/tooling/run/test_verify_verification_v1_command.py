from __future__ import annotations

from unittest.mock import patch

from tests.tooling.run.verify_roadmap0_test_support import *


class VerifyVerificationV1CommandTests(VerifyRoadmap0TestSupport):
    def test_verify_verification_v1_refreshes_and_returns_formal_output_paths(self) -> None:
        verify_module = load_module(VERIFY_MODULE_PATH, "chaos_run_verify_verification_v1_refresh")
        payload = {
            "outputRoot": "verification/projections/testing-inventory",
            "artifacts": [
                "verification/projections/testing-inventory/inventory.html",
                "verification/archive/latest/result-snapshot.json",
                "verification/archive/master/result-master.json",
                "verification/archive/reports/completed/testing-inventory/summary.md",
                "verification/evidence/owners/FixtureSubject/codegen-stubs/capability/7/31/native/stub-index.json",
            ],
            "verificationData": {
                "latestResultPath": "verification/archive/latest/result-snapshot.json",
                "masterResultPath": "verification/archive/master/result-master.json",
                "reportSummaryPath": "verification/archive/reports/completed/testing-inventory/summary.md",
                "codegenStubPaths": [
                    "verification/evidence/owners/FixtureSubject/codegen-stubs/capability/7/31/native/stub-index.json",
                ],
            },
            "validated": True,
        }

        with patch.object(verify_module, "inventory_generator_module", create=True) as generator_mock:
            generator_mock.resolve_inventory_output_root.return_value = (
                REPO_ROOT / "verification" / "projections" / "testing-inventory"
            )
            generator_mock.refresh_inventory_outputs.return_value = payload
            result = verify_module.handle(
                {
                    "id": "verify-verification-v1",
                    "handler": "verify.dispatch",
                    "target": "verification-v1",
                },
                REPO_ROOT,
                "windows",
                "verify verification-v1",
                {"output": "verification/projections/testing-inventory"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("verification-v1", result.target)
        self.assertEqual(
            "verification/archive/latest/result-snapshot.json",
            result.payload["verificationData"]["latestResultPath"],
        )
        self.assertIn(
            "verification/evidence/owners/FixtureSubject/codegen-stubs/capability/7/31/native/stub-index.json",
            result.payload["importantOutputs"],
        )
        generator_mock.resolve_inventory_output_root.assert_called_once_with(
            REPO_ROOT,
            "verification/projections/testing-inventory",
        )
        generator_mock.refresh_inventory_outputs.assert_called_once_with(
            REPO_ROOT,
            host_platform="windows",
            output_root=REPO_ROOT / "verification" / "projections" / "testing-inventory",
        )

    def test_verify_verification_v1_reports_refresh_failures(self) -> None:
        verify_module = load_module(VERIFY_MODULE_PATH, "chaos_run_verify_verification_v1_failure")

        with patch.object(verify_module, "inventory_generator_module", create=True) as generator_mock:
            generator_mock.refresh_inventory_outputs.side_effect = RuntimeError("verification refresh failed")
            result = verify_module.handle(
                {
                    "id": "verify-verification-v1",
                    "handler": "verify.dispatch",
                    "target": "verification-v1",
                },
                REPO_ROOT,
                "windows",
                "verify verification-v1",
            )

        self.assertEqual("error", result.status)
        self.assertEqual(["verification refresh failed"], result.errors)
        self.assertEqual(1, result.payload["exitCode"])
