using System;
using System.IO;
using System.Linq;
using System.Text.Json;

namespace Chaos.TestFramework.Runtime;

/// <summary>
/// Loads and validates compact test collection files.
/// </summary>
public static class ChaosTestCollectionLoader
{
    private static readonly JsonSerializerOptions JsonOptions = new(JsonSerializerDefaults.Web)
    {
        PropertyNameCaseInsensitive = true,
    };

    /// <summary>
    /// Loads a collection file from disk.
    /// </summary>
    public static ChaosTestCollection Load(string collectionPath)
    {
        if (string.IsNullOrWhiteSpace(collectionPath))
        {
            throw new ArgumentException("collection path is required.", nameof(collectionPath));
        }

        if (!File.Exists(collectionPath))
        {
            throw new FileNotFoundException("collection file is missing.", collectionPath);
        }

        var collection = JsonSerializer.Deserialize<ChaosTestCollection>(
            File.ReadAllText(collectionPath),
            JsonOptions);
        return collection ?? new ChaosTestCollection();
    }

    /// <summary>
    /// Ensures the requested entry exists in the collection file.
    /// </summary>
    public static void EnsureEntryExists(string collectionPath, ChaosManagedHostKind hostKind, int entryIndex)
    {
        if (entryIndex < 0)
        {
            throw new ArgumentOutOfRangeException(nameof(entryIndex), entryIndex, "entry index must be non-negative.");
        }

        var collection = Load(collectionPath);
        var entries = hostKind == ChaosManagedHostKind.Proof
            ? collection.DeclaredUnitTests
            : collection.DeclaredBenchmarks;
        if (entries.Any(entry => entry.EntryIndex == entryIndex))
        {
            return;
        }

        throw new InvalidOperationException(
            $"collection '{collectionPath}' does not contain {hostKind} entry index {entryIndex}.");
    }
}
