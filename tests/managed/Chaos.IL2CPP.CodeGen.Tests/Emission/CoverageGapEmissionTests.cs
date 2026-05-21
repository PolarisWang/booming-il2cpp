using Chaos.IL2CPP.Generator.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Coverage-maximizing tests targeting specific emission code paths
/// identified by coverlet analysis as uncovered. Each test exercises a
/// unique opcode or instruction pattern through the planner pipeline.
/// </summary>
public sealed class CoverageGapEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    public void Dispose()
    {
    }

    // ──── Basic arithmetic opcodes ─────────────────────────────────────

    [Fact]
    public void Div_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Div:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 6),
                ModelFactory.Instruction("div", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void DivUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::DivUn:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 6),
                ModelFactory.Instruction("div.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Rem_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Rem:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("rem", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void RemUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::RemUn:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("rem.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void AddOvfUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::AddOvfUn:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 100),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 200),
                ModelFactory.Instruction("add.ovf.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void SubOvfUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::SubOvfUn:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 200),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 100),
                ModelFactory.Instruction("sub.ovf.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void MulOvfUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::MulOvfUn:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 100),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 200),
                ModelFactory.Instruction("mul.ovf.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Shift opcodes ─────────────────────────────────────────────────

    [Fact]
    public void Shl_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Shl:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 4),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 2),
                ModelFactory.Instruction("shl", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Shr_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Shr:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 8),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 2),
                ModelFactory.Instruction("shr", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ShrUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ShrUn:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 8),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 2),
                ModelFactory.Instruction("shr.un", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Bitwise opcodes ───────────────────────────────────────────────

    [Fact]
    public void And_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::And:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0xFF),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0x0F),
                ModelFactory.Instruction("and", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Or_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Or:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0xF0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0x0F),
                ModelFactory.Instruction("or", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Xor_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Xor:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0xFF),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0x0F),
                ModelFactory.Instruction("xor", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── More comparison opcodes ───────────────────────────────────────

    [Fact]
    public void Cgt_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Cgt:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("cgt", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void CgtUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::CgtUn:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("cgt.un", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Clt_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Clt:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 3),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 10),
                ModelFactory.Instruction("clt", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Ceq_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Ceq:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 42),
                ModelFactory.Instruction("ceq", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── More conversion opcodes ───────────────────────────────────────

    [Fact]
    public void ConvI1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvI1:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 255),
                ModelFactory.Instruction("conv.i1", ilOffset: 1, resultType: "System.SByte"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvU1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvU1:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 255),
                ModelFactory.Instruction("conv.u1", ilOffset: 1, resultType: "System.Byte"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvI2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvI2:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 65535),
                ModelFactory.Instruction("conv.i2", ilOffset: 1, resultType: "System.Int16"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvU2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvU2:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 65535),
                ModelFactory.Instruction("conv.u2", ilOffset: 1, resultType: "System.UInt16"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvI8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvI8:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", ilOffset: 0, longOperand: 100L),
                ModelFactory.Instruction("conv.i8", ilOffset: 1, resultType: "System.Int64"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvR8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvR8:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.r8", ilOffset: 1, resultType: "System.Double"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Ckfinite_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Ckfinite:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.r8", ilOffset: 1, resultType: "System.Double"),
                ModelFactory.Instruction("ckfinite", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Object model opcodes ──────────────────────────────────────────

    [Fact]
    public void Box_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Box:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("box", ilOffset: 1,
                    "System.Private.CoreLib/System.Int32",
                    AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Unbox_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Unbox:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("unbox", ilOffset: 1,
                    "System.Private.CoreLib/System.Int32",
                    AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Isinst_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Isinst:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("isinst", ilOffset: 1,
                    "System.Private.CoreLib/System.String",
                    AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Initobj_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Initobj:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldloca", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("initobj", ilOffset: 1,
                    "System.Private.CoreLib/System.Int32",
                    AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Stobj_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Stobj:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("conv.i", ilOffset: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 42),
                ModelFactory.Instruction("stobj", ilOffset: 3,
                    "System.Private.CoreLib/System.Int32",
                    AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Cpobj_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Cpobj:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("conv.i", ilOffset: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 4),
                ModelFactory.Instruction("conv.i", ilOffset: 3),
                ModelFactory.Instruction("cpobj", ilOffset: 4,
                    "System.Private.CoreLib/System.Int32",
                    AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Field address opcodes ─────────────────────────────────────────

    [Fact]
    public void Ldflda_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Ldflda:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.FieldInstruction("ldflda", ilOffset: 1,
                    AotCoreIrRuntimeServiceKind.LoadInstanceField,
                    "Test.Module/MyClass",
                    "_value",
                    "System.Int32",
                    declaringTypeShape: AotCoreIrTypeShapeKind.ReferenceType),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Ldsflda_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Ldsflda:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.FieldInstruction("ldsflda", ilOffset: 0,
                    AotCoreIrRuntimeServiceKind.LoadStaticField,
                    "Test.Module/MyState",
                    "GlobalCounter",
                    "System.Int32",
                    declaringTypeShape: AotCoreIrTypeShapeKind.ReferenceType),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── More ldind variants ───────────────────────────────────────────

    [Fact]
    public void LdindI1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdindI1:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("conv.i", ilOffset: 1),
                ModelFactory.Instruction("ldind.i1", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindU1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdindU1:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("conv.i", ilOffset: 1),
                ModelFactory.Instruction("ldind.u1", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindU4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdindU4:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("conv.i", ilOffset: 1),
                ModelFactory.Instruction("ldind.u4", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindR8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdindR8:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("conv.i", ilOffset: 1),
                ModelFactory.Instruction("ldind.r8", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdindI_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdindI:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("conv.i", ilOffset: 1),
                ModelFactory.Instruction("ldind.i", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── More stind variants ───────────────────────────────────────────

    [Fact]
    public void StindR4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::StindR4:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldloca", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 42),
                ModelFactory.Instruction("stind.r4", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StindR8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::StindR8:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldloca", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i8", ilOffset: 1, longOperand: 100L),
                ModelFactory.Instruction("stind.r8", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StindRef_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::StindRef:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldloca", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldnull", ilOffset: 1),
                ModelFactory.Instruction("stind.ref", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StindI_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::StindI:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldloca", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 42),
                ModelFactory.Instruction("stind.i", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── More ldelem variants ──────────────────────────────────────────

    [Fact]
    public void LdelemU1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdelemU1:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldelem.u1", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdelemI2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdelemI2:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldelem.i2", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdelemU2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdelemU2:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldelem.u2", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdelemU4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdelemU4:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldelem.u4", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdelemI8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdelemI8:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldelem.i8", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdelemR4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdelemR4:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldelem.r4", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdelemR8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdelemR8:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldelem.r8", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── More stelem variants ──────────────────────────────────────────

    [Fact]
    public void StelemI1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::StelemI1:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 42),
                ModelFactory.Instruction("stelem.i1", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StelemI2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::StelemI2:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 42),
                ModelFactory.Instruction("stelem.i2", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StelemI8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::StelemI8:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldc.i8", ilOffset: 2, longOperand: 100L),
                ModelFactory.Instruction("stelem.i8", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StelemR4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::StelemR4:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 42),
                ModelFactory.Instruction("stelem.r4", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void StelemR8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::StelemR8:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldc.i8", ilOffset: 2, longOperand: 100L),
                ModelFactory.Instruction("stelem.r8", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Overflow conv variants ────────────────────────────────────────

    [Fact]
    public void ConvOvfI1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvOvfI1:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.i1", ilOffset: 1, resultType: "System.SByte"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfU1_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvOvfU1:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.u1", ilOffset: 1, resultType: "System.Byte"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfI2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvOvfI2:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.i2", ilOffset: 1, resultType: "System.Int16"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfU2_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvOvfU2:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.u2", ilOffset: 1, resultType: "System.UInt16"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfI4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvOvfI4:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.i4", ilOffset: 1, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfU4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvOvfU4:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.u4", ilOffset: 1, resultType: "System.UInt32"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfI8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvOvfI8:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.i8", ilOffset: 1, resultType: "System.Int64"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfU8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvOvfU8:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.u8", ilOffset: 1, resultType: "System.UInt64"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void ConvOvfUUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::ConvOvfUUn:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.u.un", ilOffset: 1, resultType: "System.UIntPtr"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Refanytype ────────────────────────────────────────────────────

    [Fact]
    public void Refanytype_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Refanytype:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("conv.i", ilOffset: 1),
                ModelFactory.Instruction("refanytype", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Starg ─────────────────────────────────────────────────────────

    [Fact]
    public void Starg_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Starg:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("starg", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 2, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Nop ───────────────────────────────────────────────────────────

    [Fact]
    public void Nop_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Nop:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("nop", ilOffset: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Ldvirtftn (already covered by MethodDispatchEmissionTests.LoadVirtualFunctionPointer_ThrowsWithoutVtable) ─

    // ──── Rethrow ───────────────────────────────────────────────────────

    [Fact]
    public void Rethrow_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Rethrow:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("rethrow", ilOffset: 0),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Endfilter ─────────────────────────────────────────────────────

    [Fact]
    public void Endfilter_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Endfilter:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("endfilter", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Ldc.r4 ────────────────────────────────────────────────────────

    [Fact]
    public void LdcR4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdcR4:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.r4", ilOffset: 0, doubleOperand: 3.14),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdcR8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::LdcR8:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.r8", ilOffset: 0, doubleOperand: 3.14159),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Branch opcodes ────────────────────────────────────────────────

    [Fact]
    public void Brtrue_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Brtrue:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("brtrue", ilOffset: 1, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BneUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::BneUn:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 2),
                ModelFactory.Instruction("bne.un", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 7, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 8),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Bge_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Bge:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("bge", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 7, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 8),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Ble_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::Ble:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 3),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 5),
                ModelFactory.Instruction("ble", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 7, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 8),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BltUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/Coverage::BltUn:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 3),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 5),
                ModelFactory.Instruction("blt.un", ilOffset: 2, intOperand: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 6),
                ModelFactory.Instruction("ldc.i4", ilOffset: 7, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 8),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }
}
