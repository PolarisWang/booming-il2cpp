from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_COMMAND_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "test.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "test-command-workspace"


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


class TestCommandWorkspaceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def test_resolve_subject_matrix_report_path_prefers_matrix_pipeline_report_root_over_entry_report_path(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_test_command_matrix_report_path")

        plan = {
            "artifactsRoot": {
                "entryReportPath": (
                    "artifacts/subjects/SolutionCorePack/runs/20260420-fixture/"
                    "matrices/windows-corelib-reference-native-hotupdate-proof/"
                    "declared/unit/corelib-reference-hotupdate-proof/report.json"
                ),
                "pipelineReportRoot": (
                    "artifacts/subjects/SolutionCorePack/runs/20260420-fixture/"
                    "matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report"
                ),
            },
            "stagePlan": [
                {
                    "bucket": "report",
                    "paths": {
                        "manifestPath": (
                            "artifacts/subjects/SolutionCorePack/runs/20260420-fixture/"
                            "matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report.json"
                        )
                    },
                }
            ],
        }

        self.assertEqual(
            (
                "artifacts/subjects/SolutionCorePack/runs/20260420-fixture/"
                "matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report.json"
            ),
            test_module._resolve_subject_matrix_report_path(plan),
        )

    def test_regenerate_subject_workspace_manifest_passes_declared_entry_selection_and_matrix_to_workspace_generator(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_test_command_workspace_regeneration")
        repo_root = self._make_repo_root("workspace-regeneration")
        subject_id = "FixtureSubject"
        workspace_manifest_path = repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json"
        selected_object = {
            "type": "declared-unit-test",
            "subjectId": subject_id,
            "defaultMatrixId": "windows-native-check",
            "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
            "alias": "fixture-proof",
            "entryIndex": 7,
        }
        normalized_options = {
            "variant": "CHECK",
        }

        try:
            def generate_subject_workspace_side_effect(
                repo_root_arg: Path,
                host_platform_arg: str,
                options_arg: dict[str, object],
                **kwargs: object,
            ) -> dict[str, object]:
                del kwargs
                self.assertEqual(repo_root, repo_root_arg)
                self.assertEqual("windows", host_platform_arg)
                self.assertEqual(f"subject/{subject_id}", options_arg["id"])
                self.assertEqual("windows-native-check", options_arg["matrix"])
                self.assertEqual("CHECK", options_arg["variant"])
                self.assertTrue(bool(options_arg["refresh-generated"]))
                self.assertTrue(bool(options_arg["auto-refresh-missing-generated"]))
                self.assertEqual(
                    {
                        "family": "declared-unit-test",
                        "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                        "alias": "fixture-proof",
                        "entryIndex": 7,
                    },
                    options_arg["entry-selection"],
                )
                self.assertNotIn("all-targets", options_arg)

                workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
                workspace_manifest_path.write_text(
                    json.dumps(
                        {
                            "workspaceVersion": 2,
                            "subjectId": subject_id,
                            "matrices": [],
                        }
                    ),
                    encoding="utf-8",
                )
                return {
                    "manifestPath": f"solutions/subjects/{subject_id}/workspace.manifest.json",
                }

            with patch.object(
                test_module.project_workspace_module,
                "generate_subject_workspace",
                side_effect=generate_subject_workspace_side_effect,
            ):
                manifest_path, manifest = test_module._regenerate_subject_workspace_manifest(
                    repo_root,
                    subject_id=subject_id,
                    host_platform="windows",
                    selected_object=selected_object,
                    normalized_options=normalized_options,
                )

            self.assertEqual(workspace_manifest_path, manifest_path)
            self.assertEqual(subject_id, manifest["subjectId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_resolve_workspace_execution_regenerates_manifest_when_declared_entry_is_missing_from_workspace_collection(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_test_command_workspace_missing_declared_entry")
        repo_root = self._make_repo_root("workspace-missing-declared-entry")
        subject_id = "FixtureSubject"
        stable_id = f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()"
        workspace_manifest_path = repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json"
        collection_path = repo_root / "solutions" / "subjects" / subject_id / "managed-tests" / "Generated" / "declared-tests.collection.json"
        selected_object = {
            "type": "declared-unit-test",
            "subjectId": subject_id,
            "defaultMatrixId": "windows-native-check",
            "defaultGoalId": "correctness.dev",
            "stableId": stable_id,
            "alias": "fixture-proof",
            "entryIndex": 7,
        }
        normalized_options = {}

        initial_manifest = {
            "workspaceVersion": 2,
            "subjectId": subject_id,
            "defaultMatrixId": "windows-native-check",
            "managedTestProjects": [
                {
                    "projectId": f"managed-test/{subject_id}/proof-host",
                    "projectPath": f"solutions/subjects/{subject_id}/managed-tests/{subject_id}.DeclaredProofHost.csproj",
                    "assemblyName": f"{subject_id}.DeclaredProofHost",
                    "hostKind": "proof-host",
                    "collectionPath": f"solutions/subjects/{subject_id}/managed-tests/Generated/declared-tests.collection.json",
                    "generatedSourcePath": f"solutions/subjects/{subject_id}/managed-tests/Generated/ChaosGeneratedDeclaredTests.g.cs",
                }
            ],
            "matrices": [
                {
                    "matrixId": "windows-native-check",
                    "goalIds": ["correctness.dev"],
                    "hostPlatform": "windows-x64",
                }
            ],
        }
        refreshed_manifest = dict(initial_manifest)

        try:
            workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            workspace_manifest_path.write_text(json.dumps(initial_manifest), encoding="utf-8")
            collection_path.parent.mkdir(parents=True, exist_ok=True)
            collection_path.write_text(json.dumps({"declaredUnitTests": []}), encoding="utf-8")

            def regenerate_side_effect(
                repo_root_arg: Path,
                *,
                subject_id: str,
                host_platform: str,
                selected_object: dict[str, object],
                normalized_options: dict[str, object],
            ) -> tuple[Path, dict[str, object]]:
                del selected_object, normalized_options
                self.assertEqual(repo_root, repo_root_arg)
                self.assertEqual("FixtureSubject", subject_id)
                self.assertEqual("windows", host_platform)
                collection_path.write_text(
                    json.dumps(
                        {
                            "declaredUnitTests": [
                                {
                                    "stableId": stable_id,
                                    "entryIndex": 5,
                                    "alias": "fixture-proof",
                                }
                            ]
                        }
                    ),
                    encoding="utf-8",
                )
                return workspace_manifest_path, refreshed_manifest

            with patch.object(
                test_module,
                "_regenerate_subject_workspace_manifest",
                side_effect=regenerate_side_effect,
            ) as regenerate_mock:
                execution = test_module._resolve_workspace_execution(
                    repo_root,
                    selected_object=selected_object,
                    normalized_options=normalized_options,
                    host_platform="windows",
                )

            self.assertEqual(5, execution["entryIndex"])
            self.assertEqual(
                f"solutions/subjects/{subject_id}/managed-tests/Generated/declared-tests.collection.json",
                execution["collectionPath"],
            )
            regenerate_mock.assert_called_once()
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_resolve_workspace_execution_prefers_hotupdate_test_project_for_hotupdate_declared_unit_test(self) -> None:
        test_module = load_module(TEST_COMMAND_MODULE_PATH, "chaos_test_command_workspace_hotupdate_host")
        repo_root = self._make_repo_root("workspace-hotupdate-host")
        subject_id = "FixtureSubject"
        stable_id = f"{subject_id}::FixturePatch::FixturePatch.Proofs::Run()"
        workspace_manifest_path = repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json"
        hotupdate_collection_path = repo_root / "solutions" / "subjects" / subject_id / "hotupdate-tests" / "Generated" / "declared-tests.collection.json"
        selected_object = {
            "type": "declared-unit-test",
            "subjectId": subject_id,
            "defaultMatrixId": "windows-hotupdate-proof",
            "defaultGoalId": "correctness.dev",
            "stableId": stable_id,
            "alias": "fixture-hotupdate-proof",
            "entryIndex": 4,
            "hotUpdateCapability": 1,
        }
        normalized_options = {}

        manifest = {
            "workspaceVersion": 2,
            "subjectId": subject_id,
            "defaultMatrixId": "windows-hotupdate-proof",
            "managedTestProjects": [
                {
                    "projectId": f"managed-test/{subject_id}/proof-host",
                    "projectPath": f"solutions/subjects/{subject_id}/managed-tests/{subject_id}.DeclaredProofHost.csproj",
                    "assemblyName": f"{subject_id}.DeclaredProofHost",
                    "hostKind": "proof-host",
                    "collectionPath": f"solutions/subjects/{subject_id}/managed-tests/Generated/declared-tests.collection.json",
                    "generatedSourcePath": f"solutions/subjects/{subject_id}/managed-tests/Generated/ChaosGeneratedDeclaredTests.g.cs",
                }
            ],
            "hotupdateTestProjects": [
                {
                    "projectId": f"hotupdate-test/{subject_id}/proof-host",
                    "projectPath": f"solutions/subjects/{subject_id}/hotupdate-tests/{subject_id}.HotUpdateProofHost.csproj",
                    "assemblyName": f"{subject_id}.HotUpdateProofHost",
                    "hostKind": "proof-host",
                    "collectionPath": f"solutions/subjects/{subject_id}/hotupdate-tests/Generated/declared-tests.collection.json",
                    "bindingManifestPath": f"solutions/subjects/{subject_id}/hotupdate-tests/Generated/declared-tests.binding.json",
                    "generatedSourcePath": f"solutions/subjects/{subject_id}/hotupdate-tests/Generated/ChaosGeneratedHotUpdateProofHost.g.cs",
                }
            ],
            "matrices": [
                {
                    "matrixId": "windows-hotupdate-proof",
                    "goalIds": ["correctness.dev"],
                    "hostPlatform": "windows-x64",
                }
            ],
        }

        try:
            workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            workspace_manifest_path.write_text(json.dumps(manifest), encoding="utf-8")
            hotupdate_collection_path.parent.mkdir(parents=True, exist_ok=True)
            hotupdate_collection_path.write_text(
                json.dumps(
                    {
                        "declaredUnitTests": [
                            {
                                "stableId": stable_id,
                                "entryIndex": 4,
                                "alias": "fixture-hotupdate-proof",
                            }
                        ]
                    }
                ),
                encoding="utf-8",
            )

            execution = test_module._resolve_workspace_execution(
                repo_root,
                selected_object=selected_object,
                normalized_options=normalized_options,
                host_platform="windows",
            )

            self.assertEqual(
                f"solutions/subjects/{subject_id}/hotupdate-tests/Generated/declared-tests.collection.json",
                execution["collectionPath"],
            )
            self.assertEqual(
                f"solutions/subjects/{subject_id}/hotupdate-tests/Generated/declared-tests.binding.json",
                execution["bindingManifestPath"],
            )
            self.assertEqual(f"hotupdate-test/{subject_id}/proof-host", execution["managedTestProject"]["projectId"])
            self.assertEqual(4, execution["entryIndex"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
