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
    /// L2: page-0 payload must be partitioned so that no emitted translation unit
    /// exceeds the documented budget.
    ///
    /// <para>
    /// Asserts on the <b>partitioned output</b>, not on
    /// <c>ModuleRegistrationCode</c>. Sectioning happens in the emitter, so the
    /// planner's raw payload string is legitimately still whole — measuring it
    /// would fail forever regardless of the fix.
    /// </para>
    /// </summary>
    [Fact]
    public void PageZeroPayload_StaysWithinTuBudget()
    {
        var model = BuildModel(PayloadScaleMethodCount);

        Assert.True(model.PayloadSections is { Count: > 0 },
            "the planner must expose named payload sections so the emitter can "
            + "partition page-0 payload into bounded translation units");

        var tus = PayloadSectionPartitioner.Partition(
            model.PayloadSections, NativeAotEmitter.PayloadSectioningBudgetChars);

        Assert.True(tus.Count > 0, "expected at least one payload translation unit");

        // Every TU within tolerance of the budget. A section that is itself over
        // budget is allowed its own TU (sections must not be cut), so the bound is
        // checked against the *sum of grouped sections*, which is what the guard
        // can actually promise.
        long tolerance = (long)(NativeAotEmitter.PayloadSectioningBudgetChars * BudgetTolerance);
        foreach (var tu in tus)
        {
            long size = tu.Sum(s => (long)s.Content.Length);
            Assert.True(size <= tolerance,
                $"a payload translation unit is {size:N0} chars, over the "
                + $"{tolerance:N0} limit (budget {NativeAotEmitter.PayloadSectioningBudgetChars:N0} "
                + $"x {BudgetTolerance}). Sections in it: "
                + string.Join(", ", tu.Select(s => $"{s.Name}({s.Content.Length:N0})")));
        }

        // And the split must actually be doing work: the whole payload cannot fit
        // in a single TU at this scale.
        long total = model.PayloadSections.Sum(s => (long)s.Content.Length);
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
    [Fact]
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

        // Qualified types are common (`static const ::ns::T kFoo[] =`), so the
        // type-name span must not be restricted to a bare identifier.
        var defPattern = new Regex(
            @"^[ \t]*static\b[^\n=]*?\b(?<sym>k[A-Z]\w*|chaos_\w+|s_[a-z]\w*)\s*(?:\[[^\]]*\])?\s*=",
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
}
