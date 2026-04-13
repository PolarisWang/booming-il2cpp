using System.Collections.Immutable;
using System.Reflection.Metadata;
using System.Reflection.PortableExecutable;
using System.Text.Json;

namespace Chaos.IL2CPP.DeclarationDiscovery;

internal static class Program
{
    private const string FrameworkAssemblyName = "Chaos.TestFramework";
    private const string UnitTestAttributeName = "Chaos.TestFramework.ChaosUnitTestAttribute";
    private const string BenchmarkAttributeName = "Chaos.TestFramework.ChaosBenchmarkAttribute";

    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
    };

    public static int Main(string[] args)
    {
        if (args.Length > 0 && string.Equals(args[0], "--", StringComparison.Ordinal))
        {
            args = args[1..];
        }

        args = ExpandAssemblyArguments(args);
        if (args.Length == 0)
        {
            Console.Error.WriteLine("usage: Chaos.IL2CPP.DeclarationDiscovery <assembly-path> [<assembly-path> ...]");
            return 1;
        }

        try
        {
            var payload = ScanAssemblies(args);
            Console.Out.Write(JsonSerializer.Serialize(payload, JsonOptions));
            return 0;
        }
        catch (Exception error)
        {
            Console.Error.WriteLine(error.ToString());
            return 1;
        }
    }

    private static string[] ExpandAssemblyArguments(string[] args)
    {
        if (args.Length == 1 && File.Exists(args[0]) && !IsManagedAssemblyPath(args[0]))
        {
            return File.ReadAllLines(args[0])
                .Select(line => line.Trim())
                .Where(line => !string.IsNullOrWhiteSpace(line))
                .ToArray();
        }

        if (args.Length == 2 && string.Equals(args[0], "--assembly-list", StringComparison.Ordinal))
        {
            var listPath = args[1];
            if (!File.Exists(listPath))
            {
                throw new FileNotFoundException($"assembly list missing: {listPath}");
            }

            return File.ReadAllLines(listPath)
                .Select(line => line.Trim())
                .Where(line => !string.IsNullOrWhiteSpace(line))
                .ToArray();
        }

        return args;
    }

    private static bool IsManagedAssemblyPath(string path)
    {
        var extension = Path.GetExtension(path);
        return string.Equals(extension, ".dll", StringComparison.OrdinalIgnoreCase) ||
               string.Equals(extension, ".exe", StringComparison.OrdinalIgnoreCase);
    }

    private static DeclarationScanPayload ScanAssemblies(IReadOnlyList<string> assemblyPaths)
    {
        var payload = new DeclarationScanPayload();
        foreach (var assemblyPath in assemblyPaths.Distinct(StringComparer.OrdinalIgnoreCase))
        {
            ScanAssembly(assemblyPath, payload);
        }

        payload.DeclaredUnitTests = payload.DeclaredUnitTests
            .OrderBy(entry => entry.AssemblyName, StringComparer.Ordinal)
            .ThenBy(entry => entry.DeclaringType, StringComparer.Ordinal)
            .ThenBy(entry => entry.MethodName, StringComparer.Ordinal)
            .ToList();
        payload.DeclaredBenchmarks = payload.DeclaredBenchmarks
            .OrderBy(entry => entry.AssemblyName, StringComparer.Ordinal)
            .ThenBy(entry => entry.DeclaringType, StringComparer.Ordinal)
            .ThenBy(entry => entry.MethodName, StringComparer.Ordinal)
            .ToList();
        return payload;
    }

    private static void ScanAssembly(string assemblyPath, DeclarationScanPayload payload)
    {
        if (!File.Exists(assemblyPath))
        {
            throw new FileNotFoundException($"assembly missing: {assemblyPath}");
        }

        using var stream = File.OpenRead(assemblyPath);
        using var peReader = new PEReader(stream);
        var metadataReader = peReader.GetMetadataReader();
        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);

        if (ReferencesFramework(metadataReader))
        {
            payload.FrameworkReferenced = true;
        }

        foreach (var typeHandle in metadataReader.TypeDefinitions)
        {
            var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);
            var typeName = metadataReader.GetString(typeDefinition.Name);
            if (string.Equals(typeName, "<Module>", StringComparison.Ordinal))
            {
                continue;
            }

            var declaringType = FullTypeName(metadataReader, typeHandle);
            foreach (var methodHandle in typeDefinition.GetMethods())
            {
                var methodDefinition = metadataReader.GetMethodDefinition(methodHandle);
                var methodName = metadataReader.GetString(methodDefinition.Name);
                var methodSignature = BuildMethodSignature(metadataReader, methodHandle);

                foreach (var attributeHandle in methodDefinition.GetCustomAttributes())
                {
                    if (!TryGetAttributeTypeName(metadataReader, attributeHandle, out var attributeTypeName))
                    {
                        continue;
                    }

                    if (string.Equals(attributeTypeName, UnitTestAttributeName, StringComparison.Ordinal))
                    {
                        payload.FrameworkReferenced = true;
                        payload.DeclaredUnitTests.Add(CreateUnitEntry(
                            metadataReader,
                            attributeHandle,
                            assemblyName,
                            declaringType,
                            methodName,
                            methodSignature));
                    }
                    else if (string.Equals(attributeTypeName, BenchmarkAttributeName, StringComparison.Ordinal))
                    {
                        payload.FrameworkReferenced = true;
                        payload.DeclaredBenchmarks.Add(CreateBenchmarkEntry(
                            metadataReader,
                            attributeHandle,
                            assemblyName,
                            declaringType,
                            methodName,
                            methodSignature));
                    }
                }
            }
        }
    }

    private static bool ReferencesFramework(MetadataReader metadataReader)
    {
        foreach (var handle in metadataReader.AssemblyReferences)
        {
            var reference = metadataReader.GetAssemblyReference(handle);
            var name = metadataReader.GetString(reference.Name);
            if (string.Equals(name, FrameworkAssemblyName, StringComparison.Ordinal))
            {
                return true;
            }
        }

        return false;
    }

    private static DeclaredUnitEntry CreateUnitEntry(
        MetadataReader metadataReader,
        CustomAttributeHandle attributeHandle,
        string assemblyName,
        string declaringType,
        string methodName,
        string methodSignature)
    {
        var reader = CreateAttributeValueReader(metadataReader, attributeHandle);
        var entry = new DeclaredUnitEntry
        {
            AssemblyName = assemblyName,
            DeclaringType = declaringType,
            MethodName = methodName,
            MethodSignature = methodSignature,
            Category = reader.ReadByte(),
        };
        ReadUnitNamedArguments(ref reader, entry);
        return entry;
    }

    private static DeclaredBenchmarkEntry CreateBenchmarkEntry(
        MetadataReader metadataReader,
        CustomAttributeHandle attributeHandle,
        string assemblyName,
        string declaringType,
        string methodName,
        string methodSignature)
    {
        var reader = CreateAttributeValueReader(metadataReader, attributeHandle);
        var entry = new DeclaredBenchmarkEntry
        {
            AssemblyName = assemblyName,
            DeclaringType = declaringType,
            MethodName = methodName,
            MethodSignature = methodSignature,
            Category = reader.ReadByte(),
            Metrics = reader.ReadUInt16(),
            Modes = 7,
        };
        ReadBenchmarkNamedArguments(ref reader, entry);
        return entry;
    }

    private static BlobReader CreateAttributeValueReader(MetadataReader metadataReader, CustomAttributeHandle attributeHandle)
    {
        var attribute = metadataReader.GetCustomAttribute(attributeHandle);
        var reader = metadataReader.GetBlobReader(attribute.Value);
        if (reader.ReadUInt16() != 1)
        {
            throw new BadImageFormatException("custom attribute signature prolog is invalid");
        }

        return reader;
    }

    private static void ReadUnitNamedArguments(ref BlobReader reader, DeclaredUnitEntry entry)
    {
        var namedArgumentCount = reader.ReadUInt16();
        for (var index = 0; index < namedArgumentCount; index++)
        {
            _ = reader.ReadByte();
            var typeCode = ReadFieldOrPropertyType(ref reader);
            var name = reader.ReadSerializedString() ?? string.Empty;
            switch (name)
            {
                case "Alias":
                    entry.Alias = reader.ReadSerializedString();
                    break;
                case "Requires":
                    entry.Requires = reader.ReadUInt32();
                    break;
                case "Evidence":
                    entry.Evidence = reader.ReadUInt16();
                    break;
                case "Priority":
                    entry.Priority = reader.ReadByte();
                    break;
                default:
                    SkipArgumentValue(ref reader, typeCode);
                    break;
            }
        }
    }

    private static void ReadBenchmarkNamedArguments(ref BlobReader reader, DeclaredBenchmarkEntry entry)
    {
        var namedArgumentCount = reader.ReadUInt16();
        for (var index = 0; index < namedArgumentCount; index++)
        {
            _ = reader.ReadByte();
            var typeCode = ReadFieldOrPropertyType(ref reader);
            var name = reader.ReadSerializedString() ?? string.Empty;
            switch (name)
            {
                case "Alias":
                    entry.Alias = reader.ReadSerializedString();
                    break;
                case "Requires":
                    entry.Requires = reader.ReadUInt32();
                    break;
                case "Modes":
                    entry.Modes = reader.ReadByte();
                    break;
                case "WarmupCount":
                    entry.WarmupCount = reader.ReadByte();
                    break;
                case "IterationCount":
                    entry.IterationCount = reader.ReadUInt16();
                    break;
                case "InvocationCount":
                    entry.InvocationCount = reader.ReadUInt16();
                    break;
                default:
                    SkipArgumentValue(ref reader, typeCode);
                    break;
            }
        }
    }

    private static SerializationTypeCode ReadFieldOrPropertyType(ref BlobReader reader)
    {
        var typeCode = (SerializationTypeCode)reader.ReadByte();
        if (typeCode == SerializationTypeCode.Enum)
        {
            return EnumUnderlyingType(reader.ReadSerializedString());
        }

        return typeCode;
    }

    private static SerializationTypeCode EnumUnderlyingType(string? enumTypeName)
    {
        if (string.IsNullOrWhiteSpace(enumTypeName))
        {
            return SerializationTypeCode.Int32;
        }

        if (enumTypeName.EndsWith("ChaosUnitCategory", StringComparison.Ordinal))
        {
            return SerializationTypeCode.Byte;
        }
        if (enumTypeName.EndsWith("ChaosBenchmarkCategory", StringComparison.Ordinal))
        {
            return SerializationTypeCode.Byte;
        }
        if (enumTypeName.EndsWith("ChaosMetric", StringComparison.Ordinal))
        {
            return SerializationTypeCode.UInt16;
        }
        if (enumTypeName.EndsWith("ChaosExecutionMode", StringComparison.Ordinal))
        {
            return SerializationTypeCode.Byte;
        }
        if (enumTypeName.EndsWith("ChaosRuntimeFeature", StringComparison.Ordinal))
        {
            return SerializationTypeCode.UInt32;
        }
        if (enumTypeName.EndsWith("ChaosEvidenceKind", StringComparison.Ordinal))
        {
            return SerializationTypeCode.UInt16;
        }

        return SerializationTypeCode.Int32;
    }

    private static void SkipArgumentValue(ref BlobReader reader, SerializationTypeCode typeCode)
    {
        switch (typeCode)
        {
            case SerializationTypeCode.Boolean:
                _ = reader.ReadBoolean();
                return;
            case SerializationTypeCode.Byte:
                _ = reader.ReadByte();
                return;
            case SerializationTypeCode.Char:
                _ = reader.ReadUInt16();
                return;
            case SerializationTypeCode.Int16:
                _ = reader.ReadInt16();
                return;
            case SerializationTypeCode.Int32:
                _ = reader.ReadInt32();
                return;
            case SerializationTypeCode.Int64:
                _ = reader.ReadInt64();
                return;
            case SerializationTypeCode.SByte:
                _ = reader.ReadSByte();
                return;
            case SerializationTypeCode.Single:
                _ = reader.ReadSingle();
                return;
            case SerializationTypeCode.Double:
                _ = reader.ReadDouble();
                return;
            case SerializationTypeCode.String:
                _ = reader.ReadSerializedString();
                return;
            case SerializationTypeCode.UInt16:
                _ = reader.ReadUInt16();
                return;
            case SerializationTypeCode.UInt32:
                _ = reader.ReadUInt32();
                return;
            case SerializationTypeCode.UInt64:
                _ = reader.ReadUInt64();
                return;
            default:
                throw new BadImageFormatException($"unsupported attribute argument type: {typeCode}");
        }
    }

    private static string BuildMethodSignature(MetadataReader metadataReader, MethodDefinitionHandle methodHandle)
    {
        var method = metadataReader.GetMethodDefinition(methodHandle);
        var methodName = metadataReader.GetString(method.Name);
        var decodedSignature = method.DecodeSignature(SignatureTypeProvider.Instance, null);
        var parameterList = string.Join(", ", decodedSignature.ParameterTypes);
        return $"{methodName}({parameterList})";
    }

    internal static string FullTypeName(MetadataReader metadataReader, TypeDefinitionHandle typeHandle)
    {
        var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);
        var typeName = metadataReader.GetString(typeDefinition.Name);
        var namespaceName = metadataReader.GetString(typeDefinition.Namespace);
        var declaringTypeHandle = typeDefinition.GetDeclaringType();
        if (!declaringTypeHandle.IsNil)
        {
            return $"{FullTypeName(metadataReader, declaringTypeHandle)}+{typeName}";
        }

        return string.IsNullOrEmpty(namespaceName) ? typeName : $"{namespaceName}.{typeName}";
    }

    internal static string FullTypeName(MetadataReader metadataReader, TypeReferenceHandle typeHandle)
    {
        var typeReference = metadataReader.GetTypeReference(typeHandle);
        var typeName = metadataReader.GetString(typeReference.Name);
        var namespaceName = metadataReader.GetString(typeReference.Namespace);
        if (typeReference.ResolutionScope.Kind == HandleKind.TypeReference)
        {
            return $"{FullTypeName(metadataReader, (TypeReferenceHandle)typeReference.ResolutionScope)}+{typeName}";
        }

        return string.IsNullOrEmpty(namespaceName) ? typeName : $"{namespaceName}.{typeName}";
    }

    private static bool TryGetAttributeTypeName(
        MetadataReader metadataReader,
        CustomAttributeHandle attributeHandle,
        out string attributeTypeName)
    {
        var attribute = metadataReader.GetCustomAttribute(attributeHandle);
        return TryGetAttributeTypeName(metadataReader, attribute.Constructor, out attributeTypeName);
    }

    private static bool TryGetAttributeTypeName(
        MetadataReader metadataReader,
        EntityHandle constructorHandle,
        out string attributeTypeName)
    {
        switch (constructorHandle.Kind)
        {
            case HandleKind.MemberReference:
                var memberReference = metadataReader.GetMemberReference((MemberReferenceHandle)constructorHandle);
                return TryGetTypeName(metadataReader, memberReference.Parent, out attributeTypeName);
            case HandleKind.MethodDefinition:
                var methodDefinition = metadataReader.GetMethodDefinition((MethodDefinitionHandle)constructorHandle);
                return TryGetTypeName(metadataReader, methodDefinition.GetDeclaringType(), out attributeTypeName);
            default:
                attributeTypeName = string.Empty;
                return false;
        }
    }

    private static bool TryGetTypeName(
        MetadataReader metadataReader,
        EntityHandle typeHandle,
        out string typeName)
    {
        switch (typeHandle.Kind)
        {
            case HandleKind.TypeDefinition:
                typeName = FullTypeName(metadataReader, (TypeDefinitionHandle)typeHandle);
                return true;
            case HandleKind.TypeReference:
                typeName = FullTypeName(metadataReader, (TypeReferenceHandle)typeHandle);
                return true;
            default:
                typeName = string.Empty;
                return false;
        }
    }

    private static byte ToByte(object? value)
    {
        return value switch
        {
            byte byteValue => byteValue,
            sbyte byteValue => unchecked((byte)byteValue),
            short shortValue => checked((byte)shortValue),
            ushort shortValue => checked((byte)shortValue),
            int intValue => checked((byte)intValue),
            uint intValue => checked((byte)intValue),
            long longValue => checked((byte)longValue),
            ulong longValue => checked((byte)longValue),
            _ => 0,
        };
    }

    private static ushort ToUInt16(object? value)
    {
        return value switch
        {
            byte byteValue => byteValue,
            sbyte byteValue => checked((ushort)byteValue),
            short shortValue => checked((ushort)shortValue),
            ushort shortValue => shortValue,
            int intValue => checked((ushort)intValue),
            uint intValue => checked((ushort)intValue),
            long longValue => checked((ushort)longValue),
            ulong longValue => checked((ushort)longValue),
            _ => 0,
        };
    }

    private static uint ToUInt32(object? value)
    {
        return value switch
        {
            byte byteValue => byteValue,
            sbyte byteValue => checked((uint)byteValue),
            short shortValue => checked((uint)shortValue),
            ushort shortValue => shortValue,
            int intValue => checked((uint)intValue),
            uint intValue => intValue,
            long longValue => checked((uint)longValue),
            ulong longValue => checked((uint)longValue),
            _ => 0U,
        };
    }
}

