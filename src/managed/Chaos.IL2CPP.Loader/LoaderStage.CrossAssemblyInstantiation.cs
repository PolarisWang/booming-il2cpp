using System.Collections.Immutable;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

public sealed partial class LoaderStage
{
    private static List<LoadedAssemblyModel> ProjectCrossAssemblyMethodInstantiations(
        IReadOnlyList<string> assemblyPaths,
        IReadOnlyList<LoadedAssemblyModel> loadedAssemblies)
    {
        // Use Last-wins dedup: when --assembly-dir provides the same assembly that
        // closure analysis also discovers, prefer whichever was loaded.  ToDictionary
        // would throw "An item with the same key has already been added."
        static Dictionary<string, TValue> BuildAssemblyDict<TValue>(
            IReadOnlyList<LoadedAssemblyModel> assemblies,
            Func<LoadedAssemblyModel, TValue> valueSelector)
        {
            var dict = new Dictionary<string, TValue>(assemblies.Count, StringComparer.Ordinal);
            foreach (var a in assemblies)
            {
                var name = a.Assembly.Name;
                if (string.IsNullOrEmpty(name)) continue;
                dict[name] = valueSelector(a);  // Last wins — harmless if identical
            }
            return dict;
        }

        var genericInstantiationDemandEntriesByAssembly = BuildAssemblyDict(
            loadedAssemblies,
            _ => new Dictionary<string, GenericInstantiationDemandModel>(StringComparer.Ordinal));
        var definitionMethodsByAssembly = BuildAssemblyDict(
            loadedAssemblies,
            assembly => (IReadOnlyDictionary<string, ManagedMethodModel>)assembly.Methods
                .ToDictionary(method => method.SubjectId, StringComparer.Ordinal));
        var projectedMethodsByAssembly = BuildAssemblyDict(
            loadedAssemblies,
            assembly => assembly.Methods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal));
        var valueTypeSubjectIdsByAssembly = BuildAssemblyDict(
            loadedAssemblies,
            assembly => (IReadOnlySet<string>)assembly.Types
                .Where(t => t.IsValueType && !string.IsNullOrEmpty(t.SubjectId))
                .Select(t => t.SubjectId)
                .ToHashSet(StringComparer.Ordinal));

        foreach (var assemblyPath in assemblyPaths)
        {
            CollectProjectedCrossAssemblyMethodInstantiations(
                assemblyPath,
                definitionMethodsByAssembly,
                projectedMethodsByAssembly,
                genericInstantiationDemandEntriesByAssembly,
                valueTypeSubjectIdsByAssembly);
        }

