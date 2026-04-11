namespace Chaos.IL2CPP.HotUpdate;

public enum RuntimeMode
{
    Aot = 0,
    Mixed = 1,
}

public sealed class RuntimeManager
{
    private readonly HotUpdateMethodRegistry _methodRegistry = new();

    public RuntimeMode Mode { get; private set; } = RuntimeMode.Aot;

    public bool IsMixedMode => Mode == RuntimeMode.Mixed;

    public LoadedHotUpdatePackage? LoadedPackage { get; private set; }

    public void LoadPackage(
        LoadedHotUpdatePackage package,
        string currentAotVersion,
        IReadOnlyDictionary<string, int> subjectIdToConstantInt32,
        IReadOnlyDictionary<string, Func<int, int>>? subjectIdToInt32Unary = null)
    {
        ArgumentNullException.ThrowIfNull(package);
        ArgumentNullException.ThrowIfNull(subjectIdToConstantInt32);

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

        LoadedPackage = package;
        Mode = RuntimeMode.Mixed;
    }

    public void UnloadPackage()
    {
        LoadedPackage = null;
        _methodRegistry.Clear();
        Mode = RuntimeMode.Aot;
    }

    public int DispatchInt32(string subjectId, Func<int> aotFallback)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(aotFallback);

        if (IsMixedMode && _methodRegistry.TryGet(subjectId, out var stub) && stub is not null)
        {
            return stub.Execute();
        }

        return aotFallback();
    }

    public void RegisterInt32Unary(string subjectId, Func<int, int> target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(target);
        _methodRegistry.RegisterInt32Unary(subjectId, target);
    }

    public int DispatchInt32Unary(string subjectId, int value, Func<int, int> aotFallback)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(aotFallback);

        if (IsMixedMode && _methodRegistry.TryDispatchInt32Unary(subjectId, value, out var mixedValue))
        {
            return mixedValue;
        }

        return aotFallback(value);
    }
}
