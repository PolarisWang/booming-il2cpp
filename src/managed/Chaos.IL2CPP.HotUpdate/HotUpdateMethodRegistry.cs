using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.HotUpdate;

public sealed class HotUpdateMethodRegistry
{
    private readonly Dictionary<string, ConstantInt32InterpreterStub> _entries = new(StringComparer.Ordinal);
    private readonly Dictionary<string, Func<int, int>> _int32UnaryEntries = new(StringComparer.Ordinal);
    private readonly Dictionary<string, Func<IReadOnlyList<object?>, object?>> _genericEntries = new(StringComparer.Ordinal);

    public void RegisterConstantInt32(ManagedMethodIdentityArtifact identity, int constantValue)
    {
        ArgumentNullException.ThrowIfNull(identity);
        RegisterConstantInt32Core(ManagedMethodIdentityResolver.ResolveSubjectId(identity), constantValue);
    }

    public void RegisterInt32Unary(ManagedMethodIdentityArtifact identity, Func<int, int> target)
    {
        ArgumentNullException.ThrowIfNull(identity);
        RegisterInt32UnaryBySubjectId(ManagedMethodIdentityResolver.ResolveSubjectId(identity), target);
    }

    public void RegisterMethod(ManagedMethodIdentityArtifact identity, Func<IReadOnlyList<object?>, object?> target)
    {
        ArgumentNullException.ThrowIfNull(identity);
        RegisterMethodBySubjectId(ManagedMethodIdentityResolver.ResolveSubjectId(identity), target);
    }

    public bool TryGet(ManagedMethodIdentityArtifact identity, out ConstantInt32InterpreterStub? stub)
    {
        ArgumentNullException.ThrowIfNull(identity);
        return TryGetCore(ManagedMethodIdentityResolver.ResolveSubjectId(identity), out stub);
    }

    public bool TryGetInt32Unary(ManagedMethodIdentityArtifact identity, out Func<int, int>? target)
    {
        ArgumentNullException.ThrowIfNull(identity);
        return TryGetInt32UnaryCore(ManagedMethodIdentityResolver.ResolveSubjectId(identity), out target);
    }

    public bool TryDispatchInt32Unary(ManagedMethodIdentityArtifact identity, int value, out int result)
    {
        ArgumentNullException.ThrowIfNull(identity);
        return TryDispatchInt32UnaryCore(ManagedMethodIdentityResolver.ResolveSubjectId(identity), value, out result);
    }

    public bool TryDispatch(ManagedMethodIdentityArtifact identity, IReadOnlyList<object?> arguments, out object? result)
    {
        ArgumentNullException.ThrowIfNull(identity);
        return TryDispatchBySubjectId(ManagedMethodIdentityResolver.ResolveSubjectId(identity), arguments, out result);
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

    private void RegisterConstantInt32Core(string subjectId, int constantValue)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        var stub = new ConstantInt32InterpreterStub(constantValue);
        _entries[subjectId] = stub;
        _genericEntries[subjectId] = _ => stub.Execute();
    }

    internal void RegisterInt32UnaryBySubjectId(string subjectId, Func<int, int> target)
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

    internal void RegisterMethodBySubjectId(string subjectId, Func<IReadOnlyList<object?>, object?> target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(target);
        _genericEntries[subjectId] = target;
    }

    private bool TryGetCore(string subjectId, out ConstantInt32InterpreterStub? stub)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        return _entries.TryGetValue(subjectId, out stub);
    }

    private bool TryGetInt32UnaryCore(string subjectId, out Func<int, int>? target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        return _int32UnaryEntries.TryGetValue(subjectId, out target);
    }

    private bool TryDispatchInt32UnaryCore(string subjectId, int value, out int result)
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

    internal bool TryDispatchBySubjectId(string subjectId, IReadOnlyList<object?> arguments, out object? result)
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
