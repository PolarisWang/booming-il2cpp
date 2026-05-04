from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
RUNTIME_CORE_DIR = REPO_ROOT / "src" / "native" / "runtime-core"
MODULE_REGISTRY_HEADER_PATH = RUNTIME_CORE_DIR / "module_registry.h"
MODULE_REGISTRY_SOURCE_PATH = RUNTIME_CORE_DIR / "module_registry.cpp"
RUNTIME_INSTANTIATION_SOURCE_PATH = RUNTIME_CORE_DIR / "runtime_instantiation.cpp"


class ModuleRegistryFileExistenceTests(unittest.TestCase):
    """Verify that module_registry files exist."""

    def test_module_registry_header_exists(self) -> None:
        self.assertTrue(
            MODULE_REGISTRY_HEADER_PATH.is_file(),
            msg=f"missing module_registry.h: {MODULE_REGISTRY_HEADER_PATH}",
        )

    def test_module_registry_source_exists(self) -> None:
        self.assertTrue(
            MODULE_REGISTRY_SOURCE_PATH.is_file(),
            msg=f"missing module_registry.cpp: {MODULE_REGISTRY_SOURCE_PATH}",
        )


class ModuleRegistryCapacityTests(unittest.TestCase):
    """Verify kMaxModules expansion and capacity-related declarations."""

    def setUp(self) -> None:
        self.header = MODULE_REGISTRY_HEADER_PATH.read_text(encoding="utf-8")

    def test_k_max_modules_expanded_to_256(self) -> None:
        self.assertIn("kMaxModules = 256", self.header)

    def test_k_max_modules_greater_than_64(self) -> None:
        # Ensure the change from 64 to 256 is effective.
        self.assertNotIn("kMaxModules = 64", self.header)

    def test_k_invalid_module_id(self) -> None:
        self.assertIn("kInvalidModuleId", self.header)


class ModuleRegistryTombstoneTests(unittest.TestCase):
    """Verify tombstone-related declarations in the header."""

    def setUp(self) -> None:
        self.header = MODULE_REGISTRY_HEADER_PATH.read_text(encoding="utf-8")

    def test_tombstone_field_declared(self) -> None:
        self.assertIn("bool tombstone", self.header)
        self.assertIn("tombstone = false", self.header)

    def test_mark_module_tombstone_declared(self) -> None:
        self.assertIn("MarkModuleTombstone", self.header)
        self.assertIn("uint32_t module_id", self.header)

    def test_is_module_tombstone_declared(self) -> None:
        self.assertIn("IsModuleTombstone", self.header)
        self.assertIn("uint32_t module_id", self.header)


class ModuleRegistryImplementationTests(unittest.TestCase):
    """Verify implementation details in module_registry.cpp."""

    def setUp(self) -> None:
        self.source = MODULE_REGISTRY_SOURCE_PATH.read_text(encoding="utf-8")

    def test_includes_native_types(self) -> None:
        self.assertIn("#include <chaos/native_types.h>", self.source)

    def test_free_list_declared(self) -> None:
        self.assertIn("g_free_list", self.source)
        self.assertIn("CHAOS_IL2CPP_VECTOR(uint32_t)", self.source)

    def test_register_module_checks_free_list_first(self) -> None:
        self.assertIn("g_free_list.empty()", self.source)
        self.assertIn("g_free_list.back()", self.source)
        self.assertIn("g_free_list.pop_back()", self.source)

    def test_register_module_sets_tombstone_false(self) -> None:
        self.assertIn("tombstone = false", self.source)

    def test_lookup_module_checks_k_max_modules(self) -> None:
        self.assertIn("module_id >= kMaxModules", self.source)
        self.assertIn("return nullptr", self.source)

    def test_lookup_module_by_name_skips_tombstone(self) -> None:
        self.assertIn("tombstone", self.source)
        self.assertIn("continue", self.source)

    def test_mark_module_tombstone_implementation(self) -> None:
        self.assertIn("MarkModuleTombstone", self.source)
        self.assertIn("module_id == 0u", self.source)  # CoreLib guard
        self.assertIn("g_free_list.push_back", self.source)

    def test_tombstone_nulls_freed_fields(self) -> None:
        self.source.index("image        = nullptr")
        self.source.index("type_flags   = nullptr")
        self.source.index("type_parent_tokens = nullptr")
        self.source.index("type_count   = 0u")

    def test_is_module_tombstone_implementation(self) -> None:
        self.assertIn("IsModuleTombstone", self.source)
        self.assertIn("return g_module_storage[module_id].tombstone", self.source)


class RuntimeInstantiationTombstoneIntegrationTests(unittest.TestCase):
    """Verify that runtime_instantiation.cpp integrates tombstone marking."""

    def setUp(self) -> None:
        self.source = RUNTIME_INSTANTIATION_SOURCE_PATH.read_text(encoding="utf-8")

    def test_unregister_module_generics_calls_mark_module_tombstone(self) -> None:
        self.assertIn("MarkModuleTombstone(module_id)", self.source)
        self.assertIn("runtime_core::MarkModuleTombstone", self.source)

    def test_tombstone_comment_explains_purpose(self) -> None:
        self.assertIn("tombstone", self.source)
        self.assertIn("LookupModule(module_id) still returns a valid pointer", self.source)


if __name__ == "__main__":
    unittest.main()
