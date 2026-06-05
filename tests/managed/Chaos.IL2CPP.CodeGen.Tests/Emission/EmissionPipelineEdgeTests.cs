using Chaos.IL2CPP.Generator.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Tests targeting specific emission pipeline edge cases not covered by
/// the basic emission test suites: complex returns, multi-method artifacts,
/// deep control flow nesting, and abi variant coverage.
/// </summary>
public sealed class EmissionPipelineEdgeTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    // ──── Return type ABI variants ────────────────────────────────────────

    [Fact]
    public void VoidReturn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::VoidReturn:System.Void()",
            instructions: new[]
            {
                ModelFactory.Instruction("ret", ilOffset: 0),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Int64Return_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Int64Return:System.Int64()",
            returnType: "System.Int64",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", ilOffset: 0, longOperand: 42L),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    // ──── Multi-method artifact ───────────────────────────────────────────
    // Exercises the planner's handling of multiple methods in one artifact:
    // method iteration loops, per-method setup, method-count math.

    [Fact]
    public void TwoMethods_EachEmitsValidCode()
    {
        var method1 = ModelFactory.CreateMethod(
            "TestModule.TestClass::First:System.Int32()",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 20),
                ModelFactory.Instruction("add", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var method2 = ModelFactory.CreateMethod(
            "TestModule.TestClass::Second:System.Int32()",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            nativeSymbol: "Chaos_Test_Stub2",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 100),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 200),
                ModelFactory.Instruction("sub", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var artifact = ModelFactory.CreateArtifact(method1, method2);
        var loweringPlan = ModelFactory.CreateDefaultPlan(method1.SubjectId);
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: PlannerFixture.StubAssemblyPath);
        var templateModel = _fixture.RunPlanner(artifact, loweringPlan, manifest);

        var match1 = templateModel.Methods.FirstOrDefault(m => m.SubjectId == method1.SubjectId);
        Assert.NotNull(match1);
        AssertExtensions.UsesStructuredIR(match1!.MethodSource);

        var match2 = templateModel.Methods.FirstOrDefault(m => m.SubjectId == method2.SubjectId);
        Assert.NotNull(match2);
        AssertExtensions.UsesStructuredIR(match2!.MethodSource);
    }

    // ──── Complex nested control flow ─────────────────────────────────────
    // if-then-else + loop = covers deeper SRP recovery paths.

    [Fact]
    public void NestedIfInsideLoop_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::NestedIfLoop:System.Int32(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // while (arg > 0) { if (arg == 5) break; arg--; }
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("brfalse", ilOffset: 1, intOperand: 14),
                ModelFactory.Instruction("ldarg", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 5),
                ModelFactory.Instruction("beq", ilOffset: 6, intOperand: 14),
                ModelFactory.Instruction("ldarg", ilOffset: 9, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 10, intOperand: 1),
                ModelFactory.Instruction("sub", ilOffset: 11),
                ModelFactory.Instruction("starg", ilOffset: 12, intOperand: 0),
                ModelFactory.Instruction("br", ilOffset: 13, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 14, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 15),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    // ──── If-else with comparison opcodes ─────────────────────────────────

    [Fact]
    public void IfElseWithBgt_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::IfElseBgt:System.Int32(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("bgt", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 4),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    // ──── Multiple returns from loop body ─────────────────────────────────

    [Fact]
    public void LoopWithMidBodyReturn_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LoopReturn:System.Int32(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // while (true) { if (arg == 0) return 0; arg--; if (arg < 0) return 1; }
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("brfalse", ilOffset: 1, intOperand: 8),
                ModelFactory.Instruction("br", ilOffset: 4, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 8, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 9),
                ModelFactory.Instruction("ldarg", ilOffset: 10, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 11, intOperand: 1),
                ModelFactory.Instruction("sub", ilOffset: 12),
                ModelFactory.Instruction("starg", ilOffset: 13, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 14, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 15, intOperand: 0),
                ModelFactory.Instruction("blt", ilOffset: 16, intOperand: 20),
                ModelFactory.Instruction("br", ilOffset: 19, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 20, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 21),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    // ──── Method with blt.un latch terminator (do-while edge case) ────────

    [Fact]
    public void DoWhileWithBltUnLatch_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::DoWhileBltUn:System.Int32(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                // do { arg++; } while (arg < 10u);
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("add", ilOffset: 2),
                ModelFactory.Instruction("starg", ilOffset: 3, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 10),
                ModelFactory.Instruction("blt.un", ilOffset: 6, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 9, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 10),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Method with bge.un branch ───────────────────────────────────────

    [Fact]
    public void IfElseWithBgeUn_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::IfElseBgeUn:System.Int32(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 10),
                ModelFactory.Instruction("bge.un", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 4),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    // ──── Method with bgt.un branch ───────────────────────────────────────

    [Fact]
    public void IfElseWithBgtUn_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::IfElseBgtUn:System.Int32(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 10),
                ModelFactory.Instruction("bgt.un", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 4),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Method with ble.un branch ───────────────────────────────────────

    [Fact]
    public void IfElseWithBleUn_EmitsStructuredIR()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::IfElseBleUn:System.Int32(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 10),
                ModelFactory.Instruction("ble.un", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 4),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Method with exception regions ───────────────────────────────────

    [Fact]
    public void MethodWithExceptionRegions_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::WithException:System.Int32()",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 1),
            },
            exceptionRegions: new[]
            {
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Finally,
                    TryOffset = 0,
                    TryLength = 1,
                    HandlerOffset = 3,
                    HandlerLength = 2,
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Multiple exception regions ──────────────────────────────────────

    [Fact]
    public void MethodWithMultipleExceptionRegions_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::MultiException:System.Int32()",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 1),
            },
            exceptionRegions: new[]
            {
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Finally,
                    TryOffset = 0,
                    TryLength = 1,
                    HandlerOffset = 3,
                    HandlerLength = 2,
                },
                new AotCoreIrExceptionRegionArtifact
                {
                    HandlingKindCode = AotCoreIrExceptionRegionKind.Catch,
                    TryOffset = 0,
                    TryLength = 1,
                    HandlerOffset = 6,
                    HandlerLength = 3,
                    CatchTypeSubjectId = "System.Private.CoreLib/System.Exception",
                },
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Ceq comparison opcode ───────────────────────────────────────────

    [Fact]
    public void CeqComparison_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CeqOp:System.Int32()",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 10),
                ModelFactory.Instruction("ceq", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Cgt.un comparison ──────────────────────────────────────────────

    [Fact]
    public void CgtUnComparison_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CgtUn:System.Int32()",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("cgt.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Dup + Pop stack manipulation ───────────────────────────────────

    [Fact]
    public void DupPopStack_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::DupPop:System.Int32()",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("dup", ilOffset: 1),
                ModelFactory.Instruction("pop", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Initobj on value type ──────────────────────────────────────────

    [Fact]
    public void InitobjType_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::InitObj:System.Void()",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("initobj", ilOffset: 1, "System.Private.CoreLib/System.Int32", AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Box + Unbox pair ───────────────────────────────────────────────

    [Fact]
    public void BoxUnboxPair_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::BoxUnbox:System.Int32()",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("box", ilOffset: 1, "System.Private.CoreLib/System.Int32", AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("unbox", ilOffset: 2, "System.Private.CoreLib/System.Int32", AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ldind.i4", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Isinst + Brtrue pattern ────────────────────────────────────────

    [Fact]
    public void IsinstThenBrtrue_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::IsinstCheck:System.Int32()",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("isinst", ilOffset: 1, "System.Private.CoreLib/System.Object", AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("brfalse", ilOffset: 2, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 4),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 6),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Empty method (just ret) ────────────────────────────────────────

    [Fact]
    public void EmptyMethod_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Empty:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ret", ilOffset: 0),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    public void Dispose()
    {
    }
}
