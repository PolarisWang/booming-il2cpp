using System.Collections.Immutable;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

public sealed partial class LoaderStage
{
    private static GenericInstantiationProjection BuildGenericInstantiationProjection(
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
        var projectedTypes = new Dictionary<string, ManagedTypeModel>(StringComparer.Ordinal);
        var projectedTypeIdentities = new Dictionary<string, TypeIdentity>(StringComparer.Ordinal);
        var genericInstantiationDemandEntries = new Dictionary<string, GenericInstantiationDemandModel>(StringComparer.Ordinal);
        var projectedFields = new Dictionary<string, ManagedFieldModel>(StringComparer.Ordinal);
        var projectedProperties = new Dictionary<string, ManagedPropertyModel>(StringComparer.Ordinal);
        var fieldBindings = new Dictionary<FieldBindingKey, string>();
        var projectedMethods = new Dictionary<string, ManagedMethodModel>(StringComparer.Ordinal);

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

        // Build the set of value type subjectIds for generic specialization classification.
        var valueTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        foreach (var defType in definitionTypes)
        {
            if (defType.IsValueType && !string.IsNullOrEmpty(defType.SubjectId))
                valueTypeSubjectIds.Add(defType.SubjectId);
        }

        for (var rowNumber = 1; rowNumber <= metadataReader.GetTableRowCount(TableIndex.TypeSpec); rowNumber++)
        {
            var typeSpecificationHandle = MetadataTokens.TypeSpecificationHandle(rowNumber);
            var typeIdentity = typeResolver.ResolveTypeIdentity(typeSpecificationHandle);
            if (!string.Equals(typeIdentity.AssemblyName, assemblyName, StringComparison.Ordinal) ||
                string.Equals(typeIdentity.SubjectId, typeIdentity.DefinitionSubjectId, StringComparison.Ordinal) ||
                existingTypeSubjects.ContainsKey(typeIdentity.SubjectId) ||
                projectedTypes.ContainsKey(typeIdentity.SubjectId))
            {
                continue;
            }

            var definitionType = existingTypeSubjects[typeIdentity.DefinitionSubjectId];
            var typeSubstitutions = CreateSubstitutionMap(typeIdentity.TypeArguments, []);
            TrackTypeInstantiationDemand(
                genericInstantiationDemandEntries,
                assemblyName,
                typeIdentity,
                demandSourceKind: "typeSpec",
                valueTypeSubjectIds);
            projectedTypes[typeIdentity.SubjectId] = new ManagedTypeModel
            {
                AssemblyName = typeIdentity.AssemblyName,
                NamespaceName = definitionType.NamespaceName,
                Name = typeIdentity.DisplayName,
                SubjectId = typeIdentity.SubjectId,
                DefinitionSubjectId = typeIdentity.DefinitionSubjectId,
                DisplayName = typeIdentity.DisplayName,
                IsInterface = definitionType.IsInterface,
                IsValueType = definitionType.IsValueType,
                IsSealed = definitionType.IsSealed,
                IsComImport = definitionType.IsComImport,
                ComInterfaceGuid = definitionType.ComInterfaceGuid,
                ComInterfaceTypeKind = definitionType.ComInterfaceTypeKind,
                BaseTypeSubjectId = definitionType.BaseTypeSubjectId is null
                    ? null
                    : ProjectInstantiationText(
                        definitionType.BaseTypeSubjectId,
                        typeSubstitutions,
                        new Dictionary<string, string>(StringComparer.Ordinal)),
                ImplementedInterfaceSubjectIds = definitionType.ImplementedInterfaceSubjectIds is null
                    ? null
                    : definitionType.ImplementedInterfaceSubjectIds
                        .Select(interfaceSubjectId => ProjectInstantiationText(
                            interfaceSubjectId,
                            typeSubstitutions,
                            new Dictionary<string, string>(StringComparer.Ordinal)))
                        .ToList(),
                IsPreserved = definitionType.IsPreserved,
                MetadataToken = MetadataTokens.GetToken(typeSpecificationHandle),
            };
            // Track projected value types for generic specialization classification.
            if (definitionType.IsValueType)
                valueTypeSubjectIds.Add(typeIdentity.SubjectId);
            projectedTypeIdentities[typeIdentity.SubjectId] = typeIdentity;
        }

        foreach (var typeIdentity in projectedTypeIdentities.Values)
        {
            if (!definitionFieldsByDeclaringType.TryGetValue(typeIdentity.DefinitionSubjectId, out var closedFieldDefinitions))
            {
                continue;
            }

            foreach (var definitionField in closedFieldDefinitions)
            {
                var subjectId = ManagedNaming.CreateFieldSubjectId(typeIdentity.SubjectId, definitionField.Name);
                if (existingFieldSubjects.ContainsKey(subjectId) || projectedFields.ContainsKey(subjectId))
                {
                    continue;
                }

                var substitutions = CreateSubstitutionMap(typeIdentity.TypeArguments, []);
                projectedFields[subjectId] = new ManagedFieldModel
                {
                    AssemblyName = typeIdentity.AssemblyName,
                    DeclaringTypeSubjectId = typeIdentity.SubjectId,
                    Name = definitionField.Name,
                    FieldType = ProjectInstantiationText(
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

        foreach (var typeIdentity in projectedTypeIdentities.Values)
        {
            if (!definitionPropertiesByDeclaringType.TryGetValue(typeIdentity.DefinitionSubjectId, out var closedPropertyDefinitions))
            {
                continue;
            }

            foreach (var definitionProperty in closedPropertyDefinitions)
            {
                var substitutedIndexParameterTypes = definitionProperty.IndexParameterTypes?
                    .Select(parameterType => ProjectInstantiationText(
                        parameterType,
                        CreateSubstitutionMap(typeIdentity.TypeArguments, []),
                        new Dictionary<string, string>(StringComparer.Ordinal)))
                    .ToList();
                var subjectId = ManagedNaming.CreatePropertySubjectId(
                    typeIdentity.SubjectId,
                    definitionProperty.Name,
                    substitutedIndexParameterTypes);
                if (existingPropertySubjects.ContainsKey(subjectId) || projectedProperties.ContainsKey(subjectId))
                {
                    continue;
                }

                var substitutions = CreateSubstitutionMap(typeIdentity.TypeArguments, []);
                projectedProperties[subjectId] = new ManagedPropertyModel
                {
                    AssemblyName = typeIdentity.AssemblyName,
                    DeclaringTypeSubjectId = typeIdentity.SubjectId,
                    Name = definitionProperty.Name,
                    PropertyType = ProjectInstantiationText(
                        definitionProperty.PropertyType,
                        substitutions,
                        new Dictionary<string, string>(StringComparer.Ordinal)),
                    IndexParameterTypes = substitutedIndexParameterTypes,
                    SubjectId = subjectId,
                    DefinitionSubjectId = definitionProperty.SubjectId,
                    IsPreserved = definitionProperty.IsPreserved,
                    MetadataToken = syntheticPropertyMetadataToken++,
                };
            }
        }

        foreach (var typeIdentity in projectedTypeIdentities.Values)
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
                        .Select(parameter => ProjectInstantiationText(
                            parameter.Type,
                            substitutions,
                            new Dictionary<string, string>(StringComparer.Ordinal)))
                        .ToList();
                    var substitutedReturnType = ProjectInstantiationText(
                        definitionMethod.ReturnType,
                        substitutions,
                        new Dictionary<string, string>(StringComparer.Ordinal));
                    return ManagedNaming.CreateMethodSubjectId(
                        typeIdentity.SubjectId,
                        definitionMethod.Name,
                        substitutedReturnType,
                        substitutedParameterTypes,
                        definitionMethod.GenericParameterCount);
                },
                StringComparer.Ordinal);

            foreach (var definitionMethod in closedMethodDefinitions)
            {
                var subjectId = methodBindings[definitionMethod.SubjectId];
                if (projectedMethods.ContainsKey(subjectId))
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
                        Type = ProjectInstantiationText(parameter.Type, substitutions, subjectSubstitutions),
                    })
                    .ToList();

                var returnType = ProjectInstantiationText(definitionMethod.ReturnType, substitutions, subjectSubstitutions);
                projectedMethods[subjectId] = new ManagedMethodModel
                {
                    AssemblyName = typeIdentity.AssemblyName,
                    DeclaringTypeSubjectId = typeIdentity.SubjectId,
                    DeclaringTypeDisplayName = typeIdentity.DisplayName,
                    Name = definitionMethod.Name,
                    GenericParameterCount = definitionMethod.GenericParameterCount,
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
                IsFinal = definitionMethod.IsFinal,
                IsPreserved = definitionMethod.IsPreserved,
                IsUnmanagedCallersOnly = definitionMethod.IsUnmanagedCallersOnly,
                IsPreserveSig = definitionMethod.IsPreserveSig,
                MetadataToken = syntheticMethodMetadataToken++,
                Parameters = parameters,
                Import = null,
                Body = ProjectInstantiationMethodBody(definitionMethod.Body, substitutions, subjectSubstitutions),
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
                projectedFields.ContainsKey(fieldReference.SubjectId))
            {
                continue;
            }

            var definitionField = existingFieldSubjects.TryGetValue(fieldReference.DefinitionSubjectId, out var existingDefinitionField)
                ? existingDefinitionField
                : projectedFields.TryGetValue(fieldReference.DefinitionSubjectId, out var projectedDefinitionField)
                    ? projectedDefinitionField
                    : null;

            projectedFields[fieldReference.SubjectId] = new ManagedFieldModel
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
            TrackMethodInstantiationDemand(
                genericInstantiationDemandEntries,
                assemblyName,
                methodReference,
                demandSourceKind: "memberReference",
                valueTypeSubjectIds);
            ProjectInstantiationMethod(
                assemblyName,
                methodReference,
                definitionMethodsBySubject,
                fieldBindings,
                projectedMethods);
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
            TrackMethodInstantiationDemand(
                genericInstantiationDemandEntries,
                assemblyName,
                methodReference,
                demandSourceKind: "methodSpec",
                valueTypeSubjectIds);
            ProjectInstantiationMethod(
                assemblyName,
                methodReference,
                definitionMethodsBySubject,
                fieldBindings,
                projectedMethods);
        }

        // ── Post-projection: project types from DemandGraph entries ──────────
        // The TypeSpec loop above projects types only from TypeSpec metadata rows.
        // Types that appear ONLY as generic method arguments or MemberReference
        // declaring types are tracked in genericInstantiationDemandEntries but
        // never have ManagedTypeModel entries created.  This pass fills the gap.
        var existingSubjectIds = new HashSet<string>(existingTypeSubjects.Keys, StringComparer.Ordinal);
        existingSubjectIds.UnionWith(projectedTypes.Keys);
        foreach (var entry in genericInstantiationDemandEntries.Values)
        {
            string? typeSubjectId = null;
            string? typeDefSubjectId = null;

            if (string.Equals(entry.SubjectKind, "type", StringComparison.Ordinal))
            {
                if (existingSubjectIds.Contains(entry.SubjectId))
                    continue;
                // Strip generic arity from definition to get the base type SubjectId
                // e.g. "ParallelLoopStateFlags`1" -> "ParallelLoopStateFlags"
                typeSubjectId = entry.SubjectId;
                typeDefSubjectId = entry.DefinitionSubjectId;
            }
            else if (string.Equals(entry.SubjectKind, "method", StringComparison.Ordinal))
            {
                // Extract the declaring type from the method SubjectId
                // Format: "Assembly/Type::Method:Ret(Params)"
                int sepIdx = entry.SubjectId.IndexOf("::", StringComparison.Ordinal);
                if (sepIdx <= 0) continue;
                string declaringType = entry.SubjectId[..sepIdx];
                if (existingSubjectIds.Contains(declaringType))
                    continue;
                typeSubjectId = declaringType;
                // For method entries, the DefinitionSubjectId is the method's definition.
                // Extract the type portion from it if it contains ::.
                int defSepIdx = entry.DefinitionSubjectId.IndexOf("::", StringComparison.Ordinal);
                typeDefSubjectId = defSepIdx > 0
                    ? entry.DefinitionSubjectId[..defSepIdx]
                    : entry.DefinitionSubjectId;
            }

            if (typeSubjectId == null || typeDefSubjectId == null)
                continue;

            // Verify definition type exists (should always — it's the open generic definition)
            if (!existingTypeSubjects.TryGetValue(typeDefSubjectId, out var baseDefType))
                continue;

            projectedTypes[typeSubjectId] = new ManagedTypeModel
            {
                AssemblyName = typeSubjectId[..typeSubjectId.IndexOf('/')],
                NamespaceName = baseDefType.NamespaceName,
                Name = typeSubjectId,
                SubjectId = typeSubjectId,
                DefinitionSubjectId = typeDefSubjectId,
                DisplayName = typeSubjectId,
                IsValueType = baseDefType.IsValueType,
                IsSealed = baseDefType.IsSealed,
                IsComImport = baseDefType.IsComImport,
                ComInterfaceGuid = baseDefType.ComInterfaceGuid,
                ComInterfaceTypeKind = baseDefType.ComInterfaceTypeKind,
                BaseTypeSubjectId = baseDefType.BaseTypeSubjectId,
                ImplementedInterfaceSubjectIds = baseDefType.ImplementedInterfaceSubjectIds,
                MetadataToken = 0,
                IsInterface = baseDefType.IsInterface,
            };
            existingSubjectIds.Add(typeSubjectId);
            if (baseDefType.IsValueType)
                valueTypeSubjectIds.Add(typeSubjectId);
        }

        return new GenericInstantiationProjection
        {
            Types = projectedTypes.Values.ToList(),
            Fields = projectedFields.Values.ToList(),
            Properties = projectedProperties.Values.OrderBy(model => model.MetadataToken).ToList(),
            Methods = projectedMethods.Values.OrderBy(model => model.MetadataToken).ToList(),
            DemandGraph = BuildGenericInstantiationDemandGraph(genericInstantiationDemandEntries),
        };
    }

