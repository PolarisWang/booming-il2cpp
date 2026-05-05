namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Configuration for the convert-to-cpp command.
/// Parses CLI arguments in Unity IL2CPP style.
/// </summary>
internal sealed class ConvertToCppConfig
{
    /// <summary>Path to the input assembly (.dll)</summary>
    public string AssemblyPath { get; init; } = "";

    /// <summary>Output directory for generated C++ files</summary>
    public string OutputDir { get; init; } = "";

    /// <summary>Additional directories to search for dependency assemblies</summary>
    public IReadOnlyList<string> AssemblyDirs { get; init; } = [];

    public bool Verbose { get; init; } = false;

    /// <summary>
    /// Parse CLI arguments.
    /// Expected: --assembly &lt;path&gt; --output &lt;dir&gt; [--assembly-dir &lt;dir&gt; ...]
    /// </summary>
    public static ConvertToCppConfig Parse(string[] args)
    {
        string assemblyPath = "";
        string outputDir = "il2cpp_dist";
        var assemblyDirs = new List<string>();
        bool verbose = false;

        for (int i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--assembly" or "-a" when i + 1 < args.Length:
                    assemblyPath = args[++i];
                    break;
                case "--output" or "-o" when i + 1 < args.Length:
                    outputDir = args[++i];
                    break;
                case "--verbose" or "-v":
                    verbose = true;
                    break;
                case "--assembly-dir" when i + 1 < args.Length:
                    assemblyDirs.Add(args[++i]);
                    break;
                case "--help" or "-h":
                    PrintHelp();
                    return new ConvertToCppConfig { AssemblyPath = "", OutputDir = "" };
            }
        }

        if (string.IsNullOrWhiteSpace(assemblyPath))
        {
            Console.Error.WriteLine("Error: --assembly is required.");
            Console.Error.WriteLine("Usage: chaos-il2cpp convert-to-cpp --assembly <path> --output <dir>");
            Environment.Exit(1);
        }

        return new ConvertToCppConfig { AssemblyPath = assemblyPath, OutputDir = outputDir, AssemblyDirs = assemblyDirs, Verbose = verbose };
    }

    private static void PrintHelp()
    {
        Console.WriteLine("chaos-il2cpp convert-to-cpp — Convert a managed assembly to native C++ code");
        Console.WriteLine();
        Console.WriteLine("Usage:");
        Console.WriteLine("  chaos-il2cpp convert-to-cpp --assembly <path> --output <dir> [options]");
        Console.WriteLine();
        Console.WriteLine("Options:");
        Console.WriteLine("  --assembly, -a <path>         Input assembly (.dll) to translate");
        Console.WriteLine("  --output, -o <dir>            Output directory for generated C++ files");
        Console.WriteLine("  --assembly-dir <dir>          Additional dependency search path (repeatable)");
        Console.WriteLine("  --verbose, -v                 Enable verbose diagnostics");
        Console.WriteLine("  --help, -h                    Show this help");
    }
}
