namespace Chaos.IL2CPP.HotUpdate;

public sealed class BridgeDispatcher
{
    private readonly Dictionary<string, AotToHotUpdateBridgeEntry> _aotToHotUpdateBridges = new(StringComparer.Ordinal);
    private readonly Dictionary<string, HotUpdateToAotBridgeEntry> _hotUpdateToAotBridges = new(StringComparer.Ordinal);
    private readonly Dictionary<string, HotUpdateToEngineBridgeEntry> _hotUpdateToEngineBridges = new(StringComparer.Ordinal);
    private readonly Dictionary<string, DelegateWrapperEntry> _delegateWrappers = new(StringComparer.Ordinal);
    private readonly Dictionary<string, Func<int, int>> _hotUpdateInt32UnaryTargets = new(StringComparer.Ordinal);
    private readonly Dictionary<string, Func<int, int, int>> _aotInt32BinaryTargets = new(StringComparer.Ordinal);
    private readonly Dictionary<string, Func<int, int>> _engineInt32UnaryTargets = new(StringComparer.Ordinal);

    public void ApplyPlan(BridgePlan plan)
    {
        ArgumentNullException.ThrowIfNull(plan);

        _aotToHotUpdateBridges.Clear();
        _hotUpdateToAotBridges.Clear();
        _hotUpdateToEngineBridges.Clear();
        _delegateWrappers.Clear();

        foreach (var entry in plan.AotToHotUpdate)
        {
            _aotToHotUpdateBridges[entry.BridgeId] = entry;
        }

        foreach (var entry in plan.HotUpdateToAot)
        {
            _hotUpdateToAotBridges[entry.BridgeId] = entry;
        }

        foreach (var entry in plan.HotUpdateToEngine)
        {
            _hotUpdateToEngineBridges[entry.BridgeId] = entry;
        }

        foreach (var entry in plan.DelegateWrappers)
        {
            _delegateWrappers[entry.WrapperId] = entry;
        }
    }

    public void RegisterHotUpdateInt32UnaryTarget(string subjectId, Func<int, int> target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(target);
        _hotUpdateInt32UnaryTargets[subjectId] = target;
    }

    public void RegisterAotInt32BinaryTarget(string subjectId, Func<int, int, int> target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(target);
        _aotInt32BinaryTargets[subjectId] = target;
    }

    public void RegisterEngineInt32UnaryTarget(string subjectId, Func<int, int> target)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        ArgumentNullException.ThrowIfNull(target);
        _engineInt32UnaryTargets[subjectId] = target;
    }

    public int InvokeAotToHotUpdateInt32(string bridgeId, int value)
    {
        var bridge = RequireBridge(_aotToHotUpdateBridges, bridgeId, "AOT->HotUpdate");
        var target = RequireTarget(_hotUpdateInt32UnaryTargets, bridge.HotUpdateSubjectId, "hot-update unary");
        return target(value);
    }

    public int InvokeHotUpdateToAotInt32(string bridgeId, int left, int right)
    {
        var bridge = RequireBridge(_hotUpdateToAotBridges, bridgeId, "HotUpdate->AOT");
        var target = RequireTarget(_aotInt32BinaryTargets, bridge.AotSubjectId, "AOT binary");
        return target(left, right);
    }

    public int InvokeHotUpdateToEngineInt32(string bridgeId, int value)
    {
        var bridge = RequireBridge(_hotUpdateToEngineBridges, bridgeId, "HotUpdate->Engine");
        var target = RequireTarget(_engineInt32UnaryTargets, bridge.EngineSubjectId, "engine unary");
        return target(value);
    }

    public Func<int, int> CreateAotDelegateWrapper(string wrapperId)
    {
        var wrapper = RequireBridge(_delegateWrappers, wrapperId, "delegate wrapper");
        var target = RequireTarget(_hotUpdateInt32UnaryTargets, wrapper.HotUpdateSubjectId, "hot-update unary");
        return value => target(value);
    }

    private static TBridge RequireBridge<TBridge>(
        IReadOnlyDictionary<string, TBridge> entries,
        string bridgeId,
        string surfaceName)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(bridgeId);

        if (entries.TryGetValue(bridgeId, out var entry))
        {
            return entry;
        }

        throw new InvalidOperationException($"missing {surfaceName} bridge '{bridgeId}'.");
    }

    private static TTarget RequireTarget<TTarget>(
        IReadOnlyDictionary<string, TTarget> entries,
        string subjectId,
        string surfaceName)
    {
        if (entries.TryGetValue(subjectId, out var entry))
        {
            return entry;
        }

        throw new InvalidOperationException($"missing {surfaceName} target '{subjectId}'.");
    }
}
