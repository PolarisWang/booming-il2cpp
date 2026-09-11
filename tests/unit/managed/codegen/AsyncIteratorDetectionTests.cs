// ASYNC-P2-8 A1: explicit detection + diagnostic for async iterator shapes.
//
// WHY THIS TEST EXISTS
// --------------------
// Before A1, an `async IAsyncEnumerable<T>` method produced NO loud signal. Its
// AsyncIteratorMethodBuilder calls were unrecognised by the shape registry, so they
// degraded to a chaos_external_runtime_* stub returning 0 — a silently-wrong
// iterable that only misbehaves much later (null iterable, or a crash inside
// await foreach). A1 converts that into an explicit, recorded signal at lowering time.
//
// WHY THE SIGNAL IS A RECORDED LIST, NOT AN EXCEPTION
// ---------------------------------------------------
// The first cut of A1 threw NotSupportedException from EmitManagedMethod and these
// tests asserted the throw. That was WRONG, and the tests failed for the right reason:
// emission runs behind BuildMethodSourceSafe, which catches EVERY exception and
// substitutes an unreachable stub. The throw was swallowed, the build stayed green, and
// the "loud failure" was itself a fake green — the exact defect A1 exists to remove.
//
// So A1 records the offending subject ids and the generator surfaces them as
// kUnsupportedAsyncIteratorCount / kUnsupportedAsyncIteratorSubjects in the generated
// C++. These tests assert on that recorded signal.
//
// WHERE THE GUARD LIVES, AND WHY THIS TEST DRIVES THE PLANNER DIRECTLY
// --------------------------------------------------------------------
// The guard fires inside NativeAotLoweringPlanner.EmitManagedMethod. That code runs
// during the *emission* stage, which PipelinePlan.Execute does NOT reach for a
// closure-only request — a first attempt at this test called Execute(), got
// IsFailure=false, and proved only that the artifact stage was clean, not that the
// guard worked. (Diagnosed with a temporary probe in EmitManagedMethod: it was never
// entered.) So the test drives NativeAotLoweringPlanner.Create() directly, the same
// route AsyncPipelineTests.BuildPlannerForMoveNext uses.
//
// These tests assert the DETECTION, not a successful lowering. When A2-A4 land and
// iterators actually lower, they must be rewritten — that is intentional: they encode
// today's contract ("we do not support this yet, and we say so"), not a permanent one.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Pipeline;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class AsyncIteratorDetectionTests
{
    private static readonly string s_iteratorAssemblyPath = LocateIteratorAssemblyDll();

    /// <summary>
    /// Locates the net9.0 iterator fixture. Mirrors AsyncPipelineTests.LocateAsyncAssemblyDll:
    /// resolve from the runner's output dir, and FAIL LOUDLY rather than falling back to a
    /// default. A silent fallback is exactly how the async fixture hid a stale-Debug read
    /// for an unknown number of revisions (see STATUS.md, P2-6).
    /// </summary>
    private static string LocateIteratorAssemblyDll()
    {
        var repoRoot = RepoRootLocator.FindFromBaseDirectory();
        string config = "Debug";
        var baseDir = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory);
        if (baseDir is not null)
        {
            var configDir = baseDir.Parent;
            var binDir = configDir?.Parent;
            if (binDir is not null &&
                string.Equals(binDir.Name, "bin", StringComparison.OrdinalIgnoreCase) &&
                configDir!.Name is "Debug" or "Release" or "RelWithDebInfo")
            {
                config = configDir.Name;
            }
        }

        var resolved = Path.Combine(
            repoRoot,
            "tests", "unit", "managed", "codegen",
            "AsyncIteratorTestAssembly", "bin", config, "net9.0",
            "AsyncIteratorTestAssembly.dll");
        if (!File.Exists(resolved))
        {
            throw new FileNotFoundException(
                $"AsyncIteratorTestAssembly fixture not resolved. Looked for '{resolved}'. "
                + $"Test output dir was '{AppDomain.CurrentDomain.BaseDirectory}' (config='{config}').",
                resolved);
        }
        return resolved;
    }

    private sealed class TempCtx : IDisposable
    {
        public string OutputRoot { get; } = Path.Combine(
            Path.GetTempPath(), "ChaosAsyncIterator_" + Guid.NewGuid().ToString("N"));
        public void Dispose()
        {
            try
            {
                if (Directory.Exists(OutputRoot)) Directory.Delete(OutputRoot, recursive: true);
            }
            catch { /* cleanup failures must not fail the test */ }
        }
    }

    /// <summary>
    /// The outcome of driving the emission planner over one async state machine.
    /// <see cref="GlobalDeclarations"/> is the file-scope C++ emitted by the planner —
    /// which is where the A1 signal actually lands. Asserting on the planner's in-memory
    /// list alone would pass even if the list were never surfaced into the build output.
    /// </summary>
    private sealed record EmitOutcome(
        Exception? Failure,
        NativeAotLoweringPlanner Planner,
        string GlobalDeclarations);

    /// <summary>
    /// Runs the pipeline, then drives the emission planner over the FIRST async
    /// state-machine MoveNext whose subject id matches <paramref name="subjectFilter"/>.
    /// Returns the exception thrown by emission (if any), the planner (so its recorded
    /// A1 state can be inspected), and the generated C++ source.
    /// </summary>
    private static EmitOutcome EmitAndCapture(string subjectFilter, string label)
    {
        using var ctx = new TempCtx();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: s_iteratorAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: null,
            AdditionalAssemblyPaths: null,
            FullAssemblyClosure: true);
        var exec = new PipelinePlan().Execute(request);
        Assert.False(exec.IsFailure,
            $"pipeline execution failed before emission: {exec.Error?.Code}: {exec.Error?.Message}");
        var result = exec.Value!;

        var movenext = result.AotCoreIr.Methods
            .FirstOrDefault(m => (m.SubjectId ?? string.Empty).Contains(subjectFilter)
                              && (m.SubjectId ?? string.Empty).Contains(">d__")
                              && (m.SubjectId ?? string.Empty).Contains("::MoveNext"));
        Assert.True(movenext is not null,
            $"fixture did not surface a '{subjectFilter}' async state machine MoveNext; "
            + "the fixture assembly is stale or the method was renamed. Subjects seen: "
            + string.Join(", ", result.AotCoreIr.Methods.Select(m => m.SubjectId)
                .Where(s => s is not null && s.Contains("d__")).Distinct()));

        var planner = new NativeAotLoweringPlanner();
        var loweringPlan = new NativeAotLoweringPlanArtifact
        {
            PlanKind = "full-assembly-entry",
            AssemblyName = "AsyncIteratorTestAssembly",
            EntrySubjectId = movenext!.SubjectId,
            NativeEntryFunctionName = string.Empty,
            EntrySymbol = movenext.NativeSymbol,
            EntryMethodToken = "0x06000001",
            WorkloadAbi = "full-assembly",
        };
        var closureManifest = new ManagedClosureManifestArtifact
        {
            AssemblyName = "AsyncIteratorTestAssembly",
            EntrySubjectId = movenext.SubjectId,
            InputAssemblyPath = s_iteratorAssemblyPath,
            InputModuleVersionId = result.ClosureManifest!.InputModuleVersionId,
            FullAssemblyClosure = true,
            Artifacts = [],
        };

        Exception? failure = null;
        NativeAotTemplateModel? emission = null;
        try
        {
            emission = planner.Create(
                loweringPlan,
                result.AotCoreIr,
                movenext,
                closureManifest,
                result.MetadataRegistration,
                result.SupplementalMetadataTemplate,
                fullAssemblyMode: true);
        }
        catch (Exception ex)
        {
            failure = ex;
        }
        _ = label;
        return new EmitOutcome(failure, planner, emission?.GlobalDeclarations ?? string.Empty);
    }

    /// <summary>
    /// The load-bearing test. Emitting an async-iterator state machine must RECORD the
    /// subject as unsupported and surface it into the generated C++ — not succeed while
    /// silently carrying a chaos_external_runtime_* stub (the pre-A1 behaviour).
    ///
    /// <para>
    /// COUNTEREXAMPLE (决策2=A): comment out the `AsyncMethodKind.AsyncIterator` branch in
    /// NativeAotLoweringPlanner.MethodEmission.cs (and the IsAsyncIteratorBuilderCallee
    /// guard in ...ExternalRuntimeHelpers.TypeResolution.cs), rebuild the Generator, and
    /// re-run. The subject id is then never recorded, the emitted source carries no
    /// kUnsupportedAsyncIteratorCount and no subject literal, and every assertion below
    /// goes RED. Verified — see the commit's regression_check.
    /// </para>
    /// </summary>
    [Fact]
    public void AsyncIteratorMoveNext_IsRecordedAndSurfaced_NotSilentlyStubbed()
    {
        var outcome = EmitAndCapture("YieldOne", "YieldOne");

        // Detection happened at all. A throw would ALSO be swallowed by
        // BuildMethodSourceSafe in production, so a null here is not itself a pass —
        // but an exception at this level means the planner is doing something
        // unmodelled, which the assertions below would misread.
        Assert.True(outcome.Failure is null,
            "Emission of an async iterator must be handled by the A1 branch, not by an "
            + "uncaught exception at the planner level: "
            + outcome.Failure?.GetType().Name + ": " + outcome.Failure?.Message);

        var recorded = outcome.Planner.UnsupportedAsyncIteratorSubjectIds;
        Assert.True(recorded.Count > 0,
            "Emitting an async-iterator state machine must record the subject as unsupported. "
            + "An empty list means AsyncIteratorMethodBuilder was silently accepted — the "
            + "pre-A1 behaviour this change removes.");

        // Discriminating assertion: the RECORDED id must be the iterator state machine,
        // not some incidental other method. Without this, ANY recording would satisfy the
        // test and removing the detection could leave it green — the non-discriminating
        // assertion shape P2-7 fixed.
        Assert.Contains(recorded, s => s.Contains("YieldOne") && s.Contains(">d__"));

        // The signal must reach the generated C++. Asserting on the in-memory list alone
        // would pass even if EmitUnsupportedAsyncIteratorDeclarations were never called —
        // i.e. even if the signal went nowhere. This is the assertion that makes the
        // channel real.
        Assert.Contains("kUnsupportedAsyncIteratorCount", outcome.GlobalDeclarations);
        Assert.Contains("kUnsupportedAsyncIteratorSubjects", outcome.GlobalDeclarations);
        Assert.Contains("YieldOne", outcome.GlobalDeclarations);

        // And it must NOT claim the count is zero while a subject is listed.
        Assert.DoesNotContain("kUnsupportedAsyncIteratorCount = 0", outcome.GlobalDeclarations);
    }

    /// <summary>
    /// Same contract for an iterator that both awaits and yields — the shape
    /// Task.WhenEach's WhenEachState.Iterate&lt;T&gt; actually uses.
    /// </summary>
    [Fact]
    public void AsyncIteratorWithAwait_IsRecordedAndSurfaced()
    {
        var outcome = EmitAndCapture("YieldAfterAwait", "YieldAfterAwait");

        Assert.True(outcome.Failure is null,
            "Emission must be handled by the A1 branch: "
            + outcome.Failure?.GetType().Name + ": " + outcome.Failure?.Message);

        Assert.Contains(outcome.Planner.UnsupportedAsyncIteratorSubjectIds,
            s => s.Contains("YieldAfterAwait") && s.Contains(">d__"));
        Assert.Contains("kUnsupportedAsyncIteratorCount", outcome.GlobalDeclarations);
        Assert.Contains("YieldAfterAwait", outcome.GlobalDeclarations);
    }

    /// <summary>
    /// The counterpart that makes A1 SCOPED. A plain async Task method in the same net9.0
    /// assembly must still lower normally. Without this, a blanket "reject anything with a
    /// d__" change would pass the tests above while breaking all async support.
    /// </summary>
    [Fact]
    public void PlainAsyncMoveNext_StillLowers_GuardIsScoped()
    {
        var outcome = EmitAndCapture("PlainAsync", "PlainAsync");

        Assert.True(outcome.Failure is null,
            "A plain async Task state machine must keep lowering after A1. It threw: "
            + outcome.Failure?.GetType().Name + ": " + outcome.Failure?.Message);

        Assert.DoesNotContain(outcome.Planner.UnsupportedAsyncIteratorSubjectIds,
            s => s.Contains("PlainAsync"));
    }
}
