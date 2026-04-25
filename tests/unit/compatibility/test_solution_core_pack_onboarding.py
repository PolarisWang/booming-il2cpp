from __future__ import annotations

import json
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from tests.support import (
    SOLUTION_CORE_PACK_HOST_PROJECT_PATH,
    SOLUTION_CORE_PACK_HOST_SOLUTION_PATH,
    SOLUTION_CORE_PACK_OWNER_MANIFEST_PATH,
    SOLUTION_CORE_PACK_OWNER_ROOT,
    SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH,
    SOLUTION_CORE_PACK_PROOFS_ROOT,
)


REPO_ROOT = Path(__file__).resolve().parents[3]
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
DRIVER_ENTRY_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "DriverEntry.cs"
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
WORKERS_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_workers.py"
CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"
MANIFEST_PATH = SOLUTION_CORE_PACK_OWNER_MANIFEST_PATH
SOURCE_PROJECT_PATH = SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH
SOURCE_PROGRAM_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "Program.cs"
TASK_FLOW_PROOF_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "AsyncAndThreading" / "TaskAndValueTaskFlowProof.cs"
SOURCE_SLICE_ROOT = SOLUTION_CORE_PACK_PROOFS_ROOT
FRAMEWORK_PROJECT_REFERENCE = "Chaos.TestFramework.Sdk.csproj"
PROOF_CMAKE_PATH = SOLUTION_CORE_PACK_OWNER_ROOT / "validation" / "proof" / "native-reference" / "CMakeLists.txt"
GENERIC_NATIVE_REFERENCE_WORKSPACE_TEMPLATE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "subject" / "templates" / "native-reference-workspace.cmake.tmpl"
)
GENERIC_NATIVE_PROOF_TEMPLATE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "subject" / "templates" / "native-proof.cmake.tmpl"
)
PERF_BASELINE_PATH = SOLUTION_CORE_PACK_OWNER_ROOT / "benchmark-baselines" / "perf" / "windows-native-perf" / "windows.json"
TRACE_COMPARE_PATH = REPO_ROOT / "tests" / "contracts" / "trace" / "compare-warmup-trace.py"
WINDOWS_TRACE_SNAPSHOT_PATH = REPO_ROOT / "tests" / "contracts" / "trace" / "snapshots" / "windows-warmup-trace.snapshot.json"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "solution-core-pack-onboarding"


def _dotnet_intermediate_args(intermediate_root: Path) -> list[str]:
    return [
        f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
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


class SolutionCorePackOnboardingTests(unittest.TestCase):
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

        self.assertNotIn("CHAOS_SUBJECT_PROOF_ROOT", cmake_source)
        self.assertNotIn("add_subdirectory(\"${CHAOS_SUBJECT_PROOF_ROOT}\"", cmake_source)
        self.assertNotIn(
            'subject_proof_root = repo_root / "subjects" / str(selection["subjectId"]) / "validation" / "proof" / "native-reference"',
            workers_source,
        )
        self.assertIn("_materialize_windows_native_reference_cmake_source(", workers_source)
        self.assertIn(
            'f"-DCHAOS_SUBJECT_REPO_ROOT={repo_root}"',
            workers_source,
        )

    def test_solution_core_pack_manifest_and_proof_slice_are_realized(self) -> None:
        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        source_project_text = SOURCE_PROJECT_PATH.read_text(encoding="utf-8")
        source_program_text = SOURCE_PROGRAM_PATH.read_text(encoding="utf-8")
        validation_profiles = dict(manifest.get("validationProfiles") or {})
        validation = dict(manifest.get("validation") or {})

        self.assertEqual("SolutionCorePack", manifest["subjectId"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION_PATH.relative_to(REPO_ROOT).as_posix(), manifest["source"]["path"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_PROJECT_PATH.relative_to(REPO_ROOT).as_posix(), manifest["source"]["primaryProjectPath"])
        self.assertEqual("CoreRuntimeFeatures/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", manifest["workloadEntry"])
        self.assertEqual("dotnet-solution", manifest["sourceModel"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual(["proof"], validation_profiles["proof-dev"])
        self.assertEqual(["proof"], validation_profiles["trace-platform"])
        self.assertEqual(["perf"], validation_profiles["perf-profile"])
        self.assertEqual("proof", validation["proof"]["kind"])
        self.assertEqual("native-runtime-perf", validation["perf"]["driver"])
        self.assertNotIn("unit", validation)
        self.assertIn(FRAMEWORK_PROJECT_REFERENCE, source_project_text)

        self.assertTrue(SOURCE_PROJECT_PATH.is_file())
        self.assertTrue(SOURCE_PROGRAM_PATH.is_file())
        self.assertTrue(TASK_FLOW_PROOF_PATH.is_file())
        self.assertFalse(PROOF_CMAKE_PATH.exists())
        self.assertTrue(GENERIC_NATIVE_REFERENCE_WORKSPACE_TEMPLATE_PATH.is_file())
        self.assertTrue(GENERIC_NATIVE_PROOF_TEMPLATE_PATH.is_file())
        self.assertTrue(PERF_BASELINE_PATH.is_file())
        self.assertNotIn("[ChaosUnitTest(", source_program_text)
        self.assertIn("Assert.Equal(0, exitCode);", source_program_text)
        self.assertIn('Alias = "task-valuetask-flow-proof"', TASK_FLOW_PROOF_PATH.read_text(encoding="utf-8"))

    def test_solution_core_pack_proof_sources_use_asserts_without_stdout_contracts(self) -> None:
        program_text = SOURCE_PROGRAM_PATH.read_text(encoding="utf-8")
        self.assertIn("Assert.", program_text, msg=f"missing Assert usage: {SOURCE_PROGRAM_PATH}")
        self.assertNotIn("Console.WriteLine", program_text, msg=f"stdout contract leaked into {SOURCE_PROGRAM_PATH}")
        self.assertNotIn("ChaosEvidenceKind.Stdout", program_text, msg=f"stdout evidence leaked into {SOURCE_PROGRAM_PATH}")

        for source_path in sorted(SOURCE_SLICE_ROOT.rglob("*Proof.cs")):
            source_text = source_path.read_text(encoding="utf-8")
            self.assertNotIn("Console.WriteLine", source_text, msg=f"stdout contract leaked into {source_path}")
            self.assertNotIn("ChaosEvidenceKind.Stdout", source_text, msg=f"stdout evidence leaked into {source_path}")

    def test_solution_core_pack_trace_export_matches_windows_snapshot(self) -> None:
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

            assembly_path = build_output_root / "CoreRuntimeFeatures.dll"
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


