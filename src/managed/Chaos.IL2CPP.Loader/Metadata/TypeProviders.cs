using System.Collections.Immutable;
using System.Reflection.Metadata;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

internal sealed class TypeNameProvider : ISignatureTypeProvider<string, SignatureContext<string>?>
{
    private readonly MetadataTypeResolver _resolver;

    public TypeNameProvider(MetadataTypeResolver resolver)
    {
        _resolver = resolver;
    }

    public string GetArrayType(string elementType, ArrayShape shape)
    {
        if (shape.Rank == 1 && shape.LowerBounds.IsDefaultOrEmpty && shape.Sizes.IsDefaultOrEmpty)
        {
            return $"{elementType}[]";
        }

        return $"{elementType}[{new string(',', shape.Rank - 1)}]";
    }

    public string GetByReferenceType(string elementType) => $"{elementType}&";

    public string GetFunctionPointerType(MethodSignature<string> signature)
    {
        return FunctionPointerTypeFormatter.Format(
            signature.Header,
            signature.ParameterTypes,
            signature.ReturnType);
    }

    public string GetGenericInstantiation(string genericType, ImmutableArray<string> typeArguments)
    {
        return ManagedNaming.CreateInstantiatedTypeDisplayName(genericType, typeArguments);
    }

    public string GetGenericMethodParameter(SignatureContext<string>? genericContext, int index)
    {
        return genericContext is not null &&
               index >= 0 &&
               index < genericContext.MethodArguments.Length
            ? genericContext.MethodArguments[index]
            : $"!!{index}";
    }

    public string GetGenericTypeParameter(SignatureContext<string>? genericContext, int index)
    {
        return genericContext is not null &&
               index >= 0 &&
               index < genericContext.TypeArguments.Length
            ? genericContext.TypeArguments[index]
            : $"!{index}";
    }

    public string GetModifiedType(string modifierType, string unmodifiedType, bool isRequired) => unmodifiedType;

    public string GetPinnedType(string elementType) => elementType;

    public string GetPointerType(string elementType) => $"{elementType}*";

    public string GetPrimitiveType(PrimitiveTypeCode typeCode)
    {
        return typeCode switch
        {
            PrimitiveTypeCode.Boolean => "System.Boolean",
            PrimitiveTypeCode.Byte => "System.Byte",
            PrimitiveTypeCode.Char => "System.Char",
            PrimitiveTypeCode.Double => "System.Double",
            PrimitiveTypeCode.Int16 => "System.Int16",
            PrimitiveTypeCode.Int32 => "System.Int32",
            PrimitiveTypeCode.Int64 => "System.Int64",
            PrimitiveTypeCode.IntPtr => "System.IntPtr",
            PrimitiveTypeCode.Object => "System.Object",
            PrimitiveTypeCode.SByte => "System.SByte",
            PrimitiveTypeCode.Single => "System.Single",
            PrimitiveTypeCode.String => "System.String",
            PrimitiveTypeCode.TypedReference => "System.TypedReference",
            PrimitiveTypeCode.UInt16 => "System.UInt16",
            PrimitiveTypeCode.UInt32 => "System.UInt32",
            PrimitiveTypeCode.UInt64 => "System.UInt64",
            PrimitiveTypeCode.UIntPtr => "System.UIntPtr",
            PrimitiveTypeCode.Void => "System.Void",
            _ => throw new NotSupportedException($"unsupported primitive type: {typeCode}"),
        };
    }

    public string GetSZArrayType(string elementType) => $"{elementType}[]";

    public string GetTypeFromDefinition(MetadataReader reader, TypeDefinitionHandle handle, byte rawTypeKind)
    {
        return _resolver.ResolveTypeIdentity(handle).DisplayName;
    }

    public string GetTypeFromReference(MetadataReader reader, TypeReferenceHandle handle, byte rawTypeKind)
    {
        return _resolver.ResolveTypeIdentity(handle).DisplayName;
    }

    public string GetTypeFromSpecification(
        MetadataReader reader,
        SignatureContext<string>? genericContext,
        TypeSpecificationHandle handle,
        byte rawTypeKind)
    {
        return reader.GetTypeSpecification(handle).DecodeSignature(this, genericContext);
    }

