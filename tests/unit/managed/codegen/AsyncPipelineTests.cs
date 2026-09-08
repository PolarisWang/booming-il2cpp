using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Generator;
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
        // SetResult, SetException, AwaitUnsafeOnCompleted should appear in the emitted C++,
        // either as the managed method name (SetResult / SetException) or as the native
        // function name (async_task_builder_set_result_raw / async_task_builder_set_exception).
        bool hasSetResult = matchMethod.MethodSource.Contains("SetResult") ||
            matchMethod.MethodSource.Contains("async_task_builder_set_result");
        Assert.True(hasSetResult, "MoveNext body should contain SetResult or async_task_builder_set_result");
        bool hasSetException = matchMethod.MethodSource.Contains("SetException") ||
            matchMethod.MethodSource.Contains("async_task_builder_set_exception");
        Assert.True(hasSetException, "MoveNext body should contain SetException or async_task_builder_set_exception");
        Assert.Contains("AwaitUnsafeOnCompleted", matchMethod.MethodSource);
    }

    [Fact]
    public void MovenextEmittedSource_CreateAndGetTaskRouteToNativeAsyncBuilder()
    {
        using var ctx = new TempCtx();
        // The async entry surfaces alongside MoveNext in full-assembly template.
        var (_, tmpl, _) = BuildPlannerForMoveNext(ctx, s_asyncAssemblyPath);
        var entry = tmpl.Methods.FirstOrDefault(m =>
            m.SubjectId.Contains("AsyncMethods::GetOne") || m.SubjectId.Contains("AsyncMethods::DoVoid"));
        Assert.NotNull(entry);

        // Phase 2 segment B/C: the async method's builder create/get_Task must route
        // to the native async.h helpers — NOT the interpreter fallback (which returns 0).
        Assert.Contains("chaos_async_task_builder_create", entry!.MethodSource);
        Assert.Contains("async_task_builder_get_task", entry.MethodSource);
        Assert.DoesNotContain("ChaosExternalRuntimeFallback", entry.MethodSource);
    }

    [Fact]
    public void MovenextEmittedSource_SetResultAndSetExceptionRouteToNativeAsyncBuilder()
    {
        using var ctx = new TempCtx();
        var (_, _, matchMethod) = BuildPlannerForMoveNext(ctx, s_asyncAssemblyPath);

        // Phase 2 segment B/C: the builder's SetResult / SetException inside the state
        // machine's MoveNext body route to native async.h setters, not interpreter fallback.
        bool hasResult = matchMethod.MethodSource.Contains("async_task_builder_set_result_raw") ||
            matchMethod.MethodSource.Contains("async_task_builder_set_result_void");
        Assert.True(hasResult, "MoveNext body should call async_task_builder_set_result_raw/_void");
        Assert.Contains("async_task_builder_set_exception", matchMethod.MethodSource);
    }

    [Fact]
    public void GetOneEntry_AllocatesOnGcHeapNotStack()
    {
        using var ctx = new TempCtx();
        var (_, tmpl, _) = BuildPlannerForMoveNext(ctx, s_asyncAssemblyPath);
        var entry = tmpl.Methods.FirstOrDefault(m =>
            m.SubjectId.Contains("AsyncMethods::GetOne") || m.SubjectId.Contains("AsyncMethods::DoVoid"));
        Assert.NotNull(entry);

        // R2b: async entry must allocate the >d__ box on the GC heap — not on the C++
        // stack — so the box survives the entry frame returning and is alive when the
        // continuation resumes on another thread.
        Assert.DoesNotContain("__chaos_stack_obj", entry!.MethodSource);
        Assert.Contains("CHAOS_IL2CPP_NEW_GC", entry.MethodSource);
    }

    /// <summary>
    /// Repo-relative stable output dir for R2-full native round-trip proof.
    /// Emitted C++ (native-aot.generated.*.h/cpp etc.) and the hand-written
    /// driver + CMakeLists.txt live here, ready for a bounded native compile.
    /// </summary>
    private static string R2FullOutputRoot()
    {
        var dir = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory);
        while (dir != null && !Directory.Exists(Path.Combine(dir.FullName, ".git")))
            dir = dir.Parent;
        var repoRoot = dir?.FullName ?? throw new DirectoryNotFoundException(
            "Could not locate repository root (.git directory).");
        return Path.Combine(repoRoot, "artifacts", "r2full", "asyncgen");
    }

    /// <summary>
    /// Full NATIVE emission of the real AsyncTestAssembly closure into a stable
    /// dir. Drives PipelinePlan.Execute -> NativeAotEmitter.GenerateFromArtifacts
    /// (the same surface FullAssemblyEmitter uses) and writes every generated
    /// source page to disk so the files can be compiled + linked by a bounded
    /// native driver (not the whole framework).
    ///
    /// This is R2-full-1 step 1: surface the REAL compiler-emitted C++ for
    /// GetOne's async state machine (incl. &lt;GetOne&gt;d__0::MoveNext) as
    /// compilable headers/translation units.
    /// </summary>
    [Fact]
    public void R2Full_RealCodegen_EmitsGetOneAndMoveNext()
    {
        Assert.True(File.Exists(s_asyncAssemblyPath),
            $"AsyncTestAssembly.dll not built at {s_asyncAssemblyPath}");

        var request = new ManagedClosureRequest(
            InputAssemblyPath: s_asyncAssemblyPath,
            OutputRootPath: R2FullOutputRoot(),
            EntryPointSubjectIdOverride: null,
            AdditionalAssemblyPaths: null,
            FullAssemblyClosure: true);

        var exec = new PipelinePlan().Execute(request);
        Assert.False(exec.IsFailure,
            $"Pipeline failed: {exec.Error?.Code}: {exec.Error?.Message}");
        var result = exec.Value!;

        // GenerateFromArtifacts emits the full assembly. The pipeline's OWN
        // lowering plan is used so this proves the un-modified codegen surface
        // produces the async state machine C++ (correct plan/entry for closure).
        var emitted = new NativeAotEmitter().GenerateFromArtifacts(
            result.NativeAotLoweringPlan,
            result.AotCoreIr,
            result.ClosureManifest!,
            result.MetadataRegistration,
            result.SupplementalMetadataTemplate,
            R2FullOutputRoot(),
            mode: CodegenMode.Aot,
            subjectMethods: null,
            goldProfilePath: null,
            allManagedMethods: result.AllManagedMethods);

        var outputRoot = R2FullOutputRoot();
        Directory.CreateDirectory(outputRoot);

        var written = new List<string>();
        foreach (var source in emitted.GeneratedSources)
        {
            string contents = source.Contents
                ?? source.ContentsBuilder?.ToString()
                ?? string.Empty;
            if (contents.Length == 0)
                continue; // structural entries (paths only) have no source text
            var destPath = Path.Combine(outputRoot, source.RelativePath);
            Directory.CreateDirectory(Path.GetDirectoryName(destPath)!);
            File.WriteAllText(destPath, contents);
            written.Add(source.RelativePath);
        }

        // The async state machine must be part of what got emitted to native C++
        // headers/TUs. Spans page source so the assertion is cheap.
        string combined = string.Join("\n", written);
        Assert.Contains("generated", combined, StringComparison.Ordinal);

        // GetOne entry and the compiler-generated MoveNext body must reference the
        // native async.h-building helpers (async_task_builder_* / async_builder_*).
        string pagesPath = written.FirstOrDefault(w =>
            w.EndsWith(".cpp", StringComparison.Ordinal) && w.Contains("generated", StringComparison.Ordinal))
            ?? throw new Xunit.Sdk.XunitException(
                "Emission produced no native-aot.generated.cpp; cannot verify bodies.");
        var pageContents = File.ReadAllText(Path.Combine(outputRoot, pagesPath));

        Assert.Contains("GetOne", pageContents);
        Assert.Contains("d__", pageContents);
        Assert.Contains("MoveNext", pageContents);
        Assert.True(pageContents.Contains("async_task_builder") || pageContents.Contains("async_builder"),
            "GetOne/MoveNext emitted C++ should route builder ops to native async.h helpers.");
    }
}
