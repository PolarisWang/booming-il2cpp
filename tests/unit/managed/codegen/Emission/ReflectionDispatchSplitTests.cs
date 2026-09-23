using System.Linq;
using Chaos.IL2CPP.Generator;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Guards for splitting the two giant reflection dispatchers on page 0.
///
/// <para>
/// These test the splitter directly rather than scanning emitted output, because
/// the properties that matter are structural and the artifact-level guard
/// (<c>PagePayloadSplitTests</c>) can only run after a full system-chunk build.
/// </para>
///
/// <para>
/// The load-bearing property is <b>order preservation</b>. A single <c>case</c>
/// in <c>resolve_method_handle</c> contains up to 16
/// <c>STRCMP(name, "AreEqual")</c> tests that return different tokens, and the
/// semantics are first-match-wins. If a split reorders or de-duplicates them the
/// wrong token comes back with no diagnostic at all — so several tests below
/// exist purely to make that failure loud.
/// </para>
/// </summary>
public sealed class ReflectionDispatchSplitTests
{
    // ── Order preservation (the R2 hazard) ──────────────────────────────

    /// <summary>
    /// A chain must come out in input order: the split may not sort or group.
    ///
    /// <para>
    /// The markers are deliberately chosen so that lexicographic order is the
    /// <i>reverse</i> of input order. With naively ascending markers (0,1,2,…) a
    /// sort is a no-op and this test passes even against a sorting implementation
    /// — it would be self-satisfying. Reversing the marker strings makes any sort
    /// visible.
    /// </para>
    /// </summary>
    [Fact]
    public void SplitFlatChain_PreservesBranchOrder()
    {
        // Input order: z, y, x, … — lexicographic order is exactly the reverse.
        var markers = new[] { "zulu", "yankee", "xray", "whiskey", "victor",
                              "uniform", "tango", "sierra", "romeo", "quebec" };

        var branches = markers
            .Select(m => $"if (chaos_method_handle == {m}) {{ return {m}; }}\n")
            .ToArray();

        var parts = ReflectionDispatchPartitioner.SplitFlatChain(
            "f", "int", "int chaos_method_handle", branches, branchesPerPart: 4);

        string all = string.Concat(parts.Select(p => p.Text));
        int prev = -1;
        foreach (var m in markers)
        {
            int idx = all.IndexOf($") {{ return {m}; }}", System.StringComparison.Ordinal);
            Assert.True(idx > prev, $"branch '{m}' is out of order (or missing)");
            prev = idx;
        }
    }

    /// <summary>
    /// Duplicate branch bodies must all survive. A de-duplicating split would
    /// collapse them, which for <c>resolve_method_handle</c> means dropping
    /// distinct method handles that share a name.
    ///
    /// <para>
    /// The returns are likewise ordered against their own sort: 3, 2, 1 would be
    /// re-ordered by any sort, so a sorting implementation fails here too.
    /// </para>
    /// </summary>
    [Fact]
    public void SplitFlatChain_KeepsDuplicateBranches()
    {
        var branches = new[]
        {
            "if (n == \"AreEqual\") { return 3; }\n",
            "if (n == \"AreEqual\") { return 2; }\n",
            "if (n == \"AreEqual\") { return 1; }\n",
        };

        var parts = ReflectionDispatchPartitioner.SplitFlatChain(
            "f", "int", "const char* n", branches, branchesPerPart: 10);

        string all = string.Concat(parts.Select(p => p.Text));
        Assert.Contains("return 3;", all);
        Assert.Contains("return 2;", all);
        Assert.Contains("return 1;", all);
        // ...and the three distinct returns keep their relative order (3, 2, 1).
        Assert.True(all.IndexOf("return 3;", System.StringComparison.Ordinal)
                  < all.IndexOf("return 2;", System.StringComparison.Ordinal));
        Assert.True(all.IndexOf("return 2;", System.StringComparison.Ordinal)
                  < all.IndexOf("return 1;", System.StringComparison.Ordinal));
    }

    // ── Conservation ────────────────────────────────────────────────────

    /// <summary>
    /// Every branch lands in exactly one part — no branch dropped, none emitted
    /// twice. Losing one silently narrows the dispatcher, which shows up much
    /// later as an unresolvable handle.
    ///
    /// <para>
    /// The count is taken from the <b>emitted text</b>, not from
    /// <see cref="ReflectionDispatchPartitioner.Part.BranchCount"/>. Asserting on
    /// the reported count would only check the splitter's own bookkeeping against
    /// itself: an implementation that reported N but appended N-1 per part would
    /// still pass. (That is exactly the bug the first version of this test
    /// missed.)
    /// </para>
    /// </summary>
    [Theory]
    [InlineData(1, 1500)]
    [InlineData(10, 3)]
    [InlineData(1000, 128)]
    [InlineData(10258, 1500)]
    public void SplitFlatChain_ConservesBranchCount(int total, int perPart)
    {
        var branches = Enumerable.Range(0, total)
            .Select(i => $"if (x == {i}) {{ return {i}; }}\n")
            .ToArray();

        var parts = ReflectionDispatchPartitioner.SplitFlatChain(
            "f", "int", "int x", branches, perPart);

        // Count in the OUTPUT: one `if (x == ` per branch body emitted.
        int emitted = parts.Sum(p =>
            System.Text.RegularExpressions.Regex.Matches(p.Text, @"if \(x == ").Count);

        Assert.Equal(total, emitted);
        // The reported tally must agree with what was actually written.
        Assert.Equal(emitted, parts.Sum(p => p.BranchCount));
    }

