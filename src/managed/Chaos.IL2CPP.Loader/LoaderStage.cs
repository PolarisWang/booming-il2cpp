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
        var propertyModels = LoadProperties(metadataReader, typeResolver, ownerIndex.PropertyOwners, assemblyName);
        var methodModels = LoadMethods(
            peReader,
            metadataReader,
            typeResolver,
            typeModels,
            ownerIndex.FieldOwners,
            ownerIndex.MethodOwners,
            assemblyName);
        var materializedGenerics = MaterializeGenericInstantiations(
            metadataReader,
            typeResolver,
            assemblyName,
            typeModels,
            ownerIndex.MethodOwners,
            typeModels.Values,
            fieldModels,
            propertyModels,
            methodModels);
        var entryPointSubjectId = ResolveEntryPointSubjectId(
            peReader,
            metadataReader,
            typeResolver,
            typeModels,
            ownerIndex.MethodOwners,
            assemblyName);
        var allTypes = typeModels.Values
            .Concat(materializedGenerics.Types)
            .OrderBy(model => model.MetadataToken)
            .ToList();
        var allFields = fieldModels
            .Concat(materializedGenerics.Fields)
            .OrderBy(model => model.MetadataToken)
            .ToList();
        var allProperties = propertyModels
            .Concat(materializedGenerics.Properties)
            .OrderBy(model => model.MetadataToken)
            .ToList();
        var allMethods = methodModels
            .Concat(materializedGenerics.Methods)
            .OrderBy(model => model.MetadataToken)
            .ToList();

        return new LoadedAssemblyModel
        {
            InputAssemblyPath = request.InputAssemblyPath,
            Assembly = assembly,
            EntryPointSubjectId = entryPointSubjectId,
            Types = allTypes,
            Fields = allFields,
            Properties = allProperties,
            Methods = allMethods,
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
        var propertyOwners = new Dictionary<PropertyDefinitionHandle, ManagedTypeModel>();
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

        foreach (var propertyHandle in metadataReader.PropertyDefinitions)
        {
            var propertyDefinition = metadataReader.GetPropertyDefinition(propertyHandle);
            var propertyAccessors = propertyDefinition.GetAccessors();
            var accessorHandle = !propertyAccessors.Getter.IsNil
                ? propertyAccessors.Getter
                : propertyAccessors.Setter;

            if (accessorHandle.IsNil || !methodOwners.TryGetValue(accessorHandle, out var typeModel))
            {
                continue;
            }

            propertyOwners[propertyHandle] = typeModel;
        }

        return new OwnerIndex(fieldOwners, propertyOwners, methodOwners);
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
                DefinitionSubjectId = subjectId,
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
                DefinitionSubjectId = ManagedNaming.CreateFieldSubjectId(typeModel.DefinitionSubjectId, fieldName),
                MetadataToken = MetadataTokens.GetToken(fieldHandle),
            });
        }

        return models;
    }

    private static List<ManagedPropertyModel> LoadProperties(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<PropertyDefinitionHandle, ManagedTypeModel> propertyOwners,
        string assemblyName)
    {
        var models = new List<ManagedPropertyModel>();

        foreach (var (propertyHandle, typeModel) in propertyOwners.OrderBy(pair => MetadataTokens.GetToken(pair.Key)))
        {
            var propertyDefinition = metadataReader.GetPropertyDefinition(propertyHandle);
            var propertyName = metadataReader.GetString(propertyDefinition.Name);
            var propertyType = propertyDefinition.DecodeSignature(typeResolver.TypeNameProvider, null).ReturnType;

            models.Add(new ManagedPropertyModel
            {
                AssemblyName = assemblyName,
                DeclaringTypeSubjectId = typeModel.SubjectId,
                Name = propertyName,
                PropertyType = propertyType,
                SubjectId = ManagedNaming.CreatePropertySubjectId(typeModel.SubjectId, propertyName),
                DefinitionSubjectId = ManagedNaming.CreatePropertySubjectId(typeModel.DefinitionSubjectId, propertyName),
                MetadataToken = MetadataTokens.GetToken(propertyHandle),
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
                DefinitionSubjectId = methodSummary.DefinitionSubjectId,
                Signature = methodSummary.Signature,
                IsStatic = methodSummary.IsStatic,
                MetadataToken = methodSummary.MetadataToken,
                Parameters = methodSummary.Parameters,
                Import = methodSummary.Import,
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
            ILOpCode.Ldtoken => DecodeLdtokenInstruction(metadataReader, typeResolver, typeModels, fieldOwners, methodOwners, ref ilReader),
            ILOpCode.Ldstr => DecodeLdstrInstruction(metadataReader, ref ilReader),
            ILOpCode.Newobj => DecodeMethodReferenceInstruction(metadataReader, typeResolver, typeModels, methodOwners, opCode, ref ilReader),
            ILOpCode.Call => DecodeMethodReferenceInstruction(metadataReader, typeResolver, typeModels, methodOwners, opCode, ref ilReader),
            ILOpCode.Callvirt => DecodeMethodReferenceInstruction(metadataReader, typeResolver, typeModels, methodOwners, opCode, ref ilReader),
            ILOpCode.Stfld => DecodeFieldReferenceInstruction(metadataReader, typeResolver, fieldOwners, opCode, ref ilReader),
            ILOpCode.Ldfld => DecodeFieldReferenceInstruction(metadataReader, typeResolver, fieldOwners, opCode, ref ilReader),
            ILOpCode.Dup => new ManagedInstructionModel { Op = "dup" },
            ILOpCode.Pop => new ManagedInstructionModel { Op = "pop" },
            ILOpCode.Ldelem_ref => new ManagedInstructionModel { Op = "ldelem.ref", ResultType = "System.Object" },
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

    private static ManagedInstructionModel DecodeLdtokenInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var handle = MetadataTokens.EntityHandle(token);

        return handle.Kind switch
        {
            HandleKind.TypeDefinition or HandleKind.TypeReference or HandleKind.TypeSpecification
                => CreateLdtokenTypeInstruction(typeResolver.ResolveTypeIdentity(handle)),
            HandleKind.FieldDefinition or HandleKind.MemberReference
                => CreateLdtokenFieldInstruction(ResolveFieldReference(metadataReader, typeResolver, fieldOwners, handle)),
            HandleKind.MethodDefinition or HandleKind.MethodSpecification
                => CreateLdtokenMethodInstruction(ResolveMethodReference(metadataReader, typeResolver, typeModels, methodOwners, handle)),
            _ => throw new NotSupportedException($"unsupported ldtoken handle kind in loader: {handle.Kind}"),
        };
    }

    private static ManagedInstructionModel CreateLdtokenTypeInstruction(TypeIdentity typeIdentity)
    {
        return new ManagedInstructionModel
        {
            Op = "ldtoken",
            Operand = typeIdentity.SubjectId,
            ResultType = "System.RuntimeTypeHandle",
            Reference = new ManagedInstructionReference
            {
                AssemblyName = typeIdentity.AssemblyName,
                SubjectKind = "type",
                SubjectId = typeIdentity.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel CreateLdtokenFieldInstruction(FieldReferenceSummary reference)
    {
        return new ManagedInstructionModel
        {
            Op = "ldtoken",
            Operand = reference.SubjectId,
            ResultType = "System.RuntimeFieldHandle",
            Reference = new ManagedInstructionReference
            {
                AssemblyName = reference.AssemblyName,
                SubjectKind = "field",
                SubjectId = reference.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel CreateLdtokenMethodInstruction(MethodReferenceSummary reference)
    {
        return new ManagedInstructionModel
        {
            Op = "ldtoken",
            Operand = reference.SubjectId,
            ResultType = "System.RuntimeMethodHandle",
            Reference = new ManagedInstructionReference
            {
                AssemblyName = reference.AssemblyName,
                SubjectKind = "method",
                SubjectId = reference.SubjectId,
            },
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
            HandleKind.MethodSpecification => DescribeMethodSpecification(
                metadataReader,
                typeResolver,
                typeModels,
                methodOwners,
                (MethodSpecificationHandle)handle),
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
        var import = TryDescribeMethodImport(metadataReader, methodDefinition);

        return new MethodSummary
        {
            AssemblyName = assemblyName,
            DeclaringTypeSubjectId = declaringType.SubjectId,
            DeclaringTypeDisplayName = declaringType.DisplayName,
            Name = methodName,
            ReturnType = signature.ReturnType,
            SubjectId = subjectId,
            DefinitionSubjectId = subjectId,
            ParameterTypes = parameterTypes,
            Signature = ManagedNaming.CreateMethodSignature(signature.ReturnType, declaringType.DisplayName, methodName, parameterTypes),
            IsStatic = methodDefinition.Attributes.HasFlag(MethodAttributes.Static),
            MetadataToken = MetadataTokens.GetToken(handle),
            Parameters = parameters,
            Import = import,
            Substitutions = ImmutableDictionary<string, string>.Empty,
        };
    }

    private static ManagedImportModel? TryDescribeMethodImport(
        MetadataReader metadataReader,
        MethodDefinition methodDefinition)
    {
        if (!methodDefinition.Attributes.HasFlag(MethodAttributes.PinvokeImpl))
        {
            return null;
        }

        var import = methodDefinition.GetImport();
        var moduleReference = metadataReader.GetModuleReference(import.Module);
        var moduleName = metadataReader.GetString(moduleReference.Name);
        var entryPointName = metadataReader.GetString(import.Name);

        return new ManagedImportModel
        {
            ModuleName = moduleName,
            EntryPointName = entryPointName,
        };
    }

    private static MethodReferenceSummary DescribeMemberReferenceMethod(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        MemberReferenceHandle handle)
    {
        var memberReference = metadataReader.GetMemberReference(handle);
        var declaringType = typeResolver.ResolveTypeIdentity(memberReference.Parent);
        var signature = memberReference.DecodeMethodSignature(
            typeResolver.TypeNameProvider,
            typeResolver.CreateTypeNameContext(declaringType));
        var parameterTypes = signature.ParameterTypes.ToArray();
        var methodName = metadataReader.GetString(memberReference.Name);
        var definitionSignature = memberReference.DecodeMethodSignature(typeResolver.TypeNameProvider, null);
        var definitionParameterTypes = definitionSignature.ParameterTypes.ToArray();

        return new MethodReferenceSummary
        {
            AssemblyName = declaringType.AssemblyName,
            DeclaringTypeSubjectId = declaringType.SubjectId,
            DeclaringTypeDisplayName = declaringType.DisplayName,
            Name = methodName,
            SubjectId = ManagedNaming.CreateMethodSubjectId(declaringType.SubjectId, methodName, parameterTypes),
            DefinitionSubjectId = ManagedNaming.CreateMethodSubjectId(
                declaringType.DefinitionSubjectId,
                methodName,
                definitionParameterTypes),
            ReturnType = signature.ReturnType,
            ParameterTypes = parameterTypes,
            MetadataToken = MetadataTokens.GetToken(handle),
            Substitutions = CreateSubstitutionMap(declaringType.TypeArguments, []),
        };
    }

    private static MethodReferenceSummary DescribeMethodSpecification(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        MethodSpecificationHandle handle)
    {
        var methodSpecification = metadataReader.GetMethodSpecification(handle);
        var methodArguments = methodSpecification.DecodeSignature(typeResolver.TypeNameProvider, null).ToImmutableArray();
        var baseReference = ResolveMethodReference(metadataReader, typeResolver, typeModels, methodOwners, methodSpecification.Method);
        var methodName = methodArguments.Any(argument => !argument.StartsWith("!", StringComparison.Ordinal))
            ? ManagedNaming.CreateGenericMethodName(baseReference.Name, methodArguments)
            : baseReference.Name;
        var substitutions = baseReference.Substitutions.SetItems(CreateSubstitutionMap([], methodArguments));

        MethodSignature<string> closedSignature = methodSpecification.Method.Kind switch
        {
            HandleKind.MethodDefinition => metadataReader
                .GetMethodDefinition((MethodDefinitionHandle)methodSpecification.Method)
                .DecodeSignature(typeResolver.TypeNameProvider, new SignatureContext<string>([], methodArguments)),
            HandleKind.MemberReference => metadataReader
                .GetMemberReference((MemberReferenceHandle)methodSpecification.Method)
                .DecodeMethodSignature(
                    typeResolver.TypeNameProvider,
                    new SignatureContext<string>(
                        substitutions
                            .Where(pair => pair.Key.StartsWith("!", StringComparison.Ordinal) && !pair.Key.StartsWith("!!", StringComparison.Ordinal))
                            .OrderBy(pair => pair.Key, StringComparer.Ordinal)
                            .Select(pair => pair.Value)
                            .ToImmutableArray(),
                        methodArguments)),
            _ => throw new NotSupportedException($"unsupported method specification target kind: {methodSpecification.Method.Kind}"),
        };

        var parameterTypes = closedSignature.ParameterTypes.ToArray();

        return new MethodReferenceSummary
        {
            AssemblyName = baseReference.AssemblyName,
            DeclaringTypeSubjectId = baseReference.DeclaringTypeSubjectId,
            DeclaringTypeDisplayName = baseReference.DeclaringTypeDisplayName,
            Name = methodName,
            SubjectId = ManagedNaming.CreateMethodSubjectId(baseReference.DeclaringTypeSubjectId, methodName, parameterTypes),
            DefinitionSubjectId = baseReference.DefinitionSubjectId,
            ReturnType = closedSignature.ReturnType,
            ParameterTypes = parameterTypes,
            MetadataToken = MetadataTokens.GetToken(handle),
            Substitutions = substitutions,
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
            DeclaringTypeSubjectId = declaringType.SubjectId,
            Name = fieldName,
            SubjectId = ManagedNaming.CreateFieldSubjectId(declaringType.SubjectId, fieldName),
            DefinitionSubjectId = ManagedNaming.CreateFieldSubjectId(declaringType.SubjectId, fieldName),
            FieldType = fieldDefinition.DecodeSignature(typeResolver.TypeNameProvider, null),
            MetadataToken = MetadataTokens.GetToken(handle),
            Substitutions = ImmutableDictionary<string, string>.Empty,
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
        var signatureContext = typeResolver.CreateTypeNameContext(declaringType);

        return new FieldReferenceSummary
        {
            AssemblyName = declaringType.AssemblyName,
            DeclaringTypeSubjectId = declaringType.SubjectId,
            Name = fieldName,
            SubjectId = ManagedNaming.CreateFieldSubjectId(declaringType.SubjectId, fieldName),
            DefinitionSubjectId = ManagedNaming.CreateFieldSubjectId(declaringType.DefinitionSubjectId, fieldName),
            FieldType = memberReference.DecodeFieldSignature(typeResolver.TypeNameProvider, signatureContext),
            MetadataToken = MetadataTokens.GetToken(handle),
            Substitutions = CreateSubstitutionMap(declaringType.TypeArguments, []),
        };
    }

    private static MaterializedGenericModels MaterializeGenericInstantiations(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        string assemblyName,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        IReadOnlyCollection<ManagedTypeModel> definitionTypes,
        IReadOnlyList<ManagedFieldModel> definitionFields,
        IReadOnlyList<ManagedPropertyModel> definitionProperties,
        IReadOnlyList<ManagedMethodModel> definitionMethods)
    {
        var materializedTypes = new Dictionary<string, ManagedTypeModel>(StringComparer.Ordinal);
        var materializedTypeIdentities = new Dictionary<string, TypeIdentity>(StringComparer.Ordinal);
        var materializedFields = new Dictionary<string, ManagedFieldModel>(StringComparer.Ordinal);
        var materializedProperties = new Dictionary<string, ManagedPropertyModel>(StringComparer.Ordinal);
        var fieldBindings = new Dictionary<FieldBindingKey, string>();
        var materializedMethods = new Dictionary<string, ManagedMethodModel>(StringComparer.Ordinal);

        var existingTypeSubjects = definitionTypes.ToDictionary(model => model.SubjectId, StringComparer.Ordinal);
        var existingFieldSubjects = definitionFields.ToDictionary(model => model.SubjectId, StringComparer.Ordinal);
        var existingPropertySubjects = definitionProperties.ToDictionary(model => model.SubjectId, StringComparer.Ordinal);
        var definitionMethodsBySubject = definitionMethods.ToDictionary(model => model.SubjectId, StringComparer.Ordinal);
        var definitionMethodsByDeclaringType = definitionMethods
            .GroupBy(method => method.DeclaringTypeSubjectId)
            .ToDictionary(group => group.Key, group => group.ToList(), StringComparer.Ordinal);
        var definitionFieldsByDeclaringType = definitionFields
            .GroupBy(field => field.DeclaringTypeSubjectId)
            .ToDictionary(group => group.Key, group => group.ToList(), StringComparer.Ordinal);
        var definitionPropertiesByDeclaringType = definitionProperties
            .GroupBy(property => property.DeclaringTypeSubjectId)
            .ToDictionary(group => group.Key, group => group.ToList(), StringComparer.Ordinal);
        var syntheticFieldMetadataToken = unchecked((int)0x70000000u);
        var syntheticPropertyMetadataToken = unchecked((int)0x71000000u);
        var syntheticMethodMetadataToken = unchecked((int)0x72000000u);

        for (var rowNumber = 1; rowNumber <= metadataReader.GetTableRowCount(TableIndex.TypeSpec); rowNumber++)
        {
            var typeSpecificationHandle = MetadataTokens.TypeSpecificationHandle(rowNumber);
            var typeIdentity = typeResolver.ResolveTypeIdentity(typeSpecificationHandle);
            if (!string.Equals(typeIdentity.AssemblyName, assemblyName, StringComparison.Ordinal) ||
                string.Equals(typeIdentity.SubjectId, typeIdentity.DefinitionSubjectId, StringComparison.Ordinal) ||
                existingTypeSubjects.ContainsKey(typeIdentity.SubjectId) ||
                materializedTypes.ContainsKey(typeIdentity.SubjectId))
            {
                continue;
            }

            materializedTypes[typeIdentity.SubjectId] = new ManagedTypeModel
            {
                AssemblyName = typeIdentity.AssemblyName,
                NamespaceName = null,
                Name = typeIdentity.DisplayName,
                SubjectId = typeIdentity.SubjectId,
                DefinitionSubjectId = typeIdentity.DefinitionSubjectId,
                DisplayName = typeIdentity.DisplayName,
                MetadataToken = MetadataTokens.GetToken(typeSpecificationHandle),
            };
            materializedTypeIdentities[typeIdentity.SubjectId] = typeIdentity;
        }

        foreach (var typeIdentity in materializedTypeIdentities.Values)
        {
            if (!definitionFieldsByDeclaringType.TryGetValue(typeIdentity.DefinitionSubjectId, out var closedFieldDefinitions))
            {
                continue;
            }

            foreach (var definitionField in closedFieldDefinitions)
            {
                var subjectId = ManagedNaming.CreateFieldSubjectId(typeIdentity.SubjectId, definitionField.Name);
                if (existingFieldSubjects.ContainsKey(subjectId) || materializedFields.ContainsKey(subjectId))
                {
                    continue;
                }

                var substitutions = CreateSubstitutionMap(typeIdentity.TypeArguments, []);
                materializedFields[subjectId] = new ManagedFieldModel
                {
                    AssemblyName = typeIdentity.AssemblyName,
                    DeclaringTypeSubjectId = typeIdentity.SubjectId,
                    Name = definitionField.Name,
                    FieldType = SubstituteText(
                        definitionField.FieldType,
                        substitutions,
                        new Dictionary<string, string>(StringComparer.Ordinal)),
                    SubjectId = subjectId,
                    DefinitionSubjectId = definitionField.SubjectId,
                    MetadataToken = syntheticFieldMetadataToken++,
                };
                fieldBindings[new FieldBindingKey(definitionField.SubjectId, typeIdentity.SubjectId)] = subjectId;
            }
        }

        foreach (var typeIdentity in materializedTypeIdentities.Values)
        {
            if (!definitionPropertiesByDeclaringType.TryGetValue(typeIdentity.DefinitionSubjectId, out var closedPropertyDefinitions))
            {
                continue;
            }

            foreach (var definitionProperty in closedPropertyDefinitions)
            {
                var subjectId = ManagedNaming.CreatePropertySubjectId(typeIdentity.SubjectId, definitionProperty.Name);
                if (existingPropertySubjects.ContainsKey(subjectId) || materializedProperties.ContainsKey(subjectId))
                {
                    continue;
                }

                var substitutions = CreateSubstitutionMap(typeIdentity.TypeArguments, []);
                materializedProperties[subjectId] = new ManagedPropertyModel
                {
                    AssemblyName = typeIdentity.AssemblyName,
                    DeclaringTypeSubjectId = typeIdentity.SubjectId,
                    Name = definitionProperty.Name,
                    PropertyType = SubstituteText(
                        definitionProperty.PropertyType,
                        substitutions,
                        new Dictionary<string, string>(StringComparer.Ordinal)),
                    SubjectId = subjectId,
                    DefinitionSubjectId = definitionProperty.SubjectId,
                    MetadataToken = syntheticPropertyMetadataToken++,
                };
            }
        }

        foreach (var typeIdentity in materializedTypeIdentities.Values)
        {
            if (!definitionMethodsByDeclaringType.TryGetValue(typeIdentity.DefinitionSubjectId, out var closedMethodDefinitions))
            {
                continue;
            }

            var substitutions = CreateSubstitutionMap(typeIdentity.TypeArguments, []);
            var methodBindings = closedMethodDefinitions.ToDictionary(
                definitionMethod => definitionMethod.SubjectId,
                definitionMethod =>
                {
                    var substitutedParameterTypes = definitionMethod.Parameters
                        .Select(parameter => SubstituteText(
                            parameter.Type,
                            substitutions,
                            new Dictionary<string, string>(StringComparer.Ordinal)))
                        .ToList();
                    return ManagedNaming.CreateMethodSubjectId(typeIdentity.SubjectId, definitionMethod.Name, substitutedParameterTypes);
                },
                StringComparer.Ordinal);

            foreach (var definitionMethod in closedMethodDefinitions)
            {
                var subjectId = methodBindings[definitionMethod.SubjectId];
                if (materializedMethods.ContainsKey(subjectId))
                {
                    continue;
                }

                var subjectSubstitutions = new Dictionary<string, string>(StringComparer.Ordinal)
                {
                    [definitionMethod.DeclaringTypeSubjectId] = typeIdentity.SubjectId,
                    [definitionMethod.SubjectId] = subjectId,
                };

                foreach (var (definitionSubjectId, boundSubjectId) in methodBindings)
                {
                    subjectSubstitutions[definitionSubjectId] = boundSubjectId;
                }

                foreach (var fieldBinding in fieldBindings)
                {
                    if (string.Equals(fieldBinding.Key.DeclaringTypeSubjectId, typeIdentity.SubjectId, StringComparison.Ordinal))
                    {
                        subjectSubstitutions[fieldBinding.Key.DefinitionSubjectId] = fieldBinding.Value;
                    }
                }

                var parameters = definitionMethod.Parameters
                    .Select(parameter => new ManagedParameterModel
                    {
                        Name = parameter.Name,
                        Type = SubstituteText(parameter.Type, substitutions, subjectSubstitutions),
                    })
                    .ToList();

                var returnType = SubstituteText(definitionMethod.ReturnType, substitutions, subjectSubstitutions);
                materializedMethods[subjectId] = new ManagedMethodModel
                {
                    AssemblyName = typeIdentity.AssemblyName,
                    DeclaringTypeSubjectId = typeIdentity.SubjectId,
                    DeclaringTypeDisplayName = typeIdentity.DisplayName,
                    Name = definitionMethod.Name,
                    ReturnType = returnType,
                    SubjectId = subjectId,
                    DefinitionSubjectId = definitionMethod.SubjectId,
                    Signature = ManagedNaming.CreateMethodSignature(
                        returnType,
                        typeIdentity.DisplayName,
                        definitionMethod.Name,
                        parameters.Select(parameter => parameter.Type).ToList()),
                    IsStatic = definitionMethod.IsStatic,
                    MetadataToken = syntheticMethodMetadataToken++,
                    Parameters = parameters,
                    Import = null,
                    Body = SubstituteMethodBody(definitionMethod.Body, substitutions, subjectSubstitutions),
                };
            }
        }

        foreach (var memberReferenceHandle in metadataReader.MemberReferences)
        {
            var memberReference = metadataReader.GetMemberReference(memberReferenceHandle);
            if (memberReference.GetKind() != MemberReferenceKind.Field)
            {
                continue;
            }

            var fieldReference = DescribeMemberReferenceField(metadataReader, typeResolver, memberReferenceHandle);
            if (!string.Equals(fieldReference.AssemblyName, assemblyName, StringComparison.Ordinal) ||
                string.Equals(fieldReference.SubjectId, fieldReference.DefinitionSubjectId, StringComparison.Ordinal) ||
                existingFieldSubjects.ContainsKey(fieldReference.SubjectId) ||
                materializedFields.ContainsKey(fieldReference.SubjectId))
            {
                continue;
            }

            materializedFields[fieldReference.SubjectId] = new ManagedFieldModel
            {
                AssemblyName = fieldReference.AssemblyName,
                DeclaringTypeSubjectId = fieldReference.DeclaringTypeSubjectId,
                Name = fieldReference.Name,
                FieldType = fieldReference.FieldType,
                SubjectId = fieldReference.SubjectId,
                DefinitionSubjectId = fieldReference.DefinitionSubjectId,
                MetadataToken = fieldReference.MetadataToken,
            };
            fieldBindings[new FieldBindingKey(fieldReference.DefinitionSubjectId, fieldReference.DeclaringTypeSubjectId)] = fieldReference.SubjectId;
        }

        foreach (var memberReferenceHandle in metadataReader.MemberReferences)
        {
            var memberReference = metadataReader.GetMemberReference(memberReferenceHandle);
            if (memberReference.GetKind() != MemberReferenceKind.Method)
            {
                continue;
            }

            var methodReference = DescribeMemberReferenceMethod(metadataReader, typeResolver, memberReferenceHandle);
            TryMaterializeMethod(
                assemblyName,
                methodReference,
                definitionMethodsBySubject,
                fieldBindings,
                materializedMethods);
        }

        for (var rowNumber = 1; rowNumber <= metadataReader.GetTableRowCount(TableIndex.MethodSpec); rowNumber++)
        {
            var methodSpecificationHandle = MetadataTokens.MethodSpecificationHandle(rowNumber);
            var methodReference = DescribeMethodSpecification(
                metadataReader,
                typeResolver,
                typeModels,
                methodOwners,
                methodSpecificationHandle);
            TryMaterializeMethod(
                assemblyName,
                methodReference,
                definitionMethodsBySubject,
                fieldBindings,
                materializedMethods);
        }

        return new MaterializedGenericModels(
            materializedTypes.Values.ToList(),
            materializedFields.Values.ToList(),
            materializedProperties.Values.OrderBy(model => model.MetadataToken).ToList(),
            materializedMethods.Values.OrderBy(model => model.MetadataToken).ToList());
    }

    private static void TryMaterializeMethod(
        string assemblyName,
        MethodReferenceSummary methodReference,
        IReadOnlyDictionary<string, ManagedMethodModel> definitionMethodsBySubject,
        IReadOnlyDictionary<FieldBindingKey, string> fieldBindings,
        IDictionary<string, ManagedMethodModel> materializedMethods)
    {
        if (!string.Equals(methodReference.AssemblyName, assemblyName, StringComparison.Ordinal) ||
            string.Equals(methodReference.SubjectId, methodReference.DefinitionSubjectId, StringComparison.Ordinal) ||
            materializedMethods.ContainsKey(methodReference.SubjectId) ||
            !definitionMethodsBySubject.TryGetValue(methodReference.DefinitionSubjectId, out var definitionMethod))
        {
            return;
        }

        var subjectSubstitutions = new Dictionary<string, string>(StringComparer.Ordinal)
        {
            [definitionMethod.DeclaringTypeSubjectId] = methodReference.DeclaringTypeSubjectId,
            [definitionMethod.SubjectId] = methodReference.SubjectId,
        };

        foreach (var fieldBinding in fieldBindings)
        {
            if (string.Equals(fieldBinding.Key.DeclaringTypeSubjectId, methodReference.DeclaringTypeSubjectId, StringComparison.Ordinal))
            {
                subjectSubstitutions[fieldBinding.Key.DefinitionSubjectId] = fieldBinding.Value;
            }
        }

        var parameters = definitionMethod.Parameters
            .Select(parameter => new ManagedParameterModel
            {
                Name = parameter.Name,
                Type = SubstituteText(parameter.Type, methodReference.Substitutions, subjectSubstitutions),
            })
            .ToList();

        materializedMethods[methodReference.SubjectId] = new ManagedMethodModel
        {
            AssemblyName = methodReference.AssemblyName,
            DeclaringTypeSubjectId = methodReference.DeclaringTypeSubjectId,
            DeclaringTypeDisplayName = methodReference.DeclaringTypeDisplayName,
            Name = methodReference.Name,
            ReturnType = methodReference.ReturnType,
            SubjectId = methodReference.SubjectId,
            DefinitionSubjectId = definitionMethod.SubjectId,
            Signature = ManagedNaming.CreateMethodSignature(
                methodReference.ReturnType,
                methodReference.DeclaringTypeDisplayName,
                methodReference.Name,
                parameters.Select(parameter => parameter.Type).ToList()),
            IsStatic = definitionMethod.IsStatic,
            MetadataToken = methodReference.MetadataToken,
            Parameters = parameters,
            Import = null,
            Body = SubstituteMethodBody(definitionMethod.Body, methodReference.Substitutions, subjectSubstitutions),
        };
    }

    private static ManagedMethodBodyModel SubstituteMethodBody(
        ManagedMethodBodyModel body,
        IReadOnlyDictionary<string, string> substitutions,
        IReadOnlyDictionary<string, string> subjectSubstitutions)
    {
        return new ManagedMethodBodyModel
        {
            Blocks = body.Blocks.Select(block => new ManagedBlockModel
            {
                BlockId = block.BlockId,
                Instructions = block.Instructions.Select(instruction => new ManagedInstructionModel
                {
                    Op = instruction.Op,
                    Operand = SubstituteOperand(instruction.Operand, substitutions, subjectSubstitutions),
                    ResultType = instruction.ResultType is null
                        ? null
                        : SubstituteText(instruction.ResultType, substitutions, subjectSubstitutions),
                    Callee = instruction.Callee is null
                        ? null
                        : SubstituteText(instruction.Callee, substitutions, subjectSubstitutions),
                    Reference = instruction.Reference is null
                        ? null
                        : new ManagedInstructionReference
                        {
                            AssemblyName = instruction.Reference.AssemblyName,
                            SubjectKind = instruction.Reference.SubjectKind,
                            SubjectId = SubstituteText(instruction.Reference.SubjectId, substitutions, subjectSubstitutions),
                        },
                }).ToList(),
            }).ToList(),
        };
    }

    private static object? SubstituteOperand(
        object? operand,
        IReadOnlyDictionary<string, string> substitutions,
        IReadOnlyDictionary<string, string> subjectSubstitutions)
    {
        return operand switch
        {
            string value => SubstituteText(value, substitutions, subjectSubstitutions),
            _ => operand,
        };
    }

    private static string SubstituteText(
        string value,
        IReadOnlyDictionary<string, string> substitutions,
        IReadOnlyDictionary<string, string> subjectSubstitutions)
    {
        if (subjectSubstitutions.TryGetValue(value, out var exactReplacement))
        {
            value = exactReplacement;
        }

        foreach (var (placeholder, replacement) in substitutions.OrderByDescending(pair => pair.Key.Length))
        {
            value = value.Replace(placeholder, replacement, StringComparison.Ordinal);
        }

        return value;
    }

    private static ImmutableDictionary<string, string> CreateSubstitutionMap(
        IReadOnlyList<string> typeArguments,
        IReadOnlyList<string> methodArguments)
    {
        var builder = ImmutableDictionary.CreateBuilder<string, string>(StringComparer.Ordinal);

        for (var index = 0; index < typeArguments.Count; index++)
        {
            builder[$"!{index}"] = typeArguments[index];
        }

        for (var index = 0; index < methodArguments.Count; index++)
        {
            builder[$"!!{index}"] = methodArguments[index];
        }

        return builder.ToImmutable();
    }
}

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
        var subjectId = ManagedNaming.CreateTypeSubjectId(CurrentAssemblyName, namespaceName, typeName);
        var displayName = ManagedNaming.CreateTypeDisplayName(CurrentAssemblyName, namespaceName, typeName);

        return new TypeIdentity
        {
            AssemblyName = CurrentAssemblyName,
            SubjectId = subjectId,
            DisplayName = displayName,
            DefinitionSubjectId = subjectId,
            DefinitionDisplayName = displayName,
            TypeArguments = [],
        };
    }

    private TypeIdentity ResolveTypeReference(TypeReferenceHandle handle)
    {
        var typeReference = _metadataReader.GetTypeReference(handle);
        var assemblyName = ResolveAssemblyName(typeReference.ResolutionScope);
        var namespaceName = _metadataReader.GetString(typeReference.Namespace);
        var typeName = _metadataReader.GetString(typeReference.Name);
        var subjectId = ManagedNaming.CreateTypeSubjectId(assemblyName, namespaceName, typeName);
        var displayName = ManagedNaming.CreateTypeDisplayName(assemblyName, namespaceName, typeName);

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
}

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
        throw new NotSupportedException("function pointers are not supported in the Stage 3 loader");
    }

    public string GetGenericInstantiation(string genericType, ImmutableArray<string> typeArguments)
    {
        return typeArguments.Any(IsGenericPlaceholder)
            ? genericType
            : ManagedNaming.CreateInstantiatedTypeDisplayName(genericType, typeArguments);
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
        throw new NotSupportedException("function pointers are not supported in the Stage 3 loader");
    }

    public TypeIdentity GetGenericInstantiation(TypeIdentity genericType, ImmutableArray<TypeIdentity> typeArguments)
    {
        var argumentDisplayNames = typeArguments.Select(argument => argument.DisplayName).ToImmutableArray();
        if (argumentDisplayNames.Any(IsGenericPlaceholder))
        {
            return new TypeIdentity
            {
                AssemblyName = genericType.AssemblyName,
                SubjectId = genericType.DefinitionSubjectId,
                DisplayName = genericType.DefinitionDisplayName,
                DefinitionSubjectId = genericType.DefinitionSubjectId,
                DefinitionDisplayName = genericType.DefinitionDisplayName,
                TypeArguments = argumentDisplayNames,
            };
        }

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

internal sealed record SignatureContext<T>(ImmutableArray<T> TypeArguments, ImmutableArray<T> MethodArguments);

internal sealed record TypeIdentity
{
    public required string AssemblyName { get; init; }

    public required string SubjectId { get; init; }

    public required string DisplayName { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string DefinitionDisplayName { get; init; }

    public required ImmutableArray<string> TypeArguments { get; init; }
}

internal record class MethodReferenceSummary
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string DeclaringTypeDisplayName { get; init; }

    public required string Name { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string ReturnType { get; init; }

    public required IReadOnlyList<string> ParameterTypes { get; init; }

    public required int MetadataToken { get; init; }

    public required ImmutableDictionary<string, string> Substitutions { get; init; }
}

internal sealed record FieldReferenceSummary
{
    public required string AssemblyName { get; init; }

    public required string DeclaringTypeSubjectId { get; init; }

    public required string Name { get; init; }

    public required string SubjectId { get; init; }

    public required string DefinitionSubjectId { get; init; }

    public required string FieldType { get; init; }

    public required int MetadataToken { get; init; }

    public required ImmutableDictionary<string, string> Substitutions { get; init; }
}

internal sealed record MethodSummary : MethodReferenceSummary
{
    public required string Signature { get; init; }

    public required bool IsStatic { get; init; }

    public required IReadOnlyList<ManagedParameterModel> Parameters { get; init; }

    public ManagedImportModel? Import { get; init; }
}

internal sealed record MaterializedGenericModels(
    IReadOnlyList<ManagedTypeModel> Types,
    IReadOnlyList<ManagedFieldModel> Fields,
    IReadOnlyList<ManagedPropertyModel> Properties,
    IReadOnlyList<ManagedMethodModel> Methods);

internal sealed record FieldBindingKey(string DefinitionSubjectId, string DeclaringTypeSubjectId);

internal sealed record OwnerIndex(
    IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> FieldOwners,
    IReadOnlyDictionary<PropertyDefinitionHandle, ManagedTypeModel> PropertyOwners,
    IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> MethodOwners);
