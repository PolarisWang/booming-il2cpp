using Scriban;

namespace Chaos.IL2CPP.CodeGen;

internal static class NativeReferenceProofCatalog
{
    public static IReadOnlyList<ICodegenTemplateBundlePlugin> GetRegisteredTemplateBundles() =>
        NativeReferenceTemplateBundleRegistry.Plugins;

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
    public const string ManagedArraysReverseReferenceArrayMinimal =
        "managed-arrays-reverse.reference-array.minimal";
    public const string ManagedArraysClearReferenceArrayMinimal =
        "managed-arrays-clear.reference-array.minimal";
    public const string ManagedArraysCopyReferenceArrayMinimal =
        "managed-arrays-copy.reference-array.minimal";
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
    public const string ArrayReverseReferenceArrayGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ArrayReverseReferenceArray.cpp.scriban";
    public const string ArrayClearReferenceArrayGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ArrayClearReferenceArray.cpp.scriban";
    public const string ArrayCopyReferenceArrayGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ArrayCopyReferenceArray.cpp.scriban";
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
    public const string AssemblyFullClosureRuntimeSkeletonSummaryTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeletonSummary.cpp.scriban";
    public const string AssemblyFullClosureRuntimeSkeletonPageTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeletonPage.cpp.scriban";
    public const string RuntimeSkeletonConstructorFieldSetterStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ConstructorFieldSetterStub.cpp.scriban";
    public const string RuntimeSkeletonFieldBackedStringReturnStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.FieldBackedStringReturnStub.cpp.scriban";
    public const string RuntimeSkeletonFieldArgumentStringReturnStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.FieldArgumentStringReturnStub.cpp.scriban";
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
    public const string RuntimeSkeletonStaticBoolIdentityForwarderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticBoolIdentityForwarderStub.cpp.scriban";
    public const string RuntimeSkeletonStaticBoolProducerForwarderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticBoolProducerForwarderStub.cpp.scriban";
    public const string RuntimeSkeletonStaticPrimitiveConvertStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticPrimitiveConvertStub.cpp.scriban";
    public const string RuntimeSkeletonStaticCheckedPrimitiveConvertStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticCheckedByteConvertStub.cpp.scriban";
    public const string RuntimeSkeletonStaticBoxedIConvertibleCharInvalidCastStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticBoxedIConvertibleCharInvalidCastStub.cpp.scriban";
    public const string RuntimeSkeletonStaticBoxedValueTypeCharInvalidCastStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticBoxedValueTypeCharInvalidCastStub.cpp.scriban";
    public const string RuntimeSkeletonStaticByteForwarderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticByteForwarderStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringReturnForwarderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringReturnForwarderStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringCallForwarderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringCallForwarderStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringForwarderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringForwarderStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringProviderPassthroughStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringProviderPassthroughStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringCharProviderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringCharProviderStub.cpp.scriban";
    public const string RuntimeSkeletonStaticObjectCharProviderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticObjectCharProviderStub.cpp.scriban";
    public const string RuntimeSkeletonUnsafeManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.UnsafeManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonMemoryExtensionsManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.MemoryExtensionsManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonSpanHelpersKernelStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.SpanHelpersKernelStub.cpp.scriban";
    public const string RuntimeSkeletonArrayAndMemoryMarshalStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ArrayAndMemoryMarshalStub.cpp.scriban";
    public const string RuntimeSkeletonMarshalPlatformFastPathStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.MarshalPlatformFastPathStub.cpp.scriban";
    public const string RuntimeSkeletonHalfManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.HalfManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonFloatingScalarManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.FloatingScalarManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonPrimitiveScalarManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.PrimitiveScalarManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonWideNumericManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.WideNumericManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonCalendarStructManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.CalendarStructManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonIdentityStructManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.IdentityStructManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonCompanionManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.CompanionManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonDateTimeSemanticEngineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.DateTimeSemanticEngineStub.cpp.scriban";
    public const string RuntimeSkeletonGlobalizationDateTimeSupportStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.GlobalizationDateTimeSupportStub.cpp.scriban";
    public const string RuntimeSkeletonValueTypeKernelStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ValueTypeKernelStub.cpp.scriban";
    public const string RuntimeSkeletonVectorCopyKernelStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.VectorCopyKernelStub.cpp.scriban";
    public const string RuntimeSkeletonVectorCapabilityQueryStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.VectorCapabilityQueryStub.cpp.scriban";
    public const string RuntimeSkeletonVectorMemoryKernelStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.VectorMemoryKernelStub.cpp.scriban";
    public const string RuntimeSkeletonVectorComposeKernelStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.VectorComposeKernelStub.cpp.scriban";
    public const string RuntimeSkeletonNumberManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.NumberManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonDecimalManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.DecimalManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonCharManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.CharManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonEnumManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.EnumManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonEnumImportedBridgeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.EnumImportedBridgeStub.cpp.scriban";
    public const string RuntimeSkeletonStringManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StringManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonStaticBoxedValueTypeStringInstanceCallStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticBoxedValueTypeStringInstanceCallStub.cpp.scriban";
    public const string RuntimeSkeletonStaticBoxedValueTypeInstanceCallStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticBoxedValueTypeInstanceCallStub.cpp.scriban";
    public const string RuntimeSkeletonStaticValueTypeIdentityForwarderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticValueTypeIdentityForwarderStub.cpp.scriban";
    public const string RuntimeSkeletonStaticValueTypeCallForwarderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticValueTypeCallForwarderStub.cpp.scriban";
    public const string RuntimeSkeletonInstanceValueTypeCallForwarderStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.InstanceValueTypeCallForwarderStub.cpp.scriban";
    public const string RuntimeSkeletonConvertRuntimeHelperStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ConvertRuntimeHelperStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringLiteralAppendStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringLiteralAppendStub.cpp.scriban";
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
    public const string RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub.cpp.scriban";
    public const string RuntimeSkeletonAsyncTaskFactoryStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.AsyncTaskFactoryStub.cpp.scriban";
    public const string RuntimeSkeletonAsyncGetResultStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.AsyncGetResultStub.cpp.scriban";
    public const string RuntimeSkeletonAsyncStateMachineNoOpStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.AsyncStateMachineNoOpStub.cpp.scriban";
    public const string RuntimeSkeletonTaskKernelFastPathStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.TaskKernelFastPathStub.cpp.scriban";
    public const string RuntimeSkeletonTaskManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.TaskManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.NativeRuntimeEventSourceManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonStaticVoidObjectSinkNoOpStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticVoidObjectSinkNoOpStub.cpp.scriban";
    public const string RuntimeSkeletonStaticExceptionThrowLiteralStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticExceptionThrowLiteralStub.cpp.scriban";
    public const string RuntimeSkeletonStaticExceptionThrowStringProducerStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticExceptionThrowStringProducerStub.cpp.scriban";
    public const string RuntimeSkeletonStaticExceptionCatchStringReturnStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.StaticExceptionCatchStringReturnStub.cpp.scriban";
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
    public const string RuntimeSkeletonArrayReverseReferenceArrayStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ArrayReverseReferenceArrayStub.cpp.scriban";
    public const string RuntimeSkeletonArrayClearReferenceArrayStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ArrayClearReferenceArrayStub.cpp.scriban";
    public const string RuntimeSkeletonArrayCopyReferenceArrayStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ArrayCopyReferenceArrayStub.cpp.scriban";
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
            [ManagedArraysReverseReferenceArrayMinimal] = ArrayReverseReferenceArrayGeneratedTranslationUnitTemplateRelativePath,
            [ManagedArraysClearReferenceArrayMinimal] = ArrayClearReferenceArrayGeneratedTranslationUnitTemplateRelativePath,
            [ManagedArraysCopyReferenceArrayMinimal] = ArrayCopyReferenceArrayGeneratedTranslationUnitTemplateRelativePath,
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
            ["arrayReverseReferenceArray"] = ArrayReverseReferenceArrayGeneratedTranslationUnitTemplateRelativePath,
            ["arrayClearReferenceArray"] = ArrayClearReferenceArrayGeneratedTranslationUnitTemplateRelativePath,
            ["arrayCopyReferenceArray"] = ArrayCopyReferenceArrayGeneratedTranslationUnitTemplateRelativePath,
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

