namespace Chaos.IL2CPP.HotUpdate;

public sealed class HotUpdateMethodRegistry
{
    private readonly Dictionary<string, ConstantInt32InterpreterStub> _entries = new(StringComparer.Ordinal);
    private readonly Dictionary<string, Func<int, int>> _int32UnaryEntries = new(StringComparer.Ordinal);

    public void RegisterConstantInt32(string subjectId, int constantValue)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        _entries[subjectId] = new ConstantInt32InterpreterStub(constantValue);
    }

    public void RegisterInt32Unary(string subjectId, Func<int, int> target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(target);
        _int32UnaryEntries[subjectId] = target;
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

    public void Clear()
    {
        _entries.Clear();
        _int32UnaryEntries.Clear();
    }
}
