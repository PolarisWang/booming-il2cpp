using System;

namespace BenchMixed;

internal static class Program
{
    private const int IterationCount = 1000;
    private static Func<int, int>? s_interpreterFn;
    private static long s_lastChecksum;

    private static int AotCompute(int x) => x * x + x;

    private static int InterpreterCompute(int x)
    {
        return s_interpreterFn?.Invoke(x) ?? x;
    }

    public static int Main()
    {
        s_lastChecksum = RunWorkload();
        return 0;
    }

    public static long RunWorkload()
    {
        s_interpreterFn = static x => x * 2 + 1;

        long checksum = 0;
        for (int i = 0; i < IterationCount * 100; i++)
        {
            int v1 = AotCompute(i);
            int v2 = InterpreterCompute(v1);
            int v3 = AotCompute(v2);
            checksum += v3;
        }

        return checksum % 10000;
    }
}
