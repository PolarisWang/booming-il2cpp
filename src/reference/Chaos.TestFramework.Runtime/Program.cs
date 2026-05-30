using System.Reflection;

namespace Chaos.TestFramework.Runtime;

internal static class Program
{
    private static int Main(string[] args)
    {
        var asmPath = string.Empty;
        var runFact = false;
        var runBenchmark = false;
        var iterations = 100000;

        for (int i = 0; i < args.Length; i++)
        {
            if (args[i] == "--assembly" && i + 1 < args.Length)
                asmPath = args[++i];
            else if (args[i] == "--kind" && i + 1 < args.Length)
            {
                var val = args[++i];
                if (val == "fact") runFact = true;
                else if (val == "benchmark") runBenchmark = true;
            }
            else if (args[i] == "--iterations" && i + 1 < args.Length)
                int.TryParse(args[++i], out iterations);
        }

        if (string.IsNullOrEmpty(asmPath))
        {
            Console.Error.WriteLine("Usage: runtime.exe --assembly <path> [--kind fact] [--kind benchmark] [--iterations N]");
            return 1;
        }

        // Default: run both when no --kind specified
        if (!runFact && !runBenchmark)
            runFact = runBenchmark = true;

        var asm = Assembly.LoadFrom(asmPath);

        if (runFact)
            FactRunner.Run(asm);

        if (runBenchmark)
            BenchmarkRunner.Run(asm, iterations);

        return 0;
    }
}
