using System.Reflection;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.MetadataWriter.Tests;

public sealed class MetadataWriterStageTests
{
    private static readonly Type s_stageType = typeof(MetadataWriterStage);
    private const BindingFlags s_flags = BindingFlags.Static | BindingFlags.NonPublic | BindingFlags.Public;

    private static T? InvokeStatic<T>(string methodName, params object[] args)
    {
        var method = s_stageType.GetMethod(methodName, s_flags)
            ?? throw new InvalidOperationException($"Method '{methodName}' not found on MetadataWriterStage");
        return (T?)method.Invoke(null, args);
    }

    // ──────────────────────────────────────────────
    // SequenceEqual
    // ──────────────────────────────────────────────

    [Fact]
    public void SequenceEqual_BothNull_ReturnsTrue()
    {
        Assert.True(InvokeStatic<bool>("SequenceEqual", null, null));
    }

    [Fact]
    public void SequenceEqual_SameReference_ReturnsTrue()
    {
        var list = new List<string> { "a", "b" };
        Assert.True(InvokeStatic<bool>("SequenceEqual", list, list));
    }

    [Fact]
    public void SequenceEqual_LeftNullRightNonNull_ReturnsFalse()
    {
        Assert.False(InvokeStatic<bool>("SequenceEqual", null, new List<string>()));
    }

    [Fact]
    public void SequenceEqual_RightNullLeftNonNull_ReturnsFalse()
    {
        Assert.False(InvokeStatic<bool>("SequenceEqual", new List<string>(), null));
    }

    [Fact]
    public void SequenceEqual_DifferentCounts_ReturnsFalse()
    {
        Assert.False(InvokeStatic<bool>("SequenceEqual",
            new List<string> { "a" }, new List<string> { "a", "b" }));
    }

    [Fact]
    public void SequenceEqual_SameValues_ReturnsTrue()
    {
        Assert.True(InvokeStatic<bool>("SequenceEqual",
            new List<string> { "a", "b" }, new List<string> { "a", "b" }));
    }

    [Fact]
    public void SequenceEqual_DifferentValues_ReturnsFalse()
    {
        Assert.False(InvokeStatic<bool>("SequenceEqual",
            new List<string> { "a", "b" }, new List<string> { "a", "c" }));
    }

    // ──────────────────────────────────────────────
    // AreEquivalentInstantiationKeys
    // ──────────────────────────────────────────────

