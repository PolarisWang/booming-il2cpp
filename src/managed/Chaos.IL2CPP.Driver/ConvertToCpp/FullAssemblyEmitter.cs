using System.Collections.Generic;
using System.IO;
using System.Text;
using Chaos.IL2CPP.Generator;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Driver;

internal sealed class FullAssemblyEmitter
{
    public NativeAotResult Emit(
        ManagedClosureResult closureResult,
        string outputRoot,
        CodegenMode mode = CodegenMode.Aot,
        HashSet<string>? subjectMethods = null,
        string? goldProfilePath = null)
    {
        ArgumentNullException.ThrowIfNull(closureResult);

        var nativeAotEmitter = new NativeAotEmitter();
        var emitResult = nativeAotEmitter.GenerateFromArtifacts(
            closureResult.NativeAotLoweringPlan,
            closureResult.AotCoreIr,
            closureResult.ClosureManifest,
            closureResult.MetadataRegistration,
            closureResult.SupplementalMetadataTemplate,
            outputRoot,
            mode,
            subjectMethods,
            goldProfilePath: goldProfilePath);

        foreach (var source in emitResult.GeneratedSources)
        {
            var targetPath = Path.Combine(outputRoot, source.RelativePath.Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
            if (source.ContentsBuilder is { } builder)
            {
                using var writer = new StreamWriter(targetPath, append: false, Encoding.UTF8);
                foreach (var chunk in builder.GetChunks())
                    writer.Write(chunk.Span);
            }
            else
            {
                File.WriteAllText(targetPath, source.Contents, Encoding.UTF8);
            }
        }

        Console.WriteLine($"    emitted {emitResult.GeneratedSources.Count} files -> {outputRoot}");
        Console.WriteLine($"    ({emitResult.CodegenMetrics.TotalMethodCount} methods, " +
            $"{emitResult.CodegenMetrics.StructuredMethodCount} structured, " +
            $"{emitResult.CodegenMetrics.FlatFallbackCount} flat-fallback)");

        return emitResult;
    }
}
