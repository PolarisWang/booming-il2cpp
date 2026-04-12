from __future__ import annotations

import subprocess
import unittest
import uuid
import xml.etree.ElementTree as ET
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CORE_SOLUTION_PATH = REPO_ROOT / "solutions" / "core" / "windows" / "chaos-il2cpp-core.sln"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase7-interpreter-mixed-execution"

INTERPRETER_ROOT = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Interpreter"
INTERPRETER_PROJECT_PATH = INTERPRETER_ROOT / "Chaos.IL2CPP.Interpreter.csproj"
INTERPRETER_IR_PATH = INTERPRETER_ROOT / "InterpreterIR.cs"
IL_TO_IR_LOWERING_PATH = INTERPRETER_ROOT / "ILToIRLowering.cs"
MANAGED_INTERPRETER_EXECUTOR_PATH = INTERPRETER_ROOT / "ManagedInterpreterExecutor.cs"
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
ROOT_CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"
HOT_UPDATE_RUNTIME_MANAGER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate" / "RuntimeManager.cs"
HOT_UPDATE_METHOD_REGISTRY_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.HotUpdate" / "HotUpdateMethodRegistry.cs"
INTERPRETER_ARITHMETIC_PROJECT_PATH = REPO_ROOT / "subjects" / "InterpreterArithmeticProof" / "source" / "InterpreterArithmeticProof.csproj"
INTERPRETER_LOWERING_ROOT = REPO_ROOT / "subjects" / "InterpreterLoweringProof"
INTERPRETER_LOWERING_MANIFEST_PATH = INTERPRETER_LOWERING_ROOT / "subject.manifest.json"
INTERPRETER_LOWERING_PROJECT_PATH = INTERPRETER_LOWERING_ROOT / "source" / "InterpreterLoweringProof.csproj"
INTERPRETER_LOWERING_PROGRAM_PATH = INTERPRETER_LOWERING_ROOT / "source" / "Program.cs"
MIXED_EXECUTION_PROOF_ROOT = REPO_ROOT / "subjects" / "MixedExecutionProof"
MIXED_EXECUTION_PROOF_MANIFEST_PATH = MIXED_EXECUTION_PROOF_ROOT / "subject.manifest.json"
MIXED_EXECUTION_PROOF_PROJECT_PATH = MIXED_EXECUTION_PROOF_ROOT / "source" / "MixedExecutionProof.csproj"
MIXED_EXECUTION_PROOF_PROGRAM_PATH = MIXED_EXECUTION_PROOF_ROOT / "source" / "Program.cs"
NATIVE_INTERPRETER_ROOT = REPO_ROOT / "src" / "native" / "interpreter"
NATIVE_INTERPRETER_CMAKE_PATH = NATIVE_INTERPRETER_ROOT / "CMakeLists.txt"
NATIVE_INTERPRETER_HEADER_PATH = NATIVE_INTERPRETER_ROOT / "interpreter_vm.h"
NATIVE_INTERPRETER_SOURCE_PATH = NATIVE_INTERPRETER_ROOT / "interpreter_vm.cpp"
NATIVE_INTERPRETER_SMOKE_ROOT = REPO_ROOT / "tests" / "contracts" / "native" / "interpreter"
NATIVE_INTERPRETER_SMOKE_CMAKE_PATH = NATIVE_INTERPRETER_SMOKE_ROOT / "CMakeLists.txt"
NATIVE_INTERPRETER_SMOKE_SOURCE_PATH = NATIVE_INTERPRETER_SMOKE_ROOT / "interpreter_smoke.cpp"


def parse_project_references(project_path: Path) -> list[str]:
    root = ET.fromstring(project_path.read_text(encoding="utf-8"))
    references: list[str] = []
    for element in root.findall(".//ProjectReference"):
        include_value = element.attrib.get("Include", "")
        references.append(Path(include_value).stem)
    return sorted(references)


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


def make_unique_build_root(prefix: str) -> Path:
    build_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
    build_root.mkdir(parents=True, exist_ok=True)
    return build_root


