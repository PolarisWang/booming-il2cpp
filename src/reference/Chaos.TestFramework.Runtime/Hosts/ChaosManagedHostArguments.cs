using System;

namespace Chaos.TestFramework.Runtime;

/// <summary>
/// Parses compact managed host arguments.
/// </summary>
public static class ChaosManagedHostArguments
{
    /// <summary>
    /// Prefix for the host-kind argument.
    /// </summary>
    public const string HostKindPrefix = "--host-kind=";

    /// <summary>
    /// Prefix for the collection-path argument.
    /// </summary>
    public const string CollectionPathPrefix = "--collection-path=";

    /// <summary>
    /// Prefix for the entry-index argument.
    /// </summary>
    public const string EntryIndexPrefix = "--entry-index=";

    /// <summary>
    /// Parses the host kind for the shared managed runtime host.
    /// </summary>
    public static ChaosManagedHostKind ParseHostKind(string[]? args)
    {
        foreach (var argument in args ?? Array.Empty<string>())
        {
            if (!argument.StartsWith(HostKindPrefix, StringComparison.Ordinal))
            {
                continue;
            }

            var value = argument[HostKindPrefix.Length..];
            if (string.Equals(value, "proof", StringComparison.OrdinalIgnoreCase))
            {
                return ChaosManagedHostKind.Proof;
            }

            if (string.Equals(value, "benchmark", StringComparison.OrdinalIgnoreCase))
            {
                return ChaosManagedHostKind.Benchmark;
            }

            throw new ArgumentException($"invalid managed host kind: {value}", nameof(args));
        }

        throw new ArgumentException("managed host requires --host-kind.", nameof(args));
    }

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
