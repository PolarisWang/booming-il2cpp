from __future__ import annotations

import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
ROOT_CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"
CORE_SOLUTION_PATH = REPO_ROOT / "solutions" / "core" / "windows" / "chaos-il2cpp-core.sln"

ENGINE_BINDING_ROOT = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.EngineBinding"
ENGINE_BINDING_PROJECT_PATH = ENGINE_BINDING_ROOT / "Chaos.IL2CPP.EngineBinding.csproj"
ENGINE_EXPORT_ATTRIBUTE_PATH = ENGINE_BINDING_ROOT / "EngineExportAttribute.cs"
ENGINE_CALLBACK_ATTRIBUTE_PATH = ENGINE_BINDING_ROOT / "EngineCallbackAttribute.cs"
ENGINE_STUB_EMITTER_PATH = ENGINE_BINDING_ROOT / "EngineBindingStubEmitter.cs"

MANAGED_SMOKE_ROOT = REPO_ROOT / "tests" / "contracts" / "managed" / "engine-binding-smoke"
MANAGED_SMOKE_PROJECT_PATH = MANAGED_SMOKE_ROOT / "EngineBindingSmoke.csproj"
MANAGED_SMOKE_PROGRAM_PATH = MANAGED_SMOKE_ROOT / "Program.cs"
MANAGED_SMOKE_CONTRACT_PATH = MANAGED_SMOKE_ROOT / "SmokeHostExports.cs"

NATIVE_SMOKE_ROOT = REPO_ROOT / "tests" / "contracts" / "native" / "engine-binding"
NATIVE_SMOKE_CMAKE_PATH = NATIVE_SMOKE_ROOT / "CMakeLists.txt"
NATIVE_SMOKE_SOURCE_PATH = NATIVE_SMOKE_ROOT / "compile_only_engine_binding_smoke.cpp"


def parse_project_references(project_path: Path) -> list[str]:
    root = ET.fromstring(project_path.read_text(encoding="utf-8"))
    references: list[str] = []
    for element in root.findall(".//ProjectReference"):
        include_value = element.attrib.get("Include", "")
        references.append(Path(include_value).stem)
    return sorted(references)


class Phase3EngineBindingCodegenBaselineTests(unittest.TestCase):
    def test_engine_binding_project_isolation_and_legacy_solution_cutover(self) -> None:
        self.assertTrue(ENGINE_BINDING_ROOT.is_dir(), msg=f"missing engine binding root: {ENGINE_BINDING_ROOT}")
        self.assertTrue(ENGINE_BINDING_PROJECT_PATH.is_file(), msg=f"missing engine binding project: {ENGINE_BINDING_PROJECT_PATH}")
        self.assertEqual(["Chaos.IL2CPP.Contracts"], parse_project_references(ENGINE_BINDING_PROJECT_PATH))
        self.assertFalse(
            CORE_SOLUTION_PATH.exists(),
            msg=f"legacy static core solution should not exist anymore: {CORE_SOLUTION_PATH}",
        )

    def test_engine_binding_sources_define_attributes_and_stub_emitter(self) -> None:
        export_source = ENGINE_EXPORT_ATTRIBUTE_PATH.read_text(encoding="utf-8")
        callback_source = ENGINE_CALLBACK_ATTRIBUTE_PATH.read_text(encoding="utf-8")
        emitter_source = ENGINE_STUB_EMITTER_PATH.read_text(encoding="utf-8")

        self.assertIn("class EngineExportAttribute", export_source)
        self.assertIn("ExportName", export_source)

        self.assertIn("class EngineCallbackAttribute", callback_source)
        self.assertIn("CallbackName", callback_source)

        for required_fragment in [
            "GenerateForInterface",
            "GetCustomAttribute<EngineExportAttribute>",
            "GetCustomAttribute<EngineCallbackAttribute>",
            'engine_bridge.h',
            "engine_register_callback",
            "ENGINE_STATUS_NOT_SUPPORTED",
        ]:
            self.assertIn(required_fragment, emitter_source)

    def test_engine_binding_smoke_assets_lock_managed_baseline_and_native_compile_only_entry(self) -> None:
        root_cmake_source = ROOT_CMAKE_PATH.read_text(encoding="utf-8")
        smoke_project_source = MANAGED_SMOKE_PROJECT_PATH.read_text(encoding="utf-8")
        smoke_program_source = MANAGED_SMOKE_PROGRAM_PATH.read_text(encoding="utf-8")
        smoke_contract_source = MANAGED_SMOKE_CONTRACT_PATH.read_text(encoding="utf-8")
        native_cmake_source = NATIVE_SMOKE_CMAKE_PATH.read_text(encoding="utf-8")
        native_source = NATIVE_SMOKE_SOURCE_PATH.read_text(encoding="utf-8")

        self.assertTrue(MANAGED_SMOKE_PROJECT_PATH.is_file(), msg=f"missing managed smoke project: {MANAGED_SMOKE_PROJECT_PATH}")
        self.assertTrue(MANAGED_SMOKE_PROGRAM_PATH.is_file(), msg=f"missing managed smoke program: {MANAGED_SMOKE_PROGRAM_PATH}")
        self.assertTrue(MANAGED_SMOKE_CONTRACT_PATH.is_file(), msg=f"missing managed smoke contract: {MANAGED_SMOKE_CONTRACT_PATH}")
        self.assertIn("Chaos.IL2CPP.EngineBinding", smoke_project_source)
        self.assertIn("EngineBindingStubEmitter.GenerateForInterface", smoke_program_source)
        self.assertIn("[EngineExport(", smoke_contract_source)
        self.assertIn("[EngineCallback(", smoke_contract_source)

        self.assertTrue(NATIVE_SMOKE_CMAKE_PATH.is_file(), msg=f"missing native smoke CMake: {NATIVE_SMOKE_CMAKE_PATH}")
        self.assertTrue(NATIVE_SMOKE_SOURCE_PATH.is_file(), msg=f"missing native smoke source: {NATIVE_SMOKE_SOURCE_PATH}")
        self.assertIn("chaos_engine_binding_compile_only_smoke", native_cmake_source)
        self.assertIn('#include "engine_bridge.h"', native_source)
        self.assertIn("engine_register_callback", native_source)
        self.assertIn("ENGINE_STATUS_NOT_SUPPORTED", native_source)

        self.assertIn("add_subdirectory(tests/contracts/native/engine-binding)", root_cmake_source)


if __name__ == "__main__":
    unittest.main()
