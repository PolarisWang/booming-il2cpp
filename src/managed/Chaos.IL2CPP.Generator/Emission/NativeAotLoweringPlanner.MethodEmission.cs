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

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
	private static string FormatMethodDeclaration(AotCoreIrMethodArtifact method,
        IReadOnlySet<string>? sharedContextSymbols = null)
	{
		return FormatMethodDeclaration(method.NativeSymbol, method.ReturnAbi, GetMethodAbiParameterSlots(method),
            sharedContextSymbols?.Contains(method.NativeSymbol) == true);
	}

	private static string FormatMethodDeclaration(string symbol, AotCoreIrAbiSlotArtifact returnAbi,
        IReadOnlyList<AotCoreIrAbiSlotArtifact> parameterAbis,
        bool needsGenericContext = false)
	{
        var paramSig = FormatAbiSlotParameterSignature(parameterAbis);
        // C2860 on MSVC: 'void' cannot appear as a function parameter except for '(void)'.
        // When there are no ABI slots but a generic context is needed, drop 'void' entirely.
        if (needsGenericContext)
        {
            paramSig = string.IsNullOrEmpty(paramSig) || paramSig == "void" || paramSig == "void"
                ? "CHAOS_IL2CPP_INTPTR chaos_generic_context"
                : paramSig + ", CHAOS_IL2CPP_INTPTR chaos_generic_context";
        }
		return $"extern \"C\" {MapAbiSlotReturnType(returnAbi)} {symbol}({paramSig});";
	}

	private static string? TryGetInstantiationStubSymbol(AotCoreIrMethodArtifact method)
	{
		if (method.InstantiationStubId is null)
		{
			return null;
		}

		return ManagedNaming.CreateInstantiationStubSymbol(method.InstantiationStubId);
	}

	private static IReadOnlyList<string> BuildMethodDeclarations(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        IReadOnlySet<string>? sharedContextSymbols = null,
        IReadOnlyDictionary<string, bool>? stubNeedsContext = null)
	{
		var declarations = new List<string>();
		var emittedStubSymbols = new HashSet<string>(StringComparer.Ordinal);
		var emittedSymbols = new HashSet<string>(StringComparer.Ordinal);
		foreach (AotCoreIrMethodArtifact reachableMethod in reachableMethods)
		{
			// Deduplicate by native symbol to avoid C2733 (extern "C" cannot be overloaded)
			if (!emittedSymbols.Add(reachableMethod.NativeSymbol))
				continue;

			declarations.Add(FormatMethodDeclaration(reachableMethod, sharedContextSymbols));
			string? text = TryGetInstantiationStubSymbol(reachableMethod);
			if (!string.IsNullOrEmpty(text) && emittedStubSymbols.Add(text))
			{
				bool needsContext = stubNeedsContext is not null
		    ? stubNeedsContext.TryGetValue(text, out bool nc) && nc
		    : sharedContextSymbols?.Contains(reachableMethod.NativeSymbol) == true;
				declarations.Add(FormatMethodDeclaration(text, reachableMethod.ReturnAbi,
                    GetMethodAbiParameterSlots(reachableMethod),
                    needsContext));
			}
		}
		return declarations;
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

		// Determine if this stub needs the chaos_generic_context parameter.
		// Uses the pre-computed _stubNeedsContext map (union semantics) to
		// match the header declaration and avoid C2733.
		bool needsContext = _stubNeedsContext.TryGetValue(text, out bool nc) && nc;

		string paramSig = FormatAbiSlotParameterSignature(methodAbiParameterSlots);
		if (needsContext)
		{
			paramSig = string.IsNullOrEmpty(paramSig) || paramSig == "void"
				? "CHAOS_IL2CPP_INTPTR chaos_generic_context"
				: paramSig + ", CHAOS_IL2CPP_INTPTR chaos_generic_context";
		}

		builder.AppendLine();
		builder.AppendLine("// Generic instantiation stub: " + ManagedNaming.GetMethodSubjectIdDisplayString(method.SubjectId));
		builder.AppendLine(FormatGenericExecutionAuthorityComment(
			method.OpenDefinitionSubjectId,
			method.SharedGenericBodyId,
			method.InstantiationStubId,
			method.RuntimeGenericContext));
		builder.AppendLine($"extern \"C\" {MapAbiSlotReturnType(method.ReturnAbi)} {text}({paramSig})");
		builder.AppendLine("{");
		var argNames = new string[methodAbiParameterSlots.Count];
			for (int i = 0; i < methodAbiParameterSlots.Count; i++)
				argNames[i] = "chaos_fn_arg_" + i.ToString();
			string text2 = string.Join(", ", argNames);
		// Build forwarding argument list, appending chaos_generic_context if needed.
		string forwardedArgs = text2;
		if (needsContext)
		{
			forwardedArgs = string.IsNullOrEmpty(text2)
				? "chaos_generic_context"
				: text2 + ", chaos_generic_context";
		}
		if (method.ReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.Void)
		{
			builder.AppendLine(string.IsNullOrEmpty(forwardedArgs)
				? $"    {targetSymbol}();"
				: $"    {targetSymbol}({forwardedArgs});");
		}
		else
		{
			builder.AppendLine(string.IsNullOrEmpty(forwardedArgs)
				? $"    return {targetSymbol}();"
				: $"    return {targetSymbol}({forwardedArgs});");
		}
		builder.AppendLine("}");
	}

	private void EmitManagedMethod(StringBuilder builder, AotCoreIrMethodArtifact method)
	{
		ValidateMethod(method);
		_linearScratchCounter = 0;
		_nextInlineId = 0;
		_preTryFoldInitializers = null;  // reset per-method

		// P/Invoke methods: emit LoadLibrary + GetProcAddress wrapper instead of IL body.
		if (method.IsPInvoke)
		{
			EmitPInvokeMethod(builder, method);
			return;
		}

		// Skip compiler-generated display class constructors (<>c::.cctor/<>c::.ctor)
		// — their newobj instructions cannot be lowered properly by the structured IR
		// emitter, producing malformed C++ (auto chaos_value = return).
		if (method.SubjectId is not null && method.SubjectId.Contains("<>c::", StringComparison.Ordinal))
		{
			builder.AppendLine("// AOT-unreachable stub: " + method.SubjectId);
			var _fnDecl = FormatMethodDeclaration(method, _sharedContextSymbols);
			builder.AppendLine(_fnDecl.Length > 0 && _fnDecl[^1] == ";"[0] ? _fnDecl[..^1] : _fnDecl);
			builder.AppendLine("{");
			builder.AppendLine("    CHAOS_IL2CPP_FAIL();");
			builder.AppendLine("}");
			return;
		}

		// Cross-assembly method filter: if the method's declaring assembly does NOT
		// match _assemblyName or a known test/subject wrapper assembly, emit a simple
		// return stub instead of the full function body.
		if (method.SubjectId is not null && !string.IsNullOrEmpty(_assemblyName))
		{
			int slashIdx = method.SubjectId.IndexOf('/');
			if (slashIdx > 0)
			{
				string methodAssembly = method.SubjectId.Substring(0, slashIdx);
				if (!string.Equals(methodAssembly, _assemblyName, StringComparison.Ordinal) &&
					!string.Equals(methodAssembly, "CombinedSubjects", StringComparison.Ordinal) &&
					!string.Equals(methodAssembly, "Chaos.TestFramework.Sdk", StringComparison.Ordinal))
				{
					builder.AppendLine("// Cross-assembly stub: " + method.SubjectId);
					var _fnDecl = FormatMethodDeclaration(method, _sharedContextSymbols);
					builder.AppendLine(_fnDecl.Length > 0 && _fnDecl[^1] == ";"[0] ? _fnDecl[..^1] : _fnDecl);
					builder.AppendLine("{");
					var _retType = MapAbiSlotReturnType(method.ReturnAbi);
					if (!string.IsNullOrEmpty(_retType) && _retType != "void")
						builder.AppendLine("    return {};");
					builder.AppendLine("}");
					return;
				}
			}
		}

		// F1 async detect
		if (IsAsyncStateMachineMoveNext(method.SubjectId))
		{
			AsyncMethodCount++;
			var ak = ClassifyAsyncMethod(method);
			if (ak == AsyncMethodKind.Complex) { AsyncInterpreterFallbackCount++; builder.AppendLine("// Complex async"); var fd = FormatMethodDeclaration(method, _sharedContextSymbols); builder.AppendLine(fd.Length > 0 && fd[^1] == ";"[0] ? fd[..^1] : fd); builder.AppendLine("{ CHAOS_IL2CPP_FAIL(); }"); return; }
			AsyncCoroutineMethodCount++;
			var abody = BuildAsyncStructuredBody(method);
			var uid = GetAsyncUid(method);
			var hr = ak == AsyncMethodKind.AsyncTaskOfT || ak == AsyncMethodKind.AsyncValueTaskOfT;
			builder.Append(GenPromise(uid, hr));
			builder.Append(GenCoro(uid, hr, method.SubjectId ?? "", abody ?? new IRSequence(new List<StructuredIRNode>())));
			return;
		}
		IReadOnlyList<AotCoreIrInstructionArtifact> instructions = method.Instructions;

		// Handle 0-instruction subject methods: emit simple return instead of throwing.
		if (instructions.Count == 0)
		{
		    var returnType = MapAbiSlotReturnType(method.ReturnAbi);
		    builder.AppendLine("// Managed method: " + ManagedNaming.GetMethodSubjectIdDisplayString(method.SubjectId));
		    builder.AppendLine(FormatMethodDeclaration(method, _sharedContextSymbols).TrimEnd(';'));
		    builder.AppendLine("{");
		    if (!string.IsNullOrEmpty(returnType) && returnType != "void")
		        builder.AppendLine("    return {};");
		    builder.AppendLine("}");
		    return;
		}

		ValidateInstructions(method, instructions);
		IReadOnlyList<AotCoreIrAbiSlotArtifact> methodAbiParameterSlots = GetMethodAbiParameterSlots(method);
		HashSet<int> offsets = new HashSet<int>(instructions.Count);
			for (int idx = 0; idx < instructions.Count; idx++)
			{
				offsets.Add(GetRequiredIlOffset(instructions[idx]));
				if (instructions[idx].Op is "br" or "leave" or "brtrue" or "brfalse"
					or "beq" or "bne.un" or "bge" or "bge.un" or "bgt" or "bgt.un"
					or "ble" or "ble.un" or "blt" or "blt.un")
				{
					offsets.Add(GetRequiredIntOperand(instructions[idx]));
				}
			}
		bool usesStructuredSlots = TryBuildStructuredMethodBody(method, instructions, offsets, out var body, out int structuredSlotCount);
		int evalStackSize = usesStructuredSlots ? 0 : Math.Max(ComputeMaxEvalStackDepth(instructions), 1);
		builder.AppendLine("// Managed method: " + ManagedNaming.GetMethodSubjectIdDisplayString(method.SubjectId));
		var fnDecl = FormatMethodDeclaration(method, _sharedContextSymbols);
		// Strip trailing semicolon from FormatMethodDeclaration for function definition header.
		builder.AppendLine(fnDecl.Length > 0 && fnDecl[^1] == ";"[0] ? fnDecl[..^1] : fnDecl);
		builder.AppendLine("{");
		StringBuilder stringBuilder = builder;
		StringBuilder.AppendInterpolatedStringHandler handler = new StringBuilder.AppendInterpolatedStringHandler(45, 1, stringBuilder);
		handler.AppendLiteral("    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, ");
		handler.AppendFormatted(Math.Max(methodAbiParameterSlots.Count, 1));
		handler.AppendLiteral(") chaos_args{};");
		stringBuilder.AppendLine(ref handler);
		stringBuilder = builder;
		StringBuilder stringBuilder5 = stringBuilder;
		handler = new StringBuilder.AppendInterpolatedStringHandler(47, 1, stringBuilder);
		handler.AppendLiteral("    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, ");
		handler.AppendFormatted(Math.Max(method.LocalCount, 1));
		handler.AppendLiteral(") chaos_locals{};");
		stringBuilder5.AppendLine(ref handler);
		EmitAbiArgumentInitialization(builder, methodAbiParameterSlots);
		EmitStaticInitializationPrologue(builder, method);
		// Emit structured IR body first to capture actual slot depth,
		// since ComputeMaxEvalStackDepth may undercount for generic methods
		// where inlined code or StringId emission expands the effective depth.
		var bodyBuilder = new System.Text.StringBuilder();
		_currentMethodNativeSymbol = method.NativeSymbol;
		_currentMethodArtifact = method;
		StructuredSlotEmissionContext? slotContext = null;
		try
		{
			slotContext = EmitViaStructuredIR(bodyBuilder, method, instructions, offsets, body);
		}
		finally
		{
			_currentMethodNativeSymbol = null;
			_currentMethodArtifact = null;
		}
		// Use the larger of ComputeMaxEvalStackDepth and the actual peak depth
		// tracked by StructuredSlotEmissionContext (the latter may be higher for
		// generic methods where StringId emission or inlined code expands depth).
		if (!usesStructuredSlots && slotContext != null)
			evalStackSize = Math.Max(evalStackSize, slotContext.MaxIntSlots);
		if (usesStructuredSlots && slotContext != null)
		{
			EmitStructuredSlotDeclarations(builder, slotContext.MaxIntSlots + 2, slotContext.MaxFloat64Slots, slotContext.MaxFloat32Slots, slotContext.MaxInt64Slots + 2, slotContext.MaxWideSlots, "	");
			if (slotContext.FloatLocalSlots is { Count: > 0 })
			{
				foreach (var (slot, type) in slotContext.FloatLocalSlots.OrderBy(kv => kv.Key))
				{
					string varType = type switch
					{
						SlotType.Float64 => "double",
						SlotType.Float32 => "float",
						_ => "CHAOS_IL2CPP_INTPTR",
					};
					builder.AppendLine($"	{varType} chaos_float_local_{slot}{{}};");
				}
			}
			// chaos_eval_stack is needed for EH (finally) condition tracking in ExceptionEmission.cs,
			// even for structured IR methods. It tracks the finally condition state via:
			//   chaos_eval_stack[--chaos_stack_top] = 1;  // finally entry
			//   if (chaos_eval_stack[--chaos_stack_top])   // finally exit condition check
			if (method.ExceptionRegionCount > 0)
			{
				builder.AppendLine("\tCHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 16) chaos_eval_stack{};");
				builder.AppendLine("\tCHAOS_IL2CPP_SIZE chaos_stack_top = 0;");
			}
		}
		else if (!usesStructuredSlots && evalStackSize > 0)
		{
			stringBuilder = builder;
			StringBuilder stringBuilder6 = stringBuilder;
			handler = new StringBuilder.AppendInterpolatedStringHandler(51, 1, stringBuilder);
			handler.AppendLiteral("	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, ");
			handler.AppendFormatted(evalStackSize);
			handler.AppendLiteral(") chaos_eval_stack{};");
			stringBuilder6.AppendLine(ref handler);
			builder.AppendLine("	CHAOS_IL2CPP_SIZE chaos_stack_top = 0;");
		}
		// Pre-try TypeInfo* fold evaluations (outside SEH frame)
		if (_preTryFoldInitializers is { Count: > 0 })
		{
			builder.AppendLine("	// Pre-try TypeInfo* fold evaluations (outside SEH frame)");
			foreach (var (varName, expr) in _preTryFoldInitializers)
				builder.AppendLine($"	const auto {varName} = {expr};");
		}

		builder.Append(bodyBuilder);
			// Safety: close any unmatched { from structured IR lowering (e.g. failed newobj)
			// to prevent C2598/C2601 cascading to subsequent functions.
			int _braceCount = 0;
			for (int _bi = 0; _bi < bodyBuilder.Length; _bi++)
			{
			    if (bodyBuilder[_bi] == '{') _braceCount++;
			    else if (bodyBuilder[_bi] == '}') _braceCount--;
			}
			while (_braceCount > 0)
			{
			    builder.AppendLine("		}");
			    _braceCount--;
			}
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
	bool hasSetLastError = method.ImportSetLastError;
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
			argNames[i] = "chaos_fn_arg_" + i.ToString();
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
		bool isInternal = method.IsInternalLink;
		bool needsGcTransition = !method.IsSuppressGCTransition;
		bool isDeclaringAssemblyKnown = method.DeclaringAssemblyName != null;
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
		if (isInternal)
		{
			builder.AppendLine($"extern \"C\" void {entryPointName}();");
		}
		builder.AppendLine($"extern \"C\" {returnType} {method.NativeSymbol}({parameterSignature})");
		builder.AppendLine("{");
		builder.AppendLine($"    using FnPtr = {fnPtrType};");
		if (isInternal)
		{
			builder.AppendLine($"    static FnPtr s_pinvoke_fn_ = reinterpret_cast<FnPtr>(&{entryPointName});");
		}
		else
		{
			builder.AppendLine("    static void* s_pinvoke_lib_ = nullptr;");
			builder.AppendLine("    static FnPtr s_pinvoke_fn_ = nullptr;");
		}

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

		if (!isInternal)
		{
			builder.AppendLine("    if (s_pinvoke_fn_ == nullptr)");
			builder.AppendLine("    {");
			if (isDeclaringAssemblyKnown)
			{
				builder.AppendLine($"        s_pinvoke_lib_ = ::chaos::il2cpp::runtime_core::TryResolveDllImport(\"{method.DeclaringAssemblyName}\", \"{moduleName}\");");
				builder.AppendLine("        if (s_pinvoke_lib_ == nullptr)");
				builder.AppendLine("        {");
			}
			builder.AppendLine($"        s_pinvoke_lib_ = ::chaos::il2cpp::runtime_core::NativeLibraryLoad(\"{moduleName}\");");
			if (isDeclaringAssemblyKnown)
			{
				builder.AppendLine("        }");
			}
			builder.AppendLine("        if (s_pinvoke_lib_ == nullptr) CHAOS_IL2CPP_FAIL();");
			builder.AppendLine($"        s_pinvoke_fn_ = reinterpret_cast<FnPtr>(");
			builder.AppendLine($"            ::chaos::il2cpp::runtime_core::NativeLibraryGetProcAddress(s_pinvoke_lib_, \"{entryPointName}\"));");
			builder.AppendLine("        if (s_pinvoke_fn_ == nullptr) CHAOS_IL2CPP_FAIL();");
			builder.AppendLine("    }");
		}

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
			if (needsGcTransition)
			{
				builder.AppendLine("    GC_TRANSITION_TO_PREEMPTIVE();");
			}
			if (isVoidLocal && methodAbiParameterSlots.Count == 0)
				builder.AppendLine("    s_pinvoke_fn_();");
			else if (isVoidLocal)
				builder.AppendLine($"    s_pinvoke_fn_({nativeArgList});");
			else if (methodAbiParameterSlots.Count == 0)
				builder.AppendLine("    return s_pinvoke_fn_();");
			else
				builder.AppendLine($"    return s_pinvoke_fn_({nativeArgList});");
			if (needsGcTransition)
			{
				builder.AppendLine("    GC_TRANSITION_TO_COOPERATIVE();");
			}
			builder.AppendLine("}");
			return;

		}

		// Paths 2-4: capture result if non-void.
		bool isNonVoid = method.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void;
		if (needsGcTransition)
		{
			builder.AppendLine("    GC_TRANSITION_TO_PREEMPTIVE();");
		}
		if (isNonVoid)
		{
			builder.AppendLine($"    {returnType} chaos_ret_ = s_pinvoke_fn_({nativeArgList});");
		}
		else
		{
			builder.AppendLine($"    s_pinvoke_fn_({nativeArgList});");
		}
		if (needsGcTransition)
		{
			builder.AppendLine("    GC_TRANSITION_TO_COOPERATIVE();");
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
			// Deduplicate by IL offset instead of throwing. Some CombinedSubjects wrappers
			// (e.g., String::Concat) produce IL with duplicate offsets from overlapping
			// C# compiler ranges — the duplicate is safe to skip.
			// Deduplicate: create a new list skipping duplicates.
			var deduped = new List<AotCoreIrInstructionArtifact>(instructions.Count);
			var seen = new HashSet<int>(instructions.Count);
			foreach (var instr in instructions)
			{
				if (seen.Add(GetRequiredIlOffset(instr)))
					deduped.Add(instr);
				else
					Console.Error.WriteLine($"[IL-DEDUP] {entryMethod.SubjectId}: skip duplicate IL offset {GetRequiredIlOffset(instr)}");
			}
			instructions = deduped;
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
	private const bool IsWindowsTarget = true;

	private static bool IsUnicodeCharSet(int importCharSet)
	{
		if (!IsWindowsTarget && importCharSet == 0x0006)
			return false;
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
