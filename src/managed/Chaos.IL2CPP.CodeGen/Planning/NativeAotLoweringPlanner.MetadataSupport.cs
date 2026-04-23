using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{

    private CustomAttributeSupportModel BuildCustomAttributeSupportModel(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        ManagedClosureManifestArtifact closureManifest,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate)
    {
        var queryDisplayNamesByCallee = new Dictionary<string, string>(StringComparer.Ordinal);
        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                if (TryParseCustomAttributeQueryCallee(instruction.Callee, out var attributeDisplayName))
                {
                    queryDisplayNamesByCallee[instruction.Callee!] = attributeDisplayName!;
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

        foreach (var assemblyPath in EnumerateClosureAssemblyPaths(closureManifest))
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

        var queryAttributeTypeByCallee = new Dictionary<string, string>(StringComparer.Ordinal);
        foreach (var (callee, attributeDisplayName) in queryDisplayNamesByCallee)
        {
            if (!displayNameToSubjectId.TryGetValue(attributeDisplayName, out var attributeTypeSubjectId))
            {
                throw new NotSupportedException(
                    $"native-aot custom-attribute lookup could not resolve attribute type '{attributeDisplayName}' from closure metadata.");
            }

            queryAttributeTypeByCallee[callee] = attributeTypeSubjectId;
        }

        var additionalReferenceTypes = new HashSet<string>(StringComparer.Ordinal);
        var additionalInstanceFields = new HashSet<string>(StringComparer.Ordinal);
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
                if (!TryParseAttributeGetterMethodSubjectId(instruction.Callee, out var attributeTypeSubjectId, out var memberName) ||
                    string.IsNullOrWhiteSpace(attributeTypeSubjectId) ||
                    string.IsNullOrWhiteSpace(memberName) ||
                    _methodsBySubjectId.ContainsKey(instruction.Callee!) ||
                    !materializedAttributeTypes.Contains(attributeTypeSubjectId!))
                {
                    continue;
                }

                var fieldSubjectId = ResolveAttributeStorageField(attributeTypeSubjectId!, memberName!);
                syntheticGetterFieldByMethodSubjectId[instruction.Callee!] = fieldSubjectId;
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
        ManagedClosureManifestArtifact closureManifest,
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
        var seenTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);

        foreach (var assemblyPath in EnumerateClosureAssemblyPaths(closureManifest))
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
        ManagedClosureManifestArtifact closureManifest,
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
        var seenTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        var seenFieldKeys = new HashSet<string>(StringComparer.Ordinal);
        var seenMethodSubjectIds = new HashSet<string>(StringComparer.Ordinal);

        foreach (var assemblyPath in EnumerateClosureAssemblyPaths(closureManifest))
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

                        fieldEntries.Add(new ReflectionMemberFieldEntry(
                            typeEntry.SubjectId,
                            fieldName,
                            MetadataTokens.GetToken(fieldHandle)));
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
        ManagedClosureManifestArtifact closureManifest,
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
                !string.IsNullOrWhiteSpace(entry.MemberType))
            .GroupBy(entry => entry.SubjectId, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.First(), StringComparer.Ordinal);
        var closureAssemblyPathsByName = BuildClosureAssemblyPathByName(closureManifest);
        var entriesBySubjectId = new Dictionary<string, StaticFieldDataEntry>(StringComparer.Ordinal);

        foreach (var fieldSubjectId in requiredFieldSubjectIds)
        {
            if (!metadataFieldEntriesBySubjectId.TryGetValue(fieldSubjectId, out var metadataFieldEntry) ||
                string.IsNullOrWhiteSpace(metadataFieldEntry.MemberType) ||
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

