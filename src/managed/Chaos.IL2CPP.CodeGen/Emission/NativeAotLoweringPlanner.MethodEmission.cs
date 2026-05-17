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

		// For shared generic instantiations, the stub forwards to the canonical
		// method's body instead of the per-instantiation body.
		string targetSymbol = ResolveStubTargetNativeSymbol(method);

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
			builder.AppendLine(methodAbiParameterSlots.Count == 0 ? $"    {targetSymbol}();" : $"    {targetSymbol}({text2});");
		}
		else
		{
			builder.AppendLine(methodAbiParameterSlots.Count == 0 ? $"    return {targetSymbol}();" : $"    return {targetSymbol}({text2});");
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
		HashSet<int> offsets = new HashSet<int>(instructions.Count);
			for (int idx = 0; idx < instructions.Count; idx++)
				offsets.Add(GetRequiredIlOffset(instructions[idx]));
		bool usesStructuredSlots = TryBuildStructuredMethodBody(method, instructions, offsets, out var body, out int structuredSlotCount);
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
		if (body is IRFlatRegion)
		{
			// Flat-goto fallback: body uses chaos_eval_stack / chaos_stack_top
			// but caller sees usesStructuredSlots=true (TryBuildStructuredMethodBody
			// returns true for IRFlatRegion) and structuredSlotCount=0, so the
			// downstream branches would skip declarations entirely.  Fix by
			// detecting IRFlatRegion and emitting eval stack unconditionally.
			int flatEvalDepth = Math.Max(ComputeMaxEvalStackDepth(instructions), 1);
			stringBuilder = builder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(51, 1, stringBuilder);
			handler.AppendLiteral("    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, ");
			handler.AppendFormatted(flatEvalDepth);
			handler.AppendLiteral(") chaos_eval_stack{};");
			stringBuilder.AppendLine(ref handler);
			builder.AppendLine("    CHAOS_IL2CPP_SIZE chaos_stack_top = 0;");
		}
		else if (usesStructuredSlots && structuredSlotCount > 0)
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
		EmitViaStructuredIR(builder, method, instructions, offsets, body);
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
		bool hasSafeHandleParams = method.SafeHandleParameterIndices is { Count: > 0 };
t	bool hasSetLastError = method.ImportSetLastError;
		bool needsMarshalling = hasStringParams || hasStringReturn || hasBlittableStructParams || hasSimpleNonBlittableStructParams || hasComplexStructParams || hasSafeHandleParams;
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
		var safeHandleParamSet = hasSafeHandleParams
			? new HashSet<int>(method.SafeHandleParameterIndices!)
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
			else if (safeHandleParamSet.Contains(i))
			{
				nativeArgs[i] = "chaos_handle_" + i;
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
			fnParamTypes[i] = blittableStructParamSet.Contains(i) || simpleNonBlittableSet.Contains(i) || complexStructSet.Contains(i)
				? "CHAOS_IL2CPP_INTPTR"
				: MapAbiSlotReturnType(methodAbiParameterSlots[i]);
		}
		string rawParamTypes = string.Join(", ", fnParamTypes);
		string ccAnnotation = GetCallingConventionAnnotation(method.ImportCallingConvention);
		string fnPtrType = string.IsNullOrEmpty(rawParamTypes)
			? $"{returnType}({ccAnnotation}*)()"
			: $"{returnType}({ccAnnotation}*)({rawParamTypes})";

		bool isUnicodeCharSet = IsUnicodeCharSet(method.ImportCharSet);
		string marshalStringFn = isUnicodeCharSet ? "MarshalStringToCoTaskMemWide" : "MarshalStringToCoTaskMemUtf8";
		string marshalPtrToStringFn = isUnicodeCharSet ? "MarshalPtrToStringWide" : "MarshalPtrToStringUtf8";

		string pinvokeTag = hasStringParams || hasStringReturn
			? (isUnicodeCharSet ? "simple non-blittable (unicode)" : "simple non-blittable")
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

		// Local variables for SafeHandle handle extraction.
		if (hasSafeHandleParams)
		{
			foreach (int idx in method.SafeHandleParameterIndices!)
			{
				builder.AppendLine($"    CHAOS_IL2CPP_INTPTR chaos_handle_{idx} = 0;");
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
		builder.AppendLine("        if (s_pinvoke_lib_ == nullptr) CHAOS_IL2CPP_FAIL();");
		builder.AppendLine($"        s_pinvoke_fn_ = reinterpret_cast<{fnPtrType}>(");
		builder.AppendLine($"            ::chaos::il2cpp::runtime_core::NativeLibraryGetProcAddress(s_pinvoke_lib_, \"{entryPointName}\"));");
		builder.AppendLine("        if (s_pinvoke_fn_ == nullptr) CHAOS_IL2CPP_FAIL();");
		builder.AppendLine("    }");

		// Pre-call: marshal string parameters to native CoTaskMem buffers (UTF-8 or UTF-16 based on CharSet).
		if (hasStringParams)
		{
			builder.AppendLine();
			foreach (int idx in method.StringParameterIndices!)
			{
				builder.AppendLine($"    if (chaos_arg_{idx} != 0)");
				builder.AppendLine("    {");
				builder.AppendLine($"        chaos_marshal_{idx} = reinterpret_cast<void*>(");
				builder.AppendLine($"            ::chaos::il2cpp::runtime_core::{marshalStringFn}(");
				builder.AppendLine($"                chaos_rs_, chaos_ts_, reinterpret_cast<void*>(chaos_arg_{idx})));");
				builder.AppendLine("    }");
			}
		}

		// Pre-call: marshal string fields in non-blittable struct copies (UTF-8 or UTF-16 based on CharSet).
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
					builder.AppendLine($"        auto* chaos_marshal_str_ = ::chaos::il2cpp::runtime_core::{marshalStringFn}(");
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

		// Pre-call: extract handle values from SafeHandle parameters.
		if (hasSafeHandleParams)
		{
			builder.AppendLine();
			foreach (int idx in method.SafeHandleParameterIndices!)
			{
				builder.AppendLine($"    if (chaos_arg_{idx} != 0)");
				builder.AppendLine("    {");
				builder.AppendLine($"        chaos_handle_{idx} = ::chaos::il2cpp::runtime_core::MarshalSafeHandleGetHandle(");
				builder.AppendLine($"            chaos_rs_, chaos_ts_, reinterpret_cast<void*>(chaos_arg_{idx}));");
				builder.AppendLine("    }");
			}
		}

		// Native call — four code paths:
		//   1. Pure blittable (no marshalling): direct call-and-return, early exit.
		//   2. Blittable struct on non-void return: capture result, cleanup, return.
		//   3. Blittable struct on void return: call, cleanup, fall through.
		//   4. Non-blittable (string): existing string marshal paths.
		builder.AppendLine();
		// SetLastError: clear OS error before the native call.
		if (hasSetLastError)
		{
			builder.AppendLine("    ::chaos::il2cpp::runtime_core::ClearOsLastError();");
		}

		if (!hasStringParams && !hasStringReturn && !hasBlittableStructParams && !hasSimpleNonBlittableStructParams && !hasComplexStructParams && !hasSafeHandleParams && !hasSetLastError)
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
		// SetLastError: capture OS error after the native call.
		if (hasSetLastError)
		{
			builder.AppendLine("    ::chaos::il2cpp::runtime_core::SetLastPInvokeError(");
			builder.AppendLine("        ::chaos::il2cpp::runtime_core::GetCurrentThreadState(),");
			builder.AppendLine("        ::chaos::il2cpp::runtime_core::GetOsLastError());");
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

		// String return: convert native char* (UTF-8) or WCHAR* (UTF-16) to managed string.
		if (hasStringReturn)
		{
			if (isUnicodeCharSet)
			{
				// Unicode: cast return to WCHAR*, use wcslen + MarshalWideToString.
				builder.AppendLine("    if (chaos_ret_ != 0)");
				builder.AppendLine("    {");
				builder.AppendLine("        auto* chaos_managed_str_ = ::chaos::il2cpp::runtime_core::MarshalPtrToStringWide(");
				builder.AppendLine("            chaos_rs_, chaos_ts_, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ret_), -1, false);");
				builder.AppendLine("        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_managed_str_);");
				builder.AppendLine("    }");
				builder.AppendLine("    return 0;");
			}
			else
			{
				builder.AppendLine("    if (chaos_ret_ != 0)");
				builder.AppendLine("    {");
				builder.AppendLine("        auto* chaos_managed_str_ = ::chaos::il2cpp::runtime_core::MarshalPtrToStringUtf8(");
				builder.AppendLine("            chaos_rs_, chaos_ts_, reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ret_), -1, false);");
				builder.AppendLine("        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_managed_str_);");
				builder.AppendLine("    }");
				builder.AppendLine("    return 0;");
			}
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
		if (entryMethod.ReturnType is not ("System.Int32" or "System.Void"))
		{
			throw new NotSupportedException("native-aot entry '" + entryMethod.SubjectId + "' must return System.Int32 or System.Void");
		}
	}

	private static void ValidateMethod(AotCoreIrMethodArtifact method)
	{
		if (string.IsNullOrEmpty(method.NativeSymbol))
		{
			throw new InvalidOperationException("native-aot method '" + method.SubjectId + "' is missing native symbol metadata");
		}
		MapAbiSlotReturnType(method.ReturnAbi);
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

	/// <summary>
	/// Maps <see cref="System.Reflection.MethodImportAttributes"/> calling-convention bits
	/// to a C++ calling-convention annotation string for the function pointer type.
	/// On x64 Windows all conventions converge; these annotations matter for x86 correctness.
	/// </summary>
	private static string GetCallingConventionAnnotation(int importCallingConvention)
	{
		return importCallingConvention switch
		{
			0x0100 => "",         // WinApi (platform default)
			0x0200 => "__cdecl ",
			0x0300 => "__stdcall ",
			0x0400 => "__thiscall ",
			0x0500 => "__fastcall ",
			_ => "",              // Unknown/default — no annotation
		};
	}

	/// <summary>
	/// Returns true when the MethodImportAttributes CharSet bits indicate Unicode.
	/// CharSet values: Ansi=0x0002, Unicode=0x0004, Auto=0x0006.
	/// Auto is treated as Unicode (Windows default).
	/// </summary>
	private static bool IsUnicodeCharSet(int importCharSet)
	{
		// Mask 0x0006: 0x0004 = Unicode, 0x0006 = Auto (Unicode on Windows)
		return importCharSet is 0x0004 or 0x0006;
	}

    private IReadOnlyList<AotCoreIrAbiSlotArtifact> ResolveComMethodParameterAbis(AotCoreIrInstructionArtifact instruction)
    {
        // Resolve from method metadata if available.
        string? comCallee = instruction.Callee ?? instruction.TargetReference?.SubjectId;
        if (!string.IsNullOrEmpty(comCallee) && _methodsBySubjectId.TryGetValue(comCallee, out var comMethod))
        {
            return GetMethodAbiParameterSlots(comMethod);
        }
        // Fallback: use instruction-level parameter count with native-int slots.
        return CreateLegacyAbiParameterSlots(GetRequiredTargetParameterCount(instruction));
    }

    private AotCoreIrAbiSlotArtifact ResolveComMethodReturnAbi(AotCoreIrInstructionArtifact instruction)
    {
        // COM methods typically return HRESULT (Int32) when no method metadata available.
        // If metadata is available, use the method's declared return ABI.
        string? comCallee = instruction.Callee ?? instruction.TargetReference?.SubjectId;
        if (!string.IsNullOrEmpty(comCallee) && _methodsBySubjectId.TryGetValue(comCallee, out var comMethod))
        {
            return comMethod.ReturnAbi;
        }
        // Fallback: infer from TargetReturnType or default to Int32.
        string? retType = instruction.TargetReturnType;
        if (!string.IsNullOrEmpty(retType))
        {
            return CreateLegacyReturnAbiSlot(retType);
        }
        return new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
            TypeShape = AotCoreIrTypeShapeKind.ValueType
        };
    }

}
