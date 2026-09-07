using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Pipeline;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

/// <summary>
/// Full-pipeline extraction tests for a REAL async subject assembly
/// (AsyncTestAssembly.dll). Running PipelinePlan.Execute() over it drives
/// Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGenStage and
/// yields the authentic AOT core IR, including the compiler-generated
/// &lt;GetOne&gt;d__0::MoveNext state machine. This is the substrate for the
/// Phase 2 translator (ASYNC-P2-1): a real async method's MoveNext IL must
/// surface as an AotCoreIr method whose emission the translator rewrites.
/// </summary>
public sealed class AsyncPipelineTests
{
    private static readonly string s_asyncAssemblyPath = LocateAsyncAssemblyDll();

    public static string AsyncAssemblyPath => s_asyncAssemblyPath;

    private static string LocateAsyncAssemblyDll()
    {
        var dir = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory);
        while (dir != null && !Directory.Exists(Path.Combine(dir.FullName, ".git")))
            dir = dir.Parent;
        var repoRoot = dir?.FullName ?? throw new DirectoryNotFoundException(
            "Could not locate repository root (.git directory).");

        // Detect build configuration (Debug/Release) and TFM from the test runner's
        // output directory. BaseDirectory is typically:
        //   <repo>/tests/unit/managed/codegen/bin/<Configuration>/<TFM>/
        string config = "Debug";
        string tfm = "net8.0";
        var baseDir = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory);
        if (baseDir?.Parent?.Parent is { } configDir &&
            configDir.Parent?.Name == "bin")
        {
            tfm = baseDir.Name;
            string configName = configDir.Name;
            if (configName is "Debug" or "Release" or "RelWithDebInfo")
                config = configName;
        }

        return Path.Combine(
            repoRoot,
            "tests", "unit", "managed", "codegen",
            "AsyncTestAssembly", "bin", config, tfm,
            "AsyncTestAssembly.dll");
    }

    private sealed class TempCtx : IDisposable
    {
        public string OutputRoot { get; } = Path.Combine(
            Path.GetTempPath(), "ChaosAsyncPipeline_" + Guid.NewGuid().ToString("N"));
        public void Dispose()
        {
            try
            {
                if (Directory.Exists(OutputRoot))
                    Directory.Delete(OutputRoot, recursive: true);
            }
            catch
            {
                // Cleanup failures should not fail the test.
            }
        }
    }

    [Fact]
    public void FullAssemblyClosure_SurfacesAsyncStateMachineMoveNext()
    {
        if (!File.Exists(s_asyncAssemblyPath))
        {
            // The assembly is a build dependency; if the test project wasn't
            // restored/built it won't be on disk. Provide a clear signal.
            Assert.Fail($"AsyncTestAssembly.dll not built at {s_asyncAssemblyPath}");
        }

        using var ctx = new TempCtx();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: s_asyncAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: null,
            AdditionalAssemblyPaths: null,
            FullAssemblyClosure: true);

        var pipeline = new PipelinePlan();
        var exec = pipeline.Execute(request);
        if (exec.IsFailure)
        {
            Assert.Fail($"Pipeline failed: {exec.Error?.Code}: {exec.Error?.Message}");
        }
        var result = exec.Value!;

        Assert.NotNull(result.AotCoreIr);

        var subjectIds = result.AotCoreIr.Methods
            .Select(m => m.SubjectId)
            .ToList();

        // The real async method surfaces.
        Assert.Contains(subjectIds, id => id.Contains("AsyncMethods::GetOne"));

        // The compiler-generated state machine MoveNext surfaces as a subject.
        var movenext = result.AotCoreIr.Methods
            .FirstOrDefault(m => m.SubjectId.Contains(">d__") && m.SubjectId.Contains("::MoveNext"));
        Assert.NotNull(movenext);

        // Its body carries the builder/awaiter calls the translator maps.
        var callees = movenext!.Instructions
            .Select(i => i.Callee ?? i.TargetReference?.SubjectId ?? string.Empty)
            .Where(c => c.Length > 0)
            .ToList();
        Assert.Contains(callees, c =>
            c.Contains("AsyncTaskMethodBuilder") &&
            (c.Contains("SetResult") || c.Contains("AwaitUnsafeOnCompleted") || c.Contains("get_Task")));
    }

    /// <summary>
    /// Builds a planner for the MoveNext method of the async state machine
    /// and returns the template model and matched method for assertions.
    /// </summary>
    private static (AotCoreIrMethodArtifact Movenext, NativeAotTemplateModel TemplateModel, NativeAotMethodTemplateModel MatchMethod)
        BuildPlannerForMoveNext(TempCtx ctx, string asyncAssemblyPath)
    {
        var request = new ManagedClosureRequest(
            InputAssemblyPath: asyncAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: null,
            AdditionalAssemblyPaths: null,
            FullAssemblyClosure: true);
        var exec = new PipelinePlan().Execute(request);
        if (exec.IsFailure)
        {
            Assert.Fail($"Pipeline failed: {exec.Error?.Code}: {exec.Error?.Message}");
        }
        var result = exec.Value!;
        var movenext = result.AotCoreIr.Methods
            .FirstOrDefault(m => m.SubjectId.Contains(">d__") && m.SubjectId.Contains("::MoveNext"));
        Assert.NotNull(movenext);

        var planner = new NativeAotLoweringPlanner();
        var loweringPlan = new NativeAotLoweringPlanArtifact
        {
            PlanKind = "full-assembly-entry",
            AssemblyName = "AsyncTestAssembly",
            EntrySubjectId = movenext.SubjectId,
            NativeEntryFunctionName = string.Empty,
            EntrySymbol = movenext.NativeSymbol,
            EntryMethodToken = "0x06000001",
            WorkloadAbi = "full-assembly",
        };
        var closureManifest = new ManagedClosureManifestArtifact
        {
            AssemblyName = "AsyncTestAssembly",
            EntrySubjectId = movenext.SubjectId,
            InputAssemblyPath = asyncAssemblyPath,
            InputModuleVersionId = result.ClosureManifest!.InputModuleVersionId,
            FullAssemblyClosure = true,
            Artifacts = [],
        };
        var templateModel = planner.Create(
            loweringPlan,
            result.AotCoreIr,
            movenext,
            closureManifest,
            result.MetadataRegistration,
            result.SupplementalMetadataTemplate,
            fullAssemblyMode: true);
        var matchMethod = templateModel.Methods
            .FirstOrDefault(m => m.SubjectId == movenext.SubjectId);
        Assert.NotNull(matchMethod);
        return (movenext, templateModel, matchMethod);
    }

    [Fact]
    public void MovenextEmittedSource_ContainsStateMachineFields()
    {
        using var ctx = new TempCtx();
        var (_, _, matchMethod) = BuildPlannerForMoveNext(ctx, s_asyncAssemblyPath);

        // Phase 2 translator: MoveNext is emitted as real C++ via structured emission.
        // It should NOT contain the old GenPromise/GenCoro placeholder markers.
        Assert.DoesNotContain("AsyncPromise_", matchMethod.MethodSource);
        Assert.DoesNotContain("_Coro()", matchMethod.MethodSource);
        Assert.DoesNotContain("co_await", matchMethod.MethodSource);
        Assert.DoesNotContain("std::suspend_always", matchMethod.MethodSource);
        Assert.DoesNotContain("fallback", matchMethod.MethodSource, StringComparison.Ordinal);

        // The emitted code should contain the real state machine structure:
        // state field access, builder field access, native yield/awaiter calls.
        Assert.Contains("field_", matchMethod.MethodSource);
        Assert.Contains("1__state", matchMethod.MethodSource);
        Assert.Contains("t__builder", matchMethod.MethodSource);
        Assert.Contains("async_yield_", matchMethod.MethodSource);
        Assert.Contains("CHAOS_EH_TRY", matchMethod.MethodSource);
    }

    [Fact]
    public void MovenextEmittedSource_ContainsSetResultAndAwaitUnsafeOnCompleted()
    {
        using var ctx = new TempCtx();
        var (_, _, matchMethod) = BuildPlannerForMoveNext(ctx, s_asyncAssemblyPath);

        // Phase 2 segment B: builder/awaiter calls resolved to native externs.
        // SetResult, SetException, AwaitUnsafeOnCompleted should appear in the emitted C++.
        Assert.Contains("SetResult", matchMethod.MethodSource);
        Assert.Contains("SetException", matchMethod.MethodSource);
        Assert.Contains("AwaitUnsafeOnCompleted", matchMethod.MethodSource);
    }
}
