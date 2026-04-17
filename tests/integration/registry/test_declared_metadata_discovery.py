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
SOLUTION_CORE_PACK_MAINLINE_SLICE_PROJECT_PATH = (
    REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Proofs" / "CoreRuntimeFeatures" / "CoreRuntimeFeatures.csproj"
)
SOLUTION_CORE_PACK_PERFORMANCE_SLICE_PROJECT_PATH = (
    REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "CoreRuntimeBenchmarks.csproj"
)
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

        self.assertEqual(1, runtime_only["schemaVersion"])
        self.assertEqual(1, framework_only["schemaVersion"])
        self.assertEqual(1, declared["schemaVersion"])
        self.assertEqual(
            {
                "schemaVersion": 1,
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
                "schemaVersion": 1,
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
        self.assertEqual(0, unit_entry["entryIndex"])
        self.assertEqual("runtime-contract", unit_entry["alias"])
        self.assertEqual("DeclaredEntries", unit_entry["assemblyName"])
        self.assertEqual("DeclaredEntries.DeclaredProofs", unit_entry["declaringType"])
        self.assertEqual("VerifyOutput", unit_entry["methodName"])
        self.assertEqual("VerifyOutput()", unit_entry["methodSignature"])
        self.assertEqual(1, unit_entry["category"])
        self.assertEqual(6, unit_entry["capabilityFamily"])
        self.assertEqual(23, unit_entry["capabilityItem"])
        self.assertEqual(2, unit_entry["requires"])
        self.assertEqual(4, unit_entry["archetype"])
        self.assertEqual(8, unit_entry["hotUpdateCapability"])
        self.assertEqual(1, unit_entry["evidence"])
        self.assertEqual(3, unit_entry["priority"])

        self.assertEqual(
            "DeclaredEntriesSubject::DeclaredEntries::DeclaredEntries.DeclaredProofs::BenchmarkDispatch()",
            benchmark_entry["stableId"],
        )
        self.assertEqual(0, benchmark_entry["entryIndex"])
        self.assertEqual("dispatch-bench", benchmark_entry["alias"])
        self.assertEqual("DeclaredEntries", benchmark_entry["assemblyName"])
        self.assertEqual("DeclaredEntries.DeclaredProofs", benchmark_entry["declaringType"])
        self.assertEqual("BenchmarkDispatch", benchmark_entry["methodName"])
        self.assertEqual("BenchmarkDispatch()", benchmark_entry["methodSignature"])
        self.assertEqual(1, benchmark_entry["category"])
        self.assertEqual(2, benchmark_entry["capabilityFamily"])
        self.assertEqual(10, benchmark_entry["capabilityItem"])
        self.assertEqual(3, benchmark_entry["metrics"])
        self.assertEqual(1, benchmark_entry["requires"])
        self.assertEqual(8, benchmark_entry["archetype"])
        self.assertEqual(3, benchmark_entry["hotUpdateCapability"])
        self.assertEqual(2, benchmark_entry["warmupCount"])
        self.assertEqual(12, benchmark_entry["iterationCount"])
        self.assertEqual(100, benchmark_entry["invocationCount"])

    def test_compiled_catalog_scans_metadata_without_loading_framework_binary(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_catalog_metadata_only")

        isolated_dll_path = self._copy_fixture_output(self.declared_entries_dll)
        framework_dll_path = isolated_dll_path.parent / "Chaos.TestFramework.Sdk.dll"
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

        self.assertEqual(1, catalog["schemaVersion"])
        self.assertEqual(
            {
                "schemaVersion": 1,
                "subjectId": "CompatibilityShellSubject",
                "frameworkReferenced": False,
                "subjectKind": "runtime-only",
                "warningCodes": [],
                "declaredUnitTests": [],
                "declaredBenchmarks": [],
            },
            catalog,
        )

    def test_solution_core_pack_mainline_slice_discovers_declared_unit_entries_from_real_subject_output(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_solution_core_pack_mainline_slice")

        build_root = TEST_TMP_ROOT / "core-runtime-features" / uuid.uuid4().hex
        build_output_root = build_root / "build"
        intermediate_root = build_root / "obj"
        build_root.mkdir(parents=True, exist_ok=True)
        try:
            run_checked(
                [
                    "dotnet",
                    "build",
                    str(SOLUTION_CORE_PACK_MAINLINE_SLICE_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-o",
                    str(build_output_root),
                    "-m:1",
                    *dotnet_intermediate_args(intermediate_root),
                ],
                cwd=REPO_ROOT,
            )
            assembly_path = build_output_root / "CoreRuntimeFeatures.dll"
            self.assertTrue(assembly_path.is_file(), msg=f"missing compiled subject dll: {assembly_path}")

            catalog = catalog_module.build_declared_test_catalog(
                repo_root=REPO_ROOT,
                subject_id="SolutionCorePack",
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
                    "CoreRuntimeFeatures.AbiWideningProofEntry",
                    "CoreRuntimeFeatures.AotClosureValidationProofEntry",
                    "CoreRuntimeFeatures.ArithmeticOpsProofEntry",
                    "CoreRuntimeFeatures.ArrayOpsProofEntry",
                    "CoreRuntimeFeatures.ArrayCovarianceProofEntry",
                    "CoreRuntimeFeatures.BitwiseOpsProofEntry",
                    "CoreRuntimeFeatures.BoxedStructFieldReadProofEntry",
                    "CoreRuntimeFeatures.BranchOpsProofEntry",
                    "CoreRuntimeFeatures.CastTypeCheckProofEntry",
                    "CoreRuntimeFeatures.ConversionOpsProofEntry",
                    "CoreRuntimeFeatures.CrossBoundaryExceptionProofEntry",
                    "CoreRuntimeFeatures.CustomAttributeLookupProofEntry",
                    "CoreRuntimeFeatures.DelegateCallbackInteropProofEntry",
                    "CoreRuntimeFeatures.DelegateChainProofEntry",
                    "CoreRuntimeFeatures.ArrayBoxingProofEntry",
                    "CoreRuntimeFeatures.AsyncAwaitProofEntry",
                    "CoreRuntimeFeatures.DelegateProofEntry",
                    "CoreRuntimeFeatures.DirectCallVirtProofEntry",
                    "CoreRuntimeFeatures.EventCallbackFlowProofEntry",
                    "CoreRuntimeFeatures.ExceptionCatchProofEntry",
                    "CoreRuntimeFeatures.ExceptionProofEntry",
                    "CoreRuntimeFeatures.FinallyAndFilterProofEntry",
                    "CoreRuntimeFeatures.FunctionPointerProofEntry",
                    "CoreRuntimeFeatures.GcSensitiveFlowProofEntry",
                    "CoreRuntimeFeatures.GenericCollectionProofEntry",
                    "CoreRuntimeFeatures.GenericConstraintProofEntry",
                    "CoreRuntimeFeatures.GenericInstantiationProofEntry",
                    "CoreRuntimeFeatures.GenericSharingBoundaryProofEntry",
                    "CoreRuntimeFeatures.GenericLayoutProofEntry",
                    "CoreRuntimeFeatures.InterfaceDispatchProofEntry",
                    "CoreRuntimeFeatures.IntegerCarrierProofEntry",
                    "CoreRuntimeFeatures.IteratorStateMachineProofEntry",
                    "CoreRuntimeFeatures.LambdaClosureCaptureProofEntry",
                    "CoreRuntimeFeatures.LinkerPreserveContractProofEntry",
                    "CoreRuntimeFeatures.LinkerStrippingProofEntry",
                    "CoreRuntimeFeatures.LoaderContractProofEntry",
                    "CoreRuntimeFeatures.MarshalingProofEntry",
                    "CoreRuntimeFeatures.MetadataSupplementProofEntry",
                    "CoreRuntimeFeatures.MemberMetadataLookupProofEntry",
                    "CoreRuntimeFeatures.MonitorAndLockingProofEntry",
                    "CoreRuntimeFeatures.NativeCallInteropProofEntry",
                    "CoreRuntimeFeatures.NestedExceptionProofEntry",
                    "CoreRuntimeFeatures.ObjectFieldProofEntry",
                    "CoreRuntimeFeatures.ObjectReturnProofEntry",
                    "CoreRuntimeFeatures.RefStructAndStackallocProofEntry",
                    "CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry",
                    "CoreRuntimeFeatures.ResourceLifecycleProofEntry",
                    "CoreRuntimeFeatures.ReflectionInteropClosureEntry",
                    "CoreRuntimeFeatures.RuntimeHelpersProofEntry",
                    "CoreRuntimeFeatures.SpanAndMemoryProofEntry",
                    "CoreRuntimeFeatures.StructMarshalingProofEntry",
                    "CoreRuntimeFeatures.TaskAndValueTaskFlowProofEntry",
                    "CoreRuntimeFeatures.TaskSchedulingProofEntry",
                    "CoreRuntimeFeatures.ThreadingProofEntry",
                    "CoreRuntimeFeatures.ThreadLocalStateProofEntry",
                    "CoreRuntimeFeatures.UInt64BitwiseProofEntry",
                    "CoreRuntimeFeatures.UnsafePointerProofEntry",
                    "CoreRuntimeFeatures.DispatchProofEntry",
                    "CoreRuntimeFeatures.OverflowOpsProofEntry",
                    "CoreRuntimeFeatures.VTableDispatchProofEntry",
                },
                set(unit_entries),
            )
            self.assertEqual(
                "arithmetic-ops-proof",
                unit_entries["CoreRuntimeFeatures.ArithmeticOpsProofEntry"]["alias"],
            )
            self.assertEqual("array-ops-proof", unit_entries["CoreRuntimeFeatures.ArrayOpsProofEntry"]["alias"])
            self.assertEqual("bitwise-ops-proof", unit_entries["CoreRuntimeFeatures.BitwiseOpsProofEntry"]["alias"])
            self.assertEqual(
                "finally-filter-proof",
                unit_entries["CoreRuntimeFeatures.FinallyAndFilterProofEntry"]["alias"],
            )
            self.assertEqual(
                "gc-sensitive-flow-proof",
                unit_entries["CoreRuntimeFeatures.GcSensitiveFlowProofEntry"]["alias"],
            )
            self.assertEqual("interface-dispatch-proof", unit_entries["CoreRuntimeFeatures.InterfaceDispatchProofEntry"]["alias"])
            self.assertEqual(
                "iterator-state-machine-proof",
                unit_entries["CoreRuntimeFeatures.IteratorStateMachineProofEntry"]["alias"],
            )
            self.assertEqual("vtable-dispatch-proof", unit_entries["CoreRuntimeFeatures.VTableDispatchProofEntry"]["alias"])
            self.assertEqual("linker-stripping-proof", unit_entries["CoreRuntimeFeatures.LinkerStrippingProofEntry"]["alias"])
            self.assertEqual("loader-contract-proof", unit_entries["CoreRuntimeFeatures.LoaderContractProofEntry"]["alias"])
            self.assertEqual("marshaling-proof", unit_entries["CoreRuntimeFeatures.MarshalingProofEntry"]["alias"])
            self.assertEqual(
                "monitor-locking-proof",
                unit_entries["CoreRuntimeFeatures.MonitorAndLockingProofEntry"]["alias"],
            )
            self.assertEqual("cross-boundary-exception-proof", unit_entries["CoreRuntimeFeatures.CrossBoundaryExceptionProofEntry"]["alias"])
            self.assertEqual(
                "custom-attribute-lookup-proof",
                unit_entries["CoreRuntimeFeatures.CustomAttributeLookupProofEntry"]["alias"],
            )
            self.assertEqual(
                "delegate-callback-interop-proof",
                unit_entries["CoreRuntimeFeatures.DelegateCallbackInteropProofEntry"]["alias"],
            )
            self.assertEqual("function-pointer-proof", unit_entries["CoreRuntimeFeatures.FunctionPointerProofEntry"]["alias"])
            self.assertEqual("generic-collection-proof", unit_entries["CoreRuntimeFeatures.GenericCollectionProofEntry"]["alias"])
            self.assertEqual(
                "generic-sharing-boundary-proof",
                unit_entries["CoreRuntimeFeatures.GenericSharingBoundaryProofEntry"]["alias"],
            )
            self.assertEqual(
                "linker-preserve-contract-proof",
                unit_entries["CoreRuntimeFeatures.LinkerPreserveContractProofEntry"]["alias"],
            )
            self.assertEqual("nested-exception-proof", unit_entries["CoreRuntimeFeatures.NestedExceptionProofEntry"]["alias"])
            self.assertEqual(
                "native-call-interop-proof",
                unit_entries["CoreRuntimeFeatures.NativeCallInteropProofEntry"]["alias"],
            )
            self.assertEqual(
                "member-metadata-lookup-proof",
                unit_entries["CoreRuntimeFeatures.MemberMetadataLookupProofEntry"]["alias"],
            )
            self.assertEqual(
                "ref-struct-stackalloc-proof",
                unit_entries["CoreRuntimeFeatures.RefStructAndStackallocProofEntry"]["alias"],
            )
            self.assertEqual(
                "required-instantiation-closure-proof",
                unit_entries["CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry"]["alias"],
            )
            self.assertEqual(
                "resource-lifecycle-proof",
                unit_entries["CoreRuntimeFeatures.ResourceLifecycleProofEntry"]["alias"],
            )
            self.assertEqual("runtime-helpers-proof", unit_entries["CoreRuntimeFeatures.RuntimeHelpersProofEntry"]["alias"])
            self.assertEqual("span-memory-proof", unit_entries["CoreRuntimeFeatures.SpanAndMemoryProofEntry"]["alias"])
            self.assertEqual("struct-marshaling-proof", unit_entries["CoreRuntimeFeatures.StructMarshalingProofEntry"]["alias"])
            self.assertEqual(
                "task-valuetask-flow-proof",
                unit_entries["CoreRuntimeFeatures.TaskAndValueTaskFlowProofEntry"]["alias"],
            )
            self.assertEqual("task-scheduling-proof", unit_entries["CoreRuntimeFeatures.TaskSchedulingProofEntry"]["alias"])
            self.assertEqual("thread-local-state-proof", unit_entries["CoreRuntimeFeatures.ThreadLocalStateProofEntry"]["alias"])
            self.assertEqual("unsafe-pointer-proof", unit_entries["CoreRuntimeFeatures.UnsafePointerProofEntry"]["alias"])
            self.assertEqual("delegate-chain-proof", unit_entries["CoreRuntimeFeatures.DelegateChainProofEntry"]["alias"])
            self.assertEqual("dispatch-proof", unit_entries["CoreRuntimeFeatures.DispatchProofEntry"]["alias"])
            self.assertTrue(all(entry["capabilityFamily"] > 0 for entry in unit_entries.values()))
            self.assertTrue(all(entry["capabilityItem"] > 0 for entry in unit_entries.values()))
            self.assertEqual(1, unit_entries["CoreRuntimeFeatures.ArithmeticOpsProofEntry"]["capabilityFamily"])
            self.assertEqual(1, unit_entries["CoreRuntimeFeatures.ArithmeticOpsProofEntry"]["capabilityItem"])
            self.assertEqual(1, unit_entries["CoreRuntimeFeatures.ArrayOpsProofEntry"]["capabilityFamily"])
            self.assertEqual(6, unit_entries["CoreRuntimeFeatures.ArrayOpsProofEntry"]["capabilityItem"])
            self.assertEqual(3, unit_entries["CoreRuntimeFeatures.TaskAndValueTaskFlowProofEntry"]["capabilityFamily"])
            self.assertEqual(15, unit_entries["CoreRuntimeFeatures.TaskAndValueTaskFlowProofEntry"]["capabilityItem"])
            self.assertEqual(3, unit_entries["CoreRuntimeFeatures.IteratorStateMachineProofEntry"]["capabilityFamily"])
            self.assertEqual(16, unit_entries["CoreRuntimeFeatures.IteratorStateMachineProofEntry"]["capabilityItem"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.FinallyAndFilterProofEntry"]["capabilityFamily"])
            self.assertEqual(29, unit_entries["CoreRuntimeFeatures.FinallyAndFilterProofEntry"]["capabilityItem"])
            self.assertEqual(2, unit_entries["CoreRuntimeFeatures.GenericCollectionProofEntry"]["capabilityFamily"])
            self.assertEqual(12, unit_entries["CoreRuntimeFeatures.GenericCollectionProofEntry"]["capabilityItem"])
            self.assertEqual(6, unit_entries["CoreRuntimeFeatures.CustomAttributeLookupProofEntry"]["capabilityFamily"])
            self.assertEqual(21, unit_entries["CoreRuntimeFeatures.CustomAttributeLookupProofEntry"]["capabilityItem"])
            self.assertEqual(7, unit_entries["CoreRuntimeFeatures.DelegateCallbackInteropProofEntry"]["capabilityFamily"])
            self.assertEqual(34, unit_entries["CoreRuntimeFeatures.DelegateCallbackInteropProofEntry"]["capabilityItem"])
            self.assertEqual(10, unit_entries["CoreRuntimeFeatures.FunctionPointerProofEntry"]["capabilityFamily"])
            self.assertEqual(46, unit_entries["CoreRuntimeFeatures.FunctionPointerProofEntry"]["capabilityItem"])
            self.assertEqual(12, unit_entries["CoreRuntimeFeatures.GenericSharingBoundaryProofEntry"]["capabilityFamily"])
            self.assertEqual(52, unit_entries["CoreRuntimeFeatures.GenericSharingBoundaryProofEntry"]["capabilityItem"])
            self.assertEqual(12, unit_entries["CoreRuntimeFeatures.LinkerPreserveContractProofEntry"]["capabilityFamily"])
            self.assertEqual(25, unit_entries["CoreRuntimeFeatures.LinkerPreserveContractProofEntry"]["capabilityItem"])
            self.assertEqual(6, unit_entries["CoreRuntimeFeatures.MemberMetadataLookupProofEntry"]["capabilityFamily"])
            self.assertEqual(22, unit_entries["CoreRuntimeFeatures.MemberMetadataLookupProofEntry"]["capabilityItem"])
            self.assertEqual(7, unit_entries["CoreRuntimeFeatures.NativeCallInteropProofEntry"]["capabilityFamily"])
            self.assertEqual(31, unit_entries["CoreRuntimeFeatures.NativeCallInteropProofEntry"]["capabilityItem"])
            self.assertEqual(10, unit_entries["CoreRuntimeFeatures.RefStructAndStackallocProofEntry"]["capabilityFamily"])
            self.assertEqual(44, unit_entries["CoreRuntimeFeatures.RefStructAndStackallocProofEntry"]["capabilityItem"])
            self.assertEqual(12, unit_entries["CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry"]["capabilityFamily"])
            self.assertEqual(51, unit_entries["CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry"]["capabilityItem"])
            self.assertEqual(8, unit_entries["CoreRuntimeFeatures.TaskSchedulingProofEntry"]["capabilityFamily"])
            self.assertEqual(36, unit_entries["CoreRuntimeFeatures.TaskSchedulingProofEntry"]["capabilityItem"])
            self.assertEqual(8, unit_entries["CoreRuntimeFeatures.MonitorAndLockingProofEntry"]["capabilityFamily"])
            self.assertEqual(37, unit_entries["CoreRuntimeFeatures.MonitorAndLockingProofEntry"]["capabilityItem"])
            self.assertEqual(8, unit_entries["CoreRuntimeFeatures.ThreadLocalStateProofEntry"]["capabilityFamily"])
            self.assertEqual(38, unit_entries["CoreRuntimeFeatures.ThreadLocalStateProofEntry"]["capabilityItem"])
            self.assertEqual(9, unit_entries["CoreRuntimeFeatures.RuntimeHelpersProofEntry"]["capabilityFamily"])
            self.assertEqual(39, unit_entries["CoreRuntimeFeatures.RuntimeHelpersProofEntry"]["capabilityItem"])
            self.assertEqual(9, unit_entries["CoreRuntimeFeatures.GcSensitiveFlowProofEntry"]["capabilityFamily"])
            self.assertEqual(40, unit_entries["CoreRuntimeFeatures.GcSensitiveFlowProofEntry"]["capabilityItem"])
            self.assertEqual(9, unit_entries["CoreRuntimeFeatures.ResourceLifecycleProofEntry"]["capabilityFamily"])
            self.assertEqual(41, unit_entries["CoreRuntimeFeatures.ResourceLifecycleProofEntry"]["capabilityItem"])
            self.assertEqual(9, unit_entries["CoreRuntimeFeatures.LoaderContractProofEntry"]["capabilityFamily"])
            self.assertEqual(42, unit_entries["CoreRuntimeFeatures.LoaderContractProofEntry"]["capabilityItem"])
            self.assertEqual(6, unit_entries["CoreRuntimeFeatures.LinkerStrippingProofEntry"]["capabilityFamily"])
            self.assertEqual(26, unit_entries["CoreRuntimeFeatures.LinkerStrippingProofEntry"]["capabilityItem"])
            self.assertEqual(10, unit_entries["CoreRuntimeFeatures.SpanAndMemoryProofEntry"]["capabilityFamily"])
            self.assertEqual(43, unit_entries["CoreRuntimeFeatures.SpanAndMemoryProofEntry"]["capabilityItem"])
            self.assertEqual(7, unit_entries["CoreRuntimeFeatures.StructMarshalingProofEntry"]["capabilityFamily"])
            self.assertEqual(33, unit_entries["CoreRuntimeFeatures.StructMarshalingProofEntry"]["capabilityItem"])
            self.assertEqual(10, unit_entries["CoreRuntimeFeatures.UnsafePointerProofEntry"]["capabilityFamily"])
            self.assertEqual(45, unit_entries["CoreRuntimeFeatures.UnsafePointerProofEntry"]["capabilityItem"])
            self.assertEqual(1, unit_entries["CoreRuntimeFeatures.GenericCollectionProofEntry"]["requires"])
            self.assertEqual(2, unit_entries["CoreRuntimeFeatures.CustomAttributeLookupProofEntry"]["requires"])
            self.assertEqual(8, unit_entries["CoreRuntimeFeatures.CrossBoundaryExceptionProofEntry"]["requires"])
            self.assertEqual(20, unit_entries["CoreRuntimeFeatures.DelegateCallbackInteropProofEntry"]["requires"])
            self.assertEqual(8, unit_entries["CoreRuntimeFeatures.FinallyAndFilterProofEntry"]["requires"])
            self.assertEqual(4, unit_entries["CoreRuntimeFeatures.DelegateChainProofEntry"]["requires"])
            self.assertEqual(4, unit_entries["CoreRuntimeFeatures.DelegateProofEntry"]["requires"])
            self.assertEqual(8, unit_entries["CoreRuntimeFeatures.ExceptionProofEntry"]["requires"])
            self.assertEqual(2048, unit_entries["CoreRuntimeFeatures.FunctionPointerProofEntry"]["requires"])
            self.assertEqual(8193, unit_entries["CoreRuntimeFeatures.GenericSharingBoundaryProofEntry"]["requires"])
            self.assertEqual(4098, unit_entries["CoreRuntimeFeatures.LinkerPreserveContractProofEntry"]["requires"])
            self.assertEqual(2, unit_entries["CoreRuntimeFeatures.MemberMetadataLookupProofEntry"]["requires"])
            self.assertEqual(16, unit_entries["CoreRuntimeFeatures.NativeCallInteropProofEntry"]["requires"])
            self.assertEqual(16896, unit_entries["CoreRuntimeFeatures.RefStructAndStackallocProofEntry"]["requires"])
            self.assertEqual(18, unit_entries["CoreRuntimeFeatures.ReflectionInteropClosureEntry"]["requires"])
            self.assertEqual(1, unit_entries["CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry"]["requires"])
            self.assertEqual(512, unit_entries["CoreRuntimeFeatures.SpanAndMemoryProofEntry"]["requires"])
            self.assertEqual(16, unit_entries["CoreRuntimeFeatures.StructMarshalingProofEntry"]["requires"])
            self.assertEqual(64, unit_entries["CoreRuntimeFeatures.TaskAndValueTaskFlowProofEntry"]["requires"])
            self.assertEqual(384, unit_entries["CoreRuntimeFeatures.TaskSchedulingProofEntry"]["requires"])
            self.assertEqual(256, unit_entries["CoreRuntimeFeatures.MonitorAndLockingProofEntry"]["requires"])
            self.assertEqual(128, unit_entries["CoreRuntimeFeatures.ThreadLocalStateProofEntry"]["requires"])
            self.assertEqual(1024, unit_entries["CoreRuntimeFeatures.UnsafePointerProofEntry"]["requires"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.CustomAttributeLookupProofEntry"]["archetype"])
            self.assertEqual(4, unit_entries["CoreRuntimeFeatures.DelegateCallbackInteropProofEntry"]["archetype"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.FunctionPointerProofEntry"]["archetype"])
            self.assertEqual(6, unit_entries["CoreRuntimeFeatures.GenericSharingBoundaryProofEntry"]["archetype"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.LinkerPreserveContractProofEntry"]["archetype"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.MemberMetadataLookupProofEntry"]["archetype"])
            self.assertEqual(4, unit_entries["CoreRuntimeFeatures.NativeCallInteropProofEntry"]["archetype"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.RefStructAndStackallocProofEntry"]["archetype"])
            self.assertEqual(6, unit_entries["CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry"]["archetype"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.SpanAndMemoryProofEntry"]["archetype"])
            self.assertEqual(4, unit_entries["CoreRuntimeFeatures.StructMarshalingProofEntry"]["archetype"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.UnsafePointerProofEntry"]["archetype"])
            self.assertTrue(all(entry["evidence"] == 0 for entry in unit_entries.values()))
        finally:
            shutil.rmtree(build_root, ignore_errors=True)

    def test_solution_core_pack_performance_slice_discovers_declared_benchmark_entries_from_real_subject_output(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_solution_core_pack_performance_slice")

        build_root = TEST_TMP_ROOT / "core-runtime-benchmarks" / uuid.uuid4().hex
        build_output_root = build_root / "build"
        intermediate_root = build_root / "obj"
        build_root.mkdir(parents=True, exist_ok=True)
        try:
            run_checked(
                [
                    "dotnet",
                    "build",
                    str(SOLUTION_CORE_PACK_PERFORMANCE_SLICE_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-o",
                    str(build_output_root),
                    "-m:1",
                    *dotnet_intermediate_args(intermediate_root),
                ],
                cwd=REPO_ROOT,
            )
            assembly_path = build_output_root / "CoreRuntimeBenchmarks.dll"
            self.assertTrue(assembly_path.is_file(), msg=f"missing compiled subject dll: {assembly_path}")

            catalog = catalog_module.build_declared_test_catalog(
                repo_root=REPO_ROOT,
                subject_id="SolutionCorePack",
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
                    "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                    "CoreRuntimeBenchmarks.AllocationBenchmarkEntry",
                    "CoreRuntimeBenchmarks.DelegateCallbackInteropBenchmarkEntry",
                    "CoreRuntimeBenchmarks.DispatchBenchmarkEntry",
                    "CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry",
                    "CoreRuntimeBenchmarks.FloatingPointCarrierBenchmarkEntry",
                    "CoreRuntimeBenchmarks.GenericBenchmarkEntry",
                    "CoreRuntimeBenchmarks.GenericInterfaceDispatchBenchmarkEntry",
                    "CoreRuntimeBenchmarks.GenericSharingBoundaryBenchmarkEntry",
                    "CoreRuntimeBenchmarks.IntegerCarrierBenchmarkEntry",
                    "CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry",
                    "CoreRuntimeBenchmarks.MonitorAndLockingBenchmarkEntry",
                    "CoreRuntimeBenchmarks.NativeCallInteropBenchmarkEntry",
                    "CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry",
                    "CoreRuntimeBenchmarks.ShiftOpsBenchmarkEntry",
                    "CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry",
                    "CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry",
                    "CoreRuntimeBenchmarks.StructMarshalingBenchmarkEntry",
                    "CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry",
                    "CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry",
                    "CoreRuntimeBenchmarks.UInt64BitwiseBenchmarkEntry",
                    "CoreRuntimeBenchmarks.UnsafePointerBenchmarkEntry",
                },
                set(benchmark_entries),
            )
            self.assertEqual("arithmetic-bench", benchmark_entries["CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry"]["alias"])
            self.assertEqual("allocation-bench", benchmark_entries["CoreRuntimeBenchmarks.AllocationBenchmarkEntry"]["alias"])
            self.assertEqual(
                "delegate-callback-interop-bench",
                benchmark_entries["CoreRuntimeBenchmarks.DelegateCallbackInteropBenchmarkEntry"]["alias"],
            )
            self.assertEqual("dispatch-bench", benchmark_entries["CoreRuntimeBenchmarks.DispatchBenchmarkEntry"]["alias"])
            self.assertEqual(
                "function-pointer-bench",
                benchmark_entries["CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry"]["alias"],
            )
            self.assertEqual("generic-bench", benchmark_entries["CoreRuntimeBenchmarks.GenericBenchmarkEntry"]["alias"])
            self.assertEqual(
                "generic-interface-dispatch-bench",
                benchmark_entries["CoreRuntimeBenchmarks.GenericInterfaceDispatchBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "generic-sharing-boundary-bench",
                benchmark_entries["CoreRuntimeBenchmarks.GenericSharingBoundaryBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "member-metadata-lookup-bench",
                benchmark_entries["CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "monitor-locking-bench",
                benchmark_entries["CoreRuntimeBenchmarks.MonitorAndLockingBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "native-call-interop-bench",
                benchmark_entries["CoreRuntimeBenchmarks.NativeCallInteropBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "required-instantiation-closure-bench",
                benchmark_entries["CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry"]["alias"],
            )
            self.assertEqual("span-memory-bench", benchmark_entries["CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry"]["alias"])
            self.assertEqual(
                "string-utf8-marshaling-bench",
                benchmark_entries["CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "struct-marshaling-bench",
                benchmark_entries["CoreRuntimeBenchmarks.StructMarshalingBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "task-valuetask-flow-bench",
                benchmark_entries["CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "task-scheduling-bench",
                benchmark_entries["CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry"]["alias"],
            )
            self.assertEqual("unsafe-pointer-bench", benchmark_entries["CoreRuntimeBenchmarks.UnsafePointerBenchmarkEntry"]["alias"])
            self.assertTrue(all(entry["capabilityFamily"] > 0 for entry in benchmark_entries.values()))
            self.assertTrue(all(entry["capabilityItem"] > 0 for entry in benchmark_entries.values()))
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.AllocationBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.DelegateCallbackInteropBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.DispatchBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.GenericBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.GenericInterfaceDispatchBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.GenericSharingBoundaryBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.MonitorAndLockingBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.NativeCallInteropBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.StructMarshalingBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.UnsafePointerBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry"]["category"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.AllocationBenchmarkEntry"]["category"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(7, benchmark_entries["CoreRuntimeBenchmarks.DelegateCallbackInteropBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(34, benchmark_entries["CoreRuntimeBenchmarks.DelegateCallbackInteropBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(10, benchmark_entries["CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(46, benchmark_entries["CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(2, benchmark_entries["CoreRuntimeBenchmarks.GenericBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(12, benchmark_entries["CoreRuntimeBenchmarks.GenericBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(2, benchmark_entries["CoreRuntimeBenchmarks.GenericInterfaceDispatchBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(11, benchmark_entries["CoreRuntimeBenchmarks.GenericInterfaceDispatchBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(12, benchmark_entries["CoreRuntimeBenchmarks.GenericSharingBoundaryBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(52, benchmark_entries["CoreRuntimeBenchmarks.GenericSharingBoundaryBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(6, benchmark_entries["CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(22, benchmark_entries["CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(8, benchmark_entries["CoreRuntimeBenchmarks.MonitorAndLockingBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(37, benchmark_entries["CoreRuntimeBenchmarks.MonitorAndLockingBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(7, benchmark_entries["CoreRuntimeBenchmarks.NativeCallInteropBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(31, benchmark_entries["CoreRuntimeBenchmarks.NativeCallInteropBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(12, benchmark_entries["CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(51, benchmark_entries["CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(10, benchmark_entries["CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(43, benchmark_entries["CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(7, benchmark_entries["CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(32, benchmark_entries["CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(7, benchmark_entries["CoreRuntimeBenchmarks.StructMarshalingBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(33, benchmark_entries["CoreRuntimeBenchmarks.StructMarshalingBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(15, benchmark_entries["CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(8, benchmark_entries["CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(36, benchmark_entries["CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(10, benchmark_entries["CoreRuntimeBenchmarks.UnsafePointerBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(45, benchmark_entries["CoreRuntimeBenchmarks.UnsafePointerBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry"]["metrics"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.AllocationBenchmarkEntry"]["metrics"])
            self.assertEqual(20, benchmark_entries["CoreRuntimeBenchmarks.DelegateCallbackInteropBenchmarkEntry"]["requires"])
            self.assertEqual(2048, benchmark_entries["CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry"]["requires"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.GenericBenchmarkEntry"]["requires"])
            self.assertEqual(8193, benchmark_entries["CoreRuntimeBenchmarks.GenericSharingBoundaryBenchmarkEntry"]["requires"])
            self.assertEqual(2, benchmark_entries["CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry"]["requires"])
            self.assertEqual(256, benchmark_entries["CoreRuntimeBenchmarks.MonitorAndLockingBenchmarkEntry"]["requires"])
            self.assertEqual(16, benchmark_entries["CoreRuntimeBenchmarks.NativeCallInteropBenchmarkEntry"]["requires"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry"]["requires"])
            self.assertEqual(512, benchmark_entries["CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry"]["requires"])
            self.assertEqual(16, benchmark_entries["CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry"]["requires"])
            self.assertEqual(16, benchmark_entries["CoreRuntimeBenchmarks.StructMarshalingBenchmarkEntry"]["requires"])
            self.assertEqual(64, benchmark_entries["CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry"]["requires"])
            self.assertEqual(384, benchmark_entries["CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry"]["requires"])
            self.assertEqual(1024, benchmark_entries["CoreRuntimeBenchmarks.UnsafePointerBenchmarkEntry"]["requires"])
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
                    "HotUpdateHostPack.MetadataSupplementProofEntry",
                    "HotUpdateHostPack.MethodReplacementProofEntry",
                    "HotUpdateHostPack.PatchIntegrityProofEntry",
                    "HotUpdateHostPack.PatchCallbackFlowProofEntry",
                    "HotUpdateHostPack.SharedContractProofEntry",
                    "HotUpdateHostPack.VersionRollbackProofEntry",
                },
                set(unit_entries),
            )
            self.assertEqual(
                {
                    "HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry",
                    "HotUpdateHostPack.HotUpdateLoadBenchmarkEntry",
                    "HotUpdateHostPack.MethodReplacementBenchmarkEntry",
                    "HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry",
                },
                set(benchmark_entries),
            )
            self.assertEqual("hot-update-skeleton-proof", unit_entries["HotUpdateHostPack.HotUpdateSkeletonProofEntry"]["alias"])
            self.assertEqual("metadata-supplement-proof", unit_entries["HotUpdateHostPack.MetadataSupplementProofEntry"]["alias"])
            self.assertEqual("method-replacement-proof", unit_entries["HotUpdateHostPack.MethodReplacementProofEntry"]["alias"])
            self.assertEqual("patch-callback-flow-proof", unit_entries["HotUpdateHostPack.PatchCallbackFlowProofEntry"]["alias"])
            self.assertEqual("shared-contract-proof", unit_entries["HotUpdateHostPack.SharedContractProofEntry"]["alias"])
            self.assertEqual("version-rollback-proof", unit_entries["HotUpdateHostPack.VersionRollbackProofEntry"]["alias"])
            self.assertEqual("hot-update-dispatch-bench", benchmark_entries["HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry"]["alias"])
            self.assertEqual("hot-update-load-bench", benchmark_entries["HotUpdateHostPack.HotUpdateLoadBenchmarkEntry"]["alias"])
            self.assertEqual("method-replacement-bench", benchmark_entries["HotUpdateHostPack.MethodReplacementBenchmarkEntry"]["alias"])
            self.assertEqual("hot-update-roundtrip-bench", benchmark_entries["HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry"]["alias"])
            self.assertTrue(all(entry["capabilityFamily"] > 0 for entry in unit_entries.values()))
            self.assertTrue(all(entry["capabilityItem"] > 0 for entry in unit_entries.values()))
            self.assertTrue(all(entry["capabilityFamily"] > 0 for entry in benchmark_entries.values()))
            self.assertTrue(all(entry["capabilityItem"] > 0 for entry in benchmark_entries.values()))
            self.assertEqual(13, unit_entries["HotUpdateHostPack.HotUpdateSkeletonProofEntry"]["capabilityFamily"])
            self.assertEqual(54, unit_entries["HotUpdateHostPack.HotUpdateSkeletonProofEntry"]["capabilityItem"])
            self.assertEqual(13, unit_entries["HotUpdateHostPack.MetadataSupplementProofEntry"]["capabilityFamily"])
            self.assertEqual(57, unit_entries["HotUpdateHostPack.MetadataSupplementProofEntry"]["capabilityItem"])
            self.assertEqual(13, unit_entries["HotUpdateHostPack.MethodReplacementProofEntry"]["capabilityFamily"])
            self.assertEqual(56, unit_entries["HotUpdateHostPack.MethodReplacementProofEntry"]["capabilityItem"])
            self.assertEqual(13, unit_entries["HotUpdateHostPack.PatchCallbackFlowProofEntry"]["capabilityFamily"])
            self.assertEqual(60, unit_entries["HotUpdateHostPack.PatchCallbackFlowProofEntry"]["capabilityItem"])
            self.assertEqual(13, unit_entries["HotUpdateHostPack.SharedContractProofEntry"]["capabilityFamily"])
            self.assertEqual(55, unit_entries["HotUpdateHostPack.SharedContractProofEntry"]["capabilityItem"])
            self.assertEqual(13, unit_entries["HotUpdateHostPack.VersionRollbackProofEntry"]["capabilityFamily"])
            self.assertEqual(59, unit_entries["HotUpdateHostPack.VersionRollbackProofEntry"]["capabilityItem"])
            self.assertEqual(7, unit_entries["HotUpdateHostPack.HotUpdateSkeletonProofEntry"]["archetype"])
            self.assertEqual(8, unit_entries["HotUpdateHostPack.MetadataSupplementProofEntry"]["archetype"])
            self.assertEqual(7, unit_entries["HotUpdateHostPack.MethodReplacementProofEntry"]["archetype"])
            self.assertEqual(7, unit_entries["HotUpdateHostPack.PatchCallbackFlowProofEntry"]["archetype"])
            self.assertEqual(8, unit_entries["HotUpdateHostPack.SharedContractProofEntry"]["archetype"])
            self.assertEqual(9, unit_entries["HotUpdateHostPack.VersionRollbackProofEntry"]["archetype"])
            self.assertEqual(13, benchmark_entries["HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(60, benchmark_entries["HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(13, benchmark_entries["HotUpdateHostPack.HotUpdateLoadBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(54, benchmark_entries["HotUpdateHostPack.HotUpdateLoadBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(13, benchmark_entries["HotUpdateHostPack.MethodReplacementBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(56, benchmark_entries["HotUpdateHostPack.MethodReplacementBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(13, benchmark_entries["HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(60, benchmark_entries["HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(7, benchmark_entries["HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry"]["archetype"])
            self.assertEqual(7, benchmark_entries["HotUpdateHostPack.HotUpdateLoadBenchmarkEntry"]["archetype"])
            self.assertEqual(7, benchmark_entries["HotUpdateHostPack.MethodReplacementBenchmarkEntry"]["archetype"])
            self.assertEqual(8, benchmark_entries["HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry"]["archetype"])
            self.assertEqual(1, benchmark_entries["HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["HotUpdateHostPack.HotUpdateLoadBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["HotUpdateHostPack.MethodReplacementBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry"]["modes"])
            self.assertTrue(all(entry["evidence"] == 0 for entry in unit_entries.values()))
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
                    "MixedExecutionFeaturePack.InterpreterArithmeticProofEntry",
                    "MixedExecutionFeaturePack.InterpreterLoweringProofEntry",
                    "MixedExecutionFeaturePack.MixedGenericFlowProofEntry",
                    "MixedExecutionFeaturePack.MixedExceptionFlowProofEntry",
                    "MixedExecutionFeaturePack.MixedDelegateFlowProofEntry",
                },
                set(unit_entries),
            )
            self.assertEqual(
                {
                    "MixedExecutionFeaturePack.MixedExecutionBenchmarkEntry",
                    "MixedExecutionFeaturePack.MixedExecutionNativeBenchmarkEntry",
                },
                set(benchmark_entries),
            )
            self.assertEqual("mixed-execution-proof", unit_entries["MixedExecutionFeaturePack.MixedExecutionProofEntry"]["alias"])
            self.assertEqual("interpreter-arithmetic-proof", unit_entries["MixedExecutionFeaturePack.InterpreterArithmeticProofEntry"]["alias"])
            self.assertEqual("interpreter-lowering-proof", unit_entries["MixedExecutionFeaturePack.InterpreterLoweringProofEntry"]["alias"])
            self.assertEqual("mixed-generic-flow-proof", unit_entries["MixedExecutionFeaturePack.MixedGenericFlowProofEntry"]["alias"])
            self.assertEqual("mixed-exception-flow-proof", unit_entries["MixedExecutionFeaturePack.MixedExceptionFlowProofEntry"]["alias"])
            self.assertEqual("mixed-delegate-flow-proof", unit_entries["MixedExecutionFeaturePack.MixedDelegateFlowProofEntry"]["alias"])
            self.assertEqual("mixed-execution-bench", benchmark_entries["MixedExecutionFeaturePack.MixedExecutionBenchmarkEntry"]["alias"])
            self.assertEqual("mixed-execution-native-bench", benchmark_entries["MixedExecutionFeaturePack.MixedExecutionNativeBenchmarkEntry"]["alias"])
            self.assertTrue(all(entry["capabilityFamily"] > 0 for entry in unit_entries.values()))
            self.assertTrue(all(entry["capabilityItem"] > 0 for entry in unit_entries.values()))
            self.assertTrue(all(entry["capabilityFamily"] > 0 for entry in benchmark_entries.values()))
            self.assertTrue(all(entry["capabilityItem"] > 0 for entry in benchmark_entries.values()))
            self.assertEqual(14, unit_entries["MixedExecutionFeaturePack.MixedExecutionProofEntry"]["capabilityFamily"])
            self.assertEqual(61, unit_entries["MixedExecutionFeaturePack.MixedExecutionProofEntry"]["capabilityItem"])
            self.assertEqual(14, unit_entries["MixedExecutionFeaturePack.InterpreterArithmeticProofEntry"]["capabilityFamily"])
            self.assertEqual(63, unit_entries["MixedExecutionFeaturePack.InterpreterArithmeticProofEntry"]["capabilityItem"])
            self.assertEqual(14, unit_entries["MixedExecutionFeaturePack.InterpreterLoweringProofEntry"]["capabilityFamily"])
            self.assertEqual(62, unit_entries["MixedExecutionFeaturePack.InterpreterLoweringProofEntry"]["capabilityItem"])
            self.assertEqual(14, unit_entries["MixedExecutionFeaturePack.MixedGenericFlowProofEntry"]["capabilityFamily"])
            self.assertEqual(64, unit_entries["MixedExecutionFeaturePack.MixedGenericFlowProofEntry"]["capabilityItem"])
            self.assertEqual(14, unit_entries["MixedExecutionFeaturePack.MixedExceptionFlowProofEntry"]["capabilityFamily"])
            self.assertEqual(65, unit_entries["MixedExecutionFeaturePack.MixedExceptionFlowProofEntry"]["capabilityItem"])
            self.assertEqual(14, unit_entries["MixedExecutionFeaturePack.MixedDelegateFlowProofEntry"]["capabilityFamily"])
            self.assertEqual(66, unit_entries["MixedExecutionFeaturePack.MixedDelegateFlowProofEntry"]["capabilityItem"])
            self.assertEqual(11, unit_entries["MixedExecutionFeaturePack.MixedExecutionProofEntry"]["archetype"])
            self.assertEqual(11, unit_entries["MixedExecutionFeaturePack.InterpreterArithmeticProofEntry"]["archetype"])
            self.assertEqual(11, unit_entries["MixedExecutionFeaturePack.InterpreterLoweringProofEntry"]["archetype"])
            self.assertEqual(11, unit_entries["MixedExecutionFeaturePack.MixedGenericFlowProofEntry"]["archetype"])
            self.assertEqual(11, unit_entries["MixedExecutionFeaturePack.MixedExceptionFlowProofEntry"]["archetype"])
            self.assertEqual(11, unit_entries["MixedExecutionFeaturePack.MixedDelegateFlowProofEntry"]["archetype"])
            self.assertEqual(14, benchmark_entries["MixedExecutionFeaturePack.MixedExecutionBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(61, benchmark_entries["MixedExecutionFeaturePack.MixedExecutionBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(14, benchmark_entries["MixedExecutionFeaturePack.MixedExecutionNativeBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(63, benchmark_entries["MixedExecutionFeaturePack.MixedExecutionNativeBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(5, benchmark_entries["MixedExecutionFeaturePack.MixedExecutionBenchmarkEntry"]["modes"])
            self.assertEqual(7, benchmark_entries["MixedExecutionFeaturePack.MixedExecutionNativeBenchmarkEntry"]["modes"])
            self.assertTrue(all(entry["evidence"] == 0 for entry in unit_entries.values()))
        finally:
            shutil.rmtree(build_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()

