using System.Collections.Immutable;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

public sealed partial class LoaderStage
{
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
        var genericInstantiationProjection = BuildGenericInstantiationProjection(
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
            .Concat(genericInstantiationProjection.Types)
            .OrderBy(model => model.MetadataToken)
            .ToList();
        var allFields = fieldModels
            .Concat(genericInstantiationProjection.Fields)
            .OrderBy(model => model.MetadataToken)
            .ToList();
        var allProperties = propertyModels
            .Concat(genericInstantiationProjection.Properties)
            .OrderBy(model => model.MetadataToken)
            .ToList();
        var allMethods = methodModels
            .Concat(genericInstantiationProjection.Methods)
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
            GenericInstantiationDemandGraph = genericInstantiationProjection.DemandGraph,
            Types = allTypes,
            Fields = allFields,
            Properties = allProperties,
            Methods = ApplyDemandDerivedRuntimeGenericContexts(allMethods, genericInstantiationProjection.DemandGraph),
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
            var matchedSubjectId = methods
                .FirstOrDefault(method => ManagedNaming.MatchesMethodSubjectId(method.SubjectId, overrideSubjectId))
                ?.SubjectId;
            if (!string.IsNullOrWhiteSpace(matchedSubjectId))
            {
                return matchedSubjectId;
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
            IsSealed = typeDefinition.Attributes.HasFlag(TypeAttributes.Sealed),
            IsComImport = typeDefinition.Attributes.HasFlag(TypeAttributes.Import),
            ComInterfaceGuid = TryGetComInterfaceGuid(metadataReader, typeHandle),
            ComInterfaceTypeKind = TryGetComInterfaceTypeKind(metadataReader, typeHandle),
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
            var propertySignature = propertyDefinition.DecodeSignature(typeResolver.TypeNameProvider, null);
            var propertyType = propertySignature.ReturnType;
            var indexParameterTypes = propertySignature.ParameterTypes.ToList();
            var isPreserved = HasPreserveAttribute(metadataReader, propertyHandle);

            models.Add(new ManagedPropertyModel
            {
                AssemblyName = assemblyName,
                DeclaringTypeSubjectId = typeModel.SubjectId,
                Name = propertyName,
                PropertyType = propertyType,
                IndexParameterTypes = indexParameterTypes,
                SubjectId = ManagedNaming.CreatePropertySubjectId(typeModel.SubjectId, propertyName, indexParameterTypes),
                DefinitionSubjectId = ManagedNaming.CreatePropertySubjectId(typeModel.DefinitionSubjectId, propertyName, indexParameterTypes),
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
                GenericParameterCount = methodSummary.GenericParameterCount,
                ReturnType = methodSummary.ReturnType,
                SubjectId = methodSummary.SubjectId,
                DefinitionSubjectId = methodSummary.DefinitionSubjectId,
                Signature = methodSummary.Signature,
                IsStatic = methodSummary.IsStatic,
                IsVirtual = methodSummary.IsVirtual,
                IsFinal = methodSummary.IsFinal,
                IsPreserved = methodSummary.IsPreserved,
                IsUnmanagedCallersOnly = methodSummary.IsUnmanagedCallersOnly,
                IsPreserveSig = methodSummary.IsPreserveSig,
                MetadataToken = methodSummary.MetadataToken,
                Parameters = methodSummary.Parameters,
                Import = methodSummary.Import,
                Body = body,
            });
        }

        return models;
    }

}
