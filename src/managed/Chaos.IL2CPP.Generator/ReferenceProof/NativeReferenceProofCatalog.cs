using Scriban;

namespace Chaos.IL2CPP.Generator;

internal static class NativeReferenceProofCatalog
{
    public static IReadOnlyList<ICodegenTemplateBundlePlugin> GetRegisteredTemplateBundles() =>
        NativeReferenceTemplateBundleRegistry.Plugins;

    private static readonly Dictionary<NativeReferencePlanKind, string> PlanKindStringValues = new()
    {
        [NativeReferencePlanKind.ManagedAsyncAwaitIntMinimal] = "managed-async.awaitable-int.minimal",
        [NativeReferencePlanKind.ManagedThreadingThreadStaticMonitorMinimal] = "managed-threading.threadstatic-monitor.minimal",
        [NativeReferencePlanKind.ManagedInterfaceDispatchMessageMinimal] = "managed-dispatch.interface-message.minimal",
        [NativeReferencePlanKind.ManagedDispatchVirtualInstanceMessageMinimal] = "managed-dispatch.virtual-instance-message.minimal",
        [NativeReferencePlanKind.ManagedObjectCapturedStateInstanceMessageMinimal] = "managed-object.captured-state-instance-message.minimal",
        [NativeReferencePlanKind.ManagedGenericStaticForwarderCapturedGetterMinimal] = "managed-generic.static-forwarder-captured-getter.minimal",
        [NativeReferencePlanKind.ManagedArraysReverseReferenceArrayMinimal] = "managed-arrays-reverse.reference-array.minimal",
        [NativeReferencePlanKind.ManagedArraysClearReferenceArrayMinimal] = "managed-arrays-clear.reference-array.minimal",
        [NativeReferencePlanKind.ManagedArraysCopyReferenceArrayMinimal] = "managed-arrays-copy.reference-array.minimal",
        [NativeReferencePlanKind.ManagedArraysBoxingReferenceArrayBoxedIntMinimal] = "managed-arrays-boxing.reference-array-boxed-int.minimal",
        [NativeReferencePlanKind.DelegateClosedTargetRelayMinimal] = "managed-delegates.closed-target-relay-message.minimal",
        [NativeReferencePlanKind.NestedExceptionThrowCatchFinallyMinimal] = "managed-exceptions.nested-throw-catch-finally.minimal",
        [NativeReferencePlanKind.ExceptionThrowCatchFinallyMinimal] = "managed-exceptions.throw-catch-finally-message.minimal",
        [NativeReferencePlanKind.ReflectionInteropClosureMinimal] = "managed-reflection-interop.closure.minimal",
        [NativeReferencePlanKind.ReflectionClosedTypeQueryMinimal] = "reflection.closed-type-query.minimal",
        [NativeReferencePlanKind.MarshalingUtf8ExportMinimal] = "interop.marshaling-utf8-export.minimal",
        [NativeReferencePlanKind.InteropPInvokeDirectCallMinimal] = "interop.pinvoke-direct-call.minimal",
        [NativeReferencePlanKind.EngineLogWriteMinimal] = "engine.log-write.minimal",
        [NativeReferencePlanKind.EngineObjectHandleRoundtripMinimal] = "engine.object-handle.roundtrip.minimal",
        [NativeReferencePlanKind.EngineLifecycleCallbackMinimal] = "engine.lifecycle-callback.minimal",
        [NativeReferencePlanKind.EngineHostProofMinimal] = "engine.host-proof.minimal",
    };

    private static readonly Dictionary<string, NativeReferencePlanKind> PlanKindLookup =
        PlanKindStringValues.ToDictionary(kvp => kvp.Value, kvp => kvp.Key, StringComparer.Ordinal);

    public static string Stringify(NativeReferencePlanKind kind) => PlanKindStringValues[kind];

