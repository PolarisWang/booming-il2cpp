from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
RUNTIME_CORE_DIR = REPO_ROOT / "src" / "native" / "runtime-core"
RUNTIME_CORE_CMAKE_PATH = RUNTIME_CORE_DIR / "CMakeLists.txt"
LAYOUT_ENGINE_HEADER_PATH = RUNTIME_CORE_DIR / "layout_engine.h"
LAYOUT_ENGINE_SOURCE_PATH = RUNTIME_CORE_DIR / "layout_engine.cpp"
RUNTIME_INSTANTIATION_SOURCE_PATH = RUNTIME_CORE_DIR / "runtime_instantiation.cpp"
RUNTIME_INSTANTIATION_HEADER_PATH = RUNTIME_CORE_DIR / "runtime_instantiation.h"


class LayoutEngineFileExistenceTests(unittest.TestCase):
    """Verify that all expected LayoutEngine files exist."""

    def test_layout_engine_header_exists(self) -> None:
        self.assertTrue(
            LAYOUT_ENGINE_HEADER_PATH.is_file(),
            msg=f"missing layout_engine.h: {LAYOUT_ENGINE_HEADER_PATH}",
        )

    def test_layout_engine_source_exists(self) -> None:
        self.assertTrue(
            LAYOUT_ENGINE_SOURCE_PATH.is_file(),
            msg=f"missing layout_engine.cpp: {LAYOUT_ENGINE_SOURCE_PATH}",
        )

    def test_layout_engine_wired_into_cmake(self) -> None:
        cmake_text = RUNTIME_CORE_CMAKE_PATH.read_text(encoding="utf-8")
        self.assertIn("layout_engine.cpp", cmake_text)
        self.assertIn("layout_engine.h", cmake_text)


class LayoutEngineHeaderContentTests(unittest.TestCase):
    """Verify that layout_engine.h exposes the expected declarations."""

    def setUp(self) -> None:
        self.header = LAYOUT_ENGINE_HEADER_PATH.read_text(encoding="utf-8")

    def test_namespace_declared(self) -> None:
        self.assertIn("namespace chaos::il2cpp::layout", self.header)

    def test_recursion_guard_declared(self) -> None:
        self.assertIn("struct RecursionGuard", self.header)
        self.assertIn("bool Enter(TypeInfoHandle type)", self.header)
        self.assertIn("void Leave()", self.header)
        self.assertIn("struct ScopedEnter", self.header)

    def test_field_layout_declared(self) -> None:
        self.assertIn("struct FieldLayout", self.header)
        self.assertIn("TypeInfoHandle resolved_type", self.header)
        self.assertIn("CHAOS_IL2CPP_UINT32 offset", self.header)
        self.assertIn("CHAOS_IL2CPP_UINT32 size", self.header)
        self.assertIn("CHAOS_IL2CPP_UINT32 alignment", self.header)
        self.assertIn("bool is_gc_reference", self.header)

    def test_type_layout_declared(self) -> None:
        self.assertIn("struct TypeLayout", self.header)
        self.assertIn("TypeInfoHandle closed_type", self.header)
        self.assertIn("CHAOS_IL2CPP_UINT32 value_size", self.header)
        self.assertIn("CHAOS_IL2CPP_UINT32 alignment", self.header)
        self.assertIn("FieldLayout* fields", self.header)
        self.assertIn("CHAOS_IL2CPP_UINT32 gc_ref_count", self.header)

    def test_layout_engine_class_declared(self) -> None:
        self.assertIn("class LayoutEngine", self.header)

    def test_get_or_compute_layout_declared(self) -> None:
        self.assertIn("GetOrComputeLayout", self.header)
        self.assertIn("TypeInfoHandle closed_type", self.header)

    def test_invalidate_module_cache_declared(self) -> None:
        self.assertIn("InvalidateModuleCache", self.header)
        self.assertIn("CHAOS_IL2CPP_UINT32 module_id", self.header)

    def test_cache_hit_and_compute_counters(self) -> None:
        self.assertIn("GetCacheHitCount", self.header)
        self.assertIn("GetComputeCount", self.header)

    def test_private_internal_methods_declared(self) -> None:
        self.assertIn("ComputeLayoutInternal", self.header)
        self.assertIn("ResolveSizeAndAlignmentInternal", self.header)
        self.assertIn("ResolveFieldType", self.header)
        self.assertIn("FindTypeByName", self.header)

    def test_alignment_helpers_declared(self) -> None:
        self.assertIn("AlignUp", self.header)
        self.assertIn("NaturalAlignment", self.header)
        self.assertIn("ComputeStructAlignment", self.header)

    def test_cache_state_members(self) -> None:
        self.assertIn("CHAOS_IL2CPP_UNORDERED_MAP(TypeInfoHandle, TypeLayout*) cache_", self.header)

    def test_layout_expectation_declared(self) -> None:
        self.assertIn("struct LayoutExpectation", self.header)
        self.assertIn("struct FieldExpectation", self.header)

    def test_verify_module_layouts_declared(self) -> None:
        self.assertIn("void VerifyModuleLayouts", self.header)

    def test_get_layout_engine_singleton(self) -> None:
        self.assertIn("LayoutEngine* GetLayoutEngine()", self.header)

    def test_mutex_for_thread_safety(self) -> None:
        self.assertIn("CHAOS_IL2CPP_MUTEX mutex_", self.header)

    def test_module_index_for_bulk_invalidation(self) -> None:
        self.assertIn("module_index_", self.header)
        self.assertIn("CHAOS_IL2CPP_VECTOR(TypeInfoHandle)", self.header)


