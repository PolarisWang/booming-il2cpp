from tests.unit.compatibility.interpreter_mixed_execution_subjects_test_support import *


class TestInterpreterMixedExecutionSubjectsRuntime(Phase7InterpreterMixedExecutionTestSupport):
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
        self.assertTrue(INTERPRETER_ARITHMETIC_PROOF_PATH.is_file(), msg=f"missing program: {INTERPRETER_ARITHMETIC_PROOF_PATH}")
        self.assertTrue(MIXED_GENERIC_FLOW_PROOF_PATH.is_file(), msg=f"missing program: {MIXED_GENERIC_FLOW_PROOF_PATH}")
        self.assertTrue(MIXED_EXCEPTION_FLOW_PROOF_PATH.is_file(), msg=f"missing program: {MIXED_EXCEPTION_FLOW_PROOF_PATH}")
        self.assertTrue(MIXED_DELEGATE_FLOW_PROOF_PATH.is_file(), msg=f"missing program: {MIXED_DELEGATE_FLOW_PROOF_PATH}")
        proof_source = MIXED_EXECUTION_PROOF_PROGRAM_PATH.read_text(encoding="utf-8")
        interpreter_arithmetic_source = INTERPRETER_ARITHMETIC_PROOF_PATH.read_text(encoding="utf-8")
        mixed_generic_source = MIXED_GENERIC_FLOW_PROOF_PATH.read_text(encoding="utf-8")
        mixed_exception_source = MIXED_EXCEPTION_FLOW_PROOF_PATH.read_text(encoding="utf-8")
        mixed_delegate_source = MIXED_DELEGATE_FLOW_PROOF_PATH.read_text(encoding="utf-8")
        host_program_source = MIXED_EXECUTION_HOST_PROGRAM_PATH.read_text(encoding="utf-8")
        for required_fragment in [
            "Assert.Equal(22, beforeLoad);",
            "Assert.Equal(42, afterLoad);",
            "Assert.Equal(30, interpreterToAot);",
            'Assert.Equal("ldarg,ldarg,callbridge,ret", interpreterToAotOps);',
            'Assert.Equal("ok", rethrowCaught);',
            "Assert.Equal(22, afterUnload);",
        ]:
            self.assertIn(required_fragment, proof_source)
        self.assertIn("InterpreterArithmeticProofEntry.Run", host_program_source)
        self.assertIn("MixedGenericFlowProofEntry.Run", host_program_source)
        self.assertIn("MixedExceptionFlowProofEntry.Run", host_program_source)
        self.assertIn("MixedDelegateFlowProofEntry.Run", host_program_source)
        self.assertIn('Alias = "interpreter-arithmetic-proof"', interpreter_arithmetic_source)
        self.assertIn("ChaosCapabilityItem.InterpreterArithmetic", interpreter_arithmetic_source)
        self.assertIn('Alias = "mixed-generic-flow-proof"', mixed_generic_source)
        self.assertIn("ChaosCapabilityItem.MixedGenericFlow", mixed_generic_source)
        self.assertIn('Alias = "mixed-exception-flow-proof"', mixed_exception_source)
        self.assertIn("ChaosCapabilityItem.MixedExceptionFlow", mixed_exception_source)
        self.assertIn('Alias = "mixed-delegate-flow-proof"', mixed_delegate_source)
        self.assertIn("ChaosCapabilityItem.MixedDelegateFlow", mixed_delegate_source)
        self.assertNotIn("Console.WriteLine", proof_source)
        self.assertNotIn("Console.WriteLine", interpreter_arithmetic_source)
        self.assertNotIn("Console.WriteLine", mixed_generic_source)
        self.assertNotIn("Console.WriteLine", mixed_exception_source)
        self.assertNotIn("Console.WriteLine", mixed_delegate_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", proof_source)

        proof_build_root = make_unique_build_root("mixed-proof")
        proof_base_output = f"-p:BaseOutputPath={proof_build_root}\\"
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
            ],
            cwd=REPO_ROOT,
        )
        self.assertEqual("", completed.stdout.strip())

    def test_mixed_execution_host_program_uses_compact_subject_entry_selection_only(self) -> None:
        host_program_source = MIXED_EXECUTION_HOST_PROGRAM_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "ChaosSubjectEntryArguments.TryParse",
            "InterpreterArithmeticProofEntry.Run",
            "InterpreterLoweringProofEntry.Run",
            "MixedGenericFlowProofEntry.Run",
            "MixedExceptionFlowProofEntry.Run",
            "MixedDelegateFlowProofEntry.Run",
        ]:
            self.assertIn(required_fragment, host_program_source)
        self.assertNotIn("ChaosSourceEntryArguments.TryParse", host_program_source)
        self.assertNotIn("DeclaredProofEntriesBySourceEntry", host_program_source)

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
