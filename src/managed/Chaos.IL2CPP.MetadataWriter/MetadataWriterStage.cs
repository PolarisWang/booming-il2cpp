using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.MetadataWriter;

public sealed class MetadataWriterStage
{
    private const int DefaultReservedTypeSlots = 256;
    private const int DefaultReservedMethodSlots = 1024;
    private const int DefaultReservedGenericInstantiationSlots = 256;

    public string Name => "MetadataWriter";

    public PipelineResult<MetadataWriterOutput> Write(LinkedWorldModel linkedWorld)
    {
        try
        {
            var genericDemandLookup = BuildGenericDemandLookup(linkedWorld.GenericInstantiationDemandGraph);
            var aotManifest = BuildAotManifest(linkedWorld);
            var registrations = BuildMetadataRegistration(linkedWorld);
            var supplementalMetadataTemplate = BuildSupplementalTemplate(linkedWorld, genericDemandLookup);

            return PipelineResult<MetadataWriterOutput>.Ok(new MetadataWriterOutput
            {
                AotManifest = aotManifest,
                MetadataRegistration = new MetadataRegistrationArtifact
                {
                    Registrations = registrations,
                },
                SupplementalMetadataTemplate = supplementalMetadataTemplate,
            });
        }
        catch (InvalidOperationException ex) when (ex.Message.Contains("conflicting generic instantiation"))
        {
            return PipelineResult<MetadataWriterOutput>.Fail("METADATA_WRITER_DEMAND_CONFLICT",
                $"Generic instantiation demand conflict: {ex.Message}", ex);
        }
        catch (ArgumentNullException ex)
        {
            return PipelineResult<MetadataWriterOutput>.Fail("METADATA_WRITER_INVALID_INPUT",
                $"Null or invalid input in LinkedWorldModel: {ex.Message}", ex);
        }
        catch (Exception ex)
        {
            return PipelineResult<MetadataWriterOutput>.Fail("METADATA_WRITER_INTERNAL_ERROR",
                $"Metadata writer stage failed: {ex.Message}", ex);
        }
    }

    private static AotManifestArtifact BuildAotManifest(LinkedWorldModel linkedWorld)
    {
        var aotEntries = new List<AotManifestEntry>();

        foreach (var method in linkedWorld.Methods)
        {
            aotEntries.Add(new AotManifestEntry
            {
                AssemblyName = method.AssemblyName,
                SubjectKind = "method",
                SubjectId = method.SubjectId,
                Reason = string.Equals(method.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal)
                    ? "entrypoint"
                    : "generated-direct-call",
            });
        }

        foreach (var field in linkedWorld.Fields)
        {
            aotEntries.Add(new AotManifestEntry
            {
                AssemblyName = field.AssemblyName,
                SubjectKind = "field",
                SubjectId = field.SubjectId,
                Reason = "instance-field-state",
            });
        }

        foreach (var dependency in linkedWorld.Dependencies)
        {
            aotEntries.Add(new AotManifestEntry
            {
                AssemblyName = dependency.AssemblyName,
                SubjectKind = dependency.SubjectKind,
                SubjectId = dependency.SubjectId,
                Reason = dependency.Reason,
            });
        }

        return new AotManifestArtifact
        {
            Entries = aotEntries,
        };
    }

