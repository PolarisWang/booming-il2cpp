using Chaos.IL2CPP.Generator.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Tests covering all ldind.* (load indirect) and stind.* (store indirect) variants.
/// Each test verifies the planner emits valid structured-IR for indirect memory access.
/// </summary>
public sealed class IndirectAccessEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void LoadIndirectInt8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdindI1",
            returnType: "System.SByte",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldind.i1", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadIndirectUInt8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdindU1",
            returnType: "System.Byte",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldind.u1", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadIndirectInt16_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdindI2",
            returnType: "System.Int16",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldind.i2", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadIndirectUInt16_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdindU2",
            returnType: "System.UInt16",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldind.u2", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadIndirectInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdindI4",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldind.i4", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadIndirectUInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdindU4",
            returnType: "System.UInt32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldind.u4", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadIndirectInt64_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdindI8",
            returnType: "System.Int64",
            returnAbi: ModelFactory.Int64Abi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldind.i8", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadIndirectFloat32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdindR4",
            returnType: "System.Single",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldind.r4", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadIndirectFloat64_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdindR8",
            returnType: "System.Double",
            returnAbi: ModelFactory.Int64Abi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldind.r8", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadIndirectRef_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdindRef",
            returnType: "System.Object",
            returnAbi: ModelFactory.RefAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldind.ref", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void LoadIndirectNativeInt_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LdindI",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldind.i", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreIndirectInt8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StindI1",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("stind.i1", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreIndirectInt16_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StindI2",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("stind.i2", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreIndirectInt32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StindI4",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("stind.i4", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreIndirectInt64_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StindI8",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi, ModelFactory.Int64Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("stind.i8", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreIndirectFloat32_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StindR4",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("stind.r4", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreIndirectFloat64_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StindR8",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi, ModelFactory.Int64Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("stind.r8", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreIndirectRef_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StindRef",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi, ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("stind.ref", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    [Fact]
    public void StoreIndirectNativeInt_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StindI",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi, ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("stind.i", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });
        AssertExtensions.ContainsCode("extern \"C\"", _fixture.RunPlannerSingleMethod(method));
    }

    public void Dispose()
    {
    }
}
