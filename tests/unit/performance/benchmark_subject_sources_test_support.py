from __future__ import annotations

import json
import unittest
from pathlib import Path

from tests.support import (
    HOT_UPDATE_HOST_PACK_OWNER_MANIFEST_PATH,
    MIXED_EXECUTION_FEATURE_PACK_OWNER_MANIFEST_PATH,
    SOLUTION_CORE_PACK_BENCHMARKS_PROJECT_PATH,
    SOLUTION_CORE_PACK_BENCHMARKS_ROOT,
    SOLUTION_CORE_PACK_HOST_PROJECT_PATH,
    SOLUTION_CORE_PACK_HOST_SOLUTION_PATH,
    SOLUTION_CORE_PACK_OWNER_MANIFEST_PATH,
    SOLUTION_CORE_PACK_OWNER_ROOT,
)

REPO_ROOT = Path(__file__).resolve().parents[3]
SHARED_PERF_PROJECT_PATH = "src/tools/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness.csproj"
SOLUTION_CORE_PACK_ROOT = SOLUTION_CORE_PACK_OWNER_ROOT
HOT_UPDATE_HOST_PACK_ROOT = HOT_UPDATE_HOST_PACK_OWNER_MANIFEST_PATH.parent
MIXED_EXECUTION_FEATURE_PACK_ROOT = MIXED_EXECUTION_FEATURE_PACK_OWNER_MANIFEST_PATH.parent
MANAGED_PERF_SUBJECTS = {
    "HotUpdateHostPack": {
        "manifest_path": HOT_UPDATE_HOST_PACK_OWNER_MANIFEST_PATH,
        "source_path": "verification/catalog/owners/HotUpdateHostPack/support/host/HotUpdateHostPack.sln",
        "primary_project_path": "verification/catalog/owners/HotUpdateHostPack/support/host/HotUpdateHostPack.csproj",
        "workload_entry": "HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()",
        "harness_iterations": 4,
    },
    "MixedExecutionFeaturePack": {
        "manifest_path": MIXED_EXECUTION_FEATURE_PACK_OWNER_MANIFEST_PATH,
        "source_path": "verification/catalog/owners/MixedExecutionFeaturePack/support/host/MixedExecutionFeaturePack.sln",
        "primary_project_path": "verification/catalog/owners/MixedExecutionFeaturePack/support/host/MixedExecutionFeaturePack.csproj",
        "workload_entry": "MixedExecutionFeaturePack/MixedExecutionNativeBenchmarkEntry::RunWorkload()",
        "native_workload_entry": "MixedExecutionFeaturePack/MixedExecutionNativeBenchmarkEntry::RunWorkload()",
        "harness_iterations": 6,
    },
}

class BenchmarkSubjectSourceTestSupport(unittest.TestCase):
    pass
