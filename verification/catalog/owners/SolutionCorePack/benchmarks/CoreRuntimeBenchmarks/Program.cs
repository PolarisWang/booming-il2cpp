namespace CoreRuntimeBenchmarks;

internal static class Program
{
    public static int Main()
    {
        _ = ArithmeticBenchmarkEntry.RunWorkload();
        _ = DelegatePerformanceBenchmarkEntry.C1_Arity0_Static();
        return 0;
    }
}