class LayoutEngineSourceContentTests(unittest.TestCase):
    """Verify that layout_engine.cpp contains the expected implementations."""

    def setUp(self) -> None:
        self.source = LAYOUT_ENGINE_SOURCE_PATH.read_text(encoding="utf-8")

    def test_primitive_type_table(self) -> None:
        self.assertIn("kPrimitiveTypes", self.source)
        self.assertIn('"System.Int32"', self.source)
        self.assertIn('"System.Int64"', self.source)
        self.assertIn('"System.Single"', self.source)
        self.assertIn('"System.Double"', self.source)
        self.assertIn('"System.Byte"', self.source)
        self.assertIn('"System.Boolean"', self.source)
        self.assertIn('"System.Char"', self.source)
        self.assertIn('"System.IntPtr"', self.source)

    def test_match_primitive_type_helper(self) -> None:
        self.assertIn("MatchPrimitiveType", self.source)

    def test_resolve_type_descriptor_helper(self) -> None:
        self.assertIn("ResolveTypeDescriptor", self.source)

    def test_is_value_type_handle_helper(self) -> None:
        self.assertIn("IsValueTypeHandle", self.source)
        self.assertIn("kFlagIsValueType", self.source)

    def test_recursion_guard_enter_leave(self) -> None:
        self.assertIn("RecursionGuard::Enter", self.source)
        self.assertIn("RecursionGuard::Leave", self.source)
        self.assertIn("RecursionGuard::ScopedEnter", self.source)

    def test_layout_engine_destructor_frees_cache(self) -> None:
        self.assertIn("LayoutEngine::~LayoutEngine", self.source)
        self.assertIn("CHAOS_IL2CPP_FREE(", self.source)

    def test_get_or_compute_layout_locks_and_caches(self) -> None:
        self.assertIn("LayoutEngine::GetOrComputeLayout", self.source)
        self.assertIn("CHAOS_IL2CPP_LOCK_GUARD", self.source)
        self.assertIn("ComputeLayoutInternal", self.source)
        self.assertIn("++cache_hits_", self.source)
        self.assertIn("++compute_count_", self.source)

    def test_invalidate_module_cache_implementation(self) -> None:
        self.assertIn("LayoutEngine::InvalidateModuleCache", self.source)

    def test_compute_layout_internal(self) -> None:
        self.assertIn("LayoutEngine::ComputeLayoutInternal", self.source)
        self.assertIn("RecursionGuard::ScopedEnter", self.source)
        self.assertIn("AlignUp(current_offset", self.source)
        self.assertIn("ComputeStructAlignment", self.source)

    def test_resolve_size_and_alignment_internal(self) -> None:
        self.assertIn("LayoutEngine::ResolveSizeAndAlignmentInternal", self.source)

    def test_resolve_field_type_implementation(self) -> None:
        self.assertIn("LayoutEngine::ResolveFieldType", self.source)
        self.assertIn("member_type_utf8", self.source)
        self.assertIn("type_args", self.source)

    def test_find_type_by_name(self) -> None:
        self.assertIn("LayoutEngine::FindTypeByName", self.source)
        self.assertIn("LookupModule", self.source)
        self.assertIn("FindReflectionQueryTypeByName", self.source)

    def test_align_up_implementation(self) -> None:
        self.assertIn("LayoutEngine::AlignUp", self.source)
        self.assertIn("alignment - 1u", self.source)

    def test_natural_alignment_implementation(self) -> None:
        self.assertIn("LayoutEngine::NaturalAlignment", self.source)

    def test_compute_struct_alignment_implementation(self) -> None:
        self.assertIn("LayoutEngine::ComputeStructAlignment", self.source)
        self.assertIn("max_alignment", self.source)

    def test_get_layout_engine_singleton(self) -> None:
        self.assertIn("LayoutEngine* GetLayoutEngine()", self.source)
        self.assertIn("static LayoutEngine s_engine", self.source)


class RuntimeInstantiationIntegrationTests(unittest.TestCase):
    """Verify that runtime_instantiation.cpp is wired to LayoutEngine."""

    def setUp(self) -> None:
        self.source = RUNTIME_INSTANTIATION_SOURCE_PATH.read_text(encoding="utf-8")
        self.header = RUNTIME_INSTANTIATION_HEADER_PATH.read_text(encoding="utf-8")

    def test_includes_layout_engine_header(self) -> None:
        self.assertIn('#include "layout_engine.h"', self.source)

    def test_compute_value_type_layout_delegates_to_layout_engine(self) -> None:
        self.assertIn("layout::GetLayoutEngine()", self.source)
        self.assertIn("engine->GetOrComputeLayout", self.source)
        self.assertIn("closed_handle", self.source)

    def test_field_offsets_copied_from_layout(self) -> None:
        self.assertIn("layout->fields[i].offset", self.source)
        self.assertIn("rt_type->field_offsets", self.source)

    def test_resolved_field_types_copied(self) -> None:
        self.assertIn("layout->fields[i].resolved_type", self.source)
        self.assertIn("rt_type->resolved_field_types", self.source)

    def test_resolved_field_types_added_to_header(self) -> None:
        self.assertIn("TypeInfoHandle*       resolved_field_types", self.header)

    def test_resolved_field_types_cleaned_up(self) -> None:
        self.assertIn("rt->resolved_field_types", self.source)
        # Should appear in UnregisterModuleGenerics cleanup
        self.assertIn("std::free(rt->resolved_field_types)", self.source)


if __name__ == "__main__":
    unittest.main()
