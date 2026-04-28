using System.Collections.Immutable;
using System.Reflection.Metadata;
using System.Reflection.PortableExecutable;
using System.Text.Json;

namespace Chaos.IL2CPP.Tools.TestFramework.CollectionGen;

internal static class Program
{
    private const string FrameworkAssemblyName = "Chaos.TestFramework.Sdk";
    private const string FrameworkNamespace = "Chaos.TestFramework";
    private const string CapabilityTestAttributeName = FrameworkNamespace + ".CapabilityTestAttribute";
    private const string UnitTestAttributeName = FrameworkNamespace + ".ChaosUnitTestAttribute";
    private const string BenchmarkAttributeName = FrameworkNamespace + ".BenchmarkAttribute";
    private const string LegacyBenchmarkAttributeName = FrameworkNamespace + ".ChaosBenchmarkAttribute";
    private const string NamedArgumentAlias = "Alias";
    private const string NamedArgumentCapabilityFamily = "CapabilityFamily";
    private const string NamedArgumentCapability = "Capability";
    private const string NamedArgumentRequires = "Requires";
    private const string NamedArgumentArchetype = "Archetype";
    private const string NamedArgumentHotUpdateCapability = "HotUpdateCapability";
    private const string NamedArgumentEvidence = "Evidence";
    private const string NamedArgumentPriority = "Priority";
    private const string NamedArgumentModes = "Modes";
    private const string NamedArgumentWarmupCount = "WarmupCount";
    private const string NamedArgumentIterationCount = "IterationCount";
    private const string NamedArgumentInvocationCount = "InvocationCount";

    private delegate void NamedArgumentReader(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode);

    private static readonly DeclarationAttributeSchema UnitTestSchema = new(UnitTestAttributeName, DeclaredAttributeKind.UnitTest);
    private static readonly DeclarationAttributeSchema BenchmarkSchema = new(BenchmarkAttributeName, DeclaredAttributeKind.Benchmark);
    private static readonly DeclarationAttributeSchema LegacyBenchmarkSchema = new(LegacyBenchmarkAttributeName, DeclaredAttributeKind.Benchmark);

    private static readonly IReadOnlyDictionary<string, DeclarationAttributeSchema> DeclaredAttributeSchemas =
        new Dictionary<string, DeclarationAttributeSchema>(StringComparer.Ordinal)
        {
            [UnitTestSchema.AttributeTypeName] = UnitTestSchema,
            [BenchmarkSchema.AttributeTypeName] = BenchmarkSchema,
            [LegacyBenchmarkSchema.AttributeTypeName] = LegacyBenchmarkSchema,
        };

    private static readonly IReadOnlyDictionary<string, NamedArgumentReader> UnitNamedArgumentReaders =
        new Dictionary<string, NamedArgumentReader>(StringComparer.Ordinal)
        {
            [NamedArgumentAlias] = ReadAliasArgument,
            [NamedArgumentCapabilityFamily] = ReadCapabilityFamilyArgument,
            [NamedArgumentCapability] = ReadCapabilityArgument,
            [NamedArgumentRequires] = ReadRequiresArgument,
            [NamedArgumentArchetype] = ReadArchetypeArgument,
            [NamedArgumentHotUpdateCapability] = ReadHotUpdateCapabilityArgument,
            [NamedArgumentEvidence] = ReadEvidenceArgument,
            [NamedArgumentPriority] = ReadPriorityArgument,
        };

    private static readonly IReadOnlyDictionary<string, NamedArgumentReader> BenchmarkNamedArgumentReaders =
        new Dictionary<string, NamedArgumentReader>(StringComparer.Ordinal)
        {
            [NamedArgumentAlias] = ReadAliasArgument,
            [NamedArgumentCapabilityFamily] = ReadCapabilityFamilyArgument,
            [NamedArgumentCapability] = ReadCapabilityArgument,
            [NamedArgumentRequires] = ReadRequiresArgument,
            [NamedArgumentArchetype] = ReadArchetypeArgument,
            [NamedArgumentHotUpdateCapability] = ReadHotUpdateCapabilityArgument,
            [NamedArgumentModes] = ReadModesArgument,
            [NamedArgumentWarmupCount] = ReadWarmupCountArgument,
            [NamedArgumentIterationCount] = ReadIterationCountArgument,
            [NamedArgumentInvocationCount] = ReadInvocationCountArgument,
        };

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
            Console.Error.WriteLine("usage: Chaos.IL2CPP.Tools.TestFramework.CollectionGen <assembly-path> [<assembly-path> ...]");
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

