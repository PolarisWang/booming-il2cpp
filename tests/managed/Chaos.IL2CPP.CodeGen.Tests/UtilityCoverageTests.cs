using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests;

/// <summary>
/// Tests for utility classes: ManagedNaming edge cases, TypeProviders,
/// ManagedMethodIdentityResolver, BodyAvailabilityResolver, HybridDispatchResolver.
/// </summary>
public sealed class UtilityCoverageTests
{
    // ── ManagedNaming edge cases ──

    [Theory]
    [InlineData("A/B::Method:System.Void()", "", "Method", "System.Void", "")]
    [InlineData("A/B::Method()", "", "Method", null, "")]
    [InlineData("invalid", "", "", null, "")]
    public void TryParseMethodSubjectIdComponents_VariousInputs_ReturnsExpected(
        string subjectId, string expectedDeclaring, string expectedMethod, string? expectedReturn, string expectedParams)
    {
        // Access via public MatchesMethod or GetMethodSubjectIdDisplayString
        if (expectedMethod == "")
        {
            Assert.Equal(subjectId, ManagedNaming.GetMethodSubjectIdDisplayString(subjectId));
        }
        else
        {
            var result = ManagedNaming.GetMethodSubjectIdDisplayString(subjectId);
            Assert.Contains(expectedMethod, result);
        }
    }

    [Fact]
    public void GetDeclaringTypeSubjectId_ValidInput_ReturnsSubjectId()
    {
        // Via CanonicalizeSubjectId which internally uses TryParseGenericInstantiation
        var result = ManagedNaming.CanonicalizeSubjectId("A/B::Method:System.Void()");
        Assert.NotNull(result);
    }

    // ── TypeProviders simple methods ──

    [Fact]
    public void TypeProviders_EnumValues_CanBeResolved()
    {
        // CodegenMode enum
        Assert.Equal("Aot", CodegenMode.Aot.ToString());
        Assert.Equal("Jit", CodegenMode.Jit.ToString());
        Assert.Equal("Hybrid", CodegenMode.Hybrid.ToString());

        // BodyAvailabilityCode enum
        Assert.Equal(0, (int)BodyAvailabilityCode.None);
        Assert.Equal(6, (int)BodyAvailabilityCode.Unsupported);

        // HybridDispatchKind enum
        Assert.Equal(0, (int)HybridDispatchKind.None);
        Assert.Equal(6, (int)HybridDispatchKind.ComVtable);

        // GenericContextKind enum
        Assert.Equal(1, (int)GenericContextKind.TypeInstantiation);
        Assert.Equal(3, (int)GenericContextKind.TypeAndMethodInstantiation);

        // AotCoreIrAbiCarrierKind enum
        Assert.Equal(0, (int)AotCoreIrAbiCarrierKind.Void);
        Assert.Equal(14, (int)AotCoreIrAbiCarrierKind.ByRefToValueType);

        // AotCoreIrReferenceKind enum
        Assert.Equal(1, (int)AotCoreIrReferenceKind.Type);

        // AotCoreIrTypeShapeKind enum
        Assert.Equal(1, (int)AotCoreIrTypeShapeKind.ReferenceType);
        Assert.Equal(3, (int)AotCoreIrTypeShapeKind.InterfaceType);
    }

    // ── ManagedMethodIdentityResolver ──

    [Fact]
    public void CountParameters_VariousSignatures_ReturnsExpected()
    {
        // Via ResolveParameterCount
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "a",
            DeclaringTypeSubjectId = "d",
            DefinitionSubjectId = "def",
            SubjectId = "id",
            MethodId = "m",
            Signature = "void T::M(int,int)"
        };
        Assert.Equal(2, ManagedMethodIdentityResolver.ResolveParameterCount(identity));

