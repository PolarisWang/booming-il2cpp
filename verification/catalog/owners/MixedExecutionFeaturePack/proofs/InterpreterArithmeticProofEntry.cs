using Chaos.TestFramework;

namespace MixedExecutionFeaturePack;

internal static class InterpreterArithmeticProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "interpreter-arithmetic-proof",
        CapabilityFamily = ChaosCapabilityFamily.MixedExecution,
        Capability = ChaosCapabilityItem.InterpreterArithmetic,
        Archetype = ChaosSolutionArchetype.MixedBridgeSolution,
        Priority = 2)]
    public static int Run()
    {
        var methods = InterpreterArithmeticSupport.BuildLoweredMethods();
        var executor = InterpreterArithmeticSupport.CreateExecutor(methods);
        var addMethod = InterpreterArithmeticSupport.GetRequiredMethod(methods, "Add");
        var localCallMethod = InterpreterArithmeticSupport.GetRequiredMethod(methods, "CallLocalAdd");

        Assert.Equal(42, executor.ExecuteInt32(addMethod, new int[] { 20, 22 }));
        Assert.Equal(42, executor.ExecuteInt32(localCallMethod, new int[] { 20, 22 }));
        return 0;
    }
}
