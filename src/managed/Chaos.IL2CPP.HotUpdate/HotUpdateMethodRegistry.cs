namespace Chaos.IL2CPP.HotUpdate;

public sealed class HotUpdateMethodRegistry
{
    private readonly Dictionary<string, ConstantInt32InterpreterStub> _entries = new(StringComparer.Ordinal);
    private readonly Dictionary<string, Func<int, int>> _int32UnaryEntries = new(StringComparer.Ordinal);
    private readonly Dictionary<string, Func<IReadOnlyList<object?>, object?>> _genericEntries = new(StringComparer.Ordinal);

    public void RegisterConstantInt32(string subjectId, int constantValue)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        var stub = new ConstantInt32InterpreterStub(constantValue);
        _entries[subjectId] = stub;
        _genericEntries[subjectId] = _ => stub.Execute();
    }

    public void RegisterInt32Unary(string subjectId, Func<int, int> target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(target);
        _int32UnaryEntries[subjectId] = target;
        _genericEntries[subjectId] = args =>
        {
            if (args.Count != 1)
            {
                throw new InvalidOperationException($"hot update unary entry '{subjectId}' expects 1 argument.");
            }

            return target(Convert.ToInt32(args[0]));
        };
    }

    public void RegisterMethod(string subjectId, Func<IReadOnlyList<object?>, object?> target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(target);
        _genericEntries[subjectId] = target;
    }

    public bool TryGet(string subjectId, out ConstantInt32InterpreterStub? stub)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        return _entries.TryGetValue(subjectId, out stub);
    }

    public bool TryGetInt32Unary(string subjectId, out Func<int, int>? target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        return _int32UnaryEntries.TryGetValue(subjectId, out target);
    }

    public bool TryDispatchInt32Unary(string subjectId, int value, out int result)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);

        if (_int32UnaryEntries.TryGetValue(subjectId, out var target))
        {
            result = target(value);
            return true;
        }

        result = default;
        return false;
    }

    public bool TryDispatch(string subjectId, IReadOnlyList<object?> arguments, out object? result)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(arguments);

        if (_genericEntries.TryGetValue(subjectId, out var target))
        {
            result = target(arguments);
            return true;
        }

        result = default;
        return false;
    }

    public HotUpdateMethodRegistrySnapshot Snapshot()
    {
        return new HotUpdateMethodRegistrySnapshot
        {
            ConstantEntries = new Dictionary<string, ConstantInt32InterpreterStub>(_entries, StringComparer.Ordinal),
            Int32UnaryEntries = new Dictionary<string, Func<int, int>>(_int32UnaryEntries, StringComparer.Ordinal),
            GenericEntries = new Dictionary<string, Func<IReadOnlyList<object?>, object?>>(_genericEntries, StringComparer.Ordinal),
        };
    }

    public void Restore(HotUpdateMethodRegistrySnapshot snapshot)
    {
        ArgumentNullException.ThrowIfNull(snapshot);

        _entries.Clear();
        _int32UnaryEntries.Clear();
        _genericEntries.Clear();

        foreach (var pair in snapshot.ConstantEntries)
        {
            _entries[pair.Key] = pair.Value;
        }

        foreach (var pair in snapshot.Int32UnaryEntries)
        {
            _int32UnaryEntries[pair.Key] = pair.Value;
        }

        foreach (var pair in snapshot.GenericEntries)
        {
            _genericEntries[pair.Key] = pair.Value;
        }
    }

    public void Clear()
    {
        _entries.Clear();
        _int32UnaryEntries.Clear();
        _genericEntries.Clear();
    }
}

public sealed record HotUpdateMethodRegistrySnapshot
{
    public IReadOnlyDictionary<string, ConstantInt32InterpreterStub> ConstantEntries { get; init; } =
        new Dictionary<string, ConstantInt32InterpreterStub>(StringComparer.Ordinal);

    public IReadOnlyDictionary<string, Func<int, int>> Int32UnaryEntries { get; init; } =
        new Dictionary<string, Func<int, int>>(StringComparer.Ordinal);

    public IReadOnlyDictionary<string, Func<IReadOnlyList<object?>, object?>> GenericEntries { get; init; } =
        new Dictionary<string, Func<IReadOnlyList<object?>, object?>>(StringComparer.Ordinal);
}
