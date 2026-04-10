from __future__ import annotations

import json
import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
FORBIDDEN_TEST_PATH_SNIPPETS = [
    "C:" + "\\" + "Users" + "\\",
    "AppData" + "\\" + "Local" + "\\" + "Temp",
]
FORBIDDEN_ACTIVE_PREFIX_SNIPPETS = [
    "boo" + "ming_",
    "boo" + "ming-",
]

EXPECTED_MANAGED_PROJECTS: dict[str, list[str]] = {
    "Chaos.IL2CPP.Contracts": [],
    "Chaos.IL2CPP.Driver": [
        "Chaos.IL2CPP.CodeGen",
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Pipeline",
    ],
    "Chaos.IL2CPP.Loader": [
        "Chaos.IL2CPP.Contracts",
    ],
    "Chaos.IL2CPP.SemanticWorld": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Loader",
    ],
    "Chaos.IL2CPP.Pipeline": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Loader",
        "Chaos.IL2CPP.SemanticWorld",
        "Chaos.IL2CPP.Linker",
        "Chaos.IL2CPP.MetadataWriter",
        "Chaos.IL2CPP.CodeGen",
    ],
    "Chaos.IL2CPP.Linker": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.SemanticWorld",
    ],
    "Chaos.IL2CPP.MetadataWriter": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.Linker",
    ],
    "Chaos.IL2CPP.CodeGen": [
        "Chaos.IL2CPP.Contracts",
        "Chaos.IL2CPP.MetadataWriter",
        "Scriban",
    ],
}
ACTIVE_CONTRACT_DOC_PATHS = [
    REPO_ROOT / "contracts" / "docs" / "v0" / "overview.md",
    REPO_ROOT / "docs" / "architecture" / "runtime-baseline" / "schema-pack-v0.md",
    REPO_ROOT / "docs" / "architecture" / "runtime-baseline" / "hello-world-proof-spec-v0.md",
    REPO_ROOT / "docs" / "architecture" / "runtime-baseline" / "local-verification.md",
    REPO_ROOT / "docs" / "architecture" / "subject-test-framework-v1" / "foundation-and-windows-cutover-v1.md",
]
ACTIVE_ANALYSIS_RETIREMENT_DOC_PATHS = [
    REPO_ROOT / "contracts" / "docs" / "v0" / "overview.md",
    REPO_ROOT / "docs" / "architecture" / "runtime-baseline" / "schema-pack-v0.md",
    REPO_ROOT / "docs" / "architecture" / "runtime-baseline" / "repo-layout.md",
    REPO_ROOT / "docs" / "architecture" / "runtime-baseline" / "ownership-map.md",
    REPO_ROOT / "docs" / "architecture" / "subject-test-framework-v1" / "foundation-and-windows-cutover-v1.md",
    REPO_ROOT / "wiki" / "06-测试验证" / "模块" / "analysis.md",
]
LEGACY_STAGE_TEST_NAMES = [
    "test_stage3_managed_minimal_closure.py",
    "test_stage4_bootstrap_support_skeleton.py",
    "test_stage4_native_reference_proof.py",
    "test_stage5a_generic_echo_windows_reference.py",
    "test_stage5b_reflection_lite_windows_reference.py",
    "test_stage5c_pinvoke_lite_windows_reference.py",
]


def parse_project_references(project_path: Path) -> list[str]:
    root = ET.fromstring(project_path.read_text(encoding="utf-8"))
    references: list[str] = []
    for element in root.findall(".//ProjectReference"):
        include_value = element.attrib.get("Include", "")
        references.append(Path(include_value).stem)
    return sorted(references)


