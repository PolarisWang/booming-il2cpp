"""Smoke tests for .github/scripts/ci_provenance.py

TARGET: ci_provenance.py build_manifest

Verifies:
  - basic manifest structure with a file artifact
  - directory fingerprinting
  - missing artifacts recorded as kind=missing
  - environment variable plumbing (run id, stage, profile)
"""
from __future__ import annotations

import importlib.util
import json
import os
import sys
import tempfile
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
PROVENANCE_PATH = REPO_ROOT / ".github" / "scripts" / "ci_provenance.py"

_loader = importlib.util.spec_from_file_location("ci_provenance", str(PROVENANCE_PATH))
_mod = importlib.util.module_from_spec(_loader)
_loader.loader.exec_module(_mod)


class BuildManifestTests(unittest.TestCase):

    def setUp(self):
        self.td = tempfile.TemporaryDirectory()
        self.root = Path(self.td.name)
        self.data_file = self.root / "data.bin"
        self.data_file.write_text("hello")
        self.nested = self.root / "sub" / "dir"
        self.nested.mkdir(parents=True)
        (self.nested / "a.txt").write_text("a")
        (self.nested / "b.txt").write_text("bb")

    def tearDown(self):
        self.td.cleanup()

    # ── helpers ──

    def _manifest(self, artifacts, env=None):
        env = env or {}
        old = {k: os.environ.get(k) for k in env}
        for k, v in env.items():
            if v is None:
                os.environ.pop(k, None)
            else:
                os.environ[k] = v
        try:
            return _mod.build_manifest(self.root, artifacts)
        finally:
            for k in env:
                if old.get(k) is None:
                    os.environ.pop(k, None)
                else:
                    os.environ[k] = old[k]

    # ── tests ──

    def test_minimal_manifest(self):
        m = self._manifest([])
        self.assertEqual(m["schemaVersion"], 1)
        self.assertIn("generatedAt", m)
        self.assertEqual(m["artifacts"], [])

    def test_file_artifact(self):
        m = self._manifest([("data", self.data_file)])
        a = m["artifacts"][0]
        self.assertEqual(a["name"], "data")
        self.assertEqual(a["kind"], "file")
        self.assertEqual(a["size_bytes"], 5)
        self.assertEqual(len(a["sha256"]), 64)

    def test_directory_artifact(self):
        m = self._manifest([("sub", self.nested)])
        a = m["artifacts"][0]
        self.assertEqual(a["kind"], "directory")
        self.assertEqual(a["file_count"], 2)
        self.assertEqual(a["total_bytes"], 3)
        self.assertIsInstance(a["fingerprint"], str)
        self.assertFalse(a["truncated"])

    def test_missing_artifact(self):
        m = self._manifest([("nope", self.root / "nope.bin")])
        a = m["artifacts"][0]
        self.assertEqual(a["name"], "nope")
        self.assertEqual(a["kind"], "missing")

    def test_run_env_vars(self):
        m = self._manifest([], env={
            "GITHUB_RUN_ID": "42",
            "GITHUB_RUN_ATTEMPT": "3",
            "GITHUB_JOB": "test-job",
            "CHAOS_PROVENANCE_STAGE": "gc-unit",
            "CHAOS_PROVENANCE_PROFILE": "pr",
        })
        r = m["run"]
        self.assertEqual(r["runId"], 42)
        self.assertIsInstance(r["runId"], int)
        self.assertEqual(r["runAttempt"], 3)
        self.assertEqual(r["job"], "test-job")
        self.assertEqual(r["stage"], "gc-unit")
        self.assertEqual(r["profile"], "pr")

    def test_deterministic_fingerprint(self):
        a1 = _mod.build_manifest(self.root, [("sub", self.nested)])
        a2 = _mod.build_manifest(self.root, [("sub", self.nested)])
        self.assertEqual(
            a1["artifacts"][0]["fingerprint"],
            a2["artifacts"][0]["fingerprint"],
        )


if __name__ == "__main__":
    unittest.main()