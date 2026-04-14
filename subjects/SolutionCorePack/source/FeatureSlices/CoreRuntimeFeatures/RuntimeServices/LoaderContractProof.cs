using Chaos.TestFramework;
using System.Reflection;

namespace CoreRuntimeFeatures;

internal static class LoaderContractProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "loader-contract-proof",
        CapabilityFamily = ChaosCapabilityFamily.RuntimeServices,
        Capability = ChaosCapabilityItem.LoaderContract,
        Priority = 2)]
    public static int Run()
    {
        Assembly current = typeof(LoaderContractProofEntry).Assembly;
        Type? self = current.GetType("CoreRuntimeFeatures.LoaderContractProofEntry");
        Type? frameworkAssert = Type.GetType("Chaos.TestFramework.Assert, Chaos.TestFramework");

        Assert.Equal("CoreRuntimeFeatures", current.GetName().Name);
        Assert.NotNull(self);
        Assert.NotNull(frameworkAssert);
        Assert.Equal("Chaos.TestFramework", frameworkAssert.Assembly.GetName().Name);
        return 0;
    }
}
