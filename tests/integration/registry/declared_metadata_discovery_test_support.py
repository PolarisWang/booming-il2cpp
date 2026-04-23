from __future__ import annotations

import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import load_module


REPO_ROOT = Path(__file__).resolve().parents[3]
COMPILED_CATALOG_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "compiled_catalog.py"
FIXTURE_ROOT = REPO_ROOT / "tests" / "fixtures" / "contracts" / "declared-metadata"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "declared-metadata-discovery"
SOLUTION_CORE_PACK_MAINLINE_SLICE_PROJECT_PATH = (
    REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Proofs" / "CoreRuntimeFeatures" / "CoreRuntimeFeatures.csproj"
)
SOLUTION_CORE_PACK_PERFORMANCE_SLICE_PROJECT_PATH = (
    REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "CoreRuntimeBenchmarks.csproj"
)
HOT_UPDATE_HOST_PACK_PROJECT_PATH = REPO_ROOT / "subjects" / "HotUpdateHostPack" / "source" / "HotUpdateHostPack.csproj"
MIXED_EXECUTION_FEATURE_PACK_PROJECT_PATH = REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "MixedExecutionFeaturePack.csproj"


def dotnet_intermediate_args(intermediate_root: Path) -> list[str]:
    return [
        f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
    ]


def run_checked(arguments: list[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        combined_output = "\n".join(part for part in [completed.stdout, completed.stderr] if part)
        raise AssertionError(f"command failed ({completed.returncode}): {' '.join(arguments)}\n{combined_output}")
    return completed

class DeclaredMetadataDiscoveryTestSupport(unittest.TestCase):
    maxDiff = None

    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        cls.runtime_only_dll = cls._build_fixture("RuntimeOnly")
        cls.framework_no_entries_dll = cls._build_fixture("FrameworkNoEntries")
        cls.declared_entries_dll = cls._build_fixture("DeclaredEntries")

    @classmethod
    def _build_fixture(cls, fixture_name: str) -> Path:
        project_path = FIXTURE_ROOT / fixture_name / f"{fixture_name}.csproj"
        run_checked(
            [
                "dotnet",
                "build",
                str(project_path),
                "-c",
                "Release",
                "-m:1",
            ],
            cwd=REPO_ROOT,
        )
        dll_path = project_path.parent / "bin" / "Release" / "net8.0" / f"{fixture_name}.dll"
        if not dll_path.is_file():
            raise AssertionError(f"missing compiled fixture dll: {dll_path}")
        return dll_path

    def _copy_fixture_output(self, dll_path: Path) -> Path:
        copied_root = TEST_TMP_ROOT / "copied-output" / f"{dll_path.stem}-{uuid.uuid4().hex}"
        shutil.copytree(dll_path.parent, copied_root)
        return copied_root / dll_path.name
