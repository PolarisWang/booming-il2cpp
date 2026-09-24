using System;
using System.Collections.Generic;
using System.Linq;
using Chaos.IL2CPP.Generator;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Step 3 of the unified-packer work: one packer replaces the two independent
/// greedy loops that placed methods and payload sections.
///
/// <para>
/// Because this is a REFACTOR, the tests that matter are the ones pinning the
/// behaviour both loops already had — a packer that is merely "reasonable"
/// would silently reshuffle every translation unit. The equivalence test below
/// re-implements the old rule inline and requires the packer to agree with it.
/// </para>
/// </summary>
public sealed class TuPackerTests
{
    private static PackableUnit U(string id, long size) => new(id, size, "test");

    /// <summary>
    /// The behaviour being preserved, written out the way both old loops did it:
    /// accumulate until the next item would overshoot, never emit an empty TU.
    ///
    /// <para>
    /// Kept as a literal transcription rather than a call into production code —
    /// deriving the expectation from the implementation under test would make
    /// the comparison vacuous.
    /// </para>
    /// </summary>
    private static List<List<string>> ReferencePack(
        IReadOnlyList<(string Id, long Size)> units, long target, long firstOverhead)
    {
        var result = new List<List<string>>();
        var current = new List<string>();
        long accumulated = firstOverhead;

        foreach (var (id, size) in units)
        {
            if (current.Count > 0 && accumulated + size > target)
            {
                result.Add(current);
                current = new List<string>();
                accumulated = 0;
            }
            current.Add(id);
            accumulated += size;
        }
        if (current.Count > 0)
            result.Add(current);
        return result;
    }

    private static List<List<string>> PackWith(
        IReadOnlyList<(string Id, long Size)> units, long target, long overhead = 0)
    {
        var packed = TuPacker.Pack(
            units.Select(u => U(u.Id, u.Size)).ToArray(),
            new TuBudget(target, overhead));
        return packed.Select(p => p.Units.Select(u => u.Id).ToList()).ToList();
    }

    // ── Equivalence with the pre-Step-3 rule ────────────────────────────

    [Theory]
    [InlineData(100, 10, 0)]
    [InlineData(1000, 100, 0)]
    [InlineData(1000, 100, 250)]   // first-TU overhead (page-0 extra content)
    [InlineData(350_000, 1200, 0)]
    public void Pack_MatchesTheRuleBothOldLoopsImplemented(long target, long size, long overhead)
    {
        var rng = new Random(12345);  // fixed seed: a failure must be reproducible
        var units = Enumerable.Range(0, 400)
            .Select(i => ($"u{i}", (long)rng.Next(1, (int)(size * 2))))
            .ToList();

        Assert.Equal(ReferencePack(units, target, overhead), PackWith(units, target, overhead));
    }

    /// <summary>
    /// The first TU is charged the overhead, which is what makes page 0 hold
    /// fewer methods than later pages. Losing this would move methods back onto
    /// page 0 and re-grow the TU this work exists to bound.
    /// </summary>
    [Fact]
    public void FirstTuOverhead_ReducesWhatFitsOnTheFirstTu()
    {
        var units = Enumerable.Range(0, 10).Select(i => ($"u{i}", 100L)).ToList();

        var noOverhead = PackWith(units, 1000, overhead: 0);
        var withOverhead = PackWith(units, 1000, overhead: 500);

        Assert.True(withOverhead[0].Count < noOverhead[0].Count,
            $"overhead must move units off the first TU "
            + $"(without={noOverhead[0].Count}, with={withOverhead[0].Count})");
    }

    // ── Invariants ──────────────────────────────────────────────────────

    /// <summary>
    /// A unit larger than the target cannot be placed inside it. It gets a TU
    /// of its own — and crucially, it does not take other units down with it.
    /// This is the behaviour the old loops expressed as
    /// <c>current.Count &gt; 0</c> / <c>methodIdx &gt; pageStart</c>.
    /// </summary>
    [Fact]
    public void OversizedUnit_GetsItsOwnTuAndDoesNotAbsorbNeighbours()
    {
        var units = new List<(string Id, long Size)> { ("a", 50), ("huge", 5000), ("b", 50) };
        var packed = PackWith(units, target: 100);

        Assert.Equal(3, packed.Count);
        Assert.Equal(new[] { "a" }, packed[0]);
        Assert.Equal(new[] { "huge" }, packed[1]);
        Assert.Equal(new[] { "b" }, packed[2]);
    }

