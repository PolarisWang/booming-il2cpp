from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase6-metadata-supplement-bridge"
HOT_UPDATE_ROOT = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate"
SUPPLEMENTAL_METADATA_WRITER_PATH = HOT_UPDATE_ROOT / "SupplementalMetadataWriter.cs"
SUPPLEMENTAL_METADATA_LOADER_PATH = HOT_UPDATE_ROOT / "SupplementalMetadataLoader.cs"
BRIDGE_GENERATOR_PATH = HOT_UPDATE_ROOT / "BridgeGenerator.cs"
BRIDGE_DISPATCHER_PATH = HOT_UPDATE_ROOT / "BridgeDispatcher.cs"
NATIVE_HOT_UPDATE_HEADER_PATH = REPO_ROOT / "src" / "native" / "hot-update" / "hot_update.h"
NATIVE_HOT_UPDATE_SOURCE_PATH = REPO_ROOT / "src" / "native" / "hot-update" / "hot_update.cpp"
BRIDGE_FIXTURE_ROOT = REPO_ROOT / "tests" / "fixtures" / "subjects"
BRIDGE_ROUNDTRIP_ROOT = BRIDGE_FIXTURE_ROOT / "BridgeRoundtripProof"
BRIDGE_ROUNDTRIP_MANIFEST_PATH = BRIDGE_ROUNDTRIP_ROOT / "subject.manifest.json"
BRIDGE_ROUNDTRIP_PROJECT_PATH = BRIDGE_ROUNDTRIP_ROOT / "source" / "BridgeRoundtripProof.csproj"
BRIDGE_ROUNDTRIP_PROGRAM_PATH = BRIDGE_ROUNDTRIP_ROOT / "source" / "Program.cs"
GENERIC_SUPPLEMENT_ROOT = BRIDGE_FIXTURE_ROOT / "GenericSupplementProof"
GENERIC_SUPPLEMENT_MANIFEST_PATH = GENERIC_SUPPLEMENT_ROOT / "subject.manifest.json"
GENERIC_SUPPLEMENT_PROJECT_PATH = GENERIC_SUPPLEMENT_ROOT / "source" / "GenericSupplementProof.csproj"
GENERIC_SUPPLEMENT_PROGRAM_PATH = GENERIC_SUPPLEMENT_ROOT / "source" / "Program.cs"
SOLUTION_SIMPLE_LIB_FIXTURE_ROOT = REPO_ROOT / "tests" / "fixtures" / "subjects" / "solution-simple-lib"


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


