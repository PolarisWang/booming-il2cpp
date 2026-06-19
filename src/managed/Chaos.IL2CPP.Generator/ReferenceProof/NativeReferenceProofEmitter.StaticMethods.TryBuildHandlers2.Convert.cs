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
