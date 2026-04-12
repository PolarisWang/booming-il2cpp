from __future__ import annotations

import json
import subprocess
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase9-hot-update-e2e"

HOT_UPDATE_ROOT = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate"
HOT_UPDATE_PROJECT_PATH = HOT_UPDATE_ROOT / "Chaos.IL2CPP.HotUpdate.csproj"
RUNTIME_MANAGER_PATH = HOT_UPDATE_ROOT / "RuntimeManager.cs"
METHOD_REGISTRY_PATH = HOT_UPDATE_ROOT / "HotUpdateMethodRegistry.cs"
BRIDGE_DISPATCHER_PATH = HOT_UPDATE_ROOT / "BridgeDispatcher.cs"
AUTO_BRIDGE_GENERATOR_PATH = HOT_UPDATE_ROOT / "AutoBridgeGenerator.cs"

INTERPRETER_ROOT = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Interpreter"
MANAGED_INTERPRETER_EXECUTOR_PATH = INTERPRETER_ROOT / "ManagedInterpreterExecutor.cs"
INTERPRETER_IR_PATH = INTERPRETER_ROOT / "InterpreterIR.cs"

NATIVE_HOT_UPDATE_ROOT = REPO_ROOT / "src" / "native" / "hot-update"
NATIVE_HOT_UPDATE_CMAKE_PATH = NATIVE_HOT_UPDATE_ROOT / "CMakeLists.txt"
NATIVE_HOT_UPDATE_HEADER_PATH = NATIVE_HOT_UPDATE_ROOT / "hot_update.h"
NATIVE_HOT_UPDATE_SOURCE_PATH = NATIVE_HOT_UPDATE_ROOT / "hot_update.cpp"
METHOD_REPLACEMENT_HEADER_PATH = NATIVE_HOT_UPDATE_ROOT / "method_replacement.h"
METHOD_REPLACEMENT_SOURCE_PATH = NATIVE_HOT_UPDATE_ROOT / "method_replacement.cpp"
BOOTSTRAP_SOURCE_PATH = REPO_ROOT / "src" / "native" / "bootstrap" / "bootstrap.cpp"
NATIVE_INTERPRETER_HEADER_PATH = REPO_ROOT / "src" / "native" / "interpreter" / "interpreter_vm.h"
NATIVE_INTERPRETER_SOURCE_PATH = REPO_ROOT / "src" / "native" / "interpreter" / "interpreter_vm.cpp"
ROOT_CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"

METHOD_REPLACEMENT_SMOKE_CMAKE_PATH = REPO_ROOT / "tests" / "contracts" / "native" / "hot-update" / "CMakeLists.txt"
METHOD_REPLACEMENT_SMOKE_SOURCE_PATH = REPO_ROOT / "tests" / "contracts" / "native" / "hot-update" / "method_replacement_smoke.cpp"

HOT_UPDATE_SKELETON_ROOT = REPO_ROOT / "subjects" / "HotUpdateSkeletonProof"
HOT_UPDATE_SKELETON_PROJECT_PATH = HOT_UPDATE_SKELETON_ROOT / "source" / "HotUpdateSkeletonProof.csproj"
HOT_UPDATE_SKELETON_PROGRAM_PATH = HOT_UPDATE_SKELETON_ROOT / "source" / "Program.cs"

METHOD_REPLACEMENT_PROOF_ROOT = REPO_ROOT / "subjects" / "MethodReplacementProof"
METHOD_REPLACEMENT_PROOF_PROJECT_PATH = METHOD_REPLACEMENT_PROOF_ROOT / "source" / "MethodReplacementProof.csproj"
METHOD_REPLACEMENT_PROOF_PROGRAM_PATH = METHOD_REPLACEMENT_PROOF_ROOT / "source" / "Program.cs"

AUTO_BRIDGE_PROOF_ROOT = REPO_ROOT / "subjects" / "AutoBridgeProof"
AUTO_BRIDGE_PROOF_PROJECT_PATH = AUTO_BRIDGE_PROOF_ROOT / "source" / "AutoBridgeProof.csproj"
AUTO_BRIDGE_PROOF_PROGRAM_PATH = AUTO_BRIDGE_PROOF_ROOT / "source" / "Program.cs"

