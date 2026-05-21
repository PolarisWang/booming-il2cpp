using Chaos.IL2CPP.CodeGen.Planning;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests;

public sealed class InliningPlannerTests
{
    // ── EstimateInstructionWeight ───────────────────────────────────────

    [Theory]
    [InlineData("call", 3)]
    [InlineData("callvirt", 3)]
    [InlineData("calli", 4)]
    [InlineData("cpblk", 4)]
    [InlineData("initblk", 4)]
    [InlineData("throw", 5)]
    [InlineData("rethrow", 5)]
    [InlineData("switch", 3)]
    [InlineData("newobj", 2)]
    [InlineData("box", 2)]
    [InlineData("unbox", 2)]
    [InlineData("ldelem", 2)]
    [InlineData("stelem", 2)]
    [InlineData("ldelema", 2)]
    [InlineData("ldfld", 1)]
    [InlineData("stfld", 1)]
    [InlineData("ldsfld", 1)]
    [InlineData("stsfld", 1)]
    [InlineData("nop", 1)]
    [InlineData("ret", 1)]
    [InlineData("add", 1)]
    [InlineData("ldc.i4", 1)]
    public void EstimateInstructionWeight_ReturnsExpectedWeight(string opCode, int expected)
    {
        var result = InliningPlanner.EstimateInstructionWeight(opCode);
        Assert.Equal(expected, result);
    }

    // ── EvaluateInline ──────────────────────────────────────────────────

    [Fact]
    public void EvaluateInline_Recursive_ReturnsFalse()
    {
        var result = InliningPlanner.EvaluateInline(
            calleeInstructionCount: 5,
            callerInstructionCount: 20,
            isRecursive: true);
        Assert.False(result.CanInline);
        Assert.Contains("recursive", result.Reason);
    }

    [Fact]
    public void EvaluateInline_ExceedsMaxCount_ReturnsFalse()
    {
        var result = InliningPlanner.EvaluateInline(
            calleeInstructionCount: InliningPlanner.kMaxInlineInstructionCount + 1,
            callerInstructionCount: 100,
            isRecursive: false);
        Assert.False(result.CanInline);
        Assert.Contains("max inline count", result.Reason);
    }

    [Fact]
    public void EvaluateInline_ExceedsCallerFraction_ReturnsFalse()
    {
        var result = InliningPlanner.EvaluateInline(
            calleeInstructionCount: 10,
            callerInstructionCount: 20,
            isRecursive: false);
        Assert.False(result.CanInline);
        Assert.Contains("ratio", result.Reason);
    }

    [Fact]
    public void EvaluateInline_WithinBudget_ReturnsTrue()
    {
        var result = InliningPlanner.EvaluateInline(
            calleeInstructionCount: 3,
            callerInstructionCount: 100,
            isRecursive: false);
        Assert.True(result.CanInline);
        Assert.Contains("within budget", result.Reason);
    }

    [Fact]
    public void EvaluateInline_ZeroCallerInstructionCount_DoesNotDivideByZero()
    {
        var result = InliningPlanner.EvaluateInline(
            calleeInstructionCount: 1,
            callerInstructionCount: 0,
            isRecursive: false);
        // fraction = 1/max(0,1) = 1.0 which exceeds 0.30, so CanInline is false
        Assert.False(result.CanInline);
    }

    // ── ClassifySection ─────────────────────────────────────────────────

    [Theory]
    [InlineData(200, ".text$hot")]
    [InlineData(100, ".text$hot")]
    [InlineData(50, ".text$warm")]
    [InlineData(11, ".text$warm")]
    [InlineData(10, ".text$cold")]
    [InlineData(0, ".text$cold")]
    [InlineData(-1, ".text$cold")]
    public void ClassifySection_ReturnsExpectedSection(int estimatedCallCount, string expected)
    {
        var result = InliningPlanner.ClassifySection(estimatedCallCount);
        Assert.Equal(expected, result);
    }

    // ── ClassifySectionByStaticHeuristic ────────────────────────────────

    [Fact]
    public void ClassifySectionByStaticHeuristic_EntryPoint_ReturnsHot()
    {
        var result = InliningPlanner.ClassifySectionByStaticHeuristic(
            "Test.Class::Method", isEntryPoint: true, instructionCount: 100);
        Assert.Equal(".text$hot", result);
    }

    [Fact]
    public void ClassifySectionByStaticHeuristic_SmallMethod_ReturnsWarm()
    {
        var result = InliningPlanner.ClassifySectionByStaticHeuristic(
            "Test.Class::Small", isEntryPoint: false, instructionCount: 5);
        Assert.Equal(".text$warm", result);
    }

    [Fact]
    public void ClassifySectionByStaticHeuristic_LargeMethod_ReturnsCold()
    {
        var result = InliningPlanner.ClassifySectionByStaticHeuristic(
            "Test.Class::Large", isEntryPoint: false, instructionCount: 100);
        Assert.Equal(".text$cold", result);
    }

    // ── GetSectionPragma ────────────────────────────────────────────────

    [Theory]
    [InlineData("", "")]
    [InlineData(".text", "")]
    [InlineData(".text$hot", "#pragma code_seg(\".text$hot\")")]
    [InlineData(".text$cold", "#pragma code_seg(\".text$cold\")")]
    public void GetSectionPragma_ReturnsExpected(string sectionName, string expected)
    {
        var result = InliningPlanner.GetSectionPragma(sectionName);
        Assert.Equal(expected, result);
    }

    // ── GetSectionAttribute ─────────────────────────────────────────────

    [Theory]
    [InlineData(".text$hot", " __attribute__((hot))")]
    [InlineData(".text$cold", " __attribute__((cold))")]
    [InlineData(".text$warm", "")]
    [InlineData(".text", "")]
    [InlineData("", "")]
    public void GetSectionAttribute_ReturnsExpected(string sectionName, string expected)
    {
        var result = InliningPlanner.GetSectionAttribute(sectionName);
        Assert.Equal(expected, result);
    }
}
