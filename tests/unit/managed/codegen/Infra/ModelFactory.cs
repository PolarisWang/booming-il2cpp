using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator.Tests.Infra;

/// <summary>
/// Factory for constructing minimal artifact instances to feed into
/// <see cref="NativeAotLoweringPlanner.Create"/>.
///
/// Each factory method produces a valid artifact with the minimum required
/// fields populated. Optional fields are left at their default values.
/// Use the returned instances as-is or override specific properties with
/// the `with { ... }` record syntax.
/// </summary>
internal static class ModelFactory
{
    internal static readonly AotCoreIrAbiSlotArtifact VoidAbi = new()
    {
        CarrierKindCode = AotCoreIrAbiCarrierKind.Void,
    };

    internal static readonly AotCoreIrAbiSlotArtifact Int32Abi = new()
    {
        CarrierKindCode = AotCoreIrAbiCarrierKind.Int32,
        TypeSubjectId = "System.Int32",
    };

    internal static readonly AotCoreIrAbiSlotArtifact Int64Abi = new()
    {
        CarrierKindCode = AotCoreIrAbiCarrierKind.Int64,
        TypeSubjectId = "System.Int64",
    };

    internal static readonly AotCoreIrAbiSlotArtifact RefAbi = new()
    {
        CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt,
        TypeSubjectId = "System.Object",
    };

    internal static readonly AotCoreIrAbiSlotArtifact IntPtrAbi = new()
    {
        CarrierKindCode = AotCoreIrAbiCarrierKind.NativeInt,
        TypeSubjectId = "System.IntPtr",
    };

    /// <summary>
    /// Creates a default lowering plan for full-assembly mode.
    /// Full-assembly mode relaxes entry-method validation, so we can
    /// test with any set of methods without worrying about entry mismatch.
    /// </summary>
    internal static NativeAotLoweringPlanArtifact CreateDefaultPlan(string entrySubjectId = "StubAssembly/StubMethods::ReturnZero:System.Int32()")
    {
        return new NativeAotLoweringPlanArtifact
        {
            PlanKind = "full-assembly-entry",
            AssemblyName = "StubAssembly",
            EntrySubjectId = entrySubjectId,
            NativeEntryFunctionName = string.Empty,
            EntrySymbol = "Chaos_StubAssembly_StubMethods_ReturnZero",
            EntryMethodToken = "0x06000001",
            WorkloadAbi = "full-assembly",
        };
    }

    /// <summary>
    /// Creates a minimal closure manifest for testing.
    /// The EntrySubjectId must match a real method in the stub assembly
    /// (StubAssembly.dll) to satisfy the Loader's entry point validation.
    /// </summary>
    internal static ManagedClosureManifestArtifact CreateDefaultManifest(string assemblyName = "StubAssembly", string? inputAssemblyPath = null)
    {
        return new ManagedClosureManifestArtifact
        {
            AssemblyName = assemblyName,
            EntrySubjectId = "StubAssembly/StubMethods::ReturnZero:System.Int32()",
            InputAssemblyPath = inputAssemblyPath ?? $"{assemblyName}.dll",
            InputModuleVersionId = Guid.NewGuid().ToString("N"),
            Artifacts = Array.Empty<ManagedClosureArtifactRef>(),
            FullAssemblyClosure = true,
        };
    }

    /// <summary>
    /// Creates a minimal metadata registration (empty — no type/method registrations).
    /// </summary>
    internal static MetadataRegistrationArtifact CreateEmptyMetadataRegistration()
    {
        return new MetadataRegistrationArtifact
        {
            Registrations = Array.Empty<MetadataRegistrationEntry>(),
        };
    }

    /// <summary>
    /// Creates a minimal supplemental metadata template (empty).
    /// </summary>
    internal static SupplementalMetadataTemplateArtifact CreateEmptySupplementalMetadata()
    {
        return new SupplementalMetadataTemplateArtifact
        {
            RegisteredTypes = Array.Empty<SupplementalMetadataTypeTemplateEntry>(),
            RegisteredMethods = Array.Empty<SupplementalMetadataMethodTemplateEntry>(),
            ReservedSlots = new SupplementalMetadataReservedSlots(),
        };
    }

