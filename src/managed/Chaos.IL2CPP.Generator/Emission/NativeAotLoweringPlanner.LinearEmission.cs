// Auto-generated partial: Linear emission methods
using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private void EmitLinearNewArray(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
        if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException($"native-aot structured EH linear newarr requires type target reference, got '{requiredTargetReference.Kind}'.");
        }

        string subjectId = HasArrayElementReference(requiredTargetReference)
            ? requiredTargetReference.ArrayElementSubjectId!
            : requiredTargetReference.SubjectId;
        AotCoreIrTypeShapeKind typeShape = HasArrayElementReference(requiredTargetReference)
            ? requiredTargetReference.ArrayElementTypeShape
            : requiredTargetReference.TypeShape;

        string lengthExpr = ConsumeEvalStackValueExpression();
        EmitEvalStackPush(builder, indentation,
            $"ChaosArrayNew1D_Inline(&chaos_type_info_managed_array.hot, {GetRuntimeTypeInfoExpression(subjectId)}, {GetNativeTypeShapeValue(typeShape)}, {lengthExpr})");

    }

    private void EmitLinearArrayElementAddress(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
        if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException($"native-aot structured EH linear ldelema requires type target reference, got '{requiredTargetReference.Kind}'.");
        }

        string rawIndexExpr = ConsumeEvalStackValueExpression();
        string rawArrayExpr = ConsumeEvalStackValueExpression();
        bool skipChecks = TrySkipArrayChecks(rawArrayExpr, rawIndexExpr);

        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>({rawIndexExpr});");
        builder.AppendLine($"{indentation}    auto* chaos_array = reinterpret_cast<chaos_managed_array*>({rawArrayExpr});");
        if (!skipChecks)
        {
            builder.AppendLine($"{indentation}    if (chaos_array == nullptr)");
            builder.AppendLine($"{indentation}    {{");
            builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
            builder.AppendLine($"{indentation}    }}");
            builder.AppendLine($"{indentation}    if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)");
            builder.AppendLine($"{indentation}    {{");
            builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
            builder.AppendLine($"{indentation}    }}");
        }
        EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array_get_elements(chaos_array) + static_cast<CHAOS_IL2CPP_SIZE>(chaos_index))");
        builder.AppendLine($"{indentation}}}");
    }

    private void EmitLinearLoadIndirect(StringBuilder builder, string nativeType, string pushedValueExpression, string indentation)
    {
        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    const auto chaos_address = {ConsumeEvalStackValueExpression()};");
        builder.AppendLine($"{indentation}    const auto chaos_value = chaos_load_indirect<{nativeType}>(chaos_address);");
        EmitEvalStackPush(builder, indentation + "    ", pushedValueExpression);
        builder.AppendLine($"{indentation}}}");
    }

    private void EmitLinearStoreIndirect(StringBuilder builder, string nativeType, string valueExpression, string indentation, bool materializeString, bool needsSatbBarrier = false)
    {
        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    auto chaos_value_raw = {ConsumeEvalStackValueExpression()};");
        if (materializeString && _stringIdMapping is { Count: > 0 })
        {
            builder.AppendLine($"{indentation}    if (chaos_runtime_get_abi_v0()->is_string_id(chaos_value_raw))");
            builder.AppendLine($"{indentation}    {{");
            builder.AppendLine($"{indentation}        chaos_value_raw = chaos_runtime_get_abi_v0()->string_materialize(chaos_value_raw);");
            builder.AppendLine($"{indentation}    }}");
        }
        builder.AppendLine($"{indentation}    const auto chaos_value = {valueExpression};");
        builder.AppendLine($"{indentation}    const auto chaos_address = {ConsumeEvalStackValueExpression()};");
        if (needsSatbBarrier)
        {
            builder.AppendLine($"{indentation}    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(chaos_address));");
        }
        builder.AppendLine($"{indentation}    chaos_store_indirect<{nativeType}>(chaos_address, chaos_value);");
        if (needsSatbBarrier)
        {
            builder.AppendLine($"{indentation}    chaos_gc_dirty_card(reinterpret_cast<void*>(chaos_address));");
        }
        builder.AppendLine($"{indentation}}}");
    }

    private void EmitLinearFieldAddress(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
        if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Field)
        {
            throw new NotSupportedException($"native-aot structured EH linear field address requires field target reference, got '{requiredTargetReference.Kind}'.");
        }

        if (instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.LoadStaticField)
        {
            EmitStaticInitializationForField(builder, requiredTargetReference.SubjectId, indentation);
            if (_staticFieldDeclarations == null)
                _staticFieldDeclarations = new Dictionary<string, string?>(System.StringComparer.Ordinal);
            _staticFieldDeclarations.TryAdd(requiredTargetReference.SubjectId, requiredTargetReference.FieldTypeSubjectId);
            EmitEvalStackPush(builder, indentation, $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&{GetNativeStaticFieldSymbol(requiredTargetReference.SubjectId)})");
            return;
        }

        builder.AppendLine($"{indentation}{{");
        string declaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
        if (requiredTargetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
        {
            builder.AppendLine($"{indentation}    auto* chaos_value = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(declaringTypeSubjectId)}>({ConsumeEvalStackValueExpression()});");
            EmitEvalStackPush(builder, indentation + "    ", $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_value->{GetNativeFieldMemberName(requiredTargetReference.SubjectId)})");
        }
        else
        {
            builder.AppendLine($"{indentation}    auto* chaos_object = reinterpret_cast<{GetNativeTypeSymbol(declaringTypeSubjectId)}*>({ConsumeEvalStackValueExpression()});");
            EmitEvalStackPush(builder, indentation + "    ", $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_object->{GetNativeFieldMemberName(requiredTargetReference.SubjectId)})");
        }
        builder.AppendLine($"{indentation}}}");
    }


    /// <summary>
    /// Resolve rawArrayExpr to a hoisted array base pointer variable name, or null if not hoisted.
    /// Checks both direct chaos_locals[N] expressions and _sN slot variable references.
    /// </summary>
    private string? TryResolveHoistedArrayBase(string rawArrayExpr)
    {
        if (__st.HoistedArrayBaseSlots == null)
            return null;

        int? slot = null;

        // Direct match: "chaos_locals[N]"
        if (rawArrayExpr.StartsWith("chaos_locals[", StringComparison.Ordinal))
        {
            int endBracket = rawArrayExpr.IndexOf(']', 13);
            if (endBracket > 13 && int.TryParse(rawArrayExpr.AsSpan(13, endBracket - 13), out int directSlot))
                slot = directSlot;
        }

        // Slot variable match: _sN via __st.SlotVarToLocalSlot tracking
        if (slot == null && __st.SlotVarToLocalSlot != null && __st.SlotVarToLocalSlot.TryGetValue(rawArrayExpr, out int trackedSlot))
            slot = trackedSlot;

        if (slot.HasValue && __st.HoistedArrayBaseSlots.TryGetValue(slot.Value, out var basePtr))
            return basePtr;

        return null;
    }

    private void EmitLinearArrayLoad(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string pushedValueExpression, string indentation, string? elementType = null)
    {
        string rawIndexExpr = ConsumeEvalStackValueExpression();
        string rawArrayExpr = ConsumeEvalStackValueExpression();
        bool skipChecks = (__st.LoopArrayAccessSkipOffsets?.Contains(instruction.IlOffset) == true)
            || TrySkipArrayChecks(rawArrayExpr, rawIndexExpr);

        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>({rawIndexExpr});");
        builder.AppendLine($"{indentation}    auto* chaos_array = reinterpret_cast<chaos_managed_array*>({rawArrayExpr});");
        if (!skipChecks)
        {
            builder.AppendLine($"{indentation}    if (chaos_array == nullptr)");
            builder.AppendLine($"{indentation}    {{");
            builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
            builder.AppendLine($"{indentation}    }}");
            builder.AppendLine($"{indentation}    if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)");
            builder.AppendLine($"{indentation}    {{");
            builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
            builder.AppendLine($"{indentation}    }}");
        }
        string? _hoistedBase = TryResolveHoistedArrayBase(rawArrayExpr);
        if (_hoistedBase != null)
        {
            // Hoisted array base pointer: direct index access, no reinterpret_cast/chaos_array_get_elements overhead
            if (elementType != null)
                builder.AppendLine($"{indentation}    const auto chaos_element = *reinterpret_cast<{elementType}*>({_hoistedBase} + static_cast<CHAOS_IL2CPP_SIZE>(chaos_index));");
            else
                builder.AppendLine($"{indentation}    const auto chaos_element = {_hoistedBase}[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];");
        }
        else
        {
            if (elementType != null)
            {
                builder.AppendLine($"{indentation}    const auto chaos_element = *reinterpret_cast<{elementType}*>(chaos_array_get_elements(chaos_array) + static_cast<CHAOS_IL2CPP_SIZE>(chaos_index));");
            }
            else
            {
                builder.AppendLine($"{indentation}    const auto chaos_element = chaos_array_get_elements(chaos_array)[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];");
            }
        }
        EmitEvalStackPush(builder, indentation + "    ", pushedValueExpression);
        builder.AppendLine($"{indentation}}}");
    }
    private void EmitLinearArrayStore(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string storedValueExpression, string indentation, bool isReferenceElement, string? elementType = null)
    {
        string rawValueExpr = ConsumeEvalStackValueExpression();
        string rawIndexExpr = ConsumeEvalStackValueExpression();
        string rawArrayExpr = ConsumeEvalStackValueExpression();
        bool skipChecks = (__st.LoopArrayAccessSkipOffsets?.Contains(instruction.IlOffset) == true)
            || TrySkipArrayChecks(rawArrayExpr, rawIndexExpr);

        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    auto chaos_value_raw = {rawValueExpr};");
        if (isReferenceElement && _stringIdMapping is { Count: > 0 })
        {
            builder.AppendLine($"{indentation}    if (chaos_runtime_get_abi_v0()->is_string_id(chaos_value_raw))");
            builder.AppendLine($"{indentation}    {{");
            builder.AppendLine($"{indentation}        chaos_value_raw = chaos_runtime_get_abi_v0()->string_materialize(chaos_value_raw);");
            builder.AppendLine($"{indentation}    }}");
        }
        if (isReferenceElement)
        {
            builder.AppendLine($"{indentation}    auto chaos_value = chaos_value_raw;");
        }
        else
        {
            builder.AppendLine($"{indentation}    const auto chaos_value = {storedValueExpression};");
        }
        builder.AppendLine($"{indentation}    const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>({rawIndexExpr});");
        builder.AppendLine($"{indentation}    auto* chaos_array = reinterpret_cast<chaos_managed_array*>({rawArrayExpr});");
        if (!skipChecks)
        {
            builder.AppendLine($"{indentation}    if (chaos_array == nullptr)");
            builder.AppendLine($"{indentation}    {{");
            builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
            builder.AppendLine($"{indentation}    }}");
            builder.AppendLine($"{indentation}    if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)");
            builder.AppendLine($"{indentation}    {{");
            builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
            builder.AppendLine($"{indentation}    }}");
            if (isReferenceElement)
            {
                builder.AppendLine($"{indentation}    if (!chaos_is_array_store_compatible(chaos_array, chaos_value))");
                builder.AppendLine($"{indentation}    {{");
                builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
                builder.AppendLine($"{indentation}    }}");
            }
        }
        if (isReferenceElement)
        {
            builder.AppendLine($"{indentation}    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(chaos_array_get_elements(chaos_array) + static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)));");
        }
        string? _hoistedBase = TryResolveHoistedArrayBase(rawArrayExpr);
        if (_hoistedBase != null)
        {
            // Hoisted array base pointer: direct index access
            if (elementType != null)
                builder.AppendLine($"{indentation}    *reinterpret_cast<{elementType}*>({_hoistedBase} + static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)) = static_cast<{elementType}>(chaos_value);");
            else
                builder.AppendLine($"{indentation}    {_hoistedBase}[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;");
        }
        else
        {
            if (elementType != null)
            {
                builder.AppendLine($"{indentation}    *reinterpret_cast<{elementType}*>(chaos_array_get_elements(chaos_array) + static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)) = static_cast<{elementType}>(chaos_value);");
            }
            else
            {
                builder.AppendLine($"{indentation}    chaos_array_get_elements(chaos_array)[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;");
            }
        }
        if (isReferenceElement)
        {
            builder.AppendLine($"{indentation}    GC_END_STUBBORN_CHANGE(chaos_array);");
            builder.AppendLine($"{indentation}    chaos_gc_dirty_card(chaos_array);");
        }
        builder.AppendLine($"{indentation}}}");
    }
    private void EmitLinearBox(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
        if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException($"native-aot structured EH linear box requires type target reference, got '{requiredTargetReference.Kind}'.");
        }

        // NOTE: Stack allocation for simple value types was removed:
        // storing a stack address into object[] via stelem.ref causes
        // GC crash (SEH access violation). All boxes go through
        // CHAOS_IL2CPP_NEW_GC. See boxing-to-object-array-crash fix.

        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    const auto chaos_value = {ConsumeEvalStackValueExpression()};");
        builder.AppendLine($"{indentation}    auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC({GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId)}, {{}});");
        builder.AppendLine($"{indentation}    chaos_boxed->header.type_info = {GetNativeBoxTypeInfoSymbol(requiredTargetReference.SubjectId)};");
        if (RequiresStructuredValueTypePayload(requiredTargetReference))
        {
            builder.AppendLine($"{indentation}    auto* chaos_payload = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}>(chaos_value);");
            builder.AppendLine($"{indentation}    chaos_boxed->value = *chaos_payload;");
        }
        else
        {
            builder.AppendLine($"{indentation}    chaos_boxed->value = chaos_value;");
        }
        EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed)");
        builder.AppendLine($"{indentation}}}");
    }

    private void EmitLinearUnbox(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
        if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException($"native-aot structured EH linear unbox requires type target reference, got '{requiredTargetReference.Kind}'.");
        }

        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    auto* chaos_boxed = reinterpret_cast<{GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId)}*>({ConsumeEvalStackValueExpression()});");
        builder.AppendLine($"{indentation}    if (chaos_boxed == nullptr)");
        builder.AppendLine($"{indentation}    {{");
        builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
        builder.AppendLine($"{indentation}    }}");
        EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_boxed->value)");
        builder.AppendLine($"{indentation}}}");
    }

    private void EmitLinearUnboxAny(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
        if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException($"native-aot structured EH linear unbox.any requires type target reference, got '{requiredTargetReference.Kind}'.");
        }

        string? scratchName = null;
        if (RequiresStructuredValueTypePayload(requiredTargetReference))
        {
            scratchName = AllocateLinearScratchName("unbox_any_value");
            builder.AppendLine($"{indentation}{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)} {scratchName}{{}};");
        }

        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    auto* chaos_boxed = reinterpret_cast<{GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId)}*>({ConsumeEvalStackValueExpression()});");
        builder.AppendLine($"{indentation}    if (chaos_boxed == nullptr)");
        builder.AppendLine($"{indentation}    {{");
        builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
        builder.AppendLine($"{indentation}    }}");
        if (RequiresStructuredValueTypePayload(requiredTargetReference))
        {
            builder.AppendLine($"{indentation}    {scratchName} = chaos_boxed->value;");
            EmitEvalStackPush(builder, indentation + "    ", $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&{scratchName})");
        }
        else
        {
            EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_boxed->value)");
        }
        builder.AppendLine($"{indentation}}}");
    }

    private void EmitLinearCastClass(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
        if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException($"native-aot structured EH linear castclass requires type target reference, got '{requiredTargetReference.Kind}'.");
        }

        string chaosValue = AccessEvalStackTopExpression();
        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    const auto chaos_value = {chaosValue};");
        builder.AppendLine($"{indentation}    if (chaos_value != 0)");
        builder.AppendLine($"{indentation}    {{");
        builder.AppendLine($"{indentation}        auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);");
        if (HasArrayElementReference(requiredTargetReference))
        {
            builder.AppendLine($"{indentation}        if (chaos_object_get_type_info(chaos_header) != &chaos_type_info_managed_array.hot)");
            builder.AppendLine($"{indentation}        {{");
            builder.AppendLine($"{indentation}            CHAOS_IL2CPP_FAIL_FAST();");
            builder.AppendLine($"{indentation}        }}");
            builder.AppendLine($"{indentation}        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_value);");
            builder.AppendLine($"{indentation}        if (!chaos_is_array_type_compatible(chaos_array->element_type_shape, chaos_array->element_type_info, {GetNativeTypeShapeValue(requiredTargetReference.ArrayElementTypeShape)}, {GetRuntimeTypeInfoExpression(requiredTargetReference.ArrayElementSubjectId)}))");
        }
        else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.InterfaceType)
        {
            builder.AppendLine($"{indentation}        if (!chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), {GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)}))");
        }
        else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
        {
            builder.AppendLine($"{indentation}        if (!chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), {GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)}))");
        }
        else
        {
            builder.AppendLine($"{indentation}        if (chaos_object_get_type_info(chaos_header) != {GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)} && chaos_object_get_type_info(chaos_header)->stable_id != ({GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)})->stable_id)");
        }
        builder.AppendLine($"{indentation}        {{");
        builder.AppendLine($"{indentation}            CHAOS_IL2CPP_FAIL_FAST();");
        builder.AppendLine($"{indentation}        }}");
        builder.AppendLine($"{indentation}    }}");
        builder.AppendLine($"{indentation}}}");
    }

    private void EmitLinearIsInst(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
    {
        AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
        if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
        {
            throw new NotSupportedException($"native-aot structured EH linear isinst requires type target reference, got '{requiredTargetReference.Kind}'.");
        }

        builder.AppendLine($"{indentation}{{");
        builder.AppendLine($"{indentation}    const auto chaos_value = {ConsumeEvalStackValueExpression()};");
        builder.AppendLine($"{indentation}    auto chaos_matches = false;");
        builder.AppendLine($"{indentation}    if (chaos_value != 0)");
        builder.AppendLine($"{indentation}    {{");
        builder.AppendLine($"{indentation}        auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);");
        if (HasArrayElementReference(requiredTargetReference))
        {
            builder.AppendLine($"{indentation}        if (chaos_object_get_type_info(chaos_header) == &chaos_type_info_managed_array.hot)");
            builder.AppendLine($"{indentation}        {{");
            builder.AppendLine($"{indentation}            auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_value);");
            builder.AppendLine($"{indentation}            chaos_matches = chaos_is_array_type_compatible(chaos_array->element_type_shape, chaos_array->element_type_info, {GetNativeTypeShapeValue(requiredTargetReference.ArrayElementTypeShape)}, {GetRuntimeTypeInfoExpression(requiredTargetReference.ArrayElementSubjectId)});");
            builder.AppendLine($"{indentation}        }}");
        }
        else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.InterfaceType)
        {
            builder.AppendLine($"{indentation}        chaos_matches = chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), {GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)});");
        }
        else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
        {
            builder.AppendLine($"{indentation}        chaos_matches = chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), {GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)});");
        }
        else
        {
            builder.AppendLine($"{indentation}        chaos_matches = chaos_object_get_type_info(chaos_header) == {GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)} || chaos_object_get_type_info(chaos_header)->stable_id == ({GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId)})->stable_id;");
        }
        builder.AppendLine($"{indentation}    }}");
        EmitEvalStackPush(builder, indentation + "    ", "chaos_matches ? chaos_value : 0");
        builder.AppendLine($"{indentation}}}");
    }

    /// <summary>
    /// Emit a pre-evaluated AOT-baked enum call result as a compile-time constant.
    /// Called from <see cref="EmitLinearCallTarget"/> when <see cref="_enumAotBakeMap"/> matches.
    /// </summary>

    /// <summary>
    /// Emit inline try/catch for Assert.Throws<T>(Action) via IL lookahead.
    /// Scans backward from the call instruction to find ldnull + ldftn <target>
    /// + newobj Action::.ctor, then emits a direct try/catch around the target method.
    /// </summary>
}
