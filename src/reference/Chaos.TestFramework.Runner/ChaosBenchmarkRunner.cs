using System;

namespace Chaos.TestFramework.Runner;

/// <summary>
/// Benchmark runner: measures elapsed time for a single method
/// over N iterations. Uses DateTime.UtcNow.Ticks instead of
/// Stopwatch to avoid flat goto IL patterns.
/// </summary>
public static class ChaosBenchmarkRunner
{
    public static double RunEntry(Action[] methods, int entryIndex, int iterations)
    {
        long start = DateTime.UtcNow.Ticks;
        for (int i = 0; i < iterations; i++)
            methods[entryIndex]();
        long elapsed = DateTime.UtcNow.Ticks - start;
        return elapsed / 10000.0;
    }
}