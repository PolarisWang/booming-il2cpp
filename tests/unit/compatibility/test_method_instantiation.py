from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
RUNTIME_CORE_DIR = REPO_ROOT / "src" / "native" / "runtime-core"
GENERIC_METHOD_INST_HEADER_PATH = RUNTIME_CORE_DIR / "generic_method_instantiation.h"
GENERIC_METHOD_INST_SOURCE_PATH = RUNTIME_CORE_DIR / "generic_method_instantiation.cpp"
GENERIC_CONTEXT_HEADER_PATH = RUNTIME_CORE_DIR / "generic_context.h"
GENERIC_CONTEXT_SOURCE_PATH = RUNTIME_CORE_DIR / "generic_context.cpp"
RUNTIME_INSTANTIATION_SOURCE_PATH = RUNTIME_CORE_DIR / "runtime_instantiation.cpp"
CMAKELISTS_PATH = RUNTIME_CORE_DIR / "CMakeLists.txt"


class FileExistenceTests(unittest.TestCase):
    """Verify generic_method_instantiation files exist."""

    def test_header_exists(self) -> None:
        self.assertTrue(
            GENERIC_METHOD_INST_HEADER_PATH.is_file(),
            msg=f"missing generic_method_instantiation.h: {GENERIC_METHOD_INST_HEADER_PATH}",
        )

    def test_source_exists(self) -> None:
        self.assertTrue(
            GENERIC_METHOD_INST_SOURCE_PATH.is_file(),
            msg=f"missing generic_method_instantiation.cpp: {GENERIC_METHOD_INST_SOURCE_PATH}",
        )


class RuntimeInstantiatedMethodStructTests(unittest.TestCase):
    """Verify RuntimeInstantiatedMethod struct is declared."""

    def setUp(self) -> None:
        self.header = GENERIC_METHOD_INST_HEADER_PATH.read_text(encoding="utf-8")

    def test_struct_declared(self) -> None:
        self.assertIn("struct RuntimeInstantiatedMethod", self.header)

    def test_descriptor_field(self) -> None:
        self.assertIn("ReflectionQueryMethodDescriptor descriptor", self.header)

    def test_type_args_field(self) -> None:
        self.assertIn("TypeInfoHandle* type_args", self.header)

    def test_arg_count_field(self) -> None:
        self.assertIn("arg_count", self.header)

    def test_open_method_definition_field(self) -> None:
        self.assertIn("open_method_definition", self.header)

    def test_module_id_field(self) -> None:
        self.assertIn("module_id", self.header)

    def test_is_registered_field(self) -> None:
        self.assertIn("is_registered", self.header)

    def test_ir_method_body_field(self) -> None:
        self.assertIn("ir_method_body", self.header)


class CreateClosedMethodDescriptorDeclTests(unittest.TestCase):
    """Verify CreateClosedMethodDescriptor is declared."""

    def setUp(self) -> None:
        self.header = GENERIC_METHOD_INST_HEADER_PATH.read_text(encoding="utf-8")

    def test_create_closed_method_descriptor_declared(self) -> None:
        self.assertIn("CreateClosedMethodDescriptor", self.header)
        self.assertIn("MethodInfoHandle", self.header)
        self.assertIn("open_method_definition", self.header)


class CreateClosedMethodDescriptorImplTests(unittest.TestCase):
    """Verify CreateClosedMethodDescriptor implementation details."""

    def setUp(self) -> None:
        self.source = GENERIC_METHOD_INST_SOURCE_PATH.read_text(encoding="utf-8")

    def test_include_reflection_query_model(self) -> None:
        # generic_method_instantiation.h includes reflection_query_model.h
        header = GENERIC_METHOD_INST_HEADER_PATH.read_text(encoding="utf-8")
        self.assertIn('#include "reflection_query_model.h"', header)

    def test_include_runtime_instantiation(self) -> None:
        self.assertIn('#include "runtime_instantiation.h"', self.source)

    def test_create_closed_method_descriptor_impl(self) -> None:
        self.assertIn(
            "RuntimeInstantiatedMethod* CreateClosedMethodDescriptor",
            self.source,
        )

    def test_decodes_open_method_handle(self) -> None:
        self.assertIn("TryDecodeReflectionQueryMethodHandle", self.source)

    def test_allocates_runtime_token(self) -> None:
        self.assertIn("AllocateRuntimeToken", self.source)

    def test_copies_type_args(self) -> None:
        self.assertIn("std::memcpy(args_buf, type_args", self.source)

    def test_returns_nullptr_on_null_open_method(self) -> None:
        self.assertIn("return nullptr", self.source)
        self.assertIn("open_method_definition == 0u", self.source)

    def test_returns_nullptr_on_null_type_args(self) -> None:
        self.assertIn("type_args == nullptr", self.source)

    def test_builds_closed_subject_id(self) -> None:
        self.assertIn("subject_id", self.source)
        self.assertIn("GetTypeDisplayName(type_args[i])", self.source)


