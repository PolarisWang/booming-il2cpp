using Chaos.TestFramework;
using System.Collections.Generic;

namespace MainlineFeaturePack;

internal static class GenericCollectionProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "generic-collection-proof",
        Requires = ChaosRuntimeFeature.GenericSharing,
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        var list = new List<int> { 10, 20, 30 };
        Console.WriteLine("generic-list-count=" + list.Count);
        Console.WriteLine("generic-list-0=" + list[0]);
        Console.WriteLine("generic-list-2=" + list[2]);

        var dict = new Dictionary<string, int>
        {
            ["alpha"] = 1,
            ["beta"] = 2,
        };
        Console.WriteLine("generic-dict-count=" + dict.Count);
        Console.WriteLine("generic-dict-alpha=" + dict["alpha"]);
        Console.WriteLine("generic-dict-beta=" + dict["beta"]);

        var names = new List<string> { "Alice", "Bob" };
        Console.WriteLine("generic-names-count=" + names.Count);
        Console.WriteLine("generic-names-0=" + names[0]);

        Console.WriteLine("generic-method-int=" + Identity(42));
        Console.WriteLine("generic-method-string=" + Identity("hello"));
        return 0;
    }

    private static T Identity<T>(T value) => value;
}