internal sealed class DeclarationScanPayload
{
    public bool FrameworkReferenced { get; set; }

    public List<DeclaredUnitEntry> DeclaredUnitTests { get; set; } = [];

    public List<DeclaredBenchmarkEntry> DeclaredBenchmarks { get; set; } = [];
}

internal class DeclaredEntryBase
{
    public string AssemblyName { get; set; } = string.Empty;

    public string DeclaringType { get; set; } = string.Empty;

    public string MethodName { get; set; } = string.Empty;

    public string MethodSignature { get; set; } = string.Empty;

    public string? Alias { get; set; }

    public byte Category { get; set; }

    public uint Requires { get; set; }
}

internal sealed class DeclaredUnitEntry : DeclaredEntryBase
{
    public ushort Evidence { get; set; }

    public byte Priority { get; set; }
}

internal sealed class DeclaredBenchmarkEntry : DeclaredEntryBase
{
    public ushort Metrics { get; set; }

    public byte Modes { get; set; }

    public byte WarmupCount { get; set; }

    public ushort IterationCount { get; set; }

    public ushort InvocationCount { get; set; }
}

internal sealed class AttributeTypeProvider : ICustomAttributeTypeProvider<string>
{
    public static readonly AttributeTypeProvider Instance = new();

    private AttributeTypeProvider()
    {
    }

