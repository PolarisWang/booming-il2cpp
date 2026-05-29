using System.Diagnostics;
using System.Text.Json;
using System.Text.Json.Serialization;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Codegen;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Metadata;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Templating;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Emission;

public sealed class CppProjectEmitter
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
    };

    private static readonly string[] AllTemplateNames =
    [
        "TestProject.RuntimeEntry.cpp.scriban",
        "TestProject.Entry.cpp.scriban",
        "TestProject.Entry.h.scriban",
        "TestProject.Dispatch.cpp.scriban",
        "TestProject.CMakeLists.txt.scriban",
        "TestProject.CMakePresets.json.scriban",
        "TestProject.RuntimePatchdata.cpp.scriban",
        "TestProject.chaos-config.cmake.scriban",
        "TestProject.chaos-targets.cmake.scriban",
        "TestProject.metadata.json.scriban",
    ];

    /// <summary>
    /// Emit only dispatch.cpp and metadata (no codegen required).
    /// Used by the verification pipeline to generate verification_dispatch.generated.cpp
    /// after entry.exe has been built with sentinel dispatch.
    /// </summary>
    public void EmitDispatchOnly(
        string outputDir,
        IReadOnlyList<SubjectModel> subjects,
        bool isJit = false,
        string configTier = "check",
        bool isWindows = true,
        string? projectRoot = null,
        string? codegenDir = null,
        string? sdkDir = null)
    {
        Directory.CreateDirectory(outputDir);

        var model = new ProjectModelBuilder().Build(
            subjects: subjects,
            isJit: isJit,
            configTier: configTier,
            isWindows: isWindows,
            hasPatchData: false,
            patchDataSize: 0,
            patchDataHostClass: "",
            projectName: "entry",
            projectRoot: projectRoot,
            codegenDir: codegenDir,
            sdkDir: sdkDir);

        // runtime-entry.cpp
        var entryCode = TemplateCatalog.Render("TestProject.RuntimeEntry.cpp.scriban", model);
        File.WriteAllText(Path.Combine(outputDir, "runtime-entry.cpp"), entryCode);

        // verification_dispatch.generated.cpp
        var dispatchCode = TemplateCatalog.Render("TestProject.Dispatch.cpp.scriban", model);
        File.WriteAllText(Path.Combine(outputDir, "verification_dispatch.generated.cpp"), dispatchCode);

        // metadata/subjects.json
        GenerateMetadataJson(outputDir, subjects);
    }

    /// <summary>
    /// Full project emit: copies SDK + codegen outputs, generates all source files
    /// via Scriban templates, writes CMake build system and metadata.
    /// When projectRoot is set (pipeline mode), codegen file copies are skipped
    /// — the caller is responsible for placing codegen output in the correct layout.
    /// </summary>
    public void Emit(
        string outputDir,
        CodegenResult codegen,
        IReadOnlyList<SubjectModel> subjects,
        bool isJit = false,
        string configTier = "check",
        bool isWindows = true,
        string? projectRoot = null,
        string? codegenDir = null,
        string? sdkDir = null)
    {
        var isPipeline = projectRoot is not null && codegenDir is not null;

        // ── 1. Copy SDK files (everything except generated/) to chaos-sdk/ ──
        var sdkDst = Path.Combine(outputDir, "chaos-sdk");
        Directory.CreateDirectory(sdkDst);
        foreach (var entry in Directory.GetFileSystemEntries(codegen.OutputDir))
        {
            var name = Path.GetFileName(entry);
            if (name == "generated") continue;
            var dest = Path.Combine(sdkDst, name);
            if (Directory.Exists(entry))
                CopyDirectory(entry, dest);
            else
                File.Copy(entry, dest, overwrite: true);
        }

        // ── 2. Copy codegen generated files to subjects/ ──
        // Flat layout: always copy .cpp and .h files to subjects/ under the
        // output directory. The CMakeLists.txt prefers this flat layout over
        // deep codegen tree paths to avoid MAX_PATH issues with MSVC.
        {
            var subjectsDir = Path.Combine(outputDir, "subjects");
            Directory.CreateDirectory(subjectsDir);
            foreach (var genDir in codegen.GeneratedDirs)
            {
                foreach (var file in Directory.GetFiles(genDir, "*.cpp"))
                    File.Copy(file, Path.Combine(subjectsDir, Path.GetFileName(file)), overwrite: true);
                foreach (var file in Directory.GetFiles(genDir, "*.h"))
                    File.Copy(file, Path.Combine(subjectsDir, Path.GetFileName(file)), overwrite: true);
            }
        }

        // ── 3. Resolve codegen assembly names ──
        // The contract's assembly_name (from methodSubjectIds) may differ from the
        // actual codegen output directory name (e.g. "NativeEntry" vs "Subjects").
        // Scan codegen output directories for the authoritative assembly names.
        var resolvedCodegenDir = codegenDir ?? Path.Combine(outputDir, "codegen");
        var resolvedSdkDir = sdkDir ?? sdkDst;
        var resolvedProjectRoot = projectRoot ?? "";

        var codegenAssemblyNames = codegen.GeneratedDirs
            .Select(d => Path.GetFileName(Path.GetDirectoryName(d))!)
            .Where(n => !string.IsNullOrEmpty(n))
            .Distinct()
            .ToList();

        // ── 4. Build Scriban model ──
        var model = new ProjectModelBuilder().Build(
            subjects: subjects,
            isJit: isJit,
            configTier: configTier,
            isWindows: isWindows,
            hasPatchData: false,
            patchDataSize: 0,
            patchDataHostClass: "",
            projectName: isPipeline ? "chaos_entry" : "entry",
            projectRoot: resolvedProjectRoot,
            codegenDir: resolvedCodegenDir,
            sdkDir: resolvedSdkDir,
            codegenAssemblyNames: codegenAssemblyNames);

        // ── 4. Render templates → output files ──
        RenderToFile("TestProject.RuntimeEntry.cpp.scriban", model, outputDir, "runtime-entry.cpp");
        RenderToFile("TestProject.Entry.cpp.scriban", model, outputDir, "entry.cpp");
        RenderToFile("TestProject.Entry.h.scriban", model, outputDir, "entry.h");

        // Pipeline uses verification_dispatch.generated.cpp; standalone uses dispatch.cpp
        var dispatchName = isPipeline ? "verification_dispatch.generated.cpp" : "dispatch.cpp";
        RenderToFile("TestProject.Dispatch.cpp.scriban", model, outputDir, dispatchName);

        RenderToFile("TestProject.CMakeLists.txt.scriban", model, outputDir, "CMakeLists.txt");
        RenderToFile("TestProject.CMakePresets.json.scriban", model, outputDir, "CMakePresets.json");
        RenderToFile("TestProject.RuntimePatchdata.cpp.scriban", model, outputDir, "runtime-patchdata.cpp");

        // microbench.cpp stub (if not already present) — provides RunMicrobench() for --microbench mode
        var microbenchPath = Path.Combine(outputDir, "microbench.cpp");
        if (!File.Exists(microbenchPath))
        {
            var microbenchStub =
                "// microbench.cpp — Interpreter internal microbenchmarks (stub).\n"
                + "#include <cstdio>\n"
                + "extern \"C\" void RunMicrobench() {\n"
                + "    printf(\"Microbench: not implemented (stub)\\n\");\n"
                + "}\n";
            File.WriteAllText(microbenchPath, microbenchStub);
        }

        // chaos-sdk cmake files
        var cmakeDir = Path.Combine(sdkDst, "cmake");
        Directory.CreateDirectory(cmakeDir);
        RenderToFile("TestProject.chaos-config.cmake.scriban", model, cmakeDir, "chaos-config.cmake");
        RenderToFile("TestProject.chaos-targets.cmake.scriban", model, cmakeDir, "chaos-targets.cmake");

        // metadata/subjects.json
        GenerateMetadataJson(outputDir, subjects);
    }

    /// <summary>
    /// Run cmake configure + build to produce the native executable.
    /// Returns the path to entry.exe on success, or null on failure.
    /// </summary>
    public string? BuildProject(
        string projectDir,
        bool isJit = false,
        string configTier = "check")
    {
        var nativeDir = new DirectoryInfo(projectDir);
        if (!nativeDir.Exists)
        {
            Console.Error.WriteLine($"  [build] Project directory not found: {projectDir}");
            return null;
        }

        // ── Clean pre-existing chaos-sdk (TPG uses CHAOS_CODEGEN_DIR, not local chaos-sdk) ──
        var oldSdk = new DirectoryInfo(Path.Combine(projectDir, "chaos-sdk"));
        if (oldSdk.Exists)
        {
            try { oldSdk.Delete(recursive: true); }
            catch { /* best-effort */ }
        }

        // ── MAX_PATH pre-check: only check files actually compiled by TPG's CMakeLists.txt ──
        //   TPG compiles: (a) *.cpp in native dir root, (b) subjects/*.cpp (flat fallback)
        //   NOT compiled: per-assembly subdirs, chaos-sdk/, codegen/ — these are
        //   resolved via CHAOS_CODEGEN_DIR / CHAOS_PROJECT_ROOT CMake variables.
        int maxPath = 0;
        string longestFile = "";

        // Root-level .cpp files
        foreach (var f in nativeDir.GetFiles("*.cpp", SearchOption.TopDirectoryOnly))
        {
            if (f.FullName.Length > maxPath)
            {
                maxPath = f.FullName.Length;
                longestFile = f.FullName;
            }
        }
        // subjects/*.cpp flat fallback
        var subjectsDir = new DirectoryInfo(Path.Combine(projectDir, "subjects"));
        if (subjectsDir.Exists)
        {
            foreach (var f in subjectsDir.GetFiles("*.cpp", SearchOption.TopDirectoryOnly))
            {
                if (f.FullName.Length > maxPath)
                {
                    maxPath = f.FullName.Length;
                    longestFile = f.FullName;
                }
            }
        }
        // Also check any per-assembly subdirectory with generated/ — these were
        // left by old pipeline and should not block the build.
        foreach (var sub in nativeDir.GetDirectories())
        {
            var genDir = new DirectoryInfo(Path.Combine(sub.FullName, "generated"));
            if (genDir.Exists && genDir.GetFiles("*.cpp", SearchOption.TopDirectoryOnly).Length > 0)
            {
                // Old codegen artifacts — clean them so they don't confuse cmake
                try { sub.Delete(recursive: true); }
                catch { /* best-effort */ }
            }
        }
        if (maxPath >= 256)
        {
            Console.Error.WriteLine($"  [build] MAX_PATH: longest .cpp path ({maxPath} chars) exceeds 256-char limit");
            Console.Error.WriteLine($"      Longest: {longestFile}");
            Console.Error.WriteLine($"      SKIPPING build");
            return null;
        }

        // ── Separate build dirs for AOT vs JIT ──
        var buildDirName = isJit ? "build_jit" : "build";
        var buildDir = new DirectoryInfo(Path.Combine(projectDir, buildDirName));
        var cachePath = Path.Combine(buildDir.FullName, "CMakeCache.txt");
        var cmakeListsPath = Path.Combine(projectDir, "CMakeLists.txt");
        var needsConfigure = !File.Exists(cachePath);
        // Force reconfigure when CMakeLists.txt has been regenerated after cache
        if (!needsConfigure && File.Exists(cmakeListsPath))
        {
            var cacheTime = File.GetLastWriteTimeUtc(cachePath);
            var cmakeTime = File.GetLastWriteTimeUtc(cmakeListsPath);
            if (cmakeTime > cacheTime)
            {
                Console.Error.WriteLine($"  [build] CMakeLists.txt newer than cache — forcing reconfigure");
                needsConfigure = true;
            }
        }

        // ── Detect Ninja ──
        string? ninjaPath = null;
        try
        {
            using var proc = new Process();
            proc.StartInfo.FileName = "where";
            proc.StartInfo.Arguments = "ninja";
            proc.StartInfo.RedirectStandardOutput = true;
            proc.StartInfo.UseShellExecute = false;
            proc.StartInfo.CreateNoWindow = true;
            proc.Start();
            var output = proc.StandardOutput.ReadToEnd();
            proc.WaitForExit(3000);
            if (proc.ExitCode == 0)
            {
                var lines = output.Split(["\r\n", "\n"], StringSplitOptions.RemoveEmptyEntries);
                if (lines.Length > 0)
                    ninjaPath = lines[0].Trim();
            }
        }
        catch { /* fallback */ }

        // VS-bundled Ninja fallback — only use when VSINSTALLDIR is set
        if (ninjaPath is null)
        {
            var vsInstallDir = Environment.GetEnvironmentVariable("VSINSTALLDIR");
            if (vsInstallDir is not null)
            {
                var vsNinja = Path.Combine(vsInstallDir,
                    @"Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe");
                if (File.Exists(vsNinja))
                    ninjaPath = vsNinja;
            }
        }

        var cmakeGeneratorArgs = new List<string>();
        if (ninjaPath is not null)
        {
            cmakeGeneratorArgs.AddRange(["-G", "Ninja", "-DCMAKE_MAKE_PROGRAM=" + ninjaPath]);
        }
        else
        {
            Console.Error.WriteLine($"  [build] Ninja not found, falling back to Visual Studio generator");
            cmakeGeneratorArgs.AddRange(["-G", "Visual Studio 17 2022", "-A", "x64"]);
        }

        cmakeGeneratorArgs.Add($"-DCHAOS_IL2CPP_CONFIG_TIER={configTier.ToLowerInvariant()}");
        cmakeGeneratorArgs.Add(isJit
            ? "-DCHAOS_IL2CPP_JIT_MODE=ON"
            : "-DCHAOS_IL2CPP_JIT_MODE=OFF");

        // ── Step 1: CMake configure (only if cache missing) ──
        if (needsConfigure)
        {
            buildDir.Create();
            Console.Error.WriteLine($"  [build] cmake configure (fresh)...");
            bool configureOk = false;
            for (int attempt = 0; attempt < 3; attempt++)
            {
                if (attempt > 0)
                {
                    var wait = 1 << attempt;
                    Console.Error.WriteLine($"  [build] cmake configure retry #{attempt} after {wait}s...");
                    Thread.Sleep(wait * 1000);
                }

                var cfgArgs = new List<string> { "-S", projectDir, "-B", buildDir.FullName };
                cfgArgs.AddRange(cmakeGeneratorArgs);

                var cfgResult = RunProcess("cmake", cfgArgs, timeoutMs: 120_000);
                if (cfgResult.ExitCode == 0)
                {
                    configureOk = true;
                    break;
                }
                PrintLastLines(cfgResult.StdErr + cfgResult.StdOut, 10);
            }

            if (!configureOk)
            {
                Console.Error.WriteLine($"  [build] cmake configure FAILED after retries");
                return null;
            }
        }
        else
        {
            Console.Error.WriteLine($"  [build] cmake configure (cached, skip)");
        }

        // ── Step 2: CMake incremental build (3 retries) ──
        Console.Error.WriteLine($"  [build] cmake build (incremental)...");
        bool buildOk = false;
        for (int attempt = 0; attempt < 3; attempt++)
        {
            if (attempt > 0)
            {
                var wait = 1 << attempt;
                Console.Error.WriteLine($"  [build] cmake build retry #{attempt} after {wait}s...");
                Thread.Sleep(wait * 1000);
            }

            var buildResult = RunProcess("cmake", ["--build", buildDir.FullName, "--config", "RelWithDebInfo", "--target", "chaos_entry"], timeoutMs: 300_000);
            if (buildResult.ExitCode == 0)
            {
                buildOk = true;
                break;
            }
            PrintLastLines(buildResult.StdErr + buildResult.StdOut, 20);
        }

        if (!buildOk)
        {
            Console.Error.WriteLine($"  [build] cmake build FAILED after retries");
            return null;
        }

        // ── Step 3: Locate entry.exe ──
        var exeCandidates = new[]
        {
            Path.Combine(buildDir.FullName, "RelWithDebInfo", "chaos_entry.exe"),
            Path.Combine(buildDir.FullName, "Release", "chaos_entry.exe"),
            Path.Combine(buildDir.FullName, "Debug", "chaos_entry.exe"),
            Path.Combine(buildDir.FullName, "chaos_entry.exe"),
        };
        string? exePath = null;
        foreach (var c in exeCandidates)
        {
            if (File.Exists(c))
            {
                exePath = c;
                break;
            }
        }

        if (exePath is null)
        {
            Console.Error.WriteLine($"  [build] chaos_entry.exe not found in build output");
            return null;
        }

        // ── Step 4: Copy to output with retries ──
        var outputName = isJit ? "entry-jit.exe" : "entry.exe";
        var targetPath = Path.Combine(projectDir, outputName);
        for (int attempt = 0; attempt < 5; attempt++)
        {
            try
            {
                if (File.Exists(targetPath))
                    File.Delete(targetPath);
                File.Copy(exePath, targetPath, overwrite: true);
                break;
            }
            catch (Exception ex) when (attempt < 4)
            {
                var wait = 1 << attempt;
                Console.Error.WriteLine($"  [build] copy locked, retry #{attempt} after {wait}s...");
                Thread.Sleep(wait * 1000);
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine($"  [build] copy FAILED after retries: {ex.Message}");
                return null;
            }
        }

        var size = new FileInfo(targetPath).Length;
        Console.Error.WriteLine($"  [build] {outputName} OK: {size} bytes -> {targetPath}");
        return targetPath;
    }

    /// <summary>
    /// Full pipeline: emit all sources then build executable.
    /// After a successful build, generated source files are cleaned up
    /// to prevent accidental edits to auto-generated code.
    /// Returns path to entry.exe on success, or null on failure.
    /// </summary>
    public string? GenerateAndBuild(
        string outputDir,
        CodegenResult codegen,
        IReadOnlyList<SubjectModel> subjects,
        bool isJit = false,
        string configTier = "check",
        bool isWindows = true,
        string? projectRoot = null,
        string? codegenDir = null,
        string? sdkDir = null)
    {
        Emit(outputDir, codegen, subjects, isJit, configTier, isWindows, projectRoot, codegenDir, sdkDir);
        var exePath = BuildProject(outputDir, isJit, configTier);
        if (exePath is not null)
        {
            CleanupGeneratedSources(outputDir, isJit);
        }
        return exePath;
    }

    /// <summary>
    /// Remove generated C++ source files and cmake project files after successful build.
    /// Keeps the native binary (entry.exe / entry-jit.exe) and build directories.
    ///
    /// Rationale: preventing AI agents from accidentally modifying auto-generated code.
    /// The build directory is preserved for incremental hotupdate rebuilds.
    /// </summary>
    public static void CleanupGeneratedSources(string projectDir, bool isJit = false)
    {
        var keepFiles = new HashSet<string>(StringComparer.OrdinalIgnoreCase)
        {
            "entry-jit.exe", // JIT binary — must survive AOT cleanup for subsequent pipeline stages
            "entry.exe",     // AOT binary — must survive JIT cleanup for subsequent pipeline stages
            "entry-aot.exe", // AOT backup — must survive JIT cleanup for restore
            "CMakeLists.txt",       // needed by hotupdate rebuild
            "CMakePresets.json",    // needed by hotupdate rebuild
        };
        var keepDirs = new HashSet<string>(StringComparer.OrdinalIgnoreCase)
        {
            "build",
            "build_jit",
        };

        // Delete generated source files in the project root
        foreach (var f in new DirectoryInfo(projectDir).EnumerateFileSystemInfos())
        {
            if (keepFiles.Contains(f.Name))
                continue;
            if (f is DirectoryInfo di && keepDirs.Contains(di.Name))
                continue;

            try
            {
                if (f is DirectoryInfo d)
                    d.Delete(recursive: true);
                else
                    f.Delete();
            }
            catch
            {
                // best-effort cleanup — files locked by cmake process may not delete
            }
        }
    }

    private static void RenderToFile(string templateName, Scriban.Runtime.ScriptObject model, string outputDir, string outputFileName)
    {
        var rendered = TemplateCatalog.Render(templateName, model);
        var path = Path.Combine(outputDir, outputFileName);
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        File.WriteAllText(path, rendered);
    }

    private static void GenerateMetadataJson(string outputDir, IReadOnlyList<SubjectModel> subjects)
    {
        var metadataDir = Path.Combine(outputDir, "metadata");
        Directory.CreateDirectory(metadataDir);

        var entries = subjects.Select((s, i) => new
        {
            index = i,
            kind = s.Kind.ToString().ToLowerInvariant(),
            subjectId = s.SubjectId,
            assemblyName = s.AssemblyName,
            typeName = s.FullTypeName,
            methodName = s.MethodName,
            isStatic = s.IsStatic,
        }).ToList();

        var obj = new
        {
            schemaVersion = 1,
            generatedAt = DateTime.UtcNow.ToString("O"),
            generator = "Chaos.IL2CPP.Tools.TestProjectGenerator",
            entryExecutable = "entry",
            subjects = entries,
            summary = new
            {
                totalSubjects = subjects.Count,
                factCount = subjects.Count(s => s.Kind == SubjectKind.Fact),
                benchmarkCount = subjects.Count(s => s.Kind == SubjectKind.Benchmark),
                hotUpdateCount = subjects.Count(s => s.Kind == SubjectKind.HotUpdate),
            },
        };

        var json = JsonSerializer.Serialize(obj, JsonOptions);
        File.WriteAllText(Path.Combine(metadataDir, "subjects.json"), json);
    }

    private static void CopyDirectory(string sourceDir, string destDir)
    {
        Directory.CreateDirectory(destDir);
        foreach (var file in Directory.GetFiles(sourceDir, "*", SearchOption.AllDirectories))
        {
            var relative = Path.GetRelativePath(sourceDir, file);
            var dest = Path.Combine(destDir, relative);
            Directory.CreateDirectory(Path.GetDirectoryName(dest)!);
            File.Copy(file, dest, overwrite: true);
        }
    }

    private static (int ExitCode, string StdOut, string StdErr) RunProcess(string fileName, List<string> args, int timeoutMs)
    {
        using var proc = new Process();
        proc.StartInfo.FileName = fileName;
        proc.StartInfo.Arguments = string.Join(" ", args.Select(a => a.Contains(' ') ? $"\"{a}\"" : a));
        proc.StartInfo.RedirectStandardOutput = true;
        proc.StartInfo.RedirectStandardError = true;
        proc.StartInfo.UseShellExecute = false;
        proc.StartInfo.CreateNoWindow = true;

        var outBuilder = new System.Text.StringBuilder();
        var errBuilder = new System.Text.StringBuilder();
        proc.OutputDataReceived += (_, e) => { if (e.Data is not null) outBuilder.AppendLine(e.Data); };
        proc.ErrorDataReceived += (_, e) => { if (e.Data is not null) errBuilder.AppendLine(e.Data); };

        proc.Start();
        proc.BeginOutputReadLine();
        proc.BeginErrorReadLine();

        if (proc.WaitForExit(timeoutMs))
            return (proc.ExitCode, outBuilder.ToString(), errBuilder.ToString());

        try { proc.Kill(entireProcessTree: true); } catch { }
        return (-1, outBuilder.ToString(), errBuilder + "\n[TIMEOUT]");
    }

    private static void PrintLastLines(string output, int count)
    {
        var lines = output.Split(["\r\n", "\n"], StringSplitOptions.RemoveEmptyEntries);
        foreach (var line in lines.TakeLast(count))
            Console.Error.WriteLine($"      {line}");
    }
}
