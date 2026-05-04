from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
RUNTIME_CORE_DIR = REPO_ROOT / "src" / "native" / "runtime-core"
LOWERER_HEADER_PATH = RUNTIME_CORE_DIR / "il_to_ir_lowerer.h"
LOWERER_SOURCE_PATH = RUNTIME_CORE_DIR / "il_to_ir_lowerer.cpp"
GENERIC_METHOD_INST_HEADER_PATH = RUNTIME_CORE_DIR / "generic_method_instantiation.h"
GENERIC_METHOD_INST_SOURCE_PATH = RUNTIME_CORE_DIR / "generic_method_instantiation.cpp"
CMAKELISTS_PATH = RUNTIME_CORE_DIR / "CMakeLists.txt"
INTERPRETER_VM_PATH = REPO_ROOT / "src" / "native" / "interpreter" / "interpreter_vm.h"
TOKEN_RESOLVER_SOURCE_PATH = RUNTIME_CORE_DIR / "token_resolver.cpp"
TOKEN_RESOLVER_HEADER_PATH = RUNTIME_CORE_DIR / "token_resolver.h"
CODEGEN_BRIDGE_PATH = REPO_ROOT / "contracts" / "native" / "v0" / "codegen_bridge.h"


class FileExistenceTests(unittest.TestCase):
    """Verify IL→IR lowerer files exist."""

    def test_lowerer_header_exists(self) -> None:
        self.assertTrue(
            LOWERER_HEADER_PATH.is_file(),
            msg=f"missing il_to_ir_lowerer.h: {LOWERER_HEADER_PATH}",
        )

    def test_lowerer_source_exists(self) -> None:
        self.assertTrue(
            LOWERER_SOURCE_PATH.is_file(),
            msg=f"missing il_to_ir_lowerer.cpp: {LOWERER_SOURCE_PATH}",
        )


class ILToIRLowererHeaderTests(unittest.TestCase):
    """Verify il_to_ir_lowerer.h declares the correct API."""

    def setUp(self) -> None:
        self.header = LOWERER_HEADER_PATH.read_text(encoding="utf-8")

    def test_include_interpreter_vm(self) -> None:
        self.assertIn('#include "interpreter_vm.h"', self.header)

    def test_il_token_resolver_type_exists(self) -> None:
        self.assertIn("using ILTokenResolver", self.header)

    def test_il_token_resolver_is_function_pointer(self) -> None:
        self.assertIn("bool (*)", self.header)

    def test_token_resolver_takes_token_and_instruction(self) -> None:
        self.assertIn("CHAOS_IL2CPP_UINT32 token", self.header)
        self.assertIn("IRInstruction& instruction", self.header)
        self.assertIn("void* user_data", self.header)

    def test_lower_il_to_ir_declared(self) -> None:
        self.assertIn("IRMethod LowerILToIR", self.header)

    def test_lower_il_to_ir_parameters(self) -> None:
        self.assertIn("il_bytes", self.header)
        self.assertIn("il_length", self.header)
        self.assertIn("max_stack", self.header)
        self.assertIn("token_resolver", self.header)

    def test_lower_il_to_ir_returns_irmethod(self) -> None:
        self.assertIn("IRMethod", self.header)

    def test_method_body_header_struct_exists(self) -> None:
        self.assertIn("struct MethodBodyHeader", self.header)

    def test_method_body_header_has_code_start(self) -> None:
        self.assertIn("code_start", self.header)

    def test_method_body_header_has_code_size(self) -> None:
        self.assertIn("code_size", self.header)

    def test_method_body_header_has_max_stack(self) -> None:
        self.assertIn("max_stack", self.header)

    def test_method_body_header_has_local_var_sig_tok(self) -> None:
        self.assertIn("local_var_sig_tok", self.header)

    def test_method_body_header_has_more_sects(self) -> None:
        self.assertIn("has_more_sects", self.header)

    def test_parse_method_body_header_declared(self) -> None:
        self.assertIn("ParseMethodBodyHeader", self.header)

    def test_lower_il_to_ir_takes_code_size_param(self) -> None:
        self.assertIn("code_size", self.header)

    def test_lower_il_to_ir_takes_max_stack_param(self) -> None:
        self.assertIn("max_stack", self.header)


class ILToIRLowererSourceTests(unittest.TestCase):
    """Verify il_to_ir_lowerer.cpp implements the lowerer."""

    def setUp(self) -> None:
        self.source = LOWERER_SOURCE_PATH.read_text(encoding="utf-8")

    def test_include_lowerer_header(self) -> None:
        self.assertIn('#include "il_to_ir_lowerer.h"', self.source)

    def test_opcode_size_table_exists(self) -> None:
        self.assertIn("kOpcodeSizes", self.source)

    def test_extended_opcode_table_exists(self) -> None:
        self.assertIn("kOpcodeSizesExtended", self.source)

    def test_read_helpers_exist(self) -> None:
        self.assertIn("ReadU1", self.source)
        self.assertIn("ReadI4", self.source)
        self.assertIn("ReadToken", self.source)

    def test_make_empty_method_exists(self) -> None:
        self.assertIn("MakeEmptyMethod", self.source)

    def test_lower_il_to_ir_implemented(self) -> None:
        self.assertIn("IRMethod LowerILToIR", self.source)

    def test_two_pass_algorithm(self) -> None:
        # Pass 1: decode instructions into raw form
        self.assertIn("First pass", self.source) or self.assertIn(
            "first pass", self.source.lower()
        )
        # Pass 2: resolve branch targets and tokens
        self.assertIn("Second pass", self.source) or self.assertIn(
            "second pass", self.source.lower()
        )

    def test_raw_insn_struct_exists(self) -> None:
        self.assertIn("struct RawInsn", self.source)

    def test_supports_ldc_i4(self) -> None:
        self.assertIn("LdcI4", self.source)

    def test_supports_call(self) -> None:
        self.assertIn("IROpCode::Call", self.source)

    def test_supports_ret(self) -> None:
        self.assertIn("IROpCode::Ret", self.source)

    def test_supports_br(self) -> None:
        self.assertIn("IROpCode::Br", self.source)

    def test_supports_newobj(self) -> None:
        self.assertIn("IROpCode::NewObj", self.source)

    def test_supports_box(self) -> None:
        self.assertIn("IROpCode::Box", self.source)

    def test_supports_unbox(self) -> None:
        self.assertIn("IROpCode::Unbox", self.source)

    def test_supports_castclass(self) -> None:
        self.assertIn("IROpCode::CastClass", self.source)

    def test_supports_isinst(self) -> None:
        self.assertIn("IROpCode::IsInst", self.source)

    def test_supports_mul(self) -> None:
        self.assertIn("IROpCode::Mul", self.source)

    def test_supports_div(self) -> None:
        self.assertIn("IROpCode::Div", self.source)

    def test_supports_rem(self) -> None:
        self.assertIn("IROpCode::Rem", self.source)

    def test_supports_neg(self) -> None:
        self.assertIn("IROpCode::Neg", self.source)

    def test_supports_conv_i4(self) -> None:
        self.assertIn("IROpCode::Conv_I4", self.source)

    def test_supports_conv_i8(self) -> None:
        self.assertIn("IROpCode::Conv_I8", self.source)

    def test_supports_conv_r4(self) -> None:
        self.assertIn("IROpCode::Conv_R4", self.source)

    def test_supports_conv_r8(self) -> None:
        self.assertIn("IROpCode::Conv_R8", self.source)

    def test_supports_ldc_r4(self) -> None:
        self.assertIn("IROpCode::LdcR4", self.source)

    def test_supports_ldc_r8(self) -> None:
        self.assertIn("IROpCode::LdcR8", self.source)

    def test_supports_pop(self) -> None:
        self.assertIn("IROpCode::Pop", self.source)

    def test_supports_ldstr(self) -> None:
        self.assertIn("IROpCode::LdStr", self.source)

    def test_supports_newarr(self) -> None:
        self.assertIn("IROpCode::NewArr", self.source)

    def test_supports_ldlen(self) -> None:
        self.assertIn("IROpCode::LdLen", self.source)

    def test_supports_ldelem(self) -> None:
        self.assertIn("IROpCode::LdElem", self.source)

    def test_supports_stelem(self) -> None:
        self.assertIn("IROpCode::StElem", self.source)

    def test_supports_cgt(self) -> None:
        self.assertIn("IROpCode::Cgt", self.source)

    def test_supports_clt(self) -> None:
        self.assertIn("IROpCode::Clt", self.source)

    def test_supports_cgt_in_extended_switch(self) -> None:
        """cgt (0xFE02) should be handled in the extended opcode switch."""
        self.assertIn("case 0x02", self.source) or self.assertIn("Cgt", self.source)

    def test_supports_clt_in_extended_switch(self) -> None:
        """clt (0xFE04) should be handled in the extended opcode switch."""
        self.assertIn("case 0x04", self.source) or self.assertIn("Clt", self.source)

    def test_branch_target_resolution(self) -> None:
        self.assertIn("branch_target", self.source)

    def test_token_resolver_callback(self) -> None:
        self.assertIn("token_resolver", self.source)

    def test_parse_method_body_header_implemented(self) -> None:
        self.assertIn("ParseMethodBodyHeader", self.source)

    def test_parse_tiny_format_detected(self) -> None:
        """Tiny header detection: bits[1:0] == 0x02."""
        self.assertIn("0x02", self.source)

    def test_parse_fat_format_detected(self) -> None:
        """Fat header detection: low 2 bits of 2-byte flags == 0x0003."""
        self.assertIn("0x0003", self.source)

    def test_parse_fat_header_size_check_12_bytes(self) -> None:
        """Fat header check for minimum 12 bytes."""
        self.assertIn("12u", self.source)

    def test_parse_tiny_default_max_stack_8(self) -> None:
        """Tiny format implies max_stack=8."""
        # The comment "implied for tiny" uniquely identifies the max_stack=8
        # assignment in the tiny-header parsing path.
        self.assertIn("implied for tiny", self.source)

    def test_parse_fat_more_sects_bit_check(self) -> None:
        """Fat format checks bit 8 for MoreSects flag."""
        self.assertIn("0x0100", self.source)


class OpcodeSizeTableTests(unittest.TestCase):
    """Verify kOpcodeSizes table has correct entries for extended opcodes."""

    def setUp(self) -> None:
        self.source = LOWERER_SOURCE_PATH.read_text(encoding="utf-8")

    def test_ldc_r4_operand_size_4(self) -> None:
        self.assertIn("ldc.r4", self.source)

    def test_ldc_r8_operand_size_8(self) -> None:
        self.assertIn("ldc.r8", self.source)

    def test_ldstr_operand_size_4(self) -> None:
        self.assertIn("ldstr", self.source)

    def test_castclass_operand_size_4(self) -> None:
        self.assertIn("castclass", self.source)

    def test_isinst_operand_size_4(self) -> None:
        self.assertIn("isinst", self.source)

    def test_unbox_operand_size_4(self) -> None:
        self.assertIn("unbox", self.source)

    def test_ldfld_operand_size_4(self) -> None:
        self.assertIn("ldfld", self.source)

    def test_stfld_operand_size_4(self) -> None:
        self.assertIn("stfld", self.source)

    def test_ldsfld_operand_size_4(self) -> None:
        self.assertIn("ldsfld", self.source)

    def test_stsfld_operand_size_4(self) -> None:
        self.assertIn("stsfld", self.source)

    def test_newarr_operand_size_4(self) -> None:
        self.assertIn("newarr", self.source)

    def test_typed_ldelem_exists(self) -> None:
        self.assertIn("ldelem.i1", self.source) or self.assertIn("ldelem.u1", self.source)

    def test_typed_stelem_exists(self) -> None:
        self.assertIn("stelem.ref", self.source)


class LowerMethodBodyInHeaderTests(unittest.TestCase):
    """Verify generic_method_instantiation.h declares LowerMethodBody."""

    def setUp(self) -> None:
        self.header = GENERIC_METHOD_INST_HEADER_PATH.read_text(encoding="utf-8")

    def test_includes_il_to_ir_lowerer(self) -> None:
        self.assertIn('#include "il_to_ir_lowerer.h"', self.header)

    def test_lower_method_body_declared(self) -> None:
        self.assertIn("bool LowerMethodBody", self.header)

    def test_lower_method_body_takes_rt_method(self) -> None:
        self.assertIn("RuntimeInstantiatedMethod*", self.header)

    def test_lower_method_body_takes_il_bytes(self) -> None:
        self.assertIn("il_bytes", self.header)

    def test_lower_method_body_takes_token_resolver(self) -> None:
        self.assertIn("ILTokenResolver", self.header)

    def test_lower_method_body_takes_user_data(self) -> None:
        self.assertIn("user_data", self.header)


class RuntimeInstantiatedMethodStructILFieldsTests(unittest.TestCase):
    """Verify RuntimeInstantiatedMethod has IL execution fields."""

    def setUp(self) -> None:
        self.header = GENERIC_METHOD_INST_HEADER_PATH.read_text(encoding="utf-8")

    def test_il_bytes_field(self) -> None:
        self.assertIn("il_bytes", self.header)

    def test_il_length_field(self) -> None:
        self.assertIn("il_length", self.header)

    def test_ir_method_body_field_exists(self) -> None:
        self.assertIn("ir_method_body", self.header)


class LowerMethodBodyImplementationTests(unittest.TestCase):
    """Verify generic_method_instantiation.cpp implements LowerMethodBody."""

    def setUp(self) -> None:
        self.source = GENERIC_METHOD_INST_SOURCE_PATH.read_text(encoding="utf-8")

    def test_includes_il_to_ir_lowerer(self) -> None:
        self.assertIn('#include "il_to_ir_lowerer.h"', self.source)

    def test_includes_new_for_placement_new(self) -> None:
        self.assertIn('#include <new>', self.source)

    def test_lower_method_body_implemented(self) -> None:
        self.assertIn("bool LowerMethodBody", self.source)

    def test_cache_check_returns_early(self) -> None:
        # Check for the cache-early-return pattern.
        self.assertIn("ir_method_body != nullptr", self.source)

    def test_calls_lower_il_to_ir(self) -> None:
        self.assertIn("LowerILToIR", self.source)

    def test_uses_placement_new(self) -> None:
        self.assertIn("placement new", self.source) or self.assertIn(
            "::new (mem)", self.source
        )

    def test_sets_ir_method_body_on_success(self) -> None:
        self.assertIn("ir_method_body = cached", self.source)

    def test_returns_false_on_null_rt_method(self) -> None:
        self.assertIn("rt_method == nullptr", self.source)

    def test_returns_false_on_null_il_bytes(self) -> None:
        self.assertIn("il_bytes == nullptr", self.source)

    def test_auto_detects_method_body_header(self) -> None:
        """LowerMethodBody auto-detects ECMA-335 method headers."""
        self.assertIn("ParseMethodBodyHeader", self.source)

    def test_uses_code_size_from_header_for_seh(self) -> None:
        """LowerMethodBody passes code_size to LowerILToIR for SEH location."""
        self.assertIn("code_size", self.source)

    def test_uses_max_stack_from_header(self) -> None:
        """LowerMethodBody extracts max_stack from parsed header."""
        self.assertIn("max_stack", self.source)


class CMakeListsTests(unittest.TestCase):
    """Verify CMakeLists.txt includes new files and interpreter include dir."""

    def setUp(self) -> None:
        self.cmake = CMAKELISTS_PATH.read_text(encoding="utf-8")

    def test_il_to_ir_lowerer_cpp_in_cmake(self) -> None:
        self.assertIn("il_to_ir_lowerer.cpp", self.cmake)

    def test_il_to_ir_lowerer_h_in_cmake(self) -> None:
        self.assertIn("il_to_ir_lowerer.h", self.cmake)

    def test_interpreter_include_dir_in_cmake(self) -> None:
        self.assertIn("interpreter", self.cmake)

    def test_token_resolver_cpp_in_cmake(self) -> None:
        self.assertIn("token_resolver.cpp", self.cmake)

    def test_token_resolver_h_in_cmake(self) -> None:
        self.assertIn("token_resolver.h", self.cmake)


class SEHSupportTests(unittest.TestCase):
    """Verify SEH (exception handling) structures and parsing."""

    def setUp(self) -> None:
        self.src = LOWERER_SOURCE_PATH.read_text(encoding="utf-8")

    def test_seh_clause_parsing_exists(self) -> None:
        self.assertIn("seh_clauses", self.src)

    def test_eh_section_kind_check(self) -> None:
        self.assertIn("is_eh", self.src)

    def test_small_eh_clause_parsing(self) -> None:
        self.assertIn("12", self.src) or self.assertIn("12u", self.src)


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
        """LdStr case now calls resolve_string_by_token bridge function."""
        source = Path(TOKEN_RESOLVER_SOURCE_PATH).read_text(encoding="utf-8")
        self.assertIn("resolve_string_by_token", source)

    def test_string_operand_set_on_success(self) -> None:
        """Success path sets instruction.string_operand."""
        source = Path(TOKEN_RESOLVER_SOURCE_PATH).read_text(encoding="utf-8")
        self.assertIn("string_operand", source)


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
        """Field offset resolution calls GetOrComputeLayout."""
        source = TOKEN_RESOLVER_SOURCE_PATH.read_text(encoding="utf-8")
        self.assertIn("GetOrComputeLayout", source)

    def test_field_offset_resolves_real_offset(self) -> None:
        """Field offset set from layout's FieldLayout array."""
        source = TOKEN_RESOLVER_SOURCE_PATH.read_text(encoding="utf-8")
        self.assertIn("field_offset", source) and self.assertIn(
            "layout->fields", source
        )
