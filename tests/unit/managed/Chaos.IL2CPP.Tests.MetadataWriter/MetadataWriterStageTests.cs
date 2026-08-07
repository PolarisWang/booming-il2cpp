using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.MetadataWriter;
using Xunit;

// All result.Value accesses are guarded by runtime Assertions that the
// compiler cannot statically verify, so suppress the nullable dereference
// warning for the entire file.
#pragma warning disable CS8602

namespace Chaos.IL2CPP.Tests.MetadataWriter;

public class MetadataWriterStageTests
{
    /// <summary>
    /// Helper: build a minimal <see cref="LinkedWorldModel"/> with the specified collections.
    /// All non-specified collections default to empty.
    /// </summary>
    private static LinkedWorldModel CreateLinkedWorld(
        AssemblyIdentityModel? assembly = null,
        IReadOnlyList<AssemblyIdentityModel>? assemblies = null,
        string entryPointSubjectId = "",
        IReadOnlyList<ManagedTypeModel>? types = null,
        IReadOnlyList<ManagedFieldModel>? fields = null,
        IReadOnlyList<ManagedPropertyModel>? properties = null,
        IReadOnlyList<ManagedMethodModel>? methods = null,
        IReadOnlyList<LinkedDependencyModel>? dependencies = null,
        GenericInstantiationDemandGraphModel? genericInstantiationDemandGraph = null)
    {
        return new LinkedWorldModel
        {
            InputAssemblyPath = "test-assembly.dll",
            FullAssemblyClosure = false,
            Assembly = assembly ?? new AssemblyIdentityModel
            {
                Name = "TestAssembly",
                ModuleVersionId = Guid.NewGuid(),
            },
            Assemblies = assemblies ?? [],
            EntryPointSubjectId = entryPointSubjectId,
            GenericInstantiationDemandGraph = genericInstantiationDemandGraph,
            Types = types ?? [],
            Fields = fields ?? [],
            Properties = properties ?? [],
            Methods = methods ?? [],
            Dependencies = dependencies ?? [],
            CanonicalSubjects = new CanonicalSubjectsModel { Subjects = [] },
            SemanticShapes = new SemanticShapesModel
            {
                Types = [],
                Fields = [],
                Properties = [],
                Methods = [],
            },
            CapabilityBundles = new CapabilityBundlesModel
            {
                World = new WorldCapabilityBundleModel { Capabilities = [] },
                Methods = [],
            },
            OptimizationFacts = new OptimizationFactsArtifact
            {
                ClosedWorldSpecializations = [],
                DispatchFacts = [],
                LayoutFacts = [],
                ExceptionFacts = [],
            },
            PreserveDescriptor = new PreserveDescriptorArtifact
            {
                Entries = [],
            },
        };
    }

    // ───────────────────────────────
    // AotManifest artifact tests
    // ───────────────────────────────

    [Fact]
    public void Write_EmptyWorld_ReturnsSuccess()
    {
        var stage = new MetadataWriterStage();
        var world = CreateLinkedWorld();

        var result = stage.Write(world);

        Assert.True(result.IsSuccess, $"Expected success but got failure: {result.Error?.Code}");
        Assert.NotNull(result.Value);
        Assert.NotNull(result.Value.AotManifest);
        Assert.NotNull(result.Value.MetadataRegistration);
        Assert.NotNull(result.Value.SupplementalMetadataTemplate);
    }

    [Fact]
    public void Write_EmptyWorld_AotManifestHasNoEntries()
    {
        var stage = new MetadataWriterStage();
        var world = CreateLinkedWorld();

        var result = stage.Write(world);

        Assert.Empty(result.Value.AotManifest.Entries);
    }

    [Fact]
    public void Write_SimpleMethod_AotManifestContainsEntry()
    {
        var method = new ManagedMethodModel
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/MyType",
            DeclaringTypeDisplayName = "MyType",
            Name = "DoSomething",
            ReturnType = "System.Void",
            SubjectId = "TestAssembly/MyType::DoSomething:System.Void()",
            DefinitionSubjectId = "TestAssembly/MyType::DoSomething:System.Void()",
            Signature = "System.Void MyType::DoSomething()",
            IsStatic = false,
            IsVirtual = false,
            IsPreserved = false,
            IsUnmanagedCallersOnly = false,
            MetadataToken = 0x06000001,
            Parameters = [],
            Body = new ManagedMethodBodyModel { Blocks = [], ExceptionRegions = [] },
        };
        var world = CreateLinkedWorld(methods: new[] { method });

        var result = new MetadataWriterStage().Write(world);

