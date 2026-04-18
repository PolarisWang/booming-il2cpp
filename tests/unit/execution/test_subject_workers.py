from __future__ import annotations

import importlib.util
import json
import os
import shutil
import subprocess
import sys
import time
import unittest
import uuid
from pathlib import Path
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_WORKERS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_workers.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "subject-workers"
WINDOWS_REFERENCE_BUILD_TARGET = "chaos_subject_reference_proof"
WINDOWS_REFERENCE_RUN_TARGET = "chaos_subject_reference_proof_run"
WINDOWS_REFERENCE_CMAKE_BUILD_STRATEGY = "windows-reference-cmake"
WINDOWS_NATIVE_AOT_BUILD_TARGET = "chaos_subject_native_aot"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def posix_path(*parts: str) -> str:
    return Path(*parts).as_posix()


def subject_run_path(subject_id: str, run_id: str, *parts: str) -> str:
    return posix_path("artifacts", "subjects", subject_id, "runs", run_id, *parts)


def subject_source_path(subject_id: str) -> str:
    return posix_path("subjects", subject_id, "source", f"{subject_id}.csproj")


class SubjectWorkersTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def _make_non_repo_path(self, *parts: str) -> Path:
        return TEST_TMP_ROOT / "_external" / Path(*parts)

    def test_perf_defaults_bias_toward_short_benchmark_batches(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_defaults")

        self.assertEqual(3, workers_module._perf_sample_count("managed-perf-release"))
        self.assertEqual(1, workers_module._perf_sample_count("native-perf-profile"))
        self.assertEqual(1, workers_module._perf_sample_count("managed-perf-dev"))
        self.assertEqual(1000, workers_module._perf_harness_iterations("managed-perf-release"))
        self.assertEqual(100, workers_module._perf_harness_iterations("interpreter-perf-dev"))

    def test_windows_build_target_uses_cmake_reference_route_and_records_build_strategy(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_windows_build")
        subject_id = "FixtureNativeSubject"
        run_id = "fixture-run-native-build-001"
        matrix_id = "windows-reference-check"
        expected_cmake_dir = self._make_non_repo_path("cmake-builds", "subject-reference-1234")
        expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"
        expected_env = {
            "Path": r"C:\VS\bin;C:\Windows\System32",
            "INCLUDE": r"C:\VS\include",
            "LIB": r"C:\VS\lib",
        }

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-build")
        try:
            generated_source_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "analysis"
                / "generated"
                / "generated"
                / "native-reference.generated.cpp"
            )
            generated_source_path.parent.mkdir(parents=True, exist_ok=True)
            generated_source_path.write_text("// fixture\n", encoding="utf-8")
            expected_cmake_source_root = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "build"
                / "cmake-src"
            )

            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value=expected_env):
                    with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                        with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                            with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir", return_value=expected_cmake_dir):
                                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                                    with patch.object(workers_module, "_run_checked") as run_checked_mock:
                                        result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "-S",
                    str(expected_cmake_source_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Visual Studio 18 2026",
                    f"-DCHAOS_SUBJECT_REPO_ROOT={repo_root}",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'out'}",
                    f"-DCHAOS_SUBJECT_RUNTIME_ROOT={repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'runtime'}",
                    f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "--build",
                    str(expected_cmake_dir),
                    "--config",
                    "Release",
                    "--target",
                    WINDOWS_REFERENCE_BUILD_TARGET,
                ],
                run_checked_mock.call_args_list[1].args[0],
            )
            self.assertEqual(expected_env, run_checked_mock.call_args_list[0].kwargs["env"])
            self.assertEqual(expected_env, run_checked_mock.call_args_list[1].kwargs["env"])

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("CHECK", manifest["variant"])
            self.assertEqual(
                {
                    "codegen": ["CHAOS_VARIANT_CHECK", "CHAOS_VARIANT_NAME=CHECK"],
                    "native": ["CHAOS_VARIANT_CHECK", "CHAOS_VARIANT_NAME=CHECK"],
                },
                manifest["variantMacros"],
            )
            self.assertEqual(WINDOWS_REFERENCE_CMAKE_BUILD_STRATEGY, manifest["buildStrategy"])
            self.assertEqual("native-reference", manifest["buildKind"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "out"),
                manifest["binaryRoot"],
            )
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "out", f"{WINDOWS_REFERENCE_BUILD_TARGET}.exe")],
                manifest["outputs"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "cmake-src", "proof", "main.cpp"),
                manifest["hostSourcePath"],
            )
            self.assertEqual(expected_cmake_dir.as_posix(), manifest["cmakeBinaryDir"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_frontend_pipeline_worker_records_analysis_bundle_under_subject_artifacts(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_frontend_pipeline")
        subject_id = "FixtureAnalysisSubject"
        run_id = "fixture-run-analysis-001"
        request = {
            "selection": {
                "subjectId": subject_id,
                "variant": "CHECK",
                "source": {
                    "entry": f"{subject_id}/ProofEntry::Run()",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "analysis"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "analysis", "analysis.manifest.json"),
                "reportPaths": [
                    subject_run_path(subject_id, run_id, "analysis", "analysis", "contract-validate.report.json"),
                ],
            },
        }

        repo_root = self._make_repo_root("frontend-pipeline")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll"),
                        "additionalAssemblyPaths": [
                            subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.Helper.dll"),
                        ],
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                    }
                ),
                encoding="utf-8",
            )

            expected_host_input = (
                repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input" / f"{subject_id}.dll"
            )
            expected_additional_host_input = (
                repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input" / f"{subject_id}.Helper.dll"
            )
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "analysis"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        str(repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"),
                        str(expected_host_input),
                        str(expected_output_root),
                        "--entry-point-subject-id",
                        f"{subject_id}/ProofEntry::Run()",
                        "--additional-assembly",
                        str(expected_additional_host_input),
                    ],
                    arguments,
                )
                expected_output_root.mkdir(parents=True, exist_ok=True)
                for name in [
                    "typed-il-ir.json",
                    "aot-manifest.json",
                    "metadata-registration.json",
                    "code-registration.json",
                    "optimization-facts.json",
                    "preserve-descriptor.json",
                    "closure.manifest.json",
                ]:
                    (expected_output_root / name).write_text("{}", encoding="utf-8")
                return ""

            with patch.object(workers_module, "_ensure_driver_built", return_value=repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    with patch.object(workers_module.contracts_module, "validate_analysis_contracts") as validate_mock:
                        result = workers_module.run_frontend_pipeline_worker(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            validate_mock.assert_called_once_with(repo_root)

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_id, manifest["subjectId"])
            self.assertEqual("analysis", manifest["bucket"])
            self.assertEqual("proof-input-bundle", manifest["bundleKind"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "analysis", "typed-il-ir.json"),
                manifest["artifacts"]["typedIlIrPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "analysis", "closure.manifest.json"),
                manifest["artifacts"]["closureManifestPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "analysis", "optimization-facts.json"),
                manifest["artifacts"]["optimizationFactsPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "analysis", "preserve-descriptor.json"),
                manifest["artifacts"]["preserveDescriptorPath"],
            )

            report = json.loads((repo_root / request["paths"]["reportPaths"][0]).read_text(encoding="utf-8"))
            self.assertEqual("ok", report["status"])
            self.assertEqual("contracts/artifacts/v0/schemas", report["schemaPath"])
            self.assertEqual("tests/contracts/analysis/v0/snapshots", report["snapshotPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_proof_emitter_records_generated_bundle_under_subject_artifacts(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_proof_emitter")
        subject_id = "FixtureGeneratedSubject"
        run_id = "fixture-run-generated-001"
        request = {
            "selection": {
                "subjectId": subject_id,
            },
            "upstream": {
                "analysis": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "analysis", "analysis.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "generated"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("native-proof-emitter")
        try:
            analysis_manifest_path = repo_root / request["upstream"]["analysis"]["manifestPath"]
            analysis_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            analysis_manifest_path.write_text("{}", encoding="utf-8")

            expected_analysis_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "analysis"
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "generated"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        str(repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"),
                        "emit-native-reference",
                        str(expected_analysis_root),
                        str(expected_output_root),
                    ],
                    arguments,
                )
                (expected_output_root / "generated").mkdir(parents=True, exist_ok=True)
                (expected_output_root / "generated" / "native-reference.generated.cpp").write_text("// generated", encoding="utf-8")
                (expected_output_root / "native-reference.manifest.json").write_text("{}", encoding="utf-8")
                (expected_output_root / "native-reference.plan.json").write_text("{}", encoding="utf-8")
                return ""

            with patch.object(workers_module, "_ensure_driver_built", return_value=repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    result = workers_module.run_native_proof_emitter(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_id, manifest["subjectId"])
            self.assertEqual("generated", manifest["bucket"])
            self.assertEqual("CHECK", manifest["variant"])
            self.assertEqual(
                ["CHAOS_VARIANT_CHECK", "CHAOS_VARIANT_NAME=CHECK"],
                manifest["codegenMacros"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-reference.generated.cpp"),
                manifest["generatedSourcePath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-reference.manifest.json"),
                manifest["nativeReferenceManifestPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-reference.plan.json"),
                manifest["nativeReferencePlanPath"],
            )
            self.assertNotIn("workloadEntry", manifest)
            self.assertNotIn("nativeProofManifestPath", manifest)
            self.assertNotIn("nativeProofPlanPath", manifest)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_proof_emitter_routes_declared_unit_test_selection_through_native_aot(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_proof_emitter_declared_unit_test")
        subject_id = "FixtureGeneratedDeclaredProofSubject"
        run_id = "fixture-run-generated-declared-proof-001"
        request = {
            "selection": {
                "subjectId": subject_id,
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                    "alias": "fixture-proof",
                    "entryIndex": 7,
                },
            },
            "upstream": {
                "analysis": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "analysis", "analysis.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "generated"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("native-proof-emitter-declared-proof")
        try:
            analysis_manifest_path = repo_root / request["upstream"]["analysis"]["manifestPath"]
            analysis_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            analysis_manifest_path.write_text("{}", encoding="utf-8")

            expected_analysis_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "analysis"
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "generated"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        str(repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"),
                        "emit-native-aot",
                        str(expected_analysis_root),
                        str(expected_output_root),
                    ],
                    arguments,
                )
                (expected_output_root / "generated").mkdir(parents=True, exist_ok=True)
                (expected_output_root / "generated" / "native-aot.generated.cpp").write_text("// generated", encoding="utf-8")
                (expected_output_root / "native-aot.manifest.json").write_text("{}", encoding="utf-8")
                (expected_output_root / "native-aot.plan.json").write_text("{}", encoding="utf-8")
                return ""

            with patch.object(workers_module, "_ensure_driver_built", return_value=repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    result = workers_module.run_native_proof_emitter(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-aot.generated.cpp"),
                manifest["generatedSourcePath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.manifest.json"),
                manifest["nativeAotManifestPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.plan.json"),
                manifest["nativeAotPlanPath"],
            )
            self.assertNotIn("nativeReferenceManifestPath", manifest)
            self.assertNotIn("nativeReferencePlanPath", manifest)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_proof_emitter_routes_subject_entry_selection_through_native_aot(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_proof_emitter_subject_entry_selection")
        subject_id = "FixtureGeneratedManagedEntrySubject"
        run_id = "fixture-run-generated-managed-entry-001"
        request = {
            "selection": {
                "subjectId": subject_id,
                "source": {
                    "entry": f"{subject_id}/Program::Run()",
                    "entrySelection": {
                        "entryKind": 1,
                        "entrySlice": 11,
                    },
                },
            },
            "upstream": {
                "analysis": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "analysis", "analysis.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "generated"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("native-proof-emitter-subject-entry-selection")
        try:
            analysis_manifest_path = repo_root / request["upstream"]["analysis"]["manifestPath"]
            analysis_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            analysis_manifest_path.write_text("{}", encoding="utf-8")

            expected_analysis_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "analysis"
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "generated"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        str(repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"),
                        "emit-native-aot",
                        str(expected_analysis_root),
                        str(expected_output_root),
                    ],
                    arguments,
                )
                (expected_output_root / "generated").mkdir(parents=True, exist_ok=True)
                (expected_output_root / "generated" / "native-aot.generated.cpp").write_text("// generated", encoding="utf-8")
                (expected_output_root / "native-aot.manifest.json").write_text("{}", encoding="utf-8")
                (expected_output_root / "native-aot.plan.json").write_text("{}", encoding="utf-8")
                return ""

            with patch.object(workers_module, "_ensure_driver_built", return_value=repo_root / "driver" / "Chaos.IL2CPP.Driver.dll"):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    result = workers_module.run_native_proof_emitter(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-aot.generated.cpp"),
                manifest["generatedSourcePath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.manifest.json"),
                manifest["nativeAotManifestPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.plan.json"),
                manifest["nativeAotPlanPath"],
            )
            self.assertNotIn("nativeReferenceManifestPath", manifest)
            self.assertNotIn("nativeReferencePlanPath", manifest)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_windows_validate_only_build_uses_visual_studio_generator_and_records_non_repo_cmake_binary_dir(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_windows_validate_only")
        subject_id = "FixtureValidateOnlySubject"
        run_id = "fixture-run-validate-only-001"
        matrix_id = "windows-android-check"
        expected_cmake_dir = self._make_non_repo_path("cmake-builds", "platform-gate-1234")
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "android-arm64",
                    "toolchainProfile": "android-ndk-r26",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-validate-only")
        try:
            expected_android_host_root = (
                repo_root / "subjects" / subject_id / "validation" / "mobile" / "android-host"
            )
            expected_android_host_root.mkdir(parents=True, exist_ok=True)
            with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                    with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir", return_value=expected_cmake_dir):
                        with patch.object(workers_module, "_run_checked") as run_checked_mock:
                            result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    "cmake",
                    "-S",
                    str(repo_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Visual Studio 18 2026",
                    "-DROADMAP0_PRESET_TARGET=android-arm64-smoke",
                    "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_ANDROID_HOST_ROOT={expected_android_host_root}",
                    f"-DCMAKE_TOOLCHAIN_FILE={repo_root / 'build' / 'toolchains' / 'android-arm64.cmake'}",
                    f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                ["cmake", "--build", str(expected_cmake_dir)],
                run_checked_mock.call_args_list[1].args[0],
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("CHECK", manifest["variant"])
            self.assertEqual(expected_cmake_dir.as_posix(), manifest["cmakeBinaryDir"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_payload_custom_perf_metrics_accepts_flat_numeric_payload(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_flat_perf_payload")

        metrics = workers_module._payload_custom_perf_metrics(
            {
                "mode": "native",
                "subjectId": "BenchArithmetic",
                "elapsedMilliseconds": 0.125,
                "opsPerSecond": 8000.0,
                "checksum": 42,
            }
        )

        self.assertEqual(
            {
                "elapsedMilliseconds": 0.125,
                "opsPerSecond": 8000.0,
                "checksum": 42.0,
            },
            metrics,
        )

    def test_windows_build_target_reuses_workspace_native_benchmark_configure_root_and_records_contract(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_windows_native_aot_cmake_build_workspace_reuse",
        )
        subject_id = "FixtureNativeAotSubject"
        run_id = "fixture-run-native-aot-build-001"
        matrix_id = "windows-native-perf"
        stable_id = f"{subject_id}::{subject_id}::{subject_id}.Benchmarks::RunWorkload()"
        alias = "fixture-native-aot-benchmark"
        workload_entry = f"{subject_id}/Benchmarks::RunWorkload()"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
        expected_configure_root = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "native",
            matrix_id,
        )
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"
        expected_env = {
            "Path": r"C:\VS\bin;C:\Windows\System32",
            "INCLUDE": r"C:\VS\include",
            "LIB": r"C:\VS\lib",
        }

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": workload_entry,
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 11,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-native-aot-build")
        try:
            for relative_path in [
                Path("src/native/benchmark-host/native_aot_main.cpp"),
                Path("artifacts")
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "analysis"
                / "generated"
                / "generated"
                / "native-aot.generated.cpp",
            ]:
                absolute_path = repo_root / relative_path
                absolute_path.parent.mkdir(parents=True, exist_ok=True)
                absolute_path.write_text("// fixture\n", encoding="utf-8")

            generated_manifest_path = repo_root / request["upstream"]["generated"]["manifestPath"]
            generated_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            generated_manifest_path.write_text(
                json.dumps(
                    {
                        "generatedSourcePath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-aot.generated.cpp"),
                        "nativeAotManifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.manifest.json"),
                    }
                ),
                encoding="utf-8",
            )

            workspace_manifest_path = repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json"
            workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredBenchmarkHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                                "hostKind": "benchmark-host",
                                "collectionPath": collection_path,
                            }
                        ],
                        "nativeTestProjects": [
                            {
                                "projectId": f"native-test/{subject_id}/{matrix_id}/benchmark-host",
                                "matrixId": matrix_id,
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "native",
                                    matrix_id,
                                    "benchmark",
                                    "chaos_subject_native_aot.vcxproj",
                                ),
                                "configureRoot": expected_configure_root,
                                "hostKind": "benchmark-host",
                                "managedTestProjectId": f"managed-test/{subject_id}/benchmark-host",
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )

            expected_cmake_dir = repo_root / expected_configure_root
            expected_cmake_source_root = repo_root / "solutions" / "subjects" / subject_id / "native-source" / matrix_id
            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value=expected_env):
                    with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                        with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                            with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir") as allocate_cmake_binary_dir_mock:
                                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                                    with patch.object(workers_module, "_run_checked") as run_checked_mock:
                                        result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "-S",
                    str(expected_cmake_source_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Visual Studio 18 2026",
                    f"-DCHAOS_SUBJECT_BENCHMARK_HOST_MAIN={(repo_root / 'src' / 'native' / 'benchmark-host' / 'native_aot_main.cpp').as_posix()}",
                    f"-DCHAOS_SUBJECT_GENERATED_INPUT_SOURCE={(repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'analysis' / 'generated' / 'generated' / 'native-aot.generated.cpp').as_posix()}",
                    "-DCHAOS_SUBJECT_VARIANT=PROFILE",
                    f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={(repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'out').as_posix()}",
                    f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "--build",
                    str(expected_cmake_dir),
                    "--config",
                    "Release",
                    "--target",
                    WINDOWS_NATIVE_AOT_BUILD_TARGET,
                ],
                run_checked_mock.call_args_list[1].args[0],
            )
            self.assertEqual(expected_env, run_checked_mock.call_args_list[0].kwargs["env"])
            self.assertEqual(expected_env, run_checked_mock.call_args_list[1].kwargs["env"])
            allocate_cmake_binary_dir_mock.assert_not_called()

            cmake_source_root = expected_cmake_source_root
            workspace_cmakelists = (cmake_source_root / "CMakeLists.txt").read_text(encoding="utf-8")
            benchmark_cmakelists = (cmake_source_root / "benchmark" / "CMakeLists.txt").read_text(encoding="utf-8")
            self.assertIn("third_party/bdwgc", workspace_cmakelists)
            self.assertIn("src/native/hot-update", workspace_cmakelists)
            self.assertIn("chaos_hot_update", benchmark_cmakelists)

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("PROFILE", manifest["variant"])
            self.assertEqual("windows-benchmark-cmake", manifest["buildStrategy"])
            self.assertEqual("native-aot", manifest["buildKind"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-aot.generated.cpp"),
                manifest["generatedSourcePath"],
            )
            self.assertEqual(
                posix_path("src", "native", "benchmark-host", "native_aot_main.cpp"),
                manifest["hostSourcePath"],
            )
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "out", f"{WINDOWS_NATIVE_AOT_BUILD_TARGET}.exe")],
                manifest["outputs"],
            )
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
            self.assertEqual(
                f"managed-test/{subject_id}/benchmark-host",
                manifest["managedTestProjectId"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "benchmark.dispatch.manifest.json"),
                manifest["dispatchManifestPath"],
            )
            dispatch_manifest = json.loads((repo_root / manifest["dispatchManifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_id, dispatch_manifest["subjectId"])
            self.assertEqual(matrix_id, dispatch_manifest["matrixId"])
            self.assertEqual("benchmark-host", dispatch_manifest["hostKind"])
            self.assertEqual(collection_path, dispatch_manifest["collectionPath"])
            self.assertNotIn("workloadEntry", dispatch_manifest)
            self.assertEqual("RunNativeAot", dispatch_manifest["nativeEntryFunctionName"])
            self.assertEqual(11, dispatch_manifest["entrySelection"]["entryIndex"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_windows_build_target_routes_subject_entry_selection_native_aot_through_benchmark_host_contract(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_windows_native_aot_subject_entry_build",
        )
        subject_id = "FixtureNativeAotManagedEntrySubject"
        run_id = "fixture-run-native-aot-managed-entry-build-001"
        matrix_id = "windows-native-check"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
        expected_configure_root = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "native",
            matrix_id,
        )
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"
        expected_env = {
            "Path": r"C:\VS\bin;C:\Windows\System32",
            "INCLUDE": r"C:\VS\include",
            "LIB": r"C:\VS\lib",
        }

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "source": {
                    "entry": f"{subject_id}/Program::RunSelected()",
                    "entrySelection": {
                        "entryKind": 1,
                        "entrySlice": 11,
                    },
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-native-aot-subject-entry-build")
        try:
            for relative_path in [
                Path("src/native/benchmark-host/native_aot_main.cpp"),
                Path("artifacts")
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "analysis"
                / "generated"
                / "generated"
                / "native-aot.generated.cpp",
            ]:
                absolute_path = repo_root / relative_path
                absolute_path.parent.mkdir(parents=True, exist_ok=True)
                absolute_path.write_text("// fixture\n", encoding="utf-8")

            generated_manifest_path = repo_root / request["upstream"]["generated"]["manifestPath"]
            generated_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            generated_manifest_path.write_text(
                json.dumps(
                    {
                        "generatedSourcePath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-aot.generated.cpp"),
                        "nativeAotManifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.manifest.json"),
                    }
                ),
                encoding="utf-8",
            )

            workspace_manifest_path = repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json"
            workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredBenchmarkHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                                "hostKind": "benchmark-host",
                                "collectionPath": collection_path,
                            }
                        ],
                        "nativeTestProjects": [
                            {
                                "projectId": f"native-test/{subject_id}/{matrix_id}/benchmark-host",
                                "matrixId": matrix_id,
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "native",
                                    matrix_id,
                                    "benchmark",
                                    "chaos_subject_native_aot.vcxproj",
                                ),
                                "configureRoot": expected_configure_root,
                                "hostKind": "benchmark-host",
                                "managedTestProjectId": f"managed-test/{subject_id}/benchmark-host",
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )

            expected_cmake_dir = repo_root / expected_configure_root
            expected_cmake_source_root = repo_root / "solutions" / "subjects" / subject_id / "native-source" / matrix_id
            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value=expected_env):
                    with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                        with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                            with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir") as allocate_cmake_binary_dir_mock:
                                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                                    with patch.object(workers_module, "_run_checked") as run_checked_mock:
                                        result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "-S",
                    str(expected_cmake_source_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Visual Studio 18 2026",
                    f"-DCHAOS_SUBJECT_BENCHMARK_HOST_MAIN={(repo_root / 'src' / 'native' / 'benchmark-host' / 'native_aot_main.cpp').as_posix()}",
                    f"-DCHAOS_SUBJECT_GENERATED_INPUT_SOURCE={(repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'analysis' / 'generated' / 'generated' / 'native-aot.generated.cpp').as_posix()}",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={(repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'out').as_posix()}",
                    f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "--build",
                    str(expected_cmake_dir),
                    "--config",
                    "Release",
                    "--target",
                    WINDOWS_NATIVE_AOT_BUILD_TARGET,
                ],
                run_checked_mock.call_args_list[1].args[0],
            )
            self.assertEqual(expected_env, run_checked_mock.call_args_list[0].kwargs["env"])
            self.assertEqual(expected_env, run_checked_mock.call_args_list[1].kwargs["env"])
            allocate_cmake_binary_dir_mock.assert_not_called()

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("CHECK", manifest["variant"])
            self.assertEqual("windows-benchmark-cmake", manifest["buildStrategy"])
            self.assertEqual("native-aot", manifest["buildKind"])
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
            self.assertEqual(
                f"managed-test/{subject_id}/benchmark-host",
                manifest["managedTestProjectId"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_windows_build_target_routes_declared_unit_test_native_aot_through_proof_host_contract(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_windows_native_aot_proof_host_build",
        )
        subject_id = "FixtureNativeAotProofSubject"
        run_id = "fixture-run-native-aot-proof-build-001"
        matrix_id = "windows-native-check"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
        expected_configure_root = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "native",
            matrix_id,
        )
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"
        expected_env = {
            "Path": r"C:\VS\bin;C:\Windows\System32",
            "INCLUDE": r"C:\VS\include",
            "LIB": r"C:\VS\lib",
        }

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                    "alias": "fixture-proof",
                    "entryIndex": 7,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                    "runtimeProfile": "native-proof-output",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-native-aot-proof-build")
        try:
            for relative_path in [
                Path("src/native/proof-host/native_aot_main.cpp"),
                Path("artifacts")
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "analysis"
                / "generated"
                / "generated"
                / "native-aot.generated.cpp",
            ]:
                absolute_path = repo_root / relative_path
                absolute_path.parent.mkdir(parents=True, exist_ok=True)
                absolute_path.write_text("// fixture\n", encoding="utf-8")

            generated_manifest_path = repo_root / request["upstream"]["generated"]["manifestPath"]
            generated_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            generated_manifest_path.write_text(
                json.dumps(
                    {
                        "generatedSourcePath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-aot.generated.cpp"),
                        "nativeAotManifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.manifest.json"),
                    }
                ),
                encoding="utf-8",
            )

            workspace_manifest_path = repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json"
            workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/proof-host",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredProofHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredProofHost",
                                "hostKind": "proof-host",
                                "collectionPath": collection_path,
                            }
                        ],
                        "nativeTestProjects": [
                            {
                                "projectId": f"native-test/{subject_id}/{matrix_id}/proof-host",
                                "matrixId": matrix_id,
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "native",
                                    matrix_id,
                                    "proof",
                                    "chaos_subject_reference_proof.vcxproj",
                                ),
                                "configureRoot": expected_configure_root,
                                "hostKind": "proof-host",
                                "managedTestProjectId": f"managed-test/{subject_id}/proof-host",
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )

            expected_cmake_dir = repo_root / expected_configure_root
            expected_cmake_source_root = repo_root / "solutions" / "subjects" / subject_id / "native-source" / matrix_id
            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value=expected_env):
                    with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                        with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                            with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir") as allocate_cmake_binary_dir_mock:
                                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                                    with patch.object(workers_module, "_run_checked") as run_checked_mock:
                                        result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "-S",
                    str(expected_cmake_source_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Visual Studio 18 2026",
                    f"-DCHAOS_SUBJECT_HOST_MAIN={(repo_root / 'src' / 'native' / 'proof-host' / 'native_aot_main.cpp').as_posix()}",
                    f"-DCHAOS_SUBJECT_GENERATED_INPUT_SOURCE={(repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'analysis' / 'generated' / 'generated' / 'native-aot.generated.cpp').as_posix()}",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_BUILD_OUT_ROOT={(repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'out').as_posix()}",
                    f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "--build",
                    str(expected_cmake_dir),
                    "--config",
                    "Release",
                    "--target",
                    WINDOWS_NATIVE_AOT_BUILD_TARGET,
                ],
                run_checked_mock.call_args_list[1].args[0],
            )
            self.assertEqual(expected_env, run_checked_mock.call_args_list[0].kwargs["env"])
            self.assertEqual(expected_env, run_checked_mock.call_args_list[1].kwargs["env"])
            allocate_cmake_binary_dir_mock.assert_not_called()

            proof_cmakelists = (expected_cmake_source_root / "proof" / "CMakeLists.txt").read_text(encoding="utf-8")
            self.assertIn("chaos_subject_native_aot", proof_cmakelists)

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("CHECK", manifest["variant"])
            self.assertEqual("windows-native-aot-cmake", manifest["buildStrategy"])
            self.assertEqual("native-aot", manifest["buildKind"])
            self.assertEqual("proof-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
            self.assertEqual(
                f"managed-test/{subject_id}/proof-host",
                manifest["managedTestProjectId"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-aot.generated.cpp"),
                manifest["generatedSourcePath"],
            )
            self.assertEqual(
                posix_path("src", "native", "proof-host", "native_aot_main.cpp"),
                manifest["hostSourcePath"],
            )
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "out", f"{WINDOWS_NATIVE_AOT_BUILD_TARGET}.exe")],
                manifest["outputs"],
            )
            self.assertEqual(
                {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                    "alias": "fixture-proof",
                    "entryIndex": 7,
                },
                manifest["entrySelection"],
            )
            self.assertNotIn("dispatchManifestPath", manifest)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_windows_build_target_infers_benchmark_host_for_native_perf_without_declared_entry_selection(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_windows_native_aot_cmake_summary_build")
        subject_id = "FixtureNativeAotSummarySubject"
        run_id = "fixture-run-native-aot-summary-build-001"
        matrix_id = "windows-native-perf"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
        expected_cmake_dir = self._make_non_repo_path("cmake-builds", "subject-native-aot-summary-1234")
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "runtimeProfile": "native-perf-profile",
                    "toolchainProfile": "msvc-reference",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-native-aot-summary-build")
        try:
            for relative_path in [
                Path("src/native/benchmark-host/native_aot_main.cpp"),
                Path("artifacts")
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "analysis"
                / "generated"
                / "generated"
                / "native-aot.generated.cpp",
            ]:
                absolute_path = repo_root / relative_path
                absolute_path.parent.mkdir(parents=True, exist_ok=True)
                absolute_path.write_text("// fixture\n", encoding="utf-8")

            generated_manifest_path = repo_root / request["upstream"]["generated"]["manifestPath"]
            generated_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            generated_manifest_path.write_text(
                json.dumps(
                    {
                        "generatedSourcePath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated", "native-aot.generated.cpp"),
                        "nativeAotManifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "native-aot.manifest.json"),
                    }
                ),
                encoding="utf-8",
            )

            workspace_manifest_path = repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json"
            workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredBenchmarkHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                                "hostKind": "benchmark-host",
                                "collectionPath": collection_path,
                            }
                        ],
                        "nativeTestProjects": [
                            {
                                "projectId": f"native-test/{subject_id}/{matrix_id}/benchmark-host",
                                "matrixId": matrix_id,
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "native",
                                    matrix_id,
                                    "benchmark",
                                    "chaos_subject_native_aot.vcxproj",
                                ),
                                "configureRoot": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "native",
                                    matrix_id,
                                ),
                                "hostKind": "benchmark-host",
                                "managedTestProjectId": f"managed-test/{subject_id}/benchmark-host",
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )

            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value={}):
                    with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                        with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                            with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir", return_value=expected_cmake_dir):
                                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                                    with patch.object(workers_module, "_run_checked") as run_checked_mock:
                                        result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                "benchmark-host",
                json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))["hostKind"],
            )
            dispatch_manifest = json.loads(
                (
                    repo_root
                    / subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "benchmark.dispatch.manifest.json")
                ).read_text(encoding="utf-8")
            )
            self.assertEqual({}, dispatch_manifest["entrySelection"])
            self.assertEqual(2, run_checked_mock.call_count)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_native_runtime_perf_propagates_benchmark_host_contract_from_build_manifest(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_native_perf_workspace_contract")
        subject_id = "FixtureNativeAotSubject"
        run_id = "fixture-run-native-perf-workspace-contract-001"
        matrix_id = "windows-native-perf"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        dispatch_manifest_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "benchmark.dispatch.manifest.json",
        )
        native_executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            f"{WINDOWS_NATIVE_AOT_BUILD_TARGET}.exe",
        )
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "native-perf-profile",
                },
            },
            "upstream": {
                "build": {
                    "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("native-perf-workspace-contract")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "outputs": [native_executable_path],
                        "hostKind": "benchmark-host",
                        "collectionPath": collection_path,
                        "dispatchManifestPath": dispatch_manifest_path,
                    }
                ),
                encoding="utf-8",
            )
            (repo_root / dispatch_manifest_path).parent.mkdir(parents=True, exist_ok=True)
            (repo_root / dispatch_manifest_path).write_text("{}", encoding="utf-8")

            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "driver": "native-runtime-perf",
                                "defaultVariant": "PROFILE",
                                "harnessIterations": 4,
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 12.5, "opsPerSecond": 8000, "checksum": 42},
                "baselinePath": "subjects/FixtureNativeAotSubject/baselines/perf/windows-native-perf/windows.json",
                "baseline": {},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=[str(repo_root / native_executable_path)],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 12.5, "opsPerSecond": 8000, "checksum": 42}) + "\n",
                stderr="",
            )

            with patch.object(workers_module, "_perf_sample_count", return_value=1):
                with patch.object(workers_module, "_native_perf_warmup_count", return_value=0):
                    with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                        with patch.object(workers_module.time, "perf_counter", side_effect=[10.0, 10.5]):
                            with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                result = workers_module.run_native_runtime_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    str(repo_root / native_executable_path),
                    "--iterations",
                    "4",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
            self.assertEqual(dispatch_manifest_path, manifest["dispatchManifestPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_windows_validate_only_build_supports_ios_target_and_subject_scoped_host_root(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_windows_ios_validate_only")
        subject_id = "FixtureIosValidateOnlySubject"
        run_id = "fixture-run-ios-validate-only-001"
        matrix_id = "windows-ios-check"
        expected_cmake_dir = self._make_non_repo_path("cmake-builds", "ios-platform-gate-1234")
        instance_spec = f"{self._make_non_repo_path('visual-studio', '18', 'Professional')},version=18.4.11626.88"

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "ios-arm64",
                    "toolchainProfile": "ios-xcode",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-ios-validate-only")
        try:
            expected_ios_host_root = repo_root / "subjects" / subject_id / "validation" / "mobile" / "ios-host"
            expected_ios_host_root.mkdir(parents=True, exist_ok=True)
            with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                    with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir", return_value=expected_cmake_dir):
                        with patch.object(workers_module, "_run_checked") as run_checked_mock:
                            result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    "cmake",
                    "-S",
                    str(repo_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Visual Studio 18 2026",
                    "-DROADMAP0_PRESET_TARGET=ios-arm64-packaging",
                    "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_IOS_HOST_ROOT={expected_ios_host_root}",
                    f"-DCMAKE_TOOLCHAIN_FILE={repo_root / 'build' / 'toolchains' / 'ios-arm64.cmake'}",
                    f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                ["cmake", "--build", str(expected_cmake_dir)],
                run_checked_mock.call_args_list[1].args[0],
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("ios-arm64", manifest["targetPlatform"])
            self.assertEqual(expected_cmake_dir.as_posix(), manifest["cmakeBinaryDir"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_windows_android_runtime_build_uses_ninja_and_records_runtime_output(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_windows_android_runtime_build")
        subject_id = "FixtureAndroidRuntimeSubject"
        run_id = "fixture-run-android-runtime-build-001"
        matrix_id = "windows-android-runtime"
        expected_cmake_dir = self._make_non_repo_path("cmake-builds", "android-runtime-1234")
        expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
        expected_ninja_path = self._make_non_repo_path("cmake", "bin", "ninja.exe")
        expected_env = {
            "ANDROID_SDK_ROOT": r"C:\android\sdk",
            "ANDROID_NDK_ROOT": r"C:\android\sdk\ndk\26.3.11579264",
            "PATH": r"C:\android\sdk\platform-tools;C:\android\sdk\emulator",
        }

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "android-arm64",
                    "toolchainProfile": "android-ndk-r26",
                    "runtimeProfile": "android-native-runtime",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "generated", "generated.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-android-runtime-build")
        try:
            expected_android_host_root = repo_root / "subjects" / subject_id / "validation" / "mobile" / "android-host"
            expected_android_host_root.mkdir(parents=True, exist_ok=True)

            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), expected_env)):
                with patch.object(workers_module.tooling_module, "find_ninja_executable", return_value=str(expected_ninja_path)):
                    with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir", return_value=expected_cmake_dir):
                        with patch.object(workers_module, "_run_checked") as run_checked_mock:
                            result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            expected_output_root = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "build"
                / "out"
            )
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "-S",
                    str(repo_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Ninja",
                    "-DROADMAP0_PRESET_TARGET=android-arm64-smoke",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_ANDROID_HOST_ROOT={expected_android_host_root}",
                    f"-DCHAOS_SUBJECT_ANDROID_ARTIFACT_ROOT={expected_output_root}",
                    f"-DCMAKE_TOOLCHAIN_FILE={repo_root / 'build' / 'toolchains' / 'android-arm64.cmake'}",
                    "-DCMAKE_BUILD_TYPE=Release",
                    f"-DCMAKE_MAKE_PROGRAM={expected_ninja_path}",
                ],
                run_checked_mock.call_args_list[0].args[0],
            )
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "--build",
                    str(expected_cmake_dir),
                    "--target",
                    "mobile_hello_world_android_host_runtime",
                ],
                run_checked_mock.call_args_list[1].args[0],
            )
            self.assertEqual(expected_env, run_checked_mock.call_args_list[0].kwargs["env"])

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("android-native-cmake", manifest["buildStrategy"])
            self.assertEqual("android-arm64", manifest["targetPlatform"])
            self.assertEqual(expected_cmake_dir.as_posix(), manifest["cmakeBinaryDir"])
            self.assertEqual(
                [
                    subject_run_path(
                        subject_id,
                        run_id,
                        "matrices",
                        matrix_id,
                        "build",
                        "out",
                        "mobile_hello_world_android_host_runtime",
                    )
                ],
                manifest["outputs"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_observe_accepts_non_repo_cmake_binary_dir_from_build_manifest(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_runtime_observe")
        subject_id = "FixtureRuntimeObserveSubject"
        run_id = "fixture-run-runtime-observe-001"
        matrix_id = "windows-reference-check"

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
            },
            "upstream": {
                "build": {
                    "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-observe")
        try:
            expected_cmake_dir = self._make_non_repo_path("cmake-builds", "native-reference-run-1234")
            expected_cmake_path = self._make_non_repo_path("cmake", "bin", "cmake.exe")
            expected_env = {
                "Path": r"C:\VS\bin;C:\Windows\System32",
                "INCLUDE": r"C:\VS\include",
                "LIB": r"C:\VS\lib",
            }
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "cmakeBinaryDir": expected_cmake_dir.as_posix(),
                        "outputs": [],
                    }
                ),
                encoding="utf-8",
            )

            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value=expected_env):
                    with patch.object(workers_module, "_run_checked") as run_checked_mock:
                        result = workers_module.run_runtime_observe(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            build_args = run_checked_mock.call_args.args[0]
            self.assertEqual([str(expected_cmake_path), "--build"], build_args[:2])
            self.assertEqual(expected_cmake_dir, Path(build_args[2]))
            self.assertEqual(
                ["--config", "Release", "--target", WINDOWS_REFERENCE_RUN_TARGET],
                build_args[3:],
            )
            self.assertEqual(expected_env, run_checked_mock.call_args.kwargs["env"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_observe_executes_direct_msvc_build_output(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_runtime_observe_direct")
        subject_id = "FixtureRuntimeObserveSubject"
        run_id = "fixture-run-runtime-observe-direct-001"
        matrix_id = "windows-reference-check"
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            "chaos_subject_reference_proof.exe",
        )

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
            },
            "upstream": {
                "build": {
                    "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-observe-direct")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "buildStrategy": "direct-msvc",
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [str(repo_root / executable_path)],
                0,
                "native proof ok\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_runtime_observe(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [str(repo_root / executable_path)],
                cwd=repo_root / request["paths"]["bucketRoot"],
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                manifest["stdoutPath"],
            )
            self.assertEqual(
                "native proof ok\n",
                (repo_root / manifest["stdoutPath"]).read_text(encoding="utf-8"),
            )
            self.assertEqual(
                "0\n",
                (repo_root / manifest["exitCodePath"]).read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_observe_executes_native_aot_proof_host_output_with_declared_entry_arguments(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_runtime_observe_native_aot_proof_host")
        subject_id = "FixtureRuntimeObserveNativeAotProofSubject"
        run_id = "fixture-run-runtime-observe-native-aot-proof-001"
        matrix_id = "windows-native-check"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            f"{WINDOWS_NATIVE_AOT_BUILD_TARGET}.exe",
        )

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                    "alias": "fixture-proof",
                    "entryIndex": 7,
                },
                "executionContext": {
                    "runtimeArguments": [
                        "--heartbeat-interval-seconds=5",
                    ],
                },
            },
            "upstream": {
                "build": {
                    "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-observe-native-aot-proof")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "buildStrategy": "windows-native-aot-cmake",
                        "buildKind": "native-aot",
                        "hostKind": "proof-host",
                        "collectionPath": collection_path,
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [str(repo_root / executable_path)],
                0,
                "native aot proof ok\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_runtime_observe(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    str(repo_root / executable_path),
                    "--heartbeat-interval-seconds=5",
                    f"--collection-path={collection_path}",
                    "--entry-index=7",
                ],
                cwd=repo_root / request["paths"]["bucketRoot"],
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("proof-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                manifest["stdoutPath"],
            )
            self.assertEqual(
                "native aot proof ok\n",
                (repo_root / manifest["stdoutPath"]).read_text(encoding="utf-8"),
            )
            self.assertEqual(
                "0\n",
                (repo_root / manifest["exitCodePath"]).read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_observe_executes_native_aot_benchmark_host_output_for_subject_entry_selection(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_runtime_observe_native_aot_benchmark_host",
        )
        subject_id = "FixtureRuntimeObserveNativeAotBenchmarkSubject"
        run_id = "fixture-run-runtime-observe-native-aot-benchmark-001"
        matrix_id = "windows-native-check"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            f"{WINDOWS_NATIVE_AOT_BUILD_TARGET}.exe",
        )

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "source": {
                    "entry": f"{subject_id}/Program::RunSelected()",
                    "entrySelection": {
                        "entryKind": 1,
                        "entrySlice": 11,
                    },
                },
            },
            "upstream": {
                "build": {
                    "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-observe-native-aot-benchmark")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "buildStrategy": "windows-benchmark-cmake",
                        "buildKind": "native-aot",
                        "hostKind": "benchmark-host",
                        "collectionPath": collection_path,
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [str(repo_root / executable_path)],
                0,
                "{\"elapsedMilliseconds\":0.123,\"opsPerSecond\":8123.0,\"checksum\":42}\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_runtime_observe(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [str(repo_root / executable_path)],
                cwd=repo_root / request["paths"]["bucketRoot"],
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                manifest["stdoutPath"],
            )
            self.assertEqual(
                "{\"elapsedMilliseconds\":0.123,\"opsPerSecond\":8123.0,\"checksum\":42}\n",
                (repo_root / manifest["stdoutPath"]).read_text(encoding="utf-8"),
            )
            self.assertEqual(
                "0\n",
                (repo_root / manifest["exitCodePath"]).read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_observe_executes_android_native_build_output_via_adb_shell(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_runtime_observe_android")
        subject_id = "FixtureAndroidRuntimeSubject"
        run_id = "fixture-run-android-runtime-observe-001"
        matrix_id = "windows-android-runtime"
        executable_path = subject_run_path(
            subject_id,
            run_id,
            "matrices",
            matrix_id,
            "build",
            "out",
            "mobile_hello_world_android_host_runtime",
        )

        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "android-arm64",
                    "runtimeProfile": "android-native-runtime",
                    "runtimeArguments": [
                        "--soak-duration-seconds=180",
                        "--heartbeat-interval-seconds=15",
                    ],
                },
            },
            "upstream": {
                "build": {
                    "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "build.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-observe-android")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "buildStrategy": "android-native-cmake",
                        "outputs": [executable_path],
                    }
                ),
                encoding="utf-8",
            )

            android_result = subprocess.CompletedProcess(
                ["adb", "shell"],
                0,
                "mobile-host|stage=shared-host-bootstrap|detail=MobileHelloWorldProof|exitCode=0\n__CHAOS_EXIT_CODE__=0\n",
                "",
            )

            with patch.object(
                workers_module.tooling_module,
                "ensure_android_host_tooling_available",
                return_value=workers_module.tooling_module.ToolBootstrapResult(ready=True, output="android host ready\n"),
            ):
                with patch.object(
                    workers_module.tooling_module,
                    "android_environment_overrides",
                    return_value={
                        "ANDROID_SDK_ROOT": r"C:\android\sdk",
                        "ANDROID_AVD_HOME": r"C:\android\.android\avd",
                        "ANDROID_EMULATOR_HOME": r"C:\android\.android",
                        "PATH": r"C:\android\sdk\platform-tools;C:\android\sdk\emulator",
                    },
                ):
                    with patch.object(workers_module, "_launch_android_emulator", return_value=("emulator-5560", object(), None, None)) as launch_mock:
                        with patch.object(workers_module, "_wait_for_android_boot_completed") as wait_mock:
                            with patch.object(workers_module, "_run_android_binary_via_adb", return_value=android_result) as adb_mock:
                                with patch.object(workers_module, "_shutdown_android_emulator") as shutdown_mock:
                                    result = workers_module.run_runtime_observe(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            launch_mock.assert_called_once()
            wait_mock.assert_called_once()
            adb_mock.assert_called_once_with(
                repo_root=repo_root,
                executable_path=repo_root / executable_path,
                serial="emulator-5560",
                env={
                    "ANDROID_SDK_ROOT": r"C:\android\sdk",
                    "ANDROID_AVD_HOME": r"C:\android\.android\avd",
                    "ANDROID_EMULATOR_HOME": r"C:\android\.android",
                    "PATH": r"C:\android\sdk\platform-tools;C:\android\sdk\emulator",
                },
                runtime_arguments=[
                    "--soak-duration-seconds=180",
                    "--heartbeat-interval-seconds=15",
                ],
            )
            shutdown_mock.assert_called_once()

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("emulator-5560", manifest["androidSerial"])
            self.assertEqual(workers_module.tooling_module.ANDROID_AVD_NAME, manifest["androidAvdName"])
            self.assertEqual(
                ["--soak-duration-seconds=180", "--heartbeat-interval-seconds=15"],
                manifest["runtimeArguments"],
            )
            self.assertEqual(
                ["mobile-host|stage=shared-host-bootstrap|detail=MobileHelloWorldProof|exitCode=0"],
                manifest["outputLines"],
            )
            self.assertEqual(
                "mobile-host|stage=shared-host-bootstrap|detail=MobileHelloWorldProof|exitCode=0\n",
                (repo_root / manifest["stdoutPath"]).read_text(encoding="utf-8"),
            )
            self.assertEqual(
                "0\n",
                (repo_root / manifest["exitCodePath"]).read_text(encoding="utf-8"),
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_run_android_binary_via_adb_prefixes_known_runtime_arguments_as_environment_variables(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_android_runtime_arguments")
        repo_root = self._make_repo_root("android-runtime-arguments")
        executable_path = repo_root / "artifacts" / "mobile_hello_world_android_host_runtime"
        executable_path.parent.mkdir(parents=True, exist_ok=True)
        executable_path.write_text("fixture", encoding="utf-8")
        env = {
            "ANDROID_SDK_ROOT": r"C:\android\sdk",
            "PATH": r"C:\android\sdk\platform-tools",
        }

        try:
            with patch.object(workers_module, "_run_android_host_command") as host_command_mock:
                with patch.object(
                    workers_module,
                    "run_process",
                    return_value=subprocess.CompletedProcess(["adb", "shell"], 0, "__CHAOS_EXIT_CODE__=0\n", ""),
                ) as run_process_mock:
                    workers_module._run_android_binary_via_adb(
                        repo_root=repo_root,
                        executable_path=executable_path,
                        serial="emulator-5554",
                        env=env,
                        runtime_arguments=[
                            "--soak-duration-seconds=180",
                            "--heartbeat-interval-seconds=15",
                            "--subject-id=Probe",
                        ],
                    )

            self.assertEqual(3, host_command_mock.call_count)
            run_process_mock.assert_called_once()
            self.assertEqual(
                [
                    workers_module._android_adb_executable(repo_root),
                    "-s",
                    "emulator-5554",
                    "shell",
                    "sh",
                    "-c",
                    (
                        "export CHAOS_MOBILE_HOST_SOAK_DURATION_SECONDS=180 >/dev/null; "
                        "export CHAOS_MOBILE_HOST_HEARTBEAT_INTERVAL_SECONDS=15 >/dev/null; "
                        "export CHAOS_MOBILE_HOST_SUBJECT_ID=Probe >/dev/null; "
                        "/data/local/tmp/chaos-subjects/mobile_hello_world_android_host_runtime "
                        "--soak-duration-seconds=180 "
                        "--heartbeat-interval-seconds=15 "
                        "--subject-id=Probe; "
                        "status=$?; printf '\\n__CHAOS_EXIT_CODE__=%s\\n' \"$status\""
                    ),
                ],
                run_process_mock.call_args.args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_dotnet_host_input_builder_uses_temp_intermediate_root_on_windows(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_dotnet_host_input")
        subject_id = "FixtureHostInputSubject"
        run_id = "fixture-run-host-input-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "source": {
                    "type": "dotnet-project",
                    "path": subject_source_path(subject_id),
                    "entry": f"{subject_id}/Program::Main(System.String[])",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                },
            },
            "upstream": {
                "source": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "source", "source.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "host-input"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("host-input-build")
        try:
            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_run_checked") as run_checked_mock:
                    result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    "dotnet",
                    "build",
                    str(repo_root / "subjects" / subject_id / "source" / f"{subject_id}.csproj"),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
                run_checked_mock.call_args.args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_driver_build_uses_chaos_temp_intermediate_root_on_windows(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_driver_build")
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "driver-1234abcd"

        with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
            with patch.object(workers_module, "_run_checked") as run_checked_mock:
                dll_path = workers_module._ensure_driver_built(REPO_ROOT)

        self.assertEqual(REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll", dll_path)
        self.assertEqual(
            [
                "dotnet",
                "build",
                str(REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"),
                "-c",
                "Release",
                "-m:1",
                f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
            ],
            run_checked_mock.call_args.args[0],
        )

    def test_host_input_build_uses_primary_solution_project_and_records_primary_project_assembly(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_host_input_solution")
        subject_id = "FixtureSolutionHostInputSubject"
        run_id = "fixture-run-host-input-solution-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-solution-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "source": {
                    "type": "dotnet-project",
                    "path": posix_path("subjects", subject_id, "source", f"{subject_id}.sln"),
                    "primaryProjectPath": posix_path(
                        "subjects",
                        subject_id,
                        "source",
                        "App",
                        "FixtureSolutionHostInput.App.csproj",
                    ),
                    "entry": "FixtureSolutionHostInput.App/Program::Main(System.String[])",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                },
            },
            "upstream": {
                "source": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "source", "source.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "host-input"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("host-input-build-solution")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            (source_root / "App").mkdir(parents=True, exist_ok=True)
            (source_root / "Library").mkdir(parents=True, exist_ok=True)
            (source_root / f"{subject_id}.sln").write_text(
                "\n".join(
                    [
                        "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"FixtureSolutionHostInput.App\", \"App\\\\FixtureSolutionHostInput.App.csproj\", \"{11111111-1111-1111-1111-111111111111}\"",
                        "EndProject",
                        "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"FixtureSolutionHostInput.Helper\", \"Library\\\\FixtureSolutionHostInput.Helper.csproj\", \"{22222222-2222-2222-2222-222222222222}\"",
                        "EndProject",
                        "",
                    ]
                ),
                encoding="utf-8",
            )
            (source_root / "App" / "FixtureSolutionHostInput.App.csproj").write_text("<Project />\n", encoding="utf-8")
            (source_root / "Library" / "FixtureSolutionHostInput.Helper.csproj").write_text("<Project />\n", encoding="utf-8")
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        "build",
                        str(repo_root / "subjects" / subject_id / "source" / "App" / "FixtureSolutionHostInput.App.csproj"),
                        "-c",
                        "Release",
                        "-m:1",
                        "-o",
                        str(expected_output_root),
                        f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                    ],
                    arguments,
                )
                expected_output_root.mkdir(parents=True, exist_ok=True)
                for file_name in [
                    "FixtureSolutionHostInput.App.dll",
                    "FixtureSolutionHostInput.App.deps.json",
                    "FixtureSolutionHostInput.App.pdb",
                    "FixtureSolutionHostInput.Helper.dll",
                    "FixtureSolutionHostInput.Helper.pdb",
                    "Chaos.TestFramework.Sdk.dll",
                    "Newtonsoft.Json.dll",
                ]:
                    (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                    with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                        result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                posix_path("subjects", subject_id, "source", "App", "FixtureSolutionHostInput.App.csproj"),
                manifest["primaryProjectPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.App.dll"),
                manifest["primaryAssemblyPath"],
            )
            self.assertEqual(
                [
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.Helper.dll"),
                ],
                manifest["additionalAssemblyPaths"],
            )
            self.assertEqual(
                [
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "Chaos.TestFramework.Sdk.dll"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.App.deps.json"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.App.dll"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.App.pdb"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.Helper.dll"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.Helper.pdb"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "Newtonsoft.Json.dll"),
                ],
                manifest["files"],
            )
            self.assertEqual(
                [
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.App.dll"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "FixtureSolutionHostInput.Helper.dll"),
                ],
                result["primaryEvidencePaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_host_input_build_uses_workspace_managed_proof_host_for_declared_unit_test(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_host_input_workspace_proof_host")
        subject_id = "FixtureWorkspaceProofHostSubject"
        run_id = "fixture-run-host-input-workspace-proof-host-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-workspace-proof-host-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                    "alias": "workspace-proof",
                    "entryIndex": 7,
                },
                "source": {
                    "type": "dotnet-project",
                    "path": subject_source_path(subject_id),
                    "entry": f"{subject_id}/Program::Main(System.String[])",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                },
            },
            "upstream": {
                "source": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "source", "source.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "host-input"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("host-input-build-workspace-proof-host")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            source_root.mkdir(parents=True, exist_ok=True)
            (source_root / f"{subject_id}.csproj").write_text("<Project />\n", encoding="utf-8")

            workspace_root = repo_root / "solutions" / "subjects" / subject_id
            managed_tests_root = workspace_root / "managed-tests"
            generated_root = managed_tests_root / "Generated"
            generated_root.mkdir(parents=True, exist_ok=True)
            proof_host_project_path = managed_tests_root / f"{subject_id}.DeclaredProofHost.csproj"
            proof_host_project_path.write_text("<Project />\n", encoding="utf-8")
            collection_path = generated_root / "declared-tests.collection.json"
            collection_path.write_text('{"declaredUnitTests":[{"entryIndex":7}]}', encoding="utf-8")
            workspace_manifest_path = workspace_root / "workspace.manifest.json"
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/proof-host",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredProofHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredProofHost",
                                "hostKind": "proof-host",
                                "collectionPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        "build",
                        str(proof_host_project_path),
                        "-c",
                        "Release",
                        "-m:1",
                        "-o",
                        str(expected_output_root),
                        f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                    ],
                    arguments,
                )
                expected_output_root.mkdir(parents=True, exist_ok=True)
                for file_name in [
                    f"{subject_id}.DeclaredProofHost.dll",
                    f"{subject_id}.DeclaredProofHost.deps.json",
                    f"{subject_id}.DeclaredProofHost.pdb",
                    f"{subject_id}.dll",
                    "Chaos.TestFramework.Sdk.dll",
                    "Chaos.TestFramework.Runtime.dll",
                ]:
                    (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                    with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                        result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                posix_path("solutions", "subjects", subject_id, "managed-tests", f"{subject_id}.DeclaredProofHost.csproj"),
                manifest["primaryProjectPath"],
            )
            self.assertEqual("proof-host", manifest["hostKind"])
            self.assertEqual(
                posix_path("solutions", "subjects", subject_id, "managed-tests", "Generated", "declared-tests.collection.json"),
                manifest["collectionPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.DeclaredProofHost.dll"),
                manifest["primaryAssemblyPath"],
            )
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")],
                manifest["additionalAssemblyPaths"],
            )
            self.assertEqual(
                [
                    subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.DeclaredProofHost.dll"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll"),
                ],
                result["primaryEvidencePaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_host_input_build_uses_workspace_managed_benchmark_host_for_declared_benchmark(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_host_input_workspace_benchmark_host")
        subject_id = "FixtureWorkspaceBenchmarkHostSubject"
        run_id = "fixture-run-host-input-workspace-benchmark-host-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-workspace-benchmark-host-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Benchmarks::RunWorkload()",
                    "alias": "workspace-benchmark",
                    "entryIndex": 11,
                },
                "source": {
                    "type": "dotnet-project",
                    "path": subject_source_path(subject_id),
                    "entry": f"{subject_id}/Program::Main(System.String[])",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                },
            },
            "upstream": {
                "source": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "source", "source.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "host-input"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("host-input-build-workspace-benchmark-host")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            source_root.mkdir(parents=True, exist_ok=True)
            (source_root / f"{subject_id}.csproj").write_text("<Project />\n", encoding="utf-8")

            workspace_root = repo_root / "solutions" / "subjects" / subject_id
            managed_tests_root = workspace_root / "managed-tests"
            generated_root = managed_tests_root / "Generated"
            generated_root.mkdir(parents=True, exist_ok=True)
            benchmark_host_project_path = managed_tests_root / f"{subject_id}.DeclaredBenchmarkHost.csproj"
            benchmark_host_project_path.write_text("<Project />\n", encoding="utf-8")
            collection_path = generated_root / "declared-tests.collection.json"
            collection_path.write_text('{"declaredBenchmarks":[{"entryIndex":11}]}', encoding="utf-8")
            workspace_manifest_path = workspace_root / "workspace.manifest.json"
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredBenchmarkHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                                "hostKind": "benchmark-host",
                                "collectionPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        "build",
                        str(benchmark_host_project_path),
                        "-c",
                        "Release",
                        "-m:1",
                        "-o",
                        str(expected_output_root),
                        f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                    ],
                    arguments,
                )
                expected_output_root.mkdir(parents=True, exist_ok=True)
                for file_name in [
                    f"{subject_id}.DeclaredBenchmarkHost.dll",
                    f"{subject_id}.DeclaredBenchmarkHost.deps.json",
                    f"{subject_id}.DeclaredBenchmarkHost.pdb",
                    f"{subject_id}.dll",
                    "Chaos.TestFramework.Sdk.dll",
                    "Chaos.TestFramework.Runtime.dll",
                ]:
                    (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                    with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                        result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                posix_path("solutions", "subjects", subject_id, "managed-tests", f"{subject_id}.DeclaredBenchmarkHost.csproj"),
                manifest["primaryProjectPath"],
            )
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(
                posix_path("solutions", "subjects", subject_id, "managed-tests", "Generated", "declared-tests.collection.json"),
                manifest["collectionPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.DeclaredBenchmarkHost.dll"),
                manifest["primaryAssemblyPath"],
            )
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")],
                manifest["additionalAssemblyPaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_host_input_build_keeps_source_project_for_subject_entry_selection_managed_output(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_host_input_subject_entry_managed_output",
        )
        subject_id = "FixtureWorkspaceManagedEntryOutputSubject"
        run_id = "fixture-run-host-input-subject-entry-managed-output-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-subject-entry-managed-output-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": "windows-managed-output",
                "source": {
                    "type": "dotnet-project",
                    "path": subject_source_path(subject_id),
                    "entry": f"{subject_id}/Program::Main(System.String[])",
                    "entrySelection": {
                        "entryKind": 1,
                        "entrySlice": 3,
                    },
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                },
            },
            "upstream": {
                "source": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "source", "source.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "host-input"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("host-input-build-subject-entry-managed-output")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            source_root.mkdir(parents=True, exist_ok=True)
            source_project_path = source_root / f"{subject_id}.csproj"
            source_project_path.write_text("<Project />\n", encoding="utf-8")

            workspace_root = repo_root / "solutions" / "subjects" / subject_id
            managed_tests_root = workspace_root / "managed-tests"
            generated_root = managed_tests_root / "Generated"
            generated_root.mkdir(parents=True, exist_ok=True)
            benchmark_host_project_path = managed_tests_root / f"{subject_id}.DeclaredBenchmarkHost.csproj"
            benchmark_host_project_path.write_text("<Project />\n", encoding="utf-8")
            collection_path = generated_root / "declared-tests.collection.json"
            collection_path.write_text('{"declaredBenchmarks":[{"entryIndex":11}]}', encoding="utf-8")
            workspace_manifest_path = workspace_root / "workspace.manifest.json"
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredBenchmarkHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                                "hostKind": "benchmark-host",
                                "collectionPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        "build",
                        str(source_project_path),
                        "-c",
                        "Release",
                        "-m:1",
                        "-o",
                        str(expected_output_root),
                        f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                    ],
                    arguments,
                )
                expected_output_root.mkdir(parents=True, exist_ok=True)
                for file_name in [
                    f"{subject_id}.dll",
                    f"{subject_id}.deps.json",
                    f"{subject_id}.pdb",
                    "Chaos.TestFramework.Sdk.dll",
                ]:
                    (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                    with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                        result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_source_path(subject_id), manifest["primaryProjectPath"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll"),
                manifest["primaryAssemblyPath"],
            )
            self.assertEqual([], manifest["additionalAssemblyPaths"])
            self.assertNotIn("hostKind", manifest)
            self.assertNotIn("collectionPath", manifest)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_host_input_build_regenerates_stale_workspace_before_using_declared_benchmark_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_host_input_workspace_benchmark_host_refresh")
        subject_id = "FixtureWorkspaceBenchmarkHostRefreshSubject"
        run_id = "fixture-run-host-input-workspace-benchmark-host-refresh-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-workspace-benchmark-host-refresh-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Benchmarks::RunWorkload()",
                    "alias": "workspace-benchmark-refresh",
                    "entryIndex": 11,
                },
                "source": {
                    "type": "dotnet-project",
                    "path": subject_source_path(subject_id),
                    "entry": f"{subject_id}/Program::Main(System.String[])",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                },
            },
            "upstream": {
                "source": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "source", "source.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "host-input"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("host-input-build-workspace-benchmark-host-refresh")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            source_root.mkdir(parents=True, exist_ok=True)
            (source_root / f"{subject_id}.csproj").write_text("<Project />\n", encoding="utf-8")

            workspace_root = repo_root / "solutions" / "subjects" / subject_id
            stale_managed_tests_root = workspace_root / "managed-tests"
            stale_generated_root = stale_managed_tests_root / "Generated"
            stale_generated_root.mkdir(parents=True, exist_ok=True)
            stale_host_project_path = stale_managed_tests_root / "StaleBenchmarkHost.csproj"
            stale_host_project_path.write_text("<Project />\n", encoding="utf-8")
            stale_collection_path = stale_generated_root / "declared-tests.collection.json"
            stale_collection_path.write_text('{"declaredBenchmarks":[{"entryIndex":1}]}', encoding="utf-8")
            workspace_manifest_path = workspace_root / "workspace.manifest.json"
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "StaleBenchmarkHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                                "hostKind": "benchmark-host",
                                "collectionPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )

            time.sleep(1.1)
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "source": {
                            "type": "dotnet-project",
                            "path": subject_source_path(subject_id),
                            "primaryProjectPath": subject_source_path(subject_id),
                        },
                    }
                ),
                encoding="utf-8",
            )

            refreshed_host_project_path = stale_managed_tests_root / f"{subject_id}.DeclaredBenchmarkHost.csproj"
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"

            def fake_generate_subject_workspace(
                repo_root_arg: Path,
                host_platform_arg: str,
                options_arg: dict[str, object],
                **kwargs: object,
            ) -> dict[str, object]:
                del kwargs
                self.assertEqual(repo_root, repo_root_arg)
                self.assertEqual("windows", host_platform_arg)
                self.assertEqual(f"subject/{subject_id}", options_arg["id"])
                self.assertTrue(bool(options_arg["all-targets"]))
                self.assertTrue(bool(options_arg["refresh-generated"]))

                refreshed_host_project_path.write_text("<Project />\n", encoding="utf-8")
                stale_collection_path.write_text('{"declaredBenchmarks":[{"entryIndex":11}]}', encoding="utf-8")
                workspace_manifest_path.write_text(
                    json.dumps(
                        {
                            "workspaceVersion": 2,
                            "subjectId": subject_id,
                            "managedTestProjects": [
                                {
                                    "projectId": f"managed-test/{subject_id}/benchmark-host",
                                    "projectPath": posix_path(
                                        "solutions",
                                        "subjects",
                                        subject_id,
                                        "managed-tests",
                                        f"{subject_id}.DeclaredBenchmarkHost.csproj",
                                    ),
                                    "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                                    "hostKind": "benchmark-host",
                                    "collectionPath": posix_path(
                                        "solutions",
                                        "subjects",
                                        subject_id,
                                        "managed-tests",
                                        "Generated",
                                        "declared-tests.collection.json",
                                    ),
                                }
                            ],
                        }
                    ),
                    encoding="utf-8",
                )
                return {
                    "manifestPath": posix_path("solutions", "subjects", subject_id, "workspace.manifest.json"),
                }

            class FakeProjectWorkspaceModule:
                @staticmethod
                def generate_subject_workspace(
                    repo_root_arg: Path,
                    host_platform_arg: str,
                    options_arg: dict[str, object],
                    **kwargs: object,
                ) -> dict[str, object]:
                    return fake_generate_subject_workspace(
                        repo_root_arg,
                        host_platform_arg,
                        options_arg,
                        **kwargs,
                    )

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        "build",
                        str(refreshed_host_project_path),
                        "-c",
                        "Release",
                        "-m:1",
                        "-o",
                        str(expected_output_root),
                        f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                    ],
                    arguments,
                )
                expected_output_root.mkdir(parents=True, exist_ok=True)
                for file_name in [
                    f"{subject_id}.DeclaredBenchmarkHost.dll",
                    f"{subject_id}.DeclaredBenchmarkHost.deps.json",
                    f"{subject_id}.DeclaredBenchmarkHost.pdb",
                    f"{subject_id}.dll",
                    "Chaos.TestFramework.Sdk.dll",
                    "Chaos.TestFramework.Runtime.dll",
                ]:
                    (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_load_project_workspace_module", return_value=FakeProjectWorkspaceModule()):
                    with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                        result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                posix_path("solutions", "subjects", subject_id, "managed-tests", f"{subject_id}.DeclaredBenchmarkHost.csproj"),
                manifest["primaryProjectPath"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_host_input_build_uses_native_specific_benchmark_host_when_native_test_project_declares_it(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_host_input_workspace_native_benchmark_host",
        )
        subject_id = "FixtureWorkspaceNativeBenchmarkHostSubject"
        run_id = "fixture-run-host-input-workspace-native-benchmark-host-001"
        matrix_id = "windows-native-perf"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-workspace-native-benchmark-host-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Benchmarks::RunNativeWorkload()",
                    "alias": "workspace-native-benchmark",
                    "entryIndex": 11,
                },
                "source": {
                    "type": "dotnet-project",
                    "path": subject_source_path(subject_id),
                    "entry": f"{subject_id}/Program::Main(System.String[])",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "native-perf-release",
                    "targetPlatform": "windows-x64",
                },
            },
            "upstream": {
                "source": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "source", "source.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "host-input"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("host-input-build-workspace-native-benchmark-host")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            source_root.mkdir(parents=True, exist_ok=True)
            (source_root / f"{subject_id}.csproj").write_text("<Project />\n", encoding="utf-8")

            workspace_root = repo_root / "solutions" / "subjects" / subject_id
            managed_tests_root = workspace_root / "managed-tests"
            generated_root = managed_tests_root / "Generated"
            generated_root.mkdir(parents=True, exist_ok=True)
            benchmark_host_project_path = managed_tests_root / f"{subject_id}.DeclaredBenchmarkHost.csproj"
            benchmark_host_project_path.write_text("<Project />\n", encoding="utf-8")
            native_benchmark_host_project_path = managed_tests_root / f"{subject_id}.DeclaredBenchmarkNativeHost.csproj"
            native_benchmark_host_project_path.write_text("<Project />\n", encoding="utf-8")
            collection_path = generated_root / "declared-tests.collection.json"
            collection_path.write_text('{"declaredBenchmarks":[{"entryIndex":11}]}', encoding="utf-8")
            workspace_manifest_path = workspace_root / "workspace.manifest.json"
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredBenchmarkHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                                "hostKind": "benchmark-host",
                                "collectionPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                            },
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host-native",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredBenchmarkNativeHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.DeclaredBenchmarkNativeHost",
                                "hostKind": "benchmark-host",
                                "collectionPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                            },
                        ],
                        "nativeTestProjects": [
                            {
                                "projectId": f"native-test/{subject_id}/{matrix_id}/benchmark-host",
                                "matrixId": matrix_id,
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "native",
                                    matrix_id,
                                    "benchmark",
                                    "chaos_subject_native_aot.vcxproj",
                                ),
                                "configureRoot": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "native",
                                    matrix_id,
                                ),
                                "hostKind": "benchmark-host",
                                "managedTestProjectId": f"managed-test/{subject_id}/benchmark-host-native",
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del failure_message
                self.assertEqual(
                    [
                        "dotnet",
                        "build",
                        str(native_benchmark_host_project_path),
                        "-c",
                        "Release",
                        "-m:1",
                        "-o",
                        str(expected_output_root),
                        f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                    ],
                    arguments,
                )
                expected_output_root.mkdir(parents=True, exist_ok=True)
                for file_name in [
                    f"{subject_id}.DeclaredBenchmarkNativeHost.dll",
                    f"{subject_id}.DeclaredBenchmarkNativeHost.deps.json",
                    f"{subject_id}.DeclaredBenchmarkNativeHost.pdb",
                    f"{subject_id}.dll",
                    "Chaos.TestFramework.Sdk.dll",
                    "Chaos.TestFramework.Runtime.dll",
                ]:
                    (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_workspace_manifest_is_stale", return_value=False):
                    with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                        result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                posix_path("solutions", "subjects", subject_id, "managed-tests", f"{subject_id}.DeclaredBenchmarkNativeHost.csproj"),
                manifest["primaryProjectPath"],
            )
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(
                posix_path("solutions", "subjects", subject_id, "managed-tests", "Generated", "declared-tests.collection.json"),
                manifest["collectionPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.DeclaredBenchmarkNativeHost.dll"),
                manifest["primaryAssemblyPath"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_workspace_manifest_generation_does_not_reenter_itself(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_workspace_manifest_reentry_guard")
        subject_id = "FixtureWorkspaceManifestReentryGuardSubject"
        selection = {
            "subjectId": subject_id,
            "executionContext": {
                "hostPlatform": "windows-x64",
            },
        }

        repo_root = self._make_repo_root("workspace-manifest-reentry-guard")
        try:
            workspace_manifest_path = repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json"
            nested_results: list[tuple[Path, dict[str, object]] | None] = []
            generate_calls = 0

            class FakeProjectWorkspaceModule:
                @staticmethod
                def generate_subject_workspace(
                    repo_root_arg: Path,
                    host_platform_arg: str,
                    options_arg: dict[str, object],
                    **kwargs: object,
                ) -> dict[str, object]:
                    nonlocal generate_calls
                    del kwargs
                    generate_calls += 1
                    self.assertEqual(repo_root, repo_root_arg)
                    self.assertEqual("windows", host_platform_arg)
                    self.assertEqual(f"subject/{subject_id}", options_arg["id"])
                    self.assertTrue(bool(options_arg["all-targets"]))
                    self.assertTrue(bool(options_arg["refresh-generated"]))
                    self.assertTrue(bool(options_arg["auto-refresh-missing-generated"]))

                    nested_results.append(workers_module._ensure_subject_workspace_manifest(repo_root, selection))

                    workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
                    workspace_manifest_path.write_text(
                        json.dumps(
                            {
                                "workspaceVersion": 2,
                                "subjectId": subject_id,
                                "managedTestProjects": [],
                                "nativeTestProjects": [],
                                "hotupdateTestProjects": [],
                            }
                        ),
                        encoding="utf-8",
                    )
                    return {
                        "manifestPath": posix_path("solutions", "subjects", subject_id, "workspace.manifest.json"),
                    }

            with patch.object(workers_module, "_load_project_workspace_module", return_value=FakeProjectWorkspaceModule):
                loaded_manifest = workers_module._ensure_subject_workspace_manifest(repo_root, selection)

            self.assertIsNotNone(loaded_manifest)
            self.assertEqual(1, generate_calls)
            self.assertEqual([None], nested_results)
            self.assertEqual(workspace_manifest_path, loaded_manifest[0])
            self.assertEqual(subject_id, loaded_manifest[1]["subjectId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_workspace_manifest_generation_passes_declared_entry_selection_to_project_workspace(self) -> None:
        workers_module = load_module(
            SUBJECT_WORKERS_MODULE_PATH,
            "chaos_subject_workers_workspace_manifest_declared_entry_selection",
        )
        subject_id = "FixtureWorkspaceDeclaredEntrySelectionSubject"
        selection = {
            "subjectId": subject_id,
            "matrixId": "windows-native-check",
            "variant": "CHECK",
            "entrySelection": {
                "family": "declared-unit-test",
                "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                "alias": "workspace-proof",
                "entryIndex": 7,
            },
            "executionContext": {
                "hostPlatform": "windows-x64",
            },
        }

        repo_root = self._make_repo_root("workspace-manifest-declared-entry-selection")
        try:
            workspace_manifest_path = repo_root / "solutions" / "subjects" / subject_id / "workspace.manifest.json"

            class FakeProjectWorkspaceModule:
                @staticmethod
                def generate_subject_workspace(
                    repo_root_arg: Path,
                    host_platform_arg: str,
                    options_arg: dict[str, object],
                    **kwargs: object,
                ) -> dict[str, object]:
                    del kwargs
                    self.assertEqual(repo_root, repo_root_arg)
                    self.assertEqual("windows", host_platform_arg)
                    self.assertEqual(f"subject/{subject_id}", options_arg["id"])
                    self.assertEqual("windows-native-check", options_arg["matrix"])
                    self.assertEqual("CHECK", options_arg["variant"])
                    self.assertTrue(bool(options_arg["refresh-generated"]))
                    self.assertTrue(bool(options_arg["auto-refresh-missing-generated"]))
                    self.assertEqual(selection["entrySelection"], options_arg["entry-selection"])
                    self.assertNotIn("all-targets", options_arg)

                    workspace_manifest_path.parent.mkdir(parents=True, exist_ok=True)
                    workspace_manifest_path.write_text(
                        json.dumps(
                            {
                                "workspaceVersion": 2,
                                "subjectId": subject_id,
                                "managedTestProjects": [],
                                "nativeTestProjects": [],
                                "hotupdateTestProjects": [],
                            }
                        ),
                        encoding="utf-8",
                    )
                    return {
                        "manifestPath": posix_path("solutions", "subjects", subject_id, "workspace.manifest.json"),
                    }

            with patch.object(workers_module, "_load_project_workspace_module", return_value=FakeProjectWorkspaceModule):
                loaded_manifest = workers_module._ensure_subject_workspace_manifest(repo_root, selection)

            self.assertIsNotNone(loaded_manifest)
            self.assertEqual(workspace_manifest_path, loaded_manifest[0])
            self.assertEqual(subject_id, loaded_manifest[1]["subjectId"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_workspace_manifest_refreshes_when_workspace_templates_are_newer(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_workspace_manifest_template_refresh")
        subject_id = "FixtureWorkspaceTemplateRefreshSubject"
        selection = {
            "subjectId": subject_id,
            "executionContext": {
                "hostPlatform": "windows-x64",
            },
        }

        repo_root = self._make_repo_root("workspace-manifest-template-refresh")
        try:
            source_project_path = repo_root / "subjects" / subject_id / "source" / f"{subject_id}.csproj"
            source_project_path.parent.mkdir(parents=True, exist_ok=True)
            source_project_path.write_text("<Project />\n", encoding="utf-8")

            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "source": {
                            "type": "dotnet-project",
                            "path": posix_path("subjects", subject_id, "source", f"{subject_id}.csproj"),
                            "entry": f"{subject_id}/Program::Main()",
                        },
                    }
                ),
                encoding="utf-8",
            )

            workspace_root = repo_root / "solutions" / "subjects" / subject_id
            generated_root = workspace_root / "managed-tests" / "Generated"
            generated_root.mkdir(parents=True, exist_ok=True)
            project_path = workspace_root / "managed-tests" / f"{subject_id}.DeclaredBenchmarkHost.csproj"
            collection_path = generated_root / "declared-tests.collection.json"
            generated_source_path = generated_root / "ChaosGeneratedDeclaredBenchmarks.g.cs"
            project_path.write_text("<Project />\n", encoding="utf-8")
            collection_path.write_text("{\"declaredBenchmarks\":[]}\n", encoding="utf-8")
            generated_source_path.write_text("// stale host\n", encoding="utf-8")

            workspace_manifest_path = workspace_root / "workspace.manifest.json"
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "managedTestProjects": [
                            {
                                "projectId": f"managed-test/{subject_id}/benchmark-host",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    f"{subject_id}.DeclaredBenchmarkHost.csproj",
                                ),
                                "hostKind": "benchmark-host",
                                "collectionPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                                "generatedSourcePath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "managed-tests",
                                    "Generated",
                                    "ChaosGeneratedDeclaredBenchmarks.g.cs",
                                ),
                            }
                        ],
                        "nativeTestProjects": [],
                        "hotupdateTestProjects": [],
                    }
                ),
                encoding="utf-8",
            )

            template_path = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "templates" / "managed-benchmark-host.cs.tmpl"
            stale_time = template_path.stat().st_mtime - 60
            for candidate in (
                source_project_path,
                subject_manifest_path,
                project_path,
                collection_path,
                generated_source_path,
                workspace_manifest_path,
            ):
                os.utime(candidate, (stale_time, stale_time))

            self.assertTrue(
                workers_module._workspace_manifest_is_stale(
                    repo_root,
                    subject_id=subject_id,
                    manifest_path=workspace_manifest_path,
                    manifest=json.loads(workspace_manifest_path.read_text(encoding="utf-8")),
                )
            )

            generate_calls = 0

            class FakeProjectWorkspaceModule:
                @staticmethod
                def generate_subject_workspace(
                    repo_root_arg: Path,
                    host_platform_arg: str,
                    options_arg: dict[str, object],
                    **kwargs: object,
                ) -> dict[str, object]:
                    nonlocal generate_calls
                    del kwargs
                    generate_calls += 1
                    self.assertEqual(repo_root, repo_root_arg)
                    self.assertEqual("windows", host_platform_arg)
                    self.assertEqual(f"subject/{subject_id}", options_arg["id"])

                    workspace_manifest_path.write_text(
                        json.dumps(
                            {
                                "workspaceVersion": 2,
                                "subjectId": subject_id,
                                "managedTestProjects": [],
                                "nativeTestProjects": [],
                                "hotupdateTestProjects": [],
                            }
                        ),
                        encoding="utf-8",
                    )
                    return {
                        "manifestPath": posix_path("solutions", "subjects", subject_id, "workspace.manifest.json"),
                    }

            with patch.object(workers_module, "_load_project_workspace_module", return_value=FakeProjectWorkspaceModule):
                loaded_manifest = workers_module._ensure_subject_workspace_manifest(repo_root, selection)

            self.assertIsNotNone(loaded_manifest)
            self.assertEqual(1, generate_calls)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_host_input_build_uses_workspace_hotupdate_host_and_records_binding_manifest(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_host_input_workspace_hotupdate_host")
        subject_id = "FixtureHotUpdateHostSubject"
        run_id = "fixture-run-host-input-workspace-hotupdate-host-001"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-host-input-workspace-hotupdate-host-1234"
        request = {
            "selection": {
                "subjectId": subject_id,
                "engineeringProfile": "hot-update-host",
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}.Patch::{subject_id}.Patch.Proofs::Run()",
                    "alias": "workspace-hotupdate-proof",
                    "entryIndex": 3,
                },
                "source": {
                    "type": "dotnet-project",
                    "path": posix_path("subjects", subject_id, "source", f"{subject_id}.sln"),
                    "primaryProjectPath": posix_path("subjects", subject_id, "source", "Host", f"{subject_id}.Host.csproj"),
                    "entry": f"{subject_id}.Host/Program::Main()",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "hot-update-proof",
                },
            },
            "upstream": {
                "source": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "source", "source.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "analysis", "host-input"),
                "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("host-input-build-workspace-hotupdate-host")
        try:
            source_root = repo_root / "subjects" / subject_id / "source"
            host_project_path = source_root / "Host" / f"{subject_id}.Host.csproj"
            patch_project_path = source_root / "Patch" / f"{subject_id}.Patch.csproj"
            solution_path = source_root / f"{subject_id}.sln"
            host_project_path.parent.mkdir(parents=True, exist_ok=True)
            patch_project_path.parent.mkdir(parents=True, exist_ok=True)
            host_project_path.write_text(
                "\n".join(
                    [
                        "<Project>",
                        "  <PropertyGroup>",
                        f"    <AssemblyName>{subject_id}.Host</AssemblyName>",
                        "  </PropertyGroup>",
                        "</Project>",
                        "",
                    ]
                ),
                encoding="utf-8",
            )
            patch_project_path.write_text(
                "\n".join(
                    [
                        "<Project>",
                        "  <PropertyGroup>",
                        f"    <AssemblyName>{subject_id}.Patch</AssemblyName>",
                        "  </PropertyGroup>",
                        "</Project>",
                        "",
                    ]
                ),
                encoding="utf-8",
            )
            solution_path.write_text(
                "\n".join(
                    [
                        "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"Host\", \"Host\\\\"
                        f"{subject_id}.Host.csproj\", \"{{11111111-1111-1111-1111-111111111111}}\"",
                        "EndProject",
                        "Project(\"{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}\") = \"Patch\", \"Patch\\\\"
                        f"{subject_id}.Patch.csproj\", \"{{22222222-2222-2222-2222-222222222222}}\"",
                        "EndProject",
                        "",
                    ]
                ),
                encoding="utf-8",
            )

            workspace_root = repo_root / "solutions" / "subjects" / subject_id
            hotupdate_tests_root = workspace_root / "hotupdate-tests"
            generated_root = hotupdate_tests_root / "Generated"
            generated_root.mkdir(parents=True, exist_ok=True)
            hotupdate_host_project_path = hotupdate_tests_root / f"{subject_id}.HotUpdateProofHost.csproj"
            hotupdate_host_project_path.write_text("<Project />\n", encoding="utf-8")
            binding_manifest_path = generated_root / "declared-tests.binding.json"
            binding_manifest_path.write_text(
                json.dumps(
                    {
                        "hostKind": "proof-host",
                        "patchAssemblyNames": [f"{subject_id}.Patch"],
                    }
                ),
                encoding="utf-8",
            )
            collection_path = generated_root / "declared-tests.collection.json"
            collection_path.write_text('{"declaredUnitTests":[{"entryIndex":3}]}', encoding="utf-8")
            workspace_manifest_path = workspace_root / "workspace.manifest.json"
            workspace_manifest_path.write_text(
                json.dumps(
                    {
                        "workspaceVersion": 2,
                        "subjectId": subject_id,
                        "hotupdatePatchProjects": [
                            {
                                "projectId": f"hotupdate-patch/{subject_id}/{subject_id}_Patch",
                                "managedProjectId": f"managed/{subject_id}/{subject_id}_Patch",
                                "projectPath": posix_path("subjects", subject_id, "source", "Patch", f"{subject_id}.Patch.csproj"),
                                "assemblyName": f"{subject_id}.Patch",
                            }
                        ],
                        "hotupdateTestProjects": [
                            {
                                "projectId": f"hotupdate-test/{subject_id}/proof-host",
                                "projectPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "hotupdate-tests",
                                    f"{subject_id}.HotUpdateProofHost.csproj",
                                ),
                                "assemblyName": f"{subject_id}.HotUpdateProofHost",
                                "hostKind": "proof-host",
                                "collectionPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "hotupdate-tests",
                                    "Generated",
                                    "declared-tests.collection.json",
                                ),
                                "bindingManifestPath": posix_path(
                                    "solutions",
                                    "subjects",
                                    subject_id,
                                    "hotupdate-tests",
                                    "Generated",
                                    "declared-tests.binding.json",
                                ),
                                "patchProjectIds": [f"hotupdate-patch/{subject_id}/{subject_id}_Patch"],
                            }
                        ],
                        "matrices": [
                            {
                                "matrixId": "windows-hotupdate-proof",
                                "hotupdatePatchProjectIds": [f"hotupdate-patch/{subject_id}/{subject_id}_Patch"],
                                "hotupdateTestProjectIds": [f"hotupdate-test/{subject_id}/proof-host"],
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )
            expected_output_root = repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"
            captured_arguments: list[list[str]] = []

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str) -> str:
                del repo_root, failure_message
                captured_arguments.append(list(arguments))
                expected_output_root.mkdir(parents=True, exist_ok=True)
                project_argument = arguments[2]
                if hotupdate_host_project_path.name in project_argument:
                    for file_name in [
                        f"{subject_id}.HotUpdateProofHost.dll",
                        f"{subject_id}.HotUpdateProofHost.deps.json",
                        f"{subject_id}.HotUpdateProofHost.pdb",
                        "Chaos.TestFramework.Sdk.dll",
                        "Chaos.TestFramework.Runtime.dll",
                    ]:
                        (expected_output_root / file_name).write_text("", encoding="utf-8")
                elif host_project_path.name in project_argument:
                    for file_name in [f"{subject_id}.Host.dll", f"{subject_id}.Host.pdb"]:
                        (expected_output_root / file_name).write_text("", encoding="utf-8")
                elif patch_project_path.name in project_argument:
                    for file_name in [f"{subject_id}.Patch.dll", f"{subject_id}.Patch.pdb"]:
                        (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                    result = workers_module.run_dotnet_host_input_builder(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    "dotnet",
                    "build",
                    str(hotupdate_host_project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(expected_output_root),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
                captured_arguments[0],
            )
            self.assertEqual(
                [
                    "dotnet",
                    "build",
                    str(host_project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(expected_output_root),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
                captured_arguments[1],
            )
            self.assertEqual(
                [
                    "dotnet",
                    "build",
                    str(patch_project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(expected_output_root),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
                captured_arguments[2],
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                posix_path("solutions", "subjects", subject_id, "hotupdate-tests", f"{subject_id}.HotUpdateProofHost.csproj"),
                manifest["primaryProjectPath"],
            )
            self.assertEqual("proof-host", manifest["hostKind"])
            self.assertEqual(
                posix_path("solutions", "subjects", subject_id, "hotupdate-tests", "Generated", "declared-tests.collection.json"),
                manifest["collectionPath"],
            )
            self.assertEqual(
                posix_path("solutions", "subjects", subject_id, "hotupdate-tests", "Generated", "declared-tests.binding.json"),
                manifest["bindingManifestPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.HotUpdateProofHost.dll"),
                manifest["primaryAssemblyPath"],
            )
            self.assertEqual(
                [
                    subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.Host.dll"),
                    subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.Patch.dll"),
                ],
                manifest["additionalAssemblyPaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_managed_output_runs_primary_assembly_and_records_runtime_manifest(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_managed_runtime_output")
        subject_id = "FixtureManagedRuntimeSubject"
        run_id = "fixture-run-managed-output-001"
        matrix_id = "windows-managed-output"
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "executionContext": {
                    "hostPlatform": "windows-x64",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("managed-runtime-output")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": assembly_path,
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                ["dotnet", str(repo_root / assembly_path)],
                0,
                "fixture entry reached.\nargs=0\nregister:Main\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_managed_runtime_output(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(subject_id, manifest["subjectId"])
            self.assertEqual(matrix_id, manifest["matrixId"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                manifest["stdoutPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stderr.log"),
                manifest["stderrPath"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "exit-code.txt"),
                manifest["exitCodePath"],
            )
            self.assertEqual(
                ["fixture entry reached.", "args=0", "register:Main"],
                manifest["outputLines"],
            )
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                result["diagnostics"]["stdoutPath"],
            )
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log")],
                result["primaryEvidencePaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_managed_output_uses_collection_arguments_for_managed_proof_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_managed_runtime_output_proof_host")
        subject_id = "FixtureManagedRuntimeSubject"
        run_id = "fixture-run-managed-output-proof-host-001"
        matrix_id = "windows-managed-output"
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.DeclaredProofHost.dll")
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                    "alias": "managed-proof",
                    "entryIndex": 7,
                },
                "source": {
                    "entrySelection": {
                        "entryKind": 1,
                        "entrySlice": 3,
                    },
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeArguments": [
                        "--heartbeat-interval-seconds=5",
                    ],
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("managed-runtime-output-proof-host")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": assembly_path,
                        "hostKind": "proof-host",
                        "collectionPath": collection_path,
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=5",
                    f"--collection-path={collection_path}",
                    "--entry-index=7",
                ],
                0,
                "managed proof host reached.\nargs=3\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_managed_runtime_output(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=5",
                    f"--collection-path={collection_path}",
                    "--entry-index=7",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                [
                    "--heartbeat-interval-seconds=5",
                    f"--collection-path={collection_path}",
                    "--entry-index=7",
                ],
                manifest["arguments"],
            )
            self.assertEqual(
                {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}::{subject_id}.Proofs::Run()",
                    "alias": "managed-proof",
                    "entryIndex": 7,
                },
                manifest["declaredEntrySelection"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_managed_output_passes_binding_manifest_for_hotupdate_proof_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_managed_runtime_output_hotupdate_proof_host")
        subject_id = "FixtureHotUpdateRuntimeSubject"
        run_id = "fixture-run-managed-output-hotupdate-proof-host-001"
        matrix_id = "windows-hotupdate-output"
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.HotUpdateProofHost.dll")
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "hotupdate-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        binding_manifest_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "hotupdate-tests",
            "Generated",
            "declared-tests.binding.json",
        )
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": f"{subject_id}::{subject_id}.Patch::{subject_id}.Patch.Proofs::Run()",
                    "alias": "hotupdate-proof",
                    "entryIndex": 4,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeArguments": [
                        "--heartbeat-interval-seconds=3",
                    ],
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("managed-runtime-output-hotupdate-proof-host")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": assembly_path,
                        "hostKind": "proof-host",
                        "collectionPath": collection_path,
                        "bindingManifestPath": binding_manifest_path,
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=3",
                    f"--collection-path={collection_path}",
                    f"--binding-manifest-path={binding_manifest_path}",
                    "--entry-index=4",
                ],
                0,
                "managed hotupdate proof host reached.\nargs=4\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_managed_runtime_output(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=3",
                    f"--collection-path={collection_path}",
                    f"--binding-manifest-path={binding_manifest_path}",
                    "--entry-index=4",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(binding_manifest_path, manifest["bindingManifestPath"])
            self.assertEqual(
                [
                    "--heartbeat-interval-seconds=3",
                    f"--collection-path={collection_path}",
                    f"--binding-manifest-path={binding_manifest_path}",
                    "--entry-index=4",
                ],
                manifest["arguments"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_managed_output_passes_runtime_and_subject_entry_selection_arguments(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_managed_runtime_output_entry_selection")
        subject_id = "FixtureManagedRuntimeSubject"
        run_id = "fixture-run-managed-output-entry-selection-001"
        matrix_id = "windows-managed-output"
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "source": {
                    "entrySelection": {
                        "entryKind": 1,
                        "entrySlice": 3,
                    },
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeArguments": [
                        "--heartbeat-interval-seconds=5",
                    ],
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("managed-runtime-output-entry-selection")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": assembly_path,
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=5",
                    "--chaos-entry-kind=1",
                    "--chaos-entry-slice=3",
                ],
                0,
                "fixture selected entry reached.\nargs=3\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_managed_runtime_output(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=5",
                    "--chaos-entry-kind=1",
                    "--chaos-entry-slice=3",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                [
                    "--heartbeat-interval-seconds=5",
                    "--chaos-entry-kind=1",
                    "--chaos-entry-slice=3",
                ],
                manifest["arguments"],
            )
            self.assertEqual(
                {
                    "entryKind": 1,
                    "entrySlice": 3,
                },
                manifest["subjectEntrySelection"],
            )
            self.assertEqual(
                ["fixture selected entry reached.", "args=3"],
                manifest["outputLines"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_managed_output_does_not_append_declared_unit_source_entry_argument(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_managed_runtime_output_declared_entry")
        subject_id = "FixtureManagedRuntimeSubject"
        run_id = "fixture-run-managed-output-declared-entry-001"
        matrix_id = "windows-managed-output"
        source_entry = "HotUpdateHostPack/MetadataSupplementProofEntry::Run()"
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "entrySelection": {
                    "family": "declared-unit-test",
                    "stableId": "HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.MetadataSupplementProofEntry::Run()",
                    "alias": "metadata-supplement-proof",
                },
                "source": {
                    "entry": source_entry,
                    "entrySelection": {
                        "entryKind": 1,
                        "entrySlice": 1,
                    },
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeArguments": [
                        "--heartbeat-interval-seconds=5",
                    ],
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("managed-runtime-output-declared-entry")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": assembly_path,
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=5",
                    "--chaos-entry-kind=1",
                    "--chaos-entry-slice=1",
                ],
                0,
                "metadata proof selected.\nargs=3\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_managed_runtime_output(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(repo_root / assembly_path),
                    "--heartbeat-interval-seconds=5",
                    "--chaos-entry-kind=1",
                    "--chaos-entry-slice=1",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(
                [
                    "--heartbeat-interval-seconds=5",
                    "--chaos-entry-kind=1",
                    "--chaos-entry-slice=1",
                ],
                manifest["arguments"],
            )
            self.assertEqual(
                ["metadata proof selected.", "args=3"],
                manifest["outputLines"],
            )
            self.assertEqual(
                {
                    "family": "declared-unit-test",
                    "stableId": "HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.MetadataSupplementProofEntry::Run()",
                    "alias": "metadata-supplement-proof",
                },
                manifest["declaredEntrySelection"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_trace_compare_uses_host_input_manifest_and_platform_snapshot(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_runtime_trace_compare")
        subject_id = "FixtureTraceSubject"
        run_id = "fixture-run-trace-compare-001"
        matrix_id = "macos-managed-trace"
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "CHECK",
                "executionContext": {
                    "hostPlatform": "macos-arm64",
                    "targetPlatform": "macos-arm64",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                },
                "runtime": {
                    "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                },
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [
                    subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "trace-compare.report.json"),
                ],
            },
        }

        repo_root = self._make_repo_root("runtime-trace-compare")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll"),
                    }
                ),
                encoding="utf-8",
            )

            runtime_manifest_path = repo_root / request["paths"]["manifestPath"]
            runtime_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            runtime_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "matrixId": matrix_id,
                        "bucket": "runtime",
                        "stdoutPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stdout.log"),
                        "stderrPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "stderr.log"),
                        "exitCodePath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "exit-code.txt"),
                        "tracePaths": [],
                    }
                ),
                encoding="utf-8",
            )

            trace_path = repo_root / request["paths"]["bucketRoot"] / "trace.runtime.json"
            calls: list[list[str]] = []

            def fake_run_checked(arguments: list[str], *, repo_root: Path, failure_message: str, env=None) -> str:
                del failure_message, env
                calls.append(arguments)
                if len(calls) == 1:
                    self.assertEqual(
                        [
                            "dotnet",
                            str(repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input" / f"{subject_id}.dll"),
                            "--trace-platform",
                            "macos",
                            "--trace-output",
                            str(trace_path),
                        ],
                        arguments,
                    )
                    trace_path.write_text("{}", encoding="utf-8")
                elif len(calls) == 2:
                    self.assertEqual(
                        [
                            sys.executable,
                            str(repo_root / "tests" / "contracts" / "trace" / "compare-warmup-trace.py"),
                            str(repo_root / "tests" / "contracts" / "trace" / "snapshots" / "macos-warmup-trace.snapshot.json"),
                            str(trace_path),
                        ],
                        arguments,
                    )
                else:
                    self.fail(f"unexpected _run_checked call: {arguments}")
                return ""

            with patch.object(workers_module, "_run_checked", side_effect=fake_run_checked):
                result = workers_module.run_runtime_trace_compare(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "trace.runtime.json")],
                result["primaryEvidencePaths"],
            )

            manifest = json.loads(runtime_manifest_path.read_text(encoding="utf-8"))
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "trace.runtime.json")],
                manifest["tracePaths"],
            )
            self.assertEqual(request["upstream"]["host-input"]["manifestPath"], manifest["hostInputManifestPath"])

            report = json.loads((repo_root / request["paths"]["reportPaths"][0]).read_text(encoding="utf-8"))
            self.assertEqual("tests/contracts/trace/schema/warmup-trace.schema.json", report["schemaPath"])
            self.assertEqual("tests/contracts/trace/snapshots/macos-warmup-trace.snapshot.json", report["expectedSnapshotPath"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "trace.runtime.json"),
                report["actualTracePath"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_perf_collect_passes_workload_entry_and_assembly_path_to_perf_harness(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_runtime_perf_workload_entry")
        subject_id = "FixtureBenchSubject"
        run_id = "fixture-run-managed-perf-001"
        matrix_id = "windows-managed-perf"
        workload_entry = f"{subject_id}/Program::RunWorkload()"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-managed-perf-1234"
        perf_project_path = (
            f"subjects/{subject_id}/validation/perf/"
            f"{subject_id}.Subject.PerfHarness/{subject_id}.Subject.PerfHarness.csproj"
        )
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": workload_entry,
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "managed-perf-dev",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-perf-workload-entry")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps({"primaryAssemblyPath": assembly_path}),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "csharp-perf-harness",
                                "defaultVariant": "PROFILE",
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 12.5, "opsPerSecond": 8000, "checksum": 42},
                "baselinePath": "subjects/FixtureBenchSubject/baselines/perf/windows-managed-perf/windows.json",
                "baseline": {"meanDurationMs": 12.0},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 12.5, "opsPerSecond": 8000, "checksum": 42}) + "\n",
                stderr="",
            )
            harness_dll_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "runtime"
                / "harness"
                / f"{subject_id}.Subject.PerfHarness.dll"
            )

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_run_checked", return_value="") as run_checked_mock:
                    with patch.object(workers_module, "_perf_sample_count", return_value=1):
                        with patch.object(workers_module, "_perf_harness_iterations", return_value=13):
                            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                                with patch.object(workers_module.time, "perf_counter", side_effect=[10.0, 10.5]):
                                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                        result = workers_module.run_runtime_perf_collect(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    "dotnet",
                    "build",
                    str(repo_root / perf_project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "matrices" / matrix_id / "runtime" / "harness"),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
                run_checked_mock.call_args.args[0],
            )
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "13",
                    "--assembly",
                    str(repo_root / assembly_path),
                    "--workload-entry",
                    workload_entry,
                    "--mode",
                    "managed",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("legacy-workload-entry", manifest["benchmarkResolutionKind"])
            self.assertEqual(workload_entry, manifest["workloadEntry"])
            self.assertEqual(assembly_path, manifest["workloadAssemblyPath"])
            self.assertEqual(perf_project_path, manifest["perfHarnessProjectPath"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "harness", f"{subject_id}.Subject.PerfHarness.dll"),
                manifest["perfHarnessDllPath"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_interpreter_runtime_perf_passes_workload_entry_and_assembly_path_to_harness(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_interpreter_perf_workload_entry")
        subject_id = "FixtureBenchSubject"
        run_id = "fixture-run-interpreter-perf-001"
        matrix_id = "windows-interpreter-perf"
        workload_entry = f"{subject_id}/Program::RunWorkload()"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-interpreter-perf-1234"
        perf_project_path = (
            f"subjects/{subject_id}/validation/perf/"
            f"{subject_id}.Subject.PerfHarness/{subject_id}.Subject.PerfHarness.csproj"
        )
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": workload_entry,
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "interpreter-perf-dev",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("interpreter-perf-workload-entry")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps({"primaryAssemblyPath": assembly_path}),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "csharp-perf-harness",
                                "defaultVariant": "PROFILE",
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 18.25, "opsPerSecond": 5600, "checksum": 84},
                "baselinePath": "subjects/FixtureBenchSubject/baselines/perf/windows-interpreter-perf/windows.json",
                "baseline": {"meanDurationMs": 18.0},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 18.25, "opsPerSecond": 5600, "checksum": 84}) + "\n",
                stderr="",
            )
            harness_dll_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "runtime"
                / "harness"
                / f"{subject_id}.Subject.PerfHarness.dll"
            )

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_run_checked", return_value="") as run_checked_mock:
                    with patch.object(workers_module, "_perf_sample_count", return_value=1):
                        with patch.object(workers_module, "_perf_harness_iterations", return_value=21):
                            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                                with patch.object(workers_module.time, "perf_counter", side_effect=[20.0, 20.75]):
                                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                        result = workers_module.run_interpreter_runtime_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    "dotnet",
                    "build",
                    str(repo_root / perf_project_path),
                    "-c",
                    "Release",
                    "-m:1",
                    "-o",
                    str(repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "matrices" / matrix_id / "runtime" / "harness"),
                    f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
                ],
                run_checked_mock.call_args.args[0],
            )
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "21",
                    "--assembly",
                    str(repo_root / assembly_path),
                    "--workload-entry",
                    workload_entry,
                    "--mode",
                    "interpreter",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("legacy-workload-entry", manifest["benchmarkResolutionKind"])
            self.assertEqual("interpreter", manifest["mode"])
            self.assertEqual(workload_entry, manifest["workloadEntry"])
            self.assertEqual(assembly_path, manifest["workloadAssemblyPath"])
            self.assertEqual(perf_project_path, manifest["harnessProjectPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_perf_collect_uses_matching_additional_solution_assembly_for_workload_entry(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_solution_slice_assembly")
        subject_id = "SolutionCorePack"
        run_id = "fixture-run-managed-perf-solution-slice-001"
        matrix_id = "windows-managed-perf"
        workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-managed-perf-solution-slice"
        perf_project_path = "src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj"
        primary_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "SolutionCorePack.dll")
        slice_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "CoreRuntimeBenchmarks.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": workload_entry,
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "managed-perf-release",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-perf-solution-slice-assembly")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": primary_assembly_path,
                        "additionalAssemblyPaths": [slice_assembly_path],
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "native-runtime-perf",
                                "defaultVariant": "PROFILE",
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 12.5, "opsPerSecond": 8000, "checksum": 42},
                "baselinePath": "subjects/SolutionCorePack/baselines/perf/windows-managed-perf/windows.json",
                "baseline": {},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 12.5, "opsPerSecond": 8000, "checksum": 42}) + "\n",
                stderr="",
            )
            harness_dll_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "runtime"
                / "harness"
                / "Benchmark.WorkloadEntry.PerfHarness.dll"
            )

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_run_checked", return_value=""):
                    with patch.object(workers_module, "_perf_sample_count", return_value=1):
                        with patch.object(workers_module, "_perf_harness_iterations", return_value=4):
                            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                                with patch.object(workers_module.time, "perf_counter", side_effect=[10.0, 10.5]):
                                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                        result = workers_module.run_runtime_perf_collect(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "4",
                    "--assembly",
                    str(repo_root / slice_assembly_path),
                    "--workload-entry",
                    workload_entry,
                    "--mode",
                    "managed",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(slice_assembly_path, manifest["workloadAssemblyPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_perf_collect_resolves_declared_benchmark_by_entry_index_for_structured_harness_execution(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_declared_benchmark_entry_index")
        subject_id = "SolutionCorePack"
        run_id = "fixture-run-managed-perf-declared-entry-index-001"
        matrix_id = "windows-managed-perf"
        stable_id = "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry::RunWorkload()"
        alias = "arithmetic-bench"
        legacy_workload_entry = "LegacyBenchmarks/LegacyEntry::RunWorkload()"
        resolved_workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-managed-perf-declared-entry-index"
        perf_project_path = "src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj"
        primary_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "SolutionCorePack.dll")
        slice_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "CoreRuntimeBenchmarks.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": legacy_workload_entry,
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 11,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "managed-perf-release",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-perf-declared-entry-index")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": primary_assembly_path,
                        "additionalAssemblyPaths": [slice_assembly_path],
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "csharp-perf-harness",
                                "defaultVariant": "PROFILE",
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 12.5, "opsPerSecond": 8000, "checksum": 42},
                "baselinePath": "subjects/SolutionCorePack/baselines/perf/windows-managed-perf/windows.json",
                "baseline": {},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 12.5, "opsPerSecond": 8000, "checksum": 42}) + "\n",
                stderr="",
            )
            harness_dll_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "runtime"
                / "harness"
                / "Benchmark.WorkloadEntry.PerfHarness.dll"
            )

            workspace_catalog = {
                "declaredBenchmarks": [
                    {
                        "stableId": stable_id,
                        "entryIndex": 11,
                        "alias": alias,
                        "assemblyName": "CoreRuntimeBenchmarks",
                        "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                    }
                ]
            }

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_run_checked", return_value=""):
                    with patch.object(workers_module.workspace_declared_collection_module, "load_workspace_declared_collection", return_value=workspace_catalog):
                        with patch.object(workers_module, "_perf_sample_count", return_value=1):
                            with patch.object(workers_module, "_perf_harness_iterations", return_value=4):
                                with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                                    with patch.object(workers_module.time, "perf_counter", side_effect=[10.0, 10.5]):
                                        with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                            result = workers_module.run_runtime_perf_collect(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "4",
                    "--assembly",
                    str(repo_root / slice_assembly_path),
                    "--assembly-name",
                    "CoreRuntimeBenchmarks",
                    "--declaring-type",
                    "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                    "--method-name",
                    "RunWorkload",
                    "--method-signature",
                    "RunWorkload()",
                    "--mode",
                    "managed",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("declared-reflection", manifest["benchmarkResolutionKind"])
            self.assertEqual(resolved_workload_entry, manifest["workloadEntry"])
            self.assertEqual(slice_assembly_path, manifest["workloadAssemblyPath"])
            self.assertEqual(11, manifest["declaredEntrySelection"]["entryIndex"])
            self.assertEqual(11, manifest["declaredBenchmark"]["entryIndex"])
            self.assertEqual("CoreRuntimeBenchmarks", manifest["declaredBenchmark"]["assemblyName"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_perf_collect_uses_collection_arguments_for_workspace_benchmark_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_workspace_benchmark_host")
        subject_id = "SolutionCorePack"
        run_id = "fixture-run-managed-perf-workspace-benchmark-host-001"
        matrix_id = "windows-managed-perf"
        stable_id = "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry::RunWorkload()"
        alias = "arithmetic-bench"
        resolved_workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        intermediate_root = TEST_TMP_ROOT / "dotnet-intermediates" / "fixture-managed-perf-workspace-benchmark-host"
        perf_project_path = "src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj"
        benchmark_host_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "SolutionCorePack.DeclaredBenchmarkHost.dll")
        slice_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "CoreRuntimeBenchmarks.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": "LegacyBenchmarks/LegacyEntry::RunWorkload()",
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 11,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "managed-perf-release",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-perf-workspace-benchmark-host")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": benchmark_host_assembly_path,
                        "additionalAssemblyPaths": [slice_assembly_path],
                        "hostKind": "benchmark-host",
                        "collectionPath": collection_path,
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "csharp-perf-harness",
                                "defaultVariant": "PROFILE",
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 12.5, "opsPerSecond": 8000, "checksum": 42},
                "baselinePath": "subjects/SolutionCorePack/baselines/perf/windows-managed-perf/windows.json",
                "baseline": {},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 12.5, "opsPerSecond": 8000, "checksum": 42}) + "\n",
                stderr="",
            )
            harness_dll_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "runtime"
                / "harness"
                / "Benchmark.WorkloadEntry.PerfHarness.dll"
            )

            workspace_catalog = {
                "declaredBenchmarks": [
                    {
                        "stableId": stable_id,
                        "entryIndex": 11,
                        "alias": alias,
                        "assemblyName": "CoreRuntimeBenchmarks",
                        "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                    }
                ]
            }

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
                with patch.object(workers_module, "_run_checked", return_value=""):
                    with patch.object(workers_module.workspace_declared_collection_module, "load_workspace_declared_collection", return_value=workspace_catalog):
                        with patch.object(workers_module, "_perf_sample_count", return_value=1):
                            with patch.object(workers_module, "_perf_harness_iterations", return_value=4):
                                with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                                    with patch.object(workers_module.time, "perf_counter", side_effect=[10.0, 10.5]):
                                        with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                            result = workers_module.run_runtime_perf_collect(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "4",
                    "--host-assembly",
                    str(repo_root / benchmark_host_assembly_path),
                    "--collection-path",
                    collection_path,
                    "--entry-index",
                    "11",
                    "--mode",
                    "managed",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("collection-entry", manifest["benchmarkResolutionKind"])
            self.assertEqual(resolved_workload_entry, manifest["workloadEntry"])
            self.assertEqual(slice_assembly_path, manifest["workloadAssemblyPath"])
            self.assertEqual(11, manifest["declaredEntrySelection"]["entryIndex"])
            self.assertEqual(11, manifest["declaredBenchmark"]["entryIndex"])
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_perf_collect_passes_binding_manifest_for_hotupdate_benchmark_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_hotupdate_benchmark_host")
        subject_id = "HotUpdateHostPack"
        run_id = "fixture-run-managed-perf-hotupdate-benchmark-host-001"
        matrix_id = "windows-managed-perf"
        stable_id = "HotUpdateHostPack::HotUpdateHostPack.Patch::HotUpdateHostPack.Patch.Benchmarks::RunWorkload()"
        alias = "hotupdate-bench"
        resolved_workload_entry = "HotUpdateHostPack.Patch/Benchmarks::RunWorkload()"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "hotupdate-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        binding_manifest_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "hotupdate-tests",
            "Generated",
            "declared-tests.binding.json",
        )
        perf_project_path = "src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj"
        benchmark_host_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "HotUpdateHostPack.HotUpdateBenchmarkHost.dll")
        slice_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "HotUpdateHostPack.Patch.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": "LegacyBenchmarks/LegacyEntry::RunWorkload()",
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 8,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "managed-perf-release",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-perf-hotupdate-benchmark-host")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": benchmark_host_assembly_path,
                        "additionalAssemblyPaths": [slice_assembly_path],
                        "hostKind": "benchmark-host",
                        "collectionPath": collection_path,
                        "bindingManifestPath": binding_manifest_path,
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "csharp-perf-harness",
                                "defaultVariant": "PROFILE",
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 14.5, "opsPerSecond": 6000, "checksum": 42},
                "baselinePath": "subjects/HotUpdateHostPack/baselines/perf/windows-managed-perf/windows.json",
                "baseline": {},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 14.5, "opsPerSecond": 6000, "checksum": 42}) + "\n",
                stderr="",
            )
            harness_dll_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "runtime"
                / "harness"
                / "Benchmark.WorkloadEntry.PerfHarness.dll"
            )

            workspace_catalog = {
                "declaredBenchmarks": [
                    {
                        "stableId": stable_id,
                        "entryIndex": 8,
                        "alias": alias,
                        "assemblyName": "HotUpdateHostPack.Patch",
                        "declaringType": "HotUpdateHostPack.Patch.Benchmarks",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                    }
                ]
            }

            with patch.object(workers_module, "_run_checked", return_value=""):
                with patch.object(workers_module.workspace_declared_collection_module, "load_workspace_declared_collection", return_value=workspace_catalog):
                    with patch.object(workers_module, "_perf_sample_count", return_value=1):
                        with patch.object(workers_module, "_perf_harness_iterations", return_value=4):
                            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                                with patch.object(workers_module.time, "perf_counter", side_effect=[10.0, 10.5]):
                                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                        result = workers_module.run_runtime_perf_collect(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "4",
                    "--host-assembly",
                    str(repo_root / benchmark_host_assembly_path),
                    "--collection-path",
                    collection_path,
                    "--entry-index",
                    "8",
                    "--binding-manifest-path",
                    binding_manifest_path,
                    "--mode",
                    "managed",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(binding_manifest_path, manifest["bindingManifestPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_interpreter_runtime_perf_uses_collection_arguments_for_workspace_benchmark_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_interpreter_perf_workspace_benchmark_host")
        subject_id = "SolutionCorePack"
        run_id = "fixture-run-interpreter-perf-workspace-benchmark-host-001"
        matrix_id = "windows-interpreter-perf"
        stable_id = "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry::RunWorkload()"
        alias = "arithmetic-bench"
        resolved_workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "managed-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        perf_project_path = "src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj"
        benchmark_host_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "SolutionCorePack.DeclaredBenchmarkHost.dll")
        slice_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "CoreRuntimeBenchmarks.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": "LegacyBenchmarks/LegacyEntry::RunWorkload()",
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 11,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "interpreter-perf-release",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("interpreter-perf-workspace-benchmark-host")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": benchmark_host_assembly_path,
                        "additionalAssemblyPaths": [slice_assembly_path],
                        "hostKind": "benchmark-host",
                        "collectionPath": collection_path,
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "interpreter-runtime-perf",
                                "defaultVariant": "PROFILE",
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 20.0, "opsPerSecond": 5000, "checksum": 42},
                "baselinePath": "subjects/SolutionCorePack/baselines/perf/windows-interpreter-perf/windows.json",
                "baseline": {},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 20.0, "opsPerSecond": 5000, "checksum": 42}) + "\n",
                stderr="",
            )
            harness_dll_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "runtime"
                / "harness"
                / "Benchmark.WorkloadEntry.PerfHarness.dll"
            )

            workspace_catalog = {
                "declaredBenchmarks": [
                    {
                        "stableId": stable_id,
                        "entryIndex": 11,
                        "alias": alias,
                        "assemblyName": "CoreRuntimeBenchmarks",
                        "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                    }
                ]
            }

            with patch.object(workers_module, "_run_checked", return_value=""):
                with patch.object(workers_module.workspace_declared_collection_module, "load_workspace_declared_collection", return_value=workspace_catalog):
                    with patch.object(workers_module, "_perf_sample_count", return_value=1):
                        with patch.object(workers_module, "_perf_harness_iterations", return_value=4):
                            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                                with patch.object(workers_module.time, "perf_counter", side_effect=[20.0, 20.75]):
                                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                        result = workers_module.run_interpreter_runtime_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "4",
                    "--host-assembly",
                    str(repo_root / benchmark_host_assembly_path),
                    "--collection-path",
                    collection_path,
                    "--entry-index",
                    "11",
                    "--mode",
                    "interpreter",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("collection-entry", manifest["benchmarkResolutionKind"])
            self.assertEqual(resolved_workload_entry, manifest["workloadEntry"])
            self.assertEqual(slice_assembly_path, manifest["workloadAssemblyPath"])
            self.assertEqual(11, manifest["declaredEntrySelection"]["entryIndex"])
            self.assertEqual(11, manifest["declaredBenchmark"]["entryIndex"])
            self.assertEqual("benchmark-host", manifest["hostKind"])
            self.assertEqual(collection_path, manifest["collectionPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_interpreter_runtime_perf_passes_binding_manifest_for_hotupdate_benchmark_host(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_interpreter_perf_hotupdate_benchmark_host")
        subject_id = "HotUpdateHostPack"
        run_id = "fixture-run-interpreter-perf-hotupdate-benchmark-host-001"
        matrix_id = "windows-interpreter-perf"
        stable_id = "HotUpdateHostPack::HotUpdateHostPack.Patch::HotUpdateHostPack.Patch.Benchmarks::RunWorkload()"
        alias = "hotupdate-bench"
        resolved_workload_entry = "HotUpdateHostPack.Patch/Benchmarks::RunWorkload()"
        collection_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "hotupdate-tests",
            "Generated",
            "declared-tests.collection.json",
        )
        binding_manifest_path = posix_path(
            "solutions",
            "subjects",
            subject_id,
            "hotupdate-tests",
            "Generated",
            "declared-tests.binding.json",
        )
        perf_project_path = "src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj"
        benchmark_host_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "HotUpdateHostPack.HotUpdateBenchmarkHost.dll")
        slice_assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", "HotUpdateHostPack.Patch.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": "LegacyBenchmarks/LegacyEntry::RunWorkload()",
                "entrySelection": {
                    "family": "declared-benchmark",
                    "stableId": stable_id,
                    "alias": alias,
                    "entryIndex": 8,
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "interpreter-perf-release",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("interpreter-perf-hotupdate-benchmark-host")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps(
                    {
                        "primaryAssemblyPath": benchmark_host_assembly_path,
                        "additionalAssemblyPaths": [slice_assembly_path],
                        "hostKind": "benchmark-host",
                        "collectionPath": collection_path,
                        "bindingManifestPath": binding_manifest_path,
                    }
                ),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "interpreter-runtime-perf",
                                "defaultVariant": "PROFILE",
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 20.0, "opsPerSecond": 5000, "checksum": 42},
                "baselinePath": "subjects/HotUpdateHostPack/baselines/perf/windows-interpreter-perf/windows.json",
                "baseline": {},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 20.0, "opsPerSecond": 5000, "checksum": 42}) + "\n",
                stderr="",
            )
            harness_dll_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "runtime"
                / "harness"
                / "Benchmark.WorkloadEntry.PerfHarness.dll"
            )

            workspace_catalog = {
                "declaredBenchmarks": [
                    {
                        "stableId": stable_id,
                        "entryIndex": 8,
                        "alias": alias,
                        "assemblyName": "HotUpdateHostPack.Patch",
                        "declaringType": "HotUpdateHostPack.Patch.Benchmarks",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                    }
                ]
            }

            with patch.object(workers_module, "_run_checked", return_value=""):
                with patch.object(workers_module.workspace_declared_collection_module, "load_workspace_declared_collection", return_value=workspace_catalog):
                    with patch.object(workers_module, "_perf_sample_count", return_value=1):
                        with patch.object(workers_module, "_perf_harness_iterations", return_value=4):
                            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                                with patch.object(workers_module.time, "perf_counter", side_effect=[20.0, 20.75]):
                                    with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                        result = workers_module.run_interpreter_runtime_perf(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "4",
                    "--host-assembly",
                    str(repo_root / benchmark_host_assembly_path),
                    "--collection-path",
                    collection_path,
                    "--entry-index",
                    "8",
                    "--binding-manifest-path",
                    binding_manifest_path,
                    "--mode",
                    "interpreter",
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(binding_manifest_path, manifest["bindingManifestPath"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_perf_collect_prefers_subject_harness_iterations_override(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_perf_iteration_override")
        subject_id = "FixtureBenchSubject"
        run_id = "fixture-run-managed-perf-iterations-override-001"
        matrix_id = "windows-managed-perf"
        workload_entry = f"{subject_id}/Program::RunWorkload()"
        perf_project_path = (
            f"subjects/{subject_id}/validation/perf/"
            f"{subject_id}.Subject.PerfHarness/{subject_id}.Subject.PerfHarness.csproj"
        )
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": workload_entry,
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "managed-perf-release",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-perf-iterations-override")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps({"primaryAssemblyPath": assembly_path}),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "csharp-perf-harness",
                                "defaultVariant": "PROFILE",
                                "harnessIterations": 7,
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 12.5, "opsPerSecond": 8000, "checksum": 42},
                "baselinePath": "subjects/FixtureBenchSubject/baselines/perf/windows-managed-perf/windows.json",
                "baseline": {"meanDurationMs": 12.0},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 12.5, "opsPerSecond": 8000, "checksum": 42}) + "\n",
                stderr="",
            )
            harness_dll_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "runtime"
                / "harness"
                / f"{subject_id}.Subject.PerfHarness.dll"
            )

            with patch.object(workers_module, "_run_checked", return_value=""):
                with patch.object(workers_module, "_perf_sample_count", return_value=1):
                    with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                        with patch.object(workers_module.time, "perf_counter", side_effect=[10.0, 10.5]):
                            with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                workers_module.run_runtime_perf_collect(repo_root=repo_root, request=request)

            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "7",
                    "--assembly",
                    str(repo_root / assembly_path),
                    "--workload-entry",
                    workload_entry,
                    "--mode",
                    "managed",
                ],
                cwd=repo_root,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_interpreter_runtime_perf_prefers_subject_harness_iterations_override(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_interpreter_iteration_override")
        subject_id = "FixtureBenchSubject"
        run_id = "fixture-run-interpreter-perf-iterations-override-001"
        matrix_id = "windows-interpreter-perf"
        workload_entry = f"{subject_id}/Program::RunWorkload()"
        perf_project_path = (
            f"subjects/{subject_id}/validation/perf/"
            f"{subject_id}.Subject.PerfHarness/{subject_id}.Subject.PerfHarness.csproj"
        )
        assembly_path = subject_run_path(subject_id, run_id, "analysis", "host-input", f"{subject_id}.dll")
        request = {
            "selection": {
                "subjectId": subject_id,
                "matrixId": matrix_id,
                "variant": "PROFILE",
                "workloadEntry": workload_entry,
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "runtimeProfile": "interpreter-perf-release",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": subject_run_path(subject_id, run_id, "analysis", "host-input", "host-input.manifest.json"),
                }
            },
            "paths": {
                "bucketRoot": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime"),
                "manifestPath": subject_run_path(subject_id, run_id, "matrices", matrix_id, "runtime", "runtime.manifest.json"),
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("interpreter-perf-iterations-override")
        try:
            host_input_manifest_path = repo_root / request["upstream"]["host-input"]["manifestPath"]
            host_input_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            host_input_manifest_path.write_text(
                json.dumps({"primaryAssemblyPath": assembly_path}),
                encoding="utf-8",
            )
            subject_manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
            subject_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            subject_manifest_path.write_text(
                json.dumps(
                    {
                        "subjectId": subject_id,
                        "validation": {
                            "perf": {
                                "kind": "perf",
                                "project": perf_project_path,
                                "driver": "interpreter-runtime-perf",
                                "defaultVariant": "PROFILE",
                                "harnessIterations": 9,
                            }
                        },
                    }
                ),
                encoding="utf-8",
            )

            perf_result = {
                "metrics": {"meanDurationMs": 18.25, "opsPerSecond": 5600, "checksum": 84},
                "baselinePath": "subjects/FixtureBenchSubject/baselines/perf/windows-interpreter-perf/windows.json",
                "baseline": {"meanDurationMs": 18.0},
                "baselineUpdated": False,
                "regressionStatus": "no-baseline",
                "regressions": [],
            }
            completed = subprocess.CompletedProcess(
                args=["dotnet"],
                returncode=0,
                stdout=json.dumps({"elapsedMilliseconds": 18.25, "opsPerSecond": 5600, "checksum": 84}) + "\n",
                stderr="",
            )
            harness_dll_path = (
                repo_root
                / "artifacts"
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "matrices"
                / matrix_id
                / "runtime"
                / "harness"
                / f"{subject_id}.Subject.PerfHarness.dll"
            )

            with patch.object(workers_module, "_run_checked", return_value=""):
                with patch.object(workers_module, "_perf_sample_count", return_value=1):
                    with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                        with patch.object(workers_module.time, "perf_counter", side_effect=[20.0, 20.75]):
                            with patch.object(workers_module.perf_module, "evaluate_perf_subject", return_value=perf_result):
                                workers_module.run_interpreter_runtime_perf(repo_root=repo_root, request=request)

            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(harness_dll_path),
                    "9",
                    "--assembly",
                    str(repo_root / assembly_path),
                    "--workload-entry",
                    workload_entry,
                    "--mode",
                    "interpreter",
                ],
                cwd=repo_root,
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()

