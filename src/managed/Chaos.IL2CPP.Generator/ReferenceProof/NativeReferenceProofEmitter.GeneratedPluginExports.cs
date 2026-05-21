namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    [RuntimeSkeletonFamilyPlugin("convert", CodeGenPluginProductLine.NativeReference, 100, RuntimeSkeletonFamilyPluginKind.Hybrid)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_ConvertFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonConvertFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("binding", CodeGenPluginProductLine.NativeReference, 110, RuntimeSkeletonFamilyPluginKind.Hybrid)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_BindingFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonBindingFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("utility", CodeGenPluginProductLine.NativeReference, 120, RuntimeSkeletonFamilyPluginKind.Hybrid)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_UtilityFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonUtilityFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("platform", CodeGenPluginProductLine.NativeReference, 130, RuntimeSkeletonFamilyPluginKind.PlatformCapability)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_PlatformFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonPlatformFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("interop-kernel32-platform", CodeGenPluginProductLine.NativeReference, 135, RuntimeSkeletonFamilyPluginKind.PlatformCapability)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_InteropKernel32PlatformFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonInteropKernel32PlatformCapabilityFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("interop", CodeGenPluginProductLine.NativeReference, 140, RuntimeSkeletonFamilyPluginKind.PlatformCapability)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_InteropFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonInteropFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("native-runtime-event-source-platform", CodeGenPluginProductLine.NativeReference, 145, RuntimeSkeletonFamilyPluginKind.PlatformCapability)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_NativeRuntimeEventSourcePlatformFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonNativeRuntimeEventSourcePlatformCapabilityFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("async", CodeGenPluginProductLine.NativeReference, 150, RuntimeSkeletonFamilyPluginKind.Hybrid)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_AsyncFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonAsyncFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("task-continuation", CodeGenPluginProductLine.NativeReference, 155, RuntimeSkeletonFamilyPluginKind.Hybrid)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_TaskContinuationFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonTaskContinuationFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("array", CodeGenPluginProductLine.NativeReference, 160, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_ArrayFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonArrayFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("collections-kernel", CodeGenPluginProductLine.NativeReference, 165, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_CollectionsKernelFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonCollectionsKernelFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("exception", CodeGenPluginProductLine.NativeReference, 170, RuntimeSkeletonFamilyPluginKind.Hybrid)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_ExceptionFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonExceptionFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("unsafe-managed-invoke", CodeGenPluginProductLine.NativeReference, 180, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_UnsafeManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonUnsafeManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("memory-extensions-managed-invoke", CodeGenPluginProductLine.NativeReference, 190, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_MemoryExtensionsManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("math-kernel", CodeGenPluginProductLine.NativeReference, 185, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_MathKernelFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonMathKernelFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("spanhelpers-kernel", CodeGenPluginProductLine.NativeReference, 200, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_SpanHelpersKernelFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonSpanHelpersKernelFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("buffermemory-kernel", CodeGenPluginProductLine.NativeReference, 205, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_BufferMemoryKernelFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonBufferMemoryKernelFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("collections-managed-invoke", CodeGenPluginProductLine.NativeReference, 216, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_CollectionsManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonCollectionsManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("valuetype-kernel", CodeGenPluginProductLine.NativeReference, 210, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_ValueTypeKernelFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonValueTypeKernelFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("vector-kernel", CodeGenPluginProductLine.NativeReference, 215, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_VectorKernelFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonVectorKernelFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("vector-managed-invoke", CodeGenPluginProductLine.NativeReference, 275, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_VectorManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonVectorManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("array-memorymarshal", CodeGenPluginProductLine.NativeReference, 220, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_ArrayAndMemoryMarshalFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonArrayAndMemoryMarshalFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("marshal-platform", CodeGenPluginProductLine.NativeReference, 225, RuntimeSkeletonFamilyPluginKind.PlatformCapability)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_MarshalPlatformFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonMarshalPlatformCapabilityFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("half-managed-invoke", CodeGenPluginProductLine.NativeReference, 230, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_HalfManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonHalfManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("floating-scalar-managed-invoke", CodeGenPluginProductLine.NativeReference, 240, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_FloatingScalarManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonFloatingScalarManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("primitive-scalar-managed-invoke", CodeGenPluginProductLine.NativeReference, 250, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_PrimitiveScalarManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonPrimitiveScalarManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("wide-numeric-managed-invoke", CodeGenPluginProductLine.NativeReference, 260, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_WideNumericManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonWideNumericManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("calendar-struct-managed-invoke", CodeGenPluginProductLine.NativeReference, 270, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_CalendarStructManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonCalendarStructManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("identity-struct-managed-invoke", CodeGenPluginProductLine.NativeReference, 280, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_IdentityStructManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonIdentityStructManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("companion-managed-invoke", CodeGenPluginProductLine.NativeReference, 290, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_CompanionManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonCompanionManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("datetime-semantic-engine", CodeGenPluginProductLine.NativeReference, 300, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_DateTimeSemanticEngineFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonDateTimeSemanticEngineFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("globalization-datetime-support", CodeGenPluginProductLine.NativeReference, 310, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_GlobalizationDateTimeSupportFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonGlobalizationDateTimeSupportFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("number-managed-invoke", CodeGenPluginProductLine.NativeReference, 320, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_NumberManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonNumberManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("decimal-managed-invoke", CodeGenPluginProductLine.NativeReference, 330, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_DecimalManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonDecimalManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("char-managed-invoke", CodeGenPluginProductLine.NativeReference, 340, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_CharManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonCharManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("enum-managed-invoke", CodeGenPluginProductLine.NativeReference, 350, RuntimeSkeletonFamilyPluginKind.ImportedBridge)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_EnumManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonEnumManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("string-managed-invoke", CodeGenPluginProductLine.NativeReference, 360, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_StringManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("string-micro", CodeGenPluginProductLine.NativeReference, 370, RuntimeSkeletonFamilyPluginKind.Hybrid)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_StringFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonStringFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("convert-like", CodeGenPluginProductLine.NativeReference, 380, RuntimeSkeletonFamilyPluginKind.Hybrid)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_ConvertLikeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonConvertLikeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("threading-atomic-kernel", CodeGenPluginProductLine.NativeReference, 410, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_ThreadingAtomicKernelFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonThreadingAtomicKernelFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("threading-task-managed-invoke", CodeGenPluginProductLine.NativeReference, 420, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_ThreadingTaskManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonThreadingTaskManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("threading-sync-managed-invoke", CodeGenPluginProductLine.NativeReference, 430, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_ThreadingSyncManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonThreadingSyncManagedInvokeFamilyHandler(buildContext);

    [RuntimeSkeletonFamilyPlugin("threading-thread-managed-invoke", CodeGenPluginProductLine.NativeReference, 440, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult Plugin_ThreadingThreadManagedInvokeFamily(RuntimeSkeletonStubBuildContext buildContext) => TryBuildRuntimeSkeletonThreadingThreadManagedInvokeFamilyHandler(buildContext);
}
