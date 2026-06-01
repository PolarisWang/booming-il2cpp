using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Chaos.IL2CPP.Generator.BuildSystem;

/// <summary>
/// Linux native platform — builds .a files from source via cmake since there
/// are no prebuilt CI artifacts on Linux.
/// </summary>
public sealed class LinuxNativePlatform : INativePlatform
{
    public string StaticLibPrefix => "lib";
    public string StaticLibSuffix => ".a";
    public bool HasPrebuiltLibraries => false;

    public string GetStaticLibFileName(string baseName) => $"lib{baseName}.a";

    public bool BuildNativeLibs(string repoRoot, string libDir, string configTier)
    {
        if (!Directory.Exists(repoRoot))
        {
            Console.Error.WriteLine($"    [Linux] repo root not found: {repoRoot}");
            return false;
        }

        Console.WriteLine("    [Linux] Building native runtime libraries from source...");

        var buildDir = Path.Combine(libDir, "..", ".native_build");
        Directory.CreateDirectory(buildDir);

        // ── Write wrapper CMakeLists.txt ──────────────────────────────────
        // Native subdirectories in dependency order: each must define its
        // dependencies before the dependent is added.
        var cmakeContent = CreateWrapperCMakeLists(repoRoot, buildDir, configTier);
        File.WriteAllText(Path.Combine(buildDir, "CMakeLists.txt"), cmakeContent);

        // ── Run cmake configure ───────────────────────────────────────────
        var generator = FindNinja() ? "Ninja" : "Unix Makefiles";
        var outputDir = Path.Combine(buildDir, "output");
        Directory.CreateDirectory(outputDir);

        var configureArgs = $"-G \"{generator}\" -B \"{outputDir}\" -S \"{buildDir}\"";
        if (!RunProcess("cmake", configureArgs))
        {
            Console.Error.WriteLine("    [Linux] cmake configure failed — native libs will be empty stubs");
            return false;
        }

        // ── Run cmake build ────────────────────────────────────────────────
        if (!RunProcess("cmake", $"--build \"{outputDir}\" -j{Environment.ProcessorCount}"))
        {
            Console.Error.WriteLine("    [Linux] cmake build failed — native libs may be incomplete");
            // Continue anyway — copy what was built
        }

        // ── Copy .a files to SDK lib/ ──────────────────────────────────────
        var libsToBuild = new (string BuildSubdir, string LibName)[]
        {
            ("fmt", "libchaos_fmt.a"),
            ("common", "libchaos_common.a"),
            ("support", "libchaos_support.a"),
            ("interpreter", "libchaos_interpreter.a"),
            ("runtime_core", "libchaos_runtime_core.a"),
            ("bootstrap", "libchaos_bootstrap.a"),
            ("hot_update", "libchaos_hot_update.a"),
            ("jit", "libchaos_jit.a"),
        };

        int copiedCount = 0;
        foreach (var (subdir, libName) in libsToBuild)
        {
            var srcPath = Path.Combine(outputDir, subdir, libName);
            if (File.Exists(srcPath))
            {
                var dstPath = Path.Combine(libDir, libName);
                File.Copy(srcPath, dstPath, overwrite: true);
                Console.WriteLine($"    SDK lib (built): {libName}");
                copiedCount++;
            }
            else
            {
                Console.WriteLine($"    WARN: {libName} not built (not found at {srcPath})");
            }
        }

        Console.WriteLine($"    SDK libs: {copiedCount} built from source, {libsToBuild.Length - copiedCount} missing");
        return copiedCount > 0;
    }

    private static string CreateWrapperCMakeLists(string repoRoot, string buildDir, string configTier)
    {
        // Escape backslashes in paths for CMake
        var fmtSrc = repoRoot + "/third_party/fmt";
        var unorderedDenseSrc = repoRoot + "/third_party/unordered_dense";
        var commonSrc = repoRoot + "/src/native/common";
        var supportSrc = repoRoot + "/src/native/support";
        var interpreterSrc = repoRoot + "/src/native/interpreter";
        var runtimeCoreSrc = repoRoot + "/src/native/runtime-core";
        var bootstrapSrc = repoRoot + "/src/native/bootstrap";
        var hotUpdateSrc = repoRoot + "/src/native/hot-update";
        var jitSrc = repoRoot + "/src/native/jit";

        var outputDir = Path.Combine(buildDir, "output");

        return $"""
cmake_minimum_required(VERSION 3.20)
project(chaos_native_libs CXX)
set(CMAKE_CXX_STANDARD 20)

# Config tier: controlled by pipeline --native-config
set(CHAOS_IL2CPP_CONFIG_TIER "{configTier}" CACHE STRING "Build config tier")
if(CHAOS_IL2CPP_CONFIG_TIER STREQUAL "profile")
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_PROFILE)
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_PROFILE)
  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=2)
elseif(CHAOS_IL2CPP_CONFIG_TIER STREQUAL "ship")
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_SHIP)
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_SHIP)
  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=0)
else()
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_TIER=CHAOS_IL2CPP_CONFIG_TIER_CHECK)
  add_compile_definitions(CHAOS_IL2CPP_CONFIG_CHECK)
  add_compile_definitions(CHAOS_IL2CPP_LOG_LEVEL=3)
endif()

# Native libraries in dependency order:
#   fmt → unordered_dense (header-only) → common → interpreter
#   → runtime-core → support → hot-update → jit → bootstrap
add_subdirectory("{fmtSrc}" "{outputDir}/fmt")
add_subdirectory("{unorderedDenseSrc}" "{outputDir}/unordered_dense")
add_subdirectory("{commonSrc}" "{outputDir}/common")
add_subdirectory("{interpreterSrc}" "{outputDir}/interpreter")
add_subdirectory("{runtimeCoreSrc}" "{outputDir}/runtime_core")
add_subdirectory("{supportSrc}" "{outputDir}/support")
add_subdirectory("{hotUpdateSrc}" "{outputDir}/hot_update")
add_subdirectory("{jitSrc}" "{outputDir}/jit")
add_subdirectory("{bootstrapSrc}" "{outputDir}/bootstrap")
""";
    }

    private static bool FindNinja()
    {
        try
        {
            var psi = new ProcessStartInfo("ninja", "--version")
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
            };
            using var proc = Process.Start(psi);
            if (proc == null) return false;
            proc.WaitForExit(5000);
            return proc.ExitCode == 0;
        }
        catch
        {
            return false;
        }
    }

    private static bool RunProcess(string executable, string arguments)
    {
        try
        {
            var psi = new ProcessStartInfo(executable, arguments)
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
            };
            using var proc = Process.Start(psi);
            if (proc == null) return false;

            // Read both streams to avoid deadlock
            var outputTask = proc.StandardOutput.ReadToEndAsync();
            var errorTask = proc.StandardError.ReadToEndAsync();

            if (!proc.WaitForExit(300_000)) // 5 min timeout
            {
                try { proc.Kill(); } catch { }
                return false;
            }

            var output = outputTask.GetAwaiter().GetResult();
            var error = errorTask.GetAwaiter().GetResult();

            if (proc.ExitCode != 0)
            {
                Console.Error.WriteLine($"    [Linux] {executable} failed (exit={proc.ExitCode}):");
                foreach (var line in error.Split('\n', StringSplitOptions.RemoveEmptyEntries))
                    Console.Error.WriteLine($"      {line.Trim()}");
                return false;
            }

            return true;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"    [Linux] failed to run {executable}: {ex.Message}");
            return false;
        }
    }
}