    [Fact]
    public void AreEquivalentInstantiationKeys_IdenticalKeys_ReturnsTrue()
    {
        var left = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.MethodInstantiation,
            DefinitionSubjectId = "T::M",
            TypeArguments = new List<string> { "System.Int32" },
            MethodArguments = new List<string>(),
        };
        var right = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.MethodInstantiation,
            DefinitionSubjectId = "T::M",
            TypeArguments = new List<string> { "System.Int32" },
            MethodArguments = new List<string>(),
        };

        Assert.True(InvokeStatic<bool>("AreEquivalentInstantiationKeys", left, right));
    }

    [Fact]
    public void AreEquivalentInstantiationKeys_DifferentContextKind_ReturnsFalse()
    {
        var left = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.MethodInstantiation,
            DefinitionSubjectId = "T::M",
        };
        var right = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.TypeInstantiation,
            DefinitionSubjectId = "T::M",
        };

        Assert.False(InvokeStatic<bool>("AreEquivalentInstantiationKeys", left, right));
    }

    [Fact]
    public void AreEquivalentInstantiationKeys_DifferentDefinitionSubjectId_ReturnsFalse()
    {
        var left = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.MethodInstantiation,
            DefinitionSubjectId = "T::M1",
        };
        var right = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.MethodInstantiation,
            DefinitionSubjectId = "T::M2",
        };

        Assert.False(InvokeStatic<bool>("AreEquivalentInstantiationKeys", left, right));
    }

    [Fact]
    public void AreEquivalentInstantiationKeys_DifferentTypeArgs_ReturnsFalse()
    {
        var left = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.MethodInstantiation,
            DefinitionSubjectId = "T::M",
            TypeArguments = new List<string> { "System.Int32" },
        };
        var right = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.MethodInstantiation,
            DefinitionSubjectId = "T::M",
            TypeArguments = new List<string> { "System.String" },
        };

        Assert.False(InvokeStatic<bool>("AreEquivalentInstantiationKeys", left, right));
    }

    // ──────────────────────────────────────────────
    // BuildGenericDemandLookup
    // ──────────────────────────────────────────────

    [Fact]
    public void BuildGenericDemandLookup_NullGraph_ReturnsEmpty()
    {
        var result = InvokeStatic<IReadOnlyDictionary<string, GenericInstantiationDemandModel>>(
            "BuildGenericDemandLookup", new object?[] { null });
        Assert.NotNull(result);
        Assert.Empty(result);
    }

    [Fact]
    public void BuildGenericDemandLookup_EmptyDemands_ReturnsEmpty()
    {
        var graph = new GenericInstantiationDemandGraphModel
        {
            Demands = new List<GenericInstantiationDemandModel>(),
        };
        var result = InvokeStatic<IReadOnlyDictionary<string, GenericInstantiationDemandModel>>(
            "BuildGenericDemandLookup", graph);
        Assert.NotNull(result);
        Assert.Empty(result);
    }

    [Fact]
    public void BuildGenericDemandLookup_SingleDemand_ReturnsLookup()
    {
        var demand = CreateDemand("A.B::M", "A.B::M");
        var graph = new GenericInstantiationDemandGraphModel { Demands = new[] { demand } };

        var result = InvokeStatic<IReadOnlyDictionary<string, GenericInstantiationDemandModel>>(
            "BuildGenericDemandLookup", graph);

        Assert.NotNull(result);
        Assert.Single(result);
        Assert.Same(demand, result["A.B::M"]);
    }

    [Fact]
    public void BuildGenericDemandLookup_DuplicateDemands_SkipsSecond()
    {
        var d1 = CreateDemand("A.B::M", "A.B::M",
            supportKind: GenericSupportKind.Legal,
            specializationKind: GenericSpecializationKind.SpecializedBody);
        var d2 = CreateDemand("A.B::M", "A.B::M",
            supportKind: GenericSupportKind.Shared,
            specializationKind: GenericSpecializationKind.SharedBody);
        var graph = new GenericInstantiationDemandGraphModel { Demands = new[] { d1, d2 } };

        // Should silently keep the first entry (no exception thrown)
        var result = InvokeStatic<IReadOnlyDictionary<string, GenericInstantiationDemandModel>>(
            "BuildGenericDemandLookup", graph);

        Assert.NotNull(result);
        Assert.Single(result);
        Assert.Same(d1, result["A.B::M"]);
    }

    [Fact]
    public void BuildGenericDemandLookup_MultipleDistinctDemands_ReturnsAll()
    {
        var d1 = CreateDemand("X::M1", "X::M1");
        var d2 = CreateDemand("Y::M2", "Y::M2");
        var d3 = CreateDemand("Z::M3", "Z::M3");
        var graph = new GenericInstantiationDemandGraphModel { Demands = new[] { d1, d2, d3 } };

        var result = InvokeStatic<IReadOnlyDictionary<string, GenericInstantiationDemandModel>>(
            "BuildGenericDemandLookup", graph);

        Assert.NotNull(result);
        Assert.Equal(3, result.Count);
        Assert.Same(d1, result["X::M1"]);
        Assert.Same(d2, result["Y::M2"]);
        Assert.Same(d3, result["Z::M3"]);
    }

    // ──────────────────────────────────────────────
    // ResolveRuntimeGenericContext
    // ──────────────────────────────────────────────

    [Fact]
    public void ResolveRuntimeGenericContext_NullLookup_ReturnsNull()
    {
        var result = InvokeStatic<RuntimeGenericContextArtifact?>(
            "ResolveRuntimeGenericContext", "T::M", "T::M", null);
        Assert.Null(result);
    }

    [Fact]
    public void ResolveRuntimeGenericContext_EmptyLookup_ReturnsNull()
    {
        var lookup = new Dictionary<string, GenericInstantiationDemandModel>();
        var result = InvokeStatic<RuntimeGenericContextArtifact?>(
            "ResolveRuntimeGenericContext", "T::M", "T::M", lookup);
        Assert.Null(result);
    }

    [Fact]
    public void ResolveRuntimeGenericContext_SubjectNotFound_ReturnsNull()
    {
        var demand = CreateDemand("A.B::M", "A.B::M");
        var lookup = BuildLookup(demand);
        var result = InvokeStatic<RuntimeGenericContextArtifact?>(
            "ResolveRuntimeGenericContext", "Other::M", "Other::M", lookup);
        Assert.Null(result);
    }

    [Fact]
    public void ResolveRuntimeGenericContext_FoundDemand_ReturnsContext()
    {
        var demand = CreateDemand("T::M", "T::M",
            supportKind: GenericSupportKind.Legal,
            specializationKind: GenericSpecializationKind.SpecializedBody);
        var lookup = BuildLookup(demand);

        var result = InvokeStatic<RuntimeGenericContextArtifact?>(
            "ResolveRuntimeGenericContext", "T::M", "T::M", lookup);

        Assert.NotNull(result);
        Assert.Equal(GenericSupportKind.Legal, result.SupportKindCode);
        Assert.Equal(GenericSpecializationKind.SpecializedBody, result.SpecializationKindCode);
        Assert.Contains("loader-demand:", result.StatusReasonCode);
    }

    // ──────────────────────────────────────────────
    // EnsureEquivalentDemand (no-op when equivalent, throws when not)
    // ──────────────────────────────────────────────

    [Fact]
    public void EnsureEquivalentDemand_EquivalentDemands_DoesNotThrow()
    {
        var d1 = CreateDemand("T::M", "T::M",
            supportKind: GenericSupportKind.Legal,
            specializationKind: GenericSpecializationKind.SpecializedBody,
            familyKind: GenericDemandFamilyKind.ClosedGenericMethod);
        var d2 = CreateDemand("T::M", "T::M",
            supportKind: GenericSupportKind.Legal,
            specializationKind: GenericSpecializationKind.SpecializedBody,
            familyKind: GenericDemandFamilyKind.ClosedGenericMethod);

        // Should not throw
        InvokeStatic<object>("EnsureEquivalentDemand", d1, d2);
    }

    [Fact]
    public void EnsureEquivalentDemand_NonEquivalent_Throws()
    {
        var d1 = CreateDemand("T::M", "T1::M",
            supportKind: GenericSupportKind.Legal,
            specializationKind: GenericSpecializationKind.SpecializedBody,
            familyKind: GenericDemandFamilyKind.ClosedGenericMethod);
        var d2 = CreateDemand("T::M", "T2::M",
            supportKind: GenericSupportKind.Legal,
            specializationKind: GenericSpecializationKind.SpecializedBody,
            familyKind: GenericDemandFamilyKind.ClosedGenericMethod);

        var ex = Assert.Throws<TargetInvocationException>(() =>
            InvokeStatic<object>("EnsureEquivalentDemand", d1, d2));
        Assert.IsType<InvalidOperationException>(ex.InnerException);
        Assert.Contains("conflicting generic instantiation", ex.InnerException.Message);
    }

    // ──────────────────────────────────────────────
    // Helpers
    // ──────────────────────────────────────────────

    private static GenericInstantiationDemandModel CreateDemand(
        string subjectId,
        string definitionSubjectId,
        GenericSupportKind supportKind = GenericSupportKind.Legal,
        GenericSpecializationKind specializationKind = GenericSpecializationKind.SpecializedBody,
        GenericDemandFamilyKind familyKind = GenericDemandFamilyKind.ClosedGenericMethod,
        string demandSourceKind = "test")
    {
        return new GenericInstantiationDemandModel
        {
            RequestingAssemblyName = "TestAssembly",
            OwningAssemblyName = "TestAssembly",
            SubjectKind = "method",
            SubjectId = subjectId,
            DefinitionSubjectId = definitionSubjectId,
            DemandSourceKind = demandSourceKind,
            InstantiationKey = new GenericInstantiationKey
            {
                ContextKind = GenericContextKind.MethodInstantiation,
                DefinitionSubjectId = definitionSubjectId,
            },
            SupportKindCode = supportKind,
            SpecializationKindCode = specializationKind,
            FamilyKindCode = familyKind,
        };
    }

    private static Dictionary<string, GenericInstantiationDemandModel> BuildLookup(
        GenericInstantiationDemandModel demand)
    {
        return new Dictionary<string, GenericInstantiationDemandModel>(StringComparer.Ordinal)
        {
            [demand.SubjectId] = demand,
        };
    }
}