    public string GetPrimitiveType(PrimitiveTypeCode typeCode)
    {
        return typeCode switch
        {
            PrimitiveTypeCode.Boolean => "bool",
            PrimitiveTypeCode.Byte => "byte",
            PrimitiveTypeCode.Char => "char",
            PrimitiveTypeCode.Double => "double",
            PrimitiveTypeCode.Int16 => "short",
            PrimitiveTypeCode.Int32 => "int",
            PrimitiveTypeCode.Int64 => "long",
            PrimitiveTypeCode.Object => "object",
            PrimitiveTypeCode.SByte => "sbyte",
            PrimitiveTypeCode.Single => "float",
            PrimitiveTypeCode.String => "string",
            PrimitiveTypeCode.UInt16 => "ushort",
            PrimitiveTypeCode.UInt32 => "uint",
            PrimitiveTypeCode.UInt64 => "ulong",
            _ => typeCode.ToString(),
        };
    }

    public string GetSystemType()
    {
        return "System.Type";
    }

    public string GetSZArrayType(string elementType)
    {
        return $"{elementType}[]";
    }

    public string GetTypeFromDefinition(MetadataReader reader, TypeDefinitionHandle handle, byte rawTypeKind)
    {
        return Program.FullTypeName(reader, handle);
    }

    public string GetTypeFromReference(MetadataReader reader, TypeReferenceHandle handle, byte rawTypeKind)
    {
        return Program.FullTypeName(reader, handle);
    }