    private static List<MetadataRegistrationEntry> BuildMetadataRegistration(LinkedWorldModel linkedWorld)
    {
        var registrations = new List<MetadataRegistrationEntry>();

        var slot = 0;
        foreach (var assembly in linkedWorld.Assemblies)
        {
            registrations.Add(new MetadataRegistrationEntry
            {
                RegistrationKind = "assembly",
                Slot = slot++,
                SubjectId = assembly.Name,
            });
        }

        foreach (var type in linkedWorld.Types)
        {
            registrations.Add(new MetadataRegistrationEntry
            {
                RegistrationKind = "type",
                Slot = slot++,
                SubjectId = type.SubjectId,
                Name = type.Name,
                NamespaceName = type.NamespaceName,
                DisplayName = type.DisplayName,
                DefinitionSubjectId = type.DefinitionSubjectId,
            });
        }

        foreach (var field in linkedWorld.Fields)
        {
            registrations.Add(new MetadataRegistrationEntry
            {
                RegistrationKind = "field",
                Slot = slot++,
                SubjectId = field.SubjectId,
                Name = field.Name,
                DefinitionSubjectId = field.DefinitionSubjectId,
                DeclaringTypeSubjectId = field.DeclaringTypeSubjectId,
                MemberType = field.FieldType,
            });
        }

        foreach (var property in linkedWorld.Properties)
        {
            registrations.Add(new MetadataRegistrationEntry
            {
                RegistrationKind = "property",
                Slot = slot++,
                SubjectId = property.SubjectId,
                Name = property.Name,
                DefinitionSubjectId = property.DefinitionSubjectId,
                DeclaringTypeSubjectId = property.DeclaringTypeSubjectId,
                MemberType = property.PropertyType,
            });
        }

        foreach (var method in linkedWorld.Methods)
        {
            registrations.Add(new MetadataRegistrationEntry
            {
                RegistrationKind = "method",
                Slot = slot++,
                SubjectId = method.SubjectId,
                Name = method.Name,
                DefinitionSubjectId = method.DefinitionSubjectId,
                DeclaringTypeSubjectId = method.DeclaringTypeSubjectId,
                MemberType = method.ReturnType,
                ParameterCount = method.Parameters.Count,
                IsImported = method.Import is not null,
                ImportModuleName = method.Import?.ModuleName,
                ImportEntryPointName = method.Import?.EntryPointName,
            });
        }

        foreach (var method in linkedWorld.Methods)
        {
            for (var parameterIndex = 0; parameterIndex < method.Parameters.Count; parameterIndex++)
            {
                var parameter = method.Parameters[parameterIndex];
                registrations.Add(new MetadataRegistrationEntry
                {
                    RegistrationKind = "parameter",
                    Slot = slot++,
                    SubjectId = ManagedNaming.CreateParameterSubjectId(method.SubjectId, parameterIndex, parameter.Name),
                    Name = parameter.Name,
                    DeclaringMethodSubjectId = method.SubjectId,
                    MemberType = parameter.Type,
                    ParameterIndex = parameterIndex,
                    ParameterAttributes = parameter.Attributes != 0 ? parameter.Attributes : null,
                });
            }
        }

        return registrations;
    }

    private static SupplementalMetadataTemplateArtifact BuildSupplementalTemplate(
        LinkedWorldModel linkedWorld,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel> genericDemandLookup)
    {
        return new SupplementalMetadataTemplateArtifact
        {
            RegisteredTypes = linkedWorld.Types
                .Select(type => new SupplementalMetadataTypeTemplateEntry
                {
                    AssemblyName = type.AssemblyName,
                    SubjectId = type.SubjectId,
                    DefinitionSubjectId = type.DefinitionSubjectId,
                    RuntimeGenericContext = ResolveRuntimeGenericContext(
                        type.SubjectId,
                        type.DefinitionSubjectId,
                        genericDemandLookup),
                    GenericDiagnostic = ResolveGenericDiagnostic(
                        type.SubjectId,
                        type.DefinitionSubjectId,
                        genericDemandLookup),
                    MetadataToken = type.MetadataToken,
                })
                .ToList(),
            RegisteredMethods = linkedWorld.Methods
                .Select(method => new SupplementalMetadataMethodTemplateEntry
                {
                    AssemblyName = method.AssemblyName,
                    SubjectId = method.SubjectId,
                    DefinitionSubjectId = method.DefinitionSubjectId,
                    RuntimeGenericContext = ResolveRuntimeGenericContext(
                        method.SubjectId,
                        method.DefinitionSubjectId,
                        genericDemandLookup),
                    GenericDiagnostic = ResolveGenericDiagnostic(
                        method.SubjectId,
                        method.DefinitionSubjectId,
                        genericDemandLookup),
                    DeclaringTypeSubjectId = method.DeclaringTypeSubjectId,
                    MetadataToken = method.MetadataToken,
                    ParameterCount = method.Parameters.Count,
                })
                .ToList(),
            ReservedSlots = new SupplementalMetadataReservedSlots
            {
                TypeCount = DefaultReservedTypeSlots,
                MethodCount = DefaultReservedMethodSlots,
                GenericInstantiationCount = DefaultReservedGenericInstantiationSlots,
            },
        };
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
            if (genericDemandLookup.ContainsKey(demand.SubjectId))
            {
                continue;
            }

            genericDemandLookup[demand.SubjectId] = demand;
        }

        return genericDemandLookup;
    }

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
            $"conflicting generic instantiation demand entries detected for '{existingDemand.SubjectId}' during MetadataWriter.");
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