        var entry = Assert.Single(result.Value.AotManifest.Entries);
        Assert.Equal("TestAssembly", entry.AssemblyName);
        Assert.Equal("method", entry.SubjectKind);
        Assert.Equal(method.SubjectId, entry.SubjectId);
        Assert.Equal("generated-direct-call", entry.Reason);
    }

    [Fact]
    public void Write_EntryPointMethod_HasEntryPointReason()
    {
        var method = new ManagedMethodModel
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/Program",
            DeclaringTypeDisplayName = "Program",
            Name = "Main",
            ReturnType = "System.Void",
            SubjectId = "TestAssembly/Program::Main:System.Void()",
            DefinitionSubjectId = "TestAssembly/Program::Main:System.Void()",
            Signature = "System.Void Program::Main()",
            IsStatic = true,
            IsVirtual = false,
            IsPreserved = false,
            IsUnmanagedCallersOnly = false,
            MetadataToken = 0x06000001,
            Parameters = [],
            Body = new ManagedMethodBodyModel { Blocks = [], ExceptionRegions = [] },
        };
        var world = CreateLinkedWorld(
            methods: new[] { method },
            entryPointSubjectId: method.SubjectId);

        var result = new MetadataWriterStage().Write(world);

        var entry = Assert.Single(result.Value.AotManifest.Entries);
        Assert.Equal("entrypoint", entry.Reason);
    }

    [Fact]
    public void Write_Field_AotManifestContainsFieldEntry()
    {
        var field = new ManagedFieldModel
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/MyType",
            Name = "_value",
            FieldType = "System.Int32",
            SubjectId = "TestAssembly/MyType::_value",
            DefinitionSubjectId = "TestAssembly/MyType::_value",
            IsStatic = false,
            IsThreadStatic = false,
            IsPreserved = false,
            MetadataToken = 0x04000001,
        };
        var world = CreateLinkedWorld(fields: new[] { field });

        var result = new MetadataWriterStage().Write(world);

        var entry = Assert.Single(result.Value.AotManifest.Entries);
        Assert.Equal("field", entry.SubjectKind);
        Assert.Equal(field.SubjectId, entry.SubjectId);
        Assert.Equal("instance-field-state", entry.Reason);
    }

    [Fact]
    public void Write_Dependencies_AppearInAotManifest()
    {
        var dep = new LinkedDependencyModel
        {
            AssemblyName = "ExternalLib",
            SubjectKind = "type",
            SubjectId = "ExternalLib/ExternalClass",
            Reason = "referenced-by-metadata",
        };
        var world = CreateLinkedWorld(dependencies: new[] { dep });

        var result = new MetadataWriterStage().Write(world);

        var entry = Assert.Single(result.Value.AotManifest.Entries);
        Assert.Equal("ExternalLib", entry.AssemblyName);
        Assert.Equal("type", entry.SubjectKind);
        Assert.Equal(dep.SubjectId, entry.SubjectId);
        Assert.Equal("referenced-by-metadata", entry.Reason);
    }

    // ───────────────────────────────
    // MetadataRegistration artifact tests
    // ───────────────────────────────

    [Fact]
    public void Write_Assembly_RegistrationHasSlotZero()
    {
        var assembly = new AssemblyIdentityModel
        {
            Name = "MyAssembly",
            ModuleVersionId = Guid.NewGuid(),
        };
        var world = CreateLinkedWorld(
            assembly: assembly,
            assemblies: new[] { assembly });

        var result = new MetadataWriterStage().Write(world);

        var regs = result.Value.MetadataRegistration.Registrations;
        var assemblyReg = regs[0];
        Assert.Equal("assembly", assemblyReg.RegistrationKind);
        Assert.Equal(0, assemblyReg.Slot);
        Assert.Equal("MyAssembly", assemblyReg.SubjectId);
    }

    [Fact]
    public void Write_Type_FullRegistrationFields()
    {
        var type = new ManagedTypeModel
        {
            AssemblyName = "TestAssembly",
            NamespaceName = "MyApp",
            Name = "Widget",
            SubjectId = "TestAssembly/MyApp.Widget",
            DefinitionSubjectId = "TestAssembly/MyApp.Widget",
            DisplayName = "MyApp.Widget",
            IsInterface = false,
            IsValueType = false,
            IsSealed = false,
            IsPreserved = false,
            MetadataToken = 0x02000001,
        };
        var world = CreateLinkedWorld(
            assemblies: new[] { new AssemblyIdentityModel { Name = "TestAssembly", ModuleVersionId = Guid.NewGuid() } },
            types: new[] { type });

        var result = new MetadataWriterStage().Write(world);

        // Skip the assembly registration (slot 0), type should be at slot 1
        var typeReg = result.Value.MetadataRegistration.Registrations[1];
        Assert.Equal("type", typeReg.RegistrationKind);
        Assert.Equal(1, typeReg.Slot);
        Assert.Equal(type.SubjectId, typeReg.SubjectId);
        Assert.Equal("Widget", typeReg.Name);
        Assert.Equal("MyApp", typeReg.NamespaceName);
        Assert.Equal("MyApp.Widget", typeReg.DisplayName);
        Assert.Equal(type.DefinitionSubjectId, typeReg.DefinitionSubjectId);
        Assert.Equal(0x02000001, typeReg.MetadataToken);
    }

    [Fact]
    public void Write_Property_CreatesRegistration()
    {
        var prop = new ManagedPropertyModel
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/MyType",
            Name = "Value",
            PropertyType = "System.Int32",
            SubjectId = "TestAssembly/MyType::property:Value",
            DefinitionSubjectId = "TestAssembly/MyType::property:Value",
            IsPreserved = false,
            MetadataToken = 0x17000001,
        };
        var world = CreateLinkedWorld(properties: new[] { prop });

        var result = new MetadataWriterStage().Write(world);

        var propReg = Assert.Single(result.Value.MetadataRegistration.Registrations);
        Assert.Equal("property", propReg.RegistrationKind);
        Assert.Equal(0, propReg.Slot);
        Assert.Equal(prop.SubjectId, propReg.SubjectId);
        Assert.Equal("Value", propReg.Name);
        Assert.Equal(prop.DefinitionSubjectId, propReg.DefinitionSubjectId);
        Assert.Equal(prop.DeclaringTypeSubjectId, propReg.DeclaringTypeSubjectId);
        Assert.Equal("System.Int32", propReg.MemberType);
    }

    [Fact]
    public void Write_Method_FullRegistrationFields()
    {
        var method = new ManagedMethodModel
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/Calculator",
            DeclaringTypeDisplayName = "Calculator",
            Name = "Add",
            ReturnType = "System.Int32",
            SubjectId = "TestAssembly/Calculator::Add:System.Int32(System.Int32,System.Int32)",
            DefinitionSubjectId = "TestAssembly/Calculator::Add:System.Int32(System.Int32,System.Int32)",
            Signature = "System.Int32 Calculator::Add(System.Int32, System.Int32)",
            IsStatic = true,
            IsVirtual = false,
            IsPreserved = false,
            IsUnmanagedCallersOnly = false,
            MetadataToken = 0x06000002,
            Parameters =
            [
                new ManagedParameterModel { Name = "a", Type = "System.Int32" },
                new ManagedParameterModel { Name = "b", Type = "System.Int32" },
            ],
            Body = new ManagedMethodBodyModel { Blocks = [], ExceptionRegions = [] },
        };
        var world = CreateLinkedWorld(methods: new[] { method });

        var result = new MetadataWriterStage().Write(world);

        // First registration: method. Then 2 parameter registrations.
        var registrations = result.Value.MetadataRegistration.Registrations;
        var methodReg = registrations[0];
        Assert.Equal("method", methodReg.RegistrationKind);
        Assert.Equal(0, methodReg.Slot);
        Assert.Equal(method.SubjectId, methodReg.SubjectId);
        Assert.Equal("Add", methodReg.Name);
        Assert.Equal(method.DefinitionSubjectId, methodReg.DefinitionSubjectId);
        Assert.Equal(method.DeclaringTypeSubjectId, methodReg.DeclaringTypeSubjectId);
        Assert.Equal("System.Int32", methodReg.MemberType);
        Assert.Equal(2, methodReg.ParameterCount);
        Assert.Equal(0x06000002, methodReg.MetadataToken);
        Assert.False(methodReg.IsImported);
    }

    [Fact]
    public void Write_MethodWithParameters_CreatesParameterRegistrations()
    {
        var method = new ManagedMethodModel
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/MyType",
            DeclaringTypeDisplayName = "MyType",
            Name = "Concat",
            ReturnType = "System.String",
            SubjectId = "TestAssembly/MyType::Concat:System.String(System.String,System.String)",
            DefinitionSubjectId = "TestAssembly/MyType::Concat:System.String(System.String,System.String)",
            Signature = "System.String MyType::Concat(System.String, System.String)",
            IsStatic = true,
            IsVirtual = false,
            IsPreserved = false,
            IsUnmanagedCallersOnly = false,
            MetadataToken = 0x06000003,
            Parameters =
            [
                new ManagedParameterModel { Name = "left", Type = "System.String" },
                new ManagedParameterModel { Name = "right", Type = "System.String" },
            ],
            Body = new ManagedMethodBodyModel { Blocks = [], ExceptionRegions = [] },
        };
        var world = CreateLinkedWorld(methods: new[] { method });

        var result = new MetadataWriterStage().Write(world);

        var registrations = result.Value.MetadataRegistration.Registrations;
        // Method at index 0, parameters at 1, 2
        Assert.Equal(3, registrations.Count);

        var param0 = registrations[1];
        Assert.Equal("parameter", param0.RegistrationKind);
        Assert.Equal(1, param0.Slot);
        Assert.Equal("left", param0.Name);
        Assert.Equal(method.SubjectId, param0.DeclaringMethodSubjectId);
        Assert.Equal("System.String", param0.MemberType);
        Assert.Equal(0, param0.ParameterIndex);

        var param1 = registrations[2];
        Assert.Equal("parameter", param1.RegistrationKind);
        Assert.Equal(2, param1.Slot);
        Assert.Equal("right", param1.Name);
        Assert.Equal(method.SubjectId, param1.DeclaringMethodSubjectId);
        Assert.Equal("System.String", param1.MemberType);
        Assert.Equal(1, param1.ParameterIndex);
    }

    [Fact]
    public void Write_ImportedMethod_SetsIsImported()
    {
        var method = new ManagedMethodModel
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/NativeMethods",
            DeclaringTypeDisplayName = "NativeMethods",
            Name = "MessageBox",
            ReturnType = "System.Int32",
            SubjectId = "TestAssembly/NativeMethods::MessageBox:System.Int32(System.IntPtr,System.String,System.String,System.Int32)",
            DefinitionSubjectId = "TestAssembly/NativeMethods::MessageBox:System.Int32(System.IntPtr,System.String,System.String,System.Int32)",
            Signature = "System.Int32 NativeMethods::MessageBox(System.IntPtr, System.String, System.String, System.Int32)",
            IsStatic = true,
            IsVirtual = false,
            IsPreserved = false,
            IsUnmanagedCallersOnly = false,
            MetadataToken = 0x06000004,
            Parameters =
            [
                new ManagedParameterModel { Name = "hWnd", Type = "System.IntPtr" },
                new ManagedParameterModel { Name = "text", Type = "System.String" },
                new ManagedParameterModel { Name = "caption", Type = "System.String" },
                new ManagedParameterModel { Name = "options", Type = "System.Int32" },
            ],
            Body = new ManagedMethodBodyModel { Blocks = [], ExceptionRegions = [] },
            Import = new ManagedImportModel
            {
                ModuleName = "user32.dll",
                EntryPointName = "MessageBoxW",
            },
        };
        var world = CreateLinkedWorld(methods: new[] { method });

        var result = new MetadataWriterStage().Write(world);

        var methodReg = result.Value.MetadataRegistration.Registrations[0];
        Assert.True(methodReg.IsImported);
        Assert.Equal("user32.dll", methodReg.ImportModuleName);
        Assert.Equal("MessageBoxW", methodReg.ImportEntryPointName);
    }

    // ───────────────────────────────
    // SupplementalMetadataTemplate artifact tests
    // ───────────────────────────────

    [Fact]
    public void Write_SupplementalTemplate_HasReservedSlots()
    {
        var world = CreateLinkedWorld();

        var result = new MetadataWriterStage().Write(world);

        var template = result.Value.SupplementalMetadataTemplate;
        Assert.Equal(256, template.ReservedSlots.TypeCount);
        Assert.Equal(1024, template.ReservedSlots.MethodCount);
        Assert.Equal(256, template.ReservedSlots.GenericInstantiationCount);
    }

    [Fact]
    public void Write_SupplementalTemplate_HasRegisteredTypeEntries()
    {
        var type = new ManagedTypeModel
        {
            AssemblyName = "TestAssembly",
            NamespaceName = "App",
            Name = "MyClass",
            SubjectId = "TestAssembly/App.MyClass",
            DefinitionSubjectId = "TestAssembly/App.MyClass",
            DisplayName = "App.MyClass",
            IsInterface = false,
            IsValueType = false,
            IsSealed = false,
            IsPreserved = false,
            MetadataToken = 0x02000001,
        };
        var world = CreateLinkedWorld(types: new[] { type });

        var result = new MetadataWriterStage().Write(world);

        var typeEntry = Assert.Single(result.Value.SupplementalMetadataTemplate.RegisteredTypes);
        Assert.Equal("TestAssembly", typeEntry.AssemblyName);
        Assert.Equal(type.SubjectId, typeEntry.SubjectId);
        Assert.Equal(type.DefinitionSubjectId, typeEntry.DefinitionSubjectId);
        Assert.Equal(type.MetadataToken, typeEntry.MetadataToken);
        Assert.Null(typeEntry.RuntimeGenericContext);
        Assert.Null(typeEntry.GenericDiagnostic);
    }

    [Fact]
    public void Write_SupplementalTemplate_HasRegisteredMethodEntries()
    {
        var method = new ManagedMethodModel
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/Util",
            DeclaringTypeDisplayName = "Util",
            Name = "Compute",
            ReturnType = "System.Double",
            SubjectId = "TestAssembly/Util::Compute:System.Double(System.Double)",
            DefinitionSubjectId = "TestAssembly/Util::Compute:System.Double(System.Double)",
            Signature = "System.Double Util::Compute(System.Double)",
            IsStatic = true,
            IsVirtual = false,
            IsPreserved = false,
            IsUnmanagedCallersOnly = false,
            MetadataToken = 0x06000005,
            Parameters =
            [
                new ManagedParameterModel { Name = "x", Type = "System.Double" },
            ],
            Body = new ManagedMethodBodyModel { Blocks = [], ExceptionRegions = [] },
        };
        var world = CreateLinkedWorld(methods: new[] { method });

        var result = new MetadataWriterStage().Write(world);

        var methodEntry = Assert.Single(result.Value.SupplementalMetadataTemplate.RegisteredMethods);
        Assert.Equal("TestAssembly", methodEntry.AssemblyName);
        Assert.Equal(method.SubjectId, methodEntry.SubjectId);
        Assert.Equal(method.DefinitionSubjectId, methodEntry.DefinitionSubjectId);
        Assert.Equal(method.DeclaringTypeSubjectId, methodEntry.DeclaringTypeSubjectId);
        Assert.Equal(method.MetadataToken, methodEntry.MetadataToken);
        Assert.Equal(method.Parameters.Count, methodEntry.ParameterCount);
        Assert.Null(methodEntry.RuntimeGenericContext);
        Assert.Null(methodEntry.GenericDiagnostic);
    }

    // ───────────────────────────────
    // Generic instantiation tests
    // ───────────────────────────────

    [Fact]
    public void Write_GenericType_SupplementalTemplateHasGenericContext()
    {
        var type = new ManagedTypeModel
        {
            AssemblyName = "TestAssembly",
            NamespaceName = "Collections",
            Name = "List`1",
            SubjectId = "TestAssembly/Collections.List<System.Int32>",
            DefinitionSubjectId = "TestAssembly/Collections.List`1",
            DisplayName = "Collections.List<System.Int32>",
            IsInterface = false,
            IsValueType = false,
            IsSealed = false,
            IsPreserved = false,
            MetadataToken = 0x02000002,
        };
        var demandGraph = new GenericInstantiationDemandGraphModel
        {
            Demands =
            [
                new GenericInstantiationDemandModel
                {
                    RequestingAssemblyName = "TestAssembly",
                    OwningAssemblyName = "TestAssembly",
                    SubjectKind = "type",
                    SubjectId = "TestAssembly/Collections.List<System.Int32>",
                    DefinitionSubjectId = "TestAssembly/Collections.List`1",
                    DemandSourceKind = "direct-call",
                    InstantiationKey = new GenericInstantiationKey
                    {
                        ContextKind = GenericContextKind.TypeInstantiation,
                        DefinitionSubjectId = "TestAssembly/Collections.List`1",
                        TypeArguments = ["System.Int32"],
                        MethodArguments = [],
                    },
                    SupportKindCode = GenericSupportKind.Specialized,
                    SpecializationKindCode = GenericSpecializationKind.SpecializedBody,
                    FamilyKindCode = GenericDemandFamilyKind.ClosedGenericType,
                    IsCrossAssembly = false,
                },
            ],
        };
        var world = CreateLinkedWorld(
            types: new[] { type },
            genericInstantiationDemandGraph: demandGraph);

        var result = new MetadataWriterStage().Write(world);

        var typeEntry = Assert.Single(result.Value.SupplementalMetadataTemplate.RegisteredTypes);
        Assert.NotNull(typeEntry.RuntimeGenericContext);
        Assert.Equal(
            GenericContextKind.TypeInstantiation,
            typeEntry.RuntimeGenericContext.InstantiationKey.ContextKind);
        Assert.Equal(
            "TestAssembly/Collections.List`1",
            typeEntry.RuntimeGenericContext.InstantiationKey.DefinitionSubjectId);
        Assert.NotNull(typeEntry.RuntimeGenericContext.SharedGenericBodyId);
        Assert.NotNull(typeEntry.RuntimeGenericContext.InstantiationStubId);
        Assert.Equal(GenericSupportKind.Specialized, typeEntry.RuntimeGenericContext.SupportKindCode);
        Assert.Equal(
            GenericSpecializationKind.SpecializedBody,
            typeEntry.RuntimeGenericContext.SpecializationKindCode);
        Assert.NotNull(typeEntry.GenericDiagnostic);
        Assert.Equal(
            type.SubjectId,
            typeEntry.GenericDiagnostic.SubjectId);
    }

    // ───────────────────────────────
    // Error handling tests
    // ───────────────────────────────

    /// <summary>
    /// Current behavior: when two demands share the same SubjectId, the first
    /// one wins and the second is silently skipped. <see cref="MetadataWriterStage.EnsureEquivalentDemand"/>
    /// is defined but currently NOT called from <c>BuildGenericDemandLookup</c> — this
    /// test documents the current first-wins semantics.
    /// </summary>
    [Fact]
    public void Write_DuplicateGenericDemands_FirstWins()
    {
        var demandGraph = new GenericInstantiationDemandGraphModel
        {
            Demands =
            [
                new GenericInstantiationDemandModel
                {
                    RequestingAssemblyName = "TestAssembly",
                    OwningAssemblyName = "TestAssembly",
                    SubjectKind = "type",
                    SubjectId = "TestAssembly/MyGeneric<System.Int32>",
                    DefinitionSubjectId = "TestAssembly/MyGeneric`1",
                    DemandSourceKind = "direct-call",
                    InstantiationKey = new GenericInstantiationKey
                    {
                        ContextKind = GenericContextKind.TypeInstantiation,
                        DefinitionSubjectId = "TestAssembly/MyGeneric`1",
                        TypeArguments = ["System.Int32"],
                        MethodArguments = [],
                    },
                    SupportKindCode = GenericSupportKind.Specialized,
                    SpecializationKindCode = GenericSpecializationKind.SpecializedBody,
                    FamilyKindCode = GenericDemandFamilyKind.ClosedGenericType,
                    IsCrossAssembly = false,
                },
                new GenericInstantiationDemandModel
                {
                    RequestingAssemblyName = "TestAssembly",
                    OwningAssemblyName = "TestAssembly",
                    SubjectKind = "type",
                    SubjectId = "TestAssembly/MyGeneric<System.Int32>",
                    DefinitionSubjectId = "TestAssembly/MyGeneric`1",
                    DemandSourceKind = "reflection",
                    InstantiationKey = new GenericInstantiationKey
                    {
                        ContextKind = GenericContextKind.TypeInstantiation,
                        DefinitionSubjectId = "TestAssembly/MyGeneric`1",
                        TypeArguments = ["System.String"],
                        MethodArguments = [],
                    },
                    SupportKindCode = GenericSupportKind.Specialized,
                    SpecializationKindCode = GenericSpecializationKind.SpecializedBody,
                    FamilyKindCode = GenericDemandFamilyKind.ClosedGenericType,
                    IsCrossAssembly = false,
                },
            ],
        };
        var type = new ManagedTypeModel
        {
            AssemblyName = "TestAssembly",
            NamespaceName = "Collections",
            Name = "MyGeneric",
            SubjectId = "TestAssembly/MyGeneric<System.Int32>",
            DefinitionSubjectId = "TestAssembly/MyGeneric`1",
            DisplayName = "Collections.MyGeneric<System.Int32>",
            IsInterface = false,
            IsValueType = false,
            IsSealed = false,
            IsPreserved = false,
            MetadataToken = 0x02000001,
        };
        var world = CreateLinkedWorld(
            types: new[] { type },
            genericInstantiationDemandGraph: demandGraph);

        var result = new MetadataWriterStage().Write(world);

        // First-wins: the demand resolves with the first demand's TypeArguments
        Assert.True(result.IsSuccess);
        var typeEntry = Assert.Single(result.Value.SupplementalMetadataTemplate.RegisteredTypes);
        Assert.NotNull(typeEntry.RuntimeGenericContext);
        Assert.Equal(
            ["System.Int32"],
            typeEntry.RuntimeGenericContext.InstantiationKey.TypeArguments);
    }

    // ───────────────────────────────
    // Artifact format/contract tests
    // ───────────────────────────────

    [Fact]
    public void Write_AllArtifacts_HaveExpectedFormatVersion()
    {
        var world = CreateLinkedWorld();

        var result = new MetadataWriterStage().Write(world);

        Assert.Equal("v0", result.Value.AotManifest.FormatVersion);
        Assert.Equal("v0", result.Value.MetadataRegistration.FormatVersion);
        Assert.Equal("v0", result.Value.SupplementalMetadataTemplate.FormatVersion);
    }

    [Fact]
    public void Write_AllArtifacts_HaveExpectedArtifactKind()
    {
        var world = CreateLinkedWorld();

        var result = new MetadataWriterStage().Write(world);

        Assert.Equal("aotManifest", result.Value.AotManifest.ArtifactKind);
        Assert.Equal("metadataRegistration", result.Value.MetadataRegistration.ArtifactKind);
        Assert.Equal("supplementalMetadataTemplate", result.Value.SupplementalMetadataTemplate.ArtifactKind);
    }

    // ───────────────────────────────
    // Multi-entry ordering tests
    // ───────────────────────────────

    [Fact]
    public void Write_MultipleMethods_Dependencies_Fields_AotManifestOrdering()
    {
        var method1 = new ManagedMethodModel
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/A",
            DeclaringTypeDisplayName = "A",
            Name = "Foo",
            ReturnType = "System.Void",
            SubjectId = "TestAssembly/A::Foo:System.Void()",
            DefinitionSubjectId = "TestAssembly/A::Foo:System.Void()",
            Signature = "System.Void A::Foo()",
            IsStatic = false,
            IsVirtual = false,
            IsPreserved = false,
            IsUnmanagedCallersOnly = false,
            MetadataToken = 0x06000001,
            Parameters = [],
            Body = new ManagedMethodBodyModel { Blocks = [], ExceptionRegions = [] },
        };
        var field = new ManagedFieldModel
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = "TestAssembly/A",
            Name = "_x",
            FieldType = "System.Int32",
            SubjectId = "TestAssembly/A::_x",
            DefinitionSubjectId = "TestAssembly/A::_x",
            IsStatic = false,
            IsThreadStatic = false,
            IsPreserved = false,
            MetadataToken = 0x04000001,
        };
        var dep = new LinkedDependencyModel
        {
            AssemblyName = "External",
            SubjectKind = "type",
            SubjectId = "External/Ext",
            Reason = "referenced",
        };
        var world = CreateLinkedWorld(
            methods: new[] { method1 },
            fields: new[] { field },
            dependencies: new[] { dep });

        var result = new MetadataWriterStage().Write(world);

        var entries = result.Value.AotManifest.Entries;
        Assert.Equal(3, entries.Count);
        // Order: methods first, then fields, then dependencies
        Assert.Equal(method1.SubjectId, entries[0].SubjectId);
        Assert.Equal(field.SubjectId, entries[1].SubjectId);
        Assert.Equal(dep.SubjectId, entries[2].SubjectId);
    }

    [Fact]
    public void Write_FullRegistrationSequence_CorrectSlotOrder()
    {
        var assembly = new AssemblyIdentityModel { Name = "MyAsm", ModuleVersionId = Guid.NewGuid() };
        var type = new ManagedTypeModel
        {
            AssemblyName = "MyAsm",
            NamespaceName = "App",
            Name = "MyType",
            SubjectId = "MyAsm/App.MyType",
            DefinitionSubjectId = "MyAsm/App.MyType",
            DisplayName = "App.MyType",
            IsInterface = false,
            IsValueType = false,
            IsSealed = false,
            IsPreserved = false,
            MetadataToken = 0x02000001,
        };
        var field = new ManagedFieldModel
        {
            AssemblyName = "MyAsm",
            DeclaringTypeSubjectId = "MyAsm/App.MyType",
            Name = "_val",
            FieldType = "System.Int32",
            SubjectId = "MyAsm/App.MyType::_val",
            DefinitionSubjectId = "MyAsm/App.MyType::_val",
            IsStatic = false,
            IsThreadStatic = false,
            IsPreserved = false,
            MetadataToken = 0x04000001,
        };
        var prop = new ManagedPropertyModel
        {
            AssemblyName = "MyAsm",
            DeclaringTypeSubjectId = "MyAsm/App.MyType",
            Name = "Value",
            PropertyType = "System.Int32",
            SubjectId = "MyAsm/App.MyType::property:Value",
            DefinitionSubjectId = "MyAsm/App.MyType::property:Value",
            IsPreserved = false,
            MetadataToken = 0x17000001,
        };
        var method = new ManagedMethodModel
        {
            AssemblyName = "MyAsm",
            DeclaringTypeSubjectId = "MyAsm/App.MyType",
            DeclaringTypeDisplayName = "MyType",
            Name = "GetValue",
            ReturnType = "System.Int32",
            SubjectId = "MyAsm/App.MyType::GetValue:System.Int32()",
            DefinitionSubjectId = "MyAsm/App.MyType::GetValue:System.Int32()",
            Signature = "System.Int32 MyType::GetValue()",
            IsStatic = false,
            IsVirtual = false,
            IsPreserved = false,
            IsUnmanagedCallersOnly = false,
            MetadataToken = 0x06000001,
            Parameters = [],
            Body = new ManagedMethodBodyModel { Blocks = [], ExceptionRegions = [] },
        };
        var world = CreateLinkedWorld(
            assembly: assembly,
            assemblies: new[] { assembly },
            types: new[] { type },
            fields: new[] { field },
            properties: new[] { prop },
            methods: new[] { method });

        var result = new MetadataWriterStage().Write(world);

        var regs = result.Value.MetadataRegistration.Registrations;
        // Expected: assembly(0) -> type(1) -> field(2) -> property(3) -> method(4)
        Assert.Equal(5, regs.Count);
        Assert.Equal("assembly", regs[0].RegistrationKind);
        Assert.Equal(0, regs[0].Slot);
        Assert.Equal("type", regs[1].RegistrationKind);
        Assert.Equal(1, regs[1].Slot);
        Assert.Equal("field", regs[2].RegistrationKind);
        Assert.Equal(2, regs[2].Slot);
        Assert.Equal("property", regs[3].RegistrationKind);
        Assert.Equal(3, regs[3].Slot);
        Assert.Equal("method", regs[4].RegistrationKind);
        Assert.Equal(4, regs[4].Slot);
    }

    [Fact]
    public void Write_NullGenericDemandGraph_DoesNotThrow()
    {
        var world = CreateLinkedWorld(genericInstantiationDemandGraph: null);

        // Should not throw - null demand graph is handled gracefully
        var result = new MetadataWriterStage().Write(world);

        Assert.True(result.IsSuccess);
        Assert.Empty(result.Value.SupplementalMetadataTemplate.RegisteredTypes);
        Assert.Empty(result.Value.SupplementalMetadataTemplate.RegisteredMethods);
    }

    [Fact]
    public void Write_EmptyGenericDemandGraph_DoesNotThrow()
    {
        var world = CreateLinkedWorld(
            genericInstantiationDemandGraph: new GenericInstantiationDemandGraphModel
            {
                Demands = [],
            });

        var result = new MetadataWriterStage().Write(world);

        Assert.True(result.IsSuccess);
    }

    // ───────────────────────────────
    // Contract integration tests
    // ───────────────────────────────

    /// <summary>
    /// Verifies that the MetadataWriter output can be consumed by
    /// ManagedClosureResult construction with expected field types.
    /// This is a structural contract test ensuring AotManifestArtifact,
    /// MetadataRegistrationArtifact, and SupplementalMetadataTemplateArtifact
    /// maintain their required property signatures.
    /// </summary>
    [Fact]
    public void Write_Output_IsConsumableByManagedClosureResult()
    {
        var world = CreateLinkedWorld();
        var result = new MetadataWriterStage().Write(world);

        // ManagedClosureResult requires all 3 artifact types with specific properties.
        // This test compiles and runs - if contract changes happen, this will fail.
        var closureResult = new ManagedClosureResult
        {
            OutputRootPath = "output/",
            TypedIlIr = new TypedIlIrArtifact
            {
                Methods = [],
            },
            AotCoreIr = new AotCoreIrArtifact
            {
                Methods = [],
            },
            AotManifest = result.Value.AotManifest,
            MetadataRegistration = result.Value.MetadataRegistration,
            SupplementalMetadataTemplate = result.Value.SupplementalMetadataTemplate,
            CodeRegistration = new CodeRegistrationArtifact
            {
                Modules = [],
            },
            GenericInstantiationDemandGraph = new GenericInstantiationDemandGraphModel
            {
                Demands = [],
            },
            GenericCapabilityMatrix = new GenericCapabilityMatrixArtifact
            {
                OwnerSubjectId = "test",
                EntrySubjectId = "test",
                HotUpdateModes = [],
                Gates = new GenericCapabilityMatrixGateStatus
                {
                    Status = "ok",
                    UnsupportedLeakCount = 0,
                    MissingAuthorityCount = 0,
                    NonCanonicalHotUpdateNameCount = 0,
                    Leaks = [],
                },
                FamilyBudgets = [],
                BoundaryCases = [],
                Entries = [],
            },
            OptimizationFacts = new OptimizationFactsArtifact
            {
                ClosedWorldSpecializations = [],
                DispatchFacts = [],
                LayoutFacts = [],
                ExceptionFacts = [],
            },
            PreserveDescriptor = new PreserveDescriptorArtifact
            {
                Entries = [],
            },
            NativeReferenceLoweringPlan = new NativeReferenceLoweringPlanArtifact
            {
                PlanKind = "native-reference",
                AssemblyName = "test",
                EntrySubjectId = "test",
                IncludeHeader = "test.h",
                NativeEntryFunctionName = "test_entry",
                EntrySymbol = "test_entry",
                ReferenceTypeToken = "0x02000001",
                CapturedFieldToken = "0x04000001",
                EntryMethodToken = "0x06000001",
                ConsoleWriteLineStringIcall = "System.Console.WriteLine",
            },
            NativeAotLoweringPlan = new NativeAotLoweringPlanArtifact
            {
                PlanKind = "native-aot",
                AssemblyName = "test",
                EntrySubjectId = "test",
                NativeEntryFunctionName = "test_entry",
                EntrySymbol = "test_entry",
                EntryMethodToken = "0x06000001",
                WorkloadAbi = "default",
            },
            ClosureManifest = new ManagedClosureManifestArtifact
            {
                AssemblyName = "test",
                EntrySubjectId = "test",
                InputAssemblyPath = "test.dll",
                InputModuleVersionId = Guid.NewGuid().ToString(),
                Artifacts = [],
            },
        };

        Assert.NotNull(closureResult);
        Assert.Same(result.Value.AotManifest, closureResult.AotManifest);
        Assert.Same(result.Value.MetadataRegistration, closureResult.MetadataRegistration);
        Assert.Same(result.Value.SupplementalMetadataTemplate, closureResult.SupplementalMetadataTemplate);
    }
}
