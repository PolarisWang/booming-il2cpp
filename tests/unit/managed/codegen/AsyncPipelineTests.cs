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

    /// <summary>
    /// Slice one emitted C++ function body out of the generated source by name.
    ///
    /// <para>
    /// Anchors on the emitted DEFINITION, not the first textual hit: the symbol
    /// also appears in the header extern block and the dispatch table, and
    /// slicing from the first hit runs past the function into unrelated code.
    /// The definition is introduced by the "// Managed method:" marker, so walk
    /// back over it when present, then take up to the closing brace at column 0.
    /// </para>
    ///
    /// <para>
    /// Splitting on "\n\n" instead (an earlier revision) produced chunks that
    /// spanned two adjacent functions, which made a wired method's call appear
    /// inside a rejected method's chunk and fired an anti-fake-green assertion
    /// on a false positive.
    /// </para>
    /// </summary>
    internal static string ExtractFunctionBody(string source, string functionName)
    {
        var marker = "// Managed method: ";
        var nameIdx = source.IndexOf("::" + functionName + "(", StringComparison.Ordinal);
        if (nameIdx < 0) return string.Empty;
        var lineStart = source.LastIndexOf('\n', nameIdx);
        lineStart = lineStart < 0 ? 0 : lineStart + 1;
        var prevLineStart = source.LastIndexOf('\n', lineStart - 2);
        if (prevLineStart >= 0 &&
            source.Substring(prevLineStart, lineStart - prevLineStart).Contains(marker))
        {
            lineStart = prevLineStart + 1;
        }
        var end = source.IndexOf("\n}", nameIdx, StringComparison.Ordinal);
        return end < 0 ? source[lineStart..] : source[lineStart..end];
    }

    private static string LocateAsyncAssemblyDll()
    {
        var repoRoot = RepoRootLocator.FindFromBaseDirectory();

        // Detect build configuration (Debug/Release) and TFM from the test runner's
        // output directory. BaseDirectory is typically:
        //   <repo>/tests/unit/managed/codegen/bin/<Configuration>/<TFM>/
        //
        // The previous heuristic walked up TWO levels from the TFM dir and required
        // that directory's parent to be named "bin" — but from
        // .../codegen/bin/Release/net8.0 that lands on .../codegen, whose parent is
        // the test project dir, not "bin".  The check therefore never matched and
        // the config silently fell back to "Debug", so a Release test run read a
        // stale Debug fixture (methods added since the last Debug build were simply
        // absent from the loader's world — which looks like a linker/codegen bug and
        // is not one).  Anchor on the directory named "bin" instead of assuming a
        // fixed depth.
        string config = "Debug";
        string tfm = "net8.0";
        var baseDir = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory);
        if (baseDir is not null)
        {
            tfm = baseDir.Name;
            var configDir = baseDir.Parent;
            var binDir = configDir?.Parent;
            if (binDir is not null &&
                string.Equals(binDir.Name, "bin", StringComparison.OrdinalIgnoreCase) &&
                configDir!.Name is "Debug" or "Release" or "RelWithDebInfo")
            {
                config = configDir.Name;
            }
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
            .FirstOrDefault(m => m.SubjectId.Contains("GetOne") && m.SubjectId.Contains(">d__") && m.SubjectId.Contains("::MoveNext"));
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
        // BuildPlannerForMoveNext resolves the <GetOne>d__0 state machine (the TCS-awaiting
        // AwaitTcs also emits a MoveNext, but it does NOT yield; yield-specific structure
        // assertions must target GetOne, which Task.Yields).
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
        // BuildPlannerForMoveNext resolves GetOne's state machine (which reaches SetResult
        // + AwaitUnsafeOnCompleted via Task.Yield). AwaitTcs's state machine awaits a TCS
        // instead and is a separate subject — not this assertion's target.
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

    [Fact]
    public void MovenextEmittedSource_SuspendReturnsAfterAwaitUnsafeOnCompleted()
    {
        using var ctx = new TempCtx();
        var (_, _, matchMethod) = BuildPlannerForMoveNext(ctx, s_asyncAssemblyPath);

        // R2-full resume-state fix.  On a real cross-thread suspension, the MoveNext
        // first-entry registers the continuation (AwaitUnsafeOnCompleted) and then MUST
        // return to the caller — it must NOT fall through to the unconditional task tail
        // SetResult(locals[1]), which runs before the worker has produced a value (and
        // would thus mark the Task complete with the stale/uninitialised result 0).
        //
        // Structural marker (visible in the emitted C++): after the AwaitUnsafeOnCompleted
        // registration there is a `return;` before any `set_result`.  The old buggy
        // emission dropped the suspend `leave` so control fell straight into the tail.
        string src = matchMethod.MethodSource;

        // Find the segment after the AwaitUnsafeOnCompleted call.
        int awIdx = src.IndexOf("AwaitUnsafeOnCompleted", StringComparison.Ordinal);
        Assert.True(awIdx >= 0, "MoveNext must contain the AwaitUnsafeOnCompleted suspension call");

        int setResultIdx = src.IndexOf("async_task_builder_set_result", awIdx, StringComparison.Ordinal);
        Assert.True(setResultIdx > awIdx, "The tail SetResult must occur after the AwaitUnsafeOnCompleted call");

        // Between the AwaitUnsafeOnCompleted registration and the tail SetResult there
        // must be at least one `return;` (the suspension exit). Without it the first entry
        // would run the tail SetResult(0) inline and complete the task prematurely.
        string between = src.Substring(awIdx, setResultIdx - awIdx);
        Assert.Contains("return", between);
    }

    [Fact]
    public void MovenextEmittedSource_BoxPointerPassedByValueNotStackSlot()
    {
        using var ctx = new TempCtx();
        var (_, tmpl, _) = BuildPlannerForMoveNext(ctx, s_asyncAssemblyPath);

        // R2b box ABI fix: async state-machine box pointers must be passed by VALUE
        // (the durable GC-heap address which already IS the struct address), NOT by
        // &chaos_locals[N] (the address of the C++ stack slot that holds the pointer).
        //
        // In the entry method (GetOne), the box pointer value from CHAOS_IL2CPP_NEW_GC
        // is stored in chaos_locals[0].  When that slot is ldloca'd and passed to
        // builder.Start as ref stateMachine, the emitted code must be:
        //   _s3 = chaos_locals[0];           ✓ box value (durable)
        // NOT:
        //   _s3 = &chaos_locals[0];          ✗ stack slot address (dies with frame)

        var entry = tmpl.Methods.FirstOrDefault(m =>
            m.SubjectId.Contains("AsyncMethods::GetOne") || m.SubjectId.Contains("AsyncMethods::DoVoid"));
        Assert.NotNull(entry);

        string entrySrc = entry!.MethodSource;

        // The entry must contain "chaos_locals[0]" (the box pointer value).
        Assert.Contains("chaos_locals[0]", entrySrc);
        // It must NOT use "&chaos_locals[0]" (stack slot address) for the box pointer.
        // The pattern "&chaos_locals[0]" would be the old buggy emission.
        Assert.DoesNotContain("&chaos_locals[0]", entrySrc);

        // However, other ldloca uses (e.g. &chaos_locals[2] for the awaiter slot)
        // must still use "&chaos_locals" — so verify the async helpers still work.
        // For the MoveNext body, the AwaitUnsafeOnCompleted box arg (slot 4) must
        // also be passed by value, not by stack-slot address.
        var (_, _, movenext) = BuildPlannerForMoveNext(ctx, s_asyncAssemblyPath);
        string mnSrc = movenext.MethodSource;

        // Verify the box pointer slot 4 is referenced as "chaos_locals[4]" (value)
        // when passed to the AwaitUnsafeOnCompleted call.  The exact C++ variable
        // name may be _s3 or another _sN; assert at the source-text level.
        // Before the AwaitUnsafeOnCompleted call there should be an assignment
        // of chaos_locals[4] (not &chaos_locals[4]).
        int aocIdx = mnSrc.IndexOf("AwaitUnsafeOnCompleted", StringComparison.Ordinal);
        Assert.True(aocIdx > 0, "MoveNext must contain AwaitUnsafeOnCompleted");

        // Find the segment before AwaitUnsafeOnCompleted that sets up its chaos_arg_2
        // (the ref state_machine argument). This contains the box arg loading.
        string preAoc = aocIdx > 200 ? mnSrc.Substring(aocIdx - 200, 200) : mnSrc.Substring(0, aocIdx);
        // It should NOT contain &chaos_locals[4] (the old buggy pattern).
        Assert.DoesNotContain("&chaos_locals[4]", preAoc);

        // Reference-type locals (&chaos_locals[2] for awaiter slots) should STILL
        // use & — verify that the general ldloca pattern is not disabled.
        Assert.Contains("&chaos_locals[2]", mnSrc);
    }

    [Fact]
    public void FullAssemblyClosure_SurfacesTaskCompletionSourceSubject()
    {
        if (!File.Exists(s_asyncAssemblyPath))
        {
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

        // The async method that awaits a TCS must surface.
        var subjectIds = result.AotCoreIr.Methods
            .Select(m => m.SubjectId)
            .ToList();
        Assert.Contains(subjectIds, id => id.Contains("AwaitTcs"));

        // Its compiler-generated state machine surfaces too.
        Assert.Contains(subjectIds, id =>
            id.Contains(">d__") && id.Contains("MoveNext"));

        // The IR method graph must reach the TaskCompletionSource construction and
        // accessor path (newobj/get_Task are part of the awaited-TCS lowering).
        var callGraph = result.AotCoreIr.Methods
            .SelectMany(m => m.Instructions)
            .Select(i => i.Callee ?? i.TargetReference?.SubjectId ?? string.Empty)
            .Where(s => s.Length > 0)
            .ToList();
        Assert.Contains(callGraph, s => s.Contains("System.Threading.Tasks.TaskCompletionSource"));
    }

    /// <summary>
    /// ASYNC-P1-1: Task.Run reaches the AOT IR end to end, and the lowered call
    /// routes to the native ThreadPool runner rather than the interpreter
    /// fallback.
    ///
    /// <para>
    /// This was previously unwritable. An earlier revision of this test asserted
    /// only that the async surface was intact and carried a comment declaring a
    /// "known limitation: AsyncTestAssembly's non-async methods (RunAction) do
    /// not reach the extracted AOT IR". That limitation was never real — it was
    /// an artifact of the repo-root walk reading a stale fixture from the main
    /// checkout (see ASYNC-P1-5, commit f80aada50). With the fixture resolving
    /// to this tree, RunAction loads and lowers normally.
    /// </para>
    ///
    /// <para>
    /// The registry-level assertion for the same wiring lives in
    /// RuntimeHelperShapeRegistryTests.TaskRun_WiredToGenericShape. This test
    /// covers the pipeline-level view: the subject is present, and the emitted
    /// C++ for it calls the native symbol.
    /// </para>
    /// </summary>
    [Fact]
    public void TaskRun_LowersToNativeAsyncTaskRun()
    {
        Assert.True(File.Exists(s_asyncAssemblyPath),
            $"AsyncTestAssembly.dll not built at {s_asyncAssemblyPath}");

        using var ctx = new TempCtx();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: s_asyncAssemblyPath,
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

        var subjectIds = result.AotCoreIr.Methods.Select(m => m.SubjectId).ToList();

        // The async surface must be intact.
        Assert.Contains(subjectIds, id => id.Contains("AsyncMethods::GetOne"));
        Assert.Contains(subjectIds, id => id.Contains("AsyncMethods::AwaitTcs"));
        Assert.Contains(subjectIds, id => id.Contains(">d__") && id.Contains("::MoveNext"));

        // ...and the plain non-async method that calls Task.Run must also be there.
        Assert.Contains(subjectIds, id => id.Contains("AsyncMethods::RunAction"));

        // The Task.Run call inside RunAction must lower to the native runner.
        var runAction = result.AotCoreIr.Methods
            .Single(m => m.SubjectId.Contains("AsyncMethods::RunAction"));

        // At the AOT IR layer the call is still recorded against the MANAGED target
        // (Task::Run). Substitution to the native symbol happens later, during
        // emission, driven by the ShapeRegistry's DirectNativeSymbol. So the IR layer
        // proves only that the call site survived lowering.
        var loweredCalls = runAction.Instructions
            .Select(i => i.TargetSymbol ?? i.Callee ?? i.TargetReference?.SubjectId)
            .Where(s => !string.IsNullOrEmpty(s))
            .ToList();
        Assert.Contains(loweredCalls, s => s!.Contains("System.Threading.Tasks.Task::Run"));

        // The load-bearing assertion is at the emission layer. This must emit the
        // WHOLE assembly closure; the registry helper is emitted where the invocation
        // is lowered, and slicing to a single subject is how an earlier revision of
        // this test fooled itself into a weaker claim.
        var outputRoot = Path.Combine(ctx.OutputRoot, "asyncgen");
        var emitted = new NativeAotEmitter().GenerateFromArtifacts(
            result.NativeAotLoweringPlan,
            result.AotCoreIr,
            result.ClosureManifest!,
            result.MetadataRegistration,
            result.SupplementalMetadataTemplate,
            outputRoot,
            mode: CodegenMode.Aot,
            subjectMethods: null,
            goldProfilePath: null,
            allManagedMethods: result.AllManagedMethods);

        var allGenerated = string.Join("\n",
            emitted.GeneratedSources.Select(source =>
                source.Contents ?? source.ContentsBuilder?.ToString() ?? string.Empty));

        // Task.Run is wired: the lowered invocation calls the native ThreadPool runner.
        Assert.Contains("async_task_run", allGenerated);

        // Anti-fake-green: the call must NOT fall through to the interpreter's
        // return-0 fallback. If it did, awaiting the result would silently yield 0
        // (or null) instead of running the action — the exact class of defect this
        // phase exists to eliminate.
        Assert.DoesNotContain("ChaosExternalRuntimeFallback(async_task_run", allGenerated);
    }

    /// <summary>
    /// ASYNC-P2-4: the two ContinueWith overload families must take DIFFERENT
    /// paths through emission, and the difference must be visible in the emitted
    /// C++.
    ///
    /// <para>
    /// Registry-level tests (RuntimeHelperShapeRegistryTests.ContinueWith_*)
    /// pin the resolver's decision; this test pins its consequence. A resolver
    /// that returned null but was never consulted, or an emitter that rewrote
    /// the call anyway, would leave the earlier tests green while the generated
    /// code still ran the body with the options argument discarded.
    /// </para>
    /// </summary>
    [Fact]
    public void ContinueWith_OverloadFamilies_EmitDifferentCode()
    {
        Assert.True(File.Exists(s_asyncAssemblyPath),
            $"AsyncTestAssembly.dll not built at {s_asyncAssemblyPath}");

        using var ctx = new TempCtx();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: s_asyncAssemblyPath,
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

        var outputRoot = Path.Combine(ctx.OutputRoot, "asyncgen");
        var emitted = new NativeAotEmitter().GenerateFromArtifacts(
            result.NativeAotLoweringPlan,
            result.AotCoreIr,
            result.ClosureManifest!,
            result.MetadataRegistration,
            result.SupplementalMetadataTemplate,
            outputRoot,
            mode: CodegenMode.Aot,
            subjectMethods: null,
            goldProfilePath: null,
            allManagedMethods: result.AllManagedMethods);

        var allGenerated = string.Join("\n",
            emitted.GeneratedSources.Select(source =>
                source.Contents ?? source.ContentsBuilder?.ToString() ?? string.Empty));

        // Both ContinueWith methods must reach the AOT IR as lowered call sites —
        // otherwise the assertions below pass vacuously.
        var subjectIds = result.AotCoreIr.Methods.Select(m => m.SubjectId).ToList();
        Assert.Contains(subjectIds, id => id.Contains("AsyncMethods::ContinueWithAction"));
        Assert.Contains(subjectIds, id => id.Contains("AsyncMethods::ContinueWithOptions"));

        // Slice out one emitted C++ function body by name. A previous revision
        // split on "\n\n" and took the first chunk containing the name, which can
        // span two adjacent functions — the wired method's call then "appeared"
        // in the rejected method's chunk and the anti-fake-green assertion fired
        // on a false positive. Anchor on the function signature and take up to
        // its closing brace at column 0.
        var actionBody = ExtractFunctionBody(allGenerated, "ContinueWithAction");
        Assert.False(string.IsNullOrEmpty(actionBody),
            "the wired ContinueWith overload must have an emitted body");
        Assert.Contains("chaos_task_continue_with", actionBody);

        // Anti-fake-green for the REJECTED overload. It has a DIFFERENT arity (3
        // params); a resolver matching on method name alone would route it too and
        // emit a call that runs the body with `options` silently discarded —
        // the caller asked for TaskContinuationOptions and got none. Its lowered
        // body must not reach the native helper.
        var optionsBody = ExtractFunctionBody(allGenerated, "ContinueWithOptions");
        Assert.False(string.IsNullOrEmpty(optionsBody),
            "the rejected ContinueWith overload must still have an emitted body (via the interpreter fallback)");
        Assert.DoesNotContain("chaos_task_continue_with", optionsBody);
    }

    /// <summary>
    /// ASYNC-P2-5: <c>Task.Factory.StartNew(Action)</c> defaults to the SAME
    /// execution semantics as <c>Task.Run(Action)</c> — both queue the delegate on
    /// the default scheduler. Routing the factory overload onto the existing
    /// ThreadPool-backed <c>async_task_run</c> is therefore not an approximation:
    /// it is what .NET does.
    ///
    /// <para>
    /// The alternative — leaving Task.Factory unwired — is the defect: it resolves
    /// to the interpreter's return-0 fallback, so <c>Task.Factory.StartNew(work)</c>
    /// yields a bogus task and <c>work</c> never runs.
    /// </para>
    ///
    /// <para>
    /// This test pairs with the resolver test in
    /// RuntimeHelperShapeRegistryTests.TaskFactoryStartNew_*. The registry test
    /// pins the decision; this one pins that the decision is actually consulted
    /// by the real pipeline, using the callee spelling the pipeline emits.
    /// </para>
    /// </summary>
    [Fact]
    public void TaskFactoryStartNew_ResolvesToThreadPoolRunner()
    {
        Assert.True(File.Exists(s_asyncAssemblyPath),
            $"AsyncTestAssembly.dll not built at {s_asyncAssemblyPath}");

        using var ctx = new TempCtx();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: s_asyncAssemblyPath,
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

        var subjectIds = result.AotCoreIr.Methods.Select(m => m.SubjectId).ToList();
        Assert.Contains(subjectIds, id => id.Contains("AsyncMethods::FactoryStartNew"));

        var outputRoot = Path.Combine(ctx.OutputRoot, "factorygen");
        var emitted = new NativeAotEmitter().GenerateFromArtifacts(
            result.NativeAotLoweringPlan,
            result.AotCoreIr,
            result.ClosureManifest!,
            result.MetadataRegistration,
            result.SupplementalMetadataTemplate,
            outputRoot,
            mode: CodegenMode.Aot,
            subjectMethods: null,
            goldProfilePath: null,
            allManagedMethods: result.AllManagedMethods);

        var allGenerated = string.Join("\n",
            emitted.GeneratedSources.Select(source =>
                source.Contents ?? source.ContentsBuilder?.ToString() ?? string.Empty));

        var body = ExtractFunctionBody(allGenerated, "FactoryStartNew");
        Assert.False(string.IsNullOrEmpty(body),
            "Task.Factory.StartNew caller must have an emitted body");

        // Wired: the call reaches the native TaskFactory entry point, which is a
        // thin shim over the same ThreadPool runner Task.Run uses
        // (chaos_task_factory_start_new -> async_task_run in async_stubs.cpp).
        // The native shim is asserted separately by the smoke test; here we are
        // pinning the CODEGEN decision, i.e. that this call site lowered to the
        // native symbol rather than the interpreter.
        Assert.Contains("chaos_task_factory_start_new", body);

        // Anti-fake-green: NOT the interpreter's return-0 fallback. If the
        // resolver never fired, the body would carry the external-runtime stub
        // instead of a real call, and StartNew would silently return a bogus task.
        Assert.DoesNotContain("ChaosExternalRuntimeFallback", body);
        Assert.DoesNotContain("chaos_external_runtime_", body);

        // The factory property itself should also be routed, so that the receiver
        // handed to StartNew is the native token rather than an interpreter 0.
        //
        // This check is GUARDED and may be vacuous: Roslyn elides the source-level
        // local in `TaskFactory f = Task.Factory; f.StartNew(work)` (the value is
        // used once), so get_Factory is inlined into the StartNew call site and no
        // separate get_Factory body is emitted for this fixture.  Asserting the
        // body's presence would fail on a correct pipeline.  The property's own
        // routing IS pinned non-vacuously by
        // RuntimeHelperShapeRegistryTests.TaskFactory_GetFactoryProperty_RoutesToNativeToken.
        var factoryGetter = ExtractFunctionBody(allGenerated, "get_Factory");
        if (!string.IsNullOrEmpty(factoryGetter))
        {
            Assert.DoesNotContain("ChaosExternalRuntimeFallback", factoryGetter);
        }
    }

    /// <summary>
    /// ASYNC-P2-6 — Task.WhenAll&lt;TResult&gt;(Task&lt;T&gt;[]) must lower to the same
    /// native array combinator as the non-generic Task[] overload.
    ///
    /// <para>
    /// Both overloads unpack a managed array and produce an aggregate whose
    /// result is the child result set, so they share chaos_task_when_all_array.
    /// The resolver's guard used to require the literal non-generic return type
    /// (<c>::WhenAll:System.Threading.Tasks.Task(</c>), which the generic
    /// overload's <c>Task&lt;int[]&gt;</c> return never matches — so
    /// <c>Task.WhenAll(tasks)</c> on <c>Task&lt;int&gt;[]</c> silently fell through to
    /// the interpreter's return-0 fallback and the caller awaited a bogus task.
    /// </para>
    /// </summary>
    [Fact]
    public void WhenAllGeneric_ResolvesToArrayCombinator()
    {
        Assert.True(File.Exists(s_asyncAssemblyPath),
            $"AsyncTestAssembly.dll not built at {s_asyncAssemblyPath}");

        using var ctx = new TempCtx();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: s_asyncAssemblyPath,
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

        Assert.Contains(result.AotCoreIr.Methods.Select(m => m.SubjectId),
            id => id.Contains("AsyncMethods::WhenAllOfInt"));

        var emitted = new NativeAotEmitter().GenerateFromArtifacts(
            result.NativeAotLoweringPlan,
            result.AotCoreIr,
            result.ClosureManifest!,
            result.MetadataRegistration,
            result.SupplementalMetadataTemplate,
            Path.Combine(ctx.OutputRoot, "whenallgen"),
            mode: CodegenMode.Aot,
            subjectMethods: null,
            goldProfilePath: null,
            allManagedMethods: result.AllManagedMethods);

        var allGenerated = string.Join("\n",
            emitted.GeneratedSources.Select(source =>
                source.Contents ?? source.ContentsBuilder?.ToString() ?? string.Empty));

        var body = ExtractFunctionBody(allGenerated, "WhenAllOfInt");
        Assert.False(string.IsNullOrEmpty(body),
            "Task.WhenAll<int> caller must have an emitted body");

        Assert.Contains("chaos_task_when_all_array", body);

        // Anti-fake-green: not the interpreter's return-0 fallback.  Without the
        // wiring the awaited aggregate would be a null handle.
        Assert.DoesNotContain("ChaosExternalRuntimeFallback", body);
        Assert.DoesNotContain("chaos_external_runtime_", body);
    }

    /// <summary>
    /// Repo-relative stable output dir for R2-full native round-trip proof.
    /// Emitted C++ (native-aot.generated.*.h/cpp etc.) and the hand-written
    /// driver + CMakeLists.txt live here, ready for a bounded native compile.
    /// </summary>
    private static string R2FullOutputRoot()
    {
        var repoRoot = RepoRootLocator.FindFromBaseDirectory();
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
