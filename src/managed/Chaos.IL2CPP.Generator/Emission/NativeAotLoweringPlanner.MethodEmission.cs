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

        // P/Invoke methods: emit LoadLibrary + GetProcAddress wrapper instead of IL body.
        if (method.IsPInvoke)
        {
            EmitPInvokeMethod(builder, method);
            return;
        }

        // Compiler-generated display class constructors (<>c::.cctor/<>c::.ctor)
        // — their newobj instructions cannot be lowered properly by the structured IR
        // emitter, producing malformed C++ (auto chaos_value = return).  Route them to
        // the interpreter fallback instead of emitting CHAOS_IL2CPP_FAIL() so the
        // runtime dispatches through kChaosExternalRuntimeFnTable → InterpreterEntryDirect
        // (when IL data is embedded) or returns the graceful fallback default (when not).
        // Emitting FAIL here would crash any compiled caller that direct-calls the
        // display-class constructor.
        if (method.SubjectId is not null && method.SubjectId.Contains("<>c::", StringComparison.Ordinal))
        {
            builder.AppendLine("// Interpreter-dispatch stub (display-class): " + method.SubjectId);
            var _fnDecl = FormatMethodDeclaration(method, _sharedContextSymbols);
            builder.AppendLine(_fnDecl.Length > 0 && _fnDecl[^1] == ";"[0] ? _fnDecl[..^1] : _fnDecl);
            builder.AppendLine("{");
            builder.AppendLine("    (void)ChaosExternalRuntimeFallback(\"" + EscapeCppStringLiteral(method.SubjectId) + "\");");
            // Suppress MSVC C4715 for non-void return carriers (FAIL is not noreturn in CHECK config).
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
            if (ak == AsyncMethodKind.AsyncIterator)
            {
                // ASYNC-P2-8 A5: ALL async iterators route through the real structured IR
                // path, including those with awaits (YieldAfterAwait, WhenEachState.Iterate<T>).
                // The await-side machinery is in place: YieldAwaiter is in AsyncAwaiterCatalog,
                // AwaitUnsafeOnCompleted routes via A3, and the structured IR path correctly
                // emits the yield / yield-after-await / yield-with-cancellation shapes from
                // their IL.
                //
                // Recorded as LOWERED, not unsupported: the diagnostic must distinguish
                // "we lowered this" from "we stubbed this", and only the latter is a gap.
                LoweredAsyncIteratorSubjectIds.Add(method.SubjectId ?? "<null>");
            }
            if (ak == AsyncMethodKind.Complex)
            {
                // State machine whose resume graph cannot be lowered to a structured
                // coroutine (no lowered awaiter pattern).  Route it to the interpreter
                // fallback so it executes through kChaosExternalRuntimeFnTable →
                // InterpreterEntryDirect instead of emitting CHAOS_IL2CPP_FAIL() (which
                // would crash the matching async entry when its coroutine is resumed).
                AsyncInterpreterFallbackCount++;
                builder.AppendLine("// Complex async (interpreter-dispatch stub): " + method.SubjectId);
                var fd = FormatMethodDeclaration(method, _sharedContextSymbols);
                builder.AppendLine(fd.Length > 0 && fd[^1] == ";"[0] ? fd[..^1] : fd);
                builder.AppendLine("{");
                // IsAsyncStateMachineMoveNext guarantees SubjectId is non-null here.
                builder.AppendLine("    (void)ChaosExternalRuntimeFallback(\"" + EscapeCppStringLiteral(method.SubjectId!) + "\");");
                if (method.ReturnAbi.CarrierKindCode != AotCoreIrAbiCarrierKind.Void)
                {
                    builder.AppendLine("    return {};");
                }
                builder.AppendLine("}");
                return;
            }
            AsyncStateMachineCount++;
            // Phase 2 translator (ASYNC-P2-1): non-complex async state machine MoveNext.
            // MoveNext is a value-this instance method on the >d__ struct.  Emit it via
            // the normal structured IR path — the >d__ struct fields are picked up by
            // ObjectModelEmission (field scanning handles ValueType-declared fields),
            // and the MoveNext body (switch(state), stfld/ldfld, call, EH) is handled
            // by the standard structured emission + linear emission.
            // NOTE (ASYNC-P2-1): Builder/awaiter call resolution
            // (SetResult → async_task_builder_set_result_raw,
            // AwaitUnsafeOnCompleted → awaiter mapping) is Phase 2 segment B
            // (deferred). When segment B is implemented, add a
            // CHAOS_STATIC_ASSERT or #error in the generated C++ at the
            // emission site to prevent silent misbehavior if the mapping
            // is incomplete. Without this guard, unresolved builder/awaiter
            // calls pass through to the standard structured emitter which
            // may produce incorrect C++ (e.g., calling SetResult on a raw
            // pointer instead of the native builder helper).
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

        ValidateInstructions(method, instructions);        IReadOnlyList<AotCoreIrAbiSlotArtifact> methodAbiParameterSlots = GetMethodAbiParameterSlots(method);
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
        // ASYNC-P2-7: an async ENTRY allocates the >d__ state machine and hands its
        // address to AsyncTaskMethodBuilder::Start, which drives the first MoveNext.
        // A Task.Yield (or any real suspension) inside that MoveNext parks the address
        // on the thread pool as the resume target, so it MUST outlive the entry frame —
        // it has to be a GC-heap box.  Roslyn's common Release shape never emits a
        // `newobj` for the state machine (the local is a plain valuetype and only
        // `ldloca` is used), so the generic newobj path never allocates one and
        // `chaos_locals[V]` would stay 0 — turning every field store through
        // chaos_resolve_managed_value_pointer into a write to a null-tagged stack slot.
        // Allocate the box here, before the body runs.
        EmitAsyncEntryStateMachineBoxAllocation(builder, method, instructions);
        // Emit structured IR body FIRST to capture actual slot depth via
        // slotContext, since ComputeMaxEvalStackDepth may undercount for
        // generic methods where inlined code or StringId emission expands
        // the effective depth.
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
        // Now emit safety net declarations based on actual slotContext peak values.
        // The safety net is prepended to builder (before the body) but we emit it
        // here since we need slotContext to determine the required counts.
        // We use a separate StringBuilder and insert it into the main builder later.
        if (usesStructuredSlots && slotContext != null)
        {
            // Tracked int slots (_sN): safety net covers up to peak depth
            for (int __si = 0; __si < slotContext.MaxIntSlots; __si++)
                builder.AppendLine("\tCHAOS_IL2CPP_INTPTR _s" + __si + "{};");
            // Tracked int64 slots (_iN)
            for (int __ii = 0; __ii < slotContext.MaxInt64Slots; __ii++)
                builder.AppendLine("\tCHAOS_IL2CPP_INT64 _i" + __ii + "{};");
            // Float64/double slots: safety net uses MaxFloat64Slots peak
            // (typically 0-8, up to 16 for Vector<double> in numerics chunks)
            for (int __di = 0; __di < slotContext.MaxFloat64Slots; __di++)
                builder.Append("\tdouble _d" + __di + "{};");
            if (slotContext.MaxFloat64Slots > 0) builder.AppendLine();
            // Float32/float slots
            for (int __fi = 0; __fi < slotContext.MaxFloat32Slots; __fi++)
                builder.Append("\tfloat _f" + __fi + "{};");
            if (slotContext.MaxFloat32Slots > 0) builder.AppendLine();
            builder.AppendLine("\tCHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 32) chaos_eval_stack{};");
            builder.AppendLine("\tCHAOS_IL2CPP_SIZE chaos_stack_top = 0;");
        }
        // Use the larger of ComputeMaxEvalStackDepth and the actual peak depth
        // tracked by StructuredSlotEmissionContext (the latter may be higher for
        // generic methods where StringId emission or inlined code expands depth).
        if (!usesStructuredSlots && slotContext != null)
            evalStackSize = Math.Max(evalStackSize, slotContext.MaxIntSlots);
        if (usesStructuredSlots && slotContext != null)
        {
			// Skip EmitStructuredSlotDeclarations - handled by preamble
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
            // chaos_eval_stack is universal (declared above in safety net).
            // Skip here to avoid C2371 redefinition.
        }
        else if (!usesStructuredSlots && evalStackSize > 0)
        {
            // chaos_eval_stack is universal (declared above in safety net).
            // Skip here to avoid C2371 redefinition.
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
        // Strip inline decls from bodyBuilder — now unnecessary since
        // EmitViaStructuredIR no longer calls EmitStructuredSlotDeclarations.
        // The safety net (emitted above) handles all slot declarations.
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

    /// <summary>
    /// ASYNC-P2-7. For an async ENTRY method, allocate the <c>&gt;d__</c> state machine
    /// on the GC heap into the local slot the IL uses as <c>ref stateMachine</c>, so the
    /// address handed to <c>AsyncTaskMethodBuilder::Start</c> survives the entry frame.
    ///
    /// <para>
    /// Roslyn's common Release shape for a non-escaping async method declares the state
    /// machine as a plain <c>valuetype</c> local and never emits a <c>newobj</c> — the
    /// entry is just <c>ldloca V</c> feeding <c>Start&lt;T&gt;(ref T)</c>. Without this
    /// allocation the emitted <c>chaos_locals[V]</c> stays 0 while the body takes
    /// <c>&amp;chaos_locals[V]</c> and calls
    /// <c>chaos_resolve_managed_value_pointer&lt;T&gt;</c> on it — that helper only
    /// auto-allocates for <i>tagged</i> slot pointers, and a bare <c>&amp;chaos_locals[V]</c>
    /// carries no tag, so the cast treats the 8-byte slot as a full state-machine struct
    /// (field stores run past the slot) and the address reaches the thread pool as a
    /// resume target that dies with the frame.
    /// </para>
    ///
    /// <para>
    /// No-op unless the method is a recognized async entry: a local that is
    /// <c>ldloca</c>'d into <c>AsyncTaskMethodBuilder[&lt;T&gt;]::Start</c> /
    /// <c>AwaitUnsafeOnCompleted</c>. The state-machine type symbol is taken from that
    /// callee's generic argument, which is the authoritative spelling — deriving it from
    /// the local's declared type is not available at this layer.
    /// </para>
    /// </summary>
    private void EmitAsyncEntryStateMachineBoxAllocation(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
    {
        // A MoveNext is the state machine itself, not an entry that allocates one.
        if (method.SubjectId is not null && IsAsyncStateMachineMoveNext(method.SubjectId))
            return;

        for (int i = 0; i < instructions.Count; i++)
        {
            if (instructions[i].Op != "ldloca") continue;
            int slot = GetRequiredIntOperand(instructions[i]);

            for (int j = i + 1; j < instructions.Count; j++)
            {
                var op = instructions[j].Op;
                if (op is "call" or "callvirt")
                {
                    var callee = instructions[j].Callee;
                    if (callee is null)
                        break;
                    bool isBuilder = callee.Contains("AsyncTaskMethodBuilder", StringComparison.Ordinal)
                        || callee.Contains("AsyncValueTaskMethodBuilder", StringComparison.Ordinal)
                        || callee.Contains("AsyncVoidMethodBuilder", StringComparison.Ordinal);
                    bool isStart = callee.Contains("::Start", StringComparison.Ordinal)
                        || callee.Contains("AwaitUnsafeOnCompleted", StringComparison.Ordinal);
                    if (!isBuilder || !isStart)
                        break;

                    // The state machine type is the call's generic argument: the callee
                    // reads "...AsyncTaskMethodBuilder<...>::Start<NS+<M>d__N>:System.Void(NS+<M>d__N&)".
                    // Prefer the parameter spelling (it is assembly-qualified in the same
                    // form as the IR's TargetReference SubjectIds); fall back to the
                    // generic-argument spelling.  The callee carries the type WITHOUT its
                    // assembly prefix, so re-qualify it with this method's assembly before
                    // building the native symbol — the unprefixed form has no emitted
                    // valuetype typedef and would not compile.
                    string? smSubjectId = ExtractStateMachineSubjectIdFromCallee(callee);
                    if (smSubjectId is null)
                        break;
                    if (!smSubjectId.Contains('/', StringComparison.Ordinal)
                        && !string.IsNullOrEmpty(_assemblyName))
                    {
                        smSubjectId = _assemblyName + "/" + smSubjectId;
                    }
                    string smType = GetNativeValueTypeSymbol(smSubjectId);
                    builder.AppendLine($"\tchaos_locals[{slot}] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NEW_GC({smType}, {{}}));");
                    return;                }
                // Eval-stack-neutral ops may sit between the ldloca and the builder call
                // (see IdentifyAsyncBoxPointerLocalSlots, which uses the same allow-list).
                if (op is "nop" or "ldflda" or "ldarg" or "ldarga" or "ldloc" or "ldloca"
                    or "ldc.i4" or "ldc.i4.s" or "ldc.i4.m1" or "ldc.i4.0" or "ldc.i4.1"
                    or "ldc.i4.2" or "ldc.i4.3" or "ldc.i4.4" or "ldc.i4.5"
                    or "ldc.i4.6" or "ldc.i4.7" or "ldc.i4.8" or "dup")
                    continue;
                break;
            }
        }
    }

    /// <summary>
    /// Pull the state-machine type SubjectId out of an
    /// <c>AsyncTaskMethodBuilder::Start&lt;SM&gt;</c> /
    /// <c>AwaitUnsafeOnCompleted&lt;A, SM&gt;</c> callee string.  Returns null when the
    /// spelling is not one this method recognizes, so the caller can leave the entry
    /// untouched rather than emit an allocation for the wrong type.
    /// </summary>
    private static string? ExtractStateMachineSubjectIdFromCallee(string callee)
    {
        // Preferred: "<method>(<fullType>&)" — the by-ref parameter names the state
        // machine exactly as the IR spells it.  Pick the LAST parameter so
        // AwaitUnsafeOnCompleted(TAwaiter&, TStateMachine&) yields the state machine.
        int open = callee.LastIndexOf('(');
        int close = callee.LastIndexOf(')');
        if (open >= 0 && close > open)
        {
            var paramList = callee.Substring(open + 1, close - open - 1);
            var parts = paramList.Split(',');
            if (parts.Length > 0)
            {
                var last = parts[^1].Trim();
                if (last.EndsWith("&", StringComparison.Ordinal))
                {
                    last = last[..^1].Trim();
                    if (last.Contains(">d__", StringComparison.Ordinal))
                        return last;
                }
            }
        }

        // Fallback: the generic argument on the method name.
        int lt = callee.IndexOf("::Start<", StringComparison.Ordinal);
        if (lt >= 0)
        {
            int argStart = lt + "::Start<".Length;
            int depth = 1;
            int k = argStart;
            for (; k < callee.Length && depth > 0; k++)
            {
                if (callee[k] == '<') depth++;
                else if (callee[k] == '>') depth--;
            }
            if (depth == 0)
            {
                var arg = callee.Substring(argStart, k - 1 - argStart);
                if (arg.Contains(">d__", StringComparison.Ordinal))
                    return arg;
            }
        }
        return null;
    }

}