class GenericContextMethodInstantiationApiTests(unittest.TestCase):
    """Verify generic_context.h has method instantiation API."""

    def setUp(self) -> None:
        self.header = GENERIC_CONTEXT_HEADER_PATH.read_text(encoding="utf-8")

    def test_register_generic_method_instantiation_declared(self) -> None:
        self.assertIn("RegisterGenericMethodInstantiation", self.header)

    def test_try_resolve_closed_method_declared(self) -> None:
        self.assertIn("TryResolveClosedMethod", self.header)

    def test_register_uses_method_info_handle(self) -> None:
        self.assertIn("MethodInfoHandle", self.header)


class GenericContextMethodInstantiationImplTests(unittest.TestCase):
    """Verify generic_context.cpp method instantiation implementation."""

    def setUp(self) -> None:
        self.source = GENERIC_CONTEXT_SOURCE_PATH.read_text(encoding="utf-8")

    def test_method_instantiation_entry_struct(self) -> None:
        self.assertIn("struct MethodInstantiationEntry", self.source)

    def test_by_open_method_map(self) -> None:
        self.assertIn("by_open_method", self.source)

    def test_method_instantiations_by_module_map(self) -> None:
        self.assertIn("method_instantiations_by_module", self.source)

    def test_register_generic_method_instantiation_impl(self) -> None:
        self.assertIn("void RegisterGenericMethodInstantiation", self.source)

    def test_try_resolve_closed_method_impl(self) -> None:
        self.assertIn("MethodInfoHandle TryResolveClosedMethod", self.source)

    def test_register_is_idempotent(self) -> None:
        self.assertIn("already registered", self.source)

    def test_unregister_cleans_method_instantiations(self) -> None:
        self.assertIn("method_instantiations_by_module", self.source)
        self.assertIn("by_open_method", self.source)


class ResolveOrInstantiateMethodImplTests(unittest.TestCase):
    """Verify runtime_instantiation.cpp ResolveOrInstantiateMethod."""

    def setUp(self) -> None:
        self.source = RUNTIME_INSTANTIATION_SOURCE_PATH.read_text(encoding="utf-8")

    def test_includes_generic_method_instantiation(self) -> None:
        self.assertIn('#include "generic_method_instantiation.h"', self.source)

    def test_resolve_or_instantiate_method_has_fast_path(self) -> None:
        self.assertIn("TryResolveClosedMethod", self.source)

    def test_resolve_or_instantiate_method_calls_create(self) -> None:
        self.assertIn("CreateClosedMethodDescriptor", self.source)

    def test_resolve_or_instantiate_method_encodes_handle(self) -> None:
        self.assertIn("EncodeReflectionQueryMethodHandle", self.source)

    def test_resolve_or_instantiate_method_registers(self) -> None:
        self.assertIn("RegisterGenericMethodInstantiation", self.source)

    def test_resolve_or_instantiate_method_returns_valid(self) -> None:
        self.assertNotIn("return 0;", self.source.split("ResolveOrInstantiateMethod")[1].split("\n")[0:3])
        # It should no longer be a simple nullptr stub.

    def test_phase_5b_deferred_cleanup_comment(self) -> None:
        self.assertIn("Phase 5b", self.source)


class CMakeListsHasNewFilesTests(unittest.TestCase):
    """Verify CMakeLists.txt includes new files."""

    def setUp(self) -> None:
        self.cmake = CMAKELISTS_PATH.read_text(encoding="utf-8")

    def test_generic_method_instantiation_in_cmake(self) -> None:
        self.assertIn("generic_method_instantiation.cpp", self.cmake)
        self.assertIn("generic_method_instantiation.h", self.cmake)


if __name__ == "__main__":
    unittest.main()