    private static bool IsGenericPlaceholder(string value)
    {
        return value.StartsWith("!", StringComparison.Ordinal);
    }
}

internal sealed class TypeIdentityProvider : ISignatureTypeProvider<TypeIdentity, SignatureContext<TypeIdentity>?>
{
    private readonly MetadataTypeResolver _resolver;

    public TypeIdentityProvider(MetadataTypeResolver resolver)
    {
        _resolver = resolver;
    }

    public TypeIdentity GetArrayType(TypeIdentity elementType, ArrayShape shape)
    {
        var displayName = shape.Rank == 1 && shape.LowerBounds.IsDefaultOrEmpty && shape.Sizes.IsDefaultOrEmpty
            ? $"{elementType.DisplayName}[]"
            : $"{elementType.DisplayName}[{new string(',', shape.Rank - 1)}]";
        return _resolver.CreateSimpleTypeIdentity(elementType.AssemblyName, displayName);
    }

    public TypeIdentity GetByReferenceType(TypeIdentity elementType)
    {
        return _resolver.CreateSimpleTypeIdentity(elementType.AssemblyName, $"{elementType.DisplayName}&");
    }

    public TypeIdentity GetFunctionPointerType(MethodSignature<TypeIdentity> signature)
    {
        return _resolver.CreateSimpleTypeIdentity(
            _resolver.CurrentAssemblyName,
            FunctionPointerTypeFormatter.Format(
                signature.Header,
                signature.ParameterTypes.Select(parameterType => parameterType.DisplayName),
                signature.ReturnType.DisplayName));
    }

    public TypeIdentity GetGenericInstantiation(TypeIdentity genericType, ImmutableArray<TypeIdentity> typeArguments)
    {
        var argumentDisplayNames = typeArguments.Select(argument => argument.DisplayName).ToImmutableArray();
        return new TypeIdentity
        {
            AssemblyName = genericType.AssemblyName,
            SubjectId = ManagedNaming.CreateInstantiatedTypeSubjectId(genericType.DefinitionSubjectId, argumentDisplayNames),
            DisplayName = ManagedNaming.CreateInstantiatedTypeDisplayName(genericType.DefinitionDisplayName, argumentDisplayNames),
            DefinitionSubjectId = genericType.DefinitionSubjectId,
            DefinitionDisplayName = genericType.DefinitionDisplayName,
            TypeArguments = argumentDisplayNames,
        };
    }

    public TypeIdentity GetGenericMethodParameter(SignatureContext<TypeIdentity>? genericContext, int index)
    {
        return genericContext is not null &&
               index >= 0 &&
               index < genericContext.MethodArguments.Length
            ? genericContext.MethodArguments[index]
            : _resolver.CreateSimpleTypeIdentity(_resolver.CurrentAssemblyName, $"!!{index}");
    }

    public TypeIdentity GetGenericTypeParameter(SignatureContext<TypeIdentity>? genericContext, int index)
    {
        return genericContext is not null &&
               index >= 0 &&
               index < genericContext.TypeArguments.Length
            ? genericContext.TypeArguments[index]
            : _resolver.CreateSimpleTypeIdentity(_resolver.CurrentAssemblyName, $"!{index}");
    }

    public TypeIdentity GetModifiedType(TypeIdentity modifierType, TypeIdentity unmodifiedType, bool isRequired) => unmodifiedType;

    public TypeIdentity GetPinnedType(TypeIdentity elementType) => elementType;

    public TypeIdentity GetPointerType(TypeIdentity elementType)
    {
        return _resolver.CreateSimpleTypeIdentity(elementType.AssemblyName, $"{elementType.DisplayName}*");
    }

