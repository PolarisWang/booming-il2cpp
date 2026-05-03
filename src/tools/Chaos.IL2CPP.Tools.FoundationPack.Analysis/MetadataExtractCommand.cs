using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.PortableExecutable;
using System.Text.Json;

namespace Chaos.IL2CPP.Tools.FoundationPack.Analysis;

/// <summary>
/// Extracts public type/method metadata from a managed DLL using PEReader
/// (zero assembly loading — pure System.Reflection.Metadata).
///
/// Output JSON schema:
/// <code>
/// {
///   "assemblyName": "System.Collections.Immutable",
///   "types": {
///     "System.Collections.Immutable.ImmutableArray`1": {
///       "typeName": "ImmutableArray",
///       "fullName": "System.Collections.Immutable.ImmutableArray`1",
///       "arity": 1,
///       "isStatic": false,
///       "isValueType": false,
///       "methods": [
///         {
///           "name": "Create",
///           "paramCount": 1,
///           "paramTypes": ["System.Byte"],
///           "returnType": "System.Collections.Immutable.ImmutableArray`1<System.Byte>",
///           "isStatic": true,
///           "isVirtual": false,
///           "isPropertyAccessor": false,
///           "propertyName": null,
///           "isConstructor": false
///         }
///       ],
///       "properties": ["Empty"]
///     }
///   }
/// }
/// </code>
/// </summary>
internal static class MetadataExtractCommand
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        WriteIndented = true,
    };

    public static int Execute(IReadOnlyDictionary<string, string> options)
    {
        var dllPath = GetRequiredOption(options, "--dll-path");
        var outputPath = GetRequiredOption(options, "--output");

        Console.Error.WriteLine($"Extracting metadata from: {dllPath}");

        using var stream = File.OpenRead(dllPath);
        using var peReader = new PEReader(stream);

        if (!peReader.HasMetadata)
        {
            Console.Error.WriteLine("No metadata in assembly (native DLL?).");
            return 1;
        }

        var metadataReader = peReader.GetMetadataReader();
        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);

        var typeMap = new Dictionary<string, TypeMetadata>(StringComparer.Ordinal);

        foreach (var typeHandle in metadataReader.TypeDefinitions)
        {
            var typeDef = metadataReader.GetTypeDefinition(typeHandle);

            // Skip <Module> type and non-public types
            if (IsModuleType(metadataReader, typeDef) || !IsPublicType(typeDef.Attributes))
                continue;

            var fullName = GetFullTypeName(metadataReader, typeHandle);
            var (typeName, arity) = ParseTypeName(fullName);

            var isStatic = (typeDef.Attributes & TypeAttributes.Sealed) != 0
                        && (typeDef.Attributes & TypeAttributes.Abstract) != 0
                        && (typeDef.Attributes & TypeAttributes.Interface) == 0;
            var isValueType = (typeDef.Attributes & TypeAttributes.SequentialLayout) != 0
                           && (typeDef.Attributes & TypeAttributes.Sealed) != 0;
            // Better value type detection: check if it extends ValueType
            if (!isValueType)
            {
                var baseTypeHandle = typeDef.BaseType;
                if (!baseTypeHandle.IsNil)
                {
                    var baseName = baseTypeHandle.Kind switch
                    {
                        HandleKind.TypeDefinition => metadataReader.GetString(metadataReader.GetTypeDefinition((TypeDefinitionHandle)baseTypeHandle).Name),
                        HandleKind.TypeReference => metadataReader.GetString(metadataReader.GetTypeReference((TypeReferenceHandle)baseTypeHandle).Name),
                        _ => null,
                    };
                    if (baseName == "ValueType" || baseName == "Enum" || baseName == "MulticastDelegate" || baseName == "Delegate")
                        isValueType = true;
                }
            }

            var methods = new List<MethodMetadata>();
            var properties = new List<string>();

            // Read properties first so we can link accessors
            var propMap = new Dictionary<string, PropertyDefinition>(StringComparer.Ordinal);
            foreach (var propHandle in typeDef.GetProperties())
            {
                var prop = metadataReader.GetPropertyDefinition(propHandle);
                var propName = metadataReader.GetString(prop.Name);
                properties.Add(propName);
                propMap[propName] = prop;
            }

            foreach (var methodHandle in typeDef.GetMethods())
            {
                var methodDef = metadataReader.GetMethodDefinition(methodHandle);

                // Skip private/internal methods
                if (!IsPublicMethod(methodDef.Attributes))
                    continue;

                var methodName = metadataReader.GetString(methodDef.Name);

                // Decode signature
                MethodSignature<TypeHandleInfo> signature;
                try
                {
                    var signatureHandle = methodDef.DecodeSignature(new TypeHandleTypeProvider(metadataReader), default);
                    signature = methodDef.DecodeSignature(new TypeHandleTypeProvider(metadataReader), default);
                }
                catch
                {
                    // Skip methods whose signature we can't decode
                    continue;
                }

                var paramTypes = new List<string>();
                foreach (var paramType in signature.ParameterTypes)
                {
                    paramTypes.Add(ResolveTypeName(paramType, metadataReader));
                }

                var returnType = ResolveTypeName(signature.ReturnType, metadataReader);

                var methodIsStatic = (methodDef.Attributes & MethodAttributes.Static) != 0;
                var isVirtual = (methodDef.Attributes & MethodAttributes.Virtual) != 0;
                var isSpecialName = (methodDef.Attributes & MethodAttributes.SpecialName) != 0;

                // Detect property accessor
                var isPropertyAccessor = false;
                string? propertyName = null;
                if (isSpecialName && (methodName.StartsWith("get_") || methodName.StartsWith("set_")))
                {
                    isPropertyAccessor = true;
                    propertyName = methodName.Substring(4);
                }

                var isConstructor = methodName == ".ctor" || methodName == ".cctor";

                methods.Add(new MethodMetadata
                {
                    Name = methodName,
                    ParamCount = paramTypes.Count,
                    ParamTypes = paramTypes.ToArray(),
                    ReturnType = returnType,
                    IsStatic = methodIsStatic,
                    IsVirtual = isVirtual,
                    IsPropertyAccessor = isPropertyAccessor,
                    PropertyName = propertyName,
                    IsConstructor = isConstructor,
                });
            }

            // Skip types with no public methods
            if (methods.Count == 0)
                continue;

            typeMap[fullName] = new TypeMetadata
            {
                TypeName = typeName,
                FullName = fullName,
                Arity = arity,
                IsStatic = isStatic,
                IsValueType = isValueType,
                Methods = methods.ToArray(),
                Properties = properties.ToArray(),
            };
        }

        var result = new AssemblyMetadataResult
        {
            AssemblyName = assemblyName,
            Types = typeMap,
        };

        var json = JsonSerializer.Serialize(result, JsonOptions);
        File.WriteAllText(outputPath, json);

        Console.Error.WriteLine($"Output written to: {outputPath}");
        Console.Error.WriteLine($"  Types: {typeMap.Count}");
        Console.Error.WriteLine($"  Methods: {typeMap.Values.Sum(t => t.Methods.Length)}");

        return 0;
    }

    private static string GetFullTypeName(MetadataReader reader, TypeDefinitionHandle handle)
    {
        var type = reader.GetTypeDefinition(handle);
        var name = reader.GetString(type.Name);
        var ns = reader.GetString(type.Namespace);
        var declaring = type.GetDeclaringType();
        if (!declaring.IsNil)
            return $"{GetFullTypeName(reader, declaring)}+{name}";
        return string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
    }

    private static (string shortName, int arity) ParseTypeName(string fullName)
    {
        // Extract short name from last segment
        var lastDot = fullName.LastIndexOf('.');
        var lastName = lastDot >= 0 ? fullName.Substring(lastDot + 1) : fullName;

        // Handle nested types
        var plusIdx = lastName.LastIndexOf('+');
        if (plusIdx >= 0)
            lastName = lastName.Substring(plusIdx + 1);

        // Extract backtick arity
        var btIdx = lastName.IndexOf('`');
        if (btIdx >= 0 && int.TryParse(lastName.AsSpan(btIdx + 1), out var arity))
            return (lastName.Substring(0, btIdx), arity);

        return (lastName, 0);
    }

    private static string ResolveTypeName(TypeHandleInfo typeInfo, MetadataReader reader)
    {
        return typeInfo.Kind switch
        {
            TypeHandleInfoKind.Primitive => ((PrimitiveTypeHandleInfo)typeInfo).PrimitiveTypeCode switch
            {
                PrimitiveTypeCode.Boolean => "System.Boolean",
                PrimitiveTypeCode.Byte => "System.Byte",
                PrimitiveTypeCode.SByte => "System.SByte",
                PrimitiveTypeCode.Int16 => "System.Int16",
                PrimitiveTypeCode.UInt16 => "System.UInt16",
                PrimitiveTypeCode.Int32 => "System.Int32",
                PrimitiveTypeCode.UInt32 => "System.UInt32",
                PrimitiveTypeCode.Int64 => "System.Int64",
                PrimitiveTypeCode.UInt64 => "System.UInt64",
                PrimitiveTypeCode.Single => "System.Single",
                PrimitiveTypeCode.Double => "System.Double",
                PrimitiveTypeCode.Char => "System.Char",
                PrimitiveTypeCode.String => "System.String",
                PrimitiveTypeCode.IntPtr => "System.IntPtr",
                PrimitiveTypeCode.UIntPtr => "System.UIntPtr",
                PrimitiveTypeCode.Object => "System.Object",
                PrimitiveTypeCode.Void => "System.Void",
                PrimitiveTypeCode.TypedReference => "System.TypedReference",
                _ => typeInfo.Kind.ToString(),
            },
            TypeHandleInfoKind.Type => ResolveTypeDefOrRef(((TypeHandleInfo)typeInfo).Handle, reader),
            TypeHandleInfoKind.GenericTypeArgument => "!!0",  // generic type parameter reference
            TypeHandleInfoKind.GenericTypeParameter => ((GenericTypeParameterHandleInfo)typeInfo).Index switch
            {
                // !!N = method-level generic parameter
                var idx when ((GenericTypeParameterHandleInfo)typeInfo).IsMethodGeneric => $"!!{idx}",
                // !N = type-level generic parameter
                var idx => $"!{idx}",
            },
            TypeHandleInfoKind.TypeSpecification => ResolveTypeSpecification((TypeSpecificationHandle)((TypeHandleInfo)typeInfo).Handle, reader),
            _ => typeInfo.Kind.ToString(),
        };
    }

    private static string ResolveTypeDefOrRef(EntityHandle handle, MetadataReader reader)
    {
        switch (handle.Kind)
        {
            case HandleKind.TypeDefinition:
                var typeDef = reader.GetTypeDefinition((TypeDefinitionHandle)handle);
                var name = reader.GetString(typeDef.Name);
                var ns = reader.GetString(typeDef.Namespace);
                return string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";

            case HandleKind.TypeReference:
                var typeRef = reader.GetTypeReference((TypeReferenceHandle)handle);
                name = reader.GetString(typeRef.Name);
                ns = reader.GetString(typeRef.Namespace);
                return string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";

            default:
                return handle.Kind.ToString();
        }
    }

    private static string ResolveTypeSpecification(TypeSpecificationHandle handle, MetadataReader reader)
    {
        var typeSpec = reader.GetTypeSpecification(handle);
        var provider = new TypeHandleTypeProvider(reader);
        var signature = typeSpec.DecodeSignature(provider, default);

        // Build a friendly name for the type specification
        var result = ResolveTypeName(signature, reader);

        // Handle generic instantiation
        if (signature.Kind == TypeHandleInfoKind.Type &&
            signature.Handle.Kind == HandleKind.TypeDefinition)
        {
            var typeDef = reader.GetTypeDefinition((TypeDefinitionHandle)signature.Handle);
            var baseName = reader.GetString(typeDef.Name);
            var ns = reader.GetString(typeDef.Namespace);
            var fullName = string.IsNullOrEmpty(ns) ? baseName : $"{ns}.{baseName}";

            // Check if this is a generic instantiation by looking at the type's generic params
            var genericParamCount = 0;
            foreach (var gpHandle in typeDef.GetGenericParameters())
                genericParamCount++;

            if (genericParamCount > 0)
            {
                // We need to detect the generic arguments from the signature.
                // This is complex; for now, return the base name and let the caller handle it.
                return fullName;
            }

            return fullName;
        }

        return result;
    }

    private static bool IsModuleType(MetadataReader reader, TypeDefinition typeDef)
    {
        return (typeDef.Attributes & TypeAttributes.SpecialName) != 0
            && reader.GetString(typeDef.Name) == "<Module>";
    }

    private static bool IsPublicType(TypeAttributes attributes)
    {
        var visibility = attributes & TypeAttributes.VisibilityMask;
        return visibility == TypeAttributes.Public
            || visibility == TypeAttributes.NestedPublic;
    }

    private static bool IsPublicMethod(MethodAttributes attributes)
    {
        var visibility = attributes & MethodAttributes.MemberAccessMask;
        return visibility == MethodAttributes.Public
            || visibility == MethodAttributes.Family
            || visibility == MethodAttributes.FamORAssem;
    }

    private static string GetRequiredOption(IReadOnlyDictionary<string, string> options, string key)
    {
        if (!options.TryGetValue(key, out var value) || string.IsNullOrWhiteSpace(value))
            throw new ArgumentException($"missing required option: {key}");
        return value;
    }

    // Lightweight type provider that maps handles to TypeHandleInfo
    private sealed class TypeHandleTypeProvider : ISignatureTypeProvider<TypeHandleInfo, object?>
    {
        private readonly MetadataReader _reader;

        public TypeHandleTypeProvider(MetadataReader reader) => _reader = reader;

        public TypeHandleInfo GetPrimitiveType(PrimitiveTypeCode typeCode)
            => new PrimitiveTypeHandleInfo(typeCode);

        public TypeHandleInfo GetTypeFromDefinition(MetadataReader reader, TypeDefinitionHandle handle, byte rawTypeKind)
            => new TypeHandleInfo(TypeHandleInfoKind.Type, handle);

        public TypeHandleInfo GetTypeFromReference(MetadataReader reader, TypeReferenceHandle handle, byte rawTypeKind)
            => new TypeHandleInfo(TypeHandleInfoKind.Type, handle);

        public TypeHandleInfo GetTypeFromSpecification(MetadataReader reader, object? genericContext, TypeSpecificationHandle handle, byte rawTypeKind)
            => new TypeHandleInfo(TypeHandleInfoKind.TypeSpecification, handle);

        public TypeHandleInfo GetSZArrayType(TypeHandleInfo elementType)
            => elementType;  // simplified; full support would wrap

        public TypeHandleInfo GetArrayType(TypeHandleInfo elementType, ArrayShape shape)
            => elementType;  // simplified; full support would wrap

        public TypeHandleInfo GetGenericInstantiation(TypeHandleInfo genericType, System.Collections.Immutable.ImmutableArray<TypeHandleInfo> typeArguments)
            => genericType;

        public TypeHandleInfo GetArrayType(TypeHandleInfo elementType, System.Collections.Immutable.ImmutableArray<int> ranks)
            => elementType;

        public TypeHandleInfo GetByReferenceType(TypeHandleInfo elementType)
            => elementType;

        public TypeHandleInfo GetPointerType(TypeHandleInfo elementType)
            => elementType;

        public TypeHandleInfo GetPinnedType(TypeHandleInfo elementType)
            => elementType;

        public TypeHandleInfo GetGenericMethodParameter(object? genericContext, int index)
            => new GenericTypeParameterHandleInfo(index, isMethodGeneric: true);

        public TypeHandleInfo GetGenericTypeParameter(object? genericContext, int index)
            => new GenericTypeParameterHandleInfo(index, isMethodGeneric: false);

        public TypeHandleInfo GetFunctionPointerType(MethodSignature<TypeHandleInfo> signature)
            => new TypeHandleInfo(TypeHandleInfoKind.Primitive, default);

        public TypeHandleInfo GetModifiedType(TypeHandleInfo modifier, TypeHandleInfo unmodifiedType, bool isRequired)
            => unmodifiedType;

        public TypeHandleInfo GetTypeFromHandle(MetadataReader reader, EntityHandle handle)
            => new TypeHandleInfo(TypeHandleInfoKind.Type, handle);
    }
}