    public string GetTypeFromSerializedName(string name)
    {
        return name;
    }

    public PrimitiveTypeCode GetUnderlyingEnumType(string type)
    {
        return type switch
        {
            "Chaos.TestFramework.ChaosUnitCategory" => PrimitiveTypeCode.Byte,
            "Chaos.TestFramework.ChaosBenchmarkCategory" => PrimitiveTypeCode.Byte,
            "Chaos.TestFramework.ChaosMetric" => PrimitiveTypeCode.UInt16,
            "Chaos.TestFramework.ChaosExecutionMode" => PrimitiveTypeCode.Byte,
            "Chaos.TestFramework.ChaosRuntimeFeature" => PrimitiveTypeCode.UInt32,
            "Chaos.TestFramework.ChaosEvidenceKind" => PrimitiveTypeCode.UInt16,
            _ => PrimitiveTypeCode.Int32,
        };
    }

    public bool IsSystemType(string type)
    {
        return string.Equals(type, "System.Type", StringComparison.Ordinal);
    }
}

internal sealed class SignatureTypeProvider : ISignatureTypeProvider<string, object?>
{
    public static readonly SignatureTypeProvider Instance = new();

    private SignatureTypeProvider()
    {
    }

    public string GetArrayType(string elementType, ArrayShape shape)
    {
        return $"{elementType}[{new string(',', Math.Max(0, shape.Rank - 1))}]";
    }

