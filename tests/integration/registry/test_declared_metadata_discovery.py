from __future__ import annotations

import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import load_module


REPO_ROOT = Path(__file__).resolve().parents[3]
COMPILED_CATALOG_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "compiled_catalog.py"
FIXTURE_ROOT = REPO_ROOT / "tests" / "fixtures" / "contracts" / "declared-metadata"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "declared-metadata-discovery"
MAINLINE_FEATURE_PACK_PROJECT_PATH = REPO_ROOT / "subjects" / "MainlineFeaturePack" / "source" / "MainlineFeaturePack.csproj"
BENCHMARK_FEATURE_PACK_PROJECT_PATH = REPO_ROOT / "subjects" / "PerformanceFeaturePack" / "source" / "PerformanceFeaturePack.csproj"
HOT_UPDATE_HOST_PACK_PROJECT_PATH = REPO_ROOT / "subjects" / "HotUpdateHostPack" / "source" / "HotUpdateHostPack.csproj"
MIXED_EXECUTION_FEATURE_PACK_PROJECT_PATH = REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "MixedExecutionFeaturePack.csproj"


def dotnet_intermediate_args(intermediate_root: Path) -> list[str]:
    return [
        f"-p:ChaosTempIntermediateRoot={intermediate_root.as_posix()}/",
    ]


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


