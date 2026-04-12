using System;

namespace BenchException;

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

        for (int i = 0; i < IterationCount * 100; i++)
        {
            try { checksum += i; }
            catch (Exception) { /* should not happen */ }
        }

        for (int i = 0; i < IterationCount * 10; i++)
        {
            try { if (i % 100 == 0) throw new InvalidOperationException("bench"); checksum += i; }
            catch (InvalidOperationException) { checksum++; }
        }

        return checksum % 10000;
    }
}
