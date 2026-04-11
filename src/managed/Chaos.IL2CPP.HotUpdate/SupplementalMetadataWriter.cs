using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.HotUpdate;

public sealed class SupplementalMetadataWriter
{
    private const int HotUpdateTypeTokenBase = 0x72000000;

    public byte[] WriteToBytes(
        SupplementalMetadataTemplateArtifact template,
        HotUpdateMetadataRequirements requirements)
    {
        ArgumentNullException.ThrowIfNull(template);
        ArgumentNullException.ThrowIfNull(requirements);

        var templateTypes = template.RegisteredTypes.ToDictionary(entry => entry.SubjectId, StringComparer.Ordinal);
        var templateMethods = template.RegisteredMethods.ToDictionary(entry => entry.SubjectId, StringComparer.Ordinal);

        var resolvedTypes = requirements.ReferencedAotTypeSubjectIds
            .Distinct(StringComparer.Ordinal)
            .Select(subjectId => RequireRegisteredType(templateTypes, subjectId))
            .Select(entry => new SupplementalMetadataResolvedType
            {
                SubjectId = entry.SubjectId,
                MetadataToken = entry.MetadataToken,
                SourceKind = "aot-registered",
            })
            .ToList();

        resolvedTypes.AddRange(
            requirements.HotUpdateTypeSubjectIds
                .Distinct(StringComparer.Ordinal)
                .Select((subjectId, index) => new SupplementalMetadataResolvedType
                {
                    SubjectId = subjectId,
                    MetadataToken = HotUpdateTypeTokenBase + index,
                    SourceKind = "hot-update",
                }));

        var resolvedMethods = requirements.ReferencedAotMethodSubjectIds
            .Distinct(StringComparer.Ordinal)
            .Select(subjectId => RequireRegisteredMethod(templateMethods, subjectId))
            .Select(entry => new SupplementalMetadataResolvedMethod
            {
                SubjectId = entry.SubjectId,
                MetadataToken = entry.MetadataToken,
                SourceKind = "aot-registered",
            })
            .ToList();

        var payload = new SupplementalMetadataPayload
        {
            Types = resolvedTypes,
            Methods = resolvedMethods,
            GenericInstantiations = requirements.GenericInstantiationSubjectIds
                .Distinct(StringComparer.Ordinal)
                .Select(subjectId => new SupplementalMetadataGenericInstantiation
                {
                    SubjectId = subjectId,
                    SourceKind = "genericInstantiation",
                })
                .ToList(),
        };

        return JsonSerializer.SerializeToUtf8Bytes(
            payload,
            new JsonSerializerOptions
            {
                WriteIndented = true,
            });
    }

    private static SupplementalMetadataTypeTemplateEntry RequireRegisteredType(
        IReadOnlyDictionary<string, SupplementalMetadataTypeTemplateEntry> templateTypes,
        string subjectId)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);

        if (templateTypes.TryGetValue(subjectId, out var entry))
        {
            return entry;
        }

        throw new InvalidOperationException($"supplemental metadata template is missing type '{subjectId}'.");
    }

    private static SupplementalMetadataMethodTemplateEntry RequireRegisteredMethod(
        IReadOnlyDictionary<string, SupplementalMetadataMethodTemplateEntry> templateMethods,
        string subjectId)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);

        if (templateMethods.TryGetValue(subjectId, out var entry))
        {
            return entry;
        }

        throw new InvalidOperationException($"supplemental metadata template is missing method '{subjectId}'.");
    }
}

public sealed record HotUpdateMetadataRequirements
{
    public IReadOnlyList<string> ReferencedAotTypeSubjectIds { get; init; } = [];

    public IReadOnlyList<string> ReferencedAotMethodSubjectIds { get; init; } = [];

    public IReadOnlyList<string> HotUpdateTypeSubjectIds { get; init; } = [];

    public IReadOnlyList<string> GenericInstantiationSubjectIds { get; init; } = [];
}

public sealed record SupplementalMetadataPayload
{
    public string FormatVersion { get; init; } = "v0";

    public string ArtifactKind { get; init; } = "supplementalMetadata";

    public required IReadOnlyList<SupplementalMetadataResolvedType> Types { get; init; }

    public required IReadOnlyList<SupplementalMetadataResolvedMethod> Methods { get; init; }

    public required IReadOnlyList<SupplementalMetadataGenericInstantiation> GenericInstantiations { get; init; }
}

public sealed record SupplementalMetadataResolvedType
{
    public required string SubjectId { get; init; }

    public required int MetadataToken { get; init; }

    public required string SourceKind { get; init; }
}

public sealed record SupplementalMetadataResolvedMethod
{
    public required string SubjectId { get; init; }

    public required int MetadataToken { get; init; }

    public required string SourceKind { get; init; }
}

public sealed record SupplementalMetadataGenericInstantiation
{
    public required string SubjectId { get; init; }

    public required string SourceKind { get; init; }
}
