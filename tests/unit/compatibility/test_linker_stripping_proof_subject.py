from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import read_contracts_source, read_linker_stage_source, read_loader_stage_source


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack"
MANIFEST_PATH = SUBJECT_ROOT / "subject.manifest.json"
SOURCE_PROJECT_PATH = SUBJECT_ROOT / "source" / "Proofs" / "CoreRuntimeFeatures" / "CoreRuntimeFeatures.csproj"
SOURCE_PROGRAM_PATH = SUBJECT_ROOT / "source" / "Proofs" / "CoreRuntimeFeatures" / "ReflectionAndMetadata" / "LinkerStrippingProof.cs"
FIXTURE_PROJECT_PATH = (
    REPO_ROOT / "tests" / "fixtures" / "contracts" / "linker-stripping-proof" / "FixtureLinkerStrippingProof.csproj"
)
PROFILE_ID = "proof-linker-stripping"
MATRIX_ID = "windows-linker-stripping-check"
ENTRY_POINT = "CoreRuntimeFeatures/LinkerStrippingProofEntry::Run:System.Int32()"

CODEGEN_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "CodeGenStage.cs"
DRIVER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "DriverEntry.cs"
MANAGED_CLOSURE_CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "linker-stripping-proof"


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


class Phase2LinkerStrippingProofTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_output_root(self) -> Path:
        output_root = TEST_TMP_ROOT / uuid.uuid4().hex
        if output_root.exists():
            shutil.rmtree(output_root)
        return output_root

    def test_linker_stripping_subject_tree_realizes_phase2_batch5_slice(self) -> None:
        self.assertTrue(MANIFEST_PATH.is_file(), msg=f"missing subject manifest: {MANIFEST_PATH}")
        self.assertTrue(SOURCE_PROJECT_PATH.is_file(), msg=f"missing source project: {SOURCE_PROJECT_PATH}")
        self.assertTrue(SOURCE_PROGRAM_PATH.is_file(), msg=f"missing source file: {SOURCE_PROGRAM_PATH}")
        self.assertTrue(FIXTURE_PROJECT_PATH.is_file(), msg=f"missing fixture project: {FIXTURE_PROJECT_PATH}")

        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        source = SOURCE_PROGRAM_PATH.read_text(encoding="utf-8")
        self.assertEqual("SolutionCorePack", manifest["subjectId"])
        self.assertEqual("dotnet-project", manifest["source"]["type"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", manifest["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Host/SolutionCorePack.csproj",
            manifest["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeFeatures/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("proof", manifest["validation"]["proof"]["kind"])

        self.assertIn("[ChaosUnitTest(", source)
        self.assertIn('Alias = "linker-stripping-proof"', source)
        self.assertIn("internal sealed class PreserveAttribute", source)
        self.assertIn("[Preserve]", source)
        self.assertIn("internal sealed class PreservedUnusedHarness", source)
        self.assertIn("internal sealed class StrippedUnusedHarness", source)
        self.assertIn("internal static class LinkerStrippingProofEntry", source)

    def test_contract_loader_and_linker_lock_preserve_aware_stripping_surface(self) -> None:
        contracts_source = read_contracts_source(REPO_ROOT)
        loader_source = read_loader_stage_source(REPO_ROOT)
        linker_source = read_linker_stage_source(REPO_ROOT)

        self.assertIn('public const string PreserveDescriptor = "preserve-descriptor.json";', contracts_source)
        self.assertGreaterEqual(contracts_source.count("public bool IsPreserved { get; init; }"), 4)
        self.assertIn('public string ArtifactKind { get; init; } = "preserveDescriptor";', contracts_source)
        self.assertIn("HasPreserveAttribute(", loader_source)
        self.assertIn('string.Equals(typeName, "PreserveAttribute", StringComparison.Ordinal)', loader_source)
        self.assertIn("IncludePreservedClosure(", linker_source)
        self.assertIn('Reason = "preserve-attribute"', linker_source)

    def test_loader_supports_starg_opcodes_for_corelib_full_assembly_closure(self) -> None:
        loader_source = read_loader_stage_source(REPO_ROOT)

        self.assertIn("ILOpCode.Starg => DecodeStargInstruction(ilReader.ReadUInt16())", loader_source)
        self.assertIn("ILOpCode.Starg_s => DecodeStargInstruction(ilReader.ReadByte())", loader_source)
        self.assertIn('Op = "starg"', loader_source)
        self.assertIn('ILOpCode.Conv_u4 => DecodeSimpleInstruction("conv.u4", "System.UInt32")', loader_source)
        self.assertIn('ILOpCode.Conv_u8 => DecodeSimpleInstruction("conv.u8", "System.UInt64")', loader_source)
        self.assertIn('ILOpCode.Conv_r_un => DecodeSimpleInstruction("conv.r.un", "System.Double")', loader_source)
        self.assertIn('ILOpCode.Conv_ovf_i => DecodeSimpleInstruction("conv.ovf.i", "System.IntPtr")', loader_source)
        self.assertIn('ILOpCode.Conv_ovf_u => DecodeSimpleInstruction("conv.ovf.u", "System.UIntPtr")', loader_source)
        self.assertIn('ILOpCode.Conv_ovf_i4 => DecodeSimpleInstruction("conv.ovf.i4", "System.Int32")', loader_source)
        self.assertIn('ILOpCode.Conv_ovf_u2 => DecodeSimpleInstruction("conv.ovf.u2", "System.UInt16")', loader_source)
        self.assertIn('ILOpCode.Conv_ovf_u4 => DecodeSimpleInstruction("conv.ovf.u4", "System.UInt32")', loader_source)
        self.assertIn('ILOpCode.Conv_ovf_u8 => DecodeSimpleInstruction("conv.ovf.u8", "System.UInt64")', loader_source)
        self.assertIn('ILOpCode.Conv_ovf_i4_un => DecodeSimpleInstruction("conv.ovf.i4.un", "System.Int32")', loader_source)
        self.assertIn('ILOpCode.Conv_ovf_u4_un => DecodeSimpleInstruction("conv.ovf.u4.un", "System.UInt32")', loader_source)
        self.assertIn('ILOpCode.Bgt_un => DecodeBranchInstruction("bgt.un", ReadBranchTargetInt32(ref ilReader))', loader_source)
        self.assertIn('ILOpCode.Bgt_un_s => DecodeBranchInstruction("bgt.un", ReadBranchTargetSByte(ref ilReader))', loader_source)
        self.assertIn('ILOpCode.Ble_un => DecodeBranchInstruction("ble.un", ReadBranchTargetInt32(ref ilReader))', loader_source)
        self.assertIn('ILOpCode.Ble_un_s => DecodeBranchInstruction("ble.un", ReadBranchTargetSByte(ref ilReader))', loader_source)
        self.assertIn('ILOpCode.Div_un => DecodeSimpleInstruction("div.un", "System.Int32")', loader_source)
        self.assertIn('ILOpCode.Rem_un => DecodeSimpleInstruction("rem.un", "System.Int32")', loader_source)
        self.assertIn('ILOpCode.Add_ovf_un => DecodeSimpleInstruction("add.ovf.un", "System.Int32")', loader_source)
        self.assertIn('ILOpCode.Sub_ovf_un => DecodeSimpleInstruction("sub.ovf.un", "System.Int32")', loader_source)
        self.assertIn('ILOpCode.Mul_ovf_un => DecodeSimpleInstruction("mul.ovf.un", "System.Int32")', loader_source)
        self.assertIn('ILOpCode.Clt_un => DecodeSimpleInstruction("clt.un", "System.Int32")', loader_source)
        self.assertIn('ILOpCode.Neg => DecodeSimpleInstruction("neg", "System.Int32")', loader_source)
        self.assertIn('ILOpCode.Ldelem_i1 => DecodeTypedArrayInstruction("ldelem", "System.SByte", "System.SByte")', loader_source)
        self.assertIn('ILOpCode.Ldelem_u1 => DecodeTypedArrayInstruction("ldelem", "System.Byte", "System.Byte")', loader_source)
        self.assertIn('ILOpCode.Ldelem_i2 => DecodeTypedArrayInstruction("ldelem", "System.Int16", "System.Int16")', loader_source)
        self.assertIn('ILOpCode.Ldelem_u2 => DecodeTypedArrayInstruction("ldelem", "System.UInt16", "System.UInt16")', loader_source)
        self.assertIn('ILOpCode.Ldelem_u4 => DecodeTypedArrayInstruction("ldelem", "System.UInt32", "System.UInt32")', loader_source)
        self.assertIn('ILOpCode.Ldelem_i8 => DecodeTypedArrayInstruction("ldelem", "System.Int64", "System.Int64")', loader_source)
        self.assertIn('ILOpCode.Ldelem_i => DecodeTypedArrayInstruction("ldelem", "System.IntPtr", "System.IntPtr")', loader_source)
        self.assertIn('ILOpCode.Ldelem_r4 => DecodeTypedArrayInstruction("ldelem", "System.Single", "System.Single")', loader_source)
        self.assertIn('ILOpCode.Stelem_i1 => DecodeTypedArrayInstruction("stelem", "System.Void", "System.SByte")', loader_source)
        self.assertIn('ILOpCode.Stelem_i2 => DecodeTypedArrayInstruction("stelem", "System.Void", "System.Int16")', loader_source)
        self.assertIn('ILOpCode.Stelem_i => DecodeTypedArrayInstruction("stelem", "System.Void", "System.IntPtr")', loader_source)
        self.assertIn('ILOpCode.Stelem_i8 => DecodeTypedArrayInstruction("stelem", "System.Void", "System.Int64")', loader_source)
        self.assertIn('ILOpCode.Stelem_r4 => DecodeTypedArrayInstruction("stelem", "System.Void", "System.Single")', loader_source)
        self.assertIn("ILOpCode.Readonly => null", loader_source)

    def test_codegen_and_driver_surface_freeze_preserve_descriptor_artifact(self) -> None:
        codegen_source = CODEGEN_STAGE_PATH.read_text(encoding="utf-8")
        driver_source = DRIVER_PATH.read_text(encoding="utf-8")
        contracts_source = MANAGED_CLOSURE_CONTRACTS_PATH.read_text(encoding="utf-8")

        self.assertIn("ManagedClosureArtifactNames.PreserveDescriptor", codegen_source)
        self.assertIn('new ManagedClosureArtifactRef { Kind = "preserveDescriptor", Path = ManagedClosureArtifactNames.PreserveDescriptor }', codegen_source)
        self.assertIn("PreserveDescriptor = linkedWorld.PreserveDescriptor", codegen_source)
        self.assertIn("WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.PreserveDescriptor), result.PreserveDescriptor);", driver_source)
        self.assertIn("bool FullAssemblyClosure = false", contracts_source)
        self.assertIn("--full-assembly-closure", driver_source)
        self.assertIn("FullAssemblyClosure: fullAssemblyClosure", driver_source)

    def test_convert_emits_preserve_descriptor_and_keeps_stripped_type_out_of_analysis(self) -> None:
        output_root = self._make_output_root()
        subject_root = output_root / "FixtureLinkerStrippingSubject"
        subject_root.mkdir(parents=True, exist_ok=True)
        (subject_root / "subject.manifest.json").write_text(
            json.dumps(
                {
                    "subjectId": "FixtureLinkerStrippingSubject",
                    "displayName": "FixtureLinkerStrippingSubject",
                    "category": "canonical",
                    "defaultGoal": "correctness.dev",
                    "defaultMatrix": "windows-native-check",
                    "defaultValidationProfile": "proof-dev",
                    "source": {
                        "type": "dotnet-project",
                        "path": "tests/fixtures/contracts/linker-stripping-proof/FixtureLinkerStrippingProof.csproj",
                        "entry": ENTRY_POINT,
                    },
                    "validationProfiles": {
                        "proof-dev": ["proof"],
                    },
                    "validation": {
                        "proof": {
                            "kind": "proof",
                            "defaultVariant": "CHECK",
                        }
                    },
                    "executionPipelines": [
                        {
                            "pipelineId": "proof-runtime-output",
                            "stages": [],
                        }
                    ],
                    "environmentMatrices": [
                        {
                            "matrixId": "windows-native-check",
                            "pipelineId": "proof-runtime-output",
                            "supportedGoals": ["correctness.dev"],
                            "executionContext": {
                                "hostPlatform": "windows-x64",
                                "targetPlatform": "windows-x64",
                                "toolchainProfile": "msvc-reference",
                            },
                            "artifactPlan": {
                                "requiredBuckets": ["source", "host-input", "analysis"],
                                "evidenceTerminalBucket": "analysis",
                            },
                        }
                    ],
                },
                indent=2,
            )
            + "\n",
            encoding="utf-8",
        )

        run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(DRIVER_PROJECT_PATH),
                "--",
                "convert",
                str(subject_root),
                "--entry-point",
                ENTRY_POINT,
                "--output",
                str(output_root),
            ],
            cwd=REPO_ROOT,
        )

        preserve_descriptor = json.loads(
            (output_root / "analysis" / "preserve-descriptor.json").read_text(encoding="utf-8")
        )
        metadata_registration = json.loads(
            (output_root / "analysis" / "metadata-registration.json").read_text(encoding="utf-8")
        )

        preserve_subject_ids = {
            entry["subjectId"]
            for entry in preserve_descriptor["entries"]
        }
        registered_subject_ids = {
            registration["subjectId"]
            for registration in metadata_registration["registrations"]
        }

        self.assertIn("CoreRuntimeFeatures/PreservedUnusedHarness", preserve_subject_ids)
        self.assertNotIn("CoreRuntimeFeatures/StrippedUnusedHarness", preserve_subject_ids)
        self.assertIn("CoreRuntimeFeatures/ReachableHarness", registered_subject_ids)
        self.assertIn("CoreRuntimeFeatures/PreservedUnusedHarness", registered_subject_ids)
        self.assertNotIn("CoreRuntimeFeatures/StrippedUnusedHarness", registered_subject_ids)
        self.assertNotIn("CoreRuntimeFeatures/StrippedUnusedHarness::Marker:System.String()", registered_subject_ids)

    def test_convert_with_full_assembly_closure_preserves_entire_input_assembly_surface(self) -> None:
        output_root = self._make_output_root()
        subject_root = output_root / "FixtureLinkerFullAssemblyClosureSubject"
        subject_root.mkdir(parents=True, exist_ok=True)
        (subject_root / "subject.manifest.json").write_text(
            json.dumps(
                {
                    "subjectId": "FixtureLinkerFullAssemblyClosureSubject",
                    "displayName": "FixtureLinkerFullAssemblyClosureSubject",
                    "category": "canonical",
                    "defaultGoal": "correctness.dev",
                    "defaultMatrix": "windows-native-check",
                    "defaultValidationProfile": "proof-dev",
                    "source": {
                        "type": "dotnet-project",
                        "path": "tests/fixtures/contracts/linker-stripping-proof/FixtureLinkerStrippingProof.csproj",
                        "entry": ENTRY_POINT,
                    },
                    "validationProfiles": {
                        "proof-dev": ["proof"],
                    },
                    "validation": {
                        "proof": {
                            "kind": "proof",
                            "defaultVariant": "CHECK",
                        }
                    },
                    "executionPipelines": [
                        {
                            "pipelineId": "proof-runtime-output",
                            "stages": [],
                        }
                    ],
                    "environmentMatrices": [
                        {
                            "matrixId": "windows-native-check",
                            "pipelineId": "proof-runtime-output",
                            "supportedGoals": ["correctness.dev"],
                            "executionContext": {
                                "hostPlatform": "windows-x64",
                                "targetPlatform": "windows-x64",
                                "toolchainProfile": "msvc-reference",
                            },
                            "artifactPlan": {
                                "requiredBuckets": ["source", "host-input", "analysis"],
                                "evidenceTerminalBucket": "analysis",
                            },
                        }
                    ],
                },
                indent=2,
            )
            + "\n",
            encoding="utf-8",
        )

        run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(DRIVER_PROJECT_PATH),
                "--",
                "convert",
                str(subject_root),
                "--entry-point",
                ENTRY_POINT,
                "--output",
                str(output_root),
                "--full-assembly-closure",
            ],
            cwd=REPO_ROOT,
        )

        preserve_descriptor = json.loads(
            (output_root / "analysis" / "preserve-descriptor.json").read_text(encoding="utf-8")
        )
        metadata_registration = json.loads(
            (output_root / "analysis" / "metadata-registration.json").read_text(encoding="utf-8")
        )

        preserve_subject_ids = {
            entry["subjectId"]
            for entry in preserve_descriptor["entries"]
        }
        registered_subject_ids = {
            registration["subjectId"]
            for registration in metadata_registration["registrations"]
        }

        self.assertIn("CoreRuntimeFeatures/ReachableHarness", preserve_subject_ids)
        self.assertIn("CoreRuntimeFeatures/PreservedUnusedHarness", preserve_subject_ids)
        self.assertIn("CoreRuntimeFeatures/StrippedUnusedHarness", preserve_subject_ids)
        self.assertIn("CoreRuntimeFeatures/StrippedUnusedHarness::Marker:System.String()", preserve_subject_ids)
        self.assertIn("CoreRuntimeFeatures/StrippedUnusedHarness", registered_subject_ids)
        self.assertIn("CoreRuntimeFeatures/StrippedUnusedHarness::Marker:System.String()", registered_subject_ids)


if __name__ == "__main__":
    unittest.main()