    public static Template GetAssemblyFullClosureRuntimeSkeletonSummaryTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(AssemblyFullClosureRuntimeSkeletonSummaryTemplateRelativePath);
    }

    public static Template GetAssemblyFullClosureRuntimeSkeletonPageTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(AssemblyFullClosureRuntimeSkeletonPageTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonConstructorFieldSetterStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonConstructorFieldSetterStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonFieldBackedStringReturnStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonFieldBackedStringReturnStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticBoolIdentityForwarderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticBoolIdentityForwarderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticBoolProducerForwarderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticBoolProducerForwarderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticPrimitiveConvertStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticPrimitiveConvertStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticCheckedPrimitiveConvertStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticCheckedPrimitiveConvertStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticBoxedIConvertibleCharInvalidCastStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticBoxedIConvertibleCharInvalidCastStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticBoxedValueTypeCharInvalidCastStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticBoxedValueTypeCharInvalidCastStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticByteForwarderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticByteForwarderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonFieldArgumentStringReturnStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonFieldArgumentStringReturnStubTemplateRelativePath);
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

    public static Template GetRuntimeSkeletonStaticStringReturnForwarderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringReturnForwarderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringCallForwarderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringCallForwarderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringForwarderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringForwarderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringProviderPassthroughStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringProviderPassthroughStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringCharProviderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringCharProviderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticObjectCharProviderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticObjectCharProviderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonUnsafeManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonUnsafeManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonMemoryExtensionsManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonMemoryExtensionsManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonSpanHelpersKernelStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonSpanHelpersKernelStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonArrayAndMemoryMarshalStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonArrayAndMemoryMarshalStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonMarshalPlatformFastPathStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonMarshalPlatformFastPathStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonHalfManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonHalfManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonFloatingScalarManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonFloatingScalarManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonPrimitiveScalarManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonPrimitiveScalarManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonWideNumericManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonWideNumericManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonCalendarStructManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonCalendarStructManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonIdentityStructManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonIdentityStructManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonCompanionManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonCompanionManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonDateTimeSemanticEngineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonDateTimeSemanticEngineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonGlobalizationDateTimeSupportStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonGlobalizationDateTimeSupportStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonValueTypeKernelStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonValueTypeKernelStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonVectorCopyKernelStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonVectorCopyKernelStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonVectorCapabilityQueryStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonVectorCapabilityQueryStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonVectorMemoryKernelStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonVectorMemoryKernelStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonVectorComposeKernelStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonVectorComposeKernelStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonNumberManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonNumberManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonDecimalManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonDecimalManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonCharManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonCharManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonEnumManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonEnumManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonEnumImportedBridgeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonEnumImportedBridgeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStringManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStringManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticBoxedValueTypeStringInstanceCallStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticBoxedValueTypeStringInstanceCallStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticBoxedValueTypeInstanceCallStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticBoxedValueTypeInstanceCallStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticValueTypeIdentityForwarderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticValueTypeIdentityForwarderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticValueTypeCallForwarderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticValueTypeCallForwarderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonInstanceValueTypeCallForwarderStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonInstanceValueTypeCallForwarderStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonConvertRuntimeHelperStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonConvertRuntimeHelperStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringLiteralAppendStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringLiteralAppendStubTemplateRelativePath);
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

    public static Template GetRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonAsyncTaskFactoryStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonAsyncTaskFactoryStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonAsyncGetResultStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonAsyncGetResultStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonAsyncStateMachineNoOpStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonAsyncStateMachineNoOpStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonTaskKernelFastPathStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonTaskKernelFastPathStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonTaskManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonTaskManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonNativeRuntimeEventSourceManagedInvokeStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonNativeRuntimeEventSourceManagedInvokeStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticVoidObjectSinkNoOpStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticVoidObjectSinkNoOpStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticExceptionThrowLiteralStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticExceptionThrowLiteralStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticExceptionThrowStringProducerStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticExceptionThrowStringProducerStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonStaticExceptionCatchStringReturnStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonStaticExceptionCatchStringReturnStubTemplateRelativePath);
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

    public static Template GetRuntimeSkeletonArrayReverseReferenceArrayStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonArrayReverseReferenceArrayStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonArrayClearReferenceArrayStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonArrayClearReferenceArrayStubTemplateRelativePath);
    }

    public static Template GetRuntimeSkeletonArrayCopyReferenceArrayStubTemplate()
    {
        return ScribanTemplateRenderer.LoadTemplate(RuntimeSkeletonArrayCopyReferenceArrayStubTemplateRelativePath);
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
