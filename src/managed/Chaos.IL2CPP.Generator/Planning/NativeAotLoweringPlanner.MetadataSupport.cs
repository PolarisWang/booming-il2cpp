using System.Collections.Immutable;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private sealed class MetadataMethodSignatureTypeNameProvider : ISignatureTypeProvider<string, object?>
    {
        private readonly MetadataReader _metadataReader;
        private readonly string _assemblyName;

        public MetadataMethodSignatureTypeNameProvider(
            MetadataReader metadataReader,
            string assemblyName)
        {
            _metadataReader = metadataReader;
            _assemblyName = assemblyName;
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
            return $"fnptr<{signature.ReturnType}({string.Join(",", signature.ParameterTypes)})>";
        }

        public string GetGenericInstantiation(string genericType, ImmutableArray<string> typeArguments)
        {
            return ManagedNaming.CreateInstantiatedTypeDisplayName(genericType, typeArguments);
        }

        public string GetGenericMethodParameter(object? genericContext, int index) => $"!!{index}";

        public string GetGenericTypeParameter(object? genericContext, int index) => $"!{index}";

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
                _ => typeCode.ToString(),
            };
        }

        public string GetSZArrayType(string elementType) => $"{elementType}[]";

        public string GetTypeFromDefinition(MetadataReader reader, TypeDefinitionHandle handle, byte rawTypeKind)
        {
            return TryResolveTypeDefinitionIdentity(reader, _assemblyName, handle, out var identity)
                ? identity.DisplayName
                : GetTypeNameFallback(
                    reader,
                    reader.GetTypeDefinition(handle).Namespace,
                    reader.GetTypeDefinition(handle).Name);
        }

        public string GetTypeFromReference(MetadataReader reader, TypeReferenceHandle handle, byte rawTypeKind)
        {
            return TryResolveTypeReferenceIdentity(reader, _assemblyName, handle, out var identity)
                ? identity.DisplayName
                : GetTypeNameFallback(
                    reader,
                    reader.GetTypeReference(handle).Namespace,
                    reader.GetTypeReference(handle).Name);
        }

        public string GetTypeFromSpecification(
            MetadataReader reader,
            object? genericContext,
            TypeSpecificationHandle handle,
            byte rawTypeKind)
        {
            return reader.GetTypeSpecification(handle).DecodeSignature(this, genericContext);
        }

        private static string GetTypeNameFallback(
            MetadataReader reader,
            StringHandle namespaceHandle,
            StringHandle typeNameHandle)
        {
            var namespaceName = reader.GetString(namespaceHandle);
            var typeName = reader.GetString(typeNameHandle);
            return string.IsNullOrEmpty(namespaceName)
                ? typeName
                : $"{namespaceName}.{typeName}";
        }
    }

    private CustomAttributeSupportModel BuildCustomAttributeSupportModel(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate)
    {
        var queryDisplayNamesByCallee = new Dictionary<string, string>(reachableMethods.Count, StringComparer.Ordinal);
        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                if (TryParseCustomAttributeQueryCallee(instruction.Callee, out var attributeDisplayName))
                {
                    queryDisplayNamesByCallee[ManagedNaming.NormalizeSubjectIdAssembly(instruction.Callee!)] = attributeDisplayName!;
                }
            }
        }

        var memberInfoIsDefinedAttributeTypeSubjectIds =
            CollectMemberInfoIsDefinedAttributeTypeSubjectIds(reachableMethods);
        var usesMemberInfoIsDefined = memberInfoIsDefinedAttributeTypeSubjectIds.Count > 0;

        if (queryDisplayNamesByCallee.Count == 0 && !usesMemberInfoIsDefined)
        {
            return CustomAttributeSupportModel.Empty;
        }

        var displayNameToSubjectId = new Dictionary<string, string>(StringComparer.Ordinal);
        var materializations = new List<CustomAttributeMaterializationPlan>();
        var materializationKeys = new HashSet<string>(StringComparer.Ordinal);
        var queriedDisplayNames = new HashSet<string>(queryDisplayNamesByCallee.Values, StringComparer.Ordinal);
        var registeredTypesByAssembly = supplementalMetadataTemplate.RegisteredTypes
            .Where(entry =>
                entry.MetadataToken != 0 &&
                string.Equals(entry.SubjectId, entry.DefinitionSubjectId, StringComparison.Ordinal))
            .GroupBy(entry => entry.AssemblyName, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.ToArray(), StringComparer.Ordinal);
        var registeredMethodsByAssembly = supplementalMetadataTemplate.RegisteredMethods
            .Where(entry =>
                entry.MetadataToken != 0 &&
                string.Equals(entry.SubjectId, entry.DefinitionSubjectId, StringComparison.Ordinal))
            .GroupBy(entry => entry.AssemblyName, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.ToArray(), StringComparer.Ordinal);

        foreach (var assemblyPath in _cachedClosureAssemblyPaths)
        {
            using var stream = File.OpenRead(assemblyPath);
            using var peReader = new PEReader(stream);
            if (!peReader.HasMetadata)
            {
                continue;
            }

            var metadataReader = peReader.GetMetadataReader();
            var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);

            if (registeredTypesByAssembly.TryGetValue(assemblyName, out var typeEntries))
            {
                foreach (var typeEntry in typeEntries)
                {
                    var handle = MetadataTokens.EntityHandle(typeEntry.MetadataToken);
                    if (handle.Kind != HandleKind.TypeDefinition)
                    {
                        continue;
                    }

                    var typeDefinition = metadataReader.GetTypeDefinition((TypeDefinitionHandle)handle);
                    CollectCustomAttributeMaterializations(
                        metadataReader,
                        assemblyName,
                        typeEntry.SubjectId,
                        CustomAttributeTargetKind.Type,
                        (uint)typeEntry.MetadataToken,
                        typeDefinition.GetCustomAttributes(),
                        queriedDisplayNames,
                        memberInfoIsDefinedAttributeTypeSubjectIds,
                        displayNameToSubjectId,
                        materializations,
                        materializationKeys);
                }
            }

            if (registeredMethodsByAssembly.TryGetValue(assemblyName, out var methodEntries))
            {
                foreach (var methodEntry in methodEntries)
                {
                    var handle = MetadataTokens.EntityHandle(methodEntry.MetadataToken);
                    if (handle.Kind != HandleKind.MethodDefinition)
                    {
                        continue;
                    }

                    var methodDefinition = metadataReader.GetMethodDefinition((MethodDefinitionHandle)handle);
                    CollectCustomAttributeMaterializations(
                        metadataReader,
                        assemblyName,
                        methodEntry.SubjectId,
                        CustomAttributeTargetKind.Method,
                        (uint)methodEntry.MetadataToken,
                        methodDefinition.GetCustomAttributes(),
                        queriedDisplayNames,
                        memberInfoIsDefinedAttributeTypeSubjectIds,
                        displayNameToSubjectId,
                        materializations,
                        materializationKeys);
                    CollectSyntheticMethodCustomAttributeMaterializations(
                        metadataReader,
                        assemblyName,
                        methodEntry.SubjectId,
                        (uint)methodEntry.MetadataToken,
                        methodDefinition,
                        queriedDisplayNames,
                        memberInfoIsDefinedAttributeTypeSubjectIds,
                        displayNameToSubjectId,
                        materializations,
                        materializationKeys);
                }
            }

            CollectClosureWideSyntheticMethodCustomAttributeMaterializations(
                metadataReader,
                assemblyName,
                queriedDisplayNames,
                memberInfoIsDefinedAttributeTypeSubjectIds,
                displayNameToSubjectId,
                materializations,
                materializationKeys);
        }

        var queryAttributeTypeByCallee = new Dictionary<string, string>(queryDisplayNamesByCallee.Count, StringComparer.Ordinal);
        foreach (var (callee, attributeDisplayName) in queryDisplayNamesByCallee)
        {
            if (!displayNameToSubjectId.TryGetValue(attributeDisplayName, out var attributeTypeSubjectId))
            {
                // Attribute type not in closure metadata — skip this materialization
                // rather than crashing. This can happen when a method references
                // System.Attribute itself (the base class of all attributes) as a
                // queried type, which is valid managed code but the closure does
                // not include System.Attribute as a registered type.
                continue;
            }

            queryAttributeTypeByCallee[callee] = attributeTypeSubjectId;
        }

        var additionalReferenceTypes = new HashSet<string>(materializations.Count, StringComparer.Ordinal);
        var additionalInstanceFields = new HashSet<string>(materializations.Count, StringComparer.Ordinal);
        var syntheticGetterFieldByMethodSubjectId = new Dictionary<string, string>(StringComparer.Ordinal);
        var materializedAttributeTypes = materializations
            .Select(plan => plan.AttributeTypeSubjectId)
            .ToHashSet(StringComparer.Ordinal);
        var requiresStringSupport = false;

        foreach (var materialization in materializations)
        {
            additionalReferenceTypes.Add(materialization.AttributeTypeSubjectId);
            foreach (var assignment in materialization.Assignments)
            {
                additionalReferenceTypes.Add(GetDeclaringTypeSubjectId(assignment.FieldSubjectId));
                additionalInstanceFields.Add(assignment.FieldSubjectId);
                if (assignment.Value.Kind is CustomAttributeLiteralKind.String or CustomAttributeLiteralKind.Null)
                {
                    requiresStringSupport = true;
                }
            }
        }

        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                var callee = ManagedNaming.NormalizeSubjectIdAssembly(instruction.Callee);
                if (!TryParseAttributeGetterMethodSubjectId(callee, out var attributeTypeSubjectId, out var memberName) ||
                    string.IsNullOrEmpty(attributeTypeSubjectId) ||
                    string.IsNullOrEmpty(memberName) ||
                    _methodsBySubjectId.ContainsKey(callee!) ||
                    !materializedAttributeTypes.Contains(attributeTypeSubjectId!))
                {
                    continue;
                }

                var fieldSubjectId = ResolveAttributeStorageField(attributeTypeSubjectId!, memberName!);
                syntheticGetterFieldByMethodSubjectId[ManagedNaming.NormalizeSubjectIdAssembly(callee!)] = fieldSubjectId;
                additionalReferenceTypes.Add(attributeTypeSubjectId!);
                additionalInstanceFields.Add(fieldSubjectId);
            }
        }

        if (requiresStringSupport)
        {
            additionalReferenceTypes.Add(StringTypeSubjectId);
        }

        return new CustomAttributeSupportModel(
            queryAttributeTypeByCallee,
            syntheticGetterFieldByMethodSubjectId,
            materializations,
            additionalReferenceTypes,
            additionalInstanceFields,
            requiresStringSupport,
            usesMemberInfoIsDefined);
    }

    private AssemblyReflectionSupportModel BuildAssemblyReflectionSupportModel(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate)
    {
        var usesAssemblyReflectionHelpers = reachableMethods.Any(method =>
            method.Instructions.Any(instruction =>
                string.Equals(instruction.Callee, GetAssemblyMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, AssemblyGetTypeMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, TypeGetTypeByNameMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, AssemblyGetNameMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, AssemblyNameGetNameMethodSubjectId, StringComparison.Ordinal)));
        if (!usesAssemblyReflectionHelpers)
        {
            return AssemblyReflectionSupportModel.Empty;
        }

        var typeEntriesByAssembly = supplementalMetadataTemplate.RegisteredTypes
            .Where(entry =>
                entry.MetadataToken != 0 &&
                string.Equals(entry.SubjectId, entry.DefinitionSubjectId, StringComparison.Ordinal))
            .GroupBy(entry => entry.AssemblyName, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.ToArray(), StringComparer.Ordinal);
        var reflectionTypeEntries = new List<AssemblyReflectionTypeEntry>();
        var seenTypeSubjectIds = new HashSet<string>(supplementalMetadataTemplate.RegisteredTypes.Count, StringComparer.Ordinal);

        foreach (var assemblyPath in _cachedClosureAssemblyPaths)
        {
            using var stream = File.OpenRead(assemblyPath);
            using var peReader = new PEReader(stream);
            if (!peReader.HasMetadata)
            {
                continue;
            }

            var metadataReader = peReader.GetMetadataReader();
            var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
            if (!typeEntriesByAssembly.TryGetValue(assemblyName, out var typeEntries))
            {
                continue;
            }

            foreach (var typeEntry in typeEntries)
            {
                if (!seenTypeSubjectIds.Add(typeEntry.SubjectId))
                {
                    continue;
                }

                var handle = MetadataTokens.EntityHandle(typeEntry.MetadataToken);
                if (handle.Kind != HandleKind.TypeDefinition)
                {
                    continue;
                }

                if (!TryResolveTypeDefinitionIdentity(
                        metadataReader,
                        assemblyName,
                        (TypeDefinitionHandle)handle,
                        out var typeIdentity))
                {
                    continue;
                }

                reflectionTypeEntries.Add(new AssemblyReflectionTypeEntry(
                    assemblyName,
                    typeEntry.SubjectId,
                    typeIdentity.DisplayName));
            }
        }

        return new AssemblyReflectionSupportModel(
            reflectionTypeEntries
                .OrderBy(entry => entry.AssemblyName, StringComparer.Ordinal)
                .ThenBy(entry => entry.TypeDisplayName, StringComparer.Ordinal)
                .ToArray());
    }

    private ReflectionMemberSupportModel BuildReflectionMemberSupportModel(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate)
    {
        var usesReflectionMemberHelpers = reachableMethods.Any(method =>
            method.Instructions.Any(instruction =>
                string.Equals(instruction.Callee, GetFieldMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, GetMethodByNameMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, GetConstructorsMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, GetParametersMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, GetGenericArgumentsMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, MemberInfoGetNameMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, MemberInfoGetDeclaringTypeMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, MemberInfoGetMetadataTokenMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, ParameterInfoGetNameMethodSubjectId, StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, GetGenericTypeDefinitionMethodSubjectId, StringComparison.Ordinal)));
        if (!usesReflectionMemberHelpers)
        {
            return ReflectionMemberSupportModel.Empty;
        }

        var typeEntriesByAssembly = supplementalMetadataTemplate.RegisteredTypes
            .Where(entry => entry.MetadataToken != 0)
            .GroupBy(entry => entry.AssemblyName, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.ToArray(), StringComparer.Ordinal);
        var methodEntriesByAssembly = supplementalMetadataTemplate.RegisteredMethods
            .Where(entry => entry.MetadataToken != 0)
            .GroupBy(entry => entry.AssemblyName, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.ToArray(), StringComparer.Ordinal);
        var typeEntries = new List<ReflectionMemberTypeEntry>();
        var fieldEntries = new List<ReflectionMemberFieldEntry>();
        var methodEntries = new List<ReflectionMemberMethodEntry>();
        var seenTypeSubjectIds = new HashSet<string>(supplementalMetadataTemplate.RegisteredTypes.Count, StringComparer.Ordinal);
        var seenFieldKeys = new HashSet<string>(supplementalMetadataTemplate.RegisteredTypes.Count, StringComparer.Ordinal);
        var seenMethodSubjectIds = new HashSet<string>(supplementalMetadataTemplate.RegisteredMethods.Count, StringComparer.Ordinal);

        foreach (var assemblyPath in _cachedClosureAssemblyPaths)
        {
            try
            {
            using var stream = File.OpenRead(assemblyPath);
            using var peReader = new PEReader(stream);
            if (!peReader.HasMetadata)
            {
                continue;
            }

            var metadataReader = peReader.GetMetadataReader();
            var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
            var assemblyTypeEntries = Array.Empty<SupplementalMetadataTypeTemplateEntry>();
            if (typeEntriesByAssembly.TryGetValue(assemblyName, out var resolvedAssemblyTypeEntries))
            {
                assemblyTypeEntries = resolvedAssemblyTypeEntries;
                foreach (var typeEntry in assemblyTypeEntries)
                {
                    if (!seenTypeSubjectIds.Add(typeEntry.SubjectId))
                    {
                        continue;
                    }

                    if (!TryResolveTypeDefinitionHandleForReflectionMemberEntry(
                            typeEntry,
                            assemblyTypeEntries,
                            out var typeDefinitionHandle))
                    {
                        continue;
                    }

                    var typeDefinition = metadataReader.GetTypeDefinition(typeDefinitionHandle);
                    var typeName = metadataReader.GetString(typeDefinition.Name);
                    var genericParameterCount = typeDefinition.GetGenericParameters().Count;
                    var genericArgumentTypeSubjectIds = typeEntry.RuntimeGenericContext?.InstantiationKey.TypeArguments?.ToArray() ?? [];
                    var metadataToken = MetadataTokens.GetToken(typeDefinitionHandle);
                    var genericDefinitionTypeSubjectId =
                        string.Equals(typeEntry.SubjectId, typeEntry.DefinitionSubjectId, StringComparison.Ordinal)
                            ? (genericParameterCount > 0 ? typeEntry.SubjectId : null)
                            : typeEntry.DefinitionSubjectId;
                    typeEntries.Add(new ReflectionMemberTypeEntry(
                        typeEntry.SubjectId,
                        typeName,
                        genericDefinitionTypeSubjectId,
                        genericArgumentTypeSubjectIds,
                        genericParameterCount,
                        metadataToken));

                    foreach (var fieldHandle in typeDefinition.GetFields())
                    {
                        var fieldDefinition = metadataReader.GetFieldDefinition(fieldHandle);
                        var fieldName = metadataReader.GetString(fieldDefinition.Name);
                        var fieldKey = $"{typeEntry.SubjectId}:{fieldName}";
                        if (!seenFieldKeys.Add(fieldKey))
                        {
                            continue;
                        }

                        long? constantValue = null;
                        var constantHandle = fieldDefinition.GetDefaultValue();
                        if (!constantHandle.IsNil)
                        {
                            try
                            {
                                var constant = metadataReader.GetConstant(constantHandle);
                                var blobReader = metadataReader.GetBlobReader(constant.Value);
                                switch ((PrimitiveTypeCode)constant.TypeCode)
                                {
                                    case PrimitiveTypeCode.Boolean:
                                        constantValue = blobReader.ReadBoolean() ? 1L : 0L;
                                        break;
                                    case PrimitiveTypeCode.Byte:
                                        constantValue = blobReader.ReadByte();
                                        break;
                                    case PrimitiveTypeCode.SByte:
                                        constantValue = blobReader.ReadSByte();
                                        break;
                                    case PrimitiveTypeCode.Int16:
                                        constantValue = blobReader.ReadInt16();
                                        break;
                                    case PrimitiveTypeCode.UInt16:
                                        constantValue = blobReader.ReadUInt16();
                                        break;
                                    case PrimitiveTypeCode.Char:
                                        constantValue = blobReader.ReadChar();
                                        break;
                                    case PrimitiveTypeCode.Int32:
                                        constantValue = blobReader.ReadInt32();
                                        break;
                                    case PrimitiveTypeCode.UInt32:
                                        constantValue = blobReader.ReadUInt32();
                                        break;
                                    case PrimitiveTypeCode.Int64:
                                        constantValue = blobReader.ReadInt64();
                                        break;
                                    case PrimitiveTypeCode.UInt64:
                                        constantValue = (long)blobReader.ReadUInt64();
                                        break;
                                }
                            }
                            catch
                            {
                                // Ignore constant read failures
                            }
                        }

                        fieldEntries.Add(new ReflectionMemberFieldEntry(
                            typeEntry.SubjectId,
                            fieldName,
                            MetadataTokens.GetToken(fieldHandle),
                            constantValue));
                    }
                }
            }

            if (!methodEntriesByAssembly.TryGetValue(assemblyName, out var assemblyMethodEntries))
            {
                continue;
            }

            foreach (var methodEntry in assemblyMethodEntries)
            {
                if (!seenMethodSubjectIds.Add(methodEntry.SubjectId))
                {
                    continue;
                }

                if (!TryResolveMethodDefinitionForReflectionMemberEntry(
                        metadataReader,
                        methodEntry,
                        assemblyTypeEntries,
                        out var methodDefinitionHandle))
                {
                    continue;
                }

                var methodDefinition = metadataReader.GetMethodDefinition(methodDefinitionHandle);
                var methodName = metadataReader.GetString(methodDefinition.Name);
                var parameterNames = methodDefinition.GetParameters()
                    .Select(parameterHandle => metadataReader.GetParameter(parameterHandle))
                    .Where(parameter => parameter.SequenceNumber > 0)
                    .OrderBy(parameter => parameter.SequenceNumber)
                    .Select(parameter => metadataReader.GetString(parameter.Name))
                    .ToArray();
                methodEntries.Add(new ReflectionMemberMethodEntry(
                    methodEntry.SubjectId,
                    methodEntry.DeclaringTypeSubjectId,
                    methodName,
                    parameterNames,
                    string.Equals(methodName, ".ctor", StringComparison.Ordinal),
                    MetadataTokens.GetToken(methodDefinitionHandle)));
            }
            }
            catch (BadImageFormatException)
            {
                // Metadata handle from different assembly — skip this assembly's entries
            }
        }

        return new ReflectionMemberSupportModel(
            typeEntries
                .OrderBy(entry => entry.TypeSubjectId, StringComparer.Ordinal)
                .ToArray(),
            fieldEntries
                .OrderBy(entry => entry.DeclaringTypeSubjectId, StringComparer.Ordinal)
                .ThenBy(entry => entry.FieldName, StringComparer.Ordinal)
                .ToArray(),
            methodEntries
                .OrderBy(entry => entry.MethodSubjectId, StringComparer.Ordinal)
                .ToArray());
    }

    private StaticFieldDataSupportModel BuildStaticFieldDataSupportModel(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        MetadataRegistrationArtifact metadataRegistration)
    {
        var requiredFieldSubjectIds = reachableMethods
            .SelectMany(method => method.Instructions)
            .Where(instruction =>
                string.Equals(instruction.Op, "ldtoken", StringComparison.Ordinal) &&
                instruction.TargetReference?.Kind == AotCoreIrReferenceKind.Field)
            .Select(instruction => instruction.TargetReference!.SubjectId)
            .Distinct(StringComparer.Ordinal)
            .ToArray();
        if (requiredFieldSubjectIds.Length == 0)
        {
            return StaticFieldDataSupportModel.Empty;
        }

        var metadataFieldEntriesBySubjectId = metadataRegistration.Registrations
            .Where(entry =>
                string.Equals(entry.RegistrationKind, "field", StringComparison.Ordinal) &&
                !string.IsNullOrEmpty(entry.MemberType))
            .GroupBy(entry => entry.SubjectId, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.First(), StringComparer.Ordinal);
        var closureAssemblyPathsByName = _closureAssemblyPathByName;
        var entriesBySubjectId = new Dictionary<string, StaticFieldDataEntry>(requiredFieldSubjectIds.Length, StringComparer.Ordinal);

        foreach (var fieldSubjectId in requiredFieldSubjectIds)
        {
            if (!metadataFieldEntriesBySubjectId.TryGetValue(fieldSubjectId, out var metadataFieldEntry) ||
                string.IsNullOrEmpty(metadataFieldEntry.MemberType) ||
                !TryParseStaticFieldDataSize(metadataFieldEntry.MemberType, out var staticFieldDataSize))
            {
                continue;
            }

            var assemblyName = GetAssemblyNameFromSubjectId(fieldSubjectId);
            if (!closureAssemblyPathsByName.TryGetValue(assemblyName, out var assemblyPath) ||
                !TryLoadStaticFieldDataBytes(assemblyPath, fieldSubjectId, staticFieldDataSize, out var bytes))
            {
                continue;
            }

            entriesBySubjectId[fieldSubjectId] = new StaticFieldDataEntry(
                fieldSubjectId,
                metadataFieldEntry.MemberType,
                bytes);
        }

        return new StaticFieldDataSupportModel(entriesBySubjectId);
    }
}

