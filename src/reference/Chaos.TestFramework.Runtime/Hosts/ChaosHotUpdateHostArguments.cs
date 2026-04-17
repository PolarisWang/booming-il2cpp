using System;

namespace Chaos.TestFramework.Runtime;

/// <summary>
/// Parses compact hotupdate host arguments.
/// </summary>
public static class ChaosHotUpdateHostArguments
{
    /// <summary>
    /// Prefix for the binding-manifest-path argument.
    /// </summary>
    public const string BindingManifestPathPrefix = "--binding-manifest-path=";

    /// <summary>
    /// Parses hotupdate host arguments.
    /// </summary>
    public static ChaosHotUpdateHostRequest Parse(string[]? args)
    {
        var managedRequest = ChaosManagedHostArguments.Parse(args);
        string? bindingManifestPath = null;

        foreach (var argument in args ?? Array.Empty<string>())
        {
            if (!argument.StartsWith(BindingManifestPathPrefix, StringComparison.Ordinal))
            {
                continue;
            }

            bindingManifestPath = argument[BindingManifestPathPrefix.Length..];
        }

        if (string.IsNullOrWhiteSpace(bindingManifestPath))
        {
            throw new ArgumentException("hotupdate host requires --binding-manifest-path.", nameof(args));
        }

        return new ChaosHotUpdateHostRequest(
            managedRequest.CollectionPath,
            bindingManifestPath,
            managedRequest.EntryIndex);
    }
}

/// <summary>
/// Represents a parsed hotupdate host execution request.
/// </summary>
public readonly record struct ChaosHotUpdateHostRequest(
    string CollectionPath,
    string BindingManifestPath,
    int EntryIndex);
