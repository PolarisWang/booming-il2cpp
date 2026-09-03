using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Generator.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Lowering;

/// <summary>
/// Tests that verify external runtime call dispatch uses the correct mechanism:
/// DirectNativeSymbol (direct C++ function call) vs. external runtime dispatch table
/// (kChaosExternalRuntimeFnTable index lookup).
///
/// After the P0 optimization, SimpleForward shapes with NativeFnSymbol set should
/// emit direct calls through DirectNativeSymbol, bypassing the wrapper function.
/// </summary>
public sealed class ExternalDispatchTests : IDisposable
{
    private readonly PlannerFixture _fixture = new();

    /// <summary>
    /// System.GC::KeepAlive is registered as a SimpleForward shape with
    /// NativeFnSymbol="chaos_gc_keepalive". The generated code should call
    /// chaos_gc_keepalive directly.
    /// </summary>
    [Fact]
    public void GcKeepAlive_UsesDirectNativeCall()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestKeepAlive",
            returnAbi: ModelFactory.VoidAbi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                // push arg (object reference)
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                // call System.GC::KeepAlive(System.Object)
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.GC::KeepAlive:System.Void(System.Object)",
                    targetSymbol: "chaos_gc_keepalive",
                    ilOffset: 1,
                    targetReturnType: "System.Void",
                    targetParameterCount: 1),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        // Should call chaos_gc_keepalive directly
        AssertExtensions.UsesDirectNativeCall("chaos_gc_keepalive", source);
    }

    /// <summary>
    /// System.String::get_Length is registered as both a GenericShapeDescriptor and a
    /// SimpleForward. The GenericShapeDescriptor takes priority and emits a call to
    /// the external runtime wrapper, which handles both string_id and native string
    /// pointer representations.
    /// </summary>
    [Fact]
    public void StringGetLength_UsesExternalRuntimeWrapper()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestGetLength",
            returnType: "System.Int32",
            returnAbi: ModelFactory.Int32Abi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.String::get_Length:System.Int32()",
                    targetSymbol: "chaos_string_get_length",
                    ilOffset: 1,
                    targetReturnType: "System.Int32",
                    targetParameterCount: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        // The call goes through the external runtime wrapper (GenericShapeDescriptor
        // for get_Length adds string_id dispatch), not chaos_string_get_length directly.
        AssertExtensions.ContainsCode("chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__", source);
    }

    /// <summary>
    /// System.GC::Collect with no args is InlineBody (not SimpleForward), so it
    /// should NOT use DirectNativeSymbol. It should use the dispatch table or
    /// inline the body instead.
    /// </summary>
    [Fact]
    public void GcCollect_NoArgs_DoesNotUseDirectCall()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestCollect",
            returnAbi: ModelFactory.VoidAbi,
            instructions: new[]
            {
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.GC::Collect:System.Void()",
                    targetSymbol: "chaos_gc_collect",
                    ilOffset: 0,
                    targetReturnType: "System.Void",
                    targetParameterCount: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        // InlineBody entries should NOT generate a direct call to chaos_gc_collect
        // (they are not SimpleForward, so no DirectNativeSymbol dispatch)
        AssertExtensions.DoesNotContainCode("chaos_gc_collect(", source);
    }

    /// <summary>
    /// Unknown/unregistered external calls now emit a direct chaos_external_runtime_*
    /// stub call (the dispatch table / kChaosExternalRuntimeFnTable is reserved for the
    /// EH thunk paths). Push a value on the eval stack before the call so the structured
    /// IR builder can pop it as the argument.
    /// </summary>
    [Fact]
    public void UnknownExternalCall_EmitsExternalRuntimeStub()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestUnknownCall",
            returnAbi: ModelFactory.VoidAbi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 42),
                ModelFactory.CallInstruction(
                    callee: "Some.Unknown::UnregisteredMethod:System.Void(System.Int32)",
                    targetSymbol: "SomeUnknown_UnregisteredMethod",
                    ilOffset: 1,
                    targetReturnType: "System.Void",
                    targetParameterCount: 1),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        // Should emit a direct chaos_external_runtime_* stub call for the
        // unknown/unregistered external method.
        AssertExtensions.ContainsCode("chaos_external_runtime_", source);
        AssertExtensions.ContainsCode("Some_Unknown__UnregisteredMethod", source);
    }

    /// <summary>
    /// System.Exception::get_Message is a SimpleForward shape with
    /// NativeFnSymbol="ChaosReflectionGetExceptionMessage".
    /// </summary>
    [Fact]
    public void ExceptionGetMessage_UsesDirectNativeCall()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestGetMessage",
            returnAbi: ModelFactory.RefAbi,
            parameterAbis: new[] { ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.Exception::get_Message:System.String()",
                    targetSymbol: "ChaosReflectionGetExceptionMessage",
                    ilOffset: 1,
                    targetReturnType: "System.String",
                    targetParameterCount: 0),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        AssertExtensions.UsesDirectNativeCall("ChaosReflectionGetExceptionMessage", source);
    }

    /// <summary>
    /// GC::GetTotalMemory has an Int64 return type and should dispatch correctly.
    /// </summary>
    [Fact]
    public void GcGetTotalMemory_UsesDirectNativeCall()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestGetTotalMemory",
            returnType: "System.Int64",
            returnAbi: ModelFactory.Int64Abi,
            instructions: new[]
            {
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)",
                    targetSymbol: "chaos_gc_get_total_memory",
                    ilOffset: 1,
                    targetReturnType: "System.Int64",
                    targetParameterCount: 1),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        AssertExtensions.UsesDirectNativeCall("chaos_gc_get_total_memory", source);
    }

    /// <summary>
    /// System.Delegate::Combine should dispatch directly to chaos_delegate_combine.
    /// </summary>
    [Fact]
    public void DelegateCombine_UsesDirectNativeCall()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestCombine",
            returnAbi: ModelFactory.RefAbi,
            parameterAbis: new[] { ModelFactory.RefAbi, ModelFactory.RefAbi },
            instructions: new[]
            {
                ModelFactory.Instruction("ldarg", ilOffset: 0, intOperand: 0),
                ModelFactory.Instruction("ldarg", ilOffset: 1, intOperand: 1),
                ModelFactory.CallInstruction(
                    callee: "System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)",
                    targetSymbol: "chaos_delegate_combine",
                    ilOffset: 2,
                    targetReturnType: "System.Delegate",
                    targetParameterCount: 2),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });

        var source = _fixture.RunPlannerSingleMethod(method);

        AssertExtensions.UsesDirectNativeCall("chaos_delegate_combine", source);
    }

    public void Dispose()
    {
        // No cleanup needed.
    }

    /// <summary>
    /// ComInterfaceMarshaller<Int32>/UniqueComInterfaceMarshaller<Int32>
    /// .ConvertToUnmanaged is forwarded (方案D) to ChaosComInterfaceMarshallerConvertToUnmanaged
    /// (a non-null inert placeholder) instead of falling through to the catch-all
    /// ExternalRuntimeFallback null return.  The GenericShapeDescriptor fires at
    /// TryMatchGenericShape (ExternalRuntimeHelpers.cs:589) and the Resolver builds
    /// a GenericShapeResolution whose CppSource contains the placeholder call.
    ///
    /// The caller body DOES NOT contain the placeholder inline — it calls the wrapper
    /// symbol (chaos_external_runtime_...).  The placeholder lives in the shared
    /// ObjectModelCode (external runtime helper definitions).  This test asserts both:
    ///   (A) the caller references the wrapper symbol (proves shape fired);
    ///   (B) the ObjectModelCode contains the placeholder body (proves generated code
    ///       returns ChaosComInterfaceMarshallerConvertToUnmanaged, not null).
    /// </summary>
    [Fact]
    public void UniqueComInterfaceMarshallerConvertToUnmanaged_EmitsPlaceholderCall()
    {
        var method = ModelFactory.CreateMethod(
            "TestModule.TestClass::TestComConvert",
            returnAbi: ModelFactory.VoidAbi,
            instructions: new[]
            {
                // push arg (managed value, Int32 -> slot arg 0)
                ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: 0),
                // call UniqueComInterfaceMarshaller<System.Int32>.ConvertToUnmanaged(System.Int32)
                ModelFactory.CallInstruction(
                    callee: "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller<System.Int32>::ConvertToUnmanaged:System.Void*(System.Int32)",
                    targetSymbol: "chaos_com_marshaler",
                    ilOffset: 1,
                    targetReturnType: "System.Void*",
                    targetParameterCount: 1),
                ModelFactory.Instruction("ret", ilOffset: 5),
            });

        // (A) Assert caller body references the wrapper symbol (proves shape fired).
        var source = _fixture.RunPlannerSingleMethod(method);
        AssertExtensions.ContainsCode(
            "chaos_external_runtime_System_Private_CoreLib_System_Runtime_InteropServices_Marshalling_UniqueComInterfaceMarshaller_System_Int32___ConvertToUnmanaged_System_Void__System_Int32_",
            source);

        // (B) Assert the shared ObjectModelCode contains the placeholder body.
        // This proves the Resolver's CppSource (which holds the actual placeholder
        // call) was emitted — not a generic null-returning fallback.
        var artifact = ModelFactory.CreateArtifact(method);
        var plan = ModelFactory.CreateDefaultPlan(method.SubjectId);
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: PlannerFixture.StubAssemblyPath);
        var templateModel = _fixture.RunPlanner(artifact, plan, manifest);
        AssertExtensions.ContainsCode(
            "ChaosComInterfaceMarshallerConvertToUnmanaged",
            templateModel.ObjectModelCode);
    }
}
