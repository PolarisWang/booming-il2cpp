from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CAPABILITY_COVERAGE_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "capability_coverage.py"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class Phase5OwnerSubjectCoverageTests(unittest.TestCase):
    def test_capability_contract_resolves_solution_core_owner_and_support_states(self) -> None:
        capability_coverage = load_module(CAPABILITY_COVERAGE_MODULE_PATH, "chaos_phase5_capability_coverage_solution_core")

        contract = capability_coverage.resolve_capability_contract(
            capability_family=7,
            capability_item=31,
        )

        self.assertEqual(7, contract["capabilityFamily"])
        self.assertEqual("Interop And Marshaling", contract["capabilityFamilyLabel"])
        self.assertEqual(31, contract["capabilityItem"])
        self.assertEqual("Native Call Interop", contract["capabilityItemLabel"])
        self.assertEqual("SolutionCorePack", contract["ownerSubjectId"])
        self.assertEqual([1, 5], contract["supportStates"])
        self.assertEqual(["NativeGenerated", "ExternalRuntime"], contract["supportStateLabels"])
        self.assertTrue(contract["proofRequired"])
        self.assertTrue(contract["benchmarkRequired"])

    def test_capability_contract_resolves_hot_update_mixed_and_new_type_system_items(self) -> None:
        capability_coverage = load_module(CAPABILITY_COVERAGE_MODULE_PATH, "chaos_phase5_capability_coverage_subjects")

        hot_update_contract = capability_coverage.resolve_capability_contract(
            capability_family=13,
            capability_item=54,
        )
        mixed_contract = capability_coverage.resolve_capability_contract(
            capability_family=14,
            capability_item=61,
        )
        cast_contract = capability_coverage.resolve_capability_contract(
            capability_family=2,
            capability_item=67,
        )
        covariance_contract = capability_coverage.resolve_capability_contract(
            capability_family=2,
            capability_item=68,
        )

        self.assertEqual("HotUpdateHostPack", hot_update_contract["ownerSubjectId"])
        self.assertEqual([3, 2], hot_update_contract["supportStates"])
        self.assertEqual(["BridgeDispatch", "InterpreterReady"], hot_update_contract["supportStateLabels"])

        self.assertEqual("MixedExecutionFeaturePack", mixed_contract["ownerSubjectId"])
        self.assertEqual([3, 2, 1], mixed_contract["supportStates"])
        self.assertEqual(["BridgeDispatch", "InterpreterReady", "NativeGenerated"], mixed_contract["supportStateLabels"])

        self.assertEqual("Object Cast And Type Check", cast_contract["capabilityItemLabel"])
        self.assertEqual("Array Covariance", covariance_contract["capabilityItemLabel"])
        self.assertEqual("SolutionCorePack", cast_contract["ownerSubjectId"])
        self.assertEqual("SolutionCorePack", covariance_contract["ownerSubjectId"])
        self.assertEqual([1], cast_contract["supportStates"])
        self.assertEqual([1], covariance_contract["supportStates"])

    def test_diagnostics_items_are_explicitly_proof_optional_reporting_contracts(self) -> None:
        capability_coverage = load_module(CAPABILITY_COVERAGE_MODULE_PATH, "chaos_phase5_capability_coverage_diagnostics")

        for capability_item in (47, 48, 49, 50):
            contract = capability_coverage.resolve_capability_contract(
                capability_family=11,
                capability_item=capability_item,
            )

            self.assertEqual("SolutionCorePack", contract["ownerSubjectId"])
            self.assertEqual([4, 6], contract["supportStates"])
            self.assertFalse(contract["proofRequired"])
            self.assertFalse(contract["benchmarkRequired"])


if __name__ == "__main__":
    unittest.main()
