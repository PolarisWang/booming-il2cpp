from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack"
MANIFEST_PATH = SUBJECT_ROOT / "subject.manifest.json"
SOURCE_PROJECT_PATH = SUBJECT_ROOT / "source" / "FeatureSlices" / "CoreRuntimeFeatures" / "CoreRuntimeFeatures.csproj"
THREADING_SOURCE_PATH = SUBJECT_ROOT / "source" / "FeatureSlices" / "CoreRuntimeFeatures" / "AsyncAndThreading" / "ThreadingProof.cs"
MONITOR_SOURCE_PATH = SUBJECT_ROOT / "source" / "FeatureSlices" / "CoreRuntimeFeatures" / "AsyncAndThreading" / "MonitorAndLockingProof.cs"
THREAD_LOCAL_SOURCE_PATH = SUBJECT_ROOT / "source" / "FeatureSlices" / "CoreRuntimeFeatures" / "AsyncAndThreading" / "ThreadLocalStateProof.cs"
GC_SOURCE_PATH = SUBJECT_ROOT / "source" / "FeatureSlices" / "CoreRuntimeFeatures" / "RuntimeServices" / "GcSensitiveFlowProof.cs"
PROOF_CMAKE_PATH = SUBJECT_ROOT / "validation" / "proof" / "native-reference" / "CMakeLists.txt"
PROOF_MAIN_PATH = SUBJECT_ROOT / "validation" / "proof" / "native-reference" / "main.cpp"
PROOF_RUN_SCRIPT_PATH = SUBJECT_ROOT / "validation" / "proof" / "native-reference" / "RunNativeReferenceProof.cmake"

CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
SEMANTIC_WORLD_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.SemanticWorld" / "SemanticWorldStage.cs"
LINKER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.cs"
LOWERING_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceLoweringPlanner.cs"
EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"
TEMPLATE_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.CodeGen"
    / "Templates"
    / "NativeReferenceProof.ThreadingThreadStaticMonitor.cpp.scriban"
)
RUNTIME_CORE_HEADER_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.h"
RUNTIME_CORE_SOURCE_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.cpp"


