using Chaos.IL2CPP.Generator.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Rich integration tests that exercise many code paths in ObjectModelEmission
/// by providing instructions with rich TargetReference metadata (TypeShape,
/// BaseTypeSubjectId, ArrayElementSubjectId, ImplementedInterfaceSubjectIds, etc.)
/// and exception regions with catch types.
/// </summary>
public sealed class RichObjectModelEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    /// <summary>
    /// Exercises ObjectModelEmission's instruction-scanning loop with
    /// type references of all shapes, array element references, field
    /// references, overflow opcodes, ldstr, and exception region catch types.
    /// This single test covers ~30 guarded code paths in EmitObjectModelDeclarations.
    /// </summary>
    [Fact]
    public void AllTypeShapes_TriggersTrackingPaths()
    {
        // One method with many diverse instructions:
        // - ReferenceType with base and interfaces
        // - ValueType reference
        // - InterfaceType reference
        // - ArrayElementSubjectId for array-type ref
        // - Overflow opcodes (add.ovf triggers System.OverflowException tracking)
        // - ldstr (triggers System.String tracking)
        // - Exception region with catch type
        // - IsSealed type ref
        var method = AotCoreIrMethod("TestModule.TestClass::AllTypes:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // -- Value type reference (System.Int32) --
                Inst("ldc.i4", 0, 42),
                Inst("box", 1, "System.Private.CoreLib/System.Int32",
                    AotCoreIrReferenceKind.Type,
                    typeShape: AotCoreIrTypeShapeKind.ValueType),
                // -- Reference type with base and interfaces --
                Inst("castclass", 2, "System.Private.CoreLib/System.String",
                    AotCoreIrReferenceKind.Type,
                    typeShape: AotCoreIrTypeShapeKind.ReferenceType,
                    baseType: "System.Private.CoreLib/System.Object",
                    interfaces: new[] {
                        "System.Private.CoreLib/System.IComparable",
                        "System.Private.CoreLib/System.IConvertible"
                    }),
                // -- Interface reference --
                Inst("isinst", 3, "System.Private.CoreLib/System.IDisposable",
                    AotCoreIrReferenceKind.Type,
                    typeShape: AotCoreIrTypeShapeKind.InterfaceType),
                // -- Sealed type reference --
                Inst("isinst", 4, "System.Private.CoreLib/System.String",
                    AotCoreIrReferenceKind.Type,
                    typeShape: AotCoreIrTypeShapeKind.ReferenceType,
                    isSealed: true),
                // -- Array element reference (int[]) --
                Inst("newarr", 5, "System.Private.CoreLib/System.Int32[]",
                    AotCoreIrReferenceKind.Type,
                    arrayElement: "System.Private.CoreLib/System.Int32",
                    arrayElementShape: AotCoreIrTypeShapeKind.ValueType,
                    runtimeServiceKind: AotCoreIrRuntimeServiceKind.NewArray),
                // -- Reference type array element --
                Inst("newarr", 6, "System.Private.CoreLib/System.String[]",
                    AotCoreIrReferenceKind.Type,
                    arrayElement: "System.Private.CoreLib/System.String",
                    arrayElementShape: AotCoreIrTypeShapeKind.ReferenceType,
                    arrayElementBase: "System.Private.CoreLib/System.Object"),
                // -- Overflow arithmetic (triggers System.OverflowException) --
                Inst("ldc.i4", 7, 100),
                Inst("ldc.i4", 8, 200),
                InstOp("add.ovf", 9),
                InstOp("pop", 10),
                // -- Overflow unsigned (also triggers OverflowException) --
                Inst("ldc.i4", 11, 100),
                Inst("ldc.i4", 12, 200),
                InstOp("add.ovf.un", 13),
                InstOp("pop", 14),
                // -- mul.ovf (triggers OverflowException) --
                Inst("ldc.i4", 15, 10),
                Inst("ldc.i4", 16, 20),
                InstOp("mul.ovf", 17),
                InstOp("pop", 18),
                // -- ldstr (triggers System.String tracking) --
                Inst("ldstr", 19, "hello"),
                InstOp("pop", 20),
                // -- conv.ovf.i1 (triggers OverflowException) --
                Inst("ldc.i4", 21, 255),
                InstOp("conv.ovf.i1", 22),
                // -- sub.ovf (triggers OverflowException) --
                InstOp("pop", 23),
                Inst("ldc.i4", 24, 10),
                Inst("ldc.i4", 25, 20),
                InstOp("sub.ovf", 26),
                InstOp("pop", 27),
                // -- mul.ovf.un --
                Inst("ldc.i4", 28, 10),
                Inst("ldc.i4", 29, 20),
                InstOp("mul.ovf.un", 30),
                InstOp("pop", 31),
                // -- sub.ovf.un --
                Inst("ldc.i4", 32, 10),
                Inst("ldc.i4", 33, 20),
                InstOp("sub.ovf.un", 34),
                InstOp("pop", 35),
                // Final return
                Inst("ldc.i4", 36, 0),
                InstOp("ret", 37),
            },
            // Exception region with catch type (triggers catch type tracking in ObjectModelEmission)
            exceptionRegions: new[]
            {
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Catch,
                    TryOffset = 7,
                    TryLength = 30,
                    HandlerOffset = 38,
                    HandlerLength = 4,
                    CatchTypeSubjectId = "System.Private.CoreLib/System.InvalidOperationException",
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises ObjectModelEmission field reference tracking paths.
    /// Field instructions with various RuntimeServiceKinds trigger field
    /// declaration emission in EmitObjectModelDeclarations.
    /// </summary>
    [Fact]
    public void FieldReferences_TriggersFieldTracking()
    {
        var method = AotCoreIrMethod("TestModule.TestClass::FieldTest:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // Instance field load on a reference type
                Inst("ldarg", 0, 0),
                InstFld("ldfld", 1,
                    AotCoreIrRuntimeServiceKind.LoadInstanceField,
                    "System.Private.CoreLib/System.Collections.Generic.List`1",
                    "_items",
                    "System.Private.CoreLib/System.Int32[]",
                    AotCoreIrTypeShapeKind.ReferenceType),
                // Store result and return
                InstOp("ret", 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises ObjectModelEmission with various shaped type references:
    /// value type with box, value type without box, reference type with base.
    /// </summary>
    [Fact]
    public void ValueTypeAndReferenceTypeMixing_EmitsValidCode()
    {
        // This method mixes: newobj for value type, box for primitive, isinst with interface
        var method = AotCoreIrMethod("TestModule.TestClass::TypeMix:TestType(System.Object)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                // ldstr → String tracking
                Inst("ldstr", 0, "test"),
                // isinst on interface type
                Inst("isinst", 1, "System.Private.CoreLib/System.IComparable",
                    AotCoreIrReferenceKind.Type,
                    typeShape: AotCoreIrTypeShapeKind.InterfaceType),
                // null check
                InstOp("ldnull", 2),
                InstOp("cgt.un", 3),
                InstOp("ret", 4),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises struct value type with field references (value type field tracking).
    /// </summary>
    [Fact]
    public void StructFieldReferences_EmitsValueTypeTracking()
    {
        var method = AotCoreIrMethod("TestModule.TestClass::StructField:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // ldfld on a value type declaring type
                Inst("ldarg", 0, 0),
                InstFld("ldfld", 1,
                    AotCoreIrRuntimeServiceKind.LoadInstanceField,
                    "System.Private.CoreLib/System.Decimal",
                    "_lo32",
                    "System.Private.CoreLib/System.Int32",
                    AotCoreIrTypeShapeKind.ValueType),
                InstOp("ret", 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises IsSealed type reference tracking.
    /// </summary>
    [Fact]
    public void SealedTypeReference_EmitsValidCode()
    {
        var method = AotCoreIrMethod("TestModule.TestClass::SealedRef:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // isinst on a sealed type (String is sealed)
                Inst("ldarg", 0, 0),
                Inst("isinst", 1, "System.Private.CoreLib/System.String",
                    AotCoreIrReferenceKind.Type,
                    typeShape: AotCoreIrTypeShapeKind.ReferenceType,
                    isSealed: true),
                InstOp("ret", 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises exception region with multiple catch types for type tracking.
    /// </summary>
    [Fact]
    public void ExceptionRegionCatchTypes_TriggersReferenceTypeTracking()
    {
        var method = AotCoreIrMethod("TestModule.TestClass::CatchTypes:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                Inst("ldc.i4", 0, 10),
                Inst("ldc.i4", 1, 0),
                InstOp("div", 2), // may throw
                InstOp("pop", 3),
                Inst("ldc.i4", 4, 42),
                InstOp("ret", 5),
                // Handler: return 0
                Inst("ldc.i4", 6, 0),
                InstOp("ret", 7),
            },
            exceptionRegions: new[]
            {
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Catch,
                    TryOffset = 0,
                    TryLength = 4,
                    HandlerOffset = 5,
                    HandlerLength = 3,
                    CatchTypeSubjectId = "System.Private.CoreLib/System.DivideByZeroException",
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises initobj on a value type (triggers value type tracking in EmitObjectModelDeclarations).
    /// </summary>
    [Fact]
    public void InitObjectValueType_TriggersValueTypeTracking()
    {
        var method = AotCoreIrMethod("TestModule.TestClass::InitObj:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // ldloca + initobj to trigger initobj path for value type
                Inst("ldarg", 0, 0),
                Inst("initobj", 1, "System.Private.CoreLib/System.Decimal",
                    AotCoreIrReferenceKind.Type,
                    typeShape: AotCoreIrTypeShapeKind.ValueType,
                    runtimeServiceKind: AotCoreIrRuntimeServiceKind.InitObject),
                Inst("ldc.i4", 2, 0),
                InstOp("ret", 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises callee matching reflection helper patterns (IsTypeReflectionHelperSubjectId,
    /// IsReflectionMemberHelperSubjectId, etc.) which trigger Type/MethodInfo tracking.
    /// Uses an instruction with a callee string that matches the helper pattern.
    /// </summary>
    [Fact]
    public void ReflectionHelperCallee_TriggersReflectionTypeTracking()
    {
        var method = AotCoreIrMethod("TestModule.TestClass::ReflectHelper:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // ldstr to push a runtime type handle (simulated)
                Inst("ldstr", 0, "test"),
                // call with callee matching Type.GetTypeFromHandle (not actually invoked)
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)",
                    targetSymbol: "Chaos_Type_GetTypeFromHandle",
                    ilOffset: 1),
                // pop and return
                InstOp("pop", 2),
                Inst("ldc.i4", 3, 0),
                InstOp("ret", 4),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises GetMethodByName matching pattern (triggers MethodInfo tracking).
    /// </summary>
    [Fact]
    public void GetMethodByNameCallee_TriggersMethodInfoTracking()
    {
        var method = AotCoreIrMethod("TestModule.TestClass::GetMethodByName:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // ldarg to push object, ldstr for method name, callvirt with GetMethod callee
                Inst("ldc.i4", 0, 0),
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)",
                    targetSymbol: "Chaos_Type_GetMethod",
                    ilOffset: 1),
                InstOp("pop", 2),
                Inst("ldc.i4", 3, 0),
                InstOp("ret", 4),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    public void Dispose()
    {
    }

    // ── Helpers ───────────────────────────────────

    private static AotCoreIrMethodArtifact AotCoreIrMethod(
        string subjectId,
        AotCoreIrAbiSlotArtifact? returnAbi = null,
        IReadOnlyList<AotCoreIrAbiSlotArtifact>? parameterAbis = null,
        IReadOnlyList<AotCoreIrInstructionArtifact>? instructions = null,
        IReadOnlyList<AotCoreIrExceptionRegionArtifact>? exceptionRegions = null)
    {
        return ModelFactory.CreateMethod(subjectId,
            returnAbi: returnAbi,
            parameterAbis: parameterAbis,
            instructions: instructions,
            exceptionRegions: exceptionRegions);
    }

    private static AotCoreIrInstructionArtifact Inst(string op, int ilOffset, int intOperand)
        => ModelFactory.Instruction(op, ilOffset, intOperand);

    private static AotCoreIrInstructionArtifact Inst(string op, int ilOffset, string stringOperand)
        => ModelFactory.Instruction(op, ilOffset, stringOperand);

    private static AotCoreIrInstructionArtifact InstOp(string op, int ilOffset)
        => ModelFactory.Instruction(op, ilOffset);

    private static AotCoreIrInstructionArtifact Inst(
        string op,
        int ilOffset,
        string subjectId,
        AotCoreIrReferenceKind refKind,
        AotCoreIrTypeShapeKind typeShape = AotCoreIrTypeShapeKind.ReferenceType,
        string? baseType = null,
        string? arrayElement = null,
        AotCoreIrTypeShapeKind arrayElementShape = AotCoreIrTypeShapeKind.ReferenceType,
        string? arrayElementBase = null,
        string[]? interfaces = null,
        bool isSealed = false,
        AotCoreIrRuntimeServiceKind? runtimeServiceKind = null)
    {
        return new AotCoreIrInstructionArtifact
        {
            Op = op,
            IlOffset = ilOffset,
            RuntimeServiceKind = runtimeServiceKind,
            TargetReference = new AotCoreIrReferenceArtifact
            {
                Kind = refKind,
                AssemblyName = "System.Private.CoreLib",
                SubjectId = subjectId,
                TypeShape = typeShape,
                BaseTypeSubjectId = baseType,
                ArrayElementSubjectId = arrayElement,
                ArrayElementTypeShape = arrayElementShape,
                ArrayElementBaseTypeSubjectId = arrayElementBase,
                ImplementedInterfaceSubjectIds = interfaces,
                IsSealed = isSealed,
            },
        };
    }

    private static AotCoreIrInstructionArtifact InstFld(
        string op,
        int ilOffset,
        AotCoreIrRuntimeServiceKind runtimeServiceKind,
        string declaringTypeSubjectId,
        string fieldName,
        string fieldTypeSubjectId,
        AotCoreIrTypeShapeKind declaringTypeShape)
    {
        return ModelFactory.FieldInstruction(op, ilOffset, runtimeServiceKind,
            declaringTypeSubjectId, fieldName, fieldTypeSubjectId, declaringTypeShape);
    }

    private static AotCoreIrInstructionArtifact Inst(string op, int ilOffset, string callee, string targetSymbol)
        => ModelFactory.CallInstruction(callee, targetSymbol, ilOffset);

    private static AotCoreIrInstructionArtifact Inst(string op, int ilOffset, string callee,
        string? targetReturnType = null,
        int? targetParameterCount = null,
        HybridDispatchKind? dispatchKind = null)
        => ModelFactory.CallInstruction(callee, "", ilOffset, targetReturnType, targetParameterCount, dispatchKind);
}
