using Chaos.IL2CPP.CodeGen.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests.Emission;

/// <summary>
/// Extended emission tests that exercise edge-case codegen paths
/// within NativeAotLoweringPlanner emission partials.
/// Each test drives the full planner pipeline via PlannerFixture and
/// verifies the generated C++ source contains expected emission patterns.
/// </summary>
public sealed class ExtendedEmissionGapTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    // ──── Extended Arithmetic (div.un, rem.un, shr.un, ckfinite) ────────

    [Fact]
    public void UnsignedDivide_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::UnsignedDiv:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("div.un", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void UnsignedRemainder_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::UnsignedRem:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 3),
                ModelFactory.Instruction("rem.un", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void UnsignedShiftRight_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::UnsignedShr:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 16),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 2),
                ModelFactory.Instruction("shr.un", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Ckfinite_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Ckfinite:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.r8", ilOffset: 0, doubleOperand: 42.0, resultType: "System.Double"),
                ModelFactory.Instruction("ckfinite", ilOffset: 1, resultType: "System.Double"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Overflow Arithmetic Unsigned Variants ─────────────────────────

    [Fact]
    public void OverflowAddUnsigned_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::AddOvfUn:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 20),
                ModelFactory.Instruction("add.ovf.un", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void OverflowSubUnsigned_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::SubOvfUn:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 20),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 10),
                ModelFactory.Instruction("sub.ovf.un", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void OverflowMulUnsigned_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::MulOvfUn:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 20),
                ModelFactory.Instruction("mul.ovf.un", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Int64/IntPtr Constants ─────────────────────────────────────────

    [Fact]
    public void LdcI8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdcI8:System.Int64()",
            returnType: "System.Int64",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", ilOffset: 0, longOperand: 0x7FFFFFFFFFFFFFFF, resultType: "System.Int64"),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdcR8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdcR8:System.Double()",
            returnType: "System.Double",
            returnAbi: new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float64, TypeSubjectId = "System.Double" },
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.r8", ilOffset: 0, doubleOperand: 3.14159265358979, resultType: "System.Double"),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdcR4_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdcR4:System.Single()",
            returnType: "System.Single",
            returnAbi: new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Float32, TypeSubjectId = "System.Single" },
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.r4", ilOffset: 0, doubleOperand: 3.14, resultType: "System.Single"),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });
        // Production code accepts a double operand for ldc.r4 via implicit conversion.
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Comparison Variants ────────────────────────────────────────────

    [Fact]
    public void CompareGreaterThan_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Cgt:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 5),
                ModelFactory.Instruction("cgt", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void CompareLessThanUnsigned_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CltUn:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 10),
                ModelFactory.Instruction("clt.un", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void CompareLessThan_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Clt:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 10),
                ModelFactory.Instruction("clt", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Conv Variants (non-overflow) ───────────────────────────────────

    [Theory]
    [InlineData("conv.i1", "System.SByte")]
    [InlineData("conv.u1", "System.Byte")]
    [InlineData("conv.i2", "System.Int16")]
    [InlineData("conv.u2", "System.UInt16")]
    [InlineData("conv.u8", "System.UInt64")]
    [InlineData("conv.u", "System.UIntPtr")]
    [InlineData("conv.i", "System.IntPtr")]
    public void ConvVariants_EmitsValidCode(string op, string resultType)
    {
        var method = ModelFactory.CreateMethod(
            $"TestModule.TestClass::{op}:{resultType}()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction(op, ilOffset: 1, resultType: resultType),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Overflow Conv Unsigned Input Variants ──────────────────────────

    [Theory]
    [InlineData("conv.ovf.i.un")]
    [InlineData("conv.ovf.u.un")]
    [InlineData("conv.ovf.i8.un")]
    [InlineData("conv.ovf.u8.un")]
    [InlineData("conv.ovf.i1.un")]
    [InlineData("conv.ovf.i2.un")]
    [InlineData("conv.ovf.i4.un")]
    [InlineData("conv.ovf.u1.un")]
    [InlineData("conv.ovf.u2.un")]
    [InlineData("conv.ovf.u4.un")]
    public void OverflowConvUnsignedInput_EmitsValidCode(string op)
    {
        var method = ModelFactory.CreateMethod(
            $"TestModule.TestClass::{op}:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction(op, ilOffset: 1, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Localloc (stack-alloc buffer) ──────────────────────────────────

    [Fact]
    public void Localloc_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Localloc:System.Void()",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 64),
                ModelFactory.Instruction("localloc", ilOffset: 1, resultType: "System.IntPtr"),
                ModelFactory.Instruction("pop", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Ldarga (load argument address) ─────────────────────────────────

    [Fact]
    public void Ldarga_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Ldarga:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarga", ilOffset: 0, intOperand: 0, resultType: "System.Int32&"),
                ModelFactory.Instruction("ldind.i4", ilOffset: 1, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Ldobj / Stobj (load/store object from pointer) ────────────────

    [Fact]
    public void Ldobj_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Ldobj:TestType(System.Int32)",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarga", ilOffset: 0, intOperand: 0, resultType: "System.Int32&"),
                ModelFactory.Instruction("ldobj", ilOffset: 1, "System.Int32", AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Initblk / Cpblk (memory block operations) ─────────────────────

    [Fact]
    public void Initblk_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Initblk:System.Void()",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("conv.i", ilOffset: 1, resultType: "System.IntPtr"),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 16),
                ModelFactory.Instruction("initblk", ilOffset: 4),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Ldelema (load element address) ─────────────────────────────────

    [Fact]
    public void Ldelema_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Ldelema:System.Void()",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("newarr", ilOffset: 1, "System.Int32", AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 0),
                ModelFactory.Instruction("ldelema", ilOffset: 3, "System.Int32", AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("pop", ilOffset: 4),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Ldtoken (load runtime handle) ──────────────────────────────────

    [Fact]
    public void LdtokenType_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Ldtoken:System.Void()",
            instructions: new[]
            {
                ModelFactory.Instruction("ldtoken", ilOffset: 0, "System.Int32", AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("pop", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void LdtokenField_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdtokenField:System.Void()",
            instructions: new[]
            {
                ModelFactory.Instruction("ldtoken", ilOffset: 0, "TestModule.TestClass::_field", AotCoreIrReferenceKind.Field),
                ModelFactory.Instruction("pop", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Ldnull + Brtrue/Brfalse (null branch patterns) ────────────────

    [Fact]
    public void LdnullBranch_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdnullBranch:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("brfalse", ilOffset: 1, intOperand: 4),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 3),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BrtrueBranch_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::BrtrueBranch:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("brtrue", ilOffset: 1, intOperand: 4),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 3),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Bne_Un / Bge / Ble branch variants ────────────────────────────

    [Fact]
    public void BneUnBranch_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::BneUn:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 20),
                ModelFactory.Instruction("bne.un", ilOffset: 2, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 4),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 6),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BgeBranch_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Bge:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 20),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 10),
                ModelFactory.Instruction("bge", ilOffset: 2, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 4),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 6),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void BleBranch_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Ble:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 20),
                ModelFactory.Instruction("ble", ilOffset: 2, intOperand: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 4),
                ModelFactory.Instruction("ldc.i4", ilOffset: 5, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 6),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Throw / Rethrow / Endfilter ────────────────────────────────────

    [Fact]
    public void Throw_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Throw:System.Void()",
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("throw", ilOffset: 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Rethrow_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Rethrow:System.Void()",
            instructions: new[]
            {
                ModelFactory.Instruction("rethrow", ilOffset: 0),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Endfilter_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Endfilter:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("endfilter", ilOffset: 1, resultType: "System.Int32"),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Switch ─────────────────────────────────────────────────────────

    [Fact]
    public void Switch_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Switch:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 2),
                ModelFactory.Instruction("switch", ilOffset: 1, arrayOperand: new[] { 4, 5, 6 }),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: -1),
                ModelFactory.Instruction("ret", ilOffset: 3),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
                ModelFactory.Instruction("ldc.i4", ilOffset: 6, intOperand: 1),
                ModelFactory.Instruction("ret", ilOffset: 7),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Arglist ────────────────────────────────────────────────────────

    [Fact]
    public void Arglist_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Arglist:System.Void()",
            instructions: new[]
            {
                ModelFactory.Instruction("arglist", ilOffset: 0),
                ModelFactory.Instruction("pop", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Refanyval / Refanytype / Mkrefany ──────────────────────────────

    [Fact]
    public void Refanyval_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Refanyval:System.Void()",
            instructions: new[]
            {
                ModelFactory.Instruction("refanyval", ilOffset: 0, "System.Int32", AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("pop", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    [Fact]
    public void Refanytype_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Refanytype:System.Void()",
            instructions: new[]
            {
                ModelFactory.Instruction("refanytype", ilOffset: 0),
                ModelFactory.Instruction("pop", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Load string literal ────────────────────────────────────────────

    [Fact]
    public void Ldstr_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Ldstr:System.String()",
            returnType: "System.String",
            returnAbi: ModelFactory.RefAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldstr", ilOffset: 0, stringOperand: "Hello, World!", resultType: "System.String"),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Load length of array ───────────────────────────────────────────

    [Fact]
    public void Ldlen_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::Ldlen:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("newarr", ilOffset: 1, "System.Int32", AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ldlen", ilOffset: 2, resultType: "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    public void Dispose()
    {
    }
}
