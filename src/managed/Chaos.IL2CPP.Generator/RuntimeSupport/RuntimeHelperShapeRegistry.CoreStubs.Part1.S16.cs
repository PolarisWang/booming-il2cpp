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

            // YieldAwaitable.YieldAwaiter.GetResult() — same void-slot hazard as
            // the TaskAwaiter / ValueTaskAwaiter GetResult registrations below:
            // the lowering always writes `const auto chaos_result = <call>()`,
            // so a void return slot yields C3313/C3536.  The native helper is
            // void; the generated TU sees only an injected declaration, so the
            // slot type does not change the expression type — it only decides
            // whether the generated wrapper compiles.
            registry.Register("System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter", "GetResult", [],
                ShapeKind.SimpleForward, "chaos_async_yield_get_result",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
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

            // ── T1: Task.Wait(CancellationToken) ──
            // Wait(CancellationToken) is an instance method with one parameter,
            // so Phase A's receiver injection gives the shim 2 slots:
            // (task_handle, token).  The native entry ignores the token
            // (no cancellation wiring at this call site yet — same limitation
            // as the MRES token overloads in the threading roadmap).
            registry.Register("System.Threading.Tasks.Task", "Wait",
                ["System.Threading.CancellationToken"],
                ShapeKind.SimpleForward, "ChaosAsyncTaskWaitToken",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── T1: Task.ConfigureAwait(bool) ──
            registry.Register("System.Threading.Tasks.Task", "ConfigureAwait",
                ["System.Boolean"],
                ShapeKind.SimpleForward, "ChaosAsyncTaskConfigureAwait",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── T1: Task.ConfigureAwait(ConfigureAwaitOptions) ──
            registry.Register("System.Threading.Tasks.Task", "ConfigureAwait",
                ["System.Threading.Tasks.ConfigureAwaitOptions"],
                ShapeKind.SimpleForward, "ChaosAsyncTaskConfigureAwaitOptions",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── T1: ConfiguredTaskAwaitable.GetAwaiter ──
            registry.Register(
                "System.Runtime.CompilerServices.ConfiguredTaskAwaitable", "GetAwaiter", [],
                ShapeKind.SimpleForward, "ChaosConfiguredTaskAwaitableGetAwaiter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── T1: ConfiguredTaskAwaitable<T>.GetAwaiter ──
            registry.Register(
                "System.Runtime.CompilerServices.ConfiguredTaskAwaitable`1", "GetAwaiter", [],
                ShapeKind.SimpleForward, "ChaosConfiguredTaskAwaitableGetAwaiter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── T1: Task<T>.ConfigureAwait(bool) ──
            registry.Register(
                "System.Threading.Tasks.Task`1", "ConfigureAwait", ["System.Boolean"],
                ShapeKind.SimpleForward, "ChaosAsyncTaskOfTConfigureAwait",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── T1: Task<T>.ConfigureAwait(ConfigureAwaitOptions) ──
            registry.Register(
                "System.Threading.Tasks.Task`1", "ConfigureAwait",
                ["System.Threading.Tasks.ConfigureAwaitOptions"],
                ShapeKind.SimpleForward, "ChaosAsyncTaskOfTConfigureAwait",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── T1: ValueTask.ConfigureAwait(bool) ──
            registry.Register(
                "System.Threading.Tasks.ValueTask", "ConfigureAwait", ["System.Boolean"],
                ShapeKind.SimpleForward, "ChaosValueTaskConfigureAwait",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── T1: ConfiguredValueTaskAwaitable.GetAwaiter ──
            registry.Register(
                "System.Runtime.CompilerServices.ConfiguredValueTaskAwaitable",
                "GetAwaiter", [],
                ShapeKind.SimpleForward, "ChaosConfiguredValueTaskAwaitableGetAwaiter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── T1: ConfiguredValueTaskAwaitable<T>.GetAwaiter ──
            registry.Register(
                "System.Runtime.CompilerServices.ConfiguredValueTaskAwaitable`1",
                "GetAwaiter", [],
                ShapeKind.SimpleForward, "ChaosConfiguredValueTaskAwaitableGetAwaiter",
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

            // ── Non-generic TaskAwaiter.GetResult (void in managed) ──
            // `await someTask` (no result) lowers against the non-generic
            // TaskAwaiter, whose GetResult returns void.  It must still
            // propagate a fault, so it routes to its own helper rather than
            // sharing the value-returning one above.
            //
            // Two independent requirements, both satisfied here:
            //   1. The slot must be INTPTR, not void — the lowering emits
            //      `const auto chaos_result = <call>(...);` unconditionally,
            //      so a void slot yields C3313 ("variable cannot have the type
            //      'const void'") + C3536.
            //   2. The native symbol itself must be declared as RETURNING
            //      INTPTR.  The shape dispatch in runtime_helper_shapes.h
            //      forwards through `reinterpret_cast<CHAOS_IL2CPP_INTPTR>(...)`,
            //      which is ill-formed over a void expression.  Fixing only the
            //      slot is not enough: the declaration comes from async_stubs.h,
            //      so `ChaosAsyncTaskAwaiterGetResultVoid` returns INTPTR (0)
            //      there and in its definition.  The managed member is void, so
            //      the value is unobservable; fault/cancel propagation happens
            //      via RaiseManagedException inside the helper.
            // This mirrors the ValueTaskAwaiter registration below, which hit
            // the same wall in commit 7a89c72d7.
            registry.Register("System.Runtime.CompilerServices.TaskAwaiter", "GetResult", [],
                ShapeKind.SimpleForward, "ChaosAsyncTaskAwaiterGetResultVoid",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── Task<T> / TaskAwaiter<T> / ValueTaskAwaiter<T> 等 ——
            // 统一补全所有泛型变体：`1[[T]] 和 `1<`1> 等嵌套泛型格式。
            // 用 backtick 前缀确保匹配到带泛型参的 callee。
            foreach (var (prefix, method, native, returnsVoid) in new[]
            {
                // Task<T>::GetAwaiter() -> TaskAwaiter<T>
                ("System.Threading.Tasks.Task`1", "GetAwaiter", "ChaosAsyncTaskGetAwaiter", false),
                // TaskAwaiter<T>::get_IsCompleted()
                ("System.Runtime.CompilerServices.TaskAwaiter`1", "get_IsCompleted", "ChaosAsyncTaskAwaiterGetIsCompleted", false),
                // TaskAwaiter<T>::GetResult() -> T
                ("System.Runtime.CompilerServices.TaskAwaiter`1", "GetResult", "ChaosAsyncTaskAwaiterGetResultValue", false),
                // ValueTask<T>::GetAwaiter() -> ValueTaskAwaiter<T>
                ("System.Threading.Tasks.ValueTask`1", "GetAwaiter", "chaos_value_task_get_awaiter", false),
                // ValueTaskAwaiter<T>::GetResult() -> T
                ("System.Runtime.CompilerServices.ValueTaskAwaiter`1", "GetResult", "ChaosAsyncTaskAwaiterGetResultValue", false),
                // ValueTaskAwaiter (non-generic) ::GetResult() -> returns void.
                // The void version produced C3313 (const void chaos_result = ...)
                // because returnsVoid=false made the codegen path assign the
                // call result to an eval-stack slot via `const auto chaos_result =`,
                // but ChaosAsyncTaskAwaiterGetResultVoid returns void.
                // Fix: mark returnsVoid=true so the void-aware emission path
                // emits a bare call without result assignment. Fault propagation
                // is handled by the GetResultVoid native helper already.
                ("System.Runtime.CompilerServices.ValueTaskAwaiter", "GetResult", "ChaosAsyncTaskAwaiterGetResultVoid", true),
            })
            {
                // Capture into locals: a closure over the foreach variable would
                // see the LAST tuple for every iteration on older C# semantics.
                string pfx = prefix, mth = method, nat = native;
                bool isVoid = returnsVoid;
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: pfx,
                    MethodName: mth,
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var symbol = GetExternalRuntimeHelperSymbol(callee);
                        return new GenericShapeResolution("", symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            isVoid ? CreateVoidAbiSlot() : CreateNativeIntAbiSlot(),
                            EmptyRawArgumentIndices,
                            DirectNativeSymbol: nat);
                    }));
            }

            // ── ValueTask / ValueTask<T> ::ConfigureAwait(bool) -> ConfiguredValueTaskAwaitable ──
            // Both variants (non-generic + generic) need SimpleForward to avoid C2065.
            // ConfigureAwait creates a wrapper struct that holds the task + flag; since this
            // runtime represents a ValueTask as its backing Task handle and ConfigureAwait(false)
            // is the default, the wrapper IS the task handle. Simply return the first arg.
            foreach (var vtp in new[] { "System.Threading.Tasks.ValueTask", "System.Threading.Tasks.ValueTask`1" })
            {
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: vtp,
                    MethodName: "ConfigureAwait",
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                        if (paramTypes.Count != 1) return null;
                        if (paramTypes[0] != "System.Boolean") return null;
                        var symbol = GetExternalRuntimeHelperSymbol(callee);
                        // The ConfiguredValueTaskAwaitable IS the task handle in this runtime.
                        // Return it directly through the identity helper.
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1",
                        [
                            "    (void)chaos_arg_1;",
                            "    return chaos_arg_0;",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact[2]
                                {
                                    CreateNativeIntAbiSlot(),
                                    CreateInt32AbiSlot(),
                                }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0, 1 });
                    }));
            }

            // ── TaskToAsyncResult —— impossible in AOT (APM Begin/End pattern) ──
            // These are APM (IAsyncResult) adapter methods that cannot exist in a pure AOT
            // runtime.  Register as zero-return stubs so the chunk compiles.
            foreach (var ttaMethod in new[] { "Begin", "End", "Unwrap" })
            {
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: "System.Threading.Tasks.TaskToAsyncResult",
                    MethodName: ttaMethod,
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                        // Matches with ANY parameter count (the overloads vary).
                        return null;  // Let it fall through to external-runtime (will be C3861
                                      // at build time, but we add a SimpleForward catch-all below).
                    }));
            }

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

            // ValueTask.FromResult / FromCanceled / FromException — the same
            // factories but for ValueTask.  Without these, any call to
            // ValueTask.FromResult / FromCanceled / FromException from the BCL
            // falls through to ChaosExternalRuntimeFallback -> emitted as
            // undefined extern "C" symbol, C3861 at build time.
            // The ValueTask routing is the identity for the finished task handle.
            foreach (var (vtName, vtSymbol) in new[]
            {
                ("FromResult", "async_task_from_result"),
                ("FromException", "async_task_from_exception"),
                ("FromCanceled", "async_task_from_canceled"),
            })
            {
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: "System.Threading.Tasks.ValueTask",
                    MethodName: vtName,
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                        var symbol = GetExternalRuntimeHelperSymbol(callee);
                        var paramCount = paramTypes.Count;
                        // ValueTask::FromResult(T) — 1 param of the value type.
                        // ValueTask::FromException(Exception) — 1 param.
                        // ValueTask::FromCanceled(CancellationToken) — 1 param.
                        if (paramCount != 1) return null;
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            $"    return {vtSymbol}(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: vtSymbol);
                    }));
            }

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
                    // (arity!=1 rejection moved below the multi-arg branch)

                    // A single-parameter overload whose parameter is not a
                    // delegate must not be routed either.  NOTE: this check is
                    // currently REDUNDANT — the BCL has no 1-parameter
                    // non-delegate ContinueWith, and disabling it alone leaves
                    // every test green (verified by in-place revert).  It is kept
                    // deliberately as a guard for a future overload, and is
                    // labelled redundant rather than described as load-bearing.
                    if (paramTypes.Count != 1) return null;
                    var only = paramTypes[0];
                    if (!IsAnyContinuationDelegate(only)) return null;

                    var symbolCw = GetExternalRuntimeHelperSymbol(callee);

                    // Multi-arg overloads — (delegate, CancellationToken / TaskContinuationOptions /
                    // TaskScheduler / object state [, ...]).  The resolver deliberately rejected
                    // these (arity != 1), dropping the receiver+delegate into an operand-less
                    // 0-arg catch-all.  Forward the FULL operand list instead; the extra
                    // operands are accepted-and-ignored by chaos_task_continue_with (probes
                    // pass defaults).  20 failing facts.
                    if (paramTypes.Count >= 2 && IsAnyContinuationDelegate(paramTypes[0]))
                    {
                        var slotCount = paramTypes.Count + 1; // + receiver
                        var cwSlots = new List<AotCoreIrAbiSlotArtifact> { CreateNativeIntAbiSlot() };
                        foreach (var pt in paramTypes)
                        {
                            // TaskContinuationOptions is an enum → Int32 carrier; everything
                            // else (delegate, CT struct, scheduler ref, object state) is INTPTR.
                            cwSlots.Add(pt == "System.Threading.Tasks.TaskContinuationOptions"
                                ? CreateInt32AbiSlot()
                                : CreateNativeIntAbiSlot());
                        }
                        var cwSig = string.Join(", ", Enumerable.Range(0, slotCount).Select(i =>
                        {
                            var paramIndex = i - 1;
                            var isInt32 = paramIndex >= 0 && paramTypes[paramIndex] == "System.Threading.Tasks.TaskContinuationOptions";
                            return (isInt32 ? "CHAOS_IL2CPP_INT32" : "CHAOS_IL2CPP_INTPTR") + $" chaos_arg_{i}";
                        }));
                        var ignoreExprs = string.Join("; ", Enumerable.Range(2, slotCount - 2).Select(i => $"(void)chaos_arg_{i}"));
                        var cwSrc = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbolCw, cwSig,
                        [
                            slotCount > 2 ? $"    {ignoreExprs};" : "",
                            "    return chaos_task_continue_with(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(cwSrc, symbolCw,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(cwSlots.ToArray()),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int>(Enumerable.Range(0, slotCount)),
                            DirectNativeSymbol: "chaos_task_continue_with");
                    }

                    var symbol = symbolCw;
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
                            "    ChaosAsyncTaskWaitInfinite(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "ChaosAsyncTaskWaitInfinite");
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
                    if (paramTypes.Count >= 1 && paramTypes[0] == "System.TimeSpan")
                    {
                        // Task.Wait(TimeSpan[, CancellationToken]) — the native
                        // ChaosAsyncTaskWaitTimeSpan already existed (async_stubs.cpp:1361)
                        // but was never registered ("dead code"); these overloads fell to
                        // the 0-arg catch-all.  TimeSpan rides an Int64 ticks carrier.
                        var slots = new List<AotCoreIrAbiSlotArtifact>
                        {
                            CreateNativeIntAbiSlot(),
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType },
                        };
                        if (paramTypes.Count >= 2)
                            slots.Add(CreateNativeIntAbiSlot());
                        var paramSig = string.Join(", ", Enumerable.Range(0, slots.Count).Select(i =>
                            i == 1 ? "CHAOS_IL2CPP_INT64 chaos_arg_1" : $"CHAOS_IL2CPP_INTPTR chaos_arg_{i}"));
                        var tokenExpr = paramTypes.Count >= 2 ? "chaos_arg_2" : "0";
                        var srcSpan = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol, paramSig,
                        [
                            $"    return ChaosAsyncTaskWaitTimeSpan(chaos_arg_0, chaos_arg_1, {tokenExpr});",
                        ]);
                        return new GenericShapeResolution(srcSpan, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(slots.ToArray()),
                            CreateInt32AbiSlot(),
                            new HashSet<int>(Enumerable.Range(0, slots.Count)));
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
        /// ASYNC-P2-8 A3. AsyncIteratorMethodBuilder native wiring — the
        /// <c>async IAsyncEnumerable&lt;T&gt;</c> / <c>async IAsyncEnumerator&lt;T&gt;</c>
        /// counterpart of <see cref="RegisterAsyncTaskBuilder"/>.
        ///
        /// Routes the 5 builder operations to the A2 native surface
        /// (<c>chaos/async_iterator.h</c>, committed 624c3682f):
        /// <code>
        ///   Create/0                        -> chaos_async_iterator_builder_create()
        ///   MoveNext`1/1                    -> chaos_async_iterator_builder_move_next(...)
        ///   AwaitOnCompleted`2/2            -> chaos_async_iterator_builder_await_on_completed(...)
        ///   AwaitUnsafeOnCompleted`2/2      -> chaos_async_iterator_builder_await_unsafe_on_completed(...)
        ///   Complete/0                      -> chaos_async_iterator_builder_complete(...)
        /// </code>
        /// Spellings are the committed contract artifact's
        /// (<c>runtime-helper-contracts-v1-01.json</c>), not a hand-written guess.
        ///
        /// <para>
        /// <b>Why BOTH AwaitOnCompleted and AwaitUnsafeOnCompleted are registered.</b>
        /// They are distinct members on the managed type, and the C# compiler emits
        /// whichever the awaiter permits — <c>await Task.Yield()</c> inside an iterator
        /// can reach the unsafe form.  The original A2 plan registered only
        /// <c>AwaitOnCompleted</c>; the recon doc flagged that omission (§1.1) and both
        /// are wired here to one native implementation (native code does not enforce the
        /// unsafe/on-completed distinction).
        /// </para>
        ///
        /// <para>
        /// <b>These are NOT stubs.</b> Each forwards to real native semantics over the
        /// pooled source.  A constant-return body here would reproduce exactly the
        /// silent-wrong-answer shape A1 was built to eliminate, while looking greener
        /// than falling through to ChaosExternalRuntimeFallback.
        /// </para>
        /// </summary>
        private static void RegisterAsyncIteratorBuilder(RuntimeHelperShapeRegistry registry)
        {
            const string Prefix = "System.Runtime.CompilerServices.AsyncIteratorMethodBuilder";

            // ── Create (static, returns builder handle) ──
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: Prefix,
                MethodName: "Create",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, "",
                    [
                        "    return chaos_async_iterator_builder_create();",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        Array.Empty<AotCoreIrAbiSlotArtifact>(),
                        CreateNativeIntAbiSlot(),
                        EmptyRawArgumentIndices,
                        DirectNativeSymbol: "chaos_async_iterator_builder_create");
                }));

            // ── Complete (instance, void()) ──
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: Prefix,
                MethodName: "Complete",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    // Complete() ends an iteration but must NOT free the builder: the state",
                        "    // machine can be enumerated again (GetAsyncEnumerator called twice).",
                        "    // Release happens through chaos_async_iterator_builder_destroy at the",
                        "    // state machine's own lifetime end.",
                        "    chaos_async_iterator_builder_complete(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateVoidAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "chaos_async_iterator_builder_complete");
                }));

            // ── MoveNext<TStateMachine> (instance; drives the iterator's MoveNext) ──
            // Needs the state machine's native MoveNext symbol, exactly as Start<SM> does.
            // Resolver parses the <SM> type argument, resolves its MoveNext method, and
            // embeds the native symbol in the emitted body.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: Prefix,
                MethodName: "MoveNext",
                Resolver: (planner, callee, typeArgs) =>
                {
                    if (!TryParseAsyncIteratorBuilderMoveNextStateMachineType(callee, out var smName) ||
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
    // MoveNext<TStateMachine>(ref stateMachine): drive the iterator one step.
    // chaos_arg_0 = builder handle (the source pool), chaos_arg_1 = ref state machine.
    return chaos_async_iterator_builder_move_next(chaos_arg_0, reinterpret_cast<CHAOS_IL2CPP_INTPTR>({mnSym}), chaos_arg_1);
}}";
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                            new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(),
                            }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: symbol);
                }));

            // ── AwaitOnCompleted / AwaitUnsafeOnCompleted<TAwaiter,TStateMachine> ──
            // Both spellings register the SAME resolver — the two managed members share one
            // native implementation (see the class doc).  Registering only one would leave
            // the other's awaits routing to ChaosExternalRuntimeFallback -> 0, i.e. an
            // iterator that silently stops advancing at that await.
            foreach (string methodName in new[] { "AwaitOnCompleted", "AwaitUnsafeOnCompleted" })
            {
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: Prefix,
                    MethodName: methodName,
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        if (!TryParseAsyncIteratorBuilderAwaitOnCompleted(callee, out _, out var smName) ||
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
                        string nativeEntry = methodName == "AwaitOnCompleted"
                            ? "chaos_async_iterator_builder_await_on_completed"
                            : "chaos_async_iterator_builder_await_unsafe_on_completed";
                        var src = $@"extern ""C"" CHAOS_IL2CPP_INTPTR {symbol}(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2) {{
    // {methodName}<TAwaiter,TStateMachine>(ref awaiter, ref stateMachine).
    // chaos_arg_0 = ref awaiter (the awaited AsyncTask handle lives in this slot),
    // chaos_arg_1 = ref state machine, chaos_arg_2 unused (kept for ABI symmetry with
    // the native entry point's move_next/sm_box pair).
    //
    // The continuation is registered on the AWAITED TASK, not on the iterator's own
    // pooled source: registering on the pool would resume the wrong object and the
    // state machine would never re-enter.
    CHAOS_IL2CPP_INTPTR awaited = *resolve_native_int_slot(chaos_arg_0);
    if (awaited == static_cast<CHAOS_IL2CPP_INTPTR>(0)) return static_cast<CHAOS_IL2CPP_INTPTR>(0);
    return {nativeEntry}(awaited, reinterpret_cast<CHAOS_IL2CPP_INTPTR>({mnSym}), chaos_arg_1);
}}";
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact[3]
                                {
                                    CreateNativeIntAbiSlot(),
                                    CreateNativeIntAbiSlot(),
                                    CreateNativeIntAbiSlot(),
                                }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0, 1 });
                    }));
            }
        }

        /// <summary>
        /// ASYNC-P2-8 A4. <c>ManualResetValueTaskSourceCore&lt;bool&gt;</c> — the
        /// <c>IValueTaskSource</c> body the iterator state machine feeds.
        ///
        /// <para>
        /// The async-iterator <c>MoveNext</c> ends each arm with a completion signal:
        /// <c>&lt;&gt;v__promiseOfValueOrEnd.SetResult(true)</c> on the yield arm and
        /// <c>.SetResult(false)</c> on the exhausted arm (plus <c>SetException</c> on the
        /// handler path).  <c>promiseOfValueOrEnd</c> is a
        /// <c>ManualResetValueTaskSourceCore&lt;bool&gt;</c> whose address is passed as the
        /// receiver.
        /// </para>
        ///
        /// <para>
        /// Without this registration those calls fall through to
        /// <c>ChaosExternalRuntimeFallback</c> with <b>zero arguments forwarded</b> — so
        /// the yielded/exhausted signal is computed and then discarded, and every
        /// <c>MoveNextAsync</c> observes an unset source.  That is a silently wrong
        /// enumerable, not a missing feature: the state machine looks correct and the
        /// value never arrives.  The gap was measured on
        /// <c>&lt;YieldOne&gt;d__0::MoveNext</c>, where both arms emitted
        /// <c>chaos_external_runtime_..._SetResult_..._System_Boolean_()</c> with an empty
        /// argument list while <c>_s4 = 0</c> / <c>_s7 = 1</c> sat unused on the stack.
        /// </para>
        ///
        /// <para>
        /// Maps to the A2 native surface over the pooled source
        /// (<c>chaos/async_iterator.h</c>): <c>AsyncIteratorSourceCore</c> is the
        /// documented <c>ManualResetValueTaskSourceCore&lt;bool&gt;</c> equivalent.
        /// "core" handle = address of the promise field = <c>chaos_arg_0</c>.
        /// </para>
        /// </summary>
        private static void RegisterManualResetValueTaskSourceCore(RuntimeHelperShapeRegistry registry)
        {
            const string Prefix = "System.Threading.Tasks.Sources.ManualResetValueTaskSourceCore";

            // ── SetResult(bool) — instance; receiver + the bool payload ──
            // The bool crosses as a widened INTPTR slot (ABI carrier), matching how the
            // iterator's `__current` store carries it; native narrows to int32.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: Prefix,
                MethodName: "SetResult",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    // SetResult(bool value): chaos_arg_0 = &promiseOfValueOrEnd (the",
                        "    // AsyncIteratorSourceCore), chaos_arg_1 = the completion value",
                        "    // (true = element yielded, false = iteration exhausted).",
                        "    chaos_async_iterator_source_set_result(chaos_arg_0,",
                        "        static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));",
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
                        DirectNativeSymbol: "chaos_async_iterator_source_set_result");
                }));

            // ── SetException(Exception) — instance; receiver + exception object ──
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: Prefix,
                MethodName: "SetException",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    // SetException(Exception): fault the source so the awaiting",
                        "    // MoveNextAsync observes the exception instead of hanging.",
                        "    chaos_async_iterator_source_set_exception(chaos_arg_0, chaos_arg_1);",
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
                        DirectNativeSymbol: "chaos_async_iterator_source_set_exception");
                }));
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
                ShapeKind.SimpleForward, "chaos_tcs_set_result_void",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
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
                ShapeKind.SimpleForward, "chaos_tcs_try_set_result_void",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
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

