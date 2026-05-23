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

    public PipelineResult<LoadedWorldModel> LoadMultiple(ManagedClosureRequest request)
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

        List<LoadedAssemblyModel> loadedAssemblies;
        try
        {
            loadedAssemblies = assemblyPaths
                .Select(assemblyPath => LoadAssembly(
                    assemblyPath,
                    entryPointSubjectIdOverride: null,
                    requireEntryPoint: false))
                .ToList();
        }
        catch (Exception ex)
        {
            return PipelineResult<LoadedWorldModel>.Fail("LOADER_LOAD_FAILED",
                $"Failed to load assemblies: {ex.Message}", ex);
        }

        loadedAssemblies = ProjectCrossAssemblyMethodInstantiations(assemblyPaths, loadedAssemblies);

        string? resolvedEntryPointSubjectIdOverride;
        try
        {
            resolvedEntryPointSubjectIdOverride = ResolveEntryPointSubjectIdOverride(
                loadedAssemblies, request.EntryPointSubjectIdOverride);
        }
        catch (InvalidOperationException ex)
        {
            return PipelineResult<LoadedWorldModel>.Fail("LOADER_ENTRY_POINT_NOT_FOUND",
                ex.Message, ex);
        }

        var entryAssembly = ResolveEntryAssembly(loadedAssemblies, resolvedEntryPointSubjectIdOverride, request.FullAssemblyClosure);
        var entryPointSubjectId = !string.IsNullOrWhiteSpace(resolvedEntryPointSubjectIdOverride)
            ? resolvedEntryPointSubjectIdOverride!
            : request.FullAssemblyClosure
                ? string.Empty
                : entryAssembly.EntryPointSubjectId;

        return PipelineResult<LoadedWorldModel>.Ok(new LoadedWorldModel
        {
            InputAssemblyPath = request.InputAssemblyPath,
            FullAssemblyClosure = request.FullAssemblyClosure,
            Assembly = entryAssembly.Assembly,
            Assemblies = loadedAssemblies,
            EntryPointSubjectId = entryPointSubjectId,
            GenericInstantiationDemandGraph = MergeGenericInstantiationDemandGraphs(loadedAssemblies),
            Types = loadedAssemblies.SelectMany(assembly => assembly.Types).OrderBy(model => model.MetadataToken).ToList(),
            Fields = loadedAssemblies.SelectMany(assembly => assembly.Fields).OrderBy(model => model.MetadataToken).ToList(),
            Properties = loadedAssemblies.SelectMany(assembly => assembly.Properties).OrderBy(model => model.MetadataToken).ToList(),
            Methods = loadedAssemblies.SelectMany(assembly => assembly.Methods).OrderBy(model => model.MetadataToken).ToList(),
        });
    }

    private static string? ResolveEntryPointSubjectIdOverride(
        IReadOnlyList<LoadedAssemblyModel> loadedAssemblies,
        string? entryPointSubjectIdOverride)
    {
        if (string.IsNullOrWhiteSpace(entryPointSubjectIdOverride))
        {
            return null;
        }

        var matchedSubjectId = loadedAssemblies
            .SelectMany(assembly => assembly.Methods)
            .FirstOrDefault(method => ManagedNaming.MatchesMethodSubjectId(method.SubjectId, entryPointSubjectIdOverride!))
            ?.SubjectId;
        if (!string.IsNullOrWhiteSpace(matchedSubjectId))
        {
            return matchedSubjectId;
        }

        throw new InvalidOperationException(
            $"managed closure entry point override '{entryPointSubjectIdOverride}' does not match any loaded method");
    }

}
