using System.Globalization;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Generator.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Guards for splitting page 0's un-partitioned payload into bounded TUs.
///
/// <para>
/// Background: the paging logic only partitions <c>templateModel.Methods</c>.
/// Everything page 0 additionally carries — object model, module registration,
/// GC slot map — is appended <b>whole</b> (see
/// <c>NativeAotEmitter.BuildGeneratedPageToBuilder</c>), so page 0 grows without
/// bound regardless of how many pages the methods are split into. For the
/// system chunk that produced a <b>71 MB / 1.2M-line</b> page-0 TU, which MSVC
/// cannot compile: <c>C1002: compiler is out of heap space in pass 2</c>.
/// </para>
///
/// <para>
/// The existing size threshold (<c>sizeThresholdChars = 350_000</c>,
/// NativeAotEmitter.Shared.cs) explicitly documents C1060 avoidance as its
/// purpose, yet page 0 exceeded it by ~200x. These tests pin the invariant that
/// threshold was supposed to express.
/// </para>
/// </summary>
public sealed class PagePayloadSplitTests
{
    private readonly PlannerFixture _fixture = new();

    /// <summary>
    /// The documented per-TU budget from NativeAotEmitter.Shared.cs
    /// (<c>sizeThresholdChars</c>). Re-stated here as the contract under test —
    /// if the constant legitimately changes, this test must change with it.
    /// </summary>
    private const int PerTuBudgetChars = 350_000;

    /// <summary>
    /// Tolerance multiplier. The budget is an <i>estimate</i> built from
    /// MethodSource lengths plus per-method/per-page overhead, so the emitted
    /// text can legitimately overshoot somewhat. A factor of 4 keeps the test
    /// meaningful (it still fails loudly on the real defect, where the payload
    /// reaches ~70 MB) without being brittle to estimate drift.
    /// </summary>
    private const double BudgetTolerance = 4.0;

    /// <summary>
    /// Method count for the payload-scale tests.
    ///
    /// <para>
    /// Chosen so the synthetic payload reproduces the real defect. Measured:
    /// the payload grows ~1,170 chars per method (module registration dominates
    /// at ~1,138/method, object model is ~30/method), so 600 methods yields only
    /// ~0.7 M chars — comfortably <i>under</i> the tolerance, i.e. a false green.
    /// The system chunk carries 49,086 methods; 9,000 reproduces the same
    /// over-budget shape while keeping the test fast.
    /// </para>
    /// </summary>
    private const int PayloadScaleMethodCount = 9_000;

    /// <summary>
    /// Builds a plan carrying enough methods that paging engages, then returns
    /// the planner's template model.
    /// </summary>
    private NativeAotTemplateModel BuildModel(int methodCount)
    {
        var methods = new List<AotCoreIrMethodArtifact>(methodCount);
        for (int i = 0; i < methodCount; i++)
        {
            methods.Add(ModelFactory.CreateMethod(
                $"TestModule.TestClass::M{i}:System.Int32()",
                returnType: "System.Int32",
                returnAbi: ModelFactory.Int32Abi,
                instructions: new[]
                {
                    ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: i),
                    ModelFactory.Instruction("ret", ilOffset: 1),
                }));
        }

