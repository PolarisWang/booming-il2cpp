from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
RUNTIME_CORE_DIR = REPO_ROOT / "src" / "native" / "runtime-core"
AOT_CORE_IR_HEADER_PATH = RUNTIME_CORE_DIR / "aot_core_ir_reader.h"
AOT_CORE_IR_SOURCE_PATH = RUNTIME_CORE_DIR / "aot_core_ir_reader.cpp"
GENERIC_METHOD_INST_HEADER_PATH = RUNTIME_CORE_DIR / "generic_method_instantiation.h"
GENERIC_METHOD_INST_SOURCE_PATH = RUNTIME_CORE_DIR / "generic_method_instantiation.cpp"
CMAKELISTS_PATH = RUNTIME_CORE_DIR / "CMakeLists.txt"
INTERPRETER_VM_PATH = REPO_ROOT / "src" / "native" / "interpreter" / "interpreter_vm.h"
TOKEN_RESOLVER_SOURCE_PATH = RUNTIME_CORE_DIR / "token_resolver.cpp"
TOKEN_RESOLVER_HEADER_PATH = RUNTIME_CORE_DIR / "token_resolver.h"
CODEGEN_BRIDGE_PATH = REPO_ROOT / "contracts" / "native" / "v0" / "codegen_bridge.h"


class AotCoreIrReaderExistenceTests(unittest.TestCase):
    """Verify AotCoreIr reader files exist (replaces il_to_ir_lowerer)."""

    def test_reader_header_exists(self) -> None:
        self.assertTrue(
            AOT_CORE_IR_HEADER_PATH.is_file(),
            msg=f"missing aot_core_ir_reader.h: {AOT_CORE_IR_HEADER_PATH}",
        )

    def test_reader_source_exists(self) -> None:
        self.assertTrue(
            AOT_CORE_IR_SOURCE_PATH.is_file(),
            msg=f"missing aot_core_ir_reader.cpp: {AOT_CORE_IR_SOURCE_PATH}",
        )


class AotCoreIrReaderHeaderTests(unittest.TestCase):
    """Verify aot_core_ir_reader.h declares the correct API."""

    def setUp(self) -> None:
        self.header = AOT_CORE_IR_HEADER_PATH.read_text(encoding="utf-8")

    def test_include_interpreter_vm(self) -> None:
        self.assertIn('#include "interpreter_vm.h"', self.header)

    def test_resolve_subject_id_fn_exists(self) -> None:
        self.assertIn("ResolveSubjectIdFn", self.header)

    def test_deserialize_aot_core_ir_declared(self) -> None:
        self.assertIn("DeserializeAotCoreIrMethod", self.header)

    def test_deserialize_takes_json_and_length(self) -> None:
        self.assertIn("const char* json", self.header)
        self.assertIn("size_t length", self.header)

    def test_deserialize_takes_resolve_fn(self) -> None:
        self.assertIn("ResolveSubjectIdFn", self.header)

    def test_deserialize_returns_irmethod(self) -> None:
        self.assertIn("interpreter::IRMethod", self.header)


class LowerMethodBodyInHeaderTests(unittest.TestCase):
    """Verify generic_method_instantiation.h declares LowerMethodBody."""

    def setUp(self) -> None:
        self.header = GENERIC_METHOD_INST_HEADER_PATH.read_text(encoding="utf-8")

    def test_includes_aot_core_ir_reader(self) -> None:
        self.assertIn('#include "aot_core_ir_reader.h"', self.header)

    def test_lower_method_body_declared(self) -> None:
        self.assertIn("bool LowerMethodBody", self.header)

    def test_lower_method_body_takes_rt_method(self) -> None:
        self.assertIn("RuntimeInstantiatedMethod*", self.header)

    def test_lower_method_body_takes_json_and_length(self) -> None:
        self.assertIn("aot_core_ir_json", self.header)
        self.assertIn("aot_core_ir_json_length", self.header)

    def test_lower_method_body_takes_resolve_subject_id(self) -> None:
        self.assertIn("ResolveSubjectIdFn", self.header)


