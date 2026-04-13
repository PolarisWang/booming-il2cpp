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

EXPECTED_ARTIFACTS = {
    "typed-il-ir.json": REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "samples" / "typed-il-ir.min.json",
    "aot-manifest.json": REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "samples" / "aot-manifest.min.json",
    "metadata-registration.json": REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "samples" / "metadata-registration.min.json",
    "code-registration.json": REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "samples" / "code-registration.min.json",
    "optimization-facts.json": REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "samples" / "optimization-facts.min.json",
    "native-reference.lowering-plan.json": REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "samples" / "native-reference.lowering-plan.min.json",
    "native-aot.lowering-plan.json": REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "samples" / "native-aot.lowering-plan.min.json",
}
ADDITIONAL_BUNDLE_ARTIFACTS = ("preserve-descriptor.json",)
SUPPLEMENTAL_METADATA_TEMPLATE_PATH = "hot-update/supplemental-metadata-template.json"
METADATA_REGISTRATION_MINIMAL_KEYS = ("registrationKind", "slot", "subjectId")
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


def project_metadata_registration_minimal(artifact: object, expected_subject_ids: set[str]) -> object:
    if not isinstance(artifact, dict):
        return artifact

    registrations = artifact.get("registrations")
    if not isinstance(registrations, list):
        return artifact

    return {
        "formatVersion": artifact.get("formatVersion"),
        "artifactKind": artifact.get("artifactKind"),
        "registrations": [
            {key: registration[key] for key in METADATA_REGISTRATION_MINIMAL_KEYS}
            for registration in registrations
            if registration.get("subjectId") in expected_subject_ids
        ],
    }


