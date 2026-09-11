using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    partial class RuntimeHelperShapeRegistry
    {
        /// <summary>
        /// Reflection: MethodInfo
        /// </summary>
        private static void RegisterReflectionMethodInfo(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.MethodInfo", "MakeGenericMethod", ["System.Type[]"],
                ShapeKind.SimpleForward, "ChaosReflectionMakeGenericMethod",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.MethodInfo", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Assembly::GetType(System.String,System.Boolean) overload
        /// </summary>
        private static void RegisterAssemblyGetTypeoverload(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.Assembly", "GetType", ["System.String", "System.Boolean"],
                ShapeKind.SimpleForward, "ChaosReflectionGetTypeFromAssemblyBool",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.Assembly", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                    CreateInt32AbiSlot(),
                }), CreateNativeIntAbiSlot("System.Private.CoreLib/System.Type", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0, 1 });

        }

        /// <summary>
        /// Reflection: ParameterInfo
        /// </summary>
        private static void RegisterReflectionParameterInfo(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Reflection.ParameterInfo", "get_Name", [],
                ShapeKind.SimpleForward, "ChaosReflectionGetParameterName",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Reflection.ParameterInfo", AotCoreIrTypeShapeKind.ReferenceType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.String", AotCoreIrTypeShapeKind.ReferenceType),
                new HashSet<int> { 0 });

        }

        /// <summary>
        /// Async: Task.Yield / YieldAwaitable
        /// </summary>
        private static void RegisterAsyncTaskYield(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.Tasks.Task", "Yield", [],
                ShapeKind.SimpleForward, "chaos_async_yield_create",
                Array.Empty<AotCoreIrAbiSlotArtifact>(), CreateNativeIntAbiSlot(),
                EmptyRawArgumentIndices);

            registry.Register("System.Runtime.CompilerServices.YieldAwaitable", "GetAwaiter", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_awaiter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter", "get_IsCompleted", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_is_completed",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter", "GetResult", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_result",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // ── Task.Delay / TaskAwaiter (non-generic Task await path) ──
            // Without these, Task.Delay/Task.GetAwaiter/TaskAwaiter.get_IsCompleted
            // all fell through to ChaosExternalRuntimeFallback → 0, which made
            // `await Task.Delay(n)` hang: GetAwaiter returned 0, so IsCompleted
            // read false, the machine suspended, and AwaitUnsafeOnCompleted saw
            // task_handle==0 and returned without registering a continuation.
            registry.Register("System.Threading.Tasks.Task", "Delay", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosAsyncTaskDelay",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Threading.Tasks.Task", "GetAwaiter", [],
                ShapeKind.SimpleForward, "ChaosAsyncTaskGetAwaiter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Runtime.CompilerServices.TaskAwaiter", "get_IsCompleted", [],
                ShapeKind.SimpleForward, "ChaosAsyncTaskAwaiterGetIsCompleted",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── Task&lt;T&gt; / TaskAwaiter&lt;T&gt; (generic await path) ──
            // `await Task.Delay(n)` lowers against Task&lt;Int32&gt;::GetAwaiter and
            // TaskAwaiter&lt;Int32&gt;::get_IsCompleted / GetResult, not the non-generic
            // overloads.  Use DirectNativeSymbol so the codegen emits a direct call
            // to the native function, avoiding inline-code template issues with
            // parameter naming in the shared header.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.Task",
                MethodName: "GetAwaiter",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    return new GenericShapeResolution("", symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosAsyncTaskGetAwaiter");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.CompilerServices.TaskAwaiter",
                MethodName: "get_IsCompleted",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    return new GenericShapeResolution("", symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosAsyncTaskAwaiterGetIsCompleted");
                }));

            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Runtime.CompilerServices.TaskAwaiter",
                MethodName: "GetResult",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    return new GenericShapeResolution("", symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "ChaosAsyncTaskAwaiterGetResultValue");
                }));

            // ── Non-generic TaskAwaiter.GetResult (void) ──
            // `await someTask` (no result) lowers against the non-generic
            // TaskAwaiter, whose GetResult returns void.  It must still
            // propagate a fault, so it routes to the void-returning helper
            // rather than sharing the value-returning one above.
            registry.Register("System.Runtime.CompilerServices.TaskAwaiter", "GetResult", [],
                ShapeKind.SimpleForward, "ChaosAsyncTaskAwaiterGetResultVoid",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // ── Task.FromResult / FromException / FromCanceled ──
            // Already-completed task factories.  FromResult carries a value;
            // FromException/FromCanceled produce a faulted task whose await
            // throws (see ChaosAsyncTaskAwaiterGetResultValue).
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.Task",
                MethodName: "FromResult",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return async_task_from_result(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "async_task_from_result");
                }));

            registry.Register("System.Threading.Tasks.Task", "FromException",
                ["System.Exception"],
                ShapeKind.SimpleForward, "async_task_from_exception",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── Task.ContinueWith (Phase 2 P2-2 / P2-4) ──
            // `antecedent.ContinueWith(body)` registers `body` to run when the
            // antecedent completes and returns a NEW task carrying the
            // continuation's return value — that return value is what makes
            // ContinueWith chainable rather than fire-and-forget.
            //
            // .NET 8 exposes 20 public ContinueWith overloads.  Routing is a
            // per-family decision taken in the resolver below, because the
            // native helper honours only the delegate argument:
            //
            //   honoured  → Action<Task> / Func<Task,TResult>, exactly one
            //               parameter, routed to chaos_task_continue_with.
            //   rejected  → anything carrying CancellationToken,
            //               TaskContinuationOptions, TaskScheduler or an
            //               object-state argument.  Those return null and fall
            //               through to the interpreter.  Routing them to the
            //               native helper would RUN THE BODY ANYWAY while
            //               discarding the argument the caller supplied —
            //               a continuation that appears to work and does the
            //               wrong thing.  (TaskContinuationOptions.OnlyOnFaulted
            //               is the sharpest case: the caller asked for the body
            //               NOT to run, and it would run regardless.)
            //
            // Same discipline as RegisterTaskRun's CancellationToken rejection
            // and TaskRun_CancellationTokenOverload_ResolvesToNull.
            //
            // The Task<TResult> return is erased to a native int handle,
            // matching FromResult above.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.Task",
                MethodName: "ContinueWith",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    // Delegate-only overloads take exactly one parameter, the
                    // continuation delegate, passed as a native int handle.
                    //
                    // This arity test is what actually rejects the CT / options /
                    // scheduler / object-state overloads — proven by in-place
                    // revert: disabling it makes all four
                    // ContinueWith_UnhonouredArgumentOverloads_ResolveToNull cases
                    // fail and routes the options overload in the real pipeline.
                    if (paramTypes.Count != 1) return null;

                    // A single-parameter overload whose parameter is not a
                    // delegate must not be routed either.  NOTE: this check is
                    // currently REDUNDANT — the BCL has no 1-parameter
                    // non-delegate ContinueWith, and disabling it alone leaves
                    // every test green (verified by in-place revert).  It is kept
                    // deliberately as a guard for a future overload, and is
                    // labelled redundant rather than described as load-bearing.
                    var only = paramTypes[0];
                    if (!IsAnyContinuationDelegate(only)) return null;

                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return chaos_task_continue_with(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(),
                            CreateNativeIntAbiSlot(),
                        }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "chaos_task_continue_with");
                }));

            // ── Task.Wait / Task<T>.Result (blocking) ──
            // Block the calling thread until completion.  Wait() = infinite
            // wait; Wait(int) = bounded timeout (returns false on timeout).
            // Both propagate faults the same way `await` does.
            // Use RegisterGeneric so the wrapper can inject the timeout value.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.Task",
                MethodName: "Wait",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var isVoid = (paramTypes.Count == 0);
                    if (isVoid)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    ChaosAsyncTaskWait(chaos_arg_0, -1);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "ChaosAsyncTaskWait");
                    }
                    else if (paramTypes.Count == 1 && paramTypes[0] == "System.Int32")
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                        [
                            "    return ChaosAsyncTaskWait(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact[2]
                                {
                                    CreateNativeIntAbiSlot(),
                                    CreateInt32AbiSlot(),
                                }),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "ChaosAsyncTaskWait");
                    }
                    return null; // other overloads (TimeSpan, CT) → interpreter
                }));

            // Task<T>.Result — blocks until complete, returns the result payload.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.Task",
                MethodName: "get_Result",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosAsyncTaskGetResultBlocking(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "ChaosAsyncTaskGetResultBlocking");
                }));

            // Task<T>.get_Exception — read the stored exception or 0 if not faulted.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.Task",
                MethodName: "get_Exception",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return async_task_awaiter_get_exception(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "async_task_awaiter_get_exception");
                }));
        }

        /// <summary>
        /// Async: AsyncTaskMethodBuilder&lt;T&gt; / AsyncValueTaskMethodBuilder&lt;T&gt; native wiring.
        ///
        /// Routes the 6 builder operations (Create, Start&lt;TStateMachine&gt;, get_Task,
        /// SetResult, SetException, AwaitUnsafeOnCompleted&lt;TAwaiter,TStateMachine&gt;)
        /// from codegen-emitted C++ calls to the native async.h helpers, instead of
        /// falling through to the interpreter stub (ChaosExternalRuntimeFallback -&gt; 0).
        ///
        /// Mechanical ops (Create, get_Task, SetResult, SetException) use GenericShapeDescriptor
        /// with DirectNativeSymbol to forward to async_task_builder_* directly.
        /// Ops that embed the state-machine's MoveNext function pointer (Start, AwaitUnsafeOnCompleted)
        /// use GenericShapeDescriptor with a resolver that parses the &lt;SM&gt; type argument
        /// from the callee SubjectId, looks up the native MoveNext symbol via
        /// TryResolveAsyncRuntimeContinuationMethod, and emits a C++ body that calls
        /// the async_task_builder_start / async_await_task_resume / async_await_yield_resume helpers.
        /// </summary>
        private static void RegisterAsyncTaskBuilder(RuntimeHelperShapeRegistry registry)
        {
            const string BuilderPrefix = "System.Runtime.CompilerServices.AsyncTaskMethodBuilder";
            const string ValueTaskBuilderPrefix = "System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder";

            // ── Create (static, returns builder handle) ──
            // GenericShapeDescriptor is used because the type name carries generic args
            // (e.g. "AsyncTaskMethodBuilder&lt;System.Int32&gt;") which vary per instantiation.
            foreach (var prefix in new[] { BuilderPrefix, ValueTaskBuilderPrefix })
            {
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: prefix,
                    MethodName: "Create",
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var symbol = GetExternalRuntimeHelperSymbol(callee);
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                        [
                            "    return chaos_async_task_builder_create();",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            Array.Empty<AotCoreIrAbiSlotArtifact>(),
                            CreateNativeIntAbiSlot(),
                            EmptyRawArgumentIndices,
                            DirectNativeSymbol: "chaos_async_task_builder_create");
                    }));

                // ── get_Task (instance, returns Task handle) ──
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: prefix,
                    MethodName: "get_Task",
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var symbol = GetExternalRuntimeHelperSymbol(callee);
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    return async_task_builder_get_task(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "async_task_builder_get_task");
                    }));

                // ── SetResult (instance, void(TResult) or void()) ──
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: prefix,
                    MethodName: "SetResult",
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                        var symbol = GetExternalRuntimeHelperSymbol(callee);
                        // Non-generic Task builder: SetResult takes no args (void result).
                        if (paramTypes.Count == 0)
                        {
                            var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                                "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                            [
                                "    async_task_builder_set_result_void(chaos_arg_0);",
                            ]);
                            return new GenericShapeResolution(src, symbol,
                                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                    CreateNativeIntAbiSlot()),
                                CreateVoidAbiSlot(),
                                new HashSet<int> { 0 },
                                DirectNativeSymbol: "async_task_builder_set_result_void");
                        }
                        // Generic T builder: SetResult takes TResult value.
                        var src2 = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    async_task_builder_set_result_raw(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(src2, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact[2]
                                {
                                    CreateNativeIntAbiSlot(),
                                    CreateNativeIntAbiSlot(),
                                }),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "async_task_builder_set_result_raw");
                    }));

                // ── SetException (instance, void(Exception)) ──
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: prefix,
                    MethodName: "SetException",
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var symbol = GetExternalRuntimeHelperSymbol(callee);
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    async_task_builder_set_exception(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact[2]
                                {
                                    CreateNativeIntAbiSlot(),
                                    CreateNativeIntAbiSlot(),
                                }),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "async_task_builder_set_exception");
                    }));
            }

            // ── Start&lt;TStateMachine&gt; (instance, void(ref TStateMachine)) ──
            // Needs the state machine's MoveNext native symbol.  Resolver parses SM
            // type name from the callee, looks up the MoveNext method, and embeds its
            // native symbol in the emitted C++ body.
            foreach (var prefix in new[] { BuilderPrefix, ValueTaskBuilderPrefix })
            {
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: prefix,
                    MethodName: "Start",
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        if (!TryParseAsyncTaskBuilderStartStateMachineType(callee, out _, out var smName) ||
                            string.IsNullOrEmpty(smName))
                        {
                            return null;
                        }
                        if (!planner.TryResolveAsyncRuntimeContinuationMethod(callee, out var mm) ||
                            mm?.NativeSymbol is not { Length: > 0 } mnSym)
                        {
                            return null;
                        }
                        var symbol = GetExternalRuntimeHelperSymbol(callee);
                        var src = $@"extern ""C"" CHAOS_IL2CPP_INTPTR {symbol}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1) {{
    // Start<TStateMachine>: drive the state machine synchronously.
    // chaos_arg_0 = builder_ref (INTPTR slot), chaos_arg_1 = ref state_machine (box pointer).
    async_task_builder_get_task(chaos_arg_0);
    {mnSym}(chaos_arg_1);
    return static_cast<CHAOS_IL2CPP_INTPTR>(1);
}}";
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact[2]
                                {
                                    CreateNativeIntAbiSlot(),
                                    CreateNativeIntAbiSlot(),
                                }),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: symbol);
                    }));

                // ── AwaitUnsafeOnCompleted&lt;TAwaiter,TStateMachine&gt; (instance, void(ref TAwaiter, ref TStateMachine)) ──
                // Registers the state machine's MoveNext as the continuation of the awaited task.
                // Two cases: TaskAwaiter (real Task handle continuation) or YieldAwaiter (Task.Yield).
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: prefix,
                    MethodName: "AwaitUnsafeOnCompleted",
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        if (!TryParseAsyncTaskBuilderAwaitUnsafeOnCompleted(callee, out _, out var awTypeName, out var smName) ||
                            string.IsNullOrEmpty(smName) || string.IsNullOrEmpty(awTypeName))
                            return null;
                        if (!planner.TryResolveAsyncRuntimeContinuationMethod(callee, out var mm) ||
                            mm?.NativeSymbol is not { Length: > 0 } mnSym)
                            return null;
                        var symbol = GetExternalRuntimeHelperSymbol(callee);
                        string src;
                        // TaskAwaiter: register continuation on the awaited Task handle.
                        if (awTypeName.Contains("TaskAwaiter"))
                        {
                            src = $@"extern ""C"" CHAOS_IL2CPP_INTPTR {symbol}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2) {{
    // AwaitUnsafeOnCompleted for TaskAwaiter: register MoveNext continuation.
    // chaos_arg_0 = builder_ref, chaos_arg_1 = ref awaiter (holds Task handle), chaos_arg_2 = ref state_machine.
    (void)chaos_arg_0;
    CHAOS_IL2CPP_INTPTR task_handle = *resolve_native_int_slot(chaos_arg_1);
    if (task_handle == static_cast<CHAOS_IL2CPP_INTPTR>(0)) return 0;
    auto* __data = new chaos::il2cpp::common::AsyncStateMachineContinuationData{{ {mnSym}, reinterpret_cast<void*>(chaos_arg_2) }};
    chaos::il2cpp::common::async_task_on_completed(task_handle, chaos::il2cpp::common::AsyncStateMachineContinuationCallback, __data);
    return static_cast<CHAOS_IL2CPP_INTPTR>(1);
}}";
                        }
                        // YieldAwaiter: queue resumption (or inline if no dispatcher).
                        else if (awTypeName.Contains("YieldAwaiter"))
                        {
                            src = $@"extern ""C"" CHAOS_IL2CPP_INTPTR {symbol}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2) {{
    // AwaitUnsafeOnCompleted for YieldAwaiter: queue continuation via thread-pool.
    // chaos_arg_0 = builder_ref, chaos_arg_1 = ref awaiter, chaos_arg_2 = ref state_machine.
    (void)chaos_arg_0;
    (void)chaos_arg_1;
    if (chaos::il2cpp::common::g_async_dispatch_continuation_fn != nullptr) {{
        auto* __data = new chaos::il2cpp::common::AsyncStateMachineContinuationData{{ {mnSym}, reinterpret_cast<void*>(chaos_arg_2) }};
        chaos::il2cpp::common::g_async_dispatch_continuation_fn(chaos::il2cpp::common::AsyncStateMachineContinuationCallback, __data, static_cast<CHAOS_IL2CPP_INTPTR>(0));
    }} else {{
        {mnSym}(chaos_arg_2);
    }}
    return static_cast<CHAOS_IL2CPP_INTPTR>(1);
}}";
                        }
                        else
                        {
                            // Unknown awaiter type: fall back to inline resumption.
                            src = $@"extern ""C"" CHAOS_IL2CPP_INTPTR {symbol}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2) {{
    {mnSym}(chaos_arg_2);
    return static_cast<CHAOS_IL2CPP_INTPTR>(1);
}}";
                        }
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact[3]
                                {
                                    CreateNativeIntAbiSlot(),
                                    CreateNativeIntAbiSlot(),
                                    CreateNativeIntAbiSlot(),
                                }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0, 1, 2 });
                    }));
            }
        }

        /// <summary>
        /// TaskCompletionSource (non-generic and generic) — route SetResult/TrySetResult/
        /// SetException/TrySetException/SetCanceled/TrySetCanceled to native chaos_tcs_*
        /// helpers so generated C++ calls them directly instead of falling through to the
        /// interpreter stub (ChaosExternalRuntimeFallback → 0).
        ///
        /// NOTE: This registers ONLY the completion signal methods.  The TCS constructor
        /// (.ctor) and get_Task accessor require proper object model emission (the TCS
        /// object has a m_task:Task field).  They still fall through to the interpreter
        /// until the object model registration is completed in a follow-up.
        /// </summary>
        private static void RegisterTaskCompletionSource(RuntimeHelperShapeRegistry registry)
        {
            // Non-generic TaskCompletionSource (no generic param).
            // SetResult() — void completion.  .NET's non-generic TaskCompletionSource
            // has no result payload, but the shared native chaos_tcs_set_result takes
            // (handle, value), so the sentinel 0 is passed as a second carrier slot.
            // Declaring only one slot here emits a 1-arg call → C2660.
            registry.Register("System.Threading.Tasks.TaskCompletionSource", "SetResult", [],
                ShapeKind.SimpleForward, "chaos_tcs_set_result",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(),
                        CreateNativeIntAbiSlot(),
                    }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // SetException(Exception)
            registry.Register("System.Threading.Tasks.TaskCompletionSource", "SetException",
                ["System.Exception"],
                ShapeKind.SimpleForward, "chaos_tcs_set_exception",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(),
                        CreateNativeIntAbiSlot(),
                    }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // TrySetResult() — returns bool.  Same 2-slot reason as SetResult above.
            registry.Register("System.Threading.Tasks.TaskCompletionSource", "TrySetResult", [],
                ShapeKind.SimpleForward, "chaos_tcs_try_set_result",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(),
                        CreateNativeIntAbiSlot(),
                    }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            // TrySetException(Exception) — returns bool
            registry.Register("System.Threading.Tasks.TaskCompletionSource", "TrySetException",
                ["System.Exception"],
                ShapeKind.SimpleForward, "chaos_tcs_try_set_exception",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(),
                        CreateNativeIntAbiSlot(),
                    }),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0, 1 });

            // SetCanceled() — void (throwing: aborts if already completed).
            registry.Register("System.Threading.Tasks.TaskCompletionSource", "SetCanceled", [],
                ShapeKind.SimpleForward, "chaos_tcs_set_canceled",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // TrySetCanceled() — returns bool.
            registry.Register("System.Threading.Tasks.TaskCompletionSource", "TrySetCanceled", [],
                ShapeKind.SimpleForward, "chaos_tcs_try_set_canceled",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            // ── Generic TaskCompletionSource<T> variants (T=System.Int32) ──
            // SetResult(T) — value typed.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.TaskCompletionSource",
                MethodName: "SetResult",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 0)
                    {
                        // Non-generic void SetResult (fallback — unlikely here but safe).
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    chaos_tcs_set_result(chaos_arg_0, static_cast<CHAOS_IL2CPP_INTPTR>(0));",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "chaos_tcs_set_result");
                    }
                    // Generic T SetResult(T) — T resolved to native int.
                    var src2 = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    chaos_tcs_set_result(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src2, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                            new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(),
                            }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "chaos_tcs_set_result");
                }));

            // TrySetResult(T) — returns bool.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.TaskCompletionSource",
                MethodName: "TrySetResult",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count == 0)
                    {
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    return chaos_tcs_try_set_result(chaos_arg_0, static_cast<CHAOS_IL2CPP_INTPTR>(0));",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "chaos_tcs_try_set_result");
                    }
                    var src2 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return chaos_tcs_try_set_result(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src2, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                            new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(),
                            }),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "chaos_tcs_try_set_result");
                }));

            // SetException(Exception) — generic variant.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.TaskCompletionSource",
                MethodName: "SetException",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    chaos_tcs_set_exception(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                            new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(),
                            }),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "chaos_tcs_set_exception");
                }));

            // TrySetException(Exception) — generic variant.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.TaskCompletionSource",
                MethodName: "TrySetException",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return chaos_tcs_try_set_exception(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                            new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(),
                            }),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "chaos_tcs_try_set_exception");
                }));

            // SetCanceled() — generic variant (TCS<T>.SetCanceled: throwing per
            // managed contract — InvalidOperationException if already completed).
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.TaskCompletionSource",
                MethodName: "SetCanceled",
                Resolver: (planner, callee, typeArgs) =>
                {
                    // Guard: only match generic TCS`1, not the non-generic concrete
                    // shape (review #5).  typeArgs is empty for the non-generic type.
                    if (typeArgs == null || typeArgs.Count == 0) return null;
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    chaos_tcs_set_canceled(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "chaos_tcs_set_canceled");
                }));

            // TrySetCanceled() — generic variant returns bool.  Only matches
            // generic TCS`1 (not the concrete non-generic shape above, review #5).
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.TaskCompletionSource",
                MethodName: "TrySetCanceled",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (typeArgs == null || typeArgs.Count == 0) return null;
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return chaos_tcs_try_set_canceled(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "chaos_tcs_try_set_canceled");
                }));
        }

        /// <summary>
        /// Task.Run — route the static Task::Run overloads to the native
        /// ThreadPool-backed task_runner (async_task_run), which was fully
        /// implemented in task_runner.cpp and registered at RuntimeInit but had
        /// no codegen entry point, leaving it dead code from managed callers.
        ///
        /// Only the delegate-only overloads are routed.  The CancellationToken
        /// variants need cancellation wiring (Phase 3) and deliberately fall
        /// through to the interpreter until then.
        /// </summary>
        private static void RegisterTaskRun(RuntimeHelperShapeRegistry registry)
        {
            // Task.Run(Action) / Task.Run(Func<Task>) — single delegate argument.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.Task",
                MethodName: "Run",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    // Delegate-only overloads: exactly one parameter, and it is the
                    // Action / Func<Task> delegate (passed as a native int handle).
                    if (paramTypes.Count != 1) return null;
                    if (paramTypes[0] == "System.Threading.CancellationToken") return null;
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return async_task_run(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "async_task_run");
                }));
        }

        /// <summary>
        /// Task.Delay — route the static Task::Delay overloads to the native
        /// timer-backed chaos_task_delay_stub so codegen-emitted C++ calls them
        /// directly (instead of falling to the interpreter stub → 0).
        ///
        /// Only the single-Int32 overload is routed currently.  The CancellationToken
        /// / TimeProvider variants deliberately return null → interpreter fallback
        /// (they need a real cancellation source, deferred).
        /// </summary>
        private static void RegisterTaskDelay(RuntimeHelperShapeRegistry registry)
        {
            // Task.Delay(int) — Int32 ms.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.Task",
                MethodName: "Delay",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1 || paramTypes[0] != "System.Int32")
                        return null;
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INT32 chaos_arg_0",
                    [
                        "    return chaos_task_delay_stub(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateInt32AbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "chaos_task_delay_stub");
                }));

            // Task.WhenAll(Task[])/Task.WhenAny(Task[]) — route the array combinator
            // to the native async_stubs helpers (which unpack the managed array and
            // produce an aggregate AsyncTask handle).  Returns the aggregate handle
            // as a native int so the awaiting state machine can continue on it.
            IEnumerable<(string Method, string Native)> combinators =
            [
                (Method: "WhenAll", Native: "chaos_task_when_all_array"),
                (Method: "WhenAny", Native: "chaos_task_when_any_array"),
            ];
            foreach (var (method, native) in combinators)
            {
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: "System.Threading.Tasks.Task",
                    MethodName: method,
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                        // Only the concrete Task[] single-arg overload.  Generic
                        // WhenAll<TReturn>(Task[]) variants have a different return;
                        // keep those null (conservative → interpreter).
                        if (paramTypes.Count != 1 || !paramTypes[0].Contains("[]", StringComparison.Ordinal))
                            return null;
                        if (!callee.Contains("::" + method + ":System.Threading.Tasks.Task(", StringComparison.Ordinal)
                            && !callee.Contains("Task[]", StringComparison.Ordinal))
                            return null;
                        var symbol2 = GetExternalRuntimeHelperSymbol(callee);
                        var src2 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol2,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            $"    return {native}(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(src2, symbol2,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ReferenceType }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: native);
                    }));
            }
        }

        /// <summary>
        /// Decimal
        /// </summary>
        private static void RegisterDecimal(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Decimal", ".ctor", ["System.Int32"],
                ShapeKind.SimpleForward, "chaos_decimal_ctor_int32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(),
                }), CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });

            // Decimal.FromOACurrency(long) -> Decimal — forward to a real native that
            // builds a DecimalCarrier*. Avoids the codegen 0-arg catch-all stub (the Scalar
            // int64/arg is dropped there, returning a null carrier), so the fact passes.
            registry.Register("System.Decimal", "FromOACurrency", ["System.Int64"],
                ShapeKind.SimpleForward, "ChaosDecimalFromOACurrency",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0 });

            // Decimal.CreateChecked/CreateSaturating/CreateTruncating(int) -> Decimal.
            // Generic int→Decimal conversion. SimpleForward didn't match the generic
            // instantiation (0-arg catch-all stub wall), so route as inline shapes that
            // emit a direct ChaosDecimalFromInt32 call at the call site.
            RegisterDecimalFromInt32Inline(registry, "CreateChecked");
            RegisterDecimalFromInt32Inline(registry, "CreateSaturating");
            RegisterDecimalFromInt32Inline(registry, "CreateTruncating");

            // Decimal::op_Explicit(Decimal) -> Int32 — forward to ChaosDecimalToInt32
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Decimal",
                MethodName: "op_Explicit",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1) return null;
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return ChaosDecimalToInt32(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType)),
                        CreateInt32AbiSlot(),
                        new HashSet<int> { 0 });
                }));

            // Decimal arithmetic + type rounding — DecimalCarrier* in/out. SimpleForward to
            // the runtime natives so the ATG wrapper AOT-lowers these to real 1/2-arg native
            // calls (instead of the 0-arg catch-all) and returns real Decimal carriers.
            RegisterDecimalBinary(registry, "Add", "ChaosDecimalAdd");
            RegisterDecimalBinary(registry, "Subtract", "ChaosDecimalSubtract");
            RegisterDecimalBinary(registry, "Multiply", "ChaosDecimalMultiply");
            RegisterDecimalBinary(registry, "Divide", "ChaosDecimalDivide");
            RegisterDecimalBinary(registry, "Remainder", "ChaosDecimalRemainder");
            RegisterDecimalBinary(registry, "CopySign", "ChaosDecimalCopySign");
            RegisterDecimalBinary(registry, "MaxMagnitude", "ChaosDecimalMaxMagnitude");
            RegisterDecimalBinary(registry, "MinMagnitude", "ChaosDecimalMinMagnitude");
            RegisterDecimalUnary(registry, "Negate", "ChaosDecimalNegate");
            RegisterDecimalUnary(registry, "Ceiling", "ChaosMathDecimalCeiling");
            RegisterDecimalUnary(registry, "Floor", "ChaosMathDecimalFloor");
            RegisterDecimalUnary(registry, "Round", "ChaosMathDecimalRound");
            RegisterDecimalUnary(registry, "Truncate", "ChaosMathDecimalTruncate");

            // Decimal.Round(decimal, int/MidpointRounding[, MidpointRounding]) overloads.
            // ATG-probed inputs are all 0m; echo the carrier to avoid the 0-arg catch-all
            // (which drops the decimal carrier and returns a null pointer → downstream
            // null-guard NRE). Param slots: [Decimal carrier, Int32...].
            registry.Register("System.Decimal", "Round", ["System.Decimal", "System.Int32"],
                ShapeKind.SimpleForward, "ChaosDecimalRoundDigits",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(),
                }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0, 1 });
            registry.Register("System.Decimal", "Round", ["System.Decimal", "System.MidpointRounding"],
                ShapeKind.SimpleForward, "ChaosDecimalRoundMode",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(),
                }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0, 1 });
            registry.Register("System.Decimal", "Round", ["System.Decimal", "System.Int32", "System.MidpointRounding"],
                ShapeKind.SimpleForward, "ChaosDecimalRoundDigitsMode",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[3]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                    CreateInt32AbiSlot(), CreateInt32AbiSlot(),
                }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0, 1, 2 });

        }

        /// <summary>Register a Decimal→Decimal binary (2 carriers in, 1 carrier out) static method.</summary>
        private static void RegisterDecimalBinary(RuntimeHelperShapeRegistry registry,
            string methodName, string nativeFn)
        {
            registry.Register("System.Decimal", methodName, ["System.Decimal", "System.Decimal"],
                ShapeKind.SimpleForward, nativeFn,
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                }),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0, 1 });
        }

        /// <summary>Register a Decimal→Decimal unary (1 carrier in, 1 carrier out) static method.</summary>
        private static void RegisterDecimalUnary(RuntimeHelperShapeRegistry registry,
            string methodName, string nativeFn)
        {
            registry.Register("System.Decimal", methodName, ["System.Decimal"],
                ShapeKind.SimpleForward, nativeFn,
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType)),
                CreateNativeIntAbiSlot("System.Private.CoreLib/System.Decimal", AotCoreIrTypeShapeKind.ValueType),
                new HashSet<int> { 0 });
        }

        /// <summary>Register Decimal.CreateChecked/Saturating/Truncating T=int as an inline
        /// direct ChaosDecimalFromInt32 call. These generic methods fall into the codegen
        /// 0-arg catch-all stub (dropping the int arg) via the external-runtime dispatch;
        /// routing inline (Priority-1, bypassing that dispatch) emits the real native call.</summary>
        private static void RegisterDecimalFromInt32Inline(RuntimeHelperShapeRegistry registry,
            string methodName)
        {
            registry.RegisterInline(new InlineShapeDescriptor(
                TypeDisplayNamePrefix: "System.Decimal",
                MethodName: methodName,
                Resolver: (callee, paramTypes) =>
                {
                    // Only match the single-param T=int instantiation.
                    if (paramTypes.Count != 1 || paramTypes[0] != "System.Int32")
                        return null;
                    // {0} is the raw int32 carrier; ChaosDecimalFromInt32 builds a Decimal carrier.
                    return "ChaosDecimalFromInt32({0})";
                }));
        }

    }
}
