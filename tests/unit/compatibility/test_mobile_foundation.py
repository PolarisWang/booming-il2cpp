from __future__ import annotations

import json
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
STRATEGY_DOC_PATH = REPO_ROOT / "contracts" / "shared" / "v0" / "ios-hot-update-strategy.md"
IOS_COMPLIANCE_VALIDATOR_PATH = (
    REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate" / "IosComplianceValidator.cs"
)
PACKAGE_VALIDATOR_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate" / "PackageValidator.cs"
ANDROID_JNI_PATH = REPO_ROOT / "src" / "mobile" / "android" / "jni" / "mobile_host_jni.cpp"
IOS_BRIDGE_PATH = REPO_ROOT / "src" / "mobile" / "ios" / "bridge" / "mobile_host_bridge.mm"
ANDROID_CMAKE_PATH = REPO_ROOT / "src" / "mobile" / "android" / "CMakeLists.txt"
IOS_CMAKE_PATH = REPO_ROOT / "src" / "mobile" / "ios" / "CMakeLists.txt"
SMOKE_PROJECT_PATH = (
    REPO_ROOT / "tests" / "contracts" / "managed" / "ios-hot-update-compliance-smoke" / "IosHotUpdateComplianceSmoke.csproj"
)
MOBILE_BENCHMARK_SUBJECTS = [
    "BenchArithmetic",
    "BenchAllocation",
    "BenchDispatch",
    "BenchHotUpdateRoundtrip",
]
CANONICAL_BENCHMARK_SUBJECTS = [
    "SolutionCorePack",
    "HotUpdateHostPack",
    "MixedExecutionFeaturePack",
]
TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase8-mobile-foundation"


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


