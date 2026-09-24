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
        List<string>? subjectMethods = null,
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
            goldProfilePath: goldProfilePath,
            allManagedMethods: closureResult.AllManagedMethods);

        foreach (var source in emitResult.GeneratedSources)
        {
            var targetPath = Path.Combine(outputRoot, source.RelativePath.Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
            if (source.ContentsBuilder is { } builder)
            {
                // Strip every CR before it reaches disk.
                //
                // The builder's content is assembled with AppendLine-style calls
                // whose separator is Environment.NewLine — CRLF on Windows, LF on
                // Linux — and rendered template fragments embedded in it carry
                // their own newlines on top.  On Windows the layers compound to
                // \r\r\r\n per line (~3 CR/line measured; Linux emits pure LF).
                // MSVC reads each stray CR as a token break and rejects the file:
                //   native-aot.generated.cpp: error C2182 / C2365 / C2065 / C2143
                //
                // Normalizing here makes the emitted bytes platform-independent.
                // CR removal is exact because C++ source never legitimately
                // contains a bare CR.  Applied per chunk to keep allocations
                // bounded for the 200+ MB translation units.
                using var writer = new StreamWriter(targetPath, append: false, Encoding.UTF8);
                foreach (var chunk in builder.GetChunks())
                {
                    var text = chunk.ToString();
                    if (text.Contains('\r'))
                        text = text.Replace("\r", "");
                    writer.Write(text);
                }
            }
            else
            {
                File.WriteAllText(targetPath, source.Contents.Replace("\r", ""), Encoding.UTF8);
            }
        }

        // Emit cross-assembly export header (if non-empty)
        if (!string.IsNullOrEmpty(closureResult.CrossAssemblyExportHeader))
        {
            var exportHeaderPath = Path.Combine(outputRoot, "chaos_assembly_exports.h");
            File.WriteAllText(exportHeaderPath, closureResult.CrossAssemblyExportHeader.Replace("\r", ""), Encoding.UTF8);
            Console.WriteLine($"    emitted cross-assembly export header -> {exportHeaderPath}");
        }

        Console.WriteLine($"    emitted {emitResult.GeneratedSources.Count} files -> {outputRoot}");
        Console.WriteLine($"    ({emitResult.CodegenMetrics.TotalMethodCount} methods, " +
            $"{emitResult.CodegenMetrics.StructuredMethodCount} structured, " +
            $"{emitResult.CodegenMetrics.FlatFallbackCount} flat-fallback)");

        return emitResult;
    }
}
