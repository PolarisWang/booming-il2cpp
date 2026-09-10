using Chaos.IL2CPP.Generator.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Tests that exercise structured control flow recovery:
/// CFG building, block splitting, loop detection, and structured IR emission.
/// The planner should recover structured patterns (while, do-while, if-then-else)
/// from reducible CFGs and fall back to flat goto for irreducible ones.
/// </summary>
public sealed class ControlFlowEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    /// <summary>
    /// A simple if-then pattern: brfalse over a block of instructions.
    /// </summary>
    [Fact]
    public void IfThenPattern_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::IfThen",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("brfalse", ilOffset: 1, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    /// <summary>
    /// An if-then-else pattern: brfalse with two distinct branches.
    /// </summary>
    [Fact]
    public void IfThenElsePattern_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::IfThenElse",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("brfalse", ilOffset: 1, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 10),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 20),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    /// <summary>
    /// A while-loop pattern: conditional branch back to loop header.
    /// </summary>
    [Fact]
    public void WhileLoop_EmitsStructuredLoop()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CountDown",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // while (arg > 0) { arg--; }
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("brfalse", ilOffset: 1, intOperand: 7),
                ModelFactory.Instruction("ldarg", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 1),
                ModelFactory.Instruction("sub", ilOffset: 6),
                ModelFactory.Instruction("starg", ilOffset: 7, intOperand: 0),
                ModelFactory.Instruction("br", ilOffset: 8, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 11, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 12),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        // Loops produce structured IR
        AssertExtensions.UsesStructuredIR(source);
    }

    /// <summary>
    /// A do-while loop: body executes at least once before condition check.
    /// </summary>
    [Fact]
    public void DoWhileLoop_EmitsStructuredLoop()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CountUp",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // do { arg++; } while (arg < 10);
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("add", ilOffset: 2),
                ModelFactory.Instruction("starg", ilOffset: 3, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 10),
                ModelFactory.Instruction("blt", ilOffset: 6, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 9, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 10),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    /// <summary>
    /// Multiple conditional branches creating a switch-like pattern.
    /// </summary>
    [Fact]
    public void CascadingBranches_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Cascade",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("brfalse", ilOffset: 1, intOperand: 8),
                ModelFactory.Instruction("ldarg", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 1),
                ModelFactory.Instruction("beq", ilOffset: 6, intOperand: 10),
                ModelFactory.Instruction("br", ilOffset: 7, intOperand: 12),
                ModelFactory.Instruction("ldc.i4", ilOffset: 8, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 9),
                ModelFactory.Instruction("ldc.i4", ilOffset: 10, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 11),
                ModelFactory.Instruction("ldc.i4", ilOffset: 12, intOperand: 2),
                ModelFactory.Instruction("ret", ilOffset: 13),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// Multiple returns from different blocks produce structured IR.
    /// </summary>
    [Fact]
    public void MultipleReturns_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::MultiReturn",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("brfalse", ilOffset: 1, intOperand: 11),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldarg", ilOffset: 6, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 7, intOperand: 0),
                ModelFactory.Instruction("bgt", ilOffset: 8, intOperand: 13),
                ModelFactory.Instruction("ldc.i4", ilOffset: 11, intOperand: 2),
                ModelFactory.Instruction("ret", ilOffset: 12),
                ModelFactory.Instruction("ldc.i4", ilOffset: 13, intOperand: 3),
                ModelFactory.Instruction("ret", ilOffset: 14),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    public void Dispose()
    {
    }
}
