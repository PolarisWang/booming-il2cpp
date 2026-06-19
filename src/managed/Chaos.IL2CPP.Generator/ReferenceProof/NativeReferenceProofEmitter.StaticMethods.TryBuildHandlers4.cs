using System.Text;
using System.Text.Json;
using System.Runtime.CompilerServices;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{















    [RuntimeSkeletonFamilyPlugin("delegate-closed-target-relay", CodeGenPluginProductLine.NativeReference, 21, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonDelegateClosedTargetRelayHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundDelegateClosedTargetRelayPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.Methods,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundDelegateClosedTargetRelayStub(
                    loweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("static-void-object-sink-no-op", CodeGenPluginProductLine.NativeReference, 22, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticVoidObjectSinkNoOpHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticVoidObjectSinkNoOpStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("static-bool-forwarder", CodeGenPluginProductLine.NativeReference, 23, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticBoolForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticBoolForwarderStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("console-write-line", CodeGenPluginProductLine.NativeReference, 24, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundConsoleWriteLineStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("interface-dispatch-message", CodeGenPluginProductLine.NativeReference, 25, RuntimeSkeletonFamilyPluginKind.PlatformCapability)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonInterfaceDispatchMessageHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundInterfaceDispatchMessagePlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundInterfaceDispatchMessageStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("threading-thread-static-monitor", CodeGenPluginProductLine.NativeReference, 26, RuntimeSkeletonFamilyPluginKind.PlatformCapability)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonThreadingThreadStaticMonitorHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundThreadingThreadStaticMonitorPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundThreadingThreadStaticMonitorStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("marshaling-utf8-export", CodeGenPluginProductLine.NativeReference, 27, RuntimeSkeletonFamilyPluginKind.ImportedBridge)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMarshalingUtf8ExportHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundMarshalingUtf8ExportPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundMarshalingUtf8ExportStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("reflection-interop-closure", CodeGenPluginProductLine.NativeReference, 28, RuntimeSkeletonFamilyPluginKind.ImportedBridge)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonReflectionInteropClosureHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundReflectionInteropClosurePlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundReflectionInteropClosureStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("pinvoke-direct-call", CodeGenPluginProductLine.NativeReference, 29, RuntimeSkeletonFamilyPluginKind.ImportedBridge)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonPInvokeDirectCallHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundPInvokeDirectCallPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundPInvokeDllImportMinimalStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("async-task-factory", CodeGenPluginProductLine.NativeReference, 30, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonAsyncTaskFactoryHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundAsyncTaskFactoryStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("async-get-result", CodeGenPluginProductLine.NativeReference, 31, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonAsyncGetResultHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundAsyncGetResultStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("async-state-machine-no-op", CodeGenPluginProductLine.NativeReference, 32, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonAsyncStateMachineNoOpHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundAsyncStateMachineNoOpStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("array-boxing-reference-array", CodeGenPluginProductLine.NativeReference, 33, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayBoxingReferenceArrayHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundArrayBoxingReferenceArrayPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundArrayBoxingReferenceArrayStub(
                    loweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("array-clear-reference-array", CodeGenPluginProductLine.NativeReference, 34, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayClearReferenceArrayHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundArrayClearReferenceArrayPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundArrayClearReferenceArrayStub(
                    loweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("array-reverse-reference-array", CodeGenPluginProductLine.NativeReference, 35, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayReverseReferenceArrayHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundArrayReverseReferenceArrayPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundArrayReverseReferenceArrayStub(
                    loweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("array-copy-reference-array", CodeGenPluginProductLine.NativeReference, 36, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayCopyReferenceArrayHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundArrayCopyReferenceArrayPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                buildContext.Methods,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundArrayCopyReferenceArrayStub(
                    loweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("static-exception-throw-literal", CodeGenPluginProductLine.NativeReference, 37, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticExceptionThrowLiteralHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticExceptionThrowLiteralStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("static-exception-throw-string-producer", CodeGenPluginProductLine.NativeReference, 38, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticExceptionThrowStringProducerHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticExceptionThrowStringProducerStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

















    [RuntimeSkeletonFamilyPlugin("static-exception-catch-string-return", CodeGenPluginProductLine.NativeReference, 39, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticExceptionCatchStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticExceptionCatchStringReturnStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

}
