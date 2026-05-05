namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Configuration for the convert-to-cpp command.
/// Parses CLI arguments in Unity IL2CPP style.
/// Supports multiple --assembly inputs for full IL2CPP translation.
/// </summary>
internal sealed class ConvertToCppConfig
{
    /// <summary>Paths to input assemblies (.dll) to translate</summary>
    public IReadOnlyList<string> AssemblyPaths { get; init; } = [];

    /// <summary>Optional explicit entry point subject ID</summary>
    public string? EntryPoint { get; init; }

    /// <summary>Output directory for generated C++ files</summary>
    public string OutputDir { get; init; } = "";

    /// <summary>Additional directories to search for dependency assemblies</summary>
    public IReadOnlyList<string> AssemblyDirs { get; init; } = [];

    /// <summary>Enable verbose diagnostics</summary>
    public bool Verbose { get; init; } = false;

    /// <summary>When true, compile full assembly closure instead of entry-point reachable</summary>
    public bool FullClosure { get; init; } = false;

    /// <summary>
    /// Parse CLI arguments.
    /// Expected: --assembly &lt;path&gt; [--assembly &lt;path&gt; ...] --output &lt;dir&gt; [options]
    /// </summary>
    public static ConvertToCppConfig Parse(string[] args)
    {
        var assemblyPaths = new List<string>();
        string outputDir = "il2cpp_dist";
        var assemblyDirs = new List<string>();
        bool verbose = false;
        bool fullClosure = false;
        string? entryPoint = null;

        for (int i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--assembly" or "-a" when i + 1 < args.Length:
                    assemblyPaths.Add(args[++i]);
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
                case "--entry-point" when i + 1 < args.Length:
                    entryPoint = args[++i];
                    break;
                case "--full-closure":
                    fullClosure = true;
                    break;
                case "--help" or "-h":
                    PrintHelp();
                    return new ConvertToCppConfig { AssemblyPaths = [], OutputDir = "" };
            }
        }

        if (assemblyPaths.Count == 0)
        {
            Console.Error.WriteLine("Error: at least one --assembly is required.");
            Console.Error.WriteLine("Usage: chaos-il2cpp convert-to-cpp --assembly <path> [--assembly <path> ...] --output <dir>");
            Environment.Exit(1);
        }

        return new ConvertToCppConfig
        {
            AssemblyPaths = assemblyPaths,
            OutputDir = outputDir,
            AssemblyDirs = assemblyDirs,
            Verbose = verbose,
            FullClosure = fullClosure,
            EntryPoint = entryPoint,
        };
    }

    private static void PrintHelp()
    {
        Console.WriteLine("chaos-il2cpp convert-to-cpp — Convert managed assemblies to native C++ code");
        Console.WriteLine();
        Console.WriteLine("Usage:");
        Console.WriteLine("  chaos-il2cpp convert-to-cpp --assembly <path> [--assembly <path> ...] --output <dir> [options]");
        Console.WriteLine();
        Console.WriteLine("Options:");
        Console.WriteLine("  --assembly, -a <path>         Input assembly (.dll) to translate (repeatable)");
        Console.WriteLine("  --output, -o <dir>            Output directory for generated C++ files");
        Console.WriteLine("  --assembly-dir <dir>          Additional dependency search path (repeatable)");
        Console.WriteLine("  --entry-point <subject-id>    Explicit entry point (default: auto-detect Main)");
        Console.WriteLine("  --full-closure                Compile full closure (all reachable methods)");
        Console.WriteLine("  --verbose, -v                 Enable verbose diagnostics");
        Console.WriteLine("  --help, -h                    Show this help");
    }
}