internal enum TypeHandleInfoKind
{
    Primitive,
    Type,
    GenericTypeParameter,
    GenericTypeArgument,
    TypeSpecification,
}

internal class TypeHandleInfo
{
    public TypeHandleInfoKind Kind { get; }
    public EntityHandle Handle { get; }

    public TypeHandleInfo(TypeHandleInfoKind kind, EntityHandle handle)
    {
        Kind = kind;
        Handle = handle;
    }
}

internal sealed class PrimitiveTypeHandleInfo : TypeHandleInfo
{
    public PrimitiveTypeCode PrimitiveTypeCode { get; }

    public PrimitiveTypeHandleInfo(PrimitiveTypeCode typeCode)
        : base(TypeHandleInfoKind.Primitive, default)
    {
        PrimitiveTypeCode = typeCode;
    }
}

internal sealed class GenericTypeParameterHandleInfo : TypeHandleInfo
{
    public int Index { get; }
    public bool IsMethodGeneric { get; }

    public GenericTypeParameterHandleInfo(int index, bool isMethodGeneric)
        : base(TypeHandleInfoKind.GenericTypeParameter, default)
    {
        Index = index;
        IsMethodGeneric = isMethodGeneric;
    }
}

// --- JSON models ---

internal sealed record AssemblyMetadataResult
{
    public string AssemblyName { get; init; } = "";
    public Dictionary<string, TypeMetadata> Types { get; init; } = new(StringComparer.Ordinal);
}

internal sealed record TypeMetadata
{
    public string TypeName { get; init; } = "";
    public string FullName { get; init; } = "";
    public int Arity { get; init; }
    public bool IsStatic { get; init; }
    public bool IsValueType { get; init; }
    public MethodMetadata[] Methods { get; init; } = [];
    public string[] Properties { get; init; } = [];
}

internal sealed record MethodMetadata
{
    public string Name { get; init; } = "";
    public int ParamCount { get; init; }
    public string[] ParamTypes { get; init; } = [];
    public string ReturnType { get; init; } = "";
    public bool IsStatic { get; init; }
    public bool IsVirtual { get; init; }
    public bool IsPropertyAccessor { get; init; }
    public string? PropertyName { get; init; }
    public bool IsConstructor { get; init; }
}
