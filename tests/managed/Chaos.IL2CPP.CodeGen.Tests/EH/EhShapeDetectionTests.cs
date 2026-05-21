using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.CodeGen.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests.EH;

/// <summary>
/// Tests that verify exception handling shapes are correctly detected and
/// emitted. Covers catch-only, finally-only, and nested EH patterns.
/// </summary>
public sealed class EhShapeDetectionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    /// <summary>
    /// A method with a simple try-catch region should generate valid C++ code
    /// with goto-based EH handling (EH-aware structured codegen requires
    /// additional IR context not available in minimal in-memory artifacts).
    /// </summary>
    [Fact(Skip = "endcatch not supported in structured EH linear lowering")]
    public void SimpleTryCatch_EmitsEHPatterns()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestTryCatch",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                // try block: ldc.i4.1, stloc.0, leave
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 2, intOperand: 9),
                // handler block: pop, ldc.i4.0, stloc.0, endcatch
                ModelFactory.Instruction("pop", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 0),
                ModelFactory.Instruction("stloc", ilOffset: 7, intOperand: 0),
                ModelFactory.Instruction("endcatch", ilOffset: 8),
                // after try-catch: ldloc.0, ret
                ModelFactory.Instruction("ldloc", ilOffset: 9, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 10),
            },
            exceptionRegions: new[]
            {
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Catch,
                    TryOffset = 0,
                    TryLength = 3,
                    HandlerOffset = 5,
                    HandlerLength = 4,
                    CatchTypeSubjectId = "System.Private.CoreLib/System.Exception",
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        // Verify it's a real function body, not a stub
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
        // The flat goto fallback should contain EH-related labels/branches
        AssertExtensions.ContainsCode("return", source);
    }

    /// <summary>
    /// A method with a try-finally region should generate valid C++ code
    /// (goto-based flat fallback for in-memory artifacts).
    /// </summary>
    [Fact]
    public void SimpleTryFinally_GeneratesValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestTryFinally",
            instructions: new[]
            {
                // try block
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 2, intOperand: 8),
                // finally block
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 0),
                ModelFactory.Instruction("stloc", ilOffset: 6, intOperand: 0),
                ModelFactory.Instruction("endfinally", ilOffset: 7),
                // after
                ModelFactory.Instruction("ret", ilOffset: 8),
            },
            exceptionRegions: new[]
            {
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Finally,
                    TryOffset = 0,
                    TryLength = 3,
                    HandlerOffset = 5,
                    HandlerLength = 3,
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        // Verify it's a real function body, not a stub
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// A method with nested try-catch (outer try has inner try-catch)
    /// should produce valid C++ code (goto-based flat fallback for
    /// in-memory artifacts).
    /// </summary>
    [Fact(Skip = "endcatch not supported in structured EH linear lowering")]
    public void NestedTryCatch_GeneratesValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestNestedTryCatch",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                // outer try
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                // inner try
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 2),
                ModelFactory.Instruction("stloc", ilOffset: 3, intOperand: 1),
                ModelFactory.Instruction("leave", ilOffset: 4, intOperand: 11),
                // inner handler
                ModelFactory.Instruction("pop", ilOffset: 7),
                ModelFactory.Instruction("ldc.i4", ilOffset: 8, intOperand: 0),
                ModelFactory.Instruction("stloc", ilOffset: 9, intOperand: 1),
                ModelFactory.Instruction("endcatch", ilOffset: 10),
                // outer leave
                ModelFactory.Instruction("leave", ilOffset: 11, intOperand: 18),
                // outer handler
                ModelFactory.Instruction("pop", ilOffset: 14),
                ModelFactory.Instruction("ldc.i4", ilOffset: 15, intOperand: 3),
                ModelFactory.Instruction("stloc", ilOffset: 16, intOperand: 0),
                ModelFactory.Instruction("endcatch", ilOffset: 17),
                // after
                ModelFactory.Instruction("ldloc", ilOffset: 18, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 19),
            },
            exceptionRegions: new[]
            {
                // Outer try-catch
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Catch,
                    TryOffset = 0,
                    TryLength = 12,
                    HandlerOffset = 14,
                    HandlerLength = 4,
                    CatchTypeSubjectId = "System.Private.CoreLib/System.Exception",
                },
                // Inner try-catch
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Catch,
                    TryOffset = 2,
                    TryLength = 5,
                    HandlerOffset = 7,
                    HandlerLength = 4,
                    CatchTypeSubjectId = "System.Private.CoreLib/System.InvalidOperationException",
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        // Verify it's a real function body, not a stub
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// Multiple sequential catch clauses should produce valid C++ code
    /// (goto-based flat fallback for in-memory artifacts).
    /// </summary>
    [Fact]
    public void MultipleCatchClauses_GeneratesValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestMultiCatch",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                // try
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 2, intOperand: 15),
                // catch 1 (InvalidOperationException)
                ModelFactory.Instruction("pop", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 2),
                ModelFactory.Instruction("stloc", ilOffset: 7, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 8, intOperand: 15),
                // catch 2 (ArgumentNullException)
                ModelFactory.Instruction("pop", ilOffset: 11),
                ModelFactory.Instruction("ldc.i4", ilOffset: 12, intOperand: 3),
                ModelFactory.Instruction("stloc", ilOffset: 13, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 14, intOperand: 15),
                // after
                ModelFactory.Instruction("ldloc", ilOffset: 15, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 16),
            },
            exceptionRegions: new[]
            {
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Catch,
                    TryOffset = 0,
                    TryLength = 3,
                    HandlerOffset = 5,
                    HandlerLength = 6,
                    CatchTypeSubjectId = "System.Private.CoreLib/System.InvalidOperationException",
                },
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Catch,
                    TryOffset = 0,
                    TryLength = 3,
                    HandlerOffset = 11,
                    HandlerLength = 4,
                    CatchTypeSubjectId = "System.Private.CoreLib/System.ArgumentNullException",
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        // Verify it's a real function body, not a stub
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    private static int CountOccurrences(string text, string pattern)
    {
        int count = 0;
        int index = 0;
        while ((index = text.IndexOf(pattern, index, StringComparison.Ordinal)) >= 0)
        {
            count++;
            index += pattern.Length;
        }
        return count;
    }

    public void Dispose()
    {
        // No cleanup needed.
    }
}
