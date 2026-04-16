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
        return LoadAssembly(
            request.InputAssemblyPath,
            request.EntryPointSubjectIdOverride,
            requireEntryPoint: true);
    }

    public LoadedWorldModel LoadMultiple(ManagedClosureRequest request)
    {
        var assemblyPaths = new List<string> { request.InputAssemblyPath };
        if (request.AdditionalAssemblyPaths is not null)
        {
            foreach (var additionalAssemblyPath in request.AdditionalAssemblyPaths)
            {
                if (string.IsNullOrWhiteSpace(additionalAssemblyPath))
                {
                    continue;
                }

                if (!assemblyPaths.Contains(additionalAssemblyPath, StringComparer.OrdinalIgnoreCase))
                {
                    assemblyPaths.Add(additionalAssemblyPath);
                }
            }
        }

        var loadedAssemblies = assemblyPaths
            .Select(assemblyPath => LoadAssembly(
                assemblyPath,
                entryPointSubjectIdOverride: null,
                requireEntryPoint: false))
            .ToList();
        var entryAssembly = ResolveEntryAssembly(loadedAssemblies, request.EntryPointSubjectIdOverride);
        var entryPointSubjectId = !string.IsNullOrWhiteSpace(request.EntryPointSubjectIdOverride)
            ? request.EntryPointSubjectIdOverride!
            : entryAssembly.EntryPointSubjectId;

        return new LoadedWorldModel
        {
            InputAssemblyPath = request.InputAssemblyPath,
            Assembly = entryAssembly.Assembly,
            Assemblies = loadedAssemblies,
            EntryPointSubjectId = entryPointSubjectId,
            Types = loadedAssemblies.SelectMany(assembly => assembly.Types).OrderBy(model => model.MetadataToken).ToList(),
            Fields = loadedAssemblies.SelectMany(assembly => assembly.Fields).OrderBy(model => model.MetadataToken).ToList(),
            Properties = loadedAssemblies.SelectMany(assembly => assembly.Properties).OrderBy(model => model.MetadataToken).ToList(),
            Methods = loadedAssemblies.SelectMany(assembly => assembly.Methods).OrderBy(model => model.MetadataToken).ToList(),
        };
    }

    private static LoadedAssemblyModel ResolveEntryAssembly(
        IReadOnlyList<LoadedAssemblyModel> loadedAssemblies,
        string? entryPointSubjectIdOverride)
    {
        if (!string.IsNullOrWhiteSpace(entryPointSubjectIdOverride))
        {
            var entryAssembly = loadedAssemblies.FirstOrDefault(assembly =>
                assembly.Methods.Any(method => string.Equals(method.SubjectId, entryPointSubjectIdOverride, StringComparison.Ordinal)));
            if (entryAssembly is not null)
            {
                return entryAssembly;
            }

            throw new InvalidOperationException(
                $"managed closure entry point override '{entryPointSubjectIdOverride}' does not match any loaded method");
        }

        var primaryAssembly = loadedAssemblies[0];
        if (!string.IsNullOrWhiteSpace(primaryAssembly.EntryPointSubjectId))
        {
            return primaryAssembly;
        }

        throw new InvalidOperationException("managed closure input assembly does not define an entry point");
    }

    private static LoadedAssemblyModel LoadAssembly(
        string inputAssemblyPath,
        string? entryPointSubjectIdOverride,
        bool requireEntryPoint)
    {
        using var stream = File.OpenRead(inputAssemblyPath);
        using var peReader = new PEReader(stream);
        var metadataReader = peReader.GetMetadataReader();

        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
        var assembly = new AssemblyIdentityModel
        {
            Name = assemblyName,
            ModuleVersionId = metadataReader.GetGuid(metadataReader.GetModuleDefinition().Mvid),
        };

        var typeResolver = new MetadataTypeResolver(metadataReader, assemblyName);
        var typeModels = LoadTypes(metadataReader, typeResolver, assemblyName);
        var ownerIndex = BuildOwnerIndex(metadataReader, typeResolver, typeModels);
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
        var entryPointSubjectId = ResolveEntryPointSubjectId(
            entryPointSubjectIdOverride,
            peReader,
            metadataReader,
            typeResolver,
            typeModels,
            ownerIndex.MethodOwners,
            assemblyName,
            allMethods,
            requireEntryPoint);

        return new LoadedAssemblyModel
        {
            InputAssemblyPath = inputAssemblyPath,
            Assembly = assembly,
            EntryPointSubjectId = entryPointSubjectId,
            Types = allTypes,
            Fields = allFields,
            Properties = allProperties,
            Methods = allMethods,
        };
    }

    private static string ResolveEntryPointSubjectId(
        string? entryPointSubjectIdOverride,
        PEReader peReader,
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        string assemblyName,
        IReadOnlyList<ManagedMethodModel> methods,
        bool requireEntryPoint)
    {
        if (!string.IsNullOrWhiteSpace(entryPointSubjectIdOverride))
        {
            var overrideSubjectId = entryPointSubjectIdOverride!;
            if (methods.Any(method => string.Equals(method.SubjectId, overrideSubjectId, StringComparison.Ordinal)))
            {
                return overrideSubjectId;
            }

            throw new InvalidOperationException(
                $"managed closure entry point override '{overrideSubjectId}' does not match any loaded method");
        }

        var entryToken = peReader.PEHeaders.CorHeader?.EntryPointTokenOrRelativeVirtualAddress ?? 0;
        if (entryToken == 0)
        {
            if (!requireEntryPoint)
            {
                return string.Empty;
            }

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
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels)
    {
        var fieldOwners = new Dictionary<FieldDefinitionHandle, ManagedTypeModel>();
        var propertyOwners = new Dictionary<PropertyDefinitionHandle, ManagedTypeModel>();
        var methodOwners = new Dictionary<MethodDefinitionHandle, ManagedTypeModel>();

        foreach (var typeHandle in metadataReader.TypeDefinitions)
        {
            var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);
            var typeName = metadataReader.GetString(typeDefinition.Name);
            if (string.Equals(typeName, "<Module>", StringComparison.Ordinal))
            {
                continue;
            }

            var typeModel = typeModels.TryGetValue(typeHandle, out var existingTypeModel)
                ? existingTypeModel
                : CreateTypeModel(metadataReader, typeResolver, typeHandle);

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

    private static Dictionary<TypeDefinitionHandle, ManagedTypeModel> LoadTypes(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        string assemblyName)
    {
        var models = new Dictionary<TypeDefinitionHandle, ManagedTypeModel>();

        foreach (var typeHandle in metadataReader.TypeDefinitions)
        {
            var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);
            var typeName = metadataReader.GetString(typeDefinition.Name);
            if (string.Equals(typeName, "<Module>", StringComparison.Ordinal))
            {
                continue;
            }

            models[typeHandle] = CreateTypeModel(metadataReader, typeResolver, typeHandle);
        }

        return models;
    }

    private static ManagedTypeModel CreateTypeModel(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        TypeDefinitionHandle typeHandle)
    {
        var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);
        var typeName = metadataReader.GetString(typeDefinition.Name);
        var namespaceName = metadataReader.GetString(typeDefinition.Namespace);
        var typeIdentity = typeResolver.ResolveTypeIdentity(typeHandle);

        return new ManagedTypeModel
        {
            AssemblyName = typeIdentity.AssemblyName,
            NamespaceName = string.IsNullOrEmpty(namespaceName) ? null : namespaceName,
            Name = typeName,
            SubjectId = typeIdentity.SubjectId,
            DefinitionSubjectId = typeIdentity.DefinitionSubjectId,
            DisplayName = typeIdentity.DisplayName,
            IsInterface = typeDefinition.Attributes.HasFlag(TypeAttributes.Interface),
            IsValueType = ResolveIsValueType(metadataReader, typeResolver, typeDefinition),
            BaseTypeSubjectId = ResolveBaseTypeSubjectId(metadataReader, typeResolver, typeDefinition),
            ImplementedInterfaceSubjectIds = ResolveImplementedInterfaceSubjectIds(metadataReader, typeResolver, typeDefinition),
            IsPreserved = HasPreserveAttribute(metadataReader, typeHandle),
            MetadataToken = MetadataTokens.GetToken(typeHandle),
        };
    }

    private static ManagedTypeModel ResolveOwningTypeModel(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        MethodDefinitionHandle handle)
    {
        if (TryResolveOwningTypeHandle(metadataReader, handle, out var typeHandle))
        {
            return typeModels.TryGetValue(typeHandle, out var typeModel)
                ? typeModel
                : CreateTypeModel(metadataReader, typeResolver, typeHandle);
        }

        throw new KeyNotFoundException($"failed to resolve declaring type for method handle {MetadataTokens.GetToken(handle):X8}");
    }

    private static ManagedTypeModel ResolveOwningTypeModel(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        FieldDefinitionHandle handle)
    {
        if (TryResolveOwningTypeHandle(metadataReader, handle, out var typeHandle))
        {
            return typeModels.TryGetValue(typeHandle, out var typeModel)
                ? typeModel
                : CreateTypeModel(metadataReader, typeResolver, typeHandle);
        }

        throw new KeyNotFoundException($"failed to resolve declaring type for field handle {MetadataTokens.GetToken(handle):X8}");
    }

    private static bool TryResolveOwningTypeHandle(
        MetadataReader metadataReader,
        MethodDefinitionHandle methodHandle,
        out TypeDefinitionHandle typeHandle)
    {
        foreach (var candidateTypeHandle in metadataReader.TypeDefinitions)
        {
            foreach (var candidateMethodHandle in metadataReader.GetTypeDefinition(candidateTypeHandle).GetMethods())
            {
                if (candidateMethodHandle.Equals(methodHandle))
                {
                    typeHandle = candidateTypeHandle;
                    return true;
                }
            }
        }

        typeHandle = default;
        return false;
    }

    private static bool TryResolveOwningTypeHandle(
        MetadataReader metadataReader,
        FieldDefinitionHandle fieldHandle,
        out TypeDefinitionHandle typeHandle)
    {
        foreach (var candidateTypeHandle in metadataReader.TypeDefinitions)
        {
            foreach (var candidateFieldHandle in metadataReader.GetTypeDefinition(candidateTypeHandle).GetFields())
            {
                if (candidateFieldHandle.Equals(fieldHandle))
                {
                    typeHandle = candidateTypeHandle;
                    return true;
                }
            }
        }

        typeHandle = default;
        return false;
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
            var isStatic = fieldDefinition.Attributes.HasFlag(FieldAttributes.Static);
            var isThreadStatic = HasThreadStaticAttribute(metadataReader, fieldHandle);
            var isPreserved = HasPreserveAttribute(metadataReader, fieldHandle);

            models.Add(new ManagedFieldModel
            {
                AssemblyName = assemblyName,
                DeclaringTypeSubjectId = typeModel.SubjectId,
                Name = fieldName,
                FieldType = fieldType,
                SubjectId = ManagedNaming.CreateFieldSubjectId(typeModel.SubjectId, fieldName),
                DefinitionSubjectId = ManagedNaming.CreateFieldSubjectId(typeModel.DefinitionSubjectId, fieldName),
                IsStatic = isStatic,
                IsThreadStatic = isThreadStatic,
                IsPreserved = isPreserved,
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
            var isPreserved = HasPreserveAttribute(metadataReader, propertyHandle);

            models.Add(new ManagedPropertyModel
            {
                AssemblyName = assemblyName,
                DeclaringTypeSubjectId = typeModel.SubjectId,
                Name = propertyName,
                PropertyType = propertyType,
                SubjectId = ManagedNaming.CreatePropertySubjectId(typeModel.SubjectId, propertyName),
                DefinitionSubjectId = ManagedNaming.CreatePropertySubjectId(typeModel.DefinitionSubjectId, propertyName),
                IsPreserved = isPreserved,
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
                IsVirtual = methodSummary.IsVirtual,
                IsPreserved = methodSummary.IsPreserved,
                IsUnmanagedCallersOnly = methodSummary.IsUnmanagedCallersOnly,
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
                ExceptionRegions = [],
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
            var instructionOffset = ilReader.Offset;
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
                instructions.Add(instruction with { IlOffset = instructionOffset });
            }
        }

        return new ManagedMethodBodyModel
        {
            // ExceptionRegions = bodyBlock.ExceptionRegions are decoded into the contracts model.
            ExceptionRegions = DecodeExceptionRegions(bodyBlock, typeResolver),
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
            ILOpCode.Volatile => null,
            ILOpCode.Constrained => SkipConstrainedInstruction(ref ilReader),
            ILOpCode.Unaligned => SkipUnalignedInstruction(ref ilReader),
            ILOpCode.Ldtoken => DecodeLdtokenInstruction(metadataReader, typeResolver, typeModels, fieldOwners, methodOwners, ref ilReader),
            ILOpCode.Ldstr => DecodeLdstrInstruction(metadataReader, ref ilReader),
            ILOpCode.Ldftn => DecodeLdftnInstruction(metadataReader, typeResolver, typeModels, methodOwners, ref ilReader),
            ILOpCode.Ldvirtftn => DecodeLdvirtftnInstruction(metadataReader, typeResolver, typeModels, methodOwners, ref ilReader),
            ILOpCode.Ldnull => new ManagedInstructionModel { Op = "ldnull", ResultType = "System.Object" },
            ILOpCode.Newarr => DecodeNewarrInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Ldlen => new ManagedInstructionModel { Op = "ldlen", ResultType = "System.IntPtr" },
            ILOpCode.Castclass => DecodeCastclassInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Isinst => DecodeIsinstInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Initobj => DecodeInitobjInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Ldobj => DecodeLdobjInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Stobj => DecodeStobjInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Mkrefany => DecodeMkrefanyInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Refanytype => DecodeSimpleInstruction("refanytype", "System.RuntimeTypeHandle"),
            ILOpCode.Refanyval => DecodeRefanyvalInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Sizeof => DecodeSizeofInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Arglist => new ManagedInstructionModel { Op = "arglist", ResultType = "System.RuntimeArgumentHandle" },
            ILOpCode.Newobj => DecodeMethodReferenceInstruction(metadataReader, typeResolver, typeModels, methodOwners, opCode, ref ilReader),
            ILOpCode.Box => DecodeBoxInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Unbox => DecodeUnboxInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Unbox_any => DecodeUnboxAnyInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Calli => DecodeCalliInstruction(metadataReader, typeResolver, methodSummary, ref ilReader),
            ILOpCode.Call => DecodeMethodReferenceInstruction(metadataReader, typeResolver, typeModels, methodOwners, opCode, ref ilReader),
            ILOpCode.Callvirt => DecodeMethodReferenceInstruction(metadataReader, typeResolver, typeModels, methodOwners, opCode, ref ilReader),
            ILOpCode.Stfld => DecodeFieldReferenceInstruction(metadataReader, typeResolver, fieldOwners, opCode, ref ilReader),
            ILOpCode.Ldfld => DecodeFieldReferenceInstruction(metadataReader, typeResolver, fieldOwners, opCode, ref ilReader),
            ILOpCode.Ldflda => DecodeFieldReferenceInstruction(metadataReader, typeResolver, fieldOwners, opCode, ref ilReader),
            ILOpCode.Stsfld => DecodeStsfldInstruction(metadataReader, typeResolver, fieldOwners, ref ilReader),
            ILOpCode.Ldsfld => DecodeLdsfldInstruction(metadataReader, typeResolver, fieldOwners, ref ilReader),
            ILOpCode.Ldsflda => DecodeFieldReferenceInstruction(metadataReader, typeResolver, fieldOwners, opCode, ref ilReader),
            ILOpCode.Ldelema => DecodeLdelemaInstruction(metadataReader, typeResolver, ref ilReader),
            ILOpCode.Stelem_ref => new ManagedInstructionModel { Op = "stelem.ref", ResultType = "System.Void" },
            ILOpCode.Stelem => DecodeArrayElementInstruction(metadataReader, typeResolver, ILOpCode.Stelem, ref ilReader),
            ILOpCode.Stelem_i4 => DecodeTypedArrayInstruction("stelem", "System.Void", "System.Int32"),
            ILOpCode.Ldelem_i4 => DecodeTypedArrayInstruction("ldelem", "System.Int32", "System.Int32"),
            ILOpCode.Stelem_r8 => DecodeTypedArrayInstruction("stelem", "System.Void", "System.Double"),
            ILOpCode.Ldelem_r8 => DecodeTypedArrayInstruction("ldelem", "System.Double", "System.Double"),
            ILOpCode.Dup => new ManagedInstructionModel { Op = "dup" },
            ILOpCode.Pop => new ManagedInstructionModel { Op = "pop" },
            ILOpCode.Ldelem => DecodeArrayElementInstruction(metadataReader, typeResolver, ILOpCode.Ldelem, ref ilReader),
            ILOpCode.Ldelem_ref => new ManagedInstructionModel { Op = "ldelem.ref", ResultType = "System.Object" },
            ILOpCode.Ldarg => DecodeLdargInstruction(methodSummary, ilReader.ReadUInt16()),
            ILOpCode.Ldarg_s => DecodeLdargInstruction(methodSummary, ilReader.ReadByte()),
            ILOpCode.Ldarg_0 => DecodeLdargInstruction(methodSummary, 0),
            ILOpCode.Ldarg_1 => DecodeLdargInstruction(methodSummary, 1),
            ILOpCode.Ldarg_2 => DecodeLdargInstruction(methodSummary, 2),
            ILOpCode.Ldarg_3 => DecodeLdargInstruction(methodSummary, 3),
            ILOpCode.Ldarga => DecodeLdargaInstruction(ilReader.ReadUInt16()),
            ILOpCode.Ldarga_s => DecodeLdargaInstruction(ilReader.ReadByte()),
            ILOpCode.Ldloc => DecodeLdlocInstruction(ilReader.ReadUInt16()),
            ILOpCode.Ldloc_s => DecodeLdlocInstruction(ilReader.ReadByte()),
            ILOpCode.Ldloc_0 => DecodeLdlocInstruction(0),
            ILOpCode.Ldloc_1 => DecodeLdlocInstruction(1),
            ILOpCode.Ldloc_2 => DecodeLdlocInstruction(2),
            ILOpCode.Ldloc_3 => DecodeLdlocInstruction(3),
            ILOpCode.Ldloca => DecodeLdlocaInstruction(ilReader.ReadUInt16()),
            ILOpCode.Ldloca_s => DecodeLdlocaInstruction(ilReader.ReadByte()),
            ILOpCode.Stloc => DecodeStlocInstruction(ilReader.ReadUInt16()),
            ILOpCode.Stloc_s => DecodeStlocInstruction(ilReader.ReadByte()),
            ILOpCode.Stloc_0 => DecodeStlocInstruction(0),
            ILOpCode.Stloc_1 => DecodeStlocInstruction(1),
            ILOpCode.Stloc_2 => DecodeStlocInstruction(2),
            ILOpCode.Stloc_3 => DecodeStlocInstruction(3),
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
            ILOpCode.Ldc_i8 => DecodeLdcI8Instruction(ilReader.ReadInt64()),
            ILOpCode.Ldc_r4 => DecodeLdcR4Instruction(ilReader.ReadSingle()),
            ILOpCode.Ldc_r8 => DecodeLdcR8Instruction(ilReader.ReadDouble()),
            ILOpCode.Ldind_i1 => DecodeSimpleInstruction("ldind.i1", "System.SByte"),
            ILOpCode.Ldind_u1 => DecodeSimpleInstruction("ldind.u1", "System.Byte"),
            ILOpCode.Ldind_i2 => DecodeSimpleInstruction("ldind.i2", "System.Int16"),
            ILOpCode.Ldind_u2 => DecodeSimpleInstruction("ldind.u2", "System.UInt16"),
            ILOpCode.Ldind_i4 => DecodeSimpleInstruction("ldind.i4", "System.Int32"),
            ILOpCode.Ldind_u4 => DecodeSimpleInstruction("ldind.u4", "System.UInt32"),
            ILOpCode.Ldind_i8 => DecodeSimpleInstruction("ldind.i8", "System.Int64"),
            ILOpCode.Ldind_i => DecodeSimpleInstruction("ldind.i", "System.IntPtr"),
            ILOpCode.Ldind_r4 => DecodeSimpleInstruction("ldind.r4", "System.Single"),
            ILOpCode.Ldind_r8 => DecodeSimpleInstruction("ldind.r8", "System.Double"),
            ILOpCode.Ldind_ref => DecodeSimpleInstruction("ldind.ref", "System.Object"),
            ILOpCode.Stind_i1 => DecodeSimpleInstruction("stind.i1", "System.Void"),
            ILOpCode.Stind_i2 => DecodeSimpleInstruction("stind.i2", "System.Void"),
            ILOpCode.Stind_i4 => DecodeSimpleInstruction("stind.i4", "System.Void"),
            ILOpCode.Stind_i8 => DecodeSimpleInstruction("stind.i8", "System.Void"),
            ILOpCode.Stind_i => DecodeSimpleInstruction("stind.i", "System.Void"),
            ILOpCode.Stind_r4 => DecodeSimpleInstruction("stind.r4", "System.Void"),
            ILOpCode.Stind_r8 => DecodeSimpleInstruction("stind.r8", "System.Void"),
            ILOpCode.Stind_ref => DecodeSimpleInstruction("stind.ref", "System.Void"),
            ILOpCode.Cpblk => new ManagedInstructionModel { Op = "cpblk", ResultType = "System.Void" },
            ILOpCode.Localloc => new ManagedInstructionModel { Op = "localloc", ResultType = "System.IntPtr" },
            ILOpCode.Conv_i => DecodeSimpleInstruction("conv.i", "System.IntPtr"),
            ILOpCode.Conv_i1 => DecodeSimpleInstruction("conv.i1", "System.SByte"),
            ILOpCode.Conv_i2 => DecodeSimpleInstruction("conv.i2", "System.Int16"),
            ILOpCode.Conv_i4 => new ManagedInstructionModel { Op = "conv.i4", ResultType = "System.Int32" },
            ILOpCode.Conv_i8 => DecodeSimpleInstruction("conv.i8", "System.Int64"),
            ILOpCode.Conv_ovf_i1 => DecodeSimpleInstruction("conv.ovf.i1", "System.SByte"),
            ILOpCode.Conv_ovf_u1 => DecodeSimpleInstruction("conv.ovf.u1", "System.Byte"),
            ILOpCode.Conv_r4 => DecodeSimpleInstruction("conv.r4", "System.Single"),
            ILOpCode.Conv_r8 => DecodeSimpleInstruction("conv.r8", "System.Double"),
            ILOpCode.Conv_u => DecodeSimpleInstruction("conv.u", "System.IntPtr"),
            ILOpCode.Conv_u1 => DecodeSimpleInstruction("conv.u1", "System.Byte"),
            ILOpCode.Conv_u2 => DecodeSimpleInstruction("conv.u2", "System.UInt16"),
            ILOpCode.Add => DecodeSimpleInstruction("add", "System.Int32"),
            ILOpCode.Add_ovf => DecodeSimpleInstruction("add.ovf", "System.Int32"),
            ILOpCode.And => DecodeSimpleInstruction("and", "System.Int32"),
            ILOpCode.Sub => DecodeSimpleInstruction("sub", "System.Int32"),
            ILOpCode.Sub_ovf => DecodeSimpleInstruction("sub.ovf", "System.Int32"),
            ILOpCode.Mul => DecodeSimpleInstruction("mul", "System.Int32"),
            ILOpCode.Mul_ovf => DecodeSimpleInstruction("mul.ovf", "System.Int32"),
            ILOpCode.Div => DecodeSimpleInstruction("div", "System.Int32"),
            ILOpCode.Rem => DecodeSimpleInstruction("rem", "System.Int32"),
            ILOpCode.Ceq => DecodeSimpleInstruction("ceq", "System.Int32"),
            ILOpCode.Clt => DecodeSimpleInstruction("clt", "System.Int32"),
            ILOpCode.Cgt => DecodeSimpleInstruction("cgt", "System.Int32"),
            ILOpCode.Cgt_un => DecodeSimpleInstruction("cgt.un", "System.Int32"),
            ILOpCode.Not => DecodeSimpleInstruction("not", "System.Int32"),
            ILOpCode.Or => DecodeSimpleInstruction("or", "System.Int32"),
            ILOpCode.Shl => DecodeSimpleInstruction("shl", "System.Int32"),
            ILOpCode.Shr => DecodeSimpleInstruction("shr", "System.Int32"),
            ILOpCode.Shr_un => DecodeSimpleInstruction("shr.un", "System.Int32"),
            ILOpCode.Xor => DecodeSimpleInstruction("xor", "System.Int32"),
            ILOpCode.Beq => DecodeBranchInstruction("beq", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Beq_s => DecodeBranchInstruction("beq", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Br => DecodeBranchInstruction("br", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Br_s => DecodeBranchInstruction("br", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Bne_un => DecodeBranchInstruction("bne.un", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Bne_un_s => DecodeBranchInstruction("bne.un", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Brtrue => DecodeBrtrueInstruction(ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Brtrue_s => DecodeBrtrueInstruction(ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Brfalse => DecodeBrfalseInstruction(ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Brfalse_s => DecodeBrfalseInstruction(ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Blt => DecodeBranchInstruction("blt", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Blt_s => DecodeBranchInstruction("blt", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Blt_un => DecodeBranchInstruction("blt.un", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Blt_un_s => DecodeBranchInstruction("blt.un", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Bgt => DecodeBranchInstruction("bgt", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Bgt_s => DecodeBranchInstruction("bgt", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Ble => DecodeBranchInstruction("ble", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Ble_s => DecodeBranchInstruction("ble", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Bge => DecodeBranchInstruction("bge", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Bge_s => DecodeBranchInstruction("bge", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Bge_un => DecodeBranchInstruction("bge.un", ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Bge_un_s => DecodeBranchInstruction("bge.un", ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Switch => DecodeSwitchInstruction(ref ilReader),
            ILOpCode.Leave => DecodeLeaveInstruction(ReadBranchTargetInt32(ref ilReader)),
            ILOpCode.Leave_s => DecodeLeaveInstruction(ReadBranchTargetSByte(ref ilReader)),
            ILOpCode.Throw => new ManagedInstructionModel { Op = "throw", ResultType = "System.Void" },
            ILOpCode.Rethrow => new ManagedInstructionModel { Op = "rethrow", ResultType = "System.Void" },
            ILOpCode.Endfilter => new ManagedInstructionModel { Op = "endfilter", ResultType = "System.Void" },
            ILOpCode.Endfinally => new ManagedInstructionModel { Op = "endfinally", ResultType = "System.Void" },
            ILOpCode.Ret => new ManagedInstructionModel { Op = "ret" },
            _ => throw new NotSupportedException($"unsupported IL opcode in loader: {opCode}"),
        };
    }

    private static ManagedInstructionModel DecodeTypeReferenceInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ILOpCode opCode,
        ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var handle = MetadataTokens.EntityHandle(token);
        var typeIdentity = typeResolver.ResolveTypeIdentity(handle);

        return new ManagedInstructionModel
        {
            Op = opCode switch
            {
                ILOpCode.Newarr => "newarr",
                ILOpCode.Box => "box",
                ILOpCode.Castclass => "castclass",
                ILOpCode.Isinst => "isinst",
                ILOpCode.Initobj => "initobj",
                ILOpCode.Ldobj => "ldobj",
                ILOpCode.Mkrefany => "mkrefany",
                ILOpCode.Refanyval => "refanyval",
                ILOpCode.Sizeof => "sizeof",
                ILOpCode.Ldelema => "ldelema",
                ILOpCode.Ldelem => "ldelem",
                ILOpCode.Stobj => "stobj",
                ILOpCode.Stelem => "stelem",
                ILOpCode.Unbox => "unbox",
                ILOpCode.Unbox_any => "unbox.any",
                _ => throw new NotSupportedException($"unsupported type reference opcode: {opCode}"),
            },
            Operand = typeIdentity.SubjectId,
            ResultType = opCode switch
            {
                ILOpCode.Newarr => $"{typeIdentity.DisplayName}[]",
                ILOpCode.Box => "System.Object",
                ILOpCode.Castclass => typeIdentity.SubjectId,
                ILOpCode.Isinst => typeIdentity.SubjectId,
                ILOpCode.Initobj => "System.Void",
                ILOpCode.Ldobj => typeIdentity.SubjectId,
                ILOpCode.Mkrefany => "System.TypedReference",
                ILOpCode.Refanyval => "System.IntPtr",
                ILOpCode.Sizeof => "System.Int32",
                ILOpCode.Ldelema => "System.IntPtr",
                ILOpCode.Ldelem => typeIdentity.SubjectId,
                ILOpCode.Stobj => "System.Void",
                ILOpCode.Stelem => "System.Void",
                ILOpCode.Unbox => "System.IntPtr",
                ILOpCode.Unbox_any => typeIdentity.SubjectId,
                _ => throw new NotSupportedException($"unsupported type reference opcode: {opCode}"),
            },
            Reference = new ManagedInstructionReference
            {
                AssemblyName = typeIdentity.AssemblyName,
                SubjectKind = "type",
                SubjectId = typeIdentity.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel DecodeNewarrInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Newarr, ref ilReader);
        return instruction with { Op = "newarr" };
    }

    private static ManagedInstructionModel DecodeBoxInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Box, ref ilReader);
        return instruction with { Op = "box" };
    }

    private static ManagedInstructionModel DecodeUnboxInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Unbox, ref ilReader);
        return instruction with { Op = "unbox" };
    }

    private static ManagedInstructionModel DecodeUnboxAnyInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Unbox_any, ref ilReader);
        return instruction with { Op = "unbox.any" };
    }

    private static ManagedInstructionModel DecodeLdelemaInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Ldelema, ref ilReader);
        return instruction with { Op = "ldelema" };
    }

    private static ManagedInstructionModel DecodeArrayElementInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ILOpCode opCode,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, opCode, ref ilReader);
        return instruction with
        {
            Op = opCode == ILOpCode.Ldelem ? "ldelem" : "stelem",
        };
    }

    private static ManagedInstructionModel DecodeCastclassInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Castclass, ref ilReader);
        return instruction with { Op = "castclass" };
    }

    private static ManagedInstructionModel DecodeIsinstInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Isinst, ref ilReader);
        return instruction with { Op = "isinst" };
    }

    private static ManagedInstructionModel DecodeInitobjInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Initobj, ref ilReader);
        return instruction with { Op = "initobj" };
    }

    private static ManagedInstructionModel DecodeLdobjInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Ldobj, ref ilReader);
        return instruction with { Op = "ldobj" };
    }

    private static ManagedInstructionModel DecodeMkrefanyInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Mkrefany, ref ilReader);
        return instruction with { Op = "mkrefany" };
    }

    private static ManagedInstructionModel DecodeRefanyvalInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Refanyval, ref ilReader);
        return instruction with { Op = "refanyval" };
    }

    private static ManagedInstructionModel DecodeSizeofInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Sizeof, ref ilReader);
        return instruction with { Op = "sizeof" };
    }

    private static ManagedInstructionModel DecodeStobjInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        ref BlobReader ilReader)
    {
        var instruction = DecodeTypeReferenceInstruction(metadataReader, typeResolver, ILOpCode.Stobj, ref ilReader);
        return instruction with { Op = "stobj" };
    }

    private static ManagedInstructionModel DecodeTypedArrayInstruction(string op, string resultType, string elementType)
    {
        return new ManagedInstructionModel
        {
            Op = op,
            Operand = elementType,
            ResultType = resultType,
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

    private static ManagedInstructionModel DecodeFunctionPointerInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        string op,
        ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var handle = MetadataTokens.EntityHandle(token);
        var reference = ResolveMethodReference(metadataReader, typeResolver, typeModels, methodOwners, handle);

        return new ManagedInstructionModel
        {
            Op = op,
            Operand = reference.SubjectId,
            ResultType = "System.IntPtr",
            Callee = reference.SubjectId,
            Reference = new ManagedInstructionReference
            {
                AssemblyName = reference.AssemblyName,
                SubjectKind = "method",
                SubjectId = reference.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel DecodeLdftnInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        ref BlobReader ilReader)
    {
        var instruction = DecodeFunctionPointerInstruction(
            metadataReader,
            typeResolver,
            typeModels,
            methodOwners,
            "ldftn",
            ref ilReader);
        return instruction with { Op = "ldftn" };
    }

    private static ManagedInstructionModel DecodeLdvirtftnInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        ref BlobReader ilReader)
    {
        var instruction = DecodeFunctionPointerInstruction(
            metadataReader,
            typeResolver,
            typeModels,
            methodOwners,
            "ldvirtftn",
            ref ilReader);
        return instruction with { Op = "ldvirtftn" };
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
                ILOpCode.Ldflda => "ldflda",
                ILOpCode.Stsfld => "stsfld",
                ILOpCode.Ldsfld => "ldsfld",
                ILOpCode.Ldsflda => "ldsflda",
                _ => throw new NotSupportedException($"unsupported field reference opcode: {opCode}"),
            },
            Operand = reference.SubjectId,
            ResultType = opCode switch
            {
                ILOpCode.Ldfld or ILOpCode.Ldsfld => reference.FieldType,
                ILOpCode.Ldflda or ILOpCode.Ldsflda => "System.IntPtr",
                _ => "System.Void",
            },
            Reference = new ManagedInstructionReference
            {
                AssemblyName = reference.AssemblyName,
                SubjectKind = "field",
                SubjectId = reference.SubjectId,
            },
        };
    }

    private static ManagedInstructionModel DecodeLdsfldInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        ref BlobReader ilReader)
    {
        var instruction = DecodeFieldReferenceInstruction(
            metadataReader,
            typeResolver,
            fieldOwners,
            ILOpCode.Ldsfld,
            ref ilReader);
        return instruction with { Op = "ldsfld" };
    }

    private static ManagedInstructionModel DecodeStsfldInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        ref BlobReader ilReader)
    {
        var instruction = DecodeFieldReferenceInstruction(
            metadataReader,
            typeResolver,
            fieldOwners,
            ILOpCode.Stsfld,
            ref ilReader);
        return instruction with { Op = "stsfld" };
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

    private static ManagedInstructionModel DecodeLdargaInstruction(int index)
    {
        return new ManagedInstructionModel
        {
            Op = "ldarga",
            Operand = index,
            ResultType = "System.IntPtr",
        };
    }

    private static ManagedInstructionModel DecodeLdlocInstruction(int index)
    {
        return new ManagedInstructionModel
        {
            Op = "ldloc",
            Operand = index,
        };
    }

    private static ManagedInstructionModel DecodeLdlocaInstruction(int index)
    {
        return new ManagedInstructionModel
        {
            Op = "ldloca",
            Operand = index,
            ResultType = "System.IntPtr",
        };
    }

    private static ManagedInstructionModel DecodeStlocInstruction(int index)
    {
        return new ManagedInstructionModel
        {
            Op = "stloc",
            Operand = index,
            ResultType = "System.Void",
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

    private static ManagedInstructionModel DecodeLdcI8Instruction(long value)
    {
        return new ManagedInstructionModel
        {
            Op = "ldc.i8",
            Operand = value,
            ResultType = "System.Int64",
        };
    }

    private static ManagedInstructionModel DecodeLdcR4Instruction(float value)
    {
        return new ManagedInstructionModel
        {
            Op = "ldc.r4",
            Operand = value,
            ResultType = "System.Single",
        };
    }

    private static ManagedInstructionModel DecodeLdcR8Instruction(double value)
    {
        return new ManagedInstructionModel
        {
            Op = "ldc.r8",
            Operand = value,
            ResultType = "System.Double",
        };
    }

    private static ManagedInstructionModel DecodeSimpleInstruction(string op, string resultType)
    {
        return op switch
        {
            "add" => new ManagedInstructionModel { Op = "add", ResultType = resultType },
            "sub" => new ManagedInstructionModel { Op = "sub", ResultType = resultType },
            "mul" => new ManagedInstructionModel { Op = "mul", ResultType = resultType },
            "div" => new ManagedInstructionModel { Op = "div", ResultType = resultType },
            "rem" => new ManagedInstructionModel { Op = "rem", ResultType = resultType },
            "ceq" => new ManagedInstructionModel { Op = "ceq", ResultType = resultType },
            "clt" => new ManagedInstructionModel { Op = "clt", ResultType = resultType },
            "cgt" => new ManagedInstructionModel { Op = "cgt", ResultType = resultType },
            _ => new ManagedInstructionModel
            {
                Op = op,
                ResultType = resultType,
            },
        };
    }

    private static ManagedInstructionModel DecodeCalliInstruction(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        MethodSummary methodSummary,
        ref BlobReader ilReader)
    {
        var token = ilReader.ReadInt32();
        var handle = MetadataTokens.Handle(token);
        if (handle.Kind != HandleKind.StandaloneSignature)
        {
            throw new NotSupportedException($"unsupported calli signature handle kind in loader: {handle.Kind}");
        }

        var signature = metadataReader
            .GetStandaloneSignature((StandaloneSignatureHandle)handle)
            .DecodeMethodSignature(typeResolver.TypeNameProvider, CreateSignatureContext(methodSummary));
        var parameterTypes = signature.ParameterTypes.ToArray();

        return new ManagedInstructionModel
        {
            Op = "calli",
            Operand = token,
            ResultType = signature.ReturnType,
            CallSiteSignature = new ManagedCallSiteSignature
            {
                KindCode = ManagedCallSiteKind.FunctionPointer,
                ReturnType = signature.ReturnType,
                ParameterTypes = parameterTypes,
            },
        };
    }

    private static SignatureContext<string>? CreateSignatureContext(MethodReferenceSummary methodSummary)
    {
        var typeArguments = methodSummary.Substitutions
            .Where(pair => pair.Key.StartsWith("!", StringComparison.Ordinal) && !pair.Key.StartsWith("!!", StringComparison.Ordinal))
            .OrderBy(pair => pair.Key, StringComparer.Ordinal)
            .Select(pair => pair.Value)
            .ToImmutableArray();
        var methodArguments = methodSummary.Substitutions
            .Where(pair => pair.Key.StartsWith("!!", StringComparison.Ordinal))
            .OrderBy(pair => pair.Key, StringComparer.Ordinal)
            .Select(pair => pair.Value)
            .ToImmutableArray();

        return typeArguments.IsDefaultOrEmpty && methodArguments.IsDefaultOrEmpty
            ? null
            : new SignatureContext<string>(typeArguments, methodArguments);
    }

    private static ManagedInstructionModel DecodeBranchInstruction(string op, int delta)
    {
        return new ManagedInstructionModel
        {
            Op = op,
            Operand = delta,
            ResultType = "System.Void",
        };
    }

    private static ManagedInstructionModel DecodeLeaveInstruction(int delta)
    {
        return new ManagedInstructionModel
        {
            Op = "leave",
            Operand = delta,
            ResultType = "System.Void",
        };
    }

    private static ManagedInstructionModel DecodeSwitchInstruction(ref BlobReader ilReader)
    {
        return new ManagedInstructionModel
        {
            Op = "switch",
            Operand = ReadSwitchTargets(ref ilReader),
            ResultType = "System.Void",
        };
    }

    private static ManagedInstructionModel? SkipConstrainedInstruction(ref BlobReader ilReader)
    {
        ilReader.ReadInt32();
        return null;
    }

    private static ManagedInstructionModel? SkipUnalignedInstruction(ref BlobReader ilReader)
    {
        ilReader.ReadByte();
        return null;
    }

    private static ManagedInstructionModel DecodeBrtrueInstruction(int delta)
    {
        return new ManagedInstructionModel
        {
            Op = "brtrue",
            Operand = delta,
            ResultType = "System.Void",
        };
    }

    private static ManagedInstructionModel DecodeBrfalseInstruction(int delta)
    {
        return new ManagedInstructionModel
        {
            Op = "brfalse",
            Operand = delta,
            ResultType = "System.Void",
        };
    }

    private static int ReadBranchTargetInt32(ref BlobReader ilReader)
    {
        var delta = ilReader.ReadInt32();
        return ilReader.Offset + delta;
    }

    private static int ReadBranchTargetSByte(ref BlobReader ilReader)
    {
        var delta = ilReader.ReadSByte();
        return ilReader.Offset + delta;
    }

    private static IReadOnlyList<int> ReadSwitchTargets(ref BlobReader ilReader)
    {
        var targetCount = ilReader.ReadInt32();
        var deltas = new int[targetCount];

        for (var index = 0; index < targetCount; index++)
        {
            deltas[index] = ilReader.ReadInt32();
        }

        var baseOffset = ilReader.Offset;
        var targets = new int[targetCount];
        for (var index = 0; index < targetCount; index++)
        {
            targets[index] = baseOffset + deltas[index];
        }

        return targets;
    }

    private static IReadOnlyList<ManagedExceptionRegionModel> DecodeExceptionRegions(
        MethodBodyBlock bodyBlock,
        MetadataTypeResolver typeResolver)
    {
        return bodyBlock.ExceptionRegions
            .Select(region => new ManagedExceptionRegionModel
            {
                HandlingKind = region.Kind switch
                {
                    ExceptionRegionKind.Catch => "catch",
                    ExceptionRegionKind.Finally => "finally",
                    ExceptionRegionKind.Fault => "fault",
                    ExceptionRegionKind.Filter => "filter",
                    _ => throw new NotSupportedException($"unsupported exception region kind in loader: {region.Kind}"),
                },
                TryOffset = region.TryOffset,
                TryLength = region.TryLength,
                HandlerOffset = region.HandlerOffset,
                HandlerLength = region.HandlerLength,
                FilterOffset = region.Kind == ExceptionRegionKind.Filter ? region.FilterOffset : null,
                CatchTypeSubjectId = region.Kind == ExceptionRegionKind.Catch && !region.CatchType.IsNil
                    ? typeResolver.ResolveTypeIdentity(region.CatchType).SubjectId
                    : null,
            })
            .ToList();
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
        var declaringType = methodOwners.TryGetValue(handle, out var existingDeclaringType)
            ? existingDeclaringType
            : ResolveOwningTypeModel(metadataReader, typeResolver, typeModels, handle);
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
        var isPreserved = HasPreserveAttribute(metadataReader, handle);
        var isUnmanagedCallersOnly = HasUnmanagedCallersOnlyAttribute(metadataReader, handle);

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
            IsVirtual = methodDefinition.Attributes.HasFlag(MethodAttributes.Virtual),
            IsPreserved = isPreserved,
            IsUnmanagedCallersOnly = isUnmanagedCallersOnly,
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
        var declaringType = ResolveMemberReferenceDeclaringType(metadataReader, typeResolver, memberReference);
        var signature = memberReference.DecodeMethodSignature(
            typeResolver.TypeNameProvider,
            typeResolver.CreateTypeNameContext(declaringType));
        var parameterTypes = signature.ParameterTypes.ToArray();
        var methodName = metadataReader.GetString(memberReference.Name);
        var definitionSignature = ResolveMemberReferenceDefinitionSignature(metadataReader, typeResolver, memberReference);
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

    private static TypeIdentity ResolveMemberReferenceDeclaringType(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        MemberReference memberReference)
    {
        if (memberReference.Parent.Kind == HandleKind.MethodDefinition)
        {
            var parentMethod = metadataReader.GetMethodDefinition((MethodDefinitionHandle)memberReference.Parent);
            return typeResolver.ResolveTypeIdentity(parentMethod.GetDeclaringType());
        }

        return typeResolver.ResolveTypeIdentity(memberReference.Parent);
    }

    private static MethodSignature<string> ResolveMemberReferenceDefinitionSignature(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        MemberReference memberReference)
    {
        if (memberReference.Parent.Kind == HandleKind.MethodDefinition)
        {
            return metadataReader
                .GetMethodDefinition((MethodDefinitionHandle)memberReference.Parent)
                .DecodeSignature(typeResolver.TypeNameProvider, null);
        }

        return memberReference.DecodeMethodSignature(typeResolver.TypeNameProvider, null);
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
            IsStatic = fieldDefinition.Attributes.HasFlag(FieldAttributes.Static),
            IsThreadStatic = HasThreadStaticAttribute(metadataReader, handle),
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

    private static bool HasThreadStaticAttribute(
        MetadataReader metadataReader,
        FieldDefinitionHandle fieldHandle)
    {
        const string threadStaticAttributeFullName = "System.ThreadStaticAttribute";
        var fieldDefinition = metadataReader.GetFieldDefinition(fieldHandle);

        foreach (var attributeHandle in fieldDefinition.GetCustomAttributes())
        {
            if (TryGetAttributeTypeName(metadataReader, attributeHandle, out var namespaceName, out var typeName) &&
                string.Equals($"{namespaceName}.{typeName}", threadStaticAttributeFullName, StringComparison.Ordinal))
            {
                return true;
            }
        }

        return false;
    }

    private static bool HasUnmanagedCallersOnlyAttribute(
        MetadataReader metadataReader,
        MethodDefinitionHandle methodHandle)
    {
        const string unmanagedCallersOnlyAttributeFullName = "System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute";
        var methodDefinition = metadataReader.GetMethodDefinition(methodHandle);

        foreach (var attributeHandle in methodDefinition.GetCustomAttributes())
        {
            if (TryGetAttributeTypeName(metadataReader, attributeHandle, out var namespaceName, out var typeName) &&
                string.Equals($"{namespaceName}.{typeName}", unmanagedCallersOnlyAttributeFullName, StringComparison.Ordinal))
            {
                return true;
            }
        }

        return false;
    }

    private static bool TryGetAttributeTypeName(
        MetadataReader metadataReader,
        CustomAttributeHandle attributeHandle,
        out string namespaceName,
        out string typeName)
    {
        var attribute = metadataReader.GetCustomAttribute(attributeHandle);
        return TryGetAttributeTypeName(metadataReader, attribute.Constructor, out namespaceName, out typeName);
    }

    private static bool TryGetAttributeTypeName(
        MetadataReader metadataReader,
        EntityHandle constructorHandle,
        out string namespaceName,
        out string typeName)
    {
        switch (constructorHandle.Kind)
        {
            case HandleKind.MemberReference:
                var memberReference = metadataReader.GetMemberReference((MemberReferenceHandle)constructorHandle);
                return TryGetTypeName(metadataReader, memberReference.Parent, out namespaceName, out typeName);
            case HandleKind.MethodDefinition:
                var methodDefinition = metadataReader.GetMethodDefinition((MethodDefinitionHandle)constructorHandle);
                return TryGetTypeName(metadataReader, methodDefinition.GetDeclaringType(), out namespaceName, out typeName);
            default:
                namespaceName = string.Empty;
                typeName = string.Empty;
                return false;
        }
    }

    private static bool TryGetTypeName(
        MetadataReader metadataReader,
        EntityHandle typeHandle,
        out string namespaceName,
        out string typeName)
    {
        switch (typeHandle.Kind)
        {
            case HandleKind.TypeReference:
                var typeReference = metadataReader.GetTypeReference((TypeReferenceHandle)typeHandle);
                namespaceName = metadataReader.GetString(typeReference.Namespace);
                typeName = metadataReader.GetString(typeReference.Name);
                return true;
            case HandleKind.TypeDefinition:
                var typeDefinition = metadataReader.GetTypeDefinition((TypeDefinitionHandle)typeHandle);
                namespaceName = metadataReader.GetString(typeDefinition.Namespace);
                typeName = metadataReader.GetString(typeDefinition.Name);
                return true;
            default:
                namespaceName = string.Empty;
                typeName = string.Empty;
                return false;
        }
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

            var definitionType = existingTypeSubjects[typeIdentity.DefinitionSubjectId];
            var typeSubstitutions = CreateSubstitutionMap(typeIdentity.TypeArguments, []);
            materializedTypes[typeIdentity.SubjectId] = new ManagedTypeModel
            {
                AssemblyName = typeIdentity.AssemblyName,
                NamespaceName = definitionType.NamespaceName,
                Name = typeIdentity.DisplayName,
                SubjectId = typeIdentity.SubjectId,
                DefinitionSubjectId = typeIdentity.DefinitionSubjectId,
                DisplayName = typeIdentity.DisplayName,
                IsInterface = definitionType.IsInterface,
                IsValueType = definitionType.IsValueType,
                BaseTypeSubjectId = definitionType.BaseTypeSubjectId is null
                    ? null
                    : SubstituteText(
                        definitionType.BaseTypeSubjectId,
                        typeSubstitutions,
                        new Dictionary<string, string>(StringComparer.Ordinal)),
                ImplementedInterfaceSubjectIds = definitionType.ImplementedInterfaceSubjectIds is null
                    ? null
                    : definitionType.ImplementedInterfaceSubjectIds
                        .Select(interfaceSubjectId => SubstituteText(
                            interfaceSubjectId,
                            typeSubstitutions,
                            new Dictionary<string, string>(StringComparer.Ordinal)))
                        .ToList(),
                IsPreserved = definitionType.IsPreserved,
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
                    IsStatic = definitionField.IsStatic,
                    IsThreadStatic = definitionField.IsThreadStatic,
                    IsPreserved = definitionField.IsPreserved,
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
                    IsPreserved = definitionProperty.IsPreserved,
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
                IsVirtual = definitionMethod.IsVirtual,
                IsPreserved = definitionMethod.IsPreserved,
                IsUnmanagedCallersOnly = definitionMethod.IsUnmanagedCallersOnly,
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

            var definitionField = existingFieldSubjects.TryGetValue(fieldReference.DefinitionSubjectId, out var existingDefinitionField)
                ? existingDefinitionField
                : materializedFields.TryGetValue(fieldReference.DefinitionSubjectId, out var materializedDefinitionField)
                    ? materializedDefinitionField
                    : null;

            materializedFields[fieldReference.SubjectId] = new ManagedFieldModel
            {
                AssemblyName = fieldReference.AssemblyName,
                DeclaringTypeSubjectId = fieldReference.DeclaringTypeSubjectId,
                Name = fieldReference.Name,
                FieldType = fieldReference.FieldType,
                SubjectId = fieldReference.SubjectId,
                DefinitionSubjectId = fieldReference.DefinitionSubjectId,
                IsStatic = definitionField?.IsStatic ?? fieldReference.IsStatic,
                IsThreadStatic = definitionField?.IsThreadStatic ?? fieldReference.IsThreadStatic,
                IsPreserved = definitionField?.IsPreserved ?? false,
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
            IsVirtual = definitionMethod.IsVirtual,
            IsPreserved = definitionMethod.IsPreserved,
            IsUnmanagedCallersOnly = definitionMethod.IsUnmanagedCallersOnly,
            MetadataToken = methodReference.MetadataToken,
            Parameters = parameters,
            Import = null,
            Body = SubstituteMethodBody(definitionMethod.Body, methodReference.Substitutions, subjectSubstitutions),
        };
    }

    private static bool ResolveIsValueType(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        TypeDefinition typeDefinition)
    {
        _ = metadataReader;
        if (typeDefinition.Attributes.HasFlag(TypeAttributes.Interface) ||
            typeDefinition.BaseType.IsNil)
        {
            return false;
        }

        var baseTypeIdentity = typeResolver.ResolveTypeIdentity(typeDefinition.BaseType);
        return string.Equals(baseTypeIdentity.DefinitionSubjectId, "System.Private.CoreLib/System.ValueType", StringComparison.Ordinal)
            || string.Equals(baseTypeIdentity.DefinitionSubjectId, "System.Private.CoreLib/System.Enum", StringComparison.Ordinal);
    }

    private static string? ResolveBaseTypeSubjectId(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        TypeDefinition typeDefinition)
    {
        _ = metadataReader;
        if (typeDefinition.Attributes.HasFlag(TypeAttributes.Interface) ||
            typeDefinition.BaseType.IsNil)
        {
            return null;
        }

        var baseTypeIdentity = typeResolver.ResolveTypeIdentity(typeDefinition.BaseType);
        return baseTypeIdentity.SubjectId;
    }

    private static IReadOnlyList<string> ResolveImplementedInterfaceSubjectIds(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        TypeDefinition typeDefinition)
    {
        var implementedInterfaceSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        foreach (var interfaceImplementationHandle in typeDefinition.GetInterfaceImplementations())
        {
            var interfaceImplementation = metadataReader.GetInterfaceImplementation(interfaceImplementationHandle);
            CollectImplementedInterfaceSubjectIds(
                metadataReader,
                typeResolver,
                interfaceImplementation.Interface,
                implementedInterfaceSubjectIds);
        }

        return implementedInterfaceSubjectIds
            .OrderBy(interfaceSubjectId => interfaceSubjectId, StringComparer.Ordinal)
            .ToList();
    }

    private static void CollectImplementedInterfaceSubjectIds(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        EntityHandle interfaceHandle,
        ISet<string> implementedInterfaceSubjectIds)
    {
        var interfaceIdentity = typeResolver.ResolveTypeIdentity(interfaceHandle);
        if (!implementedInterfaceSubjectIds.Add(interfaceIdentity.SubjectId))
        {
            return;
        }

        if (interfaceHandle.Kind != HandleKind.TypeDefinition)
        {
            return;
        }

        var interfaceDefinition = metadataReader.GetTypeDefinition((TypeDefinitionHandle)interfaceHandle);
        foreach (var interfaceImplementationHandle in interfaceDefinition.GetInterfaceImplementations())
        {
            var interfaceImplementation = metadataReader.GetInterfaceImplementation(interfaceImplementationHandle);
            CollectImplementedInterfaceSubjectIds(
                metadataReader,
                typeResolver,
                interfaceImplementation.Interface,
                implementedInterfaceSubjectIds);
        }
    }

    private static bool HasPreserveAttribute(
        MetadataReader metadataReader,
        TypeDefinitionHandle typeHandle)
    {
        var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);
        return HasPreserveAttribute(metadataReader, typeDefinition.GetCustomAttributes());
    }

    private static bool HasPreserveAttribute(
        MetadataReader metadataReader,
        FieldDefinitionHandle fieldHandle)
    {
        var fieldDefinition = metadataReader.GetFieldDefinition(fieldHandle);
        return HasPreserveAttribute(metadataReader, fieldDefinition.GetCustomAttributes());
    }

    private static bool HasPreserveAttribute(
        MetadataReader metadataReader,
        PropertyDefinitionHandle propertyHandle)
    {
        var propertyDefinition = metadataReader.GetPropertyDefinition(propertyHandle);
        return HasPreserveAttribute(metadataReader, propertyDefinition.GetCustomAttributes());
    }

    private static bool HasPreserveAttribute(
        MetadataReader metadataReader,
        MethodDefinitionHandle methodHandle)
    {
        var methodDefinition = metadataReader.GetMethodDefinition(methodHandle);
        return HasPreserveAttribute(metadataReader, methodDefinition.GetCustomAttributes());
    }

    private static bool HasPreserveAttribute(
        MetadataReader metadataReader,
        CustomAttributeHandleCollection attributeHandles)
    {
        foreach (var attributeHandle in attributeHandles)
        {
            if (TryGetAttributeTypeName(metadataReader, attributeHandle, out _, out var typeName) &&
                string.Equals(typeName, "PreserveAttribute", StringComparison.Ordinal))
            {
                return true;
            }
        }

        return false;
    }

    private static ManagedMethodBodyModel SubstituteMethodBody(
        ManagedMethodBodyModel body,
        IReadOnlyDictionary<string, string> substitutions,
        IReadOnlyDictionary<string, string> subjectSubstitutions)
    {
        return new ManagedMethodBodyModel
        {
            ExceptionRegions = body.ExceptionRegions.Select(region => new ManagedExceptionRegionModel
            {
                HandlingKind = region.HandlingKind,
                TryOffset = region.TryOffset,
                TryLength = region.TryLength,
                HandlerOffset = region.HandlerOffset,
                HandlerLength = region.HandlerLength,
                FilterOffset = region.FilterOffset,
                CatchTypeSubjectId = region.CatchTypeSubjectId is null
                    ? null
                    : SubstituteText(region.CatchTypeSubjectId, substitutions, subjectSubstitutions),
            }).ToList(),
            Blocks = body.Blocks.Select(block => new ManagedBlockModel
            {
                BlockId = block.BlockId,
                Instructions = block.Instructions.Select(instruction => new ManagedInstructionModel
                {
                    Op = instruction.Op,
                    Operand = SubstituteOperand(instruction.Operand, substitutions, subjectSubstitutions),
                    IlOffset = instruction.IlOffset,
                    ResultType = instruction.ResultType is null
                        ? null
                        : SubstituteText(instruction.ResultType, substitutions, subjectSubstitutions),
                    Callee = instruction.Callee is null
                        ? null
                        : SubstituteText(instruction.Callee, substitutions, subjectSubstitutions),
                    CallSiteSignature = instruction.CallSiteSignature is null
                        ? null
                        : new ManagedCallSiteSignature
                        {
                            KindCode = instruction.CallSiteSignature.KindCode,
                            ReturnType = SubstituteText(
                                instruction.CallSiteSignature.ReturnType,
                                substitutions,
                                subjectSubstitutions),
                            ParameterTypes = instruction.CallSiteSignature.ParameterTypes
                                .Select(parameterType => SubstituteText(parameterType, substitutions, subjectSubstitutions))
                                .ToList(),
                        },
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
