using Chaos.TestFramework;

namespace MixedExecutionFeaturePack;

internal static class MixedExceptionFlowProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "mixed-exception-flow-proof",
        CapabilityFamily = ChaosCapabilityFamily.MixedExecution,
        Capability = ChaosCapabilityItem.MixedExceptionFlow,
        Requires = ChaosRuntimeFeature.ExceptionFlow,
        Archetype = ChaosSolutionArchetype.MixedBridgeSolution,
        Priority = 2)]
    public static int Run()
    {
        var methods = InterpreterArithmeticSupport.BuildLoweredMethods();
        var executor = InterpreterArithmeticSupport.CreateExecutor(methods);
        var catchMethod = InterpreterArithmeticSupport.GetRequiredMethod(methods, "DivideOrCatch");
        var rethrowMethod = InterpreterArithmeticSupport.GetRequiredMethod(methods, "DivideOrRethrow");
        var finallyMethod = InterpreterArithmeticSupport.GetRequiredMethod(methods, "AddWithFinally");

        Assert.Equal(42, executor.ExecuteInt32(catchMethod, new int[] { 0 }));
        Assert.Throws<DivideByZeroException>(() => executor.ExecuteInt32(rethrowMethod, new int[] { 0 }));
        Assert.Equal(42, executor.ExecuteInt32(finallyMethod, Array.Empty<int>()));
        return 0;
    }
}
