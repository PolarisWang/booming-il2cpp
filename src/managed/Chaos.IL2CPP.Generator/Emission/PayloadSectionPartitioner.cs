using System;
using System.Collections.Generic;
using System.Text;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// One named block of page-0 payload, before it is placed into a translation unit.
///
/// <para>
/// Page 0 used to receive the entire payload as one unbounded string, which made
/// the page-0 TU grow without limit regardless of how finely the <i>methods</i>
/// were partitioned (measured: 71 MB for the system chunk, against a documented
/// 350 KB per-TU budget). Carrying the payload as named sections lets the emitter
/// apply the size budget to it the same way it already does for methods.
/// </para>
/// </summary>
public sealed class PayloadSection
{
    /// <summary>
    /// Stable identifier for the section, used to derive the emitted file name.
    /// Must be filesystem-safe and unique within a payload.
    /// </summary>
    public required string Name { get; init; }

    /// <summary>
    /// The C++ text of the section.
    /// </summary>
    public required string Content { get; init; }

    /// <summary>
    /// Relative ordering of this section within the payload. Sections are emitted
    /// in ascending order; ties broken by <see cref="Name"/> for determinism.
    /// </summary>
    public int Order { get; init; }

    /// <summary>
    /// Symbols this section DEFINES at top level and that other sections may
    /// reference. Declared here so the emitter can publish them in the contract
    /// header every payload translation unit includes.
    ///
    /// <para>
    /// Without this, a symbol defined in one section and used in another has no
    /// declaration anywhere: it used to be satisfied implicitly by both living in
    /// page 0's single translation unit. Splitting the payload into separate TUs
    /// breaks that, producing C2065 at every cross-section reference — and, where
    /// the reference sits in a const initializer, a cascading C2737.
    /// </para>
    /// </summary>
    public IReadOnlyList<CrossSectionSymbol> ExportedSymbols { get; init; }
        = Array.Empty<CrossSectionSymbol>();
}

/// <summary>
/// A symbol defined in one payload section and referenced from another.
///
/// <para>
/// The emitter must ensure every payload translation unit can see a declaration
/// of it, and — for symbols that cross a TU boundary — that the definition has
/// external linkage. Internal linkage (<c>static</c>) cannot be referenced across
/// translation units at all, so <see cref="NeedsExternalLinkage"/> marks the
/// definitions the planner had to emit as <c>extern</c>.
/// </para>
/// </summary>
/// <param name="Name">The C++ symbol name, e.g. <c>kSlots_Foo</c>.</param>
/// <param name="Declaration">A complete <c>extern</c> declaration for the
/// symbol, including its type, ready to emit into the contract header.</param>
/// <param name="NeedsExternalLinkage">
/// True when the definition must drop <c>static</c>. Anonymous-struct-typed
/// symbols are declared through a placeholder type (consumers only take their
/// address and cast to <c>void*</c>), so they keep external linkage but still
/// need the declaration.
/// </param>
public sealed record CrossSectionSymbol(
    string Name,
    string Declaration,
    bool NeedsExternalLinkage);

/// <summary>
/// Splits page-0 payload into bounded translation units.
///
/// <para>
/// Splitting is done on <b>section boundaries</b>: a section is never divided
/// across two TUs by the size budget alone, because sections carry internal
/// definition/reference relationships (a table and the descriptor that points at
/// it). A section larger than the budget on its own is emitted as its own
/// oversized TU rather than being cut — cutting it would silently break those
/// relationships, and the guard tests assert content conservation.
/// </para>
/// </summary>
public static class PayloadSectionPartitioner
{
    /// <summary>
    /// Maximum number of characters to accumulate into one translation unit.
    /// Mirrors <c>sizeThresholdChars</c> in <c>NativeAotEmitter.Shared.cs</c>,
    /// whose comment documents MSVC C1060 avoidance as its purpose.
    /// </summary>
    public const int DefaultBudgetChars = 350_000;

    /// <summary>
    /// Groups sections into translation units, respecting the budget.
    ///
    /// <para>
    /// Sections are kept whole and in their original order. A section that would
    /// push the current TU over budget starts a new TU instead; a section that is
    /// itself larger than the budget gets a TU to itself (never split).
    /// </para>
    /// </summary>
    public static IReadOnlyList<IReadOnlyList<PayloadSection>> Partition(
        IReadOnlyList<PayloadSection> sections,
        int budgetChars = DefaultBudgetChars)
    {
        ArgumentNullException.ThrowIfNull(sections);
        if (budgetChars <= 0)
            throw new ArgumentOutOfRangeException(nameof(budgetChars), budgetChars,
                "budget must be positive");

        var groups = new List<IReadOnlyList<PayloadSection>>();
        if (sections.Count == 0)
            return groups;

        var ordered = new List<PayloadSection>(sections);
        ordered.Sort(static (a, b) =>
        {
            int byOrder = a.Order.CompareTo(b.Order);
            return byOrder != 0 ? byOrder : string.CompareOrdinal(a.Name, b.Name);
        });

        var current = new List<PayloadSection>();
        long currentSize = 0;

        foreach (var section in ordered)
        {
            long size = section.Content?.Length ?? 0;

            // Start a new TU when adding this section would exceed the budget,
            // but never emit an empty TU: an oversized section simply gets its
            // own group (the `current.Count > 0` guard).
            if (current.Count > 0 && currentSize + size > budgetChars)
            {
                groups.Add(current);
                current = new List<PayloadSection>();
                currentSize = 0;
            }

            current.Add(section);
            currentSize += size;
        }

        if (current.Count > 0)
            groups.Add(current);

        return groups;
    }

    /// <summary>
    /// Derives the emitted file name for a payload TU. Kept inside the
    /// <c>native-aot.*.page-*.cpp</c> family so both the flat CMake glob
    /// (<c>subjects/*.cpp</c>) and the non-flat fallback patterns match it.
    /// </summary>
    public static string TranslationUnitFileName(string groupName, int index)
        => $"native-aot.{groupName}.page-{index + 1:D4}.cpp";

    /// <summary>
    /// Concatenates a group's sections back into their emitted text.
    /// </summary>
    public static string Render(IReadOnlyList<PayloadSection> group)
    {
        ArgumentNullException.ThrowIfNull(group);
        var sb = new StringBuilder();
        RenderTo(sb, group);
        return sb.ToString();
    }

    /// <summary>
    /// Appends a group's sections to an existing builder (the sections in order,
    /// each followed by a newline). Kept separate from <see cref="Render"/> so
    /// the emitter can wrap the payload text in a translation-unit preamble
    /// without an intermediate full-string copy.
    /// </summary>
    public static void RenderTo(StringBuilder sb, IReadOnlyList<PayloadSection> group)
    {
        ArgumentNullException.ThrowIfNull(sb);
        ArgumentNullException.ThrowIfNull(group);
        foreach (var section in group)
        {
            sb.Append(section.Content);
            sb.Append('\n');
        }
    }
}
