using System.Text.Json;
using Chaos.IL2CPP.Contracts;

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

public readonly record struct HotUpdateMethodHandle(string ExecutionAuthorityKey, int Generation);

public static class HotUpdateDispatchReasonCodes
{
    public const string RuntimeNotMixed = "runtime-not-mixed";

    public const string StaleHandle = "stale-handle";

    public const string UnboundAuthority = "unbound-authority";
}

public sealed class RuntimeManager
{
    public const string CurrentKernelArtifactVersion = HotUpdateVersionContract.CurrentKernelArtifactVersion;

    private readonly HotUpdateAssemblyLoader _assemblyLoader = new();
    private readonly HotUpdateMethodRegistry _methodRegistry = new();
    private readonly Stack<RuntimeSnapshot> _history = new();
    private string? _currentAotVersion;
    private int _currentGeneration;

    public RuntimeMode Mode { get; private set; } = RuntimeMode.Aot;

    public bool IsMixedMode => Mode == RuntimeMode.Mixed;

    public bool CanRollback => _history.Count > 0;

    public int CurrentGeneration => _currentGeneration;

    public LoadedHotUpdatePackage? LoadedPackage { get; private set; }

    public SupplementalMetadataRegistry SupplementalMetadata { get; } = new();

    public string? LastError { get; private set; }

    public bool LoadPackage(
        string packageRootPath,
        string currentAotVersion,
        HotUpdateMethodBindingSet bindings)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(packageRootPath);
        ArgumentNullException.ThrowIfNull(bindings);

