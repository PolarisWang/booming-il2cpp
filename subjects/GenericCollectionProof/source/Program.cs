using System;
using System.Collections.Generic;

namespace GenericCollectionProof;

internal static class GenericCollectionProofEntry
{
    public static int Run()
    {
        // Scenario 1: List<int>
        var list = new List<int>();
        list.Add(10);
        list.Add(20);
        list.Add(30);
        Console.WriteLine("generic-list-count=" + list.Count);
        Console.WriteLine("generic-list-0=" + list[0]);
        Console.WriteLine("generic-list-2=" + list[2]);

        // Scenario 2: Dictionary<string, int>
        var dict = new Dictionary<string, int>();
        dict["alpha"] = 1;
        dict["beta"]  = 2;
        Console.WriteLine("generic-dict-count=" + dict.Count);
        Console.WriteLine("generic-dict-alpha=" + dict["alpha"]);
        Console.WriteLine("generic-dict-beta="  + dict["beta"]);

        // Scenario 3: List<string>
        var names = new List<string> { "Alice", "Bob" };
        Console.WriteLine("generic-names-count=" + names.Count);
        Console.WriteLine("generic-names-0=" + names[0]);

        // Scenario 4: generic method
        Console.WriteLine("generic-method-int="    + Identity(42));
        Console.WriteLine("generic-method-string=" + Identity("hello"));

        return 0;
    }

    private static T Identity<T>(T value) => value;
}

internal static class Program
{
    public static int Main() => GenericCollectionProofEntry.Run();
}