class Phase6MetadataSupplementBridgeTests(unittest.TestCase):
    maxDiff = None

    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_output_root(self, run_label: str) -> Path:
        output_root = TEST_TMP_ROOT / f"{run_label}-{uuid.uuid4().hex}"
        if output_root.exists():
            shutil.rmtree(output_root)
        return output_root

    def _run_convert(self, subject_root: Path, run_label: str) -> Path:
        output_root = self._make_output_root(run_label)
        run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(DRIVER_PROJECT_PATH),
                "--",
                "convert",
                str(subject_root),
                "--output",
                str(output_root),
            ],
            cwd=REPO_ROOT,
        )
        return output_root

    def test_convert_writes_supplemental_metadata_template_and_closure_manifest_reference(self) -> None:
        output_root = self._run_convert(SOLUTION_SIMPLE_LIB_FIXTURE_ROOT, "solution-simple-lib")

        template_path = output_root / "analysis" / "hot-update" / "supplemental-metadata-template.json"
        closure_manifest_path = output_root / "analysis" / "closure.manifest.json"

        self.assertTrue(template_path.is_file(), msg=f"missing supplemental metadata template: {template_path}")
        self.assertTrue(closure_manifest_path.is_file(), msg=f"missing closure manifest: {closure_manifest_path}")

        template = load_json(template_path)
        self.assertEqual("v0", template["formatVersion"])
        self.assertEqual("supplementalMetadataTemplate", template["artifactKind"])

        registered_type_subject_ids = {
            entry["subjectId"]
            for entry in template["registeredTypes"]
        }
        registered_method_subject_ids = {
            entry["subjectId"]
            for entry in template["registeredMethods"]
        }

        self.assertIn("GoldenSimpleLib.App/Program", registered_type_subject_ids)
        self.assertIn("GoldenSimpleLib.Library/Greeter", registered_type_subject_ids)
        self.assertIn("GoldenSimpleLib.App/Program::Main()", registered_method_subject_ids)
        self.assertIn("GoldenSimpleLib.Library/Greeter::BuildMessage()", registered_method_subject_ids)
        self.assertTrue(all(isinstance(entry["metadataToken"], int) for entry in template["registeredTypes"]))
        self.assertTrue(all(isinstance(entry["metadataToken"], int) for entry in template["registeredMethods"]))

        reserved_slots = template["reservedSlots"]
        self.assertGreater(reserved_slots["typeCount"], 0)
        self.assertGreater(reserved_slots["methodCount"], 0)
        self.assertGreater(reserved_slots["genericInstantiationCount"], 0)

        closure_manifest = load_json(closure_manifest_path)
        artifact_paths = {
            artifact["kind"]: artifact["path"]
            for artifact in closure_manifest["artifacts"]
        }
        self.assertEqual(
            "hot-update/supplemental-metadata-template.json",
            artifact_paths["supplementalMetadataTemplate"],
        )

    def test_hot_update_sources_define_writer_loader_bridge_generator_and_native_loader_surface(self) -> None:
        writer_source = SUPPLEMENTAL_METADATA_WRITER_PATH.read_text(encoding="utf-8")
        loader_source = SUPPLEMENTAL_METADATA_LOADER_PATH.read_text(encoding="utf-8")
        bridge_generator_source = BRIDGE_GENERATOR_PATH.read_text(encoding="utf-8")
        bridge_dispatcher_source = BRIDGE_DISPATCHER_PATH.read_text(encoding="utf-8")
        native_header_source = NATIVE_HOT_UPDATE_HEADER_PATH.read_text(encoding="utf-8")
        native_source = NATIVE_HOT_UPDATE_SOURCE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "class SupplementalMetadataWriter",
            "WriteToBytes",
            "HotUpdateMetadataRequirements",
            "genericInstantiation",
        ]:
            self.assertIn(required_fragment, writer_source)

        for required_fragment in [
            "class SupplementalMetadataLoader",
            "LoadFromBytes",
            "TryGetTypeBySubjectId",
            "TryGetTypeByToken",
            "HasGenericInstantiation",
        ]:
            self.assertIn(required_fragment, loader_source)

        for required_fragment in [
            "class BridgeGenerator",
            "Generate",
            "AotToHotUpdate",
            "HotUpdateToAot",
            "HotUpdateToEngine",
            "DelegateWrappers",
        ]:
            self.assertIn(required_fragment, bridge_generator_source)

        for required_fragment in [
            "class BridgeDispatcher",
            "RegisterHotUpdateInt32UnaryTarget",
            "RegisterAotInt32BinaryTarget",
            "RegisterEngineInt32UnaryTarget",
            "InvokeAotToHotUpdateInt32",
            "InvokeHotUpdateToAotInt32",
            "InvokeHotUpdateToEngineInt32",
            "CreateAotDelegateWrapper",
        ]:
            self.assertIn(required_fragment, bridge_dispatcher_source)

        for required_fragment in [
            "struct SupplementalMetadataImage",
            "LoadSupplementalMetadataFromPath",
            "ReleaseSupplementalMetadataImage",
        ]:
            self.assertIn(required_fragment, native_header_source)
            self.assertIn(required_fragment, native_source)

    def test_bridge_roundtrip_and_generic_supplement_subjects_are_realized(self) -> None:
        bridge_manifest = load_json(BRIDGE_ROUNDTRIP_MANIFEST_PATH)
        generic_manifest = load_json(GENERIC_SUPPLEMENT_MANIFEST_PATH)

        self.assertEqual("BridgeRoundtripProof", bridge_manifest["subjectId"])
        self.assertEqual("windows-managed-output", bridge_manifest["defaultMatrix"])
        self.assertEqual("managed-output", bridge_manifest["defaultValidationProfile"])
        self.assertEqual(
            "BridgeRoundtripProof/Program::Main(System.String[])",
            bridge_manifest["source"]["entry"],
        )

        self.assertEqual("GenericSupplementProof", generic_manifest["subjectId"])
        self.assertEqual("windows-managed-output", generic_manifest["defaultMatrix"])
        self.assertEqual("managed-output", generic_manifest["defaultValidationProfile"])
        self.assertEqual(
            "GenericSupplementProof/Program::Main(System.String[])",
            generic_manifest["source"]["entry"],
        )

        self.assertTrue(BRIDGE_ROUNDTRIP_PROJECT_PATH.is_file())
        self.assertTrue(BRIDGE_ROUNDTRIP_PROGRAM_PATH.is_file())
        self.assertTrue(GENERIC_SUPPLEMENT_PROJECT_PATH.is_file())
        self.assertTrue(GENERIC_SUPPLEMENT_PROGRAM_PATH.is_file())

    def test_subject_programs_express_bridge_roundtrip_delegate_and_generic_supplement_outputs(self) -> None:
        bridge_program_source = BRIDGE_ROUNDTRIP_PROGRAM_PATH.read_text(encoding="utf-8")
        generic_program_source = GENERIC_SUPPLEMENT_PROGRAM_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "bridge-aot-to-hot-update=42",
            "bridge-hot-update-to-aot=2",
            "bridge-hot-update-to-engine=7",
            "delegate-roundtrip=42",
            "BridgeGenerator",
            "BridgeDispatcher",
        ]:
            self.assertIn(required_fragment, bridge_program_source)

        for required_fragment in [
            "generic-supplement=ok",
            "TryGetTypeBySubjectId",
            "TryGetTypeByToken",
            "List<string>",
            "SupplementalMetadataWriter",
            "SupplementalMetadataLoader",
        ]:
            self.assertIn(required_fragment, generic_program_source)


if __name__ == "__main__":
    unittest.main()
