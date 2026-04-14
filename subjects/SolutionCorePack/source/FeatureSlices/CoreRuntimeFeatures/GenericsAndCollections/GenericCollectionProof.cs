using Chaos.TestFramework;
using System.Collections.Generic;

namespace CoreRuntimeFeatures;

internal static class GenericCollectionProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "generic-collection-proof",
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.GenericCollections,
        Requires = ChaosRuntimeFeature.GenericSharing,
        Priority = 2)]
    public static int Run()
    {
        var list = new List<int> { 10, 20, 30 };
        Assert.Equal(3, list.Count);
        Assert.Equal(10, list[0]);
        Assert.Equal(30, list[2]);

        var dict = new Dictionary<string, int>
        {
            ["alpha"] = 1,
            ["beta"] = 2,
        };
        Assert.Equal(2, dict.Count);
        Assert.Equal(1, dict["alpha"]);
        Assert.Equal(2, dict["beta"]);

        var names = new List<string> { "Alice", "Bob" };
        Assert.Equal(2, names.Count);
        Assert.Equal("Alice", names[0]);

        Assert.Equal(42, Identity(42));
        Assert.Equal("hello", Identity("hello"));
        return 0;
    }

    private static T Identity<T>(T value) => value;
}
