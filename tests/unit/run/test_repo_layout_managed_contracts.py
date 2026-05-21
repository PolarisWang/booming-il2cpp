from tests.unit.run.repo_layout_test_support import *


class RepoLayoutManagedContractsTests(RepoLayoutTestSupport):
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

        self.assertIn("tests/contracts/native/abi", cmake_router)
        self.assertIn("tests/contracts/native/bridge", cmake_router)
        self.assertNotIn("subjects/SolutionCorePack/validation/proof/native-reference", cmake_router)
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

    def test_phase8_proof_root_is_retired(self) -> None:
        self.assertFalse((REPO_ROOT / "tests" / "proof").exists())

    def test_shared_contracts_directory_exists(self) -> None:
        shared_root = REPO_ROOT / "contracts" / "shared" / "v0"
        self.assertTrue(shared_root.is_dir(), msg="contracts/shared/v0/ must exist")
        self.assertTrue((shared_root / "README.md").is_file())
        self.assertTrue((shared_root / "identity-model.md").is_file())
        self.assertTrue((shared_root / "object-model.md").is_file())
        self.assertTrue((shared_root / "handle-model.md").is_file())
        self.assertTrue((shared_root / "abi-calling-convention.md").is_file())
        self.assertTrue((shared_root / "exception-boundary.md").is_file())
        self.assertTrue((shared_root / "delegate-abi.md").is_file())
        self.assertTrue((shared_root / "metadata-token-mapping.md").is_file())
        self.assertTrue((shared_root / "version-policy.md").is_file())
        self.assertTrue((shared_root / "interpreter-ir-decision.md").is_file())
        self.assertTrue((shared_root / "package-manifest.schema.json").is_file())

    def test_manifest_shard_directory_exists(self) -> None:
        shard_dir = REPO_ROOT / "build" / "toolchains" / "run" / "manifests" / "run"
        self.assertTrue(shard_dir.is_dir(), msg="manifest shard directory must exist")
        self.assertTrue((shard_dir / "groups.json").is_file())
        self.assertGreater(len(list(shard_dir.glob("commands.*.json"))), 0)

    def test_managed_codegen_uses_lowering_subdirectory(self) -> None:
        codegen_root = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator"
        lowering_root = codegen_root / "Lowering"
        self.assertTrue(lowering_root.is_dir(), msg="Generator/Lowering/ subdirectory must exist")

    def test_managed_loader_uses_metadata_subdirectory(self) -> None:
        loader_root = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader"
        metadata_root = loader_root / "Metadata"
        models_root = loader_root / "Models"
        self.assertTrue(metadata_root.is_dir(), msg="Loader/Metadata/ subdirectory must exist")
        self.assertTrue(models_root.is_dir(), msg="Loader/Models/ subdirectory must exist")
        self.assertTrue((metadata_root / "MetadataTypeResolver.cs").is_file())
        self.assertTrue((metadata_root / "TypeProviders.cs").is_file())
        self.assertTrue((models_root / "LoaderModels.cs").is_file())
