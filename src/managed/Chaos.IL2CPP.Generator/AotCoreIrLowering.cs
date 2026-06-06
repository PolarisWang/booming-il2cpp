using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed class AotCoreIrLowering
{
    // Cache: pre-built AotCoreIrInstructionArtifact for simple stack-only instructions.
    // These have no callee, no reference, no constraints — purely stack manipulation.
    // Created once and cloned with updated IlOffset to avoid per-instruction allocation overhead.
    private static readonly Dictionary<string, AotCoreIrInstructionArtifact> s_simpleInstructionCache = new(32)
    {
        ["nop"] = new() { Op = "nop", OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldnull"] = new() { Op = "ldnull", OpCode = InstructionOpCode.LdNull, IlOffset = 0 },
        ["dup"] = new() { Op = "dup", IlOffset = 0 },
        ["pop"] = new() { Op = "pop", IlOffset = 0 },
        ["ret"] = new() { Op = "ret", IlOffset = 0  },
        ["ldarg.0"] = new() { Op = "ldarg.0", OpCode = InstructionOpCode.LdArg, IlOffset = 0 },
        ["ldarg.1"] = new() { Op = "ldarg.1", OpCode = InstructionOpCode.LdArg, IlOffset = 0 },
        ["ldarg.2"] = new() { Op = "ldarg.2", OpCode = InstructionOpCode.LdArg, IlOffset = 0 },
        ["ldarg.3"] = new() { Op = "ldarg.3", OpCode = InstructionOpCode.LdArg, IlOffset = 0 },
        ["ldloc.0"] = new() { Op = "ldloc.0", OpCode = InstructionOpCode.LdLoc, IlOffset = 0 },
        ["ldloc.1"] = new() { Op = "ldloc.1", OpCode = InstructionOpCode.LdLoc, IlOffset = 0 },
        ["ldloc.2"] = new() { Op = "ldloc.2", OpCode = InstructionOpCode.LdLoc, IlOffset = 0 },
        ["ldloc.3"] = new() { Op = "ldloc.3", OpCode = InstructionOpCode.LdLoc, IlOffset = 0 },
        ["stloc.0"] = new() { Op = "stloc.0", OpCode = InstructionOpCode.StLoc, IlOffset = 0 },
        ["stloc.1"] = new() { Op = "stloc.1", OpCode = InstructionOpCode.StLoc, IlOffset = 0 },
        ["stloc.2"] = new() { Op = "stloc.2", OpCode = InstructionOpCode.StLoc, IlOffset = 0 },
        ["stloc.3"] = new() { Op = "stloc.3", OpCode = InstructionOpCode.StLoc, IlOffset = 0 },
        ["ldc.i4.m1"] = new() { Op = "ldc.i4.m1", OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.0"] = new() { Op = "ldc.i4.0", OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.1"] = new() { Op = "ldc.i4.1", OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.2"] = new() { Op = "ldc.i4.2", OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.3"] = new() { Op = "ldc.i4.3", OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.4"] = new() { Op = "ldc.i4.4", OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.5"] = new() { Op = "ldc.i4.5", OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.6"] = new() { Op = "ldc.i4.6", OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.7"] = new() { Op = "ldc.i4.7", OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.8"] = new() { Op = "ldc.i4.8", OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
    };
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

        // Build marshalling descriptors for complex struct types used in P/Invoke.
        var complexStructTypeIds = new HashSet<string>(StringComparer.Ordinal);
        foreach (var method in methods)
        {
            if (!method.IsPInvoke || method.ComplexStructParameterTypeSubjectIds == null)
                continue;
            foreach (var typeId in method.ComplexStructParameterTypeSubjectIds)
                complexStructTypeIds.Add(typeId);
        }

        IReadOnlyList<StructMarshallingDescriptorArtifact>? descriptors = null;
        if (complexStructTypeIds.Count > 0)
        {
            var list = new List<StructMarshallingDescriptorArtifact>();
            foreach (var typeId in complexStructTypeIds.OrderBy(x => x, StringComparer.Ordinal))
            {
                var desc = BuildStructMarshallingDescriptor(typeId, managedTypes, managedFields);
                if (desc != null)
                    list.Add(desc);
            }
            if (list.Count > 0)
                descriptors = list;
        }

        return new AotCoreIrArtifact
        {
            Methods = methods,
            StructMarshallingDescriptors = descriptors,
        };
    }

    internal static AotCoreIrMethodArtifact? TryCreateMethod(
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

                // Fast path: simple stack-only instructions with no callee/constraints.
                // Skip ResolveDirectCallTarget, constrained override, target reference,
                // and ComImport detection — none of these apply to simple instructions.
                if (s_simpleInstructionCache.TryGetValue(typedInstruction.Op, out var cachedSimple))
                {
                    instructions.Add(cachedSimple with { IlOffset = ilOffset });
                    continue;
                }

                var directCallTarget = ResolveDirectCallTarget(typedInstruction, managedMethods, targetSymbols);

                // ── Constrained callvirt on value types ──
                // Rewrite the callee to the value type's own override method
                // so the emission path resolves it directly instead of dispatching
                // through the base type's vtable (Object::GetHashCode → Guid::GetHashCode).
                // Also correct the TargetReference SubjectId so the JSON artifact
                // carries the correct method identity for interpreter/hotpatch paths.
                ManagedInstructionReference? instructionReference = managedInstruction.Reference;
                var constrainedOverrideSubjectId = ResolveConstrainedValueTypeOverride(typedInstruction);
                if (constrainedOverrideSubjectId is not null)
                {
                    typedInstruction = typedInstruction with { Callee = constrainedOverrideSubjectId, DispatchKindCode = HybridDispatchKind.Direct };
                    directCallTarget = ResolveDirectCallTarget(typedInstruction, managedMethods, targetSymbols);

                    // Re-resolve TargetReference with corrected SubjectId so
                    // C++ aot_core_ir_reader uses the override method identity.
                    if (managedInstruction.Reference is not null &&
                        managedMethods.TryGetValue(constrainedOverrideSubjectId, out var constrainedOverrideMethod))
                    {
                        instructionReference = new ManagedInstructionReference
                        {
                            AssemblyName = constrainedOverrideMethod.AssemblyName,
                            SubjectKind = managedInstruction.Reference.SubjectKind,
                            SubjectId = constrainedOverrideSubjectId,
                        };
                    }
                }
                else if (typedInstruction.ConstrainedTypeSubjectId is not null)
                {
                    // Clear the constrained type so the C++ emitter does not try
                    // to resolve a generic parameter (!0, !!0) as a concrete type.
                    typedInstruction = typedInstruction with { ConstrainedTypeSubjectId = null };
                }

                var targetReference = ResolveTargetReference(
                    instructionReference,
                    typedInstruction,
                    managedTypes,
                    managedFields,
                    managedMethods,
                    genericDemandLookup);

                // Detect ComImport interface methods → COM vtable dispatch
                int? comVtableSlot = null;
                var dispatchKind = typedInstruction.DispatchKindCode;
                if (targetReference?.IsComImport == true &&
                    (string.Equals(typedInstruction.Op, "callvirt", StringComparison.Ordinal) ||
                     string.Equals(typedInstruction.Op, "call", StringComparison.Ordinal)))
                {
                    dispatchKind = HybridDispatchKind.ComVtable;
                    comVtableSlot = ComputeComVtableSlot(
                        typedInstruction, managedMethods, targetReference);
                }

                instructions.Add(new AotCoreIrInstructionArtifact
                {
                    Op = typedInstruction.Op,
                    OpCode = MapOpStringToEnum(typedInstruction.Op),
                    Operand = typedInstruction.Operand,
                    IlOffset = ilOffset,
                    ResultType = typedInstruction.ResultType,
                    Callee = typedInstruction.Callee,
                    CallSiteSignature = typedInstruction.CallSiteSignature,
                    Reference = instructionReference,
                    TargetReference = targetReference,
                    RuntimeServiceKind = ResolveRuntimeServiceKind(typedInstruction),
                    TargetSymbol = directCallTarget.TargetSymbol,
                    TargetParameterCount = directCallTarget.TargetParameterCount,
                    TargetReturnType = directCallTarget.TargetReturnType,
                    DispatchKindCode = dispatchKind,
                    ComVtableSlot = comVtableSlot,
                    IsPreserveSig = method.IsPreserveSig,
                    ConstrainedTypeSubjectId = typedInstruction.ConstrainedTypeSubjectId,
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

        // Complex non-blittable struct parameters (descriptor-driven)
        IReadOnlyList<int>? complexParamIndices = null;
        IReadOnlyList<string>? complexParamTypeIds = null;
        if (method.Import is not null)
        {
            complexParamIndices = DetectComplexStructParameters(
                method.Parameters, managedTypes, managedFields);

            if (complexParamIndices != null)
            {
                var typeIds = new List<string>(complexParamIndices.Count);
                foreach (int idx in complexParamIndices)
                {
                    var paramType = method.Parameters[idx].Type;
                    if (managedTypes.TryGetValue(paramType, out var paramManagedType))
                        typeIds.Add(paramManagedType.SubjectId);
                    else
                        typeIds.Add(paramType);
                }
                complexParamTypeIds = typeIds;
            }
        }

        var artifact = new AotCoreIrMethodArtifact
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
            IsPInvoke = method.Import is not null,
            IsUnmanagedCallersOnly = method.IsUnmanagedCallersOnly,
            ImportModuleName = method.Import?.ModuleName,
            ImportEntryPointName = method.Import?.EntryPointName,
            ImportCallingConvention = method.Import?.CallingConvention ?? 0,
            ImportCharSet = method.Import?.CharSet ?? 0,
            ImportSetLastError = method.Import?.SetLastError ?? false,
            IsInternalLink = method.Import is not null && string.IsNullOrEmpty(method.Import.ModuleName),
            IsSuppressGCTransition = method.Import?.IsSuppressGCTransition ?? false,
            StringParameterIndices = method.Import is not null
                ? method.Parameters
                    .Select((p, i) => (p.Type, i))
                    .Where(x => IsPInvokeStringType(x.Type))
                    .Select(x => x.i)
                    .ToArray()
                : null,
            SafeHandleParameterIndices = method.Import is not null
                ? method.Parameters
                    .Select((p, i) => (p.Type, i))
                    .Where(x => IsSafeHandleDerivedType(x.Type, managedTypes))
                    .Select(x => x.i)
                    .ToArray()
                : null,
            BlittableStructParameterIndices = method.Import is not null
                ? DetectBlittableStructParameters(method.Parameters, managedTypes, managedFields)
                : null,
            HasBlittableStructReturn = method.Import is not null
                && IsBlittableStructType(method.ReturnType, managedTypes, managedFields),
            ComplexStructParameterIndices = complexParamIndices,
            ComplexStructParameterTypeSubjectIds = complexParamTypeIds,
            DeclaringAssemblyName = method.AssemblyName,
        };

        // Simple non-blittable struct detection needs separate computation
        // because of the out parameter.
        if (method.Import is not null)
        {
            var p2Indices = DetectSimpleNonBlittableStructParameters(
                method.Parameters, managedTypes, managedFields, out var p2StringFields);
            artifact = artifact with
            {
                SimpleNonBlittableStructParameterIndices = p2Indices,
                SimpleNonBlittableStructStringFieldSubjectIds = p2StringFields,
            };
        }

        return artifact;
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
        return ResolveTargetReference(
            managedInstruction.Reference, typedInstruction, managedTypes,
            managedFields, managedMethods, genericDemandLookup);
    }

    private static AotCoreIrReferenceArtifact? ResolveTargetReference(
        ManagedInstructionReference? reference,
        TypedIlInstructionArtifact typedInstruction,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields,
        IReadOnlyDictionary<string, ManagedMethodModel> managedMethods,
        IReadOnlyDictionary<string, GenericInstantiationDemandModel> genericDemandLookup)
    {
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
                // Many IL opcodes (ldarg.*, ldloc.*, stloc.*, ldind.*, stind.*,
                // conv.*, arithmetic, branch, etc.) don't involve type/field/method
                // references. Silently return null instead of warning for these.
                if (IsSimpleOpcode(typedInstruction.Op))
                    return null;

                System.Console.Error.WriteLine(
                    $"[warning] AotCoreIrLowering: unknown opcode '{typedInstruction.Op}' in ResolveTargetReference, returning null reference.");
                return null;
        }
    }

    /// <summary>
    /// Returns true for IL opcodes that don't involve type/field/method references
    /// and thus don't need a TargetReference. These are "simple" instructions
    /// (load/store, arithmetic, conversion, branch, etc.).
    /// </summary>
    private static bool IsSimpleOpcode(string op)
    {
        // ldarg.0, ldarg.1, ldarg.s, etc.
        if (op.StartsWith("ldarg", StringComparison.Ordinal)) return true;
        // ldloc.0, ldloc.s, ldloca.s, etc.
        if (op.StartsWith("ldloc", StringComparison.Ordinal)) return true;
        // stloc.0, stloc.s, etc.
        if (op.StartsWith("stloc", StringComparison.Ordinal)) return true;
        // ldind.i, ldind.i4, stind.i, stind.ref, etc.
        if (op.StartsWith("ldind", StringComparison.Ordinal)) return true;
        if (op.StartsWith("stind", StringComparison.Ordinal)) return true;
        // conv.i4, conv.ovf.i4.un, conv.r8, etc.
        if (op.StartsWith("conv.", StringComparison.Ordinal)) return true;
        if (string.Equals(op, "conv", StringComparison.Ordinal)) return true;
        // ldc.i4, ldc.i8, ldc.r4, ldc.r8
        if (op.StartsWith("ldc.", StringComparison.Ordinal)) return true;
        // br, br.s, brfalse, brtrue, beq, bge, bgt, ble, blt, bne, etc.
        if (op.StartsWith("br", StringComparison.Ordinal)) return true;
        if (op.StartsWith("beq", StringComparison.Ordinal)) return true;
        if (op.StartsWith("bge", StringComparison.Ordinal)) return true;
        if (op.StartsWith("bgt", StringComparison.Ordinal)) return true;
        if (op.StartsWith("ble", StringComparison.Ordinal)) return true;
        if (op.StartsWith("blt", StringComparison.Ordinal)) return true;
        if (op.StartsWith("bne", StringComparison.Ordinal)) return true;
        // Arithmetic
        switch (op)
        {
            case "add": case "sub": case "mul": case "div": case "rem":
            case "and": case "or": case "xor": case "not":
            case "neg": case "shl": case "shr":
            case "ceq": case "cgt": case "clt":
            case "add.ovf": case "sub.ovf": case "mul.ovf":
            case "ldnull": case "dup": case "pop": case "ret":
            case "nop": case "throw": case "rethrow":
            case "ldlen": case "ldc.i4": case "ldc.i8":
            case "localloc":
                return true;
        }
        return false;
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

    private static InstructionOpCode? MapOpStringToEnum(string op)
    {
        return op switch
        {
            "add" => InstructionOpCode.Add,
            "add.ovf" => InstructionOpCode.AddOvf,
            "add.ovf.un" => InstructionOpCode.AddOvf,
            "and" => InstructionOpCode.And,
            "beq" => InstructionOpCode.Beq,
            "bge" => InstructionOpCode.Bge,
            "bge.un" => InstructionOpCode.BgeUn,
            "bgt" => InstructionOpCode.Bgt,
            "bgt.un" => InstructionOpCode.BgtUn,
            "ble" => InstructionOpCode.Ble,
            "ble.un" => InstructionOpCode.BleUn,
            "blt" => InstructionOpCode.Blt,
            "blt.un" => InstructionOpCode.BltUn,
            "bne.un" => InstructionOpCode.BneUn,
            "box" => InstructionOpCode.Box,
            "br" => InstructionOpCode.Br,
            "break" => InstructionOpCode.Break,
            "brfalse" => InstructionOpCode.BrFalse,
            "brtrue" => InstructionOpCode.BrTrue,
            "call" => InstructionOpCode.Call,
            "calli" => InstructionOpCode.Calli,
            "callvirt" => InstructionOpCode.CallVirt,
            "castclass" => InstructionOpCode.CastClass,
            "ceq" => InstructionOpCode.Ceq,
            "cgt" => InstructionOpCode.Cgt,
            "cgt.un" => InstructionOpCode.Cgt,
            "clt" => InstructionOpCode.Clt,
            "clt.un" => InstructionOpCode.Clt,
            "conv.i" => InstructionOpCode.ConvI,
            "conv.i1" => InstructionOpCode.Conv_I4,
            "conv.i2" => InstructionOpCode.Conv_I4,
            "conv.i4" => InstructionOpCode.Conv_I4,
            "conv.i8" => InstructionOpCode.Conv_I8,
            "conv.ovf.i" => InstructionOpCode.ConvOvfI,
            "conv.ovf.i.un" => InstructionOpCode.ConvOvfI,
            "conv.ovf.i1" => InstructionOpCode.ConvOvfI4,
            "conv.ovf.i2" => InstructionOpCode.ConvOvfI4,
            "conv.ovf.i4" => InstructionOpCode.ConvOvfI4,
            "conv.ovf.i4.un" => InstructionOpCode.ConvOvfI4,
            "conv.ovf.i8" => InstructionOpCode.ConvOvfI8,
            "conv.ovf.u" => InstructionOpCode.ConvOvfU,
            "conv.ovf.u.un" => InstructionOpCode.ConvOvfU,
            "conv.ovf.u1" => InstructionOpCode.ConvOvfU4,
            "conv.ovf.u1.un" => InstructionOpCode.ConvOvfU4,
            "conv.ovf.u2" => InstructionOpCode.ConvOvfU4,
            "conv.ovf.u2.un" => InstructionOpCode.ConvOvfU4,
            "conv.ovf.u4" => InstructionOpCode.ConvOvfU4,
            "conv.ovf.u8" => InstructionOpCode.ConvOvfU8,
            "conv.r.un" => InstructionOpCode.ConvRUn,
            "conv.r4" => InstructionOpCode.Conv_R4,
            "conv.r8" => InstructionOpCode.Conv_R8,
            "conv.u" => InstructionOpCode.ConvU,
            "conv.u1" => InstructionOpCode.Conv_I4,
            "conv.u2" => InstructionOpCode.Conv_I4,
            "conv.u4" => InstructionOpCode.Conv_I4,
            "conv.u8" => InstructionOpCode.Conv_I8,
            "cpblk" => InstructionOpCode.Cpblk,
            "div" => InstructionOpCode.Div,
            "div.un" => InstructionOpCode.DivUn,
            "dup" => InstructionOpCode.Dup,
            "endfilter" => InstructionOpCode.EndFilter,
            "endfinally" => InstructionOpCode.EndFinally,
            "initblk" => InstructionOpCode.InitBlk,
            "initobj" => InstructionOpCode.InitObj,
            "isinst" => InstructionOpCode.IsInst,
            "ldarg" => InstructionOpCode.LdArg,
            "ldarga" => InstructionOpCode.LdArgA,
            "ldarg.0" => InstructionOpCode.LdArg,
            "ldarg.1" => InstructionOpCode.LdArg,
            "ldarg.2" => InstructionOpCode.LdArg,
            "ldarg.3" => InstructionOpCode.LdArg,
            "ldarg.s" => InstructionOpCode.LdArg,
            "ldc.i4" => InstructionOpCode.LdcI4,
            "ldc.i4.s" => InstructionOpCode.LdcI4,
            "ldc.i4.0" => InstructionOpCode.LdcI4,
            "ldc.i4.1" => InstructionOpCode.LdcI4,
            "ldc.i4.2" => InstructionOpCode.LdcI4,
            "ldc.i4.3" => InstructionOpCode.LdcI4,
            "ldc.i4.4" => InstructionOpCode.LdcI4,
            "ldc.i4.5" => InstructionOpCode.LdcI4,
            "ldc.i4.6" => InstructionOpCode.LdcI4,
            "ldc.i4.7" => InstructionOpCode.LdcI4,
            "ldc.i4.8" => InstructionOpCode.LdcI4,
            "ldc.i8" => InstructionOpCode.LdcI8,
            "ldc.r4" => InstructionOpCode.LdcR4,
            "ldc.r8" => InstructionOpCode.LdcR8,
            "ldelem" => InstructionOpCode.LdElem,
            "ldelem.ref" => InstructionOpCode.LdElem,
            "ldelema" => InstructionOpCode.LdElemA,
            "ldfld" => InstructionOpCode.LdFld,
            "ldflda" => InstructionOpCode.LdFld,
            "ldftn" => InstructionOpCode.LdFtn,
            "ldind" => InstructionOpCode.LdInd,
            "ldind.i" => InstructionOpCode.LdInd,
            "ldind.i1" => InstructionOpCode.LdInd,
            "ldind.i2" => InstructionOpCode.LdInd,
            "ldind.i4" => InstructionOpCode.LdInd,
            "ldind.i8" => InstructionOpCode.LdInd,
            "ldind.u1" => InstructionOpCode.LdInd,
            "ldind.u2" => InstructionOpCode.LdInd,
            "ldind.u4" => InstructionOpCode.LdInd,
            "ldind.r4" => InstructionOpCode.LdInd,
            "ldind.r8" => InstructionOpCode.LdInd,
            "ldind.ref" => InstructionOpCode.LdInd,
            "ldlen" => InstructionOpCode.LdLen,
            "ldloc" => InstructionOpCode.LdLoc,
            "ldloc.s" => InstructionOpCode.LdLoc,
            "ldloca" => InstructionOpCode.LdLocA,
            "ldnull" => InstructionOpCode.LdNull,
            "ldobj" => InstructionOpCode.LdObj,
            "ldsfld" => InstructionOpCode.LdSFld,
            "ldsflda" => InstructionOpCode.LdSFld,
            "ldstr" => InstructionOpCode.LdStr,
            "ldtoken" => InstructionOpCode.LdToken,
            "ldvirtftn" => InstructionOpCode.LdVirtFtn,
            "leave" => InstructionOpCode.Leave,
            "localloc" => InstructionOpCode.LocAlloc,
            "mul" => InstructionOpCode.Mul,
            "mul.ovf" => InstructionOpCode.MulOvf,
            "mul.ovf.un" => InstructionOpCode.MulOvf,
            "neg" => InstructionOpCode.Neg,
            "newarr" => InstructionOpCode.NewArr,
            "newobj" => InstructionOpCode.NewObj,
            "nop" => null,
            "not" => InstructionOpCode.Not,
            "or" => InstructionOpCode.Or,
            "pop" => InstructionOpCode.Pop,
            "rem" => InstructionOpCode.Rem,
            "rem.un" => InstructionOpCode.RemUn,
            "ret" => InstructionOpCode.Ret,
            "rethrow" => InstructionOpCode.Rethrow,
            "shl" => InstructionOpCode.Shl,
            "shr" => InstructionOpCode.Shr,
            "shr.un" => InstructionOpCode.ShrUn,
            "sizeof" => InstructionOpCode.SizeOf,
            "starg" => InstructionOpCode.StArg,
            "stelem" => InstructionOpCode.StElem,
            "stelem.ref" => InstructionOpCode.StElem,
            "stfld" => InstructionOpCode.StFld,
            "stind" => InstructionOpCode.StInd,
            "stind.i" => InstructionOpCode.StInd,
            "stind.i1" => InstructionOpCode.StInd,
            "stind.i2" => InstructionOpCode.StInd,
            "stind.i4" => InstructionOpCode.StInd,
            "stind.i8" => InstructionOpCode.StInd,
            "stind.r4" => InstructionOpCode.StInd,
            "stind.r8" => InstructionOpCode.StInd,
            "stind.ref" => InstructionOpCode.StInd,
            "stloc" => InstructionOpCode.StLoc,
            "stloc.s" => InstructionOpCode.StLoc,
            "stobj" => InstructionOpCode.StObj,
            "stsfld" => InstructionOpCode.StSFld,
            "sub" => InstructionOpCode.Sub,
            "sub.ovf" => InstructionOpCode.SubOvf,
            "switch" => InstructionOpCode.Switch,
            "throw" => InstructionOpCode.Throw,
            "unbox" => InstructionOpCode.Unbox,
            "unbox.any" => InstructionOpCode.Unbox,
            "xor" => InstructionOpCode.Xor,
            _ => MapUnknownOp(op),
        };
    }

    private static InstructionOpCode? MapUnknownOp(string op)
    {
        // Many IL opcodes (short forms like brtrue.s, ldc.i4.0, etc.) don't have
        // explicit entries in the switch. Silently return null if the opcode is
        // a "simple" opcode that doesn't need special handling.
        if (IsSimpleOpcode(op))
            return null;
        System.Console.Error.WriteLine($"[warning] AotCoreIrLowering: unknown opcode '{op}' during lowering.");
        return null;
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
            return targetSymbol;
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

    private static bool IsPInvokeStringType(string typeIdentity)
    {
        return string.Equals(typeIdentity, "System.String", StringComparison.Ordinal);
    }

    /// <summary>
    /// Checks if a type derives from <see cref="System.Runtime.InteropServices.SafeHandle"/>
    /// by walking the <see cref="ManagedTypeModel.BaseTypeSubjectId"/> chain.
    /// </summary>
    private static bool IsSafeHandleDerivedType(string typeIdentity, IReadOnlyDictionary<string, ManagedTypeModel> managedTypes)
    {
        const string safeHandleSubjectId = "System.Runtime.InteropServices.SafeHandle";
        if (!managedTypes.TryGetValue(typeIdentity, out var typeModel))
            return false;

        var current = typeModel;
        while (current != null)
        {
            if (string.Equals(current.SubjectId, safeHandleSubjectId, StringComparison.Ordinal))
                return true;

            if (current.BaseTypeSubjectId == null)
                break;

            if (!managedTypes.TryGetValue(current.BaseTypeSubjectId, out current))
                break;
        }

        return false;
    }

    /// <summary>
    /// Checks if a type identity refers to a blittable primitive that has the same
    /// managed and native representation (no conversion needed).
    /// </summary>
    private static bool IsBlittablePrimitiveType(string typeIdentity)
    {
        return typeIdentity switch
        {
            "System.Boolean" => true,
            "System.Byte" => true,
            "System.SByte" => true,
            "System.Int16" => true,
            "System.UInt16" => true,
            "System.Int32" => true,
            "System.UInt32" => true,
            "System.Int64" => true,
            "System.UInt64" => true,
            "System.IntPtr" => true,
            "System.UIntPtr" => true,
            "System.Single" => true,
            "System.Double" => true,
            "System.Char" => true,
            _ => false,
        };
    }

    private enum StructFieldClassification { NonValueType, Empty, Blittable, HasStringFields, Complex }

    /// <summary>
    /// Classifies the instance fields of a value type for P/Invoke marshalling decisions.
    /// When <paramref name="stringFieldSubjectIds"/> is provided, populates it with the
    /// SubjectId of each string field found.
    /// </summary>
    private static StructFieldClassification ClassifyValueTypeFields(
        string typeIdentity,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields,
        List<string>? stringFieldSubjectIds = null)
    {
        if (!managedTypes.TryGetValue(typeIdentity, out var typeModel))
            return StructFieldClassification.NonValueType;

        if (!typeModel.IsValueType)
            return StructFieldClassification.NonValueType;

        var fields = managedFields.Values
            .Where(f => string.Equals(f.DeclaringTypeSubjectId, typeIdentity, StringComparison.Ordinal)
                        && !f.IsStatic)
            .ToList();

        if (fields.Count == 0)
            return StructFieldClassification.Empty;

        bool hasString = false;
        foreach (var field in fields)
        {
            if (IsBlittablePrimitiveType(field.FieldType))
                continue;

            if (field.FieldType == "System.String")
            {
                hasString = true;
                stringFieldSubjectIds?.Add(field.SubjectId);
                continue;
            }

            // Nested value type — recurse
            var nestedClass = ClassifyValueTypeFields(field.FieldType, managedTypes, managedFields, stringFieldSubjectIds);
            if (nestedClass == StructFieldClassification.Blittable)
                continue;
            if (nestedClass == StructFieldClassification.HasStringFields)
            {
                hasString = true;
                continue;
            }

            return StructFieldClassification.Complex;
        }

        if (hasString)
            return StructFieldClassification.HasStringFields;

        return StructFieldClassification.Blittable;
    }

    /// <summary>
    /// Returns true when the given type identity names a value type whose fields
    /// are all blittable primitives (or nested blittable value types).
    /// </summary>
    private static bool IsBlittableStructType(
        string typeIdentity,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields)
    {
        if (string.IsNullOrEmpty(typeIdentity)) return false;
        return ClassifyValueTypeFields(typeIdentity, managedTypes, managedFields) == StructFieldClassification.Blittable;
    }

    /// <summary>
    /// For a P/Invoke method, detect which parameter indices are blittable value types.
    /// </summary>
    private static IReadOnlyList<int>? DetectBlittableStructParameters(
        IReadOnlyList<ManagedParameterModel> parameters,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields)
    {
        var indices = new List<int>();
        for (int i = 0; i < parameters.Count; i++)
        {
            if (ClassifyValueTypeFields(parameters[i].Type, managedTypes, managedFields) == StructFieldClassification.Blittable)
            {
                indices.Add(i);
            }
        }
        return indices.Count > 0 ? indices : null;
    }

    /// <summary>
    /// For a P/Invoke method, detect which parameter indices are value types containing
    /// string fields (simple non-blittable structs). Collects the SubjectIds of string
    /// fields into <paramref name="stringFieldSubjectIds"/> grouped per parameter.
    /// </summary>
    private static IReadOnlyList<int>? DetectSimpleNonBlittableStructParameters(
        IReadOnlyList<ManagedParameterModel> parameters,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields,
        out IReadOnlyList<IReadOnlyList<string>>? stringFieldSubjectIds)
    {
        var indices = new List<int>();
        var allStringFields = new List<IReadOnlyList<string>>();
        for (int i = 0; i < parameters.Count; i++)
        {
            var fieldSubjectIds = new List<string>();
            var classification = ClassifyValueTypeFields(parameters[i].Type, managedTypes, managedFields, fieldSubjectIds);
            if (classification == StructFieldClassification.HasStringFields)
            {
                indices.Add(i);
                allStringFields.Add(fieldSubjectIds);
            }
        }
        if (indices.Count > 0)
        {
            stringFieldSubjectIds = allStringFields;
            return indices;
        }
        stringFieldSubjectIds = null;
        return null;
    }

    /// <summary>
    /// For a P/Invoke method, detect which parameter indices are complex non-blittable
    /// value types (structs containing fields beyond blittable primitives and simple strings).
    /// These require descriptor-driven marshalling via StructMarshallingDescriptorV1.
    /// </summary>
    private static IReadOnlyList<int>? DetectComplexStructParameters(
        IReadOnlyList<ManagedParameterModel> parameters,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields)
    {
        var indices = new List<int>();
        for (int i = 0; i < parameters.Count; i++)
        {
            var classification = ClassifyValueTypeFields(parameters[i].Type, managedTypes, managedFields);
            if (classification == StructFieldClassification.Complex)
            {
                indices.Add(i);
            }
        }
        return indices.Count > 0 ? indices : null;
    }

    /// <summary>
    /// Determine the native-size equivalent for a managed primitive type.
    /// Returns the native byte size, or null if the type is not a recognized primitive.
    /// </summary>
    private static int? GetNativePrimitiveSize(string fieldType)
    {
        return fieldType switch
        {
            "System.Boolean" => 4,   // Win32 BOOL
            "System.Byte" => 1,
            "System.SByte" => 1,
            "System.Char" => 2,      // UNICODE char16_t
            "System.Int16" => 2,
            "System.UInt16" => 2,
            "System.Int32" => 4,
            "System.UInt32" => 4,
            "System.Int64" => 8,
            "System.UInt64" => 8,
            "System.Single" => 4,
            "System.Double" => 8,
            "System.IntPtr" => 8,
            "System.UIntPtr" => 8,
            _ => null,
        };
    }

    /// <summary>
    /// Build a marshalling descriptor tree for a value type, classifying each field
    /// and computing its native offset/size. Uses the AOT model layout where managed
    /// fields are sequential IntPtr-sized slots; native offsets match for V1.
    /// </summary>
    private static StructMarshallingDescriptorArtifact? BuildStructMarshallingDescriptor(
        string typeIdentity,
        IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
        IReadOnlyDictionary<string, ManagedFieldModel> managedFields)
    {
        if (!managedTypes.TryGetValue(typeIdentity, out var typeModel) || !typeModel.IsValueType)
            return null;

        var fields = managedFields.Values
            .Where(f => string.Equals(f.DeclaringTypeSubjectId, typeIdentity, StringComparison.Ordinal)
                        && !f.IsStatic)
            .OrderBy(f => f.Name, StringComparer.Ordinal)
            .ToList();

        if (fields.Count == 0)
            return null;

        const int ptrSize = 8; // sizeof(CHAOS_IL2CPP_INTPTR)
        var fieldDescriptors = new List<StructFieldDescriptorArtifact>();
        int currentOffset = 0;

        foreach (var field in fields)
        {
            var (kind, size, arrayCount, elementType, nestedTypeId) = ClassifyFieldForMarshalling(
                field, managedTypes, managedFields);

            fieldDescriptors.Add(new StructFieldDescriptorArtifact
            {
                Kind = kind,
                Offset = currentOffset,
                Size = size,
                ArrayCount = arrayCount,
                ElementType = elementType,
                NestedTypeSubjectId = nestedTypeId,
                Name = field.Name,
            });

            currentOffset += ptrSize; // managed offset stride
        }

        return new StructMarshallingDescriptorArtifact
        {
            TypeSubjectId = typeIdentity,
            TotalSize = currentOffset,
            Fields = fieldDescriptors,
        };
    }

    /// <summary>
    /// Classify a single field for marshalling and determine its native parameters.
    /// Returns (kind, size, arrayCount, elementType, nestedTypeSubjectId).
    /// </summary>
    private static (string Kind, int Size, int ArrayCount, string? ElementType, string? NestedTypeSubjectId)
        ClassifyFieldForMarshalling(
            ManagedFieldModel field,
            IReadOnlyDictionary<string, ManagedTypeModel> managedTypes,
            IReadOnlyDictionary<string, ManagedFieldModel> managedFields)
    {
        var ft = field.FieldType;

        // String fields
        if (ft == "System.String")
            return ("StringField", 8, 0, null, null);

        // Boolean → Win32 BOOL
        if (ft == "System.Boolean")
            return ("BoolField", 4, 0, null, null);

        // Decimal (16 bytes, COM DECIMAL compatible layout)
        if (ft == "System.Decimal")
            return ("DecimalField", 16, 0, null, null);

        // DateTime (8 bytes, FILETIME compatible)
        if (ft == "System.DateTime")
            return ("DateTimeField", 8, 0, null, null);

        // Guid (16 bytes, blittable)
        if (ft == "System.Guid")
            return ("GuidField", 16, 0, null, null);

        // object → ObjectField
        if (ft == "System.Object")
            return ("ObjectField", 8, 0, null, null);

        // Blittable primitive
        var nativeSize = GetNativePrimitiveSize(ft);
        if (nativeSize.HasValue)
            return ("Blittable", nativeSize.Value, 0, null, null);

        // Nested value type — recurse
        if (managedTypes.TryGetValue(ft, out var nestedType) && nestedType.IsValueType)
        {
            // Check if the nested type is at least partially supported
            var nestedClass = ClassifyValueTypeFields(ft, managedTypes, managedFields);
            if (nestedClass != StructFieldClassification.NonValueType)
            {
                return ("NestedStruct", 8, 0, null, ft);
            }
        }

        // Default: treat as blittable pointer-sized (managed reference or opaque)
        return ("Blittable", 8, 0, null, null);
    }

    /// <summary>
    /// Compute the COM vtable slot index for a method call on a ComImport interface.
    /// Slot = 3 (IUnknown reserved) + method ordinal within the interface's method list.
    /// The method ordinal is determined by MetadataToken order within the declaring type.
    /// </summary>
    private static int? ComputeComVtableSlot(
        TypedIlInstructionArtifact typedInstruction,
        IReadOnlyDictionary<string, ManagedMethodModel> managedMethods,
        AotCoreIrReferenceArtifact targetReference)
    {
        var declaringTypeId = targetReference.DeclaringTypeSubjectId;
        if (string.IsNullOrEmpty(declaringTypeId))
            return null;

        var calleeSubjectId = typedInstruction.Callee;
        if (string.IsNullOrEmpty(calleeSubjectId))
            return null;

        // Filter all methods belonging to the declaring interface, ordered by metadata token.
        var interfaceMethods = managedMethods.Values
            .Where(m => string.Equals(m.DeclaringTypeSubjectId, declaringTypeId, StringComparison.Ordinal))
            .OrderBy(m => m.MetadataToken)
            .ToList();

        if (interfaceMethods.Count == 0)
            return null;

        // Find the index of the target method in the ordered list.
        for (var i = 0; i < interfaceMethods.Count; i++)
        {
            if (string.Equals(interfaceMethods[i].SubjectId, calleeSubjectId, StringComparison.Ordinal))
            {
                return 3 + i; // 3 IUnknown reserved slots + method ordinal
            }
        }

        return null;
    }

    /// <summary>
    /// When a callvirt is preceded by the constrained. IL prefix on a value type,
    /// returns the value type's own override SubjectId for the virtual method slot.
    /// Returns null when no override exists (the original Callee should be kept).
    /// </summary>
    private static string? ResolveConstrainedValueTypeOverride(
        TypedIlInstructionArtifact typedInstruction)
    {
        var constrainedTypeId = typedInstruction.ConstrainedTypeSubjectId;
        if (string.IsNullOrWhiteSpace(constrainedTypeId))
            return null;

        // Extract the signature suffix (everything after "::") from the slot method.
        var callee = typedInstruction.Callee;
        if (string.IsNullOrWhiteSpace(callee))
            return null;

        var sepIdx = callee.IndexOf("::", StringComparison.Ordinal);
        if (sepIdx < 0)
            return null;
        var slotSig = callee.Substring(sepIdx + 2);

        // Constrained. callvirt on a value type should dispatch to the value type's
        // own override of the virtual method (e.g. Guid::GetHashCode instead of
        // Object::GetHashCode). We construct the expected SubjectId directly —
        // no need to verify IsValueType since the C# compiler only emits constrained.
        // for value types; for reference types it emits plain callvirt.

        // Reject generic parameter markers (!0, !!0) from F# or other sources.
        // A generic parameter is not a concrete type and has no override to dispatch to.
        if (constrainedTypeId.Contains('!'))
            return null;

        return constrainedTypeId + "::" + slotSig;
    }
}
