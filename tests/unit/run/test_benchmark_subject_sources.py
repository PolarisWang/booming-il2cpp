from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
BENCH_ARITHMETIC_PROGRAM_PATH = REPO_ROOT / "subjects" / "BenchArithmetic" / "source" / "Program.cs"
BENCH_ARITHMETIC_MANIFEST_PATH = REPO_ROOT / "subjects" / "BenchArithmetic" / "subject.manifest.json"


class BenchmarkSubjectSourceTests(unittest.TestCase):
    def test_bench_arithmetic_keeps_perf_analysis_outside_subject_source(self) -> None:
        source = BENCH_ARITHMETIC_PROGRAM_PATH.read_text(encoding="utf-8")
        manifest = json.loads(BENCH_ARITHMETIC_MANIFEST_PATH.read_text(encoding="utf-8"))

        self.assertNotIn("Stopwatch", source)
        self.assertNotIn("elapsedMilliseconds", source)
        self.assertNotIn("opsPerSecond", source)
        self.assertNotIn("TryParse", source)
        self.assertNotIn("string[] args", source)
        self.assertNotIn("args.Length", source)
        self.assertIn("RunWorkload", source)
        self.assertEqual("BenchArithmetic/Program::Main()", manifest["source"]["entry"])
        self.assertEqual("BenchArithmetic/Program::RunWorkload()", manifest["workloadEntry"])
        self.assertNotIn("Program::Main(System.String[])", json.dumps(manifest, ensure_ascii=False))
        native_matrix = next(matrix for matrix in manifest["environmentMatrices"] if matrix["matrixId"] == "windows-native-perf")
        mobile_matrix = next(matrix for matrix in manifest["environmentMatrices"] if matrix["matrixId"] == "android-arm64-native-perf")
        self.assertEqual("BenchArithmetic/Program::RunWorkload()", native_matrix["source"]["entry"])
        self.assertEqual("BenchArithmetic/Program::RunWorkload()", mobile_matrix["source"]["entry"])
        self.assertTrue(
            str(dict(manifest["validation"]["perf"]).get("project") or "").startswith(
                "subjects/BenchArithmetic/validation/perf/"
            )
        )


if __name__ == "__main__":
    unittest.main()
