using System.Collections.Immutable;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

public sealed class LoaderStage
{
    public string Name => "Loader";

    public LoadedAssemblyModel Load(ManagedClosureRequest request)
    {
        using var stream = File.OpenRead(request.InputAssemblyPath);
        using var peReader = new PEReader(stream);
        var metadataReader = peReader.GetMetadataReader();

        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
        var assembly = new AssemblyIdentityModel
        {
            Name = assemblyName,
            ModuleVersionId = metadataReader.GetGuid(metadataReader.GetModuleDefinition().Mvid),
        };

        var typeResolver = new MetadataTypeResolver(metadataReader, assemblyName);
        var typeModels = LoadTypes(metadataReader, assemblyName);
        var ownerIndex = BuildOwnerIndex(metadataReader, typeModels);
        var fieldModels = LoadFields(metadataReader, typeResolver, typeModels, ownerIndex.FieldOwners, assemblyName);
        var methodModels = LoadMethods(
            peReader,
            metadataReader,
            typeResolver,
            typeModels,
            ownerIndex.FieldOwners,
            ownerIndex.MethodOwners,
            assemblyName);
        var entryPointSubjectId = ResolveEntryPointSubjectId(
            peReader,
            metadataReader,
            typeResolver,
            typeModels,
            ownerIndex.MethodOwners,
            assemblyName);

        return new LoadedAssemblyModel
        {
            InputAssemblyPath = request.InputAssemblyPath,
            Assembly = assembly,
            EntryPointSubjectId = entryPointSubjectId,
            Types = typeModels.Values.OrderBy(model => model.MetadataToken).ToList(),
            Fields = fieldModels.OrderBy(model => model.MetadataToken).ToList(),
            Methods = methodModels.OrderBy(model => model.MetadataToken).ToList(),
        };
    }

    private static string ResolveEntryPointSubjectId(
        PEReader peReader,
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        string assemblyName)
    {
        var entryToken = peReader.PEHeaders.CorHeader?.EntryPointTokenOrRelativeVirtualAddress ?? 0;
        if (entryToken == 0)
        {
            throw new InvalidOperationException("managed closure input assembly does not define an entry point");
        }

        var handle = MetadataTokens.EntityHandle(entryToken);
        if (handle.Kind != HandleKind.MethodDefinition)
        {
            throw new NotSupportedException($"unsupported managed entry point handle kind: {handle.Kind}");
        }

        return DescribeMethodDefinition(
            metadataReader,
            typeResolver,
            typeModels,
            methodOwners,
            assemblyName,
            (MethodDefinitionHandle)handle).SubjectId;
    }

    private static OwnerIndex BuildOwnerIndex(
        MetadataReader metadataReader,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels)
    {
        var fieldOwners = new Dictionary<FieldDefinitionHandle, ManagedTypeModel>();
        var methodOwners = new Dictionary<MethodDefinitionHandle, ManagedTypeModel>();

        foreach (var (typeHandle, typeModel) in typeModels)
        {
            var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);
            foreach (var fieldHandle in typeDefinition.GetFields())
            {
                fieldOwners[fieldHandle] = typeModel;
            }

            foreach (var methodHandle in typeDefinition.GetMethods())
            {
                methodOwners[methodHandle] = typeModel;
            }
        }

