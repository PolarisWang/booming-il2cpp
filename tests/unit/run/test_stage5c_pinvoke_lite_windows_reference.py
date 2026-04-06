from __future__ import annotations

import json
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
PROOF_PROJECT_PATH = REPO_ROOT / "tests" / "proof" / "input" / "PInvokeLiteDllImportMinimal" / "PInvokeLiteDllImportMinimal.csproj"
PROOF_DLL_PATH = REPO_ROOT / "tests" / "proof" / "input" / "PInvokeLiteDllImportMinimal" / "bin" / "Release" / "net8.0" / "PInvokeLiteDllImportMinimal.dll"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / "proof" / "managed-closure" / "PInvokeLiteDllImportMinimal"
NATIVE_OUTPUT_ROOT = REPO_ROOT / "artifacts" / "proof" / "native-reference" / "PInvokeLiteDllImportMinimal"
PROOF_HOST_DIR = REPO_ROOT / "tests" / "proof" / "native-reference" / "PInvokeLiteDllImportMinimal"
PROOF_HOST_CMAKE_PATH = PROOF_HOST_DIR / "CMakeLists.txt"
PROOF_HOST_MAIN_PATH = PROOF_HOST_DIR / "main.cpp"
PROOF_HOST_RUN_SCRIPT_PATH = PROOF_HOST_DIR / "RunNativeReferenceProof.cmake"

STAGE5C_SRC_SCAN_ROOTS = [
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen",
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts",
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver",
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader",
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.MetadataWriter",
    REPO_ROOT / "src" / "native" / "bootstrap",
    REPO_ROOT / "src" / "native" / "runtime-core",
    REPO_ROOT / "src" / "native" / "support",
]
STAGE5C_SRC_TEXT_SUFFIXES = {".cs", ".csproj", ".cpp", ".h", ".scriban"}
FORBIDDEN_STAGE5C_SRC_SNIPPETS = [
    "PInvokeLiteDllImportMinimal",
    "tests/proof/input/PInvokeLiteDllImportMinimal",
    "artifacts/proof/managed-closure/PInvokeLiteDllImportMinimal",
    "artifacts/proof/native-reference/PInvokeLiteDllImportMinimal",
]

