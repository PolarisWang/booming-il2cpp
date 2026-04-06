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


class SubjectWorkersTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def test_windows_build_target_overrides_generator_and_records_absolute_cmake_binary_dir(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "booming_subject_workers_windows_build")
        expected_cmake_dir = Path(r"C:\Users\mayna\AppData\Local\Temp\booming-il2cpp\cmake-builds\subject-proof-1234")
        instance_spec = r"C:\Program Files\Microsoft Visual Studio\18\Professional,version=18.4.11626.88"

        request = {
            "selection": {
                "subjectId": "HelloWorldObject",
                "matrixId": "windows-dev-output",
                "executionContext": {
                    "targetPlatform": "windows-x64",
                    "toolchainProfile": "msvc-reference",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": "artifacts/subjects/HelloWorldObject/shared/generated/generated.manifest.json",
                }
            },
            "paths": {
                "bucketRoot": "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/build",
                "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/build/build.manifest.json",
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-build")
        try:
            with patch.object(workers_module.tooling_module, "detect_visual_studio_generator", return_value="Visual Studio 18 2026"):
                with patch.object(workers_module.tooling_module, "detect_visual_studio_instance_spec", return_value=instance_spec):
                    with patch.object(workers_module.tooling_module, "allocate_cmake_binary_dir", return_value=expected_cmake_dir):
                        with patch.object(workers_module, "_run_checked") as run_checked_mock:
                            result = workers_module.run_build_target(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            configure_args = run_checked_mock.call_args_list[0].args[0]
            self.assertEqual(
                [
                    "cmake",
                    "--preset",
                    "windows-x64-reference",
                    "-G",
                    "Visual Studio 18 2026",
                    "-B",
                    str(expected_cmake_dir),
                    f"-DCMAKE_GENERATOR_INSTANCE={instance_spec}",
                    f"-DCHAOS_HELLOWORLD_GENERATED_ROOT={repo_root / 'artifacts' / 'subjects' / 'HelloWorldObject' / 'shared' / 'generated'}",
                    f"-DCHAOS_HELLOWORLD_BUILD_OUT_ROOT={repo_root / 'artifacts' / 'subjects' / 'HelloWorldObject' / 'matrices' / 'windows-dev-output' / 'build' / 'out'}",
                    f"-DCHAOS_HELLOWORLD_RUNTIME_ROOT={repo_root / 'artifacts' / 'subjects' / 'HelloWorldObject' / 'matrices' / 'windows-dev-output' / 'runtime'}",
                ],
                configure_args,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual(expected_cmake_dir.as_posix(), manifest["cmakeBinaryDir"])
            self.assertEqual(
                "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/build/out",
                manifest["binaryRoot"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_windows_validate_only_build_uses_visual_studio_generator_and_records_absolute_cmake_binary_dir(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "booming_subject_workers_windows_validate_only")
        expected_cmake_dir = Path(r"C:\Users\mayna\AppData\Local\Temp\booming-il2cpp\cmake-builds\platform-gate-1234")
        instance_spec = r"C:\Program Files\Microsoft Visual Studio\18\Professional,version=18.4.11626.88"

        request = {
            "selection": {
                "subjectId": "HelloWorldObject",
                "matrixId": "windows-android-buildable",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "android-arm64",
                    "toolchainProfile": "android-ndk-r26",
                },
            },
            "upstream": {
                "generated": {
                    "manifestPath": "artifacts/subjects/HelloWorldObject/shared/generated/generated.manifest.json",
                }
            },
            "paths": {
                "bucketRoot": "artifacts/subjects/HelloWorldObject/matrices/windows-android-buildable/build",
                "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-android-buildable/build/build.manifest.json",
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("windows-validate-only")
        try:
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
            self.assertEqual(expected_cmake_dir.as_posix(), manifest["cmakeBinaryDir"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_runtime_observe_accepts_absolute_cmake_binary_dir_from_build_manifest(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "booming_subject_workers_runtime_observe")

        request = {
            "selection": {
                "subjectId": "HelloWorldObject",
                "matrixId": "windows-dev-output",
            },
            "upstream": {
                "build": {
                    "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/build/build.manifest.json",
                }
            },
            "paths": {
                "bucketRoot": "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/runtime",
                "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/runtime/runtime.manifest.json",
                "reportPaths": [],
            },
        }

        repo_root = self._make_repo_root("runtime-observe")
        try:
            build_manifest_path = repo_root / request["upstream"]["build"]["manifestPath"]
            build_manifest_path.parent.mkdir(parents=True, exist_ok=True)
            build_manifest_path.write_text(
                json.dumps(
                    {
                        "cmakeBinaryDir": "C:/Users/mayna/AppData/Local/Temp/booming-il2cpp/cmake-builds/subject-proof-1234",
                        "outputs": [],
                    }
                ),
                encoding="utf-8",
            )

            with patch.object(workers_module, "_run_checked") as run_checked_mock:
                result = workers_module.run_runtime_observe(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            build_args = run_checked_mock.call_args.args[0]
            self.assertEqual(["cmake", "--build"], build_args[:2])
            self.assertEqual(
                Path(r"C:\Users\mayna\AppData\Local\Temp\booming-il2cpp\cmake-builds\subject-proof-1234"),
                Path(build_args[2]),
            )
            self.assertEqual(
                ["--config", "Release", "--target", "chaos_stage4_hello_world_object_proof_run"],
                build_args[3:],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_dotnet_host_input_builder_uses_temp_intermediate_root_on_windows(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "booming_subject_workers_dotnet_host_input")
        intermediate_root = Path(r"C:\Users\mayna\AppData\Local\Temp\booming-dotnet-HelloWorldObject-1234")
        request = {
            "selection": {
                "subjectId": "HelloWorldObject",
                "source": {
                    "type": "dotnet-project",
                    "path": "subjects/HelloWorldObject/source/HelloWorldObject.csproj",
                    "entry": "HelloWorldObject/Program::Main(System.String[])",
                },
                "executionContext": {
                    "hostPlatform": "windows-x64",
                },
            },
            "upstream": {
                "source": {
                    "manifestPath": "artifacts/subjects/HelloWorldObject/shared/source/source.manifest.json",
                }
            },
            "paths": {
                "bucketRoot": "artifacts/subjects/HelloWorldObject/shared/host-input",
                "manifestPath": "artifacts/subjects/HelloWorldObject/shared/host-input/host-input.manifest.json",
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
                    str(repo_root / "subjects" / "HelloWorldObject" / "source" / "HelloWorldObject.csproj"),
                    "-c",
                    "Release",
                    "-o",
                    str(repo_root / "artifacts" / "subjects" / "HelloWorldObject" / "shared" / "host-input"),
                    f"-p:BaseIntermediateOutputPath={intermediate_root.as_posix()}/$(MSBuildProjectName)/",
                    f"-p:MSBuildProjectExtensionsPath={intermediate_root.as_posix()}/$(MSBuildProjectName)/",
                ],
                run_checked_mock.call_args.args[0],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_driver_build_uses_chaos_temp_intermediate_root_on_windows(self) -> None:
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "booming_subject_workers_driver_build")
        intermediate_root = Path(r"C:\Users\mayna\AppData\Local\Temp\bdn-1234abcd")

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
        workers_module = load_module(SUBJECT_WORKERS_MODULE_PATH, "booming_subject_workers_managed_runtime_output")
        request = {
            "selection": {
                "subjectId": "HelloWorld",
                "matrixId": "windows-managed-output",
                "executionContext": {
                    "hostPlatform": "windows-x64",
                },
            },
            "upstream": {
                "host-input": {
                    "manifestPath": "artifacts/subjects/HelloWorld/shared/host-input/host-input.manifest.json",
                }
            },
            "paths": {
                "bucketRoot": "artifacts/subjects/HelloWorld/matrices/windows-managed-output/runtime",
                "manifestPath": "artifacts/subjects/HelloWorld/matrices/windows-managed-output/runtime/runtime.manifest.json",
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
                        "primaryAssemblyPath": "artifacts/smoke/bin/HelloWorld/Release/net8.0/HelloWorld.dll",
                    }
                ),
                encoding="utf-8",
            )

            completed = subprocess.CompletedProcess(
                ["dotnet", str(repo_root / "artifacts" / "smoke" / "bin" / "HelloWorld" / "Release" / "net8.0" / "HelloWorld.dll")],
                0,
                "HelloWorld smoke entry reached.\nargs=0\nregister:Main\n",
                "",
            )

            with patch.object(workers_module, "run_process", return_value=completed) as run_process_mock:
                result = workers_module.run_managed_runtime_output(repo_root=repo_root, request=request)

            self.assertEqual("ok", result["status"])
            run_process_mock.assert_called_once_with(
                [
                    "dotnet",
                    str(repo_root / "artifacts" / "smoke" / "bin" / "HelloWorld" / "Release" / "net8.0" / "HelloWorld.dll"),
                ],
                cwd=repo_root,
            )

            manifest = json.loads((repo_root / request["paths"]["manifestPath"]).read_text(encoding="utf-8"))
            self.assertEqual("HelloWorld", manifest["subjectId"])
            self.assertEqual("windows-managed-output", manifest["matrixId"])
            self.assertEqual(
                "artifacts/subjects/HelloWorld/matrices/windows-managed-output/runtime/stdout.log",
                manifest["stdoutPath"],
            )
            self.assertEqual(
                "artifacts/subjects/HelloWorld/matrices/windows-managed-output/runtime/stderr.log",
                manifest["stderrPath"],
            )
            self.assertEqual(
                "artifacts/subjects/HelloWorld/matrices/windows-managed-output/runtime/exit-code.txt",
                manifest["exitCodePath"],
            )
            self.assertEqual(
                ["HelloWorld smoke entry reached.", "args=0", "register:Main"],
                manifest["outputLines"],
            )
            self.assertEqual(
                "artifacts/subjects/HelloWorld/matrices/windows-managed-output/runtime/stdout.log",
                result["diagnostics"]["stdoutPath"],
            )
            self.assertEqual(
                ["artifacts/subjects/HelloWorld/matrices/windows-managed-output/runtime/stdout.log"],
                result["primaryEvidencePaths"],
            )
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
