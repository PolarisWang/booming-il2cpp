using System.Text.Json;

namespace Chaos.IL2CPP.HotUpdate;

public sealed record HotUpdateIntegrityReport
{
    public required IReadOnlyList<string> ActivePatches { get; init; }

    public required IReadOnlyList<string> Issues { get; init; }

    public bool IsValid => Issues.Count == 0;
}

public enum RuntimeMode
{
    Aot = 0,
    Mixed = 1,
}

public sealed class RuntimeManager
{
    private readonly HotUpdateMethodRegistry _methodRegistry = new();
    private readonly SupplementalMetadataLoader _supplementalMetadataLoader = new();
    private readonly Stack<RuntimeSnapshot> _history = new();
    private string? _currentAotVersion;

    public RuntimeMode Mode { get; private set; } = RuntimeMode.Aot;

    public bool IsMixedMode => Mode == RuntimeMode.Mixed;

    public bool CanRollback => _history.Count > 0;

    public LoadedHotUpdatePackage? LoadedPackage { get; private set; }

    public string? LastError { get; private set; }

    public bool LoadPackage(
        string packageRootPath,
        string currentAotVersion,
        IReadOnlyDictionary<string, int>? subjectIdToConstantInt32 = null,
        IReadOnlyDictionary<string, Func<int, int>>? subjectIdToInt32Unary = null,
        IReadOnlyDictionary<string, Func<IReadOnlyList<object?>, object?>>? subjectIdToGeneric = null)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(packageRootPath);

