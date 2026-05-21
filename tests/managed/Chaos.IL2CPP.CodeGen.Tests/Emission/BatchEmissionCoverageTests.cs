using Chaos.IL2CPP.CodeGen.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests.Emission;

/// <summary>
/// Tests targeting specific emission pipelines within NativeAotLoweringPlanner
/// that may not be fully exercised by standard opcode-coverage tests.
/// </summary>
public sealed class BatchEmissionCoverageTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void OverflowAdd_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::OvAdd",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 100),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 200),
                ModelFactory.Instruction("add.ovf", 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void OverflowSub_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::OvSub",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 100),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 200),
                ModelFactory.Instruction("sub.ovf", 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void OverflowMul_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::OvMul",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 100),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 200),
                ModelFactory.Instruction("mul.ovf", 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void OverflowAddUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::OvAddUn",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 100),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 200),
                ModelFactory.Instruction("add.ovf.un", 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void OverflowSubUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::OvSubUn",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 100),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 200),
                ModelFactory.Instruction("sub.ovf.un", 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void OverflowMulUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::OvMulUn",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 100),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 200),
                ModelFactory.Instruction("mul.ovf.un", 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfI1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvOvfI1",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.i1", 1, resultType: "System.SByte"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfU1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvOvfU1",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.u1", 1, resultType: "System.Byte"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfI2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvOvfI2",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.i2", 1, resultType: "System.Int16"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfU2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvOvfU2",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.u2", 1, resultType: "System.UInt16"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfI4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvOvfI4",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.i4", 1, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfU4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvOvfU4",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.u4", 1, resultType: "System.UInt32"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfI8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvOvfI8",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", 0, longOperand: 42L, resultType: "System.Int64"),
                ModelFactory.Instruction("conv.ovf.i8", 1, resultType: "System.Int64"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfU8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvOvfU8",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", 0, longOperand: 42L, resultType: "System.Int64"),
                ModelFactory.Instruction("conv.ovf.u8", 1, resultType: "System.UInt64"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfI_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvOvfI",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.i", 1, resultType: "System.IntPtr"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfU_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvOvfU",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.u", 1, resultType: "System.UIntPtr"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindI1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdindI1",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("ldind.i1", 1, resultType: "System.SByte"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindU1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdindU1",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("ldind.u1", 1, resultType: "System.Byte"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindI2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdindI2",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("ldind.i2", 1, resultType: "System.Int16"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindU2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdindU2",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("ldind.u2", 1, resultType: "System.UInt16"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindI4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdindI4",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("ldind.i4", 1, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindU4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdindU4",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("ldind.u4", 1, resultType: "System.UInt32"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindI8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdindI8",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", 0, longOperand: 42L, resultType: "System.Int64"),
                ModelFactory.Instruction("ldind.i8", 1, resultType: "System.Int64"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindR4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdindR4",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.r4", 0, doubleOperand: 3.14, resultType: "System.Single"),
                ModelFactory.Instruction("ldind.r4", 1, resultType: "System.Single"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindR8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdindR8",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.r8", 0, doubleOperand: 3.14, resultType: "System.Double"),
                ModelFactory.Instruction("ldind.r8", 1, resultType: "System.Double"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindRef_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdindRef",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 0),
                ModelFactory.Instruction("ldind.ref", 1, resultType: "System.Object"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StindI1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::StindI1",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 42),
                ModelFactory.Instruction("stind.i1", 2),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StindI2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::StindI2",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 42),
                ModelFactory.Instruction("stind.i2", 2),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StindI4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::StindI4",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 42),
                ModelFactory.Instruction("stind.i4", 2),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StindI8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::StindI8",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i8", 1, longOperand: 42L, resultType: "System.Int64"),
                ModelFactory.Instruction("stind.i8", 2),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StindR4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::StindR4",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 0),
                ModelFactory.Instruction("ldc.r4", 1, doubleOperand: 3.14, resultType: "System.Single"),
                ModelFactory.Instruction("stind.r4", 2),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StindR8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::StindR8",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 0),
                ModelFactory.Instruction("ldc.r8", 1, doubleOperand: 3.14, resultType: "System.Double"),
                ModelFactory.Instruction("stind.r8", 2),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StindRef_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::StindRef",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 0),
                ModelFactory.Instruction("ldnull", 1),
                ModelFactory.Instruction("stind.ref", 2),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvI1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvI1",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.i1", 1, resultType: "System.SByte"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvU1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvU1",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.u1", 1, resultType: "System.Byte"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvI2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvI2",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.i2", 1, resultType: "System.Int16"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvU2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvU2",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.u2", 1, resultType: "System.UInt16"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvU8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvU8",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.u8", 1, resultType: "System.UInt64"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvRUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::ConvRUn",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 42),
                ModelFactory.Instruction("conv.r.un", 1, resultType: "System.Single"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Cgt_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::Cgt",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 3),
                ModelFactory.Instruction("cgt", 2),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void CgtUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::CgtUn",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 3),
                ModelFactory.Instruction("cgt.un", 2),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void CltUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::CltUn",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 3),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 5),
                ModelFactory.Instruction("clt.un", 2),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdcI8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdcI8",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", 0, longOperand: 1234567890123L, resultType: "System.Int64"),
                ModelFactory.Instruction("ret", 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdcR4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdcR4",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.r4", 0, doubleOperand: 3.14, resultType: "System.Single"),
                ModelFactory.Instruction("ret", 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdcR8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::LdcR8",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.r8", 0, doubleOperand: 3.14159265359, resultType: "System.Double"),
                ModelFactory.Instruction("ret", 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BgtUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::BgtUn",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 3),
                ModelFactory.Instruction("bgt.un", 2, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", 3, intOperand: 0),
                ModelFactory.Instruction("ret", 4),
                ModelFactory.Instruction("ldc.i4", 5, intOperand: 1),
                ModelFactory.Instruction("ret", 6),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BleUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::BleUn",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 3),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 5),
                ModelFactory.Instruction("ble.un", 2, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", 3, intOperand: 0),
                ModelFactory.Instruction("ret", 4),
                ModelFactory.Instruction("ldc.i4", 5, intOperand: 1),
                ModelFactory.Instruction("ret", 6),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BgeUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::BgeUn",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 3),
                ModelFactory.Instruction("bge.un", 2, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", 3, intOperand: 0),
                ModelFactory.Instruction("ret", 4),
                ModelFactory.Instruction("ldc.i4", 5, intOperand: 1),
                ModelFactory.Instruction("ret", 6),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BltUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::BltUn",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 3),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 5),
                ModelFactory.Instruction("blt.un", 2, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", 3, intOperand: 0),
                ModelFactory.Instruction("ret", 4),
                ModelFactory.Instruction("ldc.i4", 5, intOperand: 1),
                ModelFactory.Instruction("ret", 6),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BneUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::BneUn",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 3),
                ModelFactory.Instruction("ldc.i4", 1, intOperand: 5),
                ModelFactory.Instruction("bne.un", 2, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", 3, intOperand: 0),
                ModelFactory.Instruction("ret", 4),
                ModelFactory.Instruction("ldc.i4", 5, intOperand: 1),
                ModelFactory.Instruction("ret", 6),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Switch_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::Switch",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 0),
                ModelFactory.Instruction("switch", 1, arrayOperand: new[] { 3, 4, 5 }),
                ModelFactory.Instruction("ldc.i4", 2, intOperand: -1),
                ModelFactory.Instruction("ret", 3),
                ModelFactory.Instruction("ldc.i4", 4, intOperand: 0),
                ModelFactory.Instruction("ret", 5),
                ModelFactory.Instruction("ldc.i4", 6, intOperand: 1),
                ModelFactory.Instruction("ret", 7),
                ModelFactory.Instruction("ldc.i4", 8, intOperand: 2),
                ModelFactory.Instruction("ret", 9),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Localloc_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::Localloc",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 64),
                ModelFactory.Instruction("localloc", 1),
                ModelFactory.Instruction("pop", 2),
                ModelFactory.Instruction("ldc.i4", 3, intOperand: 0),
                ModelFactory.Instruction("ret", 4),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Ldarga_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::Ldarga",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarga", 0, intOperand: 0),
                ModelFactory.Instruction("ldind.i4", 1, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Arglist_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::Arglist",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("arglist", 0),
                ModelFactory.Instruction("pop", 1),
                ModelFactory.Instruction("ldc.i4", 2, intOperand: 0),
                ModelFactory.Instruction("ret", 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Endfilter_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.TestClass::Endfilter",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", 0, intOperand: 0),
                ModelFactory.Instruction("endfilter", 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    public void Dispose()
    {
    }
}
