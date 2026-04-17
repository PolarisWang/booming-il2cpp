using System.Collections.Immutable;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

public sealed partial class LoaderStage
{
    private static List<LoadedAssemblyModel> MaterializeCrossAssemblyMethodInstantiations(
        IReadOnlyList<string> assemblyPaths,
        IReadOnlyList<LoadedAssemblyModel> loadedAssemblies)
    {
        var definitionMethodsByAssembly = loadedAssemblies.ToDictionary(
            assembly => assembly.Assembly.Name,
            assembly => (IReadOnlyDictionary<string, ManagedMethodModel>)assembly.Methods
                .ToDictionary(method => method.SubjectId, StringComparer.Ordinal),
            StringComparer.Ordinal);
        var materializedMethodsByAssembly = loadedAssemblies.ToDictionary(
            assembly => assembly.Assembly.Name,
            assembly => assembly.Methods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal),
            StringComparer.Ordinal);

        foreach (var assemblyPath in assemblyPaths)
        {
            CollectCrossAssemblyMethodInstantiations(
                assemblyPath,
                definitionMethodsByAssembly,
                materializedMethodsByAssembly);
        }

        return loadedAssemblies
            .Select(assembly => assembly with
            {
                Methods = materializedMethodsByAssembly[assembly.Assembly.Name].Values
                    .OrderBy(model => model.MetadataToken)
                    .ThenBy(model => model.SubjectId, StringComparer.Ordinal)
                    .ToList(),
            })
            .ToList();
    }

    private static void CollectCrossAssemblyMethodInstantiations(
        string assemblyPath,
        IReadOnlyDictionary<string, IReadOnlyDictionary<string, ManagedMethodModel>> definitionMethodsByAssembly,
        IReadOnlyDictionary<string, Dictionary<string, ManagedMethodModel>> materializedMethodsByAssembly)
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

            TryMaterializeCrossAssemblyMethod(
                assemblyName,
                DescribeMemberReferenceMethod(metadataReader, typeResolver, memberReferenceHandle),
                definitionMethodsByAssembly,
                materializedMethodsByAssembly);
        }

        for (var rowNumber = 1; rowNumber <= metadataReader.GetTableRowCount(TableIndex.MethodSpec); rowNumber++)
        {
            var methodSpecificationHandle = MetadataTokens.MethodSpecificationHandle(rowNumber);
            TryMaterializeCrossAssemblyMethod(
                assemblyName,
                DescribeMethodSpecification(
                    metadataReader,
                    typeResolver,
                    typeModels,
                    ownerIndex.MethodOwners,
                    methodSpecificationHandle),
                definitionMethodsByAssembly,
                materializedMethodsByAssembly);
        }
    }

    private static void TryMaterializeCrossAssemblyMethod(
        string sourceAssemblyName,
        MethodReferenceSummary methodReference,
        IReadOnlyDictionary<string, IReadOnlyDictionary<string, ManagedMethodModel>> definitionMethodsByAssembly,
        IReadOnlyDictionary<string, Dictionary<string, ManagedMethodModel>> materializedMethodsByAssembly)
    {
        if (string.Equals(methodReference.AssemblyName, sourceAssemblyName, StringComparison.Ordinal) ||
            !definitionMethodsByAssembly.TryGetValue(methodReference.AssemblyName, out var targetDefinitionMethods) ||
            !materializedMethodsByAssembly.TryGetValue(methodReference.AssemblyName, out var targetMaterializedMethods))
        {
            return;
        }

        TryMaterializeMethod(
            methodReference.AssemblyName,
            methodReference,
            targetDefinitionMethods,
            EmptyFieldBindings,
            targetMaterializedMethods);
    }

    private static LoadedAssemblyModel ResolveEntryAssembly(
        IReadOnlyList<LoadedAssemblyModel> loadedAssemblies,
        string? entryPointSubjectIdOverride)
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

        throw new InvalidOperationException("managed closure input assembly does not define an entry point");
    }

}