        return loadedAssemblies
            .Select(assembly =>
            {
                var mergedDemandGraph = MergeGenericInstantiationDemandGraphs(
                    assembly.GenericInstantiationDemandGraph,
                    BuildGenericInstantiationDemandGraph(genericInstantiationDemandEntriesByAssembly[assembly.Assembly.Name]));
                var projectedMethods = projectedMethodsByAssembly[assembly.Assembly.Name].Values
                    .OrderBy(model => model.MetadataToken)
                    .ThenBy(model => model.SubjectId, StringComparer.Ordinal)
                    .ToList();

                return assembly with
                {
                    Methods = ApplyDemandDerivedRuntimeGenericContexts(projectedMethods, mergedDemandGraph),
                    GenericInstantiationDemandGraph = mergedDemandGraph,
                };
            })
            .ToList();
    }

    private static void CollectProjectedCrossAssemblyMethodInstantiations(
        string assemblyPath,
        IReadOnlyDictionary<string, IReadOnlyDictionary<string, ManagedMethodModel>> definitionMethodsByAssembly,
        IReadOnlyDictionary<string, Dictionary<string, ManagedMethodModel>> projectedMethodsByAssembly,
        IReadOnlyDictionary<string, Dictionary<string, GenericInstantiationDemandModel>> genericInstantiationDemandEntriesByAssembly,
        IReadOnlyDictionary<string, IReadOnlySet<string>> valueTypeSubjectIdsByAssembly)
    {
        using var stream = File.OpenRead(assemblyPath);
        using var peReader = new PEReader(stream);
        var metadataReader = peReader.GetMetadataReader();
        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
        var typeResolver = new MetadataTypeResolver(metadataReader, assemblyName);
        var typeModels = LoadTypes(metadataReader, typeResolver, assemblyName);
        var ownerIndex = BuildOwnerIndex(metadataReader, typeResolver, typeModels);

        foreach (var memberReferenceHandle in metadataReader.MemberReferences)
        {
            var memberReference = metadataReader.GetMemberReference(memberReferenceHandle);
            if (memberReference.GetKind() != MemberReferenceKind.Method)
            {
                continue;
            }

            TryProjectCrossAssemblyMethodInstantiation(
                assemblyName,
                DescribeMemberReferenceMethod(metadataReader, typeResolver, memberReferenceHandle),
                definitionMethodsByAssembly,
                projectedMethodsByAssembly,
                genericInstantiationDemandEntriesByAssembly,
                valueTypeSubjectIdsByAssembly,
                demandSourceKind: "memberReference");
        }

        for (var rowNumber = 1; rowNumber <= metadataReader.GetTableRowCount(TableIndex.MethodSpec); rowNumber++)
        {
            var methodSpecificationHandle = MetadataTokens.MethodSpecificationHandle(rowNumber);
            TryProjectCrossAssemblyMethodInstantiation(
                assemblyName,
                DescribeMethodSpecification(
                    metadataReader,
                    typeResolver,
                    typeModels,
                    ownerIndex.MethodOwners,
                    methodSpecificationHandle),
                definitionMethodsByAssembly,
                projectedMethodsByAssembly,
                genericInstantiationDemandEntriesByAssembly,
                valueTypeSubjectIdsByAssembly,
                demandSourceKind: "methodSpec");
        }
    }

    private static void TryProjectCrossAssemblyMethodInstantiation(
        string sourceAssemblyName,
        MethodReferenceSummary methodReference,
        IReadOnlyDictionary<string, IReadOnlyDictionary<string, ManagedMethodModel>> definitionMethodsByAssembly,
        IReadOnlyDictionary<string, Dictionary<string, ManagedMethodModel>> projectedMethodsByAssembly,
        IReadOnlyDictionary<string, Dictionary<string, GenericInstantiationDemandModel>> genericInstantiationDemandEntriesByAssembly,
        IReadOnlyDictionary<string, IReadOnlySet<string>> valueTypeSubjectIdsByAssembly,
        string demandSourceKind)
    {
        if (string.Equals(methodReference.AssemblyName, sourceAssemblyName, StringComparison.Ordinal) ||
            !definitionMethodsByAssembly.TryGetValue(methodReference.AssemblyName, out var targetDefinitionMethods) ||
            !projectedMethodsByAssembly.TryGetValue(methodReference.AssemblyName, out var targetProjectedMethods) ||
            !genericInstantiationDemandEntriesByAssembly.TryGetValue(methodReference.AssemblyName, out var targetDemandEntries) ||
            !valueTypeSubjectIdsByAssembly.TryGetValue(methodReference.AssemblyName, out var targetValueTypeSubjectIds))
        {
            return;
        }

        TrackMethodInstantiationDemand(
            genericInstantiationDemandEntries: targetDemandEntries,
            requestingAssemblyName: sourceAssemblyName,
            methodReference: methodReference,
            demandSourceKind: demandSourceKind,
            valueTypeSubjectIds: targetValueTypeSubjectIds);
        ProjectInstantiationMethod(
            methodReference.AssemblyName,
            methodReference,
            targetDefinitionMethods,
            EmptyFieldBindings,
            targetProjectedMethods);
    }

    private static LoadedAssemblyModel ResolveEntryAssembly(
        IReadOnlyList<LoadedAssemblyModel> loadedAssemblies,
        string? entryPointSubjectIdOverride,
        bool fullAssemblyClosure)
    {
        if (!string.IsNullOrWhiteSpace(entryPointSubjectIdOverride))
        {
            var entryAssembly = loadedAssemblies.FirstOrDefault(assembly =>
                assembly.Methods.Any(method => string.Equals(method.SubjectId, entryPointSubjectIdOverride, StringComparison.Ordinal)));
            if (entryAssembly is not null)
            {
                return entryAssembly;
            }

            throw new InvalidOperationException(
                $"managed closure entry point override '{entryPointSubjectIdOverride}' does not match any loaded method");
        }

        var primaryAssembly = loadedAssemblies[0];
        if (!string.IsNullOrWhiteSpace(primaryAssembly.EntryPointSubjectId))
        {
            return primaryAssembly;
        }

        if (fullAssemblyClosure)
        {
            return primaryAssembly;
        }

        throw new InvalidOperationException("managed closure input assembly does not define an entry point");
    }

}
