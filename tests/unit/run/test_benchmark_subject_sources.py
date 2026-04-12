from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
BENCH_SUBJECT_ROOT = REPO_ROOT / "subjects"
SHARED_PERF_PROJECT_PATH = "src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj"
EXPECTED_HARNESS_ITERATIONS = {
    "BenchArithmetic": 2,
    "BenchAllocation": 3,
    "BenchDelegate": 5,
    "BenchDispatch": 6,
    "BenchException": 6,
    "BenchGameLoop": 3,
    "BenchGeneric": 2,
    "BenchHotUpdateDispatch": 45,
    "BenchHotUpdateLoad": 8,
    "BenchHotUpdateRoundtrip": 30,
    "BenchMixed": 6,
}


class BenchmarkSubjectSourceTests(unittest.TestCase):
    def test_all_bench_subjects_keep_perf_analysis_outside_subject_source(self) -> None:
        manifests = sorted(BENCH_SUBJECT_ROOT.glob("Bench*/subject.manifest.json"))
        self.assertEqual(set(EXPECTED_HARNESS_ITERATIONS), {path.parent.name for path in manifests})
        self.assertTrue((REPO_ROOT / SHARED_PERF_PROJECT_PATH).is_file())

        for manifest_path in manifests:
            subject_id = manifest_path.parent.name
            source_path = manifest_path.parent / "source" / "Program.cs"
            self.assertTrue(source_path.is_file(), msg=f"missing source: {source_path}")

            source = source_path.read_text(encoding="utf-8")
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            workload_entry = f"{subject_id}/Program::RunWorkload()"

            self.assertNotIn("Stopwatch", source, msg=subject_id)
            self.assertNotIn("elapsedMilliseconds", source, msg=subject_id)
            self.assertNotIn("opsPerSecond", source, msg=subject_id)
            self.assertNotIn("framesPerSecond", source, msg=subject_id)
            self.assertNotIn("roundtripsPerSecond", source, msg=subject_id)
            self.assertNotIn("loadDurationMs", source, msg=subject_id)
            self.assertNotIn("string[] args", source, msg=subject_id)
            self.assertNotIn("args.Length", source, msg=subject_id)
            self.assertNotIn("Console.WriteLine", source, msg=subject_id)
            self.assertIn("RunWorkload", source, msg=subject_id)

            self.assertEqual(f"{subject_id}/Program::Main()", manifest["source"]["entry"])
            self.assertEqual(workload_entry, manifest["workloadEntry"])
            self.assertEqual(EXPECTED_HARNESS_ITERATIONS[subject_id], manifest["validation"]["perf"]["harnessIterations"])
            self.assertEqual(SHARED_PERF_PROJECT_PATH, manifest["validation"]["perf"]["project"])
            self.assertNotIn("Program::Main(System.String[])", json.dumps(manifest, ensure_ascii=False))

            for pipeline in manifest["executionPipelines"]:
                pipeline_id = pipeline["pipelineId"]
                stage_kinds = {stage["kind"] for stage in pipeline["stages"]}
                if pipeline_id in {"native-benchmark", "mobile-native-benchmark"}:
                    self.assertIn("generated-native-aot", stage_kinds, msg=subject_id)
                    self.assertNotIn("generated-native-proof", stage_kinds, msg=subject_id)

            for matrix in manifest["environmentMatrices"]:
                if matrix["pipelineId"] in {"native-benchmark", "mobile-native-benchmark"}:
                    self.assertEqual(workload_entry, matrix["source"]["entry"], msg=subject_id)


if __name__ == "__main__":
    unittest.main()
