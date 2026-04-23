from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SHARED_PERF_PROJECT_PATH = "src/tools/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.csproj"
SOLUTION_CORE_PACK_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack"
HOT_UPDATE_HOST_PACK_ROOT = REPO_ROOT / "subjects" / "HotUpdateHostPack"
MIXED_EXECUTION_FEATURE_PACK_ROOT = REPO_ROOT / "subjects" / "MixedExecutionFeaturePack"
MANAGED_PERF_SUBJECTS = {
    "HotUpdateHostPack": {
        "manifest_path": HOT_UPDATE_HOST_PACK_ROOT / "subject.manifest.json",
        "source_path": "subjects/HotUpdateHostPack/source/HotUpdateHostPack.sln",
        "primary_project_path": "subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj",
        "workload_entry": "HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()",
        "harness_iterations": 4,
    },
    "MixedExecutionFeaturePack": {
        "manifest_path": MIXED_EXECUTION_FEATURE_PACK_ROOT / "subject.manifest.json",
        "source_path": "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.sln",
        "primary_project_path": "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj",
        "workload_entry": "MixedExecutionFeaturePack/MixedExecutionNativeBenchmarkEntry::RunWorkload()",
        "native_workload_entry": "MixedExecutionFeaturePack/MixedExecutionNativeBenchmarkEntry::RunWorkload()",
        "harness_iterations": 6,
    },
}

class BenchmarkSubjectSourceTestSupport(unittest.TestCase):
    pass