class Phase2ThreadingGcProofTests(unittest.TestCase):
    def test_threading_proof_subject_tree_realizes_phase2_batch3_slice(self) -> None:
        self.assertTrue(MANIFEST_PATH.is_file(), msg=f"missing subject manifest: {MANIFEST_PATH}")
        self.assertTrue(SOURCE_PROJECT_PATH.is_file(), msg=f"missing source project: {SOURCE_PROJECT_PATH}")
        self.assertTrue(THREADING_SOURCE_PATH.is_file(), msg=f"missing source file: {THREADING_SOURCE_PATH}")
        self.assertTrue(MONITOR_SOURCE_PATH.is_file(), msg=f"missing source file: {MONITOR_SOURCE_PATH}")
        self.assertTrue(THREAD_LOCAL_SOURCE_PATH.is_file(), msg=f"missing source file: {THREAD_LOCAL_SOURCE_PATH}")
        self.assertTrue(GC_SOURCE_PATH.is_file(), msg=f"missing source file: {GC_SOURCE_PATH}")
        self.assertTrue(PROOF_CMAKE_PATH.is_file(), msg=f"missing proof cmake: {PROOF_CMAKE_PATH}")
        self.assertTrue(PROOF_MAIN_PATH.is_file(), msg=f"missing proof host main: {PROOF_MAIN_PATH}")
        self.assertTrue(PROOF_RUN_SCRIPT_PATH.is_file(), msg=f"missing proof run script: {PROOF_RUN_SCRIPT_PATH}")

        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        threading_source = THREADING_SOURCE_PATH.read_text(encoding="utf-8")
        monitor_source = MONITOR_SOURCE_PATH.read_text(encoding="utf-8")
        thread_local_source = THREAD_LOCAL_SOURCE_PATH.read_text(encoding="utf-8")
        gc_source = GC_SOURCE_PATH.read_text(encoding="utf-8")

        self.assertEqual("SolutionCorePack", manifest["subjectId"])
        self.assertEqual("dotnet-project", manifest["source"]["type"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", manifest["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Launcher/SolutionCorePack.csproj",
            manifest["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeFeatures/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("proof", manifest["validation"]["proof"]["kind"])

        self.assertIn("[ChaosUnitTest(", threading_source)
        self.assertIn('Alias = "threading-proof"', threading_source)
        self.assertIn("Capability = ChaosCapabilityItem.Threading", threading_source)
        self.assertIn("new Thread(", threading_source)
        self.assertIn("thread.Start();", threading_source)
        self.assertIn("thread.Join();", threading_source)
        self.assertIn("internal static class ThreadingProofEntry", threading_source)

        self.assertIn("[ChaosUnitTest(", monitor_source)
        self.assertIn('Alias = "monitor-locking-proof"', monitor_source)
        self.assertIn("lock (Gate)", monitor_source)
        self.assertIn("Monitor.TryEnter(", monitor_source)
        self.assertIn("internal static class MonitorAndLockingProofEntry", monitor_source)

        self.assertIn("[ChaosUnitTest(", thread_local_source)
        self.assertIn('Alias = "thread-local-state-proof"', thread_local_source)
        self.assertIn("[ThreadStatic]", thread_local_source)
        self.assertIn("workerBefore = ThreadLabel;", thread_local_source)
        self.assertIn("internal static class ThreadLocalStateProofEntry", thread_local_source)

        self.assertIn("[ChaosUnitTest(", gc_source)
        self.assertIn('Alias = "gc-sensitive-flow-proof"', gc_source)
        self.assertIn("GC.Collect();", gc_source)
        self.assertIn("GC.WaitForPendingFinalizers();", gc_source)
        self.assertIn("internal static class GcSensitiveFlowProofEntry", gc_source)

    def test_loader_semantic_and_linker_lock_threading_surface(self) -> None:
        contracts_source = CONTRACTS_PATH.read_text(encoding="utf-8")
        loader_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")
        semantic_world_source = SEMANTIC_WORLD_PATH.read_text(encoding="utf-8")
        linker_source = LINKER_STAGE_PATH.read_text(encoding="utf-8")

        self.assertIn("public bool IsThreadStatic { get; init; }", contracts_source)
        self.assertIn("fieldDefinition.Attributes.HasFlag(FieldAttributes.Static)", loader_source)
        self.assertIn("System.ThreadStaticAttribute", loader_source)
        self.assertIn("requires-thread-static-storage", semantic_world_source)
        self.assertIn("requires-monitor-enter-exit", semantic_world_source)
        self.assertIn("IncludeThreadStaticFieldClosure(", linker_source)
        self.assertIn("monitor-enter-exit", linker_source)

    def test_codegen_and_runtime_core_freeze_threading_gc_proof_surface(self) -> None:
        planner_source = LOWERING_PLANNER_PATH.read_text(encoding="utf-8")
        emitter_source = EMITTER_PATH.read_text(encoding="utf-8")
        runtime_header_source = RUNTIME_CORE_HEADER_PATH.read_text(encoding="utf-8")
        runtime_source = RUNTIME_CORE_SOURCE_PATH.read_text(encoding="utf-8")

        self.assertTrue(TEMPLATE_PATH.is_file(), msg=f"missing threading template: {TEMPLATE_PATH}")
        template_source = TEMPLATE_PATH.read_text(encoding="utf-8")

        self.assertIn("managed-threading.threadstatic-monitor.minimal", planner_source)
        self.assertIn("MatchesThreadingThreadStaticMonitorCandidate(", planner_source)
        self.assertIn("TryCreateThreadingThreadStaticMonitorLoweringPlan(", planner_source)
        self.assertIn("managed-threading.threadstatic-monitor.minimal", emitter_source)
        self.assertIn("case ManagedThreadingThreadStaticMonitorMinimal:", emitter_source)

        self.assertIn("ThreadStaticInt32Add(", runtime_header_source)
        self.assertIn("MonitorEnter(", runtime_header_source)
        self.assertIn("MonitorExit(", runtime_header_source)
        self.assertIn("GcSafepoint(", runtime_header_source)
        self.assertIn("ReportThreadRoot(", runtime_header_source)
        self.assertIn("EnqueueFinalizer(", runtime_header_source)
        self.assertIn("DrainFinalizerQueue(", runtime_header_source)

        self.assertIn("ThreadStaticInt32Add(", runtime_source)
        self.assertIn("MonitorEnter(", runtime_source)
        self.assertIn("MonitorExit(", runtime_source)
        self.assertIn("GcSafepoint(", runtime_source)
        self.assertIn("ReportThreadRoot(", runtime_source)
        self.assertIn("EnqueueFinalizer(", runtime_source)
        self.assertIn("DrainFinalizerQueue(", runtime_source)

        self.assertIn("ThreadStaticInt32Add(", template_source)
        self.assertIn("MonitorEnter(", template_source)
        self.assertIn("MonitorExit(", template_source)
        self.assertIn("GcSafepoint(", template_source)
        self.assertIn("ReportThreadRoot(", template_source)
        self.assertIn("EnqueueFinalizer(", template_source)
        self.assertIn("DrainFinalizerQueue(", template_source)


if __name__ == "__main__":
    unittest.main()
