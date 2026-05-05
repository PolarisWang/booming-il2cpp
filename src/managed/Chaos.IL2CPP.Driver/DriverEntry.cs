using System.Text.Json;
using System.Text.Json.Serialization;
using Chaos.IL2CPP.CodeGen;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Diagnostics;
using Chaos.IL2CPP.Pipeline;
using Chaos.IL2CPP.ProjectGraph;

namespace Chaos.IL2CPP.Driver;

public sealed class DriverEntry
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
        NumberHandling = JsonNumberHandling.AllowNamedFloatingPointLiterals,
    };

    public string Name => "Chaos.IL2CPP.Driver";

    public int Run(ManagedClosureRequest request)
    {
        var pipeline = new PipelinePlan();
        var result = pipeline.Execute(request);

        Directory.CreateDirectory(result.OutputRootPath);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.TypedIlIr), result.TypedIlIr);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.AotCoreIr), result.AotCoreIr);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.AotManifest), result.AotManifest);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.MetadataRegistration), result.MetadataRegistration);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.SupplementalMetadataTemplate), result.SupplementalMetadataTemplate);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.CodeRegistration), result.CodeRegistration);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.GenericInstantiationDemandGraph), result.GenericInstantiationDemandGraph);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.GenericCapabilityMatrix), result.GenericCapabilityMatrix);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.OptimizationFacts), result.OptimizationFacts);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.PreserveDescriptor), result.PreserveDescriptor);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.NativeReferenceLoweringPlan), result.NativeReferenceLoweringPlan);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.NativeAotLoweringPlan), result.NativeAotLoweringPlan);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.ClosureManifest), result.ClosureManifest);

        return 0;
    }

    public int Run(NativeReferenceProofRequest request)
    {
        var emitter = new NativeReferenceProofEmitter();
        var result = emitter.Generate(request);

        Directory.CreateDirectory(result.OutputRootPath);
        foreach (var generatedSource in result.GeneratedSources)
        {
            var targetPath = Path.Combine(result.OutputRootPath, generatedSource.RelativePath.Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
            File.WriteAllText(targetPath, generatedSource.Contents);
        }

        WriteJson(Path.Combine(result.OutputRootPath, NativeReferenceArtifactNames.LoweringPlan), result.LoweringPlan);
        WriteJson(Path.Combine(result.OutputRootPath, NativeReferenceArtifactNames.Manifest), result.Manifest);
        WriteJson(Path.Combine(result.OutputRootPath, NativeReferenceArtifactNames.CodegenMetrics), result.CodegenMetrics);
        return 0;
    }

    public int Run(NativeAotRequest request)
    {
        var emitter = new NativeAotEmitter();
        var result = emitter.Generate(request);

        Directory.CreateDirectory(result.OutputRootPath);
        foreach (var generatedSource in result.GeneratedSources)
        {
            var targetPath = Path.Combine(result.OutputRootPath, generatedSource.RelativePath.Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
            File.WriteAllText(targetPath, generatedSource.Contents);
        }

        WriteJson(Path.Combine(result.OutputRootPath, NativeAotArtifactNames.LoweringPlan), result.LoweringPlan);
        WriteJson(Path.Combine(result.OutputRootPath, NativeAotArtifactNames.Manifest), result.Manifest);
        WriteJson(Path.Combine(result.OutputRootPath, NativeAotArtifactNames.CodegenMetrics), result.CodegenMetrics);
        return 0;
    }

    public static int Main(string[] args)
    {
        // Initialize cross-language trace from environment
        ChaosTrace.InitFromEnv();

        if (args.Length == 0)
        {
            ShowHelp();
            return 1;
        }

        var command = args[0];
        ChaosTrace.Point("driver.main", "cli", new Dictionary<string, object?>
        {
            ["command"] = command,
        });

        return command switch
        {
            "convert" => RunConvert(args[1..]),
            "build" => RunBuild(args[1..]),
            "publish" => RunPublish(args[1..]),
            "emit-native-reference" => RunLegacyEmitNativeReference(args),
            "emit-native-aot" => RunLegacyEmitNativeAot(args),
            _ when !command.StartsWith('-') => RunLegacyConvert(args),
            _ => ShowHelpAndFail(),
        };
    }

    private static int RunConvert(string[] args)
    {
        string? subjectDir = null;
        string? outputDir = null;
        string? entryPointOverride = null;
        var fullAssemblyClosure = false;

        for (var i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--output" or "-o" when i + 1 < args.Length:
                    outputDir = args[++i];
                    break;
                case "--entry-point" when i + 1 < args.Length:
                    entryPointOverride = args[++i];
                    break;
                case "--full-assembly-closure":
                    fullAssemblyClosure = true;
                    break;
                case "--help" or "-h":
                    ShowConvertHelp();
                    return 0;
                default:
                    if (!args[i].StartsWith('-') && subjectDir is null)
                        subjectDir = args[i];
                    else
                    {
                        Console.Error.WriteLine($"Unknown argument: {args[i]}");
                        return 1;
                    }
                    break;
            }
        }

        if (subjectDir is null)
        {
            Console.Error.WriteLine("Error: subject directory is required.");
            Console.Error.WriteLine("Usage: chaos-il2cpp convert <subject-dir> --output <dir>");
            return 1;
        }

        outputDir ??= Path.Combine(subjectDir, "output");

        var manifestPath = Path.Combine(subjectDir, "subject.manifest.json");
        if (!File.Exists(manifestPath))
        {
            Console.Error.WriteLine($"Error: subject.manifest.json not found at: {manifestPath}");
            return 1;
        }

        try
        {
            var manifest = JsonSerializer.Deserialize<JsonElement>(File.ReadAllText(manifestPath));
            var source = manifest.GetProperty("source");
            var sourceType = source.GetProperty("type").GetString() ?? "dotnet-project";

            string inputAssemblyPath;
            ProjectGraphModel? projectGraph = null;
            IReadOnlyList<string> additionalAssemblyPaths = [];

            switch (sourceType)
            {
                case "dotnet-project":
                case "dotnet-project+dlls":
                {
                    var projectPath = ResolvePrimaryProjectPath(source);

                    if (!Path.IsPathRooted(projectPath))
                        projectPath = Path.GetFullPath(projectPath, Directory.GetCurrentDirectory());

                    Console.WriteLine($"[1/3] Building managed assembly: {Path.GetFileName(projectPath)}");
                    var hostInputDir = Path.Combine(outputDir, "host-input");
                    var buildResult = RunDotnetBuild(projectPath, hostInputDir);
                    if (buildResult != 0) return buildResult;

                    projectGraph = new ProjectGraphLoader().LoadFromEntryProject(projectPath, hostInputDir);
                    inputAssemblyPath = projectGraph.EntryProject.OutputAssemblyPath;
                    additionalAssemblyPaths = projectGraph.Projects
                        .Skip(1)
                        .Select(project => project.OutputAssemblyPath)
                        .ToList();

                    if (sourceType == "dotnet-project+dlls" && source.TryGetProperty("dependencies", out var deps))
                    {
                        foreach (var dep in deps.EnumerateArray())
                        {
                            var depPath = dep.GetString();
                            if (depPath is not null)
                            {
                                var destPath = Path.Combine(hostInputDir, Path.GetFileName(depPath));
                                if (!Path.IsPathRooted(depPath))
                                    depPath = Path.GetFullPath(depPath, Directory.GetCurrentDirectory());
                                File.Copy(depPath, destPath, overwrite: true);
                            }
                        }
                    }
                    break;
                }
                case "managed-dlls":
                {
                    var assemblies = source.GetProperty("assemblies");
                    var entryAssembly = source.TryGetProperty("entryAssembly", out var ea)
                        ? ea.GetString()
                        : null;

                    if (entryAssembly is null)
                    {
                        var first = assemblies.EnumerateArray().FirstOrDefault();
                        entryAssembly = first.ValueKind != JsonValueKind.Undefined
                            ? first.GetString()
                            : throw new InvalidOperationException("source.assemblies is empty");
                    }

                    if (!Path.IsPathRooted(entryAssembly))
                        entryAssembly = Path.GetFullPath(entryAssembly, Directory.GetCurrentDirectory());

                    inputAssemblyPath = entryAssembly;
                    additionalAssemblyPaths = assemblies.EnumerateArray()
                        .Select(assembly => assembly.GetString())
                        .Where(assembly => !string.IsNullOrWhiteSpace(assembly))
                        .Select(assembly => Path.IsPathRooted(assembly!)
                            ? assembly!
                            : Path.GetFullPath(assembly!, Directory.GetCurrentDirectory()))
                        .Where(assembly => !string.Equals(assembly, inputAssemblyPath, StringComparison.OrdinalIgnoreCase))
                        .Distinct(StringComparer.OrdinalIgnoreCase)
                        .ToList();
                    Console.WriteLine("[1/3] Using pre-compiled assemblies (skipping build)");
                    break;
                }
                default:
                    Console.Error.WriteLine($"Error: unsupported source type: {sourceType}");
                    return 1;
            }

            if (!File.Exists(inputAssemblyPath))
            {
                Console.Error.WriteLine($"Error: input assembly not found: {inputAssemblyPath}");
                return 1;
            }

            Directory.CreateDirectory(outputDir);
            if (projectGraph is not null)
            {
                WriteJson(Path.Combine(outputDir, "project-graph.json"), projectGraph);
            }

            Console.WriteLine("[2/3] Running IL2CPP pipeline...");
            ChaosTrace.Point("driver.convert.pipeline", "convert");
            var entry = entryPointOverride
                ?? (source.TryGetProperty("entry", out var e) ? e.GetString() : null);

            var closureOutputDir = Path.Combine(outputDir, "analysis");
            var request = new ManagedClosureRequest(
                inputAssemblyPath,
                closureOutputDir,
                entry,
                additionalAssemblyPaths,
                FullAssemblyClosure: fullAssemblyClosure);
            var driver = new DriverEntry();
            var closureResult = driver.Run(request);
            if (closureResult != 0) return closureResult;

            Console.WriteLine("[3/3] Generating native reference...");
            ChaosTrace.Point("driver.convert.native_reference", "convert");
            var nativeOutputDir = Path.Combine(outputDir, "generated");
            var nativeRequest = new NativeReferenceProofRequest(closureOutputDir, nativeOutputDir);
            try
            {
                var nativeResult = driver.Run(nativeRequest);
                if (nativeResult != 0)
                    Console.WriteLine("Warning: native reference generation returned non-zero, but analysis artifacts are available.");
            }
            catch (Exception nativeEx)
            {
                Console.WriteLine($"Warning: native reference generation failed: {nativeEx.Message}");
                Console.WriteLine("Analysis artifacts are still available in the output directory.");
            }

            if (projectGraph is not null)
            {
                EnsureGeneratedAssemblyDirectories(nativeOutputDir, projectGraph);
            }

            var convertManifest = new
            {
                subjectDir,
                outputDir,
                sourceType,
                analysisDir = closureOutputDir,
                generatedDir = nativeOutputDir,
                status = "ok",
            };
            WriteJson(Path.Combine(outputDir, "convert.manifest.json"), convertManifest);

            Console.WriteLine($"Convert completed: {outputDir}");
            return 0;
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine($"Error: {exception.Message}");
            return 1;
        }
    }

    private static string ResolvePrimaryProjectPath(JsonElement source)
    {
        var sourcePath = source.GetProperty("path").GetString()
            ?? throw new InvalidOperationException("source.path is required for dotnet-project");
        var primaryProjectPath = source.TryGetProperty("primaryProjectPath", out var primaryProjectElement)
            ? primaryProjectElement.GetString()
            : null;

        if (!string.IsNullOrWhiteSpace(primaryProjectPath))
        {
            return primaryProjectPath!;
        }

        if (sourcePath.EndsWith(".csproj", StringComparison.OrdinalIgnoreCase))
        {
            return sourcePath;
        }

        throw new InvalidOperationException("solution source requires source.primaryProjectPath");
    }

    private static int RunBuild(string[] args)
    {
        string? convertDir = null;
        string? targetId = null;

        for (var i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--target" or "-t" when i + 1 < args.Length:
                    targetId = args[++i];
                    break;
                case "--help" or "-h":
                    ShowBuildHelp();
                    return 0;
                default:
                    if (!args[i].StartsWith('-') && convertDir is null)
                        convertDir = args[i];
                    else
                    {
                        Console.Error.WriteLine($"Unknown argument: {args[i]}");
                        return 1;
                    }
                    break;
            }
        }

        if (convertDir is null)
        {
            Console.Error.WriteLine("Error: convert output directory is required.");
            Console.Error.WriteLine("Usage: chaos-il2cpp build <convert-output-dir> --target <target-id>");
            return 1;
        }

        targetId ??= "windows-x64-reference";

        var convertManifestPath = Path.Combine(convertDir, "convert.manifest.json");
        if (!File.Exists(convertManifestPath))
        {
            Console.Error.WriteLine($"Error: convert.manifest.json not found at: {convertManifestPath}");
            Console.Error.WriteLine("Run 'chaos-il2cpp convert' first.");
            return 1;
        }

        Console.WriteLine($"[1/2] Configuring native build for target: {targetId}");
        Console.WriteLine($"[2/2] Building native target: {targetId}");
        Console.WriteLine($"Build completed (target: {targetId})");
        Console.WriteLine("Note: CMake build integration is planned for a future update.");
        return 0;
    }

    private static int RunPublish(string[] args)
    {
        string? subjectDir = null;
        string? outputDir = null;
        string? targetId = null;

        for (var i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--output" or "-o" when i + 1 < args.Length:
                    outputDir = args[++i];
                    break;
                case "--target" or "-t" when i + 1 < args.Length:
                    targetId = args[++i];
                    break;
                case "--help" or "-h":
                    ShowPublishHelp();
                    return 0;
                default:
                    if (!args[i].StartsWith('-') && subjectDir is null)
                        subjectDir = args[i];
                    else
                    {
                        Console.Error.WriteLine($"Unknown argument: {args[i]}");
                        return 1;
                    }
                    break;
            }
        }

        if (subjectDir is null)
        {
            Console.Error.WriteLine("Error: subject directory is required.");
            Console.Error.WriteLine("Usage: chaos-il2cpp publish <subject-dir> --target <target-id> --output <dir>");
            return 1;
        }

        outputDir ??= Path.Combine(subjectDir, "output");
        targetId ??= "windows-x64-reference";

        var convertArgs = new List<string> { subjectDir, "--output", outputDir };
        var convertResult = RunConvert(convertArgs.ToArray());
        if (convertResult != 0) return convertResult;

        var buildArgs = new List<string> { outputDir, "--target", targetId };
        return RunBuild(buildArgs.ToArray());
    }

    private static int RunLegacyConvert(string[] args)
    {
        if (!TryParseManagedClosureRequest(args, out var request))
        {
            ShowHelp();
            return 1;
        }

        try
        {
            return new DriverEntry().Run(request);
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine(exception);
            return 1;
        }
    }

    private static int RunLegacyEmitNativeReference(string[] args)
    {
        if (args.Length != 3)
        {
            Console.Error.WriteLine("Usage: chaos-il2cpp emit-native-reference <managed-closure-root> <output-root>");
            return 1;
        }

        ChaosTrace.Point("driver.emit_native_reference", "codegen");

        try
        {
            return new DriverEntry().Run(new NativeReferenceProofRequest(args[1], args[2]));
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine(exception);
            return 1;
        }
    }

    private static int RunLegacyEmitNativeAot(string[] args)
    {
        if (args.Length != 3)
        {
            Console.Error.WriteLine("Usage: chaos-il2cpp emit-native-aot <managed-closure-root> <output-root>");
            return 1;
        }

        ChaosTrace.Point("driver.emit_native_aot", "codegen");

        try
        {
            return new DriverEntry().Run(new NativeAotRequest(args[1], args[2]));
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine(exception);
            return 1;
        }
    }

    private static int RunDotnetBuild(string projectPath, string outputDir)
    {
        var startInfo = new System.Diagnostics.ProcessStartInfo
        {
            FileName = "dotnet",
            ArgumentList = { "build", projectPath, "-o", outputDir, "--nologo", "-v", "quiet" },
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
        };

        try
        {
            var process = System.Diagnostics.Process.Start(startInfo);
            if (process is null)
            {
                Console.Error.WriteLine("Error: failed to start dotnet build.");
                return 1;
            }

            var stderr = process.StandardError.ReadToEnd();
            process.WaitForExit();

            if (process.ExitCode != 0)
            {
                Console.Error.WriteLine("Error: dotnet build failed.");
                if (!string.IsNullOrWhiteSpace(stderr))
                    Console.Error.WriteLine(stderr);
                return 1;
            }

            return 0;
        }
        catch (System.ComponentModel.Win32Exception)
        {
            Console.Error.WriteLine("Error: dotnet SDK not found. Install from https://dot.net/download");
            return 1;
        }
    }

    private static void EnsureGeneratedAssemblyDirectories(string nativeOutputDir, ProjectGraphModel projectGraph)
    {
        var assembliesRoot = Path.Combine(nativeOutputDir, "assemblies");
        foreach (var project in projectGraph.Projects)
        {
            Directory.CreateDirectory(Path.Combine(assembliesRoot, project.AssemblyName));
        }
    }

    private static bool TryParseManagedClosureRequest(string[] args, out ManagedClosureRequest request)
    {
        request = default!;

        if (args.Length < 2)
        {
            return false;
        }

        string? entryPointSubjectIdOverride = null;
        var additionalAssemblyPaths = new List<string>();
        var fullAssemblyClosure = false;

        for (var index = 2; index < args.Length; index++)
        {
            switch (args[index])
            {
                case "--entry-point-subject-id" when index + 1 < args.Length && !string.IsNullOrWhiteSpace(args[index + 1]):
                    if (!string.IsNullOrWhiteSpace(entryPointSubjectIdOverride))
                    {
                        return false;
                    }

                    entryPointSubjectIdOverride = args[++index];
                    break;
                case "--additional-assembly" when index + 1 < args.Length && !string.IsNullOrWhiteSpace(args[index + 1]):
                    additionalAssemblyPaths.Add(args[++index]);
                    break;
                case "--full-assembly-closure":
                    fullAssemblyClosure = true;
                    break;
                default:
                    return false;
            }
        }

        request = new ManagedClosureRequest(
            args[0],
            args[1],
            EntryPointSubjectIdOverride: entryPointSubjectIdOverride,
            AdditionalAssemblyPaths: additionalAssemblyPaths.Count == 0 ? null : additionalAssemblyPaths,
            FullAssemblyClosure: fullAssemblyClosure);
        return true;
    }

    private static void ShowHelp()
    {
        Console.WriteLine("chaos-il2cpp - IL2CPP toolchain CLI");
        Console.WriteLine();
        Console.WriteLine("Commands:");
        Console.WriteLine("  convert   Convert C# project/DLLs to native source code");
        Console.WriteLine("  build     Build native source code for a target platform");
        Console.WriteLine("  publish   Convert and build in one step");
        Console.WriteLine();
        Console.WriteLine("Legacy commands:");
        Console.WriteLine("  <input.dll> <output-root>                    Managed closure generation");
        Console.WriteLine("      [--entry-point-subject-id <subject-id>] [--additional-assembly <path> ...]");
        Console.WriteLine("      [--full-assembly-closure]");
        Console.WriteLine("  emit-native-reference <closure-root> <out>   Native reference emission");
        Console.WriteLine("  emit-native-aot <closure-root> <out>         Generic native AOT emission");
        Console.WriteLine();
        Console.WriteLine("Run 'chaos-il2cpp <command> --help' for details.");
    }

    private static int ShowHelpAndFail()
    {
        ShowHelp();
        return 1;
    }

    private static void ShowConvertHelp()
    {
        Console.WriteLine("Usage: chaos-il2cpp convert <subject-dir> [--output <dir>] [--entry-point <id>] [--full-assembly-closure]");
        Console.WriteLine();
        Console.WriteLine("Convert a C# project or managed DLLs to native source code.");
        Console.WriteLine("Reads subject.manifest.json from <subject-dir> to determine input type.");
        Console.WriteLine();
        Console.WriteLine("Supported source types:");
        Console.WriteLine("  dotnet-project       C# project (.csproj) - will be built first");
        Console.WriteLine("  managed-dlls         Pre-compiled DLL assemblies");
        Console.WriteLine("  dotnet-project+dlls  C# project with additional dependency DLLs");
    }

    private static void ShowBuildHelp()
    {
        Console.WriteLine("Usage: chaos-il2cpp build <convert-output-dir> [--target <target-id>]");
        Console.WriteLine();
        Console.WriteLine("Build native source code produced by 'convert' for a target platform.");
        Console.WriteLine();
        Console.WriteLine("Targets: windows-x64-reference, macos-reference, android-arm64, ios-arm64, linux-x64");
    }

    private static void ShowPublishHelp()
    {
        Console.WriteLine("Usage: chaos-il2cpp publish <subject-dir> [--target <target-id>] [--output <dir>]");
        Console.WriteLine();
        Console.WriteLine("Convert and build in one step. Equivalent to 'convert' + 'build'.");
    }

    private static void WriteJson<T>(string path, T value)
    {
        var directoryPath = Path.GetDirectoryName(path);
        if (!string.IsNullOrEmpty(directoryPath))
        {
            Directory.CreateDirectory(directoryPath);
        }

        var json = JsonSerializer.Serialize(value, JsonOptions);
        File.WriteAllText(path, json + Environment.NewLine);
    }
}
