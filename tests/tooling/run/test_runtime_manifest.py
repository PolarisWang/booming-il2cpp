from __future__ import annotations

import importlib.util
import json
import unittest
from pathlib import Path
from unittest import mock


REPO_ROOT = Path(__file__).resolve().parents[3]
RUNTIME_PY = REPO_ROOT / "build" / "toolchains" / "run" / "runtime.py"
MANIFEST_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "runtime_manifest.json"


def load_runtime_module():
    if not RUNTIME_PY.is_file():
        raise FileNotFoundError(f"runtime module missing: {RUNTIME_PY}")

    spec = importlib.util.spec_from_file_location("booming_run_runtime", RUNTIME_PY)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load runtime module: {RUNTIME_PY}")

    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class RuntimeManifestTests(unittest.TestCase):
    def test_manifest_locks_provider_and_python_version(self) -> None:
        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))

        self.assertEqual("python-build-standalone", manifest["provider"])
        self.assertEqual("20260303", manifest["provider_release"])
        self.assertEqual("3.12.13", manifest["python_version"])
        self.assertEqual("artifacts/toolchains/python", manifest["cache_root"])

    def test_manifest_platform_entries_include_required_fields(self) -> None:
        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        platforms = manifest["platforms"]

        self.assertGreaterEqual(len(platforms), 4)

        required = {
            "platform_id",
            "download_url",
            "sha256",
            "archive_type",
            "extract_dir",
            "python_relpath",
        }

        for platform_id, entry in platforms.items():
            self.assertEqual(platform_id, entry["platform_id"])
            self.assertTrue(required.issubset(entry.keys()))
            self.assertTrue(entry["download_url"].startswith("https://"))
            self.assertEqual(64, len(entry["sha256"]))

    def test_runtime_module_maps_cache_root_and_python_paths(self) -> None:
        runtime = load_runtime_module()
        manifest = runtime.load_runtime_manifest(REPO_ROOT, MANIFEST_PATH)

        self.assertEqual(
            REPO_ROOT / "artifacts" / "toolchains" / "python",
            runtime.get_cache_root(REPO_ROOT, manifest),
        )

        self.assertEqual(
            REPO_ROOT
            / "artifacts"
            / "toolchains"
            / "python"
            / "windows-x64"
            / "3.12.13"
            / "python"
            / "python.exe",
            runtime.get_runtime_python_path(REPO_ROOT, manifest, "windows-x64"),
        )

        self.assertEqual(
            REPO_ROOT
            / "artifacts"
            / "toolchains"
            / "python"
            / "linux-x64"
            / "3.12.13"
            / "python"
            / "bin"
            / "python3.12",
            runtime.get_runtime_python_path(REPO_ROOT, manifest, "linux-x64"),
        )

        self.assertEqual(
            REPO_ROOT / "artifacts" / "toolchains" / "python" / "current.json",
            runtime.get_current_state_path(REPO_ROOT, manifest),
        )

    def test_tar_gz_extraction_uses_explicit_filter(self) -> None:
        runtime = load_runtime_module()
        archive_path = Path("runtime.tar.gz")
        destination = Path("extract")

        archive_context = mock.MagicMock()
        archive_context.__enter__.return_value = archive_context
        archive_context.__exit__.return_value = False

        with (
            mock.patch.object(runtime.tarfile, "open", return_value=archive_context) as open_mock,
            mock.patch.object(Path, "exists", return_value=False),
            mock.patch.object(Path, "mkdir"),
        ):
            runtime.extract_archive(archive_path, "tar.gz", destination)

        open_mock.assert_called_once_with(archive_path, "r:gz")
        archive_context.extractall.assert_called_once_with(destination, filter="fully_trusted")


if __name__ == "__main__":
    unittest.main()
