using System.Diagnostics;

namespace Chaos.TestFramework.Runner;

/// <summary>
/// Benchmark runner: measures nanosecond execution time for a single method
/// over N iterations. Does NOT read ExitCode — only measures throughput.
/// </summary>
public static class ChaosBenchmarkRunner
{
    /// <summary>
    /// Run a specific entry method for the given number of iterations.
    /// </summary>
    /// <param name="methods">Method dispatch table.</param>
    /// <param name="entryIndex">Index of the method to benchmark.</param>
    /// <param name="iterations">Number of invocations.</param>
    /// <returns>Elapsed time in milliseconds.</returns>
    public static double RunEntry(Action[] methods, int entryIndex, int iterations)
    {
        var sw = Stopwatch.StartNew();
        for (int i = 0; i < iterations; i++)
            methods[entryIndex]();
        sw.Stop();
        return sw.Elapsed.TotalMilliseconds;
    }
}