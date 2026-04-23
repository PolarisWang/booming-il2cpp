using Chaos.TestFramework;

namespace MixedExecutionFeaturePack;

internal static class MixedGenericFlowProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "mixed-generic-flow-proof",
        CapabilityFamily = ChaosCapabilityFamily.MixedExecution,
        Capability = ChaosCapabilityItem.MixedGenericFlow,
        Requires = ChaosRuntimeFeature.GenericSharing,
        Archetype = ChaosSolutionArchetype.MixedBridgeSolution,
        Priority = 2)]
    public static int Run()
    {
        var methods = InterpreterArithmeticSupport.BuildLoweredMethods();
        var executor = InterpreterArithmeticSupport.CreateExecutor(methods);
        var method = InterpreterArithmeticSupport.GetRequiredMethod(methods, "EchoStringLength");

        Assert.Equal(5, executor.ExecuteInt32(method, new object?[] { "chaos" }));
        return 0;
    }
}
