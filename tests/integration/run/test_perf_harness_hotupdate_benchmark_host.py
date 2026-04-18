from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
HARNESS_PROJECT_PATH = (
    REPO_ROOT
    / "src"
    / "tools"
    / "Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness"
    / "Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.csproj"
)
HOTUPDATE_BENCHMARK_HOST_PROJECT_PATH = (
    REPO_ROOT
    / "solutions"
    / "subjects"
    / "HotUpdateHostPack"
    / "hotupdate-tests"
    / "HotUpdateHostPack.HotUpdateBenchmarkHost.csproj"
)
HOTUPDATE_SUBJECT_PROJECT_PATH = (
    REPO_ROOT
    / "subjects"
    / "HotUpdateHostPack"
    / "source"
    / "HotUpdateHostPack.csproj"
)
HOTUPDATE_PATCH_PROJECT_PATH = (
    REPO_ROOT
    / "subjects"
    / "HotUpdateHostPack"
    / "source"
    / "EngineeringScenarios"
    / "FullProjectHotUpdateSolution"
    / "Patch"
    / "GoldenHotUpdate.PatchModule.csproj"
)
HOTUPDATE_SHARED_CONTRACTS_PROJECT_PATH = (
    REPO_ROOT
    / "subjects"
    / "HotUpdateHostPack"
    / "source"
    / "EngineeringScenarios"
    / "FullProjectHotUpdateSolution"
    / "SharedContracts"
    / "GoldenHotUpdate.SharedContracts.csproj"
)
COLLECTION_PATH = (
    REPO_ROOT
    / "solutions"
    / "subjects"
    / "HotUpdateHostPack"
    / "hotupdate-tests"
    / "Generated"
    / "declared-tests.collection.json"
)
BINDING_MANIFEST_PATH = (
    REPO_ROOT
    / "solutions"
    / "subjects"
    / "HotUpdateHostPack"
    / "hotupdate-tests"
    / "Generated"
    / "declared-tests.binding.json"
)


class PerfHarnessHotUpdateBenchmarkHostIntegrationTests(unittest.TestCase):
    def test_perf_harness_can_execute_hotupdate_benchmark_host_entry(self) -> None:
        test_output_root = (
            REPO_ROOT
            / "artifacts"
            / ".tmp-tests"
            / "perf-harness-hotupdate-benchmark-host"
            / uuid.uuid4().hex
        )
        harness_output_root = test_output_root / "harness"
        harness_dll_path = harness_output_root / "Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.dll"
        hotupdate_benchmark_host_output_root = test_output_root / "host"
        hotupdate_benchmark_host_dll_path = (
            hotupdate_benchmark_host_output_root / "HotUpdateHostPack.HotUpdateBenchmarkHost.dll"
        )

        subprocess.run(
            [
                "dotnet",
                "build",
                str(HARNESS_PROJECT_PATH),
                "-c",
                "Release",
                "-m:1",
                "-o",
                str(harness_output_root),
            ],
            cwd=REPO_ROOT,
            check=True,
            capture_output=True,
            text=True,
            timeout=180,
        )
        subprocess.run(
            [
                "dotnet",
                "build",
                str(HOTUPDATE_BENCHMARK_HOST_PROJECT_PATH),
                "-c",
                "Release",
                "-m:1",
                "-o",
                str(hotupdate_benchmark_host_output_root),
            ],
            cwd=REPO_ROOT,
            check=True,
            capture_output=True,
            text=True,
            timeout=180,
        )
        for project_path in [
            HOTUPDATE_SUBJECT_PROJECT_PATH,
            HOTUPDATE_PATCH_PROJECT_PATH,
            HOTUPDATE_SHARED_CONTRACTS_PROJECT_PATH,
        ]:
            subprocess.run(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(hotupdate_benchmark_host_output_root),
                ],
                cwd=REPO_ROOT,
                check=True,
                capture_output=True,
                text=True,
                timeout=180,
            )

        completed = subprocess.run(
            [
                "dotnet",
                str(harness_dll_path),
                "4",
                "--host-assembly",
                str(hotupdate_benchmark_host_dll_path),
                "--collection-path",
                str(COLLECTION_PATH.relative_to(REPO_ROOT)).replace("\\", "/"),
                "--entry-index",
                "0",
                "--binding-manifest-path",
                str(BINDING_MANIFEST_PATH.relative_to(REPO_ROOT)).replace("\\", "/"),
                "--mode",
                "managed",
            ],
            cwd=REPO_ROOT,
            check=False,
            capture_output=True,
            text=True,
            timeout=30,
        )

        self.assertEqual(0, completed.returncode, msg=completed.stderr or completed.stdout)
        output_lines = [line.strip() for line in completed.stdout.splitlines() if line.strip()]
        self.assertTrue(output_lines, "perf harness did not produce any JSON payload")

        payload = json.loads(output_lines[-1])
        self.assertEqual("benchmark-workload-entry", payload["harness"])
        self.assertEqual("managed", payload["mode"])
        self.assertEqual("HotUpdateHostPack", payload["subjectId"])
        self.assertEqual(
            "HotUpdateHostPack/HotUpdateDispatchBenchmarkEntry::RunWorkload()",
            payload["workloadEntry"],
        )
        self.assertEqual(4, payload["iterations"])
        self.assertGreaterEqual(float(payload["elapsedMilliseconds"]), 0.0)
        shutil.rmtree(test_output_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
