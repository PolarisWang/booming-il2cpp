from __future__ import annotations

import importlib.util
import json
import shutil
import subprocess
import sys
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
            for relative_path in [
                Path("subjects") / subject_id / "validation" / "proof" / "native-reference" / "main.cpp",
                Path("subjects") / subject_id / "validation" / "proof" / "native-reference" / "CMakeLists.txt",
                Path("subjects") / subject_id / "validation" / "proof" / "native-reference" / "RunNativeReferenceProof.cmake",
                Path("artifacts")
                / "subjects"
                / subject_id
                / "runs"
                / run_id
                / "analysis"
                / "generated"
                / "generated"
                / "native-reference.generated.cpp",
            ]:
                absolute_path = repo_root / relative_path
                absolute_path.parent.mkdir(parents=True, exist_ok=True)
                absolute_path.write_text("// fixture\n", encoding="utf-8")

            with patch.object(workers_module.tooling_module, "cmake_environment", return_value=(str(expected_cmake_path), {})):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value=expected_env):
                    with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                        with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                            with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir", return_value=expected_cmake_dir):
                                with patch.object(workers_module, "_run_checked") as run_checked_mock:
                                    result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            self.assertEqual(
                [
                    str(expected_cmake_path),
                    "-S",
                    str(repo_root),
                    "-B",
                    str(expected_cmake_dir),
                    "-G",
                    "Visual Studio 18 2026",
                    "-DROADMAP0_PRESET_TARGET=windows-x64-reference",
                    "-DCHAOS_SUBJECT_VARIANT=CHECK",
                    f"-DCHAOS_SUBJECT_PROOF_ROOT={repo_root / 'subjects' / subject_id / 'validation' / 'proof' / 'native-reference'}",
                    f"-DCHAOS_SUBJECT_GENERATED_ROOT={repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'analysis' / 'generated'}",
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
            self.assertNotIn("nativeProofManifestPath", manifest)
            self.assertNotIn("nativeProofPlanPath", manifest)
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

    def test_windows_build_target_uses_generic_native_aot_host_for_generated_native_aot(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_windows_native_aot_build")
        subject_id = "FixtureNativeAotSubject"
        run_id = "fixture-run-native-aot-build-001"
        matrix_id = "windows-native-perf"
        expected_cl_path = self._make_non_repo_path("vs", "bin", "Hostx64", "x64", "cl.exe")
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

        repo_root = self._make_repo_root("windows-native-aot-build")
        try:
            for relative_path in [
                Path("src/native/runtime-core/runtime_core.cpp"),
                Path("src/native/engine-bridge/engine_bridge.cpp"),
                Path("src/native/bootstrap/bootstrap.cpp"),
                Path("src/native/support/support.cpp"),
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

            with patch.object(workers_module.tooling_module, "find_visual_cpp_executable", return_value=str(expected_cl_path)):
                with patch.object(workers_module.tooling_module, "windows_developer_environment", return_value=expected_env):
                    with patch.object(workers_module, "_run_checked") as run_checked_mock:
                        result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            build_args = run_checked_mock.call_args.args[0]
            self.assertEqual(
                [
                    str(expected_cl_path),
                    "/nologo",
                    "/std:c++17",
                    "/EHsc",
                    "/DWIN32",
                    "/D_WINDOWS",
                    "/DCHAOS_RUNTIME_ABI_STATIC",
                    "/DCHAOS_VARIANT_PROFILE",
                    "/DCHAOS_VARIANT_NAME=PROFILE",
                    "/O2",
                    "/DNDEBUG",
                    f"/I{repo_root / 'src' / 'native' / 'benchmark-host'}",
                    f"/Fo{repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'obj'}\\",
                    f"/Fd{repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'obj' / 'chaos_subject_native_aot.pdb'}",
                    f"/Fe{repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'out' / f'{WINDOWS_NATIVE_AOT_BUILD_TARGET}.exe'}",
                    str(repo_root / "src" / "native" / "benchmark-host" / "native_aot_main.cpp"),
                    str(
                        repo_root
                        / "artifacts"
                        / "subjects"
                        / subject_id
                        / "runs"
                        / run_id
                        / "analysis"
                        / "generated"
                        / "generated"
                        / "native-aot.generated.cpp"
                    ),
                ],
                build_args,
            )
            self.assertEqual(expected_env, run_checked_mock.call_args.kwargs["env"])

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("PROFILE", manifest["variant"])
            self.assertEqual("direct-msvc-native-aot", manifest["buildStrategy"])
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
                    "Chaos.TestFramework.dll",
                    "Newtonsoft.Json.dll",
                ]:
                    (expected_output_root / file_name).write_text("", encoding="utf-8")
                return ""

            with patch.object(workers_module.tooling_module, "allocate_dotnet_intermediate_dir", return_value=intermediate_root):
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
                    subject_run_path(subject_id, run_id, "analysis", "host-input", "Chaos.TestFramework.dll"),
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
            self.assertEqual("interpreter", manifest["mode"])
            self.assertEqual(workload_entry, manifest["workloadEntry"])
            self.assertEqual(assembly_path, manifest["workloadAssemblyPath"])
            self.assertEqual(perf_project_path, manifest["harnessProjectPath"])
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
