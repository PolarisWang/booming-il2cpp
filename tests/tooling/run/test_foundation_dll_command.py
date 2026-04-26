from __future__ import annotations

from unittest.mock import patch

from tests.tooling.run.verify_roadmap0_test_support import *


FOUNDATION_DLL_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "foundation_dll.py"


class FoundationDllCommandTests(VerifyRoadmap0TestSupport):
    def test_foundation_dll_analyze_gaps_invokes_gap_analyzer(self) -> None:
        command_module = load_module(FOUNDATION_DLL_MODULE_PATH, "chaos_run_foundation_dll_analyze_gaps")

        with patch.object(command_module, "gap_analyzer_module", create=True) as gap_mock:
            gap_mock.analyze_gaps.return_value = {
                "familyCount": 3,
                "dllCount": 1,
                "statusCounts": {"present": 1, "coverage-widened": 1, "needs-tests": 1, "no-coverage": 0},
            }
            result = command_module.handle(
                {"id": "foundation-dll-analyze-gaps", "handler": "foundation_dll.dispatch", "target": "foundation-dll"},
                REPO_ROOT,
                "windows",
                "foundation-dll analyze-gaps",
                {"scope": "all", "update_ledger": True, "auto_generate": True},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual(3, result.payload["familyCount"])
        gap_mock.analyze_gaps.assert_called_once_with(REPO_ROOT, scope="all", update_ledger=True, auto_generate=True)

    def test_foundation_dll_check_family_invokes_gap_analyzer_for_single_family(self) -> None:
        command_module = load_module(FOUNDATION_DLL_MODULE_PATH, "chaos_run_foundation_dll_check_family")

        with patch.object(command_module, "gap_analyzer_module", create=True) as gap_mock:
            gap_mock.analyze_gaps.return_value = {
                "familyCount": 1,
                "dllCount": 1,
                "families": [{"familyId": "family/System.Private.CoreLib/convert/char"}],
            }
            result = command_module.handle(
                {"id": "foundation-dll-check-family", "handler": "foundation_dll.dispatch", "target": "foundation-dll"},
                REPO_ROOT,
                "windows",
                "foundation-dll check-family",
                {"family": "family/System.Private.CoreLib/convert/char"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual(1, result.payload["familyCount"])
        gap_mock.analyze_gaps.assert_called_once_with(
            REPO_ROOT,
            scope="family/System.Private.CoreLib/convert/char",
            update_ledger=False,
        )

    def test_foundation_dll_derive_invokes_candidate_generation(self) -> None:
        command_module = load_module(FOUNDATION_DLL_MODULE_PATH, "chaos_run_foundation_dll_derive")

        with patch.object(command_module, "denominator_derive_module", create=True) as derive_mock:
            derive_mock.generate_candidate_ledger.return_value = {
                "candidateLedgerPath": "verification/projections/foundation-dll-audit/capability-family-ledger.candidate.json",
                "familyCount": 116,
                "dllCount": 14,
            }
            result = command_module.handle(
                {"id": "foundation-dll-derive", "handler": "foundation_dll.dispatch", "target": "foundation-dll"},
                REPO_ROOT,
                "windows",
                "foundation-dll derive",
                {"scope": "all"},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual("foundation-dll", result.target)
        self.assertEqual(116, result.payload["familyCount"])
        derive_mock.generate_candidate_ledger.assert_called_once_with(REPO_ROOT, scope="all")

    def test_foundation_dll_promote_requires_explicit_approval_flag(self) -> None:
        command_module = load_module(FOUNDATION_DLL_MODULE_PATH, "chaos_run_foundation_dll_promote")

        result = command_module.handle(
            {"id": "foundation-dll-promote", "handler": "foundation_dll.dispatch", "target": "foundation-dll"},
            REPO_ROOT,
            "windows",
            "foundation-dll promote",
            {"scope": "System.Text.Json", "promote_approved": False},
        )

        self.assertEqual("error", result.status)
        self.assertIn("promote-approved", result.errors[0])

    def test_foundation_dll_promote_invokes_promotion_when_approved(self) -> None:
        command_module = load_module(FOUNDATION_DLL_MODULE_PATH, "chaos_run_foundation_dll_promote_approved")

        with patch.object(command_module, "denominator_promote_module", create=True) as promote_mock:
            promote_mock.promote_candidate_ledger.return_value = {
                "ledgerPath": "verification/projections/foundation-dll-audit/capability-family-ledger.json",
                "promotedDlls": ["System.Text.Json"],
                "snapshotId": "snap-20260426-promoted",
            }
            result = command_module.handle(
                {"id": "foundation-dll-promote", "handler": "foundation_dll.dispatch", "target": "foundation-dll"},
                REPO_ROOT,
                "windows",
                "foundation-dll promote",
                {"scope": "System.Text.Json", "promote_approved": True},
            )

        self.assertEqual("ok", result.status)
        self.assertEqual(["System.Text.Json"], result.payload["promotedDlls"])
        promote_mock.promote_candidate_ledger.assert_called_once_with(
            REPO_ROOT,
            scope="System.Text.Json",
            promote_approved=True,
        )

    def test_foundation_dll_verify_consistency_invokes_projection_validation(self) -> None:
        command_module = load_module(FOUNDATION_DLL_MODULE_PATH, "chaos_run_foundation_dll_verify_consistency")

        with patch.object(command_module, "projection_consistency_module", create=True) as consistency_mock:
            consistency_mock.verify_projection_consistency.return_value = {
                "mode": "full",
                "validated": True,
                "checks": [{"name": "family-count", "status": "ok"}],
            }
            result = command_module.handle(
                {"id": "foundation-dll-verify-consistency", "handler": "foundation_dll.dispatch", "target": "foundation-dll"},
                REPO_ROOT,
                "windows",
                "foundation-dll verify-consistency",
                {"mode": "full"},
            )

        self.assertEqual("ok", result.status)
        self.assertTrue(result.payload["validated"])
        consistency_mock.verify_projection_consistency.assert_called_once_with(REPO_ROOT, mode="full")

    def test_foundation_dll_full_runs_derive_promote_refresh_and_consistency(self) -> None:
        command_module = load_module(FOUNDATION_DLL_MODULE_PATH, "chaos_run_foundation_dll_full")

        with patch.object(command_module, "denominator_derive_module", create=True) as derive_mock, patch.object(
            command_module, "denominator_promote_module", create=True
        ) as promote_mock, patch.object(command_module, "audit_generator_module", create=True) as generator_mock, patch.object(
            command_module, "projection_consistency_module", create=True
        ) as consistency_mock, patch.object(command_module, "inventory_generator_module", create=True) as inventory_mock:
            derive_mock.generate_candidate_ledger.return_value = {"candidateLedgerPath": "candidate.json", "familyCount": 116}
            promote_mock.promote_candidate_ledger.return_value = {"ledgerPath": "ledger.json", "promotedDlls": ["all"]}
            generator_mock.write_foundation_dll_audit_outputs.return_value = {"dashboardPath": "verification/projections/foundation-dll-audit/dashboard.html"}
            consistency_mock.verify_projection_consistency.return_value = {"validated": True, "mode": "full"}
            inventory_mock.resolve_inventory_output_root.return_value = REPO_ROOT / "verification" / "projections" / "testing-inventory"
            inventory_mock.refresh_inventory_outputs.return_value = {"validated": True, "artifacts": []}

            result = command_module.handle(
                {"id": "foundation-dll-full", "handler": "foundation_dll.dispatch", "target": "foundation-dll"},
                REPO_ROOT,
                "windows",
                "foundation-dll full",
                {"scope": "all", "promote_approved": True, "mode": "full"},
            )

        self.assertEqual("ok", result.status)
        derive_mock.generate_candidate_ledger.assert_called_once_with(REPO_ROOT, scope="all")
        promote_mock.promote_candidate_ledger.assert_called_once()
        generator_mock.write_foundation_dll_audit_outputs.assert_called_once_with(REPO_ROOT)
        consistency_mock.verify_projection_consistency.assert_called_once_with(REPO_ROOT, mode="full")
        inventory_mock.refresh_inventory_outputs.assert_called_once()

    def test_foundation_dll_onboard_runs_derive_promote_and_full_refresh(self) -> None:
        command_module = load_module(FOUNDATION_DLL_MODULE_PATH, "chaos_run_foundation_dll_onboard")

        with patch.object(command_module, "denominator_derive_module", create=True) as derive_mock, patch.object(
            command_module, "denominator_promote_module", create=True
        ) as promote_mock, patch.object(command_module, "audit_generator_module", create=True) as generator_mock, patch.object(
            command_module, "projection_consistency_module", create=True
        ) as consistency_mock, patch.object(command_module, "inventory_generator_module", create=True) as inventory_mock:
            derive_mock.generate_candidate_ledger.return_value = {"candidateLedgerPath": "candidate.json", "familyCount": 13}
            promote_mock.promote_candidate_ledger.return_value = {"ledgerPath": "ledger.json", "promotedDlls": ["System.Text.Json"]}
            generator_mock.write_foundation_dll_audit_outputs.return_value = {"dashboardPath": "verification/projections/foundation-dll-audit/dashboard.html"}
            consistency_mock.verify_projection_consistency.return_value = {"validated": True, "mode": "full"}
            inventory_mock.resolve_inventory_output_root.return_value = REPO_ROOT / "verification" / "projections" / "testing-inventory"
            inventory_mock.refresh_inventory_outputs.return_value = {"validated": True, "artifacts": []}

            result = command_module.handle(
                {"id": "foundation-dll-onboard", "handler": "foundation_dll.dispatch", "target": "foundation-dll"},
                REPO_ROOT,
                "windows",
                "foundation-dll onboard",
                {"scope": "System.Text.Json", "promote_approved": True, "mode": "full"},
            )

        self.assertEqual("ok", result.status)
        derive_mock.generate_candidate_ledger.assert_called_once_with(REPO_ROOT, scope="System.Text.Json")
        promote_mock.promote_candidate_ledger.assert_called_once_with(
            REPO_ROOT,
            scope="System.Text.Json",
            promote_approved=True,
        )
        generator_mock.write_foundation_dll_audit_outputs.assert_called_once_with(REPO_ROOT)
        consistency_mock.verify_projection_consistency.assert_called_once_with(REPO_ROOT, mode="full")
        inventory_mock.refresh_inventory_outputs.assert_called_once()
