from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
OWNER_ROOT = REPO_ROOT / "verification" / "catalog" / "owners"
CANONICAL_SUBJECTS = (
    "SolutionCorePack",
    "MixedExecutionFeaturePack",
    "HotUpdateHostPack",
)


def _read_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


class SubjectAuthorityFreezeTests(unittest.TestCase):
    def test_canonical_subjects_ship_subject_feature_files(self) -> None:
        for subject_id in CANONICAL_SUBJECTS:
            features_path = OWNER_ROOT / subject_id / "owner.features.json"
            self.assertTrue(features_path.is_file(), msg=f"missing owner.features.json: {subject_id}")

    def test_subject_feature_files_use_frozen_minimum_shape(self) -> None:
        for subject_id in CANONICAL_SUBJECTS:
            payload = _read_json(OWNER_ROOT / subject_id / "owner.features.json")
            self.assertEqual(1, payload.get("schemaVersion"))
            self.assertEqual(subject_id, payload.get("subjectId"))
            self.assertIsInstance(payload.get("engineeringScenarios"), list)
            self.assertIsInstance(payload.get("features"), list)

            for scenario in payload["engineeringScenarios"]:
                self.assertIsInstance(scenario.get("code"), int)
                self.assertGreater(int(scenario["code"]), 0)
                self.assertIsInstance(scenario.get("id"), str)
                self.assertTrue(str(scenario["id"]).strip())
                self.assertIsInstance(scenario.get("sourcePath"), str)
                self.assertTrue(str(scenario["sourcePath"]).startswith(f"verification/catalog/scenarios/{subject_id}/"))

    def test_solution_core_pack_scenario_set_is_frozen(self) -> None:
        payload = _read_json(OWNER_ROOT / "SolutionCorePack" / "owner.features.json")
        scenarios = {item["id"] for item in payload["engineeringScenarios"]}
        self.assertEqual(
            {
                "solution-multi-project",
                "solution-reference-assembly",
                "solution-corelib-reference",
                "solution-mixed-reference-closure",
                "solution-foundation-dll-translation",
                "solution-static-call-ctor-getter",
                "solution-constructor-then-instance-call",
                "solution-delegate-closed-target-relay",
                "solution-exception-throw-catch-finally",
                "solution-nested-exception-throw-catch-finally",
                "solution-marshaling-utf8-export",
                "solution-reflection-interop-closure",
                "solution-pinvoke-direct-call",
                "solution-array-boxing-reference-array",
                "solution-interface-dispatch-message",
                "solution-threading-threadstatic-monitor",
            },
            scenarios,
        )
        self.assertNotIn("solution-simple-lib", scenarios)
        self.assertNotIn("solution-package-reference", scenarios)

    def test_mixed_and_hotupdate_minimum_scenarios_are_frozen(self) -> None:
        mixed_payload = _read_json(OWNER_ROOT / "MixedExecutionFeaturePack" / "owner.features.json")
        hotupdate_payload = _read_json(OWNER_ROOT / "HotUpdateHostPack" / "owner.features.json")

        self.assertEqual(
            {"mixed-bridge-solution"},
            {item["id"] for item in mixed_payload["engineeringScenarios"]},
        )
        self.assertEqual(
            {"full-project-hotupdate-solution"},
            {item["id"] for item in hotupdate_payload["engineeringScenarios"]},
        )

    def test_subject_feature_files_carry_owner_capability_entries(self) -> None:
        solution_payload = _read_json(OWNER_ROOT / "SolutionCorePack" / "owner.features.json")
        mixed_payload = _read_json(OWNER_ROOT / "MixedExecutionFeaturePack" / "owner.features.json")
        hotupdate_payload = _read_json(OWNER_ROOT / "HotUpdateHostPack" / "owner.features.json")

        solution_items = {item["capabilityItem"] for item in solution_payload["features"]}
        mixed_items = {item["capabilityItem"] for item in mixed_payload["features"]}
        hotupdate_items = {item["capabilityItem"] for item in hotupdate_payload["features"]}

        self.assertIn(31, solution_items)
        self.assertIn(67, solution_items)
        self.assertIn(68, solution_items)
        self.assertIn(61, mixed_items)
        self.assertIn(54, hotupdate_items)


if __name__ == "__main__":
    unittest.main()
