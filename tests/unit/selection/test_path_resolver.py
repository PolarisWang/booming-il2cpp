from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PATH_RESOLVER_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "path_resolver.py"
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"
TRACE_SUBJECT_ID = "FixtureTraceSubject"
TRACE_MATRIX_ID = "windows-reference-check"
PERF_SUBJECT_ID = "FixturePerfSubject"
PERF_MATRIX_ID = "windows-perf-release"
LEGACY_ARCHETYPE_SUBJECT_ID = "GoldenMultiProject"


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


class PathResolverTests(unittest.TestCase):
    def test_contract_roots_prefer_samples_without_root_analysis_compatibility_dir(self) -> None:
        resolver_module = load_module(PATH_RESOLVER_MODULE_PATH, "chaos_path_resolver_contract_roots")

        roots = resolver_module.contract_roots(REPO_ROOT)

        self.assertEqual(
            REPO_ROOT / "contracts" / "artifacts" / "v0" / "schemas",
            roots["artifactSchemaRoot"],
        )
        self.assertEqual(
            REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "samples",
            roots["artifactSampleRoot"],
        )
        self.assertEqual(
            REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "snapshots",
            roots["artifactSnapshotRoot"],
        )
        self.assertEqual(
            REPO_ROOT / "tests" / "contracts" / "native" / "v0" / "samples",
            roots["nativeSampleRoot"],
        )
        self.assertNotIn("artifactSampleCompatibilityRoot", roots)
        self.assertNotIn("nativeSampleCompatibilityRoot", roots)
        self.assertNotIn("analysisCompatibilityExampleRoot", roots)

    def test_subject_helper_delegates_root_resolution_to_path_resolver(self) -> None:
        resolver_module = load_module(PATH_RESOLVER_MODULE_PATH, "chaos_path_resolver_subject_roots")
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "chaos_subjects_path_resolver_usage")
        run_id = "20260407-fixture-trace-001"

        self.assertEqual(
            REPO_ROOT / "subjects" / TRACE_SUBJECT_ID,
            resolver_module.subject_root(REPO_ROOT, TRACE_SUBJECT_ID),
        )
        self.assertEqual(
            REPO_ROOT / "subjects" / TRACE_SUBJECT_ID / "subject.manifest.json",
            resolver_module.subject_manifest_path(REPO_ROOT, TRACE_SUBJECT_ID),
        )
        self.assertEqual(
            REPO_ROOT / "artifacts" / "subjects" / TRACE_SUBJECT_ID / "runs",
            resolver_module.subject_runs_root(REPO_ROOT, TRACE_SUBJECT_ID),
        )
        self.assertEqual(
            REPO_ROOT / "artifacts" / "subjects" / TRACE_SUBJECT_ID / "runs" / run_id,
            resolver_module.subject_run_root(REPO_ROOT, TRACE_SUBJECT_ID, run_id),
        )

        roots = subjects_module.subject_artifact_roots(
            TRACE_SUBJECT_ID,
            TRACE_MATRIX_ID,
            run_id=run_id,
        )
        self.assertEqual(f"artifacts/subjects/{TRACE_SUBJECT_ID}", roots["subjectRoot"])
        self.assertEqual(
            f"artifacts/subjects/{TRACE_SUBJECT_ID}/runs",
            roots["runsRoot"],
        )
        self.assertEqual(
            f"artifacts/subjects/{TRACE_SUBJECT_ID}/runs/{run_id}",
            roots["runRoot"],
        )
        self.assertEqual(
            f"artifacts/subjects/{TRACE_SUBJECT_ID}/runs/{run_id}/analysis",
            roots["analysisRoot"],
        )
        self.assertEqual(
            f"artifacts/subjects/{TRACE_SUBJECT_ID}/runs/{run_id}/matrices/{TRACE_MATRIX_ID}",
            roots["matrixRoot"],
        )
        self.assertEqual(
            f"artifacts/subjects/{TRACE_SUBJECT_ID}/runs/{run_id}/matrices/{TRACE_MATRIX_ID}/pipeline-report",
            roots["pipelineReportRoot"],
        )

    def test_subject_content_roots_include_validation_expected_and_baselines(self) -> None:
        resolver_module = load_module(PATH_RESOLVER_MODULE_PATH, "chaos_path_resolver_subject_content_roots")

        roots = resolver_module.subject_content_roots(REPO_ROOT, TRACE_SUBJECT_ID)

        self.assertEqual(
            REPO_ROOT / "subjects" / TRACE_SUBJECT_ID / "validation",
            roots["validationRoot"],
        )
        self.assertEqual(
            REPO_ROOT / "subjects" / TRACE_SUBJECT_ID / "expected",
            roots["expectedRoot"],
        )
        self.assertEqual(
            REPO_ROOT / "subjects" / TRACE_SUBJECT_ID / "baselines",
            roots["baselinesRoot"],
        )
        self.assertEqual(
            REPO_ROOT / "subjects" / PERF_SUBJECT_ID / "baselines" / "perf" / PERF_MATRIX_ID / "windows.json",
            resolver_module.subject_perf_baseline_path(
                REPO_ROOT,
                PERF_SUBJECT_ID,
                PERF_MATRIX_ID,
                "windows",
            ),
        )

    def test_subject_root_does_not_rewrite_legacy_archetype_aliases(self) -> None:
        resolver_module = load_module(PATH_RESOLVER_MODULE_PATH, "chaos_path_resolver_without_legacy_archetype_alias")

        self.assertEqual(
            REPO_ROOT / "subjects" / LEGACY_ARCHETYPE_SUBJECT_ID,
            resolver_module.subject_root(REPO_ROOT, LEGACY_ARCHETYPE_SUBJECT_ID),
        )
        self.assertEqual(
            REPO_ROOT / "subjects" / LEGACY_ARCHETYPE_SUBJECT_ID / "subject.manifest.json",
            resolver_module.subject_manifest_path(REPO_ROOT, LEGACY_ARCHETYPE_SUBJECT_ID),
        )


if __name__ == "__main__":
    unittest.main()