class RepoLayoutTests(unittest.TestCase):
    def test_root_analysis_directory_is_retired(self) -> None:
        self.assertFalse((REPO_ROOT / "analysis").exists())

    def test_managed_projects_exist_with_expected_project_references(self) -> None:
        managed_root = REPO_ROOT / "src" / "managed"
        self.assertTrue(managed_root.is_dir(), msg=f"missing managed root: {managed_root}")

        for project_name, expected_references in EXPECTED_MANAGED_PROJECTS.items():
            project_dir = managed_root / project_name
            project_path = project_dir / f"{project_name}.csproj"

            self.assertTrue(project_dir.is_dir(), msg=f"missing managed project dir: {project_dir}")
            self.assertTrue(project_path.is_file(), msg=f"missing managed project file: {project_path}")
            self.assertEqual(sorted(expected_references), parse_project_references(project_path))

    def test_root_cmake_and_verify_scripts_use_canonical_contract_paths(self) -> None:
        cmake_router = (REPO_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
        verify_py = (REPO_ROOT / "build" / "scripts" / "verify-runtime-baseline.py").read_text(encoding="utf-8")
        verify_ps1 = (REPO_ROOT / "build" / "scripts" / "verify-runtime-baseline.ps1").read_text(encoding="utf-8")

        self.assertIn("subjects/HelloWorldObject/validation/proof/native-reference", cmake_router)
        self.assertIn("tests/contracts/native/abi", cmake_router)
        self.assertIn("tests/contracts/native/bridge", cmake_router)
        self.assertNotIn("tests/proof/native-reference/HelloWorldObject", cmake_router)
        self.assertNotIn("tests/proof/native-reference/GenericEchoClosedMinimal", cmake_router)
        self.assertNotIn("tests/proof/native-reference/ReflectionLiteQueryMinimal", cmake_router)
        self.assertNotIn("tests/contract/native/abi", cmake_router)
        self.assertNotIn("tests/contract/native/bridge", cmake_router)

        self.assertIn('"tests" / "contracts" / "trace" / "compare-warmup-trace.py"', verify_py)
        self.assertIn('"tests" / "contracts" / "trace" / "snapshots" / "windows-warmup-trace.snapshot.json"', verify_py)
        self.assertIn("tests\\contracts\\trace", verify_ps1)
        self.assertNotIn('"tests" / "contract" / "trace"', verify_py)
        self.assertNotIn("tests\\contract\\trace", verify_ps1)

    def test_active_contract_docs_use_samples_paths(self) -> None:
        offenders: list[str] = []
        for doc_path in ACTIVE_CONTRACT_DOC_PATHS:
            content = doc_path.read_text(encoding="utf-8")
            if "contracts/examples/v0/" in content:
                offenders.append(f"{doc_path.relative_to(REPO_ROOT).as_posix()}: contracts/examples/v0/")
            if "contracts/native/examples/v0/" in content:
                offenders.append(f"{doc_path.relative_to(REPO_ROOT).as_posix()}: contracts/native/examples/v0/")
            if "tests/contracts/schema" in content:
                offenders.append(f"{doc_path.relative_to(REPO_ROOT).as_posix()}: tests/contracts/schema")
            if "tests/contract/schema" in content:
                offenders.append(f"{doc_path.relative_to(REPO_ROOT).as_posix()}: tests/contract/schema")

        self.assertEqual([], offenders)

    def test_active_docs_do_not_use_root_analysis_contract_paths(self) -> None:
        offenders: list[str] = []
        for doc_path in ACTIVE_ANALYSIS_RETIREMENT_DOC_PATHS:
            content = doc_path.read_text(encoding="utf-8")
            if "analysis/contracts/" in content:
                offenders.append(f"{doc_path.relative_to(REPO_ROOT).as_posix()}: analysis/contracts/")
            if "- `analysis/`:" in content:
                offenders.append(f"{doc_path.relative_to(REPO_ROOT).as_posix()}: root analysis layout entry")
            if "Analysis track: `analysis/`" in content:
                offenders.append(f"{doc_path.relative_to(REPO_ROOT).as_posix()}: root analysis ownership entry")

        self.assertEqual([], offenders)

    def test_contract_snapshot_baselines_live_under_contracts_tree(self) -> None:
        snapshot_root = REPO_ROOT / "tests" / "contracts" / "analysis" / "v0" / "snapshots"

        self.assertTrue(snapshot_root.is_dir(), msg=f"missing snapshot root: {snapshot_root}")
        self.assertGreater(len(list(snapshot_root.glob("*.snapshot.json"))), 0)
        self.assertFalse((REPO_ROOT / "tests" / "contracts" / "schema").exists())
        self.assertFalse((REPO_ROOT / "tests" / "contract" / "schema").exists())
        self.assertFalse((REPO_ROOT / "contracts" / "artifacts" / "v0" / "samples").exists())
        self.assertFalse((REPO_ROOT / "contracts" / "artifacts" / "v0" / "snapshots").exists())
        self.assertFalse((REPO_ROOT / "contracts" / "examples").exists())
        self.assertFalse((REPO_ROOT / "contracts" / "native" / "examples").exists())
        self.assertFalse((REPO_ROOT / "tests" / "contract").exists())

    def test_tests_proof_tree_only_keeps_generic_guidance(self) -> None:
        proof_root = REPO_ROOT / "tests" / "proof"
        readme_path = proof_root / "README.md"

        self.assertTrue(readme_path.is_file(), msg=f"missing proof guidance: {readme_path}")
        self.assertFalse((proof_root / "hello-world-object").exists())
        self.assertFalse((proof_root / "input").exists())
        self.assertFalse((proof_root / "native-reference").exists())

    def test_subject_native_reference_cmake_treats_generated_cpp_as_existing_input(self) -> None:
        native_reference_cmake = (
            REPO_ROOT
            / "subjects"
            / "HelloWorldObject"
            / "validation"
            / "proof"
            / "native-reference"
            / "CMakeLists.txt"
        ).read_text(encoding="utf-8")

        self.assertIn('if(NOT EXISTS "${CHAOS_SUBJECT_GENERATED_SOURCE}")', native_reference_cmake)
        self.assertNotIn("GENERATED TRUE", native_reference_cmake)

    def test_python_tests_do_not_hardcode_user_specific_temp_paths(self) -> None:
        offenders: list[str] = []
        for test_path in sorted((REPO_ROOT / "tests").rglob("*.py")):
            content = test_path.read_text(encoding="utf-8")
            for snippet in FORBIDDEN_TEST_PATH_SNIPPETS:
                if snippet in content:
                    offenders.append(f"{test_path.relative_to(REPO_ROOT).as_posix()}: {snippet}")

        self.assertEqual([], offenders)

    def test_active_python_sources_do_not_use_chaos_prefix(self) -> None:
        search_roots = [
            REPO_ROOT / "build" / "toolchains" / "run",
            REPO_ROOT / "tests",
        ]
        offenders: list[str] = []

        for root in search_roots:
            for source_path in sorted(root.rglob("*.py")):
                if "__pycache__" in source_path.parts:
                    continue

                content = source_path.read_text(encoding="utf-8")
                for snippet in FORBIDDEN_ACTIVE_PREFIX_SNIPPETS:
                    if snippet in content:
                        offenders.append(f"{source_path.relative_to(REPO_ROOT).as_posix()}: {snippet}")

        self.assertEqual([], offenders)

    def test_subject_manifests_keep_validation_expected_and_baselines_inside_subject_root(self) -> None:
        manifest_paths = sorted((REPO_ROOT / "subjects").rglob("subject.manifest.json"))
        self.assertGreater(len(manifest_paths), 0)

        for manifest_path in manifest_paths:
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            subject_id = str(manifest.get("subjectId") or "")
            validation_profiles = dict(manifest.get("validationProfiles") or {})
            validation = dict(manifest.get("validation") or {})

            self.assertTrue(subject_id)
            self.assertEqual(subject_id, manifest_path.parent.name)
            self.assertTrue(str(manifest.get("defaultValidationProfile") or ""))
            self.assertIn(str(manifest["defaultValidationProfile"]), validation_profiles)

            for validation_spec in validation.values():
                project_path = str(dict(validation_spec).get("project") or "")
                if not project_path:
                    continue
                self.assertTrue(project_path.startswith(f"subjects/{subject_id}/validation/"))
                self.assertNotIn("/tests/", project_path.replace("\\", "/"))

            for expected_path in dict(manifest.get("expected") or {}).values():
                self.assertTrue(str(expected_path).startswith(f"subjects/{subject_id}/expected/"))

            for baseline_path in dict(manifest.get("baselines") or {}).values():
                self.assertTrue(str(baseline_path).startswith(f"subjects/{subject_id}/baselines/"))

    def test_root_tests_tree_does_not_keep_legacy_stage_subject_cases(self) -> None:
        offenders: list[str] = []
        for file_name in LEGACY_STAGE_TEST_NAMES:
            path = REPO_ROOT / "tests" / "unit" / "run" / file_name
            content = path.read_text(encoding="utf-8")
            normalized = content.strip()
            if not normalized.startswith('"""Legacy shim;'):
                offenders.append(str(path.relative_to(REPO_ROOT).as_posix()))
                continue
            if "\n" in normalized:
                offenders.append(str(path.relative_to(REPO_ROOT).as_posix()))

        self.assertEqual([], offenders)

    def test_contracts_dir_contains_only_formal_definitions(self) -> None:
        contracts_root = REPO_ROOT / "contracts"
        forbidden_subdirs = ["samples", "snapshots", "examples"]
        offenders: list[str] = []
        for subdir in contracts_root.rglob("*"):
            if subdir.is_dir() and subdir.name in forbidden_subdirs:
                offenders.append(str(subdir.relative_to(REPO_ROOT).as_posix()))

        self.assertEqual([], offenders, msg="contracts/ must only contain formal definitions, not concrete fixtures")

    def test_run_tooling_uses_domain_based_core_layout(self) -> None:
        run_root = REPO_ROOT / "build" / "toolchains" / "run"
        core_root = run_root / "core"
        subject_root = run_root / "subject"

        self.assertTrue(core_root.is_dir(), msg="build/toolchains/run/core/ must exist")
        self.assertTrue(subject_root.is_dir(), msg="build/toolchains/run/subject/ must exist")
        self.assertTrue((core_root / "common.py").is_file())
        self.assertTrue((core_root / "manifest.py").is_file())
        self.assertTrue((core_root / "result.py").is_file())
        self.assertTrue((core_root / "tooling.py").is_file())
        self.assertTrue((subject_root / "project_workspace.py").is_file())
        self.assertFalse((run_root / "common.py").exists(), msg="common.py must be in core/")
        self.assertFalse((run_root / "manifest.py").exists(), msg="manifest.py must be in core/")
        self.assertFalse((run_root / "result.py").exists(), msg="result.py must be in core/")
        self.assertFalse((run_root / "tooling.py").exists(), msg="tooling.py must be in core/")
        self.assertFalse((run_root / "project_workspace.py").exists(), msg="project_workspace.py must be in subject/")

    def test_manifest_shard_directory_exists(self) -> None:
        shard_dir = REPO_ROOT / "build" / "toolchains" / "run" / "manifests" / "run"
        self.assertTrue(shard_dir.is_dir(), msg="manifest shard directory must exist")
        self.assertTrue((shard_dir / "groups.json").is_file())
        self.assertGreater(len(list(shard_dir.glob("commands.*.json"))), 0)

    def test_managed_codegen_uses_lowering_subdirectory(self) -> None:
        codegen_root = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen"
        lowering_root = codegen_root / "Lowering"
        self.assertTrue(lowering_root.is_dir(), msg="CodeGen/Lowering/ subdirectory must exist")

    def test_managed_loader_uses_metadata_subdirectory(self) -> None:
        loader_root = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader"
        metadata_root = loader_root / "Metadata"
        models_root = loader_root / "Models"
        self.assertTrue(metadata_root.is_dir(), msg="Loader/Metadata/ subdirectory must exist")
        self.assertTrue(models_root.is_dir(), msg="Loader/Models/ subdirectory must exist")
        self.assertTrue((metadata_root / "MetadataTypeResolver.cs").is_file())
        self.assertTrue((metadata_root / "TypeProviders.cs").is_file())
        self.assertTrue((models_root / "LoaderModels.cs").is_file())


if __name__ == "__main__":
    unittest.main()
