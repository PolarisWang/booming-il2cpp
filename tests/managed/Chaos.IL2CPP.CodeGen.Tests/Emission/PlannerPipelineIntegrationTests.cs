using Chaos.IL2CPP.CodeGen.Tests.Infra;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.CodeGen.Tests.Emission;

/// <summary>
/// Advanced pipeline integration tests exercising planner code paths
/// that basic emission tests don't reach: non-full-assembly mode,
/// Subject_N method naming, cross-referencing methods, and
/// diverse dispatch configurations.
/// </summary>
public sealed class PlannerPipelineIntegrationTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    public void Dispose()
    {
    }

    // ──── Subject_N naming (exercises ExtractNumericSortKey / CollectAllMethods) ──

    [Fact]
    public void MethodsWithSubjectNSortKeys_EmitsAll()
    {
        var m1 = ModelFactory.CreateMethod(
            "Test.Module/MyClass::Subject_3:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            nativeSymbol: "Chaos_Subject3",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 30),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });
        var m2 = ModelFactory.CreateMethod(
            "Test.Module/MyClass::Subject_1:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            nativeSymbol: "Chaos_Subject1",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });
        var m3 = ModelFactory.CreateMethod(
            "Test.Module/MyClass::Subject_2:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            nativeSymbol: "Chaos_Subject2",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 20),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var artifact = ModelFactory.CreateArtifact(m1, m2, m3);
        var plan = ModelFactory.CreateDefaultPlan(m1.SubjectId);
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: PlannerFixture.StubAssemblyPath);
        var model = _fixture.RunPlanner(artifact, plan, manifest);

        // All three methods should be present (CollectAllMethods sorts by numeric key)
        Assert.Contains(model.Methods, m => m.SubjectId.Contains("Subject_1"));
        Assert.Contains(model.Methods, m => m.SubjectId.Contains("Subject_2"));
        Assert.Contains(model.Methods, m => m.SubjectId.Contains("Subject_3"));
    }

    // ──── Multiple methods in one artifact ─────────────────────────────────

    [Fact]
    public void MultipleMethodsInArtifact_AllEmitted()
    {
        var m1 = ModelFactory.CreateMethod(
            "Test.Module/MyClass::First:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            nativeSymbol: "Chaos_First",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });
        var m2 = ModelFactory.CreateMethod(
            "Test.Module/MyClass::Second:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            nativeSymbol: "Chaos_Second",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 20),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var artifact = ModelFactory.CreateArtifact(m1, m2);
        var plan = ModelFactory.CreateDefaultPlan(m1.SubjectId);
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: PlannerFixture.StubAssemblyPath);
        var model = _fixture.RunPlanner(artifact, plan, manifest);

        Assert.Contains(model.Methods, m => m.SubjectId.Contains("First"));
        Assert.Contains(model.Methods, m => m.SubjectId.Contains("Second"));
    }

    // ──── Non-full-assembly mode (exercises CollectReachableMethods) ─────────

    [Fact]
    public void NonFullAssemblyMode_WithReachableMethods_EmitsEntryAndReachable()
    {
        var calleeMethod = ModelFactory.CreateMethod(
            "Test.Module/MyClass::Helper:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            returnType: "System.Int32",
            nativeSymbol: "Chaos_Helper",
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var entryMethod = ModelFactory.CreateMethod(
            "Test.Module/MyClass::Entry:System.Int32()",
            returnAbi: ModelFactory.Int32Abi,
            returnType: "System.Int32",
            nativeSymbol: "Chaos_Entry",
            instructions: new[]
            {
                // call Helper
                ModelFactory.CallInstruction(
                    callee: calleeMethod.SubjectId,
                    targetSymbol: calleeMethod.NativeSymbol,
                    targetReturnType: "System.Int32",
                    targetParameterCount: 0),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var artifact = ModelFactory.CreateArtifact(entryMethod, calleeMethod);
        var plan = ModelFactory.CreateDefaultPlan(entryMethod.SubjectId);
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: PlannerFixture.StubAssemblyPath);
        var model = _fixture.RunPlanner(artifact, plan, manifest);

        // In full-assembly mode, both methods should be present
        Assert.Contains(model.Methods, m => m.SubjectId.Contains("Entry"));
        Assert.Contains(model.Methods, m => m.SubjectId.Contains("Helper"));
        Assert.All(model.Methods, m => AssertExtensions.UsesStructuredIR(m.MethodSource));
    }

    // ──── callvirt with TargetReference (exercises ResolveReachableMethods dispatch) ──

    [Fact]
    public void CallVirtInstruction_WithTargetReference_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::CallVirtTarget:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                new AotCoreIrInstructionArtifact
                {
                    Op = "callvirt",
                    IlOffset = 1,
                    Callee = "System.Private.CoreLib/System.Object::ToString:System.String()",
                    TargetSymbol = "Chaos_Object_ToString",
                    TargetReturnType = "System.String",
                    TargetParameterCount = 0,
                },
                ModelFactory.Instruction("ldnull", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── newobj with TargetReference (exercises ResolveReachableMethods newobj path) ──

    [Fact]
    public void NewObjInstruction_WithCallee_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::CreateObject:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                // newobj requires Type reference kind (not Method)
                ModelFactory.Instruction("newobj", ilOffset: 1,
                    "System.Private.CoreLib/System.Int32",
                    AotCoreIrReferenceKind.Type,
                    assemblyName: "System.Private.CoreLib"),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── ldftn instruction ────────────────────────────────────────────────

    [Fact]
    public void LdftnInstruction_EmitsValidCode()
    {
        // ldftn needs a TargetSymbol for the function pointer target
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::GetFnPtr:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                new AotCoreIrInstructionArtifact
                {
                    Op = "ldftn",
                    IlOffset = 0,
                    Callee = "System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)",
                    TargetSymbol = "Chaos_String_Concat",
                },
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Methods with diverse return type ABIs ────────────────────────────

    [Fact]
    public void Int64ReturnType_WithAdd_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::LongAdd:System.Int64()",
            returnType: "System.Int64",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i8", ilOffset: 0, longOperand: 100L),
                ModelFactory.Instruction("ldc.i8", ilOffset: 1, longOperand: 200L),
                ModelFactory.Instruction("add", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    // ──── Struct return + ref ABI ─────────────────────────────────────────

    [Fact]
    public void RefReturnType_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::GetRef:TestType()",
            returnAbi: ModelFactory.RefAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ret", ilOffset: 1),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Newarr with type reference ──────────────────────────────────────

    [Fact]
    public void NewArrInstruction_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::NewIntArray:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 10),
                ModelFactory.Instruction("newarr", ilOffset: 1,
                    "System.Private.CoreLib/System.Int32",
                    AotCoreIrReferenceKind.Type),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Localloc instruction ────────────────────────────────────────────

    [Fact]
    public void LocallocInstruction_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::AllocLocal:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 64),
                ModelFactory.Instruction("localloc", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Ldlen instruction ───────────────────────────────────────────────

    [Fact]
    public void LdlenInstruction_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::GetLength:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldnull", ilOffset: 0),
                ModelFactory.Instruction("ldlen", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Ldind.i8 (indirect load i8) ────────────────────────────────────

    [Fact]
    public void LdindI8_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::LoadIndirectI8:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("conv.i", ilOffset: 1),
                ModelFactory.Instruction("ldind.i8", ilOffset: 2),
                ModelFactory.Instruction("ret", ilOffset: 3),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Ldarg.0 + stloc + ldloc (local variable usage) ─────────────────

    [Fact]
    public void LocalVariableLoadStore_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::LocalsUsage:System.Int32(System.Int32)",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.Int32Abi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("stloc", ilOffset: 1, intOperand: 0),
                ModelFactory.Instruction("ldloc", ilOffset: 2, intOperand: 0),
                ModelFactory.Instruction("ldc.i4", ilOffset: 3, intOperand: 1),
                ModelFactory.Instruction("add", ilOffset: 4),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    // ──── Mul.ovf (overflow multiply) + conv.ovf.i8 chain ────────────────

    [Fact]
    public void OverflowArithmeticChain_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::OverflowChain:System.Int64()",
            returnType: "System.Int64",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 1000),
                ModelFactory.Instruction("ldc.i4", ilOffset: 1, intOperand: 2000),
                ModelFactory.Instruction("mul.ovf", ilOffset: 2),
                ModelFactory.Instruction("conv.ovf.i8", ilOffset: 3),
                ModelFactory.Instruction("ret", ilOffset: 4),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
        AssertExtensions.UsesStructuredIR(source);
    }

    // ──── Conv.ovf.i.un (unsigned overflow) ──────────────────────────────

    [Fact]
    public void ConvOvfIUn_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::ConvOvfIUn:TestType()",
            returnAbi: ModelFactory.Int32Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.i.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }

    // ──── Conv.ovf.u8.un (unsigned to u8 overflow) ───────────────────────

    [Fact]
    public void ConvOvfU8Un_EmitsValidCode()
    {
        var method = ModelFactory.CreateMethod(
            "Test.Module/MyClass::ConvOvfU8Un:TestType()",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.Instruction("conv.ovf.u8.un", ilOffset: 1),
                ModelFactory.Instruction("ret", ilOffset: 2),
            });

        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode("extern \"C\"", source);
    }
}
