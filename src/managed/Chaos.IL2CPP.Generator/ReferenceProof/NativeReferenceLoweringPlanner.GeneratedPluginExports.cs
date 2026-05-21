using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceLoweringPlanner
{
    private readonly record struct NativeReferenceLoweringPlanBuildContext(
        string AssemblyName,
        string EntryPointSubjectId,
        MetadataRegistrationArtifact MetadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> MethodPointers,
        IReadOnlyList<TypedIlMethodArtifact> Methods,
        CodeRegistrationEntry EntryPointRegistration,
        TypedIlMethodArtifact EntryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> EntryPointInstructions);

    private delegate NativeReferenceLoweringPlanArtifact? NativeReferenceLoweringPlanPlugin(
        NativeReferenceLoweringPlanBuildContext buildContext);

    [LoweringPlanPlugin("reflection-interop-closure", CodeGenPluginProductLine.NativeReference, 100)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_ReflectionInteropClosure(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateReflectionInteropClosureLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.EntryPointRegistration,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("marshaling-utf8-export", CodeGenPluginProductLine.NativeReference, 110)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_MarshalingUtf8Export(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateMarshalingUtf8ExportLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.EntryPointRegistration,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("pinvoke-dllimport-minimal", CodeGenPluginProductLine.NativeReference, 120)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_PInvokeDllImportMinimal(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreatePInvokeDllImportMinimalLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                buildContext.EntryPointRegistration,
                buildContext.EntryPointMethod,
                buildContext.EntryPointInstructions,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("reflection-query-minimal", CodeGenPluginProductLine.NativeReference, 130)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_ReflectionQueryMinimal(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateReflectionQueryMinimalLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.EntryPointRegistration,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("delegate-closed-target-relay", CodeGenPluginProductLine.NativeReference, 140)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_DelegateClosedTargetRelay(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateDelegateClosedTargetRelayLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                buildContext.EntryPointRegistration,
                buildContext.EntryPointMethod,
                buildContext.EntryPointInstructions,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("nested-exception", CodeGenPluginProductLine.NativeReference, 150)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_NestedException(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateNestedExceptionLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.EntryPointRegistration,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("exception-throw-catch-finally", CodeGenPluginProductLine.NativeReference, 160)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_ExceptionThrowCatchFinally(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateExceptionThrowCatchFinallyLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.EntryPointRegistration,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("threading-thread-static-monitor", CodeGenPluginProductLine.NativeReference, 170)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_ThreadingThreadStaticMonitor(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateThreadingThreadStaticMonitorLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("async-await-int", CodeGenPluginProductLine.NativeReference, 180)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_AsyncAwaitInt(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateAsyncAwaitIntLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("interface-dispatch-message", CodeGenPluginProductLine.NativeReference, 190)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_InterfaceDispatchMessage(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateInterfaceDispatchMessageLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                buildContext.EntryPointRegistration,
                buildContext.EntryPointMethod,
                buildContext.EntryPointInstructions,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("dispatch-virtual-instance-message", CodeGenPluginProductLine.NativeReference, 200)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_DispatchVirtualInstanceMessage(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateDispatchVirtualInstanceMessageLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                buildContext.EntryPointRegistration,
                buildContext.EntryPointMethod,
                buildContext.EntryPointInstructions,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("array-boxing-reference-array", CodeGenPluginProductLine.NativeReference, 210)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_ArrayBoxingReferenceArray(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateArrayBoxingReferenceArrayLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                buildContext.EntryPointRegistration,
                buildContext.EntryPointMethod,
                buildContext.EntryPointInstructions,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("array-clear-reference-array", CodeGenPluginProductLine.NativeReference, 220)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_ArrayClearReferenceArray(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateArrayClearReferenceArrayLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                buildContext.EntryPointRegistration,
                buildContext.EntryPointMethod,
                buildContext.EntryPointInstructions,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("array-copy-reference-array", CodeGenPluginProductLine.NativeReference, 230)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_ArrayCopyReferenceArray(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateArrayCopyReferenceArrayLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                buildContext.EntryPointRegistration,
                buildContext.EntryPointMethod,
                buildContext.EntryPointInstructions,
                out var plan)
            ? plan
            : null;
    }

    [LoweringPlanPlugin("array-reverse-reference-array", CodeGenPluginProductLine.NativeReference, 240)]
    private static NativeReferenceLoweringPlanArtifact? Plugin_ArrayReverseReferenceArray(NativeReferenceLoweringPlanBuildContext buildContext)
    {
        return TryCreateArrayReverseReferenceArrayLoweringPlan(
                buildContext.AssemblyName,
                buildContext.EntryPointSubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                buildContext.EntryPointRegistration,
                buildContext.EntryPointMethod,
                buildContext.EntryPointInstructions,
                out var plan)
            ? plan
            : null;
    }
}
