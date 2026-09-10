using System.Diagnostics;

namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Runs cmake --build in a given directory, optionally for a specific target.
/// </summary>
internal static class BuildService
{
    public sealed record BuildResult
    {
        public bool Success { get; init; }
        public int ExitCode { get; init; }
        public string? Output { get; init; }
        public string? Error { get; init; }
    }

    public static BuildResult RunBuild(string buildDir, string? targetId)
    {
        try
        {
            var args = $"--build \"{buildDir}\"";
            if (!string.IsNullOrWhiteSpace(targetId))
                args += $" --target {targetId}";

            var psi = new ProcessStartInfo("cmake", args)
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
            };

            using var process = Process.Start(psi);
            if (process is null)
                return new BuildResult { Success = false, ExitCode = -1, Error = "Failed to start cmake process." };

            var stdout = process.StandardOutput.ReadToEnd();
            var stderr = process.StandardError.ReadToEnd();
            process.WaitForExit();

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
            return new BuildResult { Success = false, ExitCode = -1, Error = "cmake not found. Ensure CMake is installed and on PATH." };
        }
    }
}