class RuntimeInstantiatedMethodJSONFieldsTests(unittest.TestCase):
    """Verify RuntimeInstantiatedMethod has AotCoreIr JSON fields."""

    def setUp(self) -> None:
        self.header = GENERIC_METHOD_INST_HEADER_PATH.read_text(encoding="utf-8")

    def test_aot_core_ir_json_field(self) -> None:
        self.assertIn("aot_core_ir_json", self.header)

    def test_aot_core_ir_json_length_field(self) -> None:
        self.assertIn("aot_core_ir_json_length", self.header)

    def test_ir_method_body_field_exists(self) -> None:
        self.assertIn("ir_method_body", self.header)


class LowerMethodBodyImplementationTests(unittest.TestCase):
    """Verify generic_method_instantiation.cpp implements LowerMethodBody."""

    def setUp(self) -> None:
        self.source = GENERIC_METHOD_INST_SOURCE_PATH.read_text(encoding="utf-8")

    def test_includes_aot_core_ir_reader(self) -> None:
        self.assertIn('#include "aot_core_ir_reader.h"', self.source)

    def test_includes_new_for_placement_new(self) -> None:
        self.assertIn('#include <new>', self.source)

    def test_lower_method_body_implemented(self) -> None:
        self.assertIn("bool LowerMethodBody", self.source)

    def test_cache_check_returns_early(self) -> None:
        self.assertIn("ir_method_body != nullptr", self.source)

    def test_calls_deserialize_aot_core_ir(self) -> None:
        self.assertIn("DeserializeAotCoreIrMethod", self.source)

    def test_uses_placement_new(self) -> None:
        self.assertIn("placement new", self.source) or self.assertIn(
            "::new (mem)", self.source
        )

    def test_sets_ir_method_body_on_success(self) -> None:
        self.assertIn("ir_method_body = cached", self.source)

    def test_returns_false_on_null_rt_method(self) -> None:
        self.assertIn("rt_method == nullptr", self.source)

    def test_returns_false_on_null_json(self) -> None:
        self.assertIn("aot_core_ir_json == nullptr", self.source)


class CMakeListsTests(unittest.TestCase):
    """Verify CMakeLists.txt includes new files and interpreter include dir."""

    def setUp(self) -> None:
        self.cmake = CMAKELISTS_PATH.read_text(encoding="utf-8")

    def test_aot_core_ir_reader_cpp_in_cmake(self) -> None:
        self.assertIn("aot_core_ir_reader.cpp", self.cmake)

    def test_aot_core_ir_reader_h_in_cmake(self) -> None:
        self.assertIn("aot_core_ir_reader.h", self.cmake)

    def test_interpreter_include_dir_in_cmake(self) -> None:
        self.assertIn("interpreter", self.cmake)

    def test_token_resolver_cpp_in_cmake(self) -> None:
        self.assertIn("token_resolver.cpp", self.cmake)

    def test_token_resolver_h_in_cmake(self) -> None:
        self.assertIn("token_resolver.h", self.cmake)

    def test_il_to_ir_lowerer_not_in_cmake(self) -> None:
        """il_to_ir_lowerer files should have been removed from CMakeLists."""
        self.assertNotIn("il_to_ir_lowerer.cpp", self.cmake)
        self.assertNotIn("il_to_ir_lowerer.h", self.cmake)


class SEHSupportTests(unittest.TestCase):
    """Verify SEH (exception handling) structures."""

    def setUp(self) -> None:
        self.src = AOT_CORE_IR_SOURCE_PATH.read_text(encoding="utf-8")

    def test_seh_clause_parsing_exists(self) -> None:
        self.assertIn("seh_clauses", self.src)

    def test_exception_region_parsing(self) -> None:
        self.assertIn("exceptionRegions", self.src)

    def test_handling_kind_parsing(self) -> None:
        self.assertIn("handlingKindCode", self.src)


