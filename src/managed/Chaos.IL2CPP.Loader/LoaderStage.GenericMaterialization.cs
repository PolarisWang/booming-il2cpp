using System.Collections.Immutable;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

public sealed partial class LoaderStage
{
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
