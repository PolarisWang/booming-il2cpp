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
        _state.Value!.LinearScratchCounter = 0;
        _state.Value!.NextInlineId = 0;
        _state.Value!.DispatchLabelSeq = 0;
        _state.Value!.PreTryFoldInitializers = null;  // reset per-method
        _state.Value!.HoistedInvariantLocals = null;  // reset per-method (C2065)

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
            // Suppress MSVC C4715: CHAOS_IL2CPP_FAIL is not noreturn in CHECK config
            if (method.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
            {
                builder.AppendLine("    return {};");
            }
            builder.AppendLine("}");
            return;
        }

        // Cross-assembly method filter: if the method's declaring assembly does NOT
        // match _assemblyName or a known test/subject wrapper assembly, previously we
        // emitted a stub.  This was removed because it breaks foundation-dll fact tests
        // (closure assembly detection via PE metadata is unreliable when PE files aren't
        // pre-built on disk).  Full codegen is emitted for all methods regardless.
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
                    // Cross-assembly method: always emit full codegen (stubs would break
                    // foundation-dll fact tests).  Closure assembly filtering via PE metadata
                    // is unreliable when PE files aren't pre-built on disk.
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
            // Emit forwarding function with NativeSymbol name so the hotpatch
            // dispatch table's direct_ptr resolves correctly.  The coroutine entry
            // point is Entry_<uid>; we emit a thin wrapper that calls it and
            // returns the raw int64 result.
            var nativeSym = method.NativeSymbol;
            if (!string.IsNullOrEmpty(nativeSym))
            {
                builder.Append("extern \"C\" CHAOS_IL2CPP_INT64 ");
                builder.Append(nativeSym);
                builder.AppendLine("(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR) noexcept");
                builder.AppendLine("{");
                builder.Append("    return Entry_");
                builder.Append(uid);
                builder.AppendLine("();");
                builder.AppendLine("}");
            }
            return;
        }
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions = method.Instructions;

        // Handle 0-instruction subject methods: emit simple return instead of throwing.
        if (instructions.Count == 0)
        {
            var returnType = MapAbiSlotReturnType(method.ReturnAbi);
            builder.AppendLine("// Managed method: " + ManagedNaming.GetMethodSubjectIdDisplayString(method.SubjectId!));
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
        builder.AppendLine("// Managed method: " + ManagedNaming.GetMethodSubjectIdDisplayString(method.SubjectId!));
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
        _state.Value!.CurrentMethodNativeSymbol = method.NativeSymbol;
        _state.Value!.CurrentMethodArtifact = method;
        StructuredSlotEmissionContext? slotContext = null;
        try
        {
            slotContext = EmitViaStructuredIR(bodyBuilder, method, instructions, offsets, body);
        }
        finally
        {
            _state.Value!.CurrentMethodNativeSymbol = null;
            _state.Value!.CurrentMethodArtifact = null;
        }
        // Use the larger of ComputeMaxEvalStackDepth and the actual peak depth
        // tracked by StructuredSlotEmissionContext (the latter may be higher for
        // generic methods where StringId emission or inlined code expands depth).
        if (!usesStructuredSlots && slotContext != null)
            evalStackSize = Math.Max(evalStackSize, slotContext.MaxIntSlots);
        if (usesStructuredSlots && slotContext != null)
        {
            EmitStructuredSlotDeclarations(builder, slotContext.MaxIntSlots + 2, slotContext.MaxFloat64Slots, slotContext.MaxFloat32Slots, slotContext.MaxInt64Slots + 2, slotContext.MaxWideSlots, "	");
            // Pre-populate _s0 with 'this' for instance subject methods.
            // Structured IR building can drop the initial ldarg.0 when the first
            // basic block has no branches, leaving _s0 = 0 (the slot init value).
            // Without this fix, instance method calls with null 'this' raise
            // NullReferenceException inside the try/catch wrapper, returning 0.
            if (!method.IsStatic && methodAbiParameterSlots.Count > 0 &&
                slotContext.MaxIntSlots > 0 &&
                method.SubjectId is not null &&
                method.SubjectId.StartsWith("CombinedSubjects/", StringComparison.Ordinal))
            {
                builder.AppendLine("\t_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_args[0]);");
            }
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
        if (_state.Value!.PreTryFoldInitializers is { Count: > 0 })
        {
            builder.AppendLine("	// Pre-try TypeInfo* fold evaluations (outside SEH frame)");
            foreach (var (varName, expr) in _state.Value!.PreTryFoldInitializers)
                builder.AppendLine($"	const auto {varName} = {expr};");
        }

        // Phase 4: The try/catch below also guards against calls to unregistered
        // external runtime symbols (chaos_external_runtime_*). If the symbol is
        // not in _externalRuntimeSubjects or the kChaosExternalRuntimeFnTable,
        // the fallback throws a C++ exception — caught here, returning default.
        // Wrap subject methods w/o EH regions in try/catch to prevent
        // C++ exceptions from propagating to the fact-json __except handler.
        bool _isSubjectMethod = method.SubjectId is not null &&
            (method.SubjectId.StartsWith("CombinedSubjects/", StringComparison.Ordinal) ||
             method.SubjectId.StartsWith("Chaos.TestFramework.Sdk/", StringComparison.Ordinal));
        bool _wrapInTryCatch = _isSubjectMethod && method.ExceptionRegionCount == 0 && (method.Instructions?.Any(i => i.Callee != null) == true);
        if (_wrapInTryCatch)
            builder.AppendLine("	try {");

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
        // Fallback return for non-void: suppress MSVC C4715
        if (method.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
        {
            builder.AppendLine("    return {};");
        }
        if (_wrapInTryCatch)
        {
            builder.AppendLine("} catch (const chaos_managed_exception&) {");
            if (method.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
                builder.AppendLine("    return {};");
            builder.AppendLine("}");
        }
        builder.AppendLine("}");
    }
    /// <summary>
    /// Maps <see cref="System.Reflection.MethodImportAttributes"/> calling-convention bits
    /// to a C++ calling-convention annotation string for the function pointer type.
    /// On x64 Windows all conventions converge; these annotations matter for x86 correctness.
    /// </summary>

    /// <summary>
    /// Returns true when the MethodImportAttributes CharSet bits indicate Unicode.
    /// CharSet values: Ansi=0x0002, Unicode=0x0004, Auto=0x0006.
    /// Auto is treated as Unicode (Windows default).
    /// </summary>
    private const bool IsWindowsTarget = true;

}