EXPECTED_ENTRY_SUBJECT = "PInvokeLiteDllImportMinimal/Program::Main()"
EXPECTED_IMPORT_METHOD_SUBJECT = "PInvokeLiteDllImportMinimal/NativeMethods::MulDiv(System.Int32,System.Int32,System.Int32)"
EXPECTED_PROOF_OUTPUT = "pinvoke-ok|14"
EXPECTED_FALLBACK_OBJECT_FILES = [
    "runtime_core.obj",
    "support.obj",
    "bootstrap.obj",
    "native-reference.generated.obj",
    "main.obj",
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


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


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


class Stage5CPInvokeLiteWindowsReferenceTests(unittest.TestCase):
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
            self.skipTest("VS developer tools are not available in this environment")

        self._build_native_reference_bundle()

        run_root = NATIVE_OUTPUT_ROOT / "run"
        run_root.mkdir(parents=True, exist_ok=True)
        exe_path = run_root / "chaos_stage5c_pinvoke_lite_dllimport_minimal_proof.exe"

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
            + " ".join([f'/I\"{path}\"' for path in include_paths])
            + " "
            + " ".join([f'\"{path}\"' for path in source_paths])
            + f' /Fe:\"{exe_path}\"',
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

    def test_stage5c_src_stays_sample_agnostic(self) -> None:
        scanned_files = 0

        for root in STAGE5C_SRC_SCAN_ROOTS:
            self.assertTrue(root.exists(), msg=f"missing Stage 5C source scan root: {root}")

            for source_path in root.rglob("*"):
                if not source_path.is_file() or source_path.suffix.lower() not in STAGE5C_SRC_TEXT_SUFFIXES:
                    continue

                scanned_files += 1
                source_text = source_path.read_text(encoding="utf-8")
                for forbidden_snippet in FORBIDDEN_STAGE5C_SRC_SNIPPETS:
                    self.assertNotIn(
                        forbidden_snippet,
                        source_text,
                        msg=f"test-project-specific logic leaked into {source_path}: {forbidden_snippet}",
                    )

        self.assertGreater(scanned_files, 0, msg="expected to scan at least one Stage 5C source file")

    def test_driver_generates_pinvoke_managed_closure_bundle(self) -> None:
        self._build_managed_closure_bundle()

        manifest = load_json(OUTPUT_ROOT / "closure.manifest.json")
        metadata_registration = load_json(OUTPUT_ROOT / "metadata-registration.json")
        code_registration = load_json(OUTPUT_ROOT / "code-registration.json")

        self.assertEqual("PInvokeLiteDllImportMinimal", manifest["assemblyName"])
        self.assertEqual(EXPECTED_ENTRY_SUBJECT, manifest["entrySubjectId"])

        import_registration = next(
            registration
            for registration in metadata_registration["registrations"]
            if registration["registrationKind"] == "method"
            and registration["subjectId"] == EXPECTED_IMPORT_METHOD_SUBJECT
        )
        self.assertTrue(import_registration["isImported"])
        self.assertEqual("kernel32.dll", import_registration["importModuleName"])
        self.assertEqual("MulDiv", import_registration["importEntryPointName"])

        method_subjects = {
            registration["subjectId"]
            for module in code_registration["modules"]
            for registration in module["registrations"]
            if registration["registrationKind"] == "methodPointer"
        }
        self.assertIn(EXPECTED_ENTRY_SUBJECT, method_subjects)
        self.assertIn(EXPECTED_IMPORT_METHOD_SUBJECT, method_subjects)

    def test_driver_emits_pinvoke_native_reference_skeleton(self) -> None:
        self._build_native_reference_bundle()

        generated_cpp = NATIVE_OUTPUT_ROOT / "generated" / "native-reference.generated.cpp"
        manifest_path = NATIVE_OUTPUT_ROOT / "native-proof.manifest.json"
        plan_path = NATIVE_OUTPUT_ROOT / "native-proof.plan.json"

        self.assertTrue(generated_cpp.is_file(), msg=f"missing generated cpp: {generated_cpp}")
        self.assertTrue(manifest_path.is_file(), msg=f"missing native proof manifest: {manifest_path}")
        self.assertTrue(plan_path.is_file(), msg=f"missing native proof plan: {plan_path}")

        manifest = load_json(manifest_path)
        plan = load_json(plan_path)
        self.assertEqual("PInvokeLiteDllImportMinimal", manifest["assemblyName"])
        self.assertEqual(EXPECTED_ENTRY_SUBJECT, manifest["entrySubjectId"])
        self.assertEqual("native-proof.plan.json", manifest["planArtifactPath"])
        self.assertEqual("nativeReferenceLoweringPlan", plan["artifactKind"])
        self.assertEqual("pinvokeDllImportMinimal", plan["planKind"])
        self.assertEqual("PInvokeLiteDllImportMinimal", plan["assemblyName"])
        self.assertEqual(EXPECTED_ENTRY_SUBJECT, plan["entrySubjectId"])
        self.assertEqual(EXPECTED_IMPORT_METHOD_SUBJECT, plan["importMethodSubjectId"])
        self.assertEqual("kernel32.dll", plan["importModuleName"])
        self.assertEqual("MulDiv", plan["importEntryPointName"])
        self.assertEqual(6, plan["importArgument0"])
        self.assertEqual(7, plan["importArgument1"])
        self.assertEqual(3, plan["importArgument2"])
        self.assertEqual("pinvoke-ok|", plan["outputPrefix"])

        generated_text = generated_cpp.read_text(encoding="utf-8")
        self.assertIn("pinvokeDllImportMinimal", generated_text)
        self.assertIn("MulDiv", generated_text)
        self.assertIn("kernel32.dll", generated_text)
        self.assertIn("pinvoke-ok|", generated_text)

    def test_stage5c_proof_host_wiring_exists(self) -> None:
        self.assertTrue(PROOF_HOST_DIR.is_dir(), msg=f"missing proof host directory: {PROOF_HOST_DIR}")
        self.assertTrue(PROOF_HOST_MAIN_PATH.is_file(), msg=f"missing proof host main: {PROOF_HOST_MAIN_PATH}")
        self.assertTrue(PROOF_HOST_CMAKE_PATH.is_file(), msg=f"missing proof host CMakeLists: {PROOF_HOST_CMAKE_PATH}")
        self.assertTrue(PROOF_HOST_RUN_SCRIPT_PATH.is_file(), msg=f"missing proof host run script: {PROOF_HOST_RUN_SCRIPT_PATH}")

        proof_host_text = PROOF_HOST_MAIN_PATH.read_text(encoding="utf-8")
        self.assertIn("stage5c-pinvoke-lite-reference", proof_host_text)
        self.assertIn("RunNativeReferenceProof", proof_host_text)

    def test_windows_reference_proof_host_reports_expected_output(self) -> None:
        run_root = NATIVE_OUTPUT_ROOT / "run"

        for object_name in EXPECTED_FALLBACK_OBJECT_FILES:
            root_object_path = REPO_ROOT / object_name
            run_object_path = run_root / object_name
            if root_object_path.exists():
                root_object_path.unlink()
            if run_object_path.exists():
                run_object_path.unlink()

        self._run_proof_host_with_msvc()

        stdout_path = run_root / "stdout.log"
        stderr_path = run_root / "stderr.log"
        exit_code_path = run_root / "exit-code.txt"

        self.assertTrue(stdout_path.is_file(), msg=f"missing stdout log: {stdout_path}")
        self.assertTrue(stderr_path.is_file(), msg=f"missing stderr log: {stderr_path}")
        self.assertTrue(exit_code_path.is_file(), msg=f"missing exit code log: {exit_code_path}")

        stdout_text = stdout_path.read_text(encoding="utf-8").strip()
        stderr_text = stderr_path.read_text(encoding="utf-8").strip()
        exit_code_text = exit_code_path.read_text(encoding="utf-8").strip()

        for object_name in EXPECTED_FALLBACK_OBJECT_FILES:
            self.assertFalse((REPO_ROOT / object_name).exists(), msg=f"unexpected repo-root object file: {object_name}")
            self.assertTrue((run_root / object_name).is_file(), msg=f"missing run-root object file: {run_root / object_name}")

        self.assertEqual(EXPECTED_PROOF_OUTPUT, stdout_text)
        self.assertEqual("", stderr_text)
        self.assertEqual("0", exit_code_text)


if __name__ == "__main__":
    unittest.main()
