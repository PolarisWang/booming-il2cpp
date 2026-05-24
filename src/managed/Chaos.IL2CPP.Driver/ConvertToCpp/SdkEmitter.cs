using System.Diagnostics;
using Chaos.IL2CPP.Generator.BuildSystem;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Assembles the chaos-sdk/ directory — a self-contained CMake package
/// that consuming projects can use via find_package(chaos).
///
/// After convert-to-cpp emits all source files to generatedRoot (= sdkRoot/generated/),
/// this emitter adds the SDK files to sdkRoot/:
///   chaos-sdk/
///   ├── chaos-config.cmake          ← find_package entry
///   ├── cmake/chaos-targets.cmake   ← target definitions
///   ├── include/
///   │   ├── chaos.h                 ← umbrella header
///   │   └── chaos_generated_module.h ← copied from generated/
///   └── lib/                        ← prebuilt native .lib files + precompiled codegen
///
/// Phase 1: Copy headers, render CMake templates, copy prebuilt runtime libs
/// Phase 2: Precompile native-aot.generated.cpp → chaos_codegen.lib (if MSVC available)
/// </summary>
internal sealed class SdkEmitter
{
    /// <summary>
    /// Assemble the chaos-sdk/ directory from codegen output.
    /// </summary>
    /// <param name="sdkRoot">Root of the chaos-sdk/ directory (contains chaos-config.cmake).</param>
    /// <param name="generatedRoot">Codegen output root (sdkRoot/generated/).</param>
    /// <param name="repoRoot">Repository root (used to resolve include paths and native lib dir).</param>
    /// <param name="nativeLibDir">Path to prebuilt native .lib files (e.g. $REPO/build/native).</param>
    /// <param name="buildConfig">Build configuration (Debug/RelWithDebInfo/Release).</param>
    /// <param name="assemblyName">Name of the assembly being codegen'd (for locating generated headers).</param>
    /// <returns>true on success, false on failure.</returns>
    public bool EmitSdk(
        string sdkRoot,
        string generatedRoot,
        string repoRoot,
        string nativeLibDir,
        string buildConfig,
        string assemblyName)
    {
        try
        {
            var includeDir = Path.Combine(sdkRoot, "include");
            var libDir = Path.Combine(sdkRoot, "lib");
            var cmakeDir = Path.Combine(sdkRoot, "cmake");

            Directory.CreateDirectory(includeDir);
            Directory.CreateDirectory(libDir);
            Directory.CreateDirectory(cmakeDir);

            // ── Phase 1a: Copy generated headers from generated/ to include/ ──
            CopyGeneratedHeader(generatedRoot, assemblyName, includeDir, "chaos_generated_module.h");
            CopyGeneratedHeader(generatedRoot, assemblyName, includeDir, "chaos_generated_module.cpp");

            // ── Phase 1b: Generate chaos.h from template ─────────────────────
            var headerModel = new ScriptObject
            {
                ["assembly_name"] = assemblyName,
            };
            var chaosHeaderContent = SdkTemplateCatalog.RenderChaosHeader(headerModel);
            File.WriteAllText(Path.Combine(includeDir, "chaos.h"), chaosHeaderContent);
            Console.WriteLine($"    SDK header: chaos.h");

            // ── Phase 1b': Copy runtime headers needed by chaos.h ────────────
            CopyRuntimeHeaders(repoRoot, includeDir);

            // ── Phase 1c: Generate chaos-config.cmake ────────────────────────
            var configContent = SdkTemplateCatalog.RenderChaosConfig();
            File.WriteAllText(Path.Combine(sdkRoot, "chaos-config.cmake"), configContent);
            Console.WriteLine($"    SDK cmake: chaos-config.cmake");

            // ── Phase 1d: Generate chaos-targets.cmake ───────────────────────
            var targetsContent = SdkTemplateCatalog.RenderChaosTargets();
            File.WriteAllText(Path.Combine(cmakeDir, "chaos-targets.cmake"), targetsContent);
            Console.WriteLine($"    SDK cmake: chaos-targets.cmake");

            // ── Phase 1e: Copy prebuilt native runtime .lib files ────────────
            CopyNativeLibs(nativeLibDir, buildConfig, libDir);

            // ── Phase 2: Precompile native-aot.generated.cpp → chaos_codegen.lib ──
            TryPrecompileCodegenLib(generatedRoot, repoRoot, sdkRoot, libDir);

            Console.WriteLine($"SDK assembled: {sdkRoot}");
            return true;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"SDK emission failed: {ex.Message}");
            return false;
        }
    }

    /// <summary>
    /// Copy a generated file from generatedRoot to the SDK include directory.
    /// Searches generatedRoot/&lt;assembly&gt;/generated/ first, then generatedRoot/generated/.
    /// </summary>
    private static void CopyGeneratedHeader(string generatedRoot, string assemblyName, string includeDir, string fileName)
    {
        var candidates = new[]
        {
            Path.Combine(generatedRoot, assemblyName, "generated", fileName),
            Path.Combine(generatedRoot, "generated", fileName),
        };

        foreach (var candidate in candidates)
        {
            if (File.Exists(candidate))
            {
                File.Copy(candidate, Path.Combine(includeDir, fileName), overwrite: true);
                Console.WriteLine($"    SDK {(fileName.EndsWith(".h") ? "header" : "source")}: {fileName}");
                return;
            }
        }

        Console.WriteLine($"    WARN: generated {fileName} not found (searched in generatedRoot)");
    }

    /// <summary>
    /// Copy runtime headers needed by chaos.h from the repo tree into the
    /// SDK include/ directory, preserving subdirectory structure.
    ///
    /// Maps: src/native/common/chaos/ → include/chaos/
    ///       src/native/runtime-core/ → include/ (top-level files)
    ///       src/native/runtime-core/gc/ → include/gc/
    ///       src/native/runtime-core/runtime_stubs/ → include/runtime_stubs/
    ///       src/native/codegen/ selected → include/
    ///       contracts/native/v0/ → include/
    ///
    /// This provides Level 1 self-containment: chaos.h + ChaosRuntimeHost
    /// compile out of the box. Full Level 2 (generated code compilation)
    /// requires the repo tree include paths.
    /// </summary>
    private static void CopyRuntimeHeaders(string repoRoot, string includeDir)
    {
        // Source directories
        var srcCommon = Path.Combine(repoRoot, "src", "native", "common");
        var srcRuntimeCore = Path.Combine(repoRoot, "src", "native", "runtime-core");
        var srcGc = Path.Combine(srcRuntimeCore, "gc");
        var srcRuntimeStubs = Path.Combine(srcRuntimeCore, "runtime_stubs");
        var srcCodegen = Path.Combine(repoRoot, "src", "native", "codegen");
        var srcContracts = Path.Combine(repoRoot, "contracts", "native", "v0");

        // Target subdirs
        var dstChaos = Path.Combine(includeDir, "chaos");
        var dstGc = Path.Combine(includeDir, "gc");
        var dstRuntimeStubs = Path.Combine(includeDir, "runtime_stubs");

        Directory.CreateDirectory(dstChaos);
        Directory.CreateDirectory(dstGc);
        Directory.CreateDirectory(dstRuntimeStubs);

        int count = 0;

        // ── Copy chaos/*.h (common public API surface) ─────────────────────
        // Copy ALL headers — too many inter-dependencies to cherry-pick.
        var chaosSrcDir = Path.Combine(srcCommon, "chaos");
        if (Directory.Exists(chaosSrcDir))
        {
            foreach (var f in Directory.GetFiles(chaosSrcDir, "*.h"))
            {
                File.Copy(f, Path.Combine(dstChaos, Path.GetFileName(f)), overwrite: true);
                count++;
            }
        }

        // ── Copy runtime-core *.h (top-level) ──────────────────────────────
        // Needed by: chaos_runtime_host.h, generated code
        var runtimeCoreHeaders = new[]
        {
            "chaos_runtime_host.h", "runtime_core.h",
            "com_ccw.h", "module_registry.h", "abi_manifest.h",
            "hotpatch_table.h", "runtime_vtable.h", "runtime_instantiation.h",
            "reflection_query_model.h", "load_store_chaos_bridge.h",
            "interpreter_entry.h", "exception_helpers.h", "thread_state.h",
            "forbid_suspend.h", "memory_domain.h", "convert.h",
            "enum_stubs.h", "patch_loader.h", "jit_registration.h",
            "ChaosGeneratedRuntimePrelude.h",
        };
        foreach (var h in runtimeCoreHeaders)
        {
            var src = Path.Combine(srcRuntimeCore, h);
            if (File.Exists(src))
            {
                File.Copy(src, Path.Combine(includeDir, h), overwrite: true);
                count++;
            }
        }

        // ── Copy gc/*.h ────────────────────────────────────────────────────
        // Needed by: chaos_runtime_host.h, generated code
        var gcHeaders = new[]
        {
            "gc_api.h", "gc_bgc_inline.h", "gc_helpers.h", "gc_card_table.h",
            "gc_layout.h", "gc_bump_cache.h",
        };
        foreach (var h in gcHeaders)
        {
            var src = Path.Combine(srcGc, h);
            if (File.Exists(src))
            {
                File.Copy(src, Path.Combine(dstGc, h), overwrite: true);
                count++;
            }
        }

        // ── Copy runtime_stubs/*.h ────────────────────────────────────────
        // Needed by: chaos_runtime_host.h
        var stubsHeaders = new[]
        {
            "misc_stubs.h", "array_stubs.h", "stub_common.h",
        };
        foreach (var h in stubsHeaders)
        {
            var src = Path.Combine(srcRuntimeStubs, h);
            if (File.Exists(src))
            {
                File.Copy(src, Path.Combine(dstRuntimeStubs, h), overwrite: true);
                count++;
            }
        }

        // ── Copy codegen/*.h ──────────────────────────────────────────────
        // Needed by: chaos_runtime_host.h, generated code
        var codegenHeaders = new[]
        {
            "codegen_bridge.h",
        };
        foreach (var h in codegenHeaders)
        {
            var src = Path.Combine(srcCodegen, h);
            if (File.Exists(src))
            {
                File.Copy(src, Path.Combine(includeDir, h), overwrite: true);
                count++;
            }
        }

        // ── Copy contracts/*.h ────────────────────────────────────────────
        // Needed by: runtime_core.h / runtime_abi.h
        var contractHeaders = new[]
        {
            "runtime_abi.h",
        };
        foreach (var h in contractHeaders)
        {
            var src = Path.Combine(srcContracts, h);
            if (File.Exists(src))
            {
                File.Copy(src, Path.Combine(includeDir, h), overwrite: true);
                count++;
            }
        }

        Console.WriteLine($"    SDK runtime headers: {count} files copied");
    }

    /// <summary>
    /// Copy prebuilt native runtime .lib files from the native build output
    /// directory into the SDK's lib/ directory.
    /// </summary>
    private static void CopyNativeLibs(string nativeLibDir, string buildConfig, string libDir)
    {
        var libMappings = new (string SourceRelative, string TargetName)[]
        {
            ($"src/native/runtime-core/{buildConfig}/chaos_runtime_core.lib",          "chaos_runtime_core.lib"),
            ($"src/native/bootstrap/{buildConfig}/chaos_bootstrap.lib",                "chaos_bootstrap.lib"),
            ($"src/native/common/{buildConfig}/chaos_common.lib",                      "chaos_common.lib"),
            ($"src/native/support/{buildConfig}/chaos_support.lib",                    "chaos_support.lib"),
            ($"src/native/interpreter/{buildConfig}/chaos_interpreter.lib",            "chaos_interpreter.lib"),
            ($"fmt_build/{buildConfig}/chaos_fmt.lib",                                 "chaos_fmt.lib"),
            ($"src/native/hot-update/{buildConfig}/chaos_hot_update.lib",              "chaos_hot_update.lib"),
            ($"src/native/diagnostics/eventpipe/{buildConfig}/chaos_eventpipe.lib",     "chaos_eventpipe.lib"),
            ($"src/native/jit/{buildConfig}/chaos_jit.lib",                            "chaos_jit.lib"),
            ($"src/native/diagnostics/debugger/{buildConfig}/chaos_debugger.lib",       "chaos_debugger.lib"),
        };

        int copiedCount = 0;
        foreach (var (sourceRelative, targetName) in libMappings)
        {
            var sourcePath = Path.Combine(nativeLibDir, sourceRelative.Replace('/', Path.DirectorySeparatorChar));
            var targetPath = Path.Combine(libDir, targetName);

            if (File.Exists(sourcePath))
            {
                File.Copy(sourcePath, targetPath, overwrite: true);
                copiedCount++;
            }
            else
            {
                Console.WriteLine($"    WARN: prebuilt lib not found: {sourcePath}");
            }
        }

        Console.WriteLine($"    SDK libs: {copiedCount}/{libMappings.Length} copied");
    }

    /// <summary>
    /// Phase 2: Precompile native-aot.generated.cpp into chaos_codegen.lib.
    /// Attempts to locate MSVC cl.exe and lib.exe, compiles the single
    /// translation unit, and bundles the .obj into a static library.
    ///
    /// Falls back gracefully with a warning if MSVC is not available or
    /// the generated source file is not found.
    /// </summary>
    private static void TryPrecompileCodegenLib(
        string generatedRoot,
        string repoRoot,
        string sdkRoot,
        string libDir)
    {
        // Locate the generated source file
        var generatedCppPaths = Directory.GetFiles(generatedRoot, "native-aot.generated.cpp", SearchOption.AllDirectories);
        if (generatedCppPaths.Length == 0)
        {
            Console.WriteLine("    WARN: native-aot.generated.cpp not found, skipping precompilation");
            return;
        }
        var generatedCpp = generatedCppPaths[0];
        Console.WriteLine($"    [precompile] found: {generatedCpp}");

        // Detect MSVC compiler
        var clPath = FindMsvcCompiler("cl.exe");
        if (clPath == null)
        {
            Console.WriteLine("    WARN: MSVC cl.exe not found, skipping precompilation");
            Console.WriteLine("    HINT: Run from a Visual Studio Developer Command Prompt, or ensure");
            Console.WriteLine("          VCToolsInstallDir environment variable is set.");
            return;
        }

        var libExePath = FindMsvcCompiler("lib.exe");
        if (libExePath == null)
        {
            Console.WriteLine("    WARN: MSVC lib.exe not found, skipping precompilation");
            return;
        }

        Console.WriteLine($"    [precompile] MSVC: {clPath}");

        // Build include paths
        var includePaths = new List<string>
        {
            // SDK headers
            Path.Combine(sdkRoot, "include"),
            // Generated code headers
            generatedRoot,
            Path.GetDirectoryName(generatedCpp)!,
            // Repo source tree
            Path.Combine(repoRoot, "src", "native", "common"),
            Path.Combine(repoRoot, "src", "native", "runtime-core"),
            Path.Combine(repoRoot, "src", "native", "runtime-core", "gc"),
            Path.Combine(repoRoot, "src", "native", "runtime-core", "runtime_stubs"),
            Path.Combine(repoRoot, "src", "native", "bootstrap"),
            Path.Combine(repoRoot, "src", "native", "interpreter"),
            Path.Combine(repoRoot, "src", "native", "interpreter", "generated"),
            Path.Combine(repoRoot, "src", "native", "codegen"),
            Path.Combine(repoRoot, "src", "native", "support"),
            Path.Combine(repoRoot, "src", "native", "hot-update"),
            Path.Combine(repoRoot, "src", "native"),
            Path.Combine(repoRoot, "contracts", "native", "v0"),
            Path.Combine(repoRoot, "third_party", "fmt", "include"),
            Path.Combine(repoRoot, "third_party", "unordered_dense", "include"),
        };

        // Add MSVC CRT include path and Windows SDK include paths for standard library headers
        var (vcIncludeDir, sdkIncludeDirs) = FindVcAndSdkIncludePaths();
        if (vcIncludeDir != null)
            includePaths.Add(vcIncludeDir);
        if (sdkIncludeDirs != null)
            includePaths.AddRange(sdkIncludeDirs);

        var includeArgs = string.Join(" ", includePaths.Select(p => $"/I\"{p}\""));

        var objPath = Path.Combine(libDir, "chaos_codegen.obj");
        var libPath = Path.Combine(libDir, "chaos_codegen.lib");

        // Step 1: Compile .cpp → .obj
        // Use /EHsc (not /EHa) for precompiled lib since generated code uses
        // C++ exception handling (throw chaos_managed_exception). The runtime
        // entry point that catches these needs /EHa, but the generated method
        // bodies themselves only throw, not catch.
        var compileArgs = $"/nologo /std:c++20 /utf-8 /EHsc /GS- /DWIN32 /D_WINDOWS " +
                           $"{includeArgs} /c \"{generatedCpp}\" /Fo\"{objPath}\"";

        Console.WriteLine($"    [precompile] compiling...");

        var compileOut = RunProcessWithOutputCapture(clPath, compileArgs);
        if (compileOut == null)
        {
            Console.WriteLine("    WARN: failed to start cl.exe, skipping precompilation");
            return;
        }

        if (!File.Exists(objPath))
        {
            Console.WriteLine("    WARN: precompilation failed (no .obj produced)");
            var errOutput = (compileOut.Value.Error ?? "").Trim();
            var stdOutput = (compileOut.Value.Output ?? "").Trim();
            if (errOutput.Length > 0)
            {
                foreach (var line in errOutput.Split('\n', StringSplitOptions.RemoveEmptyEntries))
                    Console.WriteLine($"      {line.Trim()}");
            }
            if (stdOutput.Length > 0)
            {
                foreach (var line in stdOutput.Split('\n', StringSplitOptions.RemoveEmptyEntries))
                    Console.WriteLine($"      {line.Trim()}");
            }
            return;
        }

        var objSize = new FileInfo(objPath).Length;
        Console.WriteLine($"    [precompile] compiled: {objSize} bytes -> {objPath}");

        // Step 2: Bundle .obj → .lib
        var libArgs = $"/nologo /out:\"{libPath}\" \"{objPath}\"";

        Console.WriteLine($"    [precompile] creating library...");

        var libResult = RunProcessWithOutputCapture(libExePath, libArgs);
        if (libResult == null)
        {
            Console.WriteLine("    WARN: failed to start lib.exe, skipping library creation");
            return;
        }

        if (File.Exists(libPath))
        {
            var libSize = new FileInfo(libPath).Length;
            Console.WriteLine($"    [precompile] chaos_codegen.lib: {libSize} bytes");
            // Clean up intermediate .obj
            try { File.Delete(objPath); } catch { }
        }
        else
        {
            Console.WriteLine("    WARN: chaos_codegen.lib not created");
            foreach (var line in libResult.Value.Error.Trim().Split('\n', StringSplitOptions.RemoveEmptyEntries))
                Console.WriteLine($"      {line.Trim()}");
        }
    }

    /// <summary>
    /// Run a process with stdout/stderr capture using async reads to
    /// avoid deadlock (both streams are read concurrently via tasks).
    /// Returns null if the process could not be started.
    /// </summary>
    private static (string Output, string Error)? RunProcessWithOutputCapture(string executable, string arguments)
    {
        var psi = new ProcessStartInfo(executable, arguments)
        {
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
            CreateNoWindow = true,
        };

        using var proc = Process.Start(psi);
        if (proc == null)
            return null;

        // Read both streams concurrently to avoid deadlock
        var outputTask = proc.StandardOutput.ReadToEndAsync();
        var errorTask = proc.StandardError.ReadToEndAsync();

        if (!proc.WaitForExit(120_000))
        {
            try { proc.Kill(); } catch { }
            return null;
        }

        var output = outputTask.GetAwaiter().GetResult();
        var error  = errorTask.GetAwaiter().GetResult();
        return (output, error);
    }

    /// <summary>
    /// Locate the MSVC compiler tool (cl.exe or lib.exe).
    /// Search order: PATH → VCToolsInstallDir → common VS install locations.
    /// </summary>
    private static string? FindMsvcCompiler(string toolName)
    {
        // 1. Check PATH (works in VS Developer Command Prompt)
        try
        {
            var result = RunProcessWithOutputCapture("where", toolName);
            if (result != null)
            {
                var firstLine = result.Value.Output
                    .Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries)
                    .FirstOrDefault();
                if (firstLine != null && File.Exists(firstLine))
                    return firstLine;
            }
        }
        catch
        {
            // Ignore errors from 'where' command
        }

        // 2. Check VCToolsInstallDir environment variable
        var vcToolsDir = Environment.GetEnvironmentVariable("VCToolsInstallDir");
        if (!string.IsNullOrEmpty(vcToolsDir))
        {
            var candidate = Path.Combine(vcToolsDir, "bin", "Hostx64", "x64", toolName);
            if (File.Exists(candidate))
                return candidate;
        }

        // 3. Check VS install directory via VSINSTALLDIR
        var vsInstallDir = Environment.GetEnvironmentVariable("VSINSTALLDIR");
        if (!string.IsNullOrEmpty(vsInstallDir))
        {
            var candidate = Path.Combine(vsInstallDir, "VC", "Tools", "MSVC");
            if (Directory.Exists(candidate))
            {
                // Find the latest MSVC toolchain version
                var versions = Directory.GetDirectories(candidate)
                    .Select(Path.GetFileName)
                    .OrderByDescending(v => v)
                    .ToList();
                foreach (var version in versions)
                {
                    var toolPath = Path.Combine(candidate, version, "bin", "Hostx64", "x64", toolName);
                    if (File.Exists(toolPath))
                        return toolPath;
                }
            }
        }

        // 4. Common VS 2022 install paths (fallback)
        var commonPaths = new[]
        {
            @"C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC",
            @"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC",
            @"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC",
            @"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC",
        };

        foreach (var basePath in commonPaths)
        {
            if (!Directory.Exists(basePath))
                continue;
            var versions = Directory.GetDirectories(basePath)
                .Select(Path.GetFileName)
                .OrderByDescending(v => v)
                .ToList();
            foreach (var version in versions)
            {
                var toolPath = Path.Combine(basePath, version, "bin", "Hostx64", "x64", toolName);
                if (File.Exists(toolPath))
                    return toolPath;
            }
        }

        return null;
    }

    /// <summary>
    /// Find the MSVC CRT include directory and Windows SDK include directories
    /// needed for compiling with cl.exe outside a VS developer prompt.
    /// Returns (vcIncludeDir, sdkIncludeDirs) where sdkIncludeDirs contains
    /// the ucrt, um, and shared subdirectories of the Windows SDK.
    /// </summary>
    private static (string? VcIncludeDir, List<string>? SdkIncludeDirs) FindVcAndSdkIncludePaths()
    {
        string? vcToolsRoot = null;
        string? vcVersion = null;

        // 1. From VCToolsInstallDir
        var vcToolsDir = Environment.GetEnvironmentVariable("VCToolsInstallDir");
        if (!string.IsNullOrEmpty(vcToolsDir))
        {
            var di = new DirectoryInfo(vcToolsDir);
            vcToolsRoot = di.Parent?.Parent?.FullName; // VCToolsInstallDir/bin/Hostx64/x64 → up 3 levels
            if (vcToolsRoot != null)
            {
                // The version is the leaf name under VC/Tools/MSVC/<version>/
                var msvcDir = new DirectoryInfo(Path.Combine(vcToolsRoot, ".."));
                if (msvcDir.Exists)
                    vcVersion = msvcDir.Name;
            }
        }

        // 2. Fallback: probe common VS install dirs
        if (vcToolsRoot == null)
        {
            var commonPaths = new[]
            {
                @"C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC",
                @"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC",
                @"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC",
                @"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC",
            };
            foreach (var basePath in commonPaths)
            {
                if (!Directory.Exists(basePath))
                    continue;
                var versions = Directory.GetDirectories(basePath)
                    .OrderByDescending(v => v)
                    .ToList();
                if (versions.Count > 0)
                {
                    vcToolsRoot = Path.Combine(basePath, versions[0]);
                    vcVersion = versions[0];
                    break;
                }
            }
        }

        string? vcIncludeDir = null;
        if (vcToolsRoot != null)
        {
            vcIncludeDir = Path.Combine(vcToolsRoot, "include");
            if (!Directory.Exists(vcIncludeDir))
                vcIncludeDir = null;
        }

        // Find Windows SDK include dirs
        List<string>? sdkIncludeDirs = null;
        var sdkDir = Environment.GetEnvironmentVariable("WindowsSdkDir");
        if (string.IsNullOrEmpty(sdkDir))
        {
            // Fallback: common Windows Kits install path
            var sdkRoots = new[]
            {
                @"C:\Program Files (x86)\Windows Kits\10",
                @"C:\Program Files\Windows Kits\10",
            };
            foreach (var s in sdkRoots)
            {
                if (Directory.Exists(s))
                {
                    sdkDir = s;
                    break;
                }
            }
        }

        if (sdkDir != null)
        {
            var sdkVersion = Environment.GetEnvironmentVariable("WindowsSdkVersion");
            string? sdkVerDir = null;
            if (!string.IsNullOrEmpty(sdkVersion))
            {
                sdkVerDir = Path.Combine(sdkDir, "Include", sdkVersion.TrimEnd('\\'));
                if (!Directory.Exists(sdkVerDir))
                    sdkVerDir = null;
            }

            // Fallback: probe for latest SDK version
            if (sdkVerDir == null)
            {
                var includeDir = Path.Combine(sdkDir, "Include");
                if (Directory.Exists(includeDir))
                {
                    var versions = Directory.GetDirectories(includeDir)
                        .Select(Path.GetFileName)
                        .OrderByDescending(v => v)
                        .ToList();
                    if (versions.Count > 0)
                        sdkVerDir = Path.Combine(includeDir, versions[0]);
                }
            }

            if (sdkVerDir != null)
            {
                sdkIncludeDirs = new List<string>();
                foreach (var sub in new[] { "ucrt", "um", "shared" })
                {
                    var candidate = Path.Combine(sdkVerDir, sub);
                    if (Directory.Exists(candidate))
                        sdkIncludeDirs.Add(candidate);
                }
            }
        }

        return (vcIncludeDir, sdkIncludeDirs);
    }
}
