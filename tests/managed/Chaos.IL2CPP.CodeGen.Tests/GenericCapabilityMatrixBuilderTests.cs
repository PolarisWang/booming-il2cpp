using System.Reflection;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class GenericCapabilityMatrixBuilderTests
{
    private static readonly Type s_type = typeof(GenericCapabilityMatrixBuilder);
    private static readonly BindingFlags s_flags = BindingFlags.Static | BindingFlags.NonPublic;

    private static T? InvokeStatic<T>(string name, params object[] args)
    {
        var method = s_type.GetMethod(name, s_flags)
            ?? throw new InvalidOperationException($"Method '{name}' not found");
        return (T?)method.Invoke(null, args);
    }

    // ── Helpers for private nested types ──────────────────────────────

    private static Type GetObservationType() =>
        s_type.GetNestedType("GenericAuthorityObservation", BindingFlags.NonPublic)!;

    private static object MakeObservation(string source, string openDef, string sharedBody,
        string instStub, bool hasRuntimeCtx, string reason,
        GenericSupportKind supportKind, GenericSpecializationKind specKind) =>
        Activator.CreateInstance(GetObservationType(),
            [source, openDef, sharedBody, instStub, hasRuntimeCtx, reason,
             supportKind, specKind])!;

    private static System.Collections.IDictionary MakeObservationDict()
    {
        var dictType = typeof(Dictionary<,>).MakeGenericType(typeof(string), GetObservationType());
        return (System.Collections.IDictionary)Activator.CreateInstance(dictType)!;
    }

    // ── IsReflectionBoundaryTarget ────────────────────────────────────

    [Theory]
    [InlineData("System.Reflection.MethodBase", true)]
    [InlineData("System.Console.WriteLine", false)]
    [InlineData("Activator.CreateInstance", true)]
    [InlineData("", false)]
    [InlineData(null, false)]
    [InlineData("Some.Type::Method", false)]
    public void IsReflectionBoundaryTarget_VariousInputs(string? target, bool expected)
    {
        Assert.Equal(expected, InvokeStatic<bool>("IsReflectionBoundaryTarget", target));
    }

    // ── BuildGateStatus ────────────────────────────────────────────────

    [Fact]
    public void BuildGateStatus_AllOk_ReturnsOk()
    {
        var entries = new List<GenericCapabilityMatrixEntryArtifact>
        {
            new()
            {
                SubjectKind = "method", SubjectId = "T/M::M()", DefinitionSubjectId = "0x01",
                DemandSourceKind = "direct", RequestingAssemblyName = "T", OwningAssemblyName = "T",
                FamilyKindCode = GenericDemandFamilyKind.ClosedGenericType,
                ContextKindCode = (GenericContextKind)0,
                SupportKindCode = (GenericSupportKind)0,
                SpecializationKindCode = (GenericSpecializationKind)0,
                IsCrossAssembly = false,
                StatusReasonCode = "", AuthoritySource = "aot-core-ir",
                OpenDefinitionSubjectId = "def", SharedGenericBodyId = "body",
                InstantiationStubId = "stub",
                HasOpenDefinitionAuthority = true, HasSharedBodyAuthority = true,
                HasInstantiationStubAuthority = true, HasRuntimeGenericContextAuthority = true,
            },
        };
        var leaks = new List<GenericCapabilityMatrixLeakArtifact>();
        var result = InvokeStatic<GenericCapabilityMatrixGateStatus>("BuildGateStatus", entries, leaks);
        Assert.Equal("ok", result!.Status);
    }

    [Fact]
    public void BuildGateStatus_MissingAuthority_ReturnsFail()
    {
        var entries = new List<GenericCapabilityMatrixEntryArtifact>
        {
            new()
            {
                SubjectKind = "method", SubjectId = "T/M::M()", DefinitionSubjectId = "0x01",
                DemandSourceKind = "direct", RequestingAssemblyName = "T", OwningAssemblyName = "T",
                FamilyKindCode = GenericDemandFamilyKind.ClosedGenericType,
                ContextKindCode = (GenericContextKind)0,
                SupportKindCode = (GenericSupportKind)0,
                SpecializationKindCode = (GenericSpecializationKind)0,
                IsCrossAssembly = false,
                StatusReasonCode = "", AuthoritySource = "",
                OpenDefinitionSubjectId = "", SharedGenericBodyId = "",
                InstantiationStubId = "",
                HasOpenDefinitionAuthority = false, HasSharedBodyAuthority = false,
                HasInstantiationStubAuthority = false, HasRuntimeGenericContextAuthority = false,
            },
        };
        var leaks = new List<GenericCapabilityMatrixLeakArtifact>();
        var result = InvokeStatic<GenericCapabilityMatrixGateStatus>("BuildGateStatus", entries, leaks);
        Assert.Equal("fail", result!.Status);
        Assert.Equal(1, result.MissingAuthorityCount);
    }

    [Fact]
    public void BuildGateStatus_UnsupportedLeak_ReturnsFail()
    {
        var entries = new List<GenericCapabilityMatrixEntryArtifact>();
        var leaks = new List<GenericCapabilityMatrixLeakArtifact>
        {
            new() { SubjectId = "T/M::M()", LeakKind = "unsupportedAuthorityLeak", Reason = "forbidden" },
        };
        var result = InvokeStatic<GenericCapabilityMatrixGateStatus>("BuildGateStatus", entries, leaks);
        Assert.Equal("fail", result!.Status);
        Assert.Equal(1, result.UnsupportedLeakCount);
    }

    // ── BuildAuthorityLookup ───────────────────────────────────────────

    [Fact]
    public void BuildAuthorityLookup_EmptyInput_ReturnsEmpty()
    {
        var aotCoreIr = new AotCoreIrArtifact { Methods = [] };
        var supplemental = new SupplementalMetadataTemplateArtifact
        {
            RegisteredMethods = [], RegisteredTypes = [],
            ReservedSlots = new SupplementalMetadataReservedSlots(),
        };
        var result = InvokeStatic<object>(
            "BuildAuthorityLookup", aotCoreIr, supplemental);
        Assert.NotNull(result);
        Assert.Empty((System.Collections.IEnumerable)result!);
    }

    [Fact]
    public void BuildAuthorityLookup_MethodWithoutAuthority_NotRegistered()
    {
        var method = new AotCoreIrMethodArtifact
        {
            MethodId = "m1", SubjectId = "T/M::M()", Signature = "void()",
            Identity = new ManagedMethodIdentityArtifact { AssemblyName = "T", DeclaringTypeSubjectId = "T.M", DefinitionSubjectId = "0x01", SubjectId = "T/M::M()", MethodId = "m1", Signature = "void()" },
            NativeSymbol = "", IsStatic = false, ReturnType = "System.Void",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = (AotCoreIrAbiCarrierKind)0 },
            ParameterCount = 0, ParameterAbis = [], LocalCount = 0,
            ExceptionRegionCount = 0, ExceptionRegions = [], Instructions = [],
        };
        var aotCoreIr = new AotCoreIrArtifact { Methods = [method] };
        var supplemental = new SupplementalMetadataTemplateArtifact
        {
            RegisteredMethods = [], RegisteredTypes = [],
            ReservedSlots = new SupplementalMetadataReservedSlots(),
        };
        var result = InvokeStatic<object>(
            "BuildAuthorityLookup", aotCoreIr, supplemental);
        // Method with no open-def/shared-body/instantiation-stub has no authority → not registered
        Assert.Empty((System.Collections.IEnumerable)result!);
    }

    [Fact]
    public void BuildAuthorityLookup_MethodWithAuthority_Registered()
    {
        var method = new AotCoreIrMethodArtifact
        {
            MethodId = "m1", SubjectId = "T/M::M()", Signature = "void()",
            Identity = new ManagedMethodIdentityArtifact { AssemblyName = "T", DeclaringTypeSubjectId = "T.M", DefinitionSubjectId = "0x01", SubjectId = "T/M::M()", MethodId = "m1", Signature = "void()" },
            NativeSymbol = "", IsStatic = false, ReturnType = "System.Void",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = (AotCoreIrAbiCarrierKind)0 },
            ParameterCount = 0, ParameterAbis = [], LocalCount = 0,
            ExceptionRegionCount = 0, ExceptionRegions = [], Instructions = [],
            OpenDefinitionSubjectId = "def",
            SharedGenericBodyId = new SharedGenericBodyId { Value = "body" },
            InstantiationStubId = new InstantiationStubId { Value = "stub" },
        };
        var aotCoreIr = new AotCoreIrArtifact { Methods = [method] };
        var supplemental = new SupplementalMetadataTemplateArtifact
        {
            RegisteredMethods = [], RegisteredTypes = [],
            ReservedSlots = new SupplementalMetadataReservedSlots(),
        };
        var result = InvokeStatic<object>(
            "BuildAuthorityLookup", aotCoreIr, supplemental);
        Assert.NotEmpty((System.Collections.IEnumerable)result!);
    }

    [Fact]
    public void BuildAuthorityLookup_WithInstructionTargets_IncludesTargets()
    {
        var targetRef = new AotCoreIrReferenceArtifact
        {
            Kind = AotCoreIrReferenceKind.Method,
            AssemblyName = "T",
            SubjectId = "T/M2::N()",
            OpenDefinitionSubjectId = "def2",
            SharedGenericBodyId = new SharedGenericBodyId { Value = "body2" },
            InstantiationStubId = new InstantiationStubId { Value = "stub2" },
        };
        var method = new AotCoreIrMethodArtifact
        {
            MethodId = "m1", SubjectId = "T/M::M()", Signature = "void()",
            Identity = new ManagedMethodIdentityArtifact { AssemblyName = "T", DeclaringTypeSubjectId = "T.M", DefinitionSubjectId = "0x01", SubjectId = "T/M::M()", MethodId = "m1", Signature = "void()" },
            NativeSymbol = "", IsStatic = false, ReturnType = "System.Void",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = (AotCoreIrAbiCarrierKind)0 },
            ParameterCount = 0, ParameterAbis = [], LocalCount = 0,
            ExceptionRegionCount = 0, ExceptionRegions = [],
            OpenDefinitionSubjectId = "def1",
            Instructions =
            [
                new AotCoreIrInstructionArtifact
                {
                    Op = "call", Operand = 0, IlOffset = 0,
                    TargetReference = targetRef,
                },
            ],
        };
        var aotCoreIr = new AotCoreIrArtifact { Methods = [method] };
        var supplemental = new SupplementalMetadataTemplateArtifact
        {
            RegisteredMethods = [], RegisteredTypes = [],
            ReservedSlots = new SupplementalMetadataReservedSlots(),
        };
        var result = InvokeStatic<object>(
            "BuildAuthorityLookup", aotCoreIr, supplemental);
        // Both the method and its instruction target should be registered
        var dict = (System.Collections.IDictionary)result!;
        Assert.Contains("T/M::M()", dict.Keys.Cast<string>());
        Assert.Contains("T/M2::N()", dict.Keys.Cast<string>());
    }

    [Fact]
    public void BuildAuthorityLookup_WithSupplementalEntries_IncludesThem()
    {
        var aotCoreIr = new AotCoreIrArtifact { Methods = [] };
        var supplemental = new SupplementalMetadataTemplateArtifact
        {
            RegisteredTypes =
            [
                new SupplementalMetadataTypeTemplateEntry
                {
                    AssemblyName = "T",
                    SubjectId = "type:T",
                    DefinitionSubjectId = "typeDef",
                    MetadataToken = 0,
                    RuntimeGenericContext = new RuntimeGenericContextArtifact
                    {
                        InstantiationKey = new GenericInstantiationKey
                        {
                            DefinitionSubjectId = "typeDef",
                            ContextKind = (GenericContextKind)0,
                        },
                        SharedGenericBodyId = new SharedGenericBodyId { Value = "tBody" },
                        InstantiationStubId = new InstantiationStubId { Value = "tStub" },
                        StatusReasonCode = "ok",
                        SupportKindCode = (GenericSupportKind)0,
                        SpecializationKindCode = (GenericSpecializationKind)0,
                    },
                },
            ],
            RegisteredMethods = [],
            ReservedSlots = new SupplementalMetadataReservedSlots(),
        };
        var result = InvokeStatic<object>(
            "BuildAuthorityLookup", aotCoreIr, supplemental);
        Assert.NotEmpty((System.Collections.IEnumerable)result!);
    }

    // ── TryRegisterObservation ─────────────────────────────────────────

    [Fact]
    public void TryRegisterObservation_NoAuthority_NotRegistered()
    {
        var observationType = GetObservationType();
        var dictType = typeof(Dictionary<,>).MakeGenericType(typeof(string), observationType);
        var dict = (System.Collections.IDictionary)Activator.CreateInstance(dictType)!;
        var obs = Activator.CreateInstance(
            observationType,
            ["source", "", "", "", false, "ok",
             (GenericSupportKind)0, (GenericSpecializationKind)0])!;
        var method = s_type.GetMethod("TryRegisterObservation", s_flags,
            new[] { dictType, typeof(string), observationType })!;
        method.Invoke(null, new object[] { dict, "subj", obs });
        Assert.Empty(dict);
    }

    [Fact]
    public void TryRegisterObservation_WithAuthority_Registered()
    {
        var observationType = GetObservationType();
        var dictType = typeof(Dictionary<,>).MakeGenericType(typeof(string), observationType);
        var dict = (System.Collections.IDictionary)Activator.CreateInstance(dictType)!;
        var obs = Activator.CreateInstance(
            observationType,
            ["aot-core-ir", "def", "body", "stub", true, "ok",
             (GenericSupportKind)0, (GenericSpecializationKind)0])!;
        var method = s_type.GetMethod("TryRegisterObservation", s_flags,
            new[] { dictType, typeof(string), observationType })!;
        method.Invoke(null, new object[] { dict, "subj", obs });
        Assert.Single(dict);
    }

    [Fact]
    public void TryRegisterObservation_ExistingObservation_KeepsExisting()
    {
        var observationType = GetObservationType();
        var dictType = typeof(Dictionary<,>).MakeGenericType(typeof(string), observationType);
        var dict = (System.Collections.IDictionary)Activator.CreateInstance(dictType)!;
        // First: register with supplemental-metadata (weaker authority)
        var existing = Activator.CreateInstance(
            observationType,
            ["supplemental-metadata", "defX", "bodyY", "stubZ", true, "ok",
             (GenericSupportKind)0, (GenericSpecializationKind)0])!;
        var method = s_type.GetMethod("TryRegisterObservation", s_flags,
            new[] { dictType, typeof(string), observationType })!;
        method.Invoke(null, new object[] { dict, "subj", existing });
        // Second: try to register with aot-core-ir (stronger, same def/body/stub)
        var additional = Activator.CreateInstance(
            observationType,
            ["aot-core-ir", "defX", "bodyY", "stubZ", true, "ok",
             (GenericSupportKind)0, (GenericSpecializationKind)0])!;
        method.Invoke(null, new object[] { dict, "subj", additional });
        // Should still have 1 entry — aot-core-ir replaces supplemental-metadata
        Assert.Single(dict);
    }

    // ── ShouldPreferAdditionalObservation ──────────────────────────────

    [Fact]
    public void ShouldPreferAdditionalObservation_PrefersAotCoreIr()
    {
        var existing = MakeObservation("supplemental-metadata", "", "", "", false, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        var additional = MakeObservation("aot-core-ir", "", "", "", false, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        Assert.True(InvokeStatic<bool>("ShouldPreferAdditionalObservation", existing, additional));
    }

    [Fact]
    public void ShouldPreferAdditionalObservation_DoesNotPreferNonAotCoreIr()
    {
        var existing = MakeObservation("other", "", "", "", false, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        var additional = MakeObservation("other-too", "", "", "", false, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        Assert.False(InvokeStatic<bool>("ShouldPreferAdditionalObservation", existing, additional));
    }

    // ── ValidateObservedAuthority ──────────────────────────────────────

    [Fact]
    public void ValidateObservedAuthority_NullObservation_DoesNothing()
    {
        var leaks = new List<GenericCapabilityMatrixLeakArtifact>();
        var demand = new GenericInstantiationDemandModel
        {
            SubjectKind = "method", SubjectId = "T/M::M()", DefinitionSubjectId = "0x01",
            DemandSourceKind = "direct", RequestingAssemblyName = "T", OwningAssemblyName = "T",
            FamilyKindCode = GenericDemandFamilyKind.ClosedGenericType,
            SupportKindCode = (GenericSupportKind)0,
            SpecializationKindCode = (GenericSpecializationKind)0,
            IsCrossAssembly = false,
            InstantiationKey = new GenericInstantiationKey
            {
                DefinitionSubjectId = "def",
                ContextKind = (GenericContextKind)0,
            },
        };
        InvokeStatic<object>("ValidateObservedAuthority", demand, null, "def", "body", "stub", leaks);
        Assert.Empty(leaks);
    }

    [Fact]
    public void ValidateObservedAuthority_ForbiddenSupportKind_CreatesLeak()
    {
        var leaks = new List<GenericCapabilityMatrixLeakArtifact>();
        var demand = new GenericInstantiationDemandModel
        {
            SubjectKind = "method", SubjectId = "T/M::M()", DefinitionSubjectId = "0x01",
            DemandSourceKind = "direct", RequestingAssemblyName = "T", OwningAssemblyName = "T",
            FamilyKindCode = GenericDemandFamilyKind.ClosedGenericType,
            SupportKindCode = GenericSupportKind.Forbidden,
            SpecializationKindCode = (GenericSpecializationKind)0,
            IsCrossAssembly = false,
            InstantiationKey = new GenericInstantiationKey
            {
                DefinitionSubjectId = "def",
                ContextKind = (GenericContextKind)0,
            },
        };
        var obs = MakeObservation("aot-core-ir", "def", "body", "stub", true, "ok",
            GenericSupportKind.Forbidden, (GenericSpecializationKind)0);
        InvokeStatic<object>("ValidateObservedAuthority", demand, obs, "def", "body", "stub", leaks);
        Assert.Single(leaks);
        Assert.Equal("unsupportedAuthorityLeak", leaks[0].LeakKind);
    }

    [Fact]
    public void ValidateObservedAuthority_AuthorityMismatch_CreatesLeak()
    {
        var leaks = new List<GenericCapabilityMatrixLeakArtifact>();
        var demand = new GenericInstantiationDemandModel
        {
            SubjectKind = "method", SubjectId = "T/M::M()", DefinitionSubjectId = "0x01",
            DemandSourceKind = "direct", RequestingAssemblyName = "T", OwningAssemblyName = "T",
            FamilyKindCode = GenericDemandFamilyKind.ClosedGenericType,
            SupportKindCode = (GenericSupportKind)0,
            SpecializationKindCode = (GenericSpecializationKind)0,
            IsCrossAssembly = false,
            InstantiationKey = new GenericInstantiationKey
            {
                DefinitionSubjectId = "def",
                ContextKind = (GenericContextKind)0,
            },
        };
        var obs = MakeObservation("aot-core-ir", "wrong-def", "wrong-body", "wrong-stub", true, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        InvokeStatic<object>("ValidateObservedAuthority", demand, obs, "expected-def", "expected-body", "expected-stub", leaks);
        Assert.Single(leaks);
        Assert.Equal("authorityMismatch", leaks[0].LeakKind);
    }

    // ── EnsureEquivalentObservation ────────────────────────────────────

    [Fact]
    public void EnsureEquivalentObservation_Matching_DoesNotThrow()
    {
        var obs1 = MakeObservation("src", "def", "body", "stub", true, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        var obs2 = MakeObservation("src", "def", "body", "stub", true, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        var ex = Record.Exception(() =>
            InvokeStatic<object>("EnsureEquivalentObservation", "subj", obs1, obs2));
        Assert.Null(ex);
    }

    [Fact]
    public void EnsureEquivalentObservation_Conflicting_Throws()
    {
        var obs1 = MakeObservation("src", "def1", "body1", "stub1", true, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        var obs2 = MakeObservation("src", "def2", "body2", "stub2", true, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        var ex = Assert.Throws<TargetInvocationException>(() =>
            InvokeStatic<object>("EnsureEquivalentObservation", "subj", obs1, obs2));
        Assert.Contains("conflicting generic authority observations", ex.InnerException!.Message);
    }

    // ── CreateSupplementalMetadataObservation ──────────────────────────

    [Fact]
    public void CreateSupplementalMetadataObservation_NullContext_ReturnsSourceOnly()
    {
        var method = s_type.GetMethod("CreateSupplementalMetadataObservation", s_flags,
            new[] { typeof(RuntimeGenericContextArtifact) })!;
        var result = method.Invoke(null, new object?[] { null });
        Assert.NotNull(result);
    }

    // ── BuildFamilyBudgets ─────────────────────────────────────────────

    [Fact]
    public void BuildFamilyBudgets_CountsCorrectly()
    {
        var entries = new List<GenericCapabilityMatrixEntryArtifact>
        {
            new()
            {
                SubjectKind = "method", SubjectId = "System.Threading.Tasks.Task<T>.M()", DefinitionSubjectId = "0x01",
                DemandSourceKind = "direct", RequestingAssemblyName = "T", OwningAssemblyName = "T",
                FamilyKindCode = GenericDemandFamilyKind.ClosedGenericType,
                ContextKindCode = (GenericContextKind)0,
                SupportKindCode = (GenericSupportKind)0,
                SpecializationKindCode = (GenericSpecializationKind)0,
                IsCrossAssembly = false,
                StatusReasonCode = "", AuthoritySource = "",
                OpenDefinitionSubjectId = "", SharedGenericBodyId = "",
                InstantiationStubId = "",
                HasOpenDefinitionAuthority = true, HasSharedBodyAuthority = true,
                HasInstantiationStubAuthority = true, HasRuntimeGenericContextAuthority = true,
            },
        };
        var result = InvokeStatic<IReadOnlyList<GenericCapabilityFamilyBudgetArtifact>>(
            "BuildFamilyBudgets", entries, new List<GenericCapabilityBoundaryCaseArtifact>());
        Assert.NotNull(result);
        Assert.NotEmpty(result!);
        var asyncTask = result!.FirstOrDefault(f => f.FamilyId == "AsyncTaskFamily");
        Assert.NotNull(asyncTask);
        Assert.Equal(1, asyncTask!.ObservedCount);
    }

    [Fact]
    public void BuildFamilyBudgets_NoMatch_ReturnsZero()
    {
        var entries = new List<GenericCapabilityMatrixEntryArtifact>();
        var result = InvokeStatic<IReadOnlyList<GenericCapabilityFamilyBudgetArtifact>>(
            "BuildFamilyBudgets", entries, new List<GenericCapabilityBoundaryCaseArtifact>());
        Assert.All(result!, f => Assert.Equal(0, f.ObservedCount));
    }

    // ── HasAnyAuthority ────────────────────────────────────────────────

    [Fact]
    public void HasAnyAuthority_NoAuthority_ReturnsFalse()
    {
        var obs = MakeObservation("src", "", "", "", false, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        Assert.False(InvokeStatic<bool>("HasAnyAuthority", obs));
    }

    [Fact]
    public void HasAnyAuthority_WithOpenDef_ReturnsTrue()
    {
        var obs = MakeObservation("src", "def", "", "", false, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        Assert.True(InvokeStatic<bool>("HasAnyAuthority", obs));
    }

    [Fact]
    public void HasAnyAuthority_WithSharedBody_ReturnsTrue()
    {
        var obs = MakeObservation("src", "", "body", "", false, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        Assert.True(InvokeStatic<bool>("HasAnyAuthority", obs));
    }

    [Fact]
    public void HasAnyAuthority_WithRuntimeContext_ReturnsTrue()
    {
        var obs = MakeObservation("src", "", "", "", true, "ok",
            (GenericSupportKind)0, (GenericSpecializationKind)0);
        Assert.True(InvokeStatic<bool>("HasAnyAuthority", obs));
    }

    // ── Build (main entry point) ───────────────────────────────────────

    [Fact]
    public void Build_EmptyInputs_ReturnsEmptyArtifact()
    {
        var result = new GenericCapabilityMatrixBuilder().Build(
            "TestOwner",
            "TestEntry",
            new GenericInstantiationDemandGraphModel { Demands = [] },
            new AotCoreIrArtifact { Methods = [] },
            new SupplementalMetadataTemplateArtifact
            {
                RegisteredMethods = [], RegisteredTypes = [],
                ReservedSlots = new SupplementalMetadataReservedSlots(),
            });
        Assert.Equal("TestOwner", result.OwnerSubjectId);
        Assert.Equal("TestEntry", result.EntrySubjectId);
        Assert.NotEmpty(result.HotUpdateModes);
        Assert.Empty(result.Entries);
        Assert.Empty(result.BoundaryCases);
        Assert.NotEmpty(result.FamilyBudgets);
        Assert.Equal("ok", result.Gates.Status);
    }

    [Fact]
    public void Build_WithDemandAndAuthority_GeneratesEntry()
    {
        // A method with full authority
        var method = new AotCoreIrMethodArtifact
        {
            MethodId = "m1", SubjectId = "T/M::M()", Signature = "void()",
            Identity = new ManagedMethodIdentityArtifact { AssemblyName = "T", DeclaringTypeSubjectId = "T.M", DefinitionSubjectId = "0x01", SubjectId = "T/M::M()", MethodId = "m1", Signature = "void()" },
            NativeSymbol = "", IsStatic = false, ReturnType = "System.Void",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = (AotCoreIrAbiCarrierKind)0 },
            ParameterCount = 0, ParameterAbis = [], LocalCount = 0,
            ExceptionRegionCount = 0, ExceptionRegions = [], Instructions = [],
            OpenDefinitionSubjectId = "def",
            SharedGenericBodyId = new SharedGenericBodyId { Value = "body:kernel:T/M::M()" },
            InstantiationStubId = new InstantiationStubId { Value = "stub:kernel:T/M::M()" },
        };
        var result = new GenericCapabilityMatrixBuilder().Build(
            "T",
            "T/M::M()",
            new GenericInstantiationDemandGraphModel
            {
                Demands =
                [
                    new GenericInstantiationDemandModel
                    {
                        SubjectKind = "method",
                        SubjectId = "T/M::M()",
                        DefinitionSubjectId = "0x01",
                        DemandSourceKind = "direct",
                        RequestingAssemblyName = "T",
                        OwningAssemblyName = "T",
                        FamilyKindCode = GenericDemandFamilyKind.ClosedGenericType,
                        SupportKindCode = (GenericSupportKind)0,
                        SpecializationKindCode = (GenericSpecializationKind)0,
                        IsCrossAssembly = false,
                        InstantiationKey = new GenericInstantiationKey
                        {
                            DefinitionSubjectId = "def",
                            ContextKind = (GenericContextKind)0,
                        },
                    },
                ],
            },
            new AotCoreIrArtifact { Methods = [method] },
            new SupplementalMetadataTemplateArtifact
            {
                RegisteredMethods = [], RegisteredTypes = [],
                ReservedSlots = new SupplementalMetadataReservedSlots(),
            });
        Assert.NotEmpty(result.Entries);
        Assert.NotEmpty(result.FamilyBudgets);
    }

    // ── BuildBoundaryCases (private static) ────────────────────────────

    [Fact]
    public void BuildBoundaryCases_HotUpdateInstruction_Detected()
    {
        var method = new AotCoreIrMethodArtifact
        {
            MethodId = "m1", SubjectId = "T/M::M()", Signature = "void()",
            Identity = new ManagedMethodIdentityArtifact { AssemblyName = "T", DeclaringTypeSubjectId = "T.M", DefinitionSubjectId = "0x01", SubjectId = "T/M::M()", MethodId = "m1", Signature = "void()" },
            NativeSymbol = "", IsStatic = false, ReturnType = "System.Void",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = (AotCoreIrAbiCarrierKind)0 },
            ParameterCount = 0, ParameterAbis = [], LocalCount = 0,
            ExceptionRegionCount = 0, ExceptionRegions = [],
            Instructions =
            [
                new AotCoreIrInstructionArtifact
                {
                    Op = "call", Operand = 0, IlOffset = 0,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Method,
                        AssemblyName = "T",
                        SubjectId = "T/HotUpdateHelper::ApplyPatch()",
                        OpenDefinitionSubjectId = "def",
                    },
                },
            ],
        };
        var aotCoreIr = new AotCoreIrArtifact { Methods = [method] };
        var result = InvokeStatic<IReadOnlyList<GenericCapabilityBoundaryCaseArtifact>>(
            "BuildBoundaryCases", aotCoreIr);
        Assert.NotEmpty(result!);
        Assert.Contains(result!, b => b.BoundaryKind == "HotUpdateBoundary");
    }

    [Fact]
    public void BuildBoundaryCases_DispatchCallvirt_Detected()
    {
        var method = new AotCoreIrMethodArtifact
        {
            MethodId = "m1", SubjectId = "T/M::M()", Signature = "void()",
            Identity = new ManagedMethodIdentityArtifact { AssemblyName = "T", DeclaringTypeSubjectId = "T.M", DefinitionSubjectId = "0x01", SubjectId = "T/M::M()", MethodId = "m1", Signature = "void()" },
            NativeSymbol = "", IsStatic = false, ReturnType = "System.Void",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = (AotCoreIrAbiCarrierKind)0 },
            ParameterCount = 0, ParameterAbis = [], LocalCount = 0,
            ExceptionRegionCount = 0, ExceptionRegions = [],
            Instructions =
            [
                new AotCoreIrInstructionArtifact
                {
                    Op = "callvirt", Operand = 0, IlOffset = 0,
                    DispatchKindCode = HybridDispatchKind.Virtual,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Method,
                        AssemblyName = "T",
                        SubjectId = "T/Base::VirtualMethod()",
                    },
                },
            ],
        };
        var aotCoreIr = new AotCoreIrArtifact { Methods = [method] };
        var result = InvokeStatic<IReadOnlyList<GenericCapabilityBoundaryCaseArtifact>>(
            "BuildBoundaryCases", aotCoreIr);
        Assert.NotEmpty(result!);
        Assert.Contains(result!, b => b.BoundaryKind == "DispatchBoundary");
    }

    [Fact]
    public void BuildBoundaryCases_ReflectionTarget_Detected()
    {
        var method = new AotCoreIrMethodArtifact
        {
            MethodId = "m1", SubjectId = "T/M::M()", Signature = "void()",
            Identity = new ManagedMethodIdentityArtifact { AssemblyName = "T", DeclaringTypeSubjectId = "T.M", DefinitionSubjectId = "0x01", SubjectId = "T/M::M()", MethodId = "m1", Signature = "void()" },
            NativeSymbol = "", IsStatic = false, ReturnType = "System.Void",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = (AotCoreIrAbiCarrierKind)0 },
            ParameterCount = 0, ParameterAbis = [], LocalCount = 0,
            ExceptionRegionCount = 0, ExceptionRegions = [],
            Instructions =
            [
                new AotCoreIrInstructionArtifact
                {
                    Op = "call", Operand = 0, IlOffset = 0,
                    Callee = "System.Reflection.MethodBase",
                },
            ],
        };
        var aotCoreIr = new AotCoreIrArtifact { Methods = [method] };
        var result = InvokeStatic<IReadOnlyList<GenericCapabilityBoundaryCaseArtifact>>(
            "BuildBoundaryCases", aotCoreIr);
        Assert.NotEmpty(result!);
        Assert.Contains(result!, b => b.BoundaryKind == "ReflectionBoundary");
    }

    // ── ContainsAny (static helper) ────────────────────────────────────

    [Theory]
    [InlineData("HotUpdate.Apply", true)]
    [InlineData("NormalCall", false)]
    [InlineData("", false)]
    [InlineData(null, false)]
    public void ContainsAny_VariousInputs(string? value, bool expected)
    {
        // The ContainsAny(string?, string[]) overload
        var method = s_type.GetMethod("ContainsAny", s_flags,
            new[] { typeof(string), typeof(string[]) })!;
        var result = (bool)method.Invoke(null, new object?[] { value, new[] { "HotUpdate" } })!;
        Assert.Equal(expected, result);
    }
}
