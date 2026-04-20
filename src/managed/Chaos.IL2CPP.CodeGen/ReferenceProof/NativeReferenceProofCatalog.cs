using Scriban;

namespace Chaos.IL2CPP.CodeGen;

internal static class NativeReferenceProofCatalog
{
    public const string ManagedAsyncAwaitIntMinimal =
        "managed-async.awaitable-int.minimal";
    public const string ManagedThreadingThreadStaticMonitorMinimal =
        "managed-threading.threadstatic-monitor.minimal";
    public const string ManagedInterfaceDispatchMessageMinimal =
        "managed-dispatch.interface-message.minimal";
    public const string ManagedDispatchVirtualInstanceMessageMinimal =
        "managed-dispatch.virtual-instance-message.minimal";
    public const string ManagedObjectCapturedStateInstanceMessageMinimal =
        "managed-object.captured-state-instance-message.minimal";
    public const string ManagedGenericStaticForwarderCapturedGetterMinimal =
        "managed-generic.static-forwarder-captured-getter.minimal";
    public const string ManagedArraysBoxingReferenceArrayBoxedIntMinimal =
        "managed-arrays-boxing.reference-array-boxed-int.minimal";
    public const string DelegateClosedTargetRelayMinimal =
        "managed-delegates.closed-target-relay-message.minimal";
    public const string NestedExceptionThrowCatchFinallyMinimal =
        "managed-exceptions.nested-throw-catch-finally.minimal";
    public const string ExceptionThrowCatchFinallyMinimal =
        "managed-exceptions.throw-catch-finally-message.minimal";
    public const string ReflectionInteropClosureMinimal =
        "managed-reflection-interop.closure.minimal";
    public const string ReflectionClosedTypeQueryMinimal =
        "reflection.closed-type-query.minimal";
    public const string MarshalingUtf8ExportMinimal =
        "interop.marshaling-utf8-export.minimal";
    public const string InteropPInvokeDirectCallMinimal =
        "interop.pinvoke-direct-call.minimal";
    public const string EngineLogWriteMinimal =
        "engine.log-write.minimal";
    public const string EngineObjectHandleRoundtripMinimal =
        "engine.object-handle.roundtrip.minimal";
    public const string EngineLifecycleCallbackMinimal =
        "engine.lifecycle-callback.minimal";
    public const string EngineHostProofMinimal =
        "engine.host-proof.minimal";

