from __future__ import annotations

import json
import subprocess
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SCRIBAN_PROPS_PATH = REPO_ROOT / "third_party" / "scriban" / "src" / "Scriban" / "Scriban.props"
SCRIBAN_DIRECTORY_BUILD_PROPS_PATH = REPO_ROOT / "third_party" / "scriban" / "src" / "Directory.Build.props"
MANAGED_DIRECTORY_BUILD_PROPS_PATH = REPO_ROOT / "src" / "managed" / "Directory.Build.props"
SUBJECTS_DIRECTORY_BUILD_PROPS_PATH = REPO_ROOT / "subjects" / "Directory.Build.props"
CONTRACTS_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "Chaos.IL2CPP.Contracts.csproj"
MAINLINE_FEATURE_PACK_PROJECT_PATH = REPO_ROOT / "subjects" / "MainlineFeaturePack" / "source" / "MainlineFeaturePack.csproj"


def _msbuild_compile_identities(project_path: Path, properties: list[str]) -> list[str]:
    completed = subprocess.run(
        [
            "dotnet",
            "msbuild",
            str(project_path),
            "-nologo",
            "-getItem:Compile",
            "-property:Configuration=Release",
            *properties,
        ],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        check=False,
    )
    if completed.returncode != 0:
        output = "\n".join(part for part in [completed.stdout, completed.stderr] if part).strip()
        raise AssertionError(f"command failed: {project_path}\n{output}")

    payload = json.loads(completed.stdout)
    return [str(item["Identity"]) for item in payload["Items"]["Compile"]]


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
        self.assertIn("obj/**", props_text)

    def test_subjects_directory_build_props_exclude_local_obj_sources(self) -> None:
        props_text = SUBJECTS_DIRECTORY_BUILD_PROPS_PATH.read_text(encoding="utf-8")

        self.assertIn("DefaultItemExcludes", props_text)
        self.assertIn("obj/**", props_text)

    def test_scriban_directory_build_props_exclude_local_obj_sources(self) -> None:
        props_text = SCRIBAN_DIRECTORY_BUILD_PROPS_PATH.read_text(encoding="utf-8")

        self.assertIn("DefaultItemExcludes", props_text)
        self.assertIn("obj/**", props_text)

    def test_managed_temp_intermediate_override_keeps_local_obj_out_of_compile_items(self) -> None:
        with tempfile.TemporaryDirectory(prefix="chaos-managed-intermediate-") as temp_root:
            compile_identities = _msbuild_compile_identities(
                CONTRACTS_PROJECT_PATH,
                [f"-p:ChaosTempIntermediateRoot={Path(temp_root).as_posix()}/"],
            )

        self.assertFalse(
            any(identity.startswith("obj\\") or identity.startswith("obj/") for identity in compile_identities),
            compile_identities,
        )

    def test_subject_temp_intermediate_override_keeps_local_obj_out_of_compile_items(self) -> None:
        with tempfile.TemporaryDirectory(prefix="chaos-subject-intermediate-") as temp_root:
            intermediate_text = Path(temp_root).as_posix() + "/$(MSBuildProjectName)/"
            compile_identities = _msbuild_compile_identities(
                MAINLINE_FEATURE_PACK_PROJECT_PATH,
                [
                    f"-p:BaseIntermediateOutputPath={intermediate_text}",
                    f"-p:MSBuildProjectExtensionsPath={intermediate_text}",
                ],
            )

        self.assertFalse(
            any(identity.startswith("obj\\") or identity.startswith("obj/") for identity in compile_identities),
            compile_identities,
        )


if __name__ == "__main__":
    unittest.main()