VERSION_ROLLBACK_PROOF_ROOT = REPO_ROOT / "subjects" / "VersionRollbackProof"
VERSION_ROLLBACK_PROOF_PROJECT_PATH = VERSION_ROLLBACK_PROOF_ROOT / "source" / "VersionRollbackProof.csproj"
VERSION_ROLLBACK_PROOF_PROGRAM_PATH = VERSION_ROLLBACK_PROOF_ROOT / "source" / "Program.cs"

BENCH_LOAD_ROOT = REPO_ROOT / "subjects" / "BenchHotUpdateLoad"
BENCH_LOAD_PROJECT_PATH = BENCH_LOAD_ROOT / "source" / "BenchHotUpdateLoad.csproj"
BENCH_DISPATCH_ROOT = REPO_ROOT / "subjects" / "BenchHotUpdateDispatch"
BENCH_DISPATCH_PROJECT_PATH = BENCH_DISPATCH_ROOT / "source" / "BenchHotUpdateDispatch.csproj"
BENCH_ROUNDTRIP_ROOT = REPO_ROOT / "subjects" / "BenchHotUpdateRoundtrip"
BENCH_ROUNDTRIP_PROJECT_PATH = BENCH_ROUNDTRIP_ROOT / "source" / "BenchHotUpdateRoundtrip.csproj"


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


def parse_last_json_line(output: str) -> dict[str, object]:
    lines = [line.strip() for line in output.splitlines() if line.strip()]
    if not lines:
        raise AssertionError("command produced no output")
    return json.loads(lines[-1])


