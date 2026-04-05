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
                    ? "first-proof-entry"
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
            });
        }

        foreach (var field in linkedWorld.Fields)
        {
            registrations.Add(new MetadataRegistrationEntry
            {
                RegistrationKind = "field",
                Slot = slot++,
                SubjectId = field.SubjectId,
            });
        }

        foreach (var method in linkedWorld.Methods)
        {
            registrations.Add(new MetadataRegistrationEntry
            {
                RegistrationKind = "method",
                Slot = slot++,
                SubjectId = method.SubjectId,
            });
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
