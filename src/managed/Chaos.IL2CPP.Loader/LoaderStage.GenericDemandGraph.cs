using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

public sealed partial class LoaderStage
{
    private static GenericInstantiationDemandGraphModel BuildGenericInstantiationDemandGraph(
        IDictionary<string, GenericInstantiationDemandModel> genericInstantiationDemandEntries)
    {
        ArgumentNullException.ThrowIfNull(genericInstantiationDemandEntries);

        return new GenericInstantiationDemandGraphModel
        {
            Demands = genericInstantiationDemandEntries.Values
                .OrderBy(entry => entry.OwningAssemblyName, StringComparer.Ordinal)
                .ThenBy(entry => entry.SubjectKind, StringComparer.Ordinal)
                .ThenBy(entry => entry.SubjectId, StringComparer.Ordinal)
                .ToList(),
        };
    }

    private static GenericInstantiationDemandGraphModel MergeGenericInstantiationDemandGraphs(
        IReadOnlyList<LoadedAssemblyModel> assemblies)
    {
        ArgumentNullException.ThrowIfNull(assemblies);

        var mergedEntries = new Dictionary<string, GenericInstantiationDemandModel>(StringComparer.Ordinal);
        foreach (var assembly in assemblies)
        {
            if (assembly.GenericInstantiationDemandGraph?.Demands is not { Count: > 0 } demands)
            {
                continue;
            }

            foreach (var demand in demands)
            {
                mergedEntries[CreateDemandEntryKey(demand.RequestingAssemblyName, demand.SubjectKind, demand.SubjectId)] = demand;
            }
        }

        return BuildGenericInstantiationDemandGraph(mergedEntries);
    }

    private static GenericInstantiationDemandGraphModel MergeGenericInstantiationDemandGraphs(
        GenericInstantiationDemandGraphModel? existingGraph,
        GenericInstantiationDemandGraphModel? additionalGraph)
    {
        var mergedEntries = new Dictionary<string, GenericInstantiationDemandModel>(StringComparer.Ordinal);
        MergeDemandEntries(existingGraph, mergedEntries);
        MergeDemandEntries(additionalGraph, mergedEntries);
        return BuildGenericInstantiationDemandGraph(mergedEntries);
    }

    private static IReadOnlyDictionary<string, GenericInstantiationDemandModel> BuildGenericDemandLookup(
        GenericInstantiationDemandGraphModel? genericInstantiationDemandGraph)
    {
        var genericDemandLookup = new Dictionary<string, GenericInstantiationDemandModel>(StringComparer.Ordinal);
        if (genericInstantiationDemandGraph?.Demands is not { Count: > 0 } demands)
        {
            return genericDemandLookup;
        }

        foreach (var demand in demands)
        {
            // Multiple assemblies may demand the same generic instantiation
            // (e.g. Array::Empty<String> referenced from both the subject
            // and proof assemblies). All entries for the same SubjectId
            // produce the same RuntimeGenericContextArtifact — keep the first.
            if (genericDemandLookup.ContainsKey(demand.SubjectId))
            {
                continue;
            }

            genericDemandLookup[demand.SubjectId] = demand;
        }

        return genericDemandLookup;
    }

    private static IReadOnlyList<ManagedMethodModel> ApplyDemandDerivedRuntimeGenericContexts(
        IReadOnlyList<ManagedMethodModel> methods,
        GenericInstantiationDemandGraphModel? genericInstantiationDemandGraph)
    {
        ArgumentNullException.ThrowIfNull(methods);

        var genericDemandLookup = BuildGenericDemandLookup(genericInstantiationDemandGraph);
        return methods
            .Select(method => method with
            {
                RuntimeGenericContext = ResolveRuntimeGenericContext(
                    method.SubjectId,
                    method.DefinitionSubjectId,
                    genericDemandLookup),
            })
            .ToList();
    }

    private static RuntimeGenericContextArtifact? ResolveRuntimeGenericContext(
        string subjectId,
        string definitionSubjectId,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel>? genericDemandLookup)
    {
        _ = definitionSubjectId;

        if (genericDemandLookup is not null &&
            genericDemandLookup.TryGetValue(subjectId, out var demand))
        {
            return new RuntimeGenericContextArtifact
            {
                InstantiationKey = demand.InstantiationKey,
                SharedGenericBodyId = ManagedNaming.CreateSharedGenericBodyId(demand.InstantiationKey),
                InstantiationStubId = ManagedNaming.CreateInstantiationStubId(demand.InstantiationKey),
                SupportKindCode = demand.SupportKindCode,
                SpecializationKindCode = demand.SpecializationKindCode,
                StatusReasonCode = $"loader-demand:{demand.DemandSourceKind}",
            };
        }

        return null;
    }

    private static void TrackTypeInstantiationDemand(
        IDictionary<string, GenericInstantiationDemandModel> genericInstantiationDemandEntries,
        string requestingAssemblyName,
        TypeIdentity typeIdentity,
        string demandSourceKind)
    {
        ArgumentNullException.ThrowIfNull(genericInstantiationDemandEntries);
        ArgumentException.ThrowIfNullOrWhiteSpace(requestingAssemblyName);
        ArgumentNullException.ThrowIfNull(typeIdentity);
        ArgumentException.ThrowIfNullOrWhiteSpace(demandSourceKind);

        var instantiationKey = ManagedNaming.TryCreateGenericInstantiationKey(
            typeIdentity.SubjectId,
            typeIdentity.DefinitionSubjectId);
        if (instantiationKey is null)
        {
            return;
        }

        genericInstantiationDemandEntries[CreateDemandEntryKey(requestingAssemblyName, "type", typeIdentity.SubjectId)] =
            new GenericInstantiationDemandModel
            {
                RequestingAssemblyName = requestingAssemblyName,
                OwningAssemblyName = typeIdentity.AssemblyName,
                SubjectKind = "type",
                SubjectId = typeIdentity.SubjectId,
                DefinitionSubjectId = typeIdentity.DefinitionSubjectId,
                DemandSourceKind = demandSourceKind,
                InstantiationKey = instantiationKey,
                SupportKindCode = GenericSupportKind.Specialized,
                SpecializationKindCode = GenericSpecializationKind.SpecializedBody,
                FamilyKindCode = ResolveGenericDemandFamilyKind("type", instantiationKey.ContextKind),
                IsCrossAssembly = !string.Equals(requestingAssemblyName, typeIdentity.AssemblyName, StringComparison.Ordinal),
            };
    }

