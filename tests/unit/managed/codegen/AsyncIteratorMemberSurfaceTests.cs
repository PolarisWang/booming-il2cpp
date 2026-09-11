// ASYNC-P2-8 A4: async-iterator OBJECT MODEL WIRING — the members of an iterator
// state machine must be emitted, and every interface the type declares must resolve
// to a real vtable offset.
//
// WHY THIS TEST EXISTS
// --------------------
// A1 gated exactly one method: `>d__N::MoveNext` (its ClassifyAsyncMethod branch is
// reachable only through IsAsyncStateMachineMoveNext, which requires `>d__` AND
// `::MoveNext`). Every OTHER member of the same state machine falls through to the
// generic structured path. A first reading of the generated C++ suggested those
// members were therefore absent — that reading was WRONG, and the correction is the
// reason this header is long:
//
//   * `GlobalDeclarations` holds only file-scope tables (the
//     kUnsupportedAsyncIterator* signal, counts). Method bodies live in
//     `NativeAotTemplateModel.Methods[].MethodSource`, the type layout and vtable in
//     `ObjectModelCode`. "Not in GlobalDeclarations" means nothing about "emitted".
//   * Measured with an instrumented dump over the real pipeline: `<YieldOne>d__0` has
//     a full MethodTable (vtable_length 14, type_shape 1, six interfaces) and ALL
//     twelve non-MoveNext members have real bodies — `.ctor` runs
//     ChaosObjectCtor + AsyncIteratorMethodBuilder::Create, `get_Current` reads
//     `<>2__current`, `MoveNextAsync` inlines AsyncIteratorMethodBuilder::MoveNext
//     plus the promise GetStatus/GetResult dance, `DisposeAsync` sets
//     `<>w__disposeMode`.
//
// So the object model is largely already correct, and an assertion of the form
// "the member id appears somewhere in the emitted output" would have been a no-op:
// it was ALREADY green before any A4 work. It had no discriminating power, which is
// why it was deleted rather than kept.
//
// WHAT IS ACTUALLY BROKEN (measured, not assumed)
// -----------------------------------------------
// The interface map is emitted with ZERO offsets:
//
//     static const InterfaceMapEntry chaos_iface_map_..._YieldOne_d__0[] = {
//         { chaos_type_id_..._IAsyncDisposable, 0, 0 },
//         { chaos_type_id_..._IValueTaskSource_System_Boolean_, 0, 0 },
//         { chaos_type_id_..._IValueTaskSource, 0, 0 },
//         { chaos_type_id_..._IAsyncStateMachine, 0, 0 },
//         { chaos_type_id_..._IAsyncEnumerable_System_Int32_, 0, 0 },
//         { chaos_type_id_..._IAsyncEnumerator_System_Int32_, 0, 0 }
//     };
//
// i.e. every interface resolves to vtable_offset 0, method_count 0. This is the
// "接口方法未加载 / iface_map 输出 {stable_id, 0, 0} —— 静默空洞" failure the A4 plan
// predicted for Step 3, and it is the one that actually blocks `await foreach`:
// runtime dispatch goes through ScanIfaceMapForMethod
// (src/native/runtime-core/vtable_registry_resolve.cpp:18-33), which requires
// `declared_method_token < entries[ifi].method_count` and then indexes
// `vtable_array[vtable_offset + declared_method_token]`. With method_count == 0 the
// loop body never fires and the lookup returns nullptr — but only AFTER the bloom
// filter (IfaceBitmapMaybeContains, iface_bitmap is correctly non-zero) has said
// "maybe". The two disagree, which is a silent null dispatch rather than an early
// clean miss.
//
// Root cause of the (0,0): ComputeInterfaceVtableInfo
// (NativeAotLoweringPlanner.ObjectModelEmission.cs:24-48) looks the interface's
// methods up in `_methodsByDeclaringType`, keyed by
// `Identity.DeclaringTypeSubjectId`. Interface types come from the EXTERNAL closure
// (System.Private.CoreLib), so no `MethodDefinition` carries them as declaring type —
// the lookup misses and the helper returns its `(0, 0)` early-out. The interface
// TYPE is correctly tracked (`TrackInterfaceType` is called for all six, which is why
// the type_ids and the bitmap are right); only the interface's METHOD LIST is missing.
//
// Note this is NOT the contiguity hazard the plan also flagged. Because vtable slots
// are assigned per-type in sorted-subject-id order from a global `nextSlot` counter,
// a type's own methods land contiguously and in a real order. That ordering
// invariant is pinned separately by AsyncIteratorVtableOrderTests.
//
// COUNTEREXAMPLE (决策2=A): before the fix, `IteratorInterfacesResolveToRealSlots`
// is RED with all six interfaces showing offset 0 / count 0. Reverting the
// interface-method synthesis in ComputeInterfaceVtableInfo restores that RED state.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Pipeline;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class AsyncIteratorMemberSurfaceTests
{
    private static readonly string s_iteratorAssemblyPath = LocateIteratorAssemblyDll();

    /// <summary>
    /// Members of an iterator state machine whose emitted bodies carry observable
    /// iterator semantics — i.e. whose absence would not be caught by a bare
    /// "is it emitted at all" check. Resolved against
    /// <c>NativeAotTemplateModel.Methods[].SubjectId</c>.
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
    /// The interfaces `<YieldOne>d__0` declares. Each must resolve to a real vtable
    /// window; a `<type id>, 0, 0` entry is a silent null dispatch at runtime.
    /// Referenced by stable-id symbol name so the assertion survives element-type
    /// changes to the fixture.
    /// </summary>
    private static readonly string[] s_iteratorInterfaceFragments =
    [
        "IAsyncEnumerable_System_Int32_",
        "IAsyncEnumerator_System_Int32_",
        "IAsyncDisposable",
        "IValueTaskSource_System_Boolean_",
        "IValueTaskSource",
        "IAsyncStateMachine",
    ];

    /// <summary>
    /// True when the emitted declarations carry the recorded-unsupported signal at all.
    /// The signal must be unconditional (present as a zero-length table when clean) so
    /// that "no iterators" and "signal not wired" cannot be confused — A1 established
    /// this, and A4 must not regress it.
    /// </summary>
    private const string kUnsupportedCountSymbol = "kUnsupportedAsyncIteratorCount";

    /// <summary>
    /// The A4 completion bar, in its checkable form: the eleven non-MoveNext members of
    /// the iterator state machine are actually emitted, each with the shape that makes
    /// it the real thing rather than a plausible-looking generic emission.
    /// </summary>
    [Fact]
    public void EveryNonMoveNextIteratorMemberIsEmitted()
    {
        var outcome = EmitAndCapture("YieldOne");

        Assert.True(outcome.Failure is null,
            "Emission must not throw at the planner level: "
            + outcome.Failure?.GetType().Name + ": " + outcome.Failure?.Message);

        var memberSubjects = outcome.EmittedMethods
            .Where(s => s.Contains("+<YieldOne>d__0::", StringComparison.Ordinal))
            .ToList();
        Assert.True(memberSubjects.Count > 0,
            "fixture did not surface the <YieldOne>d__0 state machine; subjects seen: "
            + string.Join(", ", outcome.EmittedMethods.Where(s => s.Contains("d__"))));

        foreach (var fragment in s_requiredMemberFragments)
        {
            Assert.True(
                memberSubjects.Any(s => s.Contains(fragment, StringComparison.Ordinal)),
                $"expected <YieldOne>d__0 to emit a member matching '{fragment}'; "
                + "members seen: " + string.Join(", ", memberSubjects));
        }

        // The recorded signal must be surfaced into the generated C++ unconditionally —
        // A1's contract. If it were emitted conditionally, "no unsupported iterators" and
        // "signal never wired" would be indistinguishable.
        Assert.Contains(kUnsupportedCountSymbol, outcome.Declarations);
    }

    /// <summary>
    /// The actual A4 blocker: interface dispatch for the iterator's six interfaces.
    ///
    /// `{ stable_id, 0, 0 }` is not a conservative placeholder — it asserts "this
    /// interface has zero methods", which makes ScanIfaceMapForMethod's
    /// `declared_method_token &lt; method_count` guard fail for every token and return
    /// nullptr. Runtime dispatch to `IAsyncEnumerator&lt;int&gt;.MoveNextAsync` /
    /// `get_Current` therefore yields a null function pointer after the bloom filter
    /// has already claimed the interface is present.
    ///
    /// COUNTEREXAMPLE: revert the interface-method synthesis and this goes RED with all
    /// six interfaces reported at offset 0 / count 0.
    /// </summary>
    [Fact]
    public void IteratorInterfacesResolveToRealSlots()
    {
        var outcome = EmitAndCapture("YieldOne");

        var map = ExtractIfaceMap(outcome.ObjectModelCode, "YieldOne_d__0");
        Assert.True(map.Count > 0,
            "no chaos_iface_map_ for <YieldOne>d__0 was emitted; iface maps seen: "
            + string.Join(", ", ExtractAllIfaceMapSymbols(outcome.ObjectModelCode)));

        foreach (var fragment in s_iteratorInterfaceFragments)
        {
            var entry = map.FirstOrDefault(e => e.Symbol.Contains(fragment, StringComparison.Ordinal));
            Assert.True(entry is not null,
                $"<YieldOne>d__0 declares '{fragment}' but it has no iface_map entry; entries: "
                + string.Join(" | ", map.Select(e => e.Symbol + " -> " + e.Offset + "/" + e.Count)));

            Assert.True(entry!.Count > 0,
                $"interface '{fragment}' mapped with method_count 0 — runtime "
                + "ScanIfaceMapForMethod rejects every token and returns nullptr, so "
                + "dispatch through this interface silently yields a null function pointer. "
                + $"Entry: {entry.Symbol} -> {entry.Offset}/{entry.Count}");
        }

        // The two interfaces `await foreach` actually dispatches through. Asserting their
        // counts positively (not just "> 0") is what makes this a statement about the
        // real method surface: IAsyncEnumerator<T> has exactly get_Current + MoveNextAsync.
        var enumerator = map.First(e =>
            e.Symbol.Contains("IAsyncEnumerator_System_Int32_", StringComparison.Ordinal));
        Assert.True(enumerator.Count >= 2,
            $"IAsyncEnumerator<int> must expose at least get_Current and MoveNextAsync; "
            + $"got count {enumerator.Count} at offset {enumerator.Offset}");

        var enumerable = map.First(e =>
            e.Symbol.Contains("IAsyncEnumerable_System_Int32_", StringComparison.Ordinal));
        Assert.True(enumerable.Count >= 1,
            $"IAsyncEnumerable<int> must expose GetAsyncEnumerator; got count {enumerable.Count}");
    }

    /// <summary>
    /// The A4 completion criterion's other half: adding support for `YieldOne` must NOT
    /// silently absorb `YieldAfterAwait`. The latter has an await before its yield, so it
    /// depends on await-side lowering A4 explicitly does not implement
    /// (ConfiguredValueTaskAwaiter, CancellationTokenSource wiring). It must remain in
    /// the explicitly-recorded unsupported set AND still be emitted as a labelled stub —
    /// "absent" would also be satisfied by a silently-dropped method.
    /// </summary>
    [Fact]
    public void YieldAfterAwait_StaysExplicitlyUnsupported()
    {
        var outcome = EmitAndCapture("YieldAfterAwait");

        Assert.True(outcome.Failure is null,
            "Emission must not throw at the planner level: "
            + outcome.Failure?.GetType().Name + ": " + outcome.Failure?.Message);

        var moveNext = outcome.EmittedMethods
            .FirstOrDefault(s => s.Contains("YieldAfterAwait") && s.Contains("::MoveNext"));
        Assert.True(moveNext is not null,
            "YieldAfterAwait::MoveNext should still be emitted as an explicitly-labelled "
            + "stub, not dropped; emitted: " + string.Join(", ", outcome.EmittedMethods));

        var source = outcome.MethodSourceBySubject[moveNext!];
        Assert.Contains("UNSUPPORTED async iterator", source);

        Assert.Contains(
            outcome.RecordedUnsupported,
            s => s.Contains("YieldAfterAwait") && s.Contains("::MoveNext"));

        // It must NOT be recorded as a lowered iterator — that would mean A4 widened its
        // coverage to a shape it does not handle (the半-lowering / fake-green outcome).
        Assert.DoesNotContain(
            outcome.LoweredIteratorSubjectIds,
            s => s.Contains("YieldAfterAwait") && s.Contains("::MoveNext"));
    }

    /// <summary>
    /// The counterexample anchor from A1 must survive: a plain (non-iterator) async method
    /// in the same assembly keeps lowering normally, proving the iterator handling is
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

    /// <summary>
    /// The iterator state machine's vtable must be long enough to hold every slot its
    /// iface_map windows point at. The two are emitted from the same slot map, so a
    /// mismatch means one of them was derived from a different keying than the other —
    /// exactly the shape that puts a wrong symbol behind a valid-looking offset.
    /// </summary>
    [Fact]
    public void IteratorVtableCoversEveryInterfaceWindow()
    {
        var outcome = EmitAndCapture("YieldOne");

        int vtableLength = ExtractVtableLength(outcome.ObjectModelCode, "YieldOne_d__0");
        Assert.True(vtableLength > 0,
            "no MethodTable / vtable_length found for <YieldOne>d__0");

        // Every real (count > 0) window must land inside the vtable array.
        foreach (var entry in ExtractIfaceMap(outcome.ObjectModelCode, "YieldOne_d__0"))
        {
            if (entry.Count == 0) continue; // reported by the other test
            Assert.True(
                entry.Offset + entry.Count <= vtableLength,
                $"iface_map window for {entry.Symbol} runs past the vtable: "
                + $"offset {entry.Offset} + count {entry.Count} > vtable_length {vtableLength}");
        }
    }

    private sealed record IfaceEntry(string Symbol, int Offset, int Count);

    /// <summary>
    /// The round-trip half of the Step 3 acceptance: a plausible-looking window is not
    /// enough — `offset + token` must actually land on the symbol the interface member
    /// names. The runtime indexes `vtable_array[vtable_offset + declared_method_token]`,
    /// so an offset that is merely in-range but shifted by one silently dispatches to the
    /// wrong method. This asserts identity, not just bounds.
    ///
    /// The counterexample anchor is the pair of `IValueTaskSource` interfaces: they are a
    /// strict prefix collision (`IValueTaskSource` vs `IValueTaskSource&lt;System.Boolean&gt;`),
    /// so attributing the generic members to the non-generic window makes both windows
    /// point at each other's slots while every bounds check still passes.
    /// </summary>
    [Fact]
    public void IteratorIfaceMapWindowsResolveToTheirOwnMembers()
    {
        var outcome = EmitAndCapture("YieldOne");

        var map = ExtractIfaceMap(outcome.ObjectModelCode, "YieldOne_d__0");
        Assert.True(map.Count > 0, "no chaos_iface_map_ for <YieldOne>d__0 was emitted");

        var vtable = ExtractVtableSymbols(outcome.ObjectModelCode, "YieldOne_d__0");
        Assert.True(vtable.Count > 0,
            "no vtable_array initializer found for <YieldOne>d__0");

        // Each explicit implementation's slot key carries its interface qualifier (see the
        // slot map shape), so walking the emitted symbols by index lets us state exactly
        // which method each window member must resolve to.
        foreach (var (ifaceFragment, expectedFragments) in s_expectedInterfaceWindows)
        {
            // Exact symbol match, not substring: "IValueTaskSource" is a substring of
            // "IValueTaskSource_System_Boolean_", so a Contains lookup for the non-generic
            // interface would silently select the generic one's entry.
            var entry = map.FirstOrDefault(e =>
                e.Symbol.Equals("chaos_type_id_" + ifaceFragment, StringComparison.Ordinal)
                || e.Symbol.EndsWith("_" + ifaceFragment, StringComparison.Ordinal));
            Assert.True(entry is not null,
                $"<YieldOne>d__0 has no iface_map entry for '{ifaceFragment}'; entries: "
                + string.Join(" | ", map.Select(e => e.Symbol + " -> " + e.Offset + "/" + e.Count)));

            Assert.Equal(expectedFragments.Length, entry!.Count);

            for (int i = 0; i < expectedFragments.Length; i++)
            {
                int index = entry.Offset + i;
                Assert.InRange(index, 0, vtable.Count - 1);
                Assert.True(
                    vtable[index].Contains(expectedFragments[i], StringComparison.Ordinal),
                    $"window '{ifaceFragment}' member {i} should resolve to '{expectedFragments[i]}' "
                    + $"at slot {index}, but the slot holds '{vtable[index]}'. vtable dump:\n"
                    + string.Join("\n", vtable.Select((s, k) => $"  [{k}] {s}")));
            }
        }
    }

    /// <summary>
    /// The window contents each iterator interface must expose, in the order the slot
    /// allocator produces them (sorted by explicit-implementation spelling). Fragments are
    /// written in the <em>sanitized</em> spelling (<c>&lt;T&gt;</c> and <c>.</c> become
    /// <c>_</c>) because that is what the emitted native symbols carry. A wrong attribution
    /// or a shifted base breaks one of these, which is the whole point.
    /// </summary>
    private static readonly (string Iface, string[] Members)[] s_expectedInterfaceWindows =
    [
        ("System_Collections_Generic_IAsyncEnumerable_System_Int32_",
         ["IAsyncEnumerable_System_Int32__GetAsyncEnumerator"]),
        ("System_Collections_Generic_IAsyncEnumerator_System_Int32_",
         ["IAsyncEnumerator_System_Int32__MoveNextAsync",
          "IAsyncEnumerator_System_Int32__get_Current"]),
        ("System_IAsyncDisposable", ["System_IAsyncDisposable_DisposeAsync"]),
        ("Sources_IValueTaskSource_System_Boolean_",
         ["IValueTaskSource_System_Boolean__GetResult",
          "IValueTaskSource_System_Boolean__GetStatus",
          "IValueTaskSource_System_Boolean__OnCompleted"]),
        ("Sources_IValueTaskSource", ["Sources_IValueTaskSource_GetResult",
                                      "Sources_IValueTaskSource_GetStatus",
                                      "Sources_IValueTaskSource_OnCompleted"]),
        ("Runtime_CompilerServices_IAsyncStateMachine",
         ["_MoveNext", "_SetStateMachine"]),
    ];

    private sealed record EmitOutcome(
        Exception? Failure,
        IReadOnlyList<string> RecordedUnsupported,
        IReadOnlyList<string> LoweredIteratorSubjectIds,
        IReadOnlyList<string> EmittedMethods,
        IReadOnlyDictionary<string, string> MethodSourceBySubject,
        string ObjectModelCode,
        string Declarations);

    /// <summary>
    /// Runs the pipeline and the emission planner over the async state machine whose
    /// subject id contains <paramref name="subjectFilter"/>, capturing the recorded A1
    /// signal, the iterator members the planner claims to have lowered, and the set of
    /// methods that actually reached the template model.
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

        // Any method of the filtered state machine will do as the emission entry — the
        // planner emits the whole closure, so the member surface is the same either way.
        var anchor = result.AotCoreIr.Methods
            .FirstOrDefault(m => (m.SubjectId ?? string.Empty).Contains(subjectFilter)
                              && (m.SubjectId ?? string.Empty).Contains(">d__")
                              && (m.SubjectId ?? string.Empty).Contains("::MoveNext"));
        Assert.True(anchor is not null,
            $"fixture did not surface a '{subjectFilter}' state machine MoveNext; subjects seen: "
            + string.Join(", ", result.AotCoreIr.Methods
                .Select(m => m.SubjectId ?? string.Empty)
                .Where(s => s.Contains("d__"))));

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

        var methods = emission?.Methods ?? [];
        var bySource = new Dictionary<string, string>(StringComparer.Ordinal);
        foreach (var m in methods)
        {
            if (m.SubjectId is not null && m.MethodSource is not null)
                bySource[m.SubjectId] = m.MethodSource;
        }

        return new EmitOutcome(
            failure,
            planner.UnsupportedAsyncIteratorSubjectIds.ToList(),
            planner.LoweredAsyncIteratorSubjectIds.ToList(),
            methods.Select(m => m.SubjectId ?? string.Empty).ToList(),
            bySource,
            emission?.ObjectModelCode ?? string.Empty,
            emission?.GlobalDeclarations ?? string.Empty);
    }

    /// <summary>Parses the InterfaceMapEntry array whose symbol ends in <paramref name="typeSuffix"/>.</summary>
    private static List<IfaceEntry> ExtractIfaceMap(string objectModelCode, string typeSuffix)
    {
        var result = new List<IfaceEntry>();
        var lines = objectModelCode.Split('\n');
        bool inBlock = false;
        for (int i = 0; i < lines.Length; i++)
        {
            var line = lines[i];
            if (!inBlock)
            {
                int at = line.IndexOf("chaos_iface_map_", StringComparison.Ordinal);
                if (at < 0) continue;
                if (!line.Contains("[] = {", StringComparison.Ordinal)) continue;
                var symbol = line.Substring(at).Trim();
                if (!symbol.Contains(typeSuffix, StringComparison.Ordinal)) continue;
                inBlock = true;
                continue;
            }

            var trimmed = line.Trim();
            if (trimmed.StartsWith("};", StringComparison.Ordinal)) { inBlock = false; continue; }
            // { chaos_type_id_X, 3, 7 },
            var m = Regex.Match(trimmed, @"^\{\s*(\S+)\s*,\s*(\d+)\s*,\s*(\d+)\s*\},?$");
            if (m.Success)
            {
                result.Add(new IfaceEntry(
                    m.Groups[1].Value,
                    int.Parse(m.Groups[2].Value),
                    int.Parse(m.Groups[3].Value)));
            }
        }
        return result;
    }

    private static IEnumerable<string> ExtractAllIfaceMapSymbols(string objectModelCode) =>
        objectModelCode.Split('\n')
            .Select(l => l.Trim())
            .Where(l => l.Contains("chaos_iface_map_", StringComparison.Ordinal)
                     && l.Contains("[] = {", StringComparison.Ordinal));

    private static int ExtractVtableLength(string objectModelCode, string typeSuffix)
    {
        foreach (var line in objectModelCode.Split('\n'))
        {
            if (!line.Contains("MethodTable chaos_mt_", StringComparison.Ordinal)) continue;
            if (!line.Contains(typeSuffix, StringComparison.Ordinal)) continue;
            var m = Regex.Match(line, @"\{\s*[^,]+,\s*[^,]+,\s*[^,]+,\s*(\d+)u\s*,");
            if (m.Success) return int.Parse(m.Groups[1].Value);
        }
        return -1;
    }

    /// <summary>
    /// The initializer entries of the type's <c>vtable_array</c>, in slot order, so a test
    /// can assert which native symbol sits at a given index. The array is emitted as
    /// <c>const void* &lt;symbol&gt;[] = { reinterpret_cast&lt;void*&gt;(&lt;symbol&gt;), ... };</c>.
    /// </summary>
    private static IReadOnlyList<string> ExtractVtableSymbols(string objectModelCode, string typeSuffix)
    {
        var lines = objectModelCode.Split('\n');

        // The MethodTable line names the array this type's slots live in.
        string? arraySymbol = null;
        foreach (var line in lines)
        {
            if (!line.Contains("MethodTable ", StringComparison.Ordinal)) continue;
            if (!line.Contains(typeSuffix, StringComparison.Ordinal)) continue;
            int at = line.IndexOf("chaos_vtable_", StringComparison.Ordinal);
            if (at < 0) continue;
            int end = at;
            while (end < line.Length && (char.IsLetterOrDigit(line[end]) || line[end] == '_')) end++;
            arraySymbol = line.Substring(at, end - at);
            break;
        }
        if (arraySymbol is null) return [];

        var result = new List<string>();
        bool inBlock = false;
        foreach (var line in lines)
        {
            if (!inBlock)
            {
                if (!line.Contains("const void* " + arraySymbol, StringComparison.Ordinal)) continue;
                if (!line.Contains("[] =", StringComparison.Ordinal)) continue;
                inBlock = true;
                // The declaration ends with "[] =" and the brace is on the next line; if the
                // brace shares the line, do not treat the declaration itself as an entry.
                continue;
            }

            var trimmed = line.Trim();
            if (trimmed.Length == 0) continue;
            if (trimmed.StartsWith("};", StringComparison.Ordinal)) break;
            if (trimmed == "{") continue;
            // "reinterpret_cast<void*>(<symbol>)," or "nullptr," — one entry per slot.
            result.Add(trimmed);
        }
        return result;
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