class Phase9HotUpdateE2ETests(unittest.TestCase):
    def test_runtime_manager_and_bridge_surface_support_generic_hot_update_e2e(self) -> None:
        self.assertTrue(HOT_UPDATE_PROJECT_PATH.is_file(), msg=f"missing hot update project: {HOT_UPDATE_PROJECT_PATH}")
        self.assertTrue(RUNTIME_MANAGER_PATH.is_file(), msg=f"missing runtime manager: {RUNTIME_MANAGER_PATH}")
        self.assertTrue(METHOD_REGISTRY_PATH.is_file(), msg=f"missing method registry: {METHOD_REGISTRY_PATH}")
        self.assertTrue(BRIDGE_DISPATCHER_PATH.is_file(), msg=f"missing bridge dispatcher: {BRIDGE_DISPATCHER_PATH}")
        self.assertTrue(AUTO_BRIDGE_GENERATOR_PATH.is_file(), msg=f"missing auto bridge generator: {AUTO_BRIDGE_GENERATOR_PATH}")

        project_source = HOT_UPDATE_PROJECT_PATH.read_text(encoding="utf-8")
        runtime_manager_source = RUNTIME_MANAGER_PATH.read_text(encoding="utf-8")
        registry_source = METHOD_REGISTRY_PATH.read_text(encoding="utf-8")
        bridge_dispatcher_source = BRIDGE_DISPATCHER_PATH.read_text(encoding="utf-8")
        auto_bridge_source = AUTO_BRIDGE_GENERATOR_PATH.read_text(encoding="utf-8")
        interpreter_executor_source = MANAGED_INTERPRETER_EXECUTOR_PATH.read_text(encoding="utf-8")
        interpreter_ir_source = INTERPRETER_IR_PATH.read_text(encoding="utf-8")

        self.assertIn("Chaos.IL2CPP.Contracts", project_source)

        for required_fragment in [
            "public bool LoadPackage(",
            "string packageRootPath",
            "string currentAotVersion",
            "CanRollback",
            "Rollback()",
            "GetActivePatches()",
            "DispatchInt32(",
            "DispatchInt32Unary(",
            "PackageReader.ReadFromDirectory",
        ]:
            self.assertIn(required_fragment, runtime_manager_source)

        for required_fragment in [
            "RegisterMethod",
            "TryDispatch",
            "IReadOnlyList<object?>",
        ]:
            self.assertIn(required_fragment, registry_source)

        for required_fragment in [
            "RegisterAutoGenerated",
            "Dispatch(string bridgeId",
            "IReadOnlyList<object?>",
        ]:
            self.assertIn(required_fragment, bridge_dispatcher_source)

        for required_fragment in [
            "class AutoBridgeGenerator",
            "BridgePlan Generate(",
            "ManagedMethodModel",
            "NotSupportedException",
        ]:
            self.assertIn(required_fragment, auto_bridge_source)

        for required_fragment in [
            "public object? Execute(",
            "ExecuteInt32(",
        ]:
            self.assertIn(required_fragment, interpreter_executor_source)

        for required_fragment in [
            "Float32",
            "Float64",
            "Object",
            "Array",
            "LdcR4",
            "LdcR8",
            "Conv_I4",
            "Conv_R8",
        ]:
            self.assertIn(required_fragment, interpreter_ir_source)

    def test_native_hot_update_surface_exposes_package_loading_method_replacement_and_extended_interpreter_values(self) -> None:
        self.assertTrue(NATIVE_HOT_UPDATE_CMAKE_PATH.is_file(), msg=f"missing native hot update cmake: {NATIVE_HOT_UPDATE_CMAKE_PATH}")
        self.assertTrue(NATIVE_HOT_UPDATE_HEADER_PATH.is_file(), msg=f"missing native hot update header: {NATIVE_HOT_UPDATE_HEADER_PATH}")
        self.assertTrue(NATIVE_HOT_UPDATE_SOURCE_PATH.is_file(), msg=f"missing native hot update source: {NATIVE_HOT_UPDATE_SOURCE_PATH}")
        self.assertTrue(METHOD_REPLACEMENT_HEADER_PATH.is_file(), msg=f"missing method replacement header: {METHOD_REPLACEMENT_HEADER_PATH}")
        self.assertTrue(METHOD_REPLACEMENT_SOURCE_PATH.is_file(), msg=f"missing method replacement source: {METHOD_REPLACEMENT_SOURCE_PATH}")

        root_cmake_source = ROOT_CMAKE_PATH.read_text(encoding="utf-8")
        hot_update_cmake_source = NATIVE_HOT_UPDATE_CMAKE_PATH.read_text(encoding="utf-8")
        hot_update_header_source = NATIVE_HOT_UPDATE_HEADER_PATH.read_text(encoding="utf-8")
        method_replacement_header_source = METHOD_REPLACEMENT_HEADER_PATH.read_text(encoding="utf-8")
        bootstrap_source = BOOTSTRAP_SOURCE_PATH.read_text(encoding="utf-8")
        interpreter_header_source = NATIVE_INTERPRETER_HEADER_PATH.read_text(encoding="utf-8")
        interpreter_source = NATIVE_INTERPRETER_SOURCE_PATH.read_text(encoding="utf-8")

        self.assertIn("add_subdirectory(tests/contracts/native/hot-update)", root_cmake_source)
        self.assertIn("method_replacement.cpp", hot_update_cmake_source)

        for required_fragment in [
            "struct HotUpdatePackageHandle",
            "LoadHotUpdatePackage",
            "UnloadHotUpdatePackage",
            "package_id",
            "target_aot_version",
        ]:
            self.assertIn(required_fragment, hot_update_header_source)

        for required_fragment in [
            "struct MethodReplacementEntry",
            "Register(",
            "Revert(",
            "RevertAll(",
            "Resolve(",
            "ActiveCount(",
        ]:
            self.assertIn(required_fragment, method_replacement_header_source)

        self.assertIn("method_replacement::Resolve", bootstrap_source)

        for required_fragment in [
            "enum class ValueTag",
            "struct InterpreterValue",
            "Float32",
            "Float64",
            "ObjectRef",
            "Null",
            "LdcR4",
            "LdcR8",
            "LdStr",
            "LdNull",
            "NewObj",
            "NewArr",
            "LdFld",
            "StFld",
            "LdElem",
            "StElem",
            "LdLen",
            "Box",
            "Unbox",
            "Call",
            "CallVirt",
            "CallBridge",
            "Conv_I4",
            "Conv_I8",
            "Conv_R4",
            "Conv_R8",
            "EndFilter",
        ]:
            self.assertIn(required_fragment, interpreter_header_source)

        for required_fragment in [
            "case IROpCode::LdcR4",
            "case IROpCode::LdcR8",
            "case IROpCode::LdStr",
            "case IROpCode::LdNull",
            "case IROpCode::NewObj",
            "case IROpCode::NewArr",
            "case IROpCode::LdFld",
            "case IROpCode::StFld",
            "case IROpCode::LdElem",
            "case IROpCode::StElem",
            "case IROpCode::LdLen",
            "case IROpCode::Box",
            "case IROpCode::Unbox",
            "case IROpCode::Call",
            "case IROpCode::CallVirt",
            "case IROpCode::CallBridge",
            "case IROpCode::Conv_I4",
            "case IROpCode::Conv_I8",
            "case IROpCode::Conv_R4",
            "case IROpCode::Conv_R8",
            "case IROpCode::EndFilter",
        ]:
            self.assertIn(required_fragment, interpreter_source)

    def test_hot_update_skeleton_proof_runs_real_package_load_path(self) -> None:
        self.assertTrue(HOT_UPDATE_SKELETON_PROJECT_PATH.is_file(), msg=f"missing project: {HOT_UPDATE_SKELETON_PROJECT_PATH}")
        self.assertTrue(HOT_UPDATE_SKELETON_PROGRAM_PATH.is_file(), msg=f"missing program: {HOT_UPDATE_SKELETON_PROGRAM_PATH}")

        completed = run_checked(
            ["dotnet", "run", "--project", str(HOT_UPDATE_SKELETON_PROJECT_PATH), "-c", "Release", "--"],
            cwd=REPO_ROOT,
        )
        output = completed.stdout

        for required_fragment in [
            "hot-update-skeleton-load=true",
            "hot-update-skeleton-dispatch=7",
            "hot-update-skeleton-mode=mixed",
            "hot-update-skeleton-after-rollback=3",
        ]:
            self.assertIn(required_fragment, output)

    def test_method_replacement_proof_and_native_smoke_run_replace_call_revert_cycle(self) -> None:
        self.assertTrue(METHOD_REPLACEMENT_PROOF_PROJECT_PATH.is_file(), msg=f"missing project: {METHOD_REPLACEMENT_PROOF_PROJECT_PATH}")
        self.assertTrue(METHOD_REPLACEMENT_PROOF_PROGRAM_PATH.is_file(), msg=f"missing program: {METHOD_REPLACEMENT_PROOF_PROGRAM_PATH}")
        self.assertTrue(METHOD_REPLACEMENT_SMOKE_CMAKE_PATH.is_file(), msg=f"missing native smoke cmake: {METHOD_REPLACEMENT_SMOKE_CMAKE_PATH}")
        self.assertTrue(METHOD_REPLACEMENT_SMOKE_SOURCE_PATH.is_file(), msg=f"missing native smoke source: {METHOD_REPLACEMENT_SMOKE_SOURCE_PATH}")

        managed_completed = run_checked(
            ["dotnet", "run", "--project", str(METHOD_REPLACEMENT_PROOF_PROJECT_PATH), "-c", "Release", "--"],
            cwd=REPO_ROOT,
        )
        managed_output = managed_completed.stdout

        for required_fragment in [
            "method-replacement-before=hello from AOT",
            "method-replacement-after=hello from interpreter",
            "method-replacement-reverted=hello from AOT",
        ]:
            self.assertIn(required_fragment, managed_output)

        build_root = TEST_TMP_ROOT / "native-method-replacement-smoke"
        build_root.mkdir(parents=True, exist_ok=True)
        run_checked(
            [
                "cmake",
                "-S",
                ".",
                "-B",
                str(build_root),
                "-G",
                "Visual Studio 17 2022",
                "-DROADMAP0_PRESET_TARGET=windows-x64-reference",
            ],
            cwd=REPO_ROOT,
        )
        run_checked(
            [
                "cmake",
                "--build",
                str(build_root),
                "--config",
                "Release",
                "--target",
                "chaos_method_replacement_smoke",
            ],
            cwd=REPO_ROOT,
        )
        executable_path = build_root / "tests" / "contracts" / "native" / "hot-update" / "Release" / "chaos_method_replacement_smoke.exe"
        self.assertTrue(executable_path.is_file(), msg=f"missing smoke executable: {executable_path}")

        native_completed = run_checked([str(executable_path)], cwd=REPO_ROOT)
        native_output = native_completed.stdout
        for required_fragment in [
            "method-replacement-active=1",
            "method-replacement-resolve=ok",
            "method-replacement-after-revert=0",
        ]:
            self.assertIn(required_fragment, native_output)

    def test_auto_bridge_and_version_rollback_proofs_cover_dynamic_dispatch_and_history(self) -> None:
        self.assertTrue(AUTO_BRIDGE_PROOF_PROJECT_PATH.is_file(), msg=f"missing project: {AUTO_BRIDGE_PROOF_PROJECT_PATH}")
        self.assertTrue(AUTO_BRIDGE_PROOF_PROGRAM_PATH.is_file(), msg=f"missing program: {AUTO_BRIDGE_PROOF_PROGRAM_PATH}")
        self.assertTrue(VERSION_ROLLBACK_PROOF_PROJECT_PATH.is_file(), msg=f"missing project: {VERSION_ROLLBACK_PROOF_PROJECT_PATH}")
        self.assertTrue(VERSION_ROLLBACK_PROOF_PROGRAM_PATH.is_file(), msg=f"missing program: {VERSION_ROLLBACK_PROOF_PROGRAM_PATH}")

        auto_bridge_completed = run_checked(
            ["dotnet", "run", "--project", str(AUTO_BRIDGE_PROOF_PROJECT_PATH), "-c", "Release", "--"],
            cwd=REPO_ROOT,
        )
        auto_bridge_output = auto_bridge_completed.stdout
        for required_fragment in [
            "auto-bridge-void=ok",
            "auto-bridge-int-unary=42",
            "auto-bridge-int-binary=42",
            "auto-bridge-string-unary=bridge-chaos",
            "auto-bridge-object-unary=payload",
            "auto-bridge-unsupported=unsupported signature",
        ]:
            self.assertIn(required_fragment, auto_bridge_output)

        rollback_completed = run_checked(
            ["dotnet", "run", "--project", str(VERSION_ROLLBACK_PROOF_PROJECT_PATH), "-c", "Release", "--"],
            cwd=REPO_ROOT,
        )
        rollback_output = rollback_completed.stdout
        for required_fragment in [
            "version-rollback-v1=11",
            "version-rollback-v2=22",
            "version-rollback-back-v1=11",
            "version-rollback-back-aot=5",
            "version-rollback-compatibility=rejected",
        ]:
            self.assertIn(required_fragment, rollback_output)

    def test_hot_update_benchmark_subjects_keep_workload_contract_and_build_serially(self) -> None:
        benchmark_projects = [
            (BENCH_LOAD_ROOT, BENCH_LOAD_PROJECT_PATH),
            (BENCH_DISPATCH_ROOT, BENCH_DISPATCH_PROJECT_PATH),
            (BENCH_ROUNDTRIP_ROOT, BENCH_ROUNDTRIP_PROJECT_PATH),
        ]

        for root, project_path in benchmark_projects:
            self.assertTrue(root.is_dir(), msg=f"missing benchmark root: {root}")
            self.assertTrue(project_path.is_file(), msg=f"missing benchmark project: {project_path}")
            program_path = root / "source" / "Program.cs"
            self.assertTrue(program_path.is_file(), msg=f"missing benchmark program: {program_path}")

            program_source = program_path.read_text(encoding="utf-8")
            self.assertIn("RunWorkload", program_source)
            self.assertNotIn("Stopwatch", program_source)
            self.assertNotIn("elapsedMilliseconds", program_source)
            self.assertNotIn("opsPerSecond", program_source)
            self.assertNotIn("roundtripsPerSecond", program_source)
            self.assertNotIn("loadDurationMs", program_source)
            self.assertNotIn("Console.WriteLine", program_source)

            run_checked(
                ["dotnet", "build", str(project_path), "-c", "Release", "-m:1"],
                cwd=REPO_ROOT,
            )


if __name__ == "__main__":
    unittest.main()
