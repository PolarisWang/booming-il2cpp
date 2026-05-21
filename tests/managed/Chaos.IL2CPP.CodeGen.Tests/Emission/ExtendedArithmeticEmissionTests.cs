using Chaos.IL2CPP.CodeGen.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests.Emission;

/// <summary>
/// Tests for extended arithmetic, conversion, comparison, and float-constant
/// operations not covered by the basic ArithmeticEmissionTests suite.
/// </summary>
public sealed class ExtendedArithmeticEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void OverflowAddInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::OvfAdd",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 2147483640),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 100),
                ModelFactory.Instruction("add.ovf", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void OverflowAddUnsignedInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::OvfAddUn",
            returnType: "System.UInt32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("add.ovf.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void OverflowSubInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::OvfSub",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -2147483640),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 100),
                ModelFactory.Instruction("sub.ovf", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void OverflowSubUnsignedInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::OvfSubUn",
            returnType: "System.UInt32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("sub.ovf.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void OverflowMulInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::OvfMul",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1000000),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 100000),
                ModelFactory.Instruction("mul.ovf", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void OverflowMulUnsignedInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::OvfMulUn",
            returnType: "System.UInt32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 2),
                ModelFactory.Instruction("mul.ovf.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void UnsignedDiv_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::DivUn",
            returnType: "System.UInt32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 2),
                ModelFactory.Instruction("div.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void UnsignedRem_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::RemUn",
            returnType: "System.UInt32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("rem.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void UnsignedShiftRight_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ShrUn",
            returnType: "System.UInt32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 4),
                ModelFactory.Instruction("shr.un", ilOffset: 2, resultType: "System.UInt32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void CkFinite_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CheckFinite",
            returnType: "System.Double",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.r8", ilOffset: 0, doubleOperand: 3.14),
                ModelFactory.Instruction("ckfinite", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void LoadInt64Constant_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LoadBigInt",
            returnType: "System.Int64",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", ilOffset: 0, longOperand: 0x7FFFFFFFFFFFFFFF),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void LoadFloat32Constant_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LoadFloat",
            returnType: "System.Single",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.r4", ilOffset: 0, doubleOperand: 3.14f),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void LoadFloat64Constant_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LoadDouble",
            returnType: "System.Double",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.r8", ilOffset: 0, doubleOperand: 3.14159265358979),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void CompareGreaterThanSigned_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CgtSigned",
            returnType: "System.Boolean",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("cgt", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void CompareLessThanSigned_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CltSigned",
            returnType: "System.Boolean",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 3),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 5),
                ModelFactory.Instruction("clt", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void CompareLessThanUnsigned_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CltUn",
            returnType: "System.Boolean",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 3),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: -5),
                ModelFactory.Instruction("clt.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ConvertToInt8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvToI1",
            returnType: "System.SByte",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 255),
                ModelFactory.Instruction("conv.i1", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ConvertToUInt8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvToU1",
            returnType: "System.Byte",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.u1", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ConvertToInt16_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvToI2",
            returnType: "System.Int16",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 70000),
                ModelFactory.Instruction("conv.i2", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ConvertToUInt16_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvToU2",
            returnType: "System.UInt16",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.u2", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ConvertToUInt64_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvToU8",
            returnType: "System.UInt64",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.u8", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ConvertToNativeInt_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvToI",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.i", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ConvertToNativeUInt_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvToU",
            returnType: "System.UIntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.u", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ConvertToUnsignedFloat_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvRUn",
            returnType: "System.Single",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.r.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    public void Dispose()
    {
    }
}