    /// <summary>
    /// No empty TU, ever — an empty translation unit is invalid C++ and would
    /// also be a wasted compile.
    /// </summary>
    [Fact]
    public void NeverEmitsAnEmptyTu()
    {
        var units = new List<(string Id, long Size)> { ("x", 10_000), ("y", 10_000) };
        var packed = PackWith(units, target: 100);
        Assert.All(packed, p => Assert.NotEmpty(p));
    }

    /// <summary>
    /// Input order is semantically significant for both methods (dispatch
    /// order) and sections (declaration-before-use), so packing must be a
    /// stable partition of the input sequence, not a bin-packing reorder.
    /// </summary>
    [Fact]
    public void PreservesInputOrderAcrossAllTus()
    {
        var units = Enumerable.Range(0, 50).Select(i => ($"u{i}", 30L)).ToList();
        var packed = PackWith(units, target: 200);

        var flattened = packed.SelectMany(p => p).ToList();
        Assert.Equal(units.Select(u => u.Item1).ToList(), flattened);
    }

    [Fact]
    public void IsDeterministic()
    {
        var units = Enumerable.Range(0, 100)
            .Select(i => ($"u{i}", (long)(i * 7 % 53 + 1))).ToList();
        Assert.Equal(PackWith(units, 200), PackWith(units, 200));
    }

    [Fact]
    public void EmptyInput_YieldsNoTus()
    {
        Assert.Empty(TuPacker.Pack(Array.Empty<PackableUnit>(), new TuBudget(100)));
    }

    /// <summary>
    /// Every unit lands in exactly one TU — no unit dropped, none duplicated.
    /// A dropped unit means generated code silently vanished.
    /// </summary>
    [Fact]
    public void EveryUnitIsPlacedExactlyOnce()
    {
        var rng = new Random(999);
        var units = Enumerable.Range(0, 500)
            .Select(i => ($"u{i}", (long)rng.Next(1, 400))).ToList();
        var packed = PackWith(units, target: 1000, overhead: 137);

        var placed = packed.SelectMany(p => p).ToList();
        Assert.Equal(units.Count, placed.Count);
        Assert.Equal(
            units.Select(u => u.Item1).OrderBy(x => x, StringComparer.Ordinal),
            placed.OrderBy(x => x, StringComparer.Ordinal));
    }

    // ── Reported metadata ───────────────────────────────────────────────

    /// <summary>
    /// The accumulated size is the number a reviewer compares against the
    /// budget, so it must include the first-TU overhead rather than report only
    /// the sum of the units.
    /// </summary>
    [Fact]
    public void AccumulatedChars_IncludesFirstTuOverhead()
    {
        var packed = TuPacker.Pack(
            new[] { U("a", 100), U("b", 100) },
            new TuBudget(1000, FirstTuOverheadChars: 250));

        Assert.Single(packed);
        Assert.Equal(450, packed[0].AccumulatedChars);
    }

    [Fact]
    public void FlushReason_DistinguishesBudgetCutFromEndOfInput()
    {
        var packed = TuPacker.Pack(
            new[] { U("a", 90), U("b", 90) },
            new TuBudget(100));

        Assert.Equal(2, packed.Count);
        Assert.Equal(TuPacker.ReasonApproachingTarget, packed[0].FlushReason);
        Assert.Equal(TuPacker.ReasonLastUnit, packed[1].FlushReason);
    }

    [Fact]
    public void InvalidBudget_Throws()
    {
        Assert.Throws<ArgumentOutOfRangeException>(
            () => TuPacker.Pack(new[] { U("a", 1) }, new TuBudget(0)));
        Assert.Throws<ArgumentOutOfRangeException>(
            () => TuPacker.Pack(new[] { U("a", 1) }, new TuBudget(100, -1)));
    }
}
