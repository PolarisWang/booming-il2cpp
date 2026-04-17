using System;

namespace Chaos.TestFramework.Runtime;

/// <summary>
/// Parses compact managed host arguments.
/// </summary>
public static class ChaosManagedHostArguments
{
    /// <summary>
    /// Prefix for the collection-path argument.
    /// </summary>
    public const string CollectionPathPrefix = "--collection-path=";

    /// <summary>
    /// Prefix for the entry-index argument.
    /// </summary>
    public const string EntryIndexPrefix = "--entry-index=";

    /// <summary>
    /// Parses managed host arguments.
    /// </summary>
    public static ChaosManagedHostRequest Parse(string[]? args)
    {
        string? collectionPath = null;
        int? entryIndex = null;

        foreach (var argument in args ?? Array.Empty<string>())
        {
            if (argument.StartsWith(CollectionPathPrefix, StringComparison.Ordinal))
            {
                collectionPath = argument[CollectionPathPrefix.Length..];
                continue;
            }

            if (argument.StartsWith(EntryIndexPrefix, StringComparison.Ordinal))
            {
                var valueText = argument[EntryIndexPrefix.Length..];
                if (!int.TryParse(valueText, out var parsedEntryIndex) || parsedEntryIndex < 0)
                {
                    throw new ArgumentException($"invalid managed host entry index: {valueText}", nameof(args));
                }

                entryIndex = parsedEntryIndex;
            }
        }

        if (string.IsNullOrWhiteSpace(collectionPath))
        {
            throw new ArgumentException("managed host requires --collection-path.", nameof(args));
        }

        if (entryIndex is null)
        {
            throw new ArgumentException("managed host requires --entry-index.", nameof(args));
        }

        return new ChaosManagedHostRequest(collectionPath, entryIndex.Value);
    }
}
