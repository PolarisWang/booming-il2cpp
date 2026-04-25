from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "loader-vararg-decode"
ASSEMBLY_NAME = "VarArgDecodeRepro"
HELPER_SUBJECT_ID = f"{ASSEMBLY_NAME}/Program::Helper:System.Int32(System.Int32,System.Int32,System.Int32)"
HELPER_DEFINITION_SUBJECT_ID = f"{ASSEMBLY_NAME}/Program::Helper:System.Int32(System.Int32)"


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


def iter_typed_instructions(method_artifact: dict[str, object]) -> list[dict[str, object]]:
    instructions: list[dict[str, object]] = []
    for block in method_artifact["blocks"]:
        instructions.extend(block["instructions"])
    return instructions


class LoaderVarArgDecodeTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.workspace_root = TEST_OUTPUT_ROOT / f"workspace-{uuid.uuid4().hex}"
        cls.output_root = TEST_OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.bundle_generated = False

    def _write_fixture_project(self) -> Path:
        project_root = self.workspace_root / ASSEMBLY_NAME
        project_root.mkdir(parents=True, exist_ok=True)
        (project_root / f"{ASSEMBLY_NAME}.csproj").write_text(
            """
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFramework>net8.0</TargetFramework>
    <ImplicitUsings>disable</ImplicitUsings>
    <Nullable>disable</Nullable>
  </PropertyGroup>
</Project>
""".strip()
            + "\n",
            encoding="utf-8",
        )
        (project_root / "Program.cs").write_text(
            """
using System;

internal static class Program
{
    private static int Main()
    {
        return Helper(1, __arglist(2, 3));
    }

    private static int Helper(int head, __arglist)
    {
        ArgIterator iterator = new ArgIterator(__arglist);
        return head == 1 && iterator.GetRemainingCount() == 2 ? 0 : 1;
    }
}
""".strip()
            + "\n",
            encoding="utf-8",
        )
        return project_root

    def _ensure_bundle_generated(self) -> None:
        if self.__class__.bundle_generated:
            return

        if self.workspace_root.exists():
            shutil.rmtree(self.workspace_root)
        if self.output_root.exists():
            shutil.rmtree(self.output_root)

        project_root = self._write_fixture_project()
        project_path = project_root / f"{ASSEMBLY_NAME}.csproj"
        assembly_path = project_root / "bin" / "Release" / "net8.0" / f"{ASSEMBLY_NAME}.dll"

        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(project_path), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(assembly_path.is_file(), msg=f"missing vararg fixture assembly: {assembly_path}")
        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(assembly_path),
                str(self.output_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.bundle_generated = True

    def test_typed_il_decodes_arglist_opcode_for_vararg_helper(self) -> None:
        self._ensure_bundle_generated()

        typed_il = load_json(self.output_root / "typed-il-ir.json")
        method = next(method for method in typed_il["methods"] if method["subjectId"] == HELPER_SUBJECT_ID)
        self.assertEqual(HELPER_DEFINITION_SUBJECT_ID, method["identity"]["definitionSubjectId"])
        self.assertEqual("System.Int32 Program::Helper(System.Int32)", method["signature"])
        arglist = next(
            instruction
            for instruction in iter_typed_instructions(method)
            if instruction["op"] == "arglist"
        )

        self.assertEqual("System.RuntimeArgumentHandle", arglist["resultType"])
        self.assertNotIn("reference", arglist)

    def test_aot_core_ir_keeps_arglist_instruction_identity(self) -> None:
        self._ensure_bundle_generated()

        aot_core_ir = load_json(self.output_root / "aot-core-ir.json")
        method = next(method for method in aot_core_ir["methods"] if method["subjectId"] == HELPER_SUBJECT_ID)
        self.assertEqual(HELPER_DEFINITION_SUBJECT_ID, method["identity"]["definitionSubjectId"])
        self.assertEqual(1, method["localCount"])
        arglist = next(
            instruction
            for instruction in method["instructions"]
            if instruction["op"] == "arglist"
        )

        self.assertEqual("System.RuntimeArgumentHandle", arglist["resultType"])
        self.assertNotIn("reference", arglist)


if __name__ == "__main__":
    unittest.main()
