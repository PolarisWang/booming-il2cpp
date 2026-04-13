using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal static class ArrayOpsProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "array-ops-proof",
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        int[] arr = new int[5];
        for (int i = 0; i < arr.Length; i++)
        {
            arr[i] = i * 10;
        }

        Console.WriteLine("arr-len=" + arr.Length);
        Console.WriteLine("arr-0=" + arr[0]);
        Console.WriteLine("arr-4=" + arr[4]);
        string[] strs = { "hello", "world" };
        Console.WriteLine("arr-str-0=" + strs[0]);
        Console.WriteLine("arr-str-1=" + strs[1]);
        double[] doubles = new double[3];
        doubles[1] = 3.14;
        Console.WriteLine("arr-dbl-1=" + doubles[1].ToString("F2"));
        return 0;
    }
}
