using System.Text;

namespace Chaos.IL2CPP.HotUpdate;

public sealed record IosComplianceValidationOptions
{
    public long MaxPackageBytes { get; init; } = 16L * 1024L * 1024L;
}

public static class IosComplianceValidator
{
    private static readonly string[] ForbiddenNativeFileExtensions =
    [
        ".dylib",
        ".so",
        ".framework",
    ];

    private static readonly string[] RestrictedApiMarkers =
    [
        "dlopen",
        "dlsym",
        "mmap",
    ];

    public static void Validate(LoadedHotUpdatePackage package, IosComplianceValidationOptions? options = null)
    {
        ArgumentNullException.ThrowIfNull(package);

        options ??= new IosComplianceValidationOptions();
        if (options.MaxPackageBytes <= 0)
        {
            throw new ArgumentOutOfRangeException(nameof(options), "MaxPackageBytes must be greater than zero.");
        }

        var packageRoot = Path.GetFullPath(package.RootPath);
        if (!Directory.Exists(packageRoot))
        {
            throw new DirectoryNotFoundException($"hot update package root missing: {packageRoot}");
        }

        var packageFiles = Directory.GetFiles(packageRoot, "*", SearchOption.AllDirectories);
        ValidatePackageSize(packageFiles, options);
        ValidateNoNativePayloads(packageRoot, packageFiles);
        ValidateNoRestrictedApis(package, packageFiles);
    }

    private static void ValidatePackageSize(IEnumerable<string> packageFiles, IosComplianceValidationOptions options)
    {
        long totalBytes = 0;
        foreach (var filePath in packageFiles)
        {
            totalBytes += new FileInfo(filePath).Length;
        }

        if (totalBytes > options.MaxPackageBytes)
        {
            throw new InvalidOperationException(
                $"iOS hot update package size {totalBytes} exceeds package size limit {options.MaxPackageBytes} bytes.");
        }
    }

    private static void ValidateNoNativePayloads(string packageRoot, IEnumerable<string> packageFiles)
    {
        foreach (var filePath in packageFiles)
        {
            var relativePath = Path.GetRelativePath(packageRoot, filePath);
            var nativeExtension = GetForbiddenNativeExtension(relativePath);
            if (nativeExtension is not null)
            {
                throw new InvalidOperationException(
                    $"iOS hot update package must not distribute native executable code; found '{relativePath}' with forbidden native artifact '{nativeExtension}'.");
            }
        }
    }

    private static void ValidateNoRestrictedApis(LoadedHotUpdatePackage package, IEnumerable<string> packageFiles)
    {
        foreach (var assembly in package.LoadedAssemblies.Values)
        {
            var restrictedApi = FindRestrictedApiMarker(assembly.Bytes);
            if (restrictedApi is not null)
            {
                throw new InvalidOperationException(
                    $"iOS hot update package references restricted native API '{restrictedApi}' in managed payload '{assembly.Name}'.");
            }
        }

        foreach (var filePath in packageFiles)
        {
            var contents = File.ReadAllBytes(filePath);
            var restrictedApi = FindRestrictedApiMarker(contents);
            if (restrictedApi is not null)
            {
                var relativePath = Path.GetRelativePath(package.RootPath, filePath);
                throw new InvalidOperationException(
                    $"iOS hot update package references restricted native API '{restrictedApi}' in package file '{relativePath}'.");
            }
        }
    }

    private static string? GetForbiddenNativeExtension(string relativePath)
    {
        foreach (var nativeExtension in ForbiddenNativeFileExtensions)
        {
            if (relativePath.EndsWith(nativeExtension, StringComparison.OrdinalIgnoreCase))
            {
                return nativeExtension;
            }

            var normalizedPath = relativePath.Replace('\\', '/');
            if (normalizedPath.Contains($"{nativeExtension}/", StringComparison.OrdinalIgnoreCase))
            {
                return nativeExtension;
            }
        }

        return null;
    }

    private static string? FindRestrictedApiMarker(byte[] bytes)
    {
        var content = Encoding.UTF8.GetString(bytes);
        foreach (var marker in RestrictedApiMarkers)
        {
            if (content.Contains(marker, StringComparison.OrdinalIgnoreCase))
            {
                return marker;
            }
        }

        return null;
    }
}
