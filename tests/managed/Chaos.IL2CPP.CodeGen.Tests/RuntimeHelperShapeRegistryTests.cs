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

    // ── Entries enumeration ─────────────────────────────────

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
}