    public static NativeReferencePlanKind Parse(string planKind)
    {
        if (PlanKindLookup.TryGetValue(planKind, out var kind))
            return kind;

        return planKind switch
        {
            "asyncAwaitIntMinimal" => NativeReferencePlanKind.ManagedAsyncAwaitIntMinimal,
            "threadingThreadStaticMonitorMinimal" => NativeReferencePlanKind.ManagedThreadingThreadStaticMonitorMinimal,
            "interfaceDispatchMessage" => NativeReferencePlanKind.ManagedInterfaceDispatchMessageMinimal,
            "dispatchVirtualInstanceMessage" => NativeReferencePlanKind.ManagedDispatchVirtualInstanceMessageMinimal,
            "constructorThenInstanceCall" => NativeReferencePlanKind.ManagedObjectCapturedStateInstanceMessageMinimal,
            "staticCallCtorGetter" => NativeReferencePlanKind.ManagedGenericStaticForwarderCapturedGetterMinimal,
            "arrayReverseReferenceArray" => NativeReferencePlanKind.ManagedArraysReverseReferenceArrayMinimal,
            "arrayClearReferenceArray" => NativeReferencePlanKind.ManagedArraysClearReferenceArrayMinimal,
            "arrayCopyReferenceArray" => NativeReferencePlanKind.ManagedArraysCopyReferenceArrayMinimal,
            "arrayBoxingReferenceArray" => NativeReferencePlanKind.ManagedArraysBoxingReferenceArrayBoxedIntMinimal,
            "delegateClosedTargetRelayMinimal" => NativeReferencePlanKind.DelegateClosedTargetRelayMinimal,
            "nestedExceptionThrowCatchFinallyMinimal" => NativeReferencePlanKind.NestedExceptionThrowCatchFinallyMinimal,
            "exceptionThrowCatchFinallyMinimal" => NativeReferencePlanKind.ExceptionThrowCatchFinallyMinimal,
            "reflectionInteropClosureMinimal" => NativeReferencePlanKind.ReflectionInteropClosureMinimal,
            "reflectionQueryMinimal" => NativeReferencePlanKind.ReflectionClosedTypeQueryMinimal,
            "marshalingUtf8ExportMinimal" => NativeReferencePlanKind.MarshalingUtf8ExportMinimal,
            "pinvokeDllImportMinimal" => NativeReferencePlanKind.InteropPInvokeDirectCallMinimal,
            _ => throw new InvalidOperationException($"unknown native-reference lowering plan kind '{planKind}'"),
        };
    }

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
    public const string RuntimeSkeletonBufferMemoryKernelStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.BufferMemoryKernelStub.cpp.scriban";
    public const string RuntimeSkeletonCollectionsKernelStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.CollectionsKernelStub.cpp.scriban";
    public const string RuntimeSkeletonCollectionsManagedInvokeStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.CollectionsManagedInvokeStub.cpp.scriban";
    public const string RuntimeSkeletonMathKernelStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.MathKernelStub.cpp.scriban";
    public const string RuntimeSkeletonThreadingAtomicStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ThreadingAtomicStub.cpp.scriban";
    public const string RuntimeSkeletonThreadingSyncKernelStubTemplateRelativePath =
        "Templates/NativeReferenceProof.RuntimeSkeleton.ThreadingSyncKernelStub.cpp.scriban";

    private static readonly IReadOnlyDictionary<NativeReferencePlanKind, string> TemplateRelativePathByPlanKind =
        new Dictionary<NativeReferencePlanKind, string>
        {
            [NativeReferencePlanKind.ManagedAsyncAwaitIntMinimal] = AsyncAwaitIntGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.ManagedThreadingThreadStaticMonitorMinimal] = ThreadingThreadStaticMonitorGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.ManagedInterfaceDispatchMessageMinimal] = InterfaceDispatchMessageGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.ManagedDispatchVirtualInstanceMessageMinimal] = DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.ManagedObjectCapturedStateInstanceMessageMinimal] = GeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.ManagedGenericStaticForwarderCapturedGetterMinimal] = StaticCallCtorGetterGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.ManagedArraysReverseReferenceArrayMinimal] = ArrayReverseReferenceArrayGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.ManagedArraysClearReferenceArrayMinimal] = ArrayClearReferenceArrayGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.ManagedArraysCopyReferenceArrayMinimal] = ArrayCopyReferenceArrayGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.ManagedArraysBoxingReferenceArrayBoxedIntMinimal] = ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.DelegateClosedTargetRelayMinimal] = DelegateClosedTargetRelayGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.NestedExceptionThrowCatchFinallyMinimal] = NestedExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.ExceptionThrowCatchFinallyMinimal] = ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.ReflectionInteropClosureMinimal] = ReflectionInteropClosureGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.ReflectionClosedTypeQueryMinimal] = ReflectionQueryMinimalGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.MarshalingUtf8ExportMinimal] = MarshalingUtf8ExportGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.InteropPInvokeDirectCallMinimal] = PInvokeDllImportMinimalGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.EngineLogWriteMinimal] = EngineLogWriteGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.EngineObjectHandleRoundtripMinimal] = EngineObjectHandleGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.EngineLifecycleCallbackMinimal] = EngineLifecycleCallbackGeneratedTranslationUnitTemplateRelativePath,
            [NativeReferencePlanKind.EngineHostProofMinimal] = EngineHostProofGeneratedTranslationUnitTemplateRelativePath,
        };

    public static Template GetTemplateForPlan(string planKind)
    {
        return ScribanTemplateRenderer.LoadTemplate(GetRequiredTemplateRelativePath(planKind));
    }


    public static string GetRequiredTemplateRelativePath(string planKind)
    {
        return TemplateRelativePathByPlanKind[Parse(planKind)];
    }
}
