from tests.unit.compatibility.interpreter_mixed_execution_test_support import *


class TestInterpreterMixedExecutionIr(InterpreterMixedExecutionTestSupport):
    def test_interpreter_project_isolation_and_legacy_solution_cutover(self) -> None:
        self.assertTrue(INTERPRETER_ROOT.is_dir(), msg=f"missing interpreter root: {INTERPRETER_ROOT}")
        self.assertTrue(INTERPRETER_PROJECT_PATH.is_file(), msg=f"missing interpreter project: {INTERPRETER_PROJECT_PATH}")
        self.assertEqual(
            ["Chaos.IL2CPP.Contracts", "Chaos.IL2CPP.HotUpdate"],
            parse_project_references(INTERPRETER_PROJECT_PATH),
        )
        self.assertNotIn("Chaos.IL2CPP.Generator", parse_project_references(INTERPRETER_PROJECT_PATH))

    def test_interpreter_ir_surface_defines_methods_blocks_instructions_and_opcode_catalog(self) -> None:
        ir_source = INTERPRETER_IR_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "record InterpreterIR",
            "record IRMethod",
            "record IRBasicBlock",
            "record IRInstruction",
            "record IROperand",
            "record IRExceptionRegion",
            "enum IRExceptionRegionKind",
            "enum IROpCode",
            "LdcI4",
            "LdStr",
            "LdNull",
            "LdLoc",
            "StLoc",
            "LdArg",
            "StArg",
            "LdFld",
            "StFld",
            "LdSFld",
            "StSFld",
            "Call",
            "CallVirt",
            "CallBridge",
            "Br",
            "BrTrue",
            "BrFalse",
            "Beq",
            "Blt",
            "Bgt",
            "Ble",
            "Bge",
            "Add",
            "Sub",
            "Mul",
            "Div",
            "Rem",
            "Neg",
            "Ceq",
            "Clt",
            "Cgt",
            "NewObj",
            "Box",
            "Unbox",
            "CastClass",
            "IsInst",
            "NewArr",
            "LdElem",
            "StElem",
            "LdLen",
            "Throw",
            "Rethrow",
            "Leave",
            "EndFinally",
            "Ret",
        ]:
            self.assertIn(required_fragment, ir_source)

    def test_interpreter_ir_operands_are_typed_instead_of_stack_machine_only(self) -> None:
        ir_source = INTERPRETER_IR_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "enum IROperandKind",
            "enum IRTypeTag",
            "TypeTag",
            "Int32",
            "String",
            "Null",
            "Local",
            "Argument",
            "Field",
            "Method",
            "BasicBlock",
        ]:
            self.assertIn(required_fragment, ir_source)

    def test_il_to_ir_lowering_surface_maps_managed_method_bodies_into_interpreter_ir(self) -> None:
        lowering_source = IL_TO_IR_LOWERING_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "class ILToIRLowering",
            "Lower",
            "ManagedMethodModel",
            "ManagedInstructionModel",
            "InterpreterIR",
            "IRMethod",
            "IRBasicBlock",
            "IRInstruction",
            "IROperand",
            "ExceptionRegions",
            "ldarg",
            "ldloc",
            "stloc",
            "ldc.i4",
            "add",
            "leave",
            "endfinally",
            "rethrow",
            "ret",
            "IROpCode.Add",
            "IROpCode.Leave",
            "IROpCode.EndFinally",
            "IROpCode.Rethrow",
            "IRExceptionRegionKind.Catch",
            "IRExceptionRegionKind.Finally",
            "IRTypeTag.Int32",
        ]:
            self.assertIn(required_fragment, lowering_source)

    def test_loader_stage_exposes_arithmetic_and_comparison_instructions_for_lowering_input(self) -> None:
        loader_source = read_loader_stage_source(REPO_ROOT)

        for required_fragment in [
            "ILOpCode.Add",
            'Op = "add"',
            "ILOpCode.Sub",
            'Op = "sub"',
            "ILOpCode.Mul",
            'Op = "mul"',
            "ILOpCode.Div",
            'Op = "div"',
            "ILOpCode.Rem",
            'Op = "rem"',
            "ILOpCode.Ceq",
            'Op = "ceq"',
            "ILOpCode.Clt",
            'Op = "clt"',
            "ILOpCode.Cgt",
            'Op = "cgt"',
        ]:
            self.assertIn(required_fragment, loader_source)

    def test_loader_and_lowering_support_relational_branch_opcodes(self) -> None:
        loader_source = read_loader_stage_source(REPO_ROOT)
        lowering_source = IL_TO_IR_LOWERING_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "ILOpCode.Blt",
            "ILOpCode.Blt_s",
            "ILOpCode.Bgt",
            "ILOpCode.Bgt_s",
            "ILOpCode.Ble",
            "ILOpCode.Ble_s",
            "ILOpCode.Bge",
            "ILOpCode.Bge_s",
        ]:
            self.assertIn(required_fragment, loader_source)

        for required_fragment in [
            '"blt" =>',
            "IROpCode.Blt",
            '"bgt" =>',
            "IROpCode.Bgt",
            '"ble" =>',
            "IROpCode.Ble",
            '"bge" =>',
            "IROpCode.Bge",
        ]:
            self.assertIn(required_fragment, lowering_source)

    def test_real_lowering_proof_executes_add_method_into_ir(self) -> None:
        self.assertTrue(INTERPRETER_LOWERING_MANIFEST_PATH.is_file(), msg=f"missing manifest: {INTERPRETER_LOWERING_MANIFEST_PATH}")
        self.assertTrue(INTERPRETER_LOWERING_PROJECT_PATH.is_file(), msg=f"missing project: {INTERPRETER_LOWERING_PROJECT_PATH}")
        self.assertTrue(INTERPRETER_LOWERING_PROGRAM_PATH.is_file(), msg=f"missing program: {INTERPRETER_LOWERING_PROGRAM_PATH}")
        self.assertTrue(MIXED_BRIDGE_SOLUTION_PATH.is_file(), msg=f"missing mixed bridge solution: {MIXED_BRIDGE_SOLUTION_PATH}")
        lowering_source = INTERPRETER_LOWERING_PROGRAM_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            'Assert.Equal("InterpreterArithmeticProof/NativeExports::Add:System.Int32(System.Int32,System.Int32)", method.SubjectId);',
            'Assert.Equal("ldarg,ldarg,add,ret", opSequence);',
            'Assert.Equal("System.Private.CoreLib/System.String::get_Length:System.Int32()", callVirtInstruction.Operands[0].Symbol);',
        ]:
            self.assertIn(required_fragment, lowering_source)
        self.assertNotIn("Console.WriteLine", lowering_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", lowering_source)
        proof_build_root = make_unique_build_root("lowering-proof")
        proof_base_output = f"-p:BaseOutputPath={proof_build_root}\\"
        run_checked(
            ["dotnet", "build", str(INTERPRETER_LOWERING_PROJECT_PATH), "-c", "Release", proof_base_output],
            cwd=REPO_ROOT,
        )

        completed = run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(INTERPRETER_LOWERING_PROJECT_PATH),
                "-c",
                "Release",
                proof_base_output,
                "--",
                "--chaos-entry-kind=1",
                "--chaos-entry-slice=3",
            ],
            cwd=REPO_ROOT,
        )
        self.assertEqual("", completed.stdout.strip())
