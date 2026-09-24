using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Records how page-0's payload was divided into translation units, and — for
/// each TU — <b>why the division fell where it did</b>.
///
/// <para>
/// <b>Why this exists.</b> Every MSVC capacity failure in this area (C1002,
/// C1060) arrives as a compiler message naming one generated file, with no
/// indication of what that file contains or which decision produced it. The
/// evidence needed to diagnose it — the unit list, the accumulated size at each
/// flush, the reason for the flush — is computed during emission and then
/// discarded. This report keeps it.
/// </para>
///
/// <para>
/// <b>Determinism.</b> The division is a pure function of (unit list, budget,
/// flush rules), so the same inputs always yield the same TUs. That makes the
/// report reproducible: re-running the packer and comparing against a recorded
/// report is a meaningful check, and a guard test does exactly that.
/// </para>
///
/// <para>
/// <b>Volume.</b> At system-chunk scale a TU carries hundreds of units, so the
/// unit-level detail is opt-in via <c>CHAOS_SPLIT_REPORT=verbose</c>. The
/// default records per-TU summaries, which is what locating a bad TU needs.
/// </para>
/// </summary>
internal static class SplitReportBuilder
{
    /// <summary>Environment switch enabling per-unit detail in the report.</summary>
    internal const string VerboseEnvVar = "CHAOS_SPLIT_REPORT";

    /// <summary>Relative path of the emitted report, alongside the generated sources.</summary>
    public const string ReportRelativePath = "native-aot.split-report.json";

    /// <summary>Tool-reader summary, one line per TU, emitted next to the JSON.</summary>
    public const string LogRelativePath = "native-aot.split-report.log";

    internal sealed record TuRecord(
        string Id,
        string Path,
        long AccumulatedChars,
        int UnitCount,
        string FlushReason,
        IReadOnlyList<UnitRecord> Units);

    internal sealed record UnitRecord(string Kind, string Name, long Size);

    /// <summary>
    /// Builds the report from the emitted TU files.
    ///
    /// <para>
    /// <paramref name="tuFiles"/> is the list of payload translation units in
    /// emission order, each with the sections it carries. It is supplied by the
    /// emitter rather than re-derived here, so the report cannot disagree with
    /// what was actually written.
    /// </para>
    /// </summary>
    public static (string Json, string Log) Build(
        IReadOnlyList<TuRecord> tus,
        long targetBudgetChars,
        IReadOnlyDictionary<string, long> unitInputSizes)
    {
        bool verbose = IsVerbose();

        var payload = new Dictionary<string, object?>
        {
            ["packerVersion"] = "TuPacker.V0-observe-only",
            ["note"] = "Step 1 of the unified-packer work: this report OBSERVES the "
                     + "existing division. It does not influence it.",
            ["tuBudget"] = new Dictionary<string, object?>
            {
                ["targetChars"] = targetBudgetChars,
            },
            ["unitInputSizes"] = unitInputSizes,
            ["tuCount"] = tus.Count,
            ["tuList"] = tus.Select(t => new Dictionary<string, object?>
            {
                ["id"] = t.Id,
                ["path"] = t.Path,
                ["accumulatedChars"] = t.AccumulatedChars,
                ["unitCount"] = t.UnitCount,
                ["flushReason"] = t.FlushReason,
                // Unit detail is what turns "this TU is 7.8 MB" into "these five
                // sections make it 7.8 MB"; opt-in because it is large.
                ["units"] = verbose
                    ? t.Units.Select(u => new Dictionary<string, object?>
                      {
                          ["kind"] = u.Kind,
                          ["name"] = u.Name,
                          ["size"] = u.Size,
                      }).ToArray()
                    : null,
            }).ToArray(),
            ["oversizedUnits"] = unitInputSizes
                .Where(kv => kv.Value > targetBudgetChars)
                .Select(kv => new Dictionary<string, object?>
                {
                    ["name"] = kv.Key,
                    ["size"] = kv.Value,
                    ["overBudgetBy"] = kv.Value - targetBudgetChars,
                })
                .ToArray(),
        };

        string json = JsonSerializer.Serialize(payload,
            new JsonSerializerOptions { WriteIndented = true });

        // Human-readable companion. Reviewers read this far more often than the JSON.
        var log = new StringBuilder();
        log.AppendLine($"TU split report — {tus.Count} translation unit(s), "
                     + $"budget {targetBudgetChars:N0} chars");
        log.AppendLine(new string('-', 78));
        foreach (var t in tus)
        {
            log.AppendLine($"{t.Id,-16} {t.AccumulatedChars,12:N0} chars  "
                         + $"{t.UnitCount,5} unit(s)  [{t.FlushReason}]");
            if (verbose)
            {
                foreach (var u in t.Units)
                    log.AppendLine($"      {u.Size,12:N0}  {u.Kind,-10} {u.Name}");
            }
        }
        if (unitInputSizes.Any(kv => kv.Value > targetBudgetChars))
        {
            log.AppendLine(new string('-', 78));
            log.AppendLine("UNITS EXCEEDING BUDGET (cannot be split by the pager):");
            foreach (var kv in unitInputSizes.Where(kv => kv.Value > targetBudgetChars))
                log.AppendLine($"  {kv.Value,12:N0}  {kv.Key}  "
                             + $"(over by {kv.Value - targetBudgetChars:N0})");
        }

        return (json, log.ToString());
    }

    private static bool IsVerbose()
    {
        string? v = Environment.GetEnvironmentVariable(VerboseEnvVar);
        return !string.IsNullOrEmpty(v) && !string.Equals(v, "0", StringComparison.Ordinal);
    }
}
