using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.MetadataWriter.Tests;

/// <summary>
/// Core path tests for MetadataWriterStage.Write().
/// Validates all three output artifacts: AotManifest, MetadataRegistration, SupplementalMetadataTemplate.
/// </summary>
public sealed class MetadataWriterStageCoreTests
{
    // ──────────────────────────────────────────────
    // Helpers — construct minimal LinkedWorldModel
    // ──────────────────────────────────────────────

    private static LinkedWorldModel CreateMinimalWorld()
    {
        return new LinkedWorldModel
        {
            InputAssemblyPath = "TestAssembly.dll",
            FullAssemblyClosure = false,
            Assembly = new AssemblyIdentityModel { Name = "TestAssembly", ModuleVersionId = Guid.Empty },
            Assemblies = Array.Empty<AssemblyIdentityModel>(),
            EntryPointSubjectId = "Test.Program::Main",
            GenericInstantiationDemandGraph = null,
            Types = Array.Empty<ManagedTypeModel>(),
            Fields = Array.Empty<ManagedFieldModel>(),
            Properties = Array.Empty<ManagedPropertyModel>(),
            Methods = Array.Empty<ManagedMethodModel>(),
            Dependencies = Array.Empty<LinkedDependencyModel>(),
            CanonicalSubjects = new CanonicalSubjectsModel { Subjects = Array.Empty<CanonicalSubjectModel>() },
            SemanticShapes = new SemanticShapesModel
            {
                Types = Array.Empty<TypeShapeModel>(),
                Fields = Array.Empty<FieldShapeModel>(),
                Properties = Array.Empty<PropertyShapeModel>(),
                Methods = Array.Empty<MethodShapeModel>(),
            },
            CapabilityBundles = new CapabilityBundlesModel
            {
                World = new WorldCapabilityBundleModel { Capabilities = Array.Empty<string>() },
                Methods = Array.Empty<MethodCapabilityBundleModel>(),
            },
            OptimizationFacts = new OptimizationFactsArtifact
            {
                ClosedWorldSpecializations = Array.Empty<ClosedWorldSpecializationFact>(),
                DispatchFacts = Array.Empty<DispatchFact>(),
                LayoutFacts = Array.Empty<LayoutFact>(),
                ExceptionFacts = Array.Empty<ExceptionFact>(),
            },
            PreserveDescriptor = new PreserveDescriptorArtifact
            {
                Entries = Array.Empty<PreserveDescriptorEntry>(),
            },
        };
    }

