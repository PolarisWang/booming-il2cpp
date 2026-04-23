from tests.unit.compatibility.full_assembly_closure_codegen_contracts_test_support import *


class TestFullAssemblyClosureCodegenContractsValidators(FullAssemblyClosureCodegenContractTestSupport):
    @staticmethod
    def _get_method_source(source: str, signature: str, next_signature: str) -> str:
        start = source.index(signature)
        end = source.index(next_signature, start)
        return source[start:end]

    def test_runtime_skeleton_async_state_machine_validator_supports_string_builder(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "private sealed record AsyncManagedTypeShape(",
            "private sealed record AsyncTaskFamilyShape(",
            "TryParseClosedTaskResultType(",
            "TryResolveAsyncManagedTypeShape(",
            "TryResolveAsyncTaskFactoryMethodShape(",
            "TryResolveAsyncGetResultWrapperShape(",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        for forbidden_fragment in [
            "AsyncTaskIntResultShape",
            "AsyncTaskBoolResultShape",
            "AsyncTaskStringResultShape",
            "SupportedAsyncTaskResultShapes",
        ]:
            self.assertNotIn(forbidden_fragment, native_reference_emitter_source)

    def test_runtime_skeleton_async_get_result_bool_validator_supports_two_literal_arguments(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "producerMethod.Parameters",
            "producerFamilyShape.TaskGetAwaiterCallee",
            "producerFamilyShape.AwaiterGetResultCallee",
            "ResolveAsyncLiteralParameterShapes(",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

    def test_runtime_skeleton_async_get_result_int_validator_supports_two_literal_arguments(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "ResolveAsyncLiteralParameterShapes(",
            "expectedLiteralParameterShapes.Count + 6",
            "expectedLiteralParameterShapes[instructionIndex].LiteralOp",
            "instructions[callInstructionIndex + 4]",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

    def test_runtime_skeleton_async_get_result_validator_preserves_awaiter_signature_boundary(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            'public string AwaiterReturnType => $"System.Runtime.CompilerServices.TaskAwaiter<{ManagedResultType}>"',
            'public string AwaiterSubjectType => $"System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter<{ManagedResultType}>"',
            'public string TaskGetAwaiterCallee => $"{TaskType}::GetAwaiter:{AwaiterReturnType}()"',
            'public string AwaiterGetResultCallee => $"{AwaiterSubjectType}::GetResult:{ManagedResultType}()"',
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

    def test_try_build_pinvoke_direct_call_paths_soft_skip_unsupported_import_metadata(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")

        full_closure_method_source = self._get_method_source(
            native_reference_emitter_source,
            "private static bool TryBuildAssemblyBoundPInvokeDirectCallPlan(",
            "private static bool TryBuildAssemblyBoundArrayBoxingReferenceArrayPlan(",
        )
        legacy_method_source = self._get_method_source(
            native_reference_emitter_source,
            "private static bool TryBuildLegacyPInvokeDirectCallLoweringPlan(",
            "private static bool TryBuildLegacyReflectionClosedTypeQueryLoweringPlan(",
        )

        for method_source in [full_closure_method_source, legacy_method_source]:
            self.assertIn('return false;', method_source)
            self.assertNotIn("RequireCapability(", method_source)
            self.assertNotIn("RequireMethodContract(", method_source)
            self.assertNotIn("throw new InvalidOperationException(", method_source)

        for required_fragment in [
            "private static bool HasMethodContract(",
            "private static bool HasCapability(",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

    def test_runtime_skeleton_array_copy_reference_array_validator_tracks_copy_shape(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        planner_shape_validation_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceLoweringPlanner.ShapeValidation.cs"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "IsArrayCopyReferenceArrayEntryPointShape(",
            'PlanKind = "arrayCopyReferenceArray"',
            "IsSupportedArrayCopyMethodSubjectId(",
            'subjectId.Contains("/System.Array::ConstrainedCopy:", StringComparison.Ordinal)',
            "SourceArrayLength",
            "TargetArrayLength",
            "SourceStoreIndex",
            "SourceArrayIndex",
            "TargetArrayIndex",
            "TargetReadIndex",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        for required_fragment in [
            "private static bool IsArrayCopyReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)",
            "private static bool IsArrayCopyReferenceArrayEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)",
            "IsArrayCopyReferenceArrayLengthOnlyEntryPointShape",
            "IsArrayCopyReferenceArrayLengthOnlyInt64EntryPointShape",
            "IsArrayCopyReferenceArrayIndexedEntryPointShape",
            "IsArrayCopyReferenceArrayIndexedInt64EntryPointShape",
            "IsArrayCopyToReferenceArrayTargetOffsetEntryPointShape",
            "IsArrayCopyToReferenceArrayTargetOffsetInt64EntryPointShape",
            "instructions.Count == 20",
            "instructions.Count == 21",
            "instructions.Count == 22",
            "instructions.Count == 25",
            'string.Equals(instructions[12].Op, "conv.i8", StringComparison.Ordinal)',
            'string.Equals(instructions[13].Op, "call", StringComparison.Ordinal)',
            'string.Equals(instructions[11].Op, "conv.i8", StringComparison.Ordinal)',
            'string.Equals(instructions[13].Op, "callvirt", StringComparison.Ordinal)',
            'string.Equals(instructions[14].Op, "conv.i8", StringComparison.Ordinal)',
            'string.Equals(instructions[16].Op, "conv.i8", StringComparison.Ordinal)',
            'string.Equals(instructions[14].Op, "call", StringComparison.Ordinal)',
            'string.Equals(instructions[17].Op, "ldelem.ref", StringComparison.Ordinal)',
            'string.Equals(instructions[12].Op, "callvirt", StringComparison.Ordinal)',
            'subjectId.EndsWith("(System.Array,System.Array,System.Int64)", StringComparison.Ordinal)',
            'subjectId.EndsWith("(System.Array,System.Int64)", StringComparison.Ordinal)',
            'subjectId.EndsWith("(System.Array,System.Int64,System.Array,System.Int64,System.Int64)", StringComparison.Ordinal)',
        ]:
            self.assertIn(required_fragment, planner_shape_validation_source)

    def test_runtime_skeleton_array_reverse_reference_array_validator_tracks_reverse_shape(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        planner_shape_validation_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceLoweringPlanner.ShapeValidation.cs"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "IsArrayReverseReferenceArrayEntryPointShape(",
            'PlanKind = "arrayReverseReferenceArray"',
            "IsSupportedArrayReverseMethodSubjectId(",
            "ArrayLength",
            "ArrayStoreIndex",
            "ReverseStartIndex",
            "ReverseLength",
            "ArrayReadIndex",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        for required_fragment in [
            "private static bool IsArrayReverseReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)",
            "private static bool IsArrayReverseReferenceArrayEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)",
            "IsArrayReverseRangeReferenceArrayEntryPointShape",
            "IsArrayReverseWholeReferenceArrayEntryPointShape",
            "instructions.Count == 15",
            "instructions.Count == 17",
            'string.Equals(instructions[7].Op, "dup", StringComparison.Ordinal)',
            'string.Equals(instructions[8].Op, "call", StringComparison.Ordinal)',
            'string.Equals(instructions[10].Op, "ldelem.ref", StringComparison.Ordinal)',
            'string.Equals(instructions[10].Op, "call", StringComparison.Ordinal)',
            'string.Equals(instructions[12].Op, "ldelem.ref", StringComparison.Ordinal)',
        ]:
            self.assertIn(required_fragment, planner_shape_validation_source)

    def test_runtime_skeleton_array_clear_reference_array_validator_tracks_clear_shape(self) -> None:
        native_reference_emitter_source = NATIVE_REFERENCE_EMITTER_PATH.read_text(encoding="utf-8")
        planner_shape_validation_source = (
            REPO_ROOT
            / "src"
            / "managed"
            / "Chaos.IL2CPP.CodeGen"
            / "ReferenceProof"
            / "NativeReferenceLoweringPlanner.ShapeValidation.cs"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "IsArrayClearReferenceArrayEntryPointShape(",
            'PlanKind = "arrayClearReferenceArray"',
            "IsSupportedArrayClearMethodSubjectId(",
            "ArrayLength",
            "ArrayStoreIndex",
            "ClearStartIndex",
            "ClearLength",
            "ArrayReadIndex",
            "StoredLiteral",
            "StoredLiteralByteCount",
        ]:
            self.assertIn(required_fragment, native_reference_emitter_source)

        for required_fragment in [
            "private static bool IsArrayClearReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)",
            "private static bool IsArrayClearReferenceArrayEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)",
            "IsArrayClearWholeReferenceArrayEntryPointShape",
            "instructions.Count == 19",
            "instructions.Count == 17",
            'string.Equals(instructions[10].Op, "call", StringComparison.Ordinal)',
            'string.Equals(instructions[14].Op, "ldelem.ref", StringComparison.Ordinal)',
            'string.Equals(instructions[15].Op, "call", StringComparison.Ordinal)',
        ]:
            self.assertIn(required_fragment, planner_shape_validation_source)
