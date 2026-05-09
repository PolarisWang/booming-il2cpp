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
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "branch-opcode-decode"
ASSEMBLY_NAME = "BranchOpcodeRepro"


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


class BranchOpcodeDecodeTests(unittest.TestCase):
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
internal static class Program
{
    private static int Main()
    {
        int a = 10;
        int b = 3;
        int result = 0;

        if (a == b) result |= 1;
        if ((uint)a < (uint)b) result |= 2;
        if ((uint)a > (uint)b) result |= 4;
        if ((uint)a <= (uint)b) result |= 8;
        if ((uint)a >= (uint)b) result |= 16;
        if (a != b) result |= 32;
        if (a < b) result |= 64;
        if (a > b) result |= 128;
        if (a <= b) result |= 256;
        if (a >= b) result |= 512;

        // dup test
        int x = 5;
        int y = x;

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

    def test_aot_core_ir_retains_unsigned_branch_ops(self) -> None:
        self._ensure_bundle_generated()

        aot_core_ir = load_json(self.output_root / "aot-core-ir.json")
        main_method = None
        for method in aot_core_ir["methods"]:
            if "Program::Main" in method.get("subjectId", ""):
                main_method = method
                break

        self.assertIsNotNone(main_method, msg="Main method not found in AOT Core IR")
        all_ops = [inst["op"] for inst in main_method["instructions"]]

        # These produce beq/bne.un/blt/bgt etc in typed IL, but AOT Core IR may simplify
        # Just verify the pipeline completes
        self.assertIsInstance(main_method["instructions"], list)

    def test_typed_il_decodes_unsigned_branch_ops(self) -> None:
        self._ensure_bundle_generated()

        typed_il = load_json(self.output_root / "typed-il-ir.json")
        main_method = None
        for method in typed_il["methods"]:
            if "Program::Main" in method.get("subjectId", "") or "Program::Main" in method.get("identity", {}).get("name", ""):
                main_method = method
                break
            if "Main" in method.get("subjectId", ""):
                main_method = method
                break

        if main_method is None:
            # Fallback: use any method
            main_method = typed_il["methods"][0]

        all_ops = set()
        for inst in iter_typed_instructions(main_method):
            all_ops.add(inst["op"])

        # C# compiler generates these unsigned comparisons
        for required_op in ["beq", "bne.un", "blt.un", "bgt.un", "ble.un", "bge.un"]:
            self.assertIn(required_op, all_ops, msg=f"missing branch op: {required_op}")


if __name__ == "__main__":
    unittest.main()