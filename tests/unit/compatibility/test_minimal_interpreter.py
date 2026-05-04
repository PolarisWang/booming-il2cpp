from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
INTERPRETER_DIR = REPO_ROOT / "src" / "native" / "interpreter"
INTERPRETER_HEADER_PATH = INTERPRETER_DIR / "interpreter_vm.h"
INTERPRETER_SOURCE_PATH = INTERPRETER_DIR / "interpreter_vm.cpp"


class InterpreterFileExistenceTests(unittest.TestCase):
    """Verify interpreter files exist."""

    def test_interpreter_header_exists(self) -> None:
        self.assertTrue(
            INTERPRETER_HEADER_PATH.is_file(),
            msg=f"missing interpreter_vm.h: {INTERPRETER_HEADER_PATH}",
        )

    def test_interpreter_source_exists(self) -> None:
        self.assertTrue(
            INTERPRETER_SOURCE_PATH.is_file(),
            msg=f"missing interpreter_vm.cpp: {INTERPRETER_SOURCE_PATH}",
        )


class InterpreterStructValueTagTests(unittest.TestCase):
    """Verify ValueTag::Struct is declared."""

    def setUp(self) -> None:
        self.header = INTERPRETER_HEADER_PATH.read_text(encoding="utf-8")

    def test_struct_tag_declared(self) -> None:
        self.assertIn("Struct = 7", self.header)


class InterpreterValueStructStorageTests(unittest.TestCase):
    """Verify InterpreterValue supports struct storage."""

    def setUp(self) -> None:
        self.header = INTERPRETER_HEADER_PATH.read_text(encoding="utf-8")

    def test_struct_size_field(self) -> None:
        self.assertIn("struct_size", self.header)

    def test_from_struct_factory(self) -> None:
        self.assertIn("from_struct", self.header)

    def test_free_struct_method(self) -> None:
        self.assertIn("FreeStruct", self.header)

    def test_copy_constructor_declared(self) -> None:
        self.assertIn("InterpreterValue(const InterpreterValue& other)", self.header)

    def test_copy_assignment_declared(self) -> None:
        self.assertIn("operator=(const InterpreterValue& other)", self.header)

    def test_destructor_declared(self) -> None:
        self.assertIn("~InterpreterValue", self.header)


class ExecutionResultExtensionTests(unittest.TestCase):
    """Verify ExecutionResult is extended for external dispatch."""

    def setUp(self) -> None:
        self.header = INTERPRETER_HEADER_PATH.read_text(encoding="utf-8")

    def test_needs_external_dispatch_declared(self) -> None:
        self.assertIn("needs_external_dispatch", self.header)

    def test_call_target_declared(self) -> None:
        self.assertIn("call_target", self.header)

    def test_call_args_declared(self) -> None:
        self.assertIn("call_args", self.header)


class ExecutionFrameDestructorTests(unittest.TestCase):
    """Verify ExecutionFrame has a destructor."""

    def setUp(self) -> None:
        self.header = INTERPRETER_HEADER_PATH.read_text(encoding="utf-8")

    def test_destructor_declared(self) -> None:
        self.assertIn("~ExecutionFrame", self.header)

    def test_non_copyable(self) -> None:
        self.assertIn("ExecutionFrame(const ExecutionFrame&) = delete", self.header)


class IRInstructionCallTargetTests(unittest.TestCase):
    """Verify IRInstruction carries call_target and arg_count."""

    def setUp(self) -> None:
        self.header = INTERPRETER_HEADER_PATH.read_text(encoding="utf-8")

    def test_call_target_field(self) -> None:
        self.assertIn("void* call_target", self.header)

    def test_arg_count_field(self) -> None:
        self.assertIn("arg_count", self.header)


class InterpreterSourceImplementationTests(unittest.TestCase):
    """Verify interpreter_vm.cpp implementations."""

    def setUp(self) -> None:
        self.source = INTERPRETER_SOURCE_PATH.read_text(encoding="utf-8")

    def test_free_struct_implementation(self) -> None:
        self.assertIn("FreeStruct", self.source)
        self.assertIn("std::free(obj)", self.source)

    def test_copy_constructor_implementation(self) -> None:
        self.assertIn("InterpreterValue::InterpreterValue(const InterpreterValue& other)", self.source)
        self.assertIn("std::memcpy(obj, other.obj, struct_size)", self.source)

    def test_from_struct_implementation(self) -> None:
        self.assertIn("InterpreterValue::from_struct", self.source)
        self.assertIn("std::malloc(size)", self.source)
        self.assertIn("std::memcpy(result.obj, data, size)", self.source)

    def test_execution_frame_destructor_frees_structs(self) -> None:
        self.assertIn("ExecutionFrame::~ExecutionFrame", self.source)
        self.assertIn("v.FreeStruct", self.source)

    def test_call_sets_external_dispatch(self) -> None:
        self.assertIn("needs_external_dispatch = true", self.source)
        self.assertIn("result.call_target = instruction.call_target", self.source)

    def test_call_collects_args(self) -> None:
        self.assertIn("result.call_args", self.source)
        self.assertIn("Pop(&frame->stack)", self.source)


if __name__ == "__main__":
    unittest.main()
