using System.Text;
using Chaos.IL2CPP.Generator;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Diagnostics;

namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Full assembly C++ emitter that uses NativeAotLoweringPlanner directly from
/// in-memory pipeline artifacts — no JSON serialization/deserialization round-trip.
///
/// This is the "direct path" for convert-to-cpp: the pipeline produces
/// ManagedClosureResult with all artifacts in memory, and this emitter
/// passes them straight to the planner and writes the C++ output.
/// </summary>
internal sealed class FullAssemblyEmitter
{
    /// <summary>
    /// Emit C++ from in-memory pipeline artifacts.
    /// </summary>
    /// <param name="closureResult">The pipeline result with all artifacts in memory.</param>
    /// <param name="outputRoot">Root output directory for generated C++ files.</param>
    /// <param name="mode">Codegen mode (AOT or JIT).</param>
    /// <returns>The NativeAotResult with all generated sources and artifacts.</returns>
    public NativeAotResult Emit(
        ManagedClosureResult closureResult,
        string outputRoot,
        CodegenMode mode = CodegenMode.Aot)
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
            mode);

        // Write all generated sources to disk
        foreach (var source in emitResult.GeneratedSources)
        {
            var targetPath = Path.Combine(outputRoot, source.RelativePath.Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
            File.WriteAllText(targetPath, source.Contents, Encoding.UTF8);
        }

        Console.WriteLine($"    emitted {emitResult.GeneratedSources.Count} files → {outputRoot}");
        Console.WriteLine($"    ({emitResult.CodegenMetrics.TotalMethodCount} methods, " +
            $"{emitResult.CodegenMetrics.StructuredMethodCount} structured, " +
            $"{emitResult.CodegenMetrics.FlatFallbackCount} flat-fallback)");

        return emitResult;
    }
}
