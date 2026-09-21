using Chaos.IL2CPP.Generator;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Unit tests for <see cref="PayloadSectionPartitioner"/> — the component that
/// bounds page-0 payload size by grouping named sections into translation units.
///
/// <para>
/// These are deliberately pure (no planner, no template rendering) so they can
/// pin the partitioning contract precisely, including the failure modes that are
/// hardest to see downstream: dropped content and split sections.
/// </para>
/// </summary>
public sealed class PayloadSectionPartitionerTests
{
    private static PayloadSection Sec(string name, int length, int order = 0) => new()
    {
        Name = name,
        Content = new string('x', length),
        Order = order,
    };

    [Fact]
    public void EmptyInput_ProducesNoTranslationUnits()
    {
        var groups = PayloadSectionPartitioner.Partition([]);
        Assert.Empty(groups);
    }

    [Fact]
    public void ContentUnderBudget_StaysInOneTranslationUnit()
    {
        var groups = PayloadSectionPartitioner.Partition(
            [Sec("a", 100, 0), Sec("b", 100, 1)], budgetChars: 1000);

        Assert.Single(groups);
        Assert.Equal(2, groups[0].Count);
    }

    /// <summary>
    /// The core guarantee: no emitted TU exceeds the budget.
    /// </summary>
    [Fact]
    public void ContentOverBudget_IsSplitAcrossTranslationUnits()
    {
        var groups = PayloadSectionPartitioner.Partition(
            [Sec("a", 600, 0), Sec("b", 600, 1)], budgetChars: 1000);

        Assert.Equal(2, groups.Count);
        Assert.All(groups, g => Assert.True(g.Sum(s => s.Content.Length) <= 1000));
    }

    /// <summary>
    /// A section must never be divided. Sections carry internal
    /// definition/reference relationships (a table and the descriptor pointing
    /// at it), so cutting one in half produces a TU that compiles but is wrong.
    /// </summary>
    [Fact]
    public void OversizedSection_GetsItsOwnTranslationUnit_AndIsNotDivided()
    {
        var big = Sec("big", 5000, 0);
        var groups = PayloadSectionPartitioner.Partition(
            [big, Sec("small", 10, 1)], budgetChars: 1000);

        Assert.Equal(2, groups.Count);
        Assert.Same(big, groups[0][0]);
        Assert.Equal(5000, groups[0][0].Content.Length);   // whole, not truncated
        Assert.Equal("small", groups[1][0].Name);
    }

    /// <summary>
    /// Conservation: every input section appears exactly once across the output,
    /// with its content whole. This is the property whose violation is silent —
    /// a dropped section still compiles.
    ///
    /// <para>
    /// Deliberately mixes section sizes that straddle the budget (some far under,
    /// some far over) so the drop path is actually exercised. An earlier version
    /// of this test used only under-budget sections and therefore passed even
    /// when the partitioner silently discarded oversized ones — a false green
    /// caught by running a negative control.
    /// </para>
    /// </summary>
    [Fact]
    public void Partition_ConservesEverySectionExactlyOnce()
    {
        var sections = new List<PayloadSection>();
        for (int i = 0; i < 40; i++)
        {
            // Straddle the 1000-char budget: every 5th section is oversized so
            // the "section bigger than a whole TU" path is covered too.
            int size = (i % 5 == 0) ? 2_500 + i : 300 + i * 7;
            sections.Add(Sec($"s{i}", size, order: i));
        }

        var groups = PayloadSectionPartitioner.Partition(sections, budgetChars: 1000);

        var emitted = groups.SelectMany(g => g).ToList();
        Assert.Equal(sections.Count, emitted.Count);

        // Same multiset of names — no drops, no duplicates.
        Assert.Equal(
            sections.Select(s => s.Name).OrderBy(n => n, StringComparer.Ordinal),
            emitted.Select(s => s.Name).OrderBy(n => n, StringComparer.Ordinal));

        // And each section's content survived intact — including the oversized ones.
        foreach (var original in sections)
        {
            var round = emitted.Single(e => e.Name == original.Name);
            Assert.Equal(original.Content, round.Content);
        }
    }

    /// <summary>
    /// Ordering must follow <see cref="PayloadSection.Order"/>, not input order —
    /// definition/reference relationships in the payload depend on it.
    /// </summary>
    [Fact]
    public void Partition_PreservesDeclaredOrder()
    {
        var groups = PayloadSectionPartitioner.Partition(
            [Sec("c", 10, 2), Sec("a", 10, 0), Sec("b", 10, 1)], budgetChars: 1000);

        Assert.Single(groups);
        Assert.Equal(new[] { "a", "b", "c" }, groups[0].Select(s => s.Name));
    }

    /// <summary>
    /// Determinism: equal inputs must produce identical partitioning, or the
    /// generated file set changes between runs (the project already has a
    /// non-determinism problem — see the GetHashCode/timestamp notes).
    /// </summary>
    [Fact]
    public void Partition_IsDeterministic()
    {
        var sections = new List<PayloadSection>();
        for (int i = 0; i < 20; i++)
            sections.Add(Sec($"n{i}", 400, order: i));

        var first = PayloadSectionPartitioner.Partition(sections, budgetChars: 1000);
        var second = PayloadSectionPartitioner.Partition(sections, budgetChars: 1000);

        Assert.Equal(first.Count, second.Count);
        for (int i = 0; i < first.Count; i++)
            Assert.Equal(first[i].Select(s => s.Name), second[i].Select(s => s.Name));
    }

    /// <summary>
    /// Emitted TU names must match the CMake source-collection patterns, or the
    /// new files are silently never compiled.
    /// </summary>
    [Theory]
    [InlineData("om", 0, "native-aot.om.page-0001.cpp")]
    [InlineData("reg", 11, "native-aot.reg.page-0012.cpp")]
    public void TranslationUnitFileName_FollowsCmakeGlobFamily(string group, int idx, string expected)
    {
        Assert.Equal(expected, PayloadSectionPartitioner.TranslationUnitFileName(group, idx));
    }
}
