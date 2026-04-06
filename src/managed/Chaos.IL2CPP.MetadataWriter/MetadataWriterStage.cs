using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.MetadataWriter;

public sealed class MetadataWriterStage
{
    public string Name => "MetadataWriter";

    public MetadataWriterOutput Write(LinkedWorldModel linkedWorld)
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

        var registrations = new List<MetadataRegistrationEntry>
        {
            new()
            {
                RegistrationKind = "assembly",
                Slot = 0,
                SubjectId = linkedWorld.Assembly.Name,
            },
        };

        var slot = 1;
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
                });
            }
        }

        return new MetadataWriterOutput
        {
            AotManifest = new AotManifestArtifact
            {
                Entries = aotEntries,
            },
            MetadataRegistration = new MetadataRegistrationArtifact
            {
                Registrations = registrations,
            },
        };
    }
}
