from __future__ import annotations

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
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase3-custom-attribute-native-aot"
GENERATED_CPP_RELATIVE_PATH = Path("generated") / "native-aot.generated.cpp"
DLL_IMPORT_SYNTHETIC_ASSEMBLY_NAME = "DllImportAttributeNativeAotHarness"
DLL_IMPORT_SYNTHETIC_ENTRY_SUBJECT_ID = f"{DLL_IMPORT_SYNTHETIC_ASSEMBLY_NAME}/Program::Main()"
UNMANAGED_CALLERS_ONLY_SYNTHETIC_ASSEMBLY_NAME = "UnmanagedCallersOnlyAttributeNativeAotHarness"
UNMANAGED_CALLERS_ONLY_SYNTHETIC_ENTRY_SUBJECT_ID = f"{UNMANAGED_CALLERS_ONLY_SYNTHETIC_ASSEMBLY_NAME}/Program::Main()"

ENTRY_CASES = {
    "proof_marker": "CoreRuntimeFeatures/CustomAttributeLookupProofEntry::Run()",
}


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


class Phase3CustomAttributeLookupNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.bundle_roots = {
            name: OUTPUT_ROOT / f"{name}-bundle-{uuid.uuid4().hex}"
            for name in ENTRY_CASES
        }
        cls.emit_roots = {
            name: OUTPUT_ROOT / f"{name}-emit-{uuid.uuid4().hex}"
            for name in ENTRY_CASES
        }
        cls.built_inputs = False
        cls.generated_cases: set[str] = set()
        cls.emitted_cases: set[str] = set()
        cls.synthetic_workspace_root = OUTPUT_ROOT / f"synthetic-workspace-{uuid.uuid4().hex}"
        cls.synthetic_bundle_root = OUTPUT_ROOT / f"synthetic-bundle-{uuid.uuid4().hex}"
        cls.synthetic_emit_root = OUTPUT_ROOT / f"synthetic-emit-{uuid.uuid4().hex}"
        cls.synthetic_built = False
        cls.synthetic_emitted = False
        cls.dll_import_synthetic_workspace_root = OUTPUT_ROOT / f"dll-import-synthetic-workspace-{uuid.uuid4().hex}"
        cls.dll_import_synthetic_bundle_root = OUTPUT_ROOT / f"dll-import-synthetic-bundle-{uuid.uuid4().hex}"
        cls.dll_import_synthetic_emit_root = OUTPUT_ROOT / f"dll-import-synthetic-emit-{uuid.uuid4().hex}"
        cls.dll_import_synthetic_built = False
        cls.dll_import_synthetic_emitted = False

    def _ensure_inputs_built(self) -> None:
        if self.__class__.built_inputs:
            return

        run_checked(["dotnet", "build", str(TEST_FRAMEWORK_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(TEST_FRAMEWORK_DLL_PATH.is_file(), msg=f"missing test framework dll: {TEST_FRAMEWORK_DLL_PATH}")
        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(DLL_PATH.is_file(), msg=f"missing proof assembly: {DLL_PATH}")

        self.__class__.built_inputs = True

    def _ensure_bundle_generated(self, case_name: str) -> None:
        if case_name in self.__class__.generated_cases:
            return

        self._ensure_inputs_built()
        output_root = self.bundle_roots[case_name]
        if output_root.exists():
            shutil.rmtree(output_root)

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(DLL_PATH),
                str(output_root),
                "--entry-point-subject-id",
                ENTRY_CASES[case_name],
                "--additional-assembly",
                str(TEST_FRAMEWORK_DLL_PATH),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.generated_cases.add(case_name)

    def _ensure_native_emitted(self, case_name: str) -> None:
        if case_name in self.__class__.emitted_cases:
            return

        self._ensure_bundle_generated(case_name)
        emit_root = self.emit_roots[case_name]
        if emit_root.exists():
            shutil.rmtree(emit_root)

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.bundle_roots[case_name]),
                str(emit_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.emitted_cases.add(case_name)

    def _write_unmanaged_callers_only_synthetic_project(self) -> Path:
        project_root = self.synthetic_workspace_root / UNMANAGED_CALLERS_ONLY_SYNTHETIC_ASSEMBLY_NAME
        project_root.mkdir(parents=True, exist_ok=True)
        (project_root / f"{UNMANAGED_CALLERS_ONLY_SYNTHETIC_ASSEMBLY_NAME}.csproj").write_text(
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
using System.Reflection;
using System.Runtime.InteropServices;

internal static class Program
{
    private static int Main()
    {
        MethodInfo method = typeof(UnmanagedExports).GetMethod(nameof(UnmanagedExports.Add), BindingFlags.Public | BindingFlags.Static)!;
        UnmanagedCallersOnlyAttribute attribute = method.GetCustomAttribute<UnmanagedCallersOnlyAttribute>()!;
        string? entryPoint = attribute.EntryPoint;
        return 0;
    }
}

internal static class UnmanagedExports
{
    [UnmanagedCallersOnly(EntryPoint = "chaos_export")]
    public static int Add(int left, int right)
    {
        return left + right;
    }
}
""".strip()
            + "\n",
            encoding="utf-8",
        )
        return project_root

    def _write_dll_import_synthetic_project(self) -> Path:
        project_root = self.dll_import_synthetic_workspace_root / DLL_IMPORT_SYNTHETIC_ASSEMBLY_NAME
        project_root.mkdir(parents=True, exist_ok=True)
        (project_root / f"{DLL_IMPORT_SYNTHETIC_ASSEMBLY_NAME}.csproj").write_text(
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
using System.Reflection;
using System.Runtime.InteropServices;

internal static class Program
{
    private static int Main()
    {
        MethodInfo method = typeof(NativeImports).GetMethod("GetTickCount64", BindingFlags.NonPublic | BindingFlags.Static)!;
        DllImportAttribute attribute = method.GetCustomAttribute<DllImportAttribute>()!;
        string dllName = attribute.Value;
        return 0;
    }
}

internal static class NativeImports
{
    [DllImport("kernel32.dll", ExactSpelling = true)]
    private static extern ulong GetTickCount64();
}
""".strip()
            + "\n",
            encoding="utf-8",
        )
        return project_root

    def _ensure_unmanaged_callers_only_synthetic_native_emitted(self) -> None:
        if self.__class__.synthetic_emitted:
            return

        self._ensure_inputs_built()
        if self.synthetic_workspace_root.exists():
            shutil.rmtree(self.synthetic_workspace_root)
        if self.synthetic_bundle_root.exists():
            shutil.rmtree(self.synthetic_bundle_root)
        if self.synthetic_emit_root.exists():
            shutil.rmtree(self.synthetic_emit_root)

        project_root = self._write_unmanaged_callers_only_synthetic_project()
        project_path = project_root / f"{UNMANAGED_CALLERS_ONLY_SYNTHETIC_ASSEMBLY_NAME}.csproj"
        harness_dll_path = project_root / "bin" / "Release" / "net8.0" / f"{UNMANAGED_CALLERS_ONLY_SYNTHETIC_ASSEMBLY_NAME}.dll"

        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(project_path), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(harness_dll_path.is_file(), msg=f"missing synthetic harness dll: {harness_dll_path}")
        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(harness_dll_path),
                str(self.synthetic_bundle_root),
                "--entry-point-subject-id",
                UNMANAGED_CALLERS_ONLY_SYNTHETIC_ENTRY_SUBJECT_ID,
            ],
            cwd=REPO_ROOT,
        )
        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.synthetic_bundle_root),
                str(self.synthetic_emit_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.synthetic_built = True
        self.__class__.synthetic_emitted = True

    def _ensure_dll_import_synthetic_native_emitted(self) -> None:
        if self.__class__.dll_import_synthetic_emitted:
            return

        self._ensure_inputs_built()
        if self.dll_import_synthetic_workspace_root.exists():
            shutil.rmtree(self.dll_import_synthetic_workspace_root)
        if self.dll_import_synthetic_bundle_root.exists():
            shutil.rmtree(self.dll_import_synthetic_bundle_root)
        if self.dll_import_synthetic_emit_root.exists():
            shutil.rmtree(self.dll_import_synthetic_emit_root)

        project_root = self._write_dll_import_synthetic_project()
        project_path = project_root / f"{DLL_IMPORT_SYNTHETIC_ASSEMBLY_NAME}.csproj"
        harness_dll_path = project_root / "bin" / "Release" / "net8.0" / f"{DLL_IMPORT_SYNTHETIC_ASSEMBLY_NAME}.dll"

        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(project_path), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(harness_dll_path.is_file(), msg=f"missing synthetic harness dll: {harness_dll_path}")
        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(harness_dll_path),
                str(self.dll_import_synthetic_bundle_root),
                "--entry-point-subject-id",
                DLL_IMPORT_SYNTHETIC_ENTRY_SUBJECT_ID,
            ],
            cwd=REPO_ROOT,
        )
        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.dll_import_synthetic_bundle_root),
                str(self.dll_import_synthetic_emit_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.dll_import_synthetic_built = True
        self.__class__.dll_import_synthetic_emitted = True

    def test_emit_native_aot_succeeds_for_proof_marker_custom_attribute_lookup(self) -> None:
        self._ensure_native_emitted("proof_marker")

        generated_cpp_path = self.emit_roots["proof_marker"] / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: CoreRuntimeFeatures/CustomAttributeLookupProofEntry::Run()", generated_cpp)
        self.assertIn("chaos_reflection_get_custom_attribute", generated_cpp)
        self.assertIn("ProofMarkerAttribute", generated_cpp)

    def test_emit_native_aot_succeeds_for_unmanaged_callers_only_attribute_lookup(self) -> None:
        self._ensure_unmanaged_callers_only_synthetic_native_emitted()

        generated_cpp_path = self.synthetic_emit_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: UnmanagedCallersOnlyAttributeNativeAotHarness/Program::Main()", generated_cpp)
        self.assertIn("UnmanagedCallersOnlyAttribute", generated_cpp)
        self.assertIn("EntryPoint", generated_cpp)

    def test_emit_native_aot_succeeds_for_dll_import_attribute_lookup(self) -> None:
        self._ensure_dll_import_synthetic_native_emitted()

        generated_cpp_path = self.dll_import_synthetic_emit_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: DllImportAttributeNativeAotHarness/Program::Main()", generated_cpp)
        self.assertIn("DllImportAttribute", generated_cpp)
        self.assertIn("chaos_reflection_get_custom_attribute", generated_cpp)


if __name__ == "__main__":
    unittest.main()