class InterpreterVMSEHTests(unittest.TestCase):
    """Verify interpreter_vm.h declares SEH types."""

    def setUp(self) -> None:
        self.header = INTERPRETER_VM_PATH.read_text(encoding="utf-8")

    def test_sehflags_enum_exists(self) -> None:
        self.assertIn("SEHFlags", self.header)

    def test_seh_clause_struct_exists(self) -> None:
        self.assertIn("SEHClause", self.header)

    def test_seh_clause_has_flags(self) -> None:
        self.assertIn("flags", self.header)

    def test_seh_clause_has_try_start(self) -> None:
        self.assertIn("try_start_idx", self.header)

    def test_seh_clause_has_handler_start(self) -> None:
        self.assertIn("handler_start_idx", self.header)

    def test_irmethod_has_seh_clauses(self) -> None:
        self.assertIn("seh_clauses", self.header)


class InterpreterDispatchSEHTests(unittest.TestCase):
    """Verify interpreter_vm.cpp has SEH dispatch logic."""

    def setUp(self) -> None:
        self.source = Path(REPO_ROOT / "src" / "native" / "interpreter" / "interpreter_vm.cpp").read_text(encoding="utf-8")

    def test_throw_searches_for_catch(self) -> None:
        self.assertIn("FindEnclosingCatch", self.source)

    def test_leave_searches_for_finally(self) -> None:
        self.assertIn("FindEnclosingFinally", self.source)

    def test_throw_has_handler_branch(self) -> None:
        self.assertIn("handler_start_idx", self.source)

    def test_endfinally_resumes_leave(self) -> None:
        self.assertIn("pending_leave", self.source)


class TokenResolverExtendedTests(unittest.TestCase):
    """Verify DefaultTokenResolver handles extended opcodes."""

    def setUp(self) -> None:
        self.source = TOKEN_RESOLVER_SOURCE_PATH.read_text(encoding="utf-8")

    def test_resolves_unbox_type_token(self) -> None:
        self.assertIn("IROpCode::Unbox", self.source)

    def test_resolves_newarr_type_token(self) -> None:
        self.assertIn("IROpCode::NewArr", self.source)

    def test_resolves_ldelem_type_token(self) -> None:
        self.assertIn("IROpCode::LdElem", self.source)

    def test_resolves_stelem_type_token(self) -> None:
        self.assertIn("IROpCode::StElem", self.source)

    def test_ldstr_returns_false(self) -> None:
        self.assertIn("IROpCode::LdStr", self.source)

    def test_resolve_string_by_token_called(self) -> None:
        self.assertIn("resolve_string_by_token", self.source)

    def test_string_operand_set_on_success(self) -> None:
        self.assertIn("string_operand", self.source)


class CodegenBridgeStringResolutionTests(unittest.TestCase):
    """Verify CodegenBridgeV0 has resolve_string_by_token."""

    def setUp(self) -> None:
        self.bridge = CODEGEN_BRIDGE_PATH.read_text(encoding="utf-8")

    def test_resolve_string_by_token_declared(self) -> None:
        self.assertIn("resolve_string_by_token", self.bridge)

    def test_resolve_string_by_token_takes_image(self) -> None:
        self.assertIn("ImageHandle image", self.bridge)

    def test_resolve_string_by_token_returns_const_char(self) -> None:
        self.assertIn("const char*", self.bridge)

    def test_resolve_string_by_token_takes_token(self) -> None:
        self.assertIn("string_token", self.bridge)


class TokenResolverContextLayoutEngineTests(unittest.TestCase):
    """Verify TokenResolverContext has layout_engine pointer."""

    def setUp(self) -> None:
        self.header = TOKEN_RESOLVER_HEADER_PATH.read_text(encoding="utf-8")

    def test_layout_engine_in_context(self) -> None:
        self.assertIn("layout_engine", self.header)

    def test_layout_engine_is_forward_declared(self) -> None:
        self.assertIn("class LayoutEngine", self.header) or self.assertIn(
            "LayoutEngine*", self.header
        )

    def test_field_offset_resolution_in_source(self) -> None:
        source = TOKEN_RESOLVER_SOURCE_PATH.read_text(encoding="utf-8")
        self.assertIn("GetOrComputeLayout", source)

    def test_field_offset_resolves_real_offset(self) -> None:
        source = TOKEN_RESOLVER_SOURCE_PATH.read_text(encoding="utf-8")
        self.assertIn("field_offset", source) and self.assertIn(
            "layout->fields", source
        )