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
PROJECT_PATH = (
    REPO_ROOT
    / "subjects"
    / "SolutionCorePack"
    / "source"
    / "FeatureSlices"
    / "CoreRuntimeFeatures"
    / "CoreRuntimeFeatures.csproj"
)
DLL_PATH = (
    REPO_ROOT
    / "subjects"
    / "SolutionCorePack"
    / "source"
    / "FeatureSlices"
    / "CoreRuntimeFeatures"
    / "bin"
    / "Release"
    / "net8.0"
    / "CoreRuntimeFeatures.dll"
)
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/AotClosureValidationProofEntry::Run()"
OWNER_SUBJECT_ID = "CoreRuntimeFeatures/AotClosureManifest::BuildRequiredClosure()"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase3-aot-closure-ldtoken-native-aot"
GENERATED_CPP_RELATIVE_PATH = Path("generated") / "native-aot.generated.cpp"
SYNTHETIC_ASSEMBLY_NAME = "LdtokenReflectionNativeAotHarness"
SYNTHETIC_ENTRY_SUBJECT_ID = f"{SYNTHETIC_ASSEMBLY_NAME}/Program::Main()"
GENERIC_SYNTHETIC_ASSEMBLY_NAME = "GenericMethodReflectionNativeAotHarness"
GENERIC_SYNTHETIC_ENTRY_SUBJECT_ID = f"{GENERIC_SYNTHETIC_ASSEMBLY_NAME}/Program::Main()"


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


