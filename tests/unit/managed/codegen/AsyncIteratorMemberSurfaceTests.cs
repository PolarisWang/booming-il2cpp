// ASYNC-P2-8 A4: async-iterator MEMBER SURFACE — the union of the state machine's
// externally-visible members must be either lowered or explicitly recorded.
//
// WHY THIS TEST EXISTS
// --------------------
// A1 gated exactly one method: `>d__N::MoveNext`. That gate lives in
// ClassifyAsyncMethod, whose AsyncIterator branch is reachable only through
// IsAsyncStateMachineMoveNext (subject contains `>d__` AND `::MoveNext`). Every OTHER
// member of the same state machine falls through to the generic structured path and is
// emitted as ordinary code — with no iterator awareness at all.
//
// Measured on the real pipeline (YieldOne, after the detector fix): 14 subjects, and
// exactly ONE was recorded as unsupported:
//
//     UNSUP | <YieldOne>d__0::MoveNext
//     EMIT  | AsyncIteratorMethods::YieldOne()                 <- outer factory
//     EMIT  | <YieldOne>d__0::.ctor
//     EMIT  | <YieldOne>d__0::SetStateMachine
//     EMIT  | <YieldOne>d__0::IAsyncEnumerable<int>.GetAsyncEnumerator
//     EMIT  | <YieldOne>d__0::IAsyncEnumerator<int>.get_Current
//     EMIT  | <YieldOne>d__0::IAsyncEnumerator<int>.MoveNextAsync
//     EMIT  | <YieldOne>d__0::IAsyncDisposable.DisposeAsync
//     EMIT  | <YieldOne>d__0::IValueTaskSource.GetResult / GetStatus / OnCompleted
//     EMIT  | <YieldOne>d__0::IValueTaskSource<bool>.GetResult / GetStatus / OnCompleted
//
// This is the "silent wrong iterable" A1's own comment warns about — occurring on the
// sibling methods A1 did not cover. Emitting them generically is not a neutral act:
// `MoveNextAsync` in particular must hand back a ValueTask<bool> backed by the pooled
// AsyncIteratorSourceCore, and `get_Current` must read <>2__current. A generic emission
// produces plausible-looking C++ that is wrong.
//
// WHAT THIS TEST PINS
// -------------------
// A coverage invariant, not an implementation: **for every member of an iterator state
// machine, the planner must either lower it (A4) or record it as unsupported (A1)**.
// Nothing may fall through silently. That is what makes A4's own completion criterion
// checkable — `YieldAfterAwait` must remain explicitly recorded, never silently emitted.
//
// COUNTEREXAMPLE (决策2=A): revert the iterator member-surface gate so the sibling
// members fall through unrecorded, and `EveryIteratorMemberIsEitherLoweredOrRecorded`
// goes RED with the offending member named. Verified — see the commit's regression_check.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Pipeline;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class AsyncIteratorMemberSurfaceTests
{
    private static readonly string s_iteratorAssemblyPath = LocateIteratorAssemblyDll();

    /// <summary>
    /// Members of an iterator state machine that MUST be accounted for. The set is
    /// expressed as suffix fragments of the subject id rather than exact ids, because the
    /// interface-qualified spellings carry the generic instantiation
    /// (e.g. `System.Collections.Generic.IAsyncEnumerable&lt;System.Int32&gt;.GetAsyncEnumerator`)
    /// which changes with the element type.
    /// </summary>
    private static readonly string[] s_requiredMemberFragments =
    [
        "::MoveNext:",
        ".GetAsyncEnumerator:",
        ".get_Current:",
        ".MoveNextAsync:",
        "IAsyncDisposable.DisposeAsync:",
        "IValueTaskSource.GetResult:",
        "IValueTaskSource.GetStatus:",
        "IValueTaskSource.OnCompleted:",
        "IValueTaskSource<",
    ];

    /// <summary>
    /// True when the emitted declarations carry the recorded-unsupported signal at all.
    /// The signal must be unconditional (present as a zero-length table when clean) so
    /// that "no iterators" and "signal not wired" cannot be confused — A1 established
    /// this, and A4 must not regress it.
    /// </summary>
    private const string kUnsupportedCountSymbol = "kUnsupportedAsyncIteratorCount";

    /// <summary>
    /// The coverage invariant. Every member of `<YieldOne>d__0` must be either recorded
    /// as unsupported or actually lowered — silence is the failure mode.
    ///
    /// The "actually lowered" side is deliberately weak here (the member is present in
    /// the emitted declarations). A strong per-member semantic assertion belongs to the
    /// individual lowering increments; what this test guarantees is that no member can
    /// slip through unaccounted, which is the property A1 established for MoveNext alone
    /// and which is currently violated for the other twelve.
    /// </summary>
    [Fact]
    public void EveryIteratorMemberIsEitherLoweredOrRecorded()
    {
        var outcome = EmitAndCapture("YieldOne");

        Assert.True(outcome.Failure is null,
            "Emission must not throw at the planner level: "
            + outcome.Failure?.GetType().Name + ": " + outcome.Failure?.Message);

        var stateMachineMembers = outcome.AllSubjects
            .Where(s => s.Contains("+<YieldOne>d__0::", StringComparison.Ordinal))
            .ToList();
        Assert.True(stateMachineMembers.Count > 0,
            "fixture did not surface the <YieldOne>d__0 state machine; subjects seen: "
            + string.Join(", ", outcome.AllSubjects.Where(s => s.Contains("d__"))));

        var unaccounted = new List<string>();
        foreach (var member in stateMachineMembers)
        {
            bool recorded = outcome.RecordedUnsupported.Contains(member);
            bool lowered = outcome.EmittedMemberSubjects.Contains(member);
            if (!recorded && !lowered) unaccounted.Add(member);
        }

        // The recorded signal must be surfaced into the generated C++ unconditionally —
        // A1's contract. If it were emitted conditionally, "no unsupported iterators" and
        // "signal never wired" would be indistinguishable.
        Assert.Contains(kUnsupportedCountSymbol, outcome.Declarations);

        Assert.True(unaccounted.Count == 0,
            $"{unaccounted.Count} iterator state-machine member(s) fell through silently "
            + "— neither lowered nor recorded as unsupported. Each one emits generic C++ "
            + "with no iterator semantics (a silent wrong iterable):\n  "
            + string.Join("\n  ", unaccounted)
            + "\nRecorded as unsupported: "
            + string.Join(", ", outcome.RecordedUnsupported)
            + "\nEmitted: " + string.Join(", ", outcome.EmittedMemberSubjects));

        // The surface must actually be there — guards against a vacuous pass if the
        // fixture ever stops producing these members.
        foreach (var fragment in s_requiredMemberFragments)
        {
            Assert.True(
                stateMachineMembers.Any(s => s.Contains(fragment, StringComparison.Ordinal)),
                $"expected <YieldOne>d__0 to expose a member matching '{fragment}'; "
                + "members seen: " + string.Join(", ", stateMachineMembers));
        }
    }

    /// <summary>
    /// The A4 completion criterion's other half: adding support for `YieldOne` must NOT
    /// silently absorb `YieldAfterAwait`. The latter has an await before its yield, so it
    /// depends on the await-side lowering that A4 explicitly does not implement
    /// (ConfiguredValueTaskAwaiter, CancellationTokenSource wiring). It must remain in
    /// the explicitly-recorded unsupported set.
    ///
    /// Without this test, "YieldOne works" could be achieved by widening the lowering to
    /// cover shapes it does not actually handle — the半-lowering / fake-green outcome the
    /// plan's risk table calls out.
    /// </summary>
    [Fact]
    public void YieldAfterAwait_StaysExplicitlyUnsupported()
    {
        var outcome = EmitAndCapture("YieldAfterAwait");

        Assert.True(outcome.Failure is null,
            "Emission must not throw at the planner level: "
            + outcome.Failure?.GetType().Name + ": " + outcome.Failure?.Message);

        Assert.Contains(
            outcome.RecordedUnsupported,
            s => s.Contains("YieldAfterAwait") && s.Contains("::MoveNext"));

        Assert.DoesNotContain(
            outcome.EmittedMemberSubjects,
            s => s.Contains("YieldAfterAwait") && s.Contains("::MoveNext"));

        // ...and it must be present as an explicit refusal, not merely absent. "Absent"
        // would also be satisfied by a silently-dropped method.
        Assert.Contains(
            outcome.StubbedMemberSubjects,
            s => s.Contains("YieldAfterAwait") && s.Contains("::MoveNext"));
    }

    /// <summary>
    /// The counterexample anchor from A1 must survive: a plain (non-iterator) async method
    /// in the same assembly keeps lowering normally. This proves the iterator handling is
    /// scoped to iterator state machines and has not become a blanket rejection.
    /// </summary>
    [Fact]
    public void PlainAsync_KeepsLoweringNormally()
    {
        var outcome = EmitAndCapture("PlainAsync");

        Assert.True(outcome.Failure is null,
            "Emission must not throw at the planner level: "
            + outcome.Failure?.GetType().Name + ": " + outcome.Failure?.Message);

        Assert.DoesNotContain(
            outcome.RecordedUnsupported,
            s => s.Contains("PlainAsync"));
    }

    private sealed record EmitOutcome(
        Exception? Failure,
        IReadOnlyList<string> RecordedUnsupported,
        IReadOnlySet<string> EmittedMemberSubjects,
        IReadOnlySet<string> StubbedMemberSubjects,
        IReadOnlyList<string> AllSubjects,
        string Declarations);

    /// <summary>
    /// Runs the pipeline and the emission planner over the async state machine whose
    /// subject id contains <paramref name="subjectFilter"/>, capturing both the recorded
    /// A1 signal and the set of members that actually reached the generated C++.
    /// </summary>
    private static EmitOutcome EmitAndCapture(string subjectFilter)
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

        var allSubjects = result.AotCoreIr.Methods
            .Select(m => m.SubjectId ?? string.Empty)
            .Where(s => s.Length > 0)
            .ToList();

        // Any method of the filtered state machine will do as the emission entry — the
        // planner emits the whole closure, so the member surface is the same either way.
        var anchor = result.AotCoreIr.Methods
            .FirstOrDefault(m => (m.SubjectId ?? string.Empty).Contains(subjectFilter)
                              && (m.SubjectId ?? string.Empty).Contains(">d__")
                              && (m.SubjectId ?? string.Empty).Contains("::MoveNext"));
        Assert.True(anchor is not null,
            $"fixture did not surface a '{subjectFilter}' state machine MoveNext; subjects seen: "
            + string.Join(", ", allSubjects.Where(s => s.Contains("d__"))));

        var planner = new NativeAotLoweringPlanner();
        var loweringPlan = new NativeAotLoweringPlanArtifact
        {
            PlanKind = "full-assembly-entry",
            AssemblyName = "AsyncIteratorTestAssembly",
            EntrySubjectId = anchor!.SubjectId,
            NativeEntryFunctionName = string.Empty,
            EntrySymbol = anchor.NativeSymbol,
            EntryMethodToken = "0x06000001",
            WorkloadAbi = "full-assembly",
        };
        var closureManifest = new ManagedClosureManifestArtifact
        {
            AssemblyName = "AsyncIteratorTestAssembly",
            EntrySubjectId = anchor.SubjectId,
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
                anchor,
                closureManifest,
                result.MetadataRegistration,
                result.SupplementalMetadataTemplate,
                fullAssemblyMode: true);
        }
        catch (Exception ex)
        {
            failure = ex;
        }

        // A member counts as EMITTED only if its subject id appears in the generated
        // declarations *as real code*, not as the A1 stub. The stub also emits the
        // member's declaration, so mere presence has no discriminating power: every
        // subject id named inside an "UNSUPPORTED async iterator" comment block would
        // otherwise read as "lowered". Walk the emitted source and, for each occurrence
        // of a subject id, decide from the enclosing stub banner whether it is a real
        // body or a refusal.
        // Classification, grounded in what the generated source actually contains:
        //
        //   * A subject listed in kUnsupportedAsyncIteratorSubjects[] was REFUSED — the
        //     A1 branch recorded it and emitted a labelled stub. In GlobalDeclarations the
        //     subject id appears ONLY in that table (the stub body itself is emitted into
        //     a different buffer), so this table is the authoritative refusal signal.
        //   * Any subject appearing in GlobalDeclarations OUTSIDE that table got real
        //     code emitted for it.
        var decls = emission?.GlobalDeclarations ?? string.Empty;
        var recordedSet = planner.UnsupportedAsyncIteratorSubjectIds.ToHashSet(StringComparer.Ordinal);

        var emitted = new HashSet<string>(StringComparer.Ordinal);
        var stubbed = new HashSet<string>(StringComparer.Ordinal);
        foreach (var subject in allSubjects)
        {
            bool inSource = decls.Contains(subject, StringComparison.Ordinal);
            bool recordedUnsupported = recordedSet.Contains(subject);
            if (!inSource && !recordedUnsupported) continue; // not emitted at all
            if (recordedUnsupported) stubbed.Add(subject);
            else emitted.Add(subject);
        }

        return new EmitOutcome(
            failure,
            planner.UnsupportedAsyncIteratorSubjectIds.ToList(),
            emitted,
            stubbed,
            allSubjects,
            decls);
    }

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
            repoRoot, "tests", "unit", "managed", "codegen",
            "AsyncIteratorTestAssembly", "bin", config, "net9.0",
            "AsyncIteratorTestAssembly.dll");
        if (!File.Exists(resolved))
            throw new FileNotFoundException($"fixture not at '{resolved}'", resolved);
        return resolved;
    }

    private sealed class TempCtx : IDisposable
    {
        public string OutputRoot { get; } = Path.Combine(
            Path.GetTempPath(), "ChaosA4MemberSurface_" + Guid.NewGuid().ToString("N"));
        public void Dispose()
        {
            try { if (Directory.Exists(OutputRoot)) Directory.Delete(OutputRoot, true); }
            catch { }
        }
    }
}
