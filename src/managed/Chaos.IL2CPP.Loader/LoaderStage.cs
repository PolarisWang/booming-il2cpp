using System.Collections.Immutable;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

public sealed partial class LoaderStage
{
    private static readonly IReadOnlyDictionary<FieldBindingKey, string> EmptyFieldBindings =
        new Dictionary<FieldBindingKey, string>();

    public string Name => "Loader";

    public LoadedAssemblyModel Load(ManagedClosureRequest request)
    {
        return LoadAssembly(
            request.InputAssemblyPath,
            request.EntryPointSubjectIdOverride,
            requireEntryPoint: true);
    }

    public LoadedWorldModel LoadMultiple(ManagedClosureRequest request)
    {
        var assemblyPaths = new List<string> { request.InputAssemblyPath };
        if (request.AdditionalAssemblyPaths is not null)
        {
            foreach (var additionalAssemblyPath in request.AdditionalAssemblyPaths)
            {
                if (string.IsNullOrWhiteSpace(additionalAssemblyPath))
                {
                    continue;
                }

                if (!assemblyPaths.Contains(additionalAssemblyPath, StringComparer.OrdinalIgnoreCase))
                {
                    assemblyPaths.Add(additionalAssemblyPath);
                }
            }
        }

        var loadedAssemblies = assemblyPaths
            .Select(assemblyPath => LoadAssembly(
                assemblyPath,
                entryPointSubjectIdOverride: null,
                requireEntryPoint: false))
            .ToList();
        loadedAssemblies = MaterializeCrossAssemblyMethodInstantiations(assemblyPaths, loadedAssemblies);
        var entryAssembly = ResolveEntryAssembly(loadedAssemblies, request.EntryPointSubjectIdOverride);
        var entryPointSubjectId = !string.IsNullOrWhiteSpace(request.EntryPointSubjectIdOverride)
            ? request.EntryPointSubjectIdOverride!
            : entryAssembly.EntryPointSubjectId;

        return new LoadedWorldModel
        {
            InputAssemblyPath = request.InputAssemblyPath,
            Assembly = entryAssembly.Assembly,
            Assemblies = loadedAssemblies,
            EntryPointSubjectId = entryPointSubjectId,
            Types = loadedAssemblies.SelectMany(assembly => assembly.Types).OrderBy(model => model.MetadataToken).ToList(),
            Fields = loadedAssemblies.SelectMany(assembly => assembly.Fields).OrderBy(model => model.MetadataToken).ToList(),
            Properties = loadedAssemblies.SelectMany(assembly => assembly.Properties).OrderBy(model => model.MetadataToken).ToList(),
            Methods = loadedAssemblies.SelectMany(assembly => assembly.Methods).OrderBy(model => model.MetadataToken).ToList(),
        };
    }

}
