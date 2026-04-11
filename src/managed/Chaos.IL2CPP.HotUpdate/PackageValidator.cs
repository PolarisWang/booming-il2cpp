namespace Chaos.IL2CPP.HotUpdate;

public static class PackageValidator
{
    public static void ValidateCompatibleTargetAotVersion(LoadedHotUpdatePackage package, string currentAotVersion)
    {
        ArgumentNullException.ThrowIfNull(package);
        ArgumentException.ThrowIfNullOrWhiteSpace(currentAotVersion);

        var targetAotVersion = package.Manifest.TargetAotVersion;
        if (!HotUpdateSemanticVersion.TryParse(currentAotVersion, out var runtimeVersion))
        {
            throw new InvalidOperationException(
                $"runtime AOT version '{currentAotVersion}' is not a valid semantic version. Expected 'major.minor.patch'.");
        }

        if (!HotUpdateSemanticVersion.TryParse(targetAotVersion, out var targetVersion))
        {
            throw new InvalidOperationException(
                $"hot update package target AOT version '{targetAotVersion}' is not a valid semantic version. Expected 'major.minor.patch'.");
        }

        if (!targetVersion.IsCompatibleWith(runtimeVersion))
        {
            throw new InvalidOperationException(
                $"hot update package target AOT version '{targetAotVersion}' is not compatible with runtime AOT version '{currentAotVersion}'. Expected matching major.minor compatibility band.");
        }

        if (string.IsNullOrWhiteSpace(package.Manifest.Signature))
        {
            throw new InvalidOperationException("hot update package signature must not be empty.");
        }
    }
}
