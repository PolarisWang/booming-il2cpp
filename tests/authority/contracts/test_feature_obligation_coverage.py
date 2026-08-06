from __future__ import annotations

import importlib.util
import json
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CAPABILITY_COVERAGE_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "capability_coverage.py"
COMPILED_CATALOG_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "compiled_catalog.py"
CANONICAL_SUBJECTS = (
    "SolutionCorePack",
    "MixedExecutionFeaturePack",
    "HotUpdateHostPack",
)


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


def _required_feature_items(
    payload: dict,
    *,
    field_name: str,
) -> set[int]:
    return {
        int(feature["capabilityItem"])
        for feature in list(payload.get("features") or [])
        if bool(feature.get(field_name))
    }


class FeatureObligationCoverageTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.capability_coverage = load_module(
            CAPABILITY_COVERAGE_MODULE_PATH,
            "chaos_phase4_feature_obligation_capability_coverage",
        )
        cls.compiled_catalog = load_module(
            COMPILED_CATALOG_MODULE_PATH,
            "chaos_phase4_feature_obligation_compiled_catalog",
        )

    def test_subject_feature_obligations_are_backed_by_declared_assets(self) -> None:
        failures: list[str] = []

        for subject_id in CANONICAL_SUBJECTS:
            features_payload = json.loads(
                (REPO_ROOT / "subjects" / subject_id / "subject.features.json").read_text(encoding="utf-8")
            )
            declared_catalog = self.compiled_catalog.build_subject_declared_test_catalog(
                repo_root=REPO_ROOT,
                subject_id=subject_id,
            )

            declared_proof_items = {
                int(entry.get("capabilityItem") or 0)
                for entry in list(declared_catalog.get("declaredUnitTests") or [])
                if int(entry.get("capabilityItem") or 0) > 0
            }
            declared_benchmark_items = {
                int(entry.get("capabilityItem") or 0)
                for entry in list(declared_catalog.get("declaredBenchmarks") or [])
                if int(entry.get("capabilityItem") or 0) > 0
            }

            missing_proof_items = sorted(
                _required_feature_items(features_payload, field_name="proofRequired") - declared_proof_items
            )
            missing_benchmark_items = sorted(
                _required_feature_items(features_payload, field_name="benchmarkRequired") - declared_benchmark_items
            )

            if missing_proof_items:
                failures.append(
                    f"{subject_id} missing proof assets: {self._format_items(missing_proof_items)}"
                )
            if missing_benchmark_items:
                failures.append(
                    f"{subject_id} missing benchmark assets: {self._format_items(missing_benchmark_items)}"
                )

        self.assertEqual([], failures)

    def _format_items(self, items: list[int]) -> str:
        labels: list[str] = []
        for item in items:
            contract = self.capability_coverage.resolve_capability_contract(
                capability_family=0,
                capability_item=item,
                repo_root=REPO_ROOT,
            )
            labels.append(f"{item}:{contract['capabilityItemLabel']}")
        return ", ".join(labels)


if __name__ == "__main__":
    unittest.main()
