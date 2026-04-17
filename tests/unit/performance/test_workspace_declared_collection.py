from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import time
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
WORKSPACE_DECLARED_COLLECTION_MODULE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "workspace_declared_collection.py"
)
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "workspace-declared-collection"


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


class WorkspaceDeclaredCollectionTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self) -> Path:
        repo_root = TEST_TMP_ROOT / uuid.uuid4().hex
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def _write_subject_and_workspace_fixture(self, repo_root: Path) -> Path:
        subject_id = "SolutionCorePack"
        source_root = repo_root / "subjects" / subject_id / "source"
        benchmark_path = source_root / "Benchmarks" / "CoreRuntimeBenchmarks" / "AllocationBenchmark.cs"
        benchmark_path.parent.mkdir(parents=True, exist_ok=True)
        benchmark_path.write_text("// benchmark source\n", encoding="utf-8")

        subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
        subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
        subject_manifest_path.write_text(
            json.dumps(
                {
                    "subjectId": subject_id,
                    "source": {
                        "path": "subjects/SolutionCorePack/source/SolutionCorePack.sln",
                    },
                },
                ensure_ascii=False,
                indent=2,
            )
            + "\n",
            encoding="utf-8",
        )
        (source_root / "SolutionCorePack.sln").write_text("Microsoft Visual Studio Solution File\n", encoding="utf-8")

        workspace_root = repo_root / "solutions" / "subjects" / subject_id
        generated_root = workspace_root / "managed-tests" / "Generated"
        generated_root.mkdir(parents=True, exist_ok=True)
        collection_path = generated_root / "declared-tests.collection.json"
        collection_path.write_text(
            json.dumps(
                {
                    "subjectId": subject_id,
                    "declaredBenchmarks": [{"stableId": "allocation", "alias": "allocation-bench", "modes": 1}],
                },
                ensure_ascii=False,
                indent=2,
            )
            + "\n",
            encoding="utf-8",
        )

        workspace_manifest_path = workspace_root / "workspace.manifest.json"
        workspace_manifest_path.write_text(
            json.dumps(
                {
                    "workspaceVersion": 2,
                    "kind": "subject-workspace",
                    "subjectId": subject_id,
                    "managedTestProjects": [
                        {
                            "projectId": "managed-test/SolutionCorePack/benchmark-host",
                            "hostKind": "benchmark-host",
                            "collectionPath": "solutions/subjects/SolutionCorePack/managed-tests/Generated/declared-tests.collection.json",
                        }
                    ],
                },
                ensure_ascii=False,
                indent=2,
            )
            + "\n",
            encoding="utf-8",
        )
        return benchmark_path

    def test_load_workspace_declared_collection_returns_collection_when_source_inputs_are_not_newer(self) -> None:
        module = load_module(
            WORKSPACE_DECLARED_COLLECTION_MODULE_PATH,
            "chaos_workspace_declared_collection_fresh",
        )
        repo_root = self._make_repo_root()

        try:
            self._write_subject_and_workspace_fixture(repo_root)
            collection = module.load_workspace_declared_collection(
                repo_root,
                "SolutionCorePack",
                host_kind="benchmark-host",
            )
            self.assertIsNotNone(collection)
            self.assertEqual("allocation-bench", collection["declaredBenchmarks"][0]["alias"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_load_workspace_declared_collection_returns_none_when_subject_source_is_newer_than_collection(self) -> None:
        module = load_module(
            WORKSPACE_DECLARED_COLLECTION_MODULE_PATH,
            "chaos_workspace_declared_collection_stale",
        )
        repo_root = self._make_repo_root()

        try:
            benchmark_path = self._write_subject_and_workspace_fixture(repo_root)
            time.sleep(1.1)
            benchmark_path.write_text("// source changed after catalog\n", encoding="utf-8")

            collection = module.load_workspace_declared_collection(
                repo_root,
                "SolutionCorePack",
                host_kind="benchmark-host",
            )
            self.assertIsNone(collection)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