        return TryLoadPackage(
            packageRootPath,
            currentAotVersion,
            package => LoadPackage(package, currentAotVersion, bindings));
    }

    public void LoadPackage(
        LoadedHotUpdatePackage package,
        string currentAotVersion,
        HotUpdateMethodBindingSet bindings)
    {
        ArgumentNullException.ThrowIfNull(package);
        ArgumentNullException.ThrowIfNull(bindings);

        ActivatePackage(package, currentAotVersion, () => ApplyBindings(bindings));
    }

    public void UnloadPackage()
    {
        _history.Clear();
        LoadedPackage = null;
        _currentAotVersion = null;
        _currentGeneration = 0;
        _methodRegistry.Clear();
        SupplementalMetadata.Clear();
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

    public HotUpdateMethodHandle CreateHandle(ManagedMethodIdentityArtifact identity)
    {
        ArgumentNullException.ThrowIfNull(identity);

        return new HotUpdateMethodHandle(
            ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(identity),
            _currentGeneration);
    }

    public bool TryDispatchHandle(
        HotUpdateMethodHandle handle,
        IReadOnlyList<object?> arguments,
        out object? result,
        out string reasonCode)
    {
        ArgumentNullException.ThrowIfNull(arguments);

        if (!IsMixedMode)
        {
            result = null;
            reasonCode = HotUpdateDispatchReasonCodes.RuntimeNotMixed;
            return false;
        }

        if (handle.Generation != _currentGeneration)
        {
            result = null;
            reasonCode = HotUpdateDispatchReasonCodes.StaleHandle;
            return false;
        }

        if (_methodRegistry.TryDispatchByExecutionAuthority(handle.ExecutionAuthorityKey, arguments, out result))
        {
            reasonCode = string.Empty;
            return true;
        }

        reasonCode = HotUpdateDispatchReasonCodes.UnboundAuthority;
        return false;
    }

    public int DispatchInt32(ManagedMethodIdentityArtifact identity, Func<int> aotFallback)
    {
        ArgumentNullException.ThrowIfNull(identity);
        ArgumentNullException.ThrowIfNull(aotFallback);

        return DispatchInt32Core(ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(identity), aotFallback);
    }

    public void RegisterInt32Unary(ManagedMethodIdentityArtifact identity, Func<int, int> target)
    {
        ArgumentNullException.ThrowIfNull(identity);
        ArgumentNullException.ThrowIfNull(target);
        RegisterInt32UnaryCore(ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(identity), target);
    }

    public void RegisterMethod(ManagedMethodIdentityArtifact identity, Func<IReadOnlyList<object?>, object?> target)
    {
        ArgumentNullException.ThrowIfNull(identity);
        ArgumentNullException.ThrowIfNull(target);
        RegisterMethodCore(ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(identity), target);
    }

    public int DispatchInt32Unary(ManagedMethodIdentityArtifact identity, int value, Func<int, int> aotFallback)
    {
        ArgumentNullException.ThrowIfNull(identity);
        ArgumentNullException.ThrowIfNull(aotFallback);

        return DispatchInt32UnaryCore(ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(identity), value, aotFallback);
    }

    private bool TryLoadPackage(
        string packageRootPath,
        string currentAotVersion,
        Action<LoadedHotUpdatePackage> loadAction)
    {
        try
        {
            var package = _assemblyLoader.LoadFromDirectory(packageRootPath);
            loadAction(package);
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

    private void ActivatePackage(
        LoadedHotUpdatePackage package,
        string currentAotVersion,
        Action applyBindings)
    {
        var snapshot = CaptureSnapshot();
        PackageValidator.ValidateCompatibleTargetAotVersion(package, currentAotVersion);
        PackageValidator.ValidateCompatibleKernelArtifactVersion(package, CurrentKernelArtifactVersion);

        try
        {
            _methodRegistry.Clear();
            applyBindings();
            SupplementalMetadata.Activate(package);

            LoadedPackage = package;
            _currentAotVersion = currentAotVersion;
            _currentGeneration = checked(snapshot.CurrentGeneration + 1);
            Mode = RuntimeMode.Mixed;
            _history.Push(snapshot);
            LastError = null;
        }
        catch
        {
            RestoreSnapshot(snapshot);
            throw;
        }
    }

    private void ApplyBindings(HotUpdateMethodBindingSet bindings)
    {
        foreach (var binding in bindings.ConstantInt32Bindings)
        {
            _methodRegistry.RegisterConstantInt32ByExecutionAuthority(
                ResolveBindingExecutionAuthorityKey(binding.ExecutionAuthorityKey, binding.Identity),
                binding.ConstantValue);
        }

        foreach (var binding in bindings.Int32UnaryBindings)
        {
            _methodRegistry.RegisterInt32UnaryByExecutionAuthority(
                ResolveBindingExecutionAuthorityKey(binding.ExecutionAuthorityKey, binding.Identity),
                binding.Target);
        }

        foreach (var binding in bindings.GenericBindings)
        {
            _methodRegistry.RegisterMethodByExecutionAuthority(
                ResolveBindingExecutionAuthorityKey(binding.ExecutionAuthorityKey, binding.Identity),
                binding.Target);
        }
    }

    private static string ResolveBindingExecutionAuthorityKey(
        string? executionAuthorityKey,
        ManagedMethodIdentityArtifact identity)
    {
        ArgumentNullException.ThrowIfNull(identity);

        if (!string.IsNullOrWhiteSpace(executionAuthorityKey))
        {
            return executionAuthorityKey;
        }

        return ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(identity);
    }

    private int DispatchInt32Core(string executionAuthorityKey, Func<int> aotFallback)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(executionAuthorityKey);
        ArgumentNullException.ThrowIfNull(aotFallback);

        if (IsMixedMode &&
            _methodRegistry.TryDispatchByExecutionAuthority(executionAuthorityKey, Array.Empty<object?>(), out var mixedValue))
        {
            return Convert.ToInt32(mixedValue);
        }

        return aotFallback();
    }

    private void RegisterInt32UnaryCore(string executionAuthorityKey, Func<int, int> target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(executionAuthorityKey);
        ArgumentNullException.ThrowIfNull(target);
        _methodRegistry.RegisterInt32UnaryByExecutionAuthority(executionAuthorityKey, target);
    }

    private void RegisterMethodCore(string executionAuthorityKey, Func<IReadOnlyList<object?>, object?> target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(executionAuthorityKey);
        ArgumentNullException.ThrowIfNull(target);
        _methodRegistry.RegisterMethodByExecutionAuthority(executionAuthorityKey, target);
    }

    private int DispatchInt32UnaryCore(string executionAuthorityKey, int value, Func<int, int> aotFallback)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(executionAuthorityKey);
        ArgumentNullException.ThrowIfNull(aotFallback);

        if (IsMixedMode &&
            _methodRegistry.TryDispatchByExecutionAuthority(executionAuthorityKey, [value], out var mixedValue))
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
            CurrentGeneration = _currentGeneration,
            Mode = Mode,
            Registry = _methodRegistry.Snapshot(),
        };
    }

    private void RestoreSnapshot(RuntimeSnapshot snapshot)
    {
        ArgumentNullException.ThrowIfNull(snapshot);

        LoadedPackage = snapshot.LoadedPackage;
        _currentAotVersion = snapshot.CurrentAotVersion;
        _currentGeneration = snapshot.CurrentGeneration;
        Mode = snapshot.Mode;
        _methodRegistry.Restore(snapshot.Registry);
        if (snapshot.LoadedPackage is null)
        {
            SupplementalMetadata.Clear();
        }
        else
        {
            SupplementalMetadata.Activate(snapshot.LoadedPackage);
        }

        LastError = null;
    }

    private void ValidatePackageCompatibility(LoadedHotUpdatePackage package, List<string> issues)
    {
        try
        {
            PackageValidator.ValidateCompatibleKernelArtifactVersion(package, CurrentKernelArtifactVersion);
        }
        catch (InvalidOperationException exception)
        {
            issues.Add(exception.Message);
        }

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

        if (SupplementalMetadata.ActiveMetadata is null)
        {
            issues.Add("supplemental metadata registry is empty while a hot update package is active.");
            return;
        }

        if (!ReferenceEquals(SupplementalMetadata.ActivePackage, package) ||
            !ReferenceEquals(SupplementalMetadata.ActiveMetadata, package.SupplementalMetadata))
        {
            issues.Add("supplemental metadata registry is not aligned with the loaded package.");
        }
    }

    private sealed record RuntimeSnapshot
    {
        public LoadedHotUpdatePackage? LoadedPackage { get; init; }

        public string? CurrentAotVersion { get; init; }

        public int CurrentGeneration { get; init; }

        public RuntimeMode Mode { get; init; }

        public required HotUpdateMethodRegistrySnapshot Registry { get; init; }
    }
}
