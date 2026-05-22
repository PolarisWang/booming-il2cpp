using Chaos.IL2CPP.Pipeline;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class PipelineIntegrationTests
{
    private static string StubAssemblyPath { get; } = LocateStubAssemblyDll();

    private static string LocateStubAssemblyDll()
    {
        var dir = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory);
        while (dir != null && !Directory.Exists(Path.Combine(dir.FullName, ".git")))
            dir = dir.Parent;
        var repoRoot = dir?.FullName ?? throw new DirectoryNotFoundException(
            "Could not locate repository root (.git directory).");

        return Path.GetFullPath(Path.Combine(
            repoRoot,
            "tests", "managed", "Chaos.IL2CPP.CodeGen.Tests",
            "StubAssembly", "bin", "Debug", "net8.0",
            "StubAssembly.dll"));
    }

    /// <summary>Creates a temp output root cleaned up after the test.</summary>
    private static PipelineTestContext CreateContext() => new();

    private sealed class PipelineTestContext : IDisposable
    {
        public string OutputRoot { get; } = Path.Combine(
            Path.GetTempPath(), "ChaosPipelineTest_" + Guid.NewGuid().ToString("N"));

        public void Dispose()
        {
            if (Directory.Exists(OutputRoot))
                Directory.Delete(OutputRoot, recursive: true);
        }
    }

    // ──────────────────────────────────────────────
    // Full-assembly closure (exercises most pipeline code)
    // ──────────────────────────────────────────────

    [Fact]
    public void FullAssemblyClosure_AllArtifactsArePresent()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: null,
            AdditionalAssemblyPaths: null,
            FullAssemblyClosure: true);

        var pipeline = new PipelinePlan();
        var result = pipeline.Execute(request);

        Assert.NotNull(result.TypedIlIr);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotNull(result.AotManifest);
        Assert.NotNull(result.MetadataRegistration);
        Assert.NotNull(result.SupplementalMetadataTemplate);
        Assert.NotNull(result.CodeRegistration);
        Assert.NotNull(result.GenericCapabilityMatrix);
        Assert.NotNull(result.OptimizationFacts);
        Assert.NotNull(result.PreserveDescriptor);
        Assert.NotNull(result.NativeReferenceLoweringPlan);
        Assert.NotNull(result.NativeAotLoweringPlan);
        Assert.NotNull(result.ClosureManifest);
    }

    [Fact]
    public void FullAssemblyClosure_TypedIlContainsStubMethods()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            FullAssemblyClosure: true);

        var result = new PipelinePlan().Execute(request);

        var subjectIds = result.TypedIlIr.Methods
            .Select(m => m.SubjectId)
            .ToList();

        Assert.Contains(subjectIds, id => id.Contains("StubMethods::ReturnZero"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::Add"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::NoOp"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::GetHello"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::BoxInt32"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::UnboxToInt32"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::Concat"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::Divide"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::IsGreaterThan"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::IsLessThan"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::MakeArray"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::ArrayLength"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::FirstElement"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::Sum"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::Max"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::SafeDivide"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::DoubleAdd"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::IsNull"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::DayName"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::IsWeekend"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::UseCalculator"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::TryCatchDivide"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::Identity"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::Coalesce"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::DayToString"));
        Assert.Contains(subjectIds, id => id.Contains("StubMethods::CountTo"));
    }

    [Fact]
    public void FullAssemblyClosure_AotCoreIrHasMethodWithInstructions()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            FullAssemblyClosure: true);

        var result = new PipelinePlan().Execute(request);

        var returnZero = result.AotCoreIr.Methods
            .FirstOrDefault(m => m.SubjectId.Contains("ReturnZero"));
        Assert.NotNull(returnZero);
        Assert.NotEmpty(returnZero.Instructions);
    }

    [Fact]
    public void FullAssemblyClosure_NativeAotPlanIsFullAssemblyEntry()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            FullAssemblyClosure: true);

        var result = new PipelinePlan().Execute(request);
        var plan = result.NativeAotLoweringPlan;

        Assert.Equal("full-assembly-entry", plan.PlanKind);
        Assert.Equal("StubAssembly", plan.AssemblyName);
        Assert.Equal("full-assembly", plan.WorkloadAbi);
    }

    [Fact]
    public void FullAssemblyClosure_CodeRegistrationHasModules()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            FullAssemblyClosure: true);

        var result = new PipelinePlan().Execute(request);

        Assert.NotEmpty(result.CodeRegistration.Modules);
        Assert.Contains(result.CodeRegistration.Modules, m =>
            m.Registrations.Any(r => r.SubjectId.Contains("StubMethods")));
    }

    [Fact]
    public void FullAssemblyClosure_MetadataRegistrationHasEntries()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            FullAssemblyClosure: true);

        var result = new PipelinePlan().Execute(request);

        Assert.NotEmpty(result.MetadataRegistration.Registrations);
        Assert.Contains(result.MetadataRegistration.Registrations, r =>
            r.RegistrationKind == "assembly");
        Assert.Contains(result.MetadataRegistration.Registrations, r =>
            r.RegistrationKind == "type");
        Assert.Contains(result.MetadataRegistration.Registrations, r =>
            r.RegistrationKind == "method");
    }

    [Fact]
    public void FullAssemblyClosure_NativeReferencePlanIsRuntimeSkeleton()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            FullAssemblyClosure: true);

        var result = new PipelinePlan().Execute(request);
        var plan = result.NativeReferenceLoweringPlan;

        Assert.Equal("assembly-full-closure-runtime-skeleton", plan.PlanKind);
        Assert.NotEmpty(plan.TranslationUnitMethodSubjectIds);
    }

    [Fact]
    public void FullAssemblyClosure_GenericCapabilityMatrixIsBuilt()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            FullAssemblyClosure: true);

        var result = new PipelinePlan().Execute(request);

        Assert.NotNull(result.GenericCapabilityMatrix);
    }

    // ──────────────────────────────────────────────
    // Entry-point mode (non-full-assembly path)
    // ──────────────────────────────────────────────

    [Fact]
    public void EntryPointMode_WithReturnZeroOverride_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ReturnZero:System.Int32()",
            AdditionalAssemblyPaths: null,
            FullAssemblyClosure: false);

        var pipeline = new PipelinePlan();
        var result = pipeline.Execute(request);

        Assert.NotNull(result);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPointMode_PlanIsGenericManagedEntry()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ReturnZero:System.Int32()",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        var plan = result.NativeAotLoweringPlan;

        Assert.Equal("generic-managed-entry", plan.PlanKind);
    }

    [Fact]
    public void EntryPointMode_NativeReferencePlanIsGenericAnalysisOnly()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ReturnZero:System.Int32()",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        var plan = result.NativeReferenceLoweringPlan;

        Assert.Equal("generic-analysis-only", plan.PlanKind);
        Assert.Equal("RunNativeReference", plan.NativeEntryFunctionName);
    }

    // ──────────────────────────────────────────────
    // Multi-assembly mode (ExecuteMulti)
    // ──────────────────────────────────────────────

    [Fact]
    public void ExecuteMulti_ReturnsPerAssemblyResults()
    {
        using var ctx = CreateContext();
        var request = new MultiAssemblyClosureRequest(
            InputAssemblyPaths: new[] { StubAssemblyPath },
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: null,
            AdditionalAssemblyPaths: null);

        var pipeline = new PipelinePlan();
        var results = pipeline.ExecuteMulti(request);

        Assert.NotEmpty(results);
        Assert.Single(results);

        var first = results[0];
        Assert.NotNull(first.AotCoreIr);
        Assert.NotEmpty(first.AotCoreIr.Methods);
    }

    [Fact]
    public void ExecuteMulti_WithEntryOverride_ReturnsFilteredResults()
    {
        using var ctx = CreateContext();
        var request = new MultiAssemblyClosureRequest(
            InputAssemblyPaths: new[] { StubAssemblyPath },
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ReturnZero:System.Int32()",
            AdditionalAssemblyPaths: null);

        var pipeline = new PipelinePlan();
        var results = pipeline.ExecuteMulti(request);

        Assert.NotEmpty(results);
        Assert.NotNull(results[0].AotCoreIr);
    }

    // ──────────────────────────────────────────────
    // Loader assembly resolution
    // ──────────────────────────────────────────────

    [Fact]
    public void Pipeline_OutputRootPathIsSet()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            FullAssemblyClosure: true);

        var result = new PipelinePlan().Execute(request);

        Assert.Equal(ctx.OutputRoot, result.OutputRootPath);
    }

    [Fact]
    public void Pipeline_ClosureManifestHasAssemblyName()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            FullAssemblyClosure: true);

        var result = new PipelinePlan().Execute(request);

        Assert.Equal("StubAssembly", result.ClosureManifest.AssemblyName);
        Assert.True(result.ClosureManifest.FullAssemblyClosure);
    }

    [Fact]
    public void Pipeline_ClosureManifestHasResolvedAssemblies()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            FullAssemblyClosure: true);

        var result = new PipelinePlan().Execute(request);

        Assert.NotEmpty(result.ClosureManifest.ResolvedAssemblies);
        Assert.Contains(result.ClosureManifest.ResolvedAssemblies, r =>
            r.AssemblyName == "StubAssembly");
    }

    [Fact]
    public void Pipeline_OptimizationFactsAndPreserveDescriptorArePresent()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            FullAssemblyClosure: true);

        var result = new PipelinePlan().Execute(request);

        Assert.NotNull(result.OptimizationFacts);
        Assert.NotNull(result.PreserveDescriptor);
    }

    // ──────────────────────────────────────────────
    // Complex entry-point methods — exercise specific codegen paths
    // ──────────────────────────────────────────────

    [Fact]
    public void EntryPoint_InterfaceDispatch_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseCalculator:System.Int32(ICalculator,System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_TryCatchDivide_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::TryCatchDivide:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_SwitchStatement_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::DayName:System.String(System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_GenericMethod_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::Identity`1:!!0(!!0)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ArraySum_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::Sum:System.Int32(System.Int32[])",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_DivideWithThrow_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::SafeDivide:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    // ── Additional StubAssembly methods for broader coverage ──────────

    [Fact]
    public void EntryPoint_NullCoalescing_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::Coalesce:System.String(System.String,System.String)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_IntToString_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::IntToString:System.String(System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_StringIsNullOrEmpty_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::IsNullOrEmpty:System.Boolean(System.String)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ArrayEmpty_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseArrayEmpty:System.Boolean()",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ArrayLiteral_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::GetFiveElements:System.Int32[]()",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_MathOps_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseMathMax:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_UseList_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseList:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_EnumToString_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::DayToString:System.String(System.DayOfWeek)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_StringConcat_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::Concat:System.String(System.String,System.String)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_UseDelegate_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseDelegate:System.Int32(System.Func<System.Int32,System.Int32>,System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_BoxUnbox_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::BoxInt32:System.Object(System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ForeachEnumerable_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::SumEnumerable:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_Nullable_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UnwrapNullable:System.Int32(System.Nullable<System.Int32>)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_BitwiseOps_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::BitwiseAndOr:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_UnsignedArith_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::DivideUnsigned:System.UInt32(System.UInt32,System.UInt32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_TypeConversions_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::IntToDouble:System.Double(System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_StaticFieldRef_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::IncrementCounter:System.Int32()",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_TryParseOutParam_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::TryParseInt:System.Boolean(System.String,System.Int32&)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_WhileLoop_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::CountTo:System.Int32(System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_CheckedArith_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::OverflowAdd:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_FloatArith_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseDouble:System.Double(System.Double,System.Double)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_TypeCastCheck_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::CastAndCheck:System.String(System.Object)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    // ── Expanded coverage: shapes, strings, generics ─────────────────────

    [Fact]
    public void EntryPoint_UseStringFormat_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseStringFormat:System.String(System.String,System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_NewGuidToString_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::NewGuidToString:System.String()",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_UseTimeSpan_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseTimeSpan:System.Double(System.Double)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_StringContains_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::StringContains:System.Boolean(System.String,System.String)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_BuildString_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::BuildString:System.String(System.String,System.String,System.String)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_StringToInt32_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::StringToInt32:System.Int32(System.String)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_CombinePaths_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::CombinePaths:System.String(System.String,System.String)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_StartsWithHello_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::StartsWithHello:System.Boolean(System.String)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_DateTimeToBinary_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::DateTimeToBinary:System.Int64(System.DateTime)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_EscapeDataString_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::EscapeDataString:System.String(System.String)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_TryParseEnum_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::TryParseDayName:System.Boolean(System.String)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_NullableGetValueOrDefault_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::NullableGetValueOrDefault:System.Int32(System.Nullable<System.Int32>)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_DelegateCombine_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::InvokeTwoDelegates:System.Int32(System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    // ──────────────────────────────────────────────
    // Bulk-generated EntryPoint tests for all untested StubMethods
    // ──────────────────────────────────────────────

    [Fact]
    public void EntryPoint_Add_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::Add:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_AddOne_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::AddOne:System.Void(System.Int32&)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ArrayLength_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ArrayLength:System.Int32(System.Int32[])",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_Divide_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::Divide:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_DoubleAdd_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::DoubleAdd:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_FirstElement_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::FirstElement:System.Int32(System.Int32[])",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_GetAgeInHours_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::GetAgeInHours:System.Double(System.DateTime)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_GetCurrentThreadId_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::GetCurrentThreadId:System.Int32()",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_GetHello_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::GetHello:System.String()",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_GetObjHash_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::GetObjHash:System.Int32(System.Object)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_GetStringType_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::GetStringType:System.Type()",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_HasNullableValue_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::HasNullableValue:System.Boolean(System.Nullable<System.Int32>)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_IntToByte_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::IntToByte:System.Byte(System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_IntToFloat_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::IntToFloat:System.Single(System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_IntToLong_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::IntToLong:System.Int64(System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_IntToShort_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::IntToShort:System.Int16(System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_IsGreaterThan_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::IsGreaterThan:System.Boolean(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_IsLessThan_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::IsLessThan:System.Boolean(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_IsNull_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::IsNull:System.Boolean(System.Object)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_IsWeekend_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::IsWeekend:System.Boolean(System.DayOfWeek)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_JoinStrings_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::JoinStrings:System.String(System.String[],System.String)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_LockAndIncrement_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::LockAndIncrement:System.Int32()",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_MakeArray_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::MakeArray:System.Int32[](System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_Max_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::Max:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_Modulo_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::Modulo:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ModUnsigned_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ModUnsigned:System.UInt32(System.UInt32,System.UInt32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_Multiply_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::Multiply:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_NegateValue_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::NegateValue:System.Int32(System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_NoOp_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::NoOp:System.Void()",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ObjEquals_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ObjEquals:System.Boolean(System.Object,System.Object)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ObjToString_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ObjToString:System.String(System.Object)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_OverflowSub_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::OverflowSub:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ReadCounter_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ReadCounter:System.Int32()",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ReadFileFirstLine_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ReadFileFirstLine:System.String(System.String)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ShiftBits_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ShiftBits:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ShortSleep_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ShortSleep:System.Void()",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_SplitComma_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::SplitComma:System.String[](System.String)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_Subtract_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::Subtract:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_TryCatchFinally_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::TryCatchFinally:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_UnboxToInt32_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UnboxToInt32:System.Int32(System.Object)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_UseFloat_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseFloat:System.Single(System.Single,System.Single)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_UseGcKeepAlive_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseGcKeepAlive:System.Void(System.Object)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_UseGcSuppressFinalize_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseGcSuppressFinalize:System.Void(System.Object)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_UseLong_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseLong:System.Int64(System.Int64,System.Int64)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_UseMathAbs_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseMathAbs:System.Double(System.Double)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_UseMathMin_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::UseMathMin:System.Int32(System.Int32,System.Int32)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_WriteError_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::WriteError:System.Void(System.String)",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_YieldOnce_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::YieldOnce:System.Boolean()",
            FullAssemblyClosure: false);

        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    // ──────────────────────────────────────────────
    // Parameterized test: runs pipeline for each untested Subject ID
    // from other StubAssembly classes (Adder, Animal, Cat, Dog, etc.)
    // ──────────────────────────────────────────────

    public static IEnumerable<object[]> AdditionalEntryPointSubjectIds()
    {
        yield return new object[] { "StubAssembly/Adder::.ctor:System.Void()" };
        yield return new object[] { "StubAssembly/Animal::.ctor:System.Void()" };
        yield return new object[] { "StubAssembly/Cat::.ctor:System.Void()" };
        yield return new object[] { "StubAssembly/Dog::.ctor:System.Void()" };
        yield return new object[] { "StubAssembly/Multiplier::.ctor:System.Void()" };
        yield return new object[] { "StubAssembly/StubMethods::.cctor:System.Void()" };
        yield return new object[] { "StubAssembly/DataOperations::DoubleField:System.Void(ValuePoint&)" };
        yield return new object[] { "StubAssembly/DataOperations::get_Origin:ValuePoint()" };
        yield return new object[] { "StubAssembly/DataOperations::SumStructFields:System.Int32()" };
        yield return new object[] { "StubAssembly/VirtualDispatchDemo::CallCompute:System.Int32(ICalculator,System.Int32,System.Int32)" };
        yield return new object[] { "StubAssembly/VirtualDispatchDemo::MakeSound:System.String(Animal)" };
        yield return new object[] { "StubAssembly/VirtualDispatchDemo::StructInterfaceDispatch:System.Int32(System.Int32,System.Int32)" };
        yield return new object[] { "StubAssembly/StubMethods::GetStringEmptyFieldHandle:System.RuntimeFieldHandle()" };
        yield return new object[] { "StubAssembly/StubMethods::GetInt32TypeHandle:System.RuntimeTypeHandle()" };
        yield return new object[] { "StubAssembly/StubMethods::IncrementVolatileCounter:System.Void()" };
        yield return new object[] { "StubAssembly/StubMethods::InvokeActionDelegate:System.Void()" };
        yield return new object[] { "StubAssembly/StubMethods::InvokeFuncDelegate:System.Int32()" };
        yield return new object[] { "StubAssembly/StubMethods::LockAndIncrementCounter2:System.Int32()" };
        yield return new object[] { "StubAssembly/StubMethods::DescribePerson:System.String()" };
        yield return new object[] { "StubAssembly/StubMethods::UseUsingStatement:System.Int32()" };
        yield return new object[] { "StubAssembly/StubMethods::UseDictionary:System.String(System.Int32)" };
        yield return new object[] { "StubAssembly/StubMethods::UseHashSet:System.Boolean(System.Int32)" };
        yield return new object[] { "StubAssembly/StubMethods::UseLazy:System.Int32()" };
        yield return new object[] { "StubAssembly/StubMethods::UseTuple:System.Int32(System.Int32,System.Int32)" };
        yield return new object[] { "StubAssembly/StubMethods::CompareInts:System.Int32(System.Int32,System.Int32)" };
        yield return new object[] { "StubAssembly/StubMethods::IntsEqual:System.Boolean(System.Int32,System.Int32)" };
        yield return new object[] { "StubAssembly/StubMethods::UseMemoryStream:System.Byte[](System.Byte[])" };
        yield return new object[] { "StubAssembly/StubMethods::UseRegex:System.Boolean(System.String)" };
        yield return new object[] { "StubAssembly/StubMethods::HasReadFlag:System.Boolean(System.IO.FileAccess)" };
        yield return new object[] { "StubAssembly/StubMethods::UseUri:System.Boolean(System.String)" };
        yield return new object[] { "StubAssembly/StubMethods::UseDateTimeOffset:System.DateTimeOffset(System.DateTime)" };
        yield return new object[] { "StubAssembly/StubMethods::UseStopwatch:System.Int64()" };
        yield return new object[] { "StubAssembly/StubMethods::UseWeakReference:System.Int32()" };
    }

    [Theory]
    [MemberData(nameof(AdditionalEntryPointSubjectIds))]
    public void EntryPoint_AdditionalSubject_CompletesSuccessfully(string subjectId)
    {
        var tempDir = Path.Combine(Path.GetTempPath(), "ChaosPipelineTest_" + Guid.NewGuid().ToString("N"));
        try
        {
            var request = new ManagedClosureRequest(
                InputAssemblyPath: StubAssemblyPath,
                OutputRootPath: tempDir,
                EntryPointSubjectIdOverride: subjectId,
                FullAssemblyClosure: false);

            var result = new PipelinePlan().Execute(request);
            Assert.NotNull(result.AotCoreIr);
            Assert.NotEmpty(result.AotCoreIr.Methods);
        }
        finally
        {
            if (Directory.Exists(tempDir))
                Directory.Delete(tempDir, recursive: true);
        }
    }

    // ──────────────────────────────────────────────
    // EntryPoint tests for newly added StubMethods patterns
    // ──────────────────────────────────────────────

    [Fact]
    public void EntryPoint_CallToStringOnInt32_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::CallToStringOnInt32:System.String(System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_GetDefaultDateTime_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::GetDefaultDateTime:System.DateTime()",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_SizeOfInt32_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::SizeOfInt32:System.Int32()",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ReadVolatileCounter_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ReadVolatileCounter:System.Int32()",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_GenericToStringInt32_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::GenericToString`1:System.String(!!0)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    [Fact]
    public void EntryPoint_ComplexProcessingInt32_CompletesSuccessfully()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: "StubAssembly/StubMethods::ComplexProcessing`1:System.String(System.Nullable<!!0>,System.Int32)",
            FullAssemblyClosure: false);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.AotCoreIr);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }

    // ──────────────────────────────────────────────
    // EntryPoint tests for BCL shapes via StubAssembly methods
    // These trigger RuntimeHelperShapeRegistry.BuildDefault() entries
    // ──────────────────────────────────────────────

    [Fact]
    public void FullAssemblyClosure_WithAdditionalAssemblies_Completes()
    {
        using var ctx = CreateContext();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: StubAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: null,
            FullAssemblyClosure: true);
        var result = new PipelinePlan().Execute(request);
        Assert.NotNull(result.NativeAotLoweringPlan);
        Assert.NotNull(result.CodeRegistration);
        Assert.NotEmpty(result.AotCoreIr.Methods);
    }
}
