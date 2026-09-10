using System.Reflection;
using Chaos.IL2CPP.Generator;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class NativeAotEmitterTests
{
    private static readonly Type s_t = typeof(NativeAotEmitter);
    private static readonly BindingFlags s_flags = BindingFlags.NonPublic | BindingFlags.Static;

    // ── ToCppStringLiteral ──────────────────────────────────────────────

    [Theory]
    [InlineData("hello", "\"hello\"")]
    [InlineData("", "\"\"")]
    [InlineData("with\"quote", "\"with\\\"quote\"")]
    [InlineData("with\\backslash", "\"with\\\\backslash\"")]
    [InlineData("with\nnewline", "\"with\\nnewline\"")]
    [InlineData("with\ttab", "\"with\\ttab\"")]
    [InlineData("with\rreturn", "\"with\\rreturn\"")]
    public void ToCppStringLiteral_ReturnsExpected(string input, string expected)
    {
        var method = s_t.GetMethod("ToCppStringLiteral", s_flags, new[] { typeof(string) })!;
        var result = (string)method.Invoke(null, new object[] { input })!;
        Assert.Equal(expected, result);
    }

    // ── RequireStringField ──────────────────────────────────────────────

    [Fact]
    public void RequireStringField_NonNullNonEmpty_DoesNotThrow()
    {
        var method = s_t.GetMethod("RequireStringField", s_flags, new[] { typeof(string), typeof(string) })!;
        var ex = Record.Exception(() =>
            method.Invoke(null, new object[] { "valid", "fieldName" }));
        Assert.Null(ex);
    }

    [Theory]
    [InlineData(null)]
    [InlineData("")]
    [InlineData("   ")]
    public void RequireStringField_NullOrEmpty_Throws(string? value)
    {
        var method = s_t.GetMethod("RequireStringField", s_flags, new[] { typeof(string), typeof(string) })!;
        var ex = Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { value!, "fieldName" }));
        Assert.Contains("requires non-empty field", ex.InnerException!.Message);
    }

    // ── ValidateLoweringPlan ───────────────────────────────────────────

    private static NativeAotLoweringPlanArtifact MakePlan(
        string planKind = "generic-managed-entry",
        string assemblyName = "Test",
        string entrySubjectId = "Test/Prog::Main:System.Int32()",
        string entrySymbol = "Test_Prog_Main",
        string entryMethodToken = "0u",
        string workloadAbi = "int()",
        string nativeEntryFunctionName = "RunNativeAot") =>
        new()
        {
            PlanKind = planKind,
            AssemblyName = assemblyName,
            EntrySubjectId = entrySubjectId,
            EntrySymbol = entrySymbol,
            EntryMethodToken = entryMethodToken,
            WorkloadAbi = workloadAbi,
            NativeEntryFunctionName = nativeEntryFunctionName,
        };

    private static ManagedClosureManifestArtifact MakeManifest(string entrySubjectId = "Test/Prog::Main:System.Int32()") =>
        new()
        {
            AssemblyName = "Test",
            EntrySubjectId = entrySubjectId,
            InputAssemblyPath = "",
            InputModuleVersionId = "",
            Artifacts = [],
        };

    [Fact]
    public void ValidateLoweringPlan_FullAssemblyEntry_DoesNotThrow()
    {
        var plan = MakePlan(planKind: "full-assembly-entry", workloadAbi: "full-assembly");
        var method = s_t.GetMethod("ValidateLoweringPlan", s_flags)!;
        var ex = Record.Exception(() =>
            method.Invoke(null, new object[] { plan, MakeManifest() }));
        Assert.Null(ex);
    }

    [Fact]
    public void ValidateLoweringPlan_ValidManagedEntry_DoesNotThrow()
    {
        var plan = MakePlan();
        var method = s_t.GetMethod("ValidateLoweringPlan", s_flags)!;
        var ex = Record.Exception(() =>
            method.Invoke(null, new object[] { plan, MakeManifest() }));
        Assert.Null(ex);
    }

    [Fact]
    public void ValidateLoweringPlan_UnsupportedPlanKind_Throws()
    {
        var plan = MakePlan(planKind: "unknown-plan");
        var method = s_t.GetMethod("ValidateLoweringPlan", s_flags)!;
        var ex = Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { plan, MakeManifest() }));
        Assert.Contains("unsupported native-aot lowering plan kind", ex.InnerException!.Message);
    }

    [Fact]
    public void ValidateLoweringPlan_UnsupportedWorkloadAbi_Throws()
    {
        var plan = MakePlan(workloadAbi: "float(float)");
        var method = s_t.GetMethod("ValidateLoweringPlan", s_flags)!;
        var ex = Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { plan, MakeManifest() }));
        Assert.Contains("unsupported native-aot workload ABI", ex.InnerException!.Message);
    }

    [Fact]
    public void ValidateLoweringPlan_EntryMismatch_Throws()
    {
        var plan = MakePlan(entrySubjectId: "Asm/Foo::Bar()");
        var method = s_t.GetMethod("ValidateLoweringPlan", s_flags)!;
        var ex = Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { plan, MakeManifest("Asm/Other::M()") }));
        Assert.Contains("does not match closure entry", ex.InnerException!.Message);
    }

    [Fact]
    public void ValidateLoweringPlan_MissingPlanKind_Throws()
    {
        var plan = MakePlan(planKind: "");
        var method = s_t.GetMethod("ValidateLoweringPlan", s_flags)!;
        var ex = Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { plan, MakeManifest() }));
        Assert.Contains("requires non-empty field", ex.InnerException!.Message);
    }

    // ── LoadEntryMethod ────────────────────────────────────────────────

    [Fact]
    public void LoadEntryMethod_Found_ReturnsMethod()
    {
        var entryMethod = new AotCoreIrMethodArtifact
        {
            MethodId = "m1",
            SubjectId = "Test/Prog::Main:System.Int32()",
            Signature = "int()",
            Identity = new ManagedMethodIdentityArtifact
            {
                AssemblyName = "Test", DeclaringTypeSubjectId = "Test.Prog",
                DefinitionSubjectId = "0x01", SubjectId = "Test/Prog::Main:System.Int32()",
                MethodId = "m1", Signature = "int()",
            },
            NativeSymbol = "", IsStatic = false, ReturnType = "System.Int32",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = (AotCoreIrAbiCarrierKind)0 },
            ParameterCount = 0, ParameterAbis = [], LocalCount = 0,
            ExceptionRegionCount = 0, ExceptionRegions = [], Instructions = [],
        };
        var aotCoreIr = new AotCoreIrArtifact { Methods = [entryMethod] };
        var method = s_t.GetMethod("LoadEntryMethod", s_flags)!;
        var result = method.Invoke(null, new object[] { aotCoreIr, "Test/Prog::Main:System.Int32()" });
        Assert.NotNull(result);
    }

    [Fact]
    public void LoadEntryMethod_NotFound_Throws()
    {
        var aotCoreIr = new AotCoreIrArtifact { Methods = [] };
        var method = s_t.GetMethod("LoadEntryMethod", s_flags)!;
        var ex = Assert.Throws<TargetInvocationException>(() =>
            method.Invoke(null, new object[] { aotCoreIr, "missing" }));
        Assert.Contains("is missing from generated closure artifacts", ex.InnerException!.Message);
    }

    // ── BuildMethodSection ──────────────────────────────────────────────

    [Fact]
    public void BuildMethodSection_ReturnsRenderedTemplate()
    {
        var method = s_t.GetMethod("BuildMethodSection", s_flags, new[] { typeof(NativeAotMethodTemplateModel) })!;
        var model = new NativeAotMethodTemplateModel
        {
            SubjectId = "TestMethod",
            MethodSource = "// test body",
        };
        var result = (string)method.Invoke(null, new object[] { model })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    // ── LoadRequiredJson ─────────────────────────────────────────────────
    // LoadRequiredJson<T> is a generic method. We use MakeGenericMethod
    // to bind T to NativeAotLoweringPlanArtifact for the test.

    [Fact]
    public void LoadRequiredJson_ValidFile_ReturnsArtifact()
    {
        var tempFile = Path.GetTempFileName();
        try
        {
            File.WriteAllText(tempFile, """{"PlanKind":"test","AssemblyName":"Test","EntrySubjectId":"Test/Prog::Main()","NativeEntryFunctionName":"Run","EntrySymbol":"Test_Prog_Main","EntryMethodToken":"0x06000001","WorkloadAbi":"int()"}""");
            var openMethod = s_t.GetMethod("LoadRequiredJson", s_flags)!;
            var closedMethod = openMethod.MakeGenericMethod(typeof(NativeAotLoweringPlanArtifact));
            var result = closedMethod.Invoke(null, new object[] { tempFile })!;
            Assert.NotNull(result);
        }
        finally
        {
            File.Delete(tempFile);
        }
    }

    // ── BuildObjectModelSection ─────────────────────────────────────────

    [Fact]
    public void BuildObjectModelSection_ReturnsRenderedTemplate()
    {
        var method = s_t.GetMethod("BuildObjectModelSection", s_flags, new[] { typeof(NativeAotTemplateModel) })!;
        var model = MakeMinimalTemplateModel(objectModelCode: "int dummy = 0;");
        var result = (string)method.Invoke(null, new object[] { model })!;
        Assert.Contains("dummy", result);
    }

    // ── BuildSharedHeader ───────────────────────────────────────────────

    [Fact]
    public void BuildSharedHeader_ReturnsTypeDeclarationsCode()
    {
        var method = s_t.GetMethod("BuildSharedHeader", s_flags, new[] { typeof(NativeAotTemplateModel) })!;
        var model = MakeMinimalTemplateModel(typeDeclarationsCode: "extern int g_value;");
        var result = (string)method.Invoke(null, new object[] { model })!;
        Assert.Equal("extern int g_value;", result);
    }

    // ── BuildGeneratedPage ──────────────────────────────────────────────

    [Fact]
    public void BuildGeneratedPage_WithRegistrationAndObjectModel_ReturnsRendered()
    {
        var method = s_t.GetMethod("BuildGeneratedPage", s_flags)!;
        var model = MakeMinimalTemplateModel(
            genericRegistrationCode: "// GENERIC",
            moduleRegistrationCode: "// MODULE",
            objectModelCode: "// OBJECT_MODEL",
            methodDeclarations: new[] { "void Foo();" },
            methods: new[] { new NativeAotMethodTemplateModel { SubjectId = "Test::Foo()", MethodSource = "// foo body" } },
            entrySubjectId: "Test::Foo()",
            entrySymbol: "Test_Foo",
            entryNativeSymbol: "Chaos_Test_Foo",
            nativeEntryFunctionName: "Run",
            entryBridgeArguments: "",
            workloadAbi: "void()",
            includes: new[] { "<cstdint>" },
            globalDeclarations: "",
            shapeDispatchHeaderContent: "// header",
            codegenNamespace: "chaos::il2cpp::codegen::test",
            generatedModuleHeaderContent: "",
            generatedModuleSourceContent: "",
            manifestJson: "",
            typeDeclarationsCode: "",
            enumMetadataHeaderContent: "");
        var pageMethods = new[]
        {
            new NativeAotMethodTemplateModel { SubjectId = "Test::Foo()", MethodSource = "// foo body" },
        };
        var result = (string)method.Invoke(null, new object[] { model, pageMethods, true, true })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    [Fact]
    public void BuildGeneratedPage_WithoutObjectModel_AddsHeaderInclude()
    {
        var method = s_t.GetMethod("BuildGeneratedPage", s_flags)!;
        var model = MakeMinimalTemplateModel(
            typeDeclarationsCode: "extern int g_val;",
            objectModelCode: "// OBJECT_MODEL",
            entrySubjectId: "Test::Foo()",
            entrySymbol: "Test_Foo",
            entryNativeSymbol: "Chaos_Test_Foo",
            nativeEntryFunctionName: "",
            entryBridgeArguments: "",
            workloadAbi: "void()",
            includes: new[] { "<cstdint>" },
            globalDeclarations: "",
            shapeDispatchHeaderContent: "// header",
            codegenNamespace: "",
            genericRegistrationCode: "",
            moduleRegistrationCode: "",
            generatedModuleHeaderContent: "",
            generatedModuleSourceContent: "",
            manifestJson: "",
            methodDeclarations: Array.Empty<string>(),
            methods: Array.Empty<NativeAotMethodTemplateModel>(),
            enumMetadataHeaderContent: "");
        var result = (string)method.Invoke(null, new object[] { model, Array.Empty<NativeAotMethodTemplateModel>(), false, false })!;
        Assert.False(string.IsNullOrEmpty(result));
    }

    // ── BuildGeneratedSources (single TU, no pages) ─────────────────────

    [Fact]
    public void BuildGeneratedSources_SingleTranslationUnit_ReturnsExpectedSources()
    {
        var method = s_t.GetMethod("BuildGeneratedSources", s_flags)!;
        var model = MakeMinimalTemplateModel(
            shapeDispatchHeaderContent: "// dispatch",
            objectModelCode: "// OBJECT_MODEL",
            entrySubjectId: "Test::Foo()",
            entrySymbol: "Test_Foo",
            entryNativeSymbol: "Chaos_Test_Foo",
            nativeEntryFunctionName: "",
            entryBridgeArguments: "",
            workloadAbi: "void()",
            includes: new[] { "<cstdint>" },
            globalDeclarations: "",
            codegenNamespace: "",
            genericRegistrationCode: "",
            moduleRegistrationCode: "",
            generatedModuleHeaderContent: "",
            generatedModuleSourceContent: "",
            manifestJson: "",
            methodDeclarations: Array.Empty<string>(),
            methods: Array.Empty<NativeAotMethodTemplateModel>(),
            typeDeclarationsCode: "",
            enumMetadataHeaderContent: "#define ENUM_HEADER");
        var plan = new NativeAotLoweringPlanArtifact
        {
            PlanKind = "generic-managed-entry", AssemblyName = "Test",
            EntrySubjectId = "Test::Foo()", EntrySymbol = "Test_Foo",
            EntryMethodToken = "0x01", WorkloadAbi = "void()",
            NativeEntryFunctionName = "",
        };
        var raw = method.Invoke(null, new object[] { model, plan })!;
        var sources = (IReadOnlyList<NativeAotGeneratedSource>)raw.GetType().GetField("Item1")!.GetValue(raw)!;
        var artifacts = (IReadOnlyList<NativeAotGeneratedArtifactRef>)raw.GetType().GetField("Item2")!.GetValue(raw)!;
        Assert.NotNull(sources);
        Assert.NotNull(artifacts);
        Assert.Contains(sources, s => s.RelativePath.EndsWith("runtime_helper_shapes.h"));
        Assert.Contains(sources, s => s.RelativePath.Contains("enum_metadata"));
    }

    [Fact]
    public void BuildGeneratedSources_PagedOutput_ReturnsMultiplePages()
    {
        var method = s_t.GetMethod("BuildGeneratedSources", s_flags)!;
        var model = MakeMinimalTemplateModel(
            shapeDispatchHeaderContent: "// dispatch",
            objectModelCode: "// OBJECT_MODEL",
            entrySubjectId: "Test::Foo()",
            entrySymbol: "Test_Foo",
            entryNativeSymbol: "Chaos_Test_Foo",
            nativeEntryFunctionName: "",
            entryBridgeArguments: "",
            workloadAbi: "void()",
            includes: new[] { "<cstdint>" },
            globalDeclarations: "",
            codegenNamespace: "",
            genericRegistrationCode: "",
            moduleRegistrationCode: "",
            generatedModuleHeaderContent: "",
            generatedModuleSourceContent: "",
            manifestJson: "",
            methodDeclarations: new[] { "void Foo();", "void Bar();" },
            methods: new[]
            {
                new NativeAotMethodTemplateModel { SubjectId = "Test::Foo()", MethodSource = "// foo" },
                new NativeAotMethodTemplateModel { SubjectId = "Test::Bar()", MethodSource = "// bar" },
            },
            typeDeclarationsCode: "extern int g_val;",
            enumMetadataHeaderContent: "");
        var plan = new NativeAotLoweringPlanArtifact
        {
            PlanKind = "generic-managed-entry", AssemblyName = "Test",
            EntrySubjectId = "Test::Foo()", EntrySymbol = "Test_Foo",
            EntryMethodToken = "0x01", WorkloadAbi = "void()",
            NativeEntryFunctionName = "",
            TranslationUnitPageSize = 1,
            TranslationUnitPages = new List<AuditTranslationUnitPageArtifact>
            {
                new() { Path = "page-0001.cpp", PageNumber = 1, MethodCount = 1 },
                new() { Path = "page-0002.cpp", PageNumber = 2, MethodCount = 1 },
            },
        };
        var raw = method.Invoke(null, new object[] { model, plan })!;
        var sources = (IReadOnlyList<NativeAotGeneratedSource>)raw.GetType().GetField("Item1")!.GetValue(raw)!;
        // Should have: shape_dispatch.h + enum_metadata header + generated header + 2 pages
        Assert.True(sources.Count >= 4);
    }

    [Fact]
    public void BuildGeneratedSources_WithModuleContent_AddsModuleSources()
    {
        var method = s_t.GetMethod("BuildGeneratedSources", s_flags)!;
        var model = MakeMinimalTemplateModel(
            shapeDispatchHeaderContent: "// dispatch",
            generatedModuleHeaderContent: "// module header",
            generatedModuleSourceContent: "// module source",
            objectModelCode: "// OBJECT_MODEL",
            entrySubjectId: "Test::Foo()",
            entrySymbol: "Test_Foo",
            entryNativeSymbol: "Chaos_Test_Foo",
            nativeEntryFunctionName: "",
            entryBridgeArguments: "",
            workloadAbi: "void()",
            includes: new[] { "<cstdint>" },
            globalDeclarations: "",
            codegenNamespace: "",
            genericRegistrationCode: "",
            moduleRegistrationCode: "",
            manifestJson: "",
            methodDeclarations: Array.Empty<string>(),
            methods: Array.Empty<NativeAotMethodTemplateModel>(),
            typeDeclarationsCode: "",
            enumMetadataHeaderContent: "");
        var plan = new NativeAotLoweringPlanArtifact
        {
            PlanKind = "generic-managed-entry", AssemblyName = "Test",
            EntrySubjectId = "Test::Foo()", EntrySymbol = "Test_Foo",
            EntryMethodToken = "0x01", WorkloadAbi = "void()",
            NativeEntryFunctionName = "",
        };
        var raw = method.Invoke(null, new object[] { model, plan })!;
        var sources = (IReadOnlyList<NativeAotGeneratedSource>)raw.GetType().GetField("Item1")!.GetValue(raw)!;
        Assert.Contains(sources, s => s.RelativePath.Contains("chaos_generated_module"));
    }

    [Fact]
    public void BuildGeneratedSources_WithManifestJson_AddsManifest()
    {
        var method = s_t.GetMethod("BuildGeneratedSources", s_flags)!;
        var model = MakeMinimalTemplateModel(
            manifestJson: """{"version":1}""",
            shapeDispatchHeaderContent: "// dispatch",
            objectModelCode: "// OBJ",
            entrySubjectId: "Test::Foo()",
            entrySymbol: "Test_Foo",
            entryNativeSymbol: "Chaos_Test_Foo",
            nativeEntryFunctionName: "",
            entryBridgeArguments: "",
            workloadAbi: "void()",
            includes: new[] { "<cstdint>" },
            globalDeclarations: "",
            codegenNamespace: "",
            genericRegistrationCode: "",
            moduleRegistrationCode: "",
            generatedModuleHeaderContent: "",
            generatedModuleSourceContent: "",
            methodDeclarations: Array.Empty<string>(),
            methods: Array.Empty<NativeAotMethodTemplateModel>(),
            typeDeclarationsCode: "",
            enumMetadataHeaderContent: "");
        var plan = new NativeAotLoweringPlanArtifact
        {
            PlanKind = "generic-managed-entry", AssemblyName = "Test",
            EntrySubjectId = "Test::Foo()", EntrySymbol = "Test_Foo",
            EntryMethodToken = "0x01", WorkloadAbi = "void()",
            NativeEntryFunctionName = "",
        };
        var raw = method.Invoke(null, new object[] { model, plan })!;
        var sources = (IReadOnlyList<NativeAotGeneratedSource>)raw.GetType().GetField("Item1")!.GetValue(raw)!;
        Assert.Contains(sources, s => s.RelativePath.Contains("native-aot.methods.json"));
    }

    // ── Helpers ─────────────────────────────────────────────────────────

    private static NativeAotTemplateModel MakeMinimalTemplateModel(
        string objectModelCode = "",
        string typeDeclarationsCode = "",
        string genericRegistrationCode = "",
        string moduleRegistrationCode = "",
        string shapeDispatchHeaderContent = "",
        string entrySubjectId = "",
        string entrySymbol = "",
        string entryNativeSymbol = "",
        string nativeEntryFunctionName = "",
        string entryBridgeArguments = "",
        string workloadAbi = "",
        string codegenNamespace = "",
        string globalDeclarations = "",
        string generatedModuleHeaderContent = "",
        string generatedModuleSourceContent = "",
        string manifestJson = "",
        string enumMetadataHeaderContent = "",
        IReadOnlyList<string>? includes = null,
        IReadOnlyList<string>? methodDeclarations = null,
        IReadOnlyList<NativeAotMethodTemplateModel>? methods = null) =>
        new()
        {
            Includes = includes ?? Array.Empty<string>(),
            ObjectModelCode = objectModelCode,
            MethodDeclarations = methodDeclarations ?? Array.Empty<string>(),
            Methods = methods ?? Array.Empty<NativeAotMethodTemplateModel>(),
            EntrySubjectId = entrySubjectId,
            EntrySymbol = entrySymbol,
            EntryNativeSymbol = entryNativeSymbol,
            NativeEntryFunctionName = nativeEntryFunctionName,
            EntryBridgeArguments = entryBridgeArguments,
            ShapeDispatchHeaderContent = shapeDispatchHeaderContent,
            EnumMetadataHeaderContent = enumMetadataHeaderContent,
            TypeDeclarationsCode = typeDeclarationsCode,
            WorkloadAbi = workloadAbi,
            GenericRegistrationCode = genericRegistrationCode,
            ModuleRegistrationCode = moduleRegistrationCode,
            GlobalDeclarations = globalDeclarations,
            ManifestJson = manifestJson,
            CodegenNamespace = codegenNamespace,
            GeneratedModuleHeaderContent = generatedModuleHeaderContent,
            GeneratedModuleSourceContent = generatedModuleSourceContent,
        };
}
