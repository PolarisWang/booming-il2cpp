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
    /// <summary>
    /// Run IL2CPP codegen on the given assemblies.
    /// </summary>
    /// <param name="assemblyPaths">Paths to managed DLLs to compile.</param>
    /// <param name="outputDir">Output directory for codegen artifacts (--sdk-out).</param>
    /// <param name="codegenMode">Codegen mode: "aot" or "jit".</param>
    public CodegenResult Run(IReadOnlyList<string> assemblyPaths, string outputDir, string codegenMode = "aot")
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

            if (codegenMode == "jit")
            {
                args.Add("--mode");
                args.Add("jit");
            }

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
            // Driver writes to sdkRoot/<assembly-name>/generated/ when --sdk-out is used
            var generatedDirs = new List<string>();
            if (Directory.Exists(outputDir))
            {
                // Per-assembly layout: <sdk-out>/<assembly>/generated/
                foreach (var subDir in Directory.GetDirectories(outputDir))
                {
                    var genDir = Path.Combine(subDir, "generated");
                    if (Directory.Exists(genDir) && Directory.GetFiles(genDir, "*.cpp").Length > 0)
                        generatedDirs.Add(genDir);
                }
                // Flat layout fallback: <sdk-out>/generated/
                var flatGen = Path.Combine(outputDir, "generated");
                if (generatedDirs.Count == 0 && Directory.Exists(flatGen))
                    generatedDirs.Add(flatGen);
            }

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
