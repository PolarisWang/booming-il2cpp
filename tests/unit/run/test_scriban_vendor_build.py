from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SCRIBAN_PROPS_PATH = REPO_ROOT / "third_party" / "scriban" / "src" / "Scriban" / "Scriban.props"
SCRIBAN_DIRECTORY_BUILD_PROPS_PATH = REPO_ROOT / "third_party" / "scriban" / "src" / "Directory.Build.props"
MANAGED_DIRECTORY_BUILD_PROPS_PATH = REPO_ROOT / "src" / "managed" / "Directory.Build.props"


class ScribanVendorBuildTests(unittest.TestCase):
    def test_scriban_defaults_to_offline_safe_target_frameworks_for_repo_builds(self) -> None:
        props_text = SCRIBAN_PROPS_PATH.read_text(encoding="utf-8")

        self.assertIn(
            "<ScribanTargetFrameworks Condition=\"'$(ScribanTargetFrameworks)' == '' and '$(ScribanEnablePackaging)' == 'true'\">netstandard2.0;net8.0</ScribanTargetFrameworks>",
            props_text,
        )
        self.assertIn(
            "<ScribanTargetFrameworks Condition=\"'$(ScribanTargetFrameworks)' == ''\">net8.0</ScribanTargetFrameworks>",
            props_text,
        )
        self.assertIn("<TargetFrameworks>$(ScribanTargetFrameworks)</TargetFrameworks>", props_text)

    def test_minver_package_is_opt_in_for_repo_builds(self) -> None:
        props_text = SCRIBAN_PROPS_PATH.read_text(encoding="utf-8")

        self.assertIn('<PackageReference Include="MinVer" Condition="\'$(ScribanEnablePackaging)\' == \'true\'">', props_text)

    def test_scriban_aot_metadata_is_opt_in_for_repo_builds(self) -> None:
        props_text = SCRIBAN_PROPS_PATH.read_text(encoding="utf-8")

        self.assertIn(
            "<ScribanEnableAotCompatibility Condition=\"'$(ScribanEnableAotCompatibility)' == '' and '$(ScribanEnablePackaging)' == 'true'\">true</ScribanEnableAotCompatibility>",
            props_text,
        )
        self.assertIn(
            "<ScribanEnableAotCompatibility Condition=\"'$(ScribanEnableAotCompatibility)' == ''\">false</ScribanEnableAotCompatibility>",
            props_text,
        )
        self.assertIn(
            "<IsAotCompatible Condition=\"'$(ScribanEnableAotCompatibility)' == 'true' and $([MSBuild]::IsTargetFrameworkCompatible('$(TargetFramework)', 'net8.0'))\">true</IsAotCompatible>",
            props_text,
        )

    def test_scriban_directory_build_props_support_temp_intermediate_override(self) -> None:
        props_text = SCRIBAN_DIRECTORY_BUILD_PROPS_PATH.read_text(encoding="utf-8")

        self.assertIn("ChaosTempIntermediateRoot", props_text)
        self.assertIn("$(ChaosTempIntermediateRoot)$(MSBuildProjectName)", props_text)

    def test_managed_directory_build_props_support_temp_intermediate_override(self) -> None:
        props_text = MANAGED_DIRECTORY_BUILD_PROPS_PATH.read_text(encoding="utf-8")

        self.assertIn("ChaosTempIntermediateRoot", props_text)
        self.assertIn("$(ChaosTempIntermediateRoot)$(MSBuildProjectName)", props_text)


if __name__ == "__main__":
    unittest.main()
