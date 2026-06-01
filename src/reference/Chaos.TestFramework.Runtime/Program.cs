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

        // Set up assembly resolution: probe the test assembly directory
        var asmDir = Path.GetDirectoryName(Path.GetFullPath(asmPath))!;
        AppDomain.CurrentDomain.AssemblyResolve += (_, args) =>
        {
            var name = new AssemblyName(args.Name).Name;
            // Try test assembly directory first
            var path = Path.Combine(asmDir, $"{name}.dll");
            if (File.Exists(path))
                return Assembly.LoadFrom(path);

            // Try runtime directory
            var runtimeDir = Path.GetDirectoryName(typeof(Program).Assembly.Location);
            if (runtimeDir is not null)
            {
                path = Path.Combine(runtimeDir, $"{name}.dll");
                if (File.Exists(path))
                    return Assembly.LoadFrom(path);
            }
            return null;
        };

        var asm = Assembly.LoadFrom(asmPath);

        if (runFact)
            FactRunner.Run(asm);

        if (runBenchmark)
            BenchmarkRunner.Run(asm, iterations);

        return 0;
    }
}