    private static void ProjectInstantiationMethod(
        string assemblyName,
        MethodReferenceSummary methodReference,
        IReadOnlyDictionary<string, ManagedMethodModel> definitionMethodsBySubject,
        IReadOnlyDictionary<FieldBindingKey, string> fieldBindings,
        IDictionary<string, ManagedMethodModel> projectedMethods)
    {
        if (!string.Equals(methodReference.AssemblyName, assemblyName, StringComparison.Ordinal) ||
            string.Equals(methodReference.SubjectId, methodReference.DefinitionSubjectId, StringComparison.Ordinal) ||
            projectedMethods.ContainsKey(methodReference.SubjectId) ||
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
                Type = ProjectInstantiationText(parameter.Type, methodReference.Substitutions, subjectSubstitutions),
            })
            .ToList();

        projectedMethods[methodReference.SubjectId] = new ManagedMethodModel
        {
            AssemblyName = methodReference.AssemblyName,
            DeclaringTypeSubjectId = methodReference.DeclaringTypeSubjectId,
            DeclaringTypeDisplayName = methodReference.DeclaringTypeDisplayName,
            Name = methodReference.Name,
            GenericParameterCount = methodReference.GenericParameterCount,
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
            IsFinal = definitionMethod.IsFinal,
            IsPreserved = definitionMethod.IsPreserved,
            IsUnmanagedCallersOnly = definitionMethod.IsUnmanagedCallersOnly,
            IsPreserveSig = definitionMethod.IsPreserveSig,
            MetadataToken = methodReference.MetadataToken,
            Parameters = parameters,
            Import = null,
            Body = ProjectInstantiationMethodBody(definitionMethod.Body, methodReference.Substitutions, subjectSubstitutions),
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

}
