using System;
using System.IO;
using System.Text.Json;

namespace Chaos.TestFramework.Runtime;

/// <summary>
/// Loads and validates compact hotupdate binding manifests.
/// </summary>
public static class ChaosHotUpdateBindingManifestLoader
{
    private static readonly JsonSerializerOptions JsonOptions = new(JsonSerializerDefaults.Web)
    {
        PropertyNameCaseInsensitive = true,
    };

    /// <summary>
    /// Loads a binding manifest from disk.
    /// </summary>
    public static ChaosHotUpdateBindingManifest Load(string bindingManifestPath)
    {
        if (string.IsNullOrWhiteSpace(bindingManifestPath))
        {
            throw new ArgumentException("binding manifest path is required.", nameof(bindingManifestPath));
        }

        if (!File.Exists(bindingManifestPath))
        {
            throw new FileNotFoundException("binding manifest is missing.", bindingManifestPath);
        }

        var manifest = JsonSerializer.Deserialize<ChaosHotUpdateBindingManifest>(
            File.ReadAllText(bindingManifestPath),
            JsonOptions);
        return manifest ?? new ChaosHotUpdateBindingManifest();
    }
}
