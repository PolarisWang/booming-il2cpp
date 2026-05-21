using Chaos.IL2CPP.Generator.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Tests covering call and call dispatch mechanisms:
/// call, callvirt, ldftn, ldvirtftn, calli, jmp.
/// </summary>
public sealed class MethodDispatchEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void DirectCall_EmitsDirectCall()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CallDirect",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.CallInstruction(
                    callee: "TestModule.Other::Compute",
                    targetSymbol: "Chaos_Test_Compute",
                    ilOffset: 0),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    [Fact]
    public void DirectCallWithArgs_EmitsDirectCall()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CallDirectWithArgs",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.CallInstruction(
                    callee: "TestModule.Other::Add",
                    targetSymbol: "Chaos_Test_Add",
                    targetReturnType: "System.Int32",
                    targetParameterCount: 2,
                    ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void VirtualCall_EmitsVirtualDispatch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CallVirtual",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("callvirt", ilOffset: 1,
                    callee: "TestModule.ITest::GetValue:System.Int32()",
                    targetSymbol: "Chaos_ITest_GetValue"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    [Fact]
    public void LoadFunctionPointer_EmitsLdFtn()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetFnPtr",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldftn", ilOffset: 0,
                    callee: "TestModule.TestClass::TargetMethod",
                    targetSymbol: "Chaos_TargetFunction"),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    [Fact]
    public void LoadVirtualFunctionPointer_ThrowsWithoutVtable()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetVirtFnPtr",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldvirtftn", ilOffset: 1,
                    subjectId: "TestModule.ITest::GetValue",
                    refKind: AotCoreIrReferenceKind.Method,
                    assemblyName: "TestAssembly"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var ex = Assert.Throws<NotSupportedException>(() => _fixture.RunPlannerSingleMethod(method));
        Assert.Contains("vtable slot not found", ex.Message);
    }

    [Fact]
    public void CalliIndirect_EmitsIndirectCall()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CallIndirect",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                new AotCoreIrInstructionArtifact
                {
                    Op = "calli",
                    IlOffset = 2,
                    TargetReference = new AotCoreIrReferenceArtifact
                    {
                        Kind = AotCoreIrReferenceKind.Method,
                        AssemblyName = "TestAssembly",
                        SubjectId = "TestModule.IFoo::Bar",
                    },
                    CallSiteSignature = new ManagedCallSiteSignature
                    {
                        KindCode = ManagedCallSiteKind.FunctionPointer,
                        ReturnType = "System.Int32",
                        ParameterTypes = new[] { "System.Int32" },
                    },
                },
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void JmpTailCall_EmitsJump()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TailCall",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                new AotCoreIrInstructionArtifact
                {
                    Op = "jmp",
                    IlOffset = 1,
                    Callee = "TestModule.Other::Process",
                    TargetSymbol = "Chaos_Other_Process",
                    TargetReturnType = "System.Void",
                },
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    public void Dispose()
    {
    }
}
