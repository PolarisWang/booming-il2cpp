using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class AotCoreIrLowering
{

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
        // Strip byref suffix (&) for underlying type resolution;
        // the caller uses CarrierKindCode to determine the native ABI type.
        string innerType;
        bool isByRef;
        if (typeIdentity.Length > 1 && typeIdentity[^1] == '&')
        {
            innerType = typeIdentity[..^1];
            isByRef = true;
        }
        else
        {
            innerType = typeIdentity;
            isByRef = false;
        }

        var managedType = TryResolveManagedType(innerType, assemblyName, managedTypes);
        var resolvedTypeShape = ResolveTypeShape(managedType, innerType);

        // If this is a byref parameter, determine the right ByRef carrier kind
        // and delegate to ByRef resolution (no need to match primitive types).
        if (isByRef)
        {
            var isByRefToValueType = managedType is { IsValueType: true };
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = isByRefToValueType
                    ? AotCoreIrAbiCarrierKind.ByRefToValueType
                    : AotCoreIrAbiCarrierKind.ByRef,
                TypeSubjectId = managedType?.SubjectId,
                TypeShape = resolvedTypeShape,
            };
        }

        if (string.Equals(innerType, "System.Void", StringComparison.Ordinal))
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

        if (string.Equals(typeIdentity, "System.String", StringComparison.Ordinal))
        {
            return new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt,
                TypeSubjectId = "System.Private.CoreLib/System.String",
                TypeShape = AotCoreIrTypeShapeKind.ReferenceType,
            };
        }

        if (managedType is not null &&
            managedType.IsValueType &&
            RequiresValueTypeByValueCarrier(managedType))
        {
            // x64 ABI: value types > 16 bytes must be passed by reference
            // (hidden pointer). Both Windows x64 and System V AMD64 use this.
            var abiSize = GetValueTypeNativeSizeForAbi(managedType);
            if (abiSize > 16)
            {
                return new AotCoreIrAbiSlotArtifact
                {
                    CarrierKindCode = AotCoreIrAbiCarrierKind.ByRefToValueType,
                    TypeSubjectId = managedType.SubjectId,
                    TypeShape = AotCoreIrTypeShapeKind.ValueType,
                };
            }

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



    /// <summary>
    /// Estimate native size (bytes) of a value type for ABI decision purposes.
    /// Returns 0 for unknown types (caller falls back to pass-by-value).
    /// The x64 ABI threshold for pass-by-reference is > 16 bytes.
    /// </summary>
    private static int GetValueTypeNativeSizeForAbi(ManagedTypeModel managedType)
    {
        var sid = managedType.SubjectId;
        if (string.IsNullOrEmpty(sid)) return 0;

        // System.Numerics large types from System.Numerics.Vectors
        if (sid.Contains("System.Numerics.Matrix4x4", StringComparison.Ordinal)) return 64;
        if (sid.Contains("System.Numerics.Matrix3x2", StringComparison.Ordinal)) return 24;

        // Vector<T> — size depends on element type
        // SubjectId: "...Vector`1<System.Int32>" → 32 bytes on x64
        if (sid.Contains("System.Numerics.Vector`1<", StringComparison.Ordinal))
        {
            var elStart = sid.IndexOf('<', StringComparison.Ordinal);
            var elEnd = sid.LastIndexOf('>');
            if (elStart > 0 && elEnd > elStart)
            {
                var elType = sid[(elStart + 1)..elEnd];
                // 4-byte elements (int, uint, float) → 32 bytes (Vector256 on x64)
                if (elType.Contains("System.Int32") || elType.Contains("System.UInt32") ||
                    elType.Contains("System.Single")) return 32;
                // 8-byte elements (long, ulong, double) → 64 bytes (Vector512)
                if (elType.Contains("System.Int64") || elType.Contains("System.UInt64") ||
                    elType.Contains("System.Double")) return 64;
                // 1/2-byte elements → 16 bytes (Vector64)
                return 16;
            }
        }

        return 0; // Unknown — caller falls back to pass-by-value
    }



    private static string GetRequiredNativeSymbol(
        IReadOnlyDictionary<string, string> targetSymbols,
        string subjectId)
    {
        if (targetSymbols.TryGetValue(subjectId, out var targetSymbol) &&
            !string.IsNullOrWhiteSpace(targetSymbol))
        {
            // Sanitize symbols matching SubjectDispatch sanitization (ProjectModel.cs)
            return targetSymbol
                .Replace('<', '_')
                .Replace('>', '_');
        }

        // For cross-assembly methods without a registration entry,
        // generate a fallback symbol to avoid crashing the compilation.
        // The Planner will handle these through external runtime dispatch.
        var sanitized = subjectId
            .Replace('<', '_')
            .Replace('>', '_')
            .Replace(',', '_')
            .Replace('(', '_')
            .Replace(')', '_')
            .Replace(':', '_')
            .Replace('/', '_')
            .Replace('.', '_')
            .Replace('&', '_')
            .Replace(' ', '_')
            .Replace('[', '_')
            .Replace(']', '_');
        return $"chaos_external_{sanitized}";
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
            IsComImport = managedType?.IsComImport ?? false,
            ComInterfaceGuid = managedType?.ComInterfaceGuid,
            ComInterfaceTypeKind = managedType?.ComInterfaceTypeKind ?? 0,
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
            FieldTypeSubjectId = field.FieldType,
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
            FieldTypeSubjectId = null,
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
            if (genericDemandLookup.ContainsKey(demand.SubjectId))
            {
                continue;
            }

            genericDemandLookup[demand.SubjectId] = demand;
        }

        return genericDemandLookup;
    }

}
