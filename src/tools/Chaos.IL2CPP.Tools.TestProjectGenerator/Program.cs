using System.Text.Json;
using System.Text.Json.Serialization;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Emission;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Metadata;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator;

// ── Subjects metadata (from AutoTestGenerator) ──
// Schema: subjects.metadata.json
internal sealed record SubjectsMetadata(
    [property: JsonPropertyName("schemaVersion")] int SchemaVersion,
    [property: JsonPropertyName("assemblyName")] string AssemblyName,
    [property: JsonPropertyName("chunkSlug")] string ChunkSlug,
    [property: JsonPropertyName("totalMethods")] int TotalMethods,
    [property: JsonPropertyName("customEntryIndices")] IReadOnlyList<int>? CustomEntryIndices,
    [property: JsonPropertyName("benchmarkMethodIndices")] IReadOnlyList<int>? BenchmarkMethodIndices,
    [property: JsonPropertyName("hotupdateMethodIndices")] IReadOnlyList<int>? HotupdateMethodIndices,
    [property: JsonPropertyName("methods")] IReadOnlyList<SubjectMethodEntry> Methods);

internal sealed record SubjectMethodEntry(
    [property: JsonPropertyName("index")] int Index,
    [property: JsonPropertyName("kind")] string Kind,
    [property: JsonPropertyName("methodSubjectId")] string MethodSubjectId);

// ── Server protocol types ─────────────────────────────────────────────
// Line-delimited JSON on stdin/stdout:
//   → {"id":1,"cmd":"generate","contract":"...","output":"...","jit":false,...}
//   ← {"id":1,"status":"ok","entryExe":"..."}
//   → {"id":2,"cmd":"exit"}
//   ← {"id":2,"status":"ok"}
internal record ServerCommand(
    [property: JsonPropertyName("id")] int Id,
    [property: JsonPropertyName("cmd")] string Cmd,
    [property: JsonPropertyName("contract")] string? Contract = null,
    [property: JsonPropertyName("output")] string? Output = null,
    [property: JsonPropertyName("jit")] bool Jit = false,
    [property: JsonPropertyName("configTier")] string ConfigTier = "check",
    [property: JsonPropertyName("skipCodegen")] bool SkipCodegen = false,
    [property: JsonPropertyName("codegenDir")] string? CodegenDir = null,
    [property: JsonPropertyName("projectRoot")] string? ProjectRoot = null,
    [property: JsonPropertyName("assemblies")] List<string>? Assemblies = null,
    [property: JsonPropertyName("sourceOnly")] bool SourceOnly = false);

internal record ServerResponse(
    [property: JsonPropertyName("id")] int Id,
    [property: JsonPropertyName("status")] string Status,
    [property: JsonPropertyName("entryExe")] string? EntryExe = null,
    [property: JsonPropertyName("error")] string? Error = null);

