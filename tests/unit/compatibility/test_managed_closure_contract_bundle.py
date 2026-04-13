from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import load_module


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
CONTRACT_OVERVIEW_PATH = REPO_ROOT / "contracts" / "docs" / "v0" / "overview.md"
SPEC_DOC_PATH = REPO_ROOT / "docs" / "architecture" / "roadmap-0" / "managed-minimal-closure-v0.md"
TOOLING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "tooling.py"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
EXPECTED_BUNDLE_ARTIFACTS = (
    "typed-il-ir.json",
    "aot-manifest.json",
    "metadata-registration.json",
    "code-registration.json",
    "optimization-facts.json",
    "preserve-descriptor.json",
    "native-reference.lowering-plan.json",
    "native-aot.lowering-plan.json",
    "closure.manifest.json",
)
TEST_INTERMEDIATE_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "managed-closure-contract"


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


class ManagedClosureContractBundleTests(unittest.TestCase):
    maxDiff = None

    @classmethod
    def setUpClass(cls) -> None:
        cls.subject_pack_id = "SolutionCorePack"
        cls.subject_id = "GoldenSimpleLib.App"
        cls.project_path = (
            REPO_ROOT
            / "subjects"
            / cls.subject_pack_id
            / "source"
            / "Archetypes"
            / "SolutionSimpleLib"
            / "App"
            / "GoldenSimpleLib.App.csproj"
        )
        cls.dll_path = (
            REPO_ROOT
            / "subjects"
            / cls.subject_pack_id
            / "source"
            / "Archetypes"
            / "SolutionSimpleLib"
            / "App"
            / "bin"
            / "Release"
            / "net8.0"
            / "GoldenSimpleLib.App.dll"
        )
        cls.output_root = TEST_INTERMEDIATE_ROOT / "outputs" / f"{cls.subject_id}-{uuid.uuid4().hex}"
        cls.emit_output_root = TEST_INTERMEDIATE_ROOT / "emit-native-reference" / f"{cls.subject_id}-{uuid.uuid4().hex}"
        cls.bundle_generated = False

    def _ensure_bundle_generated(self) -> None:
        if self.__class__.bundle_generated:
            return

        if self.output_root.exists():
            shutil.rmtree(self.output_root)

        tooling_module = load_module(TOOLING_MODULE_PATH, f"chaos_managed_closure_tooling_{self.subject_id}")
        intermediate_root = tooling_module.allocate_dotnet_intermediate_dir(self.subject_id, host_platform="windows")
        if intermediate_root is None:
            intermediate_root = TEST_INTERMEDIATE_ROOT / self.subject_id
        run_checked(
            [
                "dotnet",
                "build",
                str(self.project_path),
                "-c",
                "Release",
                f"-p:BaseIntermediateOutputPath={intermediate_root.as_posix()}/$(MSBuildProjectName)/",
                f"-p:MSBuildProjectExtensionsPath={intermediate_root.as_posix()}/$(MSBuildProjectName)/",
            ],
            cwd=REPO_ROOT,
        )
        self.assertTrue(self.dll_path.is_file(), msg=f"missing proof dll: {self.dll_path}")

        driver_intermediate_root = tooling_module.allocate_dotnet_intermediate_dir("Chaos.IL2CPP.Driver", host_platform="windows")
        self.assertIsNotNone(driver_intermediate_root)
        run_checked(
            [
                "dotnet",
                "build",
                str(DRIVER_PROJECT_PATH),
                "-c",
                "Release",
                "-m:1",
                f"-p:ChaosTempIntermediateRoot={Path(driver_intermediate_root).as_posix()}/",
            ],
            cwd=REPO_ROOT,
        )

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(self.dll_path),
                str(self.output_root),
            ],
            cwd=REPO_ROOT,
        )
        self.__class__.bundle_generated = True

    def test_spec_doc_exists_and_points_at_solution_slice_inputs(self) -> None:
        self.assertTrue(SPEC_DOC_PATH.is_file(), msg=f"missing spec doc: {SPEC_DOC_PATH}")

        spec_text = SPEC_DOC_PATH.read_text(encoding="utf-8")
        overview_text = CONTRACT_OVERVIEW_PATH.read_text(encoding="utf-8")

        self.assertIn("retained solution slice input", spec_text)
        self.assertIn("subjects/<subject-pack>/source/Slices/<slice-name>/<slice-name>.csproj", spec_text)
        self.assertIn("closure.manifest.json", spec_text)
        self.assertIn(
            "artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/hot-update/supplemental-metadata-template.json",
            spec_text,
        )
        self.assertIn("docs/architecture/roadmap-0/managed-minimal-closure-v0.md", overview_text)

    def test_driver_generates_bundle_from_selected_proof_subject(self) -> None:
        self._ensure_bundle_generated()

        for artifact_name in EXPECTED_BUNDLE_ARTIFACTS:
            artifact_path = self.output_root / artifact_name
            self.assertTrue(artifact_path.is_file(), msg=f"missing closure artifact: {artifact_path}")

        manifest = load_json(self.output_root / "closure.manifest.json")
        self.assertEqual("v0", manifest["formatVersion"])
        self.assertEqual("managedClosureManifest", manifest["artifactKind"])
        self.assertEqual(self.subject_id, manifest["assemblyName"])
        self.assertEqual("GoldenSimpleLib.App/Program::Main()", manifest["entrySubjectId"])
        self.assertEqual(str(self.dll_path.relative_to(REPO_ROOT)).replace("\\", "/"), manifest["inputAssemblyPath"])
        self.assertEqual(
            [
                "typed-il-ir.json",
                "aot-manifest.json",
                "metadata-registration.json",
                "hot-update/supplemental-metadata-template.json",
                "code-registration.json",
                "optimization-facts.json",
                "preserve-descriptor.json",
                "native-reference.lowering-plan.json",
                "native-aot.lowering-plan.json",
            ],
            [artifact["path"] for artifact in manifest["artifacts"]],
        )
        self.assertTrue(manifest["inputModuleVersionId"])

    def test_generated_preserve_descriptor_defaults_to_empty_entries_without_preserve_attributes(self) -> None:
        self._ensure_bundle_generated()

        preserve_descriptor = load_json(self.output_root / "preserve-descriptor.json")
        self.assertEqual("v0", preserve_descriptor["formatVersion"])
        self.assertEqual("preserveDescriptor", preserve_descriptor["artifactKind"])
        self.assertEqual([], preserve_descriptor["entries"])

    def test_generated_core_artifacts_match_current_solution_archetype_shape(self) -> None:
        self._ensure_bundle_generated()

        aot_manifest = load_json(self.output_root / "aot-manifest.json")
        metadata_registration = load_json(self.output_root / "metadata-registration.json")
        code_registration = load_json(self.output_root / "code-registration.json")
        optimization_facts = load_json(self.output_root / "optimization-facts.json")

        self.assertEqual("v0", aot_manifest["formatVersion"])
        self.assertEqual("aotManifest", aot_manifest["artifactKind"])
        self.assertEqual(
            [
                "GoldenSimpleLib.App/Program::Main()",
                "GoldenSimpleLib.Library/Greeter::BuildMessage()",
                "System.Console/System.Console::WriteLine(System.String)",
            ],
            [entry["subjectId"] for entry in aot_manifest["entries"]],
        )

        self.assertEqual("v0", metadata_registration["formatVersion"])
        self.assertEqual("metadataRegistration", metadata_registration["artifactKind"])
        self.assertEqual(
            [
                "GoldenSimpleLib.App",
                "GoldenSimpleLib.App/Program",
                "GoldenSimpleLib.App/Program::Main()",
            ],
            [entry["subjectId"] for entry in metadata_registration["registrations"]],
        )

        self.assertEqual("v0", code_registration["formatVersion"])
        self.assertEqual("codeRegistration", code_registration["artifactKind"])
        self.assertEqual("GoldenSimpleLib.App.dll", code_registration["modules"][0]["moduleName"])
        self.assertEqual(
            "GoldenSimpleLib.App/Program::Main()",
            code_registration["modules"][0]["registrations"][0]["subjectId"],
        )

        self.assertEqual("v0", optimization_facts["formatVersion"])
        self.assertEqual("optimizationFacts", optimization_facts["artifactKind"])
        self.assertEqual(
            [
                "GoldenSimpleLib.App/Program",
                "GoldenSimpleLib.App/Program::Main()",
            ],
            [entry["subjectId"] for entry in optimization_facts["closedWorldSpecializations"]],
        )

    def test_typed_il_methods_expose_semantic_shape_and_capability_contracts(self) -> None:
        self._ensure_bundle_generated()

        typed_il = load_json(self.output_root / "typed-il-ir.json")
        methods = {
            method["subjectId"]: method
            for method in typed_il["methods"]
        }

        self.assertEqual(["GoldenSimpleLib.App/Program::Main()"], sorted(methods.keys()))

        main_method = methods["GoldenSimpleLib.App/Program::Main()"]
        self.assertEqual("static-method", main_method["methodRole"])
        self.assertEqual("has-canonical-body", main_method["bodyAvailability"])
        self.assertEqual(["requires-console-string-output"], main_method["capabilities"])
        self.assertEqual(
            [
                "GoldenSimpleLib.Library/Greeter::BuildMessage()",
                "System.Console/System.Console::WriteLine(System.String)",
            ],
            [
                instruction["callee"]
                for instruction in main_method["blocks"][0]["instructions"]
                if instruction["op"] == "call"
            ],
        )

    def test_generic_analysis_only_bundle_rejects_legacy_emit_native_reference(self) -> None:
        self._ensure_bundle_generated()

        lowering_plan = load_json(self.output_root / "native-reference.lowering-plan.json")
        self.assertEqual("generic-analysis-only", lowering_plan["planKind"])
        self.assertEqual("GoldenSimpleLib.App/Program::Main()", lowering_plan["entrySubjectId"])

        if self.emit_output_root.exists():
            shutil.rmtree(self.emit_output_root)

        completed = subprocess.run(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-reference",
                str(self.output_root),
                str(self.emit_output_root),
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            check=False,
        )

        self.assertNotEqual(0, completed.returncode)
        combined_output = "\n".join(part for part in [completed.stdout, completed.stderr] if part)
        self.assertIn("unsupported managed lowering plan kind 'generic-analysis-only'", combined_output)


if __name__ == "__main__":
    unittest.main()