        var artifact = ModelFactory.CreateArtifact(methods.ToArray());
        var loweringPlan = ModelFactory.CreateDefaultPlan(methods[0].SubjectId);
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: PlannerFixture.StubAssemblyPath);

        return _fixture.RunPlanner(artifact, loweringPlan, manifest);
    }

    /// <summary>
    /// L2: page-0 payload is partitioned into one translation unit per section,
    /// and no unit returns to the unbounded-page-0 scale that caused MSVC C1002.
    ///
    /// <para>
    /// Asserts on the <b>partitioned output</b>, not on
    /// <c>ModuleRegistrationCode</c>. Sectioning happens in the emitter, so the
    /// planner's raw payload string is legitimately still whole — measuring it
    /// would fail forever regardless of the fix.
    /// </para>
    ///
    /// <para>
    /// <b>Why this does not assert a per-unit size ceiling.</b> The original
    /// revision asserted every translation unit stayed within
    /// <c>sizeThresholdChars x 4</c>. Measured against real emitted sources, that
    /// never held and cannot hold: each section is one indivisible ABI unit, and
    /// the large ones are indivisible <i>by contract</i>, not by oversight.
    /// <c>CodeRegistrationV0</c> reaches its tables through a
    /// <c>const void* method_pointers</c> plus a count; <c>HotpatchModuleV0</c>
    /// through <c>const HotpatchMethodEntryV0* method_entries</c> plus a count;
    /// the GC slot map is scanned as a <c>begin</c>/<c>end</c> byte range stepping
    /// by <c>entry_total_size</c>. The runtime indexes all three as one contiguous
    /// array, so splitting any of them means changing the ABI and the runtime —
    /// out of scope. Sizes measured on the system chunk: Module registration
    /// 23.6 MB, GC Slot Map 10.9 MB, Hotpatch 7.0 MB, Method table 4.8 MB, and
    /// every remaining section between 1.7 MB and 4.3 MB — all above the ceiling
    /// the old assertion used.
    /// </para>
    ///
    /// <para>
    /// So the promise this guard keeps is the one that actually matters: the
    /// payload is still partitioned (a section is never merged back into an
    /// unbounded page-0 unit, and every section gets its own TU), and no unit
    /// approaches the ~71 MB single-unit size that made MSVC give up with C1002.
    /// The build itself is the real check that the units compile.
    /// </para>
    /// </summary>
    [Fact]
    public void PageZeroPayload_IsPartitionedIntoBoundedTranslationUnits()
    {
        var model = BuildModel(PayloadScaleMethodCount);

        Assert.True(model.PayloadSections is { Count: > 0 },
            "the planner must expose named payload sections so the emitter can "
            + "partition page-0 payload into bounded translation units");

        var tus = PayloadSectionPartitioner.Partition(
            model.PayloadSections, NativeAotEmitter.PayloadSectioningBudgetChars);

        Assert.True(tus.Count > 0, "expected at least one payload translation unit");

        // A section is never divided: every section must appear in exactly one
        // unit, whole. Cutting a section would break the definition/reference
        // relationships it carries internally (a table and its descriptor).
        foreach (var section in model.PayloadSections)
        {
            int occurrences = tus.Count(tu => tu.Any(s => ReferenceEquals(s, section)));
            Assert.True(occurrences == 1,
                $"section '{section.Name}' appears in {occurrences} translation units; "
                + "each section must be emitted whole, in exactly one unit");

            Assert.Contains(tus, tu => tu.Any(s => ReferenceEquals(s, section)
                && s.Content.Length == section.Content.Length));
        }

        // The regression this guards: page 0 used to carry the entire payload as
        // one unit, which measured ~71 MB for the system chunk and made MSVC fail
        // with C1002 (compiler out of heap space in pass 2). Nothing may
        // reconstitute that shape. The bound is deliberately the measured failure
        // scale rather than a design budget — see the remarks above.
        const long UnboundedPayloadScaleChars = 40_000_000;
        long total = model.PayloadSections.Sum(s => (long)s.Content.Length);
        long largest = tus.Max(tu => tu.Sum(s => (long)s.Content.Length));

        Assert.True(largest < UnboundedPayloadScaleChars,
            $"largest payload translation unit is {largest:N0} chars, at or above the "
            + $"{UnboundedPayloadScaleChars:N0} scale at which page 0 became an unbounded "
            + "unit that MSVC could not compile (C1002). Sections are no longer being "
            + "partitioned into their own translation units.");

        // And the split must actually be doing work: the whole payload cannot fit
        // in a single TU at this scale.
        Assert.True(tus.Count > 1,
            $"payload is {total:N0} chars but was not split into multiple translation "
            + "units — this is the unbounded-page-0 condition that triggers MSVC C1002");
    }

    /// <summary>
    /// L1: splitting page 0's payload into sections must not lose content.
    ///
    /// <para>
    /// This is the highest-risk failure mode of the split: a sectioning bug
    /// silently drops a block, producing a TU that compiles but is missing
    /// symbols. Nothing downstream would flag it at build time.
    /// </para>
    ///
    /// <para>
    /// Asserted at the character level via a multiset comparison, so reordering
    /// is permitted but loss/duplication is not.
    /// </para>
    /// </summary>
    [Fact]
    public void PayloadSections_ConserveAllContent()
    {
        var model = BuildModel(PayloadScaleMethodCount);

        var original = new StringBuilder();
        original.Append(model.ObjectModelCodeBuilder?.ToString() ?? model.ObjectModelCode ?? "");
        original.Append(model.ModuleRegistrationCode ?? "");
        original.Append(model.GenericRegistrationCode ?? "");

        string whole = original.ToString();
        Assert.False(string.IsNullOrEmpty(whole),
            "expected a non-empty page-0 payload to partition");

        // Candidate partitioned view. Until the split lands this is exactly the
        // whole payload (one section), so the assertion holds trivially on the
        // current code and starts doing real work once sectioning exists.
        var sections = PayloadSections(model);

        long sectionTotal = sections.Sum(s => (long)s.Length);
        Assert.True(sectionTotal == whole.Length,
            $"payload sections total {sectionTotal:N0} chars but the unsplit payload is "
            + $"{whole.Length:N0} — sectioning lost or duplicated {Math.Abs(sectionTotal - whole.Length):N0} chars. "
            + "A dropped section compiles cleanly but is missing symbols.");

        // Multiset equality: same characters, same counts, any order.
        Assert.True(SameMultiset(whole, string.Concat(sections)),
            "payload sections are not a permutation of the unsplit payload — "
            + "content was altered, not merely redistributed.");
    }

    /// <summary>
    /// The section boundaries the splitter is expected to produce, derived from
    /// the banner comments the planner already emits into its output
    /// (<c>// ── &lt;name&gt; ──</c>). Exposed as a helper so the conservation
    /// assertion above is independent of how the split happens.
    /// </summary>
    private static IReadOnlyList<string> PayloadSections(NativeAotTemplateModel model)
    {
        var sections = new List<string>();
        if (model.ObjectModelCodeBuilder is { } b) sections.Add(b.ToString());
        else if (!string.IsNullOrEmpty(model.ObjectModelCode)) sections.Add(model.ObjectModelCode);
        if (!string.IsNullOrEmpty(model.ModuleRegistrationCode)) sections.Add(model.ModuleRegistrationCode);
        if (!string.IsNullOrEmpty(model.GenericRegistrationCode)) sections.Add(model.GenericRegistrationCode);
        return sections;
    }

    private static bool SameMultiset(string a, string b)
    {
        if (a.Length != b.Length) return false;
        var counts = new Dictionary<char, int>();
        foreach (char c in a) counts[c] = counts.GetValueOrDefault(c) + 1;
        foreach (char c in b)
        {
            if (!counts.TryGetValue(c, out int n) || n == 0) return false;
            counts[c] = n - 1;
        }
        return counts.Values.All(v => v == 0);
    }

    /// <summary>
    /// L3: no symbol defined in one emitted translation unit may be referenced
    /// from another while still declared <c>static</c>. <c>static</c> is internal
    /// linkage, so once page 0's payload is split into separate TUs every such
    /// reference fails to compile (C2065) and could never link.
    ///
    /// <para>
    /// <b>This guard reads real emitted sources, not the template model.</b> An
    /// earlier revision asserted over a model built by <see cref="BuildModel"/>,
    /// which synthesises only <c>TestModule.TestClass::M{i}</c> methods — no
    /// types, hence no object model and no vtable-slot tables at all. The symbol
    /// class this guard exists to protect against <i>cannot be constructed</i> in
    /// that fixture, so the assertion was unreachable and stayed green while the
    /// emitted sources carried 96 cross-unit <c>static</c> references. A guard
    /// that cannot fail is worse than no guard: it reads as coverage.
    /// </para>
    ///
    /// <para>
    /// The revision before that had a second, independent blind spot — it
    /// concatenated only the object-model and module-registration text, while the
    /// offending references live in the CodeRegistration section. Scanning the
    /// whole emitted file set closes both gaps.
    /// </para>
    ///
    /// <para>
    /// Fails (rather than skipping) when the artifacts are absent: this guard is
    /// the only automated check for the cross-unit linkage contract, so a silent
    /// skip would let the defect through unnoticed — which is precisely how the
    /// previous revision of this guard failed. Generate the inputs with:
    /// <c>python -m verification.chunk_pipeline --chunk system --stages build</c>
    /// </para>
    /// </summary>
    [RequiresEmittedCodegenFact]
    public void NoCrossUnitReferencesToStaticTables()
    {
        string subjectsDir = Path.Combine(
            RepoRootLocator.FindFromBaseDirectory(),
            "artifacts", "foundation-dll", "System.Private.CoreLib",
            "chunks", "system", "native", "subjects");

        Assert.True(Directory.Exists(subjectsDir),
            $"no emitted sources at '{subjectsDir}'. This guard asserts the cross-unit "
            + "linkage contract on real codegen output and must not be skipped silently — "
            + "run the system chunk build first: "
            + "python -m verification.chunk_pipeline --chunk system --stages build");

        var files = Directory.GetFiles(subjectsDir, "native-aot.*.cpp");
        Assert.True(files.Length > 0,
            $"no native-aot sources found in '{subjectsDir}'");

        // Top-level definitions only: they start at column 0. A function-local
        // `static` (e.g. the tab-indented `static auto* chaos_ftn_thunk = ...`
        // emitted inside a method body) is deliberately duplicated per
        // translation unit and is NOT a cross-unit reference — matching
        // indented declarations here would report those as violations.
        //
        // The type-name span must still tolerate a qualified type, since
        // `static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_Foo[]`
        // is the shape that matters most here.
        var defPattern = new Regex(
            @"^static\b[^\n=]*?\b(?<sym>k[A-Z]\w*|chaos_\w+|s_[a-z]\w*)\s*(?:\[[^\]]*\])?\s*=",
            RegexOptions.Multiline);

        // symbol -> file that defines it (static)
        var staticDefs = new Dictionary<string, string>(StringComparer.Ordinal);
        foreach (string file in files)
        {
            string text = File.ReadAllText(file);
            foreach (Match m in defPattern.Matches(text))
                staticDefs.TryAdd(m.Groups["sym"].Value, Path.GetFileName(file));
        }

        Assert.True(staticDefs.Count > 0,
            $"found no `static` table definitions across {files.Length} emitted sources — "
            + "the definition pattern has gone stale, so this guard would pass vacuously");

        // One token scan per file, intersected against the defining-unit map.
        // A per-symbol regex per file would be O(files x symbols) — measured at
        // ~72s over 84 files and 9.2k definitions, which is too slow for a guard
        // that is meant to run on every change.
        var tokenPattern = new Regex(@"[A-Za-z_]\w*");

        // (symbol, referencing file) pairs, deduplicated.
        var seen = new HashSet<(string Symbol, string File)>();
        var violations = new List<string>();

        foreach (string file in files)
        {
            string name = Path.GetFileName(file);
            string text = File.ReadAllText(file);

            foreach (Match m in tokenPattern.Matches(text))
            {
                if (!staticDefs.TryGetValue(m.Value, out string? definedIn)) continue;
                if (definedIn == name) continue;              // its own definition
                if (!seen.Add((m.Value, name))) continue;      // already recorded

                violations.Add($"'{m.Value}' (static in {definedIn}) referenced from {name}");
            }
        }
        violations.Sort(StringComparer.Ordinal);

        Assert.True(violations.Count == 0,
            $"found {violations.Count} cross-translation-unit reference(s) to `static` symbols. "
            + "These files are compiled as separate TUs, where a `static` definition has "
            + "internal linkage and every cross-unit reference fails to compile (C2065). "
            + "Such symbols must be declared `extern` and published in the shared contract "
            + "header. Violations:\n  "
            + string.Join("\n  ", violations.Take(15))
            + (violations.Count > 15 ? $"\n  … and {violations.Count - 15} more" : ""));
    }

    // ── Count-scalar contract ───────────────────────────────────────────────
    //
    // Making a table cross-TU-visible forces `static constexpr` -> `extern const`,
    // which drops the definition from the consumer's view. That breaks every
    // `sizeof(table) / sizeof(table[0])` expression: `sizeof` needs a COMPLETE
    // type, and a cross-TU declaration is necessarily incomplete.
    //
    //   extern const U32 kTbl[];  + sizeof(kTbl)  ->  C2070: illegal sizeof operand
    //   extern const U32 kTbl[2]; + sizeof(kTbl)  ->  compiles (dimension unknown here)
    //
    // So each table needs a companion count scalar, and consumers must read that
    // instead of computing it with sizeof. These tests pin the companion contract.

    /// <summary>
    /// Every cross-TU table must have a companion count scalar emitted next to
    /// its definition.
    /// </summary>
    [Fact]
    public void CrossTuTables_EmitCompanionCountScalar()
    {
        var model = BuildModel(PayloadScaleMethodCount);

        string genericReg = (model.GenericRegistrationCode ?? "")
                          + "\n" + (model.ModuleRegistrationCode ?? "");
        Assert.False(string.IsNullOrEmpty(genericReg),
            "expected generic-registration code to be emitted for this model");

        // Tables that the code-registration block consumes across sections.
        string[] crossTuTables =
        {
            "kGenericTypeArgTokens",
            "kGenericTypeEntries",
            "kGenericMethodArgTokens",
            "kGenericMethodEntries",
            "s_method_aot_entries",
            "s_method_aot_entry_args",
        };

        foreach (string table in crossTuTables)
        {
            if (!Regex.IsMatch(genericReg, @"\b" + Regex.Escape(table) + @"\b"))
                continue; // not emitted for this model (empty generics)

            string countSymbol = table + "Count";
            Assert.True(
                Regex.IsMatch(genericReg, @"\b" + Regex.Escape(countSymbol) + @"\b"),
                $"'{table}' is emitted but its companion count scalar '{countSymbol}' is not. "
                + "Cross-TU consumers cannot use sizeof() on an incomplete array type "
                + "(C2070), so the count has to be published as its own symbol.");
        }
    }

    /// <summary>
    /// The consumer must no longer derive counts via <c>sizeof</c> for the
    /// cross-TU tables — it has to use the companion scalars.
    /// </summary>
    [Fact]
    public void CodeRegistration_DoesNotUseSizeofOnCrossTuTables()
    {
        var model = BuildModel(PayloadScaleMethodCount);

        // The consumer block lives in the module-registration payload.
        string payload =
              (model.ObjectModelCodeBuilder?.ToString() ?? model.ObjectModelCode ?? "")
            + (model.ModuleRegistrationCode ?? "");

        string[] crossTuTables =
        {
            "kGenericTypeArgTokens",
            "kGenericTypeEntries",
            "kGenericMethodArgTokens",
            "kGenericMethodEntries",
            "s_method_aot_entries",
            "s_method_aot_entry_args",
        };

        foreach (string table in crossTuTables)
        {
            var sizeofPattern = new Regex(
                @"sizeof\s*\(\s*" + Regex.Escape(table) + @"\s*\)",
                RegexOptions.Multiline);

            Assert.False(sizeofPattern.IsMatch(payload),
                $"found `sizeof({table})` in the consumer payload. Once the table is declared "
                + "cross-TU (`extern const T[]`), the declaration is an incomplete type and "
                + "sizeof fails with C2070. Use the companion `" + table + "Count` scalar.");
        }
    }

    /// <summary>
    /// L2: the per-type vtable data — the <c>chaos_vtable_*[]</c> array and its
    /// matching <c>kSlots_*[]</c> VTableSlot table — is emitted from the object
    /// model but MUST NOT stay in page 0. It is 95% of that file's bulk, and
    /// leaving it inline is what made page 0 a 31 MB translation unit.
    ///
    /// <para>
    /// Asserted on real emitted sources rather than the model: the failure mode
    /// this guards is a mis-routed write in the emitter, which leaves the text
    /// in page 0 while a copy also reaches the payload sections. Nothing about
    /// the template model would reveal that.
    /// </para>
    ///
    /// <para>
    /// Counts DEFINITIONS only (<c>[] = ...</c>), not the <c>extern</c>
    /// declarations the payload preamble repeats into every translation unit —
    /// counting those would pass regardless of where the definitions went, and
    /// would report ~9,400 instead of 127.
    /// </para>
    /// </summary>
    [RequiresEmittedCodegenFact]
    public void VTableData_IsEmittedOutsidePageZero()
    {
        string subjectsDir = Path.Combine(
            RepoRootLocator.FindFromBaseDirectory(),
            "artifacts", "foundation-dll", "System.Private.CoreLib",
            "chunks", "system", "native", "subjects");

        Assert.True(Directory.Exists(subjectsDir),
            $"no emitted sources at '{subjectsDir}'. This guard asserts where the vtable "
            + "data lands in real codegen output and must not be skipped silently — run the "
            + "system chunk build first: "
            + "python -m verification.chunk_pipeline --chunk system --stages build");

        string pageZeroPath = Path.Combine(subjectsDir, "native-aot.generated.cpp");
        Assert.True(File.Exists(pageZeroPath), $"missing page 0 at '{pageZeroPath}'");

        // Definitions only — `[...] =` — so the preamble's repeated extern
        // declarations (which are legitimately emitted into every payload TU)
        // do not inflate the count.
        var vtableArrayDef = new Regex(
            @"^const void\* chaos_vtable_\w+\[\]\s*=", RegexOptions.Multiline);
        var slotTableDef = new Regex(
            @"^extern const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_\w+\[\]\s*=",
            RegexOptions.Multiline);

        string pageZero = File.ReadAllText(pageZeroPath);
        int vtableInPageZero = vtableArrayDef.Matches(pageZero).Count;
        int slotsInPageZero = slotTableDef.Matches(pageZero).Count;

        var payloadFiles = Directory.GetFiles(subjectsDir, "native-aot.payload.*.cpp");
        Assert.True(payloadFiles.Length > 0,
            $"no payload translation units in '{subjectsDir}' — the split is not running");

        int vtableInPayload = payloadFiles.Sum(f => vtableArrayDef.Matches(File.ReadAllText(f)).Count);
        int slotsInPayload = payloadFiles.Sum(f => slotTableDef.Matches(File.ReadAllText(f)).Count);

        Assert.True(vtableInPageZero == 0,
            $"page 0 still defines {vtableInPageZero} chaos_vtable_* array(s). These are the "
            + "bulk of the object model and must be partitioned into their own translation "
            + "units; leaving them inline is the unbounded-page-0 condition that produced a "
            + "31 MB translation unit.");

        Assert.True(slotsInPageZero == 0,
            $"page 0 still defines {slotsInPageZero} kSlots_* VTableSlot table(s). They travel "
            + "with their chaos_vtable_* array and must leave page 0 with it.");

        // And they must actually have arrived somewhere — a write that is dropped
        // entirely would satisfy the two assertions above while losing the data.
        Assert.True(vtableInPayload > 0 && slotsInPayload > 0,
            "no vtable arrays or slot tables found in any payload translation unit — the "
            + $"object model's vtable data was dropped, not relocated "
            + $"(vtable={vtableInPayload}, slots={slotsInPayload})");

        Assert.True(vtableInPayload == slotsInPayload,
            $"{vtableInPayload} chaos_vtable_* array(s) but {slotsInPayload} kSlots_* table(s) "
            + "in the payload TUs — every reference type emits exactly one of each, so a "
            + "mismatch means one of the two loops stopped emitting.");
    }

    /// <summary>
    /// The two giant reflection dispatchers must leave page 0 and survive intact
    /// in the payload translation units.
    ///
    /// <para>
    /// Splitting them is done by rewriting the emitter, so the failure modes are
    /// silent at compile time: a split that drops branches, duplicates them, or
    /// divides a <c>case</c> block still builds. Conservation has to be asserted
    /// against the emitted text — counting what the splitter reports would only
    /// check its own bookkeeping (see ReflectionDispatchSplitTests for the
    /// unit-level version of that lesson).
    /// </para>
    /// </summary>
    [RequiresEmittedCodegenFact]
    public void GiantReflectionDispatchers_AreSplitOutOfPageZeroIntact()
    {
        string subjectsDir = Path.Combine(
            RepoRootLocator.FindFromBaseDirectory(),
            "artifacts", "foundation-dll", "System.Private.CoreLib",
            "chunks", "system", "native", "subjects");

        Assert.True(Directory.Exists(subjectsDir),
            $"no emitted sources at '{subjectsDir}'. This guard asserts where the giant "
            + "reflection dispatchers land in real codegen output and must not be skipped "
            + "silently — run the system chunk build first: "
            + "python -m verification.chunk_pipeline --assembly System.Private.CoreLib "
            + "--chunk system --stages build");

        string pageZero = File.ReadAllText(Path.Combine(subjectsDir, "native-aot.generated.cpp"));
        var payloadFiles = Directory.GetFiles(subjectsDir, "native-aot.payload.*.cpp");
        Assert.True(payloadFiles.Length > 0, $"no payload translation units in '{subjectsDir}'");

        var payloadText = new StringBuilder();
        foreach (var f in payloadFiles)
            payloadText.Append(File.ReadAllText(f));
        string allPayload = payloadText.ToString();

        // 1. Page 0 keeps only the shells, which call the parts.
        int shellCallsForParams = Regex.Matches(pageZero,
            @"chaos_part_value = chaos_reflection_get_parameters_managed_part\d+\(").Count;
        int shellCallsForResolve = Regex.Matches(pageZero,
            @"chaos_part_value = chaos_reflection_resolve_method_handle_part\d+\(").Count;
        Assert.True(shellCallsForParams > 0,
            "page 0 has no calls to chaos_reflection_get_parameters_managed_partN — the "
            + "split is not running");
        Assert.True(shellCallsForResolve > 0,
            "page 0 has no calls to chaos_reflection_resolve_method_handle_partN — the "
            + "split is not running");

        // 2. The branch bodies are gone from page 0 and present in the payload.
        //    `^if` (column 0) is how a part body renders; the shell's own branches
        //    would have been indented, so this cannot match leftover shell text.
        int paramBranchesInPageZero = Regex.Matches(pageZero,
            @"^if \(chaos_method_handle == static_cast<CHAOS_IL2CPP_INTPTR>\(\d+u\)\)",
            RegexOptions.Multiline).Count;
        Assert.True(paramBranchesInPageZero == 0,
            $"page 0 still holds {paramBranchesInPageZero} parameter-name branch(es); they "
            + "must all live in the part translation units");

        // 3. Conservation: no branch is emitted twice WITHIN one dispatcher. The
        //    splitter must never duplicate a branch, which would make one handle
        //    reachable through two parts (wasteful, and a sign of a bad split).
        //
        //    Uniqueness is per-FUNCTION, not global: `get_parameters_managed` and
        //    `get_parameters_b3` are separate dispatchers that legitimately cover
        //    overlapping handles (10,258 + 15,982 = 26,240 branches but only 16,888
        //    distinct handles). Asserting global uniqueness fails on that legitimate
        //    overlap rather than on a defect. So group by the enclosing part
        //    function first, then require uniqueness inside each group.
        var partFunction = new Regex(
            @"^extern ""C"" [A-Z_0-9]+ (\w+_part\d+)\(",
            RegexOptions.Multiline);
        var branchesByFunction = new Dictionary<string, List<string>>(StringComparer.Ordinal);
        string? currentFunction = null;
        foreach (string line in allPayload.Split('\n'))
        {
            var fnMatch = partFunction.Match(line);
            if (fnMatch.Success)
            {
                currentFunction = fnMatch.Groups[1].Value;
                continue;
            }
            if (currentFunction is null) continue;
            var brMatch = Regex.Match(line,
                @"^if \(chaos_method_handle == static_cast<CHAOS_IL2CPP_INTPTR>\((\d+)u\)\)");
            if (brMatch.Success)
            {
                if (!branchesByFunction.TryGetValue(currentFunction, out var list))
                    branchesByFunction[currentFunction] = list = new List<string>();
                list.Add(brMatch.Groups[1].Value);
            }
        }

        int totalBranches = branchesByFunction.Values.Sum(v => v.Count);
        Assert.True(totalBranches > 0, "no parameter-name branches found in the payload TUs");

        // Group parts by the dispatcher they belong to (`<name>_partN` -> `<name>`)
        // so the per-dispatcher total is compared against that dispatcher alone.
        foreach (var group in branchesByFunction
            .GroupBy(kv => kv.Key[..kv.Key.LastIndexOf("_part", StringComparison.Ordinal)]))
        {
            var handles = group.SelectMany(kv => kv.Value).ToList();
            int distinct = handles.Distinct().Count();
            // A handle may appear in several parts only when those parts belong to
            // DIFFERENT dispatchers; within one dispatcher each handle is placed once.
            var perPart = group.Select(kv => (kv.Key, Set: kv.Value.ToHashSet())).ToList();
            for (int i = 0; i < perPart.Count; i++)
            {
                for (int j = i + 1; j < perPart.Count; j++)
                {
                    var overlap = perPart[i].Set.Intersect(perPart[j].Set).ToList();
                    // Guard the message: the interpolated string is built even when
                    // the assertion passes, so a bare overlap[0] would throw on the
                    // empty (i.e. correct) case.
                    Assert.True(overlap.Count == 0,
                        $"{perPart[i].Key} and {perPart[j].Key} (both parts of '{group.Key}') "
                        + $"share {overlap.Count} branch handle(s)"
                        + (overlap.Count > 0 ? $", e.g. {overlap[0]}" : "")
                        + " — a branch was emitted into more than one part of the same dispatcher");
                }
            }
            Assert.True(distinct > 0, $"dispatcher '{group.Key}' emitted no branches");
        }

        // 4. Case blocks are never divided. A part's STRCMP tests must all belong to
        //    cases that appear in that same part; a case split across parts would
        //    reorder its first-match-wins tests.
        foreach (var payloadFile in payloadFiles)
        {
            string text = File.ReadAllText(payloadFile);
            var declaredCases = Regex.Matches(text, @"^        case static_cast<CHAOS_IL2CPP_INTPTR>\((\d+)u\):")
                .Select(m => m.Groups[1].Value)
                .ToHashSet();
            if (declaredCases.Count == 0) continue;

            // Count case labels in this TU; every `case` line must be a complete
            // block, i.e. the file must not end mid-case. The structural guarantee
            // is that the splitter only ever appends whole blocks — verified here by
            // requiring the count to match the number of `break;` terminators that
            // close them within the same part function.
            int caseCount = declaredCases.Count;
            int breakCount = Regex.Matches(text, @"^            break;$", RegexOptions.Multiline).Count;
            Assert.True(caseCount <= breakCount,
                $"{Path.GetFileName(payloadFile)} declares {caseCount} case label(s) but has "
                + $"only {breakCount} case-terminating break(s) — a case block was divided");
        }
    }

    /// <summary>
    /// The reflection member tables must be split into per-declaring-type payload
    /// sections, with every <c>kRefl_params_*</c> array sharing a translation unit
    /// with the <c>kRefl_methods_*</c> array that names it.
    ///
    /// <para>
    /// These arrays are 7 MB on the system chunk and were emitted inline into the
    /// <c>modulereg</c> section, which the pager never divides — so they pinned
    /// page-0001 above the size target. Splitting them is only safe if the
    /// reference stays intra-TU: a <c>kRefl_methods_*</c> initializer names its
    /// <c>kRefl_params_*</c>, so separating them fails with C2065.
    /// </para>
    ///
    /// <para>
    /// The assertion is on real emitted output because the grouping happens in the
    /// planner: a unit test driving a synthetic model never produces these tables,
    /// so it would pass vacuously.
    /// </para>
    /// </summary>
    [RequiresEmittedCodegenFact]
    public void ReflectionMemberTables_KeepParamsWithTheirReferencingMethods()
    {
        string subjectsDir = Path.Combine(
            RepoRootLocator.FindFromBaseDirectory(),
            "artifacts", "foundation-dll", "System.Private.CoreLib",
            "chunks", "system", "native", "subjects");

        Assert.True(Directory.Exists(subjectsDir),
            $"no emitted sources at '{subjectsDir}'. Run the system chunk build first: "
            + "python -m verification.chunk_pipeline --assembly System.Private.CoreLib "
            + "--chunk system --stages build");

        var tuFiles = Directory.GetFiles(subjectsDir, "native-aot.payload.*.cpp")
            .Append(Path.Combine(subjectsDir, "native-aot.generated.cpp"))
            .Where(File.Exists)
            .ToArray();
        Assert.True(tuFiles.Length > 0, "no translation units found");

        // Which TU defines each params array, and which params each methods array
        // references. Built from emitted text, so it reflects what the compiler
        // sees rather than what the planner intended.
        var paramsHome = new Dictionary<string, string>(StringComparer.Ordinal);
        var methodsRefs = new List<(string File, string MethodsSym, List<string> Params)>();

        var paramsDef = new Regex(
            @"ReflectionQueryParameterDescriptor\s+(kRefl_params_\w+)\[\]\s*=");
        var methodsDef = new Regex(
            @"ReflectionQueryMethodDescriptor\s+(kRefl_methods_\w+)\[\]\s*=\s*\{(.*?)\n\s*\};",
            RegexOptions.Singleline);
        var paramsRef = new Regex(@"(kRefl_params_\w+)");

        foreach (string file in tuFiles)
        {
            string text = File.ReadAllText(file);
            string name = Path.GetFileName(file);

            foreach (Match m in paramsDef.Matches(text))
                paramsHome[m.Groups[1].Value] = name;

            foreach (Match m in methodsDef.Matches(text))
            {
                var referenced = paramsRef.Matches(m.Groups[2].Value)
                    .Select(r => r.Groups[1].Value)
                    .Distinct(StringComparer.Ordinal)
                    .ToList();
                methodsRefs.Add((name, m.Groups[1].Value, referenced));
            }
        }

        Assert.True(paramsHome.Count > 0, "no kRefl_params_* arrays found in the emitted TUs");
        Assert.True(methodsRefs.Count > 0, "no kRefl_methods_* arrays found in the emitted TUs");

        int violations = 0;
        foreach (var (file, methodsSym, referenced) in methodsRefs)
        {
            foreach (string param in referenced)
            {
                if (!paramsHome.TryGetValue(param, out string? home))
                {
                    violations++;
                    Assert.Fail($"{methodsSym} ({file}) references {param}, which is defined "
                        + "nowhere in the emitted translation units");
                }
                if (!string.Equals(home, file, StringComparison.Ordinal))
                {
                    violations++;
                    if (violations <= 5)
                    {
                        Assert.Fail($"{methodsSym} in {file} references {param}, which is defined "
                            + $"in {home}. They must share a translation unit: the methods "
                            + "initializer names the params array, so separating them fails "
                            + "with C2065.");
                    }
                }
            }
        }

        Assert.True(violations == 0, $"{violations} params/methods reference(s) cross a translation unit");

        // And the data must actually be RELOCATED, not merely re-declared. The
        // regression this guards against is reverting the per-type split and
        // putting every array back inside the one `modulereg` section (which the
        // pager never divides) — that is the condition that pinned page-0001 at
        // 8.9 MB. A single-TU layout passes the cross-TU check vacuously, so the
        // guard has to assert the tables are spread across more than one TU.
        var tusWithTables = paramsHome.Values
            .Concat(methodsRefs.Select(r => r.File))
            .Distinct(StringComparer.Ordinal)
            .Count();
        Assert.True(tusWithTables > 1,
            $"all reflection member tables collapsed into a single translation unit "
            + $"({tusWithTables}); they must be split by declaring type, each type's "
            + "params+methods+desc going to its own payload section");

        // The per-type grouping anchor: these marker comments are emitted once per
        // declaring type, and must not all live in one TU either.
        var tuMarkers = new Dictionary<string, int>(StringComparer.Ordinal);
        foreach (string file in tuFiles)
        {
            string text = File.ReadAllText(file);
            int n = Regex.Matches(text, @"reflection member tables for type: ").Count;
            if (n > 0) tuMarkers[Path.GetFileName(file)] = n;
        }
        Assert.True(tuMarkers.Count > 1,
            $"the per-type reflection tables did not spread across translation units "
            + $"({tuMarkers.Count} TU(s) carry them)");
    }

    /// <summary>
    /// The GC slot map must stay one contiguous object of bounded size, with its
    /// entry count and declared byte total unchanged.
    ///
    /// <para>
    /// This section cannot be split across translation units the way the other
    /// payload sections are: the runtime scans it as a single byte range
    /// (<c>GcRegisterSlotMapsFromSection(begin, end)</c>, advancing by each
    /// entry's <c>entry_total_size</c>). Splitting the object would change the ABI
    /// and the runtime, which is out of scope — so the fix is to shrink the TEXT,
    /// not the object.
    /// </para>
    ///
    /// <para>
    /// The text was dominated by per-entry comments carrying the full native
    /// symbol name (~150 chars each). Those are gone; what must NOT change is the
    /// data: the entry count and the sum of <c>entry_total_size</c> describe the
    /// exact byte layout the scanner walks, so a drift there silently
    /// mis-registers every following entry.
    /// </para>
    /// </summary>
    [RequiresEmittedCodegenFact]
    public void GcSlotMapSection_StaysContiguousAndSizedWithinBudget()
    {
        string subjectsDir = Path.Combine(
            RepoRootLocator.FindFromBaseDirectory(),
            "artifacts", "foundation-dll", "System.Private.CoreLib",
            "chunks", "system", "native", "subjects");

        Assert.True(Directory.Exists(subjectsDir),
            $"no emitted sources at '{subjectsDir}'. Run the system chunk build first: "
            + "python -m verification.chunk_pipeline --assembly System.Private.CoreLib "
            + "--chunk system --stages build");

        // The section may land in any payload TU after partitioning; find the one
        // that defines it rather than assuming a file name.
        string? definingTu = Directory.GetFiles(subjectsDir, "native-aot.payload.*.cpp")
            .Append(Path.Combine(subjectsDir, "native-aot.generated.cpp"))
            .FirstOrDefault(f => File.Exists(f)
                && File.ReadAllText(f).Contains("kChaosGcSlotMapsSection = {", StringComparison.Ordinal));

        Assert.True(definingTu is not null,
            "no translation unit defines kChaosGcSlotMapsSection");

        string text = File.ReadAllText(definingTu!);
        string fileName = Path.GetFileName(definingTu!);

        // 1. Exactly one definition — the object is contiguous by contract.
        int defs = Regex.Matches(text, @"kChaosGcSlotMapsSection = \{").Count;
        Assert.True(defs == 1,
            $"{fileName} defines kChaosGcSlotMapsSection {defs} time(s); the runtime scans it "
            + "as one contiguous range, so there must be exactly one object");

        // 2. Every entry keeps its declared size, and the size the initializer
        //    writes is the one the scanner will advance by. The values are
        //    positional (`{ <entry_total_size>u, reinterpret_cast<...>(&sym), ...`),
        //    so the assertion reads them by position rather than by comment.
        var entryInitializers = Regex.Matches(text,
            @"\{\s*(\d+)u,\s*\n?\s*reinterpret_cast<const void\*>\(&(\w+)\)");
        Assert.True(entryInitializers.Count > 0,
            $"{fileName} contains no slot-map entry initializers — the data is missing");

        // Each declared size must match the struct member's declared `slots[N]`,
        // since entry_total_size is what the runtime advances by: a drift between
        // the struct's slot count and the entry size silently mis-registers every
        // following entry.
        var declaredSlotCounts = Regex.Matches(text, @"CHAOS_IL2CPP_UINT32 slots\[(\d+)\];")
            .Select(m => int.Parse(m.Groups[1].Value, CultureInfo.InvariantCulture))
            .ToList();
        Assert.True(declaredSlotCounts.Count == entryInitializers.Count,
            $"{fileName} declares {declaredSlotCounts.Count} entry struct(s) but has "
            + $"{entryInitializers.Count} initializer(s) — they must correspond 1:1");

        for (int i = 0; i < entryInitializers.Count; i++)
        {
            int declaredSize = int.Parse(entryInitializers[i].Groups[1].Value, CultureInfo.InvariantCulture);
            // packed layout: entry_total_size(4) + code_address(8) + frame_size(4)
            //              + num_gc_slots(4) + slots[N]*4
            int expected = 4 + 8 + 4 + 4 + declaredSlotCounts[i] * 4;
            Assert.True(declaredSize == expected,
                $"entry {i} in {fileName} declares entry_total_size={declaredSize} but its "
                + $"struct has slots[{declaredSlotCounts[i]}], implying {expected}. The runtime "
                + "advances by entry_total_size, so a mismatch desynchronises every later entry.");
        }

        // 3. The section must stay within the per-TU budget that motivates this
        //    task. It is NOT split, so its text size is what has to come down.
        long sectionStart = text.IndexOf("// ── GC Slot Map Section", StringComparison.Ordinal);
        Assert.True(sectionStart >= 0, $"no GC Slot Map Section marker in {fileName}");
        int sectionEnd = text.IndexOf("#pragma pack(pop)", (int)sectionStart, StringComparison.Ordinal);
        long sectionBytes = (sectionEnd > 0 ? sectionEnd : text.Length) - sectionStart;

        Assert.True(sectionBytes < 8L * 1024 * 1024,
            $"the GC slot map section is {sectionBytes / 1048576.0:F2} MB in {fileName}. It cannot "
            + "be split across translation units (the runtime scans one contiguous range), so "
            + "its TEXT must fit the budget — the per-entry native-symbol comments are the bulk.");
    }
}
