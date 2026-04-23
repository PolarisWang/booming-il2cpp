from tests.unit.compatibility.hot_update_productization_gates_test_support import *


class TestHotUpdateProductizationGatesDocs(Phase8ProductizationGatesTestSupport):
    def test_package_validator_uses_version_compatibility_band_instead_of_exact_string_equality(self) -> None:
        validator_source = PACKAGE_VALIDATOR_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            VERSION_HELPER_PATH.is_file(),
            msg=f"missing hot update version compatibility helper: {VERSION_HELPER_PATH}",
        )

        helper_source = VERSION_HELPER_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "record struct HotUpdateSemanticVersion",
            "TryParse",
            "IsCompatibleWith",
            "Major",
            "Minor",
            "Patch",
        ]:
            self.assertIn(required_fragment, helper_source)

        for required_fragment in [
            "HotUpdateSemanticVersion",
            "TryParse",
            "IsCompatibleWith",
            "signature must not be empty",
        ]:
            self.assertIn(required_fragment, validator_source)

        self.assertNotIn("string.Equals(targetAotVersion, currentAotVersion", validator_source)

    def test_productization_docs_define_ios_distribution_boundary_and_version_matrix(self) -> None:
        self.assertTrue(IOS_POLICY_PATH.is_file(), msg=f"missing ios distribution policy doc: {IOS_POLICY_PATH}")
        self.assertTrue(VERSION_MATRIX_PATH.is_file(), msg=f"missing version compatibility matrix doc: {VERSION_MATRIX_PATH}")

        ios_policy_source = IOS_POLICY_PATH.read_text(encoding="utf-8")
        version_matrix_source = VERSION_MATRIX_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "App Store 2.5.2",
            "配置/数据热更新",
            "逻辑热更新",
            "发布合规",
        ]:
            self.assertIn(required_fragment, ios_policy_source)

        for required_fragment in [
            "AOT 主包版本",
            "hot update patch",
            "major.minor",
            "major 不兼容",
        ]:
            self.assertIn(required_fragment, version_matrix_source)

    def test_release_checklist_doc_covers_required_productization_gates(self) -> None:
        self.assertTrue(RELEASE_CHECKLIST_PATH.is_file(), msg=f"missing release checklist doc: {RELEASE_CHECKLIST_PATH}")

        checklist_source = RELEASE_CHECKLIST_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "版本兼容",
            "rollback",
            "mobile runtime host",
            "iOS distribution policy",
            "perf regression",
            "soak test",
            "unsupported feature report",
        ]:
            self.assertIn(required_fragment, checklist_source)

    def test_runtime_manager_defines_rollback_active_patch_listing_and_integrity_validation(self) -> None:
        runtime_manager_source = RUNTIME_MANAGER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "record HotUpdateIntegrityReport",
            "Rollback()",
            "GetActivePatches()",
            "ValidateIntegrity()",
            "HotUpdateAssemblyLoader",
            "SupplementalMetadataRegistry",
            "PackageValidator.ValidateCompatibleTargetAotVersion",
        ]:
            self.assertIn(required_fragment, runtime_manager_source)

    def test_hot_update_skeleton_proof_runs_apply_integrity_rollback_and_reapply(self) -> None:
        proof_source = HOT_UPDATE_SKELETON_PROOF_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "ValidateIntegrity()",
            "GetActivePatches().Count",
            "runtimeManager.Rollback();",
            'Assert.Equal(42, afterReapply);',
            'Assert.Equal(1, afterUnload);',
        ]:
            self.assertIn(required_fragment, proof_source)

        completed = run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(HOT_UPDATE_SKELETON_PROJECT_PATH),
                "--",
                "phase8",
            ],
            cwd=REPO_ROOT,
        )
        self.assertEqual("", completed.stdout.strip())
        self.assertEqual("", completed.stderr.strip())
