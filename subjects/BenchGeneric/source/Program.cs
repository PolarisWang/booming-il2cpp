using System.Collections.Generic;

namespace BenchGeneric;

internal static class Program
{
    private const int IterationCount = 1000;
    private static long s_lastChecksum;

    public static int Main()
    {
        s_lastChecksum = RunWorkload();
        return 0;
    }

    public static long RunWorkload()
    {
        long checksum = 0;
        for (int iter = 0; iter < IterationCount; iter++)
        {
            var list = new List<int>();
            for (int i = 0; i < 100; i++) list.Add(i);
            checksum += list.Count;

            var dict = new Dictionary<string, int>();
            for (int i = 0; i < 20; i++) dict["key" + i] = i;
            checksum += dict["key10"];
        }

        return checksum % 10000;
    }
}