    [Fact]
    public void SplitCaseBlocks_ConservesBranchCount()
    {
        // Distinct, countable bodies so the assertion can read the output.
        var blocks = Enumerable.Range(0, 89)
            .Select(i => ($"        case {i}:\n" + string.Concat(Enumerable.Repeat(
                $"            if (STRCMP(n, \"m{i}\") == 0) {{ return {i}; }}\n", i + 1))
                + "            break;\n", i + 1))
            .ToArray();

        var parts = ReflectionDispatchPartitioner.SplitCaseBlocks(
            "f", "int", "int t, const char* n", blocks, branchesPerPart: 200);

        int emitted = parts.Sum(p =>
            System.Text.RegularExpressions.Regex.Matches(p.Text, @"STRCMP\(n, ").Count);

        Assert.Equal(blocks.Sum(b => b.Item2), emitted);
        Assert.Equal(emitted, parts.Sum(p => p.BranchCount));
    }

    // ── Case blocks stay whole ──────────────────────────────────────────

    /// <summary>
    /// The central invariant: a case block larger than the per-part budget must
    /// not be divided. Dividing it could move its STRCMP tests into a different
    /// part relative to each other, which changes first-match-wins outcomes.
    /// </summary>
    [Fact]
    public void SplitCaseBlocks_NeverDividesAnOversizedBlock()
    {
        // One block far larger than the budget, sandwiched between small ones.
        const int Big = 5000;
        var blocks = new (string, int)[]
        {
            ("        case 1:\n            break;\n", 1),
            ("        case 2:\n" + string.Concat(Enumerable.Repeat(
                "            if (STRCMP(n, \"AreEqual\") == 0) { return 1; }\n", Big)), Big),
            ("        case 3:\n            break;\n", 1),
        };

        var parts = ReflectionDispatchPartitioner.SplitCaseBlocks(
            "f", "int", "int t, const char* n", blocks, branchesPerPart: 100);

        // The oversized block is intact inside one and only one part.
        var bigLine = "if (STRCMP(n, \"AreEqual\") == 0) { return 1; }";
        int occurrences = parts.Count(p => p.Text.Contains(bigLine, System.StringComparison.Ordinal));
        Assert.Equal(1, occurrences);

        var owner = parts.Single(p => p.Text.Contains(bigLine, System.StringComparison.Ordinal));
        int marker = owner.Text.Split('\n').Count(l => l.Contains(bigLine, System.StringComparison.Ordinal));
        Assert.Equal(Big, marker);
    }

    /// <summary>
    /// Every part is self-contained: its own <c>switch</c>, its own
    /// <c>default: break;</c>, and a trailing <c>return 0;</c> so a miss falls
    /// through to the shell's next part rather than terminating the search.
    /// </summary>
    [Fact]
    public void SplitCaseBlocks_EachPartIsSelfContained()
    {
        var blocks = new (string, int)[]
        {
            ("        case 1:\n            break;\n", 1),
            ("        case 2:\n            break;\n", 1),
        };

        var parts = ReflectionDispatchPartitioner.SplitCaseBlocks(
            "f", "int", "int t, const char* n", blocks, branchesPerPart: 1);

        Assert.Equal(2, parts.Count);
        foreach (var part in parts)
        {
            Assert.Contains("switch (chaos_type_handle)", part.Text);
            Assert.Contains("default:", part.Text);
            Assert.Contains("return 0;", part.Text);
            Assert.StartsWith("extern \"C\" int f_part", part.Text);
        }
    }

    // ── Shell shape ─────────────────────────────────────────────────────

    /// <summary>
    /// The shell calls each part in order and returns the first non-zero result,
    /// falling through to the epilogue only when every part misses.
    /// </summary>
    [Fact]
    public void BuildFlatChainShell_CallsPartsInOrderBeforeEpilogue()
    {
        var branches = new[]
        {
            "if (x == 1) { return 11; }\n",
            "if (x == 2) { return 22; }\n",
        };
        var parts = ReflectionDispatchPartitioner.SplitFlatChain(
            "f", "int", "int x", branches, branchesPerPart: 1);

        string shell = ReflectionDispatchPartitioner.BuildFlatChainShell(
            "f", "int", "int x",
            prologue: "    if (x == 0) { return -1; }\n",
            parts,
            argumentList: "x",
            epilogue: "    return 99;\n");

        int p0 = shell.IndexOf("f_part0(x)", System.StringComparison.Ordinal);
        int p1 = shell.IndexOf("f_part1(x)", System.StringComparison.Ordinal);
        int epi = shell.IndexOf("return 99;", System.StringComparison.Ordinal);

        Assert.True(p0 > 0 && p1 > p0, "parts must be called in order");
        Assert.True(epi > p1, "epilogue must run only after every part declined");
        Assert.Contains("if (x == 0) { return -1; }", shell);
        // The shell itself must not contain branch bodies.
        Assert.DoesNotContain("return 11;", shell);
    }

    /// <summary>
    /// The epilogue stays in the shell, never in a part: a part that carried it
    /// would run it once per part that missed.
    /// </summary>
    [Fact]
    public void SplitFlatChain_DoesNotPutEpilogueInAnyPart()
    {
        var branches = Enumerable.Range(0, 5)
            .Select(i => $"if (x == {i}) {{ return {i}; }}\n")
            .ToArray();

        var parts = ReflectionDispatchPartitioner.SplitFlatChain(
            "f", "int", "int x", branches, branchesPerPart: 2);

        foreach (var part in parts)
        {
            Assert.DoesNotContain("chaos_reflection_get_parameters_b3", part.Text);
        }
    }
}
