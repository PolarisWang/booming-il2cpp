from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
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
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework" / "Chaos.TestFramework.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.dll"
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/AbiWideningProofEntry::Run()"
CREATE_SUBJECT_ID = "CoreRuntimeFeatures/AbiWideningHarness::Create(System.Int32,System.Int32)"
ECHO_SUBJECT_ID = "CoreRuntimeFeatures/AbiWideningHarness::Echo(AbiWideningPair)"
PAIR_SUBJECT_ID = "CoreRuntimeFeatures/AbiWideningPair"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase4d-abi-widening-aot-core-ir"


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


class Phase4DAbiWideningAotCoreIrTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = TEST_OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.bundle_generated = False

    def _ensure_bundle_generated(self) -> None:
        if self.__class__.bundle_generated:
            return

        if self.output_root.exists():
            shutil.rmtree(self.output_root)

        run_checked(["dotnet", "build", str(PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(DLL_PATH.is_file(), msg=f"missing proof assembly: {DLL_PATH}")
        run_checked(["dotnet", "build", str(TEST_FRAMEWORK_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(TEST_FRAMEWORK_DLL_PATH.is_file(), msg=f"missing test framework assembly: {TEST_FRAMEWORK_DLL_PATH}")
        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")

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

    def test_aot_core_ir_contract_exposes_abi_widening_carriers(self) -> None:
        contracts_source = CONTRACTS_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "public enum AotCoreIrAbiCarrierKind : byte",
            "public sealed record AotCoreIrAbiSlotArtifact",
            "public required AotCoreIrAbiSlotArtifact ReturnAbi { get; init; }",
            "public required IReadOnlyList<AotCoreIrAbiSlotArtifact> ParameterAbis { get; init; }",
        ]:
            self.assertIn(required_fragment, contracts_source)

    def test_aot_core_ir_marks_value_type_parameter_and_return_as_widened_abi_carriers(self) -> None:
        self._ensure_bundle_generated()

        artifact_path = self.output_root / "aot-core-ir.json"
        artifact = json.loads(artifact_path.read_text(encoding="utf-8"))
        methods = {method["subjectId"]: method for method in artifact["methods"]}

        echo_method = methods[ECHO_SUBJECT_ID]
        self.assertEqual(3, echo_method["returnAbi"]["carrierKindCode"])
        self.assertEqual(PAIR_SUBJECT_ID, echo_method["returnAbi"]["typeSubjectId"])
        self.assertEqual(1, len(echo_method["parameterAbis"]))
        self.assertEqual(3, echo_method["parameterAbis"][0]["carrierKindCode"])
        self.assertEqual(PAIR_SUBJECT_ID, echo_method["parameterAbis"][0]["typeSubjectId"])

        create_method = methods[CREATE_SUBJECT_ID]
        self.assertEqual(3, create_method["returnAbi"]["carrierKindCode"])
        self.assertEqual(PAIR_SUBJECT_ID, create_method["returnAbi"]["typeSubjectId"])
        self.assertEqual(2, len(create_method["parameterAbis"]))
        self.assertEqual([1, 1], [slot["carrierKindCode"] for slot in create_method["parameterAbis"]])


if __name__ == "__main__":
    unittest.main()
