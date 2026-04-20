from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import load_module


REPO_ROOT = Path(__file__).resolve().parents[3]
TOOLING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "tooling.py"
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
NATIVE_REFERENCE_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeReferenceProofEmitter.cs"
ARTIFACT_MODELS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureArtifactModels.cs"
LIBRARY_PROJECT_PATH = (
    REPO_ROOT
    / "subjects"
    / "SolutionCorePack"
    / "source"
    / "EngineeringScenarios"
    / "SimpleLibrarySolution"
    / "Library"
    / "GoldenSimpleLib.Library.csproj"
)
LIBRARY_DLL_PATH = (
    REPO_ROOT
    / "subjects"
    / "SolutionCorePack"
    / "source"
    / "EngineeringScenarios"
    / "SimpleLibrarySolution"
    / "Library"
    / "bin"
    / "Release"
    / "net8.0"
    / "GoldenSimpleLib.Library.dll"
)
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "full-assembly-closure-codegen-audit"


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


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


class FullAssemblyClosureCodegenAuditPlanTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = TEST_OUTPUT_ROOT / f"GoldenSimpleLib.Library-{uuid.uuid4().hex}"
        cls.native_reference_output_root = TEST_OUTPUT_ROOT / f"GoldenSimpleLib.Library-native-reference-{uuid.uuid4().hex}"
        cls.native_aot_output_root = TEST_OUTPUT_ROOT / f"GoldenSimpleLib.Library-native-aot-{uuid.uuid4().hex}"
        cls.bundle_generated = False

    def _ensure_bundle_generated(self) -> None:
        if self.__class__.bundle_generated:
            return

        if self.output_root.exists():
            shutil.rmtree(self.output_root)

        tooling_module = load_module(TOOLING_MODULE_PATH, f"chaos_full_closure_codegen_audit_{uuid.uuid4().hex}")
        driver_intermediate_root = tooling_module.allocate_dotnet_intermediate_dir("Chaos.IL2CPP.Driver", host_platform="windows")
        self.assertIsNotNone(driver_intermediate_root)

        run_checked(
            [
                "dotnet",
                "build",
                str(LIBRARY_PROJECT_PATH),
                "-c",
                "Release",
            ],
            cwd=REPO_ROOT,
        )
        self.assertTrue(LIBRARY_DLL_PATH.is_file(), msg=f"missing library dll: {LIBRARY_DLL_PATH}")

        run_checked(
            [
                "dotnet",
                "build",
                str(DRIVER_PROJECT_PATH),
                "-c",
                "Release",
                "-m:1",
                f"-p:ChaosTempIntermediateRoot={Path(driver_intermediate_root).as_posix()}/",
            ],
            cwd=REPO_ROOT,
        )

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(LIBRARY_DLL_PATH),
                str(self.output_root),
                "--full-assembly-closure",
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.bundle_generated = True

    def test_full_assembly_closure_generates_assembly_bound_codegen_runtime_and_audit_plans(self) -> None:
        self._ensure_bundle_generated()

        closure_manifest = load_json(self.output_root / "closure.manifest.json")
        native_reference_plan = load_json(self.output_root / "native-reference.lowering-plan.json")
        native_aot_plan = load_json(self.output_root / "native-aot.lowering-plan.json")

        self.assertEqual("GoldenSimpleLib.Library", closure_manifest["assemblyName"])
        self.assertEqual("", closure_manifest["entrySubjectId"])
        self.assertTrue(closure_manifest["fullAssemblyClosure"])

        self.assertEqual("assembly-full-closure-runtime-skeleton", native_reference_plan["planKind"])
        self.assertEqual("GoldenSimpleLib.Library", native_reference_plan["assemblyName"])
        self.assertEqual("", native_reference_plan["entrySubjectId"])
        self.assertEqual("runtime-skeleton", native_reference_plan["translationUnitMode"])
        self.assertEqual("assembly-bound-native-reference-skeleton", native_reference_plan["runtimeExecutionKind"])
        self.assertGreaterEqual(len(native_reference_plan["translationUnitMethodSubjectIds"]), 1)
        self.assertEqual(1024, native_reference_plan["translationUnitPageSize"])
        self.assertEqual(1, native_reference_plan["translationUnitPageCount"])
        self.assertEqual(1, len(native_reference_plan["translationUnitPages"]))
        self.assertEqual(1, native_reference_plan["translationUnitPages"][0]["pageNumber"])
        self.assertEqual(1, native_reference_plan["translationUnitPages"][0]["methodCount"])
        self.assertEqual("generated/runtime/native-reference.runtime-skeleton.page-0001.cpp", native_reference_plan["translationUnitPages"][0]["path"])
        self.assertIn(
            "GoldenSimpleLib.Library/Greeter::BuildMessage:System.String()",
            native_reference_plan["translationUnitMethodSubjectIds"],
        )
        self.assertNotEqual("analysis_only_entry", native_reference_plan["entrySymbol"])

        self.assertEqual("assembly-full-closure-audit", native_aot_plan["planKind"])
        self.assertEqual("GoldenSimpleLib.Library", native_aot_plan["assemblyName"])
        self.assertEqual("", native_aot_plan["entrySubjectId"])
        self.assertEqual("audit-only", native_aot_plan["translationUnitMode"])
        self.assertGreaterEqual(len(native_aot_plan["translationUnitMethodSubjectIds"]), 1)
        self.assertEqual(1024, native_aot_plan["translationUnitPageSize"])
        self.assertEqual(1, native_aot_plan["translationUnitPageCount"])
        self.assertEqual(1, len(native_aot_plan["translationUnitPages"]))
        self.assertEqual(1, native_aot_plan["translationUnitPages"][0]["pageNumber"])
        self.assertEqual(1, native_aot_plan["translationUnitPages"][0]["methodCount"])
        self.assertEqual("generated/audit/native-aot.methods.page-0001.cpp", native_aot_plan["translationUnitPages"][0]["path"])
        self.assertIn(
            "GoldenSimpleLib.Library/Greeter::BuildMessage:System.String()",
            native_aot_plan["translationUnitMethodSubjectIds"],
        )
        self.assertNotEqual("analysis_only_entry", native_aot_plan["entrySymbol"])

    def test_emit_native_reference_generates_runtime_skeleton_translation_unit(self) -> None:
        self._ensure_bundle_generated()

        if self.native_reference_output_root.exists():
            shutil.rmtree(self.native_reference_output_root)

        completed = subprocess.run(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-reference",
                str(self.output_root),
                str(self.native_reference_output_root),
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            check=False,
        )

        self.assertEqual(0, completed.returncode)
        generated_cpp = (
            self.native_reference_output_root
            / "generated"
            / "native-reference.generated.cpp"
        ).read_text(encoding="utf-8")
        generated_page_cpp = (
            self.native_reference_output_root
            / "generated"
            / "runtime"
            / "native-reference.runtime-skeleton.page-0001.cpp"
        ).read_text(encoding="utf-8")
        manifest = load_json(self.native_reference_output_root / "native-reference.manifest.json")

        self.assertIn("assembly-full-closure-runtime-skeleton", generated_cpp)
        self.assertIn("RunNativeReferenceAssembly", generated_cpp)
        self.assertIn("kPageDispatchCatalog", generated_cpp)
        self.assertIn("NativeReferenceAssemblyDispatchRequest", generated_cpp)
        self.assertIn("DispatchAssemblySubject", generated_cpp)
        self.assertIn("DispatchRuntimeSkeletonPage0001", generated_cpp)
        self.assertIn("CHAOS_BRIDGE_STATUS_NOT_FOUND", generated_cpp)
        self.assertIn("translation_unit_method_count = 1", generated_cpp)
        self.assertIn("translation_unit_page_count = 1", generated_cpp)
        self.assertIn("GoldenSimpleLib.Library/Greeter::BuildMessage:System.String()", generated_page_cpp)
        self.assertIn("kPageMethodDispatch", generated_page_cpp)
        self.assertIn("NativeReferenceStub_", generated_page_cpp)
        self.assertIn("DispatchRuntimeSkeletonPage0001", generated_page_cpp)
        self.assertIn("std::strcmp(entry.subject_id, subject_id) == 0", generated_page_cpp)
        self.assertIn("CHAOS_BRIDGE_STATUS_NOT_SUPPORTED", generated_page_cpp)
        self.assertEqual("GoldenSimpleLib.Library", manifest["assemblyName"])
        self.assertEqual("", manifest["entrySubjectId"])
        self.assertEqual("assembly-bound-native-reference-skeleton", manifest["runtimeExecutionKind"])
        self.assertEqual("GoldenSimpleLib.Library/Greeter::BuildMessage:System.String()", manifest["preferredAssemblyDispatchSubjectId"])
        self.assertEqual(1024, manifest["translationUnitPageSize"])
        self.assertEqual(1, manifest["translationUnitPageCount"])
        self.assertEqual(1, len(manifest["translationUnitPages"]))
        self.assertEqual("generated/runtime/native-reference.runtime-skeleton.page-0001.cpp", manifest["translationUnitPages"][0]["path"])

    def test_emit_native_aot_generates_auditable_translation_unit(self) -> None:
        self._ensure_bundle_generated()

        if self.native_aot_output_root.exists():
            shutil.rmtree(self.native_aot_output_root)

        completed = subprocess.run(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.output_root),
                str(self.native_aot_output_root),
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="replace",
            check=False,
        )

        self.assertEqual(0, completed.returncode)
        generated_cpp = (
            self.native_aot_output_root
            / "generated"
            / "native-aot.generated.cpp"
        ).read_text(encoding="utf-8")
        generated_page_cpp = (
            self.native_aot_output_root
            / "generated"
            / "audit"
            / "native-aot.methods.page-0001.cpp"
        ).read_text(encoding="utf-8")
        manifest = load_json(self.native_aot_output_root / "native-aot.manifest.json")

        self.assertIn("assembly-full-closure-audit", generated_cpp)
        self.assertIn("translation_unit_method_count = 1", generated_cpp)
        self.assertIn("translation_unit_page_count = 1", generated_cpp)
        self.assertIn("GoldenSimpleLib.Library/Greeter::BuildMessage:System.String()", generated_page_cpp)
        self.assertEqual("GoldenSimpleLib.Library", manifest["assemblyName"])
        self.assertEqual("", manifest["entrySubjectId"])
        self.assertEqual(1024, manifest["translationUnitPageSize"])
        self.assertEqual(1, manifest["translationUnitPageCount"])
        self.assertEqual(1, len(manifest["translationUnitPages"]))
        self.assertEqual("generated/audit/native-aot.methods.page-0001.cpp", manifest["translationUnitPages"][0]["path"])

    def test_native_reference_runtime_skeleton_shape_is_implemented_for_assembly_bound_plan(self) -> None:
        emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        artifact_models_source = ARTIFACT_MODELS_PATH.read_text(encoding="utf-8")

        required_fragments = [
            'assembly-full-closure-runtime-skeleton',
            'runtime-skeleton',
            'RunNativeReferenceAssembly',
            'RuntimeExecutionKind',
        ]

        codegen_stage_source = (REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "CodeGenStage.cs").read_text(encoding="utf-8")
        combined_source = emitter_source + "\n" + artifact_models_source + "\n" + codegen_stage_source
        for fragment in required_fragments:
            self.assertIn(fragment, combined_source)

        self.assertIn('generated/runtime/native-reference.runtime-skeleton', codegen_stage_source)


if __name__ == "__main__":
    unittest.main()