    /// <summary>
    /// Creates a method identity artifact for the given subject.
    /// </summary>
    internal static ManagedMethodIdentityArtifact CreateIdentity(string subjectId)
    {
        // Extract declaring type from subject: "Namespace.Type::Method" → "Namespace.Type"
        var colonIdx = subjectId.IndexOf("::", StringComparison.Ordinal);
        var declaringType = colonIdx >= 0 ? subjectId[..colonIdx] : subjectId;

        return new ManagedMethodIdentityArtifact
        {
            AssemblyName = "TestAssembly",
            DeclaringTypeSubjectId = declaringType,
            DefinitionSubjectId = subjectId,
            SubjectId = subjectId,
            MethodId = subjectId,
            Signature = subjectId,
        };
    }

    /// <summary>
    /// Creates a single AotCoreIrMethodArtifact with the given properties.
    /// Instructions default to a simple "ret" sequence if not provided.
    /// If the subjectId does not end with ')' (i.e. lacks the method
    /// signature suffix), "()" is appended automatically since the
    /// planner expects format "Type::Method:RetType(params)" or
    /// at minimum "Type::Method(params)".
    /// </summary>
    internal static AotCoreIrMethodArtifact CreateMethod(
        string subjectId,
        IReadOnlyList<AotCoreIrInstructionArtifact>? instructions = null,
        AotCoreIrAbiSlotArtifact? returnAbi = null,
        IReadOnlyList<AotCoreIrAbiSlotArtifact>? parameterAbis = null,
        bool isStatic = true,
        string nativeSymbol = "Chaos_Test_Stub",
        string returnType = "System.Void",
        IReadOnlyList<AotCoreIrExceptionRegionArtifact>? exceptionRegions = null)
    {
        // Ensure the subject ID has a method-signature suffix so the
        // planner's GetMethodName can parse it (expects ::Name:Ret(params)).
        if (!subjectId.EndsWith(')'))
            subjectId = $"{subjectId}()";

        instructions ??= new[]
        {
            new AotCoreIrInstructionArtifact
            {
                Op = "ret",
                IlOffset = 0,
            },
        };

        return new AotCoreIrMethodArtifact
        {
            MethodId = subjectId,
            SubjectId = subjectId,
            Signature = subjectId,
            Identity = CreateIdentity(subjectId),
            NativeSymbol = nativeSymbol,
            IsStatic = isStatic,
            ReturnType = returnType,
            ReturnAbi = returnAbi ?? VoidAbi,
            ParameterCount = parameterAbis?.Count ?? 0,
            ParameterAbis = parameterAbis ?? Array.Empty<AotCoreIrAbiSlotArtifact>(),
            LocalCount = 0,
            ExceptionRegionCount = exceptionRegions?.Count ?? 0,
            ExceptionRegions = exceptionRegions ?? Array.Empty<AotCoreIrExceptionRegionArtifact>(),
            Instructions = instructions,
        };
    }

    /// <summary>
    /// Creates an AotCoreIrArtifact containing the given methods.
    /// </summary>
    internal static AotCoreIrArtifact CreateArtifact(params AotCoreIrMethodArtifact[] methods)
    {
        return new AotCoreIrArtifact
        {
            Methods = methods,
        };
    }

    /// <summary>
    /// Creates a simple instruction artifact for a commonly used op.
    /// Use <paramref name="intOperand"/> for argument indices (ldarg),
    /// local slots (stloc/ldloc), branch targets (brfalse/br/leave),
    /// and integer literals (ldc.i4).
    /// </summary>
    internal static AotCoreIrInstructionArtifact Instruction(string op, int ilOffset = 0, int intOperand = 0, string? callee = null, string? resultType = null, string? targetSymbol = null)
    {
        return new AotCoreIrInstructionArtifact
        {
            Op = op,
            IlOffset = ilOffset,
            Operand = intOperand,
            Callee = callee,
            ResultType = resultType,
            TargetSymbol = targetSymbol,
        };
    }

    /// <summary>
    /// Creates an instruction with a string literal as the operand.
    /// Required for ldstr (string literal), ldc.r4 (float), ldc.r8 (double).
    /// </summary>
    internal static AotCoreIrInstructionArtifact Instruction(string op, int ilOffset, string stringOperand, string? resultType = null)
    {
        return new AotCoreIrInstructionArtifact
        {
            Op = op,
            IlOffset = ilOffset,
            Operand = stringOperand,
            ResultType = resultType,
        };
    }

