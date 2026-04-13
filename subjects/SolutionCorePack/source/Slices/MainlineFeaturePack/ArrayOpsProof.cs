using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal static class ArrayOpsProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "array-ops-proof",
        Priority = 2)]
    public static int Run()
    {
        int[] arr = new int[5];
        for (int i = 0; i < arr.Length; i++)
        {
            arr[i] = i * 10;
        }

        Assert.Equal(5, arr.Length);
        Assert.Equal(0, arr[0]);
        Assert.Equal(40, arr[4]);
        string[] strs = { "hello", "world" };
        Assert.Equal("hello", strs[0]);
        Assert.Equal("world", strs[1]);
        double[] doubles = new double[3];
        doubles[1] = 3.14;
        Assert.Equal("3.14", doubles[1].ToString("F2"));
        return 0;
    }
}
