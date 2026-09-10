using Chaos.IL2CPP.Generator.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Targeted tests that exercise specific uncovered code paths in
/// ObjectModelUtilities and ObjectModelEmission.
/// </summary>
public sealed class UtilityPathTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void FieldValueTypeDeclaring_TriggersCollectValueTypePaths()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::FieldValueTypeDecl:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                new AotCoreIrInstructionArtifact
                {
                    Op = "ldfld", IlOffset = 0,
                    RuntimeServiceKind = AotCoreIrRuntimeServiceKind.LoadInstanceField,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Field,
                        AssemblyName = "TestModule",
                        SubjectId = "TestModule.MyValueType::_field",
                        DeclaringTypeSubjectId = "TestModule.MyValueType",
                        DeclaringTypeShape = AotCoreIrTypeShapeKind.ValueType,
                        FieldTypeSubjectId = "System.Int32",
                    },
                },
                new AotCoreIrInstructionArtifact
                {
                    Op = "newarr", IlOffset = 1,
                    RuntimeServiceKind = AotCoreIrRuntimeServiceKind.NewArray,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "TestModule",
                        SubjectId = "TestModule.MyClass[]",
                        TypeShape = AotCoreIrTypeShapeKind.ReferenceType,
                        ArrayElementSubjectId = "TestModule.MyArrayElementClass",
                        ArrayElementTypeShape = AotCoreIrTypeShapeKind.ReferenceType,
                        ArrayElementBaseTypeSubjectId = "TestModule.MyArrayElementBase",
                        ArrayElementImplementedInterfaceSubjectIds = new[] { "TestModule.IArrayElementInterface" },
                    },
                },
                ModelFactory.Instruction("pop", ilOffset: 2),
                ModelFactory.Instruction("pop", ilOffset: 3),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void FieldInterfaceTypeDeclaring_TriggersInterfaceTracking()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::FieldIfaceDecl:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                new AotCoreIrInstructionArtifact
                {
                    Op = "ldfld", IlOffset = 0,
                    RuntimeServiceKind = AotCoreIrRuntimeServiceKind.LoadInstanceField,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Field,
                        AssemblyName = "TestModule",
                        SubjectId = "TestModule.IMyInterface::_field",
                        DeclaringTypeSubjectId = "TestModule.IMyInterface",
                        DeclaringTypeShape = AotCoreIrTypeShapeKind.InterfaceType,
                        FieldTypeSubjectId = "System.Int32",
                    },
                },
                ModelFactory.Instruction("pop", ilOffset: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LoadStoreObjectOps_TriggersValueTypeTracking()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdobjStobj:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                new AotCoreIrInstructionArtifact
                {
                    Op = "ldobj", IlOffset = 0,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "TestModule",
                        SubjectId = "TestModule.MyValueType",
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                    },
                },
                new AotCoreIrInstructionArtifact
                {
                    Op = "stobj", IlOffset = 1,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "TestModule",
                        SubjectId = "TestModule.MyOtherValueType",
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                    },
                },
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ReflectionCalleeMatching_TriggersTypeTracking()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ReflCallees:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // GetConstructors callee matching (line 143-146)
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.Type::GetConstructors(System.Reflection.BindingFlags)",
                    targetSymbol: "Chaos_Type_GetConstructors",
                    ilOffset: 0),
                // GetField callee matching (line 147-150)
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.Type::GetField(System.String)",
                    targetSymbol: "Chaos_Type_GetField",
                    ilOffset: 1),
                // MethodBase::GetParameters callee matching (line 151-154)
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.Reflection.MethodBase::GetParameters()",
                    targetSymbol: "Chaos_MethodBase_GetParameters",
                    ilOffset: 2),
                // ParameterInfo::get_Name callee matching (line 151-154)
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.Reflection.ParameterInfo::get_Name()",
                    targetSymbol: "Chaos_ParameterInfo_get_Name",
                    ilOffset: 3),
                // Assembly.GetType callee for assembly reflection helper
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.Reflection.Assembly::GetType(System.String)",
                    targetSymbol: "Chaos_Assembly_GetType",
                    ilOffset: 4),
                // Assembly.get_Assembly for assembly reflection helper
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.Type::get_Assembly()",
                    targetSymbol: "Chaos_Type_get_Assembly",
                    ilOffset: 5),
                ModelFactory.Instruction("pop", ilOffset: 6),
                ModelFactory.Instruction("pop", ilOffset: 7),
                ModelFactory.Instruction("pop", ilOffset: 8),
                ModelFactory.Instruction("pop", ilOffset: 9),
                ModelFactory.Instruction("pop", ilOffset: 10),
                ModelFactory.Instruction("pop", ilOffset: 11),
                ModelFactory.Instruction("ldc.i4", ilOffset: 12, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 13),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void InterfaceWithVirtualMethods_TriggersVtableInfo()
    {
        var ifaceMethod = ModelFactory.CreateMethod(
            "TestModule.IMyInterface::VirtualMethod:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            isStatic: false,
            nativeSymbol: "Chaos_IMyInterface_VirtualMethod",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var driverMethod = ModelFactory.CreateMethod(
            "TestModule.TestClass::IfaceVtable:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                new AotCoreIrInstructionArtifact
                {
                    Op = "isinst", IlOffset = 0,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "TestModule",
                        SubjectId = "TestModule.IMyInterface",
                        TypeShape = AotCoreIrTypeShapeKind.InterfaceType,
                    },
                },
                ModelFactory.Instruction("pop", ilOffset: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var artifact = ModelFactory.CreateArtifact(ifaceMethod, driverMethod);
        var templateModel = _fixture.RunPlanner(artifact);
        Assert.NotNull(templateModel);
        Assert.NotEmpty(templateModel.ObjectModelCode);
    }

    /// <summary>
    /// Exercises AotCoreIrAbiSlotArtifact with ValueTypeByValue carrier kind
    /// to trigger the TrackAbiSlotCarrier path (line 1130-1132).
    /// </summary>
    [Fact]
    public void ValueTypeByValueAbi_TriggersValueTypeTracking()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ValueTypeAbi:TestType(System.Int32)",
            returnAbi: new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.ValueTypeByValue,
                TypeSubjectId = "TestModule.MyAbiValueType",
            },
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises default interpolated string handler helper subject ID matching.
    /// Uses a call with a callee that matches the default handler pattern.
    /// </summary>
    [Fact]
    public void DefaultInterpolatedStringHandler_TriggersTracking()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::InterpString:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.Runtime.CompilerServices.DefaultInterpolatedStringHandler::.ctor(System.Int32,System.Int32)",
                    targetSymbol: "Chaos_DefaultInterpolatedStringHandler_ctor",
                    ilOffset: 0),
                ModelFactory.Instruction("pop", ilOffset: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises the external runtime helper with referenced static fields
    /// (lines 64-74 in ObjectModelEmission.cs).
    /// </summary>
    [Fact]
    public void ExternalRuntimeHelperWithStaticFields_TriggersFieldTracking()
    {
        // Use a method with InitObject on a type that will be tracked,
        // combined with field access to trigger static field tracking
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::HelperStaticFields:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // initobj triggers InitObject path (tracked type)
                new AotCoreIrInstructionArtifact
                {
                    Op = "initobj", IlOffset = 0,
                    RuntimeServiceKind = AotCoreIrRuntimeServiceKind.InitObject,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "TestModule",
                        SubjectId = "TestModule.MyValueType",
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                    },
                },
                // ldsfld to exercise static field tracking
                ModelFactory.FieldInstruction(
                    "ldsfld", 1,
                    AotCoreIrRuntimeServiceKind.LoadStaticField,
                    "TestModule.MyStaticFieldHolder",
                    "s_staticField",
                    "System.Int32",
                    AotCoreIrTypeShapeKind.ReferenceType),
                ModelFactory.Instruction("pop", ilOffset: 2),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    public void Dispose()
    {
    }
}
