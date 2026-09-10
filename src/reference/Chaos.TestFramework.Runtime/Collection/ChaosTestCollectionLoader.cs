using Chaos.TestFramework.Runtime.Collection;

namespace Chaos.TestFramework.Runtime.Collection;

/// <summary>
/// Materializes <see cref="ChaosTestCollection"/> entries from a collection manifest.
/// Validates the collection's schemaVersion; an any unsupported version is rejected with
/// "unsupported collection schemaVersion".
/// </summary>
public static class ChaosTestCollectionLoader
{
    public const int SupportedSchemaVersion = 1;

    public static ChaosTestCollection Load(ChaosTestCollection source)
    {
        if (source.SchemaVersion != SupportedSchemaVersion)
        {
            throw new System.InvalidOperationException(
                $"unsupported collection schemaVersion: {source.SchemaVersion}");
        }
        return source;
    }
}
