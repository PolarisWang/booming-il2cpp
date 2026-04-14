using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class MethodReplacementProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.HotUpdateContract,
        Alias = "method-replacement-proof",
        CapabilityFamily = ChaosCapabilityFamily.HotUpdateWorkflow,
        Capability = ChaosCapabilityItem.MethodReplacement,
        Requires = ChaosRuntimeFeature.HotUpdate,
        Archetype = ChaosSolutionArchetype.SkeletonPatchSolution,
        HotUpdateCapability = ChaosHotUpdateCapability.MethodReplacement,
        Priority = 2)]
    public static int Run()
    {
        var slot = new ReplacementSlot(static () => "hello from AOT");
        var before = slot.Invoke();
        Assert.Equal("hello from AOT", before);

        slot.Register(static () => "hello from interpreter");
        var after = slot.Invoke();
        Assert.Equal("hello from interpreter", after);
        slot.Revert();
        var reverted = slot.Invoke();
        Assert.Equal("hello from AOT", reverted);
        return 0;
    }

    private sealed class ReplacementSlot(Func<string> aotTarget)
    {
        private readonly Func<string> _aotTarget = aotTarget;
        private Func<string>? _replacementTarget;

        public void Register(Func<string> replacementTarget)
        {
            _replacementTarget = replacementTarget;
        }

        public void Revert()
        {
            _replacementTarget = null;
        }

        public string Invoke()
        {
            return (_replacementTarget ?? _aotTarget)();
        }
    }
}