    private static ManagedMethodModel CreateMethod(
        string subjectId,
        string name,
        string returnType,
        string declaringTypeSubjectId,
        bool isEntryPoint = false,
        ManagedImportModel? import = null,
        IReadOnlyList<ManagedParameterModel>? parameters = null)
    {
        return new ManagedMethodModel
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = declaringTypeSubjectId,
            DeclaringTypeDisplayName = declaringTypeSubjectId.Split('.').LastOrDefault() ?? declaringTypeSubjectId,
            Name = name,
            ReturnType = returnType,
            SubjectId = subjectId,
            DefinitionSubjectId = subjectId,
            Signature = $"{returnType} {name}()",
            IsStatic = true,
            IsVirtual = false,
            MetadataToken = 0x06000001,
            Parameters = parameters ?? Array.Empty<ManagedParameterModel>(),
            Import = import,
            Body = new ManagedMethodBodyModel
            {
                Blocks = Array.Empty<ManagedBlockModel>(),
                ExceptionRegions = Array.Empty<ManagedExceptionRegionModel>(),
            },
            RuntimeGenericContext = null,
        };
    }

    // ──────────────────────────────────────────────
    // AotManifest tests
    // ──────────────────────────────────────────────

    [Fact]
    public void Write_MinimalWorld_ReturnsSuccess()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        var result = stage.Write(world);

        Assert.True(result.IsSuccess);
        Assert.NotNull(result.Value);
    }

    [Fact]
    public void Write_WithMethods_CreatesAotManifestEntries()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        var method1 = CreateMethod("Test.A::Foo", "Foo", "System.Void", "Test.A");
        var method2 = CreateMethod("Test.B::Bar", "Bar", "System.Int32", "Test.B");
        world = world with
        {
            Methods = new[] { method1, method2 },
        };

        var result = stage.Write(world);
        var manifest = result.Value!.AotManifest;

        Assert.Equal(2, manifest.Entries.Count);
        Assert.Contains(manifest.Entries, e =>
            e.SubjectId == "Test.A::Foo" &&
            e.SubjectKind == "method" &&
            e.AssemblyName == "TestAssembly" &&
            e.Reason == "generated-direct-call");
        Assert.Contains(manifest.Entries, e =>
            e.SubjectId == "Test.B::Bar" &&
            e.Reason == "generated-direct-call");
    }

    [Fact]
    public void Write_WithEntryPointMethod_MarksAsEntrypoint()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        var entryMethod = CreateMethod("Test.Program::Main", "Main", "System.Void", "Test.Program");
        world = world with
        {
            EntryPointSubjectId = "Test.Program::Main",
            Methods = new[] { entryMethod },
        };

        var result = stage.Write(world);
        var manifest = result.Value!.AotManifest;

        var entry = Assert.Single(manifest.Entries);
        Assert.Equal("entrypoint", entry.Reason);
    }

    [Fact]
    public void Write_WithFields_CreatesAotManifestFieldEntries()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        world = world with
        {
            Fields = new[]
            {
                new ManagedFieldModel
                {
                    AssemblyName = "TestAssembly",
                    DeclaringTypeSubjectId = "Test.A",
                    Name = "_value",
                    FieldType = "System.Int32",
                    SubjectId = "Test.A::_value",
                    DefinitionSubjectId = "Test.A::_value",
                    MetadataToken = 0x04000001,
                },
            },
        };

        var result = stage.Write(world);
        var manifest = result.Value!.AotManifest;

        var fieldEntry = Assert.Single(manifest.Entries);
        Assert.Equal("field", fieldEntry.SubjectKind);
        Assert.Equal("instance-field-state", fieldEntry.Reason);
    }

    [Fact]
    public void Write_WithDependencies_CreatesAotManifestDependencyEntries()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        world = world with
        {
            Dependencies = new[]
            {
                new LinkedDependencyModel
                {
                    AssemblyName = "System.Runtime",
                    SubjectKind = "type",
                    SubjectId = "System.Console",
                    Reason = "call-site",
                },
            },
        };

        var result = stage.Write(world);
        var manifest = result.Value!.AotManifest;

        var depEntry = Assert.Single(manifest.Entries);
        Assert.Equal("type", depEntry.SubjectKind);
        Assert.Equal("System.Console", depEntry.SubjectId);
        Assert.Equal("call-site", depEntry.Reason);
    }

    // ──────────────────────────────────────────────
    // MetadataRegistration tests
    // ──────────────────────────────────────────────

    [Fact]
    public void Write_WithAssembliesAndTypes_CreatesRegistrationSlots()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        world = world with
        {
            Assemblies = new[]
            {
                new AssemblyIdentityModel { Name = "TestAssembly", ModuleVersionId = Guid.Empty },
                new AssemblyIdentityModel { Name = "System.Runtime", ModuleVersionId = Guid.Empty },
            },
            Types = new[]
            {
                new ManagedTypeModel
                {
                    AssemblyName = "TestAssembly",
                    NamespaceName = "Test",
                    Name = "A",
                    SubjectId = "Test.A",
                    DefinitionSubjectId = "Test.A",
                    DisplayName = "A",
                    MetadataToken = 0x02000001,
                },
                new ManagedTypeModel
                {
                    AssemblyName = "TestAssembly",
                    NamespaceName = "Test",
                    Name = "B",
                    SubjectId = "Test.B",
                    DefinitionSubjectId = "Test.B",
                    DisplayName = "B",
                    MetadataToken = 0x02000002,
                },
            },
        };

        var result = stage.Write(world);
        var registration = result.Value!.MetadataRegistration;

        // Slot 0 = first assembly, Slot 1 = second assembly
        // Slot 2 = first type, Slot 3 = second type
        Assert.Equal(4, registration.Registrations.Count);

        Assert.Equal("assembly", registration.Registrations[0].RegistrationKind);
        Assert.Equal(0, registration.Registrations[0].Slot);
        Assert.Equal("TestAssembly", registration.Registrations[0].SubjectId);

        Assert.Equal("assembly", registration.Registrations[1].RegistrationKind);
        Assert.Equal(1, registration.Registrations[1].Slot);
        Assert.Equal("System.Runtime", registration.Registrations[1].SubjectId);

        Assert.Equal("type", registration.Registrations[2].RegistrationKind);
        Assert.Equal(2, registration.Registrations[2].Slot);
        Assert.Equal("Test.A", registration.Registrations[2].SubjectId);
        Assert.Equal("A", registration.Registrations[2].Name);
        Assert.Equal("Test", registration.Registrations[2].NamespaceName);

        Assert.Equal("type", registration.Registrations[3].RegistrationKind);
        Assert.Equal(3, registration.Registrations[3].Slot);
        Assert.Equal("Test.B", registration.Registrations[3].SubjectId);
    }

    [Fact]
    public void Write_WithMethodsAndParameters_CreatesMethodAndParameterSlots()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        world = world with
        {
            Methods = new[]
            {
                CreateMethod(
                    subjectId: "Test.A::Foo",
                    name: "Foo",
                    returnType: "System.Int32",
                    declaringTypeSubjectId: "Test.A",
                    parameters: new[]
                    {
                        new ManagedParameterModel { Name = "x", Type = "System.Int32" },
                        new ManagedParameterModel { Name = "y", Type = "System.String" },
                    }),
            },
        };

        var result = stage.Write(world);
        var registration = result.Value!.MetadataRegistration;

        // Slot 0 = method, Slots 1-2 = parameters
        Assert.Equal(3, registration.Registrations.Count);

        var methodEntry = registration.Registrations[0];
        Assert.Equal("method", methodEntry.RegistrationKind);
        Assert.Equal(0, methodEntry.Slot);
        Assert.Equal("Test.A::Foo", methodEntry.SubjectId);
        Assert.Equal("System.Int32", methodEntry.MemberType);
        Assert.Equal(2, methodEntry.ParameterCount);

        var param1 = registration.Registrations[1];
        Assert.Equal("parameter", param1.RegistrationKind);
        Assert.Equal(1, param1.Slot);
        Assert.Equal(0, param1.ParameterIndex);

        var param2 = registration.Registrations[2];
        Assert.Equal("parameter", param2.RegistrationKind);
        Assert.Equal(2, param2.Slot);
        Assert.Equal(1, param2.ParameterIndex);
    }

    [Fact]
    public void Write_MethodWithImport_SetsImportFields()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        world = world with
        {
            Methods = new[]
            {
                CreateMethod(
                    subjectId: "Test.PInvoke::MessageBox",
                    name: "MessageBox",
                    returnType: "System.Int32",
                    declaringTypeSubjectId: "Test.PInvoke",
                    import: new ManagedImportModel
                    {
                        ModuleName = "user32.dll",
                        EntryPointName = "MessageBoxW",
                        CallingConvention = 0x0300, // StdCall
                        CharSet = 0x0004, // Unicode
                        SetLastError = true,
                        IsSuppressGCTransition = false,
                    }),
            },
        };

        var result = stage.Write(world);
        var registration = result.Value!.MetadataRegistration;

        var methodEntry = registration.Registrations[0];
        Assert.Equal("method", methodEntry.RegistrationKind);
        Assert.True(methodEntry.IsImported);
        Assert.Equal("user32.dll", methodEntry.ImportModuleName);
        Assert.Equal("MessageBoxW", methodEntry.ImportEntryPointName);
    }

    // ──────────────────────────────────────────────
    // SupplementalMetadataTemplate tests
    // ──────────────────────────────────────────────

    [Fact]
    public void Write_WithTypes_CreatesSupplementalTypeTemplates()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        world = world with
        {
            Types = new[]
            {
                new ManagedTypeModel
                {
                    AssemblyName = "TestAssembly",
                    NamespaceName = "Test",
                    Name = "A",
                    SubjectId = "Test.A",
                    DefinitionSubjectId = "Test.A",
                    DisplayName = "A",
                    MetadataToken = 0x02000001,
                },
            },
        };

        var result = stage.Write(world);
        var template = result.Value!.SupplementalMetadataTemplate;

        var typeEntry = Assert.Single(template.RegisteredTypes);
        Assert.Equal("TestAssembly", typeEntry.AssemblyName);
        Assert.Equal("Test.A", typeEntry.SubjectId);
        Assert.Equal(0x02000001, typeEntry.MetadataToken);
    }

    [Fact]
    public void Write_WithMethods_CreatesSupplementalMethodTemplates()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        world = world with
        {
            Methods = new[]
            {
                CreateMethod("Test.A::Foo", "Foo", "System.Void", "Test.A"),
            },
        };

        var result = stage.Write(world);
        var template = result.Value!.SupplementalMetadataTemplate;

        var methodEntry = Assert.Single(template.RegisteredMethods);
        Assert.Equal("TestAssembly", methodEntry.AssemblyName);
        Assert.Equal("Test.A::Foo", methodEntry.SubjectId);
        Assert.Equal("Test.A", methodEntry.DeclaringTypeSubjectId);
    }

    [Fact]
    public void Write_WithGenericDemand_ResolvesRuntimeGenericContext()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        var demand = new GenericInstantiationDemandModel
        {
            RequestingAssemblyName = "TestAssembly",
            OwningAssemblyName = "TestAssembly",
            SubjectKind = "method",
            SubjectId = "Test.A::Foo",
            DefinitionSubjectId = "Test.A::Foo",
            DemandSourceKind = "direct-call",
            InstantiationKey = new GenericInstantiationKey
            {
                ContextKind = GenericContextKind.TypeInstantiation,
                DefinitionSubjectId = "Test.A::Foo",
                TypeArguments = new[] { "System.Int32" },
                MethodArguments = Array.Empty<string>(),
            },
            SupportKindCode = GenericSupportKind.Legal,
            SpecializationKindCode = GenericSpecializationKind.SpecializedBody,
            FamilyKindCode = GenericDemandFamilyKind.ClosedGenericMethod,
        };

        world = world with
        {
            GenericInstantiationDemandGraph = new GenericInstantiationDemandGraphModel
            {
                Demands = new[] { demand },
            },
            Methods = new[]
            {
                CreateMethod("Test.A::Foo", "Foo", "System.Void", "Test.A"),
            },
        };

        var result = stage.Write(world);
        var template = result.Value!.SupplementalMetadataTemplate;

        var methodEntry = Assert.Single(template.RegisteredMethods);
        Assert.NotNull(methodEntry.RuntimeGenericContext);
        Assert.Equal(GenericSupportKind.Legal, methodEntry.RuntimeGenericContext!.SupportKindCode);
        Assert.Equal(GenericSpecializationKind.SpecializedBody, methodEntry.RuntimeGenericContext.SpecializationKindCode);
        Assert.NotNull(methodEntry.GenericDiagnostic);
    }

    [Fact]
    public void Write_WithoutGenericDemand_GenericContextIsNull()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        world = world with
        {
            GenericInstantiationDemandGraph = null,
            Methods = new[]
            {
                CreateMethod("Test.A::Foo", "Foo", "System.Void", "Test.A"),
            },
        };

        var result = stage.Write(world);
        var template = result.Value!.SupplementalMetadataTemplate;

        var methodEntry = Assert.Single(template.RegisteredMethods);
        Assert.Null(methodEntry.RuntimeGenericContext);
        Assert.Null(methodEntry.GenericDiagnostic);
    }

    [Fact]
    public void Write_WithReservedSlots_ReturnsDefaultValues()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        var result = stage.Write(world);
        var template = result.Value!.SupplementalMetadataTemplate;

        Assert.Equal(256, template.ReservedSlots.TypeCount);
        Assert.Equal(1024, template.ReservedSlots.MethodCount);
        Assert.Equal(256, template.ReservedSlots.GenericInstantiationCount);
    }

    // ──────────────────────────────────────────────
    // Property registration tests
    // ──────────────────────────────────────────────

    [Fact]
    public void Write_WithProperties_CreatesPropertyRegistrations()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        world = world with
        {
            Properties = new[]
            {
                new ManagedPropertyModel
                {
                    AssemblyName = "TestAssembly",
                    DeclaringTypeSubjectId = "Test.A",
                    Name = "Value",
                    PropertyType = "System.Int32",
                    SubjectId = "Test.A::get_Value",
                    DefinitionSubjectId = "Test.A::get_Value",
                    MetadataToken = 0x17000001,
                },
            },
        };

        var result = stage.Write(world);
        var registration = result.Value!.MetadataRegistration;

        var propEntry = Assert.Single(registration.Registrations);
        Assert.Equal("property", propEntry.RegistrationKind);
        Assert.Equal(0, propEntry.Slot);
        Assert.Equal("Test.A::get_Value", propEntry.SubjectId);
        Assert.Equal("System.Int32", propEntry.MemberType);
    }

    // ──────────────────────────────────────────────
    // Field registration tests
    // ──────────────────────────────────────────────

    [Fact]
    public void Write_WithFieldRegistration_PopulatesAllFieldFields()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        world = world with
        {
            Fields = new[]
            {
                new ManagedFieldModel
                {
                    AssemblyName = "TestAssembly",
                    DeclaringTypeSubjectId = "Test.A",
                    Name = "_count",
                    FieldType = "System.Int32",
                    SubjectId = "Test.A::_count",
                    DefinitionSubjectId = "Test.A::_count",
                    MetadataToken = 0x04000001,
                },
            },
            // Need at least one assembly to populate assembly slots first
            Assemblies = new[]
            {
                new AssemblyIdentityModel { Name = "TestAssembly", ModuleVersionId = Guid.Empty },
            },
        };

        var result = stage.Write(world);
        var registration = result.Value!.MetadataRegistration;

        // Slot 0 = assembly, Slot 1 = field
        Assert.Equal(2, registration.Registrations.Count);

        var fieldEntry = registration.Registrations[1];
        Assert.Equal("field", fieldEntry.RegistrationKind);
        Assert.Equal(1, fieldEntry.Slot);
        Assert.Equal("Test.A::_count", fieldEntry.SubjectId);
        Assert.Equal("Test.A", fieldEntry.DeclaringTypeSubjectId);
        Assert.Equal("System.Int32", fieldEntry.MemberType);
    }

    // ──────────────────────────────────────────────
    // Full artifact format contract test
    // ──────────────────────────────────────────────

    [Fact]
    public void Write_Output_HasCorrectArtifactFormat()
    {
        var stage = new MetadataWriterStage();
        var world = CreateMinimalWorld();

        var result = stage.Write(world);
        var output = result.Value!;

        // AotManifest format
        Assert.Equal("v0", output.AotManifest.FormatVersion);
        Assert.Equal("aotManifest", output.AotManifest.ArtifactKind);

        // MetadataRegistration format
        Assert.Equal("v0", output.MetadataRegistration.FormatVersion);
        Assert.Equal("metadataRegistration", output.MetadataRegistration.ArtifactKind);

        // SupplementalMetadataTemplate format
        Assert.Equal("v0", output.SupplementalMetadataTemplate.FormatVersion);
        Assert.Equal("supplementalMetadataTemplate", output.SupplementalMetadataTemplate.ArtifactKind);
    }
}
