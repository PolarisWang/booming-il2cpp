using Chaos.IL2CPP.CodeGen.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests.Emission;

/// <summary>
/// Tests covering arithmetic, conversion, and numeric operations
/// (add, sub, mul, div, conv.*, etc.) emission paths.
/// </summary>
public sealed class ArithmeticEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void AddInt32_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Add",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("add", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void SubInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Sub",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("sub", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void MulInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Mul",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 3),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 4),
                ModelFactory.Instruction("mul", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void DivInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Div",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("div", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void RemInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Rem",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("rem", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void NegInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Neg",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 5),
                ModelFactory.Instruction("neg", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void AndInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::And",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0xFF),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0x0F),
                ModelFactory.Instruction("and", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void OrInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Or",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0xF0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0x0F),
                ModelFactory.Instruction("or", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void XorInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Xor",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0xFF),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0x0F),
                ModelFactory.Instruction("xor", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ShlInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Shl",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("shl", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ShrInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Shr",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 8),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 2),
                ModelFactory.Instruction("shr", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void NotInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Not",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0xFF),
                ModelFactory.Instruction("not", ilOffset: 1, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ConvI4ToI8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvI4ToI8",
            returnType: "System.Int64",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.i8", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ConvI4ToR4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvI4ToR4",
            returnType: "System.Single",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.r4", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ConvI4ToR8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvI4ToR8",
            returnType: "System.Double",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.r8", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ConvI4ToU4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ConvI4ToU4",
            returnType: "System.UInt32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: -1),
                ModelFactory.Instruction("conv.u4", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    [Fact]
    public void ArithmeticChain_DeepExpression_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::DeepExpr",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 2),
                ModelFactory.Instruction("add", ilOffset: 2),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 3),
                ModelFactory.Instruction("mul", ilOffset: 4),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 4),
                ModelFactory.Instruction("sub", ilOffset: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 7, intOperand: 5),
                ModelFactory.Instruction("div", ilOffset: 8),
                ModelFactory.Instruction("ret", ilOffset: 9),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    public void Dispose()
    {
    }
}
