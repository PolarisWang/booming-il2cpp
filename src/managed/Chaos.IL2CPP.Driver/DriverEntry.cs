using System.Text.Json;
using System.Text.Json.Serialization;
using Chaos.IL2CPP.Generator;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Diagnostics;
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

    private readonly PipelineOrchestrator _orchestrator = new();

    public int Run(ManagedClosureRequest request) => _orchestrator.RunPipeline(request);

    public int Run(NativeReferenceProofRequest request) => _orchestrator.RunNativeReference(request);

    public int Run(NativeAotRequest request) => _orchestrator.RunNativeAot(request);

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
            "emit-patch-data" => RunEmitPatchData(args[1..]),
            "convert-to-cpp" => ConvertToCppHandler.Run(args[1..]),
            "asm-compare" => AsmCompareHandler.Run(args[1..]),
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

                    string resolvedEntryAssembly = entryAssembly!;
                    if (!Path.IsPathRooted(resolvedEntryAssembly))
                        resolvedEntryAssembly = Path.GetFullPath(resolvedEntryAssembly, Directory.GetCurrentDirectory());

                    inputAssemblyPath = resolvedEntryAssembly;
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
        string? projectDir = null;
        string? target = null;
        var configTier = "check";

        for (var i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--target" or "-t" when i + 1 < args.Length:
                    target = args[++i];
                    break;
                case "--config-tier" when i + 1 < args.Length:
                    configTier = args[++i];
                    break;
                case "--help" or "-h":
                    ShowBuildHelp();
                    return 0;
                default:
                    if (!args[i].StartsWith('-') && projectDir is null)
                        projectDir = args[i];
                    else
                    {
                        Console.Error.WriteLine($"Unknown argument: {args[i]}");
                        return 1;
                    }
                    break;
            }
        }

        if (projectDir is null)
        {
            Console.Error.WriteLine("Error: native project directory is required.");
            Console.Error.WriteLine("Usage: chaos-il2cpp build <native-project-dir> [--target <cmake-target>] [--config-tier <tier>]");
            return 1;
        }

        target ??= BuildService.ChaosEntryTarget;

        Console.WriteLine($"[1/2] Configuring native build (target: {target}, config-tier: {configTier})");

        var result = BuildService.ConfigureAndBuild(projectDir, target, configTier);

        if (!string.IsNullOrWhiteSpace(result.Output))
            Console.Write(result.Output);
        if (!string.IsNullOrWhiteSpace(result.Error))
            Console.Error.Write(result.Error);

        if (result.Success)
        {
            Console.WriteLine($"[2/2] Build completed (target: {target})");
            return 0;
        }

        Console.Error.WriteLine($"Build failed (target: {target}, exit code: {result.ExitCode})");
        return 1;
    }

    private static int RunPublish(string[] args)
    {
        string? subjectDir = null;
        string? outputDir = null;
        string? target = null;
        string? dllPath = null;
        string? metadataPath = null;
        var configTier = "check";
        var sourceOnly = false;
        var clean = false;
        var isJit = false;
        var mode = "app";
        var assemblyDirs = new List<string>();

        for (var i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--output" or "-o" when i + 1 < args.Length:
                    outputDir = args[++i];
                    break;
                case "--target" or "-t" when i + 1 < args.Length:
                    target = args[++i];
                    break;
                case "--mode" when i + 1 < args.Length:
                    mode = args[++i].ToLowerInvariant();
                    break;
                case "--dll" when i + 1 < args.Length:
                    dllPath = Path.GetFullPath(args[++i]);
                    break;
                case "--metadata" when i + 1 < args.Length:
                    metadataPath = Path.GetFullPath(args[++i]);
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
                case "--jit":
                    isJit = true;
                    break;
                case "--assembly-dir" when i + 1 < args.Length:
                    assemblyDirs.Add(Path.GetFullPath(args[++i]));
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

        // ── App mode: publish any .csproj/.dll/.exe to a native executable ──
        // Detected when --mode app (default) OR the positional input is a
        // project/dll/exe file (as opposed to a subject directory).
        var isAppInput = subjectDir is not null &&
            (subjectDir.EndsWith(".csproj", StringComparison.OrdinalIgnoreCase) ||
             subjectDir.EndsWith(".dll", StringComparison.OrdinalIgnoreCase) ||
             subjectDir.EndsWith(".exe", StringComparison.OrdinalIgnoreCase));

        if (mode == "app" && (dllPath is null || isAppInput))
        {
            if (subjectDir is null)
            {
                Console.Error.WriteLine("Error: input path is required for app mode.");
                Console.Error.WriteLine("Usage: chaos-il2cpp publish <project.csproj|app.dll|app.exe> [--mode app] [--output <dir>]");
                return 1;
            }

            return PublishController.Run(new PublishController.PublishConfig
            {
                InputPath = subjectDir,
                OutputDir = outputDir ?? Path.Combine(subjectDir, "output"),
                Mode = "app",
                ConfigTier = configTier,
                SourceOnly = sourceOnly,
                Clean = clean,
                IsJit = isJit,
                AssemblyDirs = assemblyDirs,
            });
        }

        // ── Test mode: TPG generate-dll (subject deliverable) ─────────────
        // If --dll and --metadata are provided directly, use them.
        // Otherwise, if subjectDir is given, try to resolve from the manifest.
        if (dllPath is null || metadataPath is null)
        {
            if (subjectDir is null)
            {
                Console.Error.WriteLine("Error: subject directory is required (or use --dll + --metadata).");
                Console.Error.WriteLine("Usage: chaos-il2cpp publish <subject-dir> --output <dir> [--dll <subjects.dll> --metadata <subjects.metadata.json>]");
                return 1;
            }

            outputDir ??= Path.GetFullPath(Path.Combine(subjectDir, "output"));

            // Try to resolve dll and metadata from the manifest or well-known locations
            var manifestPath = Path.Combine(subjectDir, "subject.manifest.json");
            if (File.Exists(manifestPath))
            {
                Console.WriteLine($"[publish] Reading manifest: {manifestPath}");
                try
                {
                    var manifest = JsonSerializer.Deserialize<JsonElement>(File.ReadAllText(manifestPath));
                    var source = manifest.GetProperty("source");

                    // If the manifest has a subjectsDll or subjectsMetadata field, use those
                    if (manifest.TryGetProperty("subjectsDll", out var sd))
                        dllPath ??= Path.GetFullPath(sd.GetString()!, subjectDir);
                    if (manifest.TryGetProperty("subjectsMetadata", out var sm))
                        metadataPath ??= Path.GetFullPath(sm.GetString()!, subjectDir);

                    // If not, try convention: output/combined/CombinedSubjects.dll
                    if (dllPath is null || !File.Exists(dllPath))
                    {
                        var combinedDll = Path.Combine(outputDir, "combined", "CombinedSubjects.dll");
                        if (File.Exists(combinedDll))
                            dllPath = combinedDll;
                    }
                    if (metadataPath is null || !File.Exists(metadataPath))
                    {
                        var meta = Path.Combine(outputDir, "combined", "subjects.metadata.json");
                        if (File.Exists(meta))
                            metadataPath = meta;
                    }
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine($"Warning: could not parse manifest: {ex.Message}");
                }
            }
            else
            {
                Console.Error.WriteLine($"Warning: no subject.manifest.json at {manifestPath}");
            }
        }

        if (dllPath is null || !File.Exists(dllPath))
        {
            Console.Error.WriteLine("Error: subjects DLL not found. Provide --dll <subjects.dll> or place it in the output/combined/ directory.");
            Console.Error.WriteLine("  (subjects.dll is produced by `dotnet build CombinedSubjects.csproj` after ATG emits CombinedSubjects.cs)");
            return 1;
        }

        if (metadataPath is null || !File.Exists(metadataPath))
        {
            Console.Error.WriteLine("Error: subjects metadata not found. Provide --metadata <subjects.metadata.json>.");
            return 1;
        }

        outputDir ??= Path.GetFullPath(Path.Combine(subjectDir ?? Directory.GetCurrentDirectory(), "output"));

        // ── Locate TPG DLL ──
        string? tpgDll = null;
        var tpgAssemblyName = "Chaos.IL2CPP.Tools.TestProjectGenerator";

        // Strategy 1: from the current driver's assembly location (co-located in build output)
        var driverDir = Path.GetDirectoryName(typeof(DriverEntry).Assembly.Location);
        if (driverDir is not null)
        {
            var candidate = Path.Combine(driverDir, $"{tpgAssemblyName}.dll");
            if (File.Exists(candidate))
                tpgDll = candidate;
        }

        // Strategy 2: search for TPG in the repo build output
        if (tpgDll is null)
        {
            var repoRoot = DetectRepoRoot(subjectDir ?? driverDir);
            if (repoRoot is not null)
            {
                var candidates = new[]
                {
                    Path.Combine(repoRoot, "src", "tools", "Chaos.IL2CPP.Tools.TestProjectGenerator", "bin", "Debug", "net8.0", $"{tpgAssemblyName}.dll"),
                    Path.Combine(repoRoot, "src", "tools", "Chaos.IL2CPP.Tools.TestProjectGenerator", "bin", "Release", "net8.0", $"{tpgAssemblyName}.dll"),
                };
                foreach (var c in candidates)
                {
                    if (File.Exists(c))
                    {
                        tpgDll = c;
                        break;
                    }
                }
            }
        }

        if (tpgDll is null)
        {
            Console.Error.WriteLine($"Error: {tpgAssemblyName}.dll not found. Build the TPG project first:");
            Console.Error.WriteLine($"  dotnet build src/tools/{tpgAssemblyName}/{tpgAssemblyName}.csproj");
            return 1;
        }

        // ── Build TPG generate-dll command ──
        var tpgArgs = new List<string>
        {
            "exec", tpgDll,
            "generate-dll",
            "--dll", dllPath,
            "--metadata", metadataPath,
            "--output", outputDir,
            "--config-tier", configTier,
        };

        if (sourceOnly)
            tpgArgs.Add("--source-only");
        if (clean)
            tpgArgs.Add("--clean");
        if (isJit)
            tpgArgs.Add("--jit");
        foreach (var ad in assemblyDirs)
        {
            tpgArgs.Add("--assembly-dir");
            tpgArgs.Add(ad);
        }

        Console.WriteLine($"[publish] Running TPG generate-dll (this will run codegen, emit C++ project, and build entry.exe)...");
        Console.WriteLine($"  TPG DLL: {tpgDll}");
        Console.WriteLine($"  DLL: {dllPath}");
        Console.WriteLine($"  Metadata: {metadataPath}");
        Console.WriteLine($"  Output: {outputDir}");

        var psi = new System.Diagnostics.ProcessStartInfo("dotnet")
        {
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
        };
        foreach (var arg in tpgArgs)
            psi.ArgumentList.Add(arg);

        try
        {
            using var process = System.Diagnostics.Process.Start(psi);
            if (process is null)
            {
                Console.Error.WriteLine("Error: failed to start TPG process.");
                return 1;
            }

            // Read stdout/stderr concurrently to avoid pipe deadlock. A synchronous
            // ReadToEnd on one stream can block forever if a grandchild process (e.g.
            // the msbuild/cl toolchain spawned by TPG) inherits the pipe handle and
            // holds it open, or if the peer stream fills its buffer while unread.
            var stdoutTask = process.StandardOutput.ReadToEndAsync();
            var stderrTask = process.StandardError.ReadToEndAsync();

            if (!process.WaitForExit(1_800_000))
            {
                try { process.Kill(entireProcessTree: true); } catch { }
                Console.Error.WriteLine("Error: TPG generate-dll timed out after 30 minutes.");
                return 1;
            }

            var stdout = stdoutTask.GetAwaiter().GetResult();
            var stderr = stderrTask.GetAwaiter().GetResult();

            // Forward TPG output to console
            foreach (var line in stdout.Split(Environment.NewLine, StringSplitOptions.RemoveEmptyEntries))
                Console.WriteLine($"  {line}");
            if (!string.IsNullOrWhiteSpace(stderr))
            {
                foreach (var line in stderr.Split(Environment.NewLine, StringSplitOptions.RemoveEmptyEntries))
                    Console.Error.WriteLine($"  [TPG:err] {line}");
            }

            if (process.ExitCode != 0)
            {
                Console.Error.WriteLine($"TPG generate-dll failed (exit code: {process.ExitCode})");
                return process.ExitCode;
            }

            // ── Fix LNK1107: TPG's codegen emits stub archives (empty `!<arch>\n`)
            // for libs it couldn't find (incl. chaos_pal). Overwrite them with the
            // real prebuilt libs from the SDK staging root, then rebuild.
            CopyRealSdkLibsOverStubs(outputDir, subjectDir ?? driverDir);

            // Rebuild now that real libs are in place (TPG's build with stubs already failed).
            var buildDir = Path.Combine(outputDir, "build");
            if (Directory.Exists(buildDir) && File.Exists(Path.Combine(outputDir, "CMakeLists.txt")))
            {
                Console.WriteLine("  [publish] Rebuilding with real SDK libs...");
                // Let BuildService pick the platform-default generator (VS 2022/x64
                // on Windows, Unix Makefiles on Linux/macOS); do not hardcode one.
                var rebuildResult = BuildService.ConfigureAndBuild(outputDir, "chaos_entry", "check");
                if (!rebuildResult.Success)
                {
                    Console.Error.WriteLine($"  [publish] Rebuild failed: {rebuildResult.Error}");
                    Console.Error.WriteLine($"  [publish] Output: {rebuildResult.Output}");
                    // Continue anyway — FindEntryExe will check if the exe was produced.
                }
                else
                {
                    Console.WriteLine("  [publish] Rebuild succeeded.");
                }
            }

            // Locate entry.exe
            var entryExe = FindEntryExe(outputDir);
            if (entryExe is not null)
            {
                Console.WriteLine($"  entry.exe produced at: {entryExe}");

                // Write publish.manifest.json
                var publishManifest = new
                {
                    subjectDir = subjectDir ?? "(not specified)",
                    outputDir,
                    dllPath,
                    metadataPath,
                    entryExe,
                    configTier,
                    status = "ok",
                };
                WriteJson(Path.Combine(outputDir, "publish.manifest.json"), publishManifest);
            }
            else
            {
                Console.WriteLine("  (entry.exe not found -- source-only mode or build step skipped)");
            }

            Console.WriteLine("Publish completed.");
            return 0;
        }
        catch (System.ComponentModel.Win32Exception)
        {
            Console.Error.WriteLine("Error: dotnet SDK not found. Install from https://dot.net/download");
            return 1;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"Error: {ex.Message}");
            return 1;
        }
    }

    /// <summary>
    /// Find entry.exe in the publish output directory following the same search order
    /// as CppProjectEmitter.GenerateAndBuild.
    /// </summary>
    private static string? FindEntryExe(string outputDir)
    {
        var buildDir = Path.Combine(outputDir, "build");
        if (!Directory.Exists(buildDir))
        {
            // Try direct output dir
            buildDir = outputDir;
        }

        var candidates = new[]
        {
            Path.Combine(buildDir, "RelWithDebInfo", "chaos_entry.exe"),
            Path.Combine(buildDir, "Release", "chaos_entry.exe"),
            Path.Combine(buildDir, "Debug", "chaos_entry.exe"),
            Path.Combine(buildDir, "chaos_entry.exe"),
            Path.Combine(buildDir, "RelWithDebInfo", "entry.exe"),
            Path.Combine(buildDir, "Release", "entry.exe"),
            Path.Combine(buildDir, "Debug", "entry.exe"),
            Path.Combine(buildDir, "entry.exe"),
            // Linux
            Path.Combine(buildDir, "RelWithDebInfo", "chaos_entry"),
            Path.Combine(buildDir, "Release", "chaos_entry"),
            Path.Combine(buildDir, "Debug", "chaos_entry"),
            Path.Combine(buildDir, "chaos_entry"),
            Path.Combine(buildDir, "RelWithDebInfo", "entry"),
            Path.Combine(buildDir, "Release", "entry"),
            Path.Combine(buildDir, "Debug", "entry"),
            Path.Combine(buildDir, "entry"),
        };

        foreach (var c in candidates)
        {
            if (File.Exists(c))
                return Path.GetFullPath(c);
        }

        return null;
    }

    /// <summary>
    /// TPG's generate-dll produces stub archives (empty `!&lt;arch&gt;\n`, 8 bytes) for
    /// native libs it cannot resolve — notably chaos_pal.lib — which then fail to link
    /// with LNK1107. The real prebuilt libs live in the SDK staging root
    /// (tests/e2e/translation/sdk/&lt;preset&gt;/lib/). Copy them over the stubs under
    /// &lt;outputDir&gt;/codegen/lib/ so the CMake build can link real symbols.
    /// </summary>
    private static void CopyRealSdkLibsOverStubs(string outputDir, string detectRoot)
    {
        try
        {
            var repoRoot = DetectRepoRoot(detectRoot);
            if (repoRoot is null)
            {
                Console.WriteLine("  [publish] warning: could not resolve the chaos-il2cpp repo root from "
                                  + $"'{detectRoot}'. Skipping real-SDK-lib copy.");
                Console.WriteLine("  [publish]          Run 'chaos-il2cpp publish' from inside the repo (or pass a "
                                  + "<subject-dir> that is under it) so the prebuilt SDK libs "
                                  + "under artifacts/presets/* and tests/e2e/translation/sdk/* can be located.");
                return;
            }

            // Real prebuilt libs live under artifacts/presets/<preset>/src/native/*/<config>/
            // (produced by the build system / build_presets CI). This mirrors how
            // ConvertToCppHandler sources nativeLibDir. libs not found there fall
            // back to the SDK staging root (tests/e2e/translation/sdk/<preset>/lib).
            var isWindows = System.Runtime.InteropServices.RuntimeInformation
                .IsOSPlatform(System.Runtime.InteropServices.OSPlatform.Windows);
            var nativePresetDir = isWindows ? "windows-x64-reference" : "linux-x64-profile";
            var presetLibRoot = Path.Combine(repoRoot, "artifacts", "presets", nativePresetDir);
            var sdkLibRoot = Path.Combine(repoRoot, "tests", "e2e", "translation", "sdk", nativePresetDir, "lib");
            var stubLibRoot = Path.Combine(outputDir, "codegen", "lib");

            // Collect a flat map: targetLibName -> realLibPath (prefer artifacts/presets,
            // fall back to sdk staging). TPG's codegen/lib uses flat <name>.lib names.
            var realLibs = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            // 1. SDK staging flat lib/ (authoritative, already flattened).
            if (Directory.Exists(sdkLibRoot))
            {
                foreach (var lib in Directory.GetFiles(sdkLibRoot, "*.lib"))
                    realLibs[Path.GetFileName(lib)] = lib;
                // Linux static libraries use .a extension.
                foreach (var lib in Directory.GetFiles(sdkLibRoot, "*.a"))
                    realLibs[Path.GetFileName(lib)] = lib;
            }
            // 2. artifacts/presets deep tree (src/native/<mod>/RelWithDebInfo/<name>.lib).
            if (Directory.Exists(presetLibRoot))
            {
                foreach (var lib in Directory.GetFiles(presetLibRoot, "*.lib", SearchOption.AllDirectories))
                {
                    var name = Path.GetFileName(lib);
                    // Prefer the deepest (RelWithDebInfo / Release) build config.
                    if (!realLibs.TryGetValue(name, out var existing) ||
                        Path.GetDirectoryName(lib)!.Length > Path.GetDirectoryName(existing)!.Length)
                    {
                        realLibs[name] = lib;
                    }
                }
                // Also search .a files in the presets tree for Linux builds.
                foreach (var lib in Directory.GetFiles(presetLibRoot, "*.a", SearchOption.AllDirectories))
                {
                    var name = Path.GetFileName(lib);
                    if (!realLibs.TryGetValue(name, out var existing) ||
                        Path.GetDirectoryName(lib)!.Length > Path.GetDirectoryName(existing)!.Length)
                    {
                        realLibs[name] = lib;
                    }
                }
            }

            if (realLibs.Count == 0)
            {
                Console.WriteLine($"  [publish] warning: no real SDK libs found (preset={nativePresetDir}, sdk={sdkLibRoot}); real-lib copy skipped");
                return;
            }
            if (!Directory.Exists(stubLibRoot))
            {
                Console.WriteLine($"  [publish] warning: codegen lib dir not found ({stubLibRoot}); real-lib copy skipped");
                return;
            }

            Directory.CreateDirectory(stubLibRoot);
            var copied = 0;
            foreach (var kv in realLibs)
            {
                var dst = Path.Combine(stubLibRoot, kv.Key);
                File.Copy(kv.Value, dst, overwrite: true);
                copied++;
            }
            Console.WriteLine($"  [publish] Overwrote {copied} stub lib(s) with real SDK libs in {stubLibRoot}" + $"\n  [publish]   (source: {Path.GetDirectoryName(realLibs.Values.First())})");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"  [publish] warning: real-lib copy failed: {ex.Message}");
        }
    }

    /// <summary>
    /// Walk up from a directory to find the repo root (looks for src/managed/ or src/native/).
    /// </summary>
    private static string? DetectRepoRoot(string? startDir)
    {
        if (startDir is null) return null;
        var dir = new DirectoryInfo(startDir);
        while (dir is not null)
        {
            if (Directory.Exists(Path.Combine(dir.FullName, "src", "managed")) &&
                Directory.Exists(Path.Combine(dir.FullName, "src", "native")))
            {
                return dir.FullName;
            }
            dir = dir.Parent;
        }
        return null;
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

    private static int RunEmitPatchData(string[] args)
    {
        // Usage:
        //   chaos-il2cpp emit-patch-data <patch-dll> <output-patchdata> [--aot-core-ir <path>]
        //   chaos-il2cpp emit-patch-data dump <patchdata-path>
        //   chaos-il2cpp emit-patch-data full <patch-dll> <output-patchdata>
        if (args.Length == 0)
        {
            Console.Error.WriteLine("Usage:");
            Console.Error.WriteLine("  chaos-il2cpp emit-patch-data <patch-dll-path> <output-patchdata-path> [--aot-core-ir <path>] [--mode aot|test]");
            Console.Error.WriteLine("  chaos-il2cpp emit-patch-data dump <patchdata-path>");
            Console.Error.WriteLine("  chaos-il2cpp emit-patch-data full <patch-dll-path> <output-patchdata-path>");
            return 1;
        }

        // Subcommand: dump
        if (args[0] == "dump")
        {
            if (args.Length < 2)
            {
                Console.Error.WriteLine("Usage: chaos-il2cpp emit-patch-data dump <patchdata-path>");
                return 1;
            }
            return RunDumpPatchData(args[1]);
        }

        // Subcommand: full (closure pipeline + patch data extraction in one step)
        if (args[0] == "full")
        {
            if (args.Length < 2)
            {
                Console.Error.WriteLine("Usage: chaos-il2cpp emit-patch-data full <patch-dll-path> <output-patchdata-path>");
                return 1;
            }
            return RunEmitPatchDataFull(args[1], args.Length > 2 ? args[2] : null);
        }

        // Help for emit-patch-data
        if (args[0] is "--help" or "-h" or "/?")
        {
            Console.Error.WriteLine("Usage:");
            Console.Error.WriteLine("  chaos-il2cpp emit-patch-data <patch-dll-path> <output-patchdata-path> [--aot-core-ir <path>] [--mode aot|test]");
            Console.Error.WriteLine("  chaos-il2cpp emit-patch-data dump <patchdata-path>");
            Console.Error.WriteLine("  chaos-il2cpp emit-patch-data full <patch-dll-path> <output-patchdata-path>");
            return 0;
        }

        // Default: extract patch data from a DLL
        if (args.Length < 2)
        {
            Console.Error.WriteLine("Usage: chaos-il2cpp emit-patch-data <patch-dll-path> <output-patchdata-path> [--aot-core-ir <path>]");
            return 1;
        }
        var dllPath = args[0];
        var outputPath = args[1];
        string? aotCoreIrPath = null;
        string? direction = null;
        var subjectOnly = false;
        string? subjectIndices = null;
        var emitPatchDataMode = CodegenMode.Aot;

        for (var i = 2; i < args.Length; i++)
        {
            if (args[i] == "--aot-core-ir" && i + 1 < args.Length)
                aotCoreIrPath = args[++i];
            else if (args[i] == "--direction" && i + 1 < args.Length)
                direction = args[++i];
            else if (args[i] == "--subject-only")
                subjectOnly = true;
            else if (args[i] == "--subject-indices" && i + 1 < args.Length)
                subjectIndices = args[++i];
            else if (args[i] == "--mode" && i + 1 < args.Length)
            {
                var modeValue = args[++i].ToLowerInvariant();
                emitPatchDataMode = modeValue switch
                {
                    "test" => CodegenMode.Aot | CodegenMode.TestMode,
                    _ => CodegenMode.Aot,
                };
            }
        }

        if (!File.Exists(dllPath))
        {
            Console.Error.WriteLine($"Error: patch DLL not found: {dllPath}");
            return 1;
        }

        ChaosTrace.Point("driver.emit_patch_data", "codegen");

        try
        {
            Directory.CreateDirectory(Path.GetDirectoryName(outputPath)!);
            new PatchDataExtractor().Extract(dllPath, outputPath, aotCoreIrPath,
                direction: direction, subjectOnly: subjectOnly, subjectIndices: subjectIndices,
                mode: emitPatchDataMode);
            var fileSize = new FileInfo(outputPath).Length;
            Console.WriteLine($"Patch data written: {outputPath} ({fileSize} bytes)");
            return 0;
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine($"Error: {exception.Message}");
            return 1;
        }
    }

    private static int RunDumpPatchData(string patchdataPath)
    {
        if (!File.Exists(patchdataPath))
        {
            Console.Error.WriteLine($"Error: patchdata file not found: {patchdataPath}");
            return 1;
        }

        try
        {
            PatchDataDumper.Dump(patchdataPath, Console.Out);
            return 0;
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine($"Error dumping patch data: {exception.Message}");
            return 1;
        }
    }

    private static int RunEmitPatchDataFull(string dllPath, string? outputPath)
    {
        if (!File.Exists(dllPath))
        {
            Console.Error.WriteLine($"Error: patch DLL not found: {dllPath}");
            return 1;
        }

        outputPath ??= Path.ChangeExtension(dllPath, ".patchdata");

        var tempDir = Path.Combine(Path.GetTempPath(), "chaos-patchdata-" + Guid.NewGuid().ToString("N"));
        var irDir = Path.Combine(tempDir, "analysis");
        Directory.CreateDirectory(irDir);

        Console.WriteLine($"[1/2] Running IL2CPP closure pipeline on: {Path.GetFileName(dllPath)}");

        try
        {
            var request = new ManagedClosureRequest(
                InputAssemblyPath: dllPath,
                OutputRootPath: irDir,
                AdditionalAssemblyPaths: null,
                FullAssemblyClosure: true);

            var orchestrator = new PipelineOrchestrator();
            var result = orchestrator.RunPipelineAndGetResult(request);
            if (result is null)
                return 1;

            // AotCoreIr was already written to irDir by PipelineOrchestrator
            var irPath = Path.Combine(irDir, ManagedClosureArtifactNames.AotCoreIr);

            Console.WriteLine($"[2/2] Generating patch data: {outputPath}");
            ChaosTrace.Point("driver.emit_patch_data_full", "codegen");

            Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(outputPath))!);
            new PatchDataExtractor().Extract(dllPath, outputPath, aotCoreIrPath: irPath);

            var fileSize = new FileInfo(outputPath).Length;
            Console.WriteLine($"Patch data written: {outputPath} ({fileSize} bytes)");
            return 0;
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine($"Error: {exception.Message}");
            return 1;
        }
        finally
        {
            if (Directory.Exists(tempDir))
                Directory.Delete(tempDir, recursive: true);
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
        Console.WriteLine("  convert          Analyze C# project/DLLs (analysis/generated artifacts, no executable)");
        Console.WriteLine("  convert-to-cpp   Full assembly IL->C++ translation (Unity IL2CPP style)");
        Console.WriteLine("  asm-compare      Compare JIT vs IL2CPP AOT translation for a method");
        Console.WriteLine("  build            Build a native project produced by TPG generate-dll (or publish --source-only)");
        Console.WriteLine("  publish          Full pipeline: subject.manifest -> TPG generate-dll -> entry.exe");
        Console.WriteLine();
        Console.WriteLine("Legacy commands:");
        Console.WriteLine("  <input.dll> <output-root>                    Managed closure generation");
        Console.WriteLine("      [--entry-point-subject-id <subject-id>] [--additional-assembly <path> ...]");
        Console.WriteLine("      [--full-assembly-closure]");
        Console.WriteLine("  emit-native-reference <closure-root> <out>   Native reference emission");
        Console.WriteLine("  emit-native-aot <closure-root> <out>         Generic native AOT emission");
        Console.WriteLine("  emit-patch-data <patch-dll> <out> [--aot-core-ir <path>]   Extract patch metadata (.patchdata)");
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
        Console.WriteLine("Analyze a C# project or managed DLLs, producing analysis/generated artifacts.");
        Console.WriteLine("NOTE: This does NOT produce an executable. Use 'publish' for the full manifest->exe pipeline.");
        Console.WriteLine("Reads subject.manifest.json from <subject-dir> to determine input type.");
        Console.WriteLine();
        Console.WriteLine("Supported source types:");
        Console.WriteLine("  dotnet-project       C# project (.csproj) - will be built first");
        Console.WriteLine("  managed-dlls         Pre-compiled DLL assemblies");
        Console.WriteLine("  dotnet-project+dlls  C# project with additional dependency DLLs");
    }

    private static void ShowBuildHelp()
    {
        Console.WriteLine("Usage: chaos-il2cpp build <native-project-dir> [--target <cmake-target>] [--config-tier <tier>]");
        Console.WriteLine();
        Console.WriteLine("Build a native project produced by TPG generate-dll (or publish --source-only).");
        Console.WriteLine("The project directory must contain CMakeLists.txt.");
        Console.WriteLine();
        Console.WriteLine("Options:");
        Console.WriteLine("  --target <target>     CMake target name (default: chaos_entry; standalone: entry)");
        Console.WriteLine("  --config-tier <tier>  Build configuration: check, profile, or ship (default: check)");
        Console.WriteLine();
        Console.WriteLine("CMake targets: chaos_entry (pipeline), entry (standalone)");
        Console.WriteLine();
        Console.WriteLine("Note: This is NOT a 'convert' output directory. Use 'publish' for the full manifest->exe pipeline.");
    }

    private static void ShowPublishHelp()
    {
        Console.WriteLine("Usage: chaos-il2cpp publish <path> [--output <dir>] [--mode app|test]");
        Console.WriteLine("       [--config-tier check|profile|ship] [--clean] [--source-only] [--jit] [--assembly-dir <dir>]");
        Console.WriteLine();
        Console.WriteLine("Publish any .NET project/csproj/dll/exe to a native executable.");
        Console.WriteLine();
        Console.WriteLine("  <path>            .csproj, .dll, or .exe (app mode) | subject dir (test mode)");
        Console.WriteLine("  --mode <mode>     app (pure application entry) | test (test harness) [default: app]");
        Console.WriteLine();
        Console.WriteLine("Options:");
        Console.WriteLine("  --output <dir>             Output directory (default: <input>/output)");
        Console.WriteLine("  --config-tier <tier>       Build config: check, profile, or ship (default: check)");
        Console.WriteLine("  --source-only              Emit C++ source files only, skip native build");
        Console.WriteLine("  --jit                      Enable JIT mode (default: AOT)");
        Console.WriteLine("  --assembly-dir <dir>       Additional assembly directory (may repeat)");
        Console.WriteLine();
        Console.WriteLine("App mode: translates the project's IL to C++, emits app_main.cpp + CMake,");
        Console.WriteLine("  and builds an independent native executable that runs the user's Main.");
        Console.WriteLine("Test mode: uses TPG generate-dll for a test harness (fact/benchmark).");
        Console.WriteLine("  --dll <path> / --metadata <path>  provide subjects for test mode.");
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
