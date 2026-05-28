using System.Text.Json;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Emission;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Metadata;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator;

public static class Program
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
    };

    private static readonly ContractReader ContractReader = new();

    public static int Main(string[] args)
    {
        if (args.Length < 1)
        {
            PrintUsage();
            return 1;
        }

        var command = args[0];
        return command switch
        {
            "scan" => RunScan(args[1..]),
            "generate" => RunGenerate(args[1..]),
            "emit" => RunEmit(args[1..]),
            _ => Error($"Unknown command: {command}")
        };
    }

    private static void PrintUsage()
    {
        Console.WriteLine("""
            Chaos.IL2CPP TestProjectGenerator

            Commands:
              scan --assemblies <dll1.dll> [dll2.dll ...]
                  Scan DLLs and print discovered subjects.

              scan --contract <contract.json>
                  Read subjects from a contract.json file.

              generate --assemblies <dll1.dll> [dll2.dll ...] --contract <contract.json> --output <dir>
                       [--jit] [--config-tier check|profile|ship] [--platform windows|linux|osx]
                  Full project generation: runs IL2CPP codegen, then emits complete C++
                  project (entry.cpp/h, dispatch.cpp, CMakeLists.txt, CMakePresets.json,
                  runtime-patchdata.cpp, chaos-sdk cmake, metadata).
                  Use for standalone projects without the verification pipeline.

              emit --contract <contract.json> --output <dir>
                   [--jit] [--config-tier check|profile|ship] [--platform windows|linux|osx]
                  Emit dispatch.cpp and metadata from contract only (no IL2CPP codegen).
                  Faster than 'generate' — use when codegen output already exists.
                  Expects codegen output (--sdk-out) to already exist in the output directory.


            Options:
              --jit                  Enable JIT mode (default: AOT)
              --config-tier <tier>   Build configuration: check, profile, or ship (default: check)
              --platform <os>        Target platform: windows, linux, or osx (default: windows)
            """);
    }

    private static int RunScan(string[] args)
    {
        var parsed = ParseArgs(args);
        if (parsed.ContractPath is not null)
        {
            var contractSubjects = ContractReader.ReadContract(parsed.ContractPath);
            Console.WriteLine(JsonSerializer.Serialize(contractSubjects, JsonOptions));
            return 0;
        }

        if (parsed.Assemblies.Count == 0)
            return Error("No assemblies specified. Use --assemblies <dll1.dll> ... or --contract <contract.json>");

        var reader = new MetadataReader();
        var dllSubjects = reader.ReadAssemblies(parsed.Assemblies);
        Console.WriteLine(JsonSerializer.Serialize(dllSubjects, JsonOptions));
        return 0;
    }

    private static int RunGenerate(string[] args)
    {
        var parsed = ParseArgs(args);
        if (parsed.Assemblies.Count == 0)
            return Error("No assemblies specified. Use --assemblies <dll1.dll> ...");

        if (parsed.OutputDir is null)
            return Error("No output directory specified. Use --output <dir>");

        if (parsed.ContractPath is null)
            return Error("No contract specified. Use --contract <contract.json>");

        // Step 1: Read subjects from contract
        Console.WriteLine("  [1/3] Reading subjects from contract...");
        var subjects = ContractReader.ReadContract(parsed.ContractPath);
        Console.WriteLine($"        Found {subjects.Count} subjects");

        if (subjects.Count == 0)
        {
            Console.WriteLine("  [WARNING] No subjects found — generating empty project.");
        }

        // Step 2: Run IL2CPP codegen
        Console.WriteLine("  [2/3] Running IL2CPP codegen...");
        var codegenMode = parsed.IsJit ? "jit" : "aot";
        var orchestrator = new Codegen.CodegenOrchestrator();
        var codegenResult = orchestrator.Run(parsed.Assemblies, Path.Combine(parsed.OutputDir, "codegen"), codegenMode);

        if (!codegenResult.Success)
            return Error($"Codegen failed: {codegenResult.Error}");

        Console.WriteLine($"        {codegenResult.GeneratedDirs.Count} generated directories");

        // Step 3: Emit C++ project via Scriban templates
        Console.WriteLine("  [3/3] Emitting C++ project...");
        var emitter = new CppProjectEmitter();
        emitter.Emit(
            parsed.OutputDir,
            codegenResult,
            subjects,
            isJit: parsed.IsJit,
            configTier: parsed.ConfigTier,
            isWindows: parsed.IsWindows);
        Console.WriteLine($"        entry.cpp, dispatch.cpp, CMakeLists.txt, metadata/subjects.json written");

        Console.WriteLine("  Done.");
        return 0;
    }

    private static int RunEmit(string[] args)
    {
        var parsed = ParseArgs(args);
        if (parsed.OutputDir is null)
            return Error("No output directory specified. Use --output <dir>");

        if (parsed.ContractPath is null)
            return Error("No contract specified. Use --contract <contract.json>");

        // Step 1: Read subjects from contract
        Console.WriteLine("  [1/2] Reading subjects from contract...");
        var subjects = ContractReader.ReadContract(parsed.ContractPath);
        Console.WriteLine($"        Found {subjects.Count} subjects");

        if (subjects.Count == 0)
        {
            Console.WriteLine("  [WARNING] No subjects found — generating empty dispatch.");
        }

        // Step 2: Emit dispatch + metadata only via Scriban templates
        Console.WriteLine("  [2/2] Emitting dispatch.cpp and metadata...");
        var emitter = new CppProjectEmitter();
        emitter.EmitDispatchOnly(
            parsed.OutputDir,
            subjects,
            isJit: parsed.IsJit,
            configTier: parsed.ConfigTier,
            isWindows: parsed.IsWindows);
        Console.WriteLine($"        verification_dispatch.generated.cpp, metadata/subjects.json written");

        Console.WriteLine("  Done.");
        return 0;
    }

    private static int Error(string message)
    {
        Console.Error.WriteLine($"Error: {message}");
        return 1;
    }

    private sealed record ParsedArgs(
        List<string> Assemblies,
        string? OutputDir,
        string? ContractPath,
        bool IsJit,
        string ConfigTier,
        bool IsWindows);

    private static ParsedArgs ParseArgs(string[] args)
    {
        var assemblies = new List<string>();
        string? outputDir = null;
        string? contractPath = null;
        bool isJit = false;
        var configTier = "check";
        var isWindows = true;

        for (int i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--assemblies":
                case "--assembly":
                    while (i + 1 < args.Length && !args[i + 1].StartsWith("--"))
                    {
                        assemblies.Add(args[++i]);
                    }
                    break;
                case "--output":
                    if (i + 1 < args.Length)
                        outputDir = args[++i];
                    break;
                case "--contract":
                    if (i + 1 < args.Length)
                        contractPath = args[++i];
                    break;
                case "--jit":
                    isJit = true;
                    break;
                case "--config-tier":
                    if (i + 1 < args.Length)
                        configTier = args[++i];
                    break;
                case "--platform":
                    if (i + 1 < args.Length)
                    {
                        var platform = args[++i].ToLowerInvariant();
                        isWindows = platform is "windows" or "win";
                    }
                    break;
            }
        }

        return new ParsedArgs(assemblies, outputDir, contractPath, isJit, configTier, isWindows);
    }
}
