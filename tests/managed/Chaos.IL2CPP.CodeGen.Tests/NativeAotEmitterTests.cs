using System.Reflection;
using Chaos.IL2CPP.CodeGen;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests;

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
}
