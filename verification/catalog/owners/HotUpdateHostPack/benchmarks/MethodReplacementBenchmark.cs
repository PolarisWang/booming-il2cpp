using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class MethodReplacementBenchmarkEntry
{
    private const int IterationCount = 10_000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.HotUpdate,
        ChaosMetric.WallClockUs,
        Alias = "method-replacement-bench",
        CapabilityFamily = ChaosCapabilityFamily.HotUpdateWorkflow,
        Capability = ChaosCapabilityItem.MethodReplacement,
        Requires = ChaosRuntimeFeature.HotUpdate,
        Archetype = ChaosSolutionArchetype.SkeletonPatchSolution,
        HotUpdateCapability = ChaosHotUpdateCapability.MethodReplacement,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 20,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        var slot = new ReplacementSlot(static value => value + 1);
        slot.Register(static value => value + 2);

        var checksum = 0;
        for (var index = 0; index < IterationCount; index++)
        {
            checksum += slot.Invoke(index);
        }

        return checksum;
    }

    private sealed class ReplacementSlot(Func<int, int> aotTarget)
    {
        private readonly Func<int, int> _aotTarget = aotTarget;
        private Func<int, int>? _replacementTarget;

        public void Register(Func<int, int> replacementTarget)
        {
            _replacementTarget = replacementTarget;
        }

        public int Invoke(int value)
        {
            return (_replacementTarget ?? _aotTarget)(value);
        }
    }
}
