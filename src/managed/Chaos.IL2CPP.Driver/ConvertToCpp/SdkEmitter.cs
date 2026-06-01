using System.Diagnostics;
using System.Runtime.InteropServices;
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
        string assemblyName,
        string configTier)
    {
        try
        {
            var includeDir = Path.Combine(sdkRoot, "include");
            var libDir = Path.Combine(sdkRoot, "lib");
            var cmakeDir = Path.Combine(sdkRoot, "cmake");

            Directory.CreateDirectory(includeDir);
            Directory.CreateDirectory(libDir);
            Directory.CreateDirectory(cmakeDir);

            // ── Platform abstraction ──────────────────────────────────────────
            var platform = NativePlatformFactory.Create();

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

            // ── Phase 1b": Copy runtime_stubs .cpp sources into SDK ─────────
            CopyRuntimeStubSources(repoRoot, sdkRoot);

            // ── Phase 1c: Generate chaos-config.cmake ────────────────────────
            var configModel = new ScriptObject
            {
                ["msvc_version"] = platform.HasPrebuiltLibraries ? DetectMsvcVersion() : null,
            };
            var configContent = SdkTemplateCatalog.RenderChaosConfig(configModel);
            File.WriteAllText(Path.Combine(sdkRoot, "chaos-config.cmake"), configContent);
            Console.WriteLine($"    SDK cmake: chaos-config.cmake");

            // ── Phase 1d: Generate chaos-targets.cmake ───────────────────────
            var targetsContent = SdkTemplateCatalog.RenderChaosTargets();
            File.WriteAllText(Path.Combine(cmakeDir, "chaos-targets.cmake"), targetsContent);
            Console.WriteLine($"    SDK cmake: chaos-targets.cmake");

            // ── Phase 1d': Generate CMakePresets.json ──────────────────────
            EmitCMakePresets(sdkRoot);

            // ── Phase 1e: Build native libs from source (if needed) ─────────
            // On Linux there are no prebuilt .a files; build them from the repo
            // source tree via cmake.  On Windows this is a no-op.
            platform.BuildNativeLibs(repoRoot, libDir, configTier);

            // ── Phase 1f: Copy prebuilt native runtime library files ───────
            CopyNativeLibs(nativeLibDir, buildConfig, libDir, platform);

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
    /// Searches generatedRoot/&lt;assembly&gt;/generated/ first, then generatedRoot/.
    /// After R1 fix, files land at single-level under generatedRoot (no double-nested generated/generated/).
    /// </summary>
    private static void CopyGeneratedHeader(string generatedRoot, string assemblyName, string includeDir, string fileName)
    {
        var candidates = new[]
        {
            Path.Combine(generatedRoot, assemblyName, "generated", fileName),
            Path.Combine(generatedRoot, fileName),
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
            // Transitive deps of runtime_core.h and generated_code_compat.h:
            "generated_code_compat.h", "string_table.h",
            "reflection_api.h", "reflection_metadata_impl.h",
            "arithmetic_chaos_bridge.h",
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

        // ── Copy ALL runtime_stubs/*.h ────────────────────────────────────
        // Needed by: generated_code_compat.h (via stubs.h), chaos_runtime_host.h
        // Use glob instead of maintainable-explicit-list (20+ files with transitive deps).
        if (Directory.Exists(srcRuntimeStubs))
        {
            foreach (var f in Directory.GetFiles(srcRuntimeStubs, "*.h"))
            {
                var name = Path.GetFileName(f);
                File.Copy(f, Path.Combine(dstRuntimeStubs, name), overwrite: true);
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

        // ── Copy reflection/*.h ──────────────────────────────────────────
        // Needed by: generated code that includes "reflection/hierarchy_fast_api.h"
        // (emitted when type hierarchy pointer folding is active).
        var srcReflection = Path.Combine(srcRuntimeCore, "reflection");
        var dstReflection = Path.Combine(includeDir, "reflection");
        Directory.CreateDirectory(dstReflection);
        if (Directory.Exists(srcReflection))
        {
            foreach (var f in Directory.GetFiles(srcReflection, "*.h"))
            {
                File.Copy(f, Path.Combine(dstReflection, Path.GetFileName(f)), overwrite: true);
                count++;
            }
        }

        // ── Copy contracts/*.h ────────────────────────────────────────────
        // Needed by: runtime_core.h, runtime_abi.h, module_registry.h
        var contractHeaders = new[]
        {
            "abi_manifest.h",
            "codegen_bridge.h",
            "patch_data.h",
            "register_ir.h",
            "runtime_abi.h",
            "runtime_instantiation.h",
            "unified_metadata.h",
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

        // ── Copy third_party/fmt/include/fmt/ ────────────────────────────
        // Needed by: chaos/log.h, chaos/format.h, chaos/trace.h — all include <fmt/format.h>
        var srcFmt = Path.Combine(repoRoot, "third_party", "fmt", "include", "fmt");
        var dstFmt = Path.Combine(includeDir, "fmt");
        Directory.CreateDirectory(dstFmt);
        if (Directory.Exists(srcFmt))
        {
            foreach (var f in Directory.GetFiles(srcFmt, "*.h"))
            {
                File.Copy(f, Path.Combine(dstFmt, Path.GetFileName(f)), overwrite: true);
                count++;
            }
        }

        Console.WriteLine($"    SDK runtime headers: {count} files copied");
    }

    /// <summary>
    /// Copy runtime_stubs .cpp source files into the SDK so the test framework
    /// can compile them directly instead of relying on a prebuilt (and possibly
    /// stale) chaos_runtime_core.lib.  The .h counterparts are already copied
    /// by CopyRuntimeHeaders into include/runtime_stubs/.
    ///
    /// Also copies profile_globals.cpp from src/native/common/chaos/ — it must
    /// be compiled from source (not linked from a lib) so PROFILE_SCOPE globals
    /// stay synchronized with the current build configuration.
    ///
    /// Maps: src/native/runtime-core/runtime_stubs/*.cpp → sdkRoot/runtime_stubs/
    ///       src/native/common/chaos/profile_globals.cpp  → sdkRoot/runtime_stubs/
    /// </summary>
    private static void CopyRuntimeStubSources(string repoRoot, string sdkRoot)
    {
        var srcRuntimeStubs = Path.Combine(repoRoot, "src", "native", "runtime-core", "runtime_stubs");
        var dstRuntimeStubs = Path.Combine(sdkRoot, "runtime_stubs");
        Directory.CreateDirectory(dstRuntimeStubs);

        int count = 0;
        if (Directory.Exists(srcRuntimeStubs))
        {
            foreach (var f in Directory.GetFiles(srcRuntimeStubs, "*.cpp"))
            {
                File.Copy(f, Path.Combine(dstRuntimeStubs, Path.GetFileName(f)), overwrite: true);
                count++;
            }
        }
        Console.WriteLine($"    SDK runtime stubs sources: {count} .cpp files copied");

        // ── Also copy profile_globals.cpp ────────────────────────────────
        // PROFILE_SCOPE globals must be compiled from source (not from prebuilt lib)
        // to stay synchronized with the current config tier and build flags.
        var profileGlobalsSrc = Path.Combine(repoRoot, "src", "native", "common", "chaos", "profile_globals.cpp");
        var profileGlobalsDst = Path.Combine(dstRuntimeStubs, "profile_globals.cpp");
        if (File.Exists(profileGlobalsSrc))
        {
            File.Copy(profileGlobalsSrc, profileGlobalsDst, overwrite: true);
            Console.WriteLine($"    SDK runtime stubs: profile_globals.cpp copied");
        }
        else
        {
            Console.WriteLine($"    WARN: profile_globals.cpp not found at {profileGlobalsSrc}");
        }
    }

    /// <summary>
    /// Copy prebuilt native runtime library files from the native build output
    /// directory into the SDK's lib/ directory.
    /// On Windows: source files use .lib extension in windows-x64-reference preset.
    /// On Linux: source files use .a extension with lib prefix in linux-debug preset.
    /// Creates empty stub archives for Windows-only libraries so CMake validation passes.
    /// </summary>
    private static void CopyNativeLibs(string nativeLibDir, string buildConfig, string libDir, INativePlatform platform)
    {
        // Library source paths (relative to nativeLibDir).
        // Build config: Windows uses MSVC multi-config (RelWithDebInfo), Linux uses single-config preset.
        var libMappings = new (string SourceRelDir, string TargetName)[]
        {
            ("src/native/runtime-core",  "chaos_runtime_core"),
            ("src/native/bootstrap",     "chaos_bootstrap"),
            ("src/native/common",        "chaos_common"),
            ("src/native/support",       "chaos_support"),
            ("src/native/interpreter",   "chaos_interpreter"),
            ("fmt_build",                "chaos_fmt"),
            ("src/native/hot-update",    "chaos_hot_update"),
            ("src/native/diagnostics/eventpipe", "chaos_eventpipe"),
            ("src/native/jit",           "chaos_jit"),
            ("src/native/diagnostics/debugger",  "chaos_debugger"),
        };

        int copiedCount = 0;
        int stubCount = 0;
        foreach (var (sourceRelDir, targetName) in libMappings)
        {
            var targetPath = Path.Combine(libDir, platform.GetStaticLibFileName(targetName));

            // Skip if the target already exists (e.g. built from source on Linux)
            if (File.Exists(targetPath))
            {
                copiedCount++;
                continue;
            }

            if (platform.HasPrebuiltLibraries)
            {
                // Windows: find the .lib in the prebuilt CI output directory.
                var sourcePath = Path.Combine(nativeLibDir, sourceRelDir.Replace('/', Path.DirectorySeparatorChar),
                    buildConfig, targetName + ".lib");
                if (File.Exists(sourcePath))
                {
                    File.Copy(sourcePath, targetPath, overwrite: true);
                    copiedCount++;
                    continue;
                }
            }
            else
            {
                // Linux: try to find a prebuilt .a (fallback if source build didn't produce it).
                var sourcePath = Path.Combine(nativeLibDir, sourceRelDir.Replace('/', Path.DirectorySeparatorChar),
                    "lib" + targetName + ".a");
                if (File.Exists(sourcePath))
                {
                    File.Copy(sourcePath, targetPath, overwrite: true);
                    copiedCount++;
                    continue;
                }
                var altPath = Path.Combine(nativeLibDir, sourceRelDir.Replace('/', Path.DirectorySeparatorChar),
                    targetName + ".a");
                if (File.Exists(altPath))
                {
                    File.Copy(altPath, targetPath, overwrite: true);
                    copiedCount++;
                    continue;
                }
            }

            // Library not found — create empty stub archive
            CreateEmptyArchive(targetPath);
            stubCount++;
        }

        Console.WriteLine($"    SDK libs: {copiedCount} copied, {stubCount} stubs");
    }

    /// <summary>
    /// Create an empty static library archive file.
    /// The linker accepts an empty archive (finds no symbols, doesn't error).
    /// </summary>
    private static void CreateEmptyArchive(string path)
    {
        var dir = Path.GetDirectoryName(path);
        if (!string.IsNullOrEmpty(dir))
            Directory.CreateDirectory(dir);
        // ar(1) format: magic string "!&lt;arch&gt;\n" followed by empty symbol table
        File.WriteAllBytes(path, "!<arch>\n"u8.ToArray());
        Console.WriteLine($"    SDK lib stub: {Path.GetFileName(path)}");
    }

    /// <summary>
    /// Generate CMakePresets.json for the entry executable build.
    /// On Windows: Visual Studio 17 2022 (multi-config, x64).
    /// On Linux: Ninja (single-config, CMAKE_BUILD_TYPE=RelWithDebInfo).
    /// The consumer (pipeline_native_aot_runner.py) runs cmake --preset default
    /// instead of hardcoding generator/platform/config flags.
    /// </summary>
    private static void EmitCMakePresets(string sdkRoot)
    {
        var isWindows = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);

        string presetJson;
        if (isWindows)
        {
            presetJson = """
{
  "version": 6,
  "configurePresets": [
    {
      "name": "default",
      "generator": "Visual Studio 17 2022",
      "architecture": "x64",
      "binaryDir": "${sourceDir}/build"
    }
  ],
  "buildPresets": [
    {
      "name": "default",
      "configurePreset": "default"
    }
  ]
}
""";
        }
        else
        {
            presetJson = """
{
  "version": 6,
  "configurePresets": [
    {
      "name": "default",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "RelWithDebInfo"
      }
    }
  ],
  "buildPresets": [
    {
      "name": "default",
      "configurePreset": "default"
    }
  ]
}
""";
        }

        File.WriteAllText(Path.Combine(sdkRoot, "CMakePresets.json"), presetJson);
        Console.WriteLine("    SDK cmake: CMakePresets.json");
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

        // Skip precompilation if the .lib is already up-to-date
        if (File.Exists(libPath) && File.Exists(generatedCpp))
        {
            var libTime = File.GetLastWriteTimeUtc(libPath);
            var srcTime = File.GetLastWriteTimeUtc(generatedCpp);
            if (libTime >= srcTime)
            {
                Console.WriteLine($"    [precompile] up-to-date: {libPath}");
                return;
            }
        }

        // Use temp paths so partial compilation failures don't overwrite the
        // existing .lib or leave a half-written file (Risk R4).
        var tmpObjPath = Path.Combine(libDir, "chaos_codegen.tmp.obj");
        var tmpLibPath = Path.Combine(libDir, "chaos_codegen.tmp.lib");

        // Step 1: Compile .cpp → .obj (to temp path)
        // Use /EHsc (not /EHa) for precompiled lib since generated code uses
        // C++ exception handling (throw chaos_managed_exception). The runtime
        // entry point that catches these needs /EHa, but the generated method
        // bodies themselves only throw, not catch.
        var compileArgs = $"/nologo /std:c++20 /utf-8 /EHsc /GS- /DWIN32 /D_WINDOWS " +
                           $"{includeArgs} /c \"{generatedCpp}\" /Fo\"{tmpObjPath}\"";

        Console.WriteLine($"    [precompile] compiling...");

        var compileOut = RunProcessWithOutputCapture(clPath, compileArgs);
        if (compileOut == null)
        {
            Console.WriteLine("    WARN: failed to start cl.exe, skipping precompilation");
            return;
        }

        if (!File.Exists(tmpObjPath))
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
            // Clean up temp .obj on failure
            try { File.Delete(tmpObjPath); } catch { }
            return;
        }

        var objSize = new FileInfo(tmpObjPath).Length;
        Console.WriteLine($"    [precompile] compiled: {objSize} bytes -> {tmpObjPath}");

        // Step 2: Bundle .obj → .lib (to temp path for atomic rename)
        var libArgs = $"/nologo /out:\"{tmpLibPath}\" \"{tmpObjPath}\"";

        Console.WriteLine($"    [precompile] creating library...");

        var libResult = RunProcessWithOutputCapture(libExePath, libArgs);
        if (libResult == null)
        {
            Console.WriteLine("    WARN: failed to start lib.exe, skipping library creation");
            return;
        }

        if (File.Exists(tmpLibPath))
        {
            var libSize = new FileInfo(tmpLibPath).Length;
            // Atomic rename: only replace the final .lib if the temp .lib was
            // fully written (Risk R4: partial compilation leaves no stale file).
            try
            {
                if (File.Exists(libPath))
                    File.Delete(libPath);
                File.Move(tmpLibPath, libPath);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"    WARN: atomic rename failed: {ex.Message}");
                return;
            }
            Console.WriteLine($"    [precompile] chaos_codegen.lib: {libSize} bytes");
            // Clean up intermediate .obj
            try { File.Delete(tmpObjPath); } catch { }
        }
        else
        {
            Console.WriteLine("    WARN: chaos_codegen.lib not created");
            foreach (var line in libResult.Value.Error.Trim().Split('\n', StringSplitOptions.RemoveEmptyEntries))
                Console.WriteLine($"      {line.Trim()}");
            // Clean up temp files on failure
            try { File.Delete(tmpObjPath); } catch { }
            try { File.Delete(tmpLibPath); } catch { }
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

    /// <summary>
    /// Detect the MSVC toolchain version for recording in chaos-config.cmake.
    /// Checks VCToolsVersion env var first (set by vcvarsall.bat), then
    /// falls back to parsing the version from cl.exe's output.
    /// Returns "unknown" if detection fails.
    /// </summary>
    private static string DetectMsvcVersion()
    {
        // 1. VCToolsVersion environment variable (set by vcvarsall.bat)
        var vcVersion = Environment.GetEnvironmentVariable("VCToolsVersion");
        if (!string.IsNullOrEmpty(vcVersion))
            return vcVersion;

        // 2. From VCToolsInstallDir path
        var vcToolsDir = Environment.GetEnvironmentVariable("VCToolsInstallDir");
        if (!string.IsNullOrEmpty(vcToolsDir))
        {
            // Path is typically .../MSVC/14.3x.xxxxx/bin/Hostx64/x64/
            var di = new DirectoryInfo(vcToolsDir);
            var msvcDir = di.Parent?.Parent?.Parent;  // up 3 levels to .../MSVC/<version>/
            if (msvcDir != null && msvcDir.Exists && msvcDir.Parent?.Name == "MSVC")
                return msvcDir.Name;
        }

        // 3. From cl.exe --version output
        try
        {
            var clPath = FindMsvcCompiler("cl.exe");
            if (clPath != null)
            {
                var result = RunProcessWithOutputCapture(clPath, "");
                if (result != null)
                {
                    // cl.exe prints version banner to stderr when run without input.
                    // Both streams are captured by RunProcessWithOutputCapture.
                    var output = result.Value.Output + result.Value.Error;
                    // MSVC prints: "Microsoft (R) C/C++ Optimizing Compiler Version 19.3x.xxxxx for x64"
                    var match = System.Text.RegularExpressions.Regex.Match(
                        output, @"Version\s+(\S+)");
                    if (match.Success)
                        return match.Groups[1].Value;
                }
            }
        }
        catch
        {
            // Ignore detection failures
        }

        return "unknown";
    }
}
