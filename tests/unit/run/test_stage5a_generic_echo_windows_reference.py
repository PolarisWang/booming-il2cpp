from __future__ import annotations

import json
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
PROOF_PROJECT_PATH = REPO_ROOT / "tests" / "proof" / "input" / "GenericEchoClosedMinimal" / "GenericEchoClosedMinimal.csproj"
PROOF_DLL_PATH = REPO_ROOT / "tests" / "proof" / "input" / "GenericEchoClosedMinimal" / "bin" / "Release" / "net8.0" / "GenericEchoClosedMinimal.dll"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / "proof" / "managed-closure" / "GenericEchoClosedMinimal"
NATIVE_OUTPUT_ROOT = REPO_ROOT / "artifacts" / "proof" / "native-reference" / "GenericEchoClosedMinimal"
PRESET_BUILD_ROOT = REPO_ROOT / "artifacts" / "presets"
ROOT_CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"
PROOF_HOST_DIR = REPO_ROOT / "tests" / "proof" / "native-reference" / "GenericEchoClosedMinimal"
PROOF_HOST_CMAKE_PATH = PROOF_HOST_DIR / "CMakeLists.txt"
PROOF_HOST_MAIN_PATH = PROOF_HOST_DIR / "main.cpp"
PROOF_HOST_RUN_SCRIPT_PATH = PROOF_HOST_DIR / "RunNativeReferenceProof.cmake"

EXPECTED_ENTRY_SUBJECT = "GenericEchoClosedMinimal/Program::Main()"
EXPECTED_GENERIC_TYPE_SUBJECT = "GenericEchoClosedMinimal/EchoBox<System.String>"
EXPECTED_GENERIC_FIELD_SUBJECT = "GenericEchoClosedMinimal/EchoBox<System.String>::_value"
EXPECTED_GENERIC_CTOR_SUBJECT = "GenericEchoClosedMinimal/EchoBox<System.String>::.ctor(System.String)"
EXPECTED_GENERIC_GETTER_SUBJECT = "GenericEchoClosedMinimal/EchoBox<System.String>::get_Value()"
EXPECTED_GENERIC_ECHO_SUBJECT = "GenericEchoClosedMinimal/GenericEcho::Echo<System.String>(System.String)"


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


