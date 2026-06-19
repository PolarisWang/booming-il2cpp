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













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonFloatingScalarManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonFloatingScalarManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonPrimitiveScalarManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonPrimitiveScalarManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonWideNumericManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonWideNumericManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCalendarStructManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCalendarStructManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonIdentityStructManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonIdentityStructManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCompanionManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCompanionManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonDateTimeSemanticEngineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonDateTimeSemanticEngineCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonGlobalizationDateTimeSupportHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonGlobalizationDateTimeSupportCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonNumberManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonNumberManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonDecimalManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonDecimalManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonCharManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonCharManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonEnumManagedInvokeHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonEnumManagedInvokeCore(buildContext);













    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertRuntimeHelperHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertRuntimeHelperCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-int-forwarder", CodeGenPluginProductLine.NativeReference, 0, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeIntForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeIntForwarderCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-bool-identity-forwarder", CodeGenPluginProductLine.NativeReference, 1, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoolIdentityForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoolIdentityForwarderCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-bool-producer-forwarder", CodeGenPluginProductLine.NativeReference, 2, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoolProducerForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoolProducerForwarderCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-primitive", CodeGenPluginProductLine.NativeReference, 8, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikePrimitiveHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikePrimitiveCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-string-char-provider", CodeGenPluginProductLine.NativeReference, 4, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStringCharProviderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeStringCharProviderCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-string-provider-passthrough", CodeGenPluginProductLine.NativeReference, 3, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStringProviderPassthroughHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeStringProviderPassthroughCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-object-char-provider", CodeGenPluginProductLine.NativeReference, 5, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeObjectCharProviderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeObjectCharProviderCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-boxed-value-type-string-instance-call", CodeGenPluginProductLine.NativeReference, 6, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeStringInstanceCallHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeStringInstanceCallCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-boxed-value-type-instance-call", CodeGenPluginProductLine.NativeReference, 13, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeInstanceCallHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeInstanceCallCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-static-string-call-forwarder", CodeGenPluginProductLine.NativeReference, 7, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStaticStringCallForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeStaticStringCallForwarderCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-static-value-type-call-forwarder", CodeGenPluginProductLine.NativeReference, 14, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStaticValueTypeCallForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeStaticValueTypeCallForwarderCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-instance-value-type-call-forwarder", CodeGenPluginProductLine.NativeReference, 15, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeInstanceValueTypeCallForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeInstanceValueTypeCallForwarderCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-value-type-identity-forwarder", CodeGenPluginProductLine.NativeReference, 16, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeValueTypeIdentityForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeValueTypeIdentityForwarderCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-boxed-value-type-char-invalid-cast", CodeGenPluginProductLine.NativeReference, 9, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeCharInvalidCastHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeCharInvalidCastCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-boxed-iconvertible-char-invalid-cast", CodeGenPluginProductLine.NativeReference, 10, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedIConvertibleCharInvalidCastHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoxedIConvertibleCharInvalidCastCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-checked-primitive", CodeGenPluginProductLine.NativeReference, 11, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeCheckedPrimitiveHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeCheckedPrimitiveCore(buildContext);













    [RuntimeSkeletonFamilyPlugin("convert-like-byte-forwarder", CodeGenPluginProductLine.NativeReference, 12, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeByteForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeByteForwarderCore(buildContext);

}
