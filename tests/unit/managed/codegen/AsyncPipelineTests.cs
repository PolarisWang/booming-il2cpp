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

        return Path.Combine(
            repoRoot,
            "tests", "unit", "managed", "codegen",
            "AsyncTestAssembly", "bin", "Debug", "net8.0",
            "AsyncTestAssembly.dll");
    }

    private sealed class TempCtx : IDisposable
    {
        public string OutputRoot { get; } = Path.Combine(
            Path.GetTempPath(), "ChaosAsyncPipeline_" + Guid.NewGuid().ToString("N"));
        public void Dispose()
        {
            if (Directory.Exists(OutputRoot))
                Directory.Delete(OutputRoot, recursive: true);
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
}
