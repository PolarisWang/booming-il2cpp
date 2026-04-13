namespace PerformanceFeaturePack;

internal static class Program
{
    public static int Main()
    {
        _ = ArithmeticBenchmarkEntry.RunWorkload();
        return 0;
    }
}