        try
        {
            var package = PackageReader.ReadFromDirectory(packageRootPath);
            LoadPackage(
                package,
                currentAotVersion,
                subjectIdToConstantInt32 ?? new Dictionary<string, int>(StringComparer.Ordinal),
                subjectIdToInt32Unary,
                subjectIdToGeneric);
            return true;
        }
        catch (Exception exception) when (exception is InvalidOperationException
                                         or InvalidDataException
                                         or FileNotFoundException
                                         or DirectoryNotFoundException
                                         or JsonException)
        {
            LastError = exception.Message;
            return false;
        }
    }

    public void LoadPackage(
        LoadedHotUpdatePackage package,
        string currentAotVersion,
        IReadOnlyDictionary<string, int> subjectIdToConstantInt32,
        IReadOnlyDictionary<string, Func<int, int>>? subjectIdToInt32Unary = null,
        IReadOnlyDictionary<string, Func<IReadOnlyList<object?>, object?>>? subjectIdToGeneric = null)
    {
        ArgumentNullException.ThrowIfNull(package);
        ArgumentNullException.ThrowIfNull(subjectIdToConstantInt32);

        _history.Push(CaptureSnapshot());
        PackageValidator.ValidateCompatibleTargetAotVersion(package, currentAotVersion);

        _methodRegistry.Clear();
        foreach (var pair in subjectIdToConstantInt32)
        {
            _methodRegistry.RegisterConstantInt32(pair.Key, pair.Value);
        }

        if (subjectIdToInt32Unary is not null)
        {
            foreach (var pair in subjectIdToInt32Unary)
            {
                RegisterInt32Unary(pair.Key, pair.Value);
            }
        }

        if (subjectIdToGeneric is not null)
        {
            foreach (var pair in subjectIdToGeneric)
            {
                RegisterMethod(pair.Key, pair.Value);
            }
        }

        LoadedPackage = package;
        _currentAotVersion = currentAotVersion;
        Mode = RuntimeMode.Mixed;
        LastError = null;
    }

    public void UnloadPackage()
    {
        _history.Clear();
        LoadedPackage = null;
        _currentAotVersion = null;
        _methodRegistry.Clear();
        Mode = RuntimeMode.Aot;
        LastError = null;
    }

    public void Rollback()
    {
        if (_history.Count == 0)
        {
            UnloadPackage();
            return;
        }

        RestoreSnapshot(_history.Pop());
    }

    public IReadOnlyList<string> GetActivePatches()
    {
        return LoadedPackage is null
            ? []
            : [LoadedPackage.Manifest.PackageId];
    }

    public HotUpdateIntegrityReport ValidateIntegrity()
    {
        var issues = new List<string>();
        var activePatches = GetActivePatches();

        if (IsMixedMode != (LoadedPackage is not null))
        {
            issues.Add("runtime mode and loaded package state are inconsistent.");
        }

        if (LoadedPackage is not null)
        {
            ValidatePackageCompatibility(LoadedPackage, issues);
            ValidateLoadedAssemblies(LoadedPackage, issues);
            ValidateSupplementalMetadata(LoadedPackage, issues);
        }

        return new HotUpdateIntegrityReport
        {
            ActivePatches = activePatches,
            Issues = issues,
        };
    }

    public int DispatchInt32(string subjectId, Func<int> aotFallback)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(aotFallback);

        if (IsMixedMode && _methodRegistry.TryDispatch(subjectId, Array.Empty<object?>(), out var mixedValue))
        {
            return Convert.ToInt32(mixedValue);
        }

        return aotFallback();
    }

    public void RegisterInt32Unary(string subjectId, Func<int, int> target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(target);
        _methodRegistry.RegisterInt32Unary(subjectId, target);
    }

    public void RegisterMethod(string subjectId, Func<IReadOnlyList<object?>, object?> target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(target);
        _methodRegistry.RegisterMethod(subjectId, target);
    }

    public int DispatchInt32Unary(string subjectId, int value, Func<int, int> aotFallback)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(aotFallback);

        if (IsMixedMode && _methodRegistry.TryDispatch(subjectId, [value], out var mixedValue))
        {
            return Convert.ToInt32(mixedValue);
        }

        return aotFallback(value);
    }

    private RuntimeSnapshot CaptureSnapshot()
    {
        return new RuntimeSnapshot
        {
            LoadedPackage = LoadedPackage,
            CurrentAotVersion = _currentAotVersion,
            Mode = Mode,
            Registry = _methodRegistry.Snapshot(),
        };
    }

    private void RestoreSnapshot(RuntimeSnapshot snapshot)
    {
        ArgumentNullException.ThrowIfNull(snapshot);

        LoadedPackage = snapshot.LoadedPackage;
        _currentAotVersion = snapshot.CurrentAotVersion;
        Mode = snapshot.Mode;
        _methodRegistry.Restore(snapshot.Registry);
        LastError = null;
    }

    private void ValidatePackageCompatibility(LoadedHotUpdatePackage package, List<string> issues)
    {
        if (string.IsNullOrWhiteSpace(_currentAotVersion))
        {
            issues.Add("current AOT version is missing for integrity validation.");
            return;
        }

        try
        {
            PackageValidator.ValidateCompatibleTargetAotVersion(package, _currentAotVersion);
        }
        catch (InvalidOperationException exception)
        {
            issues.Add(exception.Message);
        }
    }

    private static void ValidateLoadedAssemblies(LoadedHotUpdatePackage package, List<string> issues)
    {
        foreach (var assembly in package.Manifest.Assemblies)
        {
            if (!package.LoadedAssemblies.TryGetValue(assembly.Name, out var loadedAssembly))
            {
                issues.Add($"loaded assembly '{assembly.Name}' is missing from runtime registry.");
                continue;
            }

            if (loadedAssembly.Bytes.Length != assembly.Size)
            {
                issues.Add(
                    $"loaded assembly size mismatch for '{assembly.Name}': expected {assembly.Size}, got {loadedAssembly.Bytes.Length}.");
            }

            var inMemoryHash = PackageReader.ComputeFileHash(loadedAssembly.Bytes);
            if (!string.Equals(inMemoryHash, assembly.Hash, StringComparison.Ordinal))
            {
                issues.Add(
                    $"loaded assembly hash mismatch for '{assembly.Name}': expected {assembly.Hash}, got {inMemoryHash}.");
            }

            var assemblyPath = Path.Combine(package.RootPath, assembly.Name);
            if (!File.Exists(assemblyPath))
            {
                issues.Add($"hot update assembly missing on disk: {assemblyPath}");
                continue;
            }

            var onDiskBytes = File.ReadAllBytes(assemblyPath);
            if (onDiskBytes.Length != assembly.Size)
            {
                issues.Add(
                    $"on-disk assembly size mismatch for '{assembly.Name}': expected {assembly.Size}, got {onDiskBytes.Length}.");
            }

            var onDiskHash = PackageReader.ComputeFileHash(onDiskBytes);
            if (!string.Equals(onDiskHash, assembly.Hash, StringComparison.Ordinal))
            {
                issues.Add(
                    $"on-disk assembly hash mismatch for '{assembly.Name}': expected {assembly.Hash}, got {onDiskHash}.");
            }
        }
    }

    private void ValidateSupplementalMetadata(LoadedHotUpdatePackage package, List<string> issues)
    {
        var supplementalMetadataPath = Path.Combine(package.RootPath, package.Manifest.SupplementalMetadata);
        if (!File.Exists(supplementalMetadataPath))
        {
            issues.Add($"hot update supplemental metadata missing: {supplementalMetadataPath}");
            return;
        }

        try
        {
            _supplementalMetadataLoader.LoadFromBytes(File.ReadAllBytes(supplementalMetadataPath));
        }
        catch (Exception exception) when (exception is InvalidDataException or JsonException)
        {
            issues.Add($"supplemental metadata is invalid: {exception.Message}");
        }
    }

    private sealed record RuntimeSnapshot
    {
        public LoadedHotUpdatePackage? LoadedPackage { get; init; }

        public string? CurrentAotVersion { get; init; }

        public RuntimeMode Mode { get; init; }

        public required HotUpdateMethodRegistrySnapshot Registry { get; init; }
    }
}