class Stage5AGenericEchoWindowsReferenceTests(unittest.TestCase):
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
        exe_path = run_root / "chaos_stage5a_generic_echo_closed_minimal_proof.exe"

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

    def test_driver_generates_closed_generic_managed_closure_bundle(self) -> None:
        self._build_managed_closure_bundle()

        typed_il = load_json(OUTPUT_ROOT / "typed-il-ir.json")
        manifest = load_json(OUTPUT_ROOT / "closure.manifest.json")
        metadata_registration = load_json(OUTPUT_ROOT / "metadata-registration.json")
        code_registration = load_json(OUTPUT_ROOT / "code-registration.json")

        self.assertEqual("GenericEchoClosedMinimal", manifest["assemblyName"])
        self.assertEqual(EXPECTED_ENTRY_SUBJECT, manifest["entrySubjectId"])

        method_subjects = {method["subjectId"] for method in typed_il["methods"]}
        self.assertIn(EXPECTED_ENTRY_SUBJECT, method_subjects)
        self.assertIn(EXPECTED_GENERIC_ECHO_SUBJECT, method_subjects)
        self.assertIn(EXPECTED_GENERIC_CTOR_SUBJECT, method_subjects)
        self.assertIn(EXPECTED_GENERIC_GETTER_SUBJECT, method_subjects)

        entry_method = next(method for method in typed_il["methods"] if method["subjectId"] == EXPECTED_ENTRY_SUBJECT)
        entry_callees = [
            instruction["callee"]
            for instruction in entry_method["blocks"][0]["instructions"]
            if "callee" in instruction
        ]
        self.assertEqual(
            [
                EXPECTED_GENERIC_ECHO_SUBJECT,
                EXPECTED_GENERIC_CTOR_SUBJECT,
                EXPECTED_GENERIC_GETTER_SUBJECT,
                "System.Console/System.Console::WriteLine(System.String)",
            ],
            entry_callees,
        )

        constructor_method = next(method for method in typed_il["methods"] if method["subjectId"] == EXPECTED_GENERIC_CTOR_SUBJECT)
        constructor_operands = [
            instruction.get("operand")
            for instruction in constructor_method["blocks"][0]["instructions"]
            if instruction["op"] == "stfld"
        ]
        self.assertEqual([EXPECTED_GENERIC_FIELD_SUBJECT], constructor_operands)

        getter_method = next(method for method in typed_il["methods"] if method["subjectId"] == EXPECTED_GENERIC_GETTER_SUBJECT)
        getter_operands = [
            instruction.get("operand")
            for instruction in getter_method["blocks"][0]["instructions"]
            if instruction["op"] == "ldfld"
        ]
        self.assertEqual([EXPECTED_GENERIC_FIELD_SUBJECT], getter_operands)

        registration_subjects = {
            registration["subjectId"]
            for module in code_registration["modules"]
            for registration in module["registrations"]
        }
        self.assertIn(EXPECTED_ENTRY_SUBJECT, registration_subjects)
        self.assertIn(EXPECTED_GENERIC_ECHO_SUBJECT, registration_subjects)
        self.assertIn(EXPECTED_GENERIC_CTOR_SUBJECT, registration_subjects)
        self.assertIn(EXPECTED_GENERIC_GETTER_SUBJECT, registration_subjects)

        metadata_subjects = {registration["subjectId"] for registration in metadata_registration["registrations"]}
        self.assertIn(EXPECTED_GENERIC_TYPE_SUBJECT, metadata_subjects)
        self.assertIn(EXPECTED_GENERIC_FIELD_SUBJECT, metadata_subjects)
        self.assertIn(EXPECTED_GENERIC_CTOR_SUBJECT, metadata_subjects)
        self.assertIn(EXPECTED_GENERIC_GETTER_SUBJECT, metadata_subjects)
        self.assertIn(EXPECTED_GENERIC_ECHO_SUBJECT, metadata_subjects)

    def test_driver_emits_closed_generic_native_reference_skeleton(self) -> None:
        self._build_native_reference_bundle()

        generated_cpp = NATIVE_OUTPUT_ROOT / "generated" / "native-reference.generated.cpp"
        manifest_path = NATIVE_OUTPUT_ROOT / "native-proof.manifest.json"
        plan_path = NATIVE_OUTPUT_ROOT / "native-proof.plan.json"

        self.assertTrue(generated_cpp.is_file(), msg=f"missing generated cpp: {generated_cpp}")
        self.assertTrue(manifest_path.is_file(), msg=f"missing native proof manifest: {manifest_path}")
        self.assertTrue(plan_path.is_file(), msg=f"missing native proof plan: {plan_path}")

        manifest = load_json(manifest_path)
        plan = load_json(plan_path)
        self.assertEqual("GenericEchoClosedMinimal", manifest["assemblyName"])
        self.assertEqual(EXPECTED_ENTRY_SUBJECT, manifest["entrySubjectId"])
        self.assertEqual("native-proof.plan.json", manifest["planArtifactPath"])
        self.assertEqual("nativeReferenceLoweringPlan", plan["artifactKind"])
        self.assertEqual("staticCallCtorGetter", plan["planKind"])
        self.assertEqual("GenericEchoClosedMinimal", plan["assemblyName"])
        self.assertEqual(EXPECTED_ENTRY_SUBJECT, plan["entrySubjectId"])
        self.assertEqual("GenericEchoClosedMinimal_Program_Main", plan["entrySymbol"])
        self.assertEqual("GenericEchoClosedMinimal_GenericEcho_Echo_System_String", plan["echoMethodSymbol"])
        self.assertEqual("GenericEchoClosedMinimal_EchoBox_System_String__ctor", plan["constructorSymbol"])
        self.assertEqual("GenericEchoClosedMinimal_EchoBox_System_String_get_Value", plan["getterSymbol"])
        self.assertEqual('"generic-ok"', plan["echoLiteral"])

        generated_text = generated_cpp.read_text(encoding="utf-8")
        self.assertIn("GenericEchoClosedMinimal_Program_Main", generated_text)
        self.assertIn("GenericEchoClosedMinimal_GenericEcho_Echo_System_String", generated_text)
        self.assertIn("GenericEchoClosedMinimal_EchoBox_System_String__ctor", generated_text)
        self.assertIn("GenericEchoClosedMinimal_EchoBox_System_String_get_Value", generated_text)
        self.assertIn("resolve_type_by_token", generated_text)
        self.assertIn("resolve_method_by_token", generated_text)
        self.assertIn("resolve_field_by_token", generated_text)
        self.assertIn("field_set_value", generated_text)
        self.assertIn("field_get_value", generated_text)
        self.assertIn('"generic-ok"', generated_text)
        self.assertIn("g_proof_context.write_line_string(runtime, thread, message)", generated_text)
        self.assertNotIn("ConcatStringPair", generated_text)
        self.assertNotIn("string_concat_pair_icall", generated_text)

    def test_stage5a_proof_host_wiring_exists(self) -> None:
        expected_paths = [
            PROOF_HOST_DIR,
            PROOF_HOST_CMAKE_PATH,
            PROOF_HOST_MAIN_PATH,
            PROOF_HOST_RUN_SCRIPT_PATH,
        ]

        for path in expected_paths:
            self.assertTrue(path.exists(), msg=f"missing Stage 5A proof host path: {path}")

        root_cmake_text = ROOT_CMAKE_PATH.read_text(encoding="utf-8")
        proof_host_cmake_text = PROOF_HOST_CMAKE_PATH.read_text(encoding="utf-8")
        proof_host_main_text = PROOF_HOST_MAIN_PATH.read_text(encoding="utf-8")
        proof_host_run_script_text = PROOF_HOST_RUN_SCRIPT_PATH.read_text(encoding="utf-8")

        self.assertIn("add_subdirectory(tests/proof/native-reference/GenericEchoClosedMinimal)", root_cmake_text)

        required_cmake_markers = [
            "artifacts/proof/managed-closure/GenericEchoClosedMinimal",
            "artifacts/proof/native-reference/GenericEchoClosedMinimal",
            "native-reference.generated.cpp",
            "native-proof.plan.json",
            "native-proof.manifest.json",
            "emit-native-reference",
            "GenericEchoClosedMinimal.csproj",
            "chaos_stage5a_generic_echo_closed_minimal_proof_run",
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
            "stage5a-generic-echo-reference",
            "GenericEchoClosedMinimal",
            "System.Console/System.Console::WriteLine(System.String)",
        ]

        for marker in required_main_markers:
            self.assertIn(marker, proof_host_main_text)

        for marker in ["execute_process(", "RESULT_VARIABLE proof_exit_code", "exit-code.txt"]:
            self.assertIn(marker, proof_host_run_script_text)

    def test_windows_reference_proof_run_outputs_generic_ok(self) -> None:
        cmake_executable = find_cmake_executable()
        if cmake_executable is not None:
            if NATIVE_OUTPUT_ROOT.exists():
                shutil.rmtree(NATIVE_OUTPUT_ROOT)

            PRESET_BUILD_ROOT.mkdir(parents=True, exist_ok=True)

            with tempfile.TemporaryDirectory(prefix="windows-x64-reference-stage5a-", dir=PRESET_BUILD_ROOT) as binary_dir:
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
                        "chaos_stage5a_generic_echo_closed_minimal_proof_run",
                    ],
                    cwd=REPO_ROOT,
                )
        else:
            self._run_proof_host_with_msvc()

        run_root = NATIVE_OUTPUT_ROOT / "run"
        stdout_path = run_root / "stdout.log"
        stderr_path = run_root / "stderr.log"
        exit_code_path = run_root / "exit-code.txt"

        self.assertTrue(stdout_path.is_file(), msg=f"missing stdout log: {stdout_path}")
        self.assertTrue(stderr_path.is_file(), msg=f"missing stderr log: {stderr_path}")
        self.assertTrue(exit_code_path.is_file(), msg=f"missing exit code log: {exit_code_path}")

        self.assertEqual("0", exit_code_path.read_text(encoding="utf-8").strip())
        self.assertEqual("", stderr_path.read_text(encoding="utf-8"))
        self.assertIn("generic-ok", stdout_path.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
