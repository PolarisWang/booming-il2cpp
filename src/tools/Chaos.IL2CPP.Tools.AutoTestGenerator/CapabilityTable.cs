using System.Text.Json;

namespace Chaos.IL2CPP.Tools.AutoTestGenerator;

/// <summary>
/// Loads the codegen capability table (see
/// <c>tests/e2e/verification/tools/export_capability_table.py</c>).
///
/// The table is derived from the AOT codegen's shape registry and answers one
/// question per managed method: <em>does the codegen have a native dispatch
/// path for this method?</em>  The ATG consults it before injecting a semantic
/// (valid) argument, because injecting a valid input for a method with no
/// implementation turns an honest smoke gap into a permanent, unfixable
/// failure.
/// </summary>
public static class CapabilityTable
{
    /// <summary>Shape of capability-table.json as written by the Python exporter.</summary>
    private sealed class TableFile
    {
        public Dictionary<string, Entry>? Real { get; set; }
    }

    private sealed class Entry
    {
        public string? Status { get; set; }
    }

    /// <summary>
    /// Load the table from <paramref name="path"/>.
    ///
    /// Returns null — meaning "no gating" — when the path is absent, unreadable,
    /// or malformed.  Degrading to the un-gated behaviour is deliberate: a
    /// missing table must never silently suppress injection entirely, or a
    /// pipeline run without a prior codegen pass would lose all semantic value
    /// sets instead of merely losing the guard.
    /// </summary>
    public static IReadOnlyDictionary<string, string>? Load(string? path)
    {
        if (string.IsNullOrEmpty(path) || !File.Exists(path))
            return null;

        try
        {
            // utf-8-sig equivalent: the codegen writes the manifest with a BOM.
            var json = File.ReadAllText(path).TrimStart('﻿');
            var parsed = JsonSerializer.Deserialize<TableFile>(json,
                new JsonSerializerOptions { PropertyNameCaseInsensitive = true });

            if (parsed?.Real is null || parsed.Real.Count == 0)
                return null;

            var result = new Dictionary<string, string>(StringComparer.Ordinal);
            foreach (var (key, entry) in parsed.Real)
            {
                if (!string.IsNullOrEmpty(key))
                    result[key] = entry?.Status ?? "real";
            }
            return result.Count > 0 ? result : null;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine(
                $"[WARN] capability table {path} could not be read ({ex.Message}); " +
                "semantic value injection runs un-gated.");
            return null;
        }
    }
}