    public const string GeneratedTranslationUnitTemplateRelativePath = "Templates/NativeReferenceProof.cpp.scriban";
    public const string DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.DispatchVirtualInstanceMessage.cpp.scriban";
    public const string InterfaceDispatchMessageGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.InterfaceDispatchMessage.cpp.scriban";
    public const string AsyncAwaitIntGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.AsyncAwaitInt.cpp.scriban";
    public const string ThreadingThreadStaticMonitorGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ThreadingThreadStaticMonitor.cpp.scriban";
    public const string StaticCallCtorGetterGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.StaticCallCtorGetter.cpp.scriban";
    public const string ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ArrayBoxingReferenceArray.cpp.scriban";
    public const string ReflectionQueryMinimalGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ReflectionQueryMinimal.cpp.scriban";
    public const string MarshalingUtf8ExportGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.MarshalingUtf8Export.cpp.scriban";
    public const string PInvokeDllImportMinimalGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.PInvokeDllImportMinimal.cpp.scriban";
    public const string DelegateClosedTargetRelayGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.DelegateClosedTargetRelay.cpp.scriban";
    public const string NestedExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.NestedExceptionThrowCatchFinally.cpp.scriban";
    public const string ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ExceptionThrowCatchFinally.cpp.scriban";
    public const string ReflectionInteropClosureGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ReflectionInteropClosure.cpp.scriban";
    public const string EngineLogWriteGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.EngineLogWrite.cpp.scriban";
    public const string EngineObjectHandleGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.EngineObjectHandle.cpp.scriban";
    public const string EngineLifecycleCallbackGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.EngineLifecycleCallback.cpp.scriban";
    public const string EngineHostProofGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.EngineHostProof.cpp.scriban";
    public const string AssemblyFullClosureAuditSummaryTemplateRelativePath =
        "Templates/NativeReferenceProof.AuditSummary.cpp.scriban";
    public const string AssemblyFullClosureAuditPageTemplateRelativePath =
        "Templates/NativeReferenceProof.AuditPage.cpp.scriban";
    public const string AssemblyFullClosureRuntimeSkeletonSummaryTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeletonSummary.cpp.scriban";
    public const string AssemblyFullClosureRuntimeSkeletonPageTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeletonPage.cpp.scriban";
    public const string RuntimeSkeletonReservedStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ReservedStub.cpp.scriban";
    public const string RuntimeSkeletonConstructorFieldSetterStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ConstructorFieldSetterStub.cpp.scriban";
    public const string RuntimeSkeletonFieldBackedStringReturnStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.FieldBackedStringReturnStub.cpp.scriban";
    public const string RuntimeSkeletonConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonMarshalingUtf8ExportStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.MarshalingUtf8ExportStub.cpp.scriban";
    public const string RuntimeSkeletonPInvokeDirectCallStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.PInvokeDirectCallStub.cpp.scriban";
    public const string RuntimeSkeletonStaticCallCtorGetterStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticCallCtorGetterStub.cpp.scriban";
    public const string RuntimeSkeletonStaticIntForwarderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticIntForwarderStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringForwarderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringForwarderStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringForwarderConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringForwarderConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringProducerForwarderConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringProducerCtorGetterConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringProducerCtorRenderConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonStaticLiteralStringReturnStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticLiteralStringReturnStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringProducerConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringProducerConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonFieldGetterStringReturnStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.FieldGetterStringReturnStub.cpp.scriban";
    public const string RuntimeSkeletonConstructorThenInstanceCallStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ConstructorThenInstanceCallStub.cpp.scriban";
    public const string RuntimeSkeletonDelegateClosedTargetRelayStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.DelegateClosedTargetRelayStub.cpp.scriban";
    public const string RuntimeSkeletonArrayBoxingReferenceArrayStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ArrayBoxingReferenceArrayStub.cpp.scriban";
    public const string RuntimeSkeletonInterfaceDispatchMessageStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.InterfaceDispatchMessageStub.cpp.scriban";
    public const string RuntimeSkeletonReflectionInteropClosureStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ReflectionInteropClosureStub.cpp.scriban";
    public const string RuntimeSkeletonExceptionThrowCatchFinallyStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ExceptionThrowCatchFinallyStub.cpp.scriban";
    public const string RuntimeSkeletonNestedExceptionThrowCatchFinallyStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.NestedExceptionThrowCatchFinallyStub.cpp.scriban";
    public const string RuntimeSkeletonThreadingThreadStaticMonitorStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ThreadingThreadStaticMonitorStub.cpp.scriban";

    private static readonly IReadOnlyDictionary<string, string> TemplateRelativePathByPlanKind =
        new Dictionary<string, string>(StringComparer.Ordinal)
        {
            [ManagedAsyncAwaitIntMinimal] = AsyncAwaitIntGeneratedTranslationUnitTemplateRelativePath,
            [ManagedThreadingThreadStaticMonitorMinimal] = ThreadingThreadStaticMonitorGeneratedTranslationUnitTemplateRelativePath,
            [ManagedInterfaceDispatchMessageMinimal] = InterfaceDispatchMessageGeneratedTranslationUnitTemplateRelativePath,
            [ManagedDispatchVirtualInstanceMessageMinimal] = DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplateRelativePath,
            [ManagedObjectCapturedStateInstanceMessageMinimal] = GeneratedTranslationUnitTemplateRelativePath,
            [ManagedGenericStaticForwarderCapturedGetterMinimal] = StaticCallCtorGetterGeneratedTranslationUnitTemplateRelativePath,
            [ManagedArraysBoxingReferenceArrayBoxedIntMinimal] = ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplateRelativePath,
            [DelegateClosedTargetRelayMinimal] = DelegateClosedTargetRelayGeneratedTranslationUnitTemplateRelativePath,
            [NestedExceptionThrowCatchFinallyMinimal] = NestedExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath,
            [ExceptionThrowCatchFinallyMinimal] = ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath,
            [ReflectionInteropClosureMinimal] = ReflectionInteropClosureGeneratedTranslationUnitTemplateRelativePath,
            [ReflectionClosedTypeQueryMinimal] = ReflectionQueryMinimalGeneratedTranslationUnitTemplateRelativePath,
            [MarshalingUtf8ExportMinimal] = MarshalingUtf8ExportGeneratedTranslationUnitTemplateRelativePath,
            [InteropPInvokeDirectCallMinimal] = PInvokeDllImportMinimalGeneratedTranslationUnitTemplateRelativePath,
            [EngineLogWriteMinimal] = EngineLogWriteGeneratedTranslationUnitTemplateRelativePath,
            [EngineObjectHandleRoundtripMinimal] = EngineObjectHandleGeneratedTranslationUnitTemplateRelativePath,
            [EngineLifecycleCallbackMinimal] = EngineLifecycleCallbackGeneratedTranslationUnitTemplateRelativePath,
            [EngineHostProofMinimal] = EngineHostProofGeneratedTranslationUnitTemplateRelativePath,
            ["asyncAwaitIntMinimal"] = AsyncAwaitIntGeneratedTranslationUnitTemplateRelativePath,
            ["interfaceDispatchMessage"] = InterfaceDispatchMessageGeneratedTranslationUnitTemplateRelativePath,
            ["dispatchVirtualInstanceMessage"] = DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplateRelativePath,
            ["constructorThenInstanceCall"] = GeneratedTranslationUnitTemplateRelativePath,
            ["staticCallCtorGetter"] = StaticCallCtorGetterGeneratedTranslationUnitTemplateRelativePath,
            ["arrayBoxingReferenceArray"] = ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplateRelativePath,
            ["delegateClosedTargetRelayMinimal"] = DelegateClosedTargetRelayGeneratedTranslationUnitTemplateRelativePath,
            ["nestedExceptionThrowCatchFinallyMinimal"] = NestedExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath,
            ["exceptionThrowCatchFinallyMinimal"] = ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath,
            ["reflectionInteropClosureMinimal"] = ReflectionInteropClosureGeneratedTranslationUnitTemplateRelativePath,
            ["reflectionQueryMinimal"] = ReflectionQueryMinimalGeneratedTranslationUnitTemplateRelativePath,
            ["marshalingUtf8ExportMinimal"] = MarshalingUtf8ExportGeneratedTranslationUnitTemplateRelativePath,
            ["pinvokeDllImportMinimal"] = PInvokeDllImportMinimalGeneratedTranslationUnitTemplateRelativePath,
        };