    /// <summary>
    /// Creates an instruction with a long (int64) operand.
    /// Required for ldc.i8 (64-bit integer constant).
    /// </summary>
    internal static AotCoreIrInstructionArtifact Instruction(string op, int ilOffset, long longOperand, string? resultType = null)
    {
        return new AotCoreIrInstructionArtifact
        {
            Op = op,
            IlOffset = ilOffset,
            Operand = longOperand,
            ResultType = resultType,
        };
    }

    /// <summary>
    /// Creates an instruction with a double (float64) operand.
    /// Required for ldc.r8 (64-bit float constant), ldc.r4 (32-bit float).
    /// </summary>
    internal static AotCoreIrInstructionArtifact Instruction(string op, int ilOffset, double doubleOperand, string? resultType = null)
    {
        return new AotCoreIrInstructionArtifact
        {
            Op = op,
            IlOffset = ilOffset,
            Operand = doubleOperand,
            ResultType = resultType,
        };
    }

    /// <summary>
    /// Creates an instruction with an array of integer operands.
    /// Required for switch (target offset list).
    /// </summary>
    internal static AotCoreIrInstructionArtifact Instruction(string op, int ilOffset, int[] arrayOperand)
    {
        return new AotCoreIrInstructionArtifact
        {
            Op = op,
            IlOffset = ilOffset,
            Operand = arrayOperand,
        };
    }

    /// <summary>
    /// Creates an instruction with a TargetReference and RuntimeServiceKind.
    /// Required for field access opcodes (ldfld, stfld, ldsfld, stsfld, ldflda, ldsflda)
    /// where the TargetReference Kind must be Field and the RuntimeServiceKind
    /// distinguishes instance vs. static and load vs. store.
    /// </summary>
    internal static AotCoreIrInstructionArtifact FieldInstruction(
        string op,
        int ilOffset,
        AotCoreIrRuntimeServiceKind runtimeServiceKind,
        string declaringTypeSubjectId,
        string fieldName,
        string fieldTypeSubjectId,
        AotCoreIrTypeShapeKind declaringTypeShape = AotCoreIrTypeShapeKind.ReferenceType,
        string assemblyName = "TestAssembly")
    {
        return new AotCoreIrInstructionArtifact
        {
            Op = op,
            IlOffset = ilOffset,
            RuntimeServiceKind = runtimeServiceKind,
            TargetReference = new AotCoreIrReferenceArtifact
            {
                Kind = AotCoreIrReferenceKind.Field,
                AssemblyName = assemblyName,
                SubjectId = $"{declaringTypeSubjectId}::{fieldName}",
                DeclaringTypeSubjectId = declaringTypeSubjectId,
                DeclaringTypeShape = declaringTypeShape,
                FieldTypeSubjectId = fieldTypeSubjectId,
            },
        };
    }

    /// <summary>
    /// Creates an instruction with a TargetReference and no Callee.
    /// Used for opcodes that require type/method metadata: newobj, newarr, box, unbox,
    /// ldelem, stelem, isinst, ldtoken, ldobj, stobj, ldelema, etc.
    /// Callee is intentionally NOT set to avoid triggering custom attribute parsing.
    /// </summary>
    internal static AotCoreIrInstructionArtifact Instruction(string op, int ilOffset, string subjectId, AotCoreIrReferenceKind refKind, string assemblyName = "System.Private.CoreLib")
    {
        return new AotCoreIrInstructionArtifact
        {
            Op = op,
            IlOffset = ilOffset,
            TargetReference = new AotCoreIrReferenceArtifact
            {
                Kind = refKind,
                AssemblyName = assemblyName,
                SubjectId = subjectId,
            },
        };
    }

    /// <summary>
    /// Creates an instruction artifact with a resolved target reference.
    /// Used for external call testing where TargetSymbol and DirectNativeSymbol matter.
    /// </summary>
    internal static AotCoreIrInstructionArtifact CallInstruction(
        string callee,
        string targetSymbol,
        int ilOffset = 0,
        string? targetReturnType = null,
        int? targetParameterCount = null,
        HybridDispatchKind? dispatchKind = null)
    {
        return new AotCoreIrInstructionArtifact
        {
            Op = "call",
            IlOffset = ilOffset,
            Callee = callee,
            TargetSymbol = targetSymbol,
            TargetReturnType = targetReturnType ?? "System.Void",
            TargetParameterCount = targetParameterCount ?? 0,
            DispatchKindCode = dispatchKind,
        };
    }
}
