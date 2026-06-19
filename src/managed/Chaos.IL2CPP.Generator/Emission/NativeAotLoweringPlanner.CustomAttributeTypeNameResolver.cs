namespace Chaos.IL2CPP.Generator;

using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;

internal static class NativeAotCustomAttributeTypeNameResolver
{
    public static string GetTypeName(MetadataReader metadataReader, TypeDefinitionHandle handle)
    {
        var typeDefinition = metadataReader.GetTypeDefinition(handle);
        var typeName = metadataReader.GetString(typeDefinition.Name);
        var namespaceName = metadataReader.GetString(typeDefinition.Namespace);
        var declaringTypeHandle = typeDefinition.GetDeclaringType();
        if (!declaringTypeHandle.IsNil)
        {
            return $"{GetTypeName(metadataReader, declaringTypeHandle)}+{typeName}";
        }

        return string.IsNullOrEmpty(namespaceName)
            ? typeName
            : $"{namespaceName}.{typeName}";
    }

    public static string GetTypeName(MetadataReader metadataReader, TypeReferenceHandle handle)
    {
        var typeReference = metadataReader.GetTypeReference(handle);
        var typeName = metadataReader.GetString(typeReference.Name);
        var namespaceName = metadataReader.GetString(typeReference.Namespace);
        if (typeReference.ResolutionScope.Kind == HandleKind.TypeReference)
        {
            return $"{GetTypeName(metadataReader, (TypeReferenceHandle)typeReference.ResolutionScope)}+{typeName}";
        }

        return string.IsNullOrEmpty(namespaceName)
            ? typeName
            : $"{namespaceName}.{typeName}";
    }
}
