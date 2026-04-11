namespace Chaos.IL2CPP.HotUpdate;

public static class PackageValidator
{
    public static void ValidateCompatibleTargetAotVersion(LoadedHotUpdatePackage package, string currentAotVersion)
    {
        ArgumentNullException.ThrowIfNull(package);
        ArgumentException.ThrowIfNullOrWhiteSpace(currentAotVersion);

        var targetAotVersion = package.Manifest.TargetAotVersion;
        if (!string.Equals(targetAotVersion, currentAotVersion, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"hot update package target AOT version '{targetAotVersion}' is not compatible with runtime AOT version '{currentAotVersion}'.");
        }

        if (string.IsNullOrWhiteSpace(package.Manifest.Signature))
        {
            throw new InvalidOperationException("hot update package signature must not be empty.");
        }
    }
}