class Phase7InterpreterMixedExecutionTests(unittest.TestCase):
    def test_interpreter_project_isolation_and_solution_wiring(self) -> None:
        solution_source = CORE_SOLUTION_PATH.read_text(encoding="utf-8")

        self.assertTrue(INTERPRETER_ROOT.is_dir(), msg=f"missing interpreter root: {INTERPRETER_ROOT}")
        self.assertTrue(INTERPRETER_PROJECT_PATH.is_file(), msg=f"missing interpreter project: {INTERPRETER_PROJECT_PATH}")
        self.assertEqual(
            ["Chaos.IL2CPP.Contracts", "Chaos.IL2CPP.HotUpdate"],
            parse_project_references(INTERPRETER_PROJECT_PATH),
        )
        self.assertIn("Chaos.IL2CPP.Interpreter", solution_source)
        self.assertNotIn("Chaos.IL2CPP.CodeGen", parse_project_references(INTERPRETER_PROJECT_PATH))

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
        loader_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")

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
        loader_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")
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

    def test_real_lowering_proof_subject_executes_add_method_into_ir(self) -> None:
        self.assertTrue(INTERPRETER_LOWERING_MANIFEST_PATH.is_file(), msg=f"missing manifest: {INTERPRETER_LOWERING_MANIFEST_PATH}")
        self.assertTrue(INTERPRETER_LOWERING_PROJECT_PATH.is_file(), msg=f"missing project: {INTERPRETER_LOWERING_PROJECT_PATH}")
        self.assertTrue(INTERPRETER_LOWERING_PROGRAM_PATH.is_file(), msg=f"missing program: {INTERPRETER_LOWERING_PROGRAM_PATH}")
        arithmetic_build_root = make_unique_build_root("arithmetic")
        proof_build_root = make_unique_build_root("lowering-proof")
        arithmetic_base_output = f"-p:BaseOutputPath={arithmetic_build_root}\\"
        proof_base_output = f"-p:BaseOutputPath={proof_build_root}\\"
        arithmetic_assembly_path = arithmetic_build_root / "Release" / "net8.0" / "InterpreterArithmeticProof.dll"

        run_checked(
            ["dotnet", "build", str(INTERPRETER_ARITHMETIC_PROJECT_PATH), "-c", "Release", arithmetic_base_output],
            cwd=REPO_ROOT,
        )
        run_checked(
            ["dotnet", "build", str(INTERPRETER_LOWERING_PROJECT_PATH), "-c", "Release", proof_base_output],
            cwd=REPO_ROOT,
        )
        self.assertTrue(
            arithmetic_assembly_path.is_file(),
            msg=f"missing lowering input assembly: {arithmetic_assembly_path}",
        )

        completed = run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(INTERPRETER_LOWERING_PROJECT_PATH),
                "-c",
                "Release",
                "--no-build",
                proof_base_output,
                "--",
                str(arithmetic_assembly_path),
            ],
            cwd=REPO_ROOT,
        )
        output = completed.stdout

        for required_fragment in [
            "lowering-proof=ok",
            "method-subject=InterpreterArithmeticProof/NativeExports::Add(System.Int32,System.Int32)",
            "blocks=1",
            "ops=ldarg,ldarg,add,ret",
            "add-result=Int32",
            "call-method-subject=InterpreterArithmeticProof/NativeExports::CallAotBridgeAdd(System.Int32,System.Int32)",
            "call-ops=ldarg,ldarg,callbridge,ret",
            "call-target=InterpreterArithmeticProof.AotBridge/AotBridgeExports::Add(System.Int32,System.Int32)",
            "local-call-method-subject=InterpreterArithmeticProof/NativeExports::CallLocalAdd(System.Int32,System.Int32)",
            "local-call-ops=ldarg,ldarg,call,ret",
            "local-call-target=InterpreterArithmeticProof/NativeExports::Add(System.Int32,System.Int32)",
            "callvirt-method-subject=InterpreterArithmeticProof/NativeExports::CallStringLength()",
            "callvirt-ops=ldstr,callbridge,ret",
            "callvirt-target=System.Private.CoreLib/System.String::get_Length()",
            "instance-call-method-subject=InterpreterArithmeticProof/NativeExports::CallInstanceAddOne(InstanceArithmetic,System.Int32)",
            "instance-call-ops=ldarg,ldarg,callvirt,ret",
            "instance-call-target=InterpreterArithmeticProof/InstanceArithmetic::AddOne(System.Int32)",
        ]:
            self.assertIn(required_fragment, output)

    def test_managed_interpreter_executor_and_runtime_manager_support_aot_to_interpreter_and_call_bridge(self) -> None:
        executor_source = MANAGED_INTERPRETER_EXECUTOR_PATH.read_text(encoding="utf-8")
        runtime_manager_source = HOT_UPDATE_RUNTIME_MANAGER_PATH.read_text(encoding="utf-8")
        registry_source = HOT_UPDATE_METHOD_REGISTRY_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "class ManagedInterpreterExecutor",
            "ExecuteInt32",
            "CreateInt32UnaryInvoker",
            "IROpCode.LdcI4",
            "IROpCode.LdStr",
            "IROpCode.LdArg",
            "IROpCode.LdLoc",
            "IROpCode.StLoc",
            "IROpCode.Add",
            "IROpCode.Ceq",
            "IROpCode.BrTrue",
            "IROpCode.Blt",
            "IROpCode.Bgt",
            "IROpCode.Ble",
            "IROpCode.Bge",
            "IROpCode.CallBridge",
            "IROpCode.CallVirt",
            "IROpCode.Leave",
            "IROpCode.EndFinally",
            "IROpCode.Rethrow",
            "bridgeInvoker",
        ]:
            self.assertIn(required_fragment, executor_source)

        for required_fragment in [
            "RegisterInt32Unary",
            "DispatchInt32Unary",
            "Func<int, int>",
        ]:
            self.assertIn(required_fragment, runtime_manager_source)
            self.assertIn(required_fragment, registry_source)

    def test_mixed_execution_proof_runs_aot_to_interpreter_and_interpreter_to_aot_paths(self) -> None:
        self.assertTrue(MIXED_EXECUTION_PROOF_MANIFEST_PATH.is_file(), msg=f"missing manifest: {MIXED_EXECUTION_PROOF_MANIFEST_PATH}")
        self.assertTrue(MIXED_EXECUTION_PROOF_PROJECT_PATH.is_file(), msg=f"missing project: {MIXED_EXECUTION_PROOF_PROJECT_PATH}")
        self.assertTrue(MIXED_EXECUTION_PROOF_PROGRAM_PATH.is_file(), msg=f"missing program: {MIXED_EXECUTION_PROOF_PROGRAM_PATH}")

        arithmetic_build_root = make_unique_build_root("mixed-proof-arithmetic")
        proof_build_root = make_unique_build_root("mixed-proof")
        arithmetic_base_output = f"-p:BaseOutputPath={arithmetic_build_root}\\"
        proof_base_output = f"-p:BaseOutputPath={proof_build_root}\\"
        arithmetic_assembly_path = arithmetic_build_root / "Release" / "net8.0" / "InterpreterArithmeticProof.dll"

        run_checked(
            ["dotnet", "build", str(INTERPRETER_ARITHMETIC_PROJECT_PATH), "-c", "Release", arithmetic_base_output],
            cwd=REPO_ROOT,
        )
        run_checked(
            ["dotnet", "build", str(MIXED_EXECUTION_PROOF_PROJECT_PATH), "-c", "Release", proof_base_output],
            cwd=REPO_ROOT,
        )

        completed = run_checked(
            [
                "dotnet",
                "run",
                "--project",
                str(MIXED_EXECUTION_PROOF_PROJECT_PATH),
                "-c",
                "Release",
                "--no-build",
                proof_base_output,
                "--",
                str(arithmetic_assembly_path),
            ],
            cwd=REPO_ROOT,
        )
        output = completed.stdout

        for required_fragment in [
            "mixed-aot-to-interpreter-before-load=22",
            "mixed-aot-to-interpreter=42",
            "mixed-interpreter-to-aot=30",
            "mixed-interpreter-to-aot-ops=ldarg,ldarg,callbridge,ret",
            "mixed-interpreter-to-aot-target=InterpreterArithmeticProof.AotBridge/AotBridgeExports::Add(System.Int32,System.Int32)",
            "mixed-interpreter-local-call=42",
            "mixed-interpreter-local-call-ops=ldarg,ldarg,call,ret",
            "mixed-interpreter-instance-call=42",
            "mixed-interpreter-instance-call-ops=ldarg,ldarg,callvirt,ret",
            "mixed-interpreter-instance-call-target=InterpreterArithmeticProof/InstanceArithmetic::AddOne(System.Int32)",
            "mixed-interpreter-string-bridge=5",
            "mixed-interpreter-string-bridge-ops=ldstr,callbridge,ret",
            "mixed-interpreter-real-catch=42",
            "mixed-interpreter-real-catch-region-kind=catch",
            "mixed-interpreter-real-rethrow-caught=ok",
            "mixed-interpreter-real-rethrow-region-kind=catch",
            "mixed-interpreter-real-leave-finally=42",
            "mixed-interpreter-real-leave-finally-region-kind=finally",
            "mixed-interpreter-real-leave-finally-opcodes=leave,endfinally",
            "mixed-interpreter-to-engine=7",
            "mixed-interpreter-throw-caught=ok",
            "mixed-interpreter-leave-finally=ok",
            "mixed-interpreter-rethrow-caught=ok",
            "mixed-aot-to-interpreter-after-unload=22",
        ]:
            self.assertIn(required_fragment, output)

    def test_native_interpreter_vm_project_scaffold_and_dispatch_loop_exist(self) -> None:
        root_cmake_source = ROOT_CMAKE_PATH.read_text(encoding="utf-8")
        self.assertTrue(NATIVE_INTERPRETER_CMAKE_PATH.is_file(), msg=f"missing native interpreter cmake: {NATIVE_INTERPRETER_CMAKE_PATH}")
        self.assertTrue(NATIVE_INTERPRETER_HEADER_PATH.is_file(), msg=f"missing native interpreter header: {NATIVE_INTERPRETER_HEADER_PATH}")
        self.assertTrue(NATIVE_INTERPRETER_SOURCE_PATH.is_file(), msg=f"missing native interpreter source: {NATIVE_INTERPRETER_SOURCE_PATH}")
        self.assertIn("add_subdirectory(src/native/interpreter)", root_cmake_source)

        cmake_source = NATIVE_INTERPRETER_CMAKE_PATH.read_text(encoding="utf-8")
        header_source = NATIVE_INTERPRETER_HEADER_PATH.read_text(encoding="utf-8")
        source_source = NATIVE_INTERPRETER_SOURCE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "add_library(chaos_interpreter STATIC",
            "interpreter_vm.cpp",
            "interpreter_vm.h",
            "target_link_libraries(",
            "chaos_runtime_core",
        ]:
            self.assertIn(required_fragment, cmake_source)

        for required_fragment in [
            "namespace chaos::il2cpp::interpreter",
            "enum class IROpCode",
            "LdcI4",
            "LdArg",
            "LdLoc",
            "StLoc",
            "Add",
            "Sub",
            "Mul",
            "Div",
            "Rem",
            "Ceq",
            "Clt",
            "Cgt",
            "Br",
            "BrTrue",
            "BrFalse",
            "Ret",
            "struct IRInstruction",
            "struct IRMethod",
            "struct ExecutionFrame",
            "struct ExecutionResult",
            "class InterpreterVM",
            "Execute",
        ]:
            self.assertIn(required_fragment, header_source)

        for required_fragment in [
            "switch",
            "case IROpCode::LdcI4",
            "case IROpCode::LdArg",
            "case IROpCode::LdLoc",
            "case IROpCode::StLoc",
            "case IROpCode::Add",
            "case IROpCode::Sub",
            "case IROpCode::Mul",
            "case IROpCode::Div",
            "case IROpCode::Rem",
            "case IROpCode::Ceq",
            "case IROpCode::Clt",
            "case IROpCode::Cgt",
            "case IROpCode::Br",
            "case IROpCode::BrTrue",
            "case IROpCode::BrFalse",
            "case IROpCode::Ret",
            "stack",
            "locals",
        ]:
            self.assertIn(required_fragment, source_source)

    def test_native_interpreter_vm_smoke_executes_add_and_local_roundtrip(self) -> None:
        self.assertTrue(NATIVE_INTERPRETER_SMOKE_CMAKE_PATH.is_file(), msg=f"missing smoke cmake: {NATIVE_INTERPRETER_SMOKE_CMAKE_PATH}")
        self.assertTrue(NATIVE_INTERPRETER_SMOKE_SOURCE_PATH.is_file(), msg=f"missing smoke source: {NATIVE_INTERPRETER_SMOKE_SOURCE_PATH}")

        build_root = make_unique_build_root("native-interpreter-smoke")
        run_checked(
            [
                "cmake",
                "-S",
                ".",
                "-B",
                str(build_root),
                "-G",
                "Visual Studio 17 2022",
                "-DROADMAP0_PRESET_TARGET=windows-x64-reference",
            ],
            cwd=REPO_ROOT,
        )
        run_checked(
            [
                "cmake",
                "--build",
                str(build_root),
                "--config",
                "Release",
                "--target",
                "chaos_interpreter_smoke",
            ],
            cwd=REPO_ROOT,
        )
        executable_path = build_root / "tests" / "contracts" / "native" / "interpreter" / "Release" / "chaos_interpreter_smoke.exe"
        self.assertTrue(executable_path.is_file(), msg=f"missing smoke executable: {executable_path}")

        completed = run_checked([str(executable_path)], cwd=REPO_ROOT)
        output = completed.stdout
        for required_fragment in [
            "native-interpreter-smoke=42",
            "native-interpreter-path=ldarg,stloc,ldloc,ldarg,add,ret",
            "native-interpreter-branch=42",
        ]:
            self.assertIn(required_fragment, output)


if __name__ == "__main__":
    unittest.main()
