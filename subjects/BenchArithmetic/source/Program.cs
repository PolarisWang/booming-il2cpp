namespace BenchArithmetic;

internal static class Program
{
    private const int IterationCount = 5_000_000;
    private static int s_lastChecksum;

    public static int Main()
    {
        s_lastChecksum = RunWorkload();
        return 0;
    }

    public static int RunWorkload()
    {
        int result = 0;
        for (int i = 0; i < IterationCount; i++)
        {
            result += i * 3 - i / 2 + i % 7;
            result -= i % 5;
        }

        return result;
    }
}
