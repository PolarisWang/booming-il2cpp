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

    /// <summary>
    /// Whether this section may be divided across translation units, and if so
    /// along which boundaries.
    ///
    /// <para>
    /// <b>Why the producer decides, not the pager.</b> Whether a piece of
    /// generated text can be cut is a property of what it <i>means</i>, not of
    /// how long it is: a vtable array and the VTableSlot table that indexes it
    /// must stay together, a dispatch chain's branches are independently
    /// reachable, and the GC slot map is scanned by the runtime as one
    /// contiguous byte range and cannot be cut at all. That knowledge lives
    /// with whoever emitted the text. Expressing it here turns a per-section
    /// special case in the pager into data the producer supplies.
    /// </para>
    ///
    /// <para>
    /// Default is <see cref="SectionSemantics.Atomic"/> — the conservative
    /// choice, and the behaviour every existing section already has. A section
    /// that can be cut must say so explicitly, using
    /// <see cref="Units"/> to give the allowed boundaries.
    /// </para>
    /// </summary>
    public SectionSemantics Semantics { get; init; } = SectionSemantics.Atomic;

    /// <summary>
    /// The allowed cut points, in emission order, when <see cref="Semantics"/>
    /// is <see cref="SectionSemantics.ByUnits"/>.
    ///
    /// <para>
    /// Each unit is a self-contained fragment: the concatenation of all units,
    /// in order, must equal <see cref="Content"/> exactly. That equality is the
    /// contract — a unit list that does not reassemble into the section is a
    /// producer bug, and the pager asserts it rather than emitting text that
    /// silently lost a fragment.
    /// </para>
    ///
    /// <para>
    /// The units are what makes a large section partitionable without
    /// estimating its size: the pager accumulates real unit lengths instead of
    /// guessing from a character-per-item formula (measured: the hotpatch
    /// estimate ran 61% low).
    /// </para>
    /// </summary>
    public IReadOnlyList<SectionUnit> Units { get; init; }
        = Array.Empty<SectionUnit>();
}

/// <summary>
/// One allowed cut point within a <see cref="PayloadSection"/>.
///
/// <para>
/// A unit carries its own name so the emitted TU and the split report can say
/// which fragment landed where — "payload.7 carries vtable.3" is actionable in
/// a way "payload.7 is 400 KB" is not.
/// </para>
/// </summary>
/// <param name="Name">Fragment identifier, unique within its section.</param>
/// <param name="Content">The fragment's C++ text.</param>
public sealed record SectionUnit(string Name, string Content);

/// <summary>
/// How a <see cref="PayloadSection"/> may be divided across TUs.
/// </summary>
public enum SectionSemantics
{
    /// <summary>
    /// Never divided. Correct whenever the text carries internal
    /// definition/reference relationships that a cut would break, and required
    /// whenever the runtime consumes it as one contiguous range (the GC slot
    /// map is scanned by advancing <c>entry_total_size</c>). A section larger
    /// than the budget under this semantic produces an oversized TU — which is
    /// why the split report calls those out rather than letting MSVC find them.
    /// </summary>
    Atomic = 0,

    /// <summary>
    /// Divisible, but only at the boundaries listed in <see cref="PayloadSection.Units"/>.
    /// Units are never merged across a boundary the producer did not offer.
    /// </summary>
    ByUnits = 1,
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

        // Expand the ordered sections into the finest pieces the producer
        // allowed: an Atomic section stays whole, a ByUnits section becomes one
        // piece per declared unit. Everything downstream then packs uniform
        // pieces, and the "a section is never divided" rule becomes "a PIECE is
        // never divided" — which the producer has already guaranteed is safe.
        var pieces = new List<PayloadSection>();
        foreach (var section in ordered)
        {
            if (section.Semantics == SectionSemantics.ByUnits && section.Units.Count > 0)
            {
                VerifyUnitsReassemble(section);
                for (int i = 0; i < section.Units.Count; i++)
                {
                    var unit = section.Units[i];
                    pieces.Add(new PayloadSection
                    {
                        Name = $"{section.Name}.{i}",
                        Content = unit.Content,
                        Order = section.Order,
                        ExportedSymbols = section.ExportedSymbols,
                        // The piece inherits the parent's atomicity: its units
                        // were the finest boundaries the producer offered, so
                        // nothing below this is safe to cut.
                        Semantics = SectionSemantics.Atomic,
                    });
                }
            }
            else
            {
                pieces.Add(section);
            }
        }

        var current = new List<PayloadSection>();
        long currentSize = 0;

        foreach (var piece in pieces)
        {
            long size = piece.Content?.Length ?? 0;

            // Start a new TU when adding this piece would exceed the budget,
            // but never emit an empty TU: an oversized piece simply gets its
            // own group (the `current.Count > 0` guard).
            if (current.Count > 0 && currentSize + size > budgetChars)
            {
                groups.Add(current);
                current = new List<PayloadSection>();
                currentSize = 0;
            }

            current.Add(piece);
            currentSize += size;
        }

        if (current.Count > 0)
            groups.Add(current);

        return groups;
    }

    /// <summary>
    /// Asserts the <see cref="SectionSemantics.ByUnits"/> contract: the units,
    /// concatenated in order, reproduce the section's content exactly.
    ///
    /// <para>
    /// Partitioning replaces the section's text with its units, so a unit list
    /// that does not reassemble would silently DROP or DUPLICATE generated
    /// code — the failure would surface far away, as a missing symbol or a
    /// duplicate definition, with nothing pointing back here. Failing loudly at
    /// the producer's own section is the only place the cause is still visible.
    /// </para>
    ///
    /// <para>
    /// Throws rather than returning a status: this is a codegen bug, not a
    /// runtime condition, and a partially-correct payload is worse than a
    /// failed build.
    /// </para>
    /// </summary>
    private static void VerifyUnitsReassemble(PayloadSection section)
    {
        long expected = 0;
        for (int i = 0; i < section.Units.Count; i++)
            expected += section.Units[i].Content?.Length ?? 0;

        long actual = section.Content?.Length ?? 0;
        // Length is the cheap check and catches the common cases (a fragment
        // dropped, given twice, or truncated). The exact comparison below is
        // what catches a fragment that changed without changing the total.
        if (expected == actual)
        {
            var sb = new StringBuilder((int)Math.Min(expected, int.MaxValue));
            foreach (var unit in section.Units)
                sb.Append(unit.Content);
            if (string.Equals(sb.ToString(), section.Content, StringComparison.Ordinal))
                return;
        }

        throw new InvalidOperationException(
            $"Section '{section.Name}' declares {section.Units.Count} unit(s) whose "
            + $"contents do not reassemble into its Content "
            + $"(units total {expected} chars, section is {actual}). "
            + "Partitioning would replace the section text with these units, so the "
            + "mismatch means generated code would be dropped or duplicated.");
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