    public static Template GetTemplateForPlan(string planKind)
    {
        return ScribanTemplateRenderer.LoadTemplate(GetRequiredTemplateRelativePath(planKind));
    }

    public static Template GetAssemblyFullClosureAuditSummaryTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(AssemblyFullClosureAuditSummaryTemplateRelativePath);
    }

    public static Template GetAssemblyFullClosureAuditPageTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(AssemblyFullClosureAuditPageTemplateRelativePath);
    }

    public static Template GetAssemblyFullClosureRuntimeSkeletonSummaryTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(AssemblyFullClosureRuntimeSkeletonSummaryTemplateRelativePath);
    }

    public static Template GetAssemblyFullClosureRuntimeSkeletonPageTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(AssemblyFullClosureRuntimeSkeletonPageTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonReservedStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonReservedStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonConstructorFieldSetterStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonConstructorFieldSetterStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonFieldBackedStringReturnStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonFieldBackedStringReturnStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonMarshalingUtf8ExportStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonMarshalingUtf8ExportStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonPInvokeDirectCallStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonPInvokeDirectCallStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticCallCtorGetterStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticCallCtorGetterStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticIntForwarderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticIntForwarderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringForwarderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringForwarderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringForwarderConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringForwarderConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticLiteralStringReturnStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticLiteralStringReturnStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringProducerConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringProducerConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonFieldGetterStringReturnStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonFieldGetterStringReturnStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonConstructorThenInstanceCallStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonConstructorThenInstanceCallStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonDelegateClosedTargetRelayStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonDelegateClosedTargetRelayStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonArrayBoxingReferenceArrayStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonArrayBoxingReferenceArrayStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonInterfaceDispatchMessageStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonInterfaceDispatchMessageStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonReflectionInteropClosureStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonReflectionInteropClosureStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonExceptionThrowCatchFinallyStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonExceptionThrowCatchFinallyStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonNestedExceptionThrowCatchFinallyStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonNestedExceptionThrowCatchFinallyStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonThreadingThreadStaticMonitorStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonThreadingThreadStaticMonitorStubTemplateRelativePath);
    }

    public static string GetRequiredTemplateRelativePath(string planKind)
    {
        if (TemplateRelativePathByPlanKind.TryGetValue(planKind, out var relativePath))
        {
            return relativePath;
        }

        throw new InvalidOperationException($"unsupported native-reference lowering plan kind '{planKind}'");
    }
}
