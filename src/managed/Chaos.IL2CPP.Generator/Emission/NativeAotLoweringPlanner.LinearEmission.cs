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
		if (_hoistedArrayBaseSlots == null)
			return null;

		int? slot = null;

		// Direct match: "chaos_locals[N]"
		if (rawArrayExpr.StartsWith("chaos_locals[", StringComparison.Ordinal))
		{
			int endBracket = rawArrayExpr.IndexOf(']', 13);
			if (endBracket > 13 && int.TryParse(rawArrayExpr.AsSpan(13, endBracket - 13), out int directSlot))
				slot = directSlot;
		}

		// Slot variable match: _sN via _slotVarToLocalSlot tracking
		if (slot == null && _slotVarToLocalSlot != null && _slotVarToLocalSlot.TryGetValue(rawArrayExpr, out int trackedSlot))
			slot = trackedSlot;

		if (slot.HasValue && _hoistedArrayBaseSlots.TryGetValue(slot.Value, out var basePtr))
			return basePtr;

		return null;
	}

	private void EmitLinearArrayLoad(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string pushedValueExpression, string indentation, string? elementType = null)
	{
		string rawIndexExpr = ConsumeEvalStackValueExpression();
		string rawArrayExpr = ConsumeEvalStackValueExpression();
		bool skipChecks = (_loopArrayAccessSkipOffsets?.Contains(instruction.IlOffset) == true)
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
		bool skipChecks = (_loopArrayAccessSkipOffsets?.Contains(instruction.IlOffset) == true)
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

	private void EmitLinearCopyObject(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear cpobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		builder.AppendLine($"{indentation}{{");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			builder.AppendLine($"{indentation}    auto* chaos_source = reinterpret_cast<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}*>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    auto* chaos_destination = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    if (chaos_source == nullptr)");
			builder.AppendLine($"{indentation}    {{");
			builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
			builder.AppendLine($"{indentation}    }}");
			builder.AppendLine($"{indentation}    *chaos_destination = *chaos_source;");
		}
		else
		{
			builder.AppendLine($"{indentation}    const auto chaos_source = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine($"{indentation}    auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    *chaos_destination = chaos_source;");
		}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearCopyBlock(StringBuilder builder, string indentation)
	{
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_size = static_cast<CHAOS_IL2CPP_SIZE>(static_cast<CHAOS_IL2CPP_UINTPTR>({ConsumeEvalStackValueExpression()}));");
		builder.AppendLine($"{indentation}    const auto chaos_source = {ConsumeEvalStackValueExpression()};");
		builder.AppendLine($"{indentation}    const auto chaos_destination = {ConsumeEvalStackValueExpression()};");
		builder.AppendLine($"{indentation}    if (chaos_size != static_cast<CHAOS_IL2CPP_SIZE>(0))");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        const auto chaos_resolve_cpblk_address = [](CHAOS_IL2CPP_INTPTR chaos_address) -> void*");
		builder.AppendLine($"{indentation}        {{");
		builder.AppendLine($"{indentation}            if ((chaos_address & CHAOS_IL2CPP_RAW_POINTER_TAG) != 0)");
		builder.AppendLine($"{indentation}            {{");
		builder.AppendLine($"{indentation}                return reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~CHAOS_IL2CPP_RAW_POINTER_TAG));");
		builder.AppendLine($"{indentation}            }}");
		builder.AppendLine($"{indentation}            if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)");
		builder.AppendLine($"{indentation}            {{");
		builder.AppendLine($"{indentation}                return reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));");
		builder.AppendLine($"{indentation}            }}");
		builder.AppendLine($"{indentation}            return reinterpret_cast<void*>(chaos_address);");
		builder.AppendLine($"{indentation}        }};");
		builder.AppendLine($"{indentation}        if (chaos_source == 0 || chaos_destination == 0)");
		builder.AppendLine($"{indentation}        {{");
		builder.AppendLine($"{indentation}            CHAOS_IL2CPP_FAIL_FAST();");
		builder.AppendLine($"{indentation}        }}");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_MEMCPY(chaos_resolve_cpblk_address(chaos_destination), chaos_resolve_cpblk_address(chaos_source), chaos_size);");
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearLocalAlloc(StringBuilder builder, string indentation)
	{
		builder.AppendLine($"{indentation}{{");
		builder.AppendLine($"{indentation}    const auto chaos_size = static_cast<CHAOS_IL2CPP_INTPTR>({ConsumeEvalStackValueExpression()});");
		builder.AppendLine($"{indentation}    if (chaos_size < 0)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
		builder.AppendLine($"{indentation}    }}");
		builder.AppendLine($"{indentation}    const auto chaos_byte_count = static_cast<CHAOS_IL2CPP_SIZE>(chaos_size);");
		builder.AppendLine($"{indentation}    void* chaos_block = CHAOS_IL2CPP_MALLOC(chaos_byte_count == static_cast<CHAOS_IL2CPP_SIZE>(0) ? static_cast<CHAOS_IL2CPP_SIZE>(1) : chaos_byte_count);");
		builder.AppendLine($"{indentation}    if (chaos_block == nullptr)");
		builder.AppendLine($"{indentation}    {{");
		builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
		builder.AppendLine($"{indentation}    }}");
		EmitEvalStackPush(builder, indentation + "    ", "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_block)");
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearLoadObjectValue(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear ldobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		string? scratchName = null;
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			scratchName = AllocateLinearScratchName("ldobj_value");
			builder.AppendLine($"{indentation}{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)} {scratchName}{{}};");
		}

		builder.AppendLine($"{indentation}{{");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			builder.AppendLine($"{indentation}    auto* chaos_source = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    {scratchName} = *chaos_source;");
			EmitEvalStackPush(builder, indentation + "    ", $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&{scratchName})");
		}
		else
		{
			builder.AppendLine($"{indentation}    auto* chaos_source = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>({ConsumeEvalStackValueExpression()});");
			EmitEvalStackPush(builder, indentation + "    ", "*chaos_source");
		}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearStoreObjectValue(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear stobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		builder.AppendLine($"{indentation}{{");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			builder.AppendLine($"{indentation}    auto* chaos_source = reinterpret_cast<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}*>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    auto* chaos_destination = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    if (chaos_source == nullptr)");
			builder.AppendLine($"{indentation}    {{");
			builder.AppendLine($"{indentation}        CHAOS_IL2CPP_FAIL_FAST();");
			builder.AppendLine($"{indentation}    }}");
			builder.AppendLine($"{indentation}    *chaos_destination = *chaos_source;");

			builder.AppendLine($"{indentation}    if (chaos_is_gc_pointer(chaos_destination))");
			builder.AppendLine($"{indentation}    {{");
			builder.AppendLine($"{indentation}        chaos_gc_dirty_card(chaos_destination);");
			builder.AppendLine($"{indentation}    }}");
		}
		else
		{
			builder.AppendLine($"{indentation}    const auto chaos_value = {ConsumeEvalStackValueExpression()};");
			builder.AppendLine($"{indentation}    auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    *chaos_destination = chaos_value;");
			// Primitive value types cannot contain GC references - skip write barrier
		}
		builder.AppendLine($"{indentation}}}");
}

private void EmitLinearInitObj(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot structured EH linear initobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}

		builder.AppendLine($"{indentation}{{");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			builder.AppendLine($"{indentation}    auto* chaos_value = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}>({ConsumeEvalStackValueExpression()});");
			builder.AppendLine($"{indentation}    std::memset(chaos_value, 0, sizeof(*chaos_value));");
		}
		else
			{
				var initExpr = ConsumeEvalStackValueExpression();
				// Carrier types (e.g. Vector<T>) on the managed pointer ABI store a pointer
				// to carrier data in the slot. Writing 0 to the slot destroys the pointer.
				// Instead, resolve the managed pointer and memset the carrier data.
				if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType &&
				    IsStructuredValueTypeSubjectId(requiredTargetReference.SubjectId))
				{
					builder.AppendLine($"{indentation}    auto* chaos_value = chaos_resolve_managed_value_pointer<{GetNativeValueTypeSymbol(requiredTargetReference.SubjectId)}>({initExpr});");
					builder.AppendLine($"{indentation}    std::memset(chaos_value, 0, sizeof(*chaos_value));");
				}
				else
				{
					builder.AppendLine($"{indentation}    const auto chaos_address = {initExpr};");
					builder.AppendLine($"{indentation}    if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)");
					builder.AppendLine($"{indentation}    {{");
					builder.AppendLine($"{indentation}        auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));");
					builder.AppendLine($"{indentation}        *chaos_slot = 0;");
					builder.AppendLine($"{indentation}    }}");
					builder.AppendLine($"{indentation}    else");
					builder.AppendLine($"{indentation}    {{");
					builder.AppendLine($"{indentation}        *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;");
					builder.AppendLine($"{indentation}    }}");
				}
			}
		builder.AppendLine($"{indentation}}}");
	}

	private void EmitLinearCallTarget(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation, bool enforceInstanceNullCheck)
	{
		// A2.5: AOT-baked enum calls — pre-evaluated at codegen time.
		if (_enumAotBakeMap.Count > 0 &&
			(instruction.OpCode is InstructionOpCode.Call) &&
			_enumAotBakeMap.TryGetValue((_currentMethodNativeSymbol ?? "", instruction.IlOffset), out var bakeEntry))
		{
			EmitEnumAotBakedCall(builder, instruction, bakeEntry, indentation);
			return;
		}

		// A2.4: Constant-folded Enum::ToString — field name known at codegen time.
		// Check BEFORE ResolveDirectInvocationTarget so we short-circuit the InlineShape.
		if (_enumToStringFoldMap.Count > 0 &&
			(instruction.OpCode is InstructionOpCode.Call or InstructionOpCode.CallVirt) &&
			_enumToStringFoldMap.TryGetValue(instruction.IlOffset, out var foldedFieldName))
		{
			// Consume the `this` argument (boxed enum reference) from the eval stack
			ConsumeEvalStackValueExpression();
			builder.AppendLine($"{indentation}{{");
			EmitEvalStackPush(builder, indentation + "    ", $"CHAOS_IL2CPP_STRING_ID({ToCppStringLiteral(foldedFieldName)})");
			builder.AppendLine($"{indentation}}}");
			return;
		}

		// A2.7: typeof(const_type) compile-time fold — emit direct TypeInfo* pointer
		// instead of calling ChaosReflectionGetTypeFromHandle.
		// The ltoken was already DCE'd in EmitInstruction by A2.7 skip set.
		if (_typeOfFoldMap.Count > 0 &&
			_typeOfFoldMap.TryGetValue((_currentMethodNativeSymbol ?? "", instruction.IlOffset), out var typeOfFold))
		{
			builder.AppendLine(indentation + "{");
			EmitEvalStackPush(builder, indentation + "    ", typeOfFold.TypeInfoExpr);
			builder.AppendLine(indentation + "}");
			return;
		}

		// A2.8: String.get_Length inlining — bypass external runtime dispatch.
		// Emit the body inline at each call site to eliminate function call
		// overhead and enable compiler inlining of the field read.
		if (_activeStructuredSlotContext == null &&
			instruction.Callee is { } callee &&
			callee.IndexOf("::get_Length:", StringComparison.Ordinal) > 0 &&
			callee.IndexOf("/System.String::", StringComparison.Ordinal) > 0)
		{
			var strArg = ConsumeEvalStackValueExpression();
			builder.AppendLine(indentation + "{");
			builder.AppendLine(indentation + "    const auto _str_arg = " + strArg + ";");
			builder.AppendLine(indentation + "    CHAOS_IL2CPP_INT32 _len_result;");
			builder.AppendLine(indentation + "    if (chaos_runtime_get_abi_v0()->is_string_id(_str_arg))");
			builder.AppendLine(indentation + "    {");
			builder.AppendLine(indentation + "        _len_result = static_cast<CHAOS_IL2CPP_INT32>(");
			builder.AppendLine(indentation + "            chaos::il2cpp::string_table::Resolve(");
			builder.AppendLine(indentation + "                chaos_extract_string_id(_str_arg)).byte_count);");
			builder.AppendLine(indentation + "    }");
			builder.AppendLine(indentation + "    else");
			builder.AppendLine(indentation + "    {");
			builder.AppendLine(indentation + "        auto* _str = reinterpret_cast<CHAOS_IL2CPP_STRING_TYPE*>(_str_arg);");
			builder.AppendLine(indentation + "        _len_result = _str->length;");
			builder.AppendLine(indentation + "    }");
			if (_activeStructuredSlotContext is not null)
				EmitEvalStackPush(builder, indentation + "    ", "static_cast<CHAOS_IL2CPP_INTPTR>(_len_result)");
			else
				EmitEvalStackPush(builder, indentation + "    ", $"static_cast<CHAOS_IL2CPP_INTPTR>(_len_result)");
			builder.AppendLine(indentation + "}");
			return;
		}

		InvocationTarget invocationTarget = ResolveDirectInvocationTarget(instruction);
		// IL-level inlining: expand small callee bodies directly at call site.
		// Skip if an InlineCppExpression is already set (higher priority shape expansion).
		if (invocationTarget.TargetSymbol != null && invocationTarget.InlineCppExpression == null)
		{
			if (TryInlineAtCallSite(builder, instruction, invocationTarget, indentation))
				return;
		}

		// A2.6: TypeInfo* direct API for typeof(T).IsAssignableFrom(typeof(U)) etc.
		// Bypasses GetTypeFromHandle and emits *Ptr call with direct TypeInfo* symbols.
		// The *Ptr call is emitted as a pre-try variable to avoid SEH frame overhead
		// (the call is noexcept — only compares pointers, no managed exception possible).
		if (_typeHierarchyPtrFoldMap.Count > 0 &&
			_typeHierarchyPtrFoldMap.TryGetValue((_currentMethodNativeSymbol ?? "", instruction.IlOffset), out var hierarchyFold))
		{
			// Build the *Ptr call expression
			string callExpr;
			if (hierarchyFold.TypeExpr2 is not null)
			{
				// Two-arg methods: stack is clean (DCE skipped ltoken + GetTypeFromHandle)
				callExpr = $"{hierarchyFold.PtrFunctionName}({hierarchyFold.TypeExpr1}, {hierarchyFold.TypeExpr2})";
			}
			else
			{
				// IsInstanceOfType: type arg is typeof(), obj arg is eval stack expression
				var objExpr = ConsumeEvalStackValueExpression();
				callExpr = $"{hierarchyFold.PtrFunctionName}({hierarchyFold.TypeExpr1}, {objExpr})";
			}

			// Emit as pre-try initialization (outside SEH frame)
			var pretryName = $"_type_hierarchy_pretry_{_preTryFoldInitializers?.Count ?? 0}";
			(_preTryFoldInitializers ??= new()).Add((pretryName, callExpr));

			// Inside try-block: just reference the pre-computed value
			builder.AppendLine(indentation + "{");
			EmitEvalStackPush(builder, indentation + "    ", pretryName);
			builder.AppendLine(indentation + "}");
			return;
		}


		// Inline shape expansion: substitute args into expression template at call site.
		if (invocationTarget.InlineCppExpression is { } inlineExpr)
		{
			builder.AppendLine(indentation + "{");
			var argExprs = new string[invocationTarget.ParameterAbis.Count];
			for (int i = invocationTarget.ParameterAbis.Count - 1; i >= 0; i--)
				argExprs[i] = ConsumeEvalStackValueExpression();
			string expr = inlineExpr;
			for (int i = 0; i < argExprs.Length; i++)
				expr = expr.Replace("{" + i + "}", argExprs[i]);
			if (invocationTarget.ReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.Void)
				builder.AppendLine(indentation + "    " + expr + ";");
			else
			{
				builder.AppendLine(indentation + "    const auto chaos_inline_result = " + expr + ";");
				EmitAbiReturnPush(builder, invocationTarget.ReturnAbi, "chaos_inline_result", indentation + "    ");
			}
			builder.AppendLine(indentation + "}");
			return;
		}
		// Assert.Throws<T>(Action) — IL lookahead + direct try/catch
		if (instruction.Callee != null && instruction.Callee.Contains("Chaos.TestFramework.Assert::Throws"))
		{
			EmitAssertThrowsInline(builder, instruction, indentation);
			return;
		}


		if (_nativeSymbolToDispatchSlot?.TryGetValue(invocationTarget.TargetSymbol!, out int slotIndex) == true)
		{
			// Self-call detection: the codegen frontend could not lower this
			// method's IL body and collapsed it to "call self; ret".  Emit
			// CHAOS_IL2CPP_FAIL instead of a hotpatch dispatch wrapper that
			// would infinite-recursion at runtime.
			if (_currentMethodNativeSymbol != null &&
				invocationTarget.TargetSymbol == _currentMethodNativeSymbol)
			{
				builder.AppendLine($"{indentation}{{");
				builder.AppendLine($"{indentation}    CHAOS_IL2CPP_FAIL(\"Unlowered method body: self-call from {invocationTarget.TargetSymbol}\");");
				builder.AppendLine($"{indentation}    chaos_extdirect_end: ;");
				builder.AppendLine($"{indentation}}}");
				return;
			}
		EmitHotpatchResolvedInvocation(builder, slotIndex, invocationTarget.TargetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, indentation, invocationTarget.DirectNativeSymbol);
		}
		else if (TryResolveModuleLocalCall(instruction, invocationTarget, out string localSymbol))
		{
			EmitLinearResolvedInvocation(builder, localSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, indentation, enforceInstanceNullCheck);
		}
		else if (invocationTarget.ExternalRuntimeTableIndex >= 0 || invocationTarget.DirectNativeSymbol != null)
		{
			EmitExternalRuntimeTableDispatch(builder, invocationTarget, indentation, enforceInstanceNullCheck, instruction);
		}
		else
		{
			EmitLinearResolvedInvocation(builder, invocationTarget.TargetSymbol!, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, indentation, enforceInstanceNullCheck);
		}
	}

	/// <summary>
	/// Emit a pre-evaluated AOT-baked enum call result as a compile-time constant.
	/// Called from <see cref="EmitLinearCallTarget"/> when <see cref="_enumAotBakeMap"/> matches.
	/// </summary>
	private void EmitEnumAotBakedCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, EnumAotBakeEntry bakeEntry, string indentation)
	{
		bool isTryParse = bakeEntry.Callee.Contains("::TryParse:", StringComparison.Ordinal);
		string? resultSlotExpr = null;

		for (int i = 0; i < bakeEntry.ArgCount; i++)
		{
			var expr = ConsumeEvalStackValueExpression();
			// For TryParse, the last pushed arg (first consumed) is the ldloca out-parameter address.
			if (isTryParse && i == 0)
				resultSlotExpr = expr;
		}

		builder.AppendLine($"{indentation}{{");

		if (bakeEntry.ConstantStr != null)
		{
			EmitEvalStackPush(builder, indentation + "    ", $"CHAOS_IL2CPP_STRING_ID({ToCppStringLiteral(bakeEntry.ConstantStr)})");
		}
		else if (bakeEntry.ConstantInt != null)
		{
			bool isIsDefined = bakeEntry.Callee.Contains("::IsDefined:", StringComparison.Ordinal);
			if (isIsDefined)
			{
				EmitEvalStackPush(builder, indentation + "    ", bakeEntry.ConstantInt.Value != 0 ? "1" : "0");
			}
			else if (isTryParse)
			{
				// TryParse (AOT-baked): emit cached box via file-scope static array
				// (avoids MSVC C2712 from function-local static with dynamic initializer
				// inside __try/__except).  The _bake_cache_ array is declared at namespace
				// scope in the generated file preamble.
				int cacheIdx = Interlocked.Increment(ref _enumAotBakeCacheCount) - 1;
				builder.AppendLine($"{indentation}    // AOT-baked: {bakeEntry.Callee}");
				builder.AppendLine($"{indentation}    if (!_g_bake_cache_[{cacheIdx}]) {{");
				builder.AppendLine($"{indentation}        auto* box = CHAOS_IL2CPP_NEW_GC({GetNativeBoxTypeSymbol(bakeEntry.EnumTypeId)}, {{}});");
				builder.AppendLine($"{indentation}        box->header.type_info = {GetNativeBoxTypeInfoSymbol(bakeEntry.EnumTypeId)};");
				builder.AppendLine($"{indentation}        box->value = static_cast<CHAOS_IL2CPP_INT64>({bakeEntry.ConstantInt.Value});");
				builder.AppendLine($"{indentation}        _g_bake_cache_[{cacheIdx}] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(box);");
				builder.AppendLine($"{indentation}    }}");
				if (resultSlotExpr != null)
					builder.AppendLine($"{indentation}    *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>({resultSlotExpr}) = _g_bake_cache_[{cacheIdx}];");
				EmitEvalStackPush(builder, indentation + "    ", "1");
			}
			else
			{
				// Other value-returning baked calls (e.g. Enum.Parse): emit cached box,
				// push the box pointer as the return value.  Uses file-scope cache array.
				int cacheIdx = Interlocked.Increment(ref _enumAotBakeCacheCount) - 1;
				builder.AppendLine($"{indentation}    // AOT-baked: {bakeEntry.Callee}");
				builder.AppendLine($"{indentation}    if (!_g_bake_cache_[{cacheIdx}]) {{");
				builder.AppendLine($"{indentation}        auto* box = CHAOS_IL2CPP_NEW_GC({GetNativeBoxTypeSymbol(bakeEntry.EnumTypeId)}, {{}});");
				builder.AppendLine($"{indentation}        box->header.type_info = {GetNativeBoxTypeInfoSymbol(bakeEntry.EnumTypeId)};");
				builder.AppendLine($"{indentation}        box->value = static_cast<CHAOS_IL2CPP_INT64>({bakeEntry.ConstantInt.Value});");
				builder.AppendLine($"{indentation}        _g_bake_cache_[{cacheIdx}] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(box);");
				builder.AppendLine($"{indentation}    }}");
				EmitEvalStackPush(builder, indentation + "    ", $"_g_bake_cache_[{cacheIdx}]");
			}
		}

		builder.AppendLine($"{indentation}}}");
	}

		/// <summary>
		/// Emit inline try/catch for Assert.Throws<T>(Action) via IL lookahead.
		/// Scans backward from the call instruction to find ldnull + ldftn <target>
		/// + newobj Action::.ctor, then emits a direct try/catch around the target method.
		/// </summary>
		private void EmitAssertThrowsInline(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)
		{
			// Consume the Action parameter from eval stack (keep balanced)
			var actionExpr = ConsumeEvalStackValueExpression();
			var (targetSymbol, isStaticTarget) = FindThrowsTargetMethod();
			var indent = indentation;
			if (targetSymbol == null)
			{
				// No target method found — can't inline the try/catch.
				// The generated code will simply not include verification for this call.
				builder.AppendLine($"{indent}{{");
				builder.AppendLine($"{indent}    // Assert.Throws: target method not found via IL lookahead");
				builder.AppendLine($"{indent}    // (verification skipped — use direct Assert.Throws in SDK for full coverage)");
				builder.AppendLine($"{indent}}}");
				return;
			}

			if (!isStaticTarget)
			{
				// Non-static target (e.g. DisplayClass lambda closure): the method
				// takes the delegate target (DisplayClass instance) as the first
				// argument. Extract it from the Action delegate and pass it through.
				var actionNativeType = GetNativeTypeSymbol("System.Private.CoreLib/System.Action");
				builder.AppendLine($"{indent}{{");
				builder.AppendLine($"{indent}    try {{");
				builder.AppendLine($"{indent}        auto* chaos_action = reinterpret_cast<{actionNativeType}*>({actionExpr});");
				builder.AppendLine($"{indent}        {targetSymbol}(chaos_action->chaos_delegate_target);");
				builder.AppendLine($"{indent}        throw chaos_managed_exception{{}};  // no exception — fail");
				builder.AppendLine($"{indent}    }} catch (chaos_managed_exception&) {{");
				builder.AppendLine($"{indent}        // expected exception was thrown — pass");
				builder.AppendLine($"{indent}    }}");
				builder.AppendLine($"{indent}}}");
				return;
			}

			builder.AppendLine($"{indent}{{");
			builder.AppendLine($"{indent}    try {{");
			builder.AppendLine($"{indent}        {targetSymbol}();");
			builder.AppendLine($"{indent}        throw chaos_managed_exception{{}};  // no exception — fail");
			builder.AppendLine($"{indent}    }} catch (chaos_managed_exception&) {{");
			builder.AppendLine($"{indent}        // expected exception was thrown — pass");
			builder.AppendLine($"{indent}    }}");
			builder.AppendLine($"{indent}}}");
		}
}
