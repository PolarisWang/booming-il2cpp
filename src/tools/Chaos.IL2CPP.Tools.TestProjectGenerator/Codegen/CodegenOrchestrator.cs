using Chaos.IL2CPP.Driver;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Codegen;

public sealed class CodegenResult
{
    public bool Success { get; init; }
    public string OutputDir { get; init; } = "";
    public List<string> GeneratedDirs { get; init; } = [];
    public string? Error { get; init; }
}

public sealed class CodegenOrchestrator
{
    public CodegenResult Run(IReadOnlyList<string> assemblyPaths, string outputDir)
    {
        try
        {
            Directory.CreateDirectory(outputDir);

            // Build args for ConvertToCppHandler
            var args = new List<string>();
            foreach (var dll in assemblyPaths)
            {
                args.Add("--assembly");
                args.Add(Path.GetFullPath(dll));
            }

            var probeDirs = assemblyPaths
                .Select(Path.GetDirectoryName)
                .OfType<string>()
                .Distinct();

            foreach (var dir in probeDirs)
            {
                args.Add("--assembly-dir");
                args.Add(Path.GetFullPath(dir));
            }

            args.Add("--sdk-out");
            args.Add(Path.GetFullPath(outputDir));
            args.Add("--full-closure");

            // Run ConvertToCppHandler directly — output goes to Console
            var exitCode = ConvertToCppHandler.Run([.. args]);

            if (exitCode != 0)
            {
                return new CodegenResult
                {
                    Success = false,
                    OutputDir = outputDir,
                    Error = $"convert-to-cpp failed with exit code {exitCode}",
                };
            }

            // Collect generated directories
            // Driver writes to sdkRoot/generated/ when --sdk-out is used
            var generatedDirs = new List<string>();
            var generatedPath = Path.Combine(outputDir, "generated");
            if (Directory.Exists(generatedPath))
                generatedDirs.Add(generatedPath);

            return new CodegenResult
            {
                Success = true,
                OutputDir = outputDir,
                GeneratedDirs = generatedDirs,
            };
        }
        catch (Exception ex)
        {
            return new CodegenResult
            {
                Success = false,
                OutputDir = outputDir,
                Error = ex.Message,
            };
        }
    }
}
