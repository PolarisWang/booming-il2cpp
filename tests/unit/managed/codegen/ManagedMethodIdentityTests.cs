using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class ManagedMethodIdentityTests
{
    private static ManagedMethodModel MakeMethod(string? importModuleName = null, bool? bodyEmpty = false)
    {
        var blocks = bodyEmpty == true
            ? new List<ManagedBlockModel>
            {
                new() { BlockId = "0", Instructions = [] },
            }
            : new List<ManagedBlockModel>
            {
                new() { BlockId = "0", Instructions = [new ManagedInstructionModel { Op = "ret" }] },
            };
        return new ManagedMethodModel
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/MyType",
            DeclaringTypeDisplayName = "MyType",
            Name = "Method",
            ReturnType = "System.Void",
            SubjectId = "TestAssembly/MyType::Method:System.Void()",
            DefinitionSubjectId = "TestAssembly/MyType",
            Signature = "System.Void()",
            IsStatic = false,
            IsVirtual = false,
            MetadataToken = 1,
            Parameters = [],
            Body = new ManagedMethodBodyModel
            {
                Blocks = blocks,
                ExceptionRegions = [],
            },
            Import = importModuleName is not null
                ? new ManagedImportModel { ModuleName = importModuleName, EntryPointName = "Entry" }
                : null,
        };
    }

    // ─────────────────────────────────────────────────────
    // ManagedMethodIdentityResolver.Create (from method)
    // ─────────────────────────────────────────────────────

    [Fact]
    public void Create_FromMethod_ReturnsIdentity()
    {
        var method = MakeMethod();
        var identity = ManagedMethodIdentityResolver.Create(method);
        Assert.NotNull(identity);
        Assert.Equal("TestAssembly", identity.AssemblyName);
        Assert.Equal("TestAssembly/MyType", identity.DeclaringTypeSubjectId);
        Assert.Equal("TestAssembly/MyType::Method:System.Void()", identity.SubjectId);
        Assert.Null(identity.ExecutionAuthorityKey);
    }

    [Fact]
    public void Create_FromMethod_WithExecutionAuthorityKey()
    {
        var method = MakeMethod();
        var identity = ManagedMethodIdentityResolver.Create(method, "CustomKey");
        Assert.Equal("CustomKey", identity.ExecutionAuthorityKey);
    }

    [Fact]
    public void Create_FromMethod_NullMethod_Throws()
    {
        Assert.Throws<ArgumentNullException>(() =>
            ManagedMethodIdentityResolver.Create((ManagedMethodModel)null!));
    }

    // ─────────────────────────────────────────────────────
    // ManagedMethodIdentityResolver.Create (from spec)
    // ─────────────────────────────────────────────────────

    [Fact]
    public void Create_FromSpec_ReturnsIdentity()
    {
        var spec = new ManagedMethodIdentitySpec
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/MyType",
            DeclaringTypeDisplayName = "MyType",
            MethodName = "Method",
            SubjectId = "TestAssembly/MyType::Method:System.Void()",
            Signature = "System.Void()",
        };
        var identity = ManagedMethodIdentityResolver.Create(spec);
        Assert.NotNull(identity);
        Assert.Equal("TestAssembly/MyType::Method:System.Void()", identity.SubjectId);
        Assert.Equal("TestAssembly/MyType", identity.DeclaringTypeSubjectId);
        // When DefinitionSubjectId is not provided, it defaults to SubjectId
        Assert.Equal("TestAssembly/MyType::Method:System.Void()", identity.DefinitionSubjectId);
    }

    [Fact]
    public void Create_FromSpec_WithDefinitionSubjectId()
    {
        var spec = new ManagedMethodIdentitySpec
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/MyType",
            DeclaringTypeDisplayName = "MyType",
            MethodName = "Method",
            SubjectId = "TestAssembly/MyType::Method:System.Void()",
            Signature = "System.Void()",
            DefinitionSubjectId = "TestAssembly/BaseType",
        };
        var identity = ManagedMethodIdentityResolver.Create(spec);
        Assert.Equal("TestAssembly/BaseType", identity.DefinitionSubjectId);
    }

    [Fact]
    public void Create_FromSpec_WithExecutionAuthorityKey()
    {
        var spec = new ManagedMethodIdentitySpec
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/MyType",
            DeclaringTypeDisplayName = "MyType",
            MethodName = "Method",
            SubjectId = "TestAssembly/MyType::Method:System.Void()",
            Signature = "System.Void()",
            ExecutionAuthorityKey = "AuthKey",
        };
        var identity = ManagedMethodIdentityResolver.Create(spec);
        Assert.Equal("AuthKey", identity.ExecutionAuthorityKey);
    }

    [Fact]
    public void Create_FromSpec_NullSpec_Throws()
    {
        Assert.Throws<ArgumentNullException>(() =>
            ManagedMethodIdentityResolver.Create((ManagedMethodIdentitySpec)null!));
    }

    [Fact]
    public void Create_FromSpec_EmptyAssemblyName_Throws()
    {
        var spec = new ManagedMethodIdentitySpec
        {
            AssemblyName = "",
            DeclaringTypeSubjectId = "Test",
            DeclaringTypeDisplayName = "Test",
            MethodName = "Test",
            SubjectId = "Test",
            Signature = "Test",
        };
        Assert.Throws<ArgumentException>(() => ManagedMethodIdentityResolver.Create(spec));
    }

    // ─────────────────────────────────────────────────────
    // ManagedMethodIdentityResolver.ResolveSubjectId
    // ─────────────────────────────────────────────────────

    [Fact]
    public void ResolveSubjectId_FromIdentity_ReturnsSubjectId()
    {
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "Test",
            DeclaringTypeSubjectId = "T",
            DefinitionSubjectId = "T",
            SubjectId = "T::M:System.Void()",
            MethodId = "M",
            Signature = "System.Void()",
        };
        Assert.Equal("T::M:System.Void()", ManagedMethodIdentityResolver.ResolveSubjectId(identity));
    }

    [Fact]
    public void ResolveSubjectId_FromIdentity_NullIdentity_Throws()
    {
        Assert.Throws<ArgumentNullException>(() =>
            ManagedMethodIdentityResolver.ResolveSubjectId((ManagedMethodIdentityArtifact)null!));
    }

    [Fact]
    public void ResolveSubjectId_FromIdentity_EmptySubjectId_Throws()
    {
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "Test",
            DeclaringTypeSubjectId = "T",
            DefinitionSubjectId = "T",
            SubjectId = "",
            MethodId = "M",
            Signature = "System.Void()",
        };
        Assert.Throws<ArgumentException>(() =>
            ManagedMethodIdentityResolver.ResolveSubjectId(identity));
    }

    [Fact]
    public void ResolveSubjectId_WithFallback_IdentityPresent_ReturnsIdentitySubjectId()
    {
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "Test",
            DeclaringTypeSubjectId = "T",
            DefinitionSubjectId = "T",
            SubjectId = "T::M:System.Void()",
            MethodId = "M",
            Signature = "System.Void()",
        };
        Assert.Equal("T::M:System.Void()",
            ManagedMethodIdentityResolver.ResolveSubjectId(identity, "fallback"));
    }

    [Fact]
    public void ResolveSubjectId_WithFallback_IdentityNull_ReturnsFallback()
    {
        Assert.Equal("fallbackSubjectId",
            ManagedMethodIdentityResolver.ResolveSubjectId(null, "fallbackSubjectId"));
    }

    [Fact]
    public void ResolveSubjectId_WithFallback_BothNull_Throws()
    {
        Assert.Throws<InvalidOperationException>(() =>
            ManagedMethodIdentityResolver.ResolveSubjectId(null, null));
    }

    // ─────────────────────────────────────────────────────
    // ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey
    // ─────────────────────────────────────────────────────

    [Fact]
    public void ResolveExecutionAuthorityKey_UsesExecutionAuthorityKey()
    {
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "Test",
            DeclaringTypeSubjectId = "T",
            DefinitionSubjectId = "Def",
            SubjectId = "T::M:System.Void()",
            MethodId = "M",
            Signature = "System.Void()",
            ExecutionAuthorityKey = "AuthKey",
        };
        Assert.Equal("AuthKey",
            ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(identity));
    }

    [Fact]
    public void ResolveExecutionAuthorityKey_UsesInstantiationStubId()
    {
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "Test",
            DeclaringTypeSubjectId = "T",
            DefinitionSubjectId = "Def",
            SubjectId = "T::M:System.Void()",
            MethodId = "M",
            Signature = "System.Void()",
            InstantiationStubId = new InstantiationStubId { Value = "StubKey" },
        };
        Assert.Equal("StubKey",
            ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(identity));
    }

    [Fact]
    public void ResolveExecutionAuthorityKey_UsesDefinitionSubjectId()
    {
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "Test",
            DeclaringTypeSubjectId = "T",
            DefinitionSubjectId = "Def",
            SubjectId = "T::M:System.Void()",
            MethodId = "M",
            Signature = "System.Void()",
        };
        Assert.Equal("Def",
            ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(identity));
    }

    [Fact]
    public void ResolveExecutionAuthorityKey_FallsBackToSubjectId()
    {
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "Test",
            DeclaringTypeSubjectId = "T",
            DefinitionSubjectId = "",
            SubjectId = "T::M:System.Void()",
            MethodId = "M",
            Signature = "System.Void()",
        };
        Assert.Equal("T::M:System.Void()",
            ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(identity));
    }

    [Fact]
    public void ResolveExecutionAuthorityKey_NullIdentity_Throws()
    {
        Assert.Throws<ArgumentNullException>(() =>
            ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey((ManagedMethodIdentityArtifact)null!));
    }

    [Fact]
    public void ResolveExecutionAuthorityKey_WithFallback_IdentityNull_UsesFallbackDefinition()
    {
        Assert.Equal("fallbackDef",
            ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(null, "fallbackSubj", "fallbackDef"));
    }

    [Fact]
    public void ResolveExecutionAuthorityKey_WithFallback_IdentityNullNoDefinition_UsesFallbackSubject()
    {
        Assert.Equal("fallbackSubj",
            ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(null, "fallbackSubj"));
    }

    [Fact]
    public void ResolveExecutionAuthorityKey_WithFallback_AllNull_Throws()
    {
        Assert.Throws<InvalidOperationException>(() =>
            ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(null, null, null));
    }

    // ─────────────────────────────────────────────────────
    // ManagedMethodIdentityResolver.ResolveParameterCount
    // ─────────────────────────────────────────────────────

    [Fact]
    public void ResolveParameterCount_FromSignature()
    {
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "Test",
            DeclaringTypeSubjectId = "T",
            DefinitionSubjectId = "T",
            SubjectId = "T::M:System.Void()",
            MethodId = "M",
            Signature = "System.Void(System.Int32,System.String)",
        };
        Assert.Equal(2, ManagedMethodIdentityResolver.ResolveParameterCount(identity));
    }

    [Fact]
    public void ResolveParameterCount_FromSignature_NoParams()
    {
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "Test",
            DeclaringTypeSubjectId = "T",
            DefinitionSubjectId = "T",
            SubjectId = "T::M:System.Void()",
            MethodId = "M",
            Signature = "System.Void()",
        };
        Assert.Equal(0, ManagedMethodIdentityResolver.ResolveParameterCount(identity));
    }

    [Fact]
    public void ResolveParameterCount_FromSubjectId()
    {
        Assert.Equal(2,
            ManagedMethodIdentityResolver.ResolveParameterCount(null,
                "Test/T::M:System.Void(System.Int32,System.String)"));
    }

    [Fact]
    public void ResolveParameterCount_IdentityWithEmptySignature_CountsFromSubjectId()
    {
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "Test",
            DeclaringTypeSubjectId = "T",
            DefinitionSubjectId = "T",
            SubjectId = "T::M:System.Void(System.Int32)",
            MethodId = "M",
            Signature = "",
        };
        Assert.Equal(1, ManagedMethodIdentityResolver.ResolveParameterCount(identity));
    }

    [Fact]
    public void ResolveParameterCount_WithGenericParams_CountsCorrectly()
    {
        var identity = new ManagedMethodIdentityArtifact
        {
            AssemblyName = "Test",
            DeclaringTypeSubjectId = "T",
            DefinitionSubjectId = "T",
            SubjectId = "T::M:System.Void(System.Collections.Generic.List<System.Int32>,System.String)",
            MethodId = "M",
            Signature = "System.Void(System.Collections.Generic.List<System.Int32>,System.String)",
        };
        Assert.Equal(2, ManagedMethodIdentityResolver.ResolveParameterCount(identity));
    }

    // ─────────────────────────────────────────────────────
    // BodyAvailabilityResolver
    // ─────────────────────────────────────────────────────

    [Fact]
    public void BodyAvailability_ExternalImport_ReturnsExternalRuntime()
    {
        var method = MakeMethod(importModuleName: "user32");
        Assert.Equal(BodyAvailabilityCode.ExternalRuntime,
            BodyAvailabilityResolver.Resolve(method));
    }

    [Fact]
    public void BodyAvailability_HasInstructions_ReturnsInterpreterReady()
    {
        var method = MakeMethod();
        Assert.Equal(BodyAvailabilityCode.InterpreterReady,
            BodyAvailabilityResolver.Resolve(method));
    }

    [Fact]
    public void BodyAvailability_EmptyBody_ReturnsUnsupported()
    {
        var method = MakeMethod(bodyEmpty: true);
        Assert.Equal(BodyAvailabilityCode.Unsupported,
            BodyAvailabilityResolver.Resolve(method));
    }

    [Fact]
    public void BodyAvailability_NullMethod_Throws()
    {
        Assert.Throws<ArgumentNullException>(() =>
            BodyAvailabilityResolver.Resolve(null!));
    }

    [Theory]
    [InlineData(BodyAvailabilityCode.NativeGenerated, "has-canonical-body")]
    [InlineData(BodyAvailabilityCode.InterpreterReady, "has-canonical-body")]
    [InlineData(BodyAvailabilityCode.Unsupported, "no-canonical-body")]
    [InlineData(BodyAvailabilityCode.ExternalRuntime, "no-canonical-body")]
    public void ToLegacyLabel_ReturnsExpected(BodyAvailabilityCode code, string expected)
    {
        Assert.Equal(expected, BodyAvailabilityResolver.ToLegacyLabel(code));
    }

    // ─────────────────────────────────────────────────────
    // HybridDispatchResolver.ResolveInstruction
    // ─────────────────────────────────────────────────────

    [Fact]
    public void HybridDispatch_NullCaller_Throws()
    {
        Assert.Throws<ArgumentNullException>(() =>
            HybridDispatchResolver.ResolveInstruction(null!, new HashSet<string>(), new ManagedInstructionModel
            {
                Op = "call",
            }));
    }

    [Fact]
    public void HybridDispatch_NotCallOrCallvirt_ReturnsNull()
    {
        var result = HybridDispatchResolver.ResolveInstruction(
            "TestAsm",
            new HashSet<string>(),
            new ManagedInstructionModel { Op = "ldarg" });
        Assert.Null(result);
    }

    [Fact]
    public void HybridDispatch_ConstrainedCallvirt_ReturnsDirect()
    {
        var result = HybridDispatchResolver.ResolveInstruction(
            "TestAsm",
            new HashSet<string>(),
            new ManagedInstructionModel
            {
                Op = "callvirt",
                ConstrainedTypeSubjectId = "SomeValueType",
            });
        Assert.Equal(HybridDispatchKind.Direct, result);
    }

    [Fact]
    public void HybridDispatch_NoAssemblyName_ReturnsUnsupported()
    {
        var result = HybridDispatchResolver.ResolveInstruction(
            "TestAsm",
            new HashSet<string>(),
            new ManagedInstructionModel { Op = "call" });
        Assert.Equal(HybridDispatchKind.Unsupported, result);
    }

    [Fact]
    public void HybridDispatch_ExternalAssembly_ReturnsExternalRuntime()
    {
        var result = HybridDispatchResolver.ResolveInstruction(
            "TestAsm",
            new HashSet<string> { "TestAsm" },
            new ManagedInstructionModel
            {
                Op = "call",
                Callee = "ExternalAsm/Type::Method:System.Void()",
            });
        Assert.Equal(HybridDispatchKind.ExternalRuntime, result);
    }

    [Fact]
    public void HybridDispatch_CrossAssemblyCall_ReturnsBridge()
    {
#pragma warning disable CS0618 // Bridge is obsolete but the resolver still returns it
        var result = HybridDispatchResolver.ResolveInstruction(
            "CallerAsm",
            new HashSet<string> { "CallerAsm", "CalleeAsm" },
            new ManagedInstructionModel
            {
                Op = "call",
                Callee = "CalleeAsm/Type::Method:System.Void()",
            });
        Assert.Equal(HybridDispatchKind.Bridge, result);
#pragma warning restore CS0618
    }

    [Fact]
    public void HybridDispatch_SameAssemblyCall_ReturnsDirect()
    {
        var result = HybridDispatchResolver.ResolveInstruction(
            "TestAsm",
            new HashSet<string> { "TestAsm" },
            new ManagedInstructionModel
            {
                Op = "call",
                Callee = "TestAsm/Type::Method:System.Void()",
            });
        Assert.Equal(HybridDispatchKind.Direct, result);
    }

    [Fact]
    public void HybridDispatch_CallvirtNonVirtual_ReturnsDirect()
    {
        var methods = new Dictionary<string, ManagedMethodModel>
        {
            ["TestAsm/Type::Method:System.Void()"] = MakeMethod(),
        };
        methods["TestAsm/Type::Method:System.Void()"] = methods["TestAsm/Type::Method:System.Void()"] with
        {
            SubjectId = "TestAsm/Type::Method:System.Void()",
            IsVirtual = false,
        };

        var result = HybridDispatchResolver.ResolveInstruction(
            "TestAsm",
            new HashSet<string> { "TestAsm" },
            new ManagedInstructionModel
            {
                Op = "callvirt",
                Callee = "TestAsm/Type::Method:System.Void()",
            },
            methods);
        Assert.Equal(HybridDispatchKind.Direct, result);
    }

    [Fact]
    public void HybridDispatch_CallvirtVirtual_ReturnsVirtual()
    {
        var methods = new Dictionary<string, ManagedMethodModel>
        {
            ["TestAsm/Type::Method:System.Void()"] = MakeMethod() with
            {
                SubjectId = "TestAsm/Type::Method:System.Void()",
                IsVirtual = true,
            },
        };

        var result = HybridDispatchResolver.ResolveInstruction(
            "TestAsm",
            new HashSet<string> { "TestAsm" },
            new ManagedInstructionModel
            {
                Op = "callvirt",
                Callee = "TestAsm/Type::Method:System.Void()",
            },
            methods);
        Assert.Equal(HybridDispatchKind.Virtual, result);
    }

    [Fact]
    public void HybridDispatch_CallvirtVirtual_MissingMethodLookup_ReturnsVirtual()
    {
        var result = HybridDispatchResolver.ResolveInstruction(
            "TestAsm",
            new HashSet<string> { "TestAsm" },
            new ManagedInstructionModel
            {
                Op = "callvirt",
                Callee = "TestAsm/Type::Method:System.Void()",
            });
        Assert.Equal(HybridDispatchKind.Virtual, result);
    }

    [Fact]
    public void HybridDispatch_CalleeThroughReference()
    {
        var result = HybridDispatchResolver.ResolveInstruction(
            "TestAsm",
            new HashSet<string> { "TestAsm" },
            new ManagedInstructionModel
            {
                Op = "call",
                Reference = new ManagedInstructionReference
                {
                    AssemblyName = "OtherAsm",
                    SubjectId = "OtherAsm/Type::Method:System.Void()",
                    SubjectKind = "method",
                },
            });
        Assert.Equal(HybridDispatchKind.ExternalRuntime, result);
    }
}
