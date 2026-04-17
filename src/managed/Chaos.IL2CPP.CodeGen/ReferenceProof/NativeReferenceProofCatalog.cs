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

    public static string GetRequiredTemplateRelativePath(string planKind)
    {
        if (TemplateRelativePathByPlanKind.TryGetValue(planKind, out var relativePath))
        {
            return relativePath;
        }

        throw new InvalidOperationException($"unsupported native-reference lowering plan kind '{planKind}'");
    }
}
