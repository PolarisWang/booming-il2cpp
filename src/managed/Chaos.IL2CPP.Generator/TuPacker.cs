using System;
using System.Collections.Generic;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// One indivisible item to be placed into a translation unit.
///
/// <para>
/// A unit is whatever the caller can no longer usefully divide: a method, or a
/// payload section that has already been reduced to the finest piece its
/// producer declared safe (see <c>SectionSemantics</c>). The packer treats them
/// uniformly, which is the point — methods and payload were previously placed
/// by two separate loops with their own budgets and their own overhead terms,
/// and nothing answered "how big will this TU actually be".
/// </para>
/// </summary>
/// <param name="Id">
/// Stable identifier, carried into the split report so a TU can be traced back
/// to the items that compose it.
/// </param>
/// <param name="SizeChars">The item's emitted size, in characters.</param>
/// <param name="Kind">Origin of the unit, for reporting ("method" / "section").</param>
public sealed record PackableUnit(string Id, long SizeChars, string Kind);

/// <summary>
/// The budget a packing run is measured against.
/// </summary>
/// <param name="TargetChars">
/// Accumulated size at which a TU is closed. Exceeding it is not an error — a
/// single item larger than the target still gets placed (see
/// <see cref="TuPacker.Pack"/>) — but a TU should not exceed it by choice.
/// </param>
/// <param name="FirstTuOverheadChars">
/// Fixed overhead charged to the FIRST translation unit only, covering
/// content that page 0 carries beyond its listed units (the object model and
/// registration bodies it still inlines). Kept separate from
/// <see cref="TargetChars"/> rather than folded in, because it applies once
/// per payload rather than once per TU.
/// </param>
public sealed record TuBudget(long TargetChars, long FirstTuOverheadChars = 0);

/// <summary>
/// One packed translation unit: the units it carries, what they total, and why
/// packing stopped there.
/// </summary>
/// <param name="FlushReason">
/// Why this TU was closed. Recorded because "which TU is oversized" is only
/// half the question — "what made it close here" is what a reviewer needs.
/// </param>
public sealed record PackedTu(
    IReadOnlyList<PackableUnit> Units,
    long AccumulatedChars,
    string FlushReason);

/// <summary>
/// Places a flat sequence of indivisible units into budget-sized translation
/// units.
///
/// <para>
/// <b>Why a single packer.</b> Methods and payload sections were partitioned by
/// two independent loops that agreed only by convention: both were greedy,
/// both kept a single oversized item rather than splitting it, both charged a
/// fixed overhead — but each had its OWN budget constant (four copies of
/// <c>350_000</c> existed, synchronised by comments) and only the method loop
/// knew that the first TU carries extra content. Two implementations of one
/// rule drift; one implementation cannot.
/// </para>
///
/// <para>
/// <b>Invariants</b> (all previously implicit, now stated and tested):
/// <list type="number">
/// <item>Accumulated size stays within the target unless a single unit exceeds
/// it on its own.</item>
/// <item>A unit is never divided. This is the caller's guarantee, since the
/// caller chose the unit granularity.</item>
/// <item>No empty TU is emitted; an oversized unit gets its own.</item>
/// <item>Input order is preserved — it is semantically significant for both
/// methods and sections.</item>
/// <item>The first TU is additionally charged
/// <see cref="TuBudget.FirstTuOverheadChars"/>.</item>
/// <item>The result is a pure function of the inputs.</item>
/// </list>
/// </para>
/// </summary>
public static class TuPacker
{
    /// <summary>Flush reason: the next unit would have exceeded the target.</summary>
    public const string ReasonApproachingTarget = "approachingTarget";

    /// <summary>Flush reason: the input ran out.</summary>
    public const string ReasonLastUnit = "lastUnit";

    /// <summary>
    /// Packs <paramref name="units"/> into translation units in order.
    ///
    /// <para>
    /// A unit is never divided, so a unit larger than the target produces a TU
    /// larger than the target. That is deliberate: dividing it is exactly what
    /// the section semantics say is unsafe. Such TUs are what the split report
    /// calls out.
    /// </para>
    /// </summary>
    public static IReadOnlyList<PackedTu> Pack(
        IReadOnlyList<PackableUnit> units,
        TuBudget budget)
    {
        ArgumentNullException.ThrowIfNull(units);
        ArgumentNullException.ThrowIfNull(budget);
        if (budget.TargetChars <= 0)
            throw new ArgumentOutOfRangeException(nameof(budget), budget.TargetChars,
                "target must be positive");
        if (budget.FirstTuOverheadChars < 0)
            throw new ArgumentOutOfRangeException(nameof(budget), budget.FirstTuOverheadChars,
                "overhead cannot be negative");

        var packed = new List<PackedTu>();
        if (units.Count == 0)
            return packed;

        var current = new List<PackableUnit>();
        long accumulated = budget.FirstTuOverheadChars;

        for (int i = 0; i < units.Count; i++)
        {
            var unit = units[i];
            bool isLast = i == units.Count - 1;

            // Close the current TU before admitting a unit that would overshoot.
            // The `current.Count > 0` guard is what keeps an oversized unit from
            // producing an empty TU ahead of it — the unit then lands in a TU of
            // its own and the next unit starts fresh.
            if (current.Count > 0 && accumulated + unit.SizeChars > budget.TargetChars)
            {
                packed.Add(new PackedTu(current, accumulated, ReasonApproachingTarget));
                current = new List<PackableUnit>();
                accumulated = 0;
            }

            current.Add(unit);
            accumulated += unit.SizeChars;

            if (isLast)
                packed.Add(new PackedTu(current, accumulated, ReasonLastUnit));
        }

        return packed;
    }
}