// SetCanceled() — void.  Previously pointed at chaos_tcs_set_exception,
            // which faults the task instead of cancelling it.
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
                        // Non-generic void SetResult.  Normally handled by the direct
                        // registration in RegisterTaskCompletionSource (inline descriptors
                        // are matched first); this branch is the safety net for subject IDs
                        // that reach here, and forwards to the dedicated 0-arg native
                        // helper rather than reusing the 2-arg one.
                        var src = RenderSimpleExternalRuntimeHelper("void", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            "    chaos_tcs_set_result_void(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            CreateVoidAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "chaos_tcs_set_result_void");
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
                            "    return chaos_tcs_try_set_result_void(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "chaos_tcs_try_set_result_void");
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

                    // ── T2: (delegate, CancellationToken) ──
                    //
                    // The second parameter is the token; the first varies by
                    // overload (Action, Func<Task>, Func<int>, Func<Task<int>>,
                    // ...) so the shape — exactly two params ending in a token —
                    // is what identifies the family.
                    //
                    // The token is accepted but NOT honoured: there is no
                    // cancellation plumbing at this call site.  Before this
                    // registration the call returned 0 through the fallback (no
                    // task at all), so queueing the delegate is strictly an
                    // improvement; a cancelled token simply does not stop the
                    // work, which is a documented limitation, not a regression.
                    if (paramTypes.Count == 2 &&
                        paramTypes[1] == "System.Threading.CancellationToken")
                    {
                        var sym2 = GetExternalRuntimeHelperSymbol(callee);
                        var src2 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", sym2,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            "    return chaos_task_run(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(src2, sym2,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                            {
                                CreateNativeIntAbiSlot(),
                                CreateNativeIntAbiSlot(),
                            }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: "chaos_task_run");
                    }

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
        /// Task.Factory.StartNew — route the delegate-only overloads of the
        /// default TaskFactory onto the SAME native ThreadPool runner as
        /// Task.Run (async_task_run).
        ///
        /// <para>
        /// This is not an approximation.  .NET's default TaskFactory (the one
        /// Task.Factory returns) uses TaskScheduler.Current for StartNew, which
        /// outside a scheduler context is the default (ThreadPool) scheduler —
        /// exactly where Task.Run queues.  So for the delegate-only overloads the
        /// two APIs are semantically identical, and sharing the runner is what
        /// .NET does, not a shortcut.
        /// </para>
        ///
        /// <para>
        /// Only SINGLE-parameter overloads are routed.  The remaining StartNew
        /// shapes each carry an argument whose semantics the runner cannot honour:
        /// </para>
        /// <list type="bullet">
        /// <item><c>StartNew(Action, TaskCreationOptions)</c> — the options are
        /// dropped, so a caller asking for LongRunning/AttachedToParent would
        /// silently get the default behaviour.</item>
        /// <item><c>StartNew(Action, CancellationToken)</c> — needs the Phase 3
        /// cancellation wiring; passing the token through unchanged would make
        /// cancellation a no-op.</item>
        /// <item><c>StartNew&lt;TResult&gt;(Func&lt;TResult&gt;)</c> — a
        /// result-producing factory; the runner returns a plain Task handle and
        /// never stores the func's return value.</item>
        /// <item><c>StartNew(Action, state)</c> — the state object is dropped.</item>
        /// </list>
        /// <para>
        /// Those fall through to the interpreter (resolver returns null), which is
        /// the honest outcome — the same precedent P2-4 set for ContinueWith.
        /// </para>
        /// </summary>
        private static void RegisterTaskFactory(RuntimeHelperShapeRegistry registry)
        {
            // Task.Factory's static property.  The runtime has no TaskFactory
            // object model, and none is needed: the factory's StartNew maps onto
            // the default scheduler, so the getter returns a non-null opaque
            // token purely so a caller that stores or passes Task.Factory does
            // not receive a bogus 0 handle.  The token is never dereferenced.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.Task",
                MethodName: "get_Factory",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "",
                    [
                        "    return 1;",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(Array.Empty<AotCoreIrAbiSlotArtifact>()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int>(),
                        DirectNativeSymbol: "chaos_task_default_factory");
                }));

            // TaskFactory::StartNew — delegate-only overloads (exactly one
            // parameter, and it is a delegate) route to the ThreadPool runner.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.TaskFactory",
                MethodName: "StartNew",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);

                    // Delegate-only: exactly one parameter, and it must BE the
                    // delegate.  The non-delegate 1-parameter overloads
                    // (state object) and the multi-parameter overloads
                    // (options / token / state) are rejected here.
                    if (paramTypes.Count != 1) return null;
                    if (!IsAnyContinuationDelegate(paramTypes[0])) return null;

                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                    [
                        "    return chaos_task_factory_start_new(chaos_arg_0, chaos_arg_1);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                        {
                            CreateNativeIntAbiSlot(),
                            CreateNativeIntAbiSlot(),
                        }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1 },
                        DirectNativeSymbol: "chaos_task_factory_start_new");
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
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    if (paramTypes.Count >= 1 && paramTypes[0] == "System.TimeSpan")
                    {
                        // Task.Delay(TimeSpan[, TimeProvider][, CancellationToken]) — the
                        // native chaos_task_delay_timespan_stub(Int64 ticks) already existed
                        // (async_stubs.cpp:501) without a registration channel.  TimeProvider
                        // / CancellationToken are accepted-and-ignored: the probes pass
                        // default(...) (= null), and a null TimeProvider is semantically
                        // TimeProvider.System.  ⚠️ CT cancellation semantics not wired.
                        var srcSpan = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INT64 chaos_arg_0",
                        [
                            "    return chaos_task_delay_timespan_stub(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(srcSpan, symbol,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ValueType }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: "chaos_task_delay_timespan_stub");
                    }
                    if (paramTypes.Count != 1 || paramTypes[0] != "System.Int32")
                        return null;
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
                        if (method == "WhenAny" && paramTypes.Count == 2)
                        {
                            // WhenAny(Task, Task) — the two-task overload had no
                            // registration; the 0-arg catch-all discarded both task
                            // handles (6 failing facts across Task / Task<T> spellings).
                            var symbolWA2 = GetExternalRuntimeHelperSymbol(callee);
                            var srcWhenAny2 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbolWA2,
                                "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                            [
                                "    return chaos_task_when_any2(chaos_arg_0, chaos_arg_1);",
                            ]);
                            return new GenericShapeResolution(srcWhenAny2, symbolWA2,
                                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                                {
                                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                    CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ReferenceType),
                                }),
                                CreateNativeIntAbiSlot(),
                                new HashSet<int> { 0, 1 },
                                DirectNativeSymbol: "chaos_task_when_any2");
                        }
                        if (paramTypes.Count != 1 || !paramTypes[0].Contains("[]", StringComparison.Ordinal))
                            return null;

                        // Only the single-argument overload: <method>(Task[]).
                        //
                        // The previous guard tested `::<method>:System.Threading.Tasks.Task(`
                        // — the NON-generic return type.  That rejected
                        // Task.WhenAll<int>(Task<int>[]) (return type
                        // `Task<int[]>`) AND any overload the declaration record
                        // spelled fully-qualified (`...Tasks.Task(...)`), i.e. the
                        // generic overload silently fell through to the
                        // interpreter's return-0 fallback.  Anchor on the method
                        // signature instead, which is what we actually mean.
                        //
                        // The name may carry an explicit generic argument list
                        // (`::WhenAll<System.Int32>(`), so match the `::<method>`
                        // prefix and accept either `(` or `<` next.  `Task[]`
                        // remains as the fallback for the reference-only form.
                        var methodAnchor = "::" + method;
                        var anchorIdx = callee.IndexOf(methodAnchor, StringComparison.Ordinal);
                        if (anchorIdx >= 0)
                        {
                            var afterName = callee[(anchorIdx + methodAnchor.Length)..];
                            if (!afterName.StartsWith('(') && !afterName.StartsWith('<'))
                                return null;
                        }
                        else if (!callee.Contains("Task[]", StringComparison.Ordinal))
                        {
                            return null;
                        }
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

            // ── T3: Task.WaitAll / Task.WaitAny (blocking) ──
            //
            // Distinct from WhenAll/WhenAny above in BOTH contract and ABI shape:
            //   * contract — these BLOCK the calling thread; WhenAll/WhenAny return
            //     an aggregate Task and never block.
            //   * ABI — these are STATIC and return Boolean / Int32 (not a Task
            //     handle), so the return slot differs from the combinators'.
            //
            // ── Why ONLY the single-argument overload is matched ──
            //
            // The earlier version matched every overload whose FIRST parameter was
            // an array, on the theory that "the shim carries exactly one slot — the
            // array" covers them all.  That theory is wrong, and it produced a
            // silent-wrong-code defect rather than a missing feature.
            //
            // `DirectNativeSymbol` makes codegen call the native symbol with the
            // call site's OWN arguments, so the declared slot count must equal the
            // CALLEE's arity — not the count of parameters we happen to forward.
            // Registering a 1-slot shim for WaitAll(Task[], TimeSpan) left the
            // second operand on the evaluation stack; the generated body did
            // `_s1 = &chaos_locals[1]` (the TimeSpan slot address) and passed THAT
            // as the array handle.  UnpackTaskArray then read length==0 from a
            // TimeSpan, WaitAll returned 0, and the caller took the false branch
            // `ChaosLoadInt64(0)` — a null dereference -> STATUS_ACCESS_VIOLATION.
            //
            // The failure was masked by a coincidence in the sibling: WaitAny on
            // the same shape returns -1 for an empty array WITHOUT dereferencing,
            // so it passed while WaitAll faulted.  Two identically-registered
            // overloads, opposite outcomes — that asymmetry is the tell.
            //
            // Timeout/CTS overloads therefore fall through to the interpreter's
            // explicit external-runtime fallback.  That is the honest outcome: a
            // visibly unimplemented method, not a plausible-but-wrong one.  Wiring
            // them properly needs a multi-slot shim plus a native signature that
            // accepts the timeout (chaos_task_wait_all takes only the handle today).
            IEnumerable<(string Method, string Native, bool ReturnsIndex)> blockers =
            [
                (Method: "WaitAll", Native: "chaos_task_wait_all", ReturnsIndex: false),
                (Method: "WaitAny", Native: "chaos_task_wait_any", ReturnsIndex: true),
            ];
            foreach (var (method, native, returnsIndex) in blockers)
            {
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: "System.Threading.Tasks.Task",
                    MethodName: method,
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                        // Exactly one managed parameter, and it is the task array.
                        if (paramTypes.Count != 1) return null;
                        if (!paramTypes[0].Contains("[]", StringComparison.Ordinal)
                            && !paramTypes[0].Contains("IEnumerable", StringComparison.Ordinal))
                            return null;

                        var symbol3 = GetExternalRuntimeHelperSymbol(callee);
                        var src3 = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INT32", symbol3,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                        [
                            $"    return {native}(chaos_arg_0);",
                        ]);
                        return new GenericShapeResolution(src3, symbol3,
                            new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                                CreateNativeIntAbiSlot()),
                            CreateInt32AbiSlot(),
                            new HashSet<int> { 0 },
                            DirectNativeSymbol: native);
                    }));
            }

            // Task.WhenEach(Task[]) / Task.WhenEach<TResult>(Task<TResult>[]) — the
            // ORDER-PRESERVING completion stream.
            //
            // Not a variant of the two above: WhenEach returns IAsyncEnumerable<Task>
            // that yields each task AS IT COMPLETES, so a caller awaiting the
            // enumerable observes completion ORDER.  Routing it to when_all would
            // defer every element until the last task finished; routing it to
            // when_any would yield exactly once.  Both are silently different
            // programs that still look like a successful registration — which is
            // why the registry test asserts the destination symbol by name and
            // asserts the other two are absent.
            //
            // The native side owns the queue semantics (a re-armed completion
            // source drained in completion order); codegen only has to name it.
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.Task",
                MethodName: "WhenEach",
                Resolver: (planner, callee, typeArgs) =>
                {
                    // Same array-only guard as WhenAll/WhenAny: the native shim
                    // unpacks a contiguous managed array.  The IEnumerable<Task>
                    // overload has no native model and must fall through to the
                    // interpreter rather than be silently approximated.
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    if (paramTypes.Count != 1 || !paramTypes[0].Contains("[]", StringComparison.Ordinal))
                        return null;

                    // The name may carry a generic argument list
                    // (`::WhenEach<System.Int32>(`), so accept `(` or `<` after the
                    // `::WhenEach` anchor — the same shape WhenAll/WhenAny needed.
                    var whenEachAnchor = "::WhenEach";
                    var weIdx = callee.IndexOf(whenEachAnchor, StringComparison.Ordinal);
                    if (weIdx >= 0)
                    {
                        var afterName = callee[(weIdx + whenEachAnchor.Length)..];
                        // Accept `(` (no return type), `<` (generic args), or `:` (return type)
                        // after the method name.
                        if (!afterName.StartsWith('(') && !afterName.StartsWith('<') && !afterName.StartsWith(':'))
                            return null;
                    }
                    else if (!callee.Contains("Task[]", StringComparison.Ordinal))
                    {
                        return null;
                    }

                    const string WhenEachNative = "chaos_task_when_each_array";
                    var weSymbol = GetExternalRuntimeHelperSymbol(callee);
                    var weSrc = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", weSymbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        $"    return {WhenEachNative}(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(weSrc, weSymbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Int64, TypeShape = AotCoreIrTypeShapeKind.ReferenceType }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: WhenEachNative);
                }));
        }

        /// <summary>
        /// TaskFactory.ContinueWhenAll / ContinueWhenAny — the array overloads.
        ///
        /// Composition rather than new mechanism: the native entry point builds the
        /// WhenAll/WhenAny aggregate over the task array and registers the
        /// continuation on THAT aggregate (see chaos_task_continue_when_*_array).
        ///
        /// <para>
        /// The distinction that matters is which object the continuation observes.
        /// Registering it on each CHILD would fire it once per task — for
        /// ContinueWhenAll that is N invocations where the caller asked for one.
        /// It looks correct on a single-element array and multiplies side effects
        /// on every other, so the guard here routes the whole array through the
        /// aggregate and the native test pins the invocation count.
        /// </para>
        ///
        /// <para>
        /// Only the <c>(Task[], Action&lt;Task[]&gt;)</c>-shaped overloads are routed:
        /// exactly two parameters, the first an array and the second a delegate.
        /// The <c>TaskCreationOptions</c> / <c>CancellationToken</c> / state-object
        /// variants carry arguments with no native model and return null, falling
        /// to the interpreter — the honest outcome, same precedent as StartNew.
        /// </para>
        /// </summary>
        private static void RegisterTaskFactoryContinueWhen(RuntimeHelperShapeRegistry registry)
        {
            IEnumerable<(string Method, string Native)> continuations =
            [
                (Method: "ContinueWhenAll", Native: "chaos_task_continue_when_all_array"),
                (Method: "ContinueWhenAny", Native: "chaos_task_continue_when_any_array"),
            ];
            foreach (var (method, native) in continuations)
            {
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: "System.Threading.Tasks.TaskFactory",
                    MethodName: method,
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        // (Task[] tasks, Action<Task[]> continuation) — two params,
                        // the array first and a delegate second.  Anything else
                        // (options / token / state / IEnumerable overloads) has no
                        // native model.
                        var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                        if (paramTypes.Count != 2) return null;
                        if (!paramTypes[0].Contains("[]", StringComparison.Ordinal)) return null;
                        if (!IsAnyContinuationDelegate(paramTypes[1])) return null;

                        var cwSymbol = GetExternalRuntimeHelperSymbol(callee);
                        var cwSrc = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", cwSymbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1",
                        [
                            $"    return {native}(chaos_arg_0, chaos_arg_1);",
                        ]);
                        return new GenericShapeResolution(cwSrc, cwSymbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact[2]
                                {
                                    CreateNativeIntAbiSlot(),
                                    CreateNativeIntAbiSlot(),
                                }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0, 1 },
                            DirectNativeSymbol: native);
                    }));
            }
        }

        /// <summary>
        /// Phase 3: AsyncLocal&lt;T&gt;.Value getter/setter.
        ///
        /// AsyncLocal&lt;T&gt; is a generic class whose native storage is thread-local (see
        /// execution_context.cpp).  The getter &amp; setter each take a KEY (the async local's
        /// stable instance id, an opaque int64) and read/write that thread-local storage.
        ///
        /// <para>
        /// Registered via SimpleForward rather than GenericShapeDescriptor: the native
        /// bridge is non-generic (the key and the boxed value both cross as flat INTPTR),
        /// so expanding the type argument into a per-T wrapper would add nothing.
        /// </para>
        ///
        /// <para>
        /// Without this registration every <c>AsyncLocal&lt;T&gt;.Value</c> read/write falls
        /// through to ChaosExternalRuntimeFallback → 0, meaning the value silently resets
        /// across an await — which is exactly the behaviour the Phase 3 AsyncLocal
        /// counter-example (set 1 → await → still 1) is designed to catch.
        /// </para>
        /// </summary>
        private static void RegisterAsyncLocal(RuntimeHelperShapeRegistry registry)
        {
            // AsyncLocal<T>::get_Value() — instance, returns the stored boxed value.
            registry.Register(
                "System.Threading.AsyncLocal`1",
                "get_Value", [],
                ShapeKind.SimpleForward, "chaos_async_local_get_value",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // AsyncLocal<T>::set_Value(T) — instance, takes the boxed value.
            registry.Register(
                "System.Threading.AsyncLocal`1",
                "set_Value", [],
                ShapeKind.SimpleForward, "chaos_async_local_set_value",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact[2]
                    {
                        CreateNativeIntAbiSlot(),
                        CreateNativeIntAbiSlot(),
                    }),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0, 1 });
        }

        /// <summary>
        /// Phase 3: CancellationToken state queries.
        ///
        /// A managed CancellationToken is (source_id, version) in this runtime, and the
        /// value crossing the ABI is the source id; 0 means CancellationToken.None.
        /// These are the three queries generated code needs to branch on cancellation
        /// state (the acceptance counter-example is `Task.Delay(10s, ct)` after
        /// `Cancel()` completing in ~10ms rather than the full 10s).
        ///
        /// <para>
        /// <b>Why CanBeCanceled is not the same question as IsCancellationRequested.</b>
        /// A live-but-uncancelled token reports CanBeCanceled = true and
        /// IsCancellationRequested = false; None reports false for both.  Conflating
        /// them makes ThrowIfCancellationRequested a no-op on real tokens — cancellation
        /// silently stops working while every "is it cancelled?" test still looks right.
        /// </para>
        /// </summary>
        private static void RegisterCancellationToken(RuntimeHelperShapeRegistry registry)
        {
            registry.Register("System.Threading.CancellationToken", "get_IsCancellationRequested", [],
                ShapeKind.SimpleForward, "chaos_cancellation_token_is_cancellation_requested",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Threading.CancellationToken", "get_CanBeCanceled", [],
                ShapeKind.SimpleForward, "chaos_cancellation_token_can_be_canceled",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });

            registry.Register("System.Threading.CancellationToken", "ThrowIfCancellationRequested", [],
                ShapeKind.SimpleForward, "chaos_cancellation_token_throw_if_cancellation_requested",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });
        }

        /// <summary>
        /// Phase 3: CancellationTokenSource lifecycle.
        ///
        /// create / Cancel / Dispose / get_Token.  A managed CancellationTokenSource
        /// is identified by its source id in this runtime, and get_Token is the
        /// identity (0 = CancellationToken.None).
        ///
        /// <para>
        /// <b>get_Token must not be routed to the fallback.</b>  Returning 0 there
        /// would turn every real token into CancellationToken.None — the source
        /// would cancel correctly and the token derived from it would still report
        /// "never cancelled", so the acceptance counter-example
        /// (Task.Delay(10s, cts.Token) + Cancel() completing in ~10ms) would wait
        /// the full 10 seconds while every individual call looked wired.
        /// </para>
        /// </summary>
        private static void RegisterCancellationTokenSource(RuntimeHelperShapeRegistry registry)
        {
            // .ctor() — creates a source, returns its id.
            registry.Register("System.Threading.CancellationTokenSource", ".ctor", [],
                ShapeKind.SimpleForward, "chaos_cancellation_token_source_create",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            // Cancel() — fires all registered callbacks.
            registry.Register("System.Threading.CancellationTokenSource", "Cancel", [],
                ShapeKind.SimpleForward, "chaos_cancellation_token_source_cancel",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // Dispose() — cancels the timer and invalidates the source.
            registry.Register("System.Threading.CancellationTokenSource", "Dispose", [],
                ShapeKind.SimpleForward, "chaos_cancellation_token_source_dispose",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // get_Token — identity in this runtime (token IS its source id).
            registry.Register("System.Threading.CancellationTokenSource", "get_Token", [],
                ShapeKind.SimpleForward, "chaos_cancellation_token_source_get_token",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateInt32AbiSlot(),
                new HashSet<int> { 0 });
        }

        /// <summary>
        /// Phase 3: ExecutionContext flow control.
        ///
        /// SuppressFlow / RestoreFlow / IsFlowSuppressed.  These control whether the
        /// runtime's AsyncLocal values are captured for a newly spawned thread or
        /// work item.  Capture/Run are invoked from the runtime's own thread-start
        /// and thread-pool paths (threading_stubs.cpp), NOT from generated code, so
        /// they are deliberately not registered here.
        ///
        /// <para>
        /// <b>SuppressFlow must return a cookie and RestoreFlow must consume it.</b>
        /// Treating SuppressFlow as fire-and-forget (routing it to a void fallback)
        /// leaves flow suppressed for the rest of the thread's life — every later
        /// thread or task on that thread silently loses its AsyncLocal values, and
        /// the loss is invisible because each individual call still returns.
        /// </para>
        /// </summary>
        private static void RegisterExecutionContext(RuntimeHelperShapeRegistry registry)
        {
            // SuppressFlow() — returns a cookie that RestoreFlow consumes.
            registry.Register("System.Threading.ExecutionContext", "SuppressFlow", [],
                ShapeKind.SimpleForward, "chaos_execution_context_suppress_flow",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);

            // RestoreFlow() — .NET's parameterless form restores the depth to 0.
            // Passing 0 as the cookie matches that: the runtime's restore takes the
            // depth to set, and the parameterless API means "not suppressed".
            registry.Register("System.Threading.ExecutionContext", "RestoreFlow", [],
                ShapeKind.SimpleForward, "chaos_execution_context_restore_flow",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // IsFlowSuppressed() — observable state; a constant would make every
            // assertion about suppression vacuous.
            registry.Register("System.Threading.ExecutionContext", "IsFlowSuppressed", [],
                ShapeKind.SimpleForward, "chaos_execution_context_is_flow_suppressed",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateInt32AbiSlot(),
                EmptyRawArgumentIndices);
        }

        /// <summary>
        /// Phase 3: SynchronizationContext.
        ///
        /// This runtime is headless — there is no UI message pump — so the
        /// placeholder context created by the parameterless constructor posts and
        /// sends INLINE on the calling thread.  That is the semantically correct
        /// behaviour for a placeholder, not a stub: the alternative is blocking
        /// the caller until a pump that does not exist drains the callback.
        ///
        /// <para>
        /// <b>What actually has to be right is SetCurrent/GetCurrent.</b>  Whether
        /// a continuation resumes on its original context is decided by reading
        /// <c>Current</c> at completion time.  If SetSynchronizationContext
        /// silently discarded its argument, ConfigureAwait(true) and
        /// ConfigureAwait(false) would become indistinguishable — the Phase 3
        /// acceptance counter-example ("resume thread != original thread" under
        /// ConfigureAwait(false)) could not tell them apart.
        /// </para>
        /// </summary>
        private static void RegisterSynchronizationContext(RuntimeHelperShapeRegistry registry)
        {
            // .ctor() — creates the placeholder context.
            registry.Register("System.Threading.SynchronizationContext", ".ctor", [],
                ShapeKind.SimpleForward, "chaos_synchronization_context_create",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot(),
                EmptyRawArgumentIndices);

            // get_Current — static; the current thread's installed context.
            registry.Register("System.Threading.SynchronizationContext", "get_Current", [],
                ShapeKind.SimpleForward, "chaos_synchronization_context_get_current",
                Array.Empty<AotCoreIrAbiSlotArtifact>(),
                CreateNativeIntAbiSlot(),
                EmptyRawArgumentIndices);

            // SetSynchronizationContext(ctx) — static; returns the previous context
            // so a caller (or the awaiter machinery) can restore it.
            registry.Register("System.Threading.SynchronizationContext", "SetSynchronizationContext",
                [],
                ShapeKind.SimpleForward, "chaos_synchronization_context_set_current",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });
        }

        /// <summary>
        /// Phase 4: ValueTask state queries + AsTask.
        ///
        /// The six members the design designates: AsTask, GetAwaiter, and the
        /// four state predicates (IsCompleted / IsCompletedSuccessfully /
        /// IsFaulted / IsCanceled).
        ///
        /// <para>
        /// <b>The four predicates must stay distinct.</b>  A ValueTask that
        /// completed with an exception is IsCompleted=true,
        /// IsCompletedSuccessfully=false, IsFaulted=true, IsCanceled=false;
        /// one cancelled by a token is the same but with IsCanceled=true and
        /// IsFaulted=false.  Collapsing them (e.g. routing all four at one
        /// "is completed" helper) would make a faulted ValueTask take the
        /// success path — the exact silent-wrong-answer shape Phase 1's
        /// three-state counter-example exists to catch.
        /// </para>
        ///
        /// <para>
        /// In this runtime a managed ValueTask crosses the ABI as its backing
        /// task handle (0 = default/completed-void), which is why the
        /// predicates take one INTPTR slot and return an Int32.
        /// </para>
        /// </summary>
        private static void RegisterValueTask(RuntimeHelperShapeRegistry registry)
        {
            // AsTask() — materialize a Task from the ValueTask.
            registry.Register("System.Threading.Tasks.ValueTask", "AsTask", [],
                ShapeKind.SimpleForward, "chaos_value_task_as_task",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // GetAwaiter() — yields the ValueTaskAwaiter the state machine drives.
            registry.Register("System.Threading.Tasks.ValueTask", "GetAwaiter", [],
                ShapeKind.SimpleForward, "chaos_value_task_get_awaiter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // The four state predicates.  Each gets its OWN native entry point so
            // that no two questions can be answered by the same computation.
            foreach (var (method, symbol) in new[]
            {
                ("get_IsCompleted", "chaos_value_task_is_completed"),
                ("get_IsCompletedSuccessfully", "chaos_value_task_is_completed_successfully"),
                ("get_IsFaulted", "chaos_value_task_is_faulted"),
                ("get_IsCanceled", "chaos_value_task_is_canceled"),
            })
            {
                registry.Register("System.Threading.Tasks.ValueTask", method, [],
                    ShapeKind.SimpleForward, symbol,
                    new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                        CreateNativeIntAbiSlot()),
                    CreateInt32AbiSlot(),
                    new HashSet<int> { 0 });
            }
        }

        /// <summary>
        /// E3 (Phase 6): hot BCL members that currently fall through to the
        /// interpreter.
        ///
        /// <para>
        /// Measured on the Parallel chunk benchmark: 8 methods account for 96% of
        /// the chunk's total runtime, and every one of them is a
        /// <c>ChaosExternalRuntimeFallback</c> stub — averaging 20.1us against
        /// 0.9us for a real lowered method (21.4x).  Two of those eight are plain
        /// BCL surface with no async or Parallel semantics at all:
        /// </para>
        /// <list type="bullet">
        /// <item><c>System.IDisposable::Dispose()</c> — an interface method.  A
        /// no-op is CORRECT for a managed object whose finalization is handled by
        /// the GC; the alternative (interpreter round-trip) costs ~40us per call
        /// to do nothing.</item>
        /// <item><c>System.Func&lt;T&gt;::.ctor(object, IntPtr)</c> /
        /// <c>System.Action&lt;…&gt;::.ctor(object, IntPtr)</c> — the delegate
        /// constructor.  The runtime already represents a delegate as a
        /// DelegateObject handle built from (target, method-ptr); the ctor binds
        /// exactly those two values, so it is a store, not an interpretation.</item>
        /// </list>
        ///
        /// <para>
        /// <b>Why these two and not more.</b>  Both are provably total — the
        /// result does not depend on any managed state the runtime cannot see.
        /// That is the bar for this method: a registration here must not be an
        /// approximation of the managed semantics.  Parallel.ForEach (the other
        /// two hot stubs) does NOT meet that bar — it needs a real partitioning
        /// scheduler — so it is deliberately left to a separate change rather
        /// than faked here.
        /// </para>
        /// </summary>
        private static void RegisterHotBclNoOps(RuntimeHelperShapeRegistry registry)
        {
            // IDisposable::Dispose() — no-op.  The object's storage is reclaimed
            // by the GC; a managed Dispose that released unmanaged resources
            // would be its own lowered method, which this does not shadow (the
            // registration matches the INTERFACE declaration only).
            registry.Register("System.IDisposable", "Dispose", [],
                ShapeKind.SimpleForward, "chaos_noop_void",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateVoidAbiSlot(),
                new HashSet<int> { 0 });

            // Delegate constructors.  Generic over the signature, so a generic
            // descriptor anchors on the prefix and accepts every arity.
            foreach (var prefix in new[]
            {
                "System.Func",
                "System.Action",
                "System.Predicate",
                "System.Comparison",
                "System.Converter",
            })
            {
                registry.RegisterGeneric(new GenericShapeDescriptor(
                    TypeDisplayNamePrefix: prefix,
                    MethodName: ".ctor",
                    Resolver: (planner, callee, typeArgs) =>
                    {
                        var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                        // (object target, IntPtr method) — the only delegate ctor
                        // shape.  Anything else is not this pattern.
                        if (paramTypes.Count != 2) return null;
                        if (paramTypes[0] != "System.Object") return null;
                        if (paramTypes[1] != "System.IntPtr") return null;

                        var symbol = GetExternalRuntimeHelperSymbol(callee);
                        // ChaosDelegateInitialize returns void (it is a constructor:
                        // it writes the target/method into an already-allocated
                        // delegate object).  The generated wrapper must still yield a
                        // value for the ABI slot, so call it and return the delegate
                        // handle — returning the void call directly is C2440.
                        var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                            "CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                        [
                            "    ChaosDelegateInitialize(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                            "    return chaos_arg_0;",
                        ]);
                        return new GenericShapeResolution(src, symbol,
                            new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                                new AotCoreIrAbiSlotArtifact[3]
                                {
                                    CreateNativeIntAbiSlot(),
                                    CreateNativeIntAbiSlot(),
                                    CreateNativeIntAbiSlot(),
                                }),
                            CreateNativeIntAbiSlot(),
                            new HashSet<int> { 0, 1, 2 },
                            DirectNativeSymbol: "ChaosDelegateInitialize");
                    }));
            }
        }

        /// <summary>
        /// E2 (Phase 6): Parallel.For / Parallel.ForEach — the data-parallel loops.
        ///
        /// <para>
        /// These were the single largest measured cost in the Parallel chunk: the
        /// two hot <c>ForEach</c> stubs alone were 585us of the chunk's 1175us
        /// benchmark total (52%), each running every delegate invocation through
        /// the interpreter (MarshalDelegateInvoke → reflection → call) at ~1us per
        /// call against ~30ns for a direct native call.
        /// </para>
        ///
        /// <para>
        /// <b>Scope: the range overload only.</b>  <c>Parallel.For(int,int,Action&lt;int&gt;)</c>
        /// is a range partitioner — it has no managed-side state beyond the bounds
        /// and the delegate, so the native implementation can partition it across
        /// the ThreadPool and call the delegate's method_ptr directly.  The
        /// <c>IEnumerable</c>/<c>ForEachAsync</c> overloads need a partitioner over
        /// an arbitrary enumerator (and, for the Async form, cancellation and
        /// ExecutionContext flow), which is a separate piece of work.  They keep
        /// returning null here and stay on the interpreter — the honest outcome,
        /// not a partial lowering.
        /// </para>
        /// </summary>
        private static void RegisterParallelLoops(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.Parallel",
                MethodName: "For",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var paramTypes = GetMethodParameterTypesFromSubjectId(callee);
                    // (int from, int to, Action<int> body) — exactly three
                    // parameters, two Int32 bounds and a delegate body.
                    if (paramTypes.Count != 3) return null;
                    if (paramTypes[0] != "System.Int32" || paramTypes[1] != "System.Int32") return null;
                    if (!IsAnyContinuationDelegate(paramTypes[2])) return null;

                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INT32 chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1, CHAOS_IL2CPP_INTPTR chaos_arg_2",
                    [
                        "    return chaos_parallel_for_range_int(chaos_arg_0, chaos_arg_1, chaos_arg_2);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(
                            new AotCoreIrAbiSlotArtifact[3]
                            {
                                CreateInt32AbiSlot(),
                                CreateInt32AbiSlot(),
                                CreateNativeIntAbiSlot(),
                            }),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0, 1, 2 },
                        DirectNativeSymbol: "chaos_parallel_for_range_int");
                }));
        }

        /// <summary>
        /// Phase 4: TaskExtensions.Unwrap + TaskToAsyncResult.
        ///
        /// <c>Unwrap</c> flattens a Task&lt;Task&lt;T&gt;&gt; into a Task&lt;T&gt;.  In this
        /// runtime an aggregate task's result is already the inner handle, so the
        /// flattening is the identity: the outer task IS the inner one once the
        /// inner completes.  The registration exists so the call does not fall
        /// through to a numeric fallback — a fallback returning 0 here would
        /// silently replace a real inner task with a null handle.
        /// </summary>
        private static void RegisterTaskUnwrap(RuntimeHelperShapeRegistry registry)
        {
            registry.RegisterGeneric(new GenericShapeDescriptor(
                TypeDisplayNamePrefix: "System.Threading.Tasks.TaskExtensions",
                MethodName: "Unwrap",
                Resolver: (planner, callee, typeArgs) =>
                {
                    var symbol = GetExternalRuntimeHelperSymbol(callee);
                    var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol,
                        "CHAOS_IL2CPP_INTPTR chaos_arg_0",
                    [
                        "    return chaos_task_unwrap(chaos_arg_0);",
                    ]);
                    return new GenericShapeResolution(src, symbol,
                        new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                            CreateNativeIntAbiSlot()),
                        CreateNativeIntAbiSlot(),
                        new HashSet<int> { 0 },
                        DirectNativeSymbol: "chaos_task_unwrap");
                }));
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

        /// <summary>
        /// T2.2: System.Threading.ReaderWriterLockSlim + SemaphoreSlim.
        ///
        /// WHY THE RECEIVER IS NOT AN ABI SLOT
        /// -----------------------------------
        /// These are all INSTANCE methods, and the lowering for them passes NO
        /// receiver: the generated body for
        /// <c>ReaderWriterLockSlimTests::EnterReadLock_0__0</c> in the threading
        /// chunk emits
        /// <code>
        ///   chaos_external_runtime_..._Create_ReaderWriterLockSlim__()   // -> _s2
        ///   chaos_external_runtime_..._ReaderWriterLockSlim__EnterReadLock_System_Void__()  // no args
        /// </code>
        /// — <c>_s2</c> holds the instance and is then never read again.  Declaring
        /// a receiver ABI slot here would therefore make the lowering pop a value
        /// off an eval stack that has nothing left on it.
        ///
        /// So the receiver reaches the helper the other way: the native entry reads
        /// it OUT OF THE INSTANCE.  That is exactly what T2.1's
        /// <c>ChaosManagedHandleGetOrCreate</c> / <c>Load</c> / <c>Release</c> are
        /// for, and it is what makes the two tasks a matched pair rather than two
        /// independent registrations.
        ///
        /// CONSEQUENCE THAT MUST NOT BE FORGOTTEN
        /// --------------------------------------
        /// <c>SubjectInstanceFactory.Create&lt;ReaderWriterLockSlim&gt;()</c> is
        /// <c>RuntimeHelpers.GetUninitializedObject</c> — it allocates a
        /// zero-initialised object and runs NO constructor.  So the handle field is
        /// 0 ("never bound"), NOT some stale value it can act on.  The native entry
        /// must treat an unbound/absent handle as an error and say so, rather than
        /// silently succeeding: "the constructor never ran" must not be
        /// indistinguishable from "the lock is held".
        ///
        /// The managed ctors themselves (<c>.ctor(int)</c>) are a SEPARATE piece of
        /// work — a parameterless/arity-1 ctor has to reach
        /// <c>ChaosManagedHandleGetOrCreate</c> with a create hook.  Registering
        /// these method entries does not do that, and this comment exists so the
        /// distinction is not lost.
        /// </summary>
        private static void RegisterReaderWriterLockSlimAndSemaphoreSlim(RuntimeHelperShapeRegistry registry)
        {
            const string Rwls = "ReaderWriterLockSlim";

            // ── Zero-argument entries: timeout is not a managed argument ──
            //
            // EnterReadLock() / EnterWriteLock() / EnterUpgradeableReadLock() /
            // Dispose() take no managed argument.  Their native counterparts DO
            // take a timeout, so the entry supplies the infinite sentinel itself
            // rather than pretending the managed signature has a parameter the
            // subject id does not list.
            //
            // Return slot is Void *in the managed sense* but NativeInt in the ABI:
            // the lowering emits `const auto chaos_result = <call>(...);`
            // unconditionally, so a Void return slot yields C3313.  This is the
            // same wall the TaskAwaiter.GetResult registration hit (7a89c72d7).
            registry.Register(Rwls, "EnterReadLock", [],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimEnterReadLockInfinite",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register(Rwls, "EnterWriteLock", [],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimEnterWriteLockInfinite",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register(Rwls, "EnterUpgradeableReadLock", [],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimEnterUpgradeableReadLockInfinite",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register(Rwls, "Dispose", [],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimDisposeManaged",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── Exit* : the family the chunk currently fakes most visibly ──
            //
            // ExitReadLock on an unheld lock throws SynchronizationLockException
            // in real .NET.  The chunk's generated body replaces that semantics
            // with `return 42L;` (see the [UNVERIFIED] marker), so wiring these to
            // a real helper is what gives the behavior back.
            registry.Register(Rwls, "ExitReadLock", [],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimExitReadLock",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register(Rwls, "ExitWriteLock", [],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimExitWriteLock",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register(Rwls, "ExitUpgradeableReadLock", [],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimExitUpgradeableReadLock",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── TryEnter*Lock — 现在能接了（A.2 receiver 注入生效）──
            // lowering 层现在为 instance 方法自动注入 receiver 槽位，
            // 所以 shim 的签名变为 (receiver, timeout) 而非仅有 timeout。
            registry.Register(Rwls, "TryEnterReadLock", ["System.TimeSpan"],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimTryEnterReadLockTimeSpan",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });
            registry.Register(Rwls, "TryEnterReadLock", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimTryEnterReadLockInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });
            registry.Register(Rwls, "TryEnterWriteLock", ["System.TimeSpan"],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimTryEnterWriteLockTimeSpan",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });
            registry.Register(Rwls, "TryEnterWriteLock", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimTryEnterWriteLockInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });
            registry.Register(Rwls, "TryEnterUpgradeableReadLock", ["System.TimeSpan"],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimTryEnterUpgradeableReadLockTimeSpan",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });
            registry.Register(Rwls, "TryEnterUpgradeableReadLock", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosReaderWriterLockSlimTryEnterUpgradeableReadLockInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });
        }

        /// <summary>
        /// T2.4: System.Threading.ManualResetEventSlim — instance→handle recovery.
        /// Same receiver-not-passed constraint as
        /// <see cref="RegisterReaderWriterLockSlimAndSemaphoreSlim"/>; see that
        /// method's doc block for why the receiver is recovered from the instance
        /// field rather than declared as an ABI slot.
        /// </summary>
        private static void RegisterManualResetEventSlim(RuntimeHelperShapeRegistry registry)
        {
            // ══════════════════════════════════════════════════════════════
            // T2.4: ManualResetEventSlim
            // ══════════════════════════════════════════════════════════════
            //
            // Same receiver-not-passed situation as the RWLock family above, and
            // the same recovery (instance field -> native handle).  Backed by the
            // T2.0 WaitHandle family with type discriminant 0 (manual), so no new
            // native object kind is introduced.
            //
            // NINE subject ids here, and the counting matters: `Wait` appears with
            // five distinct signatures because the chunk contains every overload.
            // Each needs its own canonical key — a single "Wait" registration would
            // silently leave four of them on the fallback path.
            //
            // The CancellationToken-bearing overloads are registered to the SAME
            // native entry as their token-less siblings.  That is a deliberate
            // simplification with a REAL limitation, recorded here so it is not
            // mistaken for full support:
            //
            //   The native entry ignores the token.  A token that is ALREADY
            //   cancelled therefore does not produce the immediate
            //   OperationCanceledException that managed code expects; the wait
            //   proceeds on its timeout instead.  Wiring cancellation properly
            //   requires the entry to consult the token and raise, which needs the
            //   token's handle ABI (cancellation_token_stubs.h) plus a managed
            //   exception from this call site — neither exists yet.
            //
            // So: the wait semantics are real, the cancellation semantics are not.
            // Marked TODO rather than left as an unstated gap.
            //
            // TODO(t2.4): honour the CancellationToken (is_cancellation_requested
            // -> raise OperationCanceledException) once the token ABI reaches
            // these call sites.
            const string Mres = "ManualResetEventSlim";

            registry.Register(Mres, "Set", [],
                ShapeKind.SimpleForward, "ChaosManualResetEventSlimSetManaged",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register(Mres, "Reset", [],
                ShapeKind.SimpleForward, "ChaosManualResetEventSlimResetManaged",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register(Mres, "Dispose", [],
                ShapeKind.SimpleForward, "ChaosManualResetEventSlimDisposeManaged",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // Wait() — no managed argument.
            registry.Register(Mres, "Wait", [],
                ShapeKind.SimpleForward, "ChaosManualResetEventSlimWaitManaged",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // Wait(int) / Wait(TimeSpan) / Wait(int, token) / Wait(TimeSpan, token)
            // are NOW WIRED — the A.2 receiver injection in
            // CreateDefinitionFromShapeEntry() prepends the receiver slot for
            // instance methods, so parameterised overloads work too.

            // Wait(int) and Wait(TimeSpan) are DIFFERENT canonical keys.
            registry.Register(Mres, "Wait", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosManualResetEventSlimWaitInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register(Mres, "Wait", ["System.TimeSpan"],
                ShapeKind.SimpleForward, "ChaosManualResetEventSlimWaitTimeSpan",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // The token-bearing overloads — see the TODO above.  The token slot is
            // declared (the subject id lists it, so the lowering will pop it) but
            // the native entry does not read it.
            registry.Register(Mres, "Wait", ["System.Threading.CancellationToken"],
                ShapeKind.SimpleForward, "ChaosManualResetEventSlimWaitManaged",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register(Mres, "Wait", ["System.Int32", "System.Threading.CancellationToken"],
                ShapeKind.SimpleForward, "ChaosManualResetEventSlimWaitInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    CreateNativeIntAbiSlot(),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register(Mres, "Wait", ["System.TimeSpan", "System.Threading.CancellationToken"],
                ShapeKind.SimpleForward, "ChaosManualResetEventSlimWaitTimeSpan",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot(),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });
        }

        /// <summary>
        /// T2.5: SpinLock + SpinWait + the ThreadPool callable surface.
        ///
        /// VALUE-TYPE RECEIVERS.  SpinLock and SpinWait are structs, so unlike the
        /// classes above there is no handle to recover — the receiver pointer IS
        /// the state, and the native entries read/write it in place.  That changes
        /// the failure mode worth guarding: for the handle-based types the risk
        /// was "operated on no object"; here it is "operated on a COPY and threw
        /// the result away", which looks identical from the caller's side.  The
        /// byref write-back below is the case where that would be caught.
        ///
        /// THE BYREF SLOT.  `Enter(ref bool lockTaken)` / `TryEnter(ref bool)` /
        /// `TryEnter(int, ref bool)` / `TryEnter(TimeSpan, ref bool)` carry the
        /// bool as `AotCoreIrAbiCarrierKind.ByRef` (= CHAOS_IL2CPP_INTPTR, a raw
        /// pointer), the same encoding `System.Enum.TryParse(System.Object&)` uses
        /// (Part3.S23.cs).  It must be listed in rawArgumentIndices so the
        /// lowering passes the POINTER rather than dereferencing to a bool value.
        ///
        /// This is load-bearing, not decoration: the chunk's generated test is
        ///     bool __ref_0_0_0 = default;
        ///     ...Create&lt;SpinLock&gt;().Enter(ref __ref_0_0_0);
        ///     Assert.AreEqual(true, __ref_0_0_0);
        /// so a helper that acquired the lock but did not write the bool fails.
        ///
        /// SCOPE NOTE — what is deliberately NOT registered.  The chunk also
        /// contains ThreadPool.GetMaxThreads/GetMinThreads/GetAvailableThreads/
        /// SetMaxThreads/SetMinThreads.  The native pool has no min/max
        /// configuration surface at all, so there is nothing to forward to.
        /// Registering them against a placeholder would replace one silent wrong
        /// answer (fallback 0) with another, which is exactly the defect class
        /// this phase removes — so they are left on the fallback path where their
        /// absence stays visible.
        /// </summary>
        private static void RegisterSpinPrimitivesAndThreadPool(RuntimeHelperShapeRegistry registry)
        {
            const string SpinLock = "SpinLock";
            const string SpinWait = "SpinWait";
            const string ThreadPool = "ThreadPool";

            // ── SpinLock ──
            //
            // Enter() returns void in managed but the lowering assigns the result
            // to a local unconditionally, so the return slot is NativeInt (the
            // C3313 wall again — see the TaskAwaiter.GetResult note above).

            // Enter(ref bool)
            registry.Register(SpinLock, "Enter", ["System.Boolean&"],
                ShapeKind.SimpleForward, "ChaosSpinLockEnter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // TryEnter(ref bool)
            registry.Register(SpinLock, "TryEnter", ["System.Boolean&"],
                ShapeKind.SimpleForward, "ChaosSpinLockTryEnter",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // TryEnter(int, ref bool)
            registry.Register(SpinLock, "TryEnter", ["System.Int32", "System.Boolean&"],
                ShapeKind.SimpleForward, "ChaosSpinLockTryEnterInt32",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateInt32AbiSlot(),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef },
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // TryEnter(TimeSpan, ref bool) — different canonical key from both.
            registry.Register(SpinLock, "TryEnter", ["System.TimeSpan", "System.Boolean&"],
                ShapeKind.SimpleForward, "ChaosSpinLockTryEnterTimeSpan",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.ByRef },
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            // Exit() — no argument.  Note: managed SpinLock.Exit() is void and
            // returns nothing observable, so the return slot value is discarded.
            registry.Register(SpinLock, "Exit", [],
                ShapeKind.SimpleForward, "ChaosSpinLockExit",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── SpinWait ──

            registry.Register(SpinWait, "SpinOnce", [],
                ShapeKind.SimpleForward, "ChaosSpinWaitSpinOnce",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateNativeIntAbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            registry.Register(SpinWait, "SpinOnce", ["System.Int32"],
                ShapeKind.SimpleForward, "ChaosSpinWaitSpinOnceInt32",
                new _003C_003Ez__ReadOnlySingleElementList<AotCoreIrAbiSlotArtifact>(
                    CreateInt32AbiSlot()),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0 });

            // ── ThreadPool — callable surface only ──
            //
            // Two of the three are ASSEMBLY-QUALIFIED subject ids in the chunk:
            //   System.Threading.ThreadPool/System.Threading.ThreadPool::QueueUserWorkItem<...>
            // so they are registered under the qualified type display name.  See
            // the note above on why Get*/Set*Threads are absent.

            registry.Register("System.Threading.ThreadPool", "QueueUserWorkItem",
                ["System.Action`1", "System.Int32", "System.Boolean"],
                ShapeKind.SimpleForward, "ChaosThreadPoolQueueUserWorkItemManaged",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot(),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });

            registry.Register("System.Threading.ThreadPool", "UnsafeQueueUserWorkItem",
                ["System.Action`1", "System.Int32", "System.Boolean"],
                ShapeKind.SimpleForward, "ChaosThreadPoolQueueUserWorkItemUnsafeManaged",
                new _003C_003Ez__ReadOnlyArray<AotCoreIrAbiSlotArtifact>(new AotCoreIrAbiSlotArtifact[2]
                {
                    CreateNativeIntAbiSlot(),
                    CreateNativeIntAbiSlot(),
                }),
                CreateNativeIntAbiSlot(),
                new HashSet<int> { 0, 1 });
        }

    }
}
