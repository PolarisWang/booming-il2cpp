using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class AotCoreIrLowering
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
        ["ret"] = new() { Op = "ret", IlOffset = 0 },
        ["ldarg.0"] = new() { Op = "ldarg", Operand = 0, OpCode = InstructionOpCode.LdArg, IlOffset = 0 },
        ["ldarg.1"] = new() { Op = "ldarg", Operand = 1, OpCode = InstructionOpCode.LdArg, IlOffset = 0 },
        ["ldarg.2"] = new() { Op = "ldarg", Operand = 2, OpCode = InstructionOpCode.LdArg, IlOffset = 0 },
        ["ldarg.3"] = new() { Op = "ldarg", Operand = 3, OpCode = InstructionOpCode.LdArg, IlOffset = 0 },
        ["ldloc.0"] = new() { Op = "ldloc", Operand = 0, OpCode = InstructionOpCode.LdLoc, IlOffset = 0 },
        ["ldloc.1"] = new() { Op = "ldloc", Operand = 1, OpCode = InstructionOpCode.LdLoc, IlOffset = 0 },
        ["ldloc.2"] = new() { Op = "ldloc", Operand = 2, OpCode = InstructionOpCode.LdLoc, IlOffset = 0 },
        ["ldloc.3"] = new() { Op = "ldloc", Operand = 3, OpCode = InstructionOpCode.LdLoc, IlOffset = 0 },
        ["stloc.0"] = new() { Op = "stloc", Operand = 0, OpCode = InstructionOpCode.StLoc, IlOffset = 0 },
        ["stloc.1"] = new() { Op = "stloc", Operand = 1, OpCode = InstructionOpCode.StLoc, IlOffset = 0 },
        ["stloc.2"] = new() { Op = "stloc", Operand = 2, OpCode = InstructionOpCode.StLoc, IlOffset = 0 },
        ["stloc.3"] = new() { Op = "stloc", Operand = 3, OpCode = InstructionOpCode.StLoc, IlOffset = 0 },
        ["ldc.i4.m1"] = new() { Op = "ldc.i4", Operand = -1, OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.0"] = new() { Op = "ldc.i4", Operand = 0, OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.1"] = new() { Op = "ldc.i4", Operand = 1, OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.2"] = new() { Op = "ldc.i4", Operand = 2, OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.3"] = new() { Op = "ldc.i4", Operand = 3, OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.4"] = new() { Op = "ldc.i4", Operand = 4, OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.5"] = new() { Op = "ldc.i4", Operand = 5, OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.6"] = new() { Op = "ldc.i4", Operand = 6, OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.7"] = new() { Op = "ldc.i4", Operand = 7, OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.8"] = new() { Op = "ldc.i4", Operand = 8, OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
        ["ldc.i4.9"] = new() { Op = "ldc.i4", Operand = 9, OpCode = InstructionOpCode.LdcI4, IlOffset = 0 },
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
        // Last-wins dedup: duplicate SubjectIds from cross-assembly loading (--assembly-dir)
        var managedTypes = new Dictionary<string, ManagedTypeModel>(StringComparer.Ordinal);
        foreach (var t in linkedWorld.Types) { if (!string.IsNullOrEmpty(t.SubjectId)) managedTypes[t.SubjectId] = t; }
        var managedFields = new Dictionary<string, ManagedFieldModel>(StringComparer.Ordinal);
        foreach (var f in linkedWorld.Fields) { if (!string.IsNullOrEmpty(f.SubjectId)) managedFields[f.SubjectId] = f; }
        var managedMethods = new Dictionary<string, ManagedMethodModel>(StringComparer.Ordinal);
        foreach (var m in linkedWorld.Methods) { if (!string.IsNullOrEmpty(m.SubjectId)) managedMethods[m.SubjectId] = m; }
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

                // Normalize .s branch opcodes to their base form (br.s → br).
                // The emitter switch only handles the base forms.
                var normalizedOp = typedInstruction.Op;
                if (normalizedOp.EndsWith(".s", StringComparison.Ordinal) &&
                    (normalizedOp.StartsWith("br", StringComparison.Ordinal) ||
                     normalizedOp.StartsWith("beq", StringComparison.Ordinal) ||
                     normalizedOp.StartsWith("bge", StringComparison.Ordinal) ||
                     normalizedOp.StartsWith("bgt", StringComparison.Ordinal) ||
                     normalizedOp.StartsWith("ble", StringComparison.Ordinal) ||
                     normalizedOp.StartsWith("blt", StringComparison.Ordinal) ||
                     normalizedOp.StartsWith("bne", StringComparison.Ordinal)))
                {
                    typedInstruction = typedInstruction with { Op = normalizedOp[..^2] };
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
        // refanytype (typed reference type extraction, no type reference needed)
        if (string.Equals(op, "refanytype", StringComparison.Ordinal)) return true;
        // Arithmetic
        switch (op)
        {
            case "add":
            case "sub":
            case "mul":
            case "div":
            case "rem":
            case "and":
            case "or":
            case "xor":
            case "not":
            case "neg":
            case "shl":
            case "shr":
            case "ceq":
            case "cgt":
            case "clt":
            case "add.ovf":
            case "sub.ovf":
            case "sub.ovf.un":
            case "mul.ovf":
            case "ldnull":
            case "dup":
            case "pop":
            case "ret":
            case "nop":
            case "throw":
            case "rethrow":
            case "ldlen":
            case "ldc.i4":
            case "ldc.i8":
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
            "sub.ovf.un" => InstructionOpCode.SubOvf,
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

    /// <summary>
    /// Estimate native size (bytes) of a value type for ABI decision purposes.
    /// Returns 0 for unknown types (caller falls back to pass-by-value).
    /// The x64 ABI threshold for pass-by-reference is > 16 bytes.
    /// </summary>
}
