from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"


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


class SubjectManifestSchemaTests(unittest.TestCase):
    def test_subject_manifest_loader_discovers_helloworld_canonical_subject(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "booming_subject_manifest_schema")

        manifest_paths = subjects_module.discover_subject_manifests(REPO_ROOT)
        self.assertIn(
            REPO_ROOT / "subjects" / "HelloWorldObject" / "subject.manifest.json",
            manifest_paths,
        )

        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "HelloWorldObject")

        self.assertEqual("HelloWorldObject", manifest["subjectId"])
        self.assertEqual("HelloWorldObject", manifest["displayName"])
        self.assertEqual("correctness.dev", manifest["defaultGoal"])
        self.assertEqual("windows-dev-output", manifest["defaultMatrix"])
        self.assertEqual("dotnet-project", manifest["source"]["type"])
        self.assertEqual(
            "subjects/HelloWorldObject/source/HelloWorldObject.csproj",
            manifest["source"]["path"],
        )
        self.assertEqual(
            "HelloWorldObject/Program::Main(System.String[])",
            manifest["source"]["entry"],
        )
        self.assertEqual(
            {
                "windows-dev-output",
                "windows-reference-trace",
                "windows-android-buildable",
                "windows-linux-buildable",
            },
            {item["matrixId"] for item in manifest["environmentMatrices"]},
        )

    def test_subject_artifact_roots_and_bucket_paths_follow_subject_layout(self) -> None:
        subjects_module = load_module(SUBJECTS_MODULE_PATH, "booming_subject_manifest_paths")

        roots = subjects_module.subject_artifact_roots("HelloWorldObject", "windows-reference-trace")
        self.assertEqual("artifacts/subjects/HelloWorldObject", roots["subjectRoot"])
        self.assertEqual("artifacts/subjects/HelloWorldObject/shared", roots["sharedRoot"])
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace",
            roots["matrixRoot"],
        )
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/subject-report",
            roots["subjectReportRoot"],
        )

        analysis_paths = subjects_module.stage_paths(
            "HelloWorldObject",
            "windows-reference-trace",
            bucket="analysis",
            scope="shared",
            kind="analysis-frontend",
        )
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
            analysis_paths["manifestPath"],
        )
        self.assertEqual(
            ["artifacts/subjects/HelloWorldObject/shared/analysis/contract-validate.report.json"],
            analysis_paths["reportPaths"],
        )

        generated_paths = subjects_module.stage_paths(
            "HelloWorldObject",
            "windows-reference-trace",
            bucket="generated",
            scope="shared",
            kind="generated-native-proof",
        )
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/shared/generated/generated.manifest.json",
            generated_paths["manifestPath"],
        )

        build_paths = subjects_module.stage_paths(
            "HelloWorldObject",
            "windows-reference-trace",
            bucket="build",
            scope="matrix",
            kind="build-target",
        )
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/build/build.manifest.json",
            build_paths["manifestPath"],
        )

        runtime_paths = subjects_module.stage_paths(
            "HelloWorldObject",
            "windows-reference-trace",
            bucket="runtime",
            scope="matrix",
            kind="runtime-trace-compare",
        )
        self.assertEqual(
            "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/runtime.manifest.json",
            runtime_paths["manifestPath"],
        )
        self.assertEqual(
            ["artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/trace-compare.report.json"],
            runtime_paths["reportPaths"],
        )


if __name__ == "__main__":
    unittest.main()
