from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
HOT_UPDATE_ROOT = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate"

PACKAGE_VALIDATOR_PATH = HOT_UPDATE_ROOT / "PackageValidator.cs"
VERSION_HELPER_PATH = HOT_UPDATE_ROOT / "HotUpdateVersionCompatibility.cs"

IOS_POLICY_PATH = REPO_ROOT / "docs" / "architecture" / "ios-distribution-policy.md"
VERSION_MATRIX_PATH = REPO_ROOT / "docs" / "architecture" / "version-compatibility-matrix.md"


class Phase8ProductizationGatesTests(unittest.TestCase):
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


if __name__ == "__main__":
    unittest.main()