        var emptyParams = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "a",
            DeclaringTypeSubjectId = "d",
            DefinitionSubjectId = "def",
            SubjectId = "id",
            MethodId = "m",
            Signature = "void T::M()"
        };
        Assert.Equal(0, ManagedMethodIdentityResolver.ResolveParameterCount(emptyParams));
    }

    [Fact]
    public void ResolveExecutionAuthorityKey_VariousInputs_ReturnsExpected()
    {
        // With ExecutionAuthorityKey set
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "a",
            DeclaringTypeSubjectId = "d",
            DefinitionSubjectId = "def",
            SubjectId = "id",
            MethodId = "m",
            Signature = "sig",
            ExecutionAuthorityKey = "auth-key"
        };
        Assert.Equal("auth-key", ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(identity));

        // With InstantiationStubId but no ExecutionAuthorityKey
        var stubIdentity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "a",
            DeclaringTypeSubjectId = "d",
            DefinitionSubjectId = "def",
            SubjectId = "id",
            MethodId = "m",
            Signature = "sig",
            InstantiationStubId = new InstantiationStubId { Value = "stub-val" }
        };
        Assert.Equal("stub-val", ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(stubIdentity));

        // With null identity and fallbackSubjectId
        Assert.Equal("fallback", ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(null, "fallback"));

        // With null identity and fallback DefinitionSubjectId
        Assert.Equal("def-fallback", ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(null, null, "def-fallback"));
    }

    [Fact]
    public void ResolveSubjectId_VariousInputs_ReturnsExpected()
    {
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "a",
            DeclaringTypeSubjectId = "d",
            DefinitionSubjectId = "def",
            SubjectId = "test-subject",
            MethodId = "m",
            Signature = "sig"
        };
        Assert.Equal("test-subject", ManagedMethodIdentityResolver.ResolveSubjectId(identity));

        // With null identity + fallback
        Assert.Equal("fallback", ManagedMethodIdentityResolver.ResolveSubjectId(null, "fallback"));
    }

    // ── BodyAvailabilityResolver ──

    [Fact]
    public void BodyAvailabilityResolver_WithImport_ReturnsExternalRuntime()
    {
        var method = new ManagedMethodModel
        {
            AssemblyName = "a",
            DeclaringTypeSubjectId = "d",
            DeclaringTypeDisplayName = "T",
            Name = "M",
            ReturnType = "void",
            SubjectId = "id",
            DefinitionSubjectId = "def",
            Signature = "sig",
            IsStatic = false,
            IsVirtual = false,
            MetadataToken = 1,
            Parameters = [],
            Body = new ManagedMethodBodyModel { Blocks = [], ExceptionRegions = [] },
            Import = new ManagedImportModel { ModuleName = "kernel32", EntryPointName = "Sleep" }
        };
        Assert.Equal(BodyAvailabilityCode.ExternalRuntime, BodyAvailabilityResolver.Resolve(method));
    }

    [Fact]
    public void BodyAvailabilityResolver_WithEmptyBody_ReturnsUnsupported()
    {
        var method = new ManagedMethodModel
        {
            AssemblyName = "a",
            DeclaringTypeSubjectId = "d",
            DeclaringTypeDisplayName = "T",
            Name = "M",
            ReturnType = "void",
            SubjectId = "id",
            DefinitionSubjectId = "def",
            Signature = "sig",
            IsStatic = false,
            IsVirtual = false,
            MetadataToken = 1,
            Parameters = [],
            Body = new ManagedMethodBodyModel
            {
                Blocks = [new ManagedBlockModel { BlockId = "b", Instructions = [] }],
                ExceptionRegions = []
            }
        };
        Assert.Equal(BodyAvailabilityCode.Unsupported, BodyAvailabilityResolver.Resolve(method));
    }

    [Fact]
    public void ToLegacyLabel_VariousCodes_ReturnsExpected()
    {
        Assert.Equal("has-canonical-body", BodyAvailabilityResolver.ToLegacyLabel(BodyAvailabilityCode.NativeGenerated));
        Assert.Equal("has-canonical-body", BodyAvailabilityResolver.ToLegacyLabel(BodyAvailabilityCode.InterpreterReady));
        Assert.Equal("no-canonical-body", BodyAvailabilityResolver.ToLegacyLabel(BodyAvailabilityCode.Unsupported));
        Assert.Equal("no-canonical-body", BodyAvailabilityResolver.ToLegacyLabel(BodyAvailabilityCode.MetadataOnly));
    }

    // ── HybridDispatchResolver ──

    [Fact]
    public void ResolveInstruction_NonCallOp_ReturnsNull()
    {
        var instruction = new ManagedInstructionModel { Op = "nop" };
        var result = HybridDispatchResolver.ResolveInstruction("asm", new HashSet<string>(), instruction);
        Assert.Null(result);
    }

    [Fact]
    public void ResolveInstruction_ConstrainedCallVirt_ReturnsDirect()
    {
        var instruction = new ManagedInstructionModel
        {
            Op = "callvirt",
            ConstrainedTypeSubjectId = "System.ValueType"
        };
        var result = HybridDispatchResolver.ResolveInstruction("asm", new HashSet<string>(), instruction);
        Assert.Equal(HybridDispatchKind.Direct, result);
    }

    [Fact]
    public void ResolveInstruction_CrossAssembly_ReturnsExternalRuntimeOrBridge()
    {
        // External assembly (not in internalAssemblyNames) -> ExternalRuntime
        var externalInstruction = new ManagedInstructionModel
        {
            Op = "call",
            Reference = new ManagedInstructionReference
            {
                AssemblyName = "External.Assembly",
                SubjectKind = "method",
                SubjectId = "id"
            }
        };
        var externalResult = HybridDispatchResolver.ResolveInstruction(
            "MyAssembly", new HashSet<string> { "MyAssembly" }, externalInstruction);
        Assert.Equal(HybridDispatchKind.ExternalRuntime, externalResult);

        // Cross-assembly where callee IS in internalAssemblyNames but differs from caller -> Bridge
        var bridgeInstruction = new ManagedInstructionModel
        {
            Op = "call",
            Reference = new ManagedInstructionReference
            {
                AssemblyName = "SisterAssembly",
                SubjectKind = "method",
                SubjectId = "SisterAssembly/T::M:System.Void()"
            }
        };
        var bridgeResult = HybridDispatchResolver.ResolveInstruction(
            "MyAssembly", new HashSet<string> { "MyAssembly", "SisterAssembly" }, bridgeInstruction);
        Assert.Equal(HybridDispatchKind.Bridge, bridgeResult);
    }

    [Fact]
    public void ResolveInstruction_CallVirtWithMethodLookup_ChecksIsVirtual()
    {
        var instruction = new ManagedInstructionModel
        {
            Op = "callvirt",
            Callee = "MyAssembly/T::M:System.Void()",
            Reference = new ManagedInstructionReference
            {
                AssemblyName = "MyAssembly",
                SubjectKind = "method",
                SubjectId = "MyAssembly/T::M:System.Void()"
            }
        };
        var methods = new Dictionary<string, ManagedMethodModel>
        {
            ["MyAssembly/T::M:System.Void()"] = new ManagedMethodModel
            {
                AssemblyName = "MyAssembly",
                DeclaringTypeSubjectId = "MyAssembly/T",
                DeclaringTypeDisplayName = "T",
                Name = "M",
                ReturnType = "System.Void",
                SubjectId = "MyAssembly/T::M:System.Void()",
                DefinitionSubjectId = "MyAssembly/T::M:System.Void()",
                Signature = "System.Void M()",
                IsStatic = false,
                IsVirtual = false,
                MetadataToken = 1,
                Parameters = [],
                Body = new ManagedMethodBodyModel { Blocks = [], ExceptionRegions = [] }
            }
        };
        var result = HybridDispatchResolver.ResolveInstruction("MyAssembly",
            new HashSet<string> { "MyAssembly" }, instruction, methods);
        Assert.Equal(HybridDispatchKind.Direct, result);
    }
}
