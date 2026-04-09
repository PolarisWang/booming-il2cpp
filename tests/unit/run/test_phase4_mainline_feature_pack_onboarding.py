from __future__ import annotations

import json
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
DRIVER_ENTRY_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "DriverEntry.cs"
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
WORKERS_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_workers.py"
CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"
MANIFEST_PATH = REPO_ROOT / "subjects" / "MainlineFeaturePack" / "subject.manifest.json"
SOURCE_PROJECT_PATH = REPO_ROOT / "subjects" / "MainlineFeaturePack" / "source" / "MainlineFeaturePack.csproj"
SOURCE_PROGRAM_PATH = REPO_ROOT / "subjects" / "MainlineFeaturePack" / "source" / "Program.cs"
UNIT_PROJECT_PATH = (
    REPO_ROOT
    / "subjects"
    / "MainlineFeaturePack"
    / "validation"
    / "unit"
    / "MainlineFeaturePack.Subject.UnitTests"
    / "MainlineFeaturePack.Subject.UnitTests.csproj"
)
PROOF_CMAKE_PATH = (
    REPO_ROOT
    / "subjects"
    / "MainlineFeaturePack"
    / "validation"
    / "proof"
    / "native-reference"
    / "CMakeLists.txt"
)
PERF_BASELINE_PATH = (
    REPO_ROOT
    / "subjects"
    / "MainlineFeaturePack"
    / "baselines"
    / "perf"
    / "windows-native-profile"
    / "windows.json"
)
TRACE_COMPARE_PATH = REPO_ROOT / "tests" / "contracts" / "trace" / "compare-warmup-trace.py"
WINDOWS_TRACE_SNAPSHOT_PATH = REPO_ROOT / "tests" / "contracts" / "trace" / "snapshots" / "windows-warmup-trace.snapshot.json"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase4-mainline-feature-pack"


def _dotnet_intermediate_args(intermediate_root: Path) -> list[str]:
    intermediate_text = intermediate_root.as_posix() + "/$(MSBuildProjectName)/"
    return [
        f"-p:BaseIntermediateOutputPath={intermediate_text}",
        f"-p:MSBuildProjectExtensionsPath={intermediate_text}",
    ]


def _run_checked(arguments: list[str], *, cwd: Path) -> str:
    completed = subprocess.run(
        arguments,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    output = "\n".join(part for part in [completed.stdout, completed.stderr] if part).strip()
    if completed.returncode != 0:
        raise AssertionError(f"command failed: {' '.join(arguments)}\n{output}")
    return output


class Phase4MainlineFeaturePackOnboardingTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def test_proof_entry_override_is_wired_through_managed_closure_pipeline(self) -> None:
        contracts_source = CONTRACTS_PATH.read_text(encoding="utf-8")
        driver_source = DRIVER_ENTRY_PATH.read_text(encoding="utf-8")
        loader_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")
        workers_source = WORKERS_PATH.read_text(encoding="utf-8")

        self.assertIn("EntryPointSubjectIdOverride", contracts_source)
        self.assertIn("--entry-point-subject-id", driver_source)
        self.assertIn("EntryPointSubjectIdOverride", driver_source)
        self.assertIn("EntryPointSubjectIdOverride", loader_source)
        self.assertIn("--entry-point-subject-id", workers_source)

    def test_subject_scoped_proof_build_is_routed_from_worker_to_root_cmake(self) -> None:
        cmake_source = CMAKE_PATH.read_text(encoding="utf-8")
        workers_source = WORKERS_PATH.read_text(encoding="utf-8")

        self.assertIn("CHAOS_SUBJECT_PROOF_ROOT", cmake_source)
        self.assertIn("add_subdirectory(\"${CHAOS_SUBJECT_PROOF_ROOT}\"", cmake_source)
        self.assertIn("-DCHAOS_SUBJECT_PROOF_ROOT=", workers_source)

    def test_mainline_feature_pack_manifest_and_tree_are_realized(self) -> None:
        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        validation_profiles = dict(manifest.get("validationProfiles") or {})
        validation = dict(manifest.get("validation") or {})

        self.assertEqual("MainlineFeaturePack/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual(["proof", "unit"], validation_profiles["proof-dev"])
        self.assertEqual(["proof"], validation_profiles["trace-platform"])
        self.assertEqual(["proof", "perf"], validation_profiles["perf-profile"])
        self.assertEqual(
            "subjects/MainlineFeaturePack/validation/unit/MainlineFeaturePack.Subject.UnitTests/MainlineFeaturePack.Subject.UnitTests.csproj",
            validation["unit"]["project"],
        )
        self.assertEqual("xunit", validation["unit"]["framework"])
        self.assertEqual("native-runtime-perf", validation["perf"]["driver"])

        self.assertTrue(SOURCE_PROJECT_PATH.is_file())
        self.assertTrue(SOURCE_PROGRAM_PATH.is_file())
        self.assertTrue(UNIT_PROJECT_PATH.is_file())
        self.assertTrue(PROOF_CMAKE_PATH.is_file())
        self.assertTrue(PERF_BASELINE_PATH.is_file())

    def test_mainline_feature_pack_trace_export_matches_windows_snapshot(self) -> None:
        temp_root = Path(tempfile.mkdtemp(prefix="trace-export-", dir=str(TEST_TMP_ROOT)))
        try:
            build_output_root = temp_root / "build"
            intermediate_root = temp_root / "obj"
            trace_output_path = temp_root / "windows.trace.runtime.json"

            _run_checked(
                [
                    "dotnet",
                    "build",
                    str(SOURCE_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-o",
                    str(build_output_root),
                    *_dotnet_intermediate_args(intermediate_root),
                ],
                cwd=REPO_ROOT,
            )

            assembly_path = build_output_root / "MainlineFeaturePack.dll"
            self.assertTrue(assembly_path.is_file())

            _run_checked(
                [
                    "dotnet",
                    str(assembly_path),
                    "--trace-platform",
                    "windows",
                    "--trace-output",
                    str(trace_output_path),
                ],
                cwd=REPO_ROOT,
            )

            self.assertTrue(trace_output_path.is_file())
            _run_checked(
                [
                    sys.executable,
                    str(TRACE_COMPARE_PATH),
                    str(WINDOWS_TRACE_SNAPSHOT_PATH),
                    str(trace_output_path),
                ],
                cwd=REPO_ROOT,
            )
        finally:
            shutil.rmtree(temp_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
