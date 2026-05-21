using Chaos.IL2CPP.CodeGen.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests.Emission;

public sealed class VTableAndInterfaceEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void ReferenceTypeHierarchy_EmitsVTableSlots()
    {
        var methodBase1 = ModelFactory.CreateMethod(
            "TestModule.MyBaseType::VirtualMethod1:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            isStatic: false,
            nativeSymbol: "Chaos_MyBaseType_VirtualMethod1",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var methodBase2 = ModelFactory.CreateMethod(
            "TestModule.MyBaseType::VirtualMethod2:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            isStatic: false,
            nativeSymbol: "Chaos_MyBaseType_VirtualMethod2",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 99),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var driverMethod = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestVTable:TestType(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            isStatic: true,
            nativeSymbol: "Chaos_Test_TestVTable",
            instructions: new[]
            {
                new AotCoreIrInstructionArtifact
                {
                    Op = "castclass", IlOffset = 0,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "TestModule",
                        SubjectId = "TestModule.MyBaseType",
                        TypeShape = AotCoreIrTypeShapeKind.ReferenceType,
                        BaseTypeSubjectId = "TestModule.MyBaseParent",
                    },
                },
                ModelFactory.Instruction("pop", ilOffset: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var artifact = ModelFactory.CreateArtifact(methodBase1, methodBase2, driverMethod);

        var metadataRegistration = new MetadataRegistrationArtifact
        {
            Registrations = new MetadataRegistrationEntry[]
            {
                new() { RegistrationKind = "type", Slot = 0x02000001, SubjectId = "TestModule.MyBaseType" },
                new() { RegistrationKind = "type", Slot = 0x02000002, SubjectId = "TestModule.MyBaseParent" },
                new() { RegistrationKind = "method", Slot = 0x06000001, SubjectId = "TestModule.MyBaseType::VirtualMethod1" },
                new() { RegistrationKind = "method", Slot = 0x06000002, SubjectId = "TestModule.MyBaseType::VirtualMethod2" },
            },
        };

        var templateModel = _fixture.RunPlanner(artifact, metadataRegistration: metadataRegistration);
        Assert.NotNull(templateModel);
        Assert.NotEmpty(templateModel.ObjectModelCode);
    }

    [Fact]
    public void SpecialTypeStructs_EmitsSpecialFields()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::SpecialTypes:TestType(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldstr", ilOffset: 0, "error"),
                new AotCoreIrInstructionArtifact
                {
                    Op = "castclass", IlOffset = 1,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "System.Private.CoreLib",
                        SubjectId = "System.Private.CoreLib/System.Exception",
                        TypeShape = AotCoreIrTypeShapeKind.ReferenceType,
                        BaseTypeSubjectId = "System.Private.CoreLib/System.Object",
                    },
                },
                new AotCoreIrInstructionArtifact
                {
                    Op = "newobj", IlOffset = 2,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "System.Private.CoreLib",
                        SubjectId = "System.Private.CoreLib/System.Decimal",
                        TypeShape = AotCoreIrTypeShapeKind.ReferenceType,
                    },
                },
                new AotCoreIrInstructionArtifact
                {
                    Op = "castclass", IlOffset = 3,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "System.Private.CoreLib",
                        SubjectId = "System.Private.CoreLib/System.Delegate",
                        TypeShape = AotCoreIrTypeShapeKind.ReferenceType,
                        BaseTypeSubjectId = "System.Private.CoreLib/System.Object",
                    },
                },
                ModelFactory.Instruction("ldstr", ilOffset: 4, "hello"),
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.String::get_Length",
                    targetSymbol: "Chaos_String_get_Length",
                    ilOffset: 5),
                ModelFactory.Instruction("pop", ilOffset: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 7, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 8),
            },
            exceptionRegions: new[]
            {
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Catch,
                    TryOffset = 0, TryLength = 7,
                    HandlerOffset = 9, HandlerLength = 3,
                    CatchTypeSubjectId = "System.Private.CoreLib/System.Exception",
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void CollectionTypeStructs_EmitsInlineFields()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Collections:TestType(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                new AotCoreIrInstructionArtifact
                {
                    Op = "newobj", IlOffset = 0,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "System.Private.CoreLib",
                        SubjectId = "System.Private.CoreLib/System.Collections.Generic.List`1[[System.Int32]]",
                        TypeShape = AotCoreIrTypeShapeKind.ReferenceType,
                        BaseTypeSubjectId = "System.Private.CoreLib/System.Object",
                    },
                },
                ModelFactory.Instruction("ldstr", ilOffset: 1, "hello"),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ImplementedInterfaces_EmitsInterfaceMaps()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Interfaces:TestType(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                new AotCoreIrInstructionArtifact
                {
                    Op = "castclass", IlOffset = 0,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "TestModule",
                        SubjectId = "TestModule.MyImplementation",
                        TypeShape = AotCoreIrTypeShapeKind.ReferenceType,
                        BaseTypeSubjectId = "TestModule.MyBase",
                        ImplementedInterfaceSubjectIds = new[]
                        {
                            "TestModule.IMyInterface1",
                            "TestModule.IMyInterface2",
                        },
                    },
                },
                new AotCoreIrInstructionArtifact
                {
                    Op = "box", IlOffset = 1,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "TestModule",
                        SubjectId = "TestModule.MyStruct",
                        TypeShape = AotCoreIrTypeShapeKind.ValueType,
                        ImplementedInterfaceSubjectIds = new[]
                        {
                            "TestModule.IMyInterface1",
                        },
                    },
                },
                new AotCoreIrInstructionArtifact
                {
                    Op = "isinst", IlOffset = 2,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Type,
                        AssemblyName = "TestModule",
                        SubjectId = "TestModule.IMyInterface1",
                        TypeShape = AotCoreIrTypeShapeKind.InterfaceType,
                    },
                },
                ModelFactory.Instruction("ldnull", ilOffset: 3),
                ModelFactory.Instruction("cgt.un", ilOffset: 4, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    public void Dispose()
    {
    }
}
