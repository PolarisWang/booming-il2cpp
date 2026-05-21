namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Canonical plan kinds for native reference lowering.
/// These replace the previous public const string pattern in
/// <see cref="NativeReferenceProofCatalog"/> for type-safe comparisons.
///
/// The Artifact model's PlanKind property remains string for JSON
/// serialization compatibility; use Parse/Stringify to convert.
/// </summary>
internal enum NativeReferencePlanKind
{
    ManagedAsyncAwaitIntMinimal,
    ManagedThreadingThreadStaticMonitorMinimal,
    ManagedInterfaceDispatchMessageMinimal,
    ManagedDispatchVirtualInstanceMessageMinimal,
    ManagedObjectCapturedStateInstanceMessageMinimal,
    ManagedGenericStaticForwarderCapturedGetterMinimal,
    ManagedArraysReverseReferenceArrayMinimal,
    ManagedArraysClearReferenceArrayMinimal,
    ManagedArraysCopyReferenceArrayMinimal,
    ManagedArraysBoxingReferenceArrayBoxedIntMinimal,
    DelegateClosedTargetRelayMinimal,
    NestedExceptionThrowCatchFinallyMinimal,
    ExceptionThrowCatchFinallyMinimal,
    ReflectionInteropClosureMinimal,
    ReflectionClosedTypeQueryMinimal,
    MarshalingUtf8ExportMinimal,
    InteropPInvokeDirectCallMinimal,
    EngineLogWriteMinimal,
    EngineObjectHandleRoundtripMinimal,
    EngineLifecycleCallbackMinimal,
    EngineHostProofMinimal,
}
