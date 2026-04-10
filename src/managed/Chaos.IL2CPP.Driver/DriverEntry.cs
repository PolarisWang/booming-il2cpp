using System.Text.Json;
using System.Text.Json.Serialization;
using Chaos.IL2CPP.CodeGen;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Pipeline;

namespace Chaos.IL2CPP.Driver;

public sealed class DriverEntry
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
    };

    public string Name => "Chaos.IL2CPP.Driver";

    public int Run(ManagedClosureRequest request)
    {
        var pipeline = new PipelinePlan();
        var result = pipeline.Execute(request);

        Directory.CreateDirectory(result.OutputRootPath);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.TypedIlIr), result.TypedIlIr);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.AotManifest), result.AotManifest);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.MetadataRegistration), result.MetadataRegistration);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.CodeRegistration), result.CodeRegistration);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.OptimizationFacts), result.OptimizationFacts);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.NativeReferenceLoweringPlan), result.NativeReferenceLoweringPlan);
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.ClosureManifest), result.ClosureManifest);

        return 0;
    }

    public int Run(NativeReferenceProofRequest request)
    {
        var emitter = new NativeReferenceProofEmitter();
        var result = emitter.Generate(request);

        Directory.CreateDirectory(result.OutputRootPath);
        foreach (var generatedSource in result.GeneratedSources)
        {
            var targetPath = Path.Combine(result.OutputRootPath, generatedSource.RelativePath.Replace('/', Path.DirectorySeparatorChar));
            Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
            File.WriteAllText(targetPath, generatedSource.Contents);
        }

        WriteJson(Path.Combine(result.OutputRootPath, NativeReferenceArtifactNames.LoweringPlan), result.LoweringPlan);
        WriteJson(Path.Combine(result.OutputRootPath, NativeReferenceArtifactNames.Manifest), result.Manifest);
        return 0;
    }

    public static int Main(string[] args)
    {
        if (args.Length == 3 && string.Equals(args[0], "emit-native-reference", StringComparison.Ordinal))
        {
            try
            {
                return new DriverEntry().Run(new NativeReferenceProofRequest(args[1], args[2]));
            }
            catch (Exception exception)
            {
                Console.Error.WriteLine(exception);
                return 1;
            }
        }

        if (!TryParseManagedClosureRequest(args, out var request))
        {
            Console.Error.WriteLine("usage: Chaos.IL2CPP.Driver <input-assembly-path> <output-root>");
            Console.Error.WriteLine("   or: Chaos.IL2CPP.Driver <input-assembly-path> <output-root> --entry-point-subject-id <subject-id>");
            Console.Error.WriteLine("   or: Chaos.IL2CPP.Driver emit-native-reference <managed-closure-root> <output-root>");
            return 1;
        }

        try
        {
            return new DriverEntry().Run(request);
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine(exception);
            return 1;
        }
    }

    private static bool TryParseManagedClosureRequest(string[] args, out ManagedClosureRequest request)
    {
        request = default!;

        if (args.Length == 2)
        {
            request = new ManagedClosureRequest(args[0], args[1]);
            return true;
        }

        if (args.Length == 4 &&
            string.Equals(args[2], "--entry-point-subject-id", StringComparison.Ordinal) &&
            !string.IsNullOrWhiteSpace(args[3]))
        {
            request = new ManagedClosureRequest(
                args[0],
                args[1],
                EntryPointSubjectIdOverride: args[3]);
            return true;
        }

        return false;
    }

    private static void WriteJson<T>(string path, T value)
    {
        var json = JsonSerializer.Serialize(value, JsonOptions);
        File.WriteAllText(path, json + Environment.NewLine);
    }
}
