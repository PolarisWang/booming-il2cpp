using System.Text.Json;

namespace Chaos.IL2CPP.HotUpdate;

public sealed class SupplementalMetadataLoader
{
    public LoadedSupplementalMetadata LoadFromBytes(byte[] bytes)
    {
        ArgumentNullException.ThrowIfNull(bytes);

        var payload = JsonSerializer.Deserialize<SupplementalMetadataPayload>(bytes);
        if (payload is null)
        {
            throw new InvalidDataException("failed to deserialize supplemental metadata payload.");
        }

        if (!string.Equals(payload.FormatVersion, "v0", StringComparison.Ordinal))
        {
            throw new InvalidDataException($"unsupported supplemental metadata format version: {payload.FormatVersion}");
        }

        if (!string.Equals(payload.ArtifactKind, "supplementalMetadata", StringComparison.Ordinal))
        {
            throw new InvalidDataException($"unsupported supplemental metadata artifact kind: {payload.ArtifactKind}");
        }

        return new LoadedSupplementalMetadata(payload);
    }
}

public sealed class LoadedSupplementalMetadata
{
    private readonly Dictionary<string, SupplementalMetadataResolvedType> _typesBySubjectId;
    private readonly Dictionary<int, SupplementalMetadataResolvedType> _typesByToken;
    private readonly Dictionary<string, SupplementalMetadataResolvedMethod> _methodsBySubjectId;
    private readonly Dictionary<int, SupplementalMetadataResolvedMethod> _methodsByToken;
    private readonly HashSet<string> _genericInstantiations;

    public static LoadedSupplementalMetadata Empty { get; } = new(
        new SupplementalMetadataPayload
        {
            Types = [],
            Methods = [],
            GenericInstantiations = [],
        });

    public LoadedSupplementalMetadata(SupplementalMetadataPayload payload)
    {
        ArgumentNullException.ThrowIfNull(payload);

        _typesBySubjectId = payload.Types.ToDictionary(entry => entry.SubjectId, StringComparer.Ordinal);
        _typesByToken = payload.Types.ToDictionary(entry => entry.MetadataToken);
        _methodsBySubjectId = payload.Methods.ToDictionary(entry => entry.SubjectId, StringComparer.Ordinal);
        _methodsByToken = payload.Methods.ToDictionary(entry => entry.MetadataToken);
        _genericInstantiations = payload.GenericInstantiations
            .Select(entry => entry.SubjectId)
            .ToHashSet(StringComparer.Ordinal);
    }

    public bool TryGetTypeBySubjectId(string subjectId, out SupplementalMetadataResolvedType? entry)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        return _typesBySubjectId.TryGetValue(subjectId, out entry);
    }

    public bool TryGetTypeByToken(int metadataToken, out SupplementalMetadataResolvedType? entry)
    {
        return _typesByToken.TryGetValue(metadataToken, out entry);
    }

    public bool TryGetMethodBySubjectId(string subjectId, out SupplementalMetadataResolvedMethod? entry)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        return _methodsBySubjectId.TryGetValue(subjectId, out entry);
    }

    public bool TryGetMethodByToken(int metadataToken, out SupplementalMetadataResolvedMethod? entry)
    {
        return _methodsByToken.TryGetValue(metadataToken, out entry);
    }

    public bool HasGenericInstantiation(string subjectId)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        return _genericInstantiations.Contains(subjectId);
    }
}
