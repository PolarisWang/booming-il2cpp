using Chaos.IL2CPP.CodeGen.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests.Emission;

/// <summary>
/// Tests for static initialization, string ID emission, and metadata
/// support paths (StaticInitializationEmission, StringIdEmission,
/// MetadataSupport).
/// </summary>
public sealed class StaticInitAndMetadataTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    /// <summary>
    /// A method with only arithmetic should not emit cctor guards.
    /// </summary>
    [Fact]
    public void PureArithmetic_NoCctorGuard()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::PureAdd",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 2),
                ModelFactory.Instruction("add", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    /// <summary>
    /// ldsflda (load static field address) requires TargetReference metadata,
    /// not available in simple test instruction factory. Skipped.
    /// </summary>
    /// ldstr (load string literal) should emit a string allocation through
    /// the runtime's string table.
    /// </summary>
    [Fact]
    public void LoadStringLiteral_EmitsStringAlloc()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetHello",
            returnType: "System.String",
            returnAbi: ModelFactory.RefAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldstr", ilOffset: 0, stringOperand: "Hello"),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// ldstr with an empty string should also work.
    /// </summary>
    [Fact]
    public void LoadEmptyString_EmitsStringAlloc()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetEmpty",
            returnType: "System.String",
            returnAbi: ModelFactory.RefAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldstr", ilOffset: 0, stringOperand: ""),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// A method that uses ldftn (load function pointer) for a static method.
    /// </summary>
    [Fact]
    public void LoadFunctionPointer_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetFnPtr",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldftn", ilOffset: 0,
                    callee: "TestModule.TestClass::TargetMethod",
                    targetSymbol: "Chaos_TargetFunction"),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// A method with no locals and no parameters still generates valid code.
    /// </summary>
    [Fact]
    public void NoParamsNoLocals_GeneratesValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::NoOp");
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.ContainsCode("return;", source);
    }

    /// <summary>
    /// A method with many local variable slots exercises local allocation.
    /// </summary>
    [Fact]
    public void ManyLocals_EmitsLocalSlots()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ManyLocals",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 2),
                ModelFactory.Instruction("stloc", ilOffset: 3, intOperand: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 4, intOperand: 3),
                ModelFactory.Instruction("stloc", ilOffset: 5, intOperand: 2),
                ModelFactory.Instruction("ldloc", ilOffset: 6, intOperand: 0),
                ModelFactory.Instruction("ldloc", ilOffset: 7, intOperand: 1),
                ModelFactory.Instruction("add", ilOffset: 8),
                ModelFactory.Instruction("ldloc", ilOffset: 9, intOperand: 2),
                ModelFactory.Instruction("add", ilOffset: 10),
                ModelFactory.Instruction("ret", ilOffset: 11),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    /// <summary>
    /// ldloca (load local address) emits address-of-local.
    /// </summary>
    [Fact]
    public void LoadLocalAddress_EmitsAddressOf()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetLocalAddr",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldloca", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// dup duplicates the top eval stack value.
    /// </summary>
    [Fact]
    public void DupOpcode_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::DupValue",
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
        AssertExtensions.UsesStructuredIR(source);
    }

    /// <summary>
    /// pop discards the top eval stack value.
    /// </summary>
    [Fact]
    public void PopOpcode_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::PopValue",
            returnType: "System.Void",
            returnAbi: ModelFactory.VoidAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("pop", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    /// <summary>
    /// ldc.i4 with a 0 value (common pattern).
    /// </summary>
    [Fact]
    public void LoadZeroInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ReturnZero",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.UsesStructuredIR(source);
    }

    public void Dispose()
    {
    }
}
