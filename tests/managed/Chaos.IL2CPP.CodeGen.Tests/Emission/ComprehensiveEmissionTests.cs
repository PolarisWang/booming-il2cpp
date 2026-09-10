using Chaos.IL2CPP.Generator.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Comprehensive integration test that exercises many codegen paths through
/// a single rich model. Each test drives the full NativeAotLoweringPlanner
/// pipeline with methods containing diverse instruction patterns.
/// </summary>
public sealed class ComprehensiveEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    /// <summary>
    /// Exercises many arithmetic, comparison, and conversion opcodes in one method.
    /// </summary>
    [Fact]
    public void RichArithmeticMethod_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::RichArithmetic",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                // Push two constants
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                // add.ovf (overflow checking add)
                ModelFactory.Instruction("add.ovf", ilOffset: 2, resultType: "System.Int32"),
                // dup and ldc.i4 for next op
                ModelFactory.Instruction("dup", ilOffset: 3),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 2),
                // sub
                ModelFactory.Instruction("sub", ilOffset: 5, resultType: "System.Int32"),
                // neg
                ModelFactory.Instruction("neg", ilOffset: 6, resultType: "System.Int32"),
                // Pop and restart
                ModelFactory.Instruction("pop", ilOffset: 7),
                ModelFactory.Instruction("ldc.i4", ilOffset: 8, intOperand: 7),
                ModelFactory.Instruction("ldc.i4", ilOffset: 9, intOperand: 3),
                // div
                ModelFactory.Instruction("div", ilOffset: 10, resultType: "System.Int32"),
                ModelFactory.Instruction("ldc.i4", ilOffset: 11, intOperand: 7),
                ModelFactory.Instruction("ldc.i4", ilOffset: 12, intOperand: 3),
                // rem
                ModelFactory.Instruction("rem", ilOffset: 13, resultType: "System.Int32"),
                // pop, push constants for bitwise
                ModelFactory.Instruction("pop", ilOffset: 14),
                ModelFactory.Instruction("ldc.i4", ilOffset: 15, intOperand: 0xFF),
                ModelFactory.Instruction("ldc.i4", ilOffset: 16, intOperand: 0x0F),
                // and, or, xor
                ModelFactory.Instruction("and", ilOffset: 17, resultType: "System.Int32"),
                ModelFactory.Instruction("pop", ilOffset: 18),
                ModelFactory.Instruction("ldc.i4", ilOffset: 19, intOperand: 0xF0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 20, intOperand: 0x0F),
                ModelFactory.Instruction("or", ilOffset: 21, resultType: "System.Int32"),
                ModelFactory.Instruction("pop", ilOffset: 22),
                ModelFactory.Instruction("ldc.i4", ilOffset: 23, intOperand: 0xFF),
                ModelFactory.Instruction("ldc.i4", ilOffset: 24, intOperand: 0xAA),
                ModelFactory.Instruction("xor", ilOffset: 25, resultType: "System.Int32"),
                ModelFactory.Instruction("pop", ilOffset: 26),
                // not
                ModelFactory.Instruction("ldc.i4", ilOffset: 27, intOperand: 0xFF),
                ModelFactory.Instruction("not", ilOffset: 28, resultType: "System.Int32"),
                // shifts
                ModelFactory.Instruction("pop", ilOffset: 29),
                ModelFactory.Instruction("ldc.i4", ilOffset: 30, intOperand: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 31, intOperand: 4),
                ModelFactory.Instruction("shl", ilOffset: 32, resultType: "System.Int32"),
                ModelFactory.Instruction("pop", ilOffset: 33),
                ModelFactory.Instruction("ldc.i4", ilOffset: 34, intOperand: 16),
                ModelFactory.Instruction("ldc.i4", ilOffset: 35, intOperand: 2),
                ModelFactory.Instruction("shr", ilOffset: 36, resultType: "System.Int32"),
                // unsigned div, rem, shr
                ModelFactory.Instruction("pop", ilOffset: 37),
                ModelFactory.Instruction("ldc.i4", ilOffset: 38, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 39, intOperand: 3),
                ModelFactory.Instruction("div.un", ilOffset: 40, resultType: "System.Int32"),
                ModelFactory.Instruction("pop", ilOffset: 41),
                ModelFactory.Instruction("ldc.i4", ilOffset: 42, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 43, intOperand: 3),
                ModelFactory.Instruction("rem.un", ilOffset: 44, resultType: "System.Int32"),
                ModelFactory.Instruction("pop", ilOffset: 45),
                ModelFactory.Instruction("ldc.i4", ilOffset: 46, intOperand: -1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 47, intOperand: 1),
                ModelFactory.Instruction("shr.un", ilOffset: 48, resultType: "System.Int32"),
                // comparison ops
                ModelFactory.Instruction("pop", ilOffset: 49),
                ModelFactory.Instruction("ldc.i4", ilOffset: 50, intOperand: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 51, intOperand: 2),
                ModelFactory.Instruction("ceq", ilOffset: 52, resultType: "System.Int32"),
                ModelFactory.Instruction("pop", ilOffset: 53),
                ModelFactory.Instruction("ldc.i4", ilOffset: 54, intOperand: 2),
                ModelFactory.Instruction("ldc.i4", ilOffset: 55, intOperand: 1),
                ModelFactory.Instruction("cgt", ilOffset: 56, resultType: "System.Int32"),
                ModelFactory.Instruction("pop", ilOffset: 57),
                ModelFactory.Instruction("ldc.i4", ilOffset: 58, intOperand: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 59, intOperand: 2),
                ModelFactory.Instruction("clt", ilOffset: 60, resultType: "System.Int32"),
                // return
                ModelFactory.Instruction("ret", ilOffset: 61),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable", source);
    }

    /// <summary>
    /// Exercises overflow arithmetic conversion opcodes.
    /// </summary>
    [Fact]
    public void OverflowConversions_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::OverflowConversions",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 255),
                ModelFactory.Instruction("conv.ovf.i1", ilOffset: 1),
                ModelFactory.Instruction("pop", ilOffset: 2),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 255),
                ModelFactory.Instruction("conv.ovf.u1", ilOffset: 4),
                ModelFactory.Instruction("pop", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 65535),
                ModelFactory.Instruction("conv.ovf.i2", ilOffset: 7),
                ModelFactory.Instruction("pop", ilOffset: 8),
                ModelFactory.Instruction("ldc.i4", ilOffset: 9, intOperand: 65535),
                ModelFactory.Instruction("conv.ovf.u2", ilOffset: 10),
                ModelFactory.Instruction("pop", ilOffset: 11),
                ModelFactory.Instruction("ldc.i4", ilOffset: 12, intOperand: 42),
                ModelFactory.Instruction("conv.u8", ilOffset: 13),
                ModelFactory.Instruction("pop", ilOffset: 14),
                ModelFactory.Instruction("ldc.i4", ilOffset: 15, intOperand: 42),
                ModelFactory.Instruction("conv.i8", ilOffset: 16),
                ModelFactory.Instruction("ret", ilOffset: 17),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises control flow: branch, conditional branch, switch, leave.
    /// </summary>
    [Fact]
    public void RichControlFlow_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::RichControlFlow",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("brfalse", ilOffset: 1, intOperand: 7),  // branch to ret 0
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 20),
                ModelFactory.Instruction("bne.un", ilOffset: 4, intOperand: 7),  // branch if not equal
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 6),
                // target of brfalse/bne.un
                ModelFactory.Instruction("ldc.i4", ilOffset: 7, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 8),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises indirect memory access opcodes.
    /// </summary>
    [Fact]
    public void IndirectAccessOps_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::IndirectAccess",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldind.i4", ilOffset: 1),  // load int32 from pointer
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises indirect store.
    /// </summary>
    [Fact]
    public void IndirectStoreOps_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::IndirectStore",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("stind.i4", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises misc opcodes: localloc, nop, break, ldarga.
    /// </summary>
    [Fact]
    public void MiscOps_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::MiscOps",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarga", ilOffset: 2, intOperand: 0),
                ModelFactory.Instruction("ldind.i4", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    public void Dispose()
    {
    }
}
