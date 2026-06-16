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
        "TestProject.Dispatch.cpp.scriban",
        "TestProject.SubjectDispatch.cpp.scriban",
        "TestProject.SubjectDispatch.h.scriban",
        "TestProject.SubjectDispatch.cpp.scriban",
        "TestProject.SubjectDispatch.h.scriban",
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
        string? sdkDir = null,
        bool verificationEnabled = true)
    {
        Directory.CreateDirectory(outputDir);

        var (hotupdateActive, hostNamespace, hostTypeNames, hostMethodNames) = BuildHostArrays(subjects);

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
            sdkDir: sdkDir,
            verificationEnabled: verificationEnabled,
            hotupdateActive: hotupdateActive,
            hostNamespace: hostNamespace,
            hostTypeNames: hostTypeNames,
            hostMethodNames: hostMethodNames);

        // runtime-entry.cpp
        var entryCode = TemplateCatalog.Render("TestProject.RuntimeEntry.cpp.scriban", model);
        entryCode = PatchRuntimeEntry(entryCode);
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
        string? sdkDir = null,
        bool verificationEnabled = true)
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
        // Clean subjects/ first to prevent stale page*.cpp files from previous
        // codegen runs (with different page counts) from leaking into the build.
        {
            var subjectsDir = Path.Combine(outputDir, "subjects");
            Directory.CreateDirectory(subjectsDir);
            foreach (var stale in Directory.GetFiles(subjectsDir, "*.cpp"))
                TryDeleteFile(stale);
            foreach (var stale in Directory.GetFiles(subjectsDir, "*.h"))
                TryDeleteFile(stale);
            // Copy from GeneratedDirs first, then fallback to resolved codegen dir /generated/
            bool copiedAny = false;
            foreach (var genDir in codegen.GeneratedDirs)
            {
                copiedAny = true;
                foreach (var file in Directory.GetFiles(genDir, "*.cpp"))
                    CopyWithRetry(file, Path.Combine(subjectsDir, Path.GetFileName(file)), overwrite: true);
                foreach (var file in Directory.GetFiles(genDir, "*.h"))
                    CopyWithRetry(file, Path.Combine(subjectsDir, Path.GetFileName(file)), overwrite: true);
            }
            // Fallback: if GeneratedDirs didn't detect the codegen output (flat layout),
            // copy directly from the codegen directory's generated/ subfolder.
            if (!copiedAny)
            {
                var fallbackGen = Path.Combine(outputDir, "codegen", "generated");
                if (Directory.Exists(fallbackGen))
                {
                    foreach (var file in Directory.GetFiles(fallbackGen, "*.cpp"))
                        CopyWithRetry(file, Path.Combine(subjectsDir, Path.GetFileName(file)), overwrite: true);
                    foreach (var file in Directory.GetFiles(fallbackGen, "*.h"))
                        CopyWithRetry(file, Path.Combine(subjectsDir, Path.GetFileName(file)), overwrite: true);
                }
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
        var (hotupdateActive, hostNamespace, hostTypeNames, hostMethodNames) = BuildHostArrays(subjects);

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
            codegenAssemblyNames: codegenAssemblyNames,
            verificationEnabled: verificationEnabled,
            hotupdateActive: hotupdateActive,
            hostNamespace: hostNamespace,
            hostTypeNames: hostTypeNames,
            hostMethodNames: hostMethodNames);

        // ── 4. Render templates → output files ──
        // Verify subjects array integrity before rendering
        if (model.ContainsKey("subjects") && model["subjects"] is Scriban.Runtime.ScriptArray sa)
        {
            int nullInArray = 0;
            for (int i = 0; i < sa.Count; i++)
            {
                if (sa[i] is null)
                {
                    nullInArray++;
                    Console.Error.WriteLine($"  [DIAG] subjects[{i}] is NULL");
                }
            }
            Console.Error.WriteLine($"  [DIAG] subjects array: {sa.Count} entries, {nullInArray} null");
        }
        else
        {
            var typeName = model.ContainsKey("subjects") ? model["subjects"]?.GetType().FullName : "key-not-found";
            Console.Error.WriteLine($"  [DIAG] subjects model key: {typeName}");
        }
        RenderToFile("TestProject.RuntimeEntry.cpp.scriban", model, outputDir, "runtime-entry.cpp");
        // Re-patch runtime-entry.cpp after RenderToFile overwrites it.
        // This is the second write site (post-TPG-copy) that also needs
        // the PatchRuntimeEntry fixes (assert stubs, SEH output, etc.).
        string repatchPath = Path.Combine(outputDir, "runtime-entry.cpp");
        if (File.Exists(repatchPath))
        {
            string repatchCode = File.ReadAllText(repatchPath);
            repatchCode = PatchRuntimeEntry(repatchCode);
            File.WriteAllText(repatchPath, repatchCode);
        }
        RenderToFile("TestProject.Dispatch.cpp.scriban", model, outputDir, "verification_dispatch.generated.cpp");
        RenderToFile("TestProject.SubjectDispatch.h.scriban", model, outputDir, "subjects/subject_dispatch.h");
        RenderToFile("TestProject.SubjectDispatch.cpp.scriban", model, outputDir, "subjects/subject_dispatch.cpp");
        RenderToFile("TestProject.SubjectDispatch.h.scriban", model, outputDir, "subjects/subject_dispatch.h");
        RenderToFile("TestProject.SubjectDispatch.h.scriban", model, outputDir, "subjects/subject_dispatch.h");
        RenderToFile("TestProject.SubjectDispatch.cpp.scriban", model, outputDir, "subjects/subject_dispatch.cpp");
        RenderToFile("TestProject.SubjectDispatch.cpp.scriban", model, outputDir, "subjects/subject_dispatch.cpp");
        RenderToFile("TestProject.SubjectDispatch.h.scriban", model, outputDir, "subjects/subject_dispatch.h");
        RenderToFile("TestProject.SubjectDispatch.cpp.scriban", model, outputDir, "subjects/subject_dispatch.cpp");

        RenderToFile("TestProject.CMakeLists.txt.scriban", model, outputDir, "CMakeLists.txt");
        RenderToFile("TestProject.CMakePresets.json.scriban", model, outputDir, "CMakePresets.json");
        RenderToFile("TestProject.RuntimePatchdata.cpp.scriban", model, outputDir, "runtime-patchdata.cpp");
        RenderToFile("TestProject.PatchHostArrays.cpp.scriban", model, outputDir, "patch-host-arrays.cpp");

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

        // ChaosFindExternalTypeDescByStableId stub — the prebuilt chaos_runtime_core.lib
        // in the SDK has enum_stubs.obj that references this function, but the lib was
        // built without including type_resolve.cpp.  In test environments dynamic type
        // registration is not used, so returning nullptr is always correct.
        var externalTypeDescStubPath = Path.Combine(outputDir, "chaos_stub_ChaosFindExternalTypeDescByStableId.cpp");
        if (!File.Exists(externalTypeDescStubPath))
        {
            File.WriteAllText(externalTypeDescStubPath,
                "// Auto-generated stub for ChaosFindExternalTypeDescByStableId.\n"
                + "// The real definition is in type_resolve.cpp (part of chaos_runtime_core.lib),\n"
                + "// but the prebuilt SDK lib was built without this function.\n"
                + "// In test environments dynamic type registration is not used,\n"
                + "// so returning nullptr is always correct.\n"
                + "#include \"enum_stubs.h\"\n"
                + "struct ReflectionQueryTypeDescriptor;\n"
                + "extern \"C\" const ReflectionQueryTypeDescriptor* ChaosFindExternalTypeDescByStableId(\n"
                + "    uint64_t /*stable_id*/) noexcept { return nullptr; }\n");
        }

        // ApplyPatchFromMemoryEx/Unpatch stubs — patch_loader.cpp may not be included
        // in the prebuilt chaos_runtime_core.lib shipped with the SDK.  Test environments
        // don't use hotpatch, so returning nullptr / true is always correct.
        var patchLoaderStubPath = Path.Combine(outputDir, "chaos_stub_patch_loader.cpp");
        if (!File.Exists(patchLoaderStubPath))
        {
            File.WriteAllText(patchLoaderStubPath,
                "// Auto-generated stubs for patch_loader functions.\n"
                + "// The real definitions are in patch_loader.cpp (part of chaos_runtime_core.lib),\n"
                + "// but the prebuilt SDK lib may not include this file.\n"
                + "// Test environments don't use hotpatch, so stubs are sufficient.\n"
                + "#include <cstdint>\n"
                + "#include <cstddef>\n"
                + "namespace chaos { namespace il2cpp { namespace runtime_core {\n"
                + "struct PatchContext;\n"
                + "PatchContext* ApplyPatchFromMemoryEx(const void*, size_t, const char*,\n"
                + "    const char* const*, const char* const*, int) noexcept { return nullptr; }\n"
                + "bool Unpatch(PatchContext*) noexcept { return true; }\n"
                + "} } }\n");
        }

        // Async yield stubs — async_stubs.cpp is in the SDK runtime_stubs/ but is not
        // compiled by the test project's CMakeLists.txt (only profile_globals.cpp is).
        // The generated code for async methods (e.g. Task.Yield) references these symbols.
        // Stubs return zero/success values sufficient for verification dispatch.
        var asyncStubPath = Path.Combine(outputDir, "chaos_stub_async.cpp");
        if (!File.Exists(asyncStubPath))
        {
            File.WriteAllText(asyncStubPath,
                "// Auto-generated stubs for async runtime helpers.\n"
                + "// The real implementations are in async_stubs.cpp (part of\n"
                + "// chaos_runtime_core.lib) but the SDK prebuilt lib may not\n"
                + "// include them.  Test entry points don't await, so stubs suffice.\n"
                + "#include <chaos/native_types.h>\n"
                + "extern \"C\" {\n"
                + "CHAOS_IL2CPP_INTPTR chaos_async_yield_create(void) noexcept { return 0; }\n"
                + "CHAOS_IL2CPP_INTPTR chaos_async_yield_get_awaiter(CHAOS_IL2CPP_INTPTR) noexcept { return 0; }\n"
                + "CHAOS_IL2CPP_INT32 chaos_async_yield_get_is_completed(CHAOS_IL2CPP_INTPTR) noexcept { return 1; }\n"
                + "void chaos_async_yield_get_result(CHAOS_IL2CPP_INTPTR) noexcept {}\n"
                + "}\n");
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
        string configTier = "check",
        string projectName = "entry")
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
        // IMPORTANT: Exclude the codegen/ directory (which contains the SDK
        // and is still needed for cmake find_package(chaos) to find chaos-config.cmake).
        var codegenDir = Path.GetFullPath(Path.Combine(projectDir, "codegen"));
        foreach (var sub in nativeDir.GetDirectories())
        {
            if (string.Equals(sub.FullName, codegenDir, StringComparison.OrdinalIgnoreCase))
                continue;
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

        // ── CMake generator: use platform default (VS 2022 on Windows, Makefiles on Linux) ──
        var cmakeGeneratorArgs = new List<string>();
        if (OperatingSystem.IsWindows())
        {
            cmakeGeneratorArgs.AddRange(["-G", "Visual Studio 17 2022", "-A", "x64"]);
        }
        else
        {
            cmakeGeneratorArgs.AddRange(["-G", "Unix Makefiles"]);
        }

        cmakeGeneratorArgs.Add($"-DCHAOS_IL2CPP_CONFIG_TIER={configTier.ToLowerInvariant()}");
        cmakeGeneratorArgs.Add(isJit
            ? "-DCHAOS_IL2CPP_JIT_MODE=ON"
            : "-DCHAOS_IL2CPP_JIT_MODE=OFF");

        // ── Step 1: CMake configure (only if cache missing) ──
        if (needsConfigure)
        {
            buildDir.Create();
            // Remove stale cache to prevent "generator platform: x64 does not match platform used previously"
            var oldCache = new FileInfo(Path.Combine(buildDir.FullName, "CMakeCache.txt"));
            if (oldCache.Exists)
            {
                try { oldCache.Delete(); }
                catch { /* best-effort */ }
            }
            var oldCmakeFiles = new DirectoryInfo(Path.Combine(buildDir.FullName, "CMakeFiles"));
            if (oldCmakeFiles.Exists)
            {
                try { oldCmakeFiles.Delete(recursive: true); }
                catch { /* best-effort */ }
            }
            Console.Error.WriteLine($"  [build] cmake configure (fresh)...");
            var cfgArgs = new List<string> { "-S", projectDir, "-B", buildDir.FullName };
            cfgArgs.AddRange(cmakeGeneratorArgs);
            var cfgResult = RunProcess("cmake", cfgArgs, timeoutMs: 120_000);
            if (cfgResult.ExitCode != 0)
            {
                PrintLastLines(cfgResult.StdErr + cfgResult.StdOut, 10);
                Console.Error.WriteLine($"  [build] cmake configure FAILED");
                return null;
            }
        }
        else
        {
            Console.Error.WriteLine($"  [build] cmake configure (cached, skip)");
        }

        // ── Step 2: CMake incremental build (3 retries) ──
        Console.Error.WriteLine($"  [build] cmake build (incremental)...");
        var buildResult = RunProcess("cmake", ["--build", buildDir.FullName, "--config", "RelWithDebInfo", "--target", projectName], timeoutMs: 1_800_000);
        if (buildResult.ExitCode != 0)
        {
            PrintLastLines(buildResult.StdErr + buildResult.StdOut, 20);
            Console.Error.WriteLine($"  [build] cmake build FAILED");
            return null;
        }

        // ── Step 3: Locate entry.exe ──
        var exeCandidates = new[]
        {
            Path.Combine(buildDir.FullName, "RelWithDebInfo", $"{projectName}.exe"),
            Path.Combine(buildDir.FullName, "Release", $"{projectName}.exe"),
            Path.Combine(buildDir.FullName, "Debug", $"{projectName}.exe"),
            Path.Combine(buildDir.FullName, $"{projectName}.exe"),
            // Linux — no .exe extension
            Path.Combine(buildDir.FullName, "RelWithDebInfo", projectName),
            Path.Combine(buildDir.FullName, "Release", projectName),
            Path.Combine(buildDir.FullName, "Debug", projectName),
            Path.Combine(buildDir.FullName, projectName),
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
            Console.Error.WriteLine($"  [build] {projectName} not found in build output");
            return null;
        }

        // ── Step 4: Copy to output with retries ──
        var outputName = isJit ? "entry-jit.exe" : "entry.exe";
        var targetPath = Path.Combine(projectDir, outputName);
        try
        {
            if (File.Exists(targetPath))
                File.Delete(targetPath);
            File.Copy(exePath, targetPath, overwrite: true);
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"  [build] Copy failed: {ex.Message}");
            return null;
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
        string? sdkDir = null,
        bool verificationEnabled = true)
    {
        Emit(outputDir, codegen, subjects, isJit, configTier, isWindows, projectRoot, codegenDir, sdkDir, verificationEnabled);
        var isPipeline = projectRoot is not null && codegenDir is not null;
        var projectName = isPipeline ? "chaos_entry" : "entry";
        var exePath = BuildProject(outputDir, isJit, configTier, projectName);
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
            "runtime-entry.cpp",             // needed by hotupdate cmake rebuild
            "microbench.cpp",                // needed by hotupdate cmake rebuild
            "runtime-patchdata.cpp",         // needed by hotupdate cmake rebuild
            "verification_dispatch.generated.cpp", // needed by hotupdate cmake rebuild
            "patch-host-arrays.cpp",         // needed by hotupdate cmake rebuild
            "chaos_stub_ChaosFindExternalTypeDescByStableId.cpp", // runtime stubs — needed by hotupdate cmake rebuild
            "chaos_stub_patch_loader.cpp",   // runtime stubs
            "chaos_stub_async.cpp",          // runtime stubs
        };
        var keepDirs = new HashSet<string>(StringComparer.OrdinalIgnoreCase)
        {
            "build",
            "build_jit",
            "codegen",      // SDK files (chaos-config.cmake, libs, headers) — kept for hotupdate CMake reconfigure
            "subjects",     // Flat copy of codegen output (native-aot.generated.*, type-info-defs.*) — needed by linker
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

        // Remove stale bridge-redirect stubs from subjects/ after successful build.
        // These files (bridge-redirect.generated.cpp, chaos_register_bridge_redirects.generated.cpp)
        // are injected by the hotupdate Python pipeline into subjects/ before cmake rebuild,
        // but reference symbols no longer present after codegen changes and cause LNK2019
        // on subsequent builds.  TPG must purge them to eliminate the LINK-HACK-1 workaround
        // from the Python pipeline.
        var subjectsDir2 = Path.Combine(projectDir, "subjects");
        if (Directory.Exists(subjectsDir2))
        {
            TryDeleteFile(Path.Combine(subjectsDir2, "bridge-redirect.generated.cpp"));
            TryDeleteFile(Path.Combine(subjectsDir2, "chaos_register_bridge_redirects.generated.cpp"));
        }
    }

    private static void RenderToFile(string templateName, Scriban.Runtime.ScriptObject model, string outputDir, string outputFileName)
    {
        var rendered = TemplateCatalog.Render(templateName, model);
        var path = Path.Combine(outputDir, outputFileName);
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        File.WriteAllText(path, rendered);
    }

    /// <summary>
    /// Post-process generated runtime-entry.cpp for template limitations.
    /// Currently handles: RunProfileMode function injection (not expressible in scriban).
    /// </summary>
    private static string PatchRuntimeEntry(string code)
    {
        // Fix 1: Insert RunProfileMode function before --benchmark-range marker
        // This adds --profile CLI support for GC profile baseline collection.
        // Must be done here (not in scriban) because the scriban linter reverts changes.
        if (!code.Contains("RunProfileMode"))
        {
            string rangeMarker = "// ── --benchmark-range: benchmark a contiguous range of method table indices ─";
            string profileFn =
                "\n// ── --profile: benchmark all methods with GC profile metrics ─────\n" +
                "static int RunProfileMode() noexcept {\n" +
                "    const int kCount = kSubjectEntryCount;\n" +
                "    auto* entries = GetHotpatchEntries();\n" +
                "    printf(\"{\\\"profile\\\":[\");\n" +
                "    fflush(stdout);\n" +
                "    bool first = true;\n" +
                "    for (int si = 0; si < kCount; si++) {\n" +
                "        int i = kSubjectSlotMap[si];\n" +
                "#if defined(_WIN32)\n" +
                "        __try {\n" +
                "#endif\n" +
                "            auto gcBefore = chaos::il2cpp::runtime_core::chaos_gc_get_total_pause_duration();\n" +
                "            auto allocBefore = chaos::il2cpp::runtime_core::chaos_gc_get_allocated_bytes_for_current_thread();\n" +
                "            auto gcCountBefore = chaos::il2cpp::runtime_core::chaos_gc_get_collection_count(0);\n" +
                "            for (int w = 0; w < 100; w++)\n" +
                "                chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(entries, kAotMethodCount, i);\n" +
                "            if (!first) printf(\",\");\n" +
                "            first = false;\n" +
                "            auto start = std::chrono::steady_clock::now();\n" +
                "            chaos::il2cpp::runtime_core::ChaosDispatchMethodBenchDirect(entries, kAotMethodCount, i);\n" +
                "            auto end = std::chrono::steady_clock::now();\n" +
                "            auto gcAfter = chaos::il2cpp::runtime_core::chaos_gc_get_total_pause_duration();\n" +
                "            auto allocAfter = chaos::il2cpp::runtime_core::chaos_gc_get_allocated_bytes_for_current_thread();\n" +
                "            auto gcCountAfter = chaos::il2cpp::runtime_core::chaos_gc_get_collection_count(0);\n" +
                "            double ms = std::chrono::duration<double, std::milli>(end - start).count();\n" +
                "            printf(\"{\\\"methodIndex\\\":%d,\\\"gcPauseNs\\\":%\" PRId64 \",\\\"maxGcPauseNs\\\":%\" PRId64 \",\"\n" +
                "                   \"\\\"nurseryAllocBytes\\\":%\" PRId64 \",\\\"allocCount\\\":%d,\\\"elapsedMs\\\":%.3f}\",\n" +
                "                   i, (int64_t)(gcAfter - gcBefore), (int64_t)(gcAfter - gcBefore),\n" +
                "                   (int64_t)(allocAfter - allocBefore), (int)(gcCountAfter - gcCountBefore), ms);\n" +
                "#if defined(_WIN32)\n" +
                "        } __except(EXCEPTION_EXECUTE_HANDLER) {\n" +
                "            if (!first) printf(\",\");\n" +
                "            first = false;\n" +
                "            printf(\"{\\\"methodIndex\\\":%d,\\\"gcPauseNs\\\":0,\\\"maxGcPauseNs\\\":0,\\\"nurseryAllocBytes\\\":0,\\\"allocCount\\\":0,\\\"error\\\":\\\"exception\\\"}\", i);\n" +
                "        }\n" +
                "#endif\n" +
                "        fflush(stdout);\n" +
                "    }\n" +
                "    printf(\"],\\\"summary\\\":{\\\"methodCount\\\":%d}}\\n\", kCount);\n" +
                "    fflush(stdout);\n" +
                "    CHAOS_IL2CPP_PROFILE_DUMP();\n" +
                "    return 0;\n" +
                "}\n\n";
            if (code.Contains(rangeMarker))
                code = code.Replace(rangeMarker, profileFn + rangeMarker);
        }
        // Insert --profile CLI handler after --benchmark-all handler
        string benchAllHandler = "if (std::strcmp(argv[1], \"--benchmark-all\") == 0) {";
        string profileHandler =
            "    if (std::strcmp(argv[1], \"--profile\") == 0) {\n" +
            "        ret = RunProfileMode();\n        goto shutdown;\n    }\n\n" +
            "    if (std::strcmp(argv[1], \"--benchmark-all\") == 0) {";
        if (code.Contains(benchAllHandler) && !code.Contains("strcmp(argv[1], \"--profile\")"))
            code = code.Replace(benchAllHandler, profileHandler);

        // Fix 3: Replace broken printf in Windows fact-json path with proper \n escape.
        // The Scriban linter reverts \n to real newlines; patch here as post-processing.
        code = System.Text.RegularExpressions.Regex.Replace(
            code,
            @"printf\(""\]\}\s*\n\s*""\);",
            "printf(\"]}\\n\");");

        // Fix 4: Replace Windows worker thread in RunFactJsonMode with direct dispatch.
        // The Scriban template's worker thread cannot access TLS runtime state set up
        // by ChaosRuntimeHost on the main thread, causing intermittent AV crashes.
        code = System.Text.RegularExpressions.Regex.Replace(
            code,
            @"// Use a worker thread with 300s timeout.*?" +
            @"TerminateProcess\(GetCurrentProcess\(\), 0\);\s*\}\s*return 0;\s*#else",
            "// Direct dispatch on main thread (worker thread removed).",
            System.Text.RegularExpressions.RegexOptions.Singleline);

        return code;
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

    /// <summary>
    /// File.Copy with retry to handle transient locking (e.g. NativeCodegenValidator
    /// in the Driver releases file handles asynchronously on Windows).
    /// </summary>
    private static void CopyWithRetry(string source, string dest, bool overwrite, int maxRetries = 5, int delayMs = 300)
    {
        for (int i = 0; ; i++)
        {
            try
            {
                File.Copy(source, dest, overwrite);
                return;
            }
            catch (IOException) when (i < maxRetries - 1)
            {
                Thread.Sleep(delayMs);
            }
        }
    }

    /// Best-effort file deletion (no-op if file is locked or missing).
    private static void TryDeleteFile(string path)
    {
        try { File.Delete(path); }
        catch { /* best-effort */ }
    }

    /// <summary>
    /// Build hotupdate host arrays from subjects with Kind == HotUpdate.
    /// Returns (hotupdateActive, hostNamespace, hostTypeNames, hostMethodNames).
    /// The type names are stripped of "AutoGenerated.{assembly}." prefix to match
    /// the AOT codegen's HotpatchNameRegistry registration.
    /// </summary>
    private static (bool Active, string Namespace, List<string> TypeNames, List<string> MethodNames)
        BuildHostArrays(IReadOnlyList<SubjectModel> subjects)
    {
        var hotupdateSubjects = subjects.Where(s => s.Kind == SubjectKind.HotUpdate).ToList();
        if (hotupdateSubjects.Count == 0)
            return (false, "", new List<string>(), new List<string>());

        // Use the first hotupdate subject's assembly name to build the namespace.
        // All subjects in a chunk share the same assembly.
        var assemblyName = hotupdateSubjects[0].AssemblyName;
        var sanitizedAssembly = SanitizeCppIdentifier(assemblyName);
        var hostNs = "AutoGenerated." + sanitizedAssembly;
        var prefix = hostNs + ".";

        var typeNames = new List<string>(hotupdateSubjects.Count);
        var methodNames = new List<string>(hotupdateSubjects.Count);

        foreach (var s in hotupdateSubjects)
        {
            // Strip "AutoGenerated.{assembly}." prefix from FullTypeName to get
            // the raw AOT type name (e.g. "System_Collections_Generic_XTests").
            var typeName = s.FullTypeName;
            if (typeName.StartsWith(prefix, StringComparison.Ordinal))
                typeName = typeName[prefix.Length..];
            typeNames.Add(typeName);

            // Use GeneratedMethodId if available (AOT-suffix method name),
            // otherwise fall back to the raw method name.
            var methodName = s.GeneratedMethodId ?? s.MethodName;
            methodNames.Add(methodName);
        }

        return (true, hostNs, typeNames, methodNames);
    }

    /// <summary>
    /// Convert .NET identifier to C++ identifier: '.' -> '_', strip non-alphanumeric.
    /// Mirrors SanitizeIdentifier in Program.cs and _sanitize_cpp_identifier in Python.
    /// </summary>
    private static string SanitizeCppIdentifier(string name)
    {
        var sb = new System.Text.StringBuilder();
        foreach (var c in name)
        {
            if (char.IsLetterOrDigit(c) || c == '_')
                sb.Append(c);
            else if (c == '.')
                sb.Append('_');
        }
        if (sb.Length > 0 && char.IsDigit(sb[0]))
            sb.Insert(0, '_');
        return sb.ToString();
    }
}