        return new OwnerIndex(fieldOwners, methodOwners);
    }

    private static Dictionary<TypeDefinitionHandle, ManagedTypeModel> LoadTypes(MetadataReader metadataReader, string assemblyName)
    {
        var models = new Dictionary<TypeDefinitionHandle, ManagedTypeModel>();

        foreach (var typeHandle in metadataReader.TypeDefinitions)
        {
            var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);
            var typeName = metadataReader.GetString(typeDefinition.Name);
            if (string.Equals(typeName, "<Module>", StringComparison.Ordinal) || !typeDefinition.GetDeclaringType().IsNil)
            {
                continue;
            }

            var namespaceName = metadataReader.GetString(typeDefinition.Namespace);
            var subjectId = ManagedNaming.CreateTypeSubjectId(assemblyName, namespaceName, typeName);
            var displayName = ManagedNaming.CreateTypeDisplayName(assemblyName, namespaceName, typeName);

            models[typeHandle] = new ManagedTypeModel
            {
                AssemblyName = assemblyName,
                NamespaceName = string.IsNullOrEmpty(namespaceName) ? null : namespaceName,
                Name = typeName,
                SubjectId = subjectId,
                DisplayName = displayName,
                MetadataToken = MetadataTokens.GetToken(typeHandle),
            };
        }

        return models;
    }

    private static List<ManagedFieldModel> LoadFields(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        string assemblyName)
    {
        var models = new List<ManagedFieldModel>();

        foreach (var (fieldHandle, typeModel) in fieldOwners.OrderBy(pair => MetadataTokens.GetToken(pair.Key)))
        {
            var fieldDefinition = metadataReader.GetFieldDefinition(fieldHandle);
            var fieldName = metadataReader.GetString(fieldDefinition.Name);
            var fieldType = fieldDefinition.DecodeSignature(typeResolver.TypeNameProvider, null);

            models.Add(new ManagedFieldModel
            {
                AssemblyName = assemblyName,
                DeclaringTypeSubjectId = typeModel.SubjectId,
                Name = fieldName,
                FieldType = fieldType,
                SubjectId = ManagedNaming.CreateFieldSubjectId(typeModel.SubjectId, fieldName),
                MetadataToken = MetadataTokens.GetToken(fieldHandle),
            });
        }

        return models;
    }

    private static List<ManagedMethodModel> LoadMethods(
        PEReader peReader,
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        string assemblyName)
    {
        var models = new List<ManagedMethodModel>();

        foreach (var (methodHandle, typeModel) in methodOwners.OrderBy(pair => MetadataTokens.GetToken(pair.Key)))
        {
            var methodSummary = DescribeMethodDefinition(
                metadataReader,
                typeResolver,
                typeModels,
                methodOwners,
                assemblyName,
                methodHandle);
            var body = DecodeMethodBody(
                peReader,
                metadataReader,
                typeResolver,
                typeModels,
                fieldOwners,
                methodOwners,
                methodHandle,
                methodSummary);

            models.Add(new ManagedMethodModel
            {
                AssemblyName = assemblyName,
                DeclaringTypeSubjectId = typeModel.SubjectId,
                DeclaringTypeDisplayName = typeModel.DisplayName,
                Name = methodSummary.Name,
                ReturnType = methodSummary.ReturnType,
                SubjectId = methodSummary.SubjectId,
                Signature = methodSummary.Signature,
                IsStatic = methodSummary.IsStatic,
                MetadataToken = methodSummary.MetadataToken,
                Parameters = methodSummary.Parameters,
                Body = body,
            });
        }

        return models;
    }

    private static ManagedMethodBodyModel DecodeMethodBody(
        PEReader peReader,
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        MethodDefinitionHandle methodHandle,
        MethodSummary methodSummary)
    {
        var methodDefinition = metadataReader.GetMethodDefinition(methodHandle);
        if (methodDefinition.RelativeVirtualAddress == 0)
        {
            return new ManagedMethodBodyModel
            {
                Blocks =
                [
                    new ManagedBlockModel
                    {
                        BlockId = "entry",
                        Instructions = [],
                    },
                ],
            };
        }

        var bodyBlock = peReader.GetMethodBody(methodDefinition.RelativeVirtualAddress);
        var ilReader = bodyBlock.GetILReader();
        var instructions = new List<ManagedInstructionModel>();

        while (ilReader.RemainingBytes > 0)
        {
            var opCode = ReadOpCode(ref ilReader);
            var instruction = DecodeInstruction(
                metadataReader,
                typeResolver,
                typeModels,
                fieldOwners,
                methodOwners,
                methodSummary,
                opCode,
                ref ilReader);

            if (instruction is not null)
            {
                instructions.Add(instruction);
            }
        }

        return new ManagedMethodBodyModel
        {
            Blocks =
            [
                new ManagedBlockModel
                {
                    BlockId = "entry",
                    Instructions = instructions,
                },
            ],
        };
    }

    private static ManagedInstructionModel? DecodeInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        MethodSummary methodSummary,
        ILOpCode opCode,
        ref BlobReader ilReader)
    {
        return opCode switch
        {
            ILOpCode.Nop => null,
            ILOpCode.Ldstr => DecodeLdstrInstruction(metadataReader, ref ilReader),
            ILOpCode.Newobj => DecodeMethodReferenceInstruction(metadataReader, typeResolver, typeModels, methodOwners, opCode, ref ilReader),
            ILOpCode.Call => DecodeMethodReferenceInstruction(metadataReader, typeResolver, typeModels, methodOwners, opCode, ref ilReader),
            ILOpCode.Callvirt => DecodeMethodReferenceInstruction(metadataReader, typeResolver, typeModels, methodOwners, opCode, ref ilReader),
            ILOpCode.Stfld => DecodeFieldReferenceInstruction(metadataReader, typeResolver, fieldOwners, opCode, ref ilReader),
            ILOpCode.Ldfld => DecodeFieldReferenceInstruction(metadataReader, typeResolver, fieldOwners, opCode, ref ilReader),
            ILOpCode.Ldarg => DecodeLdargInstruction(methodSummary, ilReader.ReadUInt16()),
            ILOpCode.Ldarg_s => DecodeLdargInstruction(methodSummary, ilReader.ReadByte()),
            ILOpCode.Ldarg_0 => DecodeLdargInstruction(methodSummary, 0),
            ILOpCode.Ldarg_1 => DecodeLdargInstruction(methodSummary, 1),
            ILOpCode.Ldarg_2 => DecodeLdargInstruction(methodSummary, 2),
            ILOpCode.Ldarg_3 => DecodeLdargInstruction(methodSummary, 3),
            ILOpCode.Ldc_i4 => DecodeLdcI4Instruction(ilReader.ReadInt32()),
            ILOpCode.Ldc_i4_s => DecodeLdcI4Instruction(ilReader.ReadSByte()),
            ILOpCode.Ldc_i4_m1 => DecodeLdcI4Instruction(-1),
            ILOpCode.Ldc_i4_0 => DecodeLdcI4Instruction(0),
            ILOpCode.Ldc_i4_1 => DecodeLdcI4Instruction(1),
            ILOpCode.Ldc_i4_2 => DecodeLdcI4Instruction(2),
            ILOpCode.Ldc_i4_3 => DecodeLdcI4Instruction(3),
            ILOpCode.Ldc_i4_4 => DecodeLdcI4Instruction(4),
            ILOpCode.Ldc_i4_5 => DecodeLdcI4Instruction(5),
            ILOpCode.Ldc_i4_6 => DecodeLdcI4Instruction(6),
            ILOpCode.Ldc_i4_7 => DecodeLdcI4Instruction(7),
            ILOpCode.Ldc_i4_8 => DecodeLdcI4Instruction(8),
            ILOpCode.Ret => new ManagedInstructionModel { Op = "ret" },
            _ => throw new NotSupportedException($"unsupported IL opcode in loader: {opCode}"),
        };
    }

    private static ManagedInstructionModel DecodeLdstrInstruction(MetadataReader metadataReader, ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var value = metadataReader.GetUserString(MetadataTokens.UserStringHandle(token));

        return new ManagedInstructionModel
        {
            Op = "ldstr",
            Operand = value,
            ResultType = "System.String",
        };
    }

    private static ManagedInstructionModel DecodeMethodReferenceInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        ILOpCode opCode,
        ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var handle = MetadataTokens.EntityHandle(token);
        var reference = ResolveMethodReference(metadataReader, typeResolver, typeModels, methodOwners, handle);

        return new ManagedInstructionModel
        {
            Op = opCode switch
            {
                ILOpCode.Newobj => "newobj",
                ILOpCode.Call => "call",
                ILOpCode.Callvirt => "callvirt",
                _ => throw new NotSupportedException($"unsupported method reference opcode: {opCode}"),
            },
            ResultType = opCode == ILOpCode.Newobj ? reference.DeclaringTypeSubjectId : reference.ReturnType,
            Callee = reference.SubjectId,
            Reference = new ManagedInstructionReference
            {
                AssemblyName = reference.AssemblyName,
                SubjectKind = "method",
                SubjectId = reference.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel DecodeFieldReferenceInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        ILOpCode opCode,
        ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var handle = MetadataTokens.EntityHandle(token);
        var reference = ResolveFieldReference(metadataReader, typeResolver, fieldOwners, handle);

        return new ManagedInstructionModel
        {
            Op = opCode switch
            {
                ILOpCode.Stfld => "stfld",
                ILOpCode.Ldfld => "ldfld",
                _ => throw new NotSupportedException($"unsupported field reference opcode: {opCode}"),
            },
            Operand = reference.SubjectId,
            ResultType = opCode == ILOpCode.Ldfld ? reference.FieldType : "System.Void",
            Reference = new ManagedInstructionReference
            {
                AssemblyName = reference.AssemblyName,
                SubjectKind = "field",
                SubjectId = reference.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel DecodeLdargInstruction(MethodSummary methodSummary, int index)
    {
        string resultType;

        if (!methodSummary.IsStatic && index == 0)
        {
            resultType = methodSummary.DeclaringTypeSubjectId;
        }
        else
        {
            var parameterIndex = methodSummary.IsStatic ? index : index - 1;
            if (parameterIndex < 0 || parameterIndex >= methodSummary.Parameters.Count)
            {
                throw new InvalidOperationException($"invalid ldarg index {index} for method {methodSummary.SubjectId}");
            }

            resultType = methodSummary.Parameters[parameterIndex].Type;
        }

        return new ManagedInstructionModel
        {
            Op = "ldarg",
            Operand = index,
            ResultType = resultType,
        };
    }

    private static ManagedInstructionModel DecodeLdcI4Instruction(int value)
    {
        return new ManagedInstructionModel
        {
            Op = "ldc.i4",
            Operand = value,
            ResultType = "System.Int32",
        };
    }

    private static ILOpCode ReadOpCode(ref BlobReader ilReader)
    {
        var first = ilReader.ReadByte();
        if (first != 0xFE)
        {
            return (ILOpCode)first;
        }

        return (ILOpCode)((first << 8) | ilReader.ReadByte());
    }

    private static MethodReferenceSummary ResolveMethodReference(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        EntityHandle handle)
    {
        return handle.Kind switch
        {
            HandleKind.MethodDefinition => DescribeMethodDefinition(
                metadataReader,
                typeResolver,
                typeModels,
                methodOwners,
                typeResolver.CurrentAssemblyName,
                (MethodDefinitionHandle)handle),
            HandleKind.MemberReference => DescribeMemberReferenceMethod(metadataReader, typeResolver, (MemberReferenceHandle)handle),
            _ => throw new NotSupportedException($"unsupported method reference handle kind: {handle.Kind}"),
        };
    }

    private static FieldReferenceSummary ResolveFieldReference(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        EntityHandle handle)
    {
        return handle.Kind switch
        {
            HandleKind.FieldDefinition => DescribeFieldDefinition(
                metadataReader,
                typeResolver,
                fieldOwners,
                typeResolver.CurrentAssemblyName,
                (FieldDefinitionHandle)handle),
            HandleKind.MemberReference => DescribeMemberReferenceField(metadataReader, typeResolver, (MemberReferenceHandle)handle),
            _ => throw new NotSupportedException($"unsupported field reference handle kind: {handle.Kind}"),
        };
    }

    private static MethodSummary DescribeMethodDefinition(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        string assemblyName,
        MethodDefinitionHandle handle)
    {
        var methodDefinition = metadataReader.GetMethodDefinition(handle);
        var declaringType = methodOwners[handle];
        var signature = methodDefinition.DecodeSignature(typeResolver.TypeNameProvider, null);
        var parameterTypes = signature.ParameterTypes.ToArray();
        var parameterNames = methodDefinition
            .GetParameters()
            .Select(parameterHandle => metadataReader.GetParameter(parameterHandle))
            .Where(parameter => parameter.SequenceNumber > 0)
            .OrderBy(parameter => parameter.SequenceNumber)
            .Select((parameter, index) =>
            {
                var name = metadataReader.GetString(parameter.Name);
                return string.IsNullOrEmpty(name) ? $"arg{index}" : name;
            })
            .ToArray();

        var parameters = parameterTypes
            .Select((parameterType, index) => new ManagedParameterModel
            {
                Name = index < parameterNames.Length ? parameterNames[index] : $"arg{index}",
                Type = parameterType,
            })
            .ToList();

        var methodName = metadataReader.GetString(methodDefinition.Name);
        var subjectId = ManagedNaming.CreateMethodSubjectId(declaringType.SubjectId, methodName, parameterTypes);

        return new MethodSummary
        {
            AssemblyName = assemblyName,
            DeclaringTypeSubjectId = declaringType.SubjectId,
            DeclaringTypeDisplayName = declaringType.DisplayName,
            Name = methodName,
            ReturnType = signature.ReturnType,
            SubjectId = subjectId,
            Signature = ManagedNaming.CreateMethodSignature(signature.ReturnType, declaringType.DisplayName, methodName, parameterTypes),
            IsStatic = methodDefinition.Attributes.HasFlag(MethodAttributes.Static),
            MetadataToken = MetadataTokens.GetToken(handle),
            Parameters = parameters,
        };
    }

    private static MethodReferenceSummary DescribeMemberReferenceMethod(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        MemberReferenceHandle handle)
    {
        var memberReference = metadataReader.GetMemberReference(handle);
        var declaringType = typeResolver.ResolveTypeIdentity(memberReference.Parent);
        var signature = memberReference.DecodeMethodSignature(typeResolver.TypeNameProvider, null);
        var parameterTypes = signature.ParameterTypes.ToArray();
        var methodName = metadataReader.GetString(memberReference.Name);

        return new MethodReferenceSummary
        {
            AssemblyName = declaringType.AssemblyName,
            DeclaringTypeSubjectId = declaringType.SubjectId,
            SubjectId = ManagedNaming.CreateMethodSubjectId(declaringType.SubjectId, methodName, parameterTypes),
            ReturnType = signature.ReturnType,
        };
    }

    private static FieldReferenceSummary DescribeFieldDefinition(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        string assemblyName,
        FieldDefinitionHandle handle)
    {
        var fieldDefinition = metadataReader.GetFieldDefinition(handle);
        var declaringType = fieldOwners[handle];
        var fieldName = metadataReader.GetString(fieldDefinition.Name);

        return new FieldReferenceSummary
        {
            AssemblyName = assemblyName,
            SubjectId = ManagedNaming.CreateFieldSubjectId(declaringType.SubjectId, fieldName),
            FieldType = fieldDefinition.DecodeSignature(typeResolver.TypeNameProvider, null),
        };
    }

    private static FieldReferenceSummary DescribeMemberReferenceField(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        MemberReferenceHandle handle)
    {
        var memberReference = metadataReader.GetMemberReference(handle);
        var declaringType = typeResolver.ResolveTypeIdentity(memberReference.Parent);
        var fieldName = metadataReader.GetString(memberReference.Name);

        return new FieldReferenceSummary
        {
            AssemblyName = declaringType.AssemblyName,
            SubjectId = ManagedNaming.CreateFieldSubjectId(declaringType.SubjectId, fieldName),
            FieldType = memberReference.DecodeFieldSignature(typeResolver.TypeNameProvider, null),
        };
    }
}

internal sealed class MetadataTypeResolver
{
    private readonly MetadataReader _metadataReader;

    public MetadataTypeResolver(MetadataReader metadataReader, string currentAssemblyName)
    {
        _metadataReader = metadataReader;
        CurrentAssemblyName = currentAssemblyName;
        TypeNameProvider = new TypeNameProvider(this);
    }

    public string CurrentAssemblyName { get; }

    public TypeNameProvider TypeNameProvider { get; }

    public TypeIdentity ResolveTypeIdentity(EntityHandle handle)
    {
        return handle.Kind switch
        {
            HandleKind.TypeDefinition => ResolveTypeDefinition((TypeDefinitionHandle)handle),
            HandleKind.TypeReference => ResolveTypeReference((TypeReferenceHandle)handle),
            _ => throw new NotSupportedException($"unsupported type handle kind: {handle.Kind}"),
        };
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

    private TypeIdentity ResolveTypeDefinition(TypeDefinitionHandle handle)
    {
        var typeDefinition = _metadataReader.GetTypeDefinition(handle);
        var namespaceName = _metadataReader.GetString(typeDefinition.Namespace);
        var typeName = _metadataReader.GetString(typeDefinition.Name);

        return new TypeIdentity(
            CurrentAssemblyName,
            ManagedNaming.CreateTypeSubjectId(CurrentAssemblyName, namespaceName, typeName),
            ManagedNaming.CreateTypeDisplayName(CurrentAssemblyName, namespaceName, typeName));
    }

    private TypeIdentity ResolveTypeReference(TypeReferenceHandle handle)
    {
        var typeReference = _metadataReader.GetTypeReference(handle);
        var assemblyName = ResolveAssemblyName(typeReference.ResolutionScope);
        var namespaceName = _metadataReader.GetString(typeReference.Namespace);
        var typeName = _metadataReader.GetString(typeReference.Name);

        return new TypeIdentity(
            assemblyName,
            ManagedNaming.CreateTypeSubjectId(assemblyName, namespaceName, typeName),
            ManagedNaming.CreateTypeDisplayName(assemblyName, namespaceName, typeName));
    }

    private static string NormalizeAssemblyName(string assemblyName)
    {
        return assemblyName switch
        {
            "System.Runtime" => "System.Private.CoreLib",
            "mscorlib" => "System.Private.CoreLib",
            _ => assemblyName,
        };
    }
}

internal sealed class TypeNameProvider : ISignatureTypeProvider<string, object?>
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
        throw new NotSupportedException("function pointers are not supported in the Stage 3 loader");
    }

    public string GetGenericInstantiation(string genericType, ImmutableArray<string> typeArguments)
    {
        return $"{genericType}<{string.Join(",", typeArguments)}>";
    }

    public string GetGenericMethodParameter(object? genericContext, int index)
    {
        return $"!!{index}";
    }

    public string GetGenericTypeParameter(object? genericContext, int index)
    {
        return $"!{index}";
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

    public string GetTypeFromSpecification(MetadataReader reader, object? genericContext, TypeSpecificationHandle handle, byte rawTypeKind)
    {
        return reader.GetTypeSpecification(handle).DecodeSignature(this, genericContext);
    }
}

internal sealed record TypeIdentity(string AssemblyName, string SubjectId, string DisplayName);

internal record class MethodReferenceSummary
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string SubjectId { get; init; }

    public required string ReturnType { get; init; }
}

internal sealed record FieldReferenceSummary
{
    public required string AssemblyName { get; init; }

    public required string SubjectId { get; init; }

    public required string FieldType { get; init; }
}

internal sealed record MethodSummary : MethodReferenceSummary
{
    public required string DeclaringTypeDisplayName { get; init; }

    public required string Name { get; init; }

    public required string Signature { get; init; }

    public required bool IsStatic { get; init; }

    public required int MetadataToken { get; init; }

    public required IReadOnlyList<ManagedParameterModel> Parameters { get; init; }
}

internal sealed record OwnerIndex(
    IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> FieldOwners,
    IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> MethodOwners);