            foreach (var attributeHandle in typeDefinition.GetCustomAttributes())
            {
                if (!TryGetAttributeTypeName(metadataReader, attributeHandle, out var attributeTypeName))
                {
                    continue;
                }

                if (string.Equals(attributeTypeName, CapabilityTestAttributeName, StringComparison.Ordinal))
                {
                    payload.FrameworkReferenced = true;
                    break;
                }
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

                    if (!TryResolveDeclaredAttributeSchema(attributeTypeName, out var schema))
                    {
                        continue;
                    }

                    payload.FrameworkReferenced = true;
                    switch (schema.Kind)
                    {
                        case DeclaredAttributeKind.UnitTest:
                            payload.DeclaredUnitTests.Add(CreateUnitEntry(
                                metadataReader,
                                attributeHandle,
                                assemblyName,
                                declaringType,
                                methodName,
                                methodSignature));
                            break;
                        case DeclaredAttributeKind.Benchmark:
                            payload.DeclaredBenchmarks.Add(CreateBenchmarkEntry(
                                metadataReader,
                                attributeHandle,
                                assemblyName,
                                declaringType,
                                methodName,
                                methodSignature));
                            break;
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
        ReadNamedArguments(ref reader, entry, UnitNamedArgumentReaders);
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
        ReadNamedArguments(ref reader, entry, BenchmarkNamedArgumentReaders);
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

    private static bool TryResolveDeclaredAttributeSchema(string attributeTypeName, out DeclarationAttributeSchema schema)
    {
        return DeclaredAttributeSchemas.TryGetValue(attributeTypeName, out schema!);
    }

    private static void ReadNamedArguments(
        ref BlobReader reader,
        DeclaredEntryBase entry,
        IReadOnlyDictionary<string, NamedArgumentReader> namedArgumentReaders)
    {
        var namedArgumentCount = reader.ReadUInt16();
        for (var index = 0; index < namedArgumentCount; index++)
        {
            _ = reader.ReadByte();
            var typeCode = ReadFieldOrPropertyType(ref reader);
            var name = reader.ReadSerializedString() ?? string.Empty;
            if (namedArgumentReaders.TryGetValue(name, out var namedArgumentReader))
            {
                namedArgumentReader(ref reader, entry, typeCode);
                continue;
            }

            SkipArgumentValue(ref reader, typeCode);
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

    private static void ReadAliasArgument(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode)
    {
        entry.Alias = reader.ReadSerializedString();
    }

    private static void ReadCapabilityFamilyArgument(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode)
    {
        entry.CapabilityFamily = reader.ReadByte();
    }

    private static void ReadCapabilityArgument(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode)
    {
        entry.CapabilityItem = reader.ReadUInt16();
    }

    private static void ReadRequiresArgument(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode)
    {
        entry.Requires = reader.ReadUInt32();
    }

    private static void ReadArchetypeArgument(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode)
    {
        entry.Archetype = reader.ReadByte();
    }

    private static void ReadHotUpdateCapabilityArgument(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode)
    {
        entry.HotUpdateCapability = reader.ReadUInt16();
    }

    private static void ReadEvidenceArgument(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode)
    {
        if (entry is not DeclaredUnitEntry unitEntry)
        {
            throw new BadImageFormatException("unit-test attribute schema was bound to a non-unit entry");
        }

        unitEntry.Evidence = reader.ReadUInt16();
    }

    private static void ReadPriorityArgument(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode)
    {
        if (entry is not DeclaredUnitEntry unitEntry)
        {
            throw new BadImageFormatException("unit-test attribute schema was bound to a non-unit entry");
        }

        unitEntry.Priority = reader.ReadByte();
    }

    private static void ReadModesArgument(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode)
    {
        if (entry is not DeclaredBenchmarkEntry benchmarkEntry)
        {
            throw new BadImageFormatException("benchmark attribute schema was bound to a non-benchmark entry");
        }

        benchmarkEntry.Modes = reader.ReadByte();
    }

    private static void ReadWarmupCountArgument(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode)
    {
        if (entry is not DeclaredBenchmarkEntry benchmarkEntry)
        {
            throw new BadImageFormatException("benchmark attribute schema was bound to a non-benchmark entry");
        }

        benchmarkEntry.WarmupCount = reader.ReadByte();
    }

    private static void ReadIterationCountArgument(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode)
    {
        if (entry is not DeclaredBenchmarkEntry benchmarkEntry)
        {
            throw new BadImageFormatException("benchmark attribute schema was bound to a non-benchmark entry");
        }

        benchmarkEntry.IterationCount = reader.ReadUInt16();
    }

    private static void ReadInvocationCountArgument(ref BlobReader reader, DeclaredEntryBase entry, SerializationTypeCode typeCode)
    {
        if (entry is not DeclaredBenchmarkEntry benchmarkEntry)
        {
            throw new BadImageFormatException("benchmark attribute schema was bound to a non-benchmark entry");
        }

        benchmarkEntry.InvocationCount = reader.ReadUInt16();
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
        if (enumTypeName.EndsWith("ChaosCapabilityFamily", StringComparison.Ordinal))
        {
            return SerializationTypeCode.Byte;
        }
        if (enumTypeName.EndsWith("ChaosCapabilityItem", StringComparison.Ordinal))
        {
            return SerializationTypeCode.UInt16;
        }
        if (enumTypeName.EndsWith("ChaosRuntimeFeature", StringComparison.Ordinal))
        {
            return SerializationTypeCode.UInt32;
        }
        if (enumTypeName.EndsWith("ChaosSolutionArchetype", StringComparison.Ordinal))
        {
            return SerializationTypeCode.Byte;
        }
        if (enumTypeName.EndsWith("ChaosHotUpdateCapability", StringComparison.Ordinal))
        {
            return SerializationTypeCode.UInt16;
        }
        if (enumTypeName.EndsWith("ChaosEvidenceKind", StringComparison.Ordinal))
        {
            return SerializationTypeCode.UInt16;
        }

        return SerializationTypeCode.Int32;
    }

    private static object? ReadAttributeArgumentValue(ref BlobReader reader, SerializationTypeCode typeCode)
    {
        return typeCode switch
        {
            SerializationTypeCode.Boolean => reader.ReadBoolean(),
            SerializationTypeCode.Byte => reader.ReadByte(),
            SerializationTypeCode.Char => reader.ReadUInt16(),
            SerializationTypeCode.Int16 => reader.ReadInt16(),
            SerializationTypeCode.Int32 => reader.ReadInt32(),
            SerializationTypeCode.Int64 => reader.ReadInt64(),
            SerializationTypeCode.SByte => reader.ReadSByte(),
            SerializationTypeCode.Single => reader.ReadSingle(),
            SerializationTypeCode.Double => reader.ReadDouble(),
            SerializationTypeCode.String => reader.ReadSerializedString(),
            SerializationTypeCode.UInt16 => reader.ReadUInt16(),
            SerializationTypeCode.UInt32 => reader.ReadUInt32(),
            SerializationTypeCode.UInt64 => reader.ReadUInt64(),
            _ => throw new BadImageFormatException($"unsupported attribute argument type: {typeCode}"),
        };
    }

    private static void SkipArgumentValue(ref BlobReader reader, SerializationTypeCode typeCode)
    {
        _ = ReadAttributeArgumentValue(ref reader, typeCode);
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

internal enum DeclaredAttributeKind : byte
{
    None = 0,
    UnitTest = 1,
    Benchmark = 2,
}

internal sealed record DeclarationAttributeSchema(string AttributeTypeName, DeclaredAttributeKind Kind);

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

    public byte CapabilityFamily { get; set; }

    public ushort CapabilityItem { get; set; }

    public uint Requires { get; set; }

    public byte Archetype { get; set; }

    public ushort HotUpdateCapability { get; set; }
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
            "Chaos.TestFramework.ChaosCapabilityFamily" => PrimitiveTypeCode.Byte,
            "Chaos.TestFramework.ChaosCapabilityItem" => PrimitiveTypeCode.UInt16,
            "Chaos.TestFramework.ChaosRuntimeFeature" => PrimitiveTypeCode.UInt32,
            "Chaos.TestFramework.ChaosSolutionArchetype" => PrimitiveTypeCode.Byte,
            "Chaos.TestFramework.ChaosHotUpdateCapability" => PrimitiveTypeCode.UInt16,
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
