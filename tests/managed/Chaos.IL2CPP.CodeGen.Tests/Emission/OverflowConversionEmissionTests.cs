using Chaos.IL2CPP.Generator.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Tests covering all conv.ovf.* conversion opcode variants (20 variants total).
/// Each test verifies the planner emits a valid structured-IR path for the opcode.
/// </summary>
public sealed class OverflowConversionEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void ConvOvfI1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfI1",
            returnType: "System.SByte",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 100),
                ModelFactory.Instruction("conv.ovf.i1", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfU1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfU1",
            returnType: "System.Byte",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 200),
                ModelFactory.Instruction("conv.ovf.u1", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfI2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfI2",
            returnType: "System.Int16",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 30000),
                ModelFactory.Instruction("conv.ovf.i2", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfU2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfU2",
            returnType: "System.UInt16",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 60000),
                ModelFactory.Instruction("conv.ovf.u2", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfI4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfI4",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", ilOffset: 0, longOperand: 0x7FFFFFFF),
                ModelFactory.Instruction("conv.ovf.i4", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfU4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfU4",
            returnType: "System.UInt32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", ilOffset: 0, longOperand: 0xFFFFFFFF),
                ModelFactory.Instruction("conv.ovf.u4", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfI8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfI8",
            returnType: "System.Int64",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", ilOffset: 0, longOperand: 0x7FFFFFFFFFFFFFFF),
                ModelFactory.Instruction("conv.ovf.i8", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfU8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfU8",
            returnType: "System.UInt64",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", ilOffset: 0, longOperand: -1),
                ModelFactory.Instruction("conv.ovf.u8", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfNativeInt_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfI",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.i", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfNativeUInt_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfU",
            returnType: "System.UIntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.ovf.u", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfI8Un_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfI8Un",
            returnType: "System.Int64",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", ilOffset: 0, longOperand: 0x7FFFFFFFFFFFFFFF),
                ModelFactory.Instruction("conv.ovf.i8.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfU8Un_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfU8Un",
            returnType: "System.UInt64",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", ilOffset: 0, longOperand: -1),
                ModelFactory.Instruction("conv.ovf.u8.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfNativeIntUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfIUn",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.ovf.i.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfNativeUIntUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfUUn",
            returnType: "System.UIntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.ovf.u.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfI1Un_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfI1Un",
            returnType: "System.SByte",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.ovf.i1.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfI2Un_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfI2Un",
            returnType: "System.Int16",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.ovf.i2.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfI4Un_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfI4Un",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.ovf.i4.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfU1Un_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfU1Un",
            returnType: "System.Byte",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.ovf.u1.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfU2Un_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfU2Un",
            returnType: "System.UInt16",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.ovf.u2.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void ConvOvfU4Un_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvOvfU4Un",
            returnType: "System.UInt32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.ovf.u4.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.UsesStructuredIR(_fixture.RunPlannerSingleMethod(method));
    }

    public void Dispose()
    {
    }
}