    private static void TrackMethodInstantiationDemand(
        IDictionary<string, GenericInstantiationDemandModel> genericInstantiationDemandEntries,
        string requestingAssemblyName,
        MethodReferenceSummary methodReference,
        string demandSourceKind)
    {
        ArgumentNullException.ThrowIfNull(genericInstantiationDemandEntries);
        ArgumentException.ThrowIfNullOrWhiteSpace(requestingAssemblyName);
        ArgumentNullException.ThrowIfNull(methodReference);
        ArgumentException.ThrowIfNullOrWhiteSpace(demandSourceKind);

        var instantiationKey = ManagedNaming.TryCreateGenericInstantiationKey(
            methodReference.SubjectId,
            methodReference.DefinitionSubjectId);
        if (instantiationKey is null)
        {
            return;
        }

        genericInstantiationDemandEntries[CreateDemandEntryKey(requestingAssemblyName, "method", methodReference.SubjectId)] =
            new GenericInstantiationDemandModel
            {
                RequestingAssemblyName = requestingAssemblyName,
                OwningAssemblyName = methodReference.AssemblyName,
                SubjectKind = "method",
                SubjectId = methodReference.SubjectId,
                DefinitionSubjectId = methodReference.DefinitionSubjectId,
                DemandSourceKind = demandSourceKind,
                InstantiationKey = instantiationKey,
                SupportKindCode = GenericSupportKind.Specialized,
                SpecializationKindCode = GenericSpecializationKind.SpecializedBody,
                FamilyKindCode = ResolveGenericDemandFamilyKind("method", instantiationKey.ContextKind),
                IsCrossAssembly = !string.Equals(requestingAssemblyName, methodReference.AssemblyName, StringComparison.Ordinal),
            };
    }

    private static GenericDemandFamilyKind ResolveGenericDemandFamilyKind(
        string subjectKind,
        GenericContextKind contextKind)
    {
        return (subjectKind, contextKind) switch
        {
            ("type", GenericContextKind.TypeInstantiation) => GenericDemandFamilyKind.ClosedGenericType,
            ("method", GenericContextKind.TypeInstantiation) => GenericDemandFamilyKind.ClosedMethodOnGenericType,
            ("method", GenericContextKind.MethodInstantiation) => GenericDemandFamilyKind.ClosedGenericMethod,
            ("method", GenericContextKind.TypeAndMethodInstantiation) => GenericDemandFamilyKind.ClosedGenericMethodOnGenericType,
            _ => throw new NotSupportedException(
                $"unsupported generic demand family combination '{subjectKind}' / '{contextKind}'"),
        };
    }

    private static void MergeDemandEntries(
        GenericInstantiationDemandGraphModel? graph,
        IDictionary<string, GenericInstantiationDemandModel> mergedEntries)
    {
        if (graph?.Demands is not { Count: > 0 } demands)
        {
            return;
        }

        foreach (var demand in demands)
        {
            mergedEntries[CreateDemandEntryKey(demand.RequestingAssemblyName, demand.SubjectKind, demand.SubjectId)] = demand;
        }
    }

    private static string CreateDemandEntryKey(string requestingAssemblyName, string subjectKind, string subjectId)
    {
        return $"{requestingAssemblyName}|{subjectKind}|{subjectId}";
    }

    private static void EnsureEquivalentDemand(
        GenericInstantiationDemandModel existingDemand,
        GenericInstantiationDemandModel additionalDemand)
    {
        if (string.Equals(existingDemand.DefinitionSubjectId, additionalDemand.DefinitionSubjectId, StringComparison.Ordinal) &&
            existingDemand.SupportKindCode == additionalDemand.SupportKindCode &&
            existingDemand.SpecializationKindCode == additionalDemand.SpecializationKindCode &&
            existingDemand.FamilyKindCode == additionalDemand.FamilyKindCode &&
            AreEquivalentInstantiationKeys(existingDemand.InstantiationKey, additionalDemand.InstantiationKey))
        {
            return;
        }

        throw new InvalidOperationException(
            $"conflicting generic instantiation demand entries detected for '{existingDemand.SubjectId}' during Loader.");
    }

    private static bool AreEquivalentInstantiationKeys(
        GenericInstantiationKey left,
        GenericInstantiationKey right)
    {
        return left.ContextKind == right.ContextKind &&
               string.Equals(left.DefinitionSubjectId, right.DefinitionSubjectId, StringComparison.Ordinal) &&
               SequenceEqual(left.TypeArguments, right.TypeArguments) &&
               SequenceEqual(left.MethodArguments, right.MethodArguments);
    }

    private static bool SequenceEqual(
        IReadOnlyList<string>? left,
        IReadOnlyList<string>? right)
    {
        if (ReferenceEquals(left, right))
        {
            return true;
        }

        if (left is null || right is null || left.Count != right.Count)
        {
            return false;
        }

        for (var index = 0; index < left.Count; index++)
        {
            if (!string.Equals(left[index], right[index], StringComparison.Ordinal))
            {
                return false;
            }
        }

        return true;
    }
}
