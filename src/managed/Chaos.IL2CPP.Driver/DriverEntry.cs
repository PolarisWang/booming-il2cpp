using System.Text.Json;
using System.Text.Json.Serialization;
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
        WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.ClosureManifest), result.ClosureManifest);

        return 0;
    }

    public static int Main(string[] args)
    {
        if (args.Length != 2)
        {
            Console.Error.WriteLine("usage: Chaos.IL2CPP.Driver <input-assembly-path> <output-root>");
            return 1;
        }

        try
        {
            return new DriverEntry().Run(new ManagedClosureRequest(args[0], args[1]));
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine(exception);
            return 1;
        }
    }

    private static void WriteJson<T>(string path, T value)
    {
        var json = JsonSerializer.Serialize(value, JsonOptions);
        File.WriteAllText(path, json + Environment.NewLine);
    }
}
