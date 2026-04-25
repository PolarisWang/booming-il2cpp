from __future__ import annotations

import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH, SOLUTION_CORE_PACK_PROOFS_ROOT


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
PROJECT_PATH = SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH
DLL_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "bin" / "Release" / "net8.0" / "CoreRuntimeFeatures.dll"
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
DISPATCH_SOURCE_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "ObjectModelAndDispatch" / "DispatchProof.cs"
INTERFACE_SOURCE_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "ObjectModelAndDispatch" / "InterfaceDispatchProof.cs"
ENTRY_SUBJECT_IDS = {
    "virtual": "CoreRuntimeFeatures/DispatchProofEntry::Run()",
    "interface": "CoreRuntimeFeatures/InterfaceDispatchProofEntry::Run()",
}
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "dispatch-generic-virtual-native-aot"
GENERATED_CPP_RELATIVE_PATH = Path("generated") / "native-aot.generated.cpp"


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


class DispatchGenericVirtualNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.bundle_roots = {
            key: TEST_OUTPUT_ROOT / f"{key}-bundle-{uuid.uuid4().hex}"
            for key in ENTRY_SUBJECT_IDS
        }
        cls.emit_roots = {
            key: TEST_OUTPUT_ROOT / f"{key}-emit-{uuid.uuid4().hex}"
            for key in ENTRY_SUBJECT_IDS
        }
        cls.bundle_generated = {key: False for key in ENTRY_SUBJECT_IDS}
        cls.native_emitted = {key: False for key in ENTRY_SUBJECT_IDS}
        cls.project_built = False

    def _ensure_project_built(self) -> None:
        if self.__class__.project_built:
            return

        run_checked(["dotnet", "build", str(TEST_FRAMEWORK_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(TEST_FRAMEWORK_DLL_PATH.is_file(), msg=f"missing test framework dll: {TEST_FRAMEWORK_DLL_PATH}")
        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(DLL_PATH.is_file(), msg=f"missing proof assembly: {DLL_PATH}")
        self.__class__.project_built = True

    def _ensure_bundle_generated(self, key: str) -> None:
        if self.__class__.bundle_generated[key]:
            return

        self._ensure_project_built()
        output_root = self.bundle_roots[key]
        if output_root.exists():
            shutil.rmtree(output_root)

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(DLL_PATH),
                str(output_root),
                "--entry-point-subject-id",
                ENTRY_SUBJECT_IDS[key],
                "--additional-assembly",
                str(TEST_FRAMEWORK_DLL_PATH),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.bundle_generated[key] = True

    def _ensure_native_emitted(self, key: str) -> None:
        if self.__class__.native_emitted[key]:
            return

        self._ensure_bundle_generated(key)
        output_root = self.emit_roots[key]
        if output_root.exists():
            shutil.rmtree(output_root)

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.bundle_roots[key]),
                str(output_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.native_emitted[key] = True

    def test_generic_dispatch_sources_use_real_generic_virtual_and_interface_shapes(self) -> None:
        dispatch_source = DISPATCH_SOURCE_PATH.read_text(encoding="utf-8")
        interface_source = INTERFACE_SOURCE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "internal abstract class DispatchBase<T>",
            "internal sealed class DispatchLeaf<T> : DispatchBase<T>",
            "DispatchBase<int> instance = new DispatchLeaf<int>(42);",
        ]:
            self.assertIn(required_fragment, dispatch_source)

        for required_fragment in [
            "internal interface IDispatchValue<T>",
            "internal sealed class InterfaceValue<T> : IDispatchValue<T>",
            "IDispatchValue<int> value = new InterfaceValue<int>(21);",
            "Assert.Equal(21, value.ReadValue());",
        ]:
            self.assertIn(required_fragment, interface_source)

    def test_emit_native_aot_succeeds_for_generic_virtual_dispatch_proof(self) -> None:
        self._ensure_native_emitted("virtual")

        generated_cpp_path = self.emit_roots["virtual"] / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("switch (chaos_header->type_id)", generated_cpp)
        self.assertIn("DispatchLeaf", generated_cpp)
        self.assertIn("DispatchProofEntry_Run", generated_cpp)

    def test_emit_native_aot_succeeds_for_generic_interface_dispatch_proof(self) -> None:
        self._ensure_native_emitted("interface")

        generated_cpp_path = self.emit_roots["interface"] / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("switch (chaos_header->type_id)", generated_cpp)
        self.assertIn("InterfaceValue", generated_cpp)
        self.assertIn("InterfaceDispatchProofEntry_Run", generated_cpp)


if __name__ == "__main__":
    unittest.main()

