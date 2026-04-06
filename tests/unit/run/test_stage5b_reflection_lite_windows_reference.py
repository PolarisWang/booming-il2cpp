from __future__ import annotations

import json
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
PROOF_PROJECT_PATH = REPO_ROOT / "tests" / "proof" / "input" / "ReflectionLiteQueryMinimal" / "ReflectionLiteQueryMinimal.csproj"
PROOF_DLL_PATH = REPO_ROOT / "tests" / "proof" / "input" / "ReflectionLiteQueryMinimal" / "bin" / "Release" / "net8.0" / "ReflectionLiteQueryMinimal.dll"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / "proof" / "managed-closure" / "ReflectionLiteQueryMinimal"
NATIVE_OUTPUT_ROOT = REPO_ROOT / "artifacts" / "proof" / "native-reference" / "ReflectionLiteQueryMinimal"
PRESET_BUILD_ROOT = REPO_ROOT / "artifacts" / "presets"
ROOT_CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"
PROOF_HOST_DIR = REPO_ROOT / "tests" / "proof" / "native-reference" / "ReflectionLiteQueryMinimal"
PROOF_HOST_CMAKE_PATH = PROOF_HOST_DIR / "CMakeLists.txt"
PROOF_HOST_MAIN_PATH = PROOF_HOST_DIR / "main.cpp"
PROOF_HOST_RUN_SCRIPT_PATH = PROOF_HOST_DIR / "RunNativeReferenceProof.cmake"

STAGE5B_SRC_SCAN_ROOTS = [
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen",
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts",
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver",
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader",
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.MetadataWriter",
    REPO_ROOT / "src" / "native" / "bootstrap",
    REPO_ROOT / "src" / "native" / "runtime-core",
    REPO_ROOT / "src" / "native" / "support",
]
STAGE5B_SRC_TEXT_SUFFIXES = {".cs", ".csproj", ".cpp", ".h", ".scriban"}
FORBIDDEN_STAGE5B_SRC_SNIPPETS = [
    "ReflectionLiteQueryMinimal",
    "tests/proof/input/ReflectionLiteQueryMinimal",
    "artifacts/proof/managed-closure/ReflectionLiteQueryMinimal",
    "artifacts/proof/native-reference/ReflectionLiteQueryMinimal",
]

EXPECTED_ENTRY_SUBJECT = "ReflectionLiteQueryMinimal/Program::Main()"
EXPECTED_OPEN_GENERIC_TYPE_SUBJECT = "ReflectionLiteQueryMinimal/ReflectionFixture`1"
EXPECTED_CLOSED_GENERIC_TYPE_SUBJECT = "ReflectionLiteQueryMinimal/ReflectionFixture<System.Int32>"
EXPECTED_CLOSED_FIELD_SUBJECT = "ReflectionLiteQueryMinimal/ReflectionFixture<System.Int32>::BackingField"
EXPECTED_CLOSED_PROPERTY_SUBJECT = "ReflectionLiteQueryMinimal/ReflectionFixture<System.Int32>::property:Value"
EXPECTED_CLOSED_METHOD_SUBJECT = "ReflectionLiteQueryMinimal/ReflectionFixture<System.Int32>::Echo(System.Int32,System.Int32)"
EXPECTED_FIRST_PARAMETER_SUBJECT = (
    "ReflectionLiteQueryMinimal/ReflectionFixture<System.Int32>::"
    "Echo(System.Int32,System.Int32)::parameter[0]:nextValue"
)
EXPECTED_PROOF_OUTPUT = (
    "reflection-ok|ReflectionFixture<System.Int32>|BackingField|Value|Echo|nextValue|ReflectionFixture`1"
)


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


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def find_cmake_executable() -> str | None:
    candidates = [
        shutil.which("cmake"),
        r"C:\Program Files\CMake\bin\cmake.exe",
        r"C:\Program Files (x86)\CMake\bin\cmake.exe",
        r"D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe",
    ]

    for candidate in candidates:
        if candidate and Path(candidate).is_file():
            return candidate

    return None


def find_vsdevcmd_path() -> str | None:
    candidates = [
        r"D:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat",
        r"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat",
        r"D:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat",
        r"C:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat",
    ]

    for candidate in candidates:
        if Path(candidate).is_file():
            return candidate

    return None


