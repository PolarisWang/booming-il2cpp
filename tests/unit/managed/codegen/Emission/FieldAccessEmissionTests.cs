using Chaos.IL2CPP.Generator.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Tests covering field access operations (ldfld, stfld, ldsfld, stsfld, ldflda, ldsflda, sizeof).
/// These require TargetReference with Kind=Field and appropriate RuntimeServiceKind values.
/// </summary>
public sealed class FieldAccessEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    [Fact]
    public void LoadInstanceField_EmitsFieldAccess()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetField",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.FieldInstruction("ldfld", ilOffset: 1,
                    AotCoreIrRuntimeServiceKind.LoadInstanceField,
                    "TestModule.TestClass", "_myField", "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    [Fact]
    public void StoreInstanceField_EmitsFieldStore()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::SetField",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.FieldInstruction("stfld", ilOffset: 2,
                    AotCoreIrRuntimeServiceKind.StoreInstanceField,
                    "TestModule.TestClass", "_myField", "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    [Fact]
    public void LoadStaticField_EmitsStaticFieldAccess()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetStaticField",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.FieldInstruction("ldsfld", ilOffset: 0,
                    AotCoreIrRuntimeServiceKind.LoadStaticField,
                    "TestModule.TestClass", "s_global", "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    [Fact]
    public void StoreStaticField_EmitsStaticFieldStore()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::SetStaticField",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.FieldInstruction("stsfld", ilOffset: 1,
                    AotCoreIrRuntimeServiceKind.StoreStaticField,
                    "TestModule.TestClass", "s_global", "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    [Fact]
    public void LoadInstanceFieldAddress_EmitsAddressOfField()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetFieldAddr",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.FieldInstruction("ldflda", ilOffset: 1,
                    AotCoreIrRuntimeServiceKind.LoadInstanceField,
                    "TestModule.TestClass", "_myField", "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    [Fact]
    public void LoadStaticFieldAddress_EmitsAddressOfStaticField()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetStaticFieldAddr",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.FieldInstruction("ldsflda", ilOffset: 0,
                    AotCoreIrRuntimeServiceKind.LoadStaticField,
                    "TestModule.TestClass", "s_global", "System.Int32"),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    [Fact]
    public void SizeOfType_EmitsTypeSize()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetSizeOf",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("sizeof", ilOffset: 0,
                    subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    [Fact]
    public void StoreInstanceFieldWithStringId_EmitsFieldStoreWithMaterialize()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::SetFieldString",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.FieldInstruction("stfld", ilOffset: 2,
                    AotCoreIrRuntimeServiceKind.StoreInstanceField,
                    "TestModule.TestClass", "_name", "System.String"),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    public void Dispose()
    {
    }
}
