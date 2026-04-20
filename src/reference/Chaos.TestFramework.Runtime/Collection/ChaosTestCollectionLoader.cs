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
    private const int SupportedSchemaVersion = 1;

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
        collection ??= new ChaosTestCollection();
        if (collection.SchemaVersion != SupportedSchemaVersion)
        {
            throw new InvalidOperationException(
                $"collection '{collectionPath}' has unsupported collection schemaVersion {collection.SchemaVersion}; expected {SupportedSchemaVersion}.");
        }

        return collection;
    }

    /// <summary>
    /// Ensures the requested entry exists in the collection file.
    /// </summary>
    public static void EnsureEntryExists(string collectionPath, ChaosManagedHostKind hostKind, int entryIndex)
    {
        ResolveEntry(collectionPath, hostKind, entryIndex);
    }

    /// <summary>
    /// Resolves a collection entry by host kind and entry index.
    /// </summary>
    public static ChaosTestCollectionEntry ResolveEntry(string collectionPath, ChaosManagedHostKind hostKind, int entryIndex)
    {
        if (entryIndex < 0)
        {
            throw new ArgumentOutOfRangeException(nameof(entryIndex), entryIndex, "entry index must be non-negative.");
        }

        var collection = Load(collectionPath);
        var entries = hostKind == ChaosManagedHostKind.Proof
            ? collection.DeclaredUnitTests
            : collection.DeclaredBenchmarks;
        var entry = entries.FirstOrDefault(candidate => candidate.EntryIndex == entryIndex);
        if (entry is not null)
        {
            return entry;
        }

        throw new InvalidOperationException(
            $"collection '{collectionPath}' does not contain {hostKind} entry index {entryIndex}.");
    }
}
