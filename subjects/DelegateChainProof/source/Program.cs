using System;

namespace DelegateChainProof;

internal static class DelegateChainProofEntry
{
    private static int s_counter;

    private static void Increment(string tag)
    {
        s_counter++;
        Console.WriteLine("delegate-invoked=" + tag);
    }

    public static int Run()
    {
        // Scenario 1: single delegate
        s_counter = 0;
        Action<string> single = Increment;
        single("A");
        Console.WriteLine("delegate-single-count=" + s_counter);   // 1

        // Scenario 2: multicast combine (+=)
        s_counter = 0;
        Action<string> multi = null!;
        multi += Increment;
        multi += Increment;
        multi += Increment;
        multi("B");
        Console.WriteLine("delegate-multi-count=" + s_counter);    // 3

        // Scenario 3: remove one delegate (-=)
        s_counter = 0;
        Action<string> chain = null!;
        Action<string> d1 = Increment;
        Action<string> d2 = Increment;
        chain += d1;
        chain += d2;
        chain += d1;
        chain -= d1;  // removes LAST occurrence of d1
        chain("C");
        Console.WriteLine("delegate-remove-count=" + s_counter);   // 2 (d2 + d1 remain)

        // Scenario 4: remove all
        s_counter = 0;
        Action<string> evt = null!;
        evt += Increment;
        evt -= Increment;
        // evt is now null — guard against null before invoke
        if (evt != null)
            evt("D");
        Console.WriteLine("delegate-empty-count=" + s_counter);    // 0

        return 0;
    }
}

internal static class Program
{
    public static int Main() => DelegateChainProofEntry.Run();
}
