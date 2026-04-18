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


if __name__ == "__main__":
    unittest.main()
