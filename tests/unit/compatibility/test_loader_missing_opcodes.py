from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import read_loader_stage_source


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "loader-missing-opcodes"
ASSEMBLY_NAME = "MissingOpcodesRepro"


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


class LoaderMissingOpcodesTests(unittest.TestCase):
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
    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>
  </PropertyGroup>
</Project>
""".strip()
            + "\n",
            encoding="utf-8",
        )
        (project_root / "Program.cs").write_text(
            """
using System;
using System.Runtime.CompilerServices;

internal static class Program
{
    private struct MyStruct
    {
        internal int X;
        internal int Y;
    }

    private static int Main()
    {
        // Use a variety of opcodes to verify the loader pipeline
        MyStruct a = new MyStruct { X = 42, Y = 99 };
        MyStruct b = new MyStruct { X = a.X, Y = a.Y };

        double d = 1.0;
        int result = 0;
        if (d > 0.0) result += 1;
        if (b.X == 42) result += 2;

        return result;
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
        self.assertTrue(assembly_path.is_file(), msg=f"missing fixture assembly: {assembly_path}")
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

    def test_loader_stage_has_missing_opcodes(self) -> None:
        """Verify Loader source code handles all previously-missing opcodes."""
        loader_source = read_loader_stage_source(REPO_ROOT)

        for required_fragment in [
            "ILOpCode.Break",
            "ILOpCode.Jmp",
            "ILOpCode.Cpobj",
            "ILOpCode.Ckfinite",
            "ILOpCode.Initblk",
            'Op = "jmp"',
            'Op = "ckfinite"',
            'Op = "initblk"',
        ]:
            self.assertIn(required_fragment, loader_source)

    def test_loader_stage_decodes_type_reference_cpobj(self) -> None:
        """Verify Loader routes Cpobj through DecodeTypeReferenceInstruction."""
        loader_source = read_loader_stage_source(REPO_ROOT)

        self.assertIn("ILOpCode.Cpobj => \"cpobj\"", loader_source)

    def test_aot_core_ir_pipeline_succeeds(self) -> None:
        """Verify the full pipeline completes for previously-missing opcodes."""
        self._ensure_bundle_generated()

        aot_core_ir = load_json(self.output_root / "aot-core-ir.json")
        self.assertIsInstance(aot_core_ir["methods"], list)
        self.assertGreater(len(aot_core_ir["methods"]), 0)


if __name__ == "__main__":
    unittest.main()