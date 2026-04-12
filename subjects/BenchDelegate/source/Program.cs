using System;

namespace BenchDelegate;

internal static class Program
{
    private const int IterationCount = 1000;
    private static int s_counter;

    private static void Tick(int x) => s_counter += x;

    public static int Main()
    {
        s_counter = RunWorkload();
        return 0;
    }

    public static int RunWorkload()
    {
        Action<int> single = Tick;
        Action<int> multi = Tick;
        multi += Tick;
        multi += Tick;

        s_counter = 0;
        for (int i = 0; i < IterationCount * 100; i++)
        {
            single(1);
            if (i % 10 == 0) multi(1);
        }

        return s_counter % 10000;
    }
}