    public string GetByReferenceType(string elementType)
    {
        return $"ref {elementType}";
    }

    public string GetFunctionPointerType(MethodSignature<string> signature)
    {
        return "fnptr";
    }

    public string GetGenericInstantiation(string genericType, ImmutableArray<string> typeArguments)
    {
        return $"{genericType}<{string.Join(", ", typeArguments)}>";
    }

    public string GetGenericMethodParameter(object? genericContext, int index)
    {
        return $"!!{index}";
    }

    public string GetGenericTypeParameter(object? genericContext, int index)
    {
        return $"!{index}";
    }

    public string GetModifiedType(string modifier, string unmodifiedType, bool isRequired)
    {
        return unmodifiedType;
    }

    public string GetPinnedType(string elementType)
    {
        return elementType;
    }

    public string GetPointerType(string elementType)
    {
        return $"{elementType}*";
    }

    public string GetPrimitiveType(PrimitiveTypeCode typeCode)
    {
        return typeCode switch
        {
            PrimitiveTypeCode.Boolean => "bool",
            PrimitiveTypeCode.Byte => "byte",
            PrimitiveTypeCode.Char => "char",
            PrimitiveTypeCode.Double => "double",
            PrimitiveTypeCode.Int16 => "short",
            PrimitiveTypeCode.Int32 => "int",
            PrimitiveTypeCode.Int64 => "long",
            PrimitiveTypeCode.Object => "object",
            PrimitiveTypeCode.SByte => "sbyte",
            PrimitiveTypeCode.Single => "float",
            PrimitiveTypeCode.String => "string",
            PrimitiveTypeCode.UInt16 => "ushort",
            PrimitiveTypeCode.UInt32 => "uint",
            PrimitiveTypeCode.UInt64 => "ulong",
            PrimitiveTypeCode.Void => "void",
            _ => typeCode.ToString(),
        };
    }

    public string GetSZArrayType(string elementType)
    {
        return $"{elementType}[]";
    }

    public string GetTypeFromDefinition(MetadataReader reader, TypeDefinitionHandle handle, byte rawTypeKind)
    {
        return Program.FullTypeName(reader, handle);
    }

    public string GetTypeFromReference(MetadataReader reader, TypeReferenceHandle handle, byte rawTypeKind)
    {
        return Program.FullTypeName(reader, handle);
    }

    public string GetTypeFromSpecification(MetadataReader reader, object? genericContext, TypeSpecificationHandle handle, byte rawTypeKind)
    {
        return reader.GetTypeSpecification(handle).DecodeSignature(this, genericContext);
    }

}
