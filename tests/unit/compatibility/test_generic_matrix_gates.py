from __future__ import annotations

import json
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
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/GenericLayoutProofEntry::Run()"
CANONICAL_ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/GenericLayoutProofEntry::Run:System.Int32()"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "generic-matrix-gates"
MATRIX_BUILDER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "GenericCapabilityMatrixBuilder.cs"
CONTRACTS_SOURCE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"


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


def find_matrix_entry(entries: list[dict[str, object]], subject_id: str) -> dict[str, object]:
    matches = [entry for entry in entries if str(entry.get("subjectId") or "") == subject_id]
    if len(matches) != 1:
        raise AssertionError(f"expected exactly one matrix entry for '{subject_id}', found {len(matches)}")
    return matches[0]


class GenericMatrixGateTests(unittest.TestCase):
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

    def test_generic_matrix_contracts_define_sidecar_and_gate_builder(self) -> None:
        contracts_source = CONTRACTS_SOURCE_PATH.read_text(encoding="utf-8")
        self.assertTrue(MATRIX_BUILDER_PATH.is_file(), msg=f"missing matrix builder: {MATRIX_BUILDER_PATH}")
        matrix_builder_source = MATRIX_BUILDER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            'public const string GenericCapabilityMatrix = "generic-capability-matrix.json";',
            "GenericCapabilityMatrixArtifact",
            "GenericCapabilityBoundaryCaseArtifact",
            "BoundaryCases",
            "HotUpdateNone",
            "HotUpdateMetadataAugmented",
            "HotUpdateGenericSharing",
        ]:
            self.assertIn(required_fragment, contracts_source + "\n" + matrix_builder_source)

        for required_fragment in [
            "unsupportedLeakCount",
            "missingAuthorityCount",
            "nonCanonicalHotUpdateNameCount",
            "GenericSupportKind.Forbidden",
            "HybridCLR",
            "BuildGateStatus(",
        ]:
            self.assertIn(required_fragment, matrix_builder_source)

    def test_generic_matrix_builder_does_not_backfill_missing_authority_from_loader_demand_defaults(self) -> None:
        matrix_builder_source = MATRIX_BUILDER_PATH.read_text(encoding="utf-8")

        for forbidden_fragment in [
            'AuthoritySource = observation is null ? "demand-derived" : "aot-core-ir",',
            'StatusReasonCode = observation?.StatusReasonCode ?? $"loader-demand:{demand.DemandSourceKind}",',
            "OpenDefinitionSubjectId = observation?.OpenDefinitionSubjectId ?? expectedOpenDefinitionSubjectId,",
            "SharedGenericBodyId = observation?.SharedGenericBodyId ?? expectedSharedGenericBodyId,",
            "InstantiationStubId = observation?.InstantiationStubId ?? expectedInstantiationStubId,",
            "HasOpenDefinitionAuthority = !string.IsNullOrWhiteSpace(observation?.OpenDefinitionSubjectId ?? expectedOpenDefinitionSubjectId),",
            "HasSharedBodyAuthority = !string.IsNullOrWhiteSpace(observation?.SharedGenericBodyId ?? expectedSharedGenericBodyId),",
            "HasInstantiationStubAuthority = !string.IsNullOrWhiteSpace(observation?.InstantiationStubId ?? expectedInstantiationStubId),",
            "HasRuntimeGenericContextAuthority = observation?.HasRuntimeGenericContextAuthority ?? true,",
        ]:
            self.assertNotIn(forbidden_fragment, matrix_builder_source)

    def test_driver_emits_generic_capability_matrix_sidecar_with_canonical_hotupdate_names(self) -> None:
        self._ensure_bundle_generated()

        artifact_path = self.output_root / "generic-capability-matrix.json"
        self.assertTrue(artifact_path.is_file(), msg=f"missing matrix artifact: {artifact_path}")
        artifact = json.loads(artifact_path.read_text(encoding="utf-8"))

        self.assertEqual("v0", artifact["formatVersion"])
        self.assertEqual("genericCapabilityMatrix", artifact["artifactKind"])
        self.assertEqual("CoreRuntimeFeatures", artifact["ownerSubjectId"])
        self.assertEqual(CANONICAL_ENTRY_SUBJECT_ID, artifact["entrySubjectId"])
        self.assertEqual(
            ["HotUpdateNone", "HotUpdateMetadataAugmented", "HotUpdateGenericSharing"],
            artifact["hotUpdateModes"],
        )

        gates = artifact["gates"]
        self.assertEqual("ok", gates["status"])
        self.assertEqual(0, gates["unsupportedLeakCount"])
        self.assertEqual(0, gates["missingAuthorityCount"])
        self.assertEqual(0, gates["nonCanonicalHotUpdateNameCount"])
        self.assertEqual([], gates["leaks"])
        self.assertIn("boundaryCases", artifact)
        self.assertIsInstance(artifact["boundaryCases"], list)

        budget_family_ids = {budget["familyId"] for budget in artifact["familyBudgets"]}
        self.assertTrue(
            {
                "ClosedGenericType",
                "ClosedGenericMethod",
                "CrossAssemblyBoundary",
                "HotUpdateBoundary",
                "AsyncTaskFamily",
            }.issubset(budget_family_ids)
        )

    def test_generic_matrix_entries_surface_authority_and_family_counts(self) -> None:
        self._ensure_bundle_generated()

        artifact = json.loads((self.output_root / "generic-capability-matrix.json").read_text(encoding="utf-8"))
        entries = artifact["entries"]

        generic_echo_entry = find_matrix_entry(
            entries,
            "CoreRuntimeFeatures/GenericEcho::Echo<System.Int32>:System.Int32(System.Int32)",
        )
        self.assertEqual("method", generic_echo_entry["subjectKind"])
        self.assertEqual("methodSpec", generic_echo_entry["demandSourceKind"])
        self.assertEqual(3, generic_echo_entry["familyKindCode"])
        self.assertEqual(2, generic_echo_entry["contextKindCode"])
        self.assertEqual(3, generic_echo_entry["supportKindCode"])
        self.assertEqual(2, generic_echo_entry["specializationKindCode"])
        self.assertEqual("aot-core-ir", generic_echo_entry["authoritySource"])
        self.assertTrue(generic_echo_entry["hasOpenDefinitionAuthority"])
        self.assertTrue(generic_echo_entry["hasSharedBodyAuthority"])
        self.assertTrue(generic_echo_entry["hasInstantiationStubAuthority"])
        self.assertTrue(generic_echo_entry["hasRuntimeGenericContextAuthority"])
        self.assertTrue(str(generic_echo_entry["statusReasonCode"]).startswith("loader-demand:"))
        self.assertEqual(
            "CoreRuntimeFeatures/GenericEcho::Echo`1:!!0(!!0)",
            generic_echo_entry["openDefinitionSubjectId"],
        )
        self.assertTrue(generic_echo_entry["sharedGenericBodyId"])
        self.assertTrue(generic_echo_entry["instantiationStubId"])

        generic_box_entry = find_matrix_entry(
            entries,
            "CoreRuntimeFeatures/GenericBox<System.Int32>",
        )
        self.assertEqual("type", generic_box_entry["subjectKind"])
        self.assertEqual("typeSpec", generic_box_entry["demandSourceKind"])
        self.assertEqual(1, generic_box_entry["familyKindCode"])
        self.assertEqual(1, generic_box_entry["contextKindCode"])
        self.assertEqual("aot-core-ir", generic_box_entry["authoritySource"])
        self.assertTrue(generic_box_entry["hasOpenDefinitionAuthority"])
        self.assertTrue(generic_box_entry["hasSharedBodyAuthority"])
        self.assertTrue(generic_box_entry["hasInstantiationStubAuthority"])
        self.assertTrue(generic_box_entry["hasRuntimeGenericContextAuthority"])
        self.assertTrue(str(generic_box_entry["statusReasonCode"]).startswith("loader-demand:"))

        family_budgets = {budget["familyId"]: budget for budget in artifact["familyBudgets"]}
        self.assertGreaterEqual(family_budgets["ClosedGenericType"]["observedCount"], 1)
        self.assertGreaterEqual(family_budgets["ClosedGenericMethod"]["observedCount"], 1)


if __name__ == "__main__":
    unittest.main()
