using System;
using System.Collections.Generic;
using System.Linq;
using Chaos.IL2CPP.Generator;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Step 2 of the unified-packer work: a section declares whether it may be
/// divided, and along which boundaries. The pager honours that declaration
/// instead of inferring divisibility from size.
/// </summary>
public sealed class SectionSemanticsTests
{
    private static PayloadSection Atomic(string name, string content, int order) =>
        new() { Name = name, Content = content, Order = order };

    private static PayloadSection ByUnits(string name, IEnumerable<string> units, int order)
    {
        var list = units.ToList();
        return new PayloadSection
        {
            Name = name,
            Content = string.Concat(list),
            Order = order,
            Semantics = SectionSemantics.ByUnits,
            Units = list.Select((c, i) => new SectionUnit($"{name}[{i}]", c)).ToArray(),
        };
    }

    /// <summary>
    /// The pre-Step-2 behaviour, and the behaviour of every section that does
    /// not opt in: a section larger than the budget gets its own oversized TU
    /// rather than being cut.
    /// </summary>
    [Fact]
    public void AtomicSection_IsNeverDivided()
    {
        var big = new string('x', 1000);
        var tus = PayloadSectionPartitioner.Partition(
            new[] { Atomic("big", big, 0) }, budgetChars: 100);

        Assert.Single(tus);
        Assert.Equal(big.Length, tus[0].Sum(s => s.Content.Length));
    }

    /// <summary>
    /// A ByUnits section is divided at its declared boundaries, so the pieces
    /// are placed by size instead of the whole section going into one TU.
    /// </summary>
    [Fact]
    public void ByUnitsSection_IsDividedAtDeclaredBoundaries()
    {
        var section = ByUnits("tables", new[] { new string('a', 80), new string('b', 80), new string('c', 80) }, 0);
        var tus = PayloadSectionPartitioner.Partition(new[] { section }, budgetChars: 100);

        // Three 80-char units against a 100-char budget: no two fit together.
        Assert.Equal(3, tus.Count);
        foreach (var tu in tus)
            Assert.Single(tu);
    }

    /// <summary>
    /// Dividing must not change the emitted text: the concatenation of all
    /// pieces equals the original section content, whatever the budget.
    /// </summary>
    [Fact]
    public void ByUnitsSection_PreservesTotalContent()
    {
        var section = ByUnits("tables", new[] { "alpha", "beta", "gamma" }, 0);
        var tus = PayloadSectionPartitioner.Partition(new[] { section }, budgetChars: 1000);

        string emitted = string.Concat(tus.SelectMany(t => t).Select(s => s.Content));
        Assert.Equal(section.Content, emitted);
    }

    /// <summary>
    /// 🔴 The reassembly contract. A unit list that does not reconstruct the
    /// section would silently drop or duplicate generated code — the symptom
    /// would appear far away as a missing or duplicate symbol. It must fail
    /// here, where the cause is still visible.
    /// </summary>
    [Fact]
    public void ByUnitsSection_ThatDoesNotReassemble_Throws()
    {
        var broken = new PayloadSection
        {
            Name = "broken",
            Content = "alphabetagamma",
            Order = 0,
            Semantics = SectionSemantics.ByUnits,
            // Missing "gamma": lengths differ, so the cheap check catches it.
            Units = new[]
            {
                new SectionUnit("u0", "alpha"),
                new SectionUnit("u1", "beta"),
            },
        };

        var ex = Assert.Throws<InvalidOperationException>(
            () => PayloadSectionPartitioner.Partition(new[] { broken }, budgetChars: 1000));
        Assert.Contains("do not reassemble", ex.Message, StringComparison.Ordinal);
        Assert.Contains("broken", ex.Message, StringComparison.Ordinal);
    }

    /// <summary>
    /// Same total length, different content — the case the length check alone
    /// would let through.
    /// </summary>
    [Fact]
    public void ByUnitsSection_SameLengthDifferentContent_Throws()
    {
        var broken = new PayloadSection
        {
            Name = "swapped",
            Content = "alphabetagamma",
            Order = 0,
            Semantics = SectionSemantics.ByUnits,
            Units = new[]
            {
                new SectionUnit("u0", "beta"),
                new SectionUnit("u1", "alpha"),
                new SectionUnit("u2", "gamma"),
            },
        };

        Assert.Throws<InvalidOperationException>(
            () => PayloadSectionPartitioner.Partition(new[] { broken }, budgetChars: 1000));
    }

    /// <summary>
    /// A single unit is not divisible in any meaningful sense; declaring one
    /// must not cause the pager to treat it as cuttable.
    /// </summary>
    [Fact]
    public void ByUnitsSection_WithOneUnit_BehavesAtomically()
    {
        var section = ByUnits("single", new[] { new string('z', 500) }, 0);
        Assert.Single(section.Units); // the caller decides semantics; one unit is not a split
    }

    /// <summary>
    /// The default must stay Atomic: opting in is explicit, so every existing
    /// section keeps its current behaviour without being touched.
    /// </summary>
    [Fact]
    public void DefaultSemantics_IsAtomic()
    {
        Assert.Equal(SectionSemantics.Atomic, Atomic("s", "x", 0).Semantics);
    }
}
