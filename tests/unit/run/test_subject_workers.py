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

    def test_windows_build_target_uses_direct_msvc_compile_and_records_build_strategy(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "chaos_subject_workers_windows_build")
        subject_id = "FixtureNativeSubject"
        run_id = "fixture-run-native-build-001"
        matrix_id = "windows-reference-check"
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
                Path("src/native/runtime-core/runtime_core.cpp"),
                Path("src/native/engine-bridge/engine_bridge.cpp"),
                Path("src/native/bootstrap/bootstrap.cpp"),
                Path("src/native/support/support.cpp"),
                Path("subjects") / subject_id / "validation" / "proof" / "native-reference" / "main.cpp",
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
                    "/DCHAOS_VARIANT_CHECK",
                    "/DCHAOS_VARIANT_NAME=CHECK",
                    "/Od",
                    "/Zi",
                    f"/I{repo_root / 'contracts' / 'native' / 'v0'}",
                    f"/I{repo_root / 'contracts' / 'engine' / 'v0'}",
                    f"/I{repo_root / 'src' / 'native' / 'runtime-core'}",
                    f"/I{repo_root / 'src' / 'native' / 'engine-bridge'}",
                    f"/I{repo_root / 'src' / 'native' / 'bootstrap'}",
                    f"/I{repo_root / 'src' / 'native' / 'support'}",
                    f"/Fo{repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'obj'}\\",
                    f"/Fd{repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'obj' / 'chaos_subject_reference_proof.pdb'}",
                    f"/Fe{repo_root / 'artifacts' / 'subjects' / subject_id / 'runs' / run_id / 'matrices' / matrix_id / 'build' / 'out' / f'{WINDOWS_REFERENCE_BUILD_TARGET}.exe'}",
                    str(repo_root / "src" / "native" / "runtime-core" / "runtime_core.cpp"),
                    str(repo_root / "src" / "native" / "engine-bridge" / "engine_bridge.cpp"),
                    str(repo_root / "src" / "native" / "bootstrap" / "bootstrap.cpp"),
                    str(repo_root / "src" / "native" / "support" / "support.cpp"),
                    str(repo_root / "subjects" / subject_id / "validation" / "proof" / "native-reference" / "main.cpp"),
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
                        / "native-reference.generated.cpp"
                    ),
                ],
                build_args,
            )
            self.assertEqual(expected_env, run_checked_mock.call_args.kwargs["env"])

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("CHECK", manifest["variant"])
            self.assertEqual(
                {
                    "codegen": ["CHAOS_VARIANT_CHECK", "CHAOS_VARIANT_NAME=CHECK"],
                    "native": ["CHAOS_VARIANT_CHECK", "CHAOS_VARIANT_NAME=CHECK"],
                },
                manifest["variantMacros"],
            )
            self.assertEqual("direct-msvc", manifest["buildStrategy"])
            self.assertEqual(str(expected_cl_path), manifest["compilerPath"])
            self.assertEqual(
                subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "out"),
                manifest["binaryRoot"],
            )
            self.assertEqual(
                [subject_run_path(subject_id, run_id, "matrices", matrix_id, "build", "out", f"{WINDOWS_REFERENCE_BUILD_TARGET}.exe")],
                manifest["outputs"],
            )
            self.assertNotIn("cmakeBinaryDir", manifest)
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
                    "-o",
                    str(repo_root / "artifacts" / "subjects" / subject_id / "runs" / run_id / "analysis" / "host-input"),
                    f"-p:BaseIntermediateOutputPath={intermediate_root.as_posix()}/$(MSBuildProjectName)/",
                    f"-p:MSBuildProjectExtensionsPath={intermediate_root.as_posix()}/$(MSBuildProjectName)/",
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


if __name__ == "__main__":
    unittest.main()