class DeclaredMetadataDiscoveryTests(unittest.TestCase):
    maxDiff = None

    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)
        cls.runtime_only_dll = cls._build_fixture("RuntimeOnly")
        cls.framework_no_entries_dll = cls._build_fixture("FrameworkNoEntries")
        cls.declared_entries_dll = cls._build_fixture("DeclaredEntries")

    @classmethod
    def _build_fixture(cls, fixture_name: str) -> Path:
        project_path = FIXTURE_ROOT / fixture_name / f"{fixture_name}.csproj"
        run_checked(
            [
                "dotnet",
                "build",
                str(project_path),
                "-c",
                "Release",
                "-m:1",
            ],
            cwd=REPO_ROOT,
        )
        dll_path = project_path.parent / "bin" / "Release" / "net8.0" / f"{fixture_name}.dll"
        if not dll_path.is_file():
            raise AssertionError(f"missing compiled fixture dll: {dll_path}")
        return dll_path

    def _copy_fixture_output(self, dll_path: Path) -> Path:
        copied_root = TEST_TMP_ROOT / "copied-output" / f"{dll_path.stem}-{uuid.uuid4().hex}"
        shutil.copytree(dll_path.parent, copied_root)
        return copied_root / dll_path.name

    def test_compiled_catalog_distinguishes_runtime_only_framework_only_and_declared_entries(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_catalog_state_matrix")

        runtime_only = catalog_module.build_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="RuntimeOnlySubject",
            declaration_mode="auto",
            assembly_paths=[self.runtime_only_dll],
        )
        framework_only = catalog_module.build_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="FrameworkOnlySubject",
            declaration_mode="auto",
            assembly_paths=[self.framework_no_entries_dll],
        )
        declared = catalog_module.build_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="DeclaredEntriesSubject",
            declaration_mode="auto",
            assembly_paths=[self.declared_entries_dll],
        )

        self.assertEqual(
            {
                "subjectId": "RuntimeOnlySubject",
                "frameworkReferenced": False,
                "subjectKind": "runtime-only",
                "warningCodes": [],
                "declaredUnitTests": [],
                "declaredBenchmarks": [],
            },
            runtime_only,
        )
        self.assertEqual(
            {
                "subjectId": "FrameworkOnlySubject",
                "frameworkReferenced": True,
                "subjectKind": "runtime-only",
                "warningCodes": ["framework-without-declarations"],
                "declaredUnitTests": [],
                "declaredBenchmarks": [],
            },
            framework_only,
        )
        self.assertEqual("DeclaredEntriesSubject", declared["subjectId"])
        self.assertTrue(declared["frameworkReferenced"])
        self.assertEqual("declared-test", declared["subjectKind"])
        self.assertEqual([], declared["warningCodes"])
        self.assertEqual(1, len(declared["declaredUnitTests"]))
        self.assertEqual(1, len(declared["declaredBenchmarks"]))

    def test_compiled_catalog_emits_stable_ids_and_compact_numeric_metadata(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_catalog_payload")

        catalog = catalog_module.build_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="DeclaredEntriesSubject",
            declaration_mode="require",
            assembly_paths=[self.declared_entries_dll],
        )

        unit_entry = catalog["declaredUnitTests"][0]
        benchmark_entry = catalog["declaredBenchmarks"][0]

        self.assertEqual(
            "DeclaredEntriesSubject::DeclaredEntries::DeclaredEntries.DeclaredProofs::VerifyOutput()",
            unit_entry["stableId"],
        )
        self.assertEqual("runtime-contract", unit_entry["alias"])
        self.assertEqual("DeclaredEntries", unit_entry["assemblyName"])
        self.assertEqual("DeclaredEntries.DeclaredProofs", unit_entry["declaringType"])
        self.assertEqual("VerifyOutput", unit_entry["methodName"])
        self.assertEqual("VerifyOutput()", unit_entry["methodSignature"])
        self.assertEqual(1, unit_entry["category"])
        self.assertEqual(2, unit_entry["requires"])
        self.assertEqual(1, unit_entry["evidence"])
        self.assertEqual(3, unit_entry["priority"])

        self.assertEqual(
            "DeclaredEntriesSubject::DeclaredEntries::DeclaredEntries.DeclaredProofs::BenchmarkDispatch()",
            benchmark_entry["stableId"],
        )
        self.assertEqual("dispatch-bench", benchmark_entry["alias"])
        self.assertEqual("DeclaredEntries", benchmark_entry["assemblyName"])
        self.assertEqual("DeclaredEntries.DeclaredProofs", benchmark_entry["declaringType"])
        self.assertEqual("BenchmarkDispatch", benchmark_entry["methodName"])
        self.assertEqual("BenchmarkDispatch()", benchmark_entry["methodSignature"])
        self.assertEqual(1, benchmark_entry["category"])
        self.assertEqual(3, benchmark_entry["metrics"])
        self.assertEqual(1, benchmark_entry["requires"])
        self.assertEqual(2, benchmark_entry["warmupCount"])
        self.assertEqual(12, benchmark_entry["iterationCount"])
        self.assertEqual(100, benchmark_entry["invocationCount"])

    def test_compiled_catalog_scans_metadata_without_loading_framework_binary(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_catalog_metadata_only")

        isolated_dll_path = self._copy_fixture_output(self.declared_entries_dll)
        framework_dll_path = isolated_dll_path.parent / "Chaos.TestFramework.dll"
        self.assertTrue(framework_dll_path.is_file(), msg=f"missing copied framework dll: {framework_dll_path}")
        framework_dll_path.unlink()

        catalog = catalog_module.build_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="DeclaredEntriesSubject",
            declaration_mode="require",
            assembly_paths=[isolated_dll_path],
        )

        self.assertEqual(1, len(catalog["declaredUnitTests"]))
        self.assertEqual(1, len(catalog["declaredBenchmarks"]))

    def test_compiled_catalog_skips_discovery_when_declaration_mode_is_none(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_catalog_mode_none")

        catalog = catalog_module.build_declared_test_catalog(
            repo_root=REPO_ROOT,
            subject_id="CompatibilityShellSubject",
            declaration_mode="none",
            assembly_paths=[self.declared_entries_dll],
        )

        self.assertEqual(
            {
                "subjectId": "CompatibilityShellSubject",
                "frameworkReferenced": False,
                "subjectKind": "runtime-only",
                "warningCodes": [],
                "declaredUnitTests": [],
                "declaredBenchmarks": [],
            },
            catalog,
        )

    def test_mainline_feature_pack_discovers_declared_unit_entries_from_real_subject_output(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_mainline_feature_pack")

        build_root = TEST_TMP_ROOT / "mainline-feature-pack" / uuid.uuid4().hex
        build_output_root = build_root / "build"
        intermediate_root = build_root / "obj"
        build_root.mkdir(parents=True, exist_ok=True)
        try:
            run_checked(
                [
                    "dotnet",
                    "build",
                    str(MAINLINE_FEATURE_PACK_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-o",
                    str(build_output_root),
                    "-m:1",
                    *dotnet_intermediate_args(intermediate_root),
                ],
                cwd=REPO_ROOT,
            )
            assembly_path = build_output_root / "MainlineFeaturePack.dll"
            self.assertTrue(assembly_path.is_file(), msg=f"missing compiled subject dll: {assembly_path}")

            catalog = catalog_module.build_declared_test_catalog(
                repo_root=REPO_ROOT,
                subject_id="MainlineFeaturePack",
                declaration_mode="require",
                assembly_paths=[assembly_path],
            )

            self.assertTrue(catalog["frameworkReferenced"])
            self.assertEqual("declared-test", catalog["subjectKind"])
            self.assertEqual([], catalog["warningCodes"])
            self.assertEqual([], catalog["declaredBenchmarks"])

            unit_entries = {
                entry["declaringType"]: entry
                for entry in catalog["declaredUnitTests"]
            }
            self.assertEqual(
                {
                    "MainlineFeaturePack.ProofEntry",
                    "MainlineFeaturePack.ArrayOpsProofEntry",
                    "MainlineFeaturePack.AsyncAwaitProofEntry",
                    "MainlineFeaturePack.BitwiseOpsProofEntry",
                    "MainlineFeaturePack.BranchOpsProofEntry",
                    "MainlineFeaturePack.ConversionOpsProofEntry",
                    "MainlineFeaturePack.InterfaceDispatchProofEntry",
                    "MainlineFeaturePack.VTableDispatchProofEntry",
                    "MainlineFeaturePack.LinkerStrippingProofEntry",
                    "MainlineFeaturePack.MarshalingProofEntry",
                    "MainlineFeaturePack.CrossBoundaryExceptionProofEntry",
                    "MainlineFeaturePack.GenericCollectionProofEntry",
                    "MainlineFeaturePack.ObjectOpsProofEntry",
                    "MainlineFeaturePack.OverflowOpsProofEntry",
                    "MainlineFeaturePack.ThreadingProofEntry",
                    "MainlineFeaturePack.NestedExceptionProofEntry",
                    "MainlineFeaturePack.DelegateChainProofEntry",
                    "MainlineFeaturePack.DispatchProofEntry",
                    "MainlineFeaturePack.GenericLayoutProofEntry",
                    "MainlineFeaturePack.ArrayBoxingProofEntry",
                    "MainlineFeaturePack.DelegateProofEntry",
                    "MainlineFeaturePack.ExceptionProofEntry",
                    "MainlineFeaturePack.ReflectionInteropClosureEntry",
                },
                set(unit_entries),
            )
            self.assertEqual("mainline-proof", unit_entries["MainlineFeaturePack.ProofEntry"]["alias"])
            self.assertEqual("array-ops-proof", unit_entries["MainlineFeaturePack.ArrayOpsProofEntry"]["alias"])
            self.assertEqual("async-await-proof", unit_entries["MainlineFeaturePack.AsyncAwaitProofEntry"]["alias"])
            self.assertEqual("bitwise-ops-proof", unit_entries["MainlineFeaturePack.BitwiseOpsProofEntry"]["alias"])
            self.assertEqual("interface-dispatch-proof", unit_entries["MainlineFeaturePack.InterfaceDispatchProofEntry"]["alias"])
            self.assertEqual("vtable-dispatch-proof", unit_entries["MainlineFeaturePack.VTableDispatchProofEntry"]["alias"])
            self.assertEqual("linker-stripping-proof", unit_entries["MainlineFeaturePack.LinkerStrippingProofEntry"]["alias"])
            self.assertEqual("marshaling-proof", unit_entries["MainlineFeaturePack.MarshalingProofEntry"]["alias"])
            self.assertEqual("cross-boundary-exception-proof", unit_entries["MainlineFeaturePack.CrossBoundaryExceptionProofEntry"]["alias"])
            self.assertEqual("generic-collection-proof", unit_entries["MainlineFeaturePack.GenericCollectionProofEntry"]["alias"])
            self.assertEqual("threading-proof", unit_entries["MainlineFeaturePack.ThreadingProofEntry"]["alias"])
            self.assertEqual("nested-exception-proof", unit_entries["MainlineFeaturePack.NestedExceptionProofEntry"]["alias"])
            self.assertEqual("delegate-chain-proof", unit_entries["MainlineFeaturePack.DelegateChainProofEntry"]["alias"])
            self.assertEqual("dispatch-proof", unit_entries["MainlineFeaturePack.DispatchProofEntry"]["alias"])
            self.assertEqual(1, unit_entries["MainlineFeaturePack.GenericCollectionProofEntry"]["requires"])
            self.assertEqual(8, unit_entries["MainlineFeaturePack.CrossBoundaryExceptionProofEntry"]["requires"])
            self.assertEqual(4, unit_entries["MainlineFeaturePack.DelegateChainProofEntry"]["requires"])
            self.assertEqual(4, unit_entries["MainlineFeaturePack.DelegateProofEntry"]["requires"])
            self.assertEqual(8, unit_entries["MainlineFeaturePack.ExceptionProofEntry"]["requires"])
            self.assertEqual(18, unit_entries["MainlineFeaturePack.ReflectionInteropClosureEntry"]["requires"])
        finally:
            shutil.rmtree(build_root, ignore_errors=True)

    def test_benchmark_feature_pack_discovers_declared_benchmark_entries_from_real_subject_output(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_benchmark_feature_pack")

        build_root = TEST_TMP_ROOT / "benchmark-feature-pack" / uuid.uuid4().hex
        build_output_root = build_root / "build"
        intermediate_root = build_root / "obj"
        build_root.mkdir(parents=True, exist_ok=True)
        try:
            run_checked(
                [
                    "dotnet",
                    "build",
                    str(BENCHMARK_FEATURE_PACK_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-o",
                    str(build_output_root),
                    "-m:1",
                    *dotnet_intermediate_args(intermediate_root),
                ],
                cwd=REPO_ROOT,
            )
            assembly_path = build_output_root / "PerformanceFeaturePack.dll"
            self.assertTrue(assembly_path.is_file(), msg=f"missing compiled subject dll: {assembly_path}")

            catalog = catalog_module.build_declared_test_catalog(
                repo_root=REPO_ROOT,
                subject_id="PerformanceFeaturePack",
                declaration_mode="require",
                assembly_paths=[assembly_path],
            )

            self.assertTrue(catalog["frameworkReferenced"])
            self.assertEqual("declared-test", catalog["subjectKind"])
            self.assertEqual([], catalog["warningCodes"])
            self.assertEqual([], catalog["declaredUnitTests"])

            benchmark_entries = {
                entry["declaringType"]: entry
                for entry in catalog["declaredBenchmarks"]
            }
            self.assertEqual(
                {
                    "PerformanceFeaturePack.ArithmeticBenchmarkEntry",
                    "PerformanceFeaturePack.AllocationBenchmarkEntry",
                    "PerformanceFeaturePack.DispatchBenchmarkEntry",
                    "PerformanceFeaturePack.GenericBenchmarkEntry",
                },
                set(benchmark_entries),
            )
            self.assertEqual("arithmetic-bench", benchmark_entries["PerformanceFeaturePack.ArithmeticBenchmarkEntry"]["alias"])
            self.assertEqual("allocation-bench", benchmark_entries["PerformanceFeaturePack.AllocationBenchmarkEntry"]["alias"])
            self.assertEqual("dispatch-bench", benchmark_entries["PerformanceFeaturePack.DispatchBenchmarkEntry"]["alias"])
            self.assertEqual("generic-bench", benchmark_entries["PerformanceFeaturePack.GenericBenchmarkEntry"]["alias"])
            self.assertEqual(1, benchmark_entries["PerformanceFeaturePack.ArithmeticBenchmarkEntry"]["category"])
            self.assertEqual(3, benchmark_entries["PerformanceFeaturePack.AllocationBenchmarkEntry"]["category"])
            self.assertEqual(1, benchmark_entries["PerformanceFeaturePack.ArithmeticBenchmarkEntry"]["metrics"])
            self.assertEqual(3, benchmark_entries["PerformanceFeaturePack.AllocationBenchmarkEntry"]["metrics"])
            self.assertEqual(1, benchmark_entries["PerformanceFeaturePack.GenericBenchmarkEntry"]["requires"])
        finally:
            shutil.rmtree(build_root, ignore_errors=True)

    def test_hot_update_host_pack_discovers_declared_unit_and_benchmark_entries_from_real_subject_output(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_hot_update_host_pack")

        build_root = TEST_TMP_ROOT / "hot-update-host-pack" / uuid.uuid4().hex
        build_output_root = build_root / "build"
        intermediate_root = build_root / "obj"
        build_root.mkdir(parents=True, exist_ok=True)
        try:
            run_checked(
                [
                    "dotnet",
                    "build",
                    str(HOT_UPDATE_HOST_PACK_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-o",
                    str(build_output_root),
                    "-m:1",
                    *dotnet_intermediate_args(intermediate_root),
                ],
                cwd=REPO_ROOT,
            )
            assembly_path = build_output_root / "HotUpdateHostPack.dll"
            self.assertTrue(assembly_path.is_file(), msg=f"missing compiled subject dll: {assembly_path}")

            catalog = catalog_module.build_declared_test_catalog(
                repo_root=REPO_ROOT,
                subject_id="HotUpdateHostPack",
                declaration_mode="require",
                assembly_paths=[assembly_path],
            )

            self.assertTrue(catalog["frameworkReferenced"])
            self.assertEqual("declared-test", catalog["subjectKind"])
            self.assertEqual([], catalog["warningCodes"])

            unit_entries = {
                entry["declaringType"]: entry
                for entry in catalog["declaredUnitTests"]
            }
            benchmark_entries = {
                entry["declaringType"]: entry
                for entry in catalog["declaredBenchmarks"]
            }

            self.assertEqual(
                {
                    "HotUpdateHostPack.HotUpdateSkeletonProofEntry",
                    "HotUpdateHostPack.MethodReplacementProofEntry",
                    "HotUpdateHostPack.VersionRollbackProofEntry",
                },
                set(unit_entries),
            )
            self.assertEqual(
                {
                    "HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry",
                    "HotUpdateHostPack.HotUpdateLoadBenchmarkEntry",
                    "HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry",
                },
                set(benchmark_entries),
            )
            self.assertEqual("hot-update-skeleton-proof", unit_entries["HotUpdateHostPack.HotUpdateSkeletonProofEntry"]["alias"])
            self.assertEqual("method-replacement-proof", unit_entries["HotUpdateHostPack.MethodReplacementProofEntry"]["alias"])
            self.assertEqual("version-rollback-proof", unit_entries["HotUpdateHostPack.VersionRollbackProofEntry"]["alias"])
            self.assertEqual("hot-update-dispatch-bench", benchmark_entries["HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry"]["alias"])
            self.assertEqual("hot-update-load-bench", benchmark_entries["HotUpdateHostPack.HotUpdateLoadBenchmarkEntry"]["alias"])
            self.assertEqual("hot-update-roundtrip-bench", benchmark_entries["HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry"]["alias"])
        finally:
            shutil.rmtree(build_root, ignore_errors=True)

    def test_mixed_execution_feature_pack_discovers_declared_unit_and_benchmark_entries_from_real_subject_output(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_mixed_execution_feature_pack")

        build_root = TEST_TMP_ROOT / "mixed-execution-feature-pack" / uuid.uuid4().hex
        build_output_root = build_root / "build"
        intermediate_root = build_root / "obj"
        build_root.mkdir(parents=True, exist_ok=True)
        try:
            run_checked(
                [
                    "dotnet",
                    "build",
                    str(MIXED_EXECUTION_FEATURE_PACK_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-o",
                    str(build_output_root),
                    "-m:1",
                    *dotnet_intermediate_args(intermediate_root),
                ],
                cwd=REPO_ROOT,
            )
            assembly_path = build_output_root / "MixedExecutionFeaturePack.dll"
            self.assertTrue(assembly_path.is_file(), msg=f"missing compiled subject dll: {assembly_path}")

            catalog = catalog_module.build_declared_test_catalog(
                repo_root=REPO_ROOT,
                subject_id="MixedExecutionFeaturePack",
                declaration_mode="require",
                assembly_paths=[assembly_path],
            )

            self.assertTrue(catalog["frameworkReferenced"])
            self.assertEqual("declared-test", catalog["subjectKind"])
            self.assertEqual([], catalog["warningCodes"])

            unit_entries = {
                entry["declaringType"]: entry
                for entry in catalog["declaredUnitTests"]
            }
            benchmark_entries = {
                entry["declaringType"]: entry
                for entry in catalog["declaredBenchmarks"]
            }

            self.assertEqual(
                {
                    "MixedExecutionFeaturePack.MixedExecutionProofEntry",
                    "MixedExecutionFeaturePack.InterpreterLoweringProofEntry",
                },
                set(unit_entries),
            )
            self.assertEqual(
                {
                    "MixedExecutionFeaturePack.MixedExecutionBenchmarkEntry",
                },
                set(benchmark_entries),
            )
            self.assertEqual("mixed-execution-proof", unit_entries["MixedExecutionFeaturePack.MixedExecutionProofEntry"]["alias"])
            self.assertEqual("interpreter-lowering-proof", unit_entries["MixedExecutionFeaturePack.InterpreterLoweringProofEntry"]["alias"])
            self.assertEqual("mixed-execution-bench", benchmark_entries["MixedExecutionFeaturePack.MixedExecutionBenchmarkEntry"]["alias"])
        finally:
            shutil.rmtree(build_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
