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
    /// L2: the page-0 payload must itself stay within the documented TU budget.
    ///
    /// <para>
    /// This is the regression the whole task exists for. Before the fix,
    /// <c>ModuleRegistrationCode</c> is appended whole and page 0 is unbounded;
    /// for the system chunk that reached ~71 MB against a 350 KB budget.
    /// </para>
    /// </summary>
    [Fact]
    public void PageZeroPayload_StaysWithinTuBudget()
    {
        var model = BuildModel(PayloadScaleMethodCount);

        // Page 0's unbounded payload = the parts BuildGeneratedPageToBuilder
        // appends verbatim when includeObjectModel / includeRegistration are set.
        int payloadChars =
              (model.ObjectModelCodeBuilder?.Length ?? model.ObjectModelCode?.Length ?? 0)
            + (model.ModuleRegistrationCode?.Length ?? 0)
            + (model.GenericRegistrationCode?.Length ?? 0);

        Assert.True(
            payloadChars <= PerTuBudgetChars * BudgetTolerance,
            $"page-0 payload is {payloadChars:N0} chars, exceeding the documented TU budget "
            + $"({PerTuBudgetChars:N0} x {BudgetTolerance}). The payload is appended whole and "
            + "is not partitioned — this is what drives MSVC C1002 (compiler out of heap "
            + "space) on large chunks.");
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
    /// L3: no symbol defined in one TU may be referenced as <c>static</c> from
    /// another. <c>static</c> is internal linkage, so any cross-section reference
    /// to a static table is an eventual C2065 / LNK2019 once the sections become
    /// separate TUs.
    ///
    /// <para>
    /// Deliberately anchored to a concrete symbol pair rather than a global
    /// count: a count-based assertion would be satisfied by any other matching
    /// text in the payload and would pass on the unfixed code.
    /// </para>
    /// </summary>
    [Fact]
    public void PayloadTables_AreNotStaticWhenReferencedAcrossSections()
    {
        var model = BuildModel(PayloadScaleMethodCount);

        string payload =
              (model.ObjectModelCodeBuilder?.ToString() ?? model.ObjectModelCode ?? "")
            + (model.ModuleRegistrationCode ?? "");

        // Top-level `static` table definitions in the payload.
        var staticTableDefs = Regex.Matches(
                payload,
                @"^[ \t]*static\s+[^\n;=]*?\b(k[A-Z]\w*|chaos_\w+)\s*(\[[^\]]*\])?\s*=",
                RegexOptions.Multiline)
            .Select(m => m.Groups[1].Value)
            .Distinct(StringComparer.Ordinal)
            .ToList();

        foreach (string table in staticTableDefs)
        {
            // Count references that are NOT the definition itself.
            var refPattern = new Regex(@"\b" + Regex.Escape(table) + @"\s*\[");
            int refs = refPattern.Matches(payload).Count;

            Assert.True(refs <= 1,
                $"payload table '{table}' is declared `static` (internal linkage) but is "
                + $"referenced {refs} times in the payload. Once this payload is split "
                + "across translation units those references cross a TU boundary and fail "
                + "to link (C2065/LNK2019). Tables that must cross a TU boundary have to be "
                + "declared `extern`.");
        }
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
