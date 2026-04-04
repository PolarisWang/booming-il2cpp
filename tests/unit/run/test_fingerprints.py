from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
FINGERPRINTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "fingerprints.py"


def load_fingerprints_module():
    if not FINGERPRINTS_MODULE_PATH.is_file():
        raise FileNotFoundError(f"fingerprints module missing: {FINGERPRINTS_MODULE_PATH}")

    spec = importlib.util.spec_from_file_location("booming_run_fingerprints", FINGERPRINTS_MODULE_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load fingerprints module: {FINGERPRINTS_MODULE_PATH}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class FingerprintsTests(unittest.TestCase):
    def test_fingerprint_build_cache_uses_controlled_layout_and_current_pointer(self) -> None:
        fingerprints_module = load_fingerprints_module()

        with tempfile.TemporaryDirectory() as temp_dir:
            repo_root = Path(temp_dir)
            result = fingerprints_module.materialize_fingerprint(
                repo_root=repo_root,
                host_platform="macos",
                family="smoke",
                suite="hello-world",
                fingerprint_inputs={"sourceRoots": ["tests/smoke/hello-world"], "toolchainFiles": ["global.json"]},
            )

            self.assertIn("/artifacts/tests/build/macos/smoke/hello-world/fingerprints/", result["fingerprintDir"])
            current_payload = json.loads((repo_root / result["currentPath"]).read_text(encoding="utf-8"))
            self.assertEqual(result["fingerprint"], current_payload["fingerprint"])
            self.assertEqual(result["fingerprintDir"], current_payload["fingerprintDir"])


if __name__ == "__main__":
    unittest.main()
