using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class AotCoreIrLowering
{

    private static RuntimeGenericContextArtifact? ResolveRuntimeGenericContext(
        string subjectId,
        string definitionSubjectId,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel>? genericDemandLookup)
    {
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



    private static RuntimeGenericContextArtifact? ResolveFieldRuntimeGenericContext(
        string definitionSubjectId,
        string declaringTypeSubjectId,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel>? genericDemandLookup)
    {
        if (genericDemandLookup is null ||
            !genericDemandLookup.TryGetValue(declaringTypeSubjectId, out var declaringTypeDemand))
        {
            return null;
        }

        if (declaringTypeDemand.SubjectKind != "type" ||
            declaringTypeDemand.InstantiationKey.ContextKind != GenericContextKind.TypeInstantiation)
        {
            return null;
        }

        var instantiationKey = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.TypeInstantiation,
            DefinitionSubjectId = definitionSubjectId,
            TypeArguments = declaringTypeDemand.InstantiationKey.TypeArguments,
            MethodArguments = [],
        };

        return new RuntimeGenericContextArtifact
        {
            InstantiationKey = instantiationKey,
            SharedGenericBodyId = ManagedNaming.CreateSharedGenericBodyId(instantiationKey),
            InstantiationStubId = ManagedNaming.CreateInstantiationStubId(instantiationKey),
            SupportKindCode = declaringTypeDemand.SupportKindCode,
            SpecializationKindCode = declaringTypeDemand.SpecializationKindCode,
            StatusReasonCode = $"loader-demand:{declaringTypeDemand.DemandSourceKind}:field-projection",
        };
    }



    private static GenericDiagnosticArtifact? ResolveGenericDiagnostic(
        string subjectId,
        string definitionSubjectId,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel>? genericDemandLookup)
    {
        if (genericDemandLookup is not null &&
            genericDemandLookup.TryGetValue(subjectId, out var demand))
        {
            var diagnostic = ManagedNaming.TryCreateGenericDiagnosticArtifact(
                demand.SubjectId,
                demand.DefinitionSubjectId);
            if (diagnostic is not null)
            {
                return diagnostic with
                {
                    InstantiationKey = demand.InstantiationKey,
                };
            }

            return new GenericDiagnosticArtifact
            {
                SubjectId = demand.SubjectId,
                DefinitionSubjectId = demand.DefinitionSubjectId,
                DisplaySubjectId = demand.SubjectId,
                InstantiationKey = demand.InstantiationKey,
            };
        }

        return ManagedNaming.TryCreateGenericDiagnosticArtifact(subjectId, definitionSubjectId);
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
            $"conflicting generic instantiation demand entries detected for '{existingDemand.SubjectId}' during AotCoreIr lowering.");
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

}
