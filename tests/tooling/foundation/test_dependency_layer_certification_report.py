from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PROJECT_PATH = REPO_ROOT / "src" / "tools" / "Chaos.IL2CPP.Tools.FoundationPack.Analysis" / "Chaos.IL2CPP.Tools.FoundationPack.Analysis.csproj"
ASSEMBLY_PLAN_DIR = (
    REPO_ROOT
    / "docs"
    / "dev"
    / "completed"
    / "20260418-06-dependency-driven-core-bcl-layer-nativeization"
    / "assembly-nativeization-plan"
)
PROOF_SUMMARY_PATH = (
    REPO_ROOT
    / "docs"
    / "dev"
    / "completed"
    / "20260418-10-core-bcl-layer-01-priority-batch-execution"
    / "native-proof-summary-v1-01.json"
)
BENCHMARK_SUMMARY_PATH = (
    REPO_ROOT
    / "docs"
    / "dev"
    / "completed"
    / "20260418-10-core-bcl-layer-01-priority-batch-execution"
    / "nativeization-throughput-benchmark-v1-01.json"
)
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "foundation-pack-certification-report"


class DependencyLayerCertificationReportTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def setUp(self) -> None:
        self.output_dir = TEST_TMP_ROOT / f"run-{uuid.uuid4().hex}"
        self.output_dir.mkdir(parents=True, exist_ok=False)

    def tearDown(self) -> None:
        shutil.rmtree(self.output_dir, ignore_errors=True)

    def test_generator_projects_priority_batch_evidence_into_per_dll_reports(self) -> None:
        completed = subprocess.run(
            [
                "dotnet",
                "run",
                "--project",
                str(PROJECT_PATH),
                "--",
                "dependency-layer-certify",
                "--assembly-plan-dir",
                str(ASSEMBLY_PLAN_DIR),
                "--proof-summary",
                str(PROOF_SUMMARY_PATH),
                "--benchmark-summary",
                str(BENCHMARK_SUMMARY_PATH),
                "--output-dir",
                str(self.output_dir),
                "--task-id",
                "dependency-layer-certify-test",
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
            check=False,
        )

        self.assertEqual(
            0,
            completed.returncode,
            msg=f"stdout:\n{completed.stdout}\n\nstderr:\n{completed.stderr}",
        )

        report_paths = sorted(self.output_dir.glob("*.json"))
        self.assertEqual(8, len(report_paths))

        report = json.loads((self.output_dir / "System.Collections.json").read_text(encoding="utf-8"))
        self.assertEqual("dependency-layer-certify-test", report["taskId"])
        self.assertEqual("20260418-10-core-bcl-layer-01-priority-batch-execution", report["sourceTaskId"])
        self.assertEqual("System.Collections", report["assemblyName"])
        self.assertEqual("SolutionCorePack", report["ownerSubjectId"])
        self.assertEqual(["net10.0", "net8.0"], sorted(report["targetFrameworks"]))
        self.assertEqual([], report["blockedReasons"])
        self.assertEqual("ok", report["finalStatus"])
        self.assertEqual("throughput-only", report["benchmarkAcceptance"])
        self.assertEqual(5, len(report["gateResults"]))
        self.assertGreater(len(report["nativeProofResults"]), 0)
        self.assertGreater(len(report["benchmarkResults"]), 0)
        self.assertGreater(len(report["engineeringValidationResults"]), 0)
        self.assertGreater(len(report["engineeringWorkloadResults"]), 0)
        self.assertTrue(report["evidencePaths"]["assemblyPlanPath"].endswith("System.Collections.json"))
        self.assertTrue(report["evidencePaths"]["proofSummaryPath"].endswith("native-proof-summary-v1-01.json"))
        self.assertTrue(report["evidencePaths"]["benchmarkSummaryPath"].endswith("nativeization-throughput-benchmark-v1-01.json"))


if __name__ == "__main__":
    unittest.main()
