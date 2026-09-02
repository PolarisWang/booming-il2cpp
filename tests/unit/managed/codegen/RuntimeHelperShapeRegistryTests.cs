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
}
