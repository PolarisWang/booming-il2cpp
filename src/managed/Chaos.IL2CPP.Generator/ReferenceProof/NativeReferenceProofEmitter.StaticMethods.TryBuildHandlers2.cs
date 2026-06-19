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











    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStringManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStringManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonUnsafeManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonUnsafeManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCollectionsManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCollectionsManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCollectionsKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCollectionsKernelFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonBufferMemoryKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonBufferMemoryKernelFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonSpanHelpersKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonSpanHelpersKernelFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMathKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonMathKernelFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonValueTypeKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonValueTypeKernelFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonVectorKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonVectorKernelFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonVectorManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonVectorManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayAndMemoryMarshalFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonArrayAndMemoryMarshalFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMarshalPlatformCapabilityFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonMarshalPlatformCapabilityFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonNativeRuntimeEventSourcePlatformCapabilityFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonNativeRuntimeEventSourcePlatformCapabilityFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonHalfManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonHalfManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonFloatingScalarManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonFloatingScalarManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonPrimitiveScalarManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonPrimitiveScalarManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonWideNumericManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonWideNumericManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCalendarStructManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCalendarStructManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIdentityStructManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonIdentityStructManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCompanionManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCompanionManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonDateTimeSemanticEngineFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonDateTimeSemanticEngineFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonGlobalizationDateTimeSupportFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonGlobalizationDateTimeSupportFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonNumberManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonNumberManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonDecimalManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonDecimalManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCharManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCharManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonEnumManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonEnumManagedInvokeFamilyCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStringManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStringManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonUnsafeManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonUnsafeManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonMemoryExtensionsManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonSpanHelpersKernelHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonSpanHelpersKernelCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonValueTypeKernelHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonValueTypeKernelCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonVectorKernelHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonVectorKernelCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayAndMemoryMarshalHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonArrayAndMemoryMarshalCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonHalfManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonHalfManagedInvokeCore(buildContext);

}
