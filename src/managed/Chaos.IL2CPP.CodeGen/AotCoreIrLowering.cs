using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed class AotCoreIrLowering
{
    public AotCoreIrArtifact Create(
        LinkedWorldModel linkedWorld,
        TypedIlIrArtifact typedIl,
        CodeRegistrationArtifact codeRegistration)
    {
        ArgumentNullException.ThrowIfNull(linkedWorld);
        ArgumentNullException.ThrowIfNull(typedIl);
        ArgumentNullException.ThrowIfNull(codeRegistration);

        var typedMethods = typedIl.Methods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        var managedTypes = linkedWorld.Types.ToDictionary(type => type.SubjectId, StringComparer.Ordinal);
        var managedFields = linkedWorld.Fields.ToDictionary(field => field.SubjectId, StringComparer.Ordinal);
        var managedMethods = linkedWorld.Methods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        var genericDemandLookup = BuildGenericDemandLookup(linkedWorld.GenericInstantiationDemandGraph);
        var targetSymbols = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .Where(registration => string.Equals(registration.RegistrationKind, "methodPointer", StringComparison.Ordinal))
            .GroupBy(registration => registration.SubjectId, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.First().Symbol, StringComparer.Ordinal);
        var methods = linkedWorld.Methods
            .Select(method => TryCreateMethod(
                method,
                GetRequiredTypedMethod(typedMethods, method.SubjectId),
                managedTypes,
                managedFields,
                managedMethods,
                targetSymbols,
                genericDemandLookup))
            .Where(method => method is not null)
            .Select(method => method!)
            .ToList();

        return new AotCoreIrArtifact
        {
            Methods = methods,
        };
    }

    private static AotCoreIrMethodArtifact? TryCreateMethod(
        ManagedMethodModel method,
        TypedIlMethodArtifact typedMethod,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields,
        IReadOnlyDictionary<string, ManagedMethodModel> managedMethods,
        IReadOnlyDictionary<string, string> targetSymbols,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel> genericDemandLookup)
    {
        var typedBlocks = typedMethod.Blocks.ToDictionary(block => block.BlockId, StringComparer.Ordinal);
        var instructions = new List<AotCoreIrInstructionArtifact>();

        foreach (var block in method.Body.Blocks)
        {
            if (!typedBlocks.TryGetValue(block.BlockId, out var typedBlock))
            {
                throw new InvalidOperationException(
                    $"typed-il block '{block.BlockId}' is missing for '{method.SubjectId}'.");
            }

            if (block.Instructions.Count != typedBlock.Instructions.Count)
            {
                throw new InvalidOperationException(
                    $"typed-il instruction count mismatch for block '{block.BlockId}' in '{method.SubjectId}'.");
            }

            for (var index = 0; index < block.Instructions.Count; index++)
            {
                var managedInstruction = block.Instructions[index];
                var typedInstruction = typedBlock.Instructions[index];

                if (!string.Equals(managedInstruction.Op, typedInstruction.Op, StringComparison.Ordinal))
                {
                    throw new InvalidOperationException(
                        $"typed-il opcode mismatch for '{method.SubjectId}' at block '{block.BlockId}', index {index}.");
                }

                var ilOffset = managedInstruction.IlOffset
                               ?? (instructions.Count == 0
                                   ? 0
                                   : instructions[^1].IlOffset + 1);

                var directCallTarget = ResolveDirectCallTarget(typedInstruction, managedMethods, targetSymbols);

                instructions.Add(new AotCoreIrInstructionArtifact
                {
                    Op = typedInstruction.Op,
                    Operand = typedInstruction.Operand,
                    IlOffset = ilOffset,
                    ResultType = typedInstruction.ResultType,
                    Callee = typedInstruction.Callee,
                    CallSiteSignature = typedInstruction.CallSiteSignature,
                    Reference = managedInstruction.Reference,
                    TargetReference = ResolveTargetReference(
                        managedInstruction,
                        typedInstruction,
                        managedTypes,
                        managedFields,
                        managedMethods,
                        genericDemandLookup),
                    RuntimeServiceKind = ResolveRuntimeServiceKind(typedInstruction),
                    TargetSymbol = directCallTarget.TargetSymbol,
                    TargetParameterCount = directCallTarget.TargetParameterCount,
                    TargetReturnType = directCallTarget.TargetReturnType,
                    DispatchKindCode = typedInstruction.DispatchKindCode,
                });
            }
        }

        var runtimeGenericContext = ResolveRuntimeGenericContext(
            method.SubjectId,
            method.DefinitionSubjectId,
            genericDemandLookup);
        var genericDiagnostic = ResolveGenericDiagnostic(
            method.SubjectId,
            method.DefinitionSubjectId,
            genericDemandLookup);

        return new AotCoreIrMethodArtifact
        {
            MethodId = typedMethod.MethodId,
            SubjectId = typedMethod.SubjectId,
            Signature = typedMethod.Signature,
            Identity = typedMethod.Identity,
            OpenDefinitionSubjectId = runtimeGenericContext?.InstantiationKey.DefinitionSubjectId,
            SharedGenericBodyId = runtimeGenericContext?.SharedGenericBodyId,
            InstantiationStubId = runtimeGenericContext?.InstantiationStubId,
            RuntimeGenericContext = runtimeGenericContext,
            GenericDiagnostic = genericDiagnostic,
            NativeSymbol = GetRequiredNativeSymbol(targetSymbols, typedMethod.SubjectId),
            IsStatic = method.IsStatic,
            ReturnType = method.ReturnType,
            ReturnAbi = ResolveAbiSlot(method.ReturnType, method.AssemblyName, managedTypes),
            ParameterCount = method.Parameters.Count,
            ParameterAbis = ResolveParameterAbis(method, managedTypes),
            LocalCount = DetermineLocalCount(instructions),
            ExceptionRegionCount = method.Body.ExceptionRegions.Count,
            ExceptionRegions = ResolveExceptionRegions(method.Body.ExceptionRegions),
            Instructions = instructions,
        };
    }

    private static IReadOnlyList<AotCoreIrExceptionRegionArtifact> ResolveExceptionRegions(
        IReadOnlyList<ManagedExceptionRegionModel> exceptionRegions)
    {
        if (exceptionRegions.Count == 0)
        {
            return [];
        }

        return exceptionRegions
            .Select(region => new AotCoreIrExceptionRegionArtifact
            {
                HandlingKindCode = ResolveExceptionRegionKind(region.HandlingKind),
                TryOffset = region.TryOffset,
                TryLength = region.TryLength,
                HandlerOffset = region.HandlerOffset,
                HandlerLength = region.HandlerLength,
                FilterOffset = region.FilterOffset,
                CatchTypeSubjectId = region.CatchTypeSubjectId,
            })
            .ToList();
    }

    private static AotCoreIrExceptionRegionKind ResolveExceptionRegionKind(string handlingKind)
    {
        return handlingKind switch
        {
            "catch" => AotCoreIrExceptionRegionKind.Catch,
            "finally" => AotCoreIrExceptionRegionKind.Finally,
            "fault" => AotCoreIrExceptionRegionKind.Fault,
            "filter" => AotCoreIrExceptionRegionKind.Filter,
            _ => throw new NotSupportedException(
                $"unsupported managed exception region kind '{handlingKind}' during AotCoreIr lowering."),
        };
    }

    private static AotCoreIrReferenceArtifact? ResolveTargetReference(
        ManagedInstructionModel managedInstruction,
        TypedIlInstructionArtifact typedInstruction,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields,
        IReadOnlyDictionary<string, ManagedMethodModel> managedMethods,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel> genericDemandLookup)
    {
        var reference = managedInstruction.Reference;
        if (reference is null)
        {
            return null;
        }

        switch (typedInstruction.Op)
        {
            case "newobj":
                if (!string.IsNullOrWhiteSpace(typedInstruction.Callee) &&
                    managedMethods.TryGetValue(typedInstruction.Callee, out var constructorMethod))
                {
                    if (managedTypes.TryGetValue(constructorMethod.DeclaringTypeSubjectId, out var constructorType))
                    {
                        return CreateTypeReference(
                            constructorType.AssemblyName,
                            constructorType.SubjectId,
                            constructorType,
                            managedTypes,
                            genericDemandLookup);
                    }

                    return CreateTypeReference(
                        constructorMethod.AssemblyName,
                        constructorMethod.DeclaringTypeSubjectId,
                        null,
                        managedTypes,
                        genericDemandLookup);
                }

                var constructorTypeSubjectId = GetMemberDeclaringTypeSubjectId(
                    !string.IsNullOrWhiteSpace(typedInstruction.Callee)
                        ? typedInstruction.Callee
                        : reference.SubjectId);
                return CreateTypeReference(
                    reference.AssemblyName,
                    constructorTypeSubjectId,
                    null,
                    managedTypes,
                    genericDemandLookup);

            case "call":
            case "callvirt":
            case "ldftn":
            case "ldvirtftn":
                if (managedMethods.TryGetValue(reference.SubjectId, out var referencedMethod))
                {
                    return CreateMethodReference(
                        reference.AssemblyName,
                        reference.SubjectId,
                        referencedMethod.DefinitionSubjectId,
                        genericDemandLookup);
                }

                return CreateMethodReference(reference.AssemblyName, reference.SubjectId, genericDemandLookup: genericDemandLookup);

            case "ldtoken":
                switch (reference.SubjectKind)
                {
                    case "type":
                        if (managedTypes.TryGetValue(reference.SubjectId, out var tokenType))
                        {
                            return CreateTypeReference(
                                tokenType.AssemblyName,
                                tokenType.SubjectId,
                                tokenType,
                                managedTypes,
                                genericDemandLookup);
                        }

                        return CreateTypeReference(
                            reference.AssemblyName,
                            reference.SubjectId,
                            null,
                            managedTypes,
                            genericDemandLookup);

                    case "field":
                        if (managedFields.TryGetValue(reference.SubjectId, out var tokenField))
                        {
                            managedTypes.TryGetValue(tokenField.DeclaringTypeSubjectId, out var tokenDeclaringType);
                            return CreateFieldReference(tokenField, tokenDeclaringType, genericDemandLookup);
                        }

                        return CreateFieldReference(
                            reference.AssemblyName,
                            reference.SubjectId,
                            reference.SubjectId,
                            GetDeclaringTypeSubjectId(reference.SubjectId),
                            null,
                            genericDemandLookup);

                    case "method":
                        if (managedMethods.TryGetValue(reference.SubjectId, out var tokenMethod))
                        {
                            return CreateMethodReference(
                                reference.AssemblyName,
                                reference.SubjectId,
                                tokenMethod.DefinitionSubjectId,
                                genericDemandLookup);
                        }

                        return CreateMethodReference(reference.AssemblyName, reference.SubjectId, genericDemandLookup: genericDemandLookup);

                    default:
                        throw new NotSupportedException(
                            $"unsupported ldtoken subject kind '{reference.SubjectKind}' during AotCoreIr lowering.");
                }

            case "ldfld":
            case "stfld":
            case "ldflda":
            case "ldsfld":
            case "stsfld":
            case "ldsflda":
                if (managedFields.TryGetValue(reference.SubjectId, out var field))
                {
                    managedTypes.TryGetValue(field.DeclaringTypeSubjectId, out var declaringType);
                    return CreateFieldReference(field, declaringType, genericDemandLookup);
                }

                return CreateFieldReference(
                    reference.AssemblyName,
                    reference.SubjectId,
                    reference.SubjectId,
                    GetDeclaringTypeSubjectId(reference.SubjectId),
                    null,
                    genericDemandLookup);

            case "newarr":
            case "castclass":
            case "isinst":
            case "box":
            case "mkrefany":
            case "unbox":
            case "unbox.any":
            case "initobj":
            case "ldobj":
            case "refanyval":
            case "sizeof":
            case "ldelema":
            case "ldelem":
            case "ldelem.ref":
            case "stobj":
            case "stelem":
            case "stelem.ref":
                if (managedTypes.TryGetValue(reference.SubjectId, out var managedType))
                {
                    return CreateTypeReference(
                        managedType.AssemblyName,
                        managedType.SubjectId,
                        managedType,
                        managedTypes,
                        genericDemandLookup);
                }

                return CreateTypeReference(
                    reference.AssemblyName,
                    reference.SubjectId,
                    null,
                    managedTypes,
                    genericDemandLookup);

            default:
                return null;
        }
    }

    private static AotCoreIrRuntimeServiceKind? ResolveRuntimeServiceKind(TypedIlInstructionArtifact instruction)
    {
        switch (instruction.Op)
        {
            case "newobj":
                return AotCoreIrRuntimeServiceKind.NewObject;

            case "ldfld":
                return AotCoreIrRuntimeServiceKind.LoadInstanceField;

            case "stfld":
                return AotCoreIrRuntimeServiceKind.StoreInstanceField;

            case "ldsfld":
                return AotCoreIrRuntimeServiceKind.LoadStaticField;

            case "stsfld":
                return AotCoreIrRuntimeServiceKind.StoreStaticField;

            case "newarr":
                return AotCoreIrRuntimeServiceKind.NewArray;

            case "castclass":
                return AotCoreIrRuntimeServiceKind.CastClass;

            case "isinst":
                return AotCoreIrRuntimeServiceKind.IsInst;

            case "box":
                return AotCoreIrRuntimeServiceKind.Box;

            case "unbox":
                return AotCoreIrRuntimeServiceKind.Unbox;

            case "unbox.any":
                return AotCoreIrRuntimeServiceKind.UnboxAny;

            case "initobj":
                return AotCoreIrRuntimeServiceKind.InitObject;

            case "ldelem":
            case "ldelem.ref":
                return AotCoreIrRuntimeServiceKind.LoadArrayElement;

            case "stelem":
            case "stelem.ref":
                return AotCoreIrRuntimeServiceKind.StoreArrayElement;

            default:
                return null;
        }
    }

    private static (string? TargetSymbol, int? TargetParameterCount, string? TargetReturnType) ResolveDirectCallTarget(
        TypedIlInstructionArtifact instruction,
        IReadOnlyDictionary<string, ManagedMethodModel> managedMethods,
        IReadOnlyDictionary<string, string> targetSymbols)
    {
        if (!string.Equals(instruction.Op, "call", StringComparison.Ordinal) &&
            !string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal))
        {
            return (null, null, null);
        }

        if (string.IsNullOrWhiteSpace(instruction.Callee))
        {
            return (null, null, null);
        }

        if (!managedMethods.TryGetValue(instruction.Callee, out var calleeMethod))
        {
            return (null, null, null);
        }

        if (!targetSymbols.TryGetValue(instruction.Callee, out var targetSymbol))
        {
            targetSymbol = ManagedNaming.CreateMethodSymbol(calleeMethod);
        }

        return (targetSymbol, calleeMethod.Parameters.Count, calleeMethod.ReturnType);
    }

    private static IReadOnlyList<AotCoreIrAbiSlotArtifact> ResolveParameterAbis(
        ManagedMethodModel method,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes)
    {
        return method.Parameters
            .Select(parameter => ResolveAbiSlot(parameter.Type, method.AssemblyName, managedTypes))
            .ToList();
    }

    private static AotCoreIrAbiSlotArtifact ResolveAbiSlot(
        string typeIdentity,
        string assemblyName,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes)
    {
        var managedType = TryResolveManagedType(typeIdentity, assemblyName, managedTypes);
        var resolvedTypeShape = ResolveTypeShape(managedType, typeIdentity);
        if (string.Equals(typeIdentity, "System.Void", StringComparison.Ordinal))
        {
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
                TypeSubjectId = managedType?.SubjectId,
                TypeShape = resolvedTypeShape,
            };
        }

        if (string.Equals(typeIdentity, "System.Int32", StringComparison.Ordinal))
        {
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
                TypeSubjectId = managedType?.SubjectId,
                TypeShape = resolvedTypeShape,
            };
        }

        if (string.Equals(typeIdentity, "System.SByte", StringComparison.Ordinal))
        {
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Int8,
                TypeSubjectId = managedType?.SubjectId,
                TypeShape = resolvedTypeShape,
            };
        }

        if (string.Equals(typeIdentity, "System.Byte", StringComparison.Ordinal))
        {
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.UInt8,
                TypeSubjectId = managedType?.SubjectId,
                TypeShape = resolvedTypeShape,
            };
        }

        if (string.Equals(typeIdentity, "System.Int16", StringComparison.Ordinal))
        {
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Int16,
                TypeSubjectId = managedType?.SubjectId,
                TypeShape = resolvedTypeShape,
            };
        }

        if (string.Equals(typeIdentity, "System.UInt16", StringComparison.Ordinal))
        {
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.UInt16,
                TypeSubjectId = managedType?.SubjectId,
                TypeShape = resolvedTypeShape,
            };
        }

        if (string.Equals(typeIdentity, "System.Single", StringComparison.Ordinal))
        {
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Float32,
                TypeSubjectId = managedType?.SubjectId,
                TypeShape = resolvedTypeShape,
            };
        }

        if (string.Equals(typeIdentity, "System.Double", StringComparison.Ordinal))
        {
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Float64,
                TypeSubjectId = managedType?.SubjectId,
                TypeShape = resolvedTypeShape,
            };
        }

        if (string.Equals(typeIdentity, "System.Int64", StringComparison.Ordinal))
        {
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
                TypeSubjectId = managedType?.SubjectId,
                TypeShape = resolvedTypeShape,
            };
        }

        if (string.Equals(typeIdentity, "System.UInt64", StringComparison.Ordinal))
        {
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.UInt64,
                TypeSubjectId = managedType?.SubjectId,
                TypeShape = resolvedTypeShape,
            };
        }

        if (managedType is not null &&
            managedType.IsValueType &&
            RequiresValueTypeByValueCarrier(managedType))
        {
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.ValueTypeByValue,
                TypeSubjectId = managedType.SubjectId,
                TypeShape = AotCoreIrTypeShapeKind.ValueType,
            };
        }

        return new AotCoreIrAbiSlotArtifact
        {
            CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt,
            TypeSubjectId = managedType?.SubjectId,
            TypeShape = resolvedTypeShape,
        };
    }

    private static ManagedTypeModel? TryResolveManagedType(
        string typeIdentity,
        string assemblyName,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes)
    {
        if (managedTypes.TryGetValue(typeIdentity, out var exactType))
        {
            return exactType;
        }

        return managedTypes.Values.FirstOrDefault(type =>
            string.Equals(type.AssemblyName, assemblyName, StringComparison.Ordinal) &&
            (string.Equals(type.DisplayName, typeIdentity, StringComparison.Ordinal) ||
             string.Equals(type.Name, typeIdentity, StringComparison.Ordinal))) ??
               managedTypes.Values.FirstOrDefault(type =>
                   string.Equals(type.DisplayName, typeIdentity, StringComparison.Ordinal));
    }

    private static bool RequiresValueTypeByValueCarrier(ManagedTypeModel managedType)
    {
        var subjectId = ManagedNaming.NormalizeSubjectIdAssembly(managedType.SubjectId);
        return !string.Equals(subjectId, "System.Private.CoreLib/System.Int32", StringComparison.Ordinal) &&
               !subjectId.StartsWith("System.Private.CoreLib/System.", StringComparison.Ordinal);
    }

    private static string GetRequiredNativeSymbol(
        IReadOnlyDictionary<string, string> targetSymbols,
        string subjectId)
    {
        if (targetSymbols.TryGetValue(subjectId, out var targetSymbol) &&
            !string.IsNullOrWhiteSpace(targetSymbol))
        {
            return targetSymbol;
        }

        throw new InvalidOperationException(
            $"method pointer registration is missing for '{subjectId}' during AotCoreIr lowering.");
    }

    private static TypedIlMethodArtifact GetRequiredTypedMethod(
        IReadOnlyDictionary<string, TypedIlMethodArtifact> typedMethods,
        string subjectId)
    {
        if (typedMethods.TryGetValue(subjectId, out var typedMethod))
        {
            return typedMethod;
        }

        throw new InvalidOperationException(
            $"typed-il method '{subjectId}' is missing during AotCoreIr lowering.");
    }

    private static int DetermineLocalCount(IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
    {
        var maxLocalIndex = -1;
        foreach (var instruction in instructions)
        {
            if (!string.Equals(instruction.Op, "ldloc", StringComparison.Ordinal) &&
                !string.Equals(instruction.Op, "ldloca", StringComparison.Ordinal) &&
                !string.Equals(instruction.Op, "stloc", StringComparison.Ordinal))
            {
                continue;
            }

            if (instruction.Operand is not int localIndex)
            {
                throw new InvalidOperationException(
                    $"AotCoreIr local instruction '{instruction.Op}' requires an Int32 operand.");
            }

            maxLocalIndex = Math.Max(maxLocalIndex, localIndex);
        }

        return maxLocalIndex + 1;
    }

    private static AotCoreIrReferenceArtifact CreateMethodReference(
        string assemblyName,
        string subjectId,
        string? definitionSubjectId = null,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel>? genericDemandLookup = null)
    {
        var runtimeGenericContext = ResolveRuntimeGenericContext(
            subjectId,
            definitionSubjectId ?? subjectId,
            genericDemandLookup);
        var genericDiagnostic = ResolveGenericDiagnostic(
            subjectId,
            definitionSubjectId ?? subjectId,
            genericDemandLookup);

        return new AotCoreIrReferenceArtifact
        {
            Kind = AotCoreIrReferenceKind.Method,
            AssemblyName = assemblyName,
            SubjectId = subjectId,
            OpenDefinitionSubjectId = runtimeGenericContext?.InstantiationKey.DefinitionSubjectId,
            SharedGenericBodyId = runtimeGenericContext?.SharedGenericBodyId,
            InstantiationStubId = runtimeGenericContext?.InstantiationStubId,
            RuntimeGenericContext = runtimeGenericContext,
            GenericDiagnostic = genericDiagnostic,
        };
    }

    private static AotCoreIrReferenceArtifact CreateTypeReference(
        string assemblyName,
        string subjectId,
        ManagedTypeModel? managedType,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel>? genericDemandLookup = null)
    {
        var arrayElementSubjectId = ResolveArrayElementSubjectId(subjectId);
        managedTypes.TryGetValue(arrayElementSubjectId ?? string.Empty, out var arrayElementType);
        var runtimeGenericContext = ResolveRuntimeGenericContext(
            subjectId,
            managedType?.DefinitionSubjectId ?? subjectId,
            genericDemandLookup);
        var genericDiagnostic = ResolveGenericDiagnostic(
            subjectId,
            managedType?.DefinitionSubjectId ?? subjectId,
            genericDemandLookup);

        return new AotCoreIrReferenceArtifact
        {
            Kind = AotCoreIrReferenceKind.Type,
            AssemblyName = assemblyName,
            SubjectId = subjectId,
            OpenDefinitionSubjectId = runtimeGenericContext?.InstantiationKey.DefinitionSubjectId,
            SharedGenericBodyId = runtimeGenericContext?.SharedGenericBodyId,
            InstantiationStubId = runtimeGenericContext?.InstantiationStubId,
            RuntimeGenericContext = runtimeGenericContext,
            GenericDiagnostic = genericDiagnostic,
            TypeShape = ResolveTypeShape(managedType, subjectId),
            IsSealed = managedType?.IsSealed ?? false,
            ArrayElementSubjectId = ResolveArrayElementSubjectId(subjectId),
            ArrayElementTypeShape = ResolveArrayElementTypeShape(managedTypes, ResolveArrayElementSubjectId(subjectId)),
            ArrayElementBaseTypeSubjectId = arrayElementType?.BaseTypeSubjectId,
            ArrayElementImplementedInterfaceSubjectIds = arrayElementType?.ImplementedInterfaceSubjectIds,
            BaseTypeSubjectId = managedType?.BaseTypeSubjectId,
            ImplementedInterfaceSubjectIds = managedType?.ImplementedInterfaceSubjectIds,
        };
    }

    private static AotCoreIrReferenceArtifact CreateFieldReference(
        ManagedFieldModel field,
        ManagedTypeModel? declaringType,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel>? genericDemandLookup = null)
    {
        var runtimeGenericContext = ResolveRuntimeGenericContext(
            field.SubjectId,
            field.DefinitionSubjectId,
            genericDemandLookup) ?? ResolveFieldRuntimeGenericContext(
            field.DefinitionSubjectId,
            field.DeclaringTypeSubjectId,
            genericDemandLookup);
        var genericDiagnostic = ResolveGenericDiagnostic(
            field.SubjectId,
            field.DefinitionSubjectId,
            genericDemandLookup);

        return new AotCoreIrReferenceArtifact
        {
            Kind = AotCoreIrReferenceKind.Field,
            AssemblyName = field.AssemblyName,
            SubjectId = field.SubjectId,
            OpenDefinitionSubjectId = runtimeGenericContext?.InstantiationKey.DefinitionSubjectId,
            SharedGenericBodyId = runtimeGenericContext?.SharedGenericBodyId,
            InstantiationStubId = runtimeGenericContext?.InstantiationStubId,
            RuntimeGenericContext = runtimeGenericContext,
            GenericDiagnostic = genericDiagnostic,
            DeclaringTypeSubjectId = field.DeclaringTypeSubjectId,
            DeclaringTypeShape = ResolveTypeShape(declaringType),
        };
    }

    private static AotCoreIrReferenceArtifact CreateFieldReference(
        string assemblyName,
        string subjectId,
        string definitionSubjectId,
        string declaringTypeSubjectId,
        ManagedTypeModel? declaringType,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel>? genericDemandLookup = null)
    {
        var runtimeGenericContext = ResolveRuntimeGenericContext(
            subjectId,
            definitionSubjectId,
            genericDemandLookup) ?? ResolveFieldRuntimeGenericContext(
            definitionSubjectId,
            declaringTypeSubjectId,
            genericDemandLookup);
        var genericDiagnostic = ResolveGenericDiagnostic(
            subjectId,
            definitionSubjectId,
            genericDemandLookup);

        return new AotCoreIrReferenceArtifact
        {
            Kind = AotCoreIrReferenceKind.Field,
            AssemblyName = assemblyName,
            SubjectId = subjectId,
            OpenDefinitionSubjectId = runtimeGenericContext?.InstantiationKey.DefinitionSubjectId,
            SharedGenericBodyId = runtimeGenericContext?.SharedGenericBodyId,
            InstantiationStubId = runtimeGenericContext?.InstantiationStubId,
            RuntimeGenericContext = runtimeGenericContext,
            GenericDiagnostic = genericDiagnostic,
            DeclaringTypeSubjectId = declaringTypeSubjectId,
            DeclaringTypeShape = ResolveTypeShape(declaringType),
        };
    }

    private static IReadOnlyDictionary<string, GenericInstantiationDemandModel> BuildGenericDemandLookup(
        GenericInstantiationDemandGraphModel? genericInstantiationDemandGraph)
    {
        var genericDemandLookup = new Dictionary<string, GenericInstantiationDemandModel>(StringComparer.Ordinal);
        if (genericInstantiationDemandGraph?.Demands is not { Count: > 0 } demands)
        {
            return genericDemandLookup;
        }

        foreach (var demand in demands)
        {
            if (genericDemandLookup.TryGetValue(demand.SubjectId, out var existingDemand))
            {
                EnsureEquivalentDemand(existingDemand, demand);
                continue;
            }

            genericDemandLookup[demand.SubjectId] = demand;
        }

        return genericDemandLookup;
    }

    private static RuntimeGenericContextArtifact? ResolveRuntimeGenericContext(
        string subjectId,
        string definitionSubjectId,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel>? genericDemandLookup)
    {
        if (genericDemandLookup is not null &&
            genericDemandLookup.TryGetValue(subjectId, out var demand))
        {
            return new RuntimeGenericContextArtifact
            {
                InstantiationKey = demand.InstantiationKey,
                SharedGenericBodyId = ManagedNaming.CreateSharedGenericBodyId(demand.InstantiationKey),
                InstantiationStubId = ManagedNaming.CreateInstantiationStubId(demand.InstantiationKey),
                SupportKindCode = demand.SupportKindCode,
                SpecializationKindCode = demand.SpecializationKindCode,
                StatusReasonCode = $"loader-demand:{demand.DemandSourceKind}",
            };
        }

        return null;
    }

    private static RuntimeGenericContextArtifact? ResolveFieldRuntimeGenericContext(
        string definitionSubjectId,
        string declaringTypeSubjectId,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel>? genericDemandLookup)
    {
        if (genericDemandLookup is null ||
            !genericDemandLookup.TryGetValue(declaringTypeSubjectId, out var declaringTypeDemand))
        {
            return null;
        }

        if (declaringTypeDemand.SubjectKind != "type" ||
            declaringTypeDemand.InstantiationKey.ContextKind != GenericContextKind.TypeInstantiation)
        {
            return null;
        }

        var instantiationKey = new GenericInstantiationKey
        {
            ContextKind = GenericContextKind.TypeInstantiation,
            DefinitionSubjectId = definitionSubjectId,
            TypeArguments = declaringTypeDemand.InstantiationKey.TypeArguments,
            MethodArguments = [],
        };

        return new RuntimeGenericContextArtifact
        {
            InstantiationKey = instantiationKey,
            SharedGenericBodyId = ManagedNaming.CreateSharedGenericBodyId(instantiationKey),
            InstantiationStubId = ManagedNaming.CreateInstantiationStubId(instantiationKey),
            SupportKindCode = declaringTypeDemand.SupportKindCode,
            SpecializationKindCode = declaringTypeDemand.SpecializationKindCode,
            StatusReasonCode = $"loader-demand:{declaringTypeDemand.DemandSourceKind}:field-projection",
        };
    }

    private static GenericDiagnosticArtifact? ResolveGenericDiagnostic(
        string subjectId,
        string definitionSubjectId,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel>? genericDemandLookup)
    {
        if (genericDemandLookup is not null &&
            genericDemandLookup.TryGetValue(subjectId, out var demand))
        {
            var diagnostic = ManagedNaming.TryCreateGenericDiagnosticArtifact(
                demand.SubjectId,
                demand.DefinitionSubjectId);
            if (diagnostic is not null)
            {
                return diagnostic with
                {
                    InstantiationKey = demand.InstantiationKey,
                };
            }

            return new GenericDiagnosticArtifact
            {
                SubjectId = demand.SubjectId,
                DefinitionSubjectId = demand.DefinitionSubjectId,
                DisplaySubjectId = demand.SubjectId,
                InstantiationKey = demand.InstantiationKey,
            };
        }

        return ManagedNaming.TryCreateGenericDiagnosticArtifact(subjectId, definitionSubjectId);
    }

    private static void EnsureEquivalentDemand(
        GenericInstantiationDemandModel existingDemand,
        GenericInstantiationDemandModel additionalDemand)
    {
        if (string.Equals(existingDemand.DefinitionSubjectId, additionalDemand.DefinitionSubjectId, StringComparison.Ordinal) &&
            existingDemand.SupportKindCode == additionalDemand.SupportKindCode &&
            existingDemand.SpecializationKindCode == additionalDemand.SpecializationKindCode &&
            existingDemand.FamilyKindCode == additionalDemand.FamilyKindCode &&
            AreEquivalentInstantiationKeys(existingDemand.InstantiationKey, additionalDemand.InstantiationKey))
        {
            return;
        }

        throw new InvalidOperationException(
            $"conflicting generic instantiation demand entries detected for '{existingDemand.SubjectId}' during AotCoreIr lowering.");
    }

    private static bool AreEquivalentInstantiationKeys(
        GenericInstantiationKey left,
        GenericInstantiationKey right)
    {
        return left.ContextKind == right.ContextKind &&
               string.Equals(left.DefinitionSubjectId, right.DefinitionSubjectId, StringComparison.Ordinal) &&
               SequenceEqual(left.TypeArguments, right.TypeArguments) &&
               SequenceEqual(left.MethodArguments, right.MethodArguments);
    }

    private static bool SequenceEqual(
        IReadOnlyList<string>? left,
        IReadOnlyList<string>? right)
    {
        if (ReferenceEquals(left, right))
        {
            return true;
        }

        if (left is null || right is null || left.Count != right.Count)
        {
            return false;
        }

        for (var index = 0; index < left.Count; index++)
        {
            if (!string.Equals(left[index], right[index], StringComparison.Ordinal))
            {
                return false;
            }
        }

        return true;
    }

    private static AotCoreIrTypeShapeKind ResolveTypeShape(
        ManagedTypeModel? managedType,
        string? fallbackTypeIdentityOrSubjectId = null)
    {
        if (managedType is null)
        {
            return IsKnownValueTypeIdentity(fallbackTypeIdentityOrSubjectId)
                ? AotCoreIrTypeShapeKind.ValueType
                : AotCoreIrTypeShapeKind.ReferenceType;
        }

        if (managedType.IsInterface)
        {
            return AotCoreIrTypeShapeKind.InterfaceType;
        }

        return managedType.IsValueType
            ? AotCoreIrTypeShapeKind.ValueType
            : AotCoreIrTypeShapeKind.ReferenceType;
    }

    private static string? ResolveArrayElementSubjectId(string subjectId)
    {
        return subjectId.EndsWith("[]", StringComparison.Ordinal)
            ? subjectId[..^2]
            : null;
    }

    private static AotCoreIrTypeShapeKind ResolveArrayElementTypeShape(
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        string? arrayElementSubjectId)
    {
        if (string.IsNullOrWhiteSpace(arrayElementSubjectId))
        {
            return default;
        }

        return managedTypes.TryGetValue(arrayElementSubjectId, out var arrayElementType)
            ? ResolveTypeShape(arrayElementType)
            : ResolveTypeShape(null, arrayElementSubjectId);
    }

    private static bool IsKnownValueTypeIdentity(string? typeIdentityOrSubjectId)
    {
        return typeIdentityOrSubjectId switch
        {
            "System.Boolean" => true,
            "System.Byte" => true,
            "System.Char" => true,
            "System.Double" => true,
            "System.Int16" => true,
            "System.Int32" => true,
            "System.Int64" => true,
            "System.IntPtr" => true,
            "System.RuntimeArgumentHandle" => true,
            "System.RuntimeFieldHandle" => true,
            "System.RuntimeMethodHandle" => true,
            "System.RuntimeTypeHandle" => true,
            "System.SByte" => true,
            "System.Single" => true,
            "System.TypedReference" => true,
            "System.UInt16" => true,
            "System.UInt32" => true,
            "System.UInt64" => true,
            "System.UIntPtr" => true,
            _ => false,
        };
    }

    private static string GetDeclaringTypeSubjectId(string fieldSubjectId)
    {
        var separatorIndex = fieldSubjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException(
                $"field subject '{fieldSubjectId}' is missing declaring type information during AotCoreIr lowering.");
        }

        return fieldSubjectId[..separatorIndex];
    }

    private static string GetMemberDeclaringTypeSubjectId(string memberSubjectId)
    {
        var separatorIndex = memberSubjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException(
                $"member subject '{memberSubjectId}' is missing declaring type information during AotCoreIr lowering.");
        }

        return memberSubjectId[..separatorIndex];
    }
}
