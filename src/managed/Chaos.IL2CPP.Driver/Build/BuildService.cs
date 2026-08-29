using System.Diagnostics;

namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Configures and builds a native CMake project (TPG generate-dll output or a TPG native
/// project directory containing CMakeLists.txt). Mirrors CppProjectEmitter.GenerateAndBuild:
/// delete stale CMakeCache.txt/CMakeFiles, run `cmake -S <dir> -B <build>`, then
/// `cmake --build <build> --target <target>`.
/// </summary>
internal static class BuildService
{
    /// <summary>CMake target name of a TPG project built in pipeline mode (projectRoot + codegenDir set).</summary>
    public const string ChaosEntryTarget = "chaos_entry";

    /// <summary>CMake target name of a standalone entry project (non-pipeline).</summary>
    public const string EntryTarget = "entry";

    public sealed record BuildResult
    {
        public bool Success { get; init; }
        public int ExitCode { get; init; }
        public string? Output { get; init; }
        public string? Error { get; init; }
    }

    /// <summary>
    /// Configure and build a CMake project. Aligned with CppProjectEmitter.GenerateAndBuild's
    /// configure + build sequence: stale cache is removed, `cmake -S <projectDir> -B <buildDir>`
    /// runs with the platform-default generator, then `cmake --build <buildDir> --target <target>`.
    /// </summary>
    public static BuildResult ConfigureAndBuild(
        string projectDir,
        string? target,
        string configTier = "check",
        string? generator = null,
        string? platform = null)
    {
        if (!File.Exists(Path.Combine(projectDir, "CMakeLists.txt")))
        {
            return new BuildResult
            {
                Success = false,
                ExitCode = -1,
                Error = $"No CMakeLists.txt in directory: {projectDir}. " +
                        "This command expects a native project produced by TPG generate-dll " +
                        "(or publish --source-only), not a convert output directory.",
            };
        }

        if (string.IsNullOrWhiteSpace(target))
            target = ChaosEntryTarget;

        var buildDir = Path.Combine(projectDir, "build");
        var cmakeArgs = new List<string>();

        try
        {
            // ── Step 1: CMake configure (fresh) ──
            Directory.CreateDirectory(buildDir);

            // Remove stale cache to prevent "generator platform: x64 does not match platform used previously"
            var oldCache = new FileInfo(Path.Combine(buildDir, "CMakeCache.txt"));
            if (oldCache.Exists)
            {
                try { oldCache.Delete(); }
                catch { /* best-effort */ }
            }

            var oldCmakeFiles = new DirectoryInfo(Path.Combine(buildDir, "CMakeFiles"));
            if (oldCmakeFiles.Exists)
            {
                try { oldCmakeFiles.Delete(recursive: true); }
                catch { /* best-effort */ }
            }

            cmakeArgs.Add("-S");
            cmakeArgs.Add(projectDir);
            cmakeArgs.Add("-B");
            cmakeArgs.Add(buildDir);

            cmakeArgs.Add($"-DCHAOS_IL2CPP_CONFIG_TIER={configTier.ToLowerInvariant()}");

            if (!string.IsNullOrWhiteSpace(generator))
            {
                cmakeArgs.Add("-G");
                cmakeArgs.Add(generator);
            }
            else if (OperatingSystem.IsWindows())
            {
                cmakeArgs.Add("-G");
                cmakeArgs.Add("Visual Studio 17 2022");
                cmakeArgs.Add("-A");
                cmakeArgs.Add(string.IsNullOrWhiteSpace(platform) ? "x64" : platform!);
            }
            else
            {
                cmakeArgs.Add("-G");
                cmakeArgs.Add("Unix Makefiles");
            }

            var configureResult = RunProcess("cmake", cmakeArgs, timeoutMs: 120_000);
            if (!configureResult.Success)
                return configureResult;

            // ── Step 2: CMake build ──
            var buildArgs = new List<string>
            {
                "--build", buildDir,
                "--config", "RelWithDebInfo",
                "--target", target,
            };
            return RunProcess("cmake", buildArgs, timeoutMs: 1_800_000);
        }
        catch (System.ComponentModel.Win32Exception)
        {
            return new BuildResult { Success = false, ExitCode = -1, Error = "cmake not found. Ensure CMake is installed and on PATH." };
        }
        catch (Exception ex)
        {
            return new BuildResult { Success = false, ExitCode = -1, Error = ex.Message };
        }
    }

    /// <summary>
    /// Legacy compat: RunBuild now delegates to ConfigureAndBuild, preserving the old
    /// signature so existing callers still compile. The "buildDir" parameter name is kept
    /// but is now the project directory (containing CMakeLists.txt), and the default
    /// target is chaos_entry (not the old preset name "windows-x64-reference").
    /// </summary>
    public static BuildResult RunBuild(string buildDir, string? targetId)
    {
        return ConfigureAndBuild(buildDir, targetId ?? ChaosEntryTarget);
    }

    private static BuildResult RunProcess(string fileName, IEnumerable<string> arguments, int timeoutMs)
    {
        try
        {
            var psi = new ProcessStartInfo(fileName)
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
            };
            foreach (var arg in arguments)
                psi.ArgumentList.Add(arg);

            using var process = Process.Start(psi);
            if (process is null)
                return new BuildResult { Success = false, ExitCode = -1, Error = $"Failed to start {fileName} process." };

            var stdout = process.StandardOutput.ReadToEnd();
            var stderr = process.StandardError.ReadToEnd();
            process.WaitForExit();

            if (!process.HasExited)
                process.Kill(entireProcessTree: true);

            return new BuildResult
            {
                Success = process.ExitCode == 0,
                ExitCode = process.ExitCode,
                Output = stdout,
                Error = stderr,
            };
        }
        catch (System.ComponentModel.Win32Exception)
        {
            return new BuildResult { Success = false, ExitCode = -1, Error = $"{fileName} not found. Ensure {fileName} is installed and on PATH." };
        }
    }
}
