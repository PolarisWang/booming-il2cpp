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
    / "Proofs"
    / "CoreRuntimeFeatures"
    / "CoreRuntimeFeatures.csproj"
)
DLL_PATH = (
    REPO_ROOT
    / "subjects"
    / "SolutionCorePack"
    / "source"
    / "Proofs"
    / "CoreRuntimeFeatures"
    / "bin"
    / "Release"
    / "net8.0"
    / "CoreRuntimeFeatures.dll"
)
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/RequiredInstantiationClosureProofEntry::Run()"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase4c-metadata-closure-contract"


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


class Phase4CMetadataClosureContractTests(unittest.TestCase):
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

    def test_supplemental_metadata_template_carries_required_instantiation_context(self) -> None:
        self._ensure_bundle_generated()

        generic_demand_graph = json.loads((self.output_root / "generic-instantiation-demand-graph.json").read_text(encoding="utf-8"))
        template_path = self.output_root / "hot-update" / "supplemental-metadata-template.json"
        template = json.loads(template_path.read_text(encoding="utf-8"))
        registered_methods = {entry["subjectId"]: entry for entry in template["registeredMethods"]}

        int_tail = next(
            entry
            for subject_id, entry in registered_methods.items()
            if subject_id.startswith("CoreRuntimeFeatures/RequiredInstantiationHarness::Tail<System.Int32>")
        )
        self.assertEqual(2, int_tail["runtimeGenericContext"]["instantiationKey"]["contextKind"])
        self.assertEqual(
            "CoreRuntimeFeatures/RequiredInstantiationHarness::Tail`1:!!0(System.Collections.Generic.IReadOnlyList<!!0>)",
            int_tail["runtimeGenericContext"]["instantiationKey"]["definitionSubjectId"],
        )
        self.assertEqual([], int_tail["runtimeGenericContext"]["instantiationKey"]["typeArguments"])
        self.assertEqual(
            ["System.Int32"],
            int_tail["runtimeGenericContext"]["instantiationKey"]["methodArguments"],
        )
        self.assertEqual(3, int_tail["runtimeGenericContext"]["supportKindCode"])
        self.assertEqual(2, int_tail["runtimeGenericContext"]["specializationKindCode"])
        self.assertTrue(int_tail["runtimeGenericContext"]["statusReasonCode"].startswith("loader-demand:"))
        self.assertTrue(int_tail["runtimeGenericContext"]["sharedGenericBodyId"]["value"])
        self.assertTrue(int_tail["runtimeGenericContext"]["instantiationStubId"]["value"])
        int_tail_demand = next(
            demand
            for demand in generic_demand_graph["demands"]
            if demand["subjectKind"] == "method" and demand["subjectId"] == int_tail["subjectId"]
        )
        self.assertEqual(int_tail_demand["instantiationKey"], int_tail["runtimeGenericContext"]["instantiationKey"])
        self.assertEqual(int_tail_demand["supportKindCode"], int_tail["runtimeGenericContext"]["supportKindCode"])
        self.assertEqual(
            int_tail_demand["specializationKindCode"],
            int_tail["runtimeGenericContext"]["specializationKindCode"],
        )

        string_tail = next(
            entry
            for subject_id, entry in registered_methods.items()
            if subject_id.startswith("CoreRuntimeFeatures/RequiredInstantiationHarness::Tail<System.String>")
        )
        self.assertEqual(2, string_tail["runtimeGenericContext"]["instantiationKey"]["contextKind"])
        self.assertEqual(
            "CoreRuntimeFeatures/RequiredInstantiationHarness::Tail`1:!!0(System.Collections.Generic.IReadOnlyList<!!0>)",
            string_tail["runtimeGenericContext"]["instantiationKey"]["definitionSubjectId"],
        )
        self.assertEqual([], string_tail["runtimeGenericContext"]["instantiationKey"]["typeArguments"])
        self.assertEqual(
            ["System.String"],
            string_tail["runtimeGenericContext"]["instantiationKey"]["methodArguments"],
        )
        self.assertTrue(string_tail["runtimeGenericContext"]["statusReasonCode"].startswith("loader-demand:"))
        string_tail_demand = next(
            demand
            for demand in generic_demand_graph["demands"]
            if demand["subjectKind"] == "method" and demand["subjectId"] == string_tail["subjectId"]
        )
        self.assertEqual(string_tail_demand["instantiationKey"], string_tail["runtimeGenericContext"]["instantiationKey"])


if __name__ == "__main__":
    unittest.main()