class Phase3AotClosureLdtokenNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.emit_output_root = OUTPUT_ROOT / f"emit-{uuid.uuid4().hex}"
        cls.synthetic_workspace_root = OUTPUT_ROOT / f"workspace-{uuid.uuid4().hex}"
        cls.synthetic_output_root = OUTPUT_ROOT / f"synthetic-bundle-{uuid.uuid4().hex}"
        cls.synthetic_emit_output_root = OUTPUT_ROOT / f"synthetic-emit-{uuid.uuid4().hex}"
        cls.generic_synthetic_workspace_root = OUTPUT_ROOT / f"generic-workspace-{uuid.uuid4().hex}"
        cls.generic_synthetic_output_root = OUTPUT_ROOT / f"generic-bundle-{uuid.uuid4().hex}"
        cls.generic_synthetic_emit_output_root = OUTPUT_ROOT / f"generic-emit-{uuid.uuid4().hex}"
        cls.bundle_generated = False
        cls.native_emitted = False
        cls.synthetic_bundle_generated = False
        cls.synthetic_native_emitted = False
        cls.generic_synthetic_bundle_generated = False
        cls.generic_synthetic_native_emitted = False

    def _ensure_bundle_generated(self) -> None:
        if self.__class__.bundle_generated:
            return

        if self.output_root.exists():
            shutil.rmtree(self.output_root)

        run_checked(["dotnet", "build", str(TEST_FRAMEWORK_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(TEST_FRAMEWORK_DLL_PATH.is_file(), msg=f"missing test framework dll: {TEST_FRAMEWORK_DLL_PATH}")
        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(DLL_PATH.is_file(), msg=f"missing proof assembly: {DLL_PATH}")

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(DLL_PATH),
                str(self.output_root),
                "--entry-point-subject-id",
                ENTRY_SUBJECT_ID,
                "--additional-assembly",
                str(TEST_FRAMEWORK_DLL_PATH),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.bundle_generated = True

    def _write_synthetic_project(self) -> Path:
        project_root = self.synthetic_workspace_root / SYNTHETIC_ASSEMBLY_NAME
        project_root.mkdir(parents=True, exist_ok=True)
        (project_root / f"{SYNTHETIC_ASSEMBLY_NAME}.csproj").write_text(
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
using System.Reflection;

internal static class Program
{
    private static int Main()
    {
        RuntimeTypeHandle handle = typeof(TokenCarrier).TypeHandle;
        Type resolved = Type.GetTypeFromHandle(handle);
        MethodInfo method = resolved.GetMethod(nameof(TokenCarrier.Helper), BindingFlags.Public | BindingFlags.Static)!;
        RuntimeMethodHandle methodHandle = method.MethodHandle;
        _ = methodHandle;
        return 0;
    }
}

internal static class TokenCarrier
{
    public static int Helper(int value)
    {
        return value + 1;
    }
}
""".strip()
            + "\n",
            encoding="utf-8",
        )
        return project_root

    def _write_generic_synthetic_project(self) -> Path:
        project_root = self.generic_synthetic_workspace_root / GENERIC_SYNTHETIC_ASSEMBLY_NAME
        project_root.mkdir(parents=True, exist_ok=True)
        (project_root / f"{GENERIC_SYNTHETIC_ASSEMBLY_NAME}.csproj").write_text(
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
using System.Reflection;

internal static class Program
{
    private static int Main()
    {
        MethodInfo openMethod = typeof(GenericMethods).GetMethod(nameof(GenericMethods.RoundTrip), BindingFlags.Public | BindingFlags.Static)!;
        MethodInfo closedMethod = openMethod.MakeGenericMethod(typeof(int));
        RuntimeMethodHandle methodHandle = closedMethod.MethodHandle;
        _ = methodHandle;
        return 0;
    }
}

internal static class GenericMethods
{
    public static T RoundTrip<T>(T value)
    {
        return value;
    }
}
""".strip()
            + "\n",
            encoding="utf-8",
        )
        return project_root

    def _ensure_synthetic_bundle_generated(self) -> None:
        if self.__class__.synthetic_bundle_generated:
            return

        if self.synthetic_workspace_root.exists():
            shutil.rmtree(self.synthetic_workspace_root)
        if self.synthetic_output_root.exists():
            shutil.rmtree(self.synthetic_output_root)

        project_root = self._write_synthetic_project()
        project_path = project_root / f"{SYNTHETIC_ASSEMBLY_NAME}.csproj"
        harness_dll_path = project_root / "bin" / "Release" / "net8.0" / f"{SYNTHETIC_ASSEMBLY_NAME}.dll"

        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(project_path), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(harness_dll_path.is_file(), msg=f"missing synthetic harness dll: {harness_dll_path}")

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(harness_dll_path),
                str(self.synthetic_output_root),
                "--entry-point-subject-id",
                SYNTHETIC_ENTRY_SUBJECT_ID,
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.synthetic_bundle_generated = True

    def _ensure_synthetic_native_emitted(self) -> None:
        if self.__class__.synthetic_native_emitted:
            return

        self._ensure_synthetic_bundle_generated()
        if self.synthetic_emit_output_root.exists():
            shutil.rmtree(self.synthetic_emit_output_root)

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.synthetic_output_root),
                str(self.synthetic_emit_output_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.synthetic_native_emitted = True

    def _ensure_generic_synthetic_bundle_generated(self) -> None:
        if self.__class__.generic_synthetic_bundle_generated:
            return

        if self.generic_synthetic_workspace_root.exists():
            shutil.rmtree(self.generic_synthetic_workspace_root)
        if self.generic_synthetic_output_root.exists():
            shutil.rmtree(self.generic_synthetic_output_root)

        project_root = self._write_generic_synthetic_project()
        project_path = project_root / f"{GENERIC_SYNTHETIC_ASSEMBLY_NAME}.csproj"
        harness_dll_path = project_root / "bin" / "Release" / "net8.0" / f"{GENERIC_SYNTHETIC_ASSEMBLY_NAME}.dll"

        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(project_path), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(harness_dll_path.is_file(), msg=f"missing generic synthetic harness dll: {harness_dll_path}")

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(harness_dll_path),
                str(self.generic_synthetic_output_root),
                "--entry-point-subject-id",
                GENERIC_SYNTHETIC_ENTRY_SUBJECT_ID,
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.generic_synthetic_bundle_generated = True

    def _ensure_generic_synthetic_native_emitted(self) -> None:
        if self.__class__.generic_synthetic_native_emitted:
            return

        self._ensure_generic_synthetic_bundle_generated()
        if self.generic_synthetic_emit_output_root.exists():
            shutil.rmtree(self.generic_synthetic_emit_output_root)

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.generic_synthetic_output_root),
                str(self.generic_synthetic_emit_output_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.generic_synthetic_native_emitted = True

    def test_aot_core_ir_freezes_type_only_ldtoken_owner_surface(self) -> None:
        self._ensure_bundle_generated()

        artifact_path = self.output_root / "aot-core-ir.json"
        artifact = json.loads(artifact_path.read_text(encoding="utf-8"))
        owner_method = next(method for method in artifact["methods"] if method["subjectId"] == OWNER_SUBJECT_ID)
        ldtoken_instructions = [instruction for instruction in owner_method["instructions"] if instruction["op"] == "ldtoken"]

        self.assertEqual(9, len(ldtoken_instructions))
        self.assertTrue(all(instruction["reference"]["subjectKind"] == "type" for instruction in ldtoken_instructions))
        self.assertEqual(
            [
                "CoreRuntimeFeatures/AotClosureFactory",
                "System.Private.CoreLib/System.Int32",
                "System.Private.CoreLib/System.String",
                "CoreRuntimeFeatures/AotClosureBox<System.Int32>",
                "CoreRuntimeFeatures/AotClosureBox<System.Int32>",
                "CoreRuntimeFeatures/AotClosureBox<System.String>",
                "CoreRuntimeFeatures/AotClosureBox<System.String>",
                "CoreRuntimeFeatures/AotClosureBox<System.Int32>",
                "CoreRuntimeFeatures/AotClosureBox<System.String>",
            ],
            [instruction["reference"]["subjectId"] for instruction in ldtoken_instructions],
        )

    def test_emit_native_aot_succeeds_for_synthetic_ldtoken_reflection_surface(self) -> None:
        self._ensure_synthetic_native_emitted()

        generated_cpp_path = self.synthetic_emit_output_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: LdtokenReflectionNativeAotHarness/Program::Main()", generated_cpp)
        self.assertIn("chaos_reflection_get_type_from_handle", generated_cpp)
        self.assertIn("chaos_reflection_get_method(", generated_cpp)
        self.assertIn("chaos_reflection_get_method_handle", generated_cpp)

    def test_emit_native_aot_succeeds_for_synthetic_make_generic_method_surface(self) -> None:
        self._ensure_generic_synthetic_native_emitted()

        generated_cpp_path = self.generic_synthetic_emit_output_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: GenericMethodReflectionNativeAotHarness/Program::Main()", generated_cpp)
        self.assertIn("MakeGenericMethod", generated_cpp)


if __name__ == "__main__":
    unittest.main()

