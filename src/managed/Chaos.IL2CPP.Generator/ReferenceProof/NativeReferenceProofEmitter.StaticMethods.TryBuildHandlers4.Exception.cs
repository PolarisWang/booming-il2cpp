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

















    [RuntimeSkeletonFamilyPlugin("exception-throw-catch-finally", CodeGenPluginProductLine.NativeReference, 40, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonExceptionThrowCatchFinallyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundExceptionThrowCatchFinallyPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundExceptionThrowCatchFinallyStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }



















    [RuntimeSkeletonFamilyPlugin("nested-exception-throw-catch-finally", CodeGenPluginProductLine.NativeReference, 41, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonNestedExceptionThrowCatchFinallyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundNestedExceptionThrowCatchFinallyPlan(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodPointers,
                out var loweringPlan))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundNestedExceptionThrowCatchFinallyStub(loweringPlan, buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }



















    [RuntimeSkeletonFamilyPlugin("static-string-return-forwarder", CodeGenPluginProductLine.NativeReference, 42, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringReturnForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringReturnForwarderCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-forwarder", CodeGenPluginProductLine.NativeReference, 43, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringForwarderCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-literal-append", CodeGenPluginProductLine.NativeReference, 44, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringLiteralAppendHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringLiteralAppendCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 45, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringForwarderConsoleWriteLineCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-producer-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 46, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-producer-ctor-getter-console-write-line", CodeGenPluginProductLine.NativeReference, 47, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-producer-ctor-render-console-write-line", CodeGenPluginProductLine.NativeReference, 48, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-producer-ctor-instance-call-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 49, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-producer-forwarder-ctor-instance-call-console-write-line", CodeGenPluginProductLine.NativeReference, 50, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-producer-forwarder-ctor-instance-call-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 51, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-producer-ctor-instance-call-forwarder-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 52, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-producer-forwarder-ctor-instance-call-forwarder-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 53, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-producer-ctor-instance-call-forwarder-forwarder-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 54, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-producer-forwarder-ctor-instance-call-forwarder-forwarder-forwarder-console-write-line", CodeGenPluginProductLine.NativeReference, 55, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineCore(buildContext);

}
