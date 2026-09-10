using Chaos.IL2CPP.Contracts;
using System.Reflection;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class CodeGenStageTests
{
    private static readonly Type s_stageType = typeof(CodeGenStage);
    private static readonly BindingFlags s_flags = BindingFlags.Static | BindingFlags.NonPublic | BindingFlags.Public;

    private static T? InvokeStatic<T>(string methodName, params object[] args)
    {
        var method = s_stageType.GetMethod(methodName, s_flags)
            ?? throw new InvalidOperationException($"Method '{methodName}' not found on CodeGenStage");
        return (T?)method.Invoke(null, args);
    }

    // ──────────────────────────────────────────────
    // DeriveWorkloadAbi
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("Test/MyType::Main:System.Void()", "void()")]
    [InlineData("Test/MyType::Main:System.Int32()", "int()")]
    [InlineData("Test/MyType::Run:System.Int32(System.Int32)", "int(int32)")]
    [InlineData("NoColonOrParen", "int(int32)")]
    [InlineData("Test/Foo:Bar()", "int()")]
    [InlineData("A/B::Run:System.Void(System.Int32)", "void(int32)")]
    public void DeriveWorkloadAbi_VariousInputs(string input, string expected)
    {
        Assert.Equal(expected, InvokeStatic<string>("DeriveWorkloadAbi", input));
    }

    // ──────────────────────────────────────────────
    // GetAuditPageCount
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData(0, 0)]
    [InlineData(-5, 0)]
    [InlineData(1, 1)]
    [InlineData(1024, 1)]
    [InlineData(1025, 2)]
    [InlineData(2048, 2)]
    public void GetAuditPageCount_VariousInputs(int input, int expected)
    {
        Assert.Equal(expected, InvokeStatic<int>("GetAuditPageCount", input));
    }

    // ──────────────────────────────────────────────
    // BuildAuditTranslationUnitPages
    // ──────────────────────────────────────────────

    [Fact]
    public void BuildAuditTranslationUnitPages_SinglePage()
    {
        var result = InvokeStatic<IReadOnlyList<AuditTranslationUnitPageArtifact>>(
            "BuildAuditTranslationUnitPages", new List<string> { "A/F::B()", "A/B::Q()" }, "gen/test", ".cpp");
        Assert.Single(result!);
        Assert.Equal(2, result![0].MethodCount);
        Assert.Contains("page-0001.cpp", result[0].Path);
    }

    [Fact]
    public void BuildAuditTranslationUnitPages_MultiPage()
    {
        var ids = Enumerable.Range(0, 1050).Select(i => $"A/M{i}::M()").ToList();
        var result = InvokeStatic<IReadOnlyList<AuditTranslationUnitPageArtifact>>(
            "BuildAuditTranslationUnitPages", ids, "gen/test", ".cpp");
        Assert.Equal(2, result!.Count);
        Assert.Equal(1024, result[0].MethodCount);
        Assert.Equal(26, result[1].MethodCount);
    }

    [Fact]
    public void BuildAuditTranslationUnitPages_Empty()
    {
        Assert.Empty(InvokeStatic<IReadOnlyList<AuditTranslationUnitPageArtifact>>(
            "BuildAuditTranslationUnitPages", new List<string>(), "gen/test", ".cpp")!);
    }

    // ──────────────────────────────────────────────
    // ResolveOwnerSubjectId
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData(null, "F", "F")]
    [InlineData("", "F", "F")]
    [InlineData(@"C:\work\subjects\MySubject\test.dll", "F", "MySubject")]
    [InlineData(@"C:\work\bin\test.dll", "F", "F")]
    public void ResolveOwnerSubjectId_VariousPaths(string? path, string fallback, string expected)
    {
        Assert.Equal(expected, InvokeStatic<string>("ResolveOwnerSubjectId", path!, fallback));
    }

    // ──────────────────────────────────────────────
    // BuildCanonicalSubjectIdLookup + ResolveCanonicalSubjectId
    // ──────────────────────────────────────────────

    [Fact]
    public void BuildCanonicalSubjectIdLookup_Empty()
    {
        IReadOnlyList<CanonicalSubjectModel> empty = Array.Empty<CanonicalSubjectModel>();
        Assert.Empty(InvokeStatic<IReadOnlyDictionary<string, string>>(
            "BuildCanonicalSubjectIdLookup", empty)!);
    }

    [Fact]
    public void BuildCanonicalSubjectIdLookup_Duplicates_UsesLast()
    {
        var subjects = new List<CanonicalSubjectModel>
        {
            new() { SubjectId = "id1", CanonicalSubjectId = "first", SubjectKind = "method" },
            new() { SubjectId = "id1", CanonicalSubjectId = "second", SubjectKind = "method" },
        };
        Assert.Equal("second", InvokeStatic<IReadOnlyDictionary<string, string>>(
            "BuildCanonicalSubjectIdLookup", subjects)!["id1"]);
    }

    [Fact]
    public void ResolveCanonicalSubjectId_Found()
    {
        Assert.Equal("canonical", InvokeStatic<string>("ResolveCanonicalSubjectId",
            new Dictionary<string, string> { ["orig"] = "canonical" }, "orig"));
    }

    // ──────────────────────────────────────────────
    // FormatCppTokenLiteral
    // ──────────────────────────────────────────────

    [Fact]
    public void FormatCppTokenLiteral_ReturnsZeroU()
    {
        var reg = new MetadataRegistrationArtifact { Registrations = [] };
        Assert.Equal("0u", InvokeStatic<string>("FormatCppTokenLiteral", reg, "Any/Sub::Id()"));
    }

    // ──────────────────────────────────────────────
    // Name property
    // ──────────────────────────────────────────────

    [Fact]
    public void Name_ReturnsCodeGen()
    {
        Assert.Equal("CodeGen", new CodeGenStage().Name);
    }

    // ──────────────────────────────────────────────
    // TryAddAssemblyNameFromSubjectId
    // ──────────────────────────────────────────────

    [Theory]
    [InlineData("Test.Asm/MyType::Method()", "Test.Asm")]
    [InlineData("Asm/Type::Method()", "Asm")]
    [InlineData("NoSlash", null)]
    [InlineData("/LeadingSlash", null)]
    [InlineData("", null)]
    [InlineData(null, null)]
    public void TryAddAssemblyNameFromSubjectId_VariousInputs(string? subjectId, string? expected)
    {
        var names = new HashSet<string>();
        InvokeStatic<object>("TryAddAssemblyNameFromSubjectId", names, subjectId);
        if (expected is null)
        {
            Assert.Empty(names);
        }
        else
        {
            Assert.Contains(expected, names);
        }
    }

    // ──────────────────────────────────────────────
    // AddResolvedAssemblyPath
    // ──────────────────────────────────────────────

    [Fact]
    public void AddResolvedAssemblyPath_AddsPath()
    {
        var dict = new Dictionary<string, string>();
        InvokeStatic<object>("AddResolvedAssemblyPath", dict, "MyAsm", @"C:\work\MyAsm.dll");
        Assert.True(dict.ContainsKey("MyAsm"));
    }

    [Fact]
    public void AddResolvedAssemblyPath_EmptyName_DoesNothing()
    {
        var dict = new Dictionary<string, string>();
        InvokeStatic<object>("AddResolvedAssemblyPath", dict, "", @"C:\work\test.dll");
        Assert.Empty(dict);
    }

    [Fact]
    public void AddResolvedAssemblyPath_NullPath_DoesNothing()
    {
        var dict = new Dictionary<string, string>();
        InvokeStatic<object>("AddResolvedAssemblyPath", dict, "Asm", null!);
        Assert.Empty(dict);
    }

    // ──────────────────────────────────────────────
    // GetRequiredMethodShape
    // ──────────────────────────────────────────────

    [Fact]
    public void GetRequiredMethodShape_Found_ReturnsShape()
    {
        var shape = new MethodShapeModel
        {
            SubjectId = "m1",
            MethodRole = "user",
            BodyAvailability = "full",
            BodyAvailabilityCode = 0,
        };
        var shapes = new Dictionary<string, MethodShapeModel> { ["m1"] = shape };
        var result = InvokeStatic<MethodShapeModel>("GetRequiredMethodShape", shapes, "m1");
        Assert.Equal("user", result!.MethodRole);
    }

    [Fact]
    public void GetRequiredMethodShape_NotFound_Throws()
    {
        var ex = Assert.Throws<TargetInvocationException>(() =>
            InvokeStatic<MethodShapeModel>("GetRequiredMethodShape",
                new Dictionary<string, MethodShapeModel>(), "missing"));
        Assert.Contains("missing semantic method shape", ex.InnerException!.Message);
    }

    // ──────────────────────────────────────────────
    // GetRequiredCapabilities
    // ──────────────────────────────────────────────

    [Fact]
    public void GetRequiredCapabilities_Found_ReturnsCapabilities()
    {
        var caps = new Dictionary<string, IReadOnlyList<string>>
        {
            ["m1"] = new List<string> { "cap1", "cap2" },
        };
        var result = InvokeStatic<IReadOnlyList<string>>("GetRequiredCapabilities", caps, "m1");
        Assert.Equal(2, result!.Count);
    }

    [Fact]
    public void GetRequiredCapabilities_NotFound_Throws()
    {
        var ex = Assert.Throws<TargetInvocationException>(() =>
            InvokeStatic<IReadOnlyList<string>>("GetRequiredCapabilities",
                new Dictionary<string, IReadOnlyList<string>>(), "missing"));
        Assert.Contains("missing method capability bundle", ex.InnerException!.Message);
    }

    // ──────────────────────────────────────────────
    // BuildCodeRegistrationTypeCapabilities
    // ──────────────────────────────────────────────

    private static MetadataRegistrationEntry T(string subj, string display, string def) =>
        new() { RegistrationKind = "type", SubjectId = subj, DisplayName = display, DefinitionSubjectId = def, Slot = 0 };

    [Fact]
    public void BuildCodeRegistrationTypeCapabilities_AllPrimitiveTypes()
    {
        var reg = new MetadataRegistrationArtifact
        {
            Registrations = [
                T("t:Byte", "System.Byte", "0x01"), T("t:SByte", "System.SByte", "0x02"),
                T("t:I16", "System.Int16", "0x03"), T("t:U16", "System.UInt16", "0x04"),
                T("t:I32", "System.Int32", "0x05"), T("t:U32", "System.UInt32", "0x06"),
                T("t:I64", "System.Int64", "0x07"), T("t:U64", "System.UInt64", "0x08"),
                T("t:IPtr", "System.IntPtr", "0x09"), T("t:UPtr", "System.UIntPtr", "0x0A"),
                T("t:Sgl", "System.Single", "0x0B"), T("t:Dbl", "System.Double", "0x0C"),
            ],
        };
        var result = InvokeStatic<IReadOnlyList<CodeRegistrationTypeCapabilityEntry>>(
            "BuildCodeRegistrationTypeCapabilities", reg);
        Assert.Equal(12, result!.Count);
        Assert.Equal(1u, result.First(e => e.SubjectId == "t:Byte").ValueSizeBytes);
    }

    [Fact]
    public void BuildCodeRegistrationTypeCapabilities_NonType_Skipped()
    {
        var reg = new MetadataRegistrationArtifact { Registrations = [new() { RegistrationKind = "method", SubjectId = "m:M", DisplayName = "M", DefinitionSubjectId = "0x01", Slot = 0 }] };
        Assert.Empty(InvokeStatic<IReadOnlyList<CodeRegistrationTypeCapabilityEntry>>("BuildCodeRegistrationTypeCapabilities", reg)!);
    }

    [Fact]
    public void BuildCodeRegistrationTypeCapabilities_UnknownType_Skipped()
    {
        Assert.Empty(InvokeStatic<IReadOnlyList<CodeRegistrationTypeCapabilityEntry>>(
            "BuildCodeRegistrationTypeCapabilities", new MetadataRegistrationArtifact { Registrations = [T("t:C", "Custom.Struct", "0x0D")] })!);
    }

    [Fact]
    public void BuildCodeRegistrationTypeCapabilities_NonHexToken_Skipped()
    {
        Assert.Empty(InvokeStatic<IReadOnlyList<CodeRegistrationTypeCapabilityEntry>>(
            "BuildCodeRegistrationTypeCapabilities", new MetadataRegistrationArtifact { Registrations = [T("t:I32", "System.Int32", "not_hex")] })!);
    }

    [Fact]
    public void BuildCodeRegistrationTypeCapabilities_Duplicates_Deduplicated()
    {
        Assert.Single(InvokeStatic<IReadOnlyList<CodeRegistrationTypeCapabilityEntry>>(
            "BuildCodeRegistrationTypeCapabilities", new MetadataRegistrationArtifact { Registrations = [T("t:I32", "System.Int32", "0x01"), T("t:I32", "System.Int32", "0x01")] })!);
    }

    // ──────────────────────────────────────────────
    // FilterResultPerAssembly (public method, uses AotCoreIrArtifact not ManagedMethodModel)
    // ──────────────────────────────────────────────

    private static AotCoreIrMethodArtifact M(string subjectId) =>
        new()
        {
            MethodId = subjectId,
            SubjectId = subjectId,
            Signature = "void()",
            Identity = new ManagedMethodIdentityArtifact
            {
                AssemblyName = "T",
                DeclaringTypeSubjectId = subjectId.Split("::")[0],
                DefinitionSubjectId = "0x01",
                SubjectId = subjectId,
                MethodId = subjectId,
                Signature = "void()",
            },
            NativeSymbol = "",
            IsStatic = false,
            ReturnType = "System.Void",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Void },
            ParameterCount = 0,
            ParameterAbis = [],
            LocalCount = 0,
            ExceptionRegionCount = 0,
            ExceptionRegions = [],
            Instructions = [],
        };

    private static ManagedClosureResult MakeResult(params AotCoreIrMethodArtifact[] methods) =>
        new()
        {
            OutputRootPath = System.IO.Path.GetTempPath(),
            AotCoreIr = new AotCoreIrArtifact { Methods = [.. methods] },
            TypedIlIr = new TypedIlIrArtifact { Methods = [] },
            AotManifest = new AotManifestArtifact { Entries = [] },
            SupplementalMetadataTemplate = new SupplementalMetadataTemplateArtifact { RegisteredMethods = [], RegisteredTypes = [], ReservedSlots = new SupplementalMetadataReservedSlots() },
            MetadataRegistration = new MetadataRegistrationArtifact { Registrations = [] },
            CodeRegistration = new CodeRegistrationArtifact { Modules = [] },
            GenericInstantiationDemandGraph = new GenericInstantiationDemandGraphModel { Demands = [] },
            GenericCapabilityMatrix = new GenericCapabilityMatrixArtifact
            {
                OwnerSubjectId = "",
                EntrySubjectId = "",
                HotUpdateModes = [],
                Gates = new GenericCapabilityMatrixGateStatus
                {
                    Status = "closed",
                    UnsupportedLeakCount = 0,
                    MissingAuthorityCount = 0,
                    NonCanonicalHotUpdateNameCount = 0,
                    Leaks = [],
                },
                FamilyBudgets = [],
                BoundaryCases = [],
                Entries = [],
            },
            OptimizationFacts = new OptimizationFactsArtifact { ClosedWorldSpecializations = [], DispatchFacts = [], LayoutFacts = [], ExceptionFacts = [] },
            PreserveDescriptor = new PreserveDescriptorArtifact { Entries = [] },
            NativeReferenceLoweringPlan = new NativeReferenceLoweringPlanArtifact
            {
                PlanKind = "t", AssemblyName = "T", EntrySubjectId = "", IncludeHeader = "",
                NativeEntryFunctionName = "", EntrySymbol = "",
                ReferenceTypeToken = "0u", CapturedFieldToken = "0u",
                EntryMethodToken = "0u", ConsoleWriteLineStringIcall = "",
            },
            NativeAotLoweringPlan = new NativeAotLoweringPlanArtifact
            {
                PlanKind = "t", AssemblyName = "T", EntrySubjectId = "",
                NativeEntryFunctionName = "", EntrySymbol = "",
                EntryMethodToken = "0u", WorkloadAbi = "int()",
            },
            ClosureManifest = new ManagedClosureManifestArtifact
            {
                AssemblyName = "T", EntrySubjectId = "", InputAssemblyPath = "",
                InputModuleVersionId = "", Artifacts = [],
            },
        };

    [Fact]
    public void FilterResultPerAssembly_FiltersByAssemblyName()
    {
        var result = MakeResult(M("Asm1/T::M()"), M("Asm2/T::M()"));
        var filtered = new CodeGenStage().FilterResultPerAssembly(result, ["Asm1.dll", "Asm2.dll"]);
        Assert.Equal(2, filtered.Count);
        Assert.Equal("Asm1/T::M()", filtered[0].AotCoreIr.Methods[0].SubjectId);
        Assert.Equal("Asm2/T::M()", filtered[1].AotCoreIr.Methods[0].SubjectId);
    }

    [Fact]
    public void FilterResultPerAssembly_NoMatch_ReturnsEmpty()
    {
        var result = MakeResult(M("Other/T::M()"));
        var filtered = new CodeGenStage().FilterResultPerAssembly(result, ["Target.dll"]);
        Assert.Empty(filtered[0].AotCoreIr.Methods);
    }
}