public static class Program
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
    };

    private static readonly JsonSerializerOptions ServerJsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
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
            "generate-dll" => RunGenerateDll(args[1..]),
            "emit" => RunEmit(args[1..]),
            "server" => RunServer(),
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
                       [--project-root <path>] [--source-only] [--skip-codegen]
                       [--codegen-dir <path>]
                  Full project generation: runs IL2CPP codegen, emits complete C++ project,
                  then builds the native executable by default.

              generate-dll --dll <subjects.dll> --metadata <subjects.metadata.json> --output <dir>
                          [--config-tier check|profile|ship] [--source-only] [--clean]
                  Generate native entry.exe from AutoTestGenerator output DLL + metadata.
                  Runs IL2CPP codegen, emits C++ project, builds entry.exe.
                  --clean removes intermediate files (keeps only entry.exe).

              server  (no args)
                  Start TPG server mode. Reads line-delimited JSON commands from stdin,
                  writes JSON responses to stdout. All progress goes to stderr.

              emit --contract <contract.json> --output <dir>
                   [--jit] [--config-tier check|profile|ship] [--platform windows|linux|osx]
                  Emit dispatch.cpp and metadata from contract only (no IL2CPP codegen).


            Options:
              --jit                  Enable JIT mode (default: AOT)
              --config-tier <tier>   Build configuration: check, profile, or ship (default: check)
              --platform <os>        Target platform: windows, linux, or osx (default: windows)
              --project-root <path>  Chaos repository root (auto-detected from assembly path)
              --source-only          Only emit source files, skip native build (default: build)
              --skip-codegen         Skip IL2CPP codegen step (use with --codegen-dir)
              --codegen-dir <path>   Path to existing codegen output (for --skip-codegen)
            """);
    }

    // ── Server mode ──────────────────────────────────────────────────────
    // Reads line-delimited JSON commands from stdin, processes them, writes
    // JSON responses to stdout.  Keeps the process alive (no dotnet exec
    // overhead per family).  Progress output goes to stderr.
    private static int RunServer()
    {
        Console.Error.WriteLine("[TPG server] started, reading commands from stdin...");
        string? line;
        while ((line = Console.In.ReadLine()) != null)
        {
            line = line.Trim();
            if (string.IsNullOrEmpty(line)) continue;

            ServerCommand? cmd;
            try { cmd = JsonSerializer.Deserialize<ServerCommand>(line, ServerJsonOptions); }
            catch (JsonException ex)
            {
                var resp = new ServerResponse(0, "error", Error: $"Invalid JSON: {ex.Message}");
                Console.WriteLine(JsonSerializer.Serialize(resp, ServerJsonOptions));
                continue;
            }
            if (cmd is null) continue;

            switch (cmd.Cmd)
            {
                case "generate":
                {
                    var result = ExecuteServerGenerate(cmd);
                    Console.WriteLine(JsonSerializer.Serialize(result, ServerJsonOptions));
                    break;
                }
                case "exit":
                {
                    var resp = new ServerResponse(cmd.Id, "ok");
                    Console.WriteLine(JsonSerializer.Serialize(resp, ServerJsonOptions));
                    Console.Error.WriteLine("[TPG server] exiting");
                    return 0;
                }
                default:
                {
                    var resp = new ServerResponse(cmd.Id, "error", Error: $"Unknown command: {cmd.Cmd}");
                    Console.WriteLine(JsonSerializer.Serialize(resp, ServerJsonOptions));
                    break;
                }
            }
        }
        return 0;
    }

    private static ServerResponse ExecuteServerGenerate(ServerCommand cmd)
    {
        if (string.IsNullOrEmpty(cmd.Contract))
            return new ServerResponse(cmd.Id, "error", Error: "Missing 'contract' field");
        if (string.IsNullOrEmpty(cmd.Output))
            return new ServerResponse(cmd.Id, "error", Error: "Missing 'output' field");

        var contractPath = cmd.Contract;
        var outputDir = cmd.Output;

        var projectRoot = cmd.ProjectRoot;
        if (projectRoot is not null)
            projectRoot = Path.GetFullPath(projectRoot);

        var codegenBase = cmd.CodegenDir is not null
            ? Path.GetFullPath(cmd.CodegenDir)
            : Path.GetFullPath(Path.Combine(outputDir, "codegen"));

        // Step 1: Read subjects from contract
        Console.Error.WriteLine("  [1/3] Reading subjects from contract...");
        var subjects = ContractReader.ReadContract(contractPath);
        Console.Error.WriteLine($"        Found {subjects.Count} subjects");

        // Step 2: Codegen (or skip)
        Codegen.CodegenResult codegenResult;
        if (cmd.SkipCodegen)
        {
            Console.Error.WriteLine("  [2/3] Skipping codegen, using existing output...");
            var generatedDirs = new List<string>();
            if (Directory.Exists(codegenBase))
            {
                foreach (var subDir in Directory.GetDirectories(codegenBase))
                {
                    var genDir = Path.Combine(subDir, "generated");
                    if (Directory.Exists(genDir) && Directory.GetFiles(genDir, "*.cpp").Length > 0)
                        generatedDirs.Add(genDir);
                }
            }

            codegenResult = new Codegen.CodegenResult
            {
                Success = true,
                OutputDir = codegenBase,
                GeneratedDirs = generatedDirs,
            };
        }
        else
        {
            Console.Error.WriteLine("  [2/3] Running IL2CPP codegen...");
            var codegenMode = cmd.Jit ? "jit" : "aot";
            var orchestrator = new Codegen.CodegenOrchestrator();
            codegenResult = orchestrator.Run(cmd.Assemblies ?? [], codegenBase, codegenMode);

            if (!codegenResult.Success)
                return new ServerResponse(cmd.Id, "error", Error: codegenResult.Error);

            Console.Error.WriteLine($"        {codegenResult.GeneratedDirs.Count} generated directories");
        }

        // Resolve SDK directory
        var sdkDir = codegenBase;
        if (codegenBase is not null && Directory.Exists(codegenBase))
        {
            // Check if codegenBase itself is a valid SDK root
            if (File.Exists(Path.Combine(codegenBase, "chaos-config.cmake")))
            {
                sdkDir = codegenBase;
            }
            else
            {
                // Check subdirectories for per-assembly SDK layout
                foreach (var subDir in Directory.GetDirectories(codegenBase))
                {
                    if (File.Exists(Path.Combine(subDir, "chaos-config.cmake")) &&
                        File.Exists(Path.Combine(subDir, "cmake", "chaos-targets.cmake")))
                    {
                        sdkDir = subDir;
                        break;
                    }
                }

                // If not found, check parent directory (pipeline mode:
                // codegenBase = <SubjectDir>/generated/, config lives in <SubjectDir>/)
                if (sdkDir == codegenBase)
                {
                    var parent = Path.GetDirectoryName(codegenBase);
                    if (parent is not null && File.Exists(Path.Combine(parent, "chaos-config.cmake")))
                    {
                        sdkDir = parent;
                    }
                }
            }
        }

        // Step 3: Emit + optionally build
        Console.Error.WriteLine("  [3/3] Emitting C++ project...");
        var emitter = new CppProjectEmitter();

        if (cmd.SourceOnly)
        {
            emitter.Emit(
                outputDir, codegenResult, subjects,
                isJit: cmd.Jit, configTier: cmd.ConfigTier,
                isWindows: true, projectRoot: projectRoot,
                codegenDir: codegenBase, sdkDir: sdkDir);
            Console.Error.WriteLine("        Sources written (source-only mode)");
            return new ServerResponse(cmd.Id, "ok");
        }

        var exePath = emitter.GenerateAndBuild(
            outputDir, codegenResult, subjects,
            isJit: cmd.Jit, configTier: cmd.ConfigTier,
            isWindows: true, projectRoot: projectRoot,
            codegenDir: codegenBase, sdkDir: sdkDir);

        if (exePath is null)
            return new ServerResponse(cmd.Id, "error", Error: "Build failed");

        Console.Error.WriteLine($"        entry.exe produced at: {exePath}");
        return new ServerResponse(cmd.Id, "ok", EntryExe: exePath);
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

        // Auto-detect project root from assembly path if not explicitly provided
        var projectRoot = parsed.ProjectRoot;
        if (projectRoot is null && parsed.Assemblies.Count > 0)
        {
            projectRoot = DetectProjectRoot(parsed.Assemblies[0]);
        }
        if (projectRoot is not null)
            projectRoot = Path.GetFullPath(projectRoot);

        // Resolve codegen directory (--codegen-dir or default to <output>/codegen)
        var codegenBase = parsed.CodegenDir is not null
            ? Path.GetFullPath(parsed.CodegenDir)
            : Path.GetFullPath(Path.Combine(parsed.OutputDir, "codegen"));

        // Step 1: Read subjects from contract
        Console.WriteLine("  [1/3] Reading subjects from contract...");
        var subjects = ContractReader.ReadContract(parsed.ContractPath);
        Console.WriteLine($"        Found {subjects.Count} subjects");

        if (subjects.Count == 0)
        {
            Console.WriteLine("  [WARNING] No subjects found — generating empty project.");
        }

        // Step 2: Run IL2CPP codegen (skip if --skip-codegen is set)
        Codegen.CodegenResult codegenResult;
        if (parsed.SkipCodegen)
        {
            Console.WriteLine("  [2/3] Skipping codegen (--skip-codegen), using existing output...");
            var generatedDirs = new List<string>();
            if (Directory.Exists(codegenBase))
            {
                foreach (var subDir in Directory.GetDirectories(codegenBase))
                {
                    var genDir = Path.Combine(subDir, "generated");
                    if (Directory.Exists(genDir) && Directory.GetFiles(genDir, "*.cpp").Length > 0)
                        generatedDirs.Add(genDir);
                }
            }

            codegenResult = new Codegen.CodegenResult
            {
                Success = true,
                OutputDir = codegenBase,
                GeneratedDirs = generatedDirs,
            };
        }
        else
        {
            Console.WriteLine("  [2/3] Running IL2CPP codegen...");
            var codegenMode = parsed.IsJit ? "jit" : "aot";
            var orchestrator = new Codegen.CodegenOrchestrator();
            codegenResult = orchestrator.Run(parsed.Assemblies, codegenBase, codegenMode);

            if (!codegenResult.Success)
                return Error($"Codegen failed: {codegenResult.Error}");

            Console.WriteLine($"        {codegenResult.GeneratedDirs.Count} generated directories");
        }

        // Resolve SDK directory: find chaos-config.cmake in codegen output subdirectories
        var sdkDir = codegenBase; // fallback
        if (codegenBase is not null && Directory.Exists(codegenBase))
        {
            // Check if codegenBase itself is a valid SDK root
            if (File.Exists(Path.Combine(codegenBase, "chaos-config.cmake")))
            {
                sdkDir = codegenBase;
            }
            else
            {
                foreach (var subDir in Directory.GetDirectories(codegenBase))
                {
                    if (File.Exists(Path.Combine(subDir, "chaos-config.cmake")) &&
                        File.Exists(Path.Combine(subDir, "cmake", "chaos-targets.cmake")))
                    {
                        sdkDir = subDir;
                        break;
                    }
                }

                // Not found — check parent (pipeline mode: codegenBase is generated/, config in parent)
                if (sdkDir == codegenBase)
                {
                    var parent = Path.GetDirectoryName(codegenBase);
                    if (parent is not null && File.Exists(Path.Combine(parent, "chaos-config.cmake")))
                    {
                        sdkDir = parent;
                    }
                }
            }
        }

        // Step 3: Emit C++ project via Scriban templates
        Console.WriteLine("  [3/3] Emitting C++ project...");
        var emitter = new CppProjectEmitter();

        if (parsed.SourceOnly)
        {
            // Source-only mode: emit files but don't build
            emitter.Emit(
                parsed.OutputDir,
                codegenResult,
                subjects,
                isJit: parsed.IsJit,
                configTier: parsed.ConfigTier,
                isWindows: parsed.IsWindows,
                projectRoot: projectRoot,
                codegenDir: codegenBase,
                sdkDir: sdkDir);
            Console.WriteLine($"        runtime-entry.cpp, dispatch.cpp, CMakeLists.txt, metadata/subjects.json written");
        }
        else
        {
            // Default: emit + build
            var exePath = emitter.GenerateAndBuild(
                parsed.OutputDir,
                codegenResult,
                subjects,
                isJit: parsed.IsJit,
                configTier: parsed.ConfigTier,
                isWindows: parsed.IsWindows,
                projectRoot: projectRoot,
                codegenDir: codegenBase,
                sdkDir: sdkDir);
            if (exePath is null)
                return Error("Build failed — executable not produced");
            Console.WriteLine($"        entry.exe produced at: {exePath}");
        }

        Console.WriteLine("  Done.");
        return 0;
    }

    /// <summary>
    /// Generate native entry.exe from AutoTestGenerator output DLL + metadata.
    /// This is the new DLL-mode entry point for the foundation-dll pipeline.
    /// </summary>
    private static int RunGenerateDll(string[] args)
    {
        string? dllPath = null;
        string? metadataPath = null;
        string? outputDir = null;
        var configTier = "check";
        var sourceOnly = false;
        var clean = false;

        for (int i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--dll" when i + 1 < args.Length:
                    dllPath = Path.GetFullPath(args[++i]);
                    break;
                case "--metadata" when i + 1 < args.Length:
                    metadataPath = Path.GetFullPath(args[++i]);
                    break;
                case "--output" when i + 1 < args.Length:
                    outputDir = Path.GetFullPath(args[++i]);
                    break;
                case "--config-tier" when i + 1 < args.Length:
                    configTier = args[++i];
                    break;
                case "--source-only":
                    sourceOnly = true;
                    break;
                case "--clean":
                    clean = true;
                    break;
            }
        }

        if (dllPath is null || !File.Exists(dllPath))
            return Error($"DLL not found: {dllPath}");
        if (metadataPath is null || !File.Exists(metadataPath))
            return Error($"Metadata not found: {metadataPath}");
        if (outputDir is null)
            return Error("No output directory. Use --output <dir>");

        Console.WriteLine("  [1/4] Reading subjects metadata...");
        var metadata = JsonSerializer.Deserialize<SubjectsMetadata>(
            File.ReadAllText(metadataPath), JsonOptions);
        if (metadata is null || metadata.Methods.Count == 0)
            return Error("No subjects found in metadata");

        // Convert metadata entries to SubjectModel list
        var subjects = new List<SubjectModel>();
        foreach (var entry in metadata.Methods)
        {
            // Use the ContractReader's parser to convert methodSubjectId → SubjectModel
            try
            {
                var kind = entry.Kind switch
                {
                    "benchmark" => SubjectKind.Benchmark,
                    "hotupdate" => SubjectKind.HotUpdate,
                    _ => SubjectKind.Fact
                };
                var model = Metadata.ContractReader.ParseSingle(entry.MethodSubjectId, kind);
                subjects.Add(model);
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine($"  [WARN] Skipping {entry.MethodSubjectId}: {ex.Message}");
            }
        }
        Console.WriteLine($"        {subjects.Count} subjects from chunk '{metadata.ChunkSlug}'");

        // Step 2: Run IL2CPP codegen
        Console.WriteLine("  [2/4] Running IL2CPP codegen...");
        var codegenBase = Path.Combine(outputDir, "codegen");
        var orchestrator = new Codegen.CodegenOrchestrator();
        var codegenResult = orchestrator.Run([dllPath], codegenBase, "aot");

        if (!codegenResult.Success)
            return Error($"Codegen failed: {codegenResult.Error}");

        // Resolve SDK directory
        var sdkDir = ResolveSdkDir(codegenBase);

        // Step 3: Emit C++ project
        Console.WriteLine("  [3/4] Emitting C++ project...");
        var emitter = new CppProjectEmitter();

        if (sourceOnly)
        {
            emitter.Emit(outputDir, codegenResult, subjects,
                isJit: false, configTier: configTier,
                isWindows: true, projectRoot: null,
                codegenDir: codegenBase, sdkDir: sdkDir);
            Console.WriteLine("        Sources written (source-only mode)");
            return 0;
        }

        var exePath = emitter.GenerateAndBuild(outputDir, codegenResult, subjects,
            isJit: false, configTier: configTier,
            isWindows: true, projectRoot: null,
            codegenDir: codegenBase, sdkDir: sdkDir);

        if (exePath is null)
            return Error("Build failed");

        Console.WriteLine($"        entry.exe: {exePath}");

        // Step 4: Clean up intermediate files (optional)
        if (clean)
        {
            Console.WriteLine("  [4/4] Cleaning intermediate files...");
            CleanIntermediateFiles(outputDir);
            Console.WriteLine("        Kept only entry.exe");
        }

        Console.WriteLine("  Done.");
        return 0;
    }

    /// <summary>
    /// Resolve SDK directory from codegen output.
    /// </summary>
    private static string ResolveSdkDir(string codegenBase)
    {
        if (!Directory.Exists(codegenBase))
            return codegenBase;

        // Check if codegenBase itself has chaos-config.cmake
        if (File.Exists(Path.Combine(codegenBase, "chaos-config.cmake")))
            return codegenBase;

        // Check subdirectories
        foreach (var subDir in Directory.GetDirectories(codegenBase))
        {
            if (File.Exists(Path.Combine(subDir, "chaos-config.cmake")))
                return subDir;
        }

        // Check parent (pipeline layout: generated/ in subdir, config in parent)
        var parent = Path.GetDirectoryName(codegenBase);
        if (parent is not null && File.Exists(Path.Combine(parent, "chaos-config.cmake")))
            return parent;

        return codegenBase;
    }

    /// <summary>
    /// Remove intermediate build artifacts, keeping only entry.exe.
    /// </summary>
    private static void CleanIntermediateFiles(string outputDir)
    {
        var keepDirs = new HashSet<string>(StringComparer.OrdinalIgnoreCase)
        {
            "build",           // CMake build output (entry.exe lives here)
        };
        var keepFiles = new HashSet<string>(StringComparer.OrdinalIgnoreCase)
        {
            "entry.exe",
            "entry",
        };

        foreach (var dir in Directory.GetDirectories(outputDir))
        {
            var name = Path.GetFileName(dir);
            if (keepDirs.Contains(name)) continue;

            try { Directory.Delete(dir, recursive: true); }
            catch { /* best-effort */ }
        }

        foreach (var file in Directory.GetFiles(outputDir, "*.cpp"))
        {
            try { File.Delete(file); }
            catch { /* best-effort */ }
        }
        foreach (var file in Directory.GetFiles(outputDir, "*.h"))
        {
            try { File.Delete(file); }
            catch { /* best-effort */ }
        }
        foreach (var file in Directory.GetFiles(outputDir, "*.json"))
        {
            var name = Path.GetFileName(file);
            if (keepFiles.Contains(name)) continue;
            try { File.Delete(file); }
            catch { /* best-effort */ }
        }
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
        bool IsWindows,
        string? ProjectRoot,
        bool SourceOnly,
        bool SkipCodegen,
        string? CodegenDir);

    private static ParsedArgs ParseArgs(string[] args)
    {
        var assemblies = new List<string>();
        string? outputDir = null;
        string? contractPath = null;
        bool isJit = false;
        var configTier = "check";
        var isWindows = true;
        string? projectRoot = null;
        var sourceOnly = false;
        var skipCodegen = false;
        string? codegenDir = null;

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
                case "--project-root":
                    if (i + 1 < args.Length)
                        projectRoot = args[++i];
                    break;
                case "--source-only":
                    sourceOnly = true;
                    break;
                case "--skip-codegen":
                    skipCodegen = true;
                    break;
                case "--codegen-dir":
                    if (i + 1 < args.Length)
                        codegenDir = args[++i];
                    break;
            }
        }

        return new ParsedArgs(assemblies, outputDir, contractPath, isJit, configTier, isWindows, projectRoot, sourceOnly, skipCodegen, codegenDir);
    }

    /// <summary>
    /// Walk up from a DLL path to find the repo root (looks for src/ directory).
    /// </summary>
    private static string? DetectProjectRoot(string assemblyPath)
    {
        var dir = new DirectoryInfo(Path.GetDirectoryName(Path.GetFullPath(assemblyPath))!);
        while (dir is not null)
        {
            if (Directory.Exists(Path.Combine(dir.FullName, "src")))
                return dir.FullName.Replace("\\", "/");
            dir = dir.Parent;
        }
        return null;
    }
}
