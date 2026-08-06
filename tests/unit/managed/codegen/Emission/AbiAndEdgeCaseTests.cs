using Chaos.IL2CPP.Generator.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Tests covering various ABI carrier kinds and edge-case patterns
/// in StructuredIR, ObjectModelUtilities, and other emission files.
/// </summary>
public sealed class AbiAndEdgeCaseTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    public static IEnumerable<object[]> UncommonAbiData()
    {
        // (returnAbi, returnType)
        yield return new object[] { Abi(AotCoreIrAbiCarrierKind.Int8, "System.SByte"), "System.SByte" };
        yield return new object[] { Abi(AotCoreIrAbiCarrierKind.UInt8, "System.Byte"), "System.Byte" };
        yield return new object[] { Abi(AotCoreIrAbiCarrierKind.Int16, "System.Int16"), "System.Int16" };
        yield return new object[] { Abi(AotCoreIrAbiCarrierKind.UInt16, "System.UInt16"), "System.UInt16" };
        yield return new object[] { Abi(AotCoreIrAbiCarrierKind.Float32, "System.Single"), "System.Single" };
        yield return new object[] { Abi(AotCoreIrAbiCarrierKind.Float64, "System.Double"), "System.Double" };
        yield return new object[] { Abi(AotCoreIrAbiCarrierKind.Int64, "System.Int64"), "System.Int64" };
        yield return new object[] { Abi(AotCoreIrAbiCarrierKind.UInt64, "System.UInt64"), "System.UInt64" };
        yield return new object[] { Abi(AotCoreIrAbiCarrierKind.NativeInt, "System.IntPtr"), "System.IntPtr" };
        yield return new object[] { Abi(AotCoreIrAbiCarrierKind.ByRef, "System.String&"), "System.String&" };
        yield return new object[] { Abi(AotCoreIrAbiCarrierKind.MultiReturn, "System.Void"), "System.Void" };
    }

    [Theory]
    [MemberData(nameof(UncommonAbiData))]
    public void UncommonAbiCarrierKind_EmitsValidCode(AotCoreIrAbiSlotArtifact returnAbi, string returnType)
    {
        var method = ModelFactory.CreateMethod(
            $"TestModule.TestClass::{returnType.Replace(".", "_").Replace("&", "Ref")}:{returnType}()",
            returnAbi: returnAbi,
            returnType: returnType,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ByRefToValueTypeAbi_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ByRefToValueType:TestType(System.Int32)",
            returnAbi: new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.ByRefToValueType,
                TypeSubjectId = "TestModule.MyValueType",
            },
            returnType: "TestModule.MyValueType",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void MultiReturnAbi_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::MultiReturn:TestType(System.Int32)",
            returnAbi: new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.MultiReturn,
                TypeSubjectId = "TestModule.MultiReturnType",
            },
            returnType: "TestModule.MultiReturnType",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ValueTypeByValueAbi_WithIntReturn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::VTByValue:TestType(System.Int32)",
            returnAbi: new AotCoreIrAbiSlotArtifact
            {
                CarrierKindCode = AotCoreIrAbiCarrierKind.ValueTypeByValue,
                TypeSubjectId = "TestModule.SomeStruct",
            },
            returnType: "TestModule.SomeStruct",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises EvalStackDiscard with count > 1 (line 259-262 in StructuredIR.cs).
    /// </summary>
    [Fact]
    public void MultiPop_TriggersEvalStackDiscardCount()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::MultiPop:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 20),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 30),
                ModelFactory.Instruction("add.ovf", ilOffset: 3, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// Exercises unconditional branch (br) targeting a later instruction
    /// (which triggers StructuredIR flow analysis).
    /// </summary>
    [Fact]
    public void UnconditionalBranch_EmitsStructuredControlFlow()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::UncondBranch:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("brfalse", ilOffset: 1, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 3),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 99),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    public void Dispose()
    {
    }

    private static AotCoreIrAbiSlotArtifact Abi(AotCoreIrAbiCarrierKind kind, string typeSubjectId)
        => new() { CarrierKindCode = kind, TypeSubjectId = typeSubjectId };
}
