using System.Reflection.Metadata;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

internal sealed class MetadataTypeResolver
{
    private const string CoreLibraryAssemblyName = "System.Private.CoreLib";

    private readonly MetadataReader _metadataReader;

    public MetadataTypeResolver(MetadataReader metadataReader, string currentAssemblyName)
    {
        _metadataReader = metadataReader;
        CurrentAssemblyName = currentAssemblyName;
        TypeNameProvider = new TypeNameProvider(this);
        TypeIdentityProvider = new TypeIdentityProvider(this);
    }

    public string CurrentAssemblyName { get; }

    public TypeNameProvider TypeNameProvider { get; }

    public TypeIdentityProvider TypeIdentityProvider { get; }

    public TypeIdentity ResolveTypeIdentity(EntityHandle handle)
    {
        return handle.Kind switch
        {
            HandleKind.TypeDefinition => ResolveTypeDefinition((TypeDefinitionHandle)handle),
            HandleKind.TypeReference => ResolveTypeReference((TypeReferenceHandle)handle),
            HandleKind.TypeSpecification => ResolveTypeSpecification((TypeSpecificationHandle)handle),
            _ => throw new NotSupportedException($"unsupported type handle kind: {handle.Kind}"),
        };
    }

    public SignatureContext<string>? CreateTypeNameContext(TypeIdentity typeIdentity)
    {
        return typeIdentity.TypeArguments.IsDefaultOrEmpty
            ? null
            : new SignatureContext<string>(typeIdentity.TypeArguments, []);
    }

    public string ResolveAssemblyName(EntityHandle scope)
    {
        var assemblyName = scope.Kind switch
        {
            HandleKind.AssemblyReference => _metadataReader.GetString(
                _metadataReader.GetAssemblyReference((AssemblyReferenceHandle)scope).Name),
            HandleKind.ModuleDefinition => CurrentAssemblyName,
            HandleKind.ModuleReference => CurrentAssemblyName,
            HandleKind.TypeReference => ResolveAssemblyName(_metadataReader.GetTypeReference((TypeReferenceHandle)scope).ResolutionScope),
            _ => throw new NotSupportedException($"unsupported resolution scope kind: {scope.Kind}"),
        };

        return NormalizeAssemblyName(assemblyName);
    }

    public TypeIdentity CreateSimpleTypeIdentity(string assemblyName, string displayName)
    {
        return new TypeIdentity
        {
            AssemblyName = assemblyName,
            SubjectId = $"{assemblyName}/{displayName}",
            DisplayName = displayName,
            DefinitionSubjectId = $"{assemblyName}/{displayName}",
            DefinitionDisplayName = displayName,
            TypeArguments = [],
        };
    }

    private TypeIdentity ResolveTypeDefinition(TypeDefinitionHandle handle)
    {
        var typeDefinition = _metadataReader.GetTypeDefinition(handle);
        var namespaceName = _metadataReader.GetString(typeDefinition.Namespace);
        var typeName = _metadataReader.GetString(typeDefinition.Name);
        if (!typeDefinition.GetDeclaringType().IsNil)
        {
            var declaringType = ResolveTypeIdentity(typeDefinition.GetDeclaringType());
            return CreateResolvedTypeIdentity(CurrentAssemblyName, namespaceName, typeName, declaringType);
        }

        return CreateResolvedTypeIdentity(CurrentAssemblyName, namespaceName, typeName);
    }

    private TypeIdentity ResolveTypeReference(TypeReferenceHandle handle)
    {
        var typeReference = _metadataReader.GetTypeReference(handle);
        var namespaceName = _metadataReader.GetString(typeReference.Namespace);
        var typeName = _metadataReader.GetString(typeReference.Name);
        if (typeReference.ResolutionScope.Kind == HandleKind.TypeReference)
        {
            var declaringType = ResolveTypeIdentity(typeReference.ResolutionScope);
            return CreateResolvedTypeIdentity(declaringType.AssemblyName, namespaceName, typeName, declaringType);
        }

        var assemblyName = ResolveAssemblyName(typeReference.ResolutionScope);
        return CreateResolvedTypeIdentity(assemblyName, namespaceName, typeName);
    }

    private TypeIdentity ResolveTypeSpecification(TypeSpecificationHandle handle)
    {
        return _metadataReader.GetTypeSpecification(handle).DecodeSignature(TypeIdentityProvider, null);
    }

    private static string NormalizeAssemblyName(string assemblyName)
    {
        return assemblyName switch
        {
            "System.Runtime" => CoreLibraryAssemblyName,
            "mscorlib" => CoreLibraryAssemblyName,
            _ => assemblyName,
        };
    }

    private static TypeIdentity CreateResolvedTypeIdentity(
        string assemblyName,
        string? namespaceName,
        string typeName,
        TypeIdentity? declaringType = null)
    {
        var subjectId = declaringType is null
            ? ManagedNaming.CreateTypeSubjectId(assemblyName, namespaceName, typeName)
            : $"{declaringType.SubjectId}+{typeName}";
        var displayName = declaringType is null
            ? ManagedNaming.CreateTypeDisplayName(assemblyName, namespaceName, typeName)
            : $"{declaringType.DisplayName}+{typeName}";

        return new TypeIdentity
        {
            AssemblyName = assemblyName,
            SubjectId = subjectId,
            DisplayName = displayName,
            DefinitionSubjectId = subjectId,
            DefinitionDisplayName = displayName,
            TypeArguments = [],
        };
    }
}
