using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.HotUpdate;

public sealed class SupplementalMetadataRegistry
{
    private readonly Dictionary<string, SupplementalMetadataResolvedMethod> _packageMethodsBySubjectId =
        new(StringComparer.Ordinal);
    private readonly Dictionary<int, SupplementalMetadataResolvedMethod> _packageMethodsByToken = [];

    public LoadedHotUpdatePackage? ActivePackage { get; private set; }

    public LoadedSupplementalMetadata? ActiveMetadata { get; private set; }

    public void Activate(LoadedHotUpdatePackage package)
    {
        ArgumentNullException.ThrowIfNull(package);

        ActivePackage = package;
        ActiveMetadata = package.SupplementalMetadata;
        RebuildPackageMethods(package);
    }

    public void Clear()
    {
        ActivePackage = null;
        ActiveMetadata = null;
        _packageMethodsBySubjectId.Clear();
        _packageMethodsByToken.Clear();
    }

    public bool TryGetMethod(ManagedMethodIdentityArtifact identity, out SupplementalMetadataResolvedMethod? entry)
    {
        ArgumentNullException.ThrowIfNull(identity);

        if (TryGetMethodBySubjectId(ManagedMethodIdentityResolver.ResolveSubjectId(identity), out entry))
        {
            return true;
        }

        if (!string.Equals(identity.DefinitionSubjectId, identity.SubjectId, StringComparison.Ordinal) &&
            TryGetMethodBySubjectId(identity.DefinitionSubjectId, out entry))
        {
            return true;
        }

        entry = null;
        return false;
    }

    public bool TryGetMethodByToken(int metadataToken, out SupplementalMetadataResolvedMethod? entry)
    {
        if (ActiveMetadata is not null && ActiveMetadata.TryGetMethodByToken(metadataToken, out entry))
        {
            return true;
        }

        return _packageMethodsByToken.TryGetValue(metadataToken, out entry);
    }

    public bool TryGetTypeBySubjectId(string subjectId, out SupplementalMetadataResolvedType? entry)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);

        if (ActiveMetadata is not null)
        {
            return ActiveMetadata.TryGetTypeBySubjectId(subjectId, out entry);
        }

        entry = null;
        return false;
    }

    public bool TryGetTypeByToken(int metadataToken, out SupplementalMetadataResolvedType? entry)
    {
        if (ActiveMetadata is not null)
        {
            return ActiveMetadata.TryGetTypeByToken(metadataToken, out entry);
        }

        entry = null;
        return false;
    }

    public bool HasGenericInstantiation(string subjectId)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);
        return ActiveMetadata is not null && ActiveMetadata.HasGenericInstantiation(subjectId);
    }

    internal bool TryGetMethodBySubjectId(string subjectId, out SupplementalMetadataResolvedMethod? entry)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(subjectId);

        if (ActiveMetadata is not null && ActiveMetadata.TryGetMethodBySubjectId(subjectId, out entry))
        {
            return true;
        }

        return _packageMethodsBySubjectId.TryGetValue(subjectId, out entry);
    }

    private void RebuildPackageMethods(LoadedHotUpdatePackage package)
    {
        _packageMethodsBySubjectId.Clear();
        _packageMethodsByToken.Clear();

        var metadataToken = unchecked((int)0x71000000);
        foreach (var assembly in package.Manifest.Assemblies)
        {
            if (string.IsNullOrWhiteSpace(assembly.EntryPoint))
            {
                continue;
            }

            var entry = new SupplementalMetadataResolvedMethod
            {
                SubjectId = assembly.EntryPoint,
                MetadataToken = metadataToken++,
                SourceKind = "hot-update-entry-point",
            };

            _packageMethodsBySubjectId[assembly.EntryPoint] = entry;
            _packageMethodsByToken[entry.MetadataToken] = entry;
        }
    }
}
