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
    / "verification"
    / "catalog"
    / "scenarios"
    / "SolutionCorePack"
    / "SimpleLibrarySolution"
    / "Library"
    / "GoldenSimpleLib.Library.csproj"
)
LIBRARY_DLL_PATH = (
    REPO_ROOT
    / "verification"
    / "catalog"
    / "scenarios"
    / "SolutionCorePack"
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
        self.assertEqual("generated/audit/native-aot.audit.page-0001.json", native_aot_plan["translationUnitPages"][0]["path"])
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
            / "runtime"
            / "native-reference.runtime-skeleton.generated.cpp"
        ).read_text(encoding="utf-8")
        generated_page_cpp = (
            self.native_reference_output_root
            / "generated"
            / "runtime"
            / "native-reference.runtime-skeleton.page-0001.cpp"
        ).read_text(encoding="utf-8")
        coverage_report = load_json(
            self.native_reference_output_root
            / "generated"
            / "runtime"
            / "native-reference.runtime-skeleton.coverage.json"
        )
        manifest = load_json(self.native_reference_output_root / "native-reference.manifest.json")

        self.assertIn("assembly-full-closure-runtime-skeleton", generated_cpp)
        self.assertIn("RunNativeReferenceAssembly", generated_cpp)
        self.assertIn("kMethodDispatchCatalog", generated_cpp)
        self.assertIn("NativeReferenceAssemblyDispatchRequest", generated_cpp)
        self.assertIn("FindMethodDispatchCatalogEntry", generated_cpp)
        self.assertIn("DispatchAssemblySubject", generated_cpp)
        self.assertIn("DispatchRuntimeSkeletonPage0001", generated_cpp)
        self.assertIn("CHAOS_BRIDGE_STATUS_NOT_FOUND", generated_cpp)
        self.assertIn("translation_unit_method_count = 1", generated_cpp)
        self.assertIn("translation_unit_page_count = 1", generated_cpp)
        self.assertIn("method_slot", generated_cpp)
        self.assertIn("GoldenSimpleLib.Library/Greeter::BuildMessage:System.String()", generated_cpp)
        self.assertIn("kPageMethodDispatch", generated_page_cpp)
        self.assertIn("NativeReferenceStub_Page0001_Item0001", generated_page_cpp)
        self.assertIn("DispatchRuntimeSkeletonPage0001", generated_page_cpp)
        self.assertIn("ResolveRuntimeSkeletonFieldBinding", generated_page_cpp)
        self.assertEqual("GoldenSimpleLib.Library", manifest["assemblyName"])
        self.assertEqual("", manifest["entrySubjectId"])
        self.assertEqual("assembly-bound-native-reference-skeleton", manifest["runtimeExecutionKind"])
        self.assertEqual("GoldenSimpleLib.Library/Greeter::BuildMessage:System.String()", manifest["preferredAssemblyDispatchSubjectId"])
        self.assertEqual(1024, manifest["translationUnitPageSize"])
        self.assertEqual(1, manifest["translationUnitPageCount"])
        self.assertEqual(1, len(manifest["translationUnitPages"]))
        self.assertEqual("generated/runtime/native-reference.runtime-skeleton.page-0001.cpp", manifest["translationUnitPages"][0]["path"])
        self.assertIn(
            {"kind": "runtimeSkeletonCoverageReport", "path": "generated/runtime/native-reference.runtime-skeleton.coverage.json"},
            manifest["generatedArtifacts"],
        )
        self.assertIn(
            {"kind": "generatedTranslationUnit", "path": "generated/runtime/native-reference.runtime-skeleton.generated.cpp"},
            manifest["generatedArtifacts"],
        )
        self.assertEqual("nativeReferenceRuntimeSkeletonCoverage", coverage_report["artifactKind"])
        self.assertEqual(1, coverage_report["requestedMethodCount"])
        self.assertEqual(1, coverage_report["emittedMethodCount"])
        self.assertEqual(0, coverage_report["uncoveredMethodCount"])
        self.assertEqual([], coverage_report["uncoveredMethodSubjectIds"])

    def test_emit_native_reference_reports_runtime_skeleton_coverage_for_fixture_methods(self) -> None:
        self._ensure_bundle_generated()

        fixture_root = TEST_OUTPUT_ROOT / f"FixtureUnsupportedLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureUnsupportedLibrary"
        project_path = project_root / "FixtureUnsupportedLibrary.csproj"
        source_path = project_root / "Arithmetic.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureUnsupportedLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "namespace FixtureUnsupportedLibrary;\n\n"
                    "public static class Arithmetic\n"
                    "{\n"
                    "    public static int Add(int left, int right)\n"
                    "    {\n"
                    "        return left + right;\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing unsupported library dll: {dll_path}")

            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )
            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            lowering_plan = load_json(emit_root / "native-reference.plan.json")
            generated_cpp = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.generated.cpp"
            ).read_text(encoding="utf-8")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )
            manifest = load_json(emit_root / "native-reference.manifest.json")

            self.assertEqual("assembly-full-closure-runtime-skeleton", lowering_plan["planKind"])
            self.assertEqual(1, lowering_plan["translationUnitMethodCount"])
            self.assertEqual(1, lowering_plan["translationUnitPageCount"])
            self.assertEqual(1, len(lowering_plan["translationUnitPages"]))
            self.assertIn("translation_unit_method_count = 1", generated_cpp)
            self.assertIn("translation_unit_page_count = 1", generated_cpp)
            self.assertIn("kMethodDispatchCatalogCount = sizeof(kMethodDispatchCatalog)", generated_cpp)
            self.assertIn("DispatchRuntimeSkeletonPage0001", generated_cpp)
            self.assertEqual("FixtureUnsupportedLibrary", manifest["assemblyName"])
            self.assertEqual("", manifest["entrySubjectId"])
            self.assertEqual("assembly-bound-native-reference-skeleton", manifest["runtimeExecutionKind"])
            self.assertEqual(
                "FixtureUnsupportedLibrary/Arithmetic::Add:System.Int32(System.Int32,System.Int32)",
                manifest["preferredAssemblyDispatchSubjectId"],
            )
            self.assertEqual(1024, manifest["translationUnitPageSize"])
            self.assertEqual(1, manifest["translationUnitPageCount"])
            self.assertEqual(1, len(manifest["translationUnitPages"]))
            self.assertEqual(
                [
                    {"kind": "generatedTranslationUnit", "path": "generated/runtime/native-reference.runtime-skeleton.generated.cpp"},
                    {"kind": "generatedTranslationUnit", "path": "generated/runtime/native-reference.runtime-skeleton.page-0001.cpp"},
                    {"kind": "runtimeSkeletonCoverageReport", "path": "generated/runtime/native-reference.runtime-skeleton.coverage.json"},
                    {"kind": "codegenMetrics", "path": "native-reference.codegen-metrics.json"},
                ],
                manifest["generatedArtifacts"],
            )
            self.assertEqual("nativeReferenceRuntimeSkeletonCoverage", coverage_report["artifactKind"])
            self.assertEqual(1, coverage_report["requestedMethodCount"])
            self.assertIsInstance(coverage_report["emittedMethodCount"], int)
            self.assertIsInstance(coverage_report["uncoveredMethodCount"], int)
            self.assertIsInstance(coverage_report["uncoveredReasonCounts"], dict)
            self.assertIsInstance(coverage_report["uncoveredMethods"], list)
            self.assertIsInstance(coverage_report["uncoveredMethodSubjectIds"], list)
            for uncovered_method in coverage_report["uncoveredMethods"]:
                self.assertIn("subjectId", uncovered_method)
                self.assertIn("reasonCode", uncovered_method)
                self.assertIn("canonicalSubjectId", uncovered_method)
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_emit_native_reference_supports_static_primitive_convert_runtime_skeleton_methods(self) -> None:
        fixture_root = TEST_OUTPUT_ROOT / f"FixturePrimitiveConvertLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixturePrimitiveConvertLibrary"
        project_path = project_root / "FixturePrimitiveConvertLibrary.csproj"
        source_path = project_root / "PrimitiveConvertOps.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixturePrimitiveConvertLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "using System;\n\n"
                    "namespace FixturePrimitiveConvertLibrary;\n\n"
                    "internal static class ResourceShim\n"
                    "{\n"
                    "    public static string GetResourceString(string key)\n"
                    "    {\n"
                    "        return key;\n"
                    "    }\n\n"
                    "    public static string get_Overflow_Byte()\n"
                    "    {\n"
                    '        return GetResourceString("Overflow_Byte");\n'
                    "    }\n"
                    "}\n\n"
                    "public static class PrimitiveConvertOps\n"
                    "{\n"
                    "    public static byte ToByte(bool value)\n"
                    "    {\n"
                    "        return value ? (byte)1 : (byte)0;\n"
                    "    }\n\n"
                    "    public static bool ToBoolean(byte value)\n"
                    "    {\n"
                    "        return value > 0;\n"
                    "    }\n\n"
                    "    public static bool ToBoolean(float value)\n"
                    "    {\n"
                    "        return value != 0f;\n"
                    "    }\n\n"
                    "    public static bool ToBoolean(double value)\n"
                    "    {\n"
                    "        return value != 0d;\n"
                    "    }\n"
                    "\n"
                    "    public static long ToInt64(int value)\n"
                    "    {\n"
                    "        return value;\n"
                    "    }\n\n"
                    "    public static ulong ToUInt64(uint value)\n"
                    "    {\n"
                    "        return value;\n"
                    "    }\n\n"
                    "    public static float ToSingle(int value)\n"
                    "    {\n"
                    "        return value;\n"
                    "    }\n\n"
                    "    public static double ToDouble(uint value)\n"
                    "    {\n"
                    "        return value;\n"
                    "    }\n\n"
                    "    public static char ToChar(byte value)\n"
                    "    {\n"
                    "        return (char)value;\n"
                    "    }\n\n"
                    "    public static void ThrowByteOverflowException()\n"
                    "    {\n"
                    "        throw new OverflowException(ResourceShim.get_Overflow_Byte());\n"
                    "    }\n\n"
                    "    public static byte ToByte(uint value)\n"
                    "    {\n"
                    "        if (value > byte.MaxValue)\n"
                    "        {\n"
                    "            ThrowByteOverflowException();\n"
                    "        }\n\n"
                    "        return (byte)value;\n"
                    "    }\n\n"
                    "    public static byte ToByte(ulong value)\n"
                    "    {\n"
                    "        if (value > byte.MaxValue)\n"
                    "        {\n"
                    "            ThrowByteOverflowException();\n"
                    "        }\n\n"
                    "        return (byte)value;\n"
                    "    }\n\n"
                    "    public static byte ToByte(int value)\n"
                    "    {\n"
                    "        return ToByte((uint)value);\n"
                    "    }\n\n"
                    "    public static byte ToByte(long value)\n"
                    "    {\n"
                    "        return ToByte((ulong)value);\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            tooling_module = load_module(TOOLING_MODULE_PATH, f"chaos_primitive_convert_codegen_audit_{uuid.uuid4().hex}")
            driver_intermediate_root = tooling_module.allocate_dotnet_intermediate_dir("Chaos.IL2CPP.Driver", host_platform="windows")
            self.assertIsNotNone(driver_intermediate_root)

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing primitive convert library dll: {dll_path}")

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
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )
            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            generated_page = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.page-0001.cpp"
            ).read_text(encoding="utf-8").replace("\r\n", "\n")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )

            self.assertGreaterEqual(coverage_report["requestedMethodCount"], 12)
            self.assertEqual(coverage_report["requestedMethodCount"], coverage_report["emittedMethodCount"])
            self.assertEqual(0, coverage_report["uncoveredMethodCount"])
            self.assertEqual([], coverage_report["uncoveredMethodSubjectIds"])
            self.assertIn("NativeReferenceStub_Page0001_Item0001", generated_page)
            self.assertIn("NativeReferenceStub_Page0001_Item0002", generated_page)
            self.assertIn("NativeReferenceStub_Page0001_Item0003", generated_page)
            self.assertIn("NativeReferenceStub_Page0001_Item0004", generated_page)
            self.assertIn("std::uint8_t value;", generated_page)
            self.assertIn("bool value;", generated_page)
            self.assertIn("float value;", generated_page)
            self.assertIn("double value;", generated_page)
            self.assertIn("std::int32_t value;", generated_page)
            self.assertIn("std::uint32_t value;", generated_page)
            self.assertIn(
                "*request->return_value = static_cast<std::uint8_t>(request->value ? static_cast<std::uint8_t>(1) : static_cast<std::uint8_t>(0));",
                generated_page,
            )
            self.assertIn(
                "*request->return_value = static_cast<bool>(request->value != static_cast<std::uint8_t>(0));",
                generated_page,
            )
            self.assertIn(
                "*request->return_value = static_cast<bool>(request->value);",
                generated_page,
            )
            self.assertIn(
                "*request->return_value = static_cast<std::int64_t>(request->value);",
                generated_page,
            )
            self.assertIn(
                "*request->return_value = static_cast<std::uint64_t>(request->value);",
                generated_page,
            )
            self.assertIn(
                "*request->return_value = static_cast<float>(request->value);",
                generated_page,
            )
            self.assertIn(
                "*request->return_value = static_cast<double>(request->value);",
                generated_page,
            )
            self.assertIn(
                "*request->return_value = static_cast<std::uint16_t>(request->value);",
                generated_page,
            )
            self.assertIn('"Overflow_Byte"', generated_page)
            self.assertIn("return NativeReferenceStub_Page0001_Item", generated_page)
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_emit_native_reference_supports_checked_char_runtime_skeleton_methods(self) -> None:
        fixture_root = TEST_OUTPUT_ROOT / f"FixtureCheckedCharConvertLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureCheckedCharConvertLibrary"
        project_path = project_root / "FixtureCheckedCharConvertLibrary.csproj"
        source_path = project_root / "CheckedCharConvertOps.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureCheckedCharConvertLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "using System;\n\n"
                    "namespace FixtureCheckedCharConvertLibrary;\n\n"
                    "internal static class ResourceShim\n"
                    "{\n"
                    "    public static string GetResourceString(string key)\n"
                    "    {\n"
                    "        return key;\n"
                    "    }\n\n"
                    "    public static string get_Overflow_Char()\n"
                    "    {\n"
                    '        return GetResourceString("Overflow_Char");\n'
                    "    }\n"
                    "}\n\n"
                    "public static class CheckedCharConvertOps\n"
                    "{\n"
                    "    public static void ThrowCharOverflowException()\n"
                    "    {\n"
                    "        throw new OverflowException(ResourceShim.get_Overflow_Char());\n"
                    "    }\n\n"
                    "    public static char ToChar(sbyte value)\n"
                    "    {\n"
                    "        if (value < 0)\n"
                    "        {\n"
                    "            ThrowCharOverflowException();\n"
                    "        }\n\n"
                    "        return (char)value;\n"
                    "    }\n\n"
                    "    public static char ToChar(short value)\n"
                    "    {\n"
                    "        if (value < 0)\n"
                    "        {\n"
                    "            ThrowCharOverflowException();\n"
                    "        }\n\n"
                    "        return (char)value;\n"
                    "    }\n\n"
                    "    public static char ToChar(ushort value)\n"
                    "    {\n"
                    "        return (char)value;\n"
                    "    }\n\n"
                    "    public static char ToChar(uint value)\n"
                    "    {\n"
                    "        if (value > char.MaxValue)\n"
                    "        {\n"
                    "            ThrowCharOverflowException();\n"
                    "        }\n\n"
                    "        return (char)value;\n"
                    "    }\n\n"
                    "    public static char ToChar(int value)\n"
                    "    {\n"
                    "        return ToChar((uint)value);\n"
                    "    }\n\n"
                    "    public static char ToChar(ulong value)\n"
                    "    {\n"
                    "        if (value > char.MaxValue)\n"
                    "        {\n"
                    "            ThrowCharOverflowException();\n"
                    "        }\n\n"
                    "        return (char)value;\n"
                    "    }\n\n"
                    "    public static char ToChar(long value)\n"
                    "    {\n"
                    "        return ToChar((ulong)value);\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            tooling_module = load_module(TOOLING_MODULE_PATH, f"chaos_checked_char_codegen_audit_{uuid.uuid4().hex}")
            driver_intermediate_root = tooling_module.allocate_dotnet_intermediate_dir("Chaos.IL2CPP.Driver", host_platform="windows")
            self.assertIsNotNone(driver_intermediate_root)

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing checked char convert library dll: {dll_path}")

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
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )

            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            generated_page = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.page-0001.cpp"
            ).read_text(encoding="utf-8").replace("\r\n", "\n")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )

            self.assertGreaterEqual(coverage_report["requestedMethodCount"], 10)
            self.assertEqual(coverage_report["requestedMethodCount"], coverage_report["emittedMethodCount"])
            self.assertEqual(0, coverage_report["uncoveredMethodCount"])
            self.assertEqual([], coverage_report["uncoveredMethodSubjectIds"])
            self.assertIn("std::int8_t value;", generated_page)
            self.assertIn("std::int16_t value;", generated_page)
            self.assertIn("std::int32_t value;", generated_page)
            self.assertIn("std::int64_t value;", generated_page)
            self.assertIn("std::uint16_t value;", generated_page)
            self.assertIn("std::uint32_t value;", generated_page)
            self.assertIn("std::uint64_t value;", generated_page)
            self.assertIn("std::uint16_t* return_value;", generated_page)
            self.assertIn(
                "*request->return_value = static_cast<std::uint16_t>(request->value);",
                generated_page,
            )
            self.assertIn(
                "request->value < static_cast<std::int8_t>(0)",
                generated_page,
            )
            self.assertIn(
                "request->value < static_cast<std::int16_t>(0)",
                generated_page,
            )
            self.assertIn(
                "request->value > static_cast<std::uint32_t>(65535)",
                generated_page,
            )
            self.assertIn(
                "request->value > static_cast<std::uint64_t>(65535)",
                generated_page,
            )
            self.assertIn('"Overflow_Char"', generated_page)
            self.assertIn("return NativeReferenceStub_Page0001_Item", generated_page)
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_emit_native_reference_supports_boxed_iconvertible_char_invalid_cast_runtime_skeleton_methods(self) -> None:
        fixture_root = TEST_OUTPUT_ROOT / f"FixtureBoxedIConvertibleCharLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureBoxedIConvertibleCharLibrary"
        project_path = project_root / "FixtureBoxedIConvertibleCharLibrary.csproj"
        source_path = project_root / "FloatingCharOps.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureBoxedIConvertibleCharLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "using System;\n\n"
                    "namespace FixtureBoxedIConvertibleCharLibrary;\n\n"
                    "public static class FloatingCharOps\n"
                    "{\n"
                    "    public static char ToChar(bool value)\n"
                    "    {\n"
                    "        return ((IConvertible)value).ToChar(null);\n"
                    "    }\n\n"
                    "    public static char ToChar(float value)\n"
                    "    {\n"
                    "        return ((IConvertible)value).ToChar(null);\n"
                    "    }\n\n"
                    "    public static char ToChar(double value)\n"
                    "    {\n"
                    "        return ((IConvertible)value).ToChar(null);\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            tooling_module = load_module(
                TOOLING_MODULE_PATH,
                f"chaos_boxed_iconvertible_char_codegen_audit_{uuid.uuid4().hex}",
            )
            driver_intermediate_root = tooling_module.allocate_dotnet_intermediate_dir(
                "Chaos.IL2CPP.Driver",
                host_platform="windows",
            )
            self.assertIsNotNone(driver_intermediate_root)

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing boxed iconvertible char library dll: {dll_path}")

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
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )
            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            generated_page = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.page-0001.cpp"
            ).read_text(encoding="utf-8").replace("\r\n", "\n")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )

            for required_subject_id in [
                "FixtureBoxedIConvertibleCharLibrary/FloatingCharOps::ToChar:System.Char(System.Boolean)",
                "FixtureBoxedIConvertibleCharLibrary/FloatingCharOps::ToChar:System.Char(System.Single)",
                "FixtureBoxedIConvertibleCharLibrary/FloatingCharOps::ToChar:System.Char(System.Double)",
            ]:
                self.assertNotIn(required_subject_id, coverage_report["uncoveredMethodSubjectIds"])

            self.assertIn("bool value;", generated_page)
            self.assertIn("float value;", generated_page)
            self.assertIn("double value;", generated_page)
            self.assertIn("std::uint16_t* return_value;", generated_page)
            self.assertIn('"Boolean"', generated_page)
            self.assertIn('"Single"', generated_page)
            self.assertIn('"Double"', generated_page)
            self.assertIn('"Char"', generated_page)
            self.assertIn("raise_managed_exception", generated_page)
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_emit_native_reference_supports_string_to_char_runtime_skeleton_methods(self) -> None:
        fixture_root = TEST_OUTPUT_ROOT / f"FixtureStringCharLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureStringCharLibrary"
        project_path = project_root / "FixtureStringCharLibrary.csproj"
        source_path = project_root / "StringCharOps.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureStringCharLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "using System;\n\n"
                    "namespace FixtureStringCharLibrary;\n\n"
                    "public static class StringCharOps\n"
                    "{\n"
                    "    public static char ToChar(string value)\n"
                    "    {\n"
                    "        return ToCharWithProvider(value, null);\n"
                    "    }\n\n"
                    "    public static char ToCharWithProvider(string value, IFormatProvider provider)\n"
                    "    {\n"
                    "        ArgumentNullException.ThrowIfNull(value);\n"
                    "        if (value.Length != 1)\n"
                    "        {\n"
                    '            throw new FormatException("Need single char");\n'
                    "        }\n\n"
                    "        return value[0];\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            tooling_module = load_module(
                TOOLING_MODULE_PATH,
                f"chaos_string_char_codegen_audit_{uuid.uuid4().hex}",
            )
            driver_intermediate_root = tooling_module.allocate_dotnet_intermediate_dir(
                "Chaos.IL2CPP.Driver",
                host_platform="windows",
            )
            self.assertIsNotNone(driver_intermediate_root)

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing string char library dll: {dll_path}")

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
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )
            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            generated_page = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.page-0001.cpp"
            ).read_text(encoding="utf-8").replace("\r\n", "\n")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )

            for required_subject_id in [
                "FixtureStringCharLibrary/StringCharOps::ToChar:System.Char(System.String)",
                "FixtureStringCharLibrary/StringCharOps::ToCharWithProvider:System.Char(System.String,System.IFormatProvider)",
            ]:
                self.assertNotIn(required_subject_id, coverage_report["uncoveredMethodSubjectIds"])

            self.assertIn("void* arg0;", generated_page)
            self.assertIn("void* arg1;", generated_page)
            self.assertIn("std::uint16_t* return_value;", generated_page)
            self.assertIn("method_invoke", generated_page)
            self.assertIn("resolve_method_by_token(image, 0x", generated_page)
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_emit_native_reference_supports_object_to_char_runtime_skeleton_methods(self) -> None:
        fixture_root = TEST_OUTPUT_ROOT / f"FixtureObjectCharLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureObjectCharLibrary"
        project_path = project_root / "FixtureObjectCharLibrary.csproj"
        source_path = project_root / "ObjectCharOps.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureObjectCharLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "using System;\n\n"
                    "namespace FixtureObjectCharLibrary;\n\n"
                    "public interface IObjectCharProvider\n"
                    "{\n"
                    "    char ToChar(IFormatProvider provider);\n"
                    "}\n\n"
                    "public sealed class SingleCharBox : IObjectCharProvider\n"
                    "{\n"
                    "    private readonly char _value;\n\n"
                    "    public SingleCharBox(char value)\n"
                    "    {\n"
                    "        _value = value;\n"
                    "    }\n\n"
                    "    public char ToChar(IFormatProvider provider)\n"
                    "    {\n"
                    "        return _value;\n"
                    "    }\n"
                    "}\n\n"
                    "public static class ObjectCharOps\n"
                    "{\n"
                    "    public static char ToChar(object value)\n"
                    "    {\n"
                    "        if (value == null)\n"
                    "        {\n"
                    "            return (char)0;\n"
                    "        }\n\n"
                    "        return ((IObjectCharProvider)value).ToChar(null);\n"
                    "    }\n\n"
                    "    public static char ToCharWithProvider(object value, IFormatProvider provider)\n"
                    "    {\n"
                    "        if (value == null)\n"
                    "        {\n"
                    "            return (char)0;\n"
                    "        }\n\n"
                    "        return ((IObjectCharProvider)value).ToChar(provider);\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            tooling_module = load_module(
                TOOLING_MODULE_PATH,
                f"chaos_object_char_codegen_audit_{uuid.uuid4().hex}",
            )
            driver_intermediate_root = tooling_module.allocate_dotnet_intermediate_dir(
                "Chaos.IL2CPP.Driver",
                host_platform="windows",
            )
            self.assertIsNotNone(driver_intermediate_root)

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing object char library dll: {dll_path}")

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
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )
            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            generated_page = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.page-0001.cpp"
            ).read_text(encoding="utf-8").replace("\r\n", "\n")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )

            for required_subject_id in [
                "FixtureObjectCharLibrary/ObjectCharOps::ToChar:System.Char(System.Object)",
                "FixtureObjectCharLibrary/ObjectCharOps::ToCharWithProvider:System.Char(System.Object,System.IFormatProvider)",
            ]:
                self.assertNotIn(required_subject_id, coverage_report["uncoveredMethodSubjectIds"])

            self.assertIn("void* arg0;", generated_page)
            self.assertIn("void* arg1;", generated_page)
            self.assertIn("request->arg0 == nullptr", generated_page)
            self.assertIn("method_invoke", generated_page)
            self.assertIn("resolve_method_by_token(image, 0x", generated_page)
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_emit_native_reference_supports_decimal_and_datetime_to_char_runtime_skeleton_methods(self) -> None:
        fixture_root = TEST_OUTPUT_ROOT / f"FixtureValueTypeCharLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureValueTypeCharLibrary"
        project_path = project_root / "FixtureValueTypeCharLibrary.csproj"
        source_path = project_root / "ValueTypeCharOps.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureValueTypeCharLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "using System;\n\n"
                    "namespace FixtureValueTypeCharLibrary;\n\n"
                    "public static class ValueTypeCharOps\n"
                    "{\n"
                    "    public static char ToChar(decimal value)\n"
                    "    {\n"
                    "        return ((IConvertible)value).ToChar(null);\n"
                    "    }\n\n"
                    "    public static char ToChar(DateTime value)\n"
                    "    {\n"
                    "        return ((IConvertible)value).ToChar(null);\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            tooling_module = load_module(
                TOOLING_MODULE_PATH,
                f"chaos_value_type_char_codegen_audit_{uuid.uuid4().hex}",
            )
            driver_intermediate_root = tooling_module.allocate_dotnet_intermediate_dir(
                "Chaos.IL2CPP.Driver",
                host_platform="windows",
            )
            self.assertIsNotNone(driver_intermediate_root)

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing value type char library dll: {dll_path}")

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
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )
            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            generated_page = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.page-0001.cpp"
            ).read_text(encoding="utf-8")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )

            for required_subject_id in [
                "FixtureValueTypeCharLibrary/ValueTypeCharOps::ToChar:System.Char(System.Decimal)",
                "FixtureValueTypeCharLibrary/ValueTypeCharOps::ToChar:System.Char(System.DateTime)",
            ]:
                self.assertNotIn(required_subject_id, coverage_report["uncoveredMethodSubjectIds"])

            self.assertIn("box_value", generated_page)
            self.assertIn("raise_managed_exception", generated_page)
            self.assertIn('"Decimal"', generated_page)
            self.assertIn('"DateTime"', generated_page)
            self.assertIn('"Char"', generated_page)
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_emit_native_reference_supports_convert_to_string_runtime_skeleton_methods(self) -> None:
        fixture_root = TEST_OUTPUT_ROOT / f"FixtureConvertToStringLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureConvertToStringLibrary"
        project_path = project_root / "FixtureConvertToStringLibrary.csproj"
        source_path = project_root / "ConvertToStringOps.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureConvertToStringLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "using System;\n\n"
                    "namespace FixtureConvertToStringLibrary;\n\n"
                    "public static class ConvertToStringOps\n"
                    "{\n"
                    "    public static string ToStringWithProvider(string value, IFormatProvider provider)\n"
                    "    {\n"
                    "        return value;\n"
                    "    }\n\n"
                    "    public static string ToStringBoolean(bool value)\n"
                    "    {\n"
                    "        return value.ToString();\n"
                    "    }\n\n"
                    "    public static string ToStringBooleanWithProvider(bool value, IFormatProvider provider)\n"
                    "    {\n"
                    "        return value.ToString(provider);\n"
                    "    }\n\n"
                    "    public static string ToStringInt32(int value)\n"
                    "    {\n"
                    "        return value.ToString();\n"
                    "    }\n\n"
                    "    public static string ToStringInt32WithProvider(int value, IFormatProvider provider)\n"
                    "    {\n"
                    "        return value.ToString(provider);\n"
                    "    }\n\n"
                    "    public static string ToStringDecimal(decimal value)\n"
                    "    {\n"
                    "        return value.ToString();\n"
                    "    }\n\n"
                    "    public static string ToStringDecimalWithProvider(decimal value, IFormatProvider provider)\n"
                    "    {\n"
                    "        return value.ToString(provider);\n"
                    "    }\n\n"
                    "    public static string ToStringChar(char value)\n"
                    "    {\n"
                    "        return System.Convert.ToString(value);\n"
                    "    }\n\n"
                    "    public static string ToStringCharWithProvider(char value, IFormatProvider provider)\n"
                    "    {\n"
                    "        return System.Convert.ToString(value, provider);\n"
                    "    }\n\n"
                    "    public static string ToStringBooleanWithProviderViaConvert(bool value, IFormatProvider provider)\n"
                    "    {\n"
                    "        return System.Convert.ToString(value, provider);\n"
                    "    }\n\n"
                    "    public static string ToStringObject(object value)\n"
                    "    {\n"
                    "        return System.Convert.ToString(value);\n"
                    "    }\n\n"
                    "    public static string ToStringObjectWithProvider(object value, IFormatProvider provider)\n"
                    "    {\n"
                    "        return System.Convert.ToString(value, provider);\n"
                    "    }\n\n"
                    "    public static string ToStringByteToBase(byte value, int toBase)\n"
                    "    {\n"
                    "        return System.Convert.ToString(value, toBase);\n"
                    "    }\n\n"
                    "    public static string ToStringInt16ToBase(short value, int toBase)\n"
                    "    {\n"
                    "        return System.Convert.ToString(value, toBase);\n"
                    "    }\n\n"
                    "    public static string ToStringInt32ToBase(int value, int toBase)\n"
                    "    {\n"
                    "        return System.Convert.ToString(value, toBase);\n"
                    "    }\n\n"
                    "    public static string ToStringInt64ToBase(long value, int toBase)\n"
                    "    {\n"
                    "        return System.Convert.ToString(value, toBase);\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            tooling_module = load_module(
                TOOLING_MODULE_PATH,
                f"chaos_convert_to_string_codegen_audit_{uuid.uuid4().hex}",
            )
            driver_intermediate_root = tooling_module.allocate_dotnet_intermediate_dir(
                "Chaos.IL2CPP.Driver",
                host_platform="windows",
            )
            self.assertIsNotNone(driver_intermediate_root)

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing convert-to-string library dll: {dll_path}")

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
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )
            closure_manifest = load_json(closure_root / "closure.manifest.json")
            resolved_assemblies = {
                assembly["assemblyName"]: assembly["path"]
                for assembly in closure_manifest["resolvedAssemblies"]
            }
            self.assertEqual(
                str(dll_path.relative_to(REPO_ROOT)).replace("\\", "/"),
                resolved_assemblies["FixtureConvertToStringLibrary"],
            )
            self.assertIn("System.Private.CoreLib", resolved_assemblies)
            self.assertTrue(
                Path(resolved_assemblies["System.Private.CoreLib"]).is_file(),
                msg=f"missing resolved System.Private.CoreLib path: {resolved_assemblies['System.Private.CoreLib']}",
            )
            self.assertTrue(
                resolved_assemblies["System.Private.CoreLib"].endswith("System.Private.CoreLib.dll"),
                msg=f"unexpected System.Private.CoreLib path: {resolved_assemblies['System.Private.CoreLib']}",
            )
            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            generated_page = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.page-0001.cpp"
            ).read_text(encoding="utf-8").replace("\r\n", "\n")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )

            for required_subject_id in [
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringWithProvider:System.String(System.String,System.IFormatProvider)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringBoolean:System.String(System.Boolean)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringBooleanWithProvider:System.String(System.Boolean,System.IFormatProvider)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringInt32:System.String(System.Int32)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringInt32WithProvider:System.String(System.Int32,System.IFormatProvider)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringDecimal:System.String(System.Decimal)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringDecimalWithProvider:System.String(System.Decimal,System.IFormatProvider)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringChar:System.String(System.Char)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringCharWithProvider:System.String(System.Char,System.IFormatProvider)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringBooleanWithProviderViaConvert:System.String(System.Boolean,System.IFormatProvider)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringObject:System.String(System.Object)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringObjectWithProvider:System.String(System.Object,System.IFormatProvider)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringByteToBase:System.String(System.Byte,System.Int32)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringInt16ToBase:System.String(System.Int16,System.Int32)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringInt32ToBase:System.String(System.Int32,System.Int32)",
                "FixtureConvertToStringLibrary/ConvertToStringOps::ToStringInt64ToBase:System.String(System.Int64,System.Int32)",
            ]:
                self.assertNotIn(required_subject_id, coverage_report["uncoveredMethodSubjectIds"])

            for required_corelib_subject_id in [
                "System.Private.CoreLib/System.Convert::ToString:System.String(System.Char)",
                "System.Private.CoreLib/System.Convert::ToString:System.String(System.Char,System.IFormatProvider)",
                "System.Private.CoreLib/System.Convert::ToString:System.String(System.Boolean,System.IFormatProvider)",
                "System.Private.CoreLib/System.Convert::ToString:System.String(System.Object)",
                "System.Private.CoreLib/System.Convert::ToString:System.String(System.Object,System.IFormatProvider)",
                "System.Private.CoreLib/System.Convert::ToString:System.String(System.Byte,System.Int32)",
                "System.Private.CoreLib/System.Convert::ToString:System.String(System.Int16,System.Int32)",
                "System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32,System.Int32)",
                "System.Private.CoreLib/System.Convert::ToString:System.String(System.Int64,System.Int32)",
            ]:
                self.assertNotIn(required_corelib_subject_id, coverage_report["uncoveredMethodSubjectIds"])

            self.assertIn("void* arg0;", generated_page)
            self.assertIn("void* arg1;", generated_page)
            self.assertIn("void** return_value;", generated_page)
            self.assertIn("*request->return_value = request->arg0;", generated_page)
            self.assertIn("bridge->box_value", generated_page)
            self.assertIn("method_invoke", generated_page)
            self.assertIn("resolve_method_by_token(image, 0x", generated_page)
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_emit_native_reference_supports_overflow_throw_runtime_skeleton_methods(self) -> None:
        fixture_root = TEST_OUTPUT_ROOT / f"FixtureOverflowThrowLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureOverflowThrowLibrary"
        project_path = project_root / "FixtureOverflowThrowLibrary.csproj"
        source_path = project_root / "OverflowOps.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureOverflowThrowLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "using System;\n\n"
                    "namespace FixtureOverflowThrowLibrary;\n\n"
                    "internal static class ResourceShim\n"
                    "{\n"
                    "    public static string GetResourceString(string key)\n"
                    "    {\n"
                    "        return key;\n"
                    "    }\n\n"
                    "    public static string get_Overflow_Byte()\n"
                    "    {\n"
                    '        return GetResourceString("Overflow_Byte");\n'
                    "    }\n"
                    "}\n\n"
                    "public static class OverflowOps\n"
                    "{\n"
                    "    public static void ThrowByteOverflowException()\n"
                    "    {\n"
                    "        throw new OverflowException(ResourceShim.get_Overflow_Byte());\n"
                    "    }\n\n"
                    "    public static byte ToByte(ushort value)\n"
                    "    {\n"
                    "        if (value > byte.MaxValue)\n"
                    "        {\n"
                    "            ThrowByteOverflowException();\n"
                    "        }\n\n"
                    "        return (byte)value;\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            tooling_module = load_module(TOOLING_MODULE_PATH, f"chaos_overflow_throw_codegen_audit_{uuid.uuid4().hex}")
            driver_intermediate_root = tooling_module.allocate_dotnet_intermediate_dir("Chaos.IL2CPP.Driver", host_platform="windows")
            self.assertIsNotNone(driver_intermediate_root)

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing overflow throw library dll: {dll_path}")

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
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )
            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            generated_page = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.page-0001.cpp"
            ).read_text(encoding="utf-8").replace("\r\n", "\n")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )

            self.assertEqual(4, coverage_report["requestedMethodCount"])
            self.assertEqual(4, coverage_report["emittedMethodCount"])
            self.assertEqual(0, coverage_report["uncoveredMethodCount"])
            self.assertEqual([], coverage_report["uncoveredMethodSubjectIds"])
            self.assertIn("NativeReferenceStub_Page0001_Item0001", generated_page)
            self.assertIn("NativeReferenceStub_Page0001_Item0002", generated_page)
            self.assertIn("NativeReferenceStub_Page0001_Item0003", generated_page)
            self.assertIn("NativeReferenceStub_Page0001_Item0004", generated_page)
            self.assertIn('"Overflow_Byte"', generated_page)
            self.assertIn("produced_message", generated_page)
            self.assertIn("raise_managed_exception", generated_page)
            self.assertIn("std::uint16_t value;", generated_page)
            self.assertIn(
                "*request->return_value = static_cast<std::uint8_t>(request->value);",
                generated_page,
            )
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_emit_native_reference_supports_cross_page_overflow_throw_runtime_skeleton_methods(self) -> None:
        fixture_root = TEST_OUTPUT_ROOT / f"FixtureCrossPageOverflowThrowLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureCrossPageOverflowThrowLibrary"
        project_path = project_root / "FixtureCrossPageOverflowThrowLibrary.csproj"
        source_path = project_root / "OverflowOps.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureCrossPageOverflowThrowLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        filler_methods = "".join(
            f"    public static int Filler{i:04d}() => {i};\n"
            for i in range(1022)
        )

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "using System;\n\n"
                    "namespace FixtureCrossPageOverflowThrowLibrary;\n\n"
                    "public static class FillerCatalog\n"
                    "{\n"
                    f"{filler_methods}"
                    "}\n\n"
                    "internal static class ResourceShim\n"
                    "{\n"
                    "    public static string GetResourceString(string key)\n"
                    "    {\n"
                    "        return key;\n"
                    "    }\n\n"
                    "    public static string get_Overflow_Byte()\n"
                    "    {\n"
                    '        return GetResourceString("Overflow_Byte");\n'
                    "    }\n"
                    "}\n\n"
                    "public static class OverflowOps\n"
                    "{\n"
                    "    public static void ThrowByteOverflowException()\n"
                    "    {\n"
                    "        throw new OverflowException(ResourceShim.get_Overflow_Byte());\n"
                    "    }\n\n"
                    "    public static byte ToByte(ushort value)\n"
                    "    {\n"
                    "        if (value > byte.MaxValue)\n"
                    "        {\n"
                    "            ThrowByteOverflowException();\n"
                    "        }\n\n"
                    "        return (byte)value;\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing cross-page overflow throw library dll: {dll_path}")

            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )
            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            first_page = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.page-0001.cpp"
            ).read_text(encoding="utf-8").replace("\r\n", "\n")
            second_page = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.page-0002.cpp"
            ).read_text(encoding="utf-8").replace("\r\n", "\n")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )

            self.assertEqual(1026, coverage_report["requestedMethodCount"])
            self.assertGreaterEqual(coverage_report["emittedMethodCount"], 4)
            self.assertEqual(1022, coverage_report["uncoveredMethodCount"])
            for required_subject_id in [
                "FixtureCrossPageOverflowThrowLibrary/ResourceShim::get_Overflow_Byte:System.String()",
                "FixtureCrossPageOverflowThrowLibrary/OverflowOps::ThrowByteOverflowException:System.Void()",
                "FixtureCrossPageOverflowThrowLibrary/OverflowOps::ToByte:System.Byte(System.UInt16)",
                "FixtureCrossPageOverflowThrowLibrary/ResourceShim::GetResourceString:System.String(System.String)",
            ]:
                self.assertNotIn(required_subject_id, coverage_report["uncoveredMethodSubjectIds"])
            self.assertIn("NativeReferenceStub_Page0002_Item0002", first_page)
            self.assertIn("producer_status = NativeReferenceStub_Page0002_Item0002(", first_page)
            self.assertIn('"Overflow_Byte"', second_page)
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_emit_native_reference_async_task_factory_uses_parameter_carriers_independent_of_result_type(self) -> None:
        self._ensure_bundle_generated()

        fixture_root = TEST_OUTPUT_ROOT / f"FixtureAsyncBridgeLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureAsyncBridgeLibrary"
        project_path = project_root / "FixtureAsyncBridgeLibrary.csproj"
        source_path = project_root / "AsyncBridge.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureAsyncBridgeLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "using System.Threading.Tasks;\n\n"
                    "namespace FixtureAsyncBridgeLibrary;\n\n"
                    "public static class AsyncBridge\n"
                    "{\n"
                    "    public static async Task<string> DescribeAsync(int seed)\n"
                    "    {\n"
                    "        await Task.Yield();\n"
                    '        return seed == 40 ? "value:40" : "value:unexpected";\n'
                    "    }\n\n"
                    "    public static string ComposeDescribe()\n"
                    "    {\n"
                    "        return DescribeAsync(40).GetAwaiter().GetResult();\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing async bridge dll: {dll_path}")

            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )
            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            generated_page = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.page-0001.cpp"
            ).read_text(encoding="utf-8")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )

            self.assertEqual(0, coverage_report["uncoveredMethodCount"])
            self.assertIn(
                "int32_t arg0;\n    void* arg1;\n    void** return_value;\n};\n\n// managed_result_type: System.String",
                generated_page,
            )
            self.assertNotIn(
                "void* arg0;\n    void* arg1;\n    void** return_value;\n};\n\n// managed_result_type: System.String",
                generated_page,
            )
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_delegate_closed_target_relay_lowering_supports_arbitrary_names_and_trailing_literal(self) -> None:
        self._ensure_bundle_generated()

        fixture_root = TEST_OUTPUT_ROOT / f"FixtureDelegateRelayLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureDelegateRelayLibrary"
        project_path = project_root / "FixtureDelegateRelayLibrary.csproj"
        source_path = project_root / "Program.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureDelegateRelayLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <OutputType>Exe</OutputType>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "using System;\n\n"
                    "namespace FixtureDelegateRelayLibrary;\n\n"
                    "internal delegate string RelayTextFormatter(string prefix);\n\n"
                    "internal delegate string RelayTailFormatter(string value);\n\n"
                    "internal sealed class RelayPacket\n"
                    "{\n"
                    "    private readonly string _name;\n\n"
                    "    public RelayPacket(string name)\n"
                    "    {\n"
                    "        _name = name;\n"
                    "    }\n\n"
                    "    public string Compose(string prefix)\n"
                    "    {\n"
                    '        return string.Concat(prefix, _name, ".");\n'
                    "    }\n"
                    "}\n\n"
                    "internal static class TailOps\n"
                    "{\n"
                    "    public static string AppendQuestion(string value)\n"
                    "    {\n"
                    '        return string.Concat(value, "?");\n'
                    "    }\n"
                    "}\n\n"
                    "internal static class Program\n"
                    "{\n"
                    "    private static int Main()\n"
                    "    {\n"
                    '        var packet = new RelayPacket("generic delegate");\n'
                    "        RelayTextFormatter formatter = packet.Compose;\n"
                    "        RelayTailFormatter tail = TailOps.AppendQuestion;\n"
                    '        Console.WriteLine(tail(formatter("fixture-delegate:")));\n'
                    "        return 0;\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing delegate relay dll: {dll_path}")

            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    str(dll_path),
                    str(closure_root),
                    "--entry-point-subject-id",
                    "FixtureDelegateRelayLibrary/Program::Main()",
                ],
                cwd=REPO_ROOT,
            )

            lowering_plan = load_json(closure_root / "native-reference.lowering-plan.json")
            self.assertEqual("managed-delegates.closed-target-relay-message.minimal", lowering_plan["planKind"])
            self.assertEqual('"?"', lowering_plan["trailingLiteral"])
            self.assertEqual(1, lowering_plan["trailingLiteralByteCount"])

            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            manifest = load_json(emit_root / "native-reference.manifest.json")
            generated_source_path = Path(manifest.get("generatedSourcePath") or manifest["generatedSourcePaths"][0])
            generated_cpp = (emit_root / generated_source_path).read_text(encoding="utf-8").replace("\r\n", "\n")

            self.assertIn("FixtureDelegateRelayLibrary/Program::Main:System.Int32()", manifest["entrySubjectId"])
            self.assertIn("managed-delegates.closed-target-relay-message.minimal", generated_cpp)
            self.assertIn('thread,\n        "?",\n        1u);', generated_cpp)
            self.assertNotIn('thread,\n        "!",\n        1u);', generated_cpp)
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_emit_native_reference_delegate_closed_target_relay_full_closure_supports_arbitrary_names(self) -> None:
        self._ensure_bundle_generated()

        fixture_root = TEST_OUTPUT_ROOT / f"FixtureDelegateRelayFullClosureLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureDelegateRelayFullClosureLibrary"
        project_path = project_root / "FixtureDelegateRelayFullClosureLibrary.csproj"
        source_path = project_root / "Program.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureDelegateRelayFullClosureLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <OutputType>Exe</OutputType>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "using System;\n\n"
                    "namespace FixtureDelegateRelayFullClosureLibrary;\n\n"
                    "internal delegate string RelayTextFormatter(string prefix);\n\n"
                    "internal delegate string RelayTailFormatter(string value);\n\n"
                    "internal sealed class RelayPacket\n"
                    "{\n"
                    "    private readonly string _name;\n\n"
                    "    public RelayPacket(string name)\n"
                    "    {\n"
                    "        _name = name;\n"
                    "    }\n\n"
                    "    public string Compose(string prefix)\n"
                    "    {\n"
                    '        return string.Concat(prefix, _name, ".");\n'
                    "    }\n"
                    "}\n\n"
                    "internal static class TailOps\n"
                    "{\n"
                    "    public static string AppendQuestion(string value)\n"
                    "    {\n"
                    '        return string.Concat(value, "?");\n'
                    "    }\n"
                    "}\n\n"
                    "internal static class Program\n"
                    "{\n"
                    "    private static int Main()\n"
                    "    {\n"
                    '        var packet = new RelayPacket("full closure delegate");\n'
                    "        RelayTextFormatter formatter = packet.Compose;\n"
                    "        RelayTailFormatter tail = TailOps.AppendQuestion;\n"
                    '        Console.WriteLine(tail(formatter("fixture-full-closure:")));\n'
                    "        return 0;\n"
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing delegate relay full closure dll: {dll_path}")

            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )
            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            generated_cpp = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.generated.cpp"
            ).read_text(encoding="utf-8")
            generated_page = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.page-0001.cpp"
            ).read_text(encoding="utf-8").replace("\r\n", "\n")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )

            self.assertEqual(0, coverage_report["uncoveredMethodCount"])
            self.assertIn(
                "FixtureDelegateRelayFullClosureLibrary/Program::Main:System.Int32()",
                generated_cpp,
            )
            self.assertIn("kPageDelegateClosedTargetRelayDescriptorCount = 1;", generated_page)
            self.assertIn('"?", 1u', generated_page)
            self.assertNotIn('"!", 1u', generated_page)
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

    def test_emit_native_reference_ignores_interface_declarations_without_canonical_body(self) -> None:
        self._ensure_bundle_generated()

        fixture_root = TEST_OUTPUT_ROOT / f"FixtureInterfaceDeclarationLibrary-{uuid.uuid4().hex}"
        project_root = fixture_root / "FixtureInterfaceDeclarationLibrary"
        project_path = project_root / "FixtureInterfaceDeclarationLibrary.csproj"
        source_path = project_root / "MessageBanner.cs"
        dll_path = project_root / "bin" / "Release" / "net8.0" / "FixtureInterfaceDeclarationLibrary.dll"
        closure_root = fixture_root / "closure"
        emit_root = fixture_root / "emit-native-reference"

        try:
            project_root.mkdir(parents=True, exist_ok=False)
            project_path.write_text(
                (
                    '<Project Sdk="Microsoft.NET.Sdk">\n'
                    "  <PropertyGroup>\n"
                    "    <TargetFramework>net8.0</TargetFramework>\n"
                    "    <ImplicitUsings>disable</ImplicitUsings>\n"
                    "    <Nullable>disable</Nullable>\n"
                    "  </PropertyGroup>\n"
                    "</Project>\n"
                ),
                encoding="utf-8",
            )
            source_path.write_text(
                (
                    "namespace FixtureInterfaceDeclarationLibrary;\n\n"
                    "public interface IMessageBanner\n"
                    "{\n"
                    "    string Render();\n"
                    "}\n\n"
                    "public sealed class MessageBanner : IMessageBanner\n"
                    "{\n"
                    "    private readonly string _value;\n\n"
                    "    public MessageBanner(string value)\n"
                    "    {\n"
                    "        _value = value;\n"
                    "    }\n\n"
                    "    public string Render()\n"
                    "    {\n"
                    '        return string.Concat(string.Concat("interface-declaration:", _value), "|ok");\n'
                    "    }\n"
                    "}\n"
                ),
                encoding="utf-8",
            )

            run_checked(
                [
                    "dotnet",
                    "build",
                    str(project_path),
                    "-c",
                    "Release",
                ],
                cwd=REPO_ROOT,
            )
            self.assertTrue(dll_path.is_file(), msg=f"missing interface declaration library dll: {dll_path}")

            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    str(dll_path),
                    str(closure_root),
                    "--full-assembly-closure",
                ],
                cwd=REPO_ROOT,
            )
            run_checked(
                [
                    "dotnet",
                    str(DRIVER_DLL_PATH),
                    "emit-native-reference",
                    str(closure_root),
                    str(emit_root),
                ],
                cwd=REPO_ROOT,
            )

            generated_cpp = (
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.generated.cpp"
            ).read_text(encoding="utf-8")
            coverage_report = load_json(
                emit_root
                / "generated"
                / "runtime"
                / "native-reference.runtime-skeleton.coverage.json"
            )

            self.assertIn(
                "FixtureInterfaceDeclarationLibrary/MessageBanner::.ctor:System.Void(System.String)",
                generated_cpp,
            )
            self.assertIn(
                "FixtureInterfaceDeclarationLibrary/MessageBanner::Render:System.String()",
                generated_cpp,
            )
            self.assertEqual(3, coverage_report["requestedMethodCount"])
            self.assertEqual(2, coverage_report["emittedMethodCount"])
            self.assertEqual(0, coverage_report["uncoveredMethodCount"])
            self.assertEqual([], coverage_report["uncoveredMethodSubjectIds"])
            self.assertEqual([], coverage_report["uncoveredMethods"])
        finally:
            shutil.rmtree(fixture_root, ignore_errors=True)

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
            / "audit"
            / "native-aot.audit.generated.cpp"
        ).read_text(encoding="utf-8")
        generated_page_json = load_json(
            self.native_aot_output_root
            / "generated"
            / "audit"
            / "native-aot.audit.page-0001.json"
        )
        manifest = load_json(self.native_aot_output_root / "native-aot.manifest.json")

        self.assertIn("assembly-full-closure-audit", generated_cpp)
        self.assertIn('extern "C" int RunNativeAotAudit', generated_cpp)
        self.assertIn("translation_unit_method_count = 1", generated_cpp)
        self.assertIn("translation_unit_page_count = 1", generated_cpp)
        self.assertNotIn("GoldenSimpleLib.Library/Greeter::BuildMessage:System.String()", generated_cpp)
        self.assertEqual("assemblyFullClosureAuditPage", generated_page_json["artifactKind"])
        self.assertEqual("GoldenSimpleLib.Library", generated_page_json["assemblyName"])
        self.assertEqual("assembly-full-closure-audit", generated_page_json["planKind"])
        self.assertEqual(1, generated_page_json["pageNumber"])
        self.assertEqual(1, generated_page_json["methodCount"])
        self.assertEqual(
            ["GoldenSimpleLib.Library/Greeter::BuildMessage:System.String()"],
            generated_page_json["methodSubjectIds"],
        )
        self.assertEqual("GoldenSimpleLib.Library", manifest["assemblyName"])
        self.assertEqual("", manifest["entrySubjectId"])
        self.assertEqual(1024, manifest["translationUnitPageSize"])
        self.assertEqual(1, manifest["translationUnitPageCount"])
        self.assertEqual(1, len(manifest["translationUnitPages"]))
        self.assertEqual("generated/audit/native-aot.audit.page-0001.json", manifest["translationUnitPages"][0]["path"])
        self.assertEqual(
            [
                {"kind": "generatedTranslationUnit", "path": "generated/audit/native-aot.audit.generated.cpp"},
                {"kind": "auditInventoryPage", "path": "generated/audit/native-aot.audit.page-0001.json"},
                {"kind": "codegenMetrics", "path": "native-aot.codegen-metrics.json"},
            ],
            manifest["generatedArtifacts"],
        )

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
