using Chaos.IL2CPP.CodeGen.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests.Emission;

/// <summary>
/// Tests that exercise code paths driven by populated MetadataRegistration
/// and SupplementalMetadata artifacts. These paths include MetadataTokenLookup,
/// VTableSlot descriptor emission, and generic registration emission.
/// </summary>
public sealed class MetadataRegistrationTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    /// <summary>
    /// Exercises MetadataTokenLookup with type and method registration entries.
    /// The planner creates a MetadataTokenLookup from the registrations, which
    /// is used by VTableSlot descriptor emission (non-zero type/method tokens).
    /// </summary>
    [Fact]
    public void PopulatedMetadataRegistration_TriggersTokenLookupPaths()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestMeta:TestType(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var artifact = ModelFactory.CreateArtifact(method);
        var loweringPlan = ModelFactory.CreateDefaultPlan(method.SubjectId);
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: PlannerFixture.StubAssemblyPath);

        // Metadata registration with type and method entries
        var metadataRegistration = new MetadataRegistrationArtifact
        {
            Registrations = new MetadataRegistrationEntry[]
            {
                new()
                {
                    RegistrationKind = "type",
                    Slot = 0x02000001,
                    SubjectId = "System.Private.CoreLib/System.Object",
                    Name = "Object",
                    NamespaceName = "System",
                },
                new()
                {
                    RegistrationKind = "type",
                    Slot = 0x02000002,
                    SubjectId = "System.Private.CoreLib/System.String",
                    Name = "String",
                    NamespaceName = "System",
                },
                new()
                {
                    RegistrationKind = "type",
                    Slot = 0x02000003,
                    SubjectId = "System.Private.CoreLib/System.Int32",
                    Name = "Int32",
                    NamespaceName = "System",
                },
                new()
                {
                    RegistrationKind = "method",
                    Slot = 0x06000001,
                    SubjectId = "TestModule.TestClass::TestMeta",
                    Name = "TestMeta",
                },
                new()
                {
                    RegistrationKind = "method",
                    Slot = 0x06000002,
                    SubjectId = "System.Private.CoreLib/System.Object::ToString",
                    Name = "ToString",
                    ParameterCount = 0,
                },
            },
        };

        var templateModel = _fixture.RunPlanner(
            artifact, loweringPlan, manifest, metadataRegistration);

        Assert.NotNull(templateModel);
        Assert.NotEmpty(templateModel.ObjectModelCode);
    }

    /// <summary>
    /// Exercises supplemental metadata with a registered type entry.
    /// This triggers the type registration path in the planner.
    /// </summary>
    [Fact]
    public void SupplementalTypeRegistration_EmitsTypeEntry()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::SuppType:TestType(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var artifact = ModelFactory.CreateArtifact(method);
        var loweringPlan = ModelFactory.CreateDefaultPlan(method.SubjectId);
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: PlannerFixture.StubAssemblyPath);

        var supplementalMetadata = new SupplementalMetadataTemplateArtifact
        {
            RegisteredTypes = new SupplementalMetadataTypeTemplateEntry[]
            {
                new()
                {
                    AssemblyName = "System.Private.CoreLib",
                    SubjectId = "System.Private.CoreLib/System.Collections.Generic.List`1[[System.Int32]]",
                    DefinitionSubjectId = "System.Private.CoreLib/System.Collections.Generic.List`1",
                    MetadataToken = 0x02000010,
                },
            },
            RegisteredMethods = Array.Empty<SupplementalMetadataMethodTemplateEntry>(),
            ReservedSlots = new SupplementalMetadataReservedSlots(),
        };

        var metadataRegistration = new MetadataRegistrationArtifact
        {
            Registrations = new MetadataRegistrationEntry[]
            {
                new()
                {
                    RegistrationKind = "type",
                    Slot = 0x02000010,
                    SubjectId = "System.Private.CoreLib/System.Collections.Generic.List`1",
                    Name = "List`1",
                    NamespaceName = "System.Collections.Generic",
                },
            },
        };

        var templateModel = _fixture.RunPlanner(
            artifact, loweringPlan, manifest,
            metadataRegistration, supplementalMetadata);

        Assert.NotNull(templateModel);
        Assert.NotEmpty(templateModel.ObjectModelCode);
    }

    /// <summary>
    /// Exercises supplemental metadata with a registered method entry.
    /// </summary>
    [Fact]
    public void SupplementalMethodRegistration_EmitsMethodEntry()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::SuppMethod:TestType(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var artifact = ModelFactory.CreateArtifact(method);
        var loweringPlan = ModelFactory.CreateDefaultPlan(method.SubjectId);
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: PlannerFixture.StubAssemblyPath);

        var supplementalMetadata = new SupplementalMetadataTemplateArtifact
        {
            RegisteredTypes = Array.Empty<SupplementalMetadataTypeTemplateEntry>(),
            RegisteredMethods = new SupplementalMetadataMethodTemplateEntry[]
            {
                new()
                {
                    AssemblyName = "TestModule",
                    SubjectId = "TestModule.TestClass::TestMethod",
                    DefinitionSubjectId = "TestModule.TestClass::TestMethod",
                    DeclaringTypeSubjectId = "TestModule.TestClass",
                    MetadataToken = 0x06000050,
                    ParameterCount = 0,
                },
            },
            ReservedSlots = new SupplementalMetadataReservedSlots(),
        };

        var templateModel = _fixture.RunPlanner(
            artifact, loweringPlan, manifest,
            supplementalMetadata: supplementalMetadata);

        Assert.NotNull(templateModel);
        Assert.NotEmpty(templateModel.ObjectModelCode);
    }

    public void Dispose()
    {
    }
}
