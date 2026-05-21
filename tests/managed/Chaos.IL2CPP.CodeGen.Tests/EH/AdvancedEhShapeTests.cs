using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.CodeGen.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests.EH;

/// <summary>
/// Advanced EH shape tests covering:
/// - try-finally (simple and nested)
/// - try-catch-finally combinations
/// - filter exception handlers
/// - Multiple catch clauses with finally
/// These tests verify the planner correctly detects EH shapes
/// and generates valid C++ code (either structured C++ EH or
/// flat goto fallback for in-memory artifacts).
/// </summary>
public sealed class AdvancedEhShapeTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    /// <summary>
    /// A method with try-finally region should generate valid code.
    /// Test uses endfinally to end the finally block.
    /// </summary>
    [Fact]
    public void TryFinally_GeneratesValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestFinally",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 2, intOperand: 8),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 99),
                ModelFactory.Instruction("stloc", ilOffset: 6, intOperand: 0),
                ModelFactory.Instruction("endfinally", ilOffset: 7),
                ModelFactory.Instruction("ldloc", ilOffset: 8, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 9),
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
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// Multiple catch clauses with different exception types and a finally block.
    /// </summary>
    [Fact]
    public void MultipleCatchesWithFinally_GeneratesValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::MultiCatchFinally",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 2, intOperand: 18),
                // catch InvalidOperationException
                ModelFactory.Instruction("pop", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 2),
                ModelFactory.Instruction("stloc", ilOffset: 7, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 8, intOperand: 14),
                // catch ArgumentNullException
                ModelFactory.Instruction("pop", ilOffset: 11),
                ModelFactory.Instruction("ldc.i4", ilOffset: 12, intOperand: 3),
                ModelFactory.Instruction("stloc", ilOffset: 13, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 14, intOperand: 18),
                // finally
                ModelFactory.Instruction("ldc.i4", ilOffset: 17, intOperand: 0),
                ModelFactory.Instruction("stloc", ilOffset: 18, intOperand: 1),
                ModelFactory.Instruction("endfinally", ilOffset: 19),
                // after
                ModelFactory.Instruction("ldloc", ilOffset: 20, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 21),
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
                    HandlerLength = 6,
                    CatchTypeSubjectId = "System.Private.CoreLib/System.ArgumentNullException",
                },
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Finally,
                    TryOffset = 0,
                    TryLength = 3,
                    HandlerOffset = 17,
                    HandlerLength = 3,
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// Nested try-catch inside a try-finally block.
    /// </summary>
    [Fact]
    public void NestedTryCatchInFinally_GeneratesValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::NestedInFinally",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                // outer finally set-up
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                // inner try
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 3, intOperand: 1),
                ModelFactory.Instruction("leave", ilOffset: 4, intOperand: 11),
                // inner catch
                ModelFactory.Instruction("pop", ilOffset: 7),
                ModelFactory.Instruction("ldc.i4", ilOffset: 8, intOperand: 2),
                ModelFactory.Instruction("stloc", ilOffset: 9, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 10, intOperand: 14),
                // outer finally
                ModelFactory.Instruction("ldc.i4", ilOffset: 14, intOperand: -1),
                ModelFactory.Instruction("stloc", ilOffset: 15, intOperand: 0),
                ModelFactory.Instruction("endfinally", ilOffset: 16),
                // after
                ModelFactory.Instruction("ldloc", ilOffset: 17, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 18),
            },
            exceptionRegions: new[]
            {
                // Inner try-catch
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Catch,
                    TryOffset = 2,
                    TryLength = 5,
                    HandlerOffset = 7,
                    HandlerLength = 4,
                    CatchTypeSubjectId = "System.Private.CoreLib/System.Exception",
                },
                // Outer finally
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Finally,
                    TryOffset = 0,
                    TryLength = 14,
                    HandlerOffset = 14,
                    HandlerLength = 3,
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// Fault handler (try-fault) should produce valid code.
    /// </summary>
    [Fact]
    public void TryFault_GeneratesValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestFault",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 2, intOperand: 8),
                // fault handler
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 0),
                ModelFactory.Instruction("stloc", ilOffset: 6, intOperand: 0),
                ModelFactory.Instruction("endfinally", ilOffset: 7),
                // after
                ModelFactory.Instruction("ldloc", ilOffset: 8, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 9),
            },
            exceptionRegions: new[]
            {
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Fault,
                    TryOffset = 0,
                    TryLength = 3,
                    HandlerOffset = 5,
                    HandlerLength = 3,
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// Catch with a filter clause should produce valid code.
    /// </summary>
    [Fact]
    public void CatchWithFilter_GeneratesValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestFilter",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 2, intOperand: 14),
                // filter block
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 1),
                ModelFactory.Instruction("endfilter", ilOffset: 6),
                // handler block
                ModelFactory.Instruction("pop", ilOffset: 8),
                ModelFactory.Instruction("ldc.i4", ilOffset: 9, intOperand: 0),
                ModelFactory.Instruction("stloc", ilOffset: 10, intOperand: 0),
                ModelFactory.Instruction("endcatch", ilOffset: 11),
                // after
                ModelFactory.Instruction("ldloc", ilOffset: 14, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 15),
            },
            exceptionRegions: new[]
            {
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Filter,
                    TryOffset = 0,
                    TryLength = 3,
                    HandlerOffset = 8,
                    HandlerLength = 4,
                    FilterOffset = 5,
                    CatchTypeSubjectId = "System.Private.CoreLib/System.Exception",
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// Two sequential finally blocks protecting different try ranges.
    /// </summary>
    [Fact]
    public void SequentialFinally_GeneratesValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::SeqFinally",
            returnType: "System.Void",
            returnAbi: ModelFactory.VoidAbi,
            instructions: new[]
            {
                // first try-finally
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 2, intOperand: 8),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 0),
                ModelFactory.Instruction("stloc", ilOffset: 6, intOperand: 0),
                ModelFactory.Instruction("endfinally", ilOffset: 7),
                // second try-finally
                ModelFactory.Instruction("ldc.i4", ilOffset: 8, intOperand: 2),
                ModelFactory.Instruction("stloc", ilOffset: 9, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 10, intOperand: 16),
                ModelFactory.Instruction("ldc.i4", ilOffset: 13, intOperand: 0),
                ModelFactory.Instruction("stloc", ilOffset: 14, intOperand: 0),
                ModelFactory.Instruction("endfinally", ilOffset: 15),
                // ret
                ModelFactory.Instruction("ret", ilOffset: 16),
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
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Finally,
                    TryOffset = 8,
                    TryLength = 3,
                    HandlerOffset = 13,
                    HandlerLength = 3,
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// Empty handler (try with no instructions in handler).
    /// </summary>
    [Fact]
    public void EmptyCatchHandler_GeneratesValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::EmptyCatch",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("leave", ilOffset: 2, intOperand: 6),
                // empty catch handler (just rethrow/pop internally)
                ModelFactory.Instruction("pop", ilOffset: 5),
                ModelFactory.Instruction("leave", ilOffset: 6, intOperand: 9),
                // after
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
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    public void Dispose()
    {
    }
}
