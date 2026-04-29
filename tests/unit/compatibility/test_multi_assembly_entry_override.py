from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import load_module


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
TOOLING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "tooling.py"
TEST_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "multi-assembly-entry-override"


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


class MultiAssemblyEntryOverrideTests(unittest.TestCase):
    maxDiff = None

    @classmethod
    def setUpClass(cls) -> None:
        TEST_ROOT.mkdir(parents=True, exist_ok=True)
        tooling_module = load_module(TOOLING_MODULE_PATH, "chaos_multi_assembly_entry_override_tooling")
        intermediate_root = tooling_module.allocate_dotnet_intermediate_dir("Chaos.IL2CPP.Driver", host_platform="windows")
        if intermediate_root is None:
            raise AssertionError("failed to allocate driver intermediate root")
        run_checked(
            [
                "dotnet",
                "build",
                str(DRIVER_PROJECT_PATH),
                "-c",
                "Release",
                "-m:1",
                f"-p:ChaosTempIntermediateRoot={Path(intermediate_root).as_posix()}/",
            ],
            cwd=REPO_ROOT,
        )
        if not DRIVER_DLL_PATH.is_file():
            raise AssertionError(f"missing driver dll: {DRIVER_DLL_PATH}")

    def setUp(self) -> None:
        self.test_root = TEST_ROOT / uuid.uuid4().hex
        self.test_root.mkdir(parents=True, exist_ok=False)

    def tearDown(self) -> None:
        shutil.rmtree(self.test_root, ignore_errors=True)

    def test_driver_supports_multi_assembly_entry_override_for_closure_and_native_aot(self) -> None:
        source_root = self.test_root / "source"
        app_root = source_root / "App"
        library_root = source_root / "Library"
        host_input_root = self.test_root / "host-input"
        analysis_root = self.test_root / "analysis"
        generated_root = self.test_root / "generated"
        app_root.mkdir(parents=True, exist_ok=True)
        library_root.mkdir(parents=True, exist_ok=True)

        (library_root / "Library.csproj").write_text(
            "\n".join(
                [
                    '<Project Sdk="Microsoft.NET.Sdk">',
                    "  <PropertyGroup>",
                    "    <TargetFramework>net8.0</TargetFramework>",
                    "    <ImplicitUsings>disable</ImplicitUsings>",
                    "    <Nullable>disable</Nullable>",
                    "  </PropertyGroup>",
                    "</Project>",
                    "",
                ]
            ),
            encoding="utf-8",
        )
        (library_root / "Entry.cs").write_text(
            "\n".join(
                [
                    "namespace Library;",
                    "",
                    "public static class Entry",
                    "{",
                    "    public static int Run()",
                    "    {",
                    "        var left = 40;",
                    "        return Helper.Add(left, 2);",
                    "    }",
                    "}",
                    "",
                    "public static class Helper",
                    "{",
                    "    public static int Add(int left, int right)",
                    "    {",
                    "        return left + right;",
                    "    }",
                    "}",
                    "",
                ]
            ),
            encoding="utf-8",
        )

        (app_root / "App.csproj").write_text(
            "\n".join(
                [
                    '<Project Sdk="Microsoft.NET.Sdk">',
                    "  <PropertyGroup>",
                    "    <OutputType>Exe</OutputType>",
                    "    <TargetFramework>net8.0</TargetFramework>",
                    "    <ImplicitUsings>disable</ImplicitUsings>",
                    "    <Nullable>disable</Nullable>",
                    "  </PropertyGroup>",
                    "  <ItemGroup>",
                    '    <ProjectReference Include="..\\Library\\Library.csproj" />',
                    "  </ItemGroup>",
                    "</Project>",
                    "",
                ]
            ),
            encoding="utf-8",
        )
        (app_root / "Program.cs").write_text(
            "\n".join(
                [
                    "namespace App;",
                    "",
                    "public static class Program",
                    "{",
                    "    public static int Main(string[] args)",
                    "    {",
                    "        return 0;",
                    "    }",
                    "}",
                    "",
                ]
            ),
            encoding="utf-8",
        )

        run_checked(
            [
                "dotnet",
                "build",
                str(app_root / "App.csproj"),
                "-c",
                "Release",
                "-m:1",
                "-o",
                str(host_input_root),
            ],
            cwd=REPO_ROOT,
        )

        app_dll_path = host_input_root / "App.dll"
        library_dll_path = host_input_root / "Library.dll"
        self.assertTrue(app_dll_path.is_file(), msg=f"missing primary assembly: {app_dll_path}")
        self.assertTrue(library_dll_path.is_file(), msg=f"missing secondary assembly: {library_dll_path}")

        entry_override_subject_id = "Library/Entry::Run()"
        canonical_entry_subject_id = "Library/Entry::Run:System.Int32()"
        canonical_helper_add_subject_id = "Library/Helper::Add:System.Int32(System.Int32,System.Int32)"
        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(app_dll_path),
                str(analysis_root),
                "--entry-point-subject-id",
                entry_override_subject_id,
                "--additional-assembly",
                str(library_dll_path),
            ],
            cwd=REPO_ROOT,
        )

        closure_manifest = load_json(analysis_root / "closure.manifest.json")
        self.assertEqual(canonical_entry_subject_id, closure_manifest["entrySubjectId"])
        self.assertEqual(
            str(app_dll_path.relative_to(REPO_ROOT)).replace("\\", "/"),
            closure_manifest["inputAssemblyPath"],
        )
        self.assertEqual(
            [str(library_dll_path.relative_to(REPO_ROOT)).replace("\\", "/")],
            closure_manifest["additionalAssemblyPaths"],
        )

        typed_il = load_json(analysis_root / "typed-il-ir.json")
        self.assertIn(
            canonical_entry_subject_id,
            {method["subjectId"] for method in typed_il["methods"]},
        )
        aot_core_ir = load_json(analysis_root / "aot-core-ir.json")
        entry_method = next(
            method
            for method in aot_core_ir["methods"]
            if method["subjectId"] == canonical_entry_subject_id
        )
        call_instruction = next(
            instruction
            for instruction in entry_method["instructions"]
            if instruction["op"] == "call"
        )
        self.assertEqual("Library", call_instruction["reference"]["assemblyName"])
        self.assertEqual("method", call_instruction["reference"]["subjectKind"])
        self.assertEqual(
            canonical_helper_add_subject_id,
            call_instruction["reference"]["subjectId"],
        )
        code_registration = load_json(analysis_root / "code-registration.json")
        helper_add_symbol = next(
            registration["symbol"]
            for module in code_registration["modules"]
            for registration in module["registrations"]
            if registration["subjectId"] == canonical_helper_add_subject_id
        )

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(analysis_root),
                str(generated_root),
            ],
            cwd=REPO_ROOT,
        )

        native_aot_manifest = load_json(generated_root / "native-aot.manifest.json")
        self.assertEqual(canonical_entry_subject_id, native_aot_manifest["entrySubjectId"])
        generated_cpp_path = generated_root / "generated" / "native-aot.generated.cpp"
        self.assertTrue(generated_cpp_path.is_file(), msg="missing native-aot generated source")
        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn(
            f'extern "C" CHAOS_IL2CPP_INT32 {helper_add_symbol}(CHAOS_IL2CPP_INT32 chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1)\n{{',
            generated_cpp,
        )
        self.assertIn(f"{helper_add_symbol}(", generated_cpp)


if __name__ == "__main__":
    unittest.main()