class Stage5BReflectionLiteWindowsReferenceTests(unittest.TestCase):
    maxDiff = None

    def _build_managed_closure_bundle(self) -> None:
        if OUTPUT_ROOT.exists():
            shutil.rmtree(OUTPUT_ROOT)

        run_checked(["dotnet", "build", str(PROOF_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(PROOF_DLL_PATH.is_file(), msg=f"missing proof dll: {PROOF_DLL_PATH}")

        run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(DRIVER_PROJECT_PATH),
                "-c",
                "Release",
                "--",
                str(PROOF_DLL_PATH),
                str(OUTPUT_ROOT),
            ],
            cwd=REPO_ROOT,
        )

    def _build_native_reference_bundle(self) -> None:
        self._build_managed_closure_bundle()

        if NATIVE_OUTPUT_ROOT.exists():
            shutil.rmtree(NATIVE_OUTPUT_ROOT)

        run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(DRIVER_PROJECT_PATH),
                "-c",
                "Release",
                "--",
                "emit-native-reference",
                str(OUTPUT_ROOT),
                str(NATIVE_OUTPUT_ROOT),
            ],
            cwd=REPO_ROOT,
        )

    def _run_proof_host_with_msvc(self) -> None:
        vsdevcmd_path = find_vsdevcmd_path()
        if vsdevcmd_path is None:
            self.skipTest("neither cmake nor VS developer tools are available in this environment")

        self._build_native_reference_bundle()

        run_root = NATIVE_OUTPUT_ROOT / "run"
        run_root.mkdir(parents=True, exist_ok=True)
        exe_path = run_root / "chaos_stage5b_reflection_lite_query_minimal_proof.exe"

        include_paths = [
            REPO_ROOT / "contracts" / "native" / "v0",
            REPO_ROOT / "src" / "native" / "runtime-core",
            REPO_ROOT / "src" / "native" / "bootstrap",
            REPO_ROOT / "src" / "native" / "support",
        ]
        source_paths = [
            REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.cpp",
            REPO_ROOT / "src" / "native" / "support" / "support.cpp",
            REPO_ROOT / "src" / "native" / "bootstrap" / "bootstrap.cpp",
            NATIVE_OUTPUT_ROOT / "generated" / "native-reference.generated.cpp",
            PROOF_HOST_MAIN_PATH,
        ]

        compile_script_path = run_root / "build-proof-host.cmd"
        compile_lines = [
            "@echo off",
            f'call "{vsdevcmd_path}" -arch=x64 -host_arch=x64 >nul',
            "if errorlevel 1 exit /b %errorlevel%",
            'cd /d "%~dp0"',
            "cl /nologo /std:c++17 /EHsc /DCHAOS_RUNTIME_ABI_STATIC "
            + " ".join([f'/I"{path}"' for path in include_paths])
            + " "
            + " ".join([f'"{path}"' for path in source_paths])
            + f' /Fe:"{exe_path}"',
        ]
        compile_script_path.write_text("\n".join(compile_lines) + "\n", encoding="utf-8")
        run_checked(["cmd", "/c", str(compile_script_path)], cwd=REPO_ROOT)

        completed = subprocess.run(
            [str(exe_path)],
            cwd=run_root,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            check=False,
        )

        (run_root / "stdout.log").write_text(completed.stdout, encoding="utf-8")
        (run_root / "stderr.log").write_text(completed.stderr, encoding="utf-8")
        (run_root / "exit-code.txt").write_text(f"{completed.returncode}\n", encoding="utf-8")

    def test_stage5b_src_stays_sample_agnostic(self) -> None:
        scanned_files = 0

        for root in STAGE5B_SRC_SCAN_ROOTS:
            self.assertTrue(root.exists(), msg=f"missing Stage 5B source scan root: {root}")

            for source_path in root.rglob("*"):
                if not source_path.is_file() or source_path.suffix.lower() not in STAGE5B_SRC_TEXT_SUFFIXES:
                    continue

                scanned_files += 1
                source_text = source_path.read_text(encoding="utf-8")
                for forbidden_snippet in FORBIDDEN_STAGE5B_SRC_SNIPPETS:
                    self.assertNotIn(
                        forbidden_snippet,
                        source_text,
                        msg=f"test-project-specific logic leaked into {source_path}: {forbidden_snippet}",
                    )

        self.assertGreater(scanned_files, 0, msg="expected to scan at least one Stage 5B source file")

    def test_driver_generates_reflection_query_managed_closure_bundle(self) -> None:
        self._build_managed_closure_bundle()

        manifest = load_json(OUTPUT_ROOT / "closure.manifest.json")
        metadata_registration = load_json(OUTPUT_ROOT / "metadata-registration.json")

        self.assertEqual("ReflectionLiteQueryMinimal", manifest["assemblyName"])
        self.assertEqual(EXPECTED_ENTRY_SUBJECT, manifest["entrySubjectId"])

        metadata_subjects = {registration["subjectId"] for registration in metadata_registration["registrations"]}
        self.assertIn(EXPECTED_OPEN_GENERIC_TYPE_SUBJECT, metadata_subjects)
        self.assertIn(EXPECTED_CLOSED_GENERIC_TYPE_SUBJECT, metadata_subjects)
        self.assertIn(EXPECTED_CLOSED_FIELD_SUBJECT, metadata_subjects)
        self.assertIn(EXPECTED_CLOSED_PROPERTY_SUBJECT, metadata_subjects)
        self.assertIn(EXPECTED_CLOSED_METHOD_SUBJECT, metadata_subjects)
        self.assertIn(EXPECTED_FIRST_PARAMETER_SUBJECT, metadata_subjects)

    def test_driver_emits_reflection_query_native_reference_skeleton(self) -> None:
        self._build_native_reference_bundle()

        generated_cpp = NATIVE_OUTPUT_ROOT / "generated" / "native-reference.generated.cpp"
        manifest_path = NATIVE_OUTPUT_ROOT / "native-proof.manifest.json"
        plan_path = NATIVE_OUTPUT_ROOT / "native-proof.plan.json"

        self.assertTrue(generated_cpp.is_file(), msg=f"missing generated cpp: {generated_cpp}")
        self.assertTrue(manifest_path.is_file(), msg=f"missing native proof manifest: {manifest_path}")
        self.assertTrue(plan_path.is_file(), msg=f"missing native proof plan: {plan_path}")

        manifest = load_json(manifest_path)
        plan = load_json(plan_path)
        self.assertEqual("ReflectionLiteQueryMinimal", manifest["assemblyName"])
        self.assertEqual(EXPECTED_ENTRY_SUBJECT, manifest["entrySubjectId"])
        self.assertEqual("native-proof.plan.json", manifest["planArtifactPath"])
        self.assertEqual("nativeReferenceLoweringPlan", plan["artifactKind"])
        self.assertEqual("reflectionQueryMinimal", plan["planKind"])
        self.assertEqual("ReflectionLiteQueryMinimal", plan["assemblyName"])
        self.assertEqual(EXPECTED_ENTRY_SUBJECT, plan["entrySubjectId"])
        self.assertEqual(EXPECTED_CLOSED_GENERIC_TYPE_SUBJECT, plan["closedTypeSubjectId"])
        self.assertEqual(EXPECTED_OPEN_GENERIC_TYPE_SUBJECT, plan["genericTypeDefinitionSubjectId"])
        self.assertEqual(EXPECTED_CLOSED_FIELD_SUBJECT, plan["fieldSubjectId"])
        self.assertEqual(EXPECTED_CLOSED_PROPERTY_SUBJECT, plan["propertySubjectId"])
        self.assertEqual(EXPECTED_CLOSED_METHOD_SUBJECT, plan["methodSubjectId"])
        self.assertEqual(EXPECTED_FIRST_PARAMETER_SUBJECT, plan["parameterSubjectId"])
        self.assertEqual("BackingField", plan["fieldQueryName"])
        self.assertEqual("Value", plan["propertyQueryName"])
        self.assertEqual("Echo", plan["methodQueryName"])
        self.assertEqual(2, plan["methodParameterCount"])
        self.assertEqual(0, plan["parameterIndex"])
        self.assertEqual(EXPECTED_PROOF_OUTPUT, plan["expectedOutput"])

        generated_text = generated_cpp.read_text(encoding="utf-8")
        self.assertIn("reflectionQueryMinimal", generated_text)
        self.assertIn("resolve_type_by_token", generated_text)
        self.assertIn("type_find_field", generated_text)
        self.assertIn("type_find_property", generated_text)
        self.assertIn("type_find_method", generated_text)
        self.assertIn("method_get_parameter", generated_text)
        self.assertIn("type_get_generic_type_definition", generated_text)
        self.assertIn(EXPECTED_PROOF_OUTPUT, generated_text)
        self.assertNotIn("field_set_value", generated_text)
        self.assertNotIn("field_get_value", generated_text)
        self.assertNotIn("method_invoke", generated_text)
        self.assertNotIn("type_find_event", generated_text)

    def test_stage5b_proof_host_wiring_exists(self) -> None:
        expected_paths = [
            PROOF_HOST_DIR,
            PROOF_HOST_CMAKE_PATH,
            PROOF_HOST_MAIN_PATH,
            PROOF_HOST_RUN_SCRIPT_PATH,
        ]

        for path in expected_paths:
            self.assertTrue(path.exists(), msg=f"missing Stage 5B proof host path: {path}")

        root_cmake_text = ROOT_CMAKE_PATH.read_text(encoding="utf-8")
        proof_host_cmake_text = PROOF_HOST_CMAKE_PATH.read_text(encoding="utf-8")
        proof_host_main_text = PROOF_HOST_MAIN_PATH.read_text(encoding="utf-8")
        proof_host_run_script_text = PROOF_HOST_RUN_SCRIPT_PATH.read_text(encoding="utf-8")

        self.assertIn("add_subdirectory(tests/proof/native-reference/ReflectionLiteQueryMinimal)", root_cmake_text)

        required_cmake_markers = [
            "artifacts/proof/managed-closure/ReflectionLiteQueryMinimal",
            "artifacts/proof/native-reference/ReflectionLiteQueryMinimal",
            "native-reference.generated.cpp",
            "native-proof.plan.json",
            "native-proof.manifest.json",
            "emit-native-reference",
            "ReflectionLiteQueryMinimal.csproj",
            "chaos_stage5b_reflection_lite_query_minimal_proof_run",
            "RunNativeReferenceProof.cmake",
            "stdout.log",
            "stderr.log",
            "exit-code.txt",
        ]

        for marker in required_cmake_markers:
            self.assertIn(marker, proof_host_cmake_text)

        required_main_markers = [
            "RunNativeReferenceProof",
            "chaos_runtime_get_abi_v0",
            "chaos_codegen_get_bridge_v0",
            "windows-x64-reference",
            "stage5b-reflection-lite-reference",
            "ReflectionLiteQueryMinimal",
            "System.Console/System.Console::WriteLine(System.String)",
        ]

        for marker in required_main_markers:
            self.assertIn(marker, proof_host_main_text)

        for marker in ["execute_process(", "RESULT_VARIABLE proof_exit_code", "exit-code.txt"]:
            self.assertIn(marker, proof_host_run_script_text)

    def test_windows_reference_proof_run_outputs_reflection_ok(self) -> None:
        if find_vsdevcmd_path() is not None:
            self._run_proof_host_with_msvc()
        else:
            cmake_executable = find_cmake_executable()
            if cmake_executable is not None:
                if NATIVE_OUTPUT_ROOT.exists():
                    shutil.rmtree(NATIVE_OUTPUT_ROOT)

                with tempfile.TemporaryDirectory(prefix="b5b-") as binary_dir:
                    run_checked(
                        [
                            cmake_executable,
                            "--preset",
                            "windows-x64-reference",
                            "-B",
                            binary_dir,
                        ],
                        cwd=REPO_ROOT,
                    )
                    run_checked(
                        [
                            cmake_executable,
                            "--build",
                            binary_dir,
                            "--config",
                            "Release",
                            "--target",
                            "chaos_stage5b_reflection_lite_query_minimal_proof_run",
                        ],
                        cwd=REPO_ROOT,
                    )
            else:
                self.skipTest("neither cmake nor VS developer tools are available in this environment")

        run_root = NATIVE_OUTPUT_ROOT / "run"
        stdout_path = run_root / "stdout.log"
        stderr_path = run_root / "stderr.log"
        exit_code_path = run_root / "exit-code.txt"

        self.assertTrue(stdout_path.is_file(), msg=f"missing stdout log: {stdout_path}")
        self.assertTrue(stderr_path.is_file(), msg=f"missing stderr log: {stderr_path}")
        self.assertTrue(exit_code_path.is_file(), msg=f"missing exit code log: {exit_code_path}")

        self.assertEqual("0", exit_code_path.read_text(encoding="utf-8").strip())
        self.assertEqual("", stderr_path.read_text(encoding="utf-8"))
        self.assertIn(EXPECTED_PROOF_OUTPUT, stdout_path.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
