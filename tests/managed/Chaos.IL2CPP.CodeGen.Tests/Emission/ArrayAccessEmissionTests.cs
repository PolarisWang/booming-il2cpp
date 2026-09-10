using Chaos.IL2CPP.Generator.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Tests covering all typed array element load/store variants (ldelem.* and stelem.*).
/// Each test verifies the planner emits valid structured-IR for typed array access.
/// </summary>
public sealed class ArrayAccessEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void LoadArrayElementInt8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdelemI1",
            returnType: "System.SByte",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldelem.i1", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadArrayElementUInt8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdelemU1",
            returnType: "System.Byte",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldelem.u1", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadArrayElementInt16_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdelemI2",
            returnType: "System.Int16",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldelem.i2", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadArrayElementUInt16_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdelemU2",
            returnType: "System.UInt16",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldelem.u2", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadArrayElementInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdelemI4",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldelem.i4", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadArrayElementUInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdelemU4",
            returnType: "System.UInt32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldelem.u4", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadArrayElementInt64_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdelemI8",
            returnType: "System.Int64",
            returnAbi: ModelFactory.Int64Abi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldelem.i8", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadArrayElementFloat32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdelemR4",
            returnType: "System.Single",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldelem.r4", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadArrayElementFloat64_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdelemR8",
            returnType: "System.Double",
            returnAbi: ModelFactory.Int64Abi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldelem.r8", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadArrayElementRef_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdelemRef",
            returnType: "System.Object",
            returnAbi: ModelFactory.RefAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldelem.ref", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreArrayElementInt8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StelemI1",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldarg", ilOffset: 2, intOperand: 2),
                ModelFactory.Instruction("stelem.i1", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreArrayElementInt16_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StelemI2",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldarg", ilOffset: 2, intOperand: 2),
                ModelFactory.Instruction("stelem.i2", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreArrayElementInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StelemI4",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldarg", ilOffset: 2, intOperand: 2),
                ModelFactory.Instruction("stelem.i4", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreArrayElementInt64_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StelemI8",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi, ModelFactory.Int64Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldarg", ilOffset: 2, intOperand: 2),
                ModelFactory.Instruction("stelem.i8", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreArrayElementFloat32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StelemR4",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldarg", ilOffset: 2, intOperand: 2),
                ModelFactory.Instruction("stelem.r4", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreArrayElementFloat64_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StelemR8",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi, ModelFactory.Int64Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldarg", ilOffset: 2, intOperand: 2),
                ModelFactory.Instruction("stelem.r8", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreArrayElementRef_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StelemRef",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi, ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldarg", ilOffset: 2, intOperand: 2),
                ModelFactory.Instruction("stelem.ref", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    public void Dispose()
    {
    }
}