    public TypeIdentity GetPrimitiveType(PrimitiveTypeCode typeCode)
    {
        var displayName = typeCode switch
        {
            PrimitiveTypeCode.Boolean => "System.Boolean",
            PrimitiveTypeCode.Byte => "System.Byte",
            PrimitiveTypeCode.Char => "System.Char",
            PrimitiveTypeCode.Double => "System.Double",
            PrimitiveTypeCode.Int16 => "System.Int16",
            PrimitiveTypeCode.Int32 => "System.Int32",
            PrimitiveTypeCode.Int64 => "System.Int64",
            PrimitiveTypeCode.IntPtr => "System.IntPtr",
            PrimitiveTypeCode.Object => "System.Object",
            PrimitiveTypeCode.SByte => "System.SByte",
            PrimitiveTypeCode.Single => "System.Single",
            PrimitiveTypeCode.String => "System.String",
            PrimitiveTypeCode.TypedReference => "System.TypedReference",
            PrimitiveTypeCode.UInt16 => "System.UInt16",
            PrimitiveTypeCode.UInt32 => "System.UInt32",
            PrimitiveTypeCode.UInt64 => "System.UInt64",
            PrimitiveTypeCode.UIntPtr => "System.UIntPtr",
            PrimitiveTypeCode.Void => "System.Void",
            _ => throw new NotSupportedException($"unsupported primitive type: {typeCode}"),
        };

        return _resolver.CreateSimpleTypeIdentity("System.Private.CoreLib", displayName);
    }

    public TypeIdentity GetSZArrayType(TypeIdentity elementType)
    {
        return _resolver.CreateSimpleTypeIdentity(elementType.AssemblyName, $"{elementType.DisplayName}[]");
    }

    public TypeIdentity GetTypeFromDefinition(MetadataReader reader, TypeDefinitionHandle handle, byte rawTypeKind)
    {
        return _resolver.ResolveTypeIdentity(handle);
    }

    public TypeIdentity GetTypeFromReference(MetadataReader reader, TypeReferenceHandle handle, byte rawTypeKind)
    {
        return _resolver.ResolveTypeIdentity(handle);
    }

    public TypeIdentity GetTypeFromSpecification(
        MetadataReader reader,
        SignatureContext<TypeIdentity>? genericContext,
        TypeSpecificationHandle handle,
        byte rawTypeKind)
    {
        return reader.GetTypeSpecification(handle).DecodeSignature(this, genericContext);
    }

    private static bool IsGenericPlaceholder(string value)
    {
        return value.StartsWith("!", StringComparison.Ordinal);
    }
}

internal static class FunctionPointerTypeFormatter
{
    public static string Format(
        SignatureHeader header,
        IEnumerable<string> parameterTypes,
        string returnType)
    {
        var prefix = header.CallingConvention == SignatureCallingConvention.Unmanaged
            ? "delegate* unmanaged"
            : "delegate* managed";
        var signatureParts = parameterTypes
            .Concat([returnType]);
        return $"{prefix}<{string.Join(",", signatureParts)}>";
    }
}

internal sealed class GenericArityTypeProvider : ISignatureTypeProvider<int, object?>
{
    public int GetArrayType(int elementType, ArrayShape shape) => 0;

    public int GetByReferenceType(int elementType) => 0;

    public int GetFunctionPointerType(MethodSignature<int> signature) => 0;

    public int GetGenericInstantiation(int genericType, ImmutableArray<int> typeArguments) => 0;

    public int GetGenericMethodParameter(object? genericContext, int index) => 0;

    public int GetGenericTypeParameter(object? genericContext, int index) => 0;

    public int GetModifiedType(int modifierType, int unmodifiedType, bool isRequired) => 0;

    public int GetPinnedType(int elementType) => 0;

    public int GetPointerType(int elementType) => 0;

    public int GetPrimitiveType(PrimitiveTypeCode typeCode) => 0;

    public int GetSZArrayType(int elementType) => 0;

    public int GetTypeFromDefinition(MetadataReader reader, TypeDefinitionHandle handle, byte rawTypeKind) => 0;

    public int GetTypeFromReference(MetadataReader reader, TypeReferenceHandle handle, byte rawTypeKind) => 0;

    public int GetTypeFromSpecification(MetadataReader reader, object? genericContext, TypeSpecificationHandle handle, byte rawTypeKind) => 0;
}
