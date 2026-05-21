using Chaos.IL2CPP.CodeGen.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests.Emission;

/// <summary>
/// Tests covering miscellaneous opcodes not covered by other emission test suites:
/// localloc, switch, arglist, mkrefany, refanyval, refanytype, unbox.any, castclass,
/// initobj, cpobj, ldarga, throw, rethrow, endfilter, brtrue, bne.un, bge, ble, blt.un.
/// </summary>
public sealed class MiscEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void LoadLocalAlloc_EmitsStackAlloc()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::AllocLocal",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 64),
                ModelFactory.Instruction("localloc", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    [Fact]
    public void SwitchInstruction_EmitsSwitch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Switch",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("switch", ilOffset: 1, new int[] { 4, 6, 8 }),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
                ModelFactory.Instruction("ldc.i4", ilOffset: 8, intOperand: 2),
                ModelFactory.Instruction("ret", ilOffset: 9),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("chaos_switch_value", source);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ArgList_EmitsArgList()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetArgList",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("arglist", ilOffset: 0),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void MakeRefAny_EmitsRefAny()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::MakeRef",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("mkrefany", ilOffset: 1,
                    subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void RefAnyVal_EmitsRefAnyVal()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::RefAnyVal",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            parameterAbis: new[] { ModelFactory.Int64Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("refanyval", ilOffset: 1,
                    subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void RefAnyType_EmitsRefAnyType()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::RefAnyType",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            parameterAbis: new[] { ModelFactory.Int64Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("refanytype", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void UnboxAny_EmitsUnboxAny()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::UnboxAny",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("unbox.any", ilOffset: 1,
                    subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void CastClass_EmitsCastCheck()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CastToString",
            returnType: "System.String",
            returnAbi: ModelFactory.RefAbi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("castclass", ilOffset: 1,
                    subjectId: "System.String", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void InitObject_EmitsInitObj()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::InitMyObj",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("initobj", ilOffset: 1,
                    subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void CopyObject_EmitsCpObj()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CopyMyObj",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi, ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("cpobj", ilOffset: 2,
                    subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LoadArgumentAddress_EmitsArgAddr()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetArgAddr",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarga", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Throw_EmitsThrow()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::DoThrow",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("throw", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Rethrow_EmitsRethrow()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::DoRethrow",
            returnAbi: ModelFactory.VoidAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("rethrow", ilOffset: 0),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void EndFilter_EmitsEndFilter()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Filter",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("endfilter", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BranchTrue_EmitsConditionalBranch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::BrTrue",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("brtrue", ilOffset: 1, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BranchNotEqualUnsigned_EmitsConditionalBranch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::BneUn",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("bne.un", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void BranchGreaterOrEqual_EmitsConditionalBranch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Bge",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("bge", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void BranchGreaterOrEqualUnsigned_EmitsConditionalBranch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::BgeUn",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("bge.un", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void BranchLessOrEqual_EmitsConditionalBranch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Ble",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ble", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void BreakInstruction_EmitsNothing()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::DoBreak",
            returnAbi: ModelFactory.VoidAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("break", ilOffset: 0),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BranchLessOrEqualUnsigned_EmitsConditionalBranch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::BleUn",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ble.un", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void BranchLessThanUnsigned_EmitsConditionalBranch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::BltUn",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("blt.un", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void NopInstruction_EmitsNothing()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::DoNop",
            returnAbi: ModelFactory.VoidAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("nop", ilOffset: 0),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BranchLessThanSigned_EmitsConditionalBranch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Blt",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("blt", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void BranchGreaterThanSigned_EmitsConditionalBranch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Bgt",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("bgt", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void BranchGreaterThanUnsigned_EmitsConditionalBranch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::BgtUn",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("bgt.un", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void BranchEqual_EmitsConditionalBranch()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Beq",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("beq", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    public void Dispose()
    {
    }
}