class Phase8MobileFoundationTests(unittest.TestCase):
    def test_ios_hot_update_strategy_doc_exists_and_covers_app_store_boundaries(self) -> None:
        self.assertTrue(STRATEGY_DOC_PATH.is_file(), msg=f"missing strategy doc: {STRATEGY_DOC_PATH}")

        strategy_source = STRATEGY_DOC_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "3.3.2",
            "2.5.2",
            "解释执行",
            "不得分发原生可执行代码",
            "IL bytecode",
        ]:
            self.assertIn(required_fragment, strategy_source)

    def test_ios_hot_update_compliance_smoke_covers_pass_restricted_api_native_code_and_oversized_cases(self) -> None:
        self.assertTrue(SMOKE_PROJECT_PATH.is_file(), msg=f"missing smoke project: {SMOKE_PROJECT_PATH}")

        build_root = TMP_ROOT / f"ios-compliance-{uuid.uuid4().hex}"
        build_root.mkdir(parents=True, exist_ok=True)
        base_output_arg = f"-p:BaseOutputPath={build_root}\\"

        run_checked(
            ["dotnet", "build", str(SMOKE_PROJECT_PATH), "-c", "Release", base_output_arg],
            cwd=REPO_ROOT,
        )

        compliant = run_checked(
            ["dotnet", "run", "--project", str(SMOKE_PROJECT_PATH), "-c", "Release", "--no-build", base_output_arg, "--", "compliant"],
            cwd=REPO_ROOT,
        )
        self.assertIn("ios-compliance=pass", compliant.stdout)

        native_code = run_checked(
            ["dotnet", "run", "--project", str(SMOKE_PROJECT_PATH), "-c", "Release", "--no-build", base_output_arg, "--", "native-code"],
            cwd=REPO_ROOT,
        )
        self.assertIn("ios-compliance=reject", native_code.stdout)
        self.assertIn(".dylib", native_code.stdout)

        restricted_api = run_checked(
            ["dotnet", "run", "--project", str(SMOKE_PROJECT_PATH), "-c", "Release", "--no-build", base_output_arg, "--", "restricted-api"],
            cwd=REPO_ROOT,
        )
        self.assertIn("ios-compliance=reject", restricted_api.stdout)
        self.assertIn("dlopen", restricted_api.stdout)

        oversized = run_checked(
            ["dotnet", "run", "--project", str(SMOKE_PROJECT_PATH), "-c", "Release", "--no-build", base_output_arg, "--", "oversized"],
            cwd=REPO_ROOT,
        )
        self.assertIn("ios-compliance=reject", oversized.stdout)
        self.assertIn("package size", oversized.stdout)

    def test_package_validator_mobile_entrypoint_references_ios_compliance_validator(self) -> None:
        self.assertTrue(IOS_COMPLIANCE_VALIDATOR_PATH.is_file(), msg=f"missing validator: {IOS_COMPLIANCE_VALIDATOR_PATH}")

        validator_source = IOS_COMPLIANCE_VALIDATOR_PATH.read_text(encoding="utf-8")
        package_validator_source = PACKAGE_VALIDATOR_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "class IosComplianceValidator",
            ".dylib",
            ".so",
            ".framework",
            "dlopen",
            "mmap",
            "MaxPackageBytes",
        ]:
            self.assertIn(required_fragment, validator_source)

        for required_fragment in [
            "ValidateIosDistributionCompliance",
            "IosComplianceValidator",
        ]:
            self.assertIn(required_fragment, package_validator_source)

    def test_mobile_hot_update_bridges_and_cmake_link_hot_update_runtime(self) -> None:
        android_jni_source = ANDROID_JNI_PATH.read_text(encoding="utf-8")
        ios_bridge_source = IOS_BRIDGE_PATH.read_text(encoding="utf-8")
        android_cmake_source = ANDROID_CMAKE_PATH.read_text(encoding="utf-8")
        ios_cmake_source = IOS_CMAKE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "Java_com_chaos_mobilehost_MainActivity_loadHotUpdatePackage",
            "Java_com_chaos_mobilehost_MainActivity_unloadHotUpdatePackage",
            "chaos::il2cpp::hot_update::LoadHotUpdatePackage",
            "chaos::il2cpp::hot_update::UnloadHotUpdatePackage",
        ]:
            self.assertIn(required_fragment, android_jni_source)

        for required_fragment in [
            "chaos_mobile_ios_load_hot_update_package",
            "chaos_mobile_ios_unload_hot_update_package",
            "chaos::il2cpp::hot_update::LoadHotUpdatePackage",
            "chaos::il2cpp::hot_update::UnloadHotUpdatePackage",
        ]:
            self.assertIn(required_fragment, ios_bridge_source)

        self.assertIn("chaos_hot_update", android_cmake_source)
        self.assertIn("chaos_hot_update", ios_cmake_source)

    def test_benchmark_manifests_declare_mobile_native_perf_surface(self) -> None:
        for subject_id in MOBILE_BENCHMARK_SUBJECTS:
            manifest_path = REPO_ROOT / "subjects" / subject_id / "subject.manifest.json"
            self.assertFalse(manifest_path.is_file(), msg=f"legacy mobile benchmark shell should be retired: {manifest_path}")

        for subject_id in CANONICAL_BENCHMARK_SUBJECTS:
            manifest_path = REPO_ROOT / "subjects" / subject_id / "subject.manifest.json"
            self.assertTrue(manifest_path.is_file(), msg=f"missing canonical manifest: {manifest_path}")
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))

            pipeline_ids = {
                str(pipeline.get("pipelineId") or "")
                for pipeline in list(manifest.get("executionPipelines") or [])
            }
            matrix_ids = {
                str(matrix.get("matrixId") or "")
                for matrix in list(manifest.get("environmentMatrices") or [])
            }

            self.assertNotIn("mobile-native-benchmark", pipeline_ids, msg=subject_id)
            self.assertNotIn("android-arm64-native-perf", matrix_ids, msg=subject_id)
            self.assertNotIn("ios-arm64-native-perf", matrix_ids, msg=subject_id)


if __name__ == "__main__":
    unittest.main()
