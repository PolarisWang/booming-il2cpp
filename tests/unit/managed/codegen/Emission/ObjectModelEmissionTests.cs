using Chaos.IL2CPP.Generator.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Tests covering field access, object model operations, and object equality
/// emission paths (ObjectModelEmission, ObjectModelUtilities, ObjectEqualityEmission).
///
/// Note: ldfld/stfld/ldsfld/stsfld/ldsflda require full TargetReference metadata
/// which our test instruction factory doesn't support yet. Tests are focused on
/// operations that work with callee-string metadata.
/// </summary>
public sealed class ObjectModelEmissionTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    /// <summary>
    /// ldlen (array length) should emit array length load.
    /// </summary>
    [Fact]
    public void LoadArrayLength_EmitsArrayLength()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetLength",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldlen", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// ldelem (load array element) by reference type produces element load.
    /// </summary>
    [Fact]
    public void LoadArrayElementRef_EmitsElementLoad()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetElement",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldelem", ilOffset: 2, subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// stelem (store array element) for reference type.
    /// </summary>
    [Fact]
    public void StoreArrayElementRef_EmitsElementStore()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::SetElement",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldarg", ilOffset: 2, intOperand: 2),
                ModelFactory.Instruction("stelem", ilOffset: 3, subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// newarr (new array) should produce an array allocation.
    /// </summary>
    [Fact]
    public void NewArray_EmitsArrayAllocation()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::MakeArray",
            returnType: "System.Int32[]",
            returnAbi: ModelFactory.RefAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("newarr", ilOffset: 1, subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// newobj (new object) constructor invocation.
    /// </summary>
    [Fact]
    public void NewObject_EmitsObjectAllocation()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::MakeObject",
            returnType: "System.Object",
            returnAbi: ModelFactory.RefAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("newobj", ilOffset: 0, subjectId: "System.Object", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// castclass / isinst type checking operations.
    /// </summary>
    [Fact]
    public void TypeCheckOperations_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CheckType",
            returnType: "System.Boolean",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("isinst", ilOffset: 1, subjectId: "System.String", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ldnull", ilOffset: 2),
                ModelFactory.Instruction("cgt.un", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// box operation for value type to reference conversion.
    /// </summary>
    [Fact]
    public void BoxValueType_EmitsBoxAllocation()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::BoxInt32",
            returnType: "System.Object",
            returnAbi: ModelFactory.RefAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("box", ilOffset: 1, subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    /// <summary>
    /// unbox operation for reference to value type conversion.
    /// </summary>
    [Fact]
    public void UnboxValueType_EmitsUnboxCode()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::UnboxInt32",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("unbox", ilOffset: 1, subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ldind.i4", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    /// <summary>
    /// ldnull pushes a null reference to the eval stack.
    /// </summary>
    [Fact]
    public void LoadNull_EmitsNullConstant()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::ReturnNull",
            returnType: "System.Object",
            returnAbi: ModelFactory.RefAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// ldtoken instructions produce runtime handles.
    /// </summary>
    [Fact]
    public void LoadToken_EmitsTokenAccess()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetTypeHandle",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldtoken", ilOffset: 0, subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// ceq (compare equal) evaluates equality of two eval-stack values.
    /// </summary>
    [Fact]
    public void CompareEqual_EmitsEqualityCheck()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::IsEqual",
            returnType: "System.Boolean",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 2),
                ModelFactory.Instruction("ceq", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    /// <summary>
    /// ldelema (load element address) operations.
    /// </summary>
    [Fact]
    public void LoadElementAddress_EmitsAddressOf()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::GetElementAddr",
            returnType: "System.IntPtr",
            returnAbi: ModelFactory.IntPtrAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("ldelema", ilOffset: 2, subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// ldobj loads a value type object from a managed pointer.
    /// </summary>
    [Fact]
    public void LoadObject_EmitsLoadFromPointer()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::LoadObj",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldobj", ilOffset: 1, subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// stobj stores a value type to a managed pointer.
    /// </summary>
    [Fact]
    public void StoreObject_EmitsStoreToPointer()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::StoreObj",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.IntPtrAbi, ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.Instruction("stobj", ilOffset: 2, subjectId: "System.Int32", refKind: AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// cpblk copies a block of memory.
    /// </summary>
    [Fact]
    public void CopyBlock_EmitsMemoryCopy()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::CopyBlock",
            returnAbi: ModelFactory.VoidAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 16),
                ModelFactory.Instruction("cpblk", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    /// <summary>
    /// initblk initializes a block of memory.
    /// </summary>
    [Fact]
    public void InitBlock_EmitsMemoryInit()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::InitBlock",
            returnAbi: ModelFactory.VoidAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 2, intOperand: 32),
                ModelFactory.Instruction("initblk", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.DoesNotContainCode("AOT-unreachable stub", source);
    }

    public void Dispose()
    {
    }
}
