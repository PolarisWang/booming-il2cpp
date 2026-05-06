using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.CompilerServices;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
	private static string FormatMethodDeclaration(AotCoreIrMethodArtifact method)
	{
		return FormatMethodDeclaration(method.NativeSymbol, method.ReturnAbi, GetMethodAbiParameterSlots(method));
	}

	private static string FormatMethodDeclaration(string symbol, AotCoreIrAbiSlotArtifact returnAbi, IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis)
	{
		return $"extern \"C\" {MapAbiSlotReturnType(returnAbi)} {symbol}({FormatAbiSlotParameterSignature(parameterAbis)});";
	}

	private static string? TryGetInstantiationStubSymbol(AotCoreIrMethodArtifact method)
	{
		if (method.InstantiationStubId is null)
		{
			return null;
		}

		return ManagedNaming.CreateInstantiationStubSymbol(method.InstantiationStubId);
	}

	private static IReadOnlyList<string> BuildMethodDeclarations(IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
	{
		var declarations = new List<string>();
		foreach (AotCoreIrMethodArtifact reachableMethod in reachableMethods)
		{
			declarations.Add(FormatMethodDeclaration(reachableMethod));
			string? text = TryGetInstantiationStubSymbol(reachableMethod);
			if (!string.IsNullOrEmpty(text))
			{
				declarations.Add(FormatMethodDeclaration(text, reachableMethod.ReturnAbi, GetMethodAbiParameterSlots(reachableMethod)));
			}
		}
		return declarations;
	}

	private static void EmitReachableMethodForwardDeclarations(StringBuilder builder, IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
	{
		foreach (AotCoreIrMethodArtifact reachableMethod in reachableMethods)
		{
			builder.AppendLine(FormatMethodDeclaration(reachableMethod));
			string? text = TryGetInstantiationStubSymbol(reachableMethod);
			if (!string.IsNullOrEmpty(text))
			{
				builder.AppendLine(FormatMethodDeclaration(text, reachableMethod.ReturnAbi, GetMethodAbiParameterSlots(reachableMethod)));
			}
		}

		if (reachableMethods.Count > 0)
		{
			builder.AppendLine();
		}
	}

	private void EmitGenericInstantiationStub(StringBuilder builder, AotCoreIrMethodArtifact method)
	{
		string? text = TryGetInstantiationStubSymbol(method);
		if (string.IsNullOrEmpty(text))
		{
			return;
		}

		IReadOnlyList<AotCoreIrAbiSlotArtifact> methodAbiParameterSlots = GetMethodAbiParameterSlots(method);
		builder.AppendLine();
		builder.AppendLine("// Generic instantiation stub: " + ManagedNaming.GetMethodSubjectIdDisplayString(method.SubjectId));
		builder.AppendLine(FormatGenericExecutionAuthorityComment(
			method.OpenDefinitionSubjectId,
			method.SharedGenericBodyId,
			method.InstantiationStubId,
			method.RuntimeGenericContext));
		builder.AppendLine($"extern \"C\" {MapAbiSlotReturnType(method.ReturnAbi)} {text}({FormatAbiSlotParameterSignature(methodAbiParameterSlots)})");
		builder.AppendLine("{");
		var argNames = new string[methodAbiParameterSlots.Count];
			for (int i = 0; i < methodAbiParameterSlots.Count; i++)
				argNames[i] = "chaos_arg_" + i.ToString();
			string text2 = string.Join(", ", argNames);
		if (method.ReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.Void)
		{
			builder.AppendLine(methodAbiParameterSlots.Count == 0 ? $"    {method.NativeSymbol}();" : $"    {method.NativeSymbol}({text2});");
		}
		else
		{
			builder.AppendLine(methodAbiParameterSlots.Count == 0 ? $"    return {method.NativeSymbol}();" : $"    return {method.NativeSymbol}({text2});");
		}
		builder.AppendLine("}");
	}

	private void EmitManagedMethod(StringBuilder builder, AotCoreIrMethodArtifact method)
	{
		ValidateMethod(method);
		_linearScratchCounter = 0;

		// P/Invoke methods: emit LoadLibrary + GetProcAddress wrapper instead of IL body.
		if (method.IsPInvoke)
		{
			EmitPInvokeMethod(builder, method);
			return;
		}

		IReadOnlyList<AotCoreIrInstructionArtifact> instructions = method.Instructions;
		ValidateInstructions(method, instructions);
		IReadOnlyList<AotCoreIrAbiSlotArtifact> methodAbiParameterSlots = GetMethodAbiParameterSlots(method);
		IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset = CreateNextOffsets(instructions);
		HashSet<int> offsets = new HashSet<int>(instructions.Count);
			for (int idx = 0; idx < instructions.Count; idx++)
				offsets.Add(GetRequiredIlOffset(instructions[idx]));
		bool usesStructuredSlots = TryBuildStructuredMethodBody(method, instructions, offsets, out _, out int structuredSlotCount);
		int evalStackSize = usesStructuredSlots ? 0 : Math.Max(ComputeMaxEvalStackDepth(instructions), 1);
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(19, 1, stringBuilder);
		handler.AppendLiteral("// Managed method: ");
		handler.AppendFormatted(ManagedNaming.GetMethodSubjectIdDisplayString(method.SubjectId));
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(14, 3, stringBuilder);
		handler.AppendLiteral("extern \"C\" ");
		handler.AppendFormatted(MapAbiSlotReturnType(method.ReturnAbi));
		handler.AppendLiteral(" ");
		handler.AppendFormatted(method.NativeSymbol);
		handler.AppendLiteral("(");
		handler.AppendFormatted(FormatAbiSlotParameterSignature(methodAbiParameterSlots));
		handler.AppendLiteral(")");
		stringBuilder3.AppendLine(ref handler);
		builder.AppendLine("{");
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(45, 1, stringBuilder);
		handler.AppendLiteral("    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, ");
		handler.AppendFormatted(Math.Max(methodAbiParameterSlots.Count, 1));
		handler.AppendLiteral(") chaos_args{};");
		stringBuilder4.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(47, 1, stringBuilder);
		handler.AppendLiteral("    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, ");
		handler.AppendFormatted(Math.Max(method.LocalCount, 1));
		handler.AppendLiteral(") chaos_locals{};");
		stringBuilder5.AppendLine(ref handler);
		if (usesStructuredSlots && structuredSlotCount > 0)
		{
			EmitStructuredSlotDeclarations(builder, structuredSlotCount, "    ");
		}
		else if (evalStackSize > 0)
		{
			stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(51, 1, stringBuilder);
			handler.AppendLiteral("    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, ");
			handler.AppendFormatted(evalStackSize);
			handler.AppendLiteral(") chaos_eval_stack{};");
			stringBuilder6.AppendLine(ref handler);
			builder.AppendLine("    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;");
		}
		EmitAbiArgumentInitialization(builder, methodAbiParameterSlots);
		EmitStaticInitializationPrologue(builder, method);
		builder.AppendLine();
		EmitStructuredInstructionRange(builder, method, instructions, nextOffsetsByIlOffset, offsets);
		builder.AppendLine("}");
	}

	private void EmitPInvokeMethod(StringBuilder builder, AotCoreIrMethodArtifact method)
	{
		IReadOnlyList<AotCoreIrAbiSlotArtifact> methodAbiParameterSlots = GetMethodAbiParameterSlots(method);
		string returnType = MapAbiSlotReturnType(method.ReturnAbi);
		string parameterSignature = FormatAbiSlotParameterSignature(methodAbiParameterSlots);
		string moduleName = method.ImportModuleName ?? "?";
		string entryPointName = method.ImportEntryPointName ?? method.NativeSymbol;

		bool hasStringParams = method.StringParameterIndices is { Count: > 0 };
		bool hasStringReturn = string.Equals(method.ReturnType, "System.String", StringComparison.Ordinal);
		bool hasBlittableStructParams = method.BlittableStructParameterIndices is { Count: > 0 };
		bool hasSimpleNonBlittableStructParams = method.SimpleNonBlittableStructParameterIndices is { Count: > 0 };
		bool hasComplexStructParams = method.ComplexStructParameterIndices is { Count: > 0 };
		bool needsMarshalling = hasStringParams || hasStringReturn || hasBlittableStructParams || hasSimpleNonBlittableStructParams || hasComplexStructParams;
		var stringParamSet = hasStringParams
			? new HashSet<int>(method.StringParameterIndices!)
			: new HashSet<int>();
		var blittableStructParamSet = hasBlittableStructParams
			? new HashSet<int>(method.BlittableStructParameterIndices!)
			: new HashSet<int>();
		var simpleNonBlittableSet = hasSimpleNonBlittableStructParams
			? new HashSet<int>(method.SimpleNonBlittableStructParameterIndices!)
			: new HashSet<int>();
		var complexStructSet = hasComplexStructParams
			? new HashSet<int>(method.ComplexStructParameterIndices!)
			: new HashSet<int>();
		Dictionary<int, string>? complexStructDescriptorSymbols = null;
		if (hasComplexStructParams && method.ComplexStructParameterTypeSubjectIds != null)
		{
			complexStructDescriptorSymbols = new Dictionary<int, string>(complexStructSet.Count);
			int typeIdx = 0;
			foreach (int paramIdx in method.ComplexStructParameterIndices!)
			{
				if (typeIdx < method.ComplexStructParameterTypeSubjectIds.Count)
				{
					string typeId = method.ComplexStructParameterTypeSubjectIds[typeIdx];
					complexStructDescriptorSymbols[paramIdx] = GetNativeStructMarshallingDescriptorSymbol(typeId);
				}
				typeIdx++;
			}
		}

		// Build arg names and native call args.
		var argNames = new string[methodAbiParameterSlots.Count];
		var nativeArgs = new string[methodAbiParameterSlots.Count];
		for (int i = 0; i < methodAbiParameterSlots.Count; i++)
		{
			argNames[i] = "chaos_arg_" + i.ToString();
			if (stringParamSet.Contains(i))
			{
				nativeArgs[i] = "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_marshal_" + i + ")";
			}
			else if (complexStructSet.Contains(i))
			{
				nativeArgs[i] = "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_struct_complex_copy_" + i + ")";
			}
			else if (simpleNonBlittableSet.Contains(i))
			{
				// Non-blittable struct with string fields: pass pointer to the copy
				// with pre-converted UTF-8 string fields.
				nativeArgs[i] = "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_struct_copy_" + i + ")";
			}
			else if (blittableStructParamSet.Contains(i))
			{
				// Blittable struct: take address of the by-value parameter to get a pointer.
				nativeArgs[i] = "reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_arg_" + i + ")";
			}
			else
			{
				nativeArgs[i] = argNames[i];
			}
		}
		string nativeArgList = string.Join(", ", nativeArgs);

		// Function pointer type — override blittable struct slots to CHAOS_IL2CPP_INTPTR
		// since native P/Invoke expects pointers, not by-value structs.
		var fnParamTypes = new string[methodAbiParameterSlots.Count];
		for (int i = 0; i < methodAbiParameterSlots.Count; i++)
		{
			fnParamTypes[i] = blittableStructParamSet.Contains(i) || simpleNonBlittableSet.Contains(i) || complexStructSet.Contains(i) || complexStructSet.Contains(i)
				? "CHAOS_IL2CPP_INTPTR"
				: MapAbiSlotReturnType(methodAbiParameterSlots[i]);
		}
		string rawParamTypes = string.Join(", ", fnParamTypes);
		string fnPtrType = string.IsNullOrEmpty(rawParamTypes)
			? $"{returnType}(*)()"
			: $"{returnType}(*)({rawParamTypes})";

		string pinvokeTag = hasStringParams || hasStringReturn
			? "simple non-blittable"
			: hasComplexStructParams
				? "complex non-blittable struct"
				: hasSimpleNonBlittableStructParams
					? "simple non-blittable struct"
					: hasBlittableStructParams
						? "blittable struct"
						: "blittable";

		builder.AppendLine($"// P/Invoke: {moduleName}!{entryPointName} ({pinvokeTag})");
		builder.AppendLine($"extern \"C\" {returnType} {method.NativeSymbol}({parameterSignature})");
		builder.AppendLine("{");
		builder.AppendLine("    static void* s_pinvoke_lib_ = nullptr;");
		builder.AppendLine($"    static {fnPtrType} s_pinvoke_fn_ = nullptr;");

		// Marshalling local variables for string parameters.
		if (hasStringParams)
		{
			foreach (int idx in method.StringParameterIndices!)
			{
				builder.AppendLine($"    void* chaos_marshal_{idx} = nullptr;");
			}
		}

		// Runtime state TLS access for marshalling helpers.
		if (needsMarshalling)
		{
			builder.AppendLine("    auto* chaos_rs_ = ::chaos::il2cpp::runtime_core::GetCurrentRuntimeState();");
			builder.AppendLine("    auto* chaos_ts_ = ::chaos::il2cpp::runtime_core::GetCurrentThreadState();");
		}

		// Stack-local copies for non-blittable struct parameters with string fields.

		// Stack-local copies for complex non-blittable struct parameters.
		if (hasComplexStructParams)
		{
			foreach (int idx in method.ComplexStructParameterIndices!)
			{
				builder.AppendLine("    auto chaos_struct_complex_copy_" + idx + " = chaos_arg_" + idx);
			}
		}
		if (hasSimpleNonBlittableStructParams)
		{
			foreach (int idx in method.SimpleNonBlittableStructParameterIndices!)
			{
				builder.AppendLine($"    auto chaos_struct_copy_{idx} = chaos_arg_{idx};");
			}
		}

		builder.AppendLine("    if (s_pinvoke_fn_ == nullptr)");
		builder.AppendLine("    {");
		builder.AppendLine($"        s_pinvoke_lib_ = ::chaos::il2cpp::runtime_core::NativeLibraryLoad(\"{moduleName}\");");
		builder.AppendLine("        if (s_pinvoke_lib_ == nullptr) CHAOS_IL2CPP_ABORT();");
		builder.AppendLine($"        s_pinvoke_fn_ = reinterpret_cast<{fnPtrType}>(");
		builder.AppendLine($"            ::chaos::il2cpp::runtime_core::NativeLibraryGetProcAddress(s_pinvoke_lib_, \"{entryPointName}\"));");
		builder.AppendLine("        if (s_pinvoke_fn_ == nullptr) CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("    }");

		// Pre-call: marshal string parameters to native UTF-8 CoTaskMem buffers.
		if (hasStringParams)
		{
			builder.AppendLine();
			foreach (int idx in method.StringParameterIndices!)
			{
				builder.AppendLine($"    if (chaos_arg_{idx} != 0)");
				builder.AppendLine("    {");
				builder.AppendLine($"        chaos_marshal_{idx} = reinterpret_cast<void*>(");
				builder.AppendLine($"            ::chaos::il2cpp::runtime_core::MarshalStringToCoTaskMemUtf8(");
				builder.AppendLine($"                chaos_rs_, chaos_ts_, reinterpret_cast<void*>(chaos_arg_{idx})));");
				builder.AppendLine("    }");
			}
		}

		// Pre-call: marshal string fields in non-blittable struct copies to native UTF-8.
		if (hasSimpleNonBlittableStructParams)
		{
			int fieldGroupIdx = 0;
			foreach (int paramIdx in method.SimpleNonBlittableStructParameterIndices!)
			{
				var stringFields = method.SimpleNonBlittableStructStringFieldSubjectIds![fieldGroupIdx];
				foreach (string fieldSubjectId in stringFields)
				{
					string fieldMember = GetNativeFieldMemberName(fieldSubjectId);
					builder.AppendLine($"    if (chaos_struct_copy_{paramIdx}.{fieldMember} != 0)");
					builder.AppendLine("    {");
					builder.AppendLine($"        auto* chaos_marshal_str_ = ::chaos::il2cpp::runtime_core::MarshalStringToCoTaskMemUtf8(");
					builder.AppendLine($"            chaos_rs_, chaos_ts_, reinterpret_cast<void*>(chaos_struct_copy_{paramIdx}.{fieldMember}));");
					builder.AppendLine($"        chaos_struct_copy_{paramIdx}.{fieldMember} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_marshal_str_);");
					builder.AppendLine("    }");
				}
				fieldGroupIdx++;
			}
		}

		// Pre-call: marshal complex non-blittable struct parameters.
		if (hasComplexStructParams && complexStructDescriptorSymbols != null)
		{
			foreach (int idx in method.ComplexStructParameterIndices!)
			{
				if (complexStructDescriptorSymbols.TryGetValue(idx, out string? descSymbol))
				{
					builder.AppendLine("    ::chaos::il2cpp::runtime_core::MarshalStructManagedToNative(");
					builder.AppendLine("        &" + descSymbol + ",");
					builder.AppendLine("        reinterpret_cast<unsigned char*>(&chaos_struct_complex_copy_" + idx + "),");
					builder.AppendLine("        reinterpret_cast<const unsigned char*>(&chaos_arg_" + idx + "),");
					builder.AppendLine("        chaos_rs_, chaos_ts_);");
				}
			}
		}

		// Native call — four code paths:
		//   1. Pure blittable (no marshalling): direct call-and-return, early exit.
		//   2. Blittable struct on non-void return: capture result, cleanup, return.
		//   3. Blittable struct on void return: call, cleanup, fall through.
		//   4. Non-blittable (string): existing string marshal paths.
		builder.AppendLine();
		if (!hasStringParams && !hasStringReturn && !hasBlittableStructParams && !hasSimpleNonBlittableStructParams && !hasComplexStructParams)
		{
			// Path 1: pure blittable — return directly.
			bool isVoidLocal = method.ReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.Void;
			if (isVoidLocal && methodAbiParameterSlots.Count == 0)
				builder.AppendLine("    s_pinvoke_fn_();");
			else if (isVoidLocal)
				builder.AppendLine($"    s_pinvoke_fn_({nativeArgList});");
			else if (methodAbiParameterSlots.Count == 0)
				builder.AppendLine("    return s_pinvoke_fn_();");
			else
				builder.AppendLine($"    return s_pinvoke_fn_({nativeArgList});");
			builder.AppendLine("}");
			return;
		}

		// Paths 2-4: capture result if non-void.
		bool isNonVoid = method.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void;
		if (isNonVoid)
		{
			builder.AppendLine($"    {returnType} chaos_ret_ = s_pinvoke_fn_({nativeArgList});");
		}
		else
		{
			builder.AppendLine($"    s_pinvoke_fn_({nativeArgList});");
		}

		// Post-call: cleanup.
		builder.AppendLine();

		// Free marshalled input string buffers.
		if (hasStringParams)
		{
			foreach (int idx in method.StringParameterIndices!)
			{
				builder.AppendLine($"    if (chaos_marshal_{idx} != nullptr)");
				builder.AppendLine("    {");
				builder.AppendLine($"        ::chaos::il2cpp::runtime_core::MarshalFreeCoTaskMem(");
				builder.AppendLine($"            chaos_rs_, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_marshal_{idx}));");
				builder.AppendLine("    }");
			}
		}

		// Free CoTaskMem buffers in non-blittable struct copies.
		if (hasSimpleNonBlittableStructParams)
		{
			int fieldGroupIdx = 0;
			foreach (int paramIdx in method.SimpleNonBlittableStructParameterIndices!)
			{
				var stringFields = method.SimpleNonBlittableStructStringFieldSubjectIds![fieldGroupIdx];
				foreach (string fieldSubjectId in stringFields)
				{
					string fieldMember = GetNativeFieldMemberName(fieldSubjectId);
					builder.AppendLine($"    if (chaos_struct_copy_{paramIdx}.{fieldMember} != 0)");
					builder.AppendLine("    {");
					builder.AppendLine($"        ::chaos::il2cpp::runtime_core::MarshalFreeCoTaskMem(");
					builder.AppendLine($"            chaos_rs_, chaos_struct_copy_{paramIdx}.{fieldMember});");
					builder.AppendLine("    }");
					}
					fieldGroupIdx++;
				}
			}

		// Post-call: copy marshalled complex struct data back to managed representation.
		if (hasComplexStructParams && complexStructDescriptorSymbols != null)
		{
			foreach (int idx in method.ComplexStructParameterIndices!)
			{
				if (complexStructDescriptorSymbols.TryGetValue(idx, out string? descSymbol))
				{
					builder.AppendLine("    ::chaos::il2cpp::runtime_core::MarshalStructNativeToManaged(");
					builder.AppendLine("        &" + descSymbol + ",");
					builder.AppendLine("        reinterpret_cast<unsigned char*>(&chaos_arg_" + idx + "),");
					builder.AppendLine("        reinterpret_cast<const unsigned char*>(&chaos_struct_complex_copy_" + idx + "),");
					builder.AppendLine("        chaos_rs_, chaos_ts_);");
				}
			}
		}

		// String return: convert native char* to managed string.
		if (hasStringReturn)
		{
			builder.AppendLine("    if (chaos_ret_ != 0)");
			builder.AppendLine("    {");
			builder.AppendLine("        auto* chaos_managed_str_ = ::chaos::il2cpp::runtime_core::MarshalPtrToStringUtf8(");
			builder.AppendLine("            chaos_rs_, chaos_ts_, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ret_), -1, false);");
			builder.AppendLine("        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_managed_str_);");
			builder.AppendLine("    }");
			builder.AppendLine("    return 0;");
		}
		else if (isNonVoid)
		{
			builder.AppendLine("    return chaos_ret_;");
		}

		builder.AppendLine("}");
	}

	private static void ValidateEntryMethod(AotCoreIrMethodArtifact entryMethod)
	{
		ValidateMethod(entryMethod);
		if (entryMethod.ParameterCount == 1)
		{
			if (entryMethod.ParameterAbis.Count != 1)
			{
				throw new NotSupportedException($"native-aot entry supports only zero-parameter or single-int32 entry methods, but '{entryMethod.SubjectId}' has inconsistent abi metadata");
			}
			AotCoreIrAbiCarrierKind carrierKindCode = entryMethod.ParameterAbis[0].CarrierKindCode;
			if (carrierKindCode != AotCoreIrAbiCarrierKind.Int32)
			{
				throw new NotSupportedException($"native-aot entry supports only zero-parameter or single-int32 entry methods, but '{entryMethod.SubjectId}' uses carrier '{carrierKindCode}'.");
			}
		}
		else if (entryMethod.ParameterCount != 0)
		{
			throw new NotSupportedException($"native-aot entry supports only zero-parameter or single-int32 entry methods, but '{entryMethod.SubjectId}' has {entryMethod.ParameterCount} parameters.");
		}
		if (!string.Equals(entryMethod.ReturnType, "System.Int32", StringComparison.Ordinal))
		{
			throw new NotSupportedException("native-aot entry '" + entryMethod.SubjectId + "' must return System.Int32");
		}
	}

	private static void ValidateMethod(AotCoreIrMethodArtifact method)
	{
		if (string.IsNullOrEmpty(method.NativeSymbol))
		{
			throw new InvalidOperationException("native-aot method '" + method.SubjectId + "' is missing native symbol metadata");
		}
		MapAbiSlotReturnType(method.ReturnAbi);
		if (method.ExceptionRegionCount != 0 && !TryCreateCatchOnlyExceptionMethodShape(method, out _) && !TryCreateFilterOnlyExceptionMethodShape(method, out _) && !TryCreateFinallyOnlyExceptionMethodShape(method, out _) && !TryCreateCatchAndFinallyExceptionMethodShape(method, out _) && !TryCreateFilterAndFinallyExceptionMethodShape(method, out _))
		{
			throw new NotSupportedException("native-aot method '" + method.SubjectId + "' does not support current exception region shape");
		}
	}

	private static void ValidateInstructions(AotCoreIrMethodArtifact entryMethod, IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
	{
		if (instructions.Count == 0)
		{
			throw new InvalidOperationException("native-aot entry '" + entryMethod.SubjectId + "' does not contain instructions");
		}
		HashSet<int> hashSet = new HashSet<int>(instructions.Count);
		foreach (AotCoreIrInstructionArtifact instruction in instructions)
		{
			int requiredIlOffset = GetRequiredIlOffset(instruction);
			if (!hashSet.Add(requiredIlOffset))
			{
				throw new InvalidOperationException($"native-aot entry '{entryMethod.SubjectId}' contains duplicate IL offset {requiredIlOffset}");
			}
		}
	}

	private void EmitInstruction(StringBuilder builder, AotCoreIrMethodArtifact method, AotCoreIrInstructionArtifact instruction, int? nextOffset, IReadOnlySet<int> offsets)
	{
		AotCoreIrReferenceArtifact? targetReference = instruction.TargetReference;
		if (!string.IsNullOrEmpty(targetReference?.OpenDefinitionSubjectId) ||
			targetReference?.SharedGenericBodyId is not null ||
			targetReference?.InstantiationStubId is not null ||
			targetReference?.RuntimeGenericContext is not null)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(4, 1, stringBuilder);
			handler.AppendLiteral("    ");
			handler.AppendFormatted(FormatGenericExecutionAuthorityComment(
				targetReference?.OpenDefinitionSubjectId,
				targetReference?.SharedGenericBodyId,
				targetReference?.InstantiationStubId,
				targetReference?.RuntimeGenericContext));
			stringBuilder2.AppendLine(ref handler);
		}
		switch (instruction.Op)
		{
		case "ldc.i4":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder11 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(71, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(");
			handler.AppendFormatted(FormatInt32Literal(GetRequiredIntOperand(instruction)));
			handler.AppendLiteral(");");
			stringBuilder11.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "ldc.i8":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder10 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(62, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = ChaosStoreInt64(");
			handler.AppendFormatted(FormatInt64Literal(GetRequiredInt64Operand(instruction)));
			handler.AppendLiteral(");");
			stringBuilder10.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "ldc.r4":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder9 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(64, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = ChaosStoreFloat32(");
			handler.AppendFormatted(FormatFloat32Literal(GetRequiredSingleOperand(instruction)));
			handler.AppendLiteral(");");
			stringBuilder9.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "ldc.r8":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder8 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(64, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = ChaosStoreFloat64(");
			handler.AppendFormatted(FormatFloat64Literal(GetRequiredDoubleOperand(instruction)));
			handler.AppendLiteral(");");
			stringBuilder8.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "ldarg":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder7 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(55, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_args[");
			handler.AppendFormatted(GetRequiredIntOperand(instruction));
			handler.AppendLiteral("];");
			stringBuilder7.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "ldstr":
			EmitLoadStringLiteral(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldtoken":
			EmitLoadTypeToken(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldarga":
			EmitLoadArgumentAddress(builder, method, instruction, nextOffset, instruction.Op);
			break;
		case "ldnull":
			builder.AppendLine("    chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		case "dup":
			builder.AppendLine("    chaos_eval_stack[chaos_stack_top] = chaos_eval_stack[chaos_stack_top - 1];");
			builder.AppendLine("    chaos_stack_top++;");
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		case "conv.i4":
			builder.AppendLine("    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]));");
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		case "conv.i1":
			EmitStackTopConversion(builder, "CHAOS_IL2CPP_INT8", nextOffset, instruction.Op);
			break;
		case "conv.i2":
			EmitStackTopConversion(builder, "CHAOS_IL2CPP_INT16", nextOffset, instruction.Op);
			break;
		case "conv.i8":
			EmitStackTopConversion(builder, "CHAOS_IL2CPP_INT64", nextOffset, instruction.Op);
			break;
		case "conv.u8":
			EmitStackTopWideIntegralConversion(builder, "CHAOS_IL2CPP_UINT64", "chaos_store_uint64", nextOffset, instruction.Op);
			break;
		case "conv.r4":
			EmitStackTopFloatingPointConversion(builder, "float", "ChaosStoreFloat32", nextOffset, instruction.Op);
			break;
		case "conv.r8":
			EmitStackTopFloatingPointConversion(builder, "double", "ChaosStoreFloat64", nextOffset, instruction.Op);
			break;
		case "conv.u":
			EmitStackTopConversion(builder, "CHAOS_IL2CPP_UINTPTR", nextOffset, instruction.Op);
			break;
		case "localloc":
			EmitLocalAlloc(builder, nextOffset, instruction.Op);
			break;
		case "conv.u1":
			EmitStackTopConversion(builder, "CHAOS_IL2CPP_UINT8", nextOffset, instruction.Op);
			break;
		case "conv.u2":
			EmitStackTopConversion(builder, "CHAOS_IL2CPP_UINT16", nextOffset, instruction.Op);
			break;
		case "conv.ovf.i1":
			EmitCheckedStackTopConversion(builder, "CHAOS_IL2CPP_INT8", "CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(CHAOS_IL2CPP_INT8)", "CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_INT8)", nextOffset, instruction.Op);
			break;
		case "conv.ovf.u1":
			EmitCheckedStackTopConversion(builder, "CHAOS_IL2CPP_UINT8", "0", "CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(CHAOS_IL2CPP_UINT8)", nextOffset, instruction.Op);
			break;
		case "cgt.un":
			builder.AppendLine("    {");
			builder.AppendLine("        const auto chaos_right = static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_eval_stack[--chaos_stack_top]);");
			builder.AppendLine("        const auto chaos_left = static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_eval_stack[--chaos_stack_top]);");
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left > chaos_right ? 1 : 0);");
			builder.AppendLine("    }");
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		case "ceq":
			EmitComparisonResult(builder, instruction, "CHAOS_IL2CPP_INTPTR", "==", nextOffset, instruction.Op);
			break;
		case "cgt":
			EmitComparisonResult(builder, instruction, "CHAOS_IL2CPP_INT32", ">", nextOffset, instruction.Op);
			break;
		case "clt":
			EmitComparisonResult(builder, instruction, "CHAOS_IL2CPP_INT32", "<", nextOffset, instruction.Op);
			break;
		case "ldloc":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(57, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_locals[");
			handler.AppendFormatted(GetRequiredIntOperand(instruction));
			handler.AppendLiteral("];");
			stringBuilder6.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "pop":
			builder.AppendLine("    chaos_stack_top--;");
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		case "ldloca":
			EmitLoadLocalAddress(builder, instruction, nextOffset, instruction.Op);
			break;
		case "stloc":
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(57, 1, stringBuilder);
			handler.AppendLiteral("    chaos_locals[");
			handler.AppendFormatted(GetRequiredIntOperand(instruction));
			handler.AppendLiteral("] = chaos_eval_stack[--chaos_stack_top];");
			stringBuilder5.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, instruction.Op);
			break;
		}
		case "add":
			EmitBinaryArithmetic(builder, "ChaosWrapAdd", nextOffset, instruction.Op);
			break;
		case "add.ovf":
			EmitOverflowingBinaryArithmetic(builder, instruction, "+", nextOffset, instruction.Op);
			break;
		case "sub":
			EmitBinaryArithmetic(builder, "ChaosWrapSub", nextOffset, instruction.Op);
			break;
		case "sub.ovf":
			EmitOverflowingBinaryArithmetic(builder, instruction, "-", nextOffset, instruction.Op);
			break;
		case "mul":
			EmitBinaryArithmetic(builder, "ChaosWrapMul", nextOffset, instruction.Op);
			break;
		case "mul.ovf":
			EmitOverflowingBinaryArithmetic(builder, instruction, "*", nextOffset, instruction.Op);
			break;
		case "div":
			EmitBinaryArithmetic(builder, "ChaosDiv", nextOffset, instruction.Op);
			break;
		case "rem":
			EmitBinaryArithmetic(builder, "ChaosRem", nextOffset, instruction.Op);
			break;
		case "shl":
			EmitShift(builder, instruction, "chaos_shift_left_int32", nextOffset, instruction.Op);
			break;
		case "shr":
			EmitShift(builder, instruction, "chaos_shift_right_int32", nextOffset, instruction.Op);
			break;
		case "shr.un":
			EmitShift(builder, instruction, "chaos_shift_right_un_int32", nextOffset, instruction.Op);
			break;
		case "not":
			EmitBitwiseNot(builder, instruction, nextOffset, instruction.Op);
			break;
		case "and":
			EmitBinaryBitwise(builder, "&", nextOffset, instruction.Op);
			break;
		case "or":
			EmitBinaryBitwise(builder, "|", nextOffset, instruction.Op);
			break;
		case "xor":
			EmitBinaryBitwise(builder, "^", nextOffset, instruction.Op);
			break;
		case "call":
			EmitDirectCall(builder, instruction, nextOffset, instruction.Op);
			break;
		case "callvirt":
			EmitCallVirt(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldftn":
			EmitLoadFunctionPointer(builder, instruction, nextOffset, instruction.Op);
			break;
		case "calli":
			EmitIndirectCall(builder, instruction, nextOffset, instruction.Op);
			break;
		case "newobj":
			EmitNewObject(builder, instruction, nextOffset, instruction.Op);
			break;
		case "throw":
			builder.AppendLine("    throw chaos_managed_exception{chaos_eval_stack[--chaos_stack_top]};");
			break;
		case "newarr":
			EmitNewArray(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldlen":
			EmitArrayLength(builder, nextOffset, instruction.Op);
			break;
		case "ldelema":
			EmitArrayElementAddress(builder, instruction, nextOffset, instruction.Op);
			break;
		case "castclass":
			EmitCastClass(builder, instruction, nextOffset, instruction.Op);
			break;
		case "isinst":
			EmitIsInst(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldfld":
		case "ldsfld":
			EmitFieldLoad(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldflda":
		case "ldsflda":
			EmitFieldAddress(builder, instruction, nextOffset, instruction.Op);
			break;
		case "stfld":
		case "stsfld":
			EmitFieldStore(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldelem":
		case "ldelem.ref":
			EmitArrayLoad(builder, nextOffset, instruction.Op);
			break;
		case "stelem":
		case "stelem.ref":
			EmitArrayStore(builder, nextOffset, instruction.Op);
			break;
		case "ldind.i4":
			EmitLoadIndirect(builder, "CHAOS_IL2CPP_INT32", "static_cast<CHAOS_IL2CPP_INTPTR>", nextOffset, instruction.Op);
			break;
		case "ldind.u1":
			EmitLoadIndirect(builder, "CHAOS_IL2CPP_UINT8", "static_cast<CHAOS_IL2CPP_INTPTR>", nextOffset, instruction.Op);
			break;
		case "ldind.i1":
			EmitLoadIndirect(builder, "CHAOS_IL2CPP_INT8", "static_cast<CHAOS_IL2CPP_INTPTR>", nextOffset, instruction.Op);
			break;
		case "ldind.u2":
			EmitLoadIndirect(builder, "CHAOS_IL2CPP_UINT16", "static_cast<CHAOS_IL2CPP_INTPTR>", nextOffset, instruction.Op);
			break;
		case "ldind.i2":
			EmitLoadIndirect(builder, "CHAOS_IL2CPP_INT16", "static_cast<CHAOS_IL2CPP_INTPTR>", nextOffset, instruction.Op);
			break;
		case "ldind.u4":
			EmitLoadIndirect(builder, "CHAOS_IL2CPP_UINT32", "static_cast<CHAOS_IL2CPP_INTPTR>", nextOffset, instruction.Op);
			break;
		case "ldind.i8":
			EmitLoadIndirect(builder, "CHAOS_IL2CPP_INT64", "static_cast<CHAOS_IL2CPP_INT64>", nextOffset, instruction.Op);
			break;
		case "ldind.r4":
			EmitLoadIndirect(builder, "CHAOS_IL2CPP_FLOAT32", "ChaosStoreFloat32", nextOffset, instruction.Op);
			break;
		case "ldind.r8":
			EmitLoadIndirect(builder, "CHAOS_IL2CPP_FLOAT64", "ChaosStoreFloat64", nextOffset, instruction.Op);
			break;
		case "ldind.ref":
			EmitLoadIndirect(builder, "CHAOS_IL2CPP_INTPTR", "static_cast<CHAOS_IL2CPP_INTPTR>", nextOffset, instruction.Op);
			break;
		case "stind.i4":
			EmitStoreIndirect(builder, "CHAOS_IL2CPP_INT32", "static_cast<CHAOS_IL2CPP_INT32>", nextOffset, instruction.Op);
			break;
		case "stind.i1":
			EmitStoreIndirect(builder, "CHAOS_IL2CPP_INT8", "static_cast<CHAOS_IL2CPP_INT8>", nextOffset, instruction.Op);
			break;
		case "stind.i2":
			EmitStoreIndirect(builder, "CHAOS_IL2CPP_INT16", "static_cast<CHAOS_IL2CPP_INT16>", nextOffset, instruction.Op);
			break;
		case "stind.i8":
			EmitStoreIndirect(builder, "CHAOS_IL2CPP_INT64", "static_cast<CHAOS_IL2CPP_INT64>", nextOffset, instruction.Op);
			break;
		case "stind.r4":
			EmitStoreIndirect(builder, "CHAOS_IL2CPP_FLOAT32", "static_cast<CHAOS_IL2CPP_FLOAT32>", nextOffset, instruction.Op);
			break;
		case "stind.r8":
			EmitStoreIndirect(builder, "CHAOS_IL2CPP_FLOAT64", "static_cast<CHAOS_IL2CPP_FLOAT64>", nextOffset, instruction.Op);
			break;
		case "stind.ref":
			EmitStoreIndirect(builder, "CHAOS_IL2CPP_INTPTR", "static_cast<CHAOS_IL2CPP_INTPTR>", nextOffset, instruction.Op);
			break;
		case "cpblk":
			EmitCopyBlock(builder, nextOffset, instruction.Op);
			break;
		case "box":
			EmitBox(builder, instruction, nextOffset, instruction.Op);
			break;
		case "unbox":
			EmitUnbox(builder, instruction, nextOffset, instruction.Op);
			break;
		case "unbox.any":
			EmitUnboxAny(builder, instruction, nextOffset, instruction.Op);
			break;
		case "initobj":
			EmitInitObj(builder, instruction, nextOffset, instruction.Op);
			break;
		case "ldobj":
			EmitLoadObjectValue(builder, instruction, nextOffset, instruction.Op);
			break;
		case "stobj":
			EmitStoreObjectValue(builder, instruction, nextOffset, instruction.Op);
			break;
		case "br":
		{
			EmitDispatchTransfer(builder, GetRequiredBranchTarget(instruction, offsets), "    ");
			break;
		}
		case "brtrue":
			EmitTruthBranch(builder, shouldBranchWhenNonZero: true, instruction, nextOffset, offsets);
			break;
		case "brfalse":
			EmitTruthBranch(builder, shouldBranchWhenNonZero: false, instruction, nextOffset, offsets);
			break;
		case "leave":
		{
			EmitDispatchTransfer(builder, GetRequiredBranchTarget(instruction, offsets), "    ");
			break;
		}
		case "beq":
			EmitComparisonBranch(builder, "==", instruction, nextOffset, offsets);
			break;
		case "blt":
			EmitComparisonBranch(builder, "<", instruction, nextOffset, offsets);
			break;
		case "bgt":
			EmitComparisonBranch(builder, ">", instruction, nextOffset, offsets);
			break;
		case "ble":
			EmitComparisonBranch(builder, "<=", instruction, nextOffset, offsets);
			break;
		case "bge":
			EmitComparisonBranch(builder, ">=", instruction, nextOffset, offsets);
			break;
		case "bge.un":
			EmitUnsignedComparisonBranch(builder, ">=", instruction, nextOffset, offsets);
			break;
		case "bne.un":
			EmitComparisonBranch(builder, "!=", instruction, nextOffset, offsets);
			break;
		case "switch":
			EmitSwitch(builder, instruction, nextOffset, offsets);
			break;
		case "ret":
			EmitMethodReturn(builder, method.ReturnAbi);
			break;
		default:
			throw new NotSupportedException("native-aot lowering does not support opcode '" + instruction.Op + "'");
		}
	}

	private void EmitDirectCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{

		InvocationTarget invocationTarget = ResolveDirectInvocationTarget(instruction);
		string? targetSymbol = invocationTarget.TargetSymbol;

		// Cross-module calls use method_table dispatch instead of direct symbol calls.
		// When targetSymbol is null (unresolved external generic instantiation),
		// use "0" (nullptr) as the method table function pointer placeholder.

		if (TryGetMethodTableIndex(instruction.Callee, targetSymbol ?? "0", out uint methodTableIndex))
		{
			string returnType = MapAbiSlotReturnType(invocationTarget.ReturnAbi);
			string paramSig = FormatAbiSlotParameterTypes(invocationTarget.ParameterAbis);
			targetSymbol = $"(*reinterpret_cast<{returnType}(*)({paramSig})>(::chaos::il2cpp::method_table::g_method_table[{methodTableIndex}].fn_ptr))";
			EmitResolvedInvocation(builder, targetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, nextOffset, op, enforceInstanceNullCheck: false);
		}
		else if (targetSymbol is null)
		{
			throw new NotSupportedException(
			    "native-aot lowering cannot resolve call target '" +
			    (instruction.Callee ?? "<null>") + "'");
		}
		else
		{
			int dispatchSlot = _nativeSymbolToDispatchSlot != null &&
				_nativeSymbolToDispatchSlot.TryGetValue(targetSymbol, out int slot) ? slot : -1;
			if (dispatchSlot >= 0)
			{
				EmitD3ResolvedInvocation(builder, dispatchSlot, targetSymbol,
					invocationTarget.ParameterAbis, invocationTarget.ReturnAbi,
					invocationTarget.RawArgumentIndices, nextOffset, op);
			}
			else
			{
				EmitResolvedInvocation(builder, targetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, nextOffset, op, enforceInstanceNullCheck: false);
			}
		}
	}

	private void EmitCallVirt(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		if (IsDelegateInvokeInstruction(instruction))
		{
			EmitDelegateInvoke(builder, instruction, nextOffset, op);
			return;
		}
		switch (instruction.DispatchKindCode.GetValueOrDefault())
		{
		case HybridDispatchKind.None:
		case HybridDispatchKind.Direct:
		{
			InvocationTarget invocationTarget = ResolveDirectInvocationTarget(instruction);
			string targetSymbol = invocationTarget.TargetSymbol;
			if (TryGetMethodTableIndex(instruction.Callee, targetSymbol, out uint methodTableIndex))
			{
				string returnType = MapAbiSlotReturnType(invocationTarget.ReturnAbi);
				string paramSig = FormatAbiSlotParameterTypes(invocationTarget.ParameterAbis);
				targetSymbol = $"(*reinterpret_cast<{returnType}(*)({paramSig})>(::chaos::il2cpp::method_table::g_method_table[{methodTableIndex}].fn_ptr))";
				EmitResolvedInvocation(builder, targetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, nextOffset, op, enforceInstanceNullCheck: true);
			}
			else
			{
				int dispatchSlot = _nativeSymbolToDispatchSlot != null &&
					_nativeSymbolToDispatchSlot.TryGetValue(targetSymbol, out int slot) ? slot : -1;
				if (dispatchSlot >= 0)
				{
					EmitD3ResolvedInvocation(builder, dispatchSlot, targetSymbol,
						invocationTarget.ParameterAbis, invocationTarget.ReturnAbi,
						invocationTarget.RawArgumentIndices, nextOffset, op);
				}
				else
				{
					EmitResolvedInvocation(builder, targetSymbol, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, nextOffset, op, enforceInstanceNullCheck: true);
				}
			}
			break;
		}
		case HybridDispatchKind.ExternalRuntime:
		{
			// For ExternalRuntime callvirt, try direct resolution first.
			// If the target can't be resolved (not in the closure), synthesize
			// an InvocationTarget from the subject ID and use method_table dispatch.
			var directTarget = TryResolveDirectInvocationTarget(instruction.Callee);
			InvocationTarget invocationTarget;
			if (directTarget != null)
			{
				var dt = directTarget.Value;
				invocationTarget = new InvocationTarget(
					dt.TargetSymbol, dt.ParameterAbis,
					dt.ReturnAbi, dt.RawArgumentIndices,
					instruction.TargetReference?.OpenDefinitionSubjectId,
					instruction.TargetReference?.SharedGenericBodyId,
					instruction.TargetReference?.InstantiationStubId,
					instruction.TargetReference?.RuntimeGenericContext);
			}
			else if (!string.IsNullOrEmpty(instruction.Callee))
			{
				// Fallback: infer ABI from subjectId, use "0" as placeholder
				// symbol for the method_table entry (compiles, links, but
				// will abort at runtime if actually dispatched — the batch
				// runner only validates compilation, not execution).
				var returnType = InferReturnTypeFromSubjectId(instruction.Callee);
				int paramCount = InferParameterCountFromSubjectId(instruction.Callee);
				invocationTarget = new InvocationTarget(
					string.Empty,
					CreateLegacyAbiParameterSlots(paramCount),
					CreateLegacyReturnAbiSlot(returnType),
					EmptyRawArgumentIndices,
					null, null, null, null);
			}
			else
			{
				throw new NotSupportedException(
					"native-aot lowering does not support unresolved external runtime call '" +
					(instruction.Callee ?? "<null>") + "'");
			}

			string targetSymbol = invocationTarget.TargetSymbol;
			// Use method_table for the fallback case — pass "0" as the
			// native symbol so the generated C++ compiles (writes nullptr
			// to the table slot) even when the real address is unknown.
			string methodTableSymbol = targetSymbol ?? (directTarget == null ? "0" : string.Empty);
			if (TryGetMethodTableIndex(instruction.Callee, methodTableSymbol, out uint methodTableIndex))
			{
				string returnType = MapAbiSlotReturnType(invocationTarget.ReturnAbi);
				string paramSig = FormatAbiSlotParameterTypes(invocationTarget.ParameterAbis);
				targetSymbol = $"(*reinterpret_cast<{returnType}(*)({paramSig})>(::chaos::il2cpp::method_table::g_method_table[{methodTableIndex}].fn_ptr))";
			}
			EmitResolvedInvocation(builder, targetSymbol!, invocationTarget.ParameterAbis, invocationTarget.ReturnAbi, invocationTarget.RawArgumentIndices, nextOffset, op, enforceInstanceNullCheck: true);
			break;
		}
		case HybridDispatchKind.Virtual:
			EmitVirtualDispatchCall(builder, instruction, nextOffset, op);
			break;
		case HybridDispatchKind.ComVtable:
			EmitComVtableCall(builder, instruction, nextOffset, op);
			break;
		default:
			throw new NotSupportedException($"native-aot lowering does not support callvirt dispatch kind '{instruction.DispatchKindCode}'.");
		}
	}

	private void EmitD3ResolvedInvocation(StringBuilder builder, int dispatchSlotIndex, string targetSymbol, IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis, AotCoreIrAbiSlotArtifact returnAbi, IReadOnlySet<int> rawArgumentIndices, int? nextOffset, string op)
	{
		string returnAbiType = MapAbiSlotReturnType(returnAbi);
		bool isVoid = string.Equals(returnAbiType, "void", StringComparison.Ordinal);
		bool hasArgs = parameterAbis.Count > 0;
		int argBufferSize = hasArgs ? CalculateArgBufferSize(parameterAbis) : 0;
		builder.AppendLine("    {");
		// ── Load args from eval stack (same as EmitResolvedInvocation) ──
		for (int num = parameterAbis.Count - 1; num >= 0; num--)
		{
			builder.AppendLine($"        auto chaos_raw_arg_{num} = chaos_eval_stack[--chaos_stack_top];");
			if (_stringIdMapping is { Count: > 0 } && IsStringParameterSlot(parameterAbis[num]))
			{
				builder.AppendLine($"        if (chaos_is_string_id(chaos_raw_arg_{num}))");
				builder.AppendLine("        {");
				builder.AppendLine($"            chaos_raw_arg_{num} = chaos_string_materialize(chaos_raw_arg_{num});");
				builder.AppendLine("        }");
			}
			builder.AppendLine(rawArgumentIndices.Contains(num) ? $"        const auto chaos_arg_{num} = chaos_raw_arg_{num};" : $"        const auto chaos_arg_{num} = {FormatInboundAbiArgumentExpression(parameterAbis[num], $"chaos_raw_arg_{num}")};");
		}
		// ── D3 dispatch ──
		builder.AppendLine($"        auto& _d{dispatchSlotIndex} = s_dispatch_table[{dispatchSlotIndex}];");
		builder.AppendLine($"        if (_d{dispatchSlotIndex}.flags & kDispatchPatched)");
		builder.AppendLine("        {");
		if (hasArgs)
		{
			builder.AppendLine($"            alignas(16) uint8_t _d_ab[{argBufferSize}];");
			builder.AppendLine("            ArgBuffer _d_bw(_d_ab);");
			// Serialize each arg to ArgBuffer
			for (int i = 0; i < parameterAbis.Count; i++)
			{
				string writeCall = GetArgBufferWriteCall(parameterAbis[i].CarrierKindCode, $"chaos_arg_{i}");
				builder.AppendLine($"            {writeCall};");
			}
		}
		if (isVoid)
		{
			builder.AppendLine($"            ::chaos::il2cpp::runtime_core::InterpreterEntryDirect(_d{dispatchSlotIndex}.method_key, {(hasArgs ? "_d_ab" : "nullptr")}, nullptr);");
		}
		else
		{
			builder.AppendLine($"            {returnAbiType} _d_rv{{}};");
			builder.AppendLine($"            ::chaos::il2cpp::runtime_core::InterpreterEntryDirect(_d{dispatchSlotIndex}.method_key, {(hasArgs ? "_d_ab" : "nullptr")}, &_d_rv);");
			EmitAbiReturnPush(builder, returnAbi, "_d_rv", "            ");
		}
		builder.AppendLine("        }");
		builder.AppendLine("        else");
		builder.AppendLine("        {");
		// ── Direct AOT call (same as EmitResolvedInvocation unpatched path) ──
		string callExpr = targetSymbol + "(" + FormatAbiInvocationArgumentList(parameterAbis) + ")";
		if (isVoid)
		{
			builder.AppendLine($"            {callExpr};");
		}
		else
		{
			builder.AppendLine($"            const auto chaos_result = {callExpr};");
			EmitAbiReturnPush(builder, returnAbi, "chaos_result", "            ");
		}
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitResolvedInvocation(StringBuilder builder, string targetSymbol, IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis, AotCoreIrAbiSlotArtifact returnAbi, IReadOnlySet<int> rawArgumentIndices, int? nextOffset, string op, bool enforceInstanceNullCheck)
	{
		string a = MapAbiSlotReturnType(returnAbi);
		builder.AppendLine("    {");
		for (int num = parameterAbis.Count - 1; num >= 0; num--)
		{
			builder.AppendLine($"        auto chaos_raw_arg_{num} = chaos_eval_stack[--chaos_stack_top];");
			if (_stringIdMapping is { Count: > 0 } && IsStringParameterSlot(parameterAbis[num]))
			{
				builder.AppendLine($"        if (chaos_is_string_id(chaos_raw_arg_{num}))");
				builder.AppendLine("        {");
				builder.AppendLine($"            chaos_raw_arg_{num} = chaos_string_materialize(chaos_raw_arg_{num});");
				builder.AppendLine("        }");
			}
			builder.AppendLine(rawArgumentIndices.Contains(num) ? $"        const auto chaos_arg_{num} = chaos_raw_arg_{num};" : $"        const auto chaos_arg_{num} = {FormatInboundAbiArgumentExpression(parameterAbis[num], $"chaos_raw_arg_{num}")};");
		}
		if (enforceInstanceNullCheck && parameterAbis.Count > 0)
		{
			builder.AppendLine("        if (chaos_arg_0 == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
			builder.AppendLine("        {");
			builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
			builder.AppendLine("        }");
		}
		string value = targetSymbol + "(" + FormatAbiInvocationArgumentList(parameterAbis) + ")";
		if (string.Equals(a, "void", StringComparison.Ordinal))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(9, 1, stringBuilder);
			handler.AppendLiteral("        ");
			handler.AppendFormatted(value);
			handler.AppendLiteral(";");
			stringBuilder3.AppendLine(ref handler);
		}
		else
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(35, 1, stringBuilder);
			handler.AppendLiteral("        const auto chaos_result = ");
			handler.AppendFormatted(value);
			handler.AppendLiteral(";");
			stringBuilder4.AppendLine(ref handler);
			EmitAbiReturnPush(builder, returnAbi, "chaos_result", "        ");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}
	private void EmitDelegateInvoke(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		string methodDeclaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(instruction.Callee!);
		IReadOnlyList<AotCoreIrAbiSlotArtifact> readOnlyList = ResolveDelegateInvokeParameterAbis(instruction);
		AotCoreIrAbiSlotArtifact aotCoreIrAbiSlotArtifact = ResolveDelegateInvokeReturnAbi(instruction);
		string text = MapAbiSlotReturnType(aotCoreIrAbiSlotArtifact);
		string sigCache = FormatAbiSlotParameterSignature(readOnlyList);
		string value = string.IsNullOrEmpty(sigCache) ? (text + "(*)()") : string.Concat(text, "(*)(", sigCache, ")");
		string value2 = (string.IsNullOrEmpty(sigCache) ? (text + "(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)") : (text + "(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target, " + sigCache + ")"));
		builder.AppendLine("    {");
		StringBuilder stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler;
		for (int num = readOnlyList.Count - 1; num >= 0; num--)
		{
			builder.AppendLine($"        auto chaos_raw_arg_{num} = chaos_eval_stack[--chaos_stack_top];");
			if (_stringIdMapping is { Count: > 0 } && IsStringParameterSlot(readOnlyList[num]))
			{
				builder.AppendLine($"        if (chaos_is_string_id(chaos_raw_arg_{num}))");
				builder.AppendLine("        {");
				builder.AppendLine($"            chaos_raw_arg_{num} = chaos_string_materialize(chaos_raw_arg_{num});");
				builder.AppendLine("        }");
			}
			builder.AppendLine($"        const auto chaos_arg_{num} = {FormatInboundAbiArgumentExpression(readOnlyList[num], $"chaos_raw_arg_{num}")};");
		}
		builder.AppendLine("        const auto chaos_delegate_value = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        if (chaos_delegate_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(73, 1, stringBuilder);
		handler.AppendLiteral("        auto* chaos_delegate = reinterpret_cast<");
		handler.AppendFormatted(GetNativeTypeSymbol(methodDeclaringTypeSubjectId));
		handler.AppendLiteral("*>(chaos_delegate_value);");
		stringBuilder4.AppendLine(ref handler);
		builder.AppendLine("        if (chaos_delegate->chaos_delegate_invocation_count > static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            const auto* chaos_invocation_list = reinterpret_cast<const CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INTPTR>*>(chaos_delegate->chaos_delegate_invocation_list);");
		builder.AppendLine("            if (chaos_invocation_list == nullptr ||");
		builder.AppendLine("                static_cast<CHAOS_IL2CPP_INTPTR>(chaos_invocation_list->size()) != chaos_delegate->chaos_delegate_invocation_count)");
		builder.AppendLine("            {");
		builder.AppendLine("                CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("            }");
		builder.AppendLine();
		if (!string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(28, 1, stringBuilder);
			handler.AppendLiteral("            ");
			handler.AppendFormatted(text);
			handler.AppendLiteral(" chaos_result{};");
			stringBuilder5.AppendLine(ref handler);
		}
		builder.AppendLine("            for (CHAOS_IL2CPP_SIZE chaos_delegate_index = 0; chaos_delegate_index < chaos_invocation_list->size(); ++chaos_delegate_index)");
		builder.AppendLine("            {");
		builder.AppendLine("                const auto chaos_invocation_delegate_value = (*chaos_invocation_list)[chaos_delegate_index];");
		builder.AppendLine("                if (chaos_invocation_delegate_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("                {");
		builder.AppendLine("                    CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("                }");
		stringBuilder = builder;
		StringBuilder stringBuilder6 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(103, 1, stringBuilder);
		handler.AppendLiteral("                auto* chaos_invocation_delegate = reinterpret_cast<");
		handler.AppendFormatted(GetNativeTypeSymbol(methodDeclaringTypeSubjectId));
		handler.AppendLiteral("*>(chaos_invocation_delegate_value);");
		stringBuilder6.AppendLine(ref handler);
		builder.AppendLine("                if (chaos_invocation_delegate->chaos_delegate_method_ptr == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("                {");
		builder.AppendLine("                    CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("                }");
		builder.AppendLine("                if (chaos_invocation_delegate->chaos_delegate_target == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("                {");
		stringBuilder = builder;
		StringBuilder stringBuilder7 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(126, 1, stringBuilder);
		handler.AppendLiteral("                    const auto chaos_open_function = reinterpret_cast<");
		handler.AppendFormatted(value);
		handler.AppendLiteral(">(chaos_invocation_delegate->chaos_delegate_method_ptr);");
		stringBuilder7.AppendLine(ref handler);
		string value3 = "chaos_open_function(" + FormatAbiInvocationArgumentList(readOnlyList) + ")";
		if (string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder8 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(21, 1, stringBuilder);
			handler.AppendLiteral("                    ");
			handler.AppendFormatted(value3);
			handler.AppendLiteral(";");
			stringBuilder8.AppendLine(ref handler);
		}
		else
		{
			stringBuilder = builder;
			StringBuilder stringBuilder9 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
			handler.AppendLiteral("                    chaos_result = ");
			handler.AppendFormatted(value3);
			handler.AppendLiteral(";");
			stringBuilder9.AppendLine(ref handler);
		}
		builder.AppendLine("                }");
		builder.AppendLine("                else");
		builder.AppendLine("                {");
		stringBuilder = builder;
		StringBuilder stringBuilder10 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(128, 1, stringBuilder);
		handler.AppendLiteral("                    const auto chaos_closed_function = reinterpret_cast<");
		handler.AppendFormatted(value2);
		handler.AppendLiteral(">(chaos_invocation_delegate->chaos_delegate_method_ptr);");
		stringBuilder10.AppendLine(ref handler);
		string value4 = "chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target" + ((readOnlyList.Count == 0) ? string.Empty : (", " + FormatAbiInvocationArgumentList(readOnlyList))) + ")";
		if (string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder11 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(21, 1, stringBuilder);
			handler.AppendLiteral("                    ");
			handler.AppendFormatted(value4);
			handler.AppendLiteral(";");
			stringBuilder11.AppendLine(ref handler);
		}
		else
		{
			stringBuilder = builder;
			StringBuilder stringBuilder12 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
			handler.AppendLiteral("                    chaos_result = ");
			handler.AppendFormatted(value4);
			handler.AppendLiteral(";");
			stringBuilder12.AppendLine(ref handler);
		}
		builder.AppendLine("                }");
		builder.AppendLine("            }");
		if (!string.Equals(text, "void", StringComparison.Ordinal))
		{
			EmitAbiReturnPush(builder, aotCoreIrAbiSlotArtifact, "chaos_result", "            ");
		}
		builder.AppendLine("        }");
		builder.AppendLine("        else");
		builder.AppendLine("        {");
		builder.AppendLine("            if (chaos_delegate->chaos_delegate_method_ptr == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("            {");
		builder.AppendLine("                CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("            }");
		builder.AppendLine("            if (chaos_delegate->chaos_delegate_target == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("            {");
		stringBuilder = builder;
		StringBuilder stringBuilder13 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(111, 1, stringBuilder);
		handler.AppendLiteral("                const auto chaos_open_function = reinterpret_cast<");
		handler.AppendFormatted(value);
		handler.AppendLiteral(">(chaos_delegate->chaos_delegate_method_ptr);");
		stringBuilder13.AppendLine(ref handler);
		if (string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder14 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(17, 1, stringBuilder);
			handler.AppendLiteral("                ");
			handler.AppendFormatted(value3);
			handler.AppendLiteral(";");
			stringBuilder14.AppendLine(ref handler);
		}
		else
		{
			stringBuilder = builder;
			StringBuilder stringBuilder15 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, stringBuilder);
			handler.AppendLiteral("                const auto chaos_result = ");
			handler.AppendFormatted(value3);
			handler.AppendLiteral(";");
			stringBuilder15.AppendLine(ref handler);
			EmitAbiReturnPush(builder, aotCoreIrAbiSlotArtifact, "chaos_result", "                ");
		}
		builder.AppendLine("            }");
		builder.AppendLine("            else");
		builder.AppendLine("            {");
		stringBuilder = builder;
		StringBuilder stringBuilder16 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(113, 1, stringBuilder);
		handler.AppendLiteral("                const auto chaos_closed_function = reinterpret_cast<");
		handler.AppendFormatted(value2);
		handler.AppendLiteral(">(chaos_delegate->chaos_delegate_method_ptr);");
		stringBuilder16.AppendLine(ref handler);
		string value5 = "chaos_closed_function(chaos_delegate->chaos_delegate_target" + ((readOnlyList.Count == 0) ? string.Empty : (", " + FormatAbiInvocationArgumentList(readOnlyList))) + ")";
		if (string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder17 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(17, 1, stringBuilder);
			handler.AppendLiteral("                ");
			handler.AppendFormatted(value5);
			handler.AppendLiteral(";");
			stringBuilder17.AppendLine(ref handler);
		}
		else
		{
			stringBuilder = builder;
			StringBuilder stringBuilder18 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, stringBuilder);
			handler.AppendLiteral("                const auto chaos_result = ");
			handler.AppendFormatted(value5);
			handler.AppendLiteral(";");
			stringBuilder18.AppendLine(ref handler);
			EmitAbiReturnPush(builder, aotCoreIrAbiSlotArtifact, "chaos_result", "                ");
		}
		builder.AppendLine("            }");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitVirtualDispatchCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrMethodArtifact aotCoreIrMethodArtifact = ResolveRequiredDispatchSlotMethod(instruction);
		IReadOnlyList<AotCoreIrAbiSlotArtifact> methodAbiParameterSlots = GetMethodAbiParameterSlots(aotCoreIrMethodArtifact);
		string text = MapAbiSlotReturnType(aotCoreIrMethodArtifact.ReturnAbi);
		string value = $"chaos_callvirt_done_{instruction.IlOffset}";
		builder.AppendLine("    {");
		StringBuilder stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler;
		for (int num = methodAbiParameterSlots.Count - 1; num >= 0; num--)
		{
			builder.AppendLine($"        auto chaos_raw_arg_{num} = chaos_eval_stack[--chaos_stack_top];");
			if (_stringIdMapping is { Count: > 0 } && IsStringParameterSlot(methodAbiParameterSlots[num]))
			{
				builder.AppendLine($"        if (chaos_is_string_id(chaos_raw_arg_{num}))");
				builder.AppendLine("        {");
				builder.AppendLine($"            chaos_raw_arg_{num} = chaos_string_materialize(chaos_raw_arg_{num});");
				builder.AppendLine("        }");
			}
			builder.AppendLine($"        const auto chaos_arg_{num} = {FormatInboundAbiArgumentExpression(methodAbiParameterSlots[num], $"chaos_raw_arg_{num}")};");
		}
		builder.AppendLine("        const auto chaos_instance = chaos_arg_0;");
		builder.AppendLine("        if (chaos_instance == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		builder.AppendLine("        auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_instance);");
		if (!string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(33, 1, stringBuilder);
			handler.AppendLiteral("        ");
			handler.AppendFormatted(text);
			handler.AppendLiteral(" chaos_callvirt_result{};");
			stringBuilder4.AppendLine(ref handler);
		}
				// ── Phase 3: AOT Devirtualization fast-path ──
		string devirtKey = instruction.Callee ?? instruction.TargetReference?.SubjectId ?? "";
		if (devirtKey.Length > 0 && _devirtualizationHints.TryGetValue(devirtKey, out DevirtualizationHint devirtHint) && devirtHint.CanDevirtualize)
		{
			AotCoreIrMethodArtifact devirtMethod = _methodsBySubjectId[devirtHint.ImplementationMethodSubjectId];
			IReadOnlyList<AotCoreIrAbiSlotArtifact> devirtParams = GetMethodAbiParameterSlots(devirtMethod);
			string devirtRet = MapAbiSlotReturnType(devirtMethod.ReturnAbi);
			string devirtSymbol = devirtMethod.NativeSymbol;
			string devirtArgs = FormatAbiInvocationArgumentList(devirtParams);
			bool isGuarded = !string.IsNullOrEmpty(devirtHint.GuardTypeSubjectId);
			if (isGuarded)
			{
				builder.AppendLine($"        if (chaos_header->type_info == &{GetNativeTypeInfoSymbol(devirtHint.GuardTypeSubjectId!)})");
				builder.AppendLine("        {");
			}
			if (string.Equals(devirtRet, "void", StringComparison.Ordinal))
			{
				builder.AppendLine($"        {devirtSymbol}({devirtArgs});");
			}
			else
			{
				builder.AppendLine($"        auto chaos_devirt_result = {devirtSymbol}({devirtArgs});");
				EmitAbiReturnPush(builder, devirtMethod.ReturnAbi, "chaos_devirt_result", "        ");
			}
			if (isGuarded)
			{
				builder.AppendLine($"            goto {value};");
				builder.AppendLine("        }");
			}
			else
			{
				AppendGotoNext(builder, nextOffset, op);
				builder.AppendLine("    }");
				return;
			}
		}
		// -- Interface / VTable dispatch --
		var vtableSlotSig = GetMethodSignatureSuffix(aotCoreIrMethodArtifact.SubjectId);
		string declaringTypeId = aotCoreIrMethodArtifact.Identity.DeclaringTypeSubjectId;
		bool isInterfaceDispatch = !string.IsNullOrEmpty(declaringTypeId) && _interfaceTypeSubjectIds != null && _interfaceTypeSubjectIds.Contains(declaringTypeId);
		if (isInterfaceDispatch)
		{
			if (_vtableSlotMap != null && _vtableSlotMap.TryGetValue(vtableSlotSig, out int vtableSlot))
			{
				string ifaceArgs = FormatAbiInvocationArgumentList(methodAbiParameterSlots, "chaos_instance");
				string ifaceParamSig = FormatAbiSlotParameterSignature(methodAbiParameterSlots);
				string ifaceFnType = string.IsNullOrEmpty(ifaceParamSig)
					? $"{text}(*)()"
					: $"{text}(*)({ifaceParamSig})";
				if (string.Equals(text, "void", StringComparison.Ordinal))
				{
					builder.AppendLine($"        reinterpret_cast<{ifaceFnType}>(chaos_header->vtable[{vtableSlot}])({ifaceArgs});");
				}
				else
				{
					builder.AppendLine($"        chaos_callvirt_result = reinterpret_cast<{ifaceFnType}>(chaos_header->vtable[{vtableSlot}])({ifaceArgs});");
				}
			}
			else
			{
				builder.AppendLine("        CHAOS_IL2CPP_ABORT();");
			}
		}
		else if (_vtableSlotMap != null && _vtableSlotMap.TryGetValue(vtableSlotSig, out int vtableSlot))
		{
			string vtableArgs = FormatAbiInvocationArgumentList(methodAbiParameterSlots, "chaos_instance");
			string vtableParamSig = FormatAbiSlotParameterSignature(methodAbiParameterSlots);
			string vtableFnType = string.IsNullOrEmpty(vtableParamSig)
				? $"{text}(*)()"
				: $"{text}(*)({vtableParamSig})";
			if (string.Equals(text, "void", StringComparison.Ordinal))
			{
				builder.AppendLine($"        reinterpret_cast<{vtableFnType}>(chaos_header->vtable[{vtableSlot}])({vtableArgs});");
			}
			else
			{
				builder.AppendLine($"        chaos_callvirt_result = reinterpret_cast<{vtableFnType}>(chaos_header->vtable[{vtableSlot}])({vtableArgs});");
			}
		}
		else
		{
			builder.AppendLine("        CHAOS_IL2CPP_ABORT();");
		}
		stringBuilder = builder;
		StringBuilder stringBuilder13 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(1, 1, stringBuilder);
		handler.AppendFormatted(value);
		handler.AppendLiteral(":");
		stringBuilder13.AppendLine(ref handler);
		if (!string.Equals(text, "void", StringComparison.Ordinal))
		{
			EmitAbiReturnPush(builder, aotCoreIrMethodArtifact.ReturnAbi, "chaos_callvirt_result", "        ");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitLoadFunctionPointer(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		string functionPointerExpression;
		if (!string.IsNullOrEmpty(instruction.Callee) && _methodsBySubjectId.TryGetValue(instruction.Callee, out AotCoreIrMethodArtifact? targetMethod))
		{
			IReadOnlyList<AotCoreIrAbiSlotArtifact> methodAbiParameterSlots = GetMethodAbiParameterSlots(targetMethod);
			string text = MapAbiSlotReturnType(targetMethod.ReturnAbi);
			string text2 = FormatAbiSlotParameterSignature(methodAbiParameterSlots);
			string arg = string.IsNullOrEmpty(text2) ? (text + "(*)()") : (text + "(*)(" + text2 + ")");
			string text3 = FormatAbiInvocationArgumentList(methodAbiParameterSlots);
			string text4 = string.IsNullOrEmpty(text3)
				? targetMethod.NativeSymbol + "()"
				: targetMethod.NativeSymbol + "(" + text3 + ")";
			string text5 = string.Equals(text, "void", StringComparison.Ordinal)
				? "{ " + text4 + "; }"
				: "{ return " + text4 + "; }";
			functionPointerExpression = $"static_cast<{arg}>(+[]({text2}) -> {text} {text5})";
		}
		else
		{
			functionPointerExpression = "&" + GetRequiredFunctionPointerTargetSymbol(instruction);
		}
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(66, 1, builder);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(");
		handler.AppendFormatted(functionPointerExpression);
		handler.AppendLiteral(");");
		builder.AppendLine(ref handler);
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitComVtableCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		var slot = instruction.ComVtableSlot ?? 0;
		var invocationTarget = ResolveDirectInvocationTarget(instruction);
		var returnType = MapAbiSlotReturnType(invocationTarget.ReturnAbi);
		var paramAbis = invocationTarget.ParameterAbis;

		builder.AppendLine("    {");
		// Pop real arguments (reverse order), skipping index 0 (the instance/COM ptr)
		for (var i = paramAbis.Count - 1; i >= 1; i--)
		{
			builder.AppendLine($"        auto chaos_raw_arg_{i} = chaos_eval_stack[--chaos_stack_top];");
			if (_stringIdMapping is { Count: > 0 } && IsStringParameterSlot(paramAbis[i]))
			{
				builder.AppendLine($"        if (chaos_is_string_id(chaos_raw_arg_{i}))");
				builder.AppendLine("        {");
				builder.AppendLine($"            chaos_raw_arg_{i} = chaos_string_materialize(chaos_raw_arg_{i});");
				builder.AppendLine("        }");
			}
			builder.AppendLine(invocationTarget.RawArgumentIndices.Contains(i)
				? $"        const auto chaos_arg_{i} = chaos_raw_arg_{i};"
				: $"        const auto chaos_arg_{i} = {FormatInboundAbiArgumentExpression(paramAbis[i], $"chaos_raw_arg_{i}")};");
		}

		// Pop the COM interface instance
		builder.AppendLine("        const auto chaos_instance = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        if (chaos_instance == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");

		// COM vtable dispatch: read vtable from the COM pointer, call through slot
		builder.AppendLine("        auto* __com_ptr = reinterpret_cast<void*>(chaos_instance);");
		builder.AppendLine("        auto* __vtbl = *reinterpret_cast<void***>(__com_ptr);");

		// Build COM function pointer type: void* replaces the managed this (chaos_arg_0)
		var paramSig = FormatAbiSlotParameterSignature(paramAbis);
		var firstComma = paramSig.IndexOf(',');
		var comParamSig = firstComma >= 0 ? paramSig[(firstComma + 1)..].TrimStart() : "";
		var fnType = string.IsNullOrEmpty(comParamSig)
			? $"{returnType}(*)(void*)"
			: $"{returnType}(*)(void*, {comParamSig})";
		var argSig = FormatAbiInvocationArgumentList(paramAbis);
		var firstComma2 = argSig.IndexOf(',');
		var comArgsString = firstComma2 >= 0 ? "__com_ptr," + argSig[firstComma2..] : "__com_ptr";

		if (string.Equals(returnType, "void", StringComparison.Ordinal))
		{
			builder.AppendLine($"        reinterpret_cast<{fnType}>(__vtbl[{slot}])({comArgsString});");
		}
		else
		{
			builder.AppendLine($"        const auto chaos_result = reinterpret_cast<{fnType}>(__vtbl[{slot}])({comArgsString});");
			EmitAbiReturnPush(builder, invocationTarget.ReturnAbi, "chaos_result", "        ");
		}

		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitIndirectCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		IReadOnlyList<AotCoreIrAbiSlotArtifact> readOnlyList = CreateCallSiteParameterAbis(instruction);
		AotCoreIrAbiSlotArtifact aotCoreIrAbiSlotArtifact = CreateCallSiteReturnAbi(instruction);
		string text = MapAbiSlotReturnType(aotCoreIrAbiSlotArtifact);
		string sigCache = FormatAbiSlotParameterSignature(readOnlyList);
		string value = string.IsNullOrEmpty(sigCache) ? (text + "(*)()") : string.Concat(text, "(*)(", sigCache, ")");
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_target = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        if (chaos_target == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		StringBuilder stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler;
		for (int num = readOnlyList.Count - 1; num >= 0; num--)
		{
			stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(72, 1, stringBuilder);
			handler.AppendLiteral("        const auto chaos_raw_arg_");
			handler.AppendFormatted(num);
			handler.AppendLiteral(" = chaos_eval_stack[--chaos_stack_top];");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(33, 2, stringBuilder);
			handler.AppendLiteral("        const auto chaos_arg_");
			handler.AppendFormatted(num);
			handler.AppendLiteral(" = ");
			handler.AppendFormatted(FormatInboundAbiArgumentExpression(readOnlyList[num], $"chaos_raw_arg_{num}"));
			handler.AppendLiteral(";");
			stringBuilder3.AppendLine(ref handler);
		}
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(69, 1, stringBuilder);
		handler.AppendLiteral("        const auto chaos_function = reinterpret_cast<");
		handler.AppendFormatted(value);
		handler.AppendLiteral(">(chaos_target);");
		stringBuilder4.AppendLine(ref handler);
		string value2 = "chaos_function(" + FormatAbiInvocationArgumentList(readOnlyList) + ")";
		if (string.Equals(text, "void", StringComparison.Ordinal))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(9, 1, stringBuilder);
			handler.AppendLiteral("        ");
			handler.AppendFormatted(value2);
			handler.AppendLiteral(";");
			stringBuilder5.AppendLine(ref handler);
		}
		else
		{
			stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(35, 1, stringBuilder);
			handler.AppendLiteral("        const auto chaos_result = ");
			handler.AppendFormatted(value2);
			handler.AppendLiteral(";");
			stringBuilder6.AppendLine(ref handler);
			EmitAbiReturnPush(builder, aotCoreIrAbiSlotArtifact, "chaos_result", "        ");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitBinaryArithmetic(StringBuilder builder, string helperName, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(100, 1, builder);
		handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(");
		handler.AppendFormatted(helperName);
		handler.AppendLiteral("(chaos_left, chaos_right));");
		builder.AppendLine(ref handler);
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitBinaryBitwise(StringBuilder builder, string operation, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_right = chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        const auto chaos_left = chaos_load_uint64(chaos_eval_stack[--chaos_stack_top]);");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(90, 1, builder);
		handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = chaos_store_uint64(chaos_left ");
		handler.AppendFormatted(operation);
		handler.AppendLiteral(" chaos_right);");
		builder.AppendLine(ref handler);
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitShift(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string helperName, int? nextOffset, string op)
	{
		RequireInt32IntegralResultType(instruction);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_shift = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(101, 1, builder);
		handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(");
		handler.AppendFormatted(helperName);
		handler.AppendLiteral("(chaos_value, chaos_shift));");
		builder.AppendLine(ref handler);
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitBitwiseNot(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		RequireInt32IntegralResultType(instruction);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[chaos_stack_top - 1]);");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(~chaos_value);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitStackTopConversion(StringBuilder builder, string castType, int? nextOffset, string op)
	{
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(125, 1, builder);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<");
		handler.AppendFormatted(castType);
		handler.AppendLiteral(">(chaos_eval_stack[chaos_stack_top - 1]));");
		builder.AppendLine(ref handler);
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitStackTopFloatingPointConversion(StringBuilder builder, string castType, string storeHelperName, int? nextOffset, string op)
	{
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(99, 2, builder);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top - 1] = ");
		handler.AppendFormatted(storeHelperName);
		handler.AppendLiteral("(static_cast<");
		handler.AppendFormatted(castType);
		handler.AppendLiteral(">(chaos_eval_stack[chaos_stack_top - 1]));");
		builder.AppendLine(ref handler);
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitStackTopWideIntegralConversion(StringBuilder builder, string castType, string storeHelperName, int? nextOffset, string op)
	{
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(99, 2, builder);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top - 1] = ");
		handler.AppendFormatted(storeHelperName);
		handler.AppendLiteral("(static_cast<");
		handler.AppendFormatted(castType);
		handler.AppendLiteral(">(chaos_eval_stack[chaos_stack_top - 1]));");
		builder.AppendLine(ref handler);
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitCheckedStackTopConversion(StringBuilder builder, string castType, string minValueExpression, string maxValueExpression, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = chaos_eval_stack[chaos_stack_top - 1];");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(57, 1, stringBuilder);
		handler.AppendLiteral("        if (chaos_value < static_cast<CHAOS_IL2CPP_INTPTR>(");
		handler.AppendFormatted(minValueExpression);
		handler.AppendLiteral(") ||");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(55, 1, stringBuilder);
		handler.AppendLiteral("            chaos_value > static_cast<CHAOS_IL2CPP_INTPTR>(");
		handler.AppendFormatted(maxValueExpression);
		handler.AppendLiteral("))");
		stringBuilder3.AppendLine(ref handler);
		builder.AppendLine("        {");
		EmitThrowOverflowException(builder, "            ");
		builder.AppendLine("        }");
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(103, 1, stringBuilder);
		handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top - 1] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<");
		handler.AppendFormatted(castType);
		handler.AppendLiteral(">(chaos_value));");
		stringBuilder4.AppendLine(ref handler);
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitOverflowingBinaryArithmetic(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string operation, int? nextOffset, string op)
	{
		RequireInt32IntegralResultType(instruction);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_right = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        const auto chaos_left = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(110, 1, builder);
		handler.AppendLiteral("        const auto chaos_wide = static_cast<CHAOS_IL2CPP_INT64>(chaos_left) ");
		handler.AppendFormatted(operation);
		handler.AppendLiteral(" static_cast<CHAOS_IL2CPP_INT64>(chaos_right);");
		builder.AppendLine(ref handler);
		builder.AppendLine("        if (chaos_wide < static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_NUMERIC_LIMITS_CHAOS_IL2CPP_INT32>::min()) ||");
		builder.AppendLine("            chaos_wide > static_cast<CHAOS_IL2CPP_INT64>(CHAOS_IL2CPP_NUMERIC_LIMITS_CHAOS_IL2CPP_INT32>::max()))");
		builder.AppendLine("        {");
		EmitThrowOverflowException(builder, "            ");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(chaos_wide));");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitThrowOverflowException(StringBuilder builder, string indentation)
	{
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(31, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("auto* chaos_exception = new ");
		handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.OverflowException"));
		handler.AppendLiteral("{};");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(35, 2, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("chaos_exception->header.type_info = &");
		handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.OverflowException"));
		handler.AppendLiteral(";");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(80, 1, stringBuilder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("throw chaos_managed_exception{reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_exception)};");
		stringBuilder4.AppendLine(ref handler);
	}

	private static void EmitLoadLocalAddress(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(130, 1, builder);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[");
		handler.AppendFormatted(GetRequiredIntOperand(instruction));
		handler.AppendLiteral("]) | chaos_managed_pointer_local_slot_tag;");
		builder.AppendLine(ref handler);
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitLoadArgumentAddress(StringBuilder builder, AotCoreIrMethodArtifact method, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		int requiredIntOperand = GetRequiredIntOperand(instruction);
		AotCoreIrAbiSlotArtifact requiredMethodAbiParameterSlot = GetRequiredMethodAbiParameterSlot(method, requiredIntOperand);
		switch (requiredMethodAbiParameterSlot.CarrierKindCode)
		{
		case AotCoreIrAbiCarrierKind.Int32:
		case AotCoreIrAbiCarrierKind.NativeInt:
		case AotCoreIrAbiCarrierKind.Int8:
		case AotCoreIrAbiCarrierKind.UInt8:
		case AotCoreIrAbiCarrierKind.Int16:
		case AotCoreIrAbiCarrierKind.UInt16:
		case AotCoreIrAbiCarrierKind.Float32:
		case AotCoreIrAbiCarrierKind.Float64:
		case AotCoreIrAbiCarrierKind.Int64:
		case AotCoreIrAbiCarrierKind.UInt64:
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(128, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_args[");
			handler.AppendFormatted(requiredIntOperand);
			handler.AppendLiteral("]) | chaos_managed_pointer_local_slot_tag;");
			stringBuilder3.AppendLine(ref handler);
			break;
		}
		case AotCoreIrAbiCarrierKind.ValueTypeByValue:
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(55, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = chaos_args[");
			handler.AppendFormatted(requiredIntOperand);
			handler.AppendLiteral("];");
			stringBuilder2.AppendLine(ref handler);
			break;
		}
		default:
			throw new NotSupportedException($"native-aot lowering does not support ldarga for ABI carrier '{requiredMethodAbiParameterSlot.CarrierKindCode}'.");
		}
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitLoadStringLiteral(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		string requiredStringOperand = GetRequiredStringOperand(instruction);

		// StringId-first: compile-time FNV-1a via CHAOS_IL2CPP_STRING_ID macro.
		// The hash is computed at C++ compile time (constexpr); runtime resolves via
		// string_table::Resolve() binary search in the AOT-baked string table.
		if (TryGetStringId(requiredStringOperand, out ulong _))
		{
			builder.AppendLine("    {");
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(62, 1, builder);
			handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = CHAOS_IL2CPP_STRING_ID(");
			handler.AppendFormatted(ToCppStringLiteral(requiredStringOperand));
			handler.AppendLiteral(");");
			builder.AppendLine(ref handler);
			builder.AppendLine("    }");
		}
		else
		{
			// Fallback: per-call heap allocation (should not happen for AOT-known strings).
			builder.AppendLine("    {");
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_string = new ");
			handler.AppendFormatted(GetNativeTypeSymbol("System.Private.CoreLib/System.String"));
			handler.AppendLiteral("{};");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(40, 1, stringBuilder);
			handler.AppendLiteral("        chaos_string->header.type_info = &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol("System.Private.CoreLib/System.String"));
			handler.AppendLiteral(";");
			stringBuilder3.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(60, 1, stringBuilder);
			handler.AppendLiteral("        chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(");
			handler.AppendFormatted(Encoding.UTF8.GetByteCount(requiredStringOperand));
			handler.AppendLiteral(");");
			stringBuilder4.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(35, 1, stringBuilder);
			handler.AppendLiteral("        chaos_string->utf8_data = ");
			handler.AppendFormatted(ToCppStringLiteral(requiredStringOperand));
			handler.AppendLiteral(";");
			stringBuilder5.AppendLine(ref handler);
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);");
			builder.AppendLine("    }");
		}
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitLoadTypeToken(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, builder);
		handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = ");
		handler.AppendFormatted(GetRequiredTypeHandleLiteral(instruction));
		handler.AppendLiteral(";");
		builder.AppendLine(ref handler);
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitNewObject(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot newobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		if (IsDelegateTypeSubjectId(requiredTargetReference.SubjectId, _referenceTypeBaseSubjectIds))
		{
			builder.AppendLine("    {");
			builder.AppendLine("        const auto chaos_method_ptr = chaos_eval_stack[--chaos_stack_top];");
			builder.AppendLine("        const auto chaos_target = chaos_eval_stack[--chaos_stack_top];");
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_object = new ");
			handler.AppendFormatted(GetNativeTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("{};");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(40, 1, stringBuilder);
			handler.AppendLiteral("        chaos_object->header.type_info = &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder3.AppendLine(ref handler);
			builder.AppendLine("        chaos_object->chaos_delegate_target = chaos_target;");
			builder.AppendLine("        chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;");
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);");
			builder.AppendLine("    }");
			AppendGotoNext(builder, nextOffset, op);
			return;
		}
		if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
		{
			InvocationTarget invocationTarget = TryResolveDirectInvocationTarget(instruction.Callee) ?? throw new NotSupportedException("native-aot lowering requires constructor target for value-type newobj '" + (instruction.Callee ?? "<null>") + "'.");
			if (invocationTarget.ParameterAbis.Count == 0)
			{
				throw new NotSupportedException("native-aot lowering requires instance constructor ABI for '" + (instruction.Callee ?? "<null>") + "'.");
			}
			if (invocationTarget.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
			{
				throw new NotSupportedException("native-aot lowering requires void constructor return ABI for '" + (instruction.Callee ?? "<null>") + "'.");
			}
			builder.AppendLine("    {");
			StringBuilder stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler;
			for (int num = invocationTarget.ParameterAbis.Count - 1; num >= 1; num--)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder4 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(72, 1, stringBuilder);
				handler.AppendLiteral("        const auto chaos_raw_arg_");
				handler.AppendFormatted(num);
				handler.AppendLiteral(" = chaos_eval_stack[--chaos_stack_top];");
				stringBuilder4.AppendLine(ref handler);
				builder.AppendLine(invocationTarget.RawArgumentIndices.Contains(num) ? $"        const auto chaos_arg_{num} = chaos_raw_arg_{num};" : $"        const auto chaos_arg_{num} = {FormatInboundAbiArgumentExpression(invocationTarget.ParameterAbis[num], $"chaos_raw_arg_{num}")};");
			}
			builder.AppendLine("        CHAOS_IL2CPP_INTPTR chaos_value = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("        const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_value) | chaos_managed_pointer_local_slot_tag;");
			stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(11, 2, stringBuilder);
			handler.AppendLiteral("        ");
			handler.AppendFormatted(invocationTarget.TargetSymbol);
			handler.AppendLiteral("(");
			handler.AppendFormatted(FormatAbiInvocationArgumentList(invocationTarget.ParameterAbis));
			handler.AppendLiteral(");");
			stringBuilder5.AppendLine(ref handler);
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_value;");
			builder.AppendLine("    }");
			AppendGotoNext(builder, nextOffset, op);
			return;
		}
		InvocationTarget? invocationTarget2 = TryResolveDirectInvocationTarget(instruction.Callee);
		if (invocationTarget2.HasValue)
		{
			InvocationTarget valueOrDefault = invocationTarget2.GetValueOrDefault();
			if (valueOrDefault.ParameterAbis.Count == 0)
			{
				throw new NotSupportedException("native-aot lowering requires instance constructor ABI for '" + (instruction.Callee ?? "<null>") + "'.");
			}
			if (valueOrDefault.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
			{
				throw new NotSupportedException("native-aot lowering requires void constructor return ABI for '" + (instruction.Callee ?? "<null>") + "'.");
			}
			builder.AppendLine("    {");
			StringBuilder stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler;
			for (int num2 = valueOrDefault.ParameterAbis.Count - 1; num2 >= 1; num2--)
			{
				stringBuilder = builder;
				StringBuilder stringBuilder6 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(72, 1, stringBuilder);
				handler.AppendLiteral("        const auto chaos_raw_arg_");
				handler.AppendFormatted(num2);
				handler.AppendLiteral(" = chaos_eval_stack[--chaos_stack_top];");
				stringBuilder6.AppendLine(ref handler);
				builder.AppendLine(valueOrDefault.RawArgumentIndices.Contains(num2) ? $"        const auto chaos_arg_{num2} = chaos_raw_arg_{num2};" : $"        const auto chaos_arg_{num2} = {FormatInboundAbiArgumentExpression(valueOrDefault.ParameterAbis[num2], $"chaos_raw_arg_{num2}")};");
			}
			stringBuilder = builder;
			StringBuilder stringBuilder7 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_object = new ");
			handler.AppendFormatted(GetNativeTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("{};");
			stringBuilder7.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder8 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(40, 1, stringBuilder);
			handler.AppendLiteral("        chaos_object->header.type_info = &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder8.AppendLine(ref handler);
			if (_vtableTypes?.Contains(requiredTargetReference.SubjectId) == true)
			{
				StringBuilder.AppendInterpolatedStringHandler handler2 = new StringBuilder.AppendInterpolatedStringHandler(40, 1, builder);
				handler2.AppendLiteral("        chaos_object->header.vtable = ");
				handler2.AppendFormatted(GetNativeVTableSymbol(requiredTargetReference.SubjectId));
				handler2.AppendLiteral(";");
				builder.AppendLine(ref handler2);
			}
			builder.AppendLine("        const auto chaos_arg_0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);");
			stringBuilder = builder;
			StringBuilder stringBuilder9 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(11, 2, stringBuilder);
			handler.AppendLiteral("        ");
			handler.AppendFormatted(valueOrDefault.TargetSymbol);
			handler.AppendLiteral("(");
			handler.AppendFormatted(FormatAbiInvocationArgumentList(valueOrDefault.ParameterAbis));
			handler.AppendLiteral(");");
			stringBuilder9.AppendLine(ref handler);
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);");
			builder.AppendLine("    }");
			AppendGotoNext(builder, nextOffset, op);
		}
		else
		{
			builder.AppendLine("    {");
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder10 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_object = new ");
			handler.AppendFormatted(GetNativeTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("{};");
			stringBuilder10.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder11 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(40, 1, stringBuilder);
			handler.AppendLiteral("        chaos_object->header.type_info = &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder11.AppendLine(ref handler);
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);");
			builder.AppendLine("    }");
			AppendGotoNext(builder, nextOffset, op);
		}
	}

	private static void EmitNewArray(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot newarr requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		string subjectId = HasArrayElementReference(requiredTargetReference) ? requiredTargetReference.ArrayElementSubjectId! : requiredTargetReference.SubjectId;
		AotCoreIrTypeShapeKind typeShape = (HasArrayElementReference(requiredTargetReference) ? requiredTargetReference.ArrayElementTypeShape : requiredTargetReference.TypeShape);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_length = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        if (chaos_length < 0)");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		builder.AppendLine("        auto* chaos_array = new chaos_managed_array{};");
		builder.AppendLine("        chaos_array->header.type_info = &chaos_type_info_managed_array;");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, stringBuilder);
		handler.AppendLiteral("        chaos_array->element_type_shape = ");
		handler.AppendFormatted(GetNativeTypeShapeValue(typeShape));
		handler.AppendLiteral(";");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(40, 1, stringBuilder);
		handler.AppendLiteral("        chaos_array->element_type_info = ");
		handler.AppendFormatted(GetRuntimeTypeInfoExpression(subjectId));
		handler.AppendLiteral(";");
		stringBuilder3.AppendLine(ref handler);
		builder.AppendLine("        chaos_array->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_length);");
		builder.AppendLine("        chaos_array->elements = chaos_length == 0 ? nullptr : new CHAOS_IL2CPP_INTPTR[static_cast<CHAOS_IL2CPP_SIZE>(chaos_length)]{};");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_array);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitArrayLength(StringBuilder builder, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        if (chaos_array == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_array->length;");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitArrayElementAddress(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot ldelema requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        if (chaos_array == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		builder.AppendLine("        if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitCastClass(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot castclass requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = chaos_eval_stack[chaos_stack_top - 1];");
		builder.AppendLine("        if (chaos_value != static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_value);");
		if (HasArrayElementReference(requiredTargetReference))
		{
			builder.AppendLine("            if (chaos_header->type_info != &chaos_type_info_managed_array)");
			builder.AppendLine("            {");
			builder.AppendLine("                CHAOS_IL2CPP_ABORT();");
			builder.AppendLine("            }");
			builder.AppendLine("            auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_value);");
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(115, 2, stringBuilder);
			handler.AppendLiteral("            if (!chaos_is_array_type_compatible(chaos_array->element_type_shape, chaos_array->element_type_info, ");
			handler.AppendFormatted(GetNativeTypeShapeValue(requiredTargetReference.ArrayElementTypeShape));
			handler.AppendLiteral(", ");
			handler.AppendFormatted(GetRuntimeTypeInfoExpression(requiredTargetReference.ArrayElementSubjectId));
			handler.AppendLiteral("))");
			stringBuilder2.AppendLine(ref handler);
		}
		else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.InterfaceType)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(78, 1, stringBuilder);
			handler.AppendLiteral("            if (!chaos_does_type_implement_interface(chaos_header->type_info, &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("))");
			stringBuilder3.AppendLine(ref handler);
		}
		else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(67, 1, stringBuilder);
			handler.AppendLiteral("            if (!chaos_is_type_compatible(chaos_header->type_info, &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("))");
			stringBuilder4.AppendLine(ref handler);
		}
		else
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(42, 1, stringBuilder);
			handler.AppendLiteral("            if (chaos_header->type_info != &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(" && chaos_header->type_info->stable_id != (&");
			handler.AppendFormatted(GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(")->stable_id)");
			stringBuilder5.AppendLine(ref handler);
		}
		builder.AppendLine("            {");
		builder.AppendLine("                CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("            }");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitIsInst(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot isinst requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        auto chaos_matches = false;");
		builder.AppendLine("        if (chaos_value != static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            auto* chaos_header = reinterpret_cast<chaos_object_header*>(chaos_value);");
		if (HasArrayElementReference(requiredTargetReference))
		{
			builder.AppendLine("            if (chaos_header->type_info == &chaos_type_info_managed_array)");
			builder.AppendLine("            {");
			builder.AppendLine("                auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_value);");
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(130, 2, stringBuilder);
			handler.AppendLiteral("                chaos_matches = chaos_is_array_type_compatible(chaos_array->element_type_shape, chaos_array->element_type_info, ");
			handler.AppendFormatted(GetNativeTypeShapeValue(requiredTargetReference.ArrayElementTypeShape));
			handler.AppendLiteral(", ");
			handler.AppendFormatted(GetRuntimeTypeInfoExpression(requiredTargetReference.ArrayElementSubjectId));
			handler.AppendLiteral(");");
			stringBuilder2.AppendLine(ref handler);
			builder.AppendLine("            }");
		}
		else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.InterfaceType)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(89, 1, stringBuilder);
			handler.AppendLiteral("            chaos_matches = chaos_does_type_implement_interface(chaos_header->type_info, &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(");");
			stringBuilder3.AppendLine(ref handler);
		}
		else if (requiredTargetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(78, 1, stringBuilder);
			handler.AppendLiteral("            chaos_matches = chaos_is_type_compatible(chaos_header->type_info, &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(");");
			stringBuilder4.AppendLine(ref handler);
		}
		else
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(54, 1, stringBuilder);
			handler.AppendLiteral("            chaos_matches = chaos_header->type_info == &");
			handler.AppendFormatted(GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(" || chaos_header->type_info->stable_id == (&");
			handler.AppendFormatted(GetNativeTypeInfoSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(")->stable_id;");
			stringBuilder5.AppendLine(ref handler);
		}
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_matches ? chaos_value : static_cast<CHAOS_IL2CPP_INTPTR>(0);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitFieldLoad(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Field)
		{
			throw new NotSupportedException($"native-aot field load requires field target reference, got '{requiredTargetReference.Kind}'.");
		}
		if (instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.LoadStaticField)
		{
			EmitStaticInitializationForField(builder, requiredTargetReference.SubjectId, "    ");
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(43, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = ");
			handler.AppendFormatted(GetNativeStaticFieldSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder2.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, op);
			return;
		}
		builder.AppendLine("    {");
		if (requiredTargetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
		{
			string requiredDeclaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(103, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_value = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredDeclaringTypeSubjectId));
			handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder3.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(60, 1, stringBuilder);
			handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = chaos_value->");
			handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder4.AppendLine(ref handler);
		}
		else
		{
			string requiredDeclaringTypeSubjectId2 = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(86, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_object = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol(requiredDeclaringTypeSubjectId2));
			handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder5.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(61, 1, stringBuilder);
			handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = chaos_object->");
			handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
			handler.AppendLiteral(";");
			stringBuilder6.AppendLine(ref handler);
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitFieldAddress(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Field)
		{
			throw new NotSupportedException($"native-aot field address requires field target reference, got '{requiredTargetReference.Kind}'.");
		}
		if (instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.LoadStaticField)
		{
			EmitStaticInitializationForField(builder, requiredTargetReference.SubjectId, "    ");
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(77, 1, stringBuilder);
			handler.AppendLiteral("    chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&");
			handler.AppendFormatted(GetNativeStaticFieldSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(");");
			stringBuilder2.AppendLine(ref handler);
			AppendGotoNext(builder, nextOffset, op);
			return;
		}
		builder.AppendLine("    {");
		string requiredDeclaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
		if (requiredTargetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(103, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_value = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredDeclaringTypeSubjectId));
			handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder3.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(94, 1, stringBuilder);
			handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_value->");
			handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
			handler.AppendLiteral(");");
			stringBuilder4.AppendLine(ref handler);
		}
		else
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder5 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(86, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_object = reinterpret_cast<");
			handler.AppendFormatted(GetNativeTypeSymbol(requiredDeclaringTypeSubjectId));
			handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder5.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(95, 1, stringBuilder);
			handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_object->");
			handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
			handler.AppendLiteral(");");
			stringBuilder6.AppendLine(ref handler);
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitFieldStore(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Field)
		{
			throw new NotSupportedException($"native-aot field store requires field target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		builder.AppendLine("        auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
		if (_stringIdMapping is { Count: > 0 })
		{
			builder.AppendLine("        if (chaos_is_string_id(chaos_value))");
			builder.AppendLine("        {");
			builder.AppendLine("            chaos_value = chaos_string_materialize(chaos_value);");
			builder.AppendLine("        }");
		}
		var cppFieldType = MapFieldTypeToCppType(requiredTargetReference.FieldTypeSubjectId);
		if (instruction.RuntimeServiceKind == AotCoreIrRuntimeServiceKind.StoreStaticField)
		{
			EmitStaticInitializationForField(builder, requiredTargetReference.SubjectId, "        ");
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(38, 2, stringBuilder);
			handler.AppendLiteral("        ");
			handler.AppendFormatted(GetNativeStaticFieldSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(" = static_cast<");
			handler.AppendFormatted(cppFieldType);
			handler.AppendLiteral(">(chaos_value);");
			stringBuilder2.AppendLine(ref handler);
		}
		else
		{
			string requiredDeclaringTypeSubjectId = GetRequiredDeclaringTypeSubjectId(requiredTargetReference);
			if (requiredTargetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType)
			{
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder3 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(109, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_value_owner = chaos_resolve_managed_value_pointer<");
				handler.AppendFormatted(GetNativeValueTypeSymbol(requiredDeclaringTypeSubjectId));
				handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
				stringBuilder3.AppendLine(ref handler);
				stringBuilder = builder;
				StringBuilder stringBuilder4 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(60, 2, stringBuilder);
				handler.AppendLiteral("        chaos_value_owner->");
				handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
				handler.AppendLiteral(" = static_cast<");
				handler.AppendFormatted(cppFieldType);
				handler.AppendLiteral(">(chaos_value);");
				stringBuilder4.AppendLine(ref handler);
			}
			else
			{
				StringBuilder stringBuilder = builder;
				StringBuilder stringBuilder5 = stringBuilder;
				StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(86, 1, stringBuilder);
				handler.AppendLiteral("        auto* chaos_object = reinterpret_cast<");
				handler.AppendFormatted(GetNativeTypeSymbol(requiredDeclaringTypeSubjectId));
				handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
				stringBuilder5.AppendLine(ref handler);
				stringBuilder = builder;
				StringBuilder stringBuilder6 = stringBuilder;
				handler = new StringBuilder.AppendInterpolatedStringHandler(55, 2, stringBuilder);
				handler.AppendLiteral("        chaos_object->");
				handler.AppendFormatted(GetNativeFieldMemberName(requiredTargetReference.SubjectId));
				handler.AppendLiteral(" = static_cast<");
				handler.AppendFormatted(cppFieldType);
				handler.AppendLiteral(">(chaos_value);");
				stringBuilder6.AppendLine(ref handler);
			}
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitArrayLoad(StringBuilder builder, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        if (chaos_array == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		builder.AppendLine("        if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitArrayStore(StringBuilder builder, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
		if (_stringIdMapping is { Count: > 0 })
		{
			builder.AppendLine("        if (chaos_is_string_id(chaos_value))");
			builder.AppendLine("        {");
			builder.AppendLine("            chaos_value = chaos_string_materialize(chaos_value);");
			builder.AppendLine("        }");
		}
		builder.AppendLine("        const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        if (chaos_array == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		builder.AppendLine("        if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		if (string.Equals(op, "stelem.ref", StringComparison.Ordinal))
		{
			builder.AppendLine("        if (!chaos_is_array_store_compatible(chaos_array, chaos_value))");
			builder.AppendLine("        {");
			builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
			builder.AppendLine("        }");
		}
		builder.AppendLine("        chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;");
		if (string.Equals(op, "stelem.ref", StringComparison.Ordinal))
		{
			builder.AppendLine("        GC_END_STUBBORN_CHANGE(chaos_array);");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	
	private static void EmitLoadIndirect(StringBuilder builder, string nativeType, string stackOp, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        const auto chaos_value = chaos_load_indirect<" + nativeType + ">(chaos_address);");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = " + stackOp + "(chaos_value);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private void EmitStoreIndirect(StringBuilder builder, string nativeType, string valueCast, int? nextOffset, string op)
	{
		builder.AppendLine("    {");
		builder.AppendLine("        auto chaos_value = " + valueCast + "(chaos_eval_stack[--chaos_stack_top]);");
		if (string.Equals(op, "stind.ref", StringComparison.Ordinal) && _stringIdMapping is { Count: > 0 })
		{
			builder.AppendLine("        if (chaos_is_string_id(chaos_value))");
			builder.AppendLine("        {");
			builder.AppendLine("            chaos_value = chaos_string_materialize(chaos_value);");
			builder.AppendLine("        }");
		}
		builder.AppendLine("        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        chaos_store_indirect<" + nativeType + ">(chaos_address, chaos_value);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}


	private static void EmitLocalAlloc(StringBuilder builder, int? nextOffset, string op)
	{
		builder.AppendLine("    // localloc");
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_size = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine("        if (chaos_size < 0)");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		builder.AppendLine("        const auto chaos_byte_count = static_cast<CHAOS_IL2CPP_SIZE>(chaos_size);");
		builder.AppendLine("        void* chaos_block = CHAOS_IL2CPP_MALLOC(chaos_byte_count == static_cast<CHAOS_IL2CPP_SIZE>(0) ? static_cast<CHAOS_IL2CPP_SIZE>(1) : chaos_byte_count);");
		builder.AppendLine("        if (chaos_block == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_block);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitCopyBlock(StringBuilder builder, int? nextOffset, string op)
	{
		builder.AppendLine("    // cpblk");
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_size = static_cast<CHAOS_IL2CPP_SIZE>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_eval_stack[--chaos_stack_top]));");
		builder.AppendLine("        const auto chaos_source = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        const auto chaos_destination = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine("        if (chaos_size != static_cast<CHAOS_IL2CPP_SIZE>(0))");
		builder.AppendLine("        {");
		builder.AppendLine("            const auto chaos_resolve_cpblk_address = [](CHAOS_IL2CPP_INTPTR chaos_address) -> void*");
		builder.AppendLine("            {");
		builder.AppendLine("                if ((chaos_address & CHAOS_IL2CPP_RAW_POINTER_TAG) != 0)");
		builder.AppendLine("                {");
		builder.AppendLine("                    return reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~CHAOS_IL2CPP_RAW_POINTER_TAG));");
		builder.AppendLine("                }");
		builder.AppendLine("                if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)");
		builder.AppendLine("                {");
		builder.AppendLine("                    return reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));");
		builder.AppendLine("                }");
		builder.AppendLine("                return reinterpret_cast<void*>(chaos_address);");
		builder.AppendLine("            };");
		builder.AppendLine("            if (chaos_source == static_cast<CHAOS_IL2CPP_INTPTR>(0) || chaos_destination == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("            {");
		builder.AppendLine("                CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("            }");
		builder.AppendLine("            CHAOS_IL2CPP_MEMCPY(chaos_resolve_cpblk_address(chaos_destination), chaos_resolve_cpblk_address(chaos_source), chaos_size);");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitBox(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot box requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(35, 1, stringBuilder);
		handler.AppendLiteral("        auto* chaos_boxed = new ");
		handler.AppendFormatted(GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId));
		handler.AppendLiteral("{};");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(39, 1, stringBuilder);
		handler.AppendLiteral("        chaos_boxed->header.type_info = &");
		handler.AppendFormatted(GetNativeBoxTypeInfoSymbol(requiredTargetReference.SubjectId));
		handler.AppendLiteral(";");
		stringBuilder3.AppendLine(ref handler);
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder4 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(81, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_payload = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(">(chaos_value);");
			stringBuilder4.AppendLine(ref handler);
			builder.AppendLine("        chaos_boxed->value = *chaos_payload;");
		}
		else
		{
			builder.AppendLine("        chaos_boxed->value = chaos_value;");
		}
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitUnbox(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot unbox requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(85, 1, builder);
		handler.AppendLiteral("        auto* chaos_boxed = reinterpret_cast<");
		handler.AppendFormatted(GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId));
		handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
		builder.AppendLine(ref handler);
		builder.AppendLine("        if (chaos_boxed == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_boxed->value);");
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitUnboxAny(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot unbox.any requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(85, 1, stringBuilder);
		handler.AppendLiteral("        auto* chaos_boxed = reinterpret_cast<");
		handler.AppendFormatted(GetNativeBoxTypeSymbol(requiredTargetReference.SubjectId));
		handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder2.AppendLine(ref handler);
		builder.AppendLine("        if (chaos_boxed == nullptr)");
		builder.AppendLine("        {");
		builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("        }");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(35, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_value = new ");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("{};");
			stringBuilder3.AppendLine(ref handler);
			builder.AppendLine("        *chaos_value = chaos_boxed->value;");
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_value);");
		}
		else
		{
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = chaos_boxed->value;");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitLoadObjectValue(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot ldobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(104, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_source = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(35, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_value = new ");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("{};");
			stringBuilder3.AppendLine(ref handler);
			builder.AppendLine("        *chaos_value = *chaos_source;");
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_value);");
		}
		else
		{
			builder.AppendLine("        auto* chaos_source = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(chaos_eval_stack[--chaos_stack_top]);");
			builder.AppendLine("        chaos_eval_stack[chaos_stack_top++] = *chaos_source;");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitStoreObjectValue(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot stobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(86, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_source = reinterpret_cast<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("*>(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(109, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_destination = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder3.AppendLine(ref handler);
			builder.AppendLine("        if (chaos_source == nullptr)");
			builder.AppendLine("        {");
			builder.AppendLine("            CHAOS_IL2CPP_ABORT();");
			builder.AppendLine("        }");
			builder.AppendLine("        *chaos_destination = *chaos_source;");
		}
		else
		{
			builder.AppendLine("        const auto chaos_value = chaos_eval_stack[--chaos_stack_top];");
			builder.AppendLine("        auto* chaos_destination = chaos_resolve_managed_value_pointer<CHAOS_IL2CPP_INTPTR>(chaos_eval_stack[--chaos_stack_top]);");
			builder.AppendLine("        *chaos_destination = chaos_value;");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static void EmitInitObj(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, string op)
	{
		AotCoreIrReferenceArtifact requiredTargetReference = GetRequiredTargetReference(instruction);
		if (requiredTargetReference.Kind != AotCoreIrReferenceKind.Type)
		{
			throw new NotSupportedException($"native-aot initobj requires type target reference, got '{requiredTargetReference.Kind}'.");
		}
		builder.AppendLine("    {");
		if (RequiresStructuredValueTypePayload(requiredTargetReference))
		{
			StringBuilder stringBuilder = builder;
			StringBuilder stringBuilder2 = stringBuilder;
			StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(103, 1, stringBuilder);
			handler.AppendLiteral("        auto* chaos_value = chaos_resolve_managed_value_pointer<");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
			stringBuilder2.AppendLine(ref handler);
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(26, 1, stringBuilder);
			handler.AppendLiteral("        *chaos_value = ");
			handler.AppendFormatted(GetNativeValueTypeSymbol(requiredTargetReference.SubjectId));
			handler.AppendLiteral("{};");
			stringBuilder3.AppendLine(ref handler);
		}
		else
		{
			builder.AppendLine("        const auto chaos_address = chaos_eval_stack[--chaos_stack_top];");
			builder.AppendLine("        if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)");
			builder.AppendLine("        {");
			builder.AppendLine("            auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));");
			builder.AppendLine("            *chaos_slot = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("        }");
			builder.AppendLine("        else");
			builder.AppendLine("        {");
			builder.AppendLine("            *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = static_cast<CHAOS_IL2CPP_INTPTR>(0);");
			builder.AppendLine("        }");
		}
		builder.AppendLine("    }");
		AppendGotoNext(builder, nextOffset, op);
	}

	private static HashSet<int>? _dispatchExitTargets;

	private static void EmitDispatchTransfer(StringBuilder builder, int targetOffset, string indentation)
	{
		if (_dispatchExitTargets?.Contains(targetOffset) == true)
		{
			builder.AppendLine(indentation + "chaos_dispatch_completed = true;");
			builder.AppendLine(indentation + "break;");
			return;
		}

		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(23, 2, builder);
		handler.AppendFormatted(indentation);
		handler.AppendLiteral("chaos_pc = ");
		handler.AppendFormatted(targetOffset);
		handler.AppendLiteral(";");
		builder.AppendLine(ref handler);
		builder.AppendLine(indentation + "continue;");
	}

	private static void AppendDispatchNext(StringBuilder builder, int? nextOffset, string op, string indentation)
	{
		if (!nextOffset.HasValue)
		{
			throw new InvalidOperationException("opcode '" + op + "' cannot be the final instruction in the method");
		}

		EmitDispatchTransfer(builder, nextOffset.Value, indentation);
	}

	private static void EmitComparisonBranch(StringBuilder builder, string comparisonOperator, AotCoreIrInstructionArtifact instruction, int? nextOffset, IReadOnlySet<int> offsets)
	{
		if (!nextOffset.HasValue)
		{
			throw new InvalidOperationException("branch opcode '" + instruction.Op + "' cannot terminate the method");
		}
		int requiredBranchTarget = GetRequiredBranchTarget(instruction, offsets);
		bool flag = ((comparisonOperator == "==" || comparisonOperator == "!=") ? true : false);
		string value = (flag ? "CHAOS_IL2CPP_INTPTR" : "CHAOS_IL2CPP_INT32");
		builder.AppendLine("    {");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(84, 1, stringBuilder);
		handler.AppendLiteral("        const auto chaos_right = static_cast<");
		handler.AppendFormatted(value);
		handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(83, 1, stringBuilder);
		handler.AppendLiteral("        const auto chaos_left = static_cast<");
		handler.AppendFormatted(value);
		handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
		handler.AppendLiteral("        if (chaos_left ");
		handler.AppendFormatted(comparisonOperator);
		handler.AppendLiteral(" chaos_right)");
		stringBuilder4.AppendLine(ref handler);
		builder.AppendLine("        {");
		EmitDispatchTransfer(builder, requiredBranchTarget, "            ");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		AppendDispatchNext(builder, nextOffset, instruction.Op, "    ");
	}

	private static void EmitUnsignedComparisonBranch(StringBuilder builder, string comparisonOperator, AotCoreIrInstructionArtifact instruction, int? nextOffset, IReadOnlySet<int> offsets)
	{
		if (!nextOffset.HasValue)
		{
			throw new InvalidOperationException("branch opcode '" + instruction.Op + "' cannot terminate the method");
		}
		int requiredBranchTarget = GetRequiredBranchTarget(instruction, offsets);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_right = static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]));");
		builder.AppendLine("        const auto chaos_left = static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]));");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(36, 1, stringBuilder);
		handler.AppendLiteral("        if (chaos_left ");
		handler.AppendFormatted(comparisonOperator);
		handler.AppendLiteral(" chaos_right)");
		stringBuilder2.AppendLine(ref handler);
		builder.AppendLine("        {");
		EmitDispatchTransfer(builder, requiredBranchTarget, "            ");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		AppendDispatchNext(builder, nextOffset, instruction.Op, "    ");
	}

	private static void EmitTruthBranch(StringBuilder builder, bool shouldBranchWhenNonZero, AotCoreIrInstructionArtifact instruction, int? nextOffset, IReadOnlySet<int> offsets)
	{
		if (!nextOffset.HasValue)
		{
			throw new InvalidOperationException("branch opcode '" + instruction.Op + "' cannot terminate the method");
		}
		int requiredBranchTarget = GetRequiredBranchTarget(instruction, offsets);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_condition = chaos_eval_stack[--chaos_stack_top];");
		builder.AppendLine(shouldBranchWhenNonZero ? "        if (chaos_condition != static_cast<CHAOS_IL2CPP_INTPTR>(0))" : "        if (chaos_condition == static_cast<CHAOS_IL2CPP_INTPTR>(0))");
		builder.AppendLine("        {");
		EmitDispatchTransfer(builder, requiredBranchTarget, "            ");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
		AppendDispatchNext(builder, nextOffset, instruction.Op, "    ");
	}

	private static void EmitSwitch(StringBuilder builder, AotCoreIrInstructionArtifact instruction, int? nextOffset, IReadOnlySet<int> offsets)
	{
		if (!nextOffset.HasValue)
		{
			throw new InvalidOperationException("branch opcode '" + instruction.Op + "' cannot terminate the method");
		}
		IReadOnlyList<int> requiredSwitchTargets = GetRequiredSwitchTargets(instruction, offsets);
		builder.AppendLine("    {");
		builder.AppendLine("        const auto chaos_switch_value = static_cast<CHAOS_IL2CPP_INT32>(chaos_eval_stack[--chaos_stack_top]);");
		StringBuilder stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler;
		if (requiredSwitchTargets.Count == 0)
		{
			EmitDispatchTransfer(builder, nextOffset.Value, "        ");
			builder.AppendLine("    }");
			return;
		}
		builder.AppendLine("        switch (chaos_switch_value)");
		builder.AppendLine("        {");
		for (int i = 0; i < requiredSwitchTargets.Count; i++)
		{
			stringBuilder = builder;
			StringBuilder stringBuilder3 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(18, 1, stringBuilder);
			handler.AppendLiteral("            case ");
			handler.AppendFormatted(i);
			handler.AppendLiteral(":");
			stringBuilder3.AppendLine(ref handler);
			EmitDispatchTransfer(builder, requiredSwitchTargets[i], "                ");
		}
		builder.AppendLine("            default:");
		EmitDispatchTransfer(builder, nextOffset.Value, "                ");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
	}

	private static void EmitComparisonResult(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string operandType, string comparisonOperator, int? nextOffset, string op)
	{
		RequireInt32IntegralResultType(instruction);
		builder.AppendLine("    {");
		StringBuilder stringBuilder = builder;
		StringBuilder stringBuilder2 = stringBuilder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(84, 1, stringBuilder);
		handler.AppendLiteral("        const auto chaos_right = static_cast<");
		handler.AppendFormatted(operandType);
		handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder2.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder3 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(83, 1, stringBuilder);
		handler.AppendLiteral("        const auto chaos_left = static_cast<");
		handler.AppendFormatted(operandType);
		handler.AppendLiteral(">(chaos_eval_stack[--chaos_stack_top]);");
		stringBuilder3.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder4 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(106, 1, stringBuilder);
		handler.AppendLiteral("        chaos_eval_stack[chaos_stack_top++] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_left ");
		handler.AppendFormatted(comparisonOperator);
		handler.AppendLiteral(" chaos_right ? 1 : 0);");
		stringBuilder4.AppendLine(ref handler);
		builder.AppendLine("    }");
		AppendDispatchNext(builder, nextOffset, op, "    ");
	}

	private static void AppendGotoNext(StringBuilder builder, int? nextOffset, string op)
	{
		AppendDispatchNext(builder, nextOffset, op, "    ");
	}

	private void EmitInstructionRange(StringBuilder builder, AotCoreIrMethodArtifact method, IReadOnlyList<AotCoreIrInstructionArtifact> instructions, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets, IReadOnlySet<int>? exitTargets = null)
	{
		if (instructions.Count == 0)
		{
			return;
		}

		builder.AppendLine("    {");
		builder.AppendLine("        bool chaos_dispatch_completed = false;");
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(18, 1, builder);
		handler.AppendLiteral("        int chaos_pc = ");
		handler.AppendFormatted(GetRequiredIlOffset(instructions[0]));
		handler.AppendLiteral(";");
		builder.AppendLine(ref handler);
		builder.AppendLine("        for (;;)");
		builder.AppendLine("        {");
		builder.AppendLine("            chaos_safepoint_poll();");
		builder.AppendLine("            switch (chaos_pc)");
		builder.AppendLine("            {");
		var previousDispatchExitTargets = _dispatchExitTargets;
		_dispatchExitTargets = exitTargets != null ? new HashSet<int>(exitTargets) : null;
		try
		{
			foreach (AotCoreIrInstructionArtifact instruction in instructions)
			{
				int requiredIlOffset = GetRequiredIlOffset(instruction);
				builder.AppendLine($"                case {requiredIlOffset}:");
				builder.AppendLine("                {");
				EmitInstruction(builder, method, instruction, nextOffsetsByIlOffset[requiredIlOffset], offsets);
				builder.AppendLine("                    break;");
				builder.AppendLine("                }");
				builder.AppendLine();
			}
		}
		finally
		{
			_dispatchExitTargets = previousDispatchExitTargets;
		}
		builder.AppendLine("                default:");
		builder.AppendLine("                {");
		builder.AppendLine("                    CHAOS_IL2CPP_ABORT();");
		builder.AppendLine("                }");
		builder.AppendLine("            }");
		builder.AppendLine("            if (chaos_dispatch_completed)");
		builder.AppendLine("            {");
		builder.AppendLine("                break;");
		builder.AppendLine("            }");
		builder.AppendLine("            continue;  // dispatch not completed, loop back;");
		builder.AppendLine("        }");
		builder.AppendLine("    }");
	}

	private void EmitStructuredInstructionRange(StringBuilder builder, AotCoreIrMethodArtifact method, IReadOnlyList<AotCoreIrInstructionArtifact> instructions, IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset, IReadOnlySet<int> offsets)
	{
		EmitViaStructuredIR(builder, method, instructions, nextOffsetsByIlOffset, offsets);
	}


    private static IReadOnlyList<AotCoreIrAbiSlotArtifact> ResolveComMethodParameterAbis(AotCoreIrInstructionArtifact instruction)
    {
        // COM parameter ABI resolution is not yet implemented.
        return System.Array.Empty<AotCoreIrAbiSlotArtifact>();
    }

    private static AotCoreIrAbiSlotArtifact ResolveComMethodReturnAbi(AotCoreIrInstructionArtifact instruction)
    {
        return new AotCoreIrAbiSlotArtifact { CarrierKindCode = default, TypeShape = default };
    }

}
