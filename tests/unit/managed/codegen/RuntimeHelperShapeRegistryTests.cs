using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class RuntimeHelperShapeRegistryTests
{
    private NativeAotLoweringPlanner.RuntimeHelperShapeRegistry CreateRegistry()
    {
        return new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry();
    }

    private static AotCoreIrAbiSlotArtifact VoidAbi => new()
    {
        CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
    };

    private static AotCoreIrAbiSlotArtifact Int32Abi => new()
    {
        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
        TypeSubjectId = "System.Int32",
    };

    private static AotCoreIrAbiSlotArtifact RefAbi => new()
    {
        CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt,
        TypeSubjectId = "System.Object",
    };

    // ── Fnv1aHash ───────────────────────────────────────────

    [Fact]
    public void Fnv1aHash_EmptyString_ReturnsBase()
    {
        var hash = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.Fnv1aHash("");
        Assert.Equal(2166136261u, hash);
    }

    [Fact]
    public void Fnv1aHash_KnownString_Deterministic()
    {
        var h1 = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.Fnv1aHash("System.GC::KeepAlive(System.Object)");
        var h2 = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.Fnv1aHash("System.GC::KeepAlive(System.Object)");
        Assert.Equal(h1, h2);
    }

    [Fact]
    public void Fnv1aHash_DifferentStrings_DifferentHashes()
    {
        var h1 = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.Fnv1aHash("A");
        var h2 = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.Fnv1aHash("B");
        Assert.NotEqual(h1, h2);
    }

    // ── BuildCanonicalKey ────────────────────────────────────

    [Theory]
    [InlineData(new[] { "System.Int32" }, "System.GC::KeepAlive(System.Int32)")]
    [InlineData(new string[] { }, "System.GC::KeepAlive()")]
    [InlineData(new[] { "System.Int32", "System.String" }, "System.GC::KeepAlive(System.Int32,System.String)")]
    public void BuildCanonicalKey_ValidInputs_ReturnsExpectedKey(string[] paramTypes, string expected)
    {
        var key = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildCanonicalKey(
            "System.GC", "KeepAlive", paramTypes);
        Assert.Equal(expected, key);
    }

    // ── BuildShapeEnumName ──────────────────────────────────

    [Fact]
    public void BuildShapeEnumName_NoParams_ReturnsEnumName()
    {
        var name = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildShapeEnumName(
            "System.GC", "KeepAlive", Array.Empty<string>());
        Assert.Contains("SYSTEM_GC_KEEPALIVE", name);
    }

    [Fact]
    public void BuildShapeEnumName_WithParams_IncludesParamNames()
    {
        var name = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildShapeEnumName(
            "System.GC", "Collect", new[] { "System.Int32" });
        Assert.Contains("SYSTEM_INT32", name);
    }

    // ── Register / ShapeEntry ───────────────────────────────

    [Fact]
    public void Register_NewShape_ReturnsEntry()
    {
        var registry = CreateRegistry();
        var entry = registry.Register(
            "System.GC", "KeepAlive",
            new[] { "System.Object" },
            NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.SimpleForward,
            "chaos_gc_keepalive",
            new[] { RefAbi }, VoidAbi);

        Assert.NotNull(entry);
        Assert.Equal("System.GC::KeepAlive(System.Object)", entry.CanonicalKey);
        Assert.Equal("chaos_gc_keepalive", entry.NativeFnSymbol);
        Assert.Equal(NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.SimpleForward, entry.Kind);
    }

    [Fact]
    public void Register_DuplicateShape_Throws()
    {
        var registry = CreateRegistry();
        registry.Register(
            "System.GC", "KeepAlive", new[] { "System.Object" },
            NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.SimpleForward,
            "chaos_gc_keepalive", new[] { RefAbi }, VoidAbi);

        Assert.Throws<InvalidOperationException>(() =>
            registry.Register(
                "System.GC", "KeepAlive", new[] { "System.Object" },
                NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.InlineBody,
                "chaos_gc_keepalive", new[] { RefAbi }, VoidAbi));
    }

    [Fact]
    public void EntryCount_AfterRegister_ReturnsCorrectCount()
    {
        var registry = CreateRegistry();
        Assert.Equal(0, registry.EntryCount);

        registry.Register(
            "System.GC", "KeepAlive", new[] { "System.Object" },
            NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.SimpleForward,
            "chaos_gc_keepalive", new[] { RefAbi }, VoidAbi);
        Assert.Equal(1, registry.EntryCount);
    }

    [Fact]
    public void Register_WithRawArgumentIndices_StoresThem()
    {
        var registry = CreateRegistry();
        var entry = registry.Register(
            "System.Runtime.CompilerServices.Unsafe", "Unbox",
            new[] { "System.Object" },
            NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.InlineBody,
            "Unsafe_Unbox", new[] { RefAbi }, Int32Abi,
            rawArgumentIndices: new HashSet<int> { 0 });

        Assert.Contains(0, entry.RawArgumentIndices!);
    }

    [Fact]
    public void Register_WithReferencedStaticFields_StoresThem()
    {
        var registry = CreateRegistry();
        var entry = registry.Register(
            "System.GC", "Collect", Array.Empty<string>(),
            NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.SimpleForward,
            "chaos_gc_collect", Array.Empty<AotCoreIrAbiSlotArtifact>(), VoidAbi,
            referencedStaticFieldSubjectIds: new HashSet<string> { "System.Environment::s_hasShutdownStarted" });

        Assert.Contains("System.Environment::s_hasShutdownStarted", entry.ReferencedStaticFieldSubjectIds!);
    }

    // ── TryMatchShape ───────────────────────────────────────

    [Fact]
    public void TryMatchShape_RegisteredShape_ReturnsEntry()
    {
        var registry = CreateRegistry();
        registry.Register(
            "System.GC", "KeepAlive", new[] { "System.Object" },
            NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.SimpleForward,
            "chaos_gc_keepalive", new[] { RefAbi }, VoidAbi);

        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.GC::KeepAlive:System.Void(System.Object)",
                out var entry));
        Assert.NotNull(entry);
        Assert.Equal("chaos_gc_keepalive", entry.NativeFnSymbol);
    }

    [Fact]
    public void TryMatchShape_UnregisteredShape_ReturnsFalse()
    {
        var registry = CreateRegistry();
        Assert.False(registry.TryMatchShape("System.Foo::Bar()", out _));
    }

    [Fact]
    public void TryMatchShape_NullCallee_ReturnsFalse()
    {
        var registry = CreateRegistry();
        Assert.False(registry.TryMatchShape(null!, out _));
    }

    [Fact]
    public void TryMatchShape_EmptyCallee_ReturnsFalse()
    {
        var registry = CreateRegistry();
        Assert.False(registry.TryMatchShape("", out _));
    }

    [Fact]
    public void TryMatchShape_InvalidCalleeFormat_ReturnsFalse()
    {
        var registry = CreateRegistry();
        Assert.False(registry.TryMatchShape("no-slash-or-colon", out _));
    }

    // ── RegisterGeneric / TryMatchGenericShape ──────────────

    [Fact]
    public void RegisterGeneric_And_MatchByExactName()
    {
        var registry = CreateRegistry();
        registry.RegisterGeneric(
            new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.GenericShapeDescriptor(
                "System.Collections.Generic.Comparer",
                "get_Default",
                (planner, callee, typeArgs) =>
                    new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.GenericShapeResolution(
                        "// generic call", "Comparer_get_Default",
                        Array.Empty<AotCoreIrAbiSlotArtifact>(), Int32Abi,
                        new HashSet<int>())));

        Assert.True(
            registry.TryMatchGenericShape(
                "System.Private.CoreLib/System.Collections.Generic.Comparer::get_Default:()",
                out var descriptor, out var typeArgs));
        Assert.NotNull(descriptor);
    }

    [Fact]
    public void TryMatchGenericShape_NullCallee_ReturnsFalse()
    {
        var registry = CreateRegistry();
        Assert.False(registry.TryMatchGenericShape(null!, out _, out _));
    }

    [Fact]
    public void TryMatchGenericShape_EmptyCallee_ReturnsFalse()
    {
        var registry = CreateRegistry();
        Assert.False(registry.TryMatchGenericShape("", out _, out _));
    }

    // ── TryMatchGenericShape: [[...]] method-level generic args ────────

    [Fact]
    public void TryMatchGenericShape_DoubleBracketSyntax_ExtractsTypeArgs()
    {
        var registry = CreateRegistry();
        registry.RegisterGeneric(
            new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.GenericShapeDescriptor(
                "System.Collections.Generic.EqualityComparer",
                "Equals",
                (planner, callee, typeArgs) =>
                    new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.GenericShapeResolution(
                        "// generic call", "EqualityComparer_Equals",
                        Array.Empty<AotCoreIrAbiSlotArtifact>(), Int32Abi,
                        new HashSet<int>())));

        Assert.True(
            registry.TryMatchGenericShape(
                "System.Private.CoreLib/System.Collections.Generic.EqualityComparer::Equals[[System.Int32]]:System.Boolean(System.Int32,System.Int32)",
                out var descriptor, out var typeArgs));
        Assert.NotNull(descriptor);
        Assert.Contains("System.Int32", typeArgs);
    }

    // ── TryMatchGenericShape: <...> angle-bracket syntax ───────────────

    [Fact]
    public void TryMatchGenericShape_AngleBracketSyntax_ExtractsTypeArgs()
    {
        var registry = CreateRegistry();
        registry.RegisterGeneric(
            new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.GenericShapeDescriptor(
                "System.Collections.Generic.List",
                "Sort",
                (planner, callee, typeArgs) =>
                    new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.GenericShapeResolution(
                        "// generic call", "List_Sort",
                        Array.Empty<AotCoreIrAbiSlotArtifact>(), Int32Abi,
                        new HashSet<int>())));

        Assert.True(
            registry.TryMatchGenericShape(
                "System.Private.CoreLib/System.Collections.Generic.List::Sort<System.Int32>:System.Void()",
                out var descriptor, out var typeArgs));
        Assert.NotNull(descriptor);
        Assert.Contains("System.Int32", typeArgs);
    }

    [Fact]
    public void TryMatchGenericShape_AngleBracket_WrongTypePrefix_ReturnsFalse()
    {
        var registry = CreateRegistry();
        registry.RegisterGeneric(
            new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.GenericShapeDescriptor(
                "System.Collections.Generic.List",
                "Sort",
                (planner, callee, typeArgs) =>
                    new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.GenericShapeResolution(
                        "// generic call", "List_Sort",
                        Array.Empty<AotCoreIrAbiSlotArtifact>(), Int32Abi,
                        new HashSet<int>())));

        // Array::Sort<...> should not match List<T>::Sort descriptor
        Assert.False(
            registry.TryMatchGenericShape(
                "System.Private.CoreLib/System.Array::Sort<System.Int32>:System.Void()",
                out _, out _));
    }

    // ── TryMatchGenericShape: backtick type name matching ──────────────

    [Fact]
    public void TryMatchGenericShape_BacktickTypeName_ExtractsTypeArgs()
    {
        var registry = CreateRegistry();
        registry.RegisterGeneric(
            new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.GenericShapeDescriptor(
                "System.Nullable`1",
                "get_HasValue",
                (planner, callee, typeArgs) =>
                    new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.GenericShapeResolution(
                        "// generic call", "Nullable_get_HasValue",
                        Array.Empty<AotCoreIrAbiSlotArtifact>(), Int32Abi,
                        new HashSet<int>())));

        Assert.True(
            registry.TryMatchGenericShape(
                "System.Private.CoreLib/System.Nullable<System.Int32>::get_HasValue:()",
                out var descriptor, out var typeArgs));
        Assert.NotNull(descriptor);
        Assert.Contains("System.Int32", typeArgs);
    }

    [Fact]
    public void TryMatchGenericShape_BacktickTypeName_WrongBase_ReturnsFalse()
    {
        var registry = CreateRegistry();
        registry.RegisterGeneric(
            new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.GenericShapeDescriptor(
                "System.Nullable`1",
                "get_HasValue",
                (planner, callee, typeArgs) =>
                    new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.GenericShapeResolution(
                        "// generic call", "Nullable_get_HasValue",
                        Array.Empty<AotCoreIrAbiSlotArtifact>(), Int32Abi,
                        new HashSet<int>())));

        // Span<System.Byte> should not match Nullable`1 descriptor
        Assert.False(
            registry.TryMatchGenericShape(
                "System.Private.CoreLib/System.Span<System.Byte>::get_Item(System.Int32)",
                out _, out _));
    }

    // ── RegisterInline / TryMatchInlineShape ────────────────

    [Fact]
    public void RegisterInline_And_MatchByPrefix()
    {
        var registry = CreateRegistry();
        registry.RegisterInline(
            new NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.InlineShapeDescriptor(
                "System.GC",
                "KeepAlive",
                (callee, paramTypes) => "// inline: chaos_gc_keepalive(arg0)"));

        Assert.True(
            registry.TryMatchInlineShape(
                "System.Private.CoreLib/System.GC::KeepAlive:System.Void(System.Object)",
                out var expression, out var inlineDescriptor));
        Assert.NotNull(inlineDescriptor);
        Assert.NotNull(expression);
    }

    [Fact]
    public void TryMatchInlineShape_NullCallee_ReturnsFalse()
    {
        var registry = CreateRegistry();
        Assert.False(registry.TryMatchInlineShape(null!, out _, out _));
    }

    [Fact]
    public void TryMatchInlineShape_EmptyCallee_ReturnsFalse()
    {
        var registry = CreateRegistry();
        Assert.False(registry.TryMatchInlineShape("", out _, out _));
    }

    // ── BuildDefault: TryMatchShape for registered shapes ──────────────

    [Fact]
    public void BuildDefault_TryMatchShape_StringOpEquality_ReturnsEntry()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.String::op_Equality:System.Boolean(System.String,System.String)",
                out var entry));
        Assert.NotNull(entry);
        Assert.Equal("chaos_object_equals", entry.NativeFnSymbol);
    }

    [Fact]
    public void BuildDefault_TryMatchShape_StringStartsWith_ReturnsEntry()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String,System.StringComparison)",
                out var entry));
        Assert.NotNull(entry);
    }

    [Fact]
    public void BuildDefault_TryMatchShape_GcCollect_ReturnsInlineBody()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.GC::Collect:System.Void()",
                out var entry));
        Assert.NotNull(entry);
        Assert.Equal(NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.InlineBody, entry.Kind);
    }

    [Fact]
    public void BuildDefault_TryMatchShape_GcCollectWithMode_ReturnsEntry()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.GC::Collect:System.Void(System.Int32,System.GCCollectionMode)",
                out var entry));
        Assert.NotNull(entry);
    }

    // ══════════════════════════════════════════════════════════════════════
    // ASYNC-P2-8 A3 — AsyncIteratorMethodBuilder registration.
    //
    // This is the layer the parser tests (NativeAotPlannerHelperTests) do NOT
    // reach: those check that a callee can be PARSED, these check that the real
    // production registry (BuildDefault) actually MATCHES the form the pipeline
    // emits and routes it to the A2 native symbol.
    //
    // A registration keyed on a plausible-looking but wrong prefix is the exact
    // failure P2-4 cost us before: the registry simply never fires, the call
    // falls through to ChaosExternalRuntimeFallback -> 0, and everything stays
    // green.  So every assertion here names the native symbol — matching without
    // checking the destination would pass even against a stub registration.
    // ══════════════════════════════════════════════════════════════════════

    private const string IteratorBuilderCalleePrefix =
        "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncIteratorMethodBuilder";

    [Fact]
    public void BuildDefault_IteratorBuilder_CreateResolverEmitsTheRealNativeSymbol()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchGenericShape(
                IteratorBuilderCalleePrefix + "::Create()",
                out var descriptor, out _));
        Assert.Equal("Create", descriptor.MethodName);
        Assert.Equal(
            "System.Runtime.CompilerServices.AsyncIteratorMethodBuilder",
            descriptor.TypeDisplayNamePrefix);

        // Matching is not the claim under test — ROUTING is.  Invoke the resolver and
        // assert the emitted C++ calls the A2 native entry point.  Without this, a
        // registration whose body is a constant-return stub would still pass.
        var resolution = descriptor.Resolver(null!, IteratorBuilderCalleePrefix + "::Create()",
            Array.Empty<string>());
        Assert.NotNull(resolution);
        Assert.Equal("chaos_async_iterator_builder_create", resolution.DirectNativeSymbol);
        Assert.Contains("chaos_async_iterator_builder_create", resolution.CppSource);
    }

    /// <summary>
    /// MoveNext resolves the state machine's native symbol through the planner, so it
    /// cannot be driven without one — but its registration must still MATCH, otherwise
    /// the iterator never advances.  (Resolution behaviour is covered by the parser
    /// tests plus the A2 runtime tests.)
    /// </summary>
    [Fact]
    public void BuildDefault_TryMatchGenericShape_IteratorBuilder_MoveNext()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchGenericShape(
                IteratorBuilderCalleePrefix + "::MoveNext<StateMachine>(StateMachine&)",
                out var descriptor, out _));
        Assert.Equal("MoveNext", descriptor.MethodName);
    }

    /// <summary>
    /// Complete must match and route to its native entry point.  An iterator that never
    /// calls Complete leaves its pooled sources unreleased (the A2 `destroy` note).
    /// </summary>
    [Fact]
    public void BuildDefault_IteratorBuilder_CompleteResolverEmitsTheRealNativeSymbol()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchGenericShape(
                IteratorBuilderCalleePrefix + "::Complete()",
                out var descriptor, out _));

        var resolution = descriptor.Resolver(null!, IteratorBuilderCalleePrefix + "::Complete()",
            Array.Empty<string>());
        Assert.NotNull(resolution);
        Assert.Equal("chaos_async_iterator_builder_complete", resolution.DirectNativeSymbol);
        Assert.Contains("chaos_async_iterator_builder_complete", resolution.CppSource);
    }

    /// <summary>
    /// BOTH await spellings must be REGISTERED (not merely parseable).
    ///
    /// This is the registry-level counterpart of the recon doc's §1.1 finding: the
    /// original A2 plan registered only AwaitOnCompleted, so `await Task.Yield()`
    /// inside an iterator — which can reach the unsafe form — would have had no
    /// registration and silently fallen through to ChaosExternalRuntimeFallback.
    ///
    /// Counterexample: delete either entry from the registration loop's array and that
    /// row goes red while the other stays green.
    /// </summary>
    [Theory]
    [InlineData("AwaitOnCompleted")]
    [InlineData("AwaitUnsafeOnCompleted")]
    public void BuildDefault_TryMatchGenericShape_IteratorBuilder_BothAwaitSpellings(string methodName)
    {
        const string smSubjectId = "System.Private.CoreLib/IteratorSm";
        var planner = CreatePlannerWithIteratorMoveNext(smSubjectId);

        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        string callee =
            $"{IteratorBuilderCalleePrefix}::{methodName}<Awaiter,IteratorSm>(Awaiter&,IteratorSm&)";
        Assert.True(
            registry.TryMatchGenericShape(callee, out var descriptor, out _),
            $"{methodName} is not registered — an iterator awaiting through this "
            + "spelling would fall through to the external-runtime stub and stall");
        Assert.Equal(methodName, descriptor.MethodName);

        // Both spellings route to the SAME native entry, wired to the state machine's
        // real MoveNext so the resume continues the right iterator.
        var resolution = descriptor.Resolver(planner, callee, Array.Empty<string>());
        Assert.NotNull(resolution);
        Assert.Contains("chaos_async_iterator_builder_await", resolution!.CppSource);
        Assert.Contains(IteratorSmMoveNextSymbol, resolution.CppSource);
    }

    /// <summary>
    /// The iterator registrations must NOT answer for the async TASK builder.
    ///
    /// Both builders declare MoveNext / AwaitOnCompleted / AwaitUnsafeOnCompleted with
    /// identical signatures, so an over-broad prefix would capture the async Task path
    /// and route it through the iterator runtime — corrupting every ordinary
    /// `async Task` method in the program.  This asserts the prefixes stay disjoint.
    /// </summary>
    [Fact]
    public void BuildDefault_TryMatchGenericShape_TaskBuilder_IsNotAnsweredByIteratorRegistration()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        string taskBuilderStart =
            "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder::Start<StateMachine>(StateMachine&)";

        Assert.True(registry.TryMatchGenericShape(taskBuilderStart, out var descriptor, out _));
        Assert.Equal(
            "System.Runtime.CompilerServices.AsyncTaskMethodBuilder",
            descriptor.TypeDisplayNamePrefix);
        Assert.DoesNotContain("Iterator", descriptor.TypeDisplayNamePrefix, StringComparison.Ordinal);
    }

    // ── Task.Run registration (ASYNC-P1-1) ─────────────────────────────────
    // Task::Run delegates to the native async_task_run (already fully
    // implemented in task_runner.cpp, registered at RuntimeInit, but had no
    // codegen entry point — making it dead code from managed callers).
    // Task.Run is registered via RegisterGeneric, so it is matched through
    // TryMatchGenericShape, and the resolver emits a wrapper calling
    // async_task_run.

    [Theory]
    [InlineData("System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action)")]
    [InlineData("System.Private.CoreLib/System.Threading.Tasks.Task`1[[System.Int32]]::Run:System.Threading.Tasks.Task(System.Func`1<System.Int32>)")]
    public void TaskRun_WiredToGenericShape(string callee)
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchGenericShape(callee, out var descriptor, out _),
            $"Task::Run callee '{callee}' should match a generic registry descriptor");
        Assert.NotNull(descriptor);

        // The resolver must produce C++ that calls the native async_task_run symbol.
        var planner = new NativeAotLoweringPlanner();
        var resolution = descriptor!.Resolver(planner, callee, Array.Empty<string>());
        Assert.NotNull(resolution);
        Assert.Equal("async_task_run", resolution!.DirectNativeSymbol);
        Assert.Contains("async_task_run", resolution.CppSource);
    }

    [Fact]
    public void TaskRun_CancellationTokenOverload_ResolvesToNull()
    {
        // CT overloads deliberately fall through to the interpreter until
        // Phase 3 (CancellationToken wiring) — their resolver must return null
        // so codegen does not route them to async_task_run (which ignores the CT).
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(registry.TryMatchGenericShape(
            "System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action,System.Threading.CancellationToken)",
            out var descriptor, out _));
        Assert.NotNull(descriptor);

        var planner = new NativeAotLoweringPlanner();
        var resolution = descriptor!.Resolver(planner,
            "System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action,System.Threading.CancellationToken)",
            Array.Empty<string>());
        Assert.Null(resolution);
    }

    // ── Task.Factory registration (ASYNC-P2-5) ─────────────────────────────
    // Task.Factory.StartNew(delegate) queues on the default scheduler, which is
    // exactly where Task.Run queues, so the delegate-only StartNew overloads are
    // routed to chaos_task_factory_start_new (a shim over async_task_run).
    //
    // NOTE on callee spelling: these use the angle-bracket form the real
    // pipeline emits for cross-assembly BCL callees. Using the backtick form
    // let an earlier revision of the ContinueWith tests stay green while the
    // pipeline resolved everything to null — see ASYNC-P2-4.

    [Theory]
    // The form the real pipeline produces, verified by dumping the lowered
    // callees of AsyncMethods::FactoryStartNew.
    [InlineData("System.Private.CoreLib/System.Threading.Tasks.TaskFactory::StartNew:System.Threading.Tasks.Task(System.Action)")]
    public void TaskFactoryStartNew_DelegateOnlyOverload_RoutesToNative(string callee)
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchGenericShape(callee, out var descriptor, out _),
            $"TaskFactory::StartNew callee '{callee}' should match a generic registry descriptor");
        Assert.NotNull(descriptor);

        var planner = new NativeAotLoweringPlanner();
        var resolution = descriptor!.Resolver(planner, callee, Array.Empty<string>());
        Assert.NotNull(resolution);
        Assert.Equal("chaos_task_factory_start_new", resolution!.DirectNativeSymbol);
        Assert.Contains("chaos_task_factory_start_new", resolution.CppSource);
    }

    [Theory]
    // Overloads whose argument the runner cannot honour. Each must resolve to
    // null so the call falls through to the interpreter rather than silently
    // dropping the options / token / state / result-type argument.
    [InlineData("System.Private.CoreLib/System.Threading.Tasks.TaskFactory::StartNew:System.Threading.Tasks.Task(System.Action,System.Threading.CancellationToken)")]
    [InlineData("System.Private.CoreLib/System.Threading.Tasks.TaskFactory::StartNew:System.Threading.Tasks.Task(System.Action,System.Threading.Tasks.TaskCreationOptions)")]
    [InlineData("System.Private.CoreLib/System.Threading.Tasks.TaskFactory::StartNew:System.Threading.Tasks.Task(System.Action,System.Object)")]
    public void TaskFactoryStartNew_UnhonouredArgumentOverloads_ResolveToNull(string callee)
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        if (!registry.TryMatchGenericShape(callee, out var descriptor, out _))
        {
            // No descriptor matched at all — also an acceptable outcome (the call
            // falls to the interpreter either way), but it must not be silent.
            return;
        }

        var planner = new NativeAotLoweringPlanner();
        var resolution = descriptor!.Resolver(planner, callee, Array.Empty<string>());
        Assert.Null(resolution);
    }

    [Fact]
    public void TaskFactory_GetFactoryProperty_RoutesToNativeToken()
    {
        const string callee =
            "System.Private.CoreLib/System.Threading.Tasks.Task::get_Factory:System.Threading.Tasks.TaskFactory()";
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(registry.TryMatchGenericShape(callee, out var descriptor, out _));
        Assert.NotNull(descriptor);

        var planner = new NativeAotLoweringPlanner();
        var resolution = descriptor!.Resolver(planner, callee, Array.Empty<string>());
        Assert.NotNull(resolution);
        Assert.Equal("chaos_task_default_factory", resolution!.DirectNativeSymbol);
    }

    [Fact]
    // ASYNC-P2-6. The generic overload returns Task<int[]> where the non-generic
    // one returns Task, so both must reach the SAME array combinator. The guard
    // used to require the non-generic return type verbatim, which silently sent
    // Task.WhenAll<int> to the interpreter's return-0 fallback.
    //
    // The callee is copied VERBATIM from what the real pipeline hands the resolver
    // (dumped from the pipeline run over AsyncTestAssembly), not hand-written. Note
    // the generic form carries the type argument on the METHOD name —
    // `::WhenAll<System.Int32>:` — which is why a guard anchored on `::WhenAll(`
    // missed it. (P2-4/P2-5 lesson: hand-written callees diverge from the real
    // pipeline; a guessed non-generic spelling does not even match a descriptor.)
    public void WhenAll_GenericArrayOverload_RoutesToArrayCombinator()
    {
        const string callee =
            "System.Private.CoreLib/System.Threading.Tasks.Task::WhenAll<System.Int32>:System.Threading.Tasks.Task<System.Int32[]>(System.Threading.Tasks.Task<System.Int32>[])";
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(registry.TryMatchGenericShape(callee, out var descriptor, out _),
            $"WhenAll callee '{callee}' should match a generic registry descriptor");
        Assert.NotNull(descriptor);

        var planner = new NativeAotLoweringPlanner();
        var resolution = descriptor!.Resolver(planner, callee, Array.Empty<string>());
        Assert.NotNull(resolution);
        Assert.Equal("chaos_task_when_all_array", resolution!.DirectNativeSymbol);
        Assert.Contains("chaos_task_when_all_array", resolution.CppSource);
    }

    [Fact]
    // The single-argument guard must still reject multi-argument overloads —
    // Task.WhenAll(IEnumerable<Task>) and friends take something other than a
    // plain array, and the native shim only understands a contiguous array.
    public void WhenAll_NonArrayArgument_ResolvesToNull()
    {
        const string callee =
            "System.Private.CoreLib/System.Threading.Tasks.Task::WhenAll:System.Threading.Tasks.Task(System.Collections.Generic.IEnumerable`1<System.Threading.Tasks.Task>)";
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        if (!registry.TryMatchGenericShape(callee, out var descriptor, out _))
        {
            // No descriptor at all is also acceptable — it falls to the interpreter.
            return;
        }

        var planner = new NativeAotLoweringPlanner();
        Assert.Null(descriptor!.Resolver(planner, callee, Array.Empty<string>()));
    }

    [Fact]
    public void BuildDefault_TryMatchShape_GcGetTotalMemory_ReturnsEntry()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)",
                out var entry));
        Assert.NotNull(entry);
    }

    [Fact]
    public void BuildDefault_TryMatchShape_DelegateCombine_ReturnsEntry()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)",
                out var entry));
        Assert.NotNull(entry);
    }

    [Fact]
    public void BuildDefault_TryMatchShape_ObjectGetType_ReturnsEntry()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.Object::GetType:System.Type(System.Object)",
                out var entry));
        Assert.NotNull(entry);
    }

    [Fact]
    public void BuildDefault_TryMatchShape_StringContains_ReturnsEntry()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.String::Contains:System.Boolean(System.String,System.StringComparison)",
                out var entry));
        Assert.NotNull(entry);
    }

    [Fact]
    public void BuildDefault_TryMatchShape_ExceptionCtor_ReturnsEntry()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)",
                out var entry));
        Assert.NotNull(entry);
    }

    [Fact]
    public void BuildDefault_TryMatchShape_GcAddMemoryPressure_ReturnsEntry()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.GC::AddMemoryPressure:System.Void(System.Int64)",
                out var entry));
        Assert.NotNull(entry);
    }

    [Fact]
    public void BuildDefault_TryMatchShape_GcGetTotalPauseDuration_ReturnsEntry()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.GC::GetTotalPauseDuration:System.Int64()",
                out var entry));
        Assert.NotNull(entry);
    }

    // ── BuildDefault: TryMatchGenericShape for generic descriptors ─────

    [Fact]
    public void BuildDefault_TryMatchGenericShape_StringConcat3_ReturnsDescriptor()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchGenericShape(
                "System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)",
                out var descriptor, out var typeArgs));
        Assert.NotNull(descriptor);
    }

    [Fact]
    public void BuildDefault_TryMatchGenericShape_StringOpEquality_ReturnsDescriptor()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchGenericShape(
                "System.Private.CoreLib/System.String::op_Equality:System.Boolean(System.String,System.String)",
                out var descriptor, out var typeArgs));
        Assert.NotNull(descriptor);
    }

    [Fact]
    public void BuildDefault_TryMatchShape_Unregistered_ReturnsFalse()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.False(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.Nonexistent::Foo:System.Void()",
                out _));
    }

    [Fact]
    public void Entries_EmptyRegistry_ReturnsEmpty()
    {
        var registry = CreateRegistry();
        Assert.Empty(registry.Entries);
    }

    [Fact]
    public void Entries_AfterRegister_ContainsEntry()
    {
        var registry = CreateRegistry();
        registry.Register(
            "System.GC", "KeepAlive", new[] { "System.Object" },
            NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.SimpleForward,
            "chaos_gc_keepalive", new[] { RefAbi }, VoidAbi);

        var entry = Assert.Single(registry.Entries);
        Assert.Equal("chaos_gc_keepalive", entry.NativeFnSymbol);
    }

    // ── GenerateCppShapeHeader ──────────────────────────────

    [Fact]
    public void GenerateCppShapeHeader_WithShapes_ReturnsNonEmptyHeader()
    {
        var registry = CreateRegistry();
        registry.Register(
            "System.GC", "KeepAlive", new[] { "System.Object" },
            NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.SimpleForward,
            "chaos_gc_keepalive", new[] { RefAbi }, VoidAbi);

        var header = registry.GenerateCppShapeHeader();
        Assert.NotEmpty(header);
        Assert.Contains("chaos_gc_keepalive", header);
    }

    [Fact]
    public void GenerateCppShapeHeader_WithInlineBodyShapes_ExcludesFromDispatch()
    {
        var registry = CreateRegistry();
        // InlineBody shape (excluded from dispatch)
        registry.Register(
            "System.String", "Concat", new[] { "System.String", "System.String", "System.String" },
            NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.InlineBody,
            "ChaosReflectionConcatStringPairValues",
            new[] { RefAbi, RefAbi, RefAbi }, RefAbi,
            rawArgumentIndices: new HashSet<int> { 0, 1, 2 });

        var header = registry.GenerateCppShapeHeader();
        Assert.NotEmpty(header);
    }

    [Fact]
    public void GenerateCppShapeHeader_WithInt32Return_SeparatesEntries()
    {
        var registry = CreateRegistry();
        registry.Register(
            "System.String", "get_Length", Array.Empty<string>(),
            NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.ShapeKind.SimpleForward,
            "chaos_string_get_length",
            Array.Empty<AotCoreIrAbiSlotArtifact>(), Int32Abi);

        var header = registry.GenerateCppShapeHeader();
        Assert.NotEmpty(header);
        Assert.Contains("get_Length", header, StringComparison.OrdinalIgnoreCase);
    }

    [Fact]
    public void GenerateCppShapeHeader_EmptyRegistry_StillReturnsHeader()
    {
        var registry = CreateRegistry();
        var header = registry.GenerateCppShapeHeader();
        Assert.NotEmpty(header);
    }

    // ── BuildDefault ────────────────────────────────────────

    [Fact]
    public void BuildDefault_ReturnsNonEmptyRegistry()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.NotNull(registry);
        Assert.True(registry.EntryCount > 0);
    }

    [Fact]
    public void BuildDefault_IncludesGcKeepAlive()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.GC::KeepAlive:System.Void(System.Object)",
                out var entry));
        Assert.NotNull(entry);
        Assert.Equal("chaos_gc_keepalive", entry.NativeFnSymbol);
    }

    [Fact]
    public void BuildDefault_IncludesStringGetLength()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchShape(
                "System.Private.CoreLib/System.String::get_Length:System.Int32()",
                out var entry));
        Assert.NotNull(entry);
    }

    [Fact]
    public void BuildDefault_GenerateHeader_WithFullRegistry()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        var header = registry.GenerateCppShapeHeader();
        // Verify the header contains shape entries
        Assert.Contains("SHAPE_", header);
        Assert.NotEmpty(header);
    }

    [Fact]
    public void BuildDefault_TryMatchGenericShape_AllMarshalMethods_Match()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        var marshalMethods = new[]
        {
            ("System.Private.CoreLib/System.Runtime.InteropServices.Marshal::GetLastPInvokeError():System.Int32", "GetLastPInvokeError"),
            ("System.Private.CoreLib/System.Runtime.InteropServices.Marshal::GetHRForLastWin32Error():System.Int32", "GetHRForLastWin32Error"),
            ("System.Private.CoreLib/System.Runtime.InteropServices.Marshal::SetLastPInvokeError:System.Void(System.Int32)", "SetLastPInvokeError"),
            ("System.Private.CoreLib/System.Runtime.InteropServices.Marshal::GetExceptionCode():System.Int32", "GetExceptionCode"),
            ("System.Private.CoreLib/System.Runtime.InteropServices.Marshal::GetExceptionPointers():System.IntPtr", "GetExceptionPointers"),
            ("System.Private.CoreLib/System.Runtime.InteropServices.Marshal::AreComObjectsAvailableForCleanup():System.Int32", "AreComObjectsAvailableForCleanup"),
        };
        foreach (var (callee, methodName) in marshalMethods)
        {
            Assert.True(
                registry.TryMatchGenericShape(callee, out var descriptor, out _),
                $"TryMatchGenericShape should match {methodName}");
            Assert.Equal(methodName, descriptor!.MethodName);
        }
    }

    [Fact]
    public void BuildDefault_TryMatchGenericShape_ShortNameMarshal_Match()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        var marshalMethods = new[]
        {
            ("System.Private.CoreLib/Marshal::GetLastPInvokeError():System.Int32", "GetLastPInvokeError"),
            ("System.Private.CoreLib/Marshal::GetHRForLastWin32Error():System.Int32", "GetHRForLastWin32Error"),
            ("System.Private.CoreLib/Marshal::SetLastPInvokeError:System.Void(System.Int32)", "SetLastPInvokeError"),
            ("System.Private.CoreLib/Marshal::GetExceptionCode():System.Int32", "GetExceptionCode"),
            ("System.Private.CoreLib/Marshal::GetExceptionPointers():System.IntPtr", "GetExceptionPointers"),
            ("System.Private.CoreLib/Marshal::AreComObjectsAvailableForCleanup():System.Int32", "AreComObjectsAvailableForCleanup"),
        };
        foreach (var (callee, methodName) in marshalMethods)
        {
            Assert.True(
                registry.TryMatchGenericShape(callee, out var descriptor, out _),
                $"Short-name TryMatchGenericShape should match {methodName}");
            Assert.Equal(methodName, descriptor!.MethodName);
        }
    }

    // ── COM marshaller placeholder shapes (Built from 454c84f33) ────────
    // ComInterfaceMarshaller<Int32>/UniqueComInterfaceMarshaller<Int32>
    // .ConvertToUnmanaged must resolve via BuildDefault() to the native
    // ChaosComInterfaceMarshallerConvertToUnmanaged placeholder so C++ AOT
    // returns a non-null COM pointer (matching C#), not ExternalRuntimeFallback→null.

    [Fact]
    public void BuildDefault_ComInterfaceMarshallerConvertToUnmanaged_Resolves()
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        var comSubjects = new[]
        {
            "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller<System.Int32>::ConvertToUnmanaged:System.Void*(System.Int32)",
            "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller<System.Int32>::ConvertToUnmanaged:System.Void*(System.Int32)",
        };
        foreach (var callee in comSubjects)
        {
            Assert.True(
                registry.TryMatchGenericShape(callee, out var descriptor, out var typeArgs),
                $"TryMatchGenericShape should match COM ConvertToUnmanaged for {callee}");
            Assert.Equal("ConvertToUnmanaged", descriptor!.MethodName);

            // Resolver lambda must yield the native placeholder body.
            var resolution = descriptor.Resolver(null!, callee, typeArgs!);
            Assert.NotNull(resolution);
            Assert.Contains("ChaosComInterfaceMarshallerConvertToUnmanaged()", resolution!.CppSource);
        }
    }

    // ── Task.ContinueWith overload surface (ASYNC-P2-4) ────────────────────
    //
    // Task.ContinueWith has 20 public overloads on .NET 8 (enumerated from the
    // shipped CoreLib; see the test-class comment).  Only ONE was wired
    // (Action<Task>), so every other overload silently fell through to the
    // interpreter.  The correct routing is not "wire all 20" — it is a
    // per-family decision, and the decision is what these tests pin:
    //
    //   WIRED  — overloads whose full semantics the native helper honours:
    //            Action<Task> / Func<Task,TResult> (the return value becomes the
    //            continuation task's result, which is what makes ContinueWith
    //            chainable).
    //   NULL   — overloads carrying CancellationToken / TaskContinuationOptions
    //            / TaskScheduler / object-state.  Routing these to the native
    //            helper would RUN THE CONTINUATION ANYWAY while ignoring the
    //            argument the caller supplied — the exact fake-green this phase
    //            exists to eliminate.  They return null and fall through to the
    //            interpreter until Phase 3 provides real CT/options semantics.
    //
    // This mirrors TaskRun_CancellationTokenOverload_ResolvesToNull: an
    // unhonoured argument must be a visible gap, not a plausible-looking call.

    [Theory]
    // NOTE: the callee strings here are the form the REAL pipeline produces for
    // a cross-assembly BCL callee — angle-bracket generics, not backtick arity.
    // Using the backtick form let an earlier revision of this test stay green
    // while every overload resolved to null in the actual pipeline.
    [InlineData("System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action<System.Threading.Tasks.Task>)")]
    // Assembly-qualified generic arguments, the form real subject ids actually
    // use: note the comma INSIDE the brackets.  A naive Split(',') turns this
    // one parameter into two fragments and the overload never matches.
    [InlineData("System.Private.CoreLib/System.Threading.Tasks.Task`1[[System.Int32]]::ContinueWith:System.Threading.Tasks.Task`1[[System.Int32]](System.Func`2[[System.Threading.Tasks.Task`1[[System.Int32]], System.Private.CoreLib, Version=8.0.0.0, Culture=neutral, PublicKeyToken=7cec85d7bea7798e],[System.Int32, System.Private.CoreLib, Version=8.0.0.0, Culture=neutral, PublicKeyToken=7cec85d7bea7798e]])")]
    public void ContinueWith_DelegateOnlyOverloads_RouteToNative(string callee)
    {
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchGenericShape(callee, out var descriptor, out _),
            $"ContinueWith callee '{callee}' should match a generic registry descriptor");
        Assert.NotNull(descriptor);

        var planner = new NativeAotLoweringPlanner();
        var resolution = descriptor!.Resolver(planner, callee, Array.Empty<string>());
        Assert.NotNull(resolution);
        Assert.Equal("chaos_task_continue_with", resolution!.DirectNativeSymbol);
        Assert.Contains("chaos_task_continue_with", resolution.CppSource);
    }

    [Theory]
    // CancellationToken overloads — CT cannot be honoured yet.
    [InlineData("System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action<System.Threading.Tasks.Task>,System.Threading.CancellationToken)")]
    // TaskContinuationOptions overloads — OnlyOn* would change whether the body
    // runs at all; ignoring it would run the body when the caller asked it not to.
    [InlineData("System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action<System.Threading.Tasks.Task>,System.Threading.Tasks.TaskContinuationOptions)")]
    // TaskScheduler overloads — the body must run on the caller's scheduler.
    [InlineData("System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action<System.Threading.Tasks.Task>,System.Threading.Tasks.TaskScheduler)")]
    // The 4-argument kitchen sink.
    [InlineData("System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action<System.Threading.Tasks.Task>,System.Threading.CancellationToken,System.Threading.Tasks.TaskContinuationOptions,System.Threading.Tasks.TaskScheduler)")]
    public void ContinueWith_UnhonouredArgumentOverloads_ResolveToNull(string callee)
    {
        // These MUST NOT produce a resolution. Returning C++ that calls
        // chaos_task_continue_with would run the continuation while silently
        // discarding the CT/options/scheduler the caller passed — a task that
        // appears to work and does the wrong thing. Falling through to the
        // interpreter leaves the gap visible and honest.
        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        Assert.True(
            registry.TryMatchGenericShape(callee, out var descriptor, out _),
            $"ContinueWith callee '{callee}' should match the generic descriptor");
        Assert.NotNull(descriptor);

        var planner = new NativeAotLoweringPlanner();
        var resolution = descriptor!.Resolver(planner, callee, Array.Empty<string>());
        Assert.Null(resolution);
    }

    /// <summary>
    /// The MoveNext resolver must embed the state machine's REAL native MoveNext symbol.
    /// Asserting only the descriptor would let a wrong symbol through: the registration
    /// would still MATCH while the emitted call named a symbol that does not exist,
    /// turning every iterator into a link error.
    ///
    /// This drives a real planner through Create() rather than passing a null one — the
    /// resolver legitimately dereferences the planner to resolve the continuation method,
    /// so null only proves the resolver crashes.  (Counterexample: rename the symbol in the
    /// MoveNext resolver body and this goes red.)
    /// </summary>
    [Fact]
    public void BuildDefault_IteratorBuilder_MoveNextResolverEmitsTheRealNativeSymbol()
    {
        const string smSubjectId = "System.Private.CoreLib/IteratorSm";
        var planner = CreatePlannerWithIteratorMoveNext(smSubjectId);

        var registry = NativeAotLoweringPlanner.RuntimeHelperShapeRegistry.BuildDefault();
        string callee = IteratorBuilderCalleePrefix + "::MoveNext<IteratorSm>(IteratorSm&)";
        Assert.True(registry.TryMatchGenericShape(callee, out var descriptor, out _));

        var resolution = descriptor!.Resolver(planner, callee, Array.Empty<string>());
        Assert.NotNull(resolution);
        Assert.Contains("chaos_async_iterator_builder_move_next", resolution!.CppSource);
        // The embedded symbol is the state machine's own MoveNext, not a placeholder.
        Assert.Contains(IteratorSmMoveNextSymbol, resolution.CppSource);
    }

    private const string IteratorSmMoveNextSymbol = "chaos_iterator_sm_move_next";

    /// <summary>
    /// Builds a planner whose MoveNext index contains a MoveNext for <paramref name="smSubjectId"/>.
    /// `_asyncMoveNextMethods` is populated by Create() from the AOT IR's method list, and
    /// `TryResolveAsyncRuntimeContinuationMethod` matches by DeclaringTypeSubjectId suffix —
    /// so the artifact's Identity.DeclaringTypeSubjectId is what makes the resolver succeed.
    /// </summary>
    private static NativeAotLoweringPlanner CreatePlannerWithIteratorMoveNext(string smSubjectId)
    {
        var moveNext = new Chaos.IL2CPP.Contracts.AotCoreIrMethodArtifact
        {
            MethodId = smSubjectId + "::MoveNext",
            SubjectId = smSubjectId + "::MoveNext:System.Void()",
            Signature = "System.Void()",
            NativeSymbol = IteratorSmMoveNextSymbol,
            IsStatic = false,
            BodyAvailability = "Full",
            ReturnType = "System.Void",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Void },
            ParameterCount = 0,
            ParameterAbis = Array.Empty<AotCoreIrAbiSlotArtifact>(),
            LocalCount = 0,
            ExceptionRegionCount = 0,
            ExceptionRegions = Array.Empty<Chaos.IL2CPP.Contracts.AotCoreIrExceptionRegionArtifact>(),
            Instructions = new Chaos.IL2CPP.Contracts.AotCoreIrInstructionArtifact[]
            {
                new() { Op = "ret", IlOffset = 0 },
            },
            Identity = new Chaos.IL2CPP.Contracts.ManagedMethodIdentityArtifact
            {
                AssemblyName = "System.Private.CoreLib",
                DeclaringTypeSubjectId = smSubjectId,
                DefinitionSubjectId = smSubjectId + "::MoveNext",
                SubjectId = smSubjectId + "::MoveNext:System.Void()",
                MethodId = smSubjectId + "::MoveNext",
                Signature = "System.Void()",
            },
        };

        var ir = new Chaos.IL2CPP.Contracts.AotCoreIrArtifact
        {
            FormatVersion = "v0",
            ArtifactKind = "aotCoreIr",
            Methods = new[] { moveNext },
        };

        var plan = new Chaos.IL2CPP.Contracts.NativeAotLoweringPlanArtifact
        {
            PlanKind = "fullAssembly",
            AssemblyName = "System.Private.CoreLib",
            EntrySubjectId = smSubjectId + "::MoveNext",
            NativeEntryFunctionName = "chaos_entry",
            EntrySymbol = "chaos_entry",
            EntryMethodToken = "0",
            WorkloadAbi = "v1",
        };

        var planner = new NativeAotLoweringPlanner();
        planner.Create(
            plan,
            ir,
            moveNext,
            new Chaos.IL2CPP.Contracts.ManagedClosureManifestArtifact
            {
                AssemblyName = "System.Private.CoreLib",
                EntrySubjectId = smSubjectId + "::MoveNext",
                InputAssemblyPath = typeof(RuntimeHelperShapeRegistryTests).Assembly.Location,
                InputModuleVersionId = "00000000-0000-0000-0000-000000000000",
                Artifacts = Array.Empty<Chaos.IL2CPP.Contracts.ManagedClosureArtifactRef>(),
            },
            new Chaos.IL2CPP.Contracts.MetadataRegistrationArtifact
            {
                Registrations = Array.Empty<Chaos.IL2CPP.Contracts.MetadataRegistrationEntry>(),
            },
            new Chaos.IL2CPP.Contracts.SupplementalMetadataTemplateArtifact
            {
                RegisteredTypes = Array.Empty<Chaos.IL2CPP.Contracts.SupplementalMetadataTypeTemplateEntry>(),
                RegisteredMethods = Array.Empty<Chaos.IL2CPP.Contracts.SupplementalMetadataMethodTemplateEntry>(),
                ReservedSlots = new Chaos.IL2CPP.Contracts.SupplementalMetadataReservedSlots(),
            },
            fullAssemblyMode: true);
        return planner;
    }
}