class ManagedClosureContractBundleTests(unittest.TestCase):
    maxDiff = None

    @classmethod
    def setUpClass(cls) -> None:
        cls.subject_pack_id = "SolutionCorePack"
        cls.subject_id = "HelloWorldObject"
        cls.project_path = (
            REPO_ROOT
            / "subjects"
            / cls.subject_pack_id
            / "source"
            / "Slices"
            / cls.subject_id
            / f"{cls.subject_id}.csproj"
        )
        cls.dll_path = (
            REPO_ROOT
            / "subjects"
            / cls.subject_pack_id
            / "source"
            / "Slices"
            / cls.subject_id
            / "bin"
            / "Release"
            / "net8.0"
            / f"{cls.subject_id}.dll"
        )
        cls.output_root = TEST_INTERMEDIATE_ROOT / "outputs" / f"{cls.subject_id}-{uuid.uuid4().hex}"
        cls.native_output_root = TEST_INTERMEDIATE_ROOT / "native-proof-outputs" / f"{cls.subject_id}-{uuid.uuid4().hex}"
        cls.bundle_generated = False
        cls.native_reference_generated = False

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
                str(REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"),
                str(self.dll_path),
                str(self.output_root),
            ],
            cwd=REPO_ROOT,
        )
        self.__class__.bundle_generated = True

    def _ensure_native_reference_generated(self) -> None:
        self._ensure_bundle_generated()

        if self.__class__.native_reference_generated:
            return

        if self.native_output_root.exists():
            shutil.rmtree(self.native_output_root)

        run_checked(
            [
                "dotnet",
                str(REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"),
                "emit-native-reference",
                str(self.output_root),
                str(self.native_output_root),
            ],
            cwd=REPO_ROOT,
        )
        self.__class__.native_reference_generated = True

    def test_spec_doc_exists_and_points_at_solution_slice_inputs(self) -> None:
        self.assertTrue(SPEC_DOC_PATH.is_file(), msg=f"missing spec doc: {SPEC_DOC_PATH}")

        spec_text = SPEC_DOC_PATH.read_text(encoding="utf-8")
        overview_text = CONTRACT_OVERVIEW_PATH.read_text(encoding="utf-8")

        self.assertIn("subjects/<subject-pack>/source/Slices/<slice-name>/<slice-name>.csproj", spec_text)
        self.assertIn("subjects/<subject-pack>/source/Slices/<slice-name>/bin/Release/net8.0/<slice-name>.dll", spec_text)
        self.assertIn("artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/typed-il-ir.json", spec_text)
        self.assertIn(
            "artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/hot-update/supplemental-metadata-template.json",
            spec_text,
        )
        self.assertIn("closure.manifest.json", spec_text)
        self.assertIn("docs/architecture/roadmap-0/managed-minimal-closure-v0.md", overview_text)

    def test_driver_generates_bundle_from_selected_proof_subject(self) -> None:
        self._ensure_bundle_generated()

        for artifact_name in [*EXPECTED_ARTIFACTS.keys(), *ADDITIONAL_BUNDLE_ARTIFACTS, "closure.manifest.json"]:
            artifact_path = self.output_root / artifact_name
            self.assertTrue(artifact_path.is_file(), msg=f"missing closure artifact: {artifact_path}")

        manifest = load_json(self.output_root / "closure.manifest.json")
        self.assertEqual("v0", manifest["formatVersion"])
        self.assertEqual("managedClosureManifest", manifest["artifactKind"])
        self.assertEqual(self.subject_id, manifest["assemblyName"])
        self.assertEqual(
            f"{self.subject_id}/Program::Main(System.String[])",
            manifest["entrySubjectId"],
        )
        self.assertEqual(str(self.dll_path.relative_to(REPO_ROOT)).replace("\\", "/"), manifest["inputAssemblyPath"])
        self.assertEqual(
            [
                "typed-il-ir.json",
                "aot-manifest.json",
                "metadata-registration.json",
                SUPPLEMENTAL_METADATA_TEMPLATE_PATH,
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

    def test_generated_core_artifacts_match_canonical_contract_samples(self) -> None:
        self._ensure_bundle_generated()

        for generated_name, expected_path in EXPECTED_ARTIFACTS.items():
            generated_path = self.output_root / generated_name
            expected_json = load_json(expected_path)
            generated_json = load_json(generated_path)

            if generated_name == "metadata-registration.json":
                expected_subject_ids = {
                    registration["subjectId"]
                    for registration in expected_json["registrations"]
                }
                generated_json = project_metadata_registration_minimal(generated_json, expected_subject_ids)

            self.assertEqual(expected_json, generated_json, msg=f"artifact mismatch: {generated_name}")

    def test_typed_il_methods_expose_semantic_shape_and_capability_contracts(self) -> None:
        self._ensure_bundle_generated()

        typed_il = load_json(self.output_root / "typed-il-ir.json")
        methods = {
            method["subjectId"]: method
            for method in typed_il["methods"]
        }

        main_method = methods["HelloWorldObject/Program::Main(System.String[])"]
        constructor_method = methods["HelloWorldObject/Greeter::.ctor(System.String)"]
        build_message_method = methods["HelloWorldObject/Greeter::BuildMessage()"]

        self.assertEqual("static-method", main_method["methodRole"])
        self.assertEqual("has-canonical-body", main_method["bodyAvailability"])
        self.assertEqual(
            ["requires-console-string-output", "uses-virtual-call-site"],
            sorted(main_method["capabilities"]),
        )

        self.assertEqual("constructor", constructor_method["methodRole"])
        self.assertEqual("has-canonical-body", constructor_method["bodyAvailability"])

        self.assertEqual("instance-method", build_message_method["methodRole"])
        self.assertEqual("has-canonical-body", build_message_method["bodyAvailability"])
        self.assertEqual(
            ["requires-string-concat", "uses-instance-field-state"],
            sorted(build_message_method["capabilities"]),
        )

    def test_generated_bundle_emits_managed_lowering_plan_and_native_reference_mirrors_it(self) -> None:
        self._ensure_native_reference_generated()

        managed_plan_path = self.output_root / "native-reference.lowering-plan.json"
        native_reference_plan_path = self.native_output_root / "native-reference.plan.json"
        generated_cpp_path = self.native_output_root / "generated" / "native-reference.generated.cpp"

        self.assertTrue(managed_plan_path.is_file(), msg=f"missing managed lowering plan: {managed_plan_path}")
        self.assertTrue(
            native_reference_plan_path.is_file(),
            msg=f"missing mirrored native reference plan: {native_reference_plan_path}",
        )
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated native source: {generated_cpp_path}")

        managed_plan = load_json(managed_plan_path)
        native_reference_plan = load_json(native_reference_plan_path)

        self.assertEqual(managed_plan, native_reference_plan)
        self.assertEqual(
            "managed-object.captured-state-instance-message.minimal",
            managed_plan["planKind"],
        )
        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("NativeReferenceContext", generated_cpp)
        self.assertNotIn("NativeReferenceProofContext", generated_cpp)
        self.assertNotIn("g_proof_context", generated_cpp)


if __name__ == "__main__":
    unittest.main()
