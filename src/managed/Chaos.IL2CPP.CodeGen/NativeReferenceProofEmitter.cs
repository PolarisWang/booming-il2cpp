using System.Text;
using System.Text.Json;
using System.Runtime.CompilerServices;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed class NativeReferenceProofEmitter
{
    private sealed record AsyncManagedTypeShape(
        string ManagedType,
        string CppType,
        string RuntimeReturnInitializer,
        string? LiteralOp,
        string LiteralExpectation)
    {
        public bool SupportsLiteralLoading => !string.IsNullOrWhiteSpace(LiteralOp);
    }

    private sealed record AsyncTaskFamilyShape(
        string ManagedResultType,
        AsyncManagedTypeShape ResultTypeShape)
    {
        public string TaskReturnType => $"System.Threading.Tasks.Task<{ManagedResultType}>";
        public string BuilderType => $"System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<{ManagedResultType}>";
        public string TaskType => $"System.Private.CoreLib/System.Threading.Tasks.Task<{ManagedResultType}>";
        public string AwaiterReturnType => $"System.Runtime.CompilerServices.TaskAwaiter<{ManagedResultType}>";
        public string AwaiterSubjectType => $"System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter<{ManagedResultType}>";
        public string BuilderCreateCallee => $"{BuilderType}::Create:System.Runtime.CompilerServices.AsyncTaskMethodBuilder<{ManagedResultType}>()";
        public string BuilderStartPrefix => $"{BuilderType}::Start<";
        public string BuilderGetTaskCallee => $"{BuilderType}::get_Task:{TaskReturnType}()";
        public string BuilderAwaitUnsafeOnCompletedPrefix => $"{BuilderType}::AwaitUnsafeOnCompleted<";
        public string BuilderSetResultCallee => $"{BuilderType}::SetResult:System.Void({ManagedResultType})";
        public string BuilderSetExceptionCallee => $"{BuilderType}::SetException:System.Void(System.Exception)";
        public string BuilderSetStateMachineCallee => $"{BuilderType}::SetStateMachine:System.Void(System.Runtime.CompilerServices.IAsyncStateMachine)";
        public string TaskGetAwaiterCallee => $"{TaskType}::GetAwaiter:{AwaiterReturnType}()";
        public string AwaiterGetResultCallee => $"{AwaiterSubjectType}::GetResult:{ManagedResultType}()";
        public string DisplayName => $"Task<{ManagedResultType}>";
    }

    private sealed record AsyncTaskFactoryMethodShape(
        AsyncTaskFamilyShape Family,
        IReadOnlyList<AsyncManagedTypeShape> ParameterTypeShapes);

    private sealed record AsyncGetResultWrapperShape(
        AsyncTaskFamilyShape ProducerFamilyShape,
        IReadOnlyList<AsyncManagedTypeShape> ExpectedLiteralParameterShapes);

    private sealed record RuntimeSkeletonMethodEmission(
        int MethodId,
        string SubjectId,
        string StubName,
        int DispatchSlot,
        string StubDeclaration,
        string StubDefinition);

    private sealed record RuntimeSkeletonSupportedMethodDispatch(
        int MethodId,
        string SubjectId,
        int DispatchSlot);

    private sealed record RuntimeSkeletonUnsupportedMethodEmission(
        string SubjectId,
        string ReasonCode);

    private sealed record RuntimeSkeletonExternalMethodStubDeclaration(
        string SubjectId,
        string StubName,
        string Declaration);

    private sealed record RuntimeSkeletonPageEmission(
        AuditTranslationUnitPageArtifact Page,
        string Contents,
        IReadOnlyList<RuntimeSkeletonSupportedMethodDispatch> SupportedMethods,
        IReadOnlyList<RuntimeSkeletonUnsupportedMethodEmission> UnsupportedMethods);

    private sealed record RuntimeSkeletonStubBuildContext(
        NativeReferenceLoweringPlanArtifact LoweringPlan,
        MetadataRegistrationArtifact MetadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> MethodPointers,
        IReadOnlyList<TypedIlMethodArtifact> Methods,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> MethodsBySubjectId,
        IReadOnlyDictionary<string, string> MethodStubNamesBySubjectId,
        RuntimeSkeletonPageSupportBuilder PageSupportBuilder,
        string StubName,
        string SubjectId);

    private enum RuntimeSkeletonFamilyHandlerMatchKind
    {
        NoMatch,
        Unsupported,
        Match,
    }

    private sealed record RuntimeSkeletonFamilyHandlerResult(
        RuntimeSkeletonFamilyHandlerMatchKind MatchKind,
        string? StubDefinition,
        string? UnsupportedReason)
    {
        public static RuntimeSkeletonFamilyHandlerResult NoMatch { get; } =
            new(RuntimeSkeletonFamilyHandlerMatchKind.NoMatch, null, null);

        public static RuntimeSkeletonFamilyHandlerResult CreateMatch(string stubDefinition) =>
            new(RuntimeSkeletonFamilyHandlerMatchKind.Match, stubDefinition, null);

        public static RuntimeSkeletonFamilyHandlerResult CreateUnsupported(string unsupportedReason) =>
            new(RuntimeSkeletonFamilyHandlerMatchKind.Unsupported, null, unsupportedReason);
    }

    private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext);

    private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonConvertFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext);

    private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonBindingFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext);

    private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonUtilityFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext);

    private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonPlatformFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext);

    private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonInteropFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext);

    private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonAsyncFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext);

    private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonArrayFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext);

    private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonExceptionFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext);

    private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonStringFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext);

    private static readonly RuntimeSkeletonFamilyHandler[] RuntimeSkeletonFamilyHandlers =
    [
        TryBuildRuntimeSkeletonConvertFamilyHandler,
        TryBuildRuntimeSkeletonBindingFamilyHandler,
        TryBuildRuntimeSkeletonUtilityFamilyHandler,
        TryBuildRuntimeSkeletonPlatformFamilyHandler,
        TryBuildRuntimeSkeletonInteropFamilyHandler,
        TryBuildRuntimeSkeletonAsyncFamilyHandler,
        TryBuildRuntimeSkeletonArrayFamilyHandler,
        TryBuildRuntimeSkeletonExceptionFamilyHandler,
        TryBuildRuntimeSkeletonStringFamilyHandler,
        TryBuildRuntimeSkeletonConvertLikeFamilyHandler,
    ];

    private static readonly RuntimeSkeletonConvertFamilyHandler[] RuntimeSkeletonConvertFamilyHandlers =
    [
        TryBuildRuntimeSkeletonConvertIntForwarderHandler,
        TryBuildRuntimeSkeletonConvertBoolIdentityForwarderHandler,
        TryBuildRuntimeSkeletonConvertBoolProducerForwarderHandler,
        TryBuildRuntimeSkeletonConvertPrimitiveHandler,
        TryBuildRuntimeSkeletonConvertCheckedByteHandler,
        TryBuildRuntimeSkeletonConvertCheckedCharHandler,
        TryBuildRuntimeSkeletonConvertByteForwarderHandler,
    ];

    private static readonly RuntimeSkeletonBindingFamilyHandler[] RuntimeSkeletonBindingFamilyHandlers =
    [
        TryBuildRuntimeSkeletonConstructorFieldSetterHandler,
        TryBuildRuntimeSkeletonFieldBackedStringReturnHandler,
        TryBuildRuntimeSkeletonFieldArgumentStringReturnHandler,
        TryBuildRuntimeSkeletonFieldGetterStringReturnHandler,
        TryBuildRuntimeSkeletonDelegateClosedTargetRelayHandler,
    ];

    private static readonly RuntimeSkeletonUtilityFamilyHandler[] RuntimeSkeletonUtilityFamilyHandlers =
    [
        TryBuildRuntimeSkeletonStaticVoidObjectSinkNoOpHandler,
        TryBuildRuntimeSkeletonStaticBoolForwarderHandler,
        TryBuildRuntimeSkeletonConsoleWriteLineHandler,
    ];

    private static readonly RuntimeSkeletonPlatformFamilyHandler[] RuntimeSkeletonPlatformFamilyHandlers =
    [
        TryBuildRuntimeSkeletonInterfaceDispatchMessageHandler,
        TryBuildRuntimeSkeletonThreadingThreadStaticMonitorHandler,
    ];

    private static readonly RuntimeSkeletonInteropFamilyHandler[] RuntimeSkeletonInteropFamilyHandlers =
    [
        TryBuildRuntimeSkeletonMarshalingUtf8ExportHandler,
        TryBuildRuntimeSkeletonReflectionInteropClosureHandler,
        TryBuildRuntimeSkeletonPInvokeDirectCallHandler,
    ];

    private static readonly RuntimeSkeletonAsyncFamilyHandler[] RuntimeSkeletonAsyncFamilyHandlers =
    [
        TryBuildRuntimeSkeletonAsyncTaskFactoryHandler,
        TryBuildRuntimeSkeletonAsyncGetResultHandler,
        TryBuildRuntimeSkeletonAsyncStateMachineNoOpHandler,
    ];

    private static readonly RuntimeSkeletonArrayFamilyHandler[] RuntimeSkeletonArrayFamilyHandlers =
    [
        TryBuildRuntimeSkeletonArrayBoxingReferenceArrayHandler,
        TryBuildRuntimeSkeletonArrayClearReferenceArrayHandler,
        TryBuildRuntimeSkeletonArrayReverseReferenceArrayHandler,
        TryBuildRuntimeSkeletonArrayCopyReferenceArrayHandler,
    ];

    private static readonly RuntimeSkeletonExceptionFamilyHandler[] RuntimeSkeletonExceptionFamilyHandlers =
    [
        TryBuildRuntimeSkeletonStaticExceptionThrowLiteralHandler,
        TryBuildRuntimeSkeletonStaticExceptionThrowStringProducerHandler,
        TryBuildRuntimeSkeletonStaticExceptionCatchStringReturnHandler,
        TryBuildRuntimeSkeletonExceptionThrowCatchFinallyHandler,
        TryBuildRuntimeSkeletonNestedExceptionThrowCatchFinallyHandler,
    ];

    private static readonly RuntimeSkeletonStringFamilyHandler[] RuntimeSkeletonStringFamilyHandlers =
    [
        TryBuildRuntimeSkeletonStaticStringReturnForwarderHandler,
        TryBuildRuntimeSkeletonStaticStringForwarderHandler,
        TryBuildRuntimeSkeletonStaticStringLiteralAppendHandler,
        TryBuildRuntimeSkeletonStaticStringForwarderConsoleWriteLineHandler,
        TryBuildRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineHandler,
        TryBuildRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineHandler,
        TryBuildRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineHandler,
        TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineHandler,
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineHandler,
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineHandler,
        TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineHandler,
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineHandler,
        TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineHandler,
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineHandler,
        TryBuildRuntimeSkeletonStaticLiteralStringReturnHandler,
        TryBuildRuntimeSkeletonStaticResourceKeyStringReturnHandler,
        TryBuildRuntimeSkeletonStaticStringProducerConsoleWriteLineHandler,
        TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableHandler,
        TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableHandler,
    ];

    private sealed class RuntimeSkeletonPageSupportBuilder
    {
        private readonly Dictionary<string, int> fieldBindingDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> fieldBindingDescriptors = [];
        private readonly Dictionary<string, int> typeDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> typeDescriptors = [];
        private readonly Dictionary<string, int> constructorFieldSetterDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> constructorFieldSetterDescriptors = [];
        private readonly Dictionary<string, int> fieldGetterStringReturnDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> fieldGetterStringReturnDescriptors = [];
        private readonly Dictionary<string, int> fieldBackedStringReturnDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> fieldBackedStringReturnDescriptors = [];
        private readonly Dictionary<string, int> fieldArgumentStringReturnDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> fieldArgumentStringReturnDescriptors = [];
        private readonly Dictionary<string, int> staticCallCtorGetterDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> staticCallCtorGetterDescriptors = [];
        private readonly Dictionary<string, int> staticStringLiteralAppendDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> staticStringLiteralAppendDescriptors = [];
        private readonly Dictionary<string, int> constructorThenInstanceCallDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> constructorThenInstanceCallDescriptors = [];
        private readonly Dictionary<string, int> delegateClosedTargetRelayDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> delegateClosedTargetRelayDescriptors = [];
        private readonly Dictionary<string, int> arrayBoxingReferenceArrayDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> arrayBoxingReferenceArrayDescriptors = [];
        private readonly Dictionary<string, int> arrayReverseReferenceArrayDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> arrayReverseReferenceArrayDescriptors = [];
        private readonly Dictionary<string, int> arrayClearReferenceArrayDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> arrayClearReferenceArrayDescriptors = [];
        private readonly Dictionary<string, int> arrayCopyReferenceArrayDescriptorIndices = new(StringComparer.Ordinal);
        private readonly List<string> arrayCopyReferenceArrayDescriptors = [];

        public int GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            string assemblyNameLiteral,
            string referenceTypeToken,
            string capturedFieldToken)
        {
            var key = string.Join("|", assemblyNameLiteral, referenceTypeToken, capturedFieldToken);
            if (fieldBindingDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = fieldBindingDescriptors.Count;
            fieldBindingDescriptorIndices.Add(key, descriptorIndex);
            fieldBindingDescriptors.Add($"    {{ {assemblyNameLiteral}, {referenceTypeToken}, {capturedFieldToken} }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonTypeDescriptor(
            string assemblyNameLiteral,
            string typeToken)
        {
            var key = string.Join("|", assemblyNameLiteral, typeToken);
            if (typeDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = typeDescriptors.Count;
            typeDescriptorIndices.Add(key, descriptorIndex);
            typeDescriptors.Add($"    {{ {assemblyNameLiteral}, {typeToken} }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonConstructorFieldSetterDescriptor(int fieldBindingIndex)
        {
            var key = fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture);
            if (constructorFieldSetterDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = constructorFieldSetterDescriptors.Count;
            constructorFieldSetterDescriptorIndices.Add(key, descriptorIndex);
            constructorFieldSetterDescriptors.Add($"    {{ {fieldBindingIndex}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonFieldGetterStringReturnDescriptor(int fieldBindingIndex)
        {
            var key = fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture);
            if (fieldGetterStringReturnDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = fieldGetterStringReturnDescriptors.Count;
            fieldGetterStringReturnDescriptorIndices.Add(key, descriptorIndex);
            fieldGetterStringReturnDescriptors.Add($"    {{ {fieldBindingIndex}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonFieldBackedStringReturnDescriptor(
            int fieldBindingIndex,
            string concatIcallLiteral,
            string stringConcatTripleIcallLiteral,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                stringConcatTripleIcallLiteral,
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (fieldBackedStringReturnDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = fieldBackedStringReturnDescriptors.Count;
            fieldBackedStringReturnDescriptorIndices.Add(key, descriptorIndex);
            fieldBackedStringReturnDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {concatIcallLiteral}, {stringConcatTripleIcallLiteral}, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonFieldArgumentStringReturnDescriptor(
            int fieldBindingIndex,
            string concatIcallLiteral,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (fieldArgumentStringReturnDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = fieldArgumentStringReturnDescriptors.Count;
            fieldArgumentStringReturnDescriptorIndices.Add(key, descriptorIndex);
            fieldArgumentStringReturnDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {concatIcallLiteral}, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonStaticCallCtorGetterDescriptor(
            int fieldBindingIndex,
            string consoleWriteLineStringIcallLiteral,
            string echoLiteral,
            int echoLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                consoleWriteLineStringIcallLiteral,
                echoLiteral,
                echoLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (staticCallCtorGetterDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = staticCallCtorGetterDescriptors.Count;
            staticCallCtorGetterDescriptorIndices.Add(key, descriptorIndex);
            staticCallCtorGetterDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {consoleWriteLineStringIcallLiteral}, {echoLiteral}, {echoLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonStaticStringLiteralAppendDescriptor(
            string concatIcallLiteral,
            string appendedLiteral,
            int appendedLiteralByteCount)
        {
            var key = string.Join(
                "|",
                concatIcallLiteral,
                appendedLiteral,
                appendedLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (staticStringLiteralAppendDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = staticStringLiteralAppendDescriptors.Count;
            staticStringLiteralAppendDescriptorIndices.Add(key, descriptorIndex);
            staticStringLiteralAppendDescriptors.Add(
                $"    {{ {concatIcallLiteral}, {appendedLiteral}, {appendedLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonConstructorThenInstanceCallDescriptor(
            int fieldBindingIndex,
            string concatIcallLiteral,
            string stringConcatTripleIcallLiteral,
            string consoleWriteLineStringIcallLiteral,
            string constructorLiteral,
            int constructorLiteralByteCount,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                stringConcatTripleIcallLiteral,
                consoleWriteLineStringIcallLiteral,
                constructorLiteral,
                constructorLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (constructorThenInstanceCallDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = constructorThenInstanceCallDescriptors.Count;
            constructorThenInstanceCallDescriptorIndices.Add(key, descriptorIndex);
            constructorThenInstanceCallDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {concatIcallLiteral}, {stringConcatTripleIcallLiteral}, {consoleWriteLineStringIcallLiteral}, {constructorLiteral}, {constructorLiteralByteCount}u, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonDelegateClosedTargetRelayDescriptor(
            int fieldBindingIndex,
            string concatIcallLiteral,
            string consoleWriteLineStringIcallLiteral,
            string constructorLiteral,
            int constructorLiteralByteCount,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount,
            string trailingLiteral,
            int trailingLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                consoleWriteLineStringIcallLiteral,
                constructorLiteral,
                constructorLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                trailingLiteral,
                trailingLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (delegateClosedTargetRelayDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = delegateClosedTargetRelayDescriptors.Count;
            delegateClosedTargetRelayDescriptorIndices.Add(key, descriptorIndex);
            delegateClosedTargetRelayDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {concatIcallLiteral}, {consoleWriteLineStringIcallLiteral}, {constructorLiteral}, {constructorLiteralByteCount}u, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u, {trailingLiteral}, {trailingLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonArrayBoxingReferenceArrayDescriptor(
            int fieldBindingIndex,
            int boxedValueTypeIndex,
            string concatIcallLiteral,
            string stringConcatTripleIcallLiteral,
            string consoleWriteLineStringIcallLiteral,
            int boxedInt32Value,
            string constructorLiteral,
            int constructorLiteralByteCount,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                boxedValueTypeIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                stringConcatTripleIcallLiteral,
                consoleWriteLineStringIcallLiteral,
                boxedInt32Value.ToString(System.Globalization.CultureInfo.InvariantCulture),
                constructorLiteral,
                constructorLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (arrayBoxingReferenceArrayDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = arrayBoxingReferenceArrayDescriptors.Count;
            arrayBoxingReferenceArrayDescriptorIndices.Add(key, descriptorIndex);
            arrayBoxingReferenceArrayDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {boxedValueTypeIndex}u, {concatIcallLiteral}, {stringConcatTripleIcallLiteral}, {consoleWriteLineStringIcallLiteral}, {boxedInt32Value}, {constructorLiteral}, {constructorLiteralByteCount}u, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonArrayCopyReferenceArrayDescriptor(
            int fieldBindingIndex,
            string concatIcallLiteral,
            string stringConcatTripleIcallLiteral,
            string consoleWriteLineStringIcallLiteral,
            int sourceArrayLength,
            int targetArrayLength,
            int sourceStoreIndex,
            int sourceArrayIndex,
            int targetArrayIndex,
            int targetReadIndex,
            int copyLength,
            string constructorLiteral,
            int constructorLiteralByteCount,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                stringConcatTripleIcallLiteral,
                consoleWriteLineStringIcallLiteral,
                sourceArrayLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                targetArrayLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                sourceStoreIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                sourceArrayIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                targetArrayIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                targetReadIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                copyLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                constructorLiteral,
                constructorLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (arrayCopyReferenceArrayDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = arrayCopyReferenceArrayDescriptors.Count;
            arrayCopyReferenceArrayDescriptorIndices.Add(key, descriptorIndex);
            arrayCopyReferenceArrayDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {concatIcallLiteral}, {stringConcatTripleIcallLiteral}, {consoleWriteLineStringIcallLiteral}, {sourceArrayLength}u, {targetArrayLength}u, {sourceStoreIndex}u, {sourceArrayIndex}u, {targetArrayIndex}u, {targetReadIndex}u, {copyLength}u, {constructorLiteral}, {constructorLiteralByteCount}u, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonArrayClearReferenceArrayDescriptor(
            int stringTypeIndex,
            string concatIcallLiteral,
            string consoleWriteLineStringIcallLiteral,
            int arrayLength,
            int arrayStoreIndex,
            int clearStartIndex,
            int clearLength,
            int arrayReadIndex,
            string storedLiteral,
            int storedLiteralByteCount,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                stringTypeIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                consoleWriteLineStringIcallLiteral,
                arrayLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                arrayStoreIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                clearStartIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                clearLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                arrayReadIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                storedLiteral,
                storedLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (arrayClearReferenceArrayDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = arrayClearReferenceArrayDescriptors.Count;
            arrayClearReferenceArrayDescriptorIndices.Add(key, descriptorIndex);
            arrayClearReferenceArrayDescriptors.Add(
                $"    {{ {stringTypeIndex}u, {concatIcallLiteral}, {consoleWriteLineStringIcallLiteral}, {arrayLength}u, {arrayStoreIndex}u, {clearStartIndex}u, {clearLength}u, {arrayReadIndex}u, {storedLiteral}, {storedLiteralByteCount}u, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public int GetOrAddRuntimeSkeletonArrayReverseReferenceArrayDescriptor(
            int fieldBindingIndex,
            string concatIcallLiteral,
            string stringConcatTripleIcallLiteral,
            string consoleWriteLineStringIcallLiteral,
            int arrayLength,
            int arrayStoreIndex,
            int reverseStartIndex,
            int reverseLength,
            int arrayReadIndex,
            string constructorLiteral,
            int constructorLiteralByteCount,
            string messagePrefixLiteral,
            int messagePrefixLiteralByteCount,
            string messageSuffixLiteral,
            int messageSuffixLiteralByteCount)
        {
            var key = string.Join(
                "|",
                fieldBindingIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                concatIcallLiteral,
                stringConcatTripleIcallLiteral,
                consoleWriteLineStringIcallLiteral,
                arrayLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                arrayStoreIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                reverseStartIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                reverseLength.ToString(System.Globalization.CultureInfo.InvariantCulture),
                arrayReadIndex.ToString(System.Globalization.CultureInfo.InvariantCulture),
                constructorLiteral,
                constructorLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messagePrefixLiteral,
                messagePrefixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture),
                messageSuffixLiteral,
                messageSuffixLiteralByteCount.ToString(System.Globalization.CultureInfo.InvariantCulture));
            if (arrayReverseReferenceArrayDescriptorIndices.TryGetValue(key, out var descriptorIndex))
            {
                return descriptorIndex;
            }

            descriptorIndex = arrayReverseReferenceArrayDescriptors.Count;
            arrayReverseReferenceArrayDescriptorIndices.Add(key, descriptorIndex);
            arrayReverseReferenceArrayDescriptors.Add(
                $"    {{ {fieldBindingIndex}u, {concatIcallLiteral}, {stringConcatTripleIcallLiteral}, {consoleWriteLineStringIcallLiteral}, {arrayLength}u, {arrayStoreIndex}u, {reverseStartIndex}u, {reverseLength}u, {arrayReadIndex}u, {constructorLiteral}, {constructorLiteralByteCount}u, {messagePrefixLiteral}, {messagePrefixLiteralByteCount}u, {messageSuffixLiteral}, {messageSuffixLiteralByteCount}u }},");
            return descriptorIndex;
        }

        public string[] GetFieldBindingDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                fieldBindingDescriptors,
                "    { nullptr, 0u, 0u },");
        }

        public string[] GetTypeDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                typeDescriptors,
                "    { nullptr, 0u },");
        }

        public string[] GetConstructorFieldSetterDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                constructorFieldSetterDescriptors,
                "    { 0u },");
        }

        public string[] GetFieldGetterStringReturnDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                fieldGetterStringReturnDescriptors,
                "    { 0u },");
        }

        public string[] GetFieldBackedStringReturnDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                fieldBackedStringReturnDescriptors,
                "    { 0u, nullptr, nullptr, nullptr, 0u, nullptr, 0u },");
        }

        public string[] GetFieldArgumentStringReturnDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                fieldArgumentStringReturnDescriptors,
                "    { 0u, nullptr, nullptr, 0u },");
        }

        public string[] GetStaticCallCtorGetterDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                staticCallCtorGetterDescriptors,
                "    { 0u, nullptr, nullptr, 0u },");
        }

        public string[] GetStaticStringLiteralAppendDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                staticStringLiteralAppendDescriptors,
                "    { nullptr, nullptr, 0u },");
        }

        public string[] GetConstructorThenInstanceCallDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                constructorThenInstanceCallDescriptors,
                "    { 0u, nullptr, nullptr, nullptr, nullptr, 0u, nullptr, 0u, nullptr, 0u },");
        }

        public string[] GetDelegateClosedTargetRelayDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                delegateClosedTargetRelayDescriptors,
                "    { 0u, nullptr, nullptr, nullptr, 0u, nullptr, 0u, nullptr, 0u, nullptr, 0u },");
        }

        public string[] GetArrayBoxingReferenceArrayDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                arrayBoxingReferenceArrayDescriptors,
                "    { 0u, 0u, nullptr, nullptr, nullptr, 0, nullptr, 0u, nullptr, 0u, nullptr, 0u },");
        }

        public string[] GetArrayCopyReferenceArrayDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                arrayCopyReferenceArrayDescriptors,
                "    { 0u, nullptr, nullptr, nullptr, 0u, 0u, 0u, 0u, 0u, 0u, 0u, nullptr, 0u, nullptr, 0u, nullptr, 0u },");
        }

        public string[] GetArrayClearReferenceArrayDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                arrayClearReferenceArrayDescriptors,
                "    { 0u, nullptr, nullptr, 0u, 0u, 0u, 0u, 0u, nullptr, 0u, nullptr, 0u },");
        }

        public string[] GetArrayReverseReferenceArrayDescriptors()
        {
            return GetDescriptorArrayOrPlaceholder(
                arrayReverseReferenceArrayDescriptors,
                "    { 0u, nullptr, nullptr, nullptr, 0u, 0u, 0u, 0u, 0u, nullptr, 0u, nullptr, 0u, nullptr, 0u },");
        }

        public int FieldBindingDescriptorCount => fieldBindingDescriptors.Count;

        public int TypeDescriptorCount => typeDescriptors.Count;

        public int ConstructorFieldSetterDescriptorCount => constructorFieldSetterDescriptors.Count;

        public int FieldGetterStringReturnDescriptorCount => fieldGetterStringReturnDescriptors.Count;

        public int FieldBackedStringReturnDescriptorCount => fieldBackedStringReturnDescriptors.Count;

        public int FieldArgumentStringReturnDescriptorCount => fieldArgumentStringReturnDescriptors.Count;

        public int StaticCallCtorGetterDescriptorCount => staticCallCtorGetterDescriptors.Count;

        public int StaticStringLiteralAppendDescriptorCount => staticStringLiteralAppendDescriptors.Count;

        public int ConstructorThenInstanceCallDescriptorCount => constructorThenInstanceCallDescriptors.Count;

        public int DelegateClosedTargetRelayDescriptorCount => delegateClosedTargetRelayDescriptors.Count;

        public int ArrayBoxingReferenceArrayDescriptorCount => arrayBoxingReferenceArrayDescriptors.Count;

        public int ArrayReverseReferenceArrayDescriptorCount => arrayReverseReferenceArrayDescriptors.Count;

        public int ArrayClearReferenceArrayDescriptorCount => arrayClearReferenceArrayDescriptors.Count;

        public int ArrayCopyReferenceArrayDescriptorCount => arrayCopyReferenceArrayDescriptors.Count;

        private static string[] GetDescriptorArrayOrPlaceholder(List<string> descriptors, string placeholder)
        {
            return descriptors.Count > 0
                ? [.. descriptors]
                : [placeholder];
        }
    }

    private sealed record AssemblyFullClosureRuntimeSkeletonEmission(
        IReadOnlyList<NativeReferenceGeneratedSource> GeneratedSources,
        IReadOnlyList<NativeReferenceGeneratedArtifactRef> GeneratedArtifacts,
        IReadOnlyList<AuditTranslationUnitPageArtifact> TranslationUnitPages,
        int TranslationUnitMethodCount,
        string? PreferredAssemblyDispatchSubjectId);

    private sealed class MetadataRegistrationLookup
    {
        private static readonly IReadOnlyList<MetadataRegistrationEntry> EmptyRegistrations = [];
        private readonly Dictionary<string, List<MetadataRegistrationEntry>> registrationsByKind;
        private readonly Dictionary<string, Dictionary<string, MetadataRegistrationEntry>> registrationsByKindAndSubjectId;
        private readonly Dictionary<string, Dictionary<string, uint>> metadataTokensByKindAndSubjectId;
        private readonly HashSet<string> concreteTypeSubjectIds;

        public MetadataRegistrationLookup(IReadOnlyList<MetadataRegistrationEntry> registrations)
        {
            registrationsByKind = new Dictionary<string, List<MetadataRegistrationEntry>>(StringComparer.Ordinal);
            registrationsByKindAndSubjectId = new Dictionary<string, Dictionary<string, MetadataRegistrationEntry>>(StringComparer.Ordinal);
            metadataTokensByKindAndSubjectId = new Dictionary<string, Dictionary<string, uint>>(StringComparer.Ordinal);
            concreteTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);

            foreach (var registration in registrations)
            {
                if (!registrationsByKind.TryGetValue(registration.RegistrationKind, out var registrationsForKind))
                {
                    registrationsForKind = [];
                    registrationsByKind.Add(registration.RegistrationKind, registrationsForKind);
                }

                registrationsForKind.Add(registration);

                if (!registrationsByKindAndSubjectId.TryGetValue(registration.RegistrationKind, out var registrationsForKindBySubjectId))
                {
                    registrationsForKindBySubjectId = new Dictionary<string, MetadataRegistrationEntry>(StringComparer.Ordinal);
                    registrationsByKindAndSubjectId.Add(registration.RegistrationKind, registrationsForKindBySubjectId);
                }

                registrationsForKindBySubjectId[registration.SubjectId] = registration;

                if (string.Equals(registration.RegistrationKind, "type", StringComparison.Ordinal))
                {
                    concreteTypeSubjectIds.Add(registration.SubjectId);
                }

                if (!TryGetMetadataTokenPrefix(registration.RegistrationKind, out var tokenPrefix))
                {
                    continue;
                }

                if (!metadataTokensByKindAndSubjectId.TryGetValue(registration.RegistrationKind, out var tokensForKind))
                {
                    tokensForKind = new Dictionary<string, uint>(StringComparer.Ordinal);
                    metadataTokensByKindAndSubjectId.Add(registration.RegistrationKind, tokensForKind);
                }

                tokensForKind[registration.SubjectId] = tokenPrefix | (uint)registration.Slot;
            }
        }

        public IReadOnlyList<MetadataRegistrationEntry> GetRegistrationsForKind(string registrationKind)
        {
            return registrationsByKind.TryGetValue(registrationKind, out var registrationsForKind)
                ? registrationsForKind
                : EmptyRegistrations;
        }

        public bool TryGetRegistration(
            string registrationKind,
            string subjectId,
            out MetadataRegistrationEntry registration)
        {
            if (registrationsByKindAndSubjectId.TryGetValue(registrationKind, out var registrationsForKindBySubjectId) &&
                registrationsForKindBySubjectId.TryGetValue(subjectId, out var resolvedRegistration))
            {
                registration = resolvedRegistration;
                return true;
            }

            registration = default!;
            return false;
        }

        public uint GetRequiredToken(
            string registrationKind,
            string subjectId)
        {
            if (metadataTokensByKindAndSubjectId.TryGetValue(registrationKind, out var tokensForKind) &&
                tokensForKind.TryGetValue(subjectId, out var token))
            {
                return token;
            }

            throw new InvalidOperationException(
                $"missing required '{registrationKind}' metadata registration for '{subjectId}'");
        }

        public bool HasConcreteType(string subjectId)
        {
            return concreteTypeSubjectIds.Contains(subjectId);
        }
    }

    private const int AuditTranslationUnitPageSize = 1024;
    private const string ManagedAsyncAwaitIntMinimal = NativeReferenceProofCatalog.ManagedAsyncAwaitIntMinimal;
    private const string ManagedThreadingThreadStaticMonitorMinimal = NativeReferenceProofCatalog.ManagedThreadingThreadStaticMonitorMinimal;
    private const string ManagedInterfaceDispatchMessageMinimal = NativeReferenceProofCatalog.ManagedInterfaceDispatchMessageMinimal;
    private const string ManagedDispatchVirtualInstanceMessageMinimal = NativeReferenceProofCatalog.ManagedDispatchVirtualInstanceMessageMinimal;
    private const string ManagedObjectCapturedStateInstanceMessageMinimal = NativeReferenceProofCatalog.ManagedObjectCapturedStateInstanceMessageMinimal;
    private const string ManagedGenericStaticForwarderCapturedGetterMinimal = NativeReferenceProofCatalog.ManagedGenericStaticForwarderCapturedGetterMinimal;
    private const string ManagedArraysReverseReferenceArrayMinimal = NativeReferenceProofCatalog.ManagedArraysReverseReferenceArrayMinimal;
    private const string ManagedArraysClearReferenceArrayMinimal = NativeReferenceProofCatalog.ManagedArraysClearReferenceArrayMinimal;
    private const string ManagedArraysCopyReferenceArrayMinimal = NativeReferenceProofCatalog.ManagedArraysCopyReferenceArrayMinimal;
    private const string ManagedArraysBoxingReferenceArrayBoxedIntMinimal = NativeReferenceProofCatalog.ManagedArraysBoxingReferenceArrayBoxedIntMinimal;
    private const string DelegateClosedTargetRelayMinimal = NativeReferenceProofCatalog.DelegateClosedTargetRelayMinimal;
    private const string NestedExceptionThrowCatchFinallyMinimal = NativeReferenceProofCatalog.NestedExceptionThrowCatchFinallyMinimal;
    private const string ExceptionThrowCatchFinallyMinimal = NativeReferenceProofCatalog.ExceptionThrowCatchFinallyMinimal;
    private const string ReflectionInteropClosureMinimal = NativeReferenceProofCatalog.ReflectionInteropClosureMinimal;
    private const string ReflectionClosedTypeQueryMinimal = NativeReferenceProofCatalog.ReflectionClosedTypeQueryMinimal;
    private const string MarshalingUtf8ExportMinimal = NativeReferenceProofCatalog.MarshalingUtf8ExportMinimal;
    private const string InteropPInvokeDirectCallMinimal = NativeReferenceProofCatalog.InteropPInvokeDirectCallMinimal;
    private const string EngineLogWriteMinimal = NativeReferenceProofCatalog.EngineLogWriteMinimal;
    private const string EngineObjectHandleRoundtripMinimal = NativeReferenceProofCatalog.EngineObjectHandleRoundtripMinimal;
    private const string EngineLifecycleCallbackMinimal = NativeReferenceProofCatalog.EngineLifecycleCallbackMinimal;
    private const string EngineHostProofMinimal = NativeReferenceProofCatalog.EngineHostProofMinimal;
    private const string GeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.GeneratedTranslationUnitTemplateRelativePath;
    private const string DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplateRelativePath;
    private const string InterfaceDispatchMessageGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.InterfaceDispatchMessageGeneratedTranslationUnitTemplateRelativePath;
    private const string AsyncAwaitIntGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.AsyncAwaitIntGeneratedTranslationUnitTemplateRelativePath;
    private const string ThreadingThreadStaticMonitorGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.ThreadingThreadStaticMonitorGeneratedTranslationUnitTemplateRelativePath;
    private const string StaticCallCtorGetterGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.StaticCallCtorGetterGeneratedTranslationUnitTemplateRelativePath;
    private const string ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplateRelativePath;
    private const string ReflectionQueryMinimalGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.ReflectionQueryMinimalGeneratedTranslationUnitTemplateRelativePath;
    private const string MarshalingUtf8ExportGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.MarshalingUtf8ExportGeneratedTranslationUnitTemplateRelativePath;
    private const string PInvokeDllImportMinimalGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.PInvokeDllImportMinimalGeneratedTranslationUnitTemplateRelativePath;
    private const string DelegateClosedTargetRelayGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.DelegateClosedTargetRelayGeneratedTranslationUnitTemplateRelativePath;
    private const string NestedExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.NestedExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath;
    private const string ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath;
    private const string ReflectionInteropClosureGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.ReflectionInteropClosureGeneratedTranslationUnitTemplateRelativePath;
    private const string EngineLogWriteGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.EngineLogWriteGeneratedTranslationUnitTemplateRelativePath;
    private const string EngineObjectHandleGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.EngineObjectHandleGeneratedTranslationUnitTemplateRelativePath;
    private const string EngineLifecycleCallbackGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.EngineLifecycleCallbackGeneratedTranslationUnitTemplateRelativePath;
    private const string EngineHostProofGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.EngineHostProofGeneratedTranslationUnitTemplateRelativePath;
    private const string ConsoleWriteLineStringIcall = "System.Console/System.Console::WriteLine(System.String)";
    private const string ConsoleWriteLineStringMethodSubjectId = "System.Console/System.Console::WriteLine:System.Void(System.String)";
    private const string StringConcatPairIcall = "System.Private.CoreLib/System.String::Concat(System.String,System.String)";
    private const string StringConcatTripleIcall = "System.Private.CoreLib/System.String::Concat(System.String,System.String,System.String)";
    private const string StringConcatPairMethodSubjectId = "System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)";
    private const string StringConcatTripleMethodSubjectId = "System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)";

    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

    private static readonly ConditionalWeakTable<IReadOnlyList<TypedIlMethodArtifact>, IReadOnlyDictionary<string, TypedIlMethodArtifact>> MethodLookupCache = new();
    private static readonly ConditionalWeakTable<IReadOnlyList<CodeRegistrationEntry>, IReadOnlyDictionary<string, CodeRegistrationEntry>> CodeRegistrationLookupCache = new();
    private static readonly ConditionalWeakTable<IReadOnlyList<MetadataRegistrationEntry>, MetadataRegistrationLookup> MetadataRegistrationLookupCache = new();

    public NativeReferenceProofResult Generate(NativeReferenceProofRequest request)
    {
        var managedClosureRoot = Path.GetFullPath(request.ManagedClosureRootPath);
        var loweringPlanPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.NativeReferenceLoweringPlan);
        var loweringPlan = LoadRequiredJson<NativeReferenceLoweringPlanArtifact>(loweringPlanPath);
        IReadOnlyList<NativeReferenceGeneratedSource> generatedSources;
        IReadOnlyList<NativeReferenceGeneratedArtifactRef> generatedArtifacts;
        string? preferredAssemblyDispatchSubjectId = BuildPreferredAssemblyDispatchSubjectId(loweringPlan);
        var translationUnitPageSize = loweringPlan.TranslationUnitPageSize;
        var translationUnitPageCount = loweringPlan.TranslationUnitPageCount;
        IReadOnlyList<AuditTranslationUnitPageArtifact>? translationUnitPages = loweringPlan.TranslationUnitPages;
        if (string.Equals(loweringPlan.PlanKind, "assembly-full-closure-runtime-skeleton", StringComparison.Ordinal))
        {
            var typedIlPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.TypedIlIr);
            var metadataRegistrationPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.MetadataRegistration);
            var codeRegistrationPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.CodeRegistration);
            var typedIl = LoadRequiredJson<TypedIlIrArtifact>(typedIlPath);
            var metadataRegistration = LoadRequiredJson<MetadataRegistrationArtifact>(metadataRegistrationPath);
            var codeRegistration = LoadRequiredJson<CodeRegistrationArtifact>(codeRegistrationPath);
            var runtimeSkeletonEmission = BuildAssemblyFullClosureRuntimeSkeletonGeneratedSources(
                loweringPlan,
                request.OutputRootPath,
                typedIl,
                metadataRegistration,
                codeRegistration);
            generatedSources = runtimeSkeletonEmission.GeneratedSources;
            generatedArtifacts = runtimeSkeletonEmission.GeneratedArtifacts;
            preferredAssemblyDispatchSubjectId = runtimeSkeletonEmission.PreferredAssemblyDispatchSubjectId;
            translationUnitPageCount = runtimeSkeletonEmission.TranslationUnitPages.Count;
            translationUnitPages = runtimeSkeletonEmission.TranslationUnitPages;
        }
        else if (string.Equals(loweringPlan.PlanKind, "assembly-full-closure-audit", StringComparison.Ordinal))
        {
            generatedSources = BuildAssemblyFullClosureAuditGeneratedSources(loweringPlan);
            generatedArtifacts = new List<NativeReferenceGeneratedArtifactRef>
            {
                new()
                {
                    Kind = "generatedTranslationUnit",
                    Path = NativeReferenceArtifactNames.AuditSummaryTranslationUnit,
                },
            };
            generatedArtifacts = generatedArtifacts
                .Concat((loweringPlan.TranslationUnitPages ?? []).Select(page => new NativeReferenceGeneratedArtifactRef
                {
                    Kind = "auditInventoryPage",
                    Path = page.Path,
                }))
                .ToList();
        }
        else
        {
            generatedSources =
            [
                new NativeReferenceGeneratedSource
                {
                    RelativePath = NativeReferenceArtifactNames.GeneratedTranslationUnit,
                    Contents = BuildGeneratedTranslationUnit(loweringPlan),
                },
            ];
            generatedArtifacts = generatedSources
                .Select(generatedSource => new NativeReferenceGeneratedArtifactRef
                {
                    Kind = "generatedTranslationUnit",
                    Path = generatedSource.RelativePath,
                })
                .ToList();
        }

        var codegenMetrics = NativeCodegenMetricsBuilder.Build(
            "native-reference",
            loweringPlan.PlanKind,
            generatedSources.Select(generatedSource => (generatedSource.RelativePath, generatedSource.Contents)));
        generatedArtifacts = generatedArtifacts
            .Concat(
            [
                new NativeReferenceGeneratedArtifactRef
                {
                    Kind = "codegenMetrics",
                    Path = NativeReferenceArtifactNames.CodegenMetrics,
                },
            ])
            .ToList();

        var generatedSourcePaths = generatedArtifacts
            .Where(artifact => string.Equals(artifact.Kind, "generatedTranslationUnit", StringComparison.Ordinal))
            .Select(artifact => artifact.Path)
            .ToList();

        var manifest = new NativeReferenceProofManifestArtifact
        {
            AssemblyName = loweringPlan.AssemblyName,
            EntrySubjectId = loweringPlan.EntrySubjectId,
            ManagedClosureRootPath = ManagedNaming.NormalizePathForManifest(managedClosureRoot, Environment.CurrentDirectory),
            PlanArtifactPath = NativeReferenceArtifactNames.LoweringPlan,
            RuntimeExecutionKind = loweringPlan.RuntimeExecutionKind,
            PreferredAssemblyDispatchSubjectId = preferredAssemblyDispatchSubjectId,
            TranslationUnitPageSize = translationUnitPageSize,
            TranslationUnitPageCount = translationUnitPageCount,
            TranslationUnitPages = translationUnitPages,
            GeneratedSourcePath = generatedSourcePaths.FirstOrDefault(),
            GeneratedSourcePaths = generatedSourcePaths,
            GeneratedArtifacts = generatedArtifacts,
        };

        return new NativeReferenceProofResult
        {
            OutputRootPath = request.OutputRootPath,
            LoweringPlan = loweringPlan,
            Manifest = manifest,
            CodegenMetrics = codegenMetrics,
            GeneratedSources = generatedSources,
        };
    }

    private static string BuildGeneratedTranslationUnit(
        NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        return ScribanTemplateRenderer.RenderTemplate(
            GetTemplateForPlan(loweringPlan.PlanKind),
            CreateTemplateModel(loweringPlan));
    }

    private static IReadOnlyList<NativeReferenceGeneratedSource> BuildAssemblyFullClosureAuditGeneratedSources(
        NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        ValidateAssemblyFullClosureAuditPlan(loweringPlan);
        return AssemblyFullClosureAuditEmitter.BuildGeneratedFiles(
                loweringPlan.AssemblyName,
                loweringPlan.PlanKind,
                loweringPlan.TranslationUnitMethodCount ?? 0,
                loweringPlan.TranslationUnitPageSize ?? AuditTranslationUnitPageSize,
                loweringPlan.TranslationUnitPages ?? [],
                loweringPlan.TranslationUnitMethodSubjectIds ?? [],
                NativeReferenceArtifactNames.AuditSummaryTranslationUnit)
            .Select(generatedFile => new NativeReferenceGeneratedSource
            {
                RelativePath = generatedFile.RelativePath,
                Contents = generatedFile.Contents,
            })
            .ToList();
    }

    private static AssemblyFullClosureRuntimeSkeletonEmission BuildAssemblyFullClosureRuntimeSkeletonGeneratedSources(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string outputRootPath,
        TypedIlIrArtifact typedIl,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationArtifact codeRegistration)
    {
        ValidateAssemblyFullClosureRuntimeSkeletonPlan(loweringPlan);

        var methods = typedIl.Methods ?? [];
        var methodsBySubjectId = methods
            .ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        var methodPointers = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .Where(registration => string.Equals(registration.RegistrationKind, "methodPointer", StringComparison.Ordinal))
            .ToList();
        _ = GetCodeRegistrationLookup(methodPointers);
        _ = GetMetadataRegistrationLookup(metadataRegistration.Registrations);
        var requestedPages = (loweringPlan.TranslationUnitPages ?? []).ToArray();
        var requestedMethodSubjectIds = loweringPlan.TranslationUnitMethodSubjectIds ?? [];
        var translationUnitPageSize = loweringPlan.TranslationUnitPageSize ?? AuditTranslationUnitPageSize;
        var allMethodStubNamesBySubjectId = new Dictionary<string, string>(requestedMethodSubjectIds.Count, StringComparer.Ordinal);
        for (var requestedMethodIndex = 0; requestedMethodIndex < requestedMethodSubjectIds.Count; requestedMethodIndex++)
        {
            var requestedSubjectId = requestedMethodSubjectIds[requestedMethodIndex];
            var pageNumber = (requestedMethodIndex / translationUnitPageSize) + 1;
            var itemNumber = (requestedMethodIndex % translationUnitPageSize) + 1;
            allMethodStubNamesBySubjectId.Add(
                requestedSubjectId,
                BuildAssemblyFullClosureRuntimeSkeletonMethodStubName(pageNumber, itemNumber));
        }

        var pageEmissionsByIndex = new RuntimeSkeletonPageEmission?[requestedPages.Length];
        var unsupportedMethodsByIndex = new IReadOnlyList<RuntimeSkeletonUnsupportedMethodEmission>[requestedPages.Length];
        Parallel.For(
            0,
            requestedPages.Length,
            new ParallelOptions
            {
                MaxDegreeOfParallelism = GetRuntimeSkeletonPageParallelism(requestedPages.Length),
            },
            pageIndex =>
            {
                var page = requestedPages[pageIndex];
                var pageStartIndex = (page.PageNumber - 1) * translationUnitPageSize;
                var pageEmission = BuildAssemblyFullClosureRuntimeSkeletonPageTranslationUnit(
                    loweringPlan,
                    metadataRegistration,
                    methods,
                    methodsBySubjectId,
                    methodPointers,
                    allMethodStubNamesBySubjectId,
                    page.PageNumber,
                    page.Path,
                    requestedMethodSubjectIds,
                    pageStartIndex,
                    page.MethodCount);
                if (pageEmission.SupportedMethods.Count > 0)
                {
                    WriteGeneratedSource(
                        outputRootPath,
                        new NativeReferenceGeneratedSource
                        {
                            RelativePath = pageEmission.Page.Path,
                            Contents = pageEmission.Contents,
                        });
                        pageEmissionsByIndex[pageIndex] = new RuntimeSkeletonPageEmission(
                            pageEmission.Page,
                            string.Empty,
                            pageEmission.SupportedMethods,
                            pageEmission.UnsupportedMethods);
                }

                unsupportedMethodsByIndex[pageIndex] = pageEmission.UnsupportedMethods;
            });

        var pageEmissions = new List<RuntimeSkeletonPageEmission>(requestedPages.Length);
        var emittedPages = new List<AuditTranslationUnitPageArtifact>(requestedPages.Length);
        var unsupportedMethods = new List<RuntimeSkeletonUnsupportedMethodEmission>();
        var supportedSubjectIds = new List<string>();
        var emittedMethodCount = 0;
        for (var pageIndex = 0; pageIndex < requestedPages.Length; pageIndex++)
        {
            var pageEmission = pageEmissionsByIndex[pageIndex];
            if (pageEmission is not null)
            {
                pageEmissions.Add(pageEmission);
                emittedPages.Add(pageEmission.Page);
                emittedMethodCount += pageEmission.SupportedMethods.Count;
                supportedSubjectIds.AddRange(pageEmission.SupportedMethods.Select(method => method.SubjectId));
            }

            var pageUnsupportedMethods = unsupportedMethodsByIndex[pageIndex];
            if (pageUnsupportedMethods is not null && pageUnsupportedMethods.Count > 0)
            {
                unsupportedMethods.AddRange(pageUnsupportedMethods);
            }
        }

        var summaryRelativePath = NativeReferenceArtifactNames.RuntimeSkeletonGeneratedTranslationUnit;
        var coverageReportRelativePath = NativeReferenceArtifactNames.RuntimeSkeletonCoverageReport;
        WriteGeneratedSource(
            outputRootPath,
            new NativeReferenceGeneratedSource
            {
                RelativePath = coverageReportRelativePath,
                Contents = BuildAssemblyFullClosureRuntimeSkeletonCoverageReport(
                    loweringPlan,
                    emittedMethodCount,
                    unsupportedMethods),
            });
        var generatedSources = new List<NativeReferenceGeneratedSource>
        {
            new()
            {
                RelativePath = summaryRelativePath,
                Contents = BuildAssemblyFullClosureRuntimeSkeletonSummaryTranslationUnit(
                    loweringPlan,
                    pageEmissions,
                    emittedMethodCount),
            },
        };

        var generatedArtifacts = new List<NativeReferenceGeneratedArtifactRef>
        {
            new()
            {
                Kind = "generatedTranslationUnit",
                Path = summaryRelativePath,
            },
        };
        foreach (var page in emittedPages)
        {
            generatedArtifacts.Add(new NativeReferenceGeneratedArtifactRef
            {
                Kind = "generatedTranslationUnit",
                Path = page.Path,
            });
        }

        generatedArtifacts.Add(new NativeReferenceGeneratedArtifactRef
        {
            Kind = "runtimeSkeletonCoverageReport",
            Path = coverageReportRelativePath,
        });

        return new AssemblyFullClosureRuntimeSkeletonEmission(
            generatedSources,
            generatedArtifacts,
            emittedPages,
            emittedMethodCount,
            BuildPreferredAssemblyDispatchSubjectId(loweringPlan, supportedSubjectIds));
    }

    private static void WriteGeneratedSource(
        string outputRootPath,
        NativeReferenceGeneratedSource generatedSource)
    {
        var targetPath = Path.Combine(outputRootPath, generatedSource.RelativePath.Replace('/', Path.DirectorySeparatorChar));
        Directory.CreateDirectory(Path.GetDirectoryName(targetPath)!);
        File.WriteAllText(targetPath, generatedSource.Contents);
    }

    private static int GetRuntimeSkeletonPageParallelism(int pageCount)
    {
        if (pageCount <= 1)
        {
            return 1;
        }

        var recommendedParallelism = Math.Max(1, Environment.ProcessorCount - 1);
        return Math.Min(pageCount, recommendedParallelism);
    }

    private static string BuildAssemblyFullClosureRuntimeSkeletonSummaryTranslationUnit(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        IReadOnlyList<RuntimeSkeletonPageEmission> pageEmissions,
        int emittedMethodCount)
    {
        ValidateAssemblyFullClosureRuntimeSkeletonPlan(loweringPlan);
        var emittedPages = pageEmissions
            .Select(pageEmission => pageEmission.Page)
            .ToArray();
        var pageDispatchDeclarations = emittedPages
            .Select(page =>
            {
                var pageDispatchName = BuildAssemblyFullClosureRuntimeSkeletonPageDispatchFunctionName(page.PageNumber);
                return $"""
int32_t CHAOS_RUNTIME_ABI_CALL {pageDispatchName}(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    std::uint32_t method_slot,
    void* managed_args);
""";
            })
            .ToArray();
        var methodDispatchCatalogEntries = pageEmissions
            .SelectMany(pageEmission =>
            {
                var pageDispatchName = BuildAssemblyFullClosureRuntimeSkeletonPageDispatchFunctionName(pageEmission.Page.PageNumber);
                return pageEmission.SupportedMethods.Select(method =>
                    $"    {{ {method.MethodId}u, {ToCppStringLiteral(method.SubjectId)}, &{pageDispatchName}, {method.DispatchSlot}u }},");
            })
            .ToArray();
        var model = new ScriptObject
        {
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["plan_kind_literal"] = ToCppStringLiteral(loweringPlan.PlanKind),
            ["runtime_execution_kind_literal"] = ToCppStringLiteral(loweringPlan.RuntimeExecutionKind ?? "assembly-bound-native-reference-skeleton"),
            ["translation_unit_mode_literal"] = ToCppStringLiteral(loweringPlan.TranslationUnitMode ?? "runtime-skeleton"),
            ["translation_unit_method_count"] = emittedMethodCount,
            ["translation_unit_page_size"] = loweringPlan.TranslationUnitPageSize ?? AuditTranslationUnitPageSize,
            ["translation_unit_page_count"] = emittedPages.Length,
            ["has_method_dispatch_entries"] = methodDispatchCatalogEntries.Length > 0,
            ["page_dispatch_declarations"] = pageDispatchDeclarations,
            ["method_dispatch_catalog_entries"] = methodDispatchCatalogEntries,
            ["native_entry_function_name"] = loweringPlan.NativeEntryFunctionName,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetAssemblyFullClosureRuntimeSkeletonSummaryTemplate(),
            model);
    }

    private static RuntimeSkeletonPageEmission BuildAssemblyFullClosureRuntimeSkeletonPageTranslationUnit(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyDictionary<string, string> allMethodStubNamesBySubjectId,
        int pageNumber,
        string pagePath,
        IReadOnlyList<string> requestedMethodSubjectIds,
        int pageStartIndex,
        int pageItemCount)
    {
        var methodStubNamesBySubjectId = new Dictionary<string, string>(pageItemCount, StringComparer.Ordinal);
        var methodStubNamesByIndex = new string[pageItemCount];
        for (var index = 0; index < pageItemCount; index++)
        {
            var subjectId = requestedMethodSubjectIds[pageStartIndex + index];
            var stubName = BuildAssemblyFullClosureRuntimeSkeletonMethodStubName(pageNumber, index + 1);
            methodStubNamesByIndex[index] = stubName;
            methodStubNamesBySubjectId.Add(subjectId, stubName);
        }

        var emittedMethods = new List<RuntimeSkeletonMethodEmission>(pageItemCount);
        var unsupportedMethods = new List<RuntimeSkeletonUnsupportedMethodEmission>();
        var pageSupportBuilder = new RuntimeSkeletonPageSupportBuilder();
        for (var index = 0; index < pageItemCount; index++)
        {
            var subjectId = requestedMethodSubjectIds[pageStartIndex + index];
            var stubName = methodStubNamesByIndex[index];
            var stubDefinition = TryBuildAssemblyFullClosureRuntimeSkeletonMethodStub(
                loweringPlan,
                metadataRegistration,
                methodPointers,
                methods,
                methodsBySubjectId,
                allMethodStubNamesBySubjectId,
                pageSupportBuilder,
                stubName,
                subjectId);
            if (stubDefinition is null)
            {
                if (methodsBySubjectId.TryGetValue(subjectId, out var requestedMethod) &&
                    IsRuntimeSkeletonNonExecutableDeclaration(requestedMethod))
                {
                    continue;
                }

                unsupportedMethods.Add(new RuntimeSkeletonUnsupportedMethodEmission(subjectId, "unsupportedShapeOrCapability"));
                continue;
            }

            emittedMethods.Add(new RuntimeSkeletonMethodEmission(
                pageStartIndex + emittedMethods.Count + 1,
                subjectId,
                stubName,
                emittedMethods.Count,
                BuildAssemblyFullClosureRuntimeSkeletonMethodStubDeclaration(stubName),
                stubDefinition));
        }

        var methodStubDeclarations = new List<string>(emittedMethods.Count);
        var externalMethodStubDeclarations = new Dictionary<string, RuntimeSkeletonExternalMethodStubDeclaration>(StringComparer.Ordinal);
        var methodStubDefinitions = new List<string>(emittedMethods.Count);
        var dispatchEntries = new List<string>(emittedMethods.Count);
        var supportedMethods = new List<RuntimeSkeletonSupportedMethodDispatch>(emittedMethods.Count);
        foreach (var emission in emittedMethods)
        {
            supportedMethods.Add(new RuntimeSkeletonSupportedMethodDispatch(emission.MethodId, emission.SubjectId, emission.DispatchSlot));
            methodStubDeclarations.Add(emission.StubDeclaration);
            methodStubDefinitions.Add(emission.StubDefinition);
            dispatchEntries.Add($"    &{emission.StubName},");

            foreach (var referencedSubjectId in GetReferencedRuntimeSkeletonMethodSubjectIds(methodsBySubjectId, emission.SubjectId))
            {
                if (methodStubNamesBySubjectId.ContainsKey(referencedSubjectId) ||
                    !allMethodStubNamesBySubjectId.TryGetValue(referencedSubjectId, out var referencedStubName))
                {
                    continue;
                }

                externalMethodStubDeclarations.TryAdd(
                    referencedSubjectId,
                    new RuntimeSkeletonExternalMethodStubDeclaration(
                        referencedSubjectId,
                        referencedStubName,
                        BuildAssemblyFullClosureRuntimeSkeletonMethodStubDeclaration(referencedStubName)));
            }
        }

        var firstEmittedSubjectId = supportedMethods.Count > 0 ? supportedMethods[0].SubjectId : null;
        var lastEmittedSubjectId = supportedMethods.Count > 0 ? supportedMethods[^1].SubjectId : null;
        var model = new ScriptObject
        {
            ["runtime_execution_kind_literal"] = ToCppStringLiteral(loweringPlan.RuntimeExecutionKind ?? "assembly-bound-native-reference-skeleton"),
            ["page_number"] = pageNumber,
            ["page_item_count"] = supportedMethods.Count,
            ["method_stub_declarations"] = methodStubDeclarations.ToArray(),
            ["method_stub_definitions"] = methodStubDefinitions.ToArray(),
            ["field_binding_descriptors"] = pageSupportBuilder.GetFieldBindingDescriptors(),
            ["field_binding_descriptor_count"] = pageSupportBuilder.FieldBindingDescriptorCount,
            ["field_binding_storage_count"] = Math.Max(1, pageSupportBuilder.FieldBindingDescriptorCount),
            ["type_descriptors"] = pageSupportBuilder.GetTypeDescriptors(),
            ["type_descriptor_count"] = pageSupportBuilder.TypeDescriptorCount,
            ["type_storage_count"] = Math.Max(1, pageSupportBuilder.TypeDescriptorCount),
            ["constructor_field_setter_descriptors"] = pageSupportBuilder.GetConstructorFieldSetterDescriptors(),
            ["constructor_field_setter_descriptor_count"] = pageSupportBuilder.ConstructorFieldSetterDescriptorCount,
            ["field_getter_string_return_descriptors"] = pageSupportBuilder.GetFieldGetterStringReturnDescriptors(),
            ["field_getter_string_return_descriptor_count"] = pageSupportBuilder.FieldGetterStringReturnDescriptorCount,
            ["field_backed_string_return_descriptors"] = pageSupportBuilder.GetFieldBackedStringReturnDescriptors(),
            ["field_backed_string_return_descriptor_count"] = pageSupportBuilder.FieldBackedStringReturnDescriptorCount,
            ["field_argument_string_return_descriptors"] = pageSupportBuilder.GetFieldArgumentStringReturnDescriptors(),
            ["field_argument_string_return_descriptor_count"] = pageSupportBuilder.FieldArgumentStringReturnDescriptorCount,
            ["static_call_ctor_getter_descriptors"] = pageSupportBuilder.GetStaticCallCtorGetterDescriptors(),
            ["static_call_ctor_getter_descriptor_count"] = pageSupportBuilder.StaticCallCtorGetterDescriptorCount,
            ["static_string_literal_append_descriptors"] = pageSupportBuilder.GetStaticStringLiteralAppendDescriptors(),
            ["static_string_literal_append_descriptor_count"] = pageSupportBuilder.StaticStringLiteralAppendDescriptorCount,
            ["constructor_then_instance_call_descriptors"] = pageSupportBuilder.GetConstructorThenInstanceCallDescriptors(),
            ["constructor_then_instance_call_descriptor_count"] = pageSupportBuilder.ConstructorThenInstanceCallDescriptorCount,
            ["delegate_closed_target_relay_descriptors"] = pageSupportBuilder.GetDelegateClosedTargetRelayDescriptors(),
            ["delegate_closed_target_relay_descriptor_count"] = pageSupportBuilder.DelegateClosedTargetRelayDescriptorCount,
            ["array_boxing_reference_array_descriptors"] = pageSupportBuilder.GetArrayBoxingReferenceArrayDescriptors(),
            ["array_boxing_reference_array_descriptor_count"] = pageSupportBuilder.ArrayBoxingReferenceArrayDescriptorCount,
            ["array_reverse_reference_array_descriptors"] = pageSupportBuilder.GetArrayReverseReferenceArrayDescriptors(),
            ["array_reverse_reference_array_descriptor_count"] = pageSupportBuilder.ArrayReverseReferenceArrayDescriptorCount,
            ["array_clear_reference_array_descriptors"] = pageSupportBuilder.GetArrayClearReferenceArrayDescriptors(),
            ["array_clear_reference_array_descriptor_count"] = pageSupportBuilder.ArrayClearReferenceArrayDescriptorCount,
            ["array_copy_reference_array_descriptors"] = pageSupportBuilder.GetArrayCopyReferenceArrayDescriptors(),
            ["array_copy_reference_array_descriptor_count"] = pageSupportBuilder.ArrayCopyReferenceArrayDescriptorCount,
            ["external_method_stub_declarations"] = externalMethodStubDeclarations.Values.Select(item => item.Declaration).ToArray(),
            ["dispatch_entries"] = dispatchEntries.ToArray(),
            ["page_dispatch_function_name"] = BuildAssemblyFullClosureRuntimeSkeletonPageDispatchFunctionName(pageNumber),
        };
        return new RuntimeSkeletonPageEmission(
            new AuditTranslationUnitPageArtifact
            {
                PageNumber = pageNumber,
                MethodCount = supportedMethods.Count,
                Path = pagePath,
                FirstMethodSubjectId = firstEmittedSubjectId,
                LastMethodSubjectId = lastEmittedSubjectId,
            },
            ScribanTemplateRenderer.RenderTemplate(
                NativeReferenceProofCatalog.GetAssemblyFullClosureRuntimeSkeletonPageTemplate(),
                model),
            supportedMethods,
            unsupportedMethods);
    }

    private static string BuildAssemblyFullClosureRuntimeSkeletonMethodStubName(
        int pageNumber,
        int itemNumber)
    {
        return $"NativeReferenceStub_Page{pageNumber:D4}_Item{itemNumber:D4}";
    }

    private static string BuildAssemblyFullClosureRuntimeSkeletonPageDispatchFunctionName(int pageNumber)
    {
        return $"DispatchRuntimeSkeletonPage{pageNumber:D4}";
    }

    private static string BuildAssemblyFullClosureRuntimeSkeletonCoverageReport(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        int emittedMethodCount,
        IReadOnlyList<RuntimeSkeletonUnsupportedMethodEmission> unsupportedMethods)
    {
        var uncoveredReasonCounts = unsupportedMethods
            .GroupBy(method => method.ReasonCode, StringComparer.Ordinal)
            .ToDictionary(group => group.Key, group => group.Count(), StringComparer.Ordinal);
        var payload = new Dictionary<string, object?>
        {
            ["formatVersion"] = "v0",
            ["artifactKind"] = "nativeReferenceRuntimeSkeletonCoverage",
            ["assemblyName"] = loweringPlan.AssemblyName,
            ["planKind"] = loweringPlan.PlanKind,
            ["translationUnitMode"] = loweringPlan.TranslationUnitMode,
            ["requestedMethodCount"] = loweringPlan.TranslationUnitMethodCount ?? 0,
            ["emittedMethodCount"] = emittedMethodCount,
            ["uncoveredMethodCount"] = unsupportedMethods.Count,
            ["uncoveredReasonCounts"] = uncoveredReasonCounts,
            ["uncoveredMethodSubjectIds"] = unsupportedMethods.Select(method => method.SubjectId).ToArray(),
            ["uncoveredMethods"] = unsupportedMethods.Select(method => new Dictionary<string, object?>
            {
                ["subjectId"] = method.SubjectId,
                ["reasonCode"] = method.ReasonCode,
            }).ToArray(),
        };
        return JsonSerializer.Serialize(payload, new JsonSerializerOptions
        {
            WriteIndented = true,
        });
    }

    private static string BuildAssemblyFullClosureRuntimeSkeletonMethodStubDeclaration(string stubName)
    {
        return $@"int32_t CHAOS_RUNTIME_ABI_CALL {stubName}(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);";
    }

    private static string? TryBuildAssemblyFullClosureRuntimeSkeletonMethodStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        string subjectId)
    {
        var buildContext = new RuntimeSkeletonStubBuildContext(
            loweringPlan,
            metadataRegistration,
            methodPointers,
            methods,
            methodsBySubjectId,
            methodStubNamesBySubjectId,
            pageSupportBuilder,
            stubName,
            subjectId);
        return TryBuildAssemblyFullClosureRuntimeSkeletonMethodStubVia4CDispatcher(buildContext);
    }

    private static string? TryBuildAssemblyFullClosureRuntimeSkeletonMethodStubVia4CDispatcher(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        foreach (var familyHandler in RuntimeSkeletonFamilyHandlers)
        {
            var result = familyHandler(buildContext);
            if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match)
            {
                return result.StubDefinition;
            }
        }

        return null;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Convert::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        return TryBuildRuntimeSkeletonConvertHandlers(
            buildContext,
            RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("convert-family-owned-unsupported-shape"));
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonBindingFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        foreach (var bindingFamilyHandler in RuntimeSkeletonBindingFamilyHandlers)
        {
            var result = bindingFamilyHandler(buildContext);
            if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match)
            {
                return result;
            }
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonUtilityFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        foreach (var utilityFamilyHandler in RuntimeSkeletonUtilityFamilyHandlers)
        {
            var result = utilityFamilyHandler(buildContext);
            if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match)
            {
                return result;
            }
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonPlatformFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        foreach (var platformFamilyHandler in RuntimeSkeletonPlatformFamilyHandlers)
        {
            var result = platformFamilyHandler(buildContext);
            if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match)
            {
                return result;
            }
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonInteropFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        foreach (var interopFamilyHandler in RuntimeSkeletonInteropFamilyHandlers)
        {
            var result = interopFamilyHandler(buildContext);
            if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match)
            {
                return result;
            }
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonAsyncFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        foreach (var asyncFamilyHandler in RuntimeSkeletonAsyncFamilyHandlers)
        {
            var result = asyncFamilyHandler(buildContext);
            if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match)
            {
                return result;
            }
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonArrayFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        foreach (var arrayFamilyHandler in RuntimeSkeletonArrayFamilyHandlers)
        {
            var result = arrayFamilyHandler(buildContext);
            if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match)
            {
                return result;
            }
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonExceptionFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("Exception", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        foreach (var exceptionFamilyHandler in RuntimeSkeletonExceptionFamilyHandlers)
        {
            var result = exceptionFamilyHandler(buildContext);
            if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match)
            {
                return result;
            }
        }

        return RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("exception-family-owned-unsupported-shape");
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        return TryBuildRuntimeSkeletonConvertHandlers(buildContext, RuntimeSkeletonFamilyHandlerResult.NoMatch);
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertHandlers(
        RuntimeSkeletonStubBuildContext buildContext,
        RuntimeSkeletonFamilyHandlerResult missResult)
    {
        foreach (var convertFamilyHandler in RuntimeSkeletonConvertFamilyHandlers)
        {
            var result = convertFamilyHandler(buildContext);
            if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match)
            {
                return result;
            }
        }

        return missResult;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStringFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        foreach (var stringFamilyHandler in RuntimeSkeletonStringFamilyHandlers)
        {
            var result = stringFamilyHandler(buildContext);
            if (result.MatchKind == RuntimeSkeletonFamilyHandlerMatchKind.Match)
            {
                return result;
            }
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertIntForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticIntForwarderStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertBoolIdentityForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticBoolIdentityForwarderStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertBoolProducerForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticBoolProducerForwarderStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertPrimitiveHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticPrimitiveConvertStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertCheckedByteHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticCheckedByteConvertStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertCheckedCharHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticCheckedCharConvertStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertByteForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticByteReturnForwarderStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        if (TryBuildAssemblyBoundStaticCharReturnForwarderStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.MethodStubNamesBySubjectId,
                buildContext.StubName,
                out stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConstructorFieldSetterHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundConstructorFieldSetterStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.PageSupportBuilder,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonFieldBackedStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundFieldBackedStringReturnStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.PageSupportBuilder,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonFieldArgumentStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundFieldArgumentStringReturnStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.PageSupportBuilder,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonFieldGetterStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundFieldGetterStringReturnStub(
                buildContext.LoweringPlan.AssemblyName,
                buildContext.SubjectId,
                buildContext.MetadataRegistration,
                buildContext.MethodsBySubjectId,
                buildContext.PageSupportBuilder,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringReturnForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringReturnForwarderStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringForwarderStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringLiteralAppendHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringLiteralAppendStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.PageSupportBuilder,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringForwarderConsoleWriteLineStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerForwarderConsoleWriteLineStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerCtorGetterConsoleWriteLineStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerCtorRenderConsoleWriteLineStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticLiteralStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticLiteralStringReturnStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticResourceKeyStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticResourceKeyStringReturnStub(
                buildContext.SubjectId,
                buildContext.MethodsBySubjectId,
                buildContext.StubName,
                out var stubDefinition))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stubDefinition);
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (TryBuildAssemblyBoundStaticStringProducerConsoleWriteLineStub(
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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        var executableLoweringPlan = TryBuildAssemblyFullClosureExecutableLoweringPlan(
            buildContext.LoweringPlan.AssemblyName,
            buildContext.SubjectId,
            buildContext.Methods,
            buildContext.MetadataRegistration,
            buildContext.MethodPointers);
        if (executableLoweringPlan is not null &&
            string.Equals(executableLoweringPlan.PlanKind, "staticCallCtorGetter", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundStaticCallCtorGetterStub(
                    executableLoweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableHandler(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        var executableLoweringPlan = TryBuildAssemblyFullClosureExecutableLoweringPlan(
            buildContext.LoweringPlan.AssemblyName,
            buildContext.SubjectId,
            buildContext.Methods,
            buildContext.MetadataRegistration,
            buildContext.MethodPointers);
        if (executableLoweringPlan is not null &&
            string.Equals(executableLoweringPlan.PlanKind, "constructorThenInstanceCall", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(
                BuildAssemblyBoundConstructorThenInstanceCallStub(
                    executableLoweringPlan,
                    buildContext.PageSupportBuilder,
                    buildContext.StubName));
        }

        return RuntimeSkeletonFamilyHandlerResult.NoMatch;
    }

    private static string BuildAssemblyBoundStaticCallCtorGetterStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var echoLiteral = loweringPlan.EchoLiteral ?? throw new InvalidOperationException("runtime skeleton staticCallCtorGetter requires echo literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonStaticCallCtorGetterDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            echoLiteral,
            loweringPlan.EchoLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticCallCtorGetterStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundConstructorThenInstanceCallStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton constructorThenInstanceCall requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton constructorThenInstanceCall requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton constructorThenInstanceCall requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonConstructorThenInstanceCallDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonConstructorThenInstanceCallStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundDelegateClosedTargetRelayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton delegateClosedTargetRelay requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton delegateClosedTargetRelay requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton delegateClosedTargetRelay requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonDelegateClosedTargetRelayDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0,
            loweringPlan.TrailingLiteral ?? throw new InvalidOperationException("delegate closed target relay requires trailing literal"),
            loweringPlan.TrailingLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonDelegateClosedTargetRelayStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundArrayBoxingReferenceArrayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var boxedValueTypeToken = loweringPlan.BoxedValueTypeToken ?? throw new InvalidOperationException("runtime skeleton arrayBoxingReferenceArray requires boxed value type token");
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton arrayBoxingReferenceArray requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayBoxingReferenceArray requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayBoxingReferenceArray requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var boxedValueTypeIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonTypeDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            boxedValueTypeToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonArrayBoxingReferenceArrayDescriptor(
            fieldBindingIndex,
            boxedValueTypeIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            loweringPlan.BoxedInt32Value ?? 0,
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonArrayBoxingReferenceArrayStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundArrayCopyReferenceArrayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton arrayCopyReferenceArray requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayCopyReferenceArray requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayCopyReferenceArray requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonArrayCopyReferenceArrayDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            loweringPlan.SourceArrayLength ?? 0,
            loweringPlan.TargetArrayLength ?? 0,
            loweringPlan.SourceStoreIndex ?? 0,
            loweringPlan.SourceArrayIndex ?? 0,
            loweringPlan.TargetArrayIndex ?? 0,
            loweringPlan.TargetReadIndex ?? 0,
            loweringPlan.CopyLength ?? 0,
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonArrayCopyReferenceArrayStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundArrayClearReferenceArrayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var storedLiteral = loweringPlan.StoredLiteral ?? throw new InvalidOperationException("runtime skeleton arrayClearReferenceArray requires stored literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayClearReferenceArray requires message prefix literal");
        var stringTypeIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonTypeDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonArrayClearReferenceArrayDescriptor(
            stringTypeIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            loweringPlan.ArrayLength ?? 0,
            loweringPlan.ArrayStoreIndex ?? 0,
            loweringPlan.ClearStartIndex ?? 0,
            loweringPlan.ClearLength ?? 0,
            loweringPlan.ArrayReadIndex ?? 0,
            storedLiteral,
            loweringPlan.StoredLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonArrayClearReferenceArrayStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundArrayReverseReferenceArrayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton arrayReverseReferenceArray requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayReverseReferenceArray requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayReverseReferenceArray requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonArrayReverseReferenceArrayDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            loweringPlan.ArrayLength ?? 0,
            loweringPlan.ArrayStoreIndex ?? 0,
            loweringPlan.ReverseStartIndex ?? 0,
            loweringPlan.ReverseLength ?? 0,
            loweringPlan.ArrayReadIndex ?? 0,
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonArrayReverseReferenceArrayStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundInterfaceDispatchMessageStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["captured_field_token"] = loweringPlan.CapturedFieldToken,
            ["instance_method_token"] = loweringPlan.InstanceMethodToken,
            ["concat_icall_literal"] = ToCppStringLiteral(concatIcall),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["constructor_literal"] = loweringPlan.ConstructorLiteral,
            ["constructor_literal_byte_count"] = loweringPlan.ConstructorLiteralByteCount ?? 0,
            ["message_prefix_literal"] = loweringPlan.MessagePrefixLiteral,
            ["message_prefix_literal_byte_count"] = loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            ["message_suffix_literal"] = loweringPlan.MessageSuffixLiteral,
            ["message_suffix_literal_byte_count"] = loweringPlan.MessageSuffixLiteralByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonInterfaceDispatchMessageStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundThreadingThreadStaticMonitorStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(loweringPlan.ConsoleWriteLineStringIcall ?? ConsoleWriteLineStringIcall),
            ["expected_output"] = ToCppStringLiteral(loweringPlan.ExpectedOutput ?? string.Empty),
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonThreadingThreadStaticMonitorStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundExceptionThrowCatchFinallyStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["expected_output"] = loweringPlan.ExpectedOutput,
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
            ["finally_literal"] = loweringPlan.FinallyLiteral,
            ["finally_literal_byte_count"] = loweringPlan.FinallyLiteralByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonExceptionThrowCatchFinallyStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundNestedExceptionThrowCatchFinallyStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["expected_output"] = loweringPlan.ExpectedOutput,
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
            ["finally_literal"] = loweringPlan.FinallyLiteral,
            ["finally_literal_byte_count"] = loweringPlan.FinallyLiteralByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonNestedExceptionThrowCatchFinallyStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundMarshalingUtf8ExportStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["expected_output"] = loweringPlan.ExpectedOutput,
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonMarshalingUtf8ExportStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundReflectionInteropClosureStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["parameter_subject_id_literal"] = ToCppStringLiteral(loweringPlan.ParameterSubjectId ?? string.Empty),
            ["parameter_index"] = loweringPlan.ParameterIndex ?? 0,
            ["captured_field_token"] = loweringPlan.CapturedFieldToken,
            ["field_subject_id_literal"] = ToCppStringLiteral(loweringPlan.FieldSubjectId ?? string.Empty),
            ["field_query_name_literal"] = ToCppStringLiteral(loweringPlan.FieldQueryName ?? string.Empty),
            ["instance_method_token"] = loweringPlan.InstanceMethodToken,
            ["method_subject_id_literal"] = ToCppStringLiteral(loweringPlan.MethodSubjectId ?? string.Empty),
            ["method_query_name_literal"] = ToCppStringLiteral(loweringPlan.MethodQueryName ?? string.Empty),
            ["method_parameter_count"] = loweringPlan.MethodParameterCount ?? 0,
            ["generic_type_definition_token"] = loweringPlan.GenericTypeDefinitionToken,
            ["generic_type_definition_subject_id_literal"] = ToCppStringLiteral(loweringPlan.GenericTypeDefinitionSubjectId ?? string.Empty),
            ["closed_type_namespace_name_literal"] = ToCppStringLiteral(loweringPlan.ClosedTypeNamespaceName ?? string.Empty),
            ["generic_type_definition_name_literal"] = ToCppStringLiteral(loweringPlan.GenericTypeDefinitionName ?? string.Empty),
            ["closed_type_token"] = loweringPlan.ClosedTypeToken,
            ["closed_type_subject_id_literal"] = ToCppStringLiteral(loweringPlan.ClosedTypeSubjectId ?? string.Empty),
            ["closed_type_name_literal"] = ToCppStringLiteral(loweringPlan.ClosedTypeName ?? string.Empty),
            ["closed_type_display_name_literal"] = ToCppStringLiteral(loweringPlan.ClosedTypeDisplayName ?? string.Empty),
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["import_module_name_literal"] = ToCppStringLiteral(loweringPlan.ImportModuleName ?? string.Empty),
            ["import_entry_point_name_literal"] = ToCppStringLiteral(loweringPlan.ImportEntryPointName ?? string.Empty),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["expected_output"] = loweringPlan.ExpectedOutput,
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonReflectionInteropClosureStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundPInvokeDllImportMinimalStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["import_module_name_literal"] = ToCppStringLiteral(loweringPlan.ImportModuleName ?? string.Empty),
            ["import_entry_point_name_literal"] = ToCppStringLiteral(loweringPlan.ImportEntryPointName ?? string.Empty),
            ["import_argument0"] = loweringPlan.ImportArgument0 ?? 0,
            ["import_argument1"] = loweringPlan.ImportArgument1 ?? 0,
            ["import_argument2"] = loweringPlan.ImportArgument2 ?? 0,
            ["output_prefix_literal"] = ToCppStringLiteral(loweringPlan.OutputPrefix ?? string.Empty),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonPInvokeDirectCallStubTemplate(),
            model);
    }

    private static NativeReferenceLoweringPlanArtifact? TryBuildAssemblyFullClosureExecutableLoweringPlan(
        string assemblyName,
        string subjectId,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers)
    {
        try
        {
            return BuildLegacyLoweringPlan(
                assemblyName,
                subjectId,
                methods,
                metadataRegistration,
                methodPointers);
        }
        catch
        {
            return null;
        }
    }

    private static bool TryBuildAssemblyBoundDelegateClosedTargetRelayPlan(
        string assemblyName,
        string subjectId,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions;
        TypedIlMethodArtifact constructorMethod;
        TypedIlMethodArtifact instanceMethod;
        TypedIlMethodArtifact staticMethod;
        string constructorSubjectId;
        string instanceMethodSubjectId;
        string staticMethodSubjectId;
        string constructorLiteral;
        string messagePrefixLiteral;
        string messageSuffixLiteral;
        string trailingLiteral;
        string capturedFieldSubjectId;
        string concatPairIcall;

        try
        {
            entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
            ValidateDelegateClosedTargetRelayEntryPointShape(entryPointMethod, entryPointInstructions);

            constructorLiteral = GetRequiredOperandString(entryPointInstructions[0]);
            constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
            instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
            staticMethodSubjectId = GetRequiredInstructionCallee(
                entryPointInstructions[GetDelegateClosedTargetRelayStaticMethodInstructionIndex(entryPointInstructions)],
                entryPointMethod.SubjectId,
                GetDelegateClosedTargetRelayStaticMethodInstructionIndex(entryPointInstructions));
            messagePrefixLiteral = GetRequiredOperandString(
                entryPointInstructions[GetDelegateClosedTargetRelayMessagePrefixInstructionIndex(entryPointInstructions)]);

            constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
            instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);
            staticMethod = GetRequiredMethod(methods, staticMethodSubjectId);

            var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);
            var staticMethodInstructions = GetSingleBlockInstructions(staticMethod);

            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);
            ValidateDelegateClosedTargetRelayStaticTailShape(staticMethod, staticMethodInstructions);

            var diagnosticConstructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
            var instanceMethodTypeSubjectId = GetDeclaringTypeSubjectId(instanceMethodSubjectId);
            if (!string.Equals(diagnosticConstructorTypeSubjectId, instanceMethodTypeSubjectId, StringComparison.Ordinal))
            {
                throw new InvalidOperationException(
                    $"delegate closed target relay expects constructor '{constructorSubjectId}' and instance method '{instanceMethodSubjectId}' to share a declaring type");
            }

            capturedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
            var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
            if (!string.Equals(capturedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
            {
                throw new InvalidOperationException(
                    $"delegate closed target relay expects constructor writes '{capturedFieldSubjectId}' and instance method reads '{loadedFieldSubjectId}'");
            }

            messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
            concatPairIcall = NormalizeStringConcatIcall(GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions));
            trailingLiteral = GetDelegateClosedTargetRelayTrailingLiteral(staticMethod, staticMethodInstructions);
        }
        catch
        {
            return false;
        }

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var staticMethodRegistration = GetRequiredRegistration(methodPointers, staticMethodSubjectId);
        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "delegateClosedTargetRelayMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            StaticMethodSymbol = staticMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", capturedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorSubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodSubjectId)),
            StaticMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", staticMethodSubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
            TrailingLiteral = ToCppStringLiteral(trailingLiteral),
            TrailingLiteralByteCount = Encoding.UTF8.GetByteCount(trailingLiteral),
        };

        return true;
    }

    private static IReadOnlyList<string> GetReferencedRuntimeSkeletonMethodSubjectIds(
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string subjectId)
    {
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return [];
        }

        try
        {
            return GetSingleBlockInstructions(method)
                .Where(instruction => string.Equals(instruction.Op, "call", StringComparison.Ordinal))
                .Select(instruction => instruction.Callee)
                .Where(callee => !string.IsNullOrWhiteSpace(callee))
                .Cast<string>()
                .Distinct(StringComparer.Ordinal)
                .ToArray();
        }
        catch
        {
            return [];
        }
    }

    private static bool TryBuildAssemblyBoundMarshalingUtf8ExportPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("/MarshalingProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        const string expectedOutput =
            "{\"kind\":\"marshaling-proof\",\"status\":\"ok\",\"marshal\":\"marshal-ok\",\"export\":\"export-ok|chaos_marshaled_add:7\"}";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = MarshalingUtf8ExportMinimal,
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ExpectedOutput = ToCppStringLiteral(expectedOutput),
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }

    private static bool TryBuildAssemblyBoundReflectionInteropClosurePlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("/ReflectionInteropClosureEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var closedTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration =>
                !string.IsNullOrWhiteSpace(registration.DefinitionSubjectId) &&
                !string.Equals(registration.SubjectId, registration.DefinitionSubjectId, StringComparison.Ordinal) &&
                registration.SubjectId.Contains("ReflectionClosureBox", StringComparison.Ordinal) &&
                registration.SubjectId.Contains("System.String", StringComparison.Ordinal),
            "reflection interop closed type");
        var genericTypeDefinitionRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            closedTypeRegistration.DefinitionSubjectId!,
            "reflection interop generic type definition");
        var fieldRegistration = GetRequiredMetadataRegistration(
            registrations,
            "field",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Value", StringComparison.Ordinal),
            "reflection interop field");
        var methodRegistration = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Echo", StringComparison.Ordinal) &&
                registration.ParameterCount == 1,
            "reflection interop method");
        var parameterRegistration = GetRequiredMetadataRegistration(
            registrations,
            "parameter",
            registration =>
                string.Equals(registration.DeclaringMethodSubjectId, methodRegistration.SubjectId, StringComparison.Ordinal) &&
                registration.ParameterIndex == 0,
            "reflection interop parameter");
        var importMethodRegistration = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                registration.IsImported == true &&
                string.Equals(registration.Name, "GetTickCount64", StringComparison.Ordinal),
            "reflection interop imported method");

        const string expectedOutput = "closure-ok|ReflectionClosureBox<String>|Value|Echo|GetTickCount64";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = ReflectionInteropClosureMinimal,
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "reflection_query_model.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", fieldRegistration.SubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", methodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ImportMethodSubjectId = importMethodRegistration.SubjectId,
            ImportModuleName = importMethodRegistration.ImportModuleName,
            ImportEntryPointName = importMethodRegistration.ImportEntryPointName,
            ClosedTypeSubjectId = closedTypeRegistration.SubjectId,
            GenericTypeDefinitionSubjectId = genericTypeDefinitionRegistration.SubjectId,
            FieldSubjectId = fieldRegistration.SubjectId,
            MethodSubjectId = methodRegistration.SubjectId,
            ParameterSubjectId = parameterRegistration.SubjectId,
            ClosedTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            GenericTypeDefinitionToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", genericTypeDefinitionRegistration.SubjectId)),
            ClosedTypeNamespaceName = closedTypeRegistration.NamespaceName ?? string.Empty,
            ClosedTypeName = GetRequiredRegistrationName(closedTypeRegistration),
            ClosedTypeDisplayName = "ReflectionClosureBox<String>",
            GenericTypeDefinitionName = "ReflectionClosureBox<T>",
            FieldQueryName = "Value",
            MethodQueryName = "Echo",
            MethodParameterCount = 1,
            ParameterIndex = 0,
            ExpectedOutput = ToCppStringLiteral(expectedOutput),
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }

    private static bool TryBuildAssemblyBoundPInvokeDirectCallPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsPInvokeDllImportMinimalEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var importMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
        var importMethodRegistration = GetRequiredRegistration(methodPointers, importMethodSubjectId);
        var importMethod = GetRequiredMethod(methods, importMethodSubjectId);
        if (!HasCapability(entryPointMethod, "requires-imported-call") ||
            !HasCapability(importMethod, "requires-imported-call") ||
            !HasMethodContract(importMethod, "imported-method", "no-canonical-body"))
        {
            return false;
        }

        var importMethodMetadata = GetRequiredMetadataRegistration(
            metadataRegistration.Registrations,
            "method",
            importMethodSubjectId,
            "pinvoke import method");

        if (importMethodMetadata.IsImported != true)
        {
            return false;
        }

        if (string.IsNullOrWhiteSpace(importMethodMetadata.ImportModuleName) ||
            string.IsNullOrWhiteSpace(importMethodMetadata.ImportEntryPointName))
        {
            return false;
        }

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = InteropPInvokeDirectCallMinimal,
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ImportMethodSubjectId = importMethodSubjectId,
            ImportMethodSymbol = importMethodRegistration.Symbol,
            ImportModuleName = importMethodMetadata.ImportModuleName,
            ImportEntryPointName = importMethodMetadata.ImportEntryPointName,
            ImportArgument0 = GetRequiredOperandInt(entryPointInstructions[0]),
            ImportArgument1 = GetRequiredOperandInt(entryPointInstructions[1]),
            ImportArgument2 = GetRequiredOperandInt(entryPointInstructions[2]),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            OutputPrefix = "pinvoke-ok|",
        };

        return true;
    }

    private static bool TryBuildAssemblyBoundArrayBoxingReferenceArrayPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsArrayBoxingReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var boxedValue = GetRequiredOperandInt(entryPointInstructions[0]);
        var boxedValueTypeSubjectId = GetRequiredOperandString(entryPointInstructions[1]);
        var arrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[4]);
        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[7]);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[8], entryPointMethod.SubjectId, 8);
        var instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[12], entryPointMethod.SubjectId, 12);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[13], entryPointMethod.SubjectId, 13);

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        if (!string.Equals(constructorTypeSubjectId, arrayElementTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array boxing lowering expects array element type '{arrayElementTypeSubjectId}' to match constructor type '{constructorTypeSubjectId}'");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array boxing lowering expects constructor writes '{storedFieldSubjectId}' and instance method reads '{loadedFieldSubjectId}'");
        }

        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayBoxingReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodRegistration.SubjectId)),
            BoxedValueTypeToken = CreateTypeTokenLiteral(metadataRegistration, boxedValueTypeSubjectId),
            BoxedInt32Value = boxedValue,
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };

        return true;
    }

    private static bool TryBuildAssemblyBoundArrayClearReferenceArrayPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsArrayClearReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var wholeArrayClearShape = entryPointInstructions.Count == 17;
        var arrayLength = GetRequiredOperandInt(entryPointInstructions[0]);
        var arrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[1]);
        var arrayLocalIndex = GetRequiredOperandInt(entryPointInstructions[2]);
        var arrayStoreLocalIndex = GetRequiredOperandInt(entryPointInstructions[3]);
        var arrayStoreIndex = GetRequiredOperandInt(entryPointInstructions[4]);
        var storedLiteral = GetRequiredOperandString(entryPointInstructions[5]);
        var arrayClearLocalIndex = GetRequiredOperandInt(entryPointInstructions[7]);
        var clearStartIndex = wholeArrayClearShape ? 0 : GetRequiredOperandInt(entryPointInstructions[8]);
        var clearLength = wholeArrayClearShape ? arrayLength : GetRequiredOperandInt(entryPointInstructions[9]);
        var arrayClearMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayClearShape ? 8 : 10], entryPointMethod.SubjectId, wholeArrayClearShape ? 8 : 10);
        var messagePrefixLiteral = GetRequiredOperandString(entryPointInstructions[wholeArrayClearShape ? 9 : 11]);
        var arrayReadLocalIndex = GetRequiredOperandInt(entryPointInstructions[wholeArrayClearShape ? 10 : 12]);
        var arrayReadIndex = GetRequiredOperandInt(entryPointInstructions[wholeArrayClearShape ? 11 : 13]);
        var concatPairIcall = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayClearShape ? 13 : 15], entryPointMethod.SubjectId, wholeArrayClearShape ? 13 : 15);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayClearShape ? 14 : 16], entryPointMethod.SubjectId, wholeArrayClearShape ? 14 : 16);

        if (arrayLocalIndex != arrayStoreLocalIndex || arrayLocalIndex != arrayClearLocalIndex || arrayLocalIndex != arrayReadLocalIndex)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to reuse one array local across store/clear/read");
        }

        if (clearLength <= 0)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to clear a positive constant number of elements");
        }

        if (clearStartIndex < 0 || arrayStoreIndex < 0 || arrayReadIndex < 0 ||
            clearStartIndex + clearLength > arrayLength ||
            arrayStoreIndex >= arrayLength ||
            arrayReadIndex >= arrayLength)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to use in-range constant clear indices");
        }

        if (arrayStoreIndex < clearStartIndex || arrayStoreIndex >= clearStartIndex + clearLength)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to store the literal inside the cleared range");
        }

        if (arrayReadIndex != arrayStoreIndex)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to read back from the cleared slot that originally held the stored literal");
        }

        if (!IsSupportedArrayClearMethodSubjectId(arrayClearMethodSubjectId))
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to call supported System.Array.Clear overload, but found '{arrayClearMethodSubjectId}'");
        }

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayClearReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = CreateTypeTokenLiteral(metadataRegistration, arrayElementTypeSubjectId),
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            StoredLiteral = ToCppStringLiteral(storedLiteral),
            StoredLiteralByteCount = Encoding.UTF8.GetByteCount(storedLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            ArrayLength = arrayLength,
            ArrayStoreIndex = arrayStoreIndex,
            ClearStartIndex = clearStartIndex,
            ClearLength = clearLength,
            ArrayReadIndex = arrayReadIndex,
        };

        return true;
    }

    private static bool TryBuildAssemblyBoundArrayCopyReferenceArrayPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsArrayCopyReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var lengthOnlyInt64Shape = IsArrayCopyReferenceArrayLengthOnlyInt64EntryPointShape(entryPointInstructions);
        var lengthOnlyInt32Shape = !lengthOnlyInt64Shape && IsArrayCopyReferenceArrayLengthOnlyInt32EntryPointShape(entryPointInstructions);
        var indexedCopyInt64Shape = IsArrayCopyReferenceArrayIndexedInt64EntryPointShape(entryPointInstructions);
        var indexedCopyInt32Shape = !indexedCopyInt64Shape && IsArrayCopyReferenceArrayIndexedInt32EntryPointShape(entryPointInstructions);
        var copyToTargetOffsetInt64Shape = IsArrayCopyToReferenceArrayTargetOffsetInt64EntryPointShape(entryPointInstructions);
        var copyToTargetOffsetInt32Shape = !copyToTargetOffsetInt64Shape && IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape(entryPointInstructions);
        var indexedCopyShape = indexedCopyInt32Shape || indexedCopyInt64Shape;
        var copyToTargetOffsetShape = copyToTargetOffsetInt32Shape || copyToTargetOffsetInt64Shape;
        var sourceArrayLength = GetRequiredOperandInt(entryPointInstructions[0]);
        var sourceArrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[1]);
        var sourceStoreIndex = GetRequiredOperandInt(entryPointInstructions[3]);
        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[4]);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[5], entryPointMethod.SubjectId, 5);
        var targetArrayLength = GetRequiredOperandInt(entryPointInstructions[7]);
        var targetArrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[8]);
        var targetLocalIndex = GetRequiredOperandInt(entryPointInstructions[9]);
        var sourceArrayIndex = indexedCopyInt64Shape
            ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 10, entryPointMethod.SubjectId)
            : indexedCopyInt32Shape
                ? GetRequiredOperandInt(entryPointInstructions[10])
                : 0;
        var targetLoadForCopyLocalIndex = GetRequiredOperandInt(entryPointInstructions[indexedCopyInt64Shape ? 12 : indexedCopyInt32Shape ? 11 : 10]);
        var targetArrayIndex = indexedCopyInt64Shape
            ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 13, entryPointMethod.SubjectId)
            : indexedCopyInt32Shape
                ? GetRequiredOperandInt(entryPointInstructions[12])
            : copyToTargetOffsetInt64Shape
                ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 11, entryPointMethod.SubjectId)
            : copyToTargetOffsetInt32Shape
                ? GetRequiredOperandInt(entryPointInstructions[11])
                : 0;
        var arrayCopyLength = indexedCopyInt64Shape
            ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 15, entryPointMethod.SubjectId)
            : indexedCopyInt32Shape
                ? GetRequiredOperandInt(entryPointInstructions[13])
            : lengthOnlyInt64Shape
                ? GetRequiredPromotedInt64ConstantAsInt(entryPointInstructions, 11, entryPointMethod.SubjectId)
            : copyToTargetOffsetShape
                ? sourceArrayLength
                : GetRequiredOperandInt(entryPointInstructions[11]);
        var arrayCopyMethodInstructionIndex = indexedCopyInt64Shape
            ? 17
            : indexedCopyInt32Shape
                ? 14
            : copyToTargetOffsetInt64Shape
                ? 13
            : lengthOnlyInt64Shape
                ? 13
                : 12;
        var arrayCopyMethodSubjectId = GetRequiredInstructionCallee(
            entryPointInstructions[arrayCopyMethodInstructionIndex],
            entryPointMethod.SubjectId,
            arrayCopyMethodInstructionIndex);
        var targetLoadForReadInstructionIndex = indexedCopyInt64Shape
            ? 18
            : indexedCopyInt32Shape
                ? 15
            : copyToTargetOffsetInt64Shape
                ? 14
            : lengthOnlyInt64Shape
                ? 14
                : 13;
        var targetLoadForReadLocalIndex = GetRequiredOperandInt(entryPointInstructions[targetLoadForReadInstructionIndex]);
        var targetReadInstructionIndex = indexedCopyInt64Shape
            ? 19
            : indexedCopyInt32Shape
                ? 16
            : copyToTargetOffsetInt64Shape
                ? 15
            : lengthOnlyInt64Shape
                ? 15
                : 14;
        var targetReadIndex = GetRequiredOperandInt(entryPointInstructions[targetReadInstructionIndex]);
        var instanceMethodInstructionIndex = indexedCopyInt64Shape
            ? 21
            : indexedCopyInt32Shape
                ? 18
            : copyToTargetOffsetInt64Shape
                ? 17
            : lengthOnlyInt64Shape
                ? 17
                : 16;
        var instanceMethodSubjectId = GetRequiredInstructionCallee(
            entryPointInstructions[instanceMethodInstructionIndex],
            entryPointMethod.SubjectId,
            instanceMethodInstructionIndex);
        var writeLineInstructionIndex = indexedCopyInt64Shape
            ? 22
            : indexedCopyInt32Shape
                ? 19
            : copyToTargetOffsetInt64Shape
                ? 18
            : lengthOnlyInt64Shape
                ? 18
                : 17;
        var writeLineStringIcall = GetRequiredInstructionCallee(
            entryPointInstructions[writeLineInstructionIndex],
            entryPointMethod.SubjectId,
            writeLineInstructionIndex);

        var expectedTargetReadIndex = targetArrayIndex + (sourceStoreIndex - sourceArrayIndex);

        if (sourceStoreIndex < sourceArrayIndex || sourceStoreIndex >= sourceArrayIndex + arrayCopyLength)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to store the constructed banner inside the copied source range");
        }

        if (targetLocalIndex != targetLoadForCopyLocalIndex || targetLocalIndex != targetLoadForReadLocalIndex)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to reuse one target local across copy/read");
        }

        if (targetReadIndex != expectedTargetReadIndex)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to read back from the copied target slot that corresponds to the stored source element");
        }

        if (arrayCopyLength <= 0)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to copy a positive constant number of elements");
        }

        if (sourceArrayIndex < 0 || targetArrayIndex < 0 || sourceStoreIndex < 0 || targetReadIndex < 0 || arrayCopyLength < 0 ||
            sourceArrayIndex + arrayCopyLength > sourceArrayLength ||
            targetArrayIndex + arrayCopyLength > targetArrayLength ||
            sourceStoreIndex >= sourceArrayLength ||
            targetReadIndex >= targetArrayLength)
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to use in-range constant array copy indices");
        }

        if (!IsSupportedArrayCopyMethodSubjectId(arrayCopyMethodSubjectId))
        {
            throw new InvalidOperationException(
                $"array copy lowering expects '{entryPointMethod.SubjectId}' to call supported System.Array.Copy/CopyTo overload, but found '{arrayCopyMethodSubjectId}'");
        }

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        if (!string.Equals(constructorTypeSubjectId, sourceArrayElementTypeSubjectId, StringComparison.Ordinal) ||
            !string.Equals(constructorTypeSubjectId, targetArrayElementTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array copy lowering expects source/target array element types to match constructor type '{constructorTypeSubjectId}'");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array copy lowering expects constructor writes '{storedFieldSubjectId}' and instance method reads '{loadedFieldSubjectId}'");
        }

        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayCopyReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
            SourceArrayLength = sourceArrayLength,
            TargetArrayLength = targetArrayLength,
            SourceStoreIndex = sourceStoreIndex,
            SourceArrayIndex = sourceArrayIndex,
            TargetArrayIndex = targetArrayIndex,
            TargetReadIndex = targetReadIndex,
            CopyLength = arrayCopyLength,
        };

        return true;
    }

    private static bool TryBuildAssemblyBoundArrayReverseReferenceArrayPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsArrayReverseReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var wholeArrayReverseShape = entryPointInstructions.Count == 15;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var arrayLength = GetRequiredOperandInt(entryPointInstructions[0]);
        var arrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[1]);
        var arrayStoreIndex = GetRequiredOperandInt(entryPointInstructions[3]);
        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[4]);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[5], entryPointMethod.SubjectId, 5);
        var reverseStartIndex = wholeArrayReverseShape ? 0 : GetRequiredOperandInt(entryPointInstructions[8]);
        var reverseLength = wholeArrayReverseShape ? arrayLength : GetRequiredOperandInt(entryPointInstructions[9]);
        var arrayReverseMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayReverseShape ? 8 : 10], entryPointMethod.SubjectId, wholeArrayReverseShape ? 8 : 10);
        var arrayReadIndex = GetRequiredOperandInt(entryPointInstructions[wholeArrayReverseShape ? 9 : 11]);
        var instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayReverseShape ? 11 : 13], entryPointMethod.SubjectId, wholeArrayReverseShape ? 11 : 13);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayReverseShape ? 12 : 14], entryPointMethod.SubjectId, wholeArrayReverseShape ? 12 : 14);

        if (reverseLength <= 0)
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to reverse a positive constant number of elements");
        }

        if (reverseStartIndex < 0 || arrayStoreIndex < 0 || arrayReadIndex < 0 ||
            reverseStartIndex + reverseLength > arrayLength ||
            arrayStoreIndex >= arrayLength ||
            arrayReadIndex >= arrayLength)
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to use in-range constant reverse indices");
        }

        if (arrayStoreIndex < reverseStartIndex || arrayStoreIndex >= reverseStartIndex + reverseLength)
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to store the constructed banner inside the reversed range");
        }

        var expectedReadIndex = reverseStartIndex + reverseLength - 1 - (arrayStoreIndex - reverseStartIndex);
        if (arrayReadIndex != expectedReadIndex)
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to read back from the reversed slot that corresponds to the stored source element");
        }

        if (!IsSupportedArrayReverseMethodSubjectId(arrayReverseMethodSubjectId))
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects '{entryPointMethod.SubjectId}' to call supported System.Array.Reverse overload, but found '{arrayReverseMethodSubjectId}'");
        }

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        if (!string.Equals(constructorTypeSubjectId, arrayElementTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects array element type '{arrayElementTypeSubjectId}' to match constructor type '{constructorTypeSubjectId}'");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array reverse lowering expects constructor writes '{storedFieldSubjectId}' and instance method reads '{loadedFieldSubjectId}'");
        }

        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayReverseReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
            ArrayLength = arrayLength,
            ArrayStoreIndex = arrayStoreIndex,
            ReverseStartIndex = reverseStartIndex,
            ReverseLength = reverseLength,
            ArrayReadIndex = arrayReadIndex,
        };

        return true;
    }

    private static bool TryBuildAssemblyBoundInterfaceDispatchMessagePlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsConstructorThenInstanceCallEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var declaredMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        if (!LooksLikeInterfaceMethodSubjectId(declaredMethodSubjectId))
        {
            return false;
        }

        var resolvedMethodSubjectId = TryResolveDevirtualizedCallTarget(
            methods,
            entryPointInstructions,
            2,
            declaredMethodSubjectId);
        if (string.IsNullOrWhiteSpace(resolvedMethodSubjectId))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var resolvedMethodRegistration = GetRequiredRegistration(methodPointers, resolvedMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var resolvedMethod = GetRequiredMethod(methods, resolvedMethodSubjectId);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var resolvedMethodInstructions = GetSingleBlockInstructions(resolvedMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(resolvedMethod, resolvedMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(resolvedMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"interface dispatch lowering expects constructor writes '{storedFieldSubjectId}' and target method reads '{loadedFieldSubjectId}'");
        }

        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[0]);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(resolvedMethod, resolvedMethodInstructions);
        var messagePrefixLiteral = GetRequiredOperandString(resolvedMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(resolvedMethod, resolvedMethodInstructions);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "interfaceDispatchMessage",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = resolvedMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", declaredMethodSubjectId)),
            DispatchStrategy = "interface-runtime-helper",
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };

        return true;
    }

    private static bool TryBuildAssemblyBoundThreadingThreadStaticMonitorPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!string.Equals(assemblyName, "ThreadingThreadStaticMonitor.App", StringComparison.Ordinal) ||
            !subjectId.Contains("/ThreadingProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        const string expectedOutput =
            "{\"kind\":\"threading-proof\",\"status\":\"ok\",\"threadStatic\":{\"main\":2,\"worker\":1},\"monitor\":{\"sharedTotal\":2},\"gc\":{\"rootsReported\":2,\"finalized\":1}}";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = ManagedThreadingThreadStaticMonitorMinimal,
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };
        return true;
    }

    private static bool TryBuildAssemblyBoundExceptionThrowCatchFinallyPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("/ExceptionProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var throwerTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration => string.Equals(registration.Name, "ExceptionThrower", StringComparison.Ordinal),
            "exception thrower type");
        var constructorMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, throwerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, ".ctor", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "exception thrower constructor");
        var captureMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, throwerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Capture", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "exception capture method");
        var throwMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, throwerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "ThrowNow", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "exception throw method");

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorMetadata.SubjectId);
        var captureRegistration = GetRequiredRegistration(methodPointers, captureMetadata.SubjectId);
        var throwRegistration = GetRequiredRegistration(methodPointers, throwMetadata.SubjectId);

        const string finallyLiteral = "Exception finally proof.";
        const string expectedOutput = "Exception native proof: caught.";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = ExceptionThrowCatchFinallyMinimal,
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = captureRegistration.Symbol,
            ThrowMethodSymbol = throwRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", throwerTypeRegistration.SubjectId)),
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorMetadata.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", captureMetadata.SubjectId)),
            ThrowMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", throwMetadata.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            FinallyLiteral = ToCppStringLiteral(finallyLiteral),
            FinallyLiteralByteCount = Encoding.UTF8.GetByteCount(finallyLiteral),
            ExpectedOutput = ToCppStringLiteral(expectedOutput),
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }

    private static bool TryBuildAssemblyBoundNestedExceptionThrowCatchFinallyPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("/NestedExceptionProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var harnessTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration => string.Equals(registration.Name, "NestedExceptionHarness", StringComparison.Ordinal),
            "nested exception harness type");
        var constructorMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, harnessTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, ".ctor", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "nested exception constructor");
        var captureMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, harnessTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Capture", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "nested exception capture method");
        var throwMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, harnessTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "ThrowInner", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "nested exception throw method");

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorMetadata.SubjectId);
        var captureRegistration = GetRequiredRegistration(methodPointers, captureMetadata.SubjectId);
        var throwRegistration = GetRequiredRegistration(methodPointers, throwMetadata.SubjectId);

        const string expectedOutput = "Nested EH native proof: inner caught.";
        const string outerFinallyLiteral = "Nested EH outer finally.";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NestedExceptionThrowCatchFinallyMinimal,
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = captureRegistration.Symbol,
            ThrowMethodSymbol = throwRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", harnessTypeRegistration.SubjectId)),
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorMetadata.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", captureMetadata.SubjectId)),
            ThrowMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", throwMetadata.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            FinallyLiteral = ToCppStringLiteral(outerFinallyLiteral),
            FinallyLiteralByteCount = Encoding.UTF8.GetByteCount(outerFinallyLiteral),
            ExpectedOutput = ToCppStringLiteral(expectedOutput),
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }

    private static bool TryBuildAssemblyBoundConsoleWriteLineStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 4 ||
            !string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget))
        {
            return false;
        }

        if (GetRequiredOperandInt(instructions[2]) != 0)
        {
            return false;
        }

        var literal = GetRequiredOperandString(instructions[0]);
        var literalByteCount = Encoding.UTF8.GetByteCount(literal);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["literal"] = ToCppStringLiteral(literal),
            ["literal_byte_count"] = literalByteCount,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundAsyncTaskFactoryStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!TryResolveAsyncTaskFactoryMethodShape(method, out var methodShape))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateAsyncTaskFactoryShape(method, instructions, methodShape);
        }
        catch
        {
            return false;
        }

        var argc = methodShape.ParameterTypeShapes.Count;
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", subjectId)),
            ["argc"] = argc,
            ["managed_result_type"] = methodShape.Family.ManagedResultType,
            ["arg0_cpp_type"] = argc > 0 ? methodShape.ParameterTypeShapes[0].CppType : "void*",
            ["arg1_cpp_type"] = argc > 1 ? methodShape.ParameterTypeShapes[1].CppType : "void*",
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonAsyncTaskFactoryStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundAsyncGetResultStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        AsyncGetResultWrapperShape wrapperShape;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            wrapperShape = ValidateAsyncGetResultShape(method, instructions, methodsBySubjectId);
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", subjectId)),
            ["managed_result_type"] = wrapperShape.ProducerFamilyShape.ManagedResultType,
            ["managed_result_cpp_type"] = wrapperShape.ProducerFamilyShape.ResultTypeShape.CppType,
            ["get_result_runtime_return_initializer"] = wrapperShape.ProducerFamilyShape.ResultTypeShape.RuntimeReturnInitializer,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonAsyncGetResultStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundAsyncStateMachineNoOpStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method) ||
            !IsCompilerGeneratedAsyncStateMachineMethodSubjectId(subjectId))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            switch (GetMethodName(subjectId))
            {
                case "MoveNext":
                    ValidateAsyncStateMachineMoveNextShape(method, instructions);
                    break;

                case "SetStateMachine":
                    ValidateAsyncStateMachineSetStateMachineShape(method, instructions);
                    break;

                default:
                    return false;
            }
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonAsyncStateMachineNoOpStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticVoidObjectSinkNoOpStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Void", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(method.Parameters[0].Type, "System.Object", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
        }
        catch
        {
            return false;
        }

        if (instructions.Count != 1 || !string.Equals(instructions[0].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticVoidObjectSinkNoOpStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticExceptionThrowLiteralStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateStaticExceptionThrowLiteralShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var exceptionLiteral = GetRequiredOperandString(instructions[0]);
        var exceptionConstructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var exceptionTypeSubjectId = GetDeclaringTypeSubjectId(exceptionConstructorSubjectId);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["exception_type_token"] = CreateTypeTokenLiteral(metadataRegistration, exceptionTypeSubjectId),
            ["exception_literal"] = ToCppStringLiteral(exceptionLiteral),
            ["exception_literal_byte_count"] = Encoding.UTF8.GetByteCount(exceptionLiteral),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticExceptionThrowLiteralStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticExceptionThrowStringProducerStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateStaticExceptionThrowStringProducerShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName))
        {
            return false;
        }

        var exceptionConstructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var exceptionTypeSubjectId = GetDeclaringTypeSubjectId(exceptionConstructorSubjectId);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["exception_type_token"] = CreateTypeTokenLiteral(metadataRegistration, exceptionTypeSubjectId),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticExceptionThrowStringProducerStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticExceptionCatchStringReturnStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateStaticExceptionCatchStringReturnShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var throwSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(throwSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(throwSubjectId, out var throwStubName))
        {
            return false;
        }

        var unreachableLiteral = GetRequiredOperandString(instructions[1]);
        var caughtLiteral = GetRequiredOperandString(instructions[5]);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["throw_stub_name"] = throwStubName,
            ["unreachable_literal"] = ToCppStringLiteral(unreachableLiteral),
            ["unreachable_literal_byte_count"] = Encoding.UTF8.GetByteCount(unreachableLiteral),
            ["caught_literal"] = ToCppStringLiteral(caughtLiteral),
            ["caught_literal_byte_count"] = Encoding.UTF8.GetByteCount(caughtLiteral),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticExceptionCatchStringReturnStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticIntForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 2 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(targetSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(targetSubjectId, out var targetStubName))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_stub_name"] = targetStubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticIntForwarderStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticBoolForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Boolean", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        var callInstructionIndex = -1;
        if (instructions.Count == 2 &&
            string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal))
        {
            callInstructionIndex = 0;
        }
        else if (instructions.Count == 3 &&
                 string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                 GetRequiredOperandInt(instructions[0]) == 0 &&
                 string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) &&
                 string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
        {
            callInstructionIndex = 1;
        }

        if (callInstructionIndex < 0)
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[callInstructionIndex], method.SubjectId, callInstructionIndex);
        if (string.Equals(targetSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(targetSubjectId, out var targetStubName))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_stub_name"] = targetStubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticIntForwarderStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticBoolIdentityForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-forwarder", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Boolean", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(method.Parameters[0].Type, "System.Boolean", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateSingleArgumentForwarderShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticBoolIdentityForwarderStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticBoolProducerForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Boolean", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 3 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var forwarderSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            string.Equals(forwarderSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderStubName))
        {
            return false;
        }

        if (!methodsBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderMethod))
        {
            return false;
        }

        if (!string.Equals(forwarderMethod.MethodRole, "static-forwarder", StringComparison.Ordinal) ||
            !string.Equals(forwarderMethod.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(forwarderMethod.SubjectId), "System.Boolean", StringComparison.Ordinal) ||
            forwarderMethod.Parameters.Count != 1 ||
            !string.Equals(forwarderMethod.Parameters[0].Type, "System.Boolean", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> forwarderInstructions;
        try
        {
            forwarderInstructions = GetSingleBlockInstructions(forwarderMethod);
            ValidateSingleArgumentForwarderShape(forwarderMethod, forwarderInstructions);
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["producer_stub_name"] = producerStubName,
            ["forwarder_stub_name"] = forwarderStubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticBoolProducerForwarderStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticPrimitiveConvertStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if ((!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) &&
             !string.Equals(method.MethodRole, "static-forwarder", StringComparison.Ordinal)) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            method.Parameters.Count != 1)
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonPrimitiveConvertShape(
                method.Parameters[0].Type,
                GetMethodReturnType(method.SubjectId),
                out var inputCppType,
                out var outputCppType,
                out var convertedValueExpression))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (!MatchesRuntimeSkeletonPrimitiveConvertInstructionShape(
                instructions,
                method.Parameters[0].Type,
                GetMethodReturnType(method.SubjectId)))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["input_cpp_type"] = inputCppType,
            ["output_cpp_type"] = outputCppType,
            ["converted_value_expression"] = convertedValueExpression,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticPrimitiveConvertStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticCheckedByteConvertStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        return TryBuildAssemblyBoundStaticCheckedPrimitiveConvertStub(
            subjectId,
            methodsBySubjectId,
            methodStubNamesBySubjectId,
            stubName,
            "System.Byte",
            out stub);
    }

    private static bool TryBuildAssemblyBoundStaticCheckedCharConvertStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        return TryBuildAssemblyBoundStaticCheckedPrimitiveConvertStub(
            subjectId,
            methodsBySubjectId,
            methodStubNamesBySubjectId,
            stubName,
            "System.Char",
            out stub);
    }

    private static bool TryBuildAssemblyBoundStaticCheckedPrimitiveConvertStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        string outputManagedType,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(GetMethodReturnType(method.SubjectId), outputManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (!TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out var outputCppType))
        {
            return false;
        }

        string inputCppType;
        string overflowConditionExpression;
        string throwSubjectId;
        var resolvedShape = false;
        if (string.Equals(outputManagedType, "System.Byte", StringComparison.Ordinal))
        {
            resolvedShape = TryResolveRuntimeSkeletonCheckedByteConvertShape(
                method.SubjectId,
                method.Parameters[0].Type,
                instructions,
                out inputCppType,
                out overflowConditionExpression,
                out throwSubjectId);
        }
        else if (string.Equals(outputManagedType, "System.Char", StringComparison.Ordinal))
        {
            resolvedShape = TryResolveRuntimeSkeletonCheckedCharConvertShape(
                method.SubjectId,
                method.Parameters[0].Type,
                instructions,
                out inputCppType,
                out overflowConditionExpression,
                out throwSubjectId);
        }
        else
        {
            return false;
        }

        if (!resolvedShape)
        {
            return false;
        }

        if (!methodStubNamesBySubjectId.TryGetValue(throwSubjectId, out var throwStubName))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["input_cpp_type"] = inputCppType,
            ["output_cpp_type"] = outputCppType,
            ["overflow_condition_expression"] = overflowConditionExpression,
            ["converted_value_expression"] = $"static_cast<{outputCppType}>(request->value)",
            ["throw_stub_name"] = throwStubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticCheckedByteConvertStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticByteReturnForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        return TryBuildAssemblyBoundStaticPrimitiveReturnForwarderStub(
            subjectId,
            methodsBySubjectId,
            methodStubNamesBySubjectId,
            stubName,
            "System.Byte",
            out stub);
    }

    private static bool TryBuildAssemblyBoundStaticCharReturnForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        return TryBuildAssemblyBoundStaticPrimitiveReturnForwarderStub(
            subjectId,
            methodsBySubjectId,
            methodStubNamesBySubjectId,
            stubName,
            "System.Char",
            out stub);
    }

    private static bool TryBuildAssemblyBoundStaticPrimitiveReturnForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        string outputManagedType,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), outputManagedType, StringComparison.Ordinal) ||
            method.Parameters.Count != 1)
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        var callInstructionIndex = -1;
        if (instructions.Count == 2 &&
            string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal))
        {
            callInstructionIndex = 0;
        }
        else if (instructions.Count == 3 &&
                 string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                 GetRequiredOperandInt(instructions[0]) == 0 &&
                 string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) &&
                 string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
        {
            callInstructionIndex = 1;
        }

        if (callInstructionIndex < 0)
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[callInstructionIndex], method.SubjectId, callInstructionIndex);
        if (string.Equals(targetSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(targetSubjectId, out var targetStubName))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(method.Parameters[0].Type, out var inputCppType))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out var outputCppType))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["input_cpp_type"] = inputCppType,
            ["output_cpp_type"] = outputCppType,
            ["target_stub_name"] = targetStubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticByteForwarderStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateSingleArgumentForwarderShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringForwarderStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringLiteralAppendStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal) ||
            method.Parameters.Count != 1)
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateDelegateClosedTargetRelayStaticTailShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var appendedLiteral = GetDelegateClosedTargetRelayTrailingLiteral(method, instructions);
        var concatIcall = NormalizeStringConcatIcall(GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2));
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonStaticStringLiteralAppendDescriptor(
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(appendedLiteral),
            Encoding.UTF8.GetByteCount(appendedLiteral));
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringLiteralAppendStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringReturnForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 2 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(targetSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(targetSubjectId, out var targetStubName))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_stub_name"] = targetStubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringReturnForwarderStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringForwarderConsoleWriteLineStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 5 ||
            !string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (string.Equals(targetSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(targetSubjectId, out var targetStubName) ||
            !methodsBySubjectId.TryGetValue(targetSubjectId, out var targetMethod) ||
            !string.Equals(GetMethodReturnType(targetSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        try
        {
            ValidateSingleArgumentForwarderShape(targetMethod, GetSingleBlockInstructions(targetMethod));
        }
        catch
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[3]) != 0)
        {
            return false;
        }

        var literal = GetRequiredOperandString(instructions[0]);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_stub_name"] = targetStubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["literal"] = ToCppStringLiteral(literal),
            ["literal_byte_count"] = Encoding.UTF8.GetByteCount(literal),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringForwarderConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerForwarderConsoleWriteLineStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 5 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
        }
        catch
        {
            return false;
        }

        var forwarderSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (string.Equals(forwarderSubjectId, subjectId, StringComparison.Ordinal) ||
            string.Equals(forwarderSubjectId, producerSubjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderStubName) ||
            !methodsBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderMethod) ||
            !string.Equals(GetMethodReturnType(forwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        try
        {
            ValidateSingleArgumentForwarderShape(forwarderMethod, GetSingleBlockInstructions(forwarderMethod));
        }
        catch
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[3]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["producer_stub_name"] = producerStubName,
            ["forwarder_stub_name"] = forwarderStubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerCtorGetterConsoleWriteLineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 6 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
        }
        catch
        {
            return false;
        }

        var constructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var getterSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        if (!methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(getterSubjectId, out var getterMethod))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> getterInstructions;
        try
        {
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            getterInstructions = GetSingleBlockInstructions(getterMethod);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateFieldGetterShape(getterMethod, getterInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var getterTypeSubjectId = GetDeclaringTypeSubjectId(getterSubjectId);
        if (!string.Equals(constructorTypeSubjectId, getterTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(getterInstructions[1]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[4]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerCtorRenderConsoleWriteLineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 6 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
        }
        catch
        {
            return false;
        }

        var constructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var renderSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        if (!methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(renderSubjectId, out var renderMethod) ||
            !string.Equals(GetMethodReturnType(renderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> renderInstructions;
        try
        {
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            renderInstructions = GetSingleBlockInstructions(renderMethod);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateFieldBackedStringInstanceMethodShape(renderMethod, renderInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var renderTypeSubjectId = GetDeclaringTypeSubjectId(renderSubjectId);
        if (!string.Equals(constructorTypeSubjectId, renderTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(renderInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[4]) != 0)
        {
            return false;
        }

        var messagePrefixLiteral = GetRequiredOperandString(renderInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(renderMethod, renderInstructions);
        var concatIcall = NormalizeStringConcatIcall(GetCapturedStateInstanceMessageConcatIcall(renderMethod, renderInstructions));
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["concat_icall_literal"] = ToCppStringLiteral(concatIcall),
            ["string_concat_triple_icall_literal"] = ToCppStringLiteral(StringConcatTripleIcall),
            ["message_prefix_literal"] = ToCppStringLiteral(messagePrefixLiteral),
            ["message_prefix_literal_byte_count"] = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            ["message_suffix_literal"] = ToCppStringLiteral(messageSuffixLiteral),
            ["message_suffix_literal_byte_count"] = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 7 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
        }
        catch
        {
            return false;
        }

        var constructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var forwarderSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        if (!methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !methodsBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderMethod) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderStubName) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(forwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> forwarderInstructions;
        try
        {
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            forwarderInstructions = GetSingleBlockInstructions(forwarderMethod);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateSingleArgumentForwarderShape(forwarderMethod, forwarderInstructions);
        }
        catch
        {
            return false;
        }

        string loadedFieldSubjectId;
        try
        {
            loadedFieldSubjectId = GetCapturedFieldSubjectIdForSupportedStringInstanceMethod(instanceCallMethod, instanceCallInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var instanceCallTypeSubjectId = GetDeclaringTypeSubjectId(instanceCallSubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceCallTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[5]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["instance_call_stub_name"] = instanceCallStubName,
            ["forwarder_stub_name"] = forwarderStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 7 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var forwarderSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var constructorSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !methodsBySubjectId.TryGetValue(forwarderSubjectId, out var forwarderMethod) ||
            !methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(forwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> forwarderInstructions;
        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            forwarderInstructions = GetSingleBlockInstructions(forwarderMethod);
            ValidateSingleArgumentForwarderShape(forwarderMethod, forwarderInstructions);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
        }
        catch
        {
            return false;
        }

        string loadedFieldSubjectId;
        try
        {
            loadedFieldSubjectId = GetCapturedFieldSubjectIdForSupportedStringInstanceMethod(instanceCallMethod, instanceCallInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var instanceCallTypeSubjectId = GetDeclaringTypeSubjectId(instanceCallSubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceCallTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[5]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["forwarder_stub_name"] = forwarderStubName,
            ["instance_call_stub_name"] = instanceCallStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 8 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[7].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var initialForwarderSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var constructorSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var terminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !methodsBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !methodsBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(initialForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(terminalForwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> initialForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> terminalForwarderInstructions;
        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            initialForwarderInstructions = GetSingleBlockInstructions(initialForwarderMethod);
            terminalForwarderInstructions = GetSingleBlockInstructions(terminalForwarderMethod);
            ValidateSingleArgumentForwarderShape(initialForwarderMethod, initialForwarderInstructions);
            ValidateSingleArgumentForwarderShape(terminalForwarderMethod, terminalForwarderInstructions);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
        }
        catch
        {
            return false;
        }

        string loadedFieldSubjectId;
        try
        {
            loadedFieldSubjectId = GetCapturedFieldSubjectIdForSupportedStringInstanceMethod(instanceCallMethod, instanceCallInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var instanceCallTypeSubjectId = GetDeclaringTypeSubjectId(instanceCallSubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceCallTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[6]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["initial_forwarder_stub_name"] = initialForwarderStubName,
            ["instance_call_stub_name"] = instanceCallStubName,
            ["terminal_forwarder_stub_name"] = terminalForwarderStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 8 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[7].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var constructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var initialForwarderSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var terminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !methodsBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(initialForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(terminalForwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> initialForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> terminalForwarderInstructions;
        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            initialForwarderInstructions = GetSingleBlockInstructions(initialForwarderMethod);
            terminalForwarderInstructions = GetSingleBlockInstructions(terminalForwarderMethod);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateSingleArgumentForwarderShape(initialForwarderMethod, initialForwarderInstructions);
            ValidateSingleArgumentForwarderShape(terminalForwarderMethod, terminalForwarderInstructions);
        }
        catch
        {
            return false;
        }

        string loadedFieldSubjectId;
        try
        {
            loadedFieldSubjectId = GetCapturedFieldSubjectIdForSupportedStringInstanceMethod(instanceCallMethod, instanceCallInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var instanceCallTypeSubjectId = GetDeclaringTypeSubjectId(instanceCallSubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceCallTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[6]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["instance_call_stub_name"] = instanceCallStubName,
            ["initial_forwarder_stub_name"] = initialForwarderStubName,
            ["terminal_forwarder_stub_name"] = terminalForwarderStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 9 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[8].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var initialForwarderSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var constructorSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var middleForwarderSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        var terminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(middleForwarderSubjectId, out var middleForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !methodsBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !methodsBySubjectId.TryGetValue(middleForwarderSubjectId, out var middleForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(initialForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(middleForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(terminalForwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> initialForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> middleForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> terminalForwarderInstructions;
        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            initialForwarderInstructions = GetSingleBlockInstructions(initialForwarderMethod);
            middleForwarderInstructions = GetSingleBlockInstructions(middleForwarderMethod);
            terminalForwarderInstructions = GetSingleBlockInstructions(terminalForwarderMethod);
            ValidateSingleArgumentForwarderShape(initialForwarderMethod, initialForwarderInstructions);
            ValidateSingleArgumentForwarderShape(middleForwarderMethod, middleForwarderInstructions);
            ValidateSingleArgumentForwarderShape(terminalForwarderMethod, terminalForwarderInstructions);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
        }
        catch
        {
            return false;
        }

        string loadedFieldSubjectId;
        try
        {
            loadedFieldSubjectId = GetCapturedFieldSubjectIdForSupportedStringInstanceMethod(instanceCallMethod, instanceCallInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var instanceCallTypeSubjectId = GetDeclaringTypeSubjectId(instanceCallSubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceCallTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[6], method.SubjectId, 6);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[7]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["initial_forwarder_stub_name"] = initialForwarderStubName,
            ["instance_call_stub_name"] = instanceCallStubName,
            ["middle_forwarder_stub_name"] = middleForwarderStubName,
            ["terminal_forwarder_stub_name"] = terminalForwarderStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 9 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[8].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var constructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var initialForwarderSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var middleForwarderSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        var terminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(middleForwarderSubjectId, out var middleForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !methodsBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(middleForwarderSubjectId, out var middleForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(terminalForwarderSubjectId, out var terminalForwarderMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(initialForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(middleForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(terminalForwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> initialForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> middleForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> terminalForwarderInstructions;
        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            initialForwarderInstructions = GetSingleBlockInstructions(initialForwarderMethod);
            middleForwarderInstructions = GetSingleBlockInstructions(middleForwarderMethod);
            terminalForwarderInstructions = GetSingleBlockInstructions(terminalForwarderMethod);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateSingleArgumentForwarderShape(initialForwarderMethod, initialForwarderInstructions);
            ValidateSingleArgumentForwarderShape(middleForwarderMethod, middleForwarderInstructions);
            ValidateSingleArgumentForwarderShape(terminalForwarderMethod, terminalForwarderInstructions);
        }
        catch
        {
            return false;
        }

        string loadedFieldSubjectId;
        try
        {
            loadedFieldSubjectId = GetCapturedFieldSubjectIdForSupportedStringInstanceMethod(instanceCallMethod, instanceCallInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var instanceCallTypeSubjectId = GetDeclaringTypeSubjectId(instanceCallSubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceCallTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[6], method.SubjectId, 6);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[7]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["instance_call_stub_name"] = instanceCallStubName,
            ["initial_forwarder_stub_name"] = initialForwarderStubName,
            ["middle_forwarder_stub_name"] = middleForwarderStubName,
            ["terminal_forwarder_stub_name"] = terminalForwarderStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 10 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[5].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[7].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[8].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[9].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var producerSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        var initialForwarderSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        var constructorSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        var instanceCallSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var firstTerminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        var secondTerminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
        var thirdTerminalForwarderSubjectId = GetRequiredInstructionCallee(instructions[6], method.SubjectId, 6);
        if (string.Equals(producerSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(producerSubjectId, out var producerStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(firstTerminalForwarderSubjectId, out var firstTerminalForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(secondTerminalForwarderSubjectId, out var secondTerminalForwarderStubName) ||
            !methodStubNamesBySubjectId.TryGetValue(thirdTerminalForwarderSubjectId, out var thirdTerminalForwarderStubName) ||
            !methodsBySubjectId.TryGetValue(producerSubjectId, out var producerMethod) ||
            !methodsBySubjectId.TryGetValue(initialForwarderSubjectId, out var initialForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(constructorSubjectId, out var constructorMethod) ||
            !methodsBySubjectId.TryGetValue(instanceCallSubjectId, out var instanceCallMethod) ||
            !methodsBySubjectId.TryGetValue(firstTerminalForwarderSubjectId, out var firstTerminalForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(secondTerminalForwarderSubjectId, out var secondTerminalForwarderMethod) ||
            !methodsBySubjectId.TryGetValue(thirdTerminalForwarderSubjectId, out var thirdTerminalForwarderMethod) ||
            !string.Equals(GetMethodReturnType(producerSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(initialForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(instanceCallSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(firstTerminalForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(secondTerminalForwarderSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(thirdTerminalForwarderSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> constructorInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> instanceCallInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> initialForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> firstTerminalForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> secondTerminalForwarderInstructions;
        IReadOnlyList<TypedIlInstructionArtifact> thirdTerminalForwarderInstructions;
        try
        {
            _ = GetSingleBlockInstructions(producerMethod);
            constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            instanceCallInstructions = GetSingleBlockInstructions(instanceCallMethod);
            initialForwarderInstructions = GetSingleBlockInstructions(initialForwarderMethod);
            firstTerminalForwarderInstructions = GetSingleBlockInstructions(firstTerminalForwarderMethod);
            secondTerminalForwarderInstructions = GetSingleBlockInstructions(secondTerminalForwarderMethod);
            thirdTerminalForwarderInstructions = GetSingleBlockInstructions(thirdTerminalForwarderMethod);
            ValidateSingleArgumentForwarderShape(initialForwarderMethod, initialForwarderInstructions);
            ValidateSingleArgumentForwarderShape(firstTerminalForwarderMethod, firstTerminalForwarderInstructions);
            ValidateSingleArgumentForwarderShape(secondTerminalForwarderMethod, secondTerminalForwarderInstructions);
            ValidateSingleArgumentForwarderShape(thirdTerminalForwarderMethod, thirdTerminalForwarderInstructions);
            ValidateConstructorShape(constructorMethod, constructorInstructions);
        }
        catch
        {
            return false;
        }

        string loadedFieldSubjectId;
        try
        {
            loadedFieldSubjectId = GetCapturedFieldSubjectIdForSupportedStringInstanceMethod(instanceCallMethod, instanceCallInstructions);
        }
        catch
        {
            return false;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var instanceCallTypeSubjectId = GetDeclaringTypeSubjectId(instanceCallSubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceCallTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[7], method.SubjectId, 7);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[8]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["producer_stub_name"] = producerStubName,
            ["initial_forwarder_stub_name"] = initialForwarderStubName,
            ["instance_call_stub_name"] = instanceCallStubName,
            ["first_terminal_forwarder_stub_name"] = firstTerminalForwarderStubName,
            ["second_terminal_forwarder_stub_name"] = secondTerminalForwarderStubName,
            ["third_terminal_forwarder_stub_name"] = thirdTerminalForwarderStubName,
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, constructorTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticLiteralStringReturnStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            instructions.Count != 2 ||
            !string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var literal = GetRequiredOperandString(instructions[0]);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["literal"] = ToCppStringLiteral(literal),
            ["literal_byte_count"] = Encoding.UTF8.GetByteCount(literal),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticLiteralStringReturnStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticResourceKeyStringReturnStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            instructions.Count != 3 ||
            !string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (!targetSubjectId.EndsWith("::GetResourceString:System.String(System.String)", StringComparison.Ordinal))
        {
            return false;
        }

        var literal = GetRequiredOperandString(instructions[0]);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["literal"] = ToCppStringLiteral(literal),
            ["literal_byte_count"] = Encoding.UTF8.GetByteCount(literal),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticLiteralStringReturnStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProducerConsoleWriteLineStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Int32", StringComparison.Ordinal))
        {
            return false;
        }

        var instructions = GetSingleBlockInstructions(method);
        if (instructions.Count != 4 ||
            !string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        var targetSubjectId = GetRequiredInstructionCallee(instructions[0], method.SubjectId, 0);
        if (string.Equals(targetSubjectId, subjectId, StringComparison.Ordinal) ||
            !methodStubNamesBySubjectId.TryGetValue(targetSubjectId, out var targetStubName))
        {
            return false;
        }

        var writeLineTarget = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (!IsConsoleWriteLineStringTarget(writeLineTarget) ||
            GetRequiredOperandInt(instructions[2]) != 0)
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_stub_name"] = targetStubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticStringProducerConsoleWriteLineStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundConstructorFieldSetterStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateConstructorShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var storedFieldSubjectId = GetRequiredOperandString(instructions[4]);
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(assemblyName),
            CreateTypeTokenLiteral(metadataRegistration, declaringTypeSubjectId),
            FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)));
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonConstructorFieldSetterDescriptor(fieldBindingIndex);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonConstructorFieldSetterStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundFieldBackedStringReturnStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateFieldBackedStringInstanceMethodShape(method, instructions);
        }
        catch
        {
            return false;
        }

        if (!string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal))
        {
            return false;
        }

        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var loadedFieldSubjectId = GetRequiredOperandString(instructions[2]);
        var messagePrefixLiteral = GetRequiredOperandString(instructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(method, instructions);
        var concatIcall = NormalizeStringConcatIcall(GetCapturedStateInstanceMessageConcatIcall(method, instructions));
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(assemblyName),
            CreateTypeTokenLiteral(metadataRegistration, declaringTypeSubjectId),
            FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", loadedFieldSubjectId)));
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBackedStringReturnDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(messagePrefixLiteral),
            Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            ToCppStringLiteral(messageSuffixLiteral),
            Encoding.UTF8.GetByteCount(messageSuffixLiteral));
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonFieldBackedStringReturnStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundFieldArgumentStringReturnStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal) ||
            method.Parameters.Count != 1)
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateFieldBackedStringInstanceMethodShape(method, instructions);
        }
        catch
        {
            return false;
        }

        if (instructions.Count != 7 ||
            !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[0]) != 1)
        {
            return false;
        }

        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var loadedFieldSubjectId = GetRequiredOperandString(instructions[2]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(method, instructions);
        var concatIcall = NormalizeStringConcatIcall(GetCapturedStateInstanceMessageConcatIcall(method, instructions));
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(assemblyName),
            CreateTypeTokenLiteral(metadataRegistration, declaringTypeSubjectId),
            FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", loadedFieldSubjectId)));
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldArgumentStringReturnDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(messageSuffixLiteral),
            Encoding.UTF8.GetByteCount(messageSuffixLiteral));
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonFieldArgumentStringReturnStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundFieldGetterStringReturnStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateFieldGetterShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var loadedFieldSubjectId = GetRequiredOperandString(instructions[1]);
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(assemblyName),
            CreateTypeTokenLiteral(metadataRegistration, declaringTypeSubjectId),
            FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", loadedFieldSubjectId)));
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldGetterStringReturnDescriptor(fieldBindingIndex);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonFieldGetterStringReturnStubTemplate(),
            model);
        return true;
    }

    private static string? BuildPreferredAssemblyDispatchSubjectId(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        return BuildPreferredAssemblyDispatchSubjectId(
            loweringPlan,
            loweringPlan.TranslationUnitMethodSubjectIds ?? []);
    }

    private static string? BuildPreferredAssemblyDispatchSubjectId(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        IReadOnlyList<string> methodSubjectIds)
    {
        if (!string.Equals(loweringPlan.RuntimeExecutionKind, "assembly-bound-native-reference-skeleton", StringComparison.Ordinal))
        {
            return null;
        }

        if (!string.IsNullOrWhiteSpace(loweringPlan.EntrySubjectId))
        {
            return loweringPlan.EntrySubjectId;
        }

        var proofEntrySubjectId = methodSubjectIds.FirstOrDefault(subjectId =>
            subjectId.Contains("ProofEntry::Run", StringComparison.Ordinal) ||
            subjectId.Contains("Entry::Run", StringComparison.Ordinal));
        if (!string.IsNullOrWhiteSpace(proofEntrySubjectId))
        {
            return proofEntrySubjectId;
        }
        return methodSubjectIds.FirstOrDefault(subjectId => subjectId.Contains("::Main:", StringComparison.Ordinal))
            ?? methodSubjectIds.FirstOrDefault(subjectId =>
                !subjectId.Contains("::.cctor:", StringComparison.Ordinal) &&
                !subjectId.Contains("::.ctor:", StringComparison.Ordinal) &&
                !subjectId.Contains("::<", StringComparison.Ordinal))
            ?? methodSubjectIds.FirstOrDefault();
    }

    private static bool IsConsoleWriteLineStringTarget(string? callee)
    {
        return string.Equals(callee, ConsoleWriteLineStringIcall, StringComparison.Ordinal) ||
               string.Equals(callee, ConsoleWriteLineStringMethodSubjectId, StringComparison.Ordinal);
    }

    private static string NormalizeStringConcatIcall(string? callee)
    {
        if (string.Equals(callee, StringConcatPairIcall, StringComparison.Ordinal) ||
            string.Equals(callee, StringConcatPairMethodSubjectId, StringComparison.Ordinal))
        {
            return StringConcatPairIcall;
        }

        if (string.Equals(callee, StringConcatTripleIcall, StringComparison.Ordinal) ||
            string.Equals(callee, StringConcatTripleMethodSubjectId, StringComparison.Ordinal))
        {
            return StringConcatTripleIcall;
        }

        return callee ?? StringConcatPairIcall;
    }

    private static void ValidateManagedLoweringPlan(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        RequireStringField(loweringPlan.PlanKind, nameof(loweringPlan.PlanKind));
        RequireStringField(loweringPlan.AssemblyName, nameof(loweringPlan.AssemblyName));
        RequireStringField(loweringPlan.EntrySubjectId, nameof(loweringPlan.EntrySubjectId));
        RequireStringField(loweringPlan.IncludeHeader, nameof(loweringPlan.IncludeHeader));
        RequireStringField(loweringPlan.NativeEntryFunctionName, nameof(loweringPlan.NativeEntryFunctionName));
        RequireStringField(loweringPlan.EntrySymbol, nameof(loweringPlan.EntrySymbol));
        RequireStringField(loweringPlan.EntryMethodToken, nameof(loweringPlan.EntryMethodToken));
        RequireStringField(loweringPlan.ConsoleWriteLineStringIcall, nameof(loweringPlan.ConsoleWriteLineStringIcall));

        switch (loweringPlan.PlanKind)
        {
            case ManagedAsyncAwaitIntMinimal:
            case ManagedThreadingThreadStaticMonitorMinimal:
            case MarshalingUtf8ExportMinimal:
                RequireStringField(loweringPlan.ExpectedOutput, nameof(loweringPlan.ExpectedOutput));
                RequireIntField(loweringPlan.ExpectedOutputByteCount, nameof(loweringPlan.ExpectedOutputByteCount));
                return;

            case "interfaceDispatchMessage":
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.DispatchStrategy, nameof(loweringPlan.DispatchStrategy));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.ConstructorLiteral, nameof(loweringPlan.ConstructorLiteral));
                RequireIntField(loweringPlan.ConstructorLiteralByteCount, nameof(loweringPlan.ConstructorLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireStringField(loweringPlan.MessageSuffixLiteral, nameof(loweringPlan.MessageSuffixLiteral));
                RequireIntField(loweringPlan.MessageSuffixLiteralByteCount, nameof(loweringPlan.MessageSuffixLiteralByteCount));
                return;

            case ManagedInterfaceDispatchMessageMinimal:
            case ManagedDispatchVirtualInstanceMessageMinimal:
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.DispatchStrategy, nameof(loweringPlan.DispatchStrategy));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.ConstructorLiteral, nameof(loweringPlan.ConstructorLiteral));
                RequireIntField(loweringPlan.ConstructorLiteralByteCount, nameof(loweringPlan.ConstructorLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireStringField(loweringPlan.MessageSuffixLiteral, nameof(loweringPlan.MessageSuffixLiteral));
                RequireIntField(loweringPlan.MessageSuffixLiteralByteCount, nameof(loweringPlan.MessageSuffixLiteralByteCount));
                return;

            case ManagedObjectCapturedStateInstanceMessageMinimal:
            case "constructorThenInstanceCall":
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.ConstructorLiteral, nameof(loweringPlan.ConstructorLiteral));
                RequireIntField(loweringPlan.ConstructorLiteralByteCount, nameof(loweringPlan.ConstructorLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireStringField(loweringPlan.MessageSuffixLiteral, nameof(loweringPlan.MessageSuffixLiteral));
                RequireIntField(loweringPlan.MessageSuffixLiteralByteCount, nameof(loweringPlan.MessageSuffixLiteralByteCount));
                return;

            case ManagedArraysCopyReferenceArrayMinimal:
            case "arrayCopyReferenceArray":
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.ConstructorLiteral, nameof(loweringPlan.ConstructorLiteral));
                RequireIntField(loweringPlan.ConstructorLiteralByteCount, nameof(loweringPlan.ConstructorLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireStringField(loweringPlan.MessageSuffixLiteral, nameof(loweringPlan.MessageSuffixLiteral));
                RequireIntField(loweringPlan.MessageSuffixLiteralByteCount, nameof(loweringPlan.MessageSuffixLiteralByteCount));
                RequireIntField(loweringPlan.SourceArrayLength, nameof(loweringPlan.SourceArrayLength));
                RequireIntField(loweringPlan.TargetArrayLength, nameof(loweringPlan.TargetArrayLength));
                RequireIntField(loweringPlan.SourceStoreIndex, nameof(loweringPlan.SourceStoreIndex));
                RequireIntField(loweringPlan.SourceArrayIndex, nameof(loweringPlan.SourceArrayIndex));
                RequireIntField(loweringPlan.TargetArrayIndex, nameof(loweringPlan.TargetArrayIndex));
                RequireIntField(loweringPlan.TargetReadIndex, nameof(loweringPlan.TargetReadIndex));
                RequireIntField(loweringPlan.CopyLength, nameof(loweringPlan.CopyLength));
                return;

            case ManagedArraysReverseReferenceArrayMinimal:
            case "arrayReverseReferenceArray":
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.ConstructorLiteral, nameof(loweringPlan.ConstructorLiteral));
                RequireIntField(loweringPlan.ConstructorLiteralByteCount, nameof(loweringPlan.ConstructorLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireStringField(loweringPlan.MessageSuffixLiteral, nameof(loweringPlan.MessageSuffixLiteral));
                RequireIntField(loweringPlan.MessageSuffixLiteralByteCount, nameof(loweringPlan.MessageSuffixLiteralByteCount));
                RequireIntField(loweringPlan.ArrayLength, nameof(loweringPlan.ArrayLength));
                RequireIntField(loweringPlan.ArrayStoreIndex, nameof(loweringPlan.ArrayStoreIndex));
                RequireIntField(loweringPlan.ReverseStartIndex, nameof(loweringPlan.ReverseStartIndex));
                RequireIntField(loweringPlan.ReverseLength, nameof(loweringPlan.ReverseLength));
                RequireIntField(loweringPlan.ArrayReadIndex, nameof(loweringPlan.ArrayReadIndex));
                return;

            case ManagedArraysClearReferenceArrayMinimal:
            case "arrayClearReferenceArray":
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.StoredLiteral, nameof(loweringPlan.StoredLiteral));
                RequireIntField(loweringPlan.StoredLiteralByteCount, nameof(loweringPlan.StoredLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireIntField(loweringPlan.ArrayLength, nameof(loweringPlan.ArrayLength));
                RequireIntField(loweringPlan.ArrayStoreIndex, nameof(loweringPlan.ArrayStoreIndex));
                RequireIntField(loweringPlan.ClearStartIndex, nameof(loweringPlan.ClearStartIndex));
                RequireIntField(loweringPlan.ClearLength, nameof(loweringPlan.ClearLength));
                RequireIntField(loweringPlan.ArrayReadIndex, nameof(loweringPlan.ArrayReadIndex));
                return;

            case ManagedGenericStaticForwarderCapturedGetterMinimal:
            case "staticCallCtorGetter":
                RequireStringField(loweringPlan.EchoMethodSymbol, nameof(loweringPlan.EchoMethodSymbol));
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.GetterSymbol, nameof(loweringPlan.GetterSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.EchoMethodToken, nameof(loweringPlan.EchoMethodToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.GetterMethodToken, nameof(loweringPlan.GetterMethodToken));
                RequireStringField(loweringPlan.EchoLiteral, nameof(loweringPlan.EchoLiteral));
                RequireIntField(loweringPlan.EchoLiteralByteCount, nameof(loweringPlan.EchoLiteralByteCount));
                return;

            case ManagedArraysBoxingReferenceArrayBoxedIntMinimal:
            case "arrayBoxingReferenceArray":
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.BoxedValueTypeToken, nameof(loweringPlan.BoxedValueTypeToken));
                RequireIntField(loweringPlan.BoxedInt32Value, nameof(loweringPlan.BoxedInt32Value));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.ConstructorLiteral, nameof(loweringPlan.ConstructorLiteral));
                RequireIntField(loweringPlan.ConstructorLiteralByteCount, nameof(loweringPlan.ConstructorLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireStringField(loweringPlan.MessageSuffixLiteral, nameof(loweringPlan.MessageSuffixLiteral));
                RequireIntField(loweringPlan.MessageSuffixLiteralByteCount, nameof(loweringPlan.MessageSuffixLiteralByteCount));
                return;

            case DelegateClosedTargetRelayMinimal:
            case "delegateClosedTargetRelayMinimal":
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.StaticMethodSymbol, nameof(loweringPlan.StaticMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.StaticMethodToken, nameof(loweringPlan.StaticMethodToken));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.ConstructorLiteral, nameof(loweringPlan.ConstructorLiteral));
                RequireIntField(loweringPlan.ConstructorLiteralByteCount, nameof(loweringPlan.ConstructorLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireStringField(loweringPlan.MessageSuffixLiteral, nameof(loweringPlan.MessageSuffixLiteral));
                RequireIntField(loweringPlan.MessageSuffixLiteralByteCount, nameof(loweringPlan.MessageSuffixLiteralByteCount));
                RequireStringField(loweringPlan.TrailingLiteral, nameof(loweringPlan.TrailingLiteral));
                RequireIntField(loweringPlan.TrailingLiteralByteCount, nameof(loweringPlan.TrailingLiteralByteCount));
                return;

            case NestedExceptionThrowCatchFinallyMinimal:
            case ExceptionThrowCatchFinallyMinimal:
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.ThrowMethodSymbol, nameof(loweringPlan.ThrowMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.ThrowMethodToken, nameof(loweringPlan.ThrowMethodToken));
                RequireStringField(loweringPlan.FinallyLiteral, nameof(loweringPlan.FinallyLiteral));
                RequireIntField(loweringPlan.FinallyLiteralByteCount, nameof(loweringPlan.FinallyLiteralByteCount));
                RequireStringField(loweringPlan.ExpectedOutput, nameof(loweringPlan.ExpectedOutput));
                RequireIntField(loweringPlan.ExpectedOutputByteCount, nameof(loweringPlan.ExpectedOutputByteCount));
                return;

            case ReflectionInteropClosureMinimal:
                RequireStringField(loweringPlan.ClosedTypeSubjectId, nameof(loweringPlan.ClosedTypeSubjectId));
                RequireStringField(loweringPlan.GenericTypeDefinitionSubjectId, nameof(loweringPlan.GenericTypeDefinitionSubjectId));
                RequireStringField(loweringPlan.FieldSubjectId, nameof(loweringPlan.FieldSubjectId));
                RequireStringField(loweringPlan.MethodSubjectId, nameof(loweringPlan.MethodSubjectId));
                RequireStringField(loweringPlan.ParameterSubjectId, nameof(loweringPlan.ParameterSubjectId));
                RequireStringField(loweringPlan.ClosedTypeToken, nameof(loweringPlan.ClosedTypeToken));
                RequireStringField(loweringPlan.GenericTypeDefinitionToken, nameof(loweringPlan.GenericTypeDefinitionToken));
                RequireStringField(loweringPlan.ClosedTypeNamespaceName, nameof(loweringPlan.ClosedTypeNamespaceName));
                RequireStringField(loweringPlan.ClosedTypeName, nameof(loweringPlan.ClosedTypeName));
                RequireStringField(loweringPlan.ClosedTypeDisplayName, nameof(loweringPlan.ClosedTypeDisplayName));
                RequireStringField(loweringPlan.FieldQueryName, nameof(loweringPlan.FieldQueryName));
                RequireStringField(loweringPlan.MethodQueryName, nameof(loweringPlan.MethodQueryName));
                RequireIntField(loweringPlan.MethodParameterCount, nameof(loweringPlan.MethodParameterCount));
                RequireIntField(loweringPlan.ParameterIndex, nameof(loweringPlan.ParameterIndex));
                RequireStringField(loweringPlan.ImportModuleName, nameof(loweringPlan.ImportModuleName));
                RequireStringField(loweringPlan.ImportEntryPointName, nameof(loweringPlan.ImportEntryPointName));
                RequireStringField(loweringPlan.ExpectedOutput, nameof(loweringPlan.ExpectedOutput));
                RequireIntField(loweringPlan.ExpectedOutputByteCount, nameof(loweringPlan.ExpectedOutputByteCount));
                return;

            case ReflectionClosedTypeQueryMinimal:
                RequireStringField(loweringPlan.ClosedTypeSubjectId, nameof(loweringPlan.ClosedTypeSubjectId));
                RequireStringField(loweringPlan.GenericTypeDefinitionSubjectId, nameof(loweringPlan.GenericTypeDefinitionSubjectId));
                RequireStringField(loweringPlan.FieldSubjectId, nameof(loweringPlan.FieldSubjectId));
                RequireStringField(loweringPlan.PropertySubjectId, nameof(loweringPlan.PropertySubjectId));
                RequireStringField(loweringPlan.MethodSubjectId, nameof(loweringPlan.MethodSubjectId));
                RequireStringField(loweringPlan.ParameterSubjectId, nameof(loweringPlan.ParameterSubjectId));
                RequireStringField(loweringPlan.ClosedTypeToken, nameof(loweringPlan.ClosedTypeToken));
                RequireStringField(loweringPlan.GenericTypeDefinitionToken, nameof(loweringPlan.GenericTypeDefinitionToken));
                RequireStringField(loweringPlan.ClosedTypeNamespaceName, nameof(loweringPlan.ClosedTypeNamespaceName));
                RequireStringField(loweringPlan.ClosedTypeName, nameof(loweringPlan.ClosedTypeName));
                RequireStringField(loweringPlan.ClosedTypeDisplayName, nameof(loweringPlan.ClosedTypeDisplayName));
                RequireStringField(loweringPlan.GenericTypeDefinitionName, nameof(loweringPlan.GenericTypeDefinitionName));
                RequireStringField(loweringPlan.FieldQueryName, nameof(loweringPlan.FieldQueryName));
                RequireStringField(loweringPlan.PropertyQueryName, nameof(loweringPlan.PropertyQueryName));
                RequireStringField(loweringPlan.MethodQueryName, nameof(loweringPlan.MethodQueryName));
                RequireIntField(loweringPlan.MethodParameterCount, nameof(loweringPlan.MethodParameterCount));
                RequireIntField(loweringPlan.ParameterIndex, nameof(loweringPlan.ParameterIndex));
                RequireStringField(loweringPlan.ExpectedOutput, nameof(loweringPlan.ExpectedOutput));
                RequireIntField(loweringPlan.ExpectedOutputByteCount, nameof(loweringPlan.ExpectedOutputByteCount));
                return;

            case InteropPInvokeDirectCallMinimal:
                RequireStringField(loweringPlan.ImportMethodSymbol, nameof(loweringPlan.ImportMethodSymbol));
                RequireStringField(loweringPlan.ImportModuleName, nameof(loweringPlan.ImportModuleName));
                RequireStringField(loweringPlan.ImportEntryPointName, nameof(loweringPlan.ImportEntryPointName));
                RequireIntField(loweringPlan.ImportArgument0, nameof(loweringPlan.ImportArgument0));
                RequireIntField(loweringPlan.ImportArgument1, nameof(loweringPlan.ImportArgument1));
                RequireIntField(loweringPlan.ImportArgument2, nameof(loweringPlan.ImportArgument2));
                RequireStringField(loweringPlan.OutputPrefix, nameof(loweringPlan.OutputPrefix));
                return;

            case EngineLogWriteMinimal:
            case EngineObjectHandleRoundtripMinimal:
            case EngineLifecycleCallbackMinimal:
            case EngineHostProofMinimal:
                RequireStringField(loweringPlan.ExpectedOutput, nameof(loweringPlan.ExpectedOutput));
                RequireIntField(loweringPlan.ExpectedOutputByteCount, nameof(loweringPlan.ExpectedOutputByteCount));
                if (loweringPlan.EngineBindings is null)
                {
                    throw new InvalidOperationException("managed lowering plan is missing required field 'EngineBindings'");
                }

                if (loweringPlan.HostBindings is null)
                {
                    throw new InvalidOperationException("managed lowering plan is missing required field 'HostBindings'");
                }

                return;

            default:
                throw new InvalidOperationException(
                    $"unsupported managed lowering plan kind '{loweringPlan.PlanKind}'");
        }
    }

    private static void ValidateAssemblyFullClosureAuditPlan(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        AssemblyFullClosureAuditEmitter.ValidatePlan(
            loweringPlan.PlanKind,
            loweringPlan.AssemblyName,
            loweringPlan.TranslationUnitMode,
            loweringPlan.TranslationUnitMethodSubjectIds,
            loweringPlan.TranslationUnitPageSize,
            loweringPlan.TranslationUnitPageCount,
            loweringPlan.TranslationUnitPages,
            AuditTranslationUnitPageSize);
    }

    private static void ValidateAssemblyFullClosureRuntimeSkeletonPlan(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        RequireStringField(loweringPlan.PlanKind, nameof(loweringPlan.PlanKind));
        RequireStringField(loweringPlan.AssemblyName, nameof(loweringPlan.AssemblyName));
        RequireStringField(loweringPlan.NativeEntryFunctionName, nameof(loweringPlan.NativeEntryFunctionName));

        if (!string.Equals(loweringPlan.PlanKind, "assembly-full-closure-runtime-skeleton", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure runtime skeleton emitter expected plan kind 'assembly-full-closure-runtime-skeleton', but found '{loweringPlan.PlanKind}'");
        }

        if (!string.Equals(loweringPlan.TranslationUnitMode, "runtime-skeleton", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure runtime skeleton emitter expected translation-unit mode 'runtime-skeleton', but found '{loweringPlan.TranslationUnitMode ?? "<null>"}'");
        }

        if (!string.Equals(loweringPlan.AuditStatus, "runtime-skeleton", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure runtime skeleton emitter expected audit status 'runtime-skeleton', but found '{loweringPlan.AuditStatus ?? "<null>"}'");
        }

        if (loweringPlan.TranslationUnitMethodSubjectIds is null ||
            loweringPlan.TranslationUnitMethodSubjectIds.Count == 0)
        {
            throw new InvalidOperationException(
                "assembly-full-closure runtime skeleton emitter requires at least one translation-unit method subject id");
        }

        if (loweringPlan.TranslationUnitPageSize != AuditTranslationUnitPageSize)
        {
            throw new InvalidOperationException(
                $"assembly-full-closure runtime skeleton emitter expected translation-unit page size '{AuditTranslationUnitPageSize}', but found '{loweringPlan.TranslationUnitPageSize?.ToString() ?? "<null>"}'");
        }

        if (loweringPlan.TranslationUnitPageCount != GetAuditPageCount(loweringPlan.TranslationUnitMethodSubjectIds.Count))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure runtime skeleton emitter expected translation-unit page count '{GetAuditPageCount(loweringPlan.TranslationUnitMethodSubjectIds.Count)}', but found '{loweringPlan.TranslationUnitPageCount?.ToString() ?? "<null>"}'");
        }

        if (loweringPlan.TranslationUnitPages is null ||
            loweringPlan.TranslationUnitPages.Count != loweringPlan.TranslationUnitPageCount)
        {
            throw new InvalidOperationException(
                "assembly-full-closure runtime skeleton emitter requires translation-unit page metadata matching the declared page count");
        }
    }

    private static int GetAuditPageCount(int totalMethodCount)
    {
        if (totalMethodCount <= 0)
        {
            return 0;
        }

        return (totalMethodCount + AuditTranslationUnitPageSize - 1) / AuditTranslationUnitPageSize;
    }

    private static void RequireStringField(string? value, string fieldName)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            throw new InvalidOperationException(
                $"managed lowering plan is missing required field '{fieldName}'");
        }
    }

    private static void RequireIntField<T>(T? value, string fieldName)
        where T : struct
    {
        if (!value.HasValue)
        {
            throw new InvalidOperationException(
                $"managed lowering plan is missing required field '{fieldName}'");
        }
    }

    private static NativeReferenceLoweringPlanArtifact BuildLegacyLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers)
    {
        var entryPointRegistration = GetRequiredRegistration(methodPointers, entryPointSubjectId);
        var entryPointMethod = GetRequiredMethod(methods, entryPointSubjectId);
        RequireMethodContract(entryPointMethod, "static-method", "has-canonical-body");
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);

        if (TryBuildLegacyPInvokeDirectCallLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                methods,
                entryPointRegistration,
                entryPointMethod,
                entryPointInstructions,
                out var pinvokeDllImportMinimalPlan))
        {
            return pinvokeDllImportMinimalPlan;
        }

        if (TryBuildLegacyReflectionClosedTypeQueryLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                entryPointRegistration,
                out var reflectionQueryMinimalPlan))
        {
            return reflectionQueryMinimalPlan;
        }

        if (MatchesLegacyStaticForwarderCtorGetterEntryPointShape(entryPointInstructions))
        {
            return CreateLegacyStaticForwarderCapturedGetterLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                methods,
                entryPointRegistration,
                entryPointMethod,
                entryPointInstructions);
        }

        return CreateLegacyCapturedStateInstanceMessageLoweringPlan(
            assemblyName,
            entryPointSubjectId,
            metadataRegistration,
            methodPointers,
            methods,
            entryPointRegistration,
            entryPointMethod,
            entryPointInstructions);
    }

    private static NativeReferenceLoweringPlanArtifact CreateLegacyCapturedStateInstanceMessageLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions)
    {
        ValidateLegacyCapturedStateInstanceMessageEntryPointShape(entryPointMethod, entryPointInstructions);

        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
        var instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);

        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorRegistration.SubjectId);
        var instanceMethodTypeSubjectId = GetDeclaringTypeSubjectId(instanceMethodRegistration.SubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceMethodTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects constructor '{constructorRegistration.SubjectId}' and instance method '{instanceMethodRegistration.SubjectId}' to share a declaring type");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter field access mismatch: constructor writes '{storedFieldSubjectId}', instance method reads '{loadedFieldSubjectId}'");
        }

        var constructorStringLiteral = GetRequiredOperandString(entryPointInstructions[0]);
        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions);

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "constructorThenInstanceCall",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorStringLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorStringLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };
    }

    private static NativeReferenceLoweringPlanArtifact CreateLegacyStaticForwarderCapturedGetterLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions)
    {
        ValidateStaticCallCtorGetterEntryPointShape(entryPointMethod, entryPointInstructions);

        var echoMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        var getterSubjectId = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[4], entryPointMethod.SubjectId, 4);

        var echoMethodRegistration = GetRequiredRegistration(methodPointers, echoMethodSubjectId);
        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var getterRegistration = GetRequiredRegistration(methodPointers, getterSubjectId);
        var echoMethod = GetRequiredMethod(methods, echoMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var getterMethod = GetRequiredMethod(methods, getterSubjectId);

        var echoInstructions = GetSingleBlockInstructions(echoMethod);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var getterInstructions = GetSingleBlockInstructions(getterMethod);

        ValidateSingleArgumentForwarderShape(echoMethod, echoInstructions);
        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldGetterShape(getterMethod, getterInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorRegistration.SubjectId);
        var getterTypeSubjectId = GetDeclaringTypeSubjectId(getterRegistration.SubjectId);
        if (!string.Equals(constructorTypeSubjectId, getterTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects constructor '{constructorRegistration.SubjectId}' and getter '{getterRegistration.SubjectId}' to share a declaring type");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(getterInstructions[1]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter field access mismatch: constructor writes '{storedFieldSubjectId}', getter reads '{loadedFieldSubjectId}'");
        }

        var echoLiteral = GetRequiredOperandString(entryPointInstructions[0]);

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "staticCallCtorGetter",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            EchoMethodSymbol = echoMethodRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            GetterSymbol = getterRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            EchoMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", echoMethodRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            GetterMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", getterRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            EchoLiteral = ToCppStringLiteral(echoLiteral),
            EchoLiteralByteCount = Encoding.UTF8.GetByteCount(echoLiteral),
        };
    }

    private static bool TryBuildLegacyPInvokeDirectCallLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!IsPInvokeDllImportMinimalEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var importMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
        var importMethodRegistration = GetRequiredRegistration(methodPointers, importMethodSubjectId);
        var importMethod = GetRequiredMethod(methods, importMethodSubjectId);
        if (!HasCapability(entryPointMethod, "requires-imported-call") ||
            !HasCapability(importMethod, "requires-imported-call") ||
            !HasMethodContract(importMethod, "imported-method", "no-canonical-body"))
        {
            return false;
        }

        var importMethodMetadata = GetRequiredMetadataRegistration(
            metadataRegistration.Registrations,
            "method",
            importMethodSubjectId,
            "pinvoke import method");

        if (importMethodMetadata.IsImported != true)
        {
            return false;
        }

        if (string.IsNullOrWhiteSpace(importMethodMetadata.ImportModuleName) ||
            string.IsNullOrWhiteSpace(importMethodMetadata.ImportEntryPointName))
        {
            return false;
        }

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = InteropPInvokeDirectCallMinimal,
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ImportMethodSubjectId = importMethodSubjectId,
            ImportMethodSymbol = importMethodRegistration.Symbol,
            ImportModuleName = importMethodMetadata.ImportModuleName,
            ImportEntryPointName = importMethodMetadata.ImportEntryPointName,
            ImportArgument0 = GetRequiredOperandInt(entryPointInstructions[0]),
            ImportArgument1 = GetRequiredOperandInt(entryPointInstructions[1]),
            ImportArgument2 = GetRequiredOperandInt(entryPointInstructions[2]),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            OutputPrefix = "pinvoke-ok|",
        };

        return true;
    }

    private static bool TryBuildLegacyReflectionClosedTypeQueryLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationEntry entryPointRegistration,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        var registrations = metadataRegistration.Registrations;
        var closedTypeRegistration = GetMetadataRegistrationsForKind(registrations, "type").FirstOrDefault(registration =>
            !string.IsNullOrWhiteSpace(registration.DefinitionSubjectId) &&
            !string.Equals(registration.SubjectId, registration.DefinitionSubjectId, StringComparison.Ordinal));
        if (closedTypeRegistration is null)
        {
            return false;
        }

        var propertyRegistration = GetMetadataRegistrationsForKind(registrations, "property").FirstOrDefault(registration =>
            string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
            !IsCompilerGeneratedMemberName(registration.Name));
        var queryMethodRegistration = GetMetadataRegistrationsForKind(registrations, "method").FirstOrDefault(registration =>
            string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
            (registration.ParameterCount ?? 0) > 0 &&
            !IsSpecialMethodName(registration.Name));
        if (propertyRegistration is null || queryMethodRegistration is null)
        {
            return false;
        }

        var genericTypeDefinitionRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            closedTypeRegistration.DefinitionSubjectId!,
            "reflection query generic type definition");
        var fieldRegistration = GetRequiredMetadataRegistration(
            registrations,
            "field",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                !IsCompilerGeneratedMemberName(registration.Name),
            $"reflection query field on '{closedTypeRegistration.SubjectId}'");
        var parameterRegistration = GetRequiredMetadataRegistration(
            registrations,
            "parameter",
            registration =>
                string.Equals(registration.DeclaringMethodSubjectId, queryMethodRegistration.SubjectId, StringComparison.Ordinal) &&
                registration.ParameterIndex == 0,
            $"reflection query first parameter for '{queryMethodRegistration.SubjectId}'");

        var closedTypeDisplayName = GetRequiredRegistrationDisplayName(closedTypeRegistration);
        var genericTypeDefinitionName = GetRequiredRegistrationName(genericTypeDefinitionRegistration);
        var fieldQueryName = GetRequiredRegistrationName(fieldRegistration);
        var propertyQueryName = GetRequiredRegistrationName(propertyRegistration);
        var methodQueryName = GetRequiredRegistrationName(queryMethodRegistration);
        var parameterName = GetRequiredRegistrationName(parameterRegistration);
        var methodParameterCount = queryMethodRegistration.ParameterCount
            ?? throw new InvalidOperationException(
                $"reflection query method '{queryMethodRegistration.SubjectId}' is missing parameterCount metadata");
        var parameterIndex = parameterRegistration.ParameterIndex
            ?? throw new InvalidOperationException(
                $"reflection query parameter '{parameterRegistration.SubjectId}' is missing parameterIndex metadata");
        var expectedOutput =
            $"reflection-ok|{closedTypeDisplayName}|{fieldQueryName}|{propertyQueryName}|{methodQueryName}|{parameterName}|{genericTypeDefinitionName}";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "reflectionQueryMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "reflection_query_model.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", fieldRegistration.SubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", queryMethodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ClosedTypeSubjectId = closedTypeRegistration.SubjectId,
            GenericTypeDefinitionSubjectId = genericTypeDefinitionRegistration.SubjectId,
            FieldSubjectId = fieldRegistration.SubjectId,
            PropertySubjectId = propertyRegistration.SubjectId,
            MethodSubjectId = queryMethodRegistration.SubjectId,
            ParameterSubjectId = parameterRegistration.SubjectId,
            ClosedTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            GenericTypeDefinitionToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", genericTypeDefinitionRegistration.SubjectId)),
            ClosedTypeNamespaceName = closedTypeRegistration.NamespaceName ?? string.Empty,
            ClosedTypeName = GetRequiredRegistrationName(closedTypeRegistration),
            ClosedTypeDisplayName = closedTypeDisplayName,
            GenericTypeDefinitionName = genericTypeDefinitionName,
            FieldQueryName = fieldQueryName,
            PropertyQueryName = propertyQueryName,
            MethodQueryName = methodQueryName,
            MethodParameterCount = methodParameterCount,
            ParameterIndex = parameterIndex,
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }

    private static Template GetTemplateForPlan(string planKind)
    {
        return NativeReferenceProofCatalog.GetTemplateForPlan(planKind);
    }

    private static ScriptObject CreateTemplateModel(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        var model = new ScriptObject
        {
            ["include_header"] = loweringPlan.IncludeHeader,
            ["plan_kind"] = loweringPlan.PlanKind,
            ["native_reference_function_name"] = loweringPlan.NativeEntryFunctionName,
            ["assembly_name"] = loweringPlan.AssemblyName,
            ["entry_symbol"] = loweringPlan.EntrySymbol,
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["captured_field_token"] = loweringPlan.CapturedFieldToken,
            ["entry_method_token"] = loweringPlan.EntryMethodToken,
            ["console_write_line_string_icall"] = loweringPlan.ConsoleWriteLineStringIcall,
        };

        AddIfNotNull(model, "constructor_symbol", loweringPlan.ConstructorSymbol);
        AddIfNotNull(model, "instance_method_symbol", loweringPlan.InstanceMethodSymbol);
        AddIfNotNull(model, "static_method_symbol", loweringPlan.StaticMethodSymbol);
        AddIfNotNull(model, "throw_method_symbol", loweringPlan.ThrowMethodSymbol);
        AddIfNotNull(model, "echo_method_symbol", loweringPlan.EchoMethodSymbol);
        AddIfNotNull(model, "getter_symbol", loweringPlan.GetterSymbol);
        AddIfNotNull(model, "import_method_subject_id", loweringPlan.ImportMethodSubjectId);
        AddIfNotNull(model, "import_method_symbol", loweringPlan.ImportMethodSymbol);
        AddIfNotNull(model, "import_module_name", loweringPlan.ImportModuleName);
        AddIfNotNull(model, "import_entry_point_name", loweringPlan.ImportEntryPointName);
        AddIfNotNull(model, "import_argument0", loweringPlan.ImportArgument0);
        AddIfNotNull(model, "import_argument1", loweringPlan.ImportArgument1);
        AddIfNotNull(model, "import_argument2", loweringPlan.ImportArgument2);
        AddIfNotNull(model, "constructor_method_token", loweringPlan.ConstructorMethodToken);
        AddIfNotNull(model, "instance_method_token", loweringPlan.InstanceMethodToken);
        AddIfNotNull(model, "static_method_token", loweringPlan.StaticMethodToken);
        AddIfNotNull(model, "throw_method_token", loweringPlan.ThrowMethodToken);
        AddIfNotNull(model, "dispatch_strategy", loweringPlan.DispatchStrategy);
        AddIfNotNull(model, "echo_method_token", loweringPlan.EchoMethodToken);
        AddIfNotNull(model, "getter_method_token", loweringPlan.GetterMethodToken);
        AddIfNotNull(model, "string_concat_pair_icall", loweringPlan.StringConcatPairIcall);
        AddIfNotNull(model, "constructor_literal", loweringPlan.ConstructorLiteral);
        AddIfNotNull(model, "constructor_literal_byte_count", loweringPlan.ConstructorLiteralByteCount);
        AddIfNotNull(model, "stored_literal", loweringPlan.StoredLiteral);
        AddIfNotNull(model, "stored_literal_byte_count", loweringPlan.StoredLiteralByteCount);
        AddIfNotNull(model, "message_prefix_literal", loweringPlan.MessagePrefixLiteral);
        AddIfNotNull(model, "message_prefix_literal_byte_count", loweringPlan.MessagePrefixLiteralByteCount);
        AddIfNotNull(model, "message_suffix_literal", loweringPlan.MessageSuffixLiteral);
        AddIfNotNull(model, "message_suffix_literal_byte_count", loweringPlan.MessageSuffixLiteralByteCount);
        AddIfNotNull(model, "trailing_literal", loweringPlan.TrailingLiteral);
        AddIfNotNull(model, "trailing_literal_byte_count", loweringPlan.TrailingLiteralByteCount);
        AddIfNotNull(model, "echo_literal", loweringPlan.EchoLiteral);
        AddIfNotNull(model, "echo_literal_byte_count", loweringPlan.EchoLiteralByteCount);
        AddIfNotNull(model, "finally_literal", loweringPlan.FinallyLiteral);
        AddIfNotNull(model, "finally_literal_byte_count", loweringPlan.FinallyLiteralByteCount);
        AddIfNotNull(model, "boxed_value_type_token", loweringPlan.BoxedValueTypeToken);
        AddIfNotNull(model, "boxed_int32_value", loweringPlan.BoxedInt32Value);
        AddIfNotNull(model, "source_array_length", loweringPlan.SourceArrayLength);
        AddIfNotNull(model, "target_array_length", loweringPlan.TargetArrayLength);
        AddIfNotNull(model, "source_store_index", loweringPlan.SourceStoreIndex);
        AddIfNotNull(model, "source_array_index", loweringPlan.SourceArrayIndex);
        AddIfNotNull(model, "target_array_index", loweringPlan.TargetArrayIndex);
        AddIfNotNull(model, "target_read_index", loweringPlan.TargetReadIndex);
        AddIfNotNull(model, "copy_length", loweringPlan.CopyLength);
        AddIfNotNull(model, "array_length", loweringPlan.ArrayLength);
        AddIfNotNull(model, "array_store_index", loweringPlan.ArrayStoreIndex);
        AddIfNotNull(model, "clear_start_index", loweringPlan.ClearStartIndex);
        AddIfNotNull(model, "clear_length", loweringPlan.ClearLength);
        AddIfNotNull(model, "reverse_start_index", loweringPlan.ReverseStartIndex);
        AddIfNotNull(model, "reverse_length", loweringPlan.ReverseLength);
        AddIfNotNull(model, "array_read_index", loweringPlan.ArrayReadIndex);
        AddIfNotNull(model, "closed_type_subject_id", loweringPlan.ClosedTypeSubjectId);
        AddIfNotNull(model, "generic_type_definition_subject_id", loweringPlan.GenericTypeDefinitionSubjectId);
        AddIfNotNull(model, "field_subject_id", loweringPlan.FieldSubjectId);
        AddIfNotNull(model, "property_subject_id", loweringPlan.PropertySubjectId);
        AddIfNotNull(model, "method_subject_id", loweringPlan.MethodSubjectId);
        AddIfNotNull(model, "parameter_subject_id", loweringPlan.ParameterSubjectId);
        AddIfNotNull(model, "closed_type_token", loweringPlan.ClosedTypeToken);
        AddIfNotNull(model, "generic_type_definition_token", loweringPlan.GenericTypeDefinitionToken);
        AddIfNotNull(model, "closed_type_namespace_name", loweringPlan.ClosedTypeNamespaceName);
        AddIfNotNull(model, "closed_type_name", loweringPlan.ClosedTypeName);
        AddIfNotNull(model, "closed_type_display_name", loweringPlan.ClosedTypeDisplayName);
        AddIfNotNull(model, "generic_type_definition_name", loweringPlan.GenericTypeDefinitionName);
        AddIfNotNull(model, "field_query_name", loweringPlan.FieldQueryName);
        AddIfNotNull(model, "property_query_name", loweringPlan.PropertyQueryName);
        AddIfNotNull(model, "method_query_name", loweringPlan.MethodQueryName);
        AddIfNotNull(model, "method_parameter_count", loweringPlan.MethodParameterCount);
        AddIfNotNull(model, "parameter_index", loweringPlan.ParameterIndex);
        AddIfNotNull(model, "output_prefix", loweringPlan.OutputPrefix);
        AddIfNotNull(model, "expected_output", loweringPlan.ExpectedOutput);
        AddIfNotNull(model, "expected_output_byte_count", loweringPlan.ExpectedOutputByteCount);

        AddStringLiteralIfNotNull(model, "closed_type_subject_id_literal", loweringPlan.ClosedTypeSubjectId);
        AddStringLiteralIfNotNull(model, "generic_type_definition_subject_id_literal", loweringPlan.GenericTypeDefinitionSubjectId);
        AddStringLiteralIfNotNull(model, "field_subject_id_literal", loweringPlan.FieldSubjectId);
        AddStringLiteralIfNotNull(model, "property_subject_id_literal", loweringPlan.PropertySubjectId);
        AddStringLiteralIfNotNull(model, "method_subject_id_literal", loweringPlan.MethodSubjectId);
        AddStringLiteralIfNotNull(model, "parameter_subject_id_literal", loweringPlan.ParameterSubjectId);
        AddStringLiteralIfNotNull(model, "closed_type_namespace_name_literal", loweringPlan.ClosedTypeNamespaceName);
        AddStringLiteralIfNotNull(model, "closed_type_name_literal", loweringPlan.ClosedTypeName);
        AddStringLiteralIfNotNull(model, "closed_type_display_name_literal", loweringPlan.ClosedTypeDisplayName);
        AddStringLiteralIfNotNull(model, "generic_type_definition_name_literal", loweringPlan.GenericTypeDefinitionName);
        AddStringLiteralIfNotNull(model, "field_query_name_literal", loweringPlan.FieldQueryName);
        AddStringLiteralIfNotNull(model, "property_query_name_literal", loweringPlan.PropertyQueryName);
        AddStringLiteralIfNotNull(model, "method_query_name_literal", loweringPlan.MethodQueryName);
        AddStringLiteralIfNotNull(model, "import_module_name_literal", loweringPlan.ImportModuleName);
        AddStringLiteralIfNotNull(model, "import_entry_point_name_literal", loweringPlan.ImportEntryPointName);
        AddStringLiteralIfNotNull(model, "output_prefix_literal", loweringPlan.OutputPrefix);
        AddStringLiteralIfNotNull(model, "expected_output_literal", loweringPlan.ExpectedOutput);

        return model;
    }

    private static void AddIfNotNull<T>(ScriptObject model, string key, T? value)
    {
        if (value is not null)
        {
            model[key] = value;
        }
    }

    private static void AddStringLiteralIfNotNull(ScriptObject model, string key, string? value)
    {
        if (value is not null)
        {
            model[key] = ToCppStringLiteral(value);
        }
    }

    private static IReadOnlyList<TypedIlInstructionArtifact> GetSingleBlockInstructions(TypedIlMethodArtifact method)
    {
        if (method.Blocks.Count != 1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects a single block in '{method.SubjectId}', but found {method.Blocks.Count}");
        }

        return method.Blocks[0].Instructions;
    }

    private static bool MatchesLegacyStaticForwarderCtorGetterEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 7 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsPInvokeDllImportMinimalEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 5 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal);
    }

    private static void ValidateLegacyCapturedStateInstanceMessageEntryPointShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        RequireInstructionCount(method, instructions, 6);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "newobj", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "call", "callvirt", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "ldc.i4", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ret", method.SubjectId, 5);
        RequireInstructionCallee(instructions[3], ConsoleWriteLineStringIcall, ConsoleWriteLineStringMethodSubjectId, method.SubjectId, 3);

        if (GetRequiredOperandInt(instructions[4]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to return ldc.i4 0 before ret");
        }
    }

    private static void ValidateStaticCallCtorGetterEntryPointShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        RequireInstructionCount(method, instructions, 7);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "call", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "newobj", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "call", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ldc.i4", method.SubjectId, 5);
        RequireInstructionOp(instructions[6], "ret", method.SubjectId, 6);
        RequireInstructionCallee(instructions[4], ConsoleWriteLineStringIcall, ConsoleWriteLineStringMethodSubjectId, method.SubjectId, 4);

        if (GetRequiredOperandInt(instructions[5]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to return ldc.i4 0 before ret");
        }
    }

    private static void ValidateConstructorShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "constructor", "has-canonical-body");
        RequireInstructionCount(method, instructions, 6);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "call", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldarg", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "ldarg", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "stfld", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ret", method.SubjectId, 5);

        if (GetRequiredOperandInt(instructions[0]) != 0 ||
            GetRequiredOperandInt(instructions[2]) != 0 ||
            GetRequiredOperandInt(instructions[3]) != 1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to use ldarg 0/0/1 pattern");
        }

        _ = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
    }

    private static void ValidateFieldBackedStringInstanceMethodShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-method", "has-canonical-body");
        if (instructions.Count == 7)
        {
            if (string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal))
            {
                RequireInstructionOp(instructions[1], "ldarg", method.SubjectId, 1);
                if (GetRequiredOperandInt(instructions[1]) != 0)
                {
                    throw new InvalidOperationException(
                        $"native-reference emitter expects '{method.SubjectId}' literal-prefix shape to load ldarg 0 after the prefix literal");
                }
            }
            else
            {
                RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
                RequireInstructionOp(instructions[1], "ldarg", method.SubjectId, 1);
                if (GetRequiredOperandInt(instructions[0]) != 1 ||
                    GetRequiredOperandInt(instructions[1]) != 0)
                {
                    throw new InvalidOperationException(
                        $"native-reference emitter expects '{method.SubjectId}' parameter-prefix shape to use ldarg 1 / ldarg 0 before the captured field load");
                }
            }

            RequireInstructionOp(instructions[2], "ldfld", method.SubjectId, 2);
            RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
            RequireInstructionOp(instructions[4], "ldstr", method.SubjectId, 4);
            RequireInstructionOp(instructions[5], "call", method.SubjectId, 5);
            RequireInstructionOp(instructions[6], "ret", method.SubjectId, 6);

            RequireInstructionCallee(
                instructions[3],
                StringConcatPairIcall,
                StringConcatPairMethodSubjectId,
                method.SubjectId,
                3);
            RequireInstructionCallee(
                instructions[5],
                StringConcatPairIcall,
                StringConcatPairMethodSubjectId,
                method.SubjectId,
                5);
            return;
        }

        if (instructions.Count == 6)
        {
            RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
            RequireInstructionOp(instructions[1], "ldarg", method.SubjectId, 1);
            RequireInstructionOp(instructions[2], "ldfld", method.SubjectId, 2);
            RequireInstructionOp(instructions[3], "ldstr", method.SubjectId, 3);
            RequireInstructionOp(instructions[4], "call", method.SubjectId, 4);
            RequireInstructionOp(instructions[5], "ret", method.SubjectId, 5);

            if (GetRequiredOperandInt(instructions[1]) != 0)
            {
                throw new InvalidOperationException(
                    $"native-reference emitter expects '{method.SubjectId}' to start with ldarg 0");
            }

            RequireInstructionCallee(
                instructions[4],
                StringConcatTripleIcall,
                "System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)",
                method.SubjectId,
                4);
            return;
        }

        throw new InvalidOperationException(
            $"native-reference emitter expects '{method.SubjectId}' field-backed string instance method to use 6 or 7 canonical instructions");
    }

    private static string GetCapturedStateInstanceMessageSuffixLiteral(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count switch
        {
            7 => GetRequiredOperandString(instructions[4]),
            6 => GetRequiredOperandString(instructions[3]),
            _ => throw new InvalidOperationException(
                $"native-reference emitter cannot extract suffix literal from '{method.SubjectId}'"),
        };
    }

    private static string GetCapturedStateInstanceMessageConcatIcall(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count switch
        {
            7 => GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3),
            6 => GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4),
            _ => throw new InvalidOperationException(
                $"native-reference emitter cannot extract concat icall from '{method.SubjectId}'"),
        };
    }

    private static bool IsArrayBoxingReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 16 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "box", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayCopyReferenceArrayLengthOnlyEntryPointShape(instructions) ||
               IsArrayCopyToReferenceArrayTargetOffsetEntryPointShape(instructions) ||
               IsArrayCopyReferenceArrayIndexedEntryPointShape(instructions);
    }

    private static bool IsArrayReverseReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayReverseRangeReferenceArrayEntryPointShape(instructions) ||
               IsArrayReverseWholeReferenceArrayEntryPointShape(instructions);
    }

    private static bool IsArrayReverseRangeReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 17 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayReverseWholeReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 15 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayClearReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayClearRangeReferenceArrayEntryPointShape(instructions) ||
               IsArrayClearWholeReferenceArrayEntryPointShape(instructions);
    }

    private static bool IsArrayClearRangeReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 19 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayClearWholeReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 17 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayLengthOnlyEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayCopyReferenceArrayLengthOnlyInt32EntryPointShape(instructions) ||
               IsArrayCopyReferenceArrayLengthOnlyInt64EntryPointShape(instructions);
    }

    private static bool IsArrayCopyReferenceArrayLengthOnlyInt32EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 20 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayLengthOnlyInt64EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayIndexedEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayCopyReferenceArrayIndexedInt32EntryPointShape(instructions) ||
               IsArrayCopyReferenceArrayIndexedInt64EntryPointShape(instructions);
    }

    private static bool IsArrayCopyReferenceArrayIndexedInt32EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 22 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[21].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayIndexedInt64EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 25 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[21].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[22].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[23].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[24].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyToReferenceArrayTargetOffsetEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape(instructions) ||
               IsArrayCopyToReferenceArrayTargetOffsetInt64EntryPointShape(instructions);
    }

    private static bool IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 20 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyToReferenceArrayTargetOffsetInt64EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsSupportedArrayCopyMethodSubjectId(string subjectId)
    {
        return (subjectId.Contains("/System.Array::Copy:", StringComparison.Ordinal) &&
                (subjectId.EndsWith("(System.Array,System.Array,System.Int32)", StringComparison.Ordinal) ||
                 subjectId.EndsWith("(System.Array,System.Array,System.Int64)", StringComparison.Ordinal) ||
                 subjectId.EndsWith("(System.Array,System.Int32,System.Array,System.Int32,System.Int32)", StringComparison.Ordinal) ||
                 subjectId.EndsWith("(System.Array,System.Int64,System.Array,System.Int64,System.Int64)", StringComparison.Ordinal))) ||
               (subjectId.Contains("/System.Array::ConstrainedCopy:", StringComparison.Ordinal) &&
                subjectId.EndsWith("(System.Array,System.Int32,System.Array,System.Int32,System.Int32)", StringComparison.Ordinal)) ||
               (subjectId.Contains("/System.Array::CopyTo:", StringComparison.Ordinal) &&
                (subjectId.EndsWith("(System.Array,System.Int32)", StringComparison.Ordinal) ||
                 subjectId.EndsWith("(System.Array,System.Int64)", StringComparison.Ordinal)));
    }

    private static bool IsSupportedArrayReverseMethodSubjectId(string subjectId)
    {
        return subjectId.Contains("/System.Array::Reverse", StringComparison.Ordinal);
    }

    private static bool IsSupportedArrayClearMethodSubjectId(string subjectId)
    {
        return subjectId.Contains("/System.Array::Clear:", StringComparison.Ordinal) &&
               (subjectId.EndsWith("(System.Array,System.Int32,System.Int32)", StringComparison.Ordinal) ||
                subjectId.EndsWith("(System.Array)", StringComparison.Ordinal));
    }

    private static bool IsConstructorThenInstanceCallEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 6 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ret", StringComparison.Ordinal);
    }

    private static string? TryResolveDevirtualizedCallTarget(
        IReadOnlyList<TypedIlMethodArtifact> methods,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        int instructionIndex,
        string declaredTargetSubjectId)
    {
        if (instructionIndex <= 0)
        {
            return null;
        }

        var precedingInstruction = instructions[instructionIndex - 1];
        if (!string.Equals(precedingInstruction.Op, "newobj", StringComparison.Ordinal) ||
            string.IsNullOrWhiteSpace(precedingInstruction.Callee))
        {
            return null;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(precedingInstruction.Callee);
        var declaredMethod = GetRequiredMethod(methods, declaredTargetSubjectId);
        var exactMatch = methods.FirstOrDefault(candidate =>
            string.Equals(GetDeclaringTypeSubjectId(candidate.SubjectId), constructorTypeSubjectId, StringComparison.Ordinal) &&
            string.Equals(GetMethodName(candidate.SubjectId), GetMethodName(declaredMethod.SubjectId), StringComparison.Ordinal) &&
            string.Equals(candidate.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) &&
            candidate.Parameters.Select(parameter => parameter.Type).SequenceEqual(
                declaredMethod.Parameters.Select(parameter => parameter.Type),
                StringComparer.Ordinal))
            ?.SubjectId;
        if (!string.IsNullOrWhiteSpace(exactMatch))
        {
            return exactMatch;
        }

        return methods.FirstOrDefault(candidate =>
            string.Equals(GetDeclaringTypeSubjectId(candidate.SubjectId), constructorTypeSubjectId, StringComparison.Ordinal) &&
            string.Equals(GetMethodName(candidate.SubjectId), GetMethodName(declaredMethod.SubjectId), StringComparison.Ordinal) &&
            string.Equals(candidate.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
            ?.SubjectId;
    }

    private static bool LooksLikeInterfaceMethodSubjectId(string subjectId)
    {
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var typeName = declaringTypeSubjectId[(declaringTypeSubjectId.LastIndexOfAny(['/', '.']) + 1)..];
        return typeName.StartsWith("I", StringComparison.Ordinal) &&
               typeName.Length > 1 &&
               char.IsUpper(typeName[1]);
    }

    private static string GetMethodName(string subjectId)
    {
        var methodSeparatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        var returnTypeSeparatorIndex = subjectId.LastIndexOf(':');
        var parameterSeparatorIndex = subjectId.IndexOf('(', methodSeparatorIndex + 2);
        if (methodSeparatorIndex <= 0 || parameterSeparatorIndex <= methodSeparatorIndex + 2)
        {
            throw new InvalidOperationException($"failed to extract method name from subject id '{subjectId}'");
        }

        var methodEndIndex = returnTypeSeparatorIndex > methodSeparatorIndex
            && returnTypeSeparatorIndex < parameterSeparatorIndex
            ? returnTypeSeparatorIndex
            : parameterSeparatorIndex;
        if (methodEndIndex <= methodSeparatorIndex + 2)
        {
            throw new InvalidOperationException($"failed to extract method name from subject id '{subjectId}'");
        }

        return subjectId[(methodSeparatorIndex + 2)..methodEndIndex];
    }

    private static string GetMethodReturnType(string subjectId)
    {
        var returnTypeSeparatorIndex = subjectId.LastIndexOf(':');
        var parameterSeparatorIndex = subjectId.IndexOf('(', StringComparison.Ordinal);
        if (returnTypeSeparatorIndex <= 0 || parameterSeparatorIndex <= returnTypeSeparatorIndex + 1)
        {
            throw new InvalidOperationException($"failed to extract return type from subject id '{subjectId}'");
        }

        return subjectId[(returnTypeSeparatorIndex + 1)..parameterSeparatorIndex];
    }

    private static bool IsCompilerGeneratedAsyncStateMachineMethodSubjectId(string subjectId)
    {
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        return declaringTypeSubjectId.Contains("+<", StringComparison.Ordinal) &&
               declaringTypeSubjectId.Contains(">d__", StringComparison.Ordinal);
    }

    private static bool IsSupportedAsyncAwaitUnsafeOnCompletedCallee(string callee)
    {
        return IsSupportedAsyncBuilderCallee(callee, "::AwaitUnsafeOnCompleted<");
    }

    private static bool IsSupportedAsyncSetResultCallee(string callee)
    {
        return IsSupportedAsyncBuilderCallee(callee, "::SetResult:System.Void(");
    }

    private static bool IsSupportedAsyncSetExceptionCallee(string callee)
    {
        return IsSupportedAsyncBuilderCallee(callee, "::SetException:System.Void(System.Exception)");
    }

    private static bool IsSupportedAsyncSetStateMachineCallee(string callee)
    {
        return IsSupportedAsyncBuilderCallee(callee, "::SetStateMachine:System.Void(System.Runtime.CompilerServices.IAsyncStateMachine)");
    }

    private static bool IsSupportedAsyncTaskAwaiterGetResultCallee(string callee)
    {
        return IsSupportedAsyncAwaiterCallee(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter<") ||
               IsSupportedAsyncAwaiterCallee(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.ValueTaskAwaiter<");
    }

    private static bool IsSupportedAsyncBuilderCallee(string callee, string marker)
    {
        return (callee.StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<", StringComparison.Ordinal) ||
                callee.StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<", StringComparison.Ordinal)) &&
               callee.Contains(marker, StringComparison.Ordinal);
    }

    private static bool IsSupportedAsyncAwaiterCallee(string callee, string prefix)
    {
        return callee.StartsWith(prefix, StringComparison.Ordinal) &&
               callee.Contains("::GetResult:", StringComparison.Ordinal) &&
               callee.EndsWith("()", StringComparison.Ordinal);
    }

    private static bool TryParseClosedTaskResultType(string taskReturnType, out string managedResultType)
    {
        const string taskPrefix = "System.Threading.Tasks.Task<";
        managedResultType = string.Empty;
        if (!taskReturnType.StartsWith(taskPrefix, StringComparison.Ordinal) ||
            !taskReturnType.EndsWith(">", StringComparison.Ordinal) ||
            taskReturnType.Length <= taskPrefix.Length + 1)
        {
            return false;
        }

        managedResultType = taskReturnType[taskPrefix.Length..^1];
        return !string.IsNullOrWhiteSpace(managedResultType);
    }

    private static bool TryResolveAsyncManagedTypeShape(string managedType, out AsyncManagedTypeShape typeShape)
    {
        switch (managedType)
        {
            case "System.Boolean":
            case "System.Byte":
            case "System.SByte":
            case "System.Int16":
            case "System.UInt16":
            case "System.Int32":
            case "System.UInt32":
            case "System.Char":
                typeShape = new AsyncManagedTypeShape(managedType, "int32_t", "0", "ldc.i4", "integer-like literals");
                return true;
            case "System.Int64":
            case "System.UInt64":
                typeShape = new AsyncManagedTypeShape(managedType, "int64_t", "0", "ldc.i8", "int64 literals");
                return true;
            case "System.Single":
                typeShape = new AsyncManagedTypeShape(managedType, "float", "0.0f", "ldc.r4", "float literals");
                return true;
            case "System.Double":
                typeShape = new AsyncManagedTypeShape(managedType, "double", "0.0", "ldc.r8", "double literals");
                return true;
            case "System.String":
                typeShape = new AsyncManagedTypeShape(managedType, "void*", "nullptr", "ldstr", "string literals");
                return true;
            case "System.Object":
                typeShape = new AsyncManagedTypeShape(managedType, "void*", "nullptr", "ldnull", "null literals");
                return true;
            default:
                typeShape = null!;
                return false;
        }
    }

    private static bool TryResolveRuntimeSkeletonPrimitiveConvertShape(
        string inputManagedType,
        string outputManagedType,
        out string inputCppType,
        out string outputCppType,
        out string convertedValueExpression)
    {
        inputCppType = string.Empty;
        outputCppType = string.Empty;
        convertedValueExpression = string.Empty;

        if (string.Equals(inputManagedType, "System.Boolean", StringComparison.Ordinal) &&
            string.Equals(outputManagedType, "System.Byte", StringComparison.Ordinal))
        {
            inputCppType = "bool";
            outputCppType = "std::uint8_t";
            convertedValueExpression = "request->value ? static_cast<std::uint8_t>(1) : static_cast<std::uint8_t>(0)";
            return true;
        }

        if (string.Equals(inputManagedType, "System.Byte", StringComparison.Ordinal) &&
            string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal))
        {
            inputCppType = "std::uint8_t";
            outputCppType = "bool";
            convertedValueExpression = "request->value != static_cast<std::uint8_t>(0)";
            return true;
        }

        if (string.Equals(inputManagedType, outputManagedType, StringComparison.Ordinal) &&
            TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType))
        {
            outputCppType = inputCppType;
            convertedValueExpression = "request->value";
            return true;
        }

        if (TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType) &&
            TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out outputCppType))
        {
            convertedValueExpression = "request->value";
            return true;
        }

        if (string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal) &&
            TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType))
        {
            outputCppType = "bool";
            convertedValueExpression = "request->value != 0";
            return true;
        }

        return false;
    }

    private static bool MatchesRuntimeSkeletonPrimitiveConvertInstructionShape(
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        string inputManagedType,
        string outputManagedType)
    {
        if (instructions.Count == 2)
        {
            return string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                   GetRequiredOperandInt(instructions[0]) == 0 &&
                   string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal) &&
                   TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out _) &&
                   TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out _) &&
                   IsRuntimeSkeletonDirectPrimitiveValuePreservingReturn(inputManagedType, outputManagedType);
        }

        if (TryMatchRuntimeSkeletonPrimitiveConvRetInstructionShape(instructions, inputManagedType, outputManagedType))
        {
            return true;
        }

        if (instructions.Count == 6)
        {
            return string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal) &&
                   (string.Equals(inputManagedType, "System.Single", StringComparison.Ordinal) ||
                    string.Equals(inputManagedType, "System.Double", StringComparison.Ordinal)) &&
                   string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                   GetRequiredOperandInt(instructions[0]) == 0 &&
                   string.Equals(instructions[1].Op, string.Equals(inputManagedType, "System.Single", StringComparison.Ordinal) ? "ldc.r4" : "ldc.r8", StringComparison.Ordinal) &&
                   IsZeroLiteralOperand(instructions[1]) &&
                   string.Equals(instructions[2].Op, "ceq", StringComparison.Ordinal) &&
                   string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
                   GetRequiredOperandInt(instructions[3]) == 0 &&
                   string.Equals(instructions[4].Op, "ceq", StringComparison.Ordinal) &&
                   string.Equals(instructions[5].Op, "ret", StringComparison.Ordinal);
        }

        if (instructions.Count < 4 || instructions.Count > 5)
        {
            return false;
        }

        if (!string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[0]) != 0 ||
            !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[1]) != 0)
        {
            return false;
        }

        var compareInstructionIndex = 2;
        if (instructions.Count == 5 &&
            string.Equals(instructions[2].Op, "conv.i8", StringComparison.Ordinal))
        {
            compareInstructionIndex = 3;
        }

        var compareOp = instructions[compareInstructionIndex].Op;
        if (!string.Equals(compareOp, "cgt", StringComparison.Ordinal) &&
            !string.Equals(compareOp, "cgt.un", StringComparison.Ordinal))
        {
            return false;
        }

        if (compareInstructionIndex == 3)
        {
            return string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal) &&
                   string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal) &&
                   (string.Equals(inputManagedType, "System.Int64", StringComparison.Ordinal) ||
                    string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal)) &&
                   string.Equals(compareOp, "cgt.un", StringComparison.Ordinal);
        }

        if (instructions.Count == 4)
        {
            return string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal) &&
                   string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal) &&
                   (string.Equals(compareOp, "cgt", StringComparison.Ordinal) ||
                    string.Equals(compareOp, "cgt.un", StringComparison.Ordinal));
        }

        return string.Equals(instructions[3].Op, "conv.u1", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal) &&
               string.Equals(inputManagedType, "System.Boolean", StringComparison.Ordinal) &&
               string.Equals(outputManagedType, "System.Byte", StringComparison.Ordinal) &&
               string.Equals(compareOp, "cgt.un", StringComparison.Ordinal);
    }

    private static bool TryResolveRuntimeSkeletonCheckedByteConvertShape(
        string subjectId,
        string inputManagedType,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        out string inputCppType,
        out string overflowConditionExpression,
        out string throwSubjectId)
    {
        inputCppType = string.Empty;
        overflowConditionExpression = string.Empty;
        throwSubjectId = string.Empty;

        if (!TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType))
        {
            return false;
        }

        if (string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal))
        {
            if (instructions.Count != 8 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 255 ||
                !string.Equals(instructions[2].Op, "conv.i8", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ble.un", StringComparison.Ordinal) ||
                !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[5].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[5]) != 0 ||
                !string.Equals(instructions[6].Op, "conv.u1", StringComparison.Ordinal) ||
                !string.Equals(instructions[7].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            throwSubjectId = GetRequiredInstructionCallee(instructions[4], subjectId, 4);
            overflowConditionExpression = "request->value > static_cast<std::uint64_t>(255)";
            return true;
        }

        switch (inputManagedType)
        {
            case "System.SByte":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 0 ||
                    !string.Equals(instructions[2].Op, "bge", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<std::int8_t>(0)";
                return true;
            case "System.Int16":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 255 ||
                    !string.Equals(instructions[2].Op, "ble.un", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<std::int16_t>(0) || request->value > static_cast<std::int16_t>(255)";
                return true;
            case "System.UInt16":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 255 ||
                    !string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value > static_cast<std::uint16_t>(255)";
                return true;
            case "System.UInt32":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 255 ||
                    !string.Equals(instructions[2].Op, "ble.un", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value > static_cast<std::uint32_t>(255)";
                return true;
            case "System.Char":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 255 ||
                    !string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value > static_cast<std::uint16_t>(255)";
                return true;
            default:
                return false;
        }
    }

    private static bool TryResolveRuntimeSkeletonCheckedCharConvertShape(
        string subjectId,
        string inputManagedType,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        out string inputCppType,
        out string overflowConditionExpression,
        out string throwSubjectId)
    {
        inputCppType = string.Empty;
        overflowConditionExpression = string.Empty;
        throwSubjectId = string.Empty;

        if (!TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType))
        {
            return false;
        }

        if (string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal))
        {
            if (instructions.Count != 8 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 65535 ||
                !string.Equals(instructions[2].Op, "conv.i8", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ble.un", StringComparison.Ordinal) ||
                !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[5].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[5]) != 0 ||
                !string.Equals(instructions[6].Op, "conv.u2", StringComparison.Ordinal) ||
                !string.Equals(instructions[7].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            throwSubjectId = GetRequiredInstructionCallee(instructions[4], subjectId, 4);
            overflowConditionExpression = "request->value > static_cast<std::uint64_t>(65535)";
            return true;
        }

        switch (inputManagedType)
        {
            case "System.SByte":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 0 ||
                    !string.Equals(instructions[2].Op, "bge", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u2", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<std::int8_t>(0)";
                return true;
            case "System.Int16":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 0 ||
                    !string.Equals(instructions[2].Op, "bge", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u2", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<std::int16_t>(0)";
                return true;
            case "System.UInt32":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 65535 ||
                    !string.Equals(instructions[2].Op, "ble.un", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u2", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value > static_cast<std::uint32_t>(65535)";
                return true;
            default:
                return false;
        }
    }


    private static bool TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(
        string inputManagedType,
        out string inputCppType)
    {
        inputCppType = string.Empty;
        switch (inputManagedType)
        {
            case "System.Boolean":
                inputCppType = "bool";
                return true;
            case "System.Byte":
                inputCppType = "std::uint8_t";
                return true;
            case "System.SByte":
                inputCppType = "std::int8_t";
                return true;
            case "System.Int16":
                inputCppType = "std::int16_t";
                return true;
            case "System.UInt16":
                inputCppType = "std::uint16_t";
                return true;
            case "System.Int32":
                inputCppType = "std::int32_t";
                return true;
            case "System.UInt32":
                inputCppType = "std::uint32_t";
                return true;
            case "System.Int64":
                inputCppType = "std::int64_t";
                return true;
            case "System.UInt64":
                inputCppType = "std::uint64_t";
                return true;
            case "System.Char":
                inputCppType = "std::uint16_t";
                return true;
            case "System.Single":
                inputCppType = "float";
                return true;
            case "System.Double":
                inputCppType = "double";
                return true;
            default:
                return false;
        }
    }

    private static bool TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(
        string outputManagedType,
        out string outputCppType)
    {
        return TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(outputManagedType, out outputCppType);
    }

    private static bool IsRuntimeSkeletonDirectPrimitiveValuePreservingReturn(
        string inputManagedType,
        string outputManagedType)
    {
        if (string.Equals(inputManagedType, outputManagedType, StringComparison.Ordinal))
        {
            return true;
        }

        if ((string.Equals(inputManagedType, "System.UInt16", StringComparison.Ordinal) &&
             string.Equals(outputManagedType, "System.Char", StringComparison.Ordinal)) ||
            (string.Equals(inputManagedType, "System.Char", StringComparison.Ordinal) &&
             string.Equals(outputManagedType, "System.UInt16", StringComparison.Ordinal)))
        {
            return true;
        }

        return string.Equals(inputManagedType, "System.Byte", StringComparison.Ordinal) &&
               (string.Equals(outputManagedType, "System.Char", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.UInt16", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Int16", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Int32", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.UInt32", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Int64", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.UInt64", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Single", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Double", StringComparison.Ordinal));
    }

    private static bool TryMatchRuntimeSkeletonPrimitiveConvRetInstructionShape(
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        string inputManagedType,
        string outputManagedType)
    {
        if (!TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out _) ||
            !TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out _))
        {
            return false;
        }

        if (instructions.Count == 3 &&
            string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[0]) == 0 &&
            string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
        {
            return IsExpectedPrimitiveConvOp(instructions[1].Op, inputManagedType, outputManagedType);
        }

        return instructions.Count == 4 &&
               string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
               GetRequiredOperandInt(instructions[0]) == 0 &&
               string.Equals(instructions[1].Op, "conv.r.un", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal) &&
               IsExpectedPrimitiveConvOp(instructions[2].Op, inputManagedType, outputManagedType) &&
               (string.Equals(inputManagedType, "System.UInt32", StringComparison.Ordinal) ||
                string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal)) &&
               (string.Equals(outputManagedType, "System.Single", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Double", StringComparison.Ordinal));
    }

    private static bool IsExpectedPrimitiveConvOp(
        string op,
        string inputManagedType,
        string outputManagedType)
    {
        if (string.Equals(inputManagedType, outputManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        if (string.Equals(outputManagedType, "System.Byte", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.u1", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.SByte", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.i1", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.Int16", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.i2", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.UInt16", StringComparison.Ordinal) ||
            string.Equals(outputManagedType, "System.Char", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.u2", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.Int32", StringComparison.Ordinal) ||
            string.Equals(outputManagedType, "System.UInt32", StringComparison.Ordinal))
        {
            return false;
        }

        if (string.Equals(outputManagedType, "System.Int64", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.i8", StringComparison.Ordinal) ||
                   string.Equals(op, "conv.u8", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.UInt64", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.i8", StringComparison.Ordinal) ||
                   string.Equals(op, "conv.u8", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.Single", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.r4", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.Double", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.r8", StringComparison.Ordinal);
        }

        return false;
    }

    private static bool IsZeroLiteralOperand(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            int value => value == 0,
            long value => value == 0L,
            float value => value == 0f,
            double value => value == 0d,
            JsonElement element when element.ValueKind == JsonValueKind.Number => element.GetDouble() == 0d,
            _ => false,
        };
    }

    private static bool TryCreateAsyncTaskFamilyShape(string taskReturnType, out AsyncTaskFamilyShape familyShape)
    {
        familyShape = null!;
        if (!TryParseClosedTaskResultType(taskReturnType, out var managedResultType) ||
            !TryResolveAsyncManagedTypeShape(managedResultType, out var resultTypeShape))
        {
            return false;
        }

        familyShape = new AsyncTaskFamilyShape(managedResultType, resultTypeShape);
        return true;
    }

    private static bool TryResolveAsyncTaskFactoryMethodShape(
        TypedIlMethodArtifact method,
        out AsyncTaskFactoryMethodShape methodShape)
    {
        methodShape = null!;
        if (method.Parameters.Count > 2 ||
            !TryCreateAsyncTaskFamilyShape(GetMethodReturnType(method.SubjectId), out var familyShape))
        {
            return false;
        }

        var parameterTypeShapes = new List<AsyncManagedTypeShape>(method.Parameters.Count);
        foreach (var parameter in method.Parameters)
        {
            if (!TryResolveAsyncManagedTypeShape(parameter.Type, out var parameterTypeShape))
            {
                return false;
            }

            parameterTypeShapes.Add(parameterTypeShape);
        }

        methodShape = new AsyncTaskFactoryMethodShape(familyShape, parameterTypeShapes);
        return true;
    }

    private static bool ResolveAsyncLiteralParameterShapes(
        IReadOnlyList<TypedIlParameterArtifact> producerParameters,
        out IReadOnlyList<AsyncManagedTypeShape> literalParameterShapes)
    {
        var resolvedShapes = new List<AsyncManagedTypeShape>(producerParameters.Count);
        foreach (var parameter in producerParameters)
        {
            if (!TryResolveAsyncManagedTypeShape(parameter.Type, out var parameterTypeShape) ||
                !parameterTypeShape.SupportsLiteralLoading)
            {
                literalParameterShapes = [];
                return false;
            }

            resolvedShapes.Add(parameterTypeShape);
        }

        literalParameterShapes = resolvedShapes;
        return true;
    }

    private static bool TryResolveAsyncGetResultWrapperShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        out AsyncGetResultWrapperShape wrapperShape)
    {
        wrapperShape = null!;
        if (!TryResolveAsyncManagedTypeShape(GetMethodReturnType(method.SubjectId), out var wrapperResultTypeShape))
        {
            return false;
        }

        for (var callInstructionIndex = 0; callInstructionIndex <= instructions.Count - 6; callInstructionIndex++)
        {
            if (!string.Equals(instructions[callInstructionIndex].Op, "call", StringComparison.Ordinal) ||
                string.IsNullOrWhiteSpace(instructions[callInstructionIndex].Callee))
            {
                continue;
            }

            var producedTaskMethod = instructions[callInstructionIndex].Callee!;
            if (string.Equals(producedTaskMethod, method.SubjectId, StringComparison.Ordinal) ||
                !methodsBySubjectId.TryGetValue(producedTaskMethod, out var producerMethod) ||
                !TryResolveAsyncTaskFactoryMethodShape(producerMethod, out var producerMethodShape) ||
                !ResolveAsyncLiteralParameterShapes(producerMethod.Parameters, out var expectedLiteralParameterShapes))
            {
                continue;
            }

            if (!string.Equals(producerMethodShape.Family.ManagedResultType, wrapperResultTypeShape.ManagedType, StringComparison.Ordinal) ||
                callInstructionIndex != expectedLiteralParameterShapes.Count ||
                instructions.Count != expectedLiteralParameterShapes.Count + 6)
            {
                continue;
            }

            var matchesLiteralPrefix = true;
            for (var instructionIndex = 0; instructionIndex < expectedLiteralParameterShapes.Count; instructionIndex++)
            {
                if (!string.Equals(
                        instructions[instructionIndex].Op,
                        expectedLiteralParameterShapes[instructionIndex].LiteralOp,
                        StringComparison.Ordinal))
                {
                    matchesLiteralPrefix = false;
                    break;
                }
            }

            if (!matchesLiteralPrefix)
            {
                continue;
            }

            wrapperShape = new AsyncGetResultWrapperShape(producerMethodShape.Family, expectedLiteralParameterShapes);
            return true;
        }

        return false;
    }

    private static void ValidateAsyncTaskFactoryShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        AsyncTaskFactoryMethodShape methodShape)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");

        var argc = methodShape.ParameterTypeShapes.Count;
        if (argc > 2)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async {methodShape.Family.DisplayName} factory '{method.SubjectId}' to take no more than two supported arguments");
        }

        var expectedInstructionCount = 14 + (argc * 3);
        RequireInstructionCount(method, instructions, expectedInstructionCount);
        RequireInstructionOp(instructions[0], "ldloca", method.SubjectId, 0);
        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to initialize local state machine slot 0");
        }

        RequireInstructionOp(instructions[1], "call", method.SubjectId, 1);
        RequireInstructionCallee(
            instructions[1],
            methodShape.Family.BuilderCreateCallee,
            method.SubjectId,
            1);
        RequireInstructionOp(instructions[2], "stfld", method.SubjectId, 2);

        var instructionIndex = 3;
        for (var argIndex = 0; argIndex < argc; argIndex++)
        {
            RequireInstructionOp(instructions[instructionIndex], "ldloca", method.SubjectId, instructionIndex);
            if (GetRequiredOperandInt(instructions[instructionIndex]) != 0)
            {
                throw new InvalidOperationException(
                    $"native-reference emitter expects '{method.SubjectId}' to reload local state machine slot 0");
            }

            RequireInstructionOp(instructions[instructionIndex + 1], "ldarg", method.SubjectId, instructionIndex + 1);
            if (GetRequiredOperandInt(instructions[instructionIndex + 1]) != argIndex)
            {
                throw new InvalidOperationException(
                    $"native-reference emitter expects '{method.SubjectId}' to store async {methodShape.Family.DisplayName} wrapper argument {argIndex}");
            }

            RequireInstructionOp(instructions[instructionIndex + 2], "stfld", method.SubjectId, instructionIndex + 2);
            instructionIndex += 3;
        }

        RequireInstructionOp(instructions[instructionIndex], "ldloca", method.SubjectId, instructionIndex);
        RequireInstructionOp(instructions[instructionIndex + 1], "ldc.i4", method.SubjectId, instructionIndex + 1);
        RequireInstructionOp(instructions[instructionIndex + 2], "stfld", method.SubjectId, instructionIndex + 2);
        if (GetRequiredOperandInt(instructions[instructionIndex]) != 0 ||
            GetRequiredOperandInt(instructions[instructionIndex + 1]) != -1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to set async state to -1 on local slot 0");
        }

        RequireInstructionOp(instructions[instructionIndex + 3], "ldloca", method.SubjectId, instructionIndex + 3);
        RequireInstructionOp(instructions[instructionIndex + 4], "ldflda", method.SubjectId, instructionIndex + 4);
        RequireInstructionOp(instructions[instructionIndex + 5], "ldloca", method.SubjectId, instructionIndex + 5);
        RequireInstructionOp(instructions[instructionIndex + 6], "call", method.SubjectId, instructionIndex + 6);
        RequireInstructionOp(instructions[instructionIndex + 7], "ldloca", method.SubjectId, instructionIndex + 7);
        RequireInstructionOp(instructions[instructionIndex + 8], "ldflda", method.SubjectId, instructionIndex + 8);
        RequireInstructionOp(instructions[instructionIndex + 9], "call", method.SubjectId, instructionIndex + 9);
        RequireInstructionOp(instructions[instructionIndex + 10], "ret", method.SubjectId, instructionIndex + 10);

        if (GetRequiredOperandInt(instructions[instructionIndex + 3]) != 0 ||
            GetRequiredOperandInt(instructions[instructionIndex + 5]) != 0 ||
            GetRequiredOperandInt(instructions[instructionIndex + 7]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to use local async state machine slot 0");
        }

        RequireInstructionCalleePrefix(
            instructions[instructionIndex + 6],
            methodShape.Family.BuilderStartPrefix,
            method.SubjectId,
            instructionIndex + 6);
        RequireInstructionCallee(
            instructions[instructionIndex + 9],
            methodShape.Family.BuilderGetTaskCallee,
            method.SubjectId,
            instructionIndex + 9);
    }

    private static AsyncGetResultWrapperShape ValidateAsyncGetResultShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (method.Parameters.Count != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async get-result wrapper '{method.SubjectId}' to be parameterless");
        }

        if (!TryResolveAsyncGetResultWrapperShape(method, instructions, methodsBySubjectId, out var wrapperShape))
        {
            var literalExpectation = "supported literal arguments";
            if (instructions.Count >= 6 &&
                !string.IsNullOrWhiteSpace(instructions[0].Callee) &&
                methodsBySubjectId.TryGetValue(instructions[0].Callee!, out var producerMethod) &&
                ResolveAsyncLiteralParameterShapes(producerMethod.Parameters, out var literalParameterShapes))
            {
                literalExpectation = literalParameterShapes.Count == 0
                    ? "no literal arguments"
                    : string.Join(", ", literalParameterShapes.Select(shape => shape.LiteralExpectation));
            }

            throw new InvalidOperationException(
                $"native-reference emitter expects async get-result wrapper '{method.SubjectId}' to load {literalExpectation} before invoking the async producer");
        }

        var producerFamilyShape = wrapperShape.ProducerFamilyShape;
        var expectedLiteralParameterShapes = wrapperShape.ExpectedLiteralParameterShapes;
        var callInstructionIndex = expectedLiteralParameterShapes.Count;
        RequireInstructionOp(instructions[callInstructionIndex], "call", method.SubjectId, callInstructionIndex);
        RequireInstructionOp(instructions[callInstructionIndex + 1], "call", "callvirt", method.SubjectId, callInstructionIndex + 1);
        RequireInstructionOp(instructions[callInstructionIndex + 2], "stloc", method.SubjectId, callInstructionIndex + 2);
        RequireInstructionOp(instructions[callInstructionIndex + 3], "ldloca", method.SubjectId, callInstructionIndex + 3);
        RequireInstructionOp(instructions[callInstructionIndex + 4], "call", method.SubjectId, callInstructionIndex + 4);
        RequireInstructionOp(instructions[callInstructionIndex + 5], "ret", method.SubjectId, callInstructionIndex + 5);

        var producedTaskMethod = GetRequiredInstructionCallee(instructions[callInstructionIndex], method.SubjectId, callInstructionIndex);
        if (string.Equals(producedTaskMethod, method.SubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to consume an async producer, not recurse into itself");
        }

        RequireInstructionCallee(
            instructions[callInstructionIndex + 1],
            producerFamilyShape.TaskGetAwaiterCallee,
            method.SubjectId,
            callInstructionIndex + 1);
        RequireInstructionCallee(
            instructions[callInstructionIndex + 4],
            producerFamilyShape.AwaiterGetResultCallee,
            method.SubjectId,
            callInstructionIndex + 4);
        return wrapperShape;
    }

    private static void ValidateAsyncStateMachineMoveNextShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-method", "has-canonical-body");
        if (instructions.Count < 20)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to contain the canonical async body");
        }

        var callees = instructions
            .Where(instruction =>
                (string.Equals(instruction.Op, "call", StringComparison.Ordinal) ||
                 string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal)) &&
                !string.IsNullOrWhiteSpace(instruction.Callee))
            .Select(instruction => instruction.Callee!)
            .ToList();

        if (!callees.Contains("System.Private.CoreLib/System.Threading.Tasks.Task::Yield:System.Runtime.CompilerServices.YieldAwaitable()", StringComparer.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to call Task::Yield()");
        }

        if (!callees.Any(IsSupportedAsyncAwaitUnsafeOnCompletedCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to call AwaitUnsafeOnCompleted");
        }

        if (!callees.Any(IsSupportedAsyncSetResultCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to call SetResult");
        }

        if (!callees.Any(IsSupportedAsyncSetExceptionCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to call SetException");
        }

        if (!callees.Contains(
                "System.Private.CoreLib/System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter::GetResult:System.Void()",
                StringComparer.Ordinal) &&
            !callees.Any(IsSupportedAsyncTaskAwaiterGetResultCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to observe an awaiter GetResult call");
        }
    }

    private static void ValidateAsyncStateMachineSetStateMachineShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-method", "has-canonical-body");
        if (method.Parameters.Count != 1 ||
            !string.Equals(method.Parameters[0].Type, "System.Runtime.CompilerServices.IAsyncStateMachine", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine SetStateMachine '{method.SubjectId}' to take IAsyncStateMachine");
        }

        RequireInstructionCount(method, instructions, 5);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldflda", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldarg", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "ret", method.SubjectId, 4);

        if (GetRequiredOperandInt(instructions[0]) != 0 ||
            GetRequiredOperandInt(instructions[2]) != 1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to load ldarg 0 and ldarg 1");
        }

        var setStateMachineCallee = instructions[3].Callee;
        if (string.IsNullOrWhiteSpace(setStateMachineCallee) ||
            !IsSupportedAsyncSetStateMachineCallee(setStateMachineCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction 3 in '{method.SubjectId}' to call a supported AsyncTaskMethodBuilder<T>.SetStateMachine, but found '{setStateMachineCallee ?? "<null>"}'");
        }
    }

    private static void ValidateSingleArgumentForwarderShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-forwarder", "has-canonical-body");
        RequireInstructionCount(method, instructions, 2);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ret", method.SubjectId, 1);

        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to forward ldarg 0");
        }
    }

    private static void ValidateDelegateClosedTargetRelayEntryPointShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");

        if (!IsDelegateClosedTargetRelayEntryPointShape(instructions))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to follow delegate-closed-target-relay proof shape");
        }

        var writeLineInstructionIndex = GetDelegateClosedTargetRelayWriteLineInstructionIndex(instructions);
        if (!IsConsoleWriteLineStringTarget(GetRequiredInstructionCallee(instructions[writeLineInstructionIndex], method.SubjectId, writeLineInstructionIndex)))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to call Console.WriteLine(System.String)");
        }

        var returnValueInstructionIndex = GetDelegateClosedTargetRelayReturnValueInstructionIndex(instructions);
        if (GetRequiredOperandInt(instructions[returnValueInstructionIndex]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to return ldc.i4 0 before ret");
        }
    }

    private static bool IsDelegateClosedTargetRelayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "ldftn", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldsfld", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "brtrue", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "pop", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldnull", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldftn", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "stsfld", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ret", StringComparison.Ordinal);
    }

    private static int GetDelegateClosedTargetRelayStaticMethodInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 10
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }

    private static int GetDelegateClosedTargetRelayMessagePrefixInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 15
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }

    private static int GetDelegateClosedTargetRelayWriteLineInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 18
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }

    private static int GetDelegateClosedTargetRelayReturnValueInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 19
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }

    private static void ValidateDelegateClosedTargetRelayStaticTailShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        RequireInstructionCount(method, instructions, 4);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldstr", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "call", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "ret", method.SubjectId, 3);

        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to load ldarg 0 before appending a trailing literal");
        }

        var concatIcall = NormalizeStringConcatIcall(GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2));
        if (!string.Equals(concatIcall, StringConcatPairIcall, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects delegate static tail '{method.SubjectId}' to use pair string concat");
        }
    }

    private static string GetDelegateClosedTargetRelayTrailingLiteral(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        ValidateDelegateClosedTargetRelayStaticTailShape(method, instructions);
        return GetRequiredOperandString(instructions[1]);
    }

    private static void ValidateStaticExceptionThrowLiteralShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.Void", StringComparison.Ordinal) ||
            method.Parameters.Count != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects static exception throw literal '{method.SubjectId}' to be parameterless and return void");
        }

        RequireInstructionCount(method, instructions, 3);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "newobj", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "throw", method.SubjectId, 2);
        RequireInstructionCallee(
            instructions[1],
            "System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)",
            method.SubjectId,
            1);
    }

    private static void ValidateStaticExceptionThrowStringProducerShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.Void", StringComparison.Ordinal) ||
            method.Parameters.Count != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects static exception throw string producer '{method.SubjectId}' to be parameterless and return void");
        }

        RequireInstructionCount(method, instructions, 3);
        RequireInstructionOp(instructions[0], "call", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "newobj", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "throw", method.SubjectId, 2);

        var exceptionConstructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (!exceptionConstructorSubjectId.EndsWith("::.ctor:System.Void(System.String)", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects static exception throw string producer '{method.SubjectId}' to construct a string-taking exception, but found '{exceptionConstructorSubjectId}'");
        }
    }

    private static void ValidateStaticExceptionCatchStringReturnShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal) ||
            method.Parameters.Count != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects static exception catch string return '{method.SubjectId}' to be parameterless and return string");
        }

        RequireInstructionCount(method, instructions, 10);
        RequireInstructionOp(instructions[0], "call", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldstr", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "stloc", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "leave", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "pop", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ldstr", method.SubjectId, 5);
        RequireInstructionOp(instructions[6], "stloc", method.SubjectId, 6);
        RequireInstructionOp(instructions[7], "leave", method.SubjectId, 7);
        RequireInstructionOp(instructions[8], "ldloc", method.SubjectId, 8);
        RequireInstructionOp(instructions[9], "ret", method.SubjectId, 9);
    }

    private static void ValidateFieldGetterShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-field-getter", "has-canonical-body");
        RequireInstructionCount(method, instructions, 3);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldfld", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ret", method.SubjectId, 2);

        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to start with ldarg 0");
        }
    }

    private static string GetCapturedFieldSubjectIdForSupportedStringInstanceMethod(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        try
        {
            ValidateFieldGetterShape(method, instructions);
            return GetRequiredOperandString(instructions[1]);
        }
        catch
        {
        }

        ValidateFieldBackedStringInstanceMethodShape(method, instructions);
        return GetRequiredOperandString(instructions[2]);
    }

    private static bool HasMethodContract(
        TypedIlMethodArtifact method,
        string expectedMethodRole,
        string expectedBodyAvailability)
    {
        return string.Equals(method.MethodRole, expectedMethodRole, StringComparison.Ordinal) &&
               string.Equals(method.BodyAvailability, expectedBodyAvailability, StringComparison.Ordinal);
    }

    private static bool IsRuntimeSkeletonNonExecutableDeclaration(TypedIlMethodArtifact method)
    {
        return string.Equals(method.BodyAvailability, "no-canonical-body", StringComparison.Ordinal) &&
               !string.Equals(method.MethodRole, "imported-method", StringComparison.Ordinal) &&
               method.Blocks.All(block => block.Instructions.Count == 0);
    }

    private static bool HasCapability(
        TypedIlMethodArtifact method,
        string requiredCapability)
    {
        return method.Capabilities.Contains(requiredCapability, StringComparer.Ordinal);
    }

    private static void RequireMethodContract(
        TypedIlMethodArtifact method,
        string expectedMethodRole,
        string expectedBodyAvailability)
    {
        if (!string.Equals(method.MethodRole, expectedMethodRole, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to have method role '{expectedMethodRole}', but found '{method.MethodRole}'");
        }

        if (!string.Equals(method.BodyAvailability, expectedBodyAvailability, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to have body availability '{expectedBodyAvailability}', but found '{method.BodyAvailability}'");
        }
    }

    private static void RequireCapability(
        TypedIlMethodArtifact method,
        string requiredCapability)
    {
        if (!method.Capabilities.Contains(requiredCapability, StringComparer.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to carry capability '{requiredCapability}'");
        }
    }

    private static void RequireInstructionCount(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        int expectedCount)
    {
        if (instructions.Count != expectedCount)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to have {expectedCount} instructions, but found {instructions.Count}");
        }
    }

    private static void RequireInstructionOp(
        TypedIlInstructionArtifact instruction,
        string expectedOp,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Op, expectedOp, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to be '{expectedOp}', but found '{instruction.Op}'");
        }
    }

    private static void RequireInstructionOp(
        TypedIlInstructionArtifact instruction,
        string expectedOp,
        string alternateExpectedOp,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Op, expectedOp, StringComparison.Ordinal) &&
            !string.Equals(instruction.Op, alternateExpectedOp, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to be '{expectedOp}' or '{alternateExpectedOp}', but found '{instruction.Op}'");
        }
    }

    private static void RequireInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string expectedCallee,
        string subjectId,
        int instructionIndex)
    {
        RequireInstructionCallee(instruction, expectedCallee, null, subjectId, instructionIndex);
    }

    private static void RequireInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string expectedCallee,
        string? alternateExpectedCallee,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Callee, expectedCallee, StringComparison.Ordinal) &&
            !string.Equals(instruction.Callee, alternateExpectedCallee, StringComparison.Ordinal))
        {
            var expectedDescription = alternateExpectedCallee is null
                ? $"'{expectedCallee}'"
                : $"'{expectedCallee}' or '{alternateExpectedCallee}'";
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to call {expectedDescription}, but found '{instruction.Callee ?? "<null>"}'");
        }
    }

    private static void RequireInstructionCalleePrefix(
        TypedIlInstructionArtifact instruction,
        string expectedPrefix,
        string subjectId,
        int instructionIndex)
    {
        if (string.IsNullOrWhiteSpace(instruction.Callee) ||
            !instruction.Callee.StartsWith(expectedPrefix, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to call a method starting with '{expectedPrefix}', but found '{instruction.Callee ?? "<null>"}'");
        }
    }

    private static string GetRequiredInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string subjectId,
        int instructionIndex)
    {
        if (string.IsNullOrWhiteSpace(instruction.Callee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to carry a callee");
        }

        return instruction.Callee;
    }

    private static string GetRequiredOperandString(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            string value => value,
            JsonElement element when element.ValueKind == JsonValueKind.String => element.GetString()
                ?? throw new InvalidOperationException($"expected string operand for '{instruction.Op}'"),
            _ => throw new InvalidOperationException($"expected string operand for '{instruction.Op}'"),
        };
    }

    private static int GetRequiredOperandInt(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            int value => value,
            long value => checked((int)value),
            JsonElement element when element.ValueKind == JsonValueKind.Number => element.GetInt32(),
            _ => throw new InvalidOperationException($"expected integer operand for '{instruction.Op}'"),
        };
    }

    private static int GetRequiredPromotedInt64ConstantAsInt(
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        int constantInstructionIndex,
        string subjectId)
    {
        RequireInstructionOp(instructions[constantInstructionIndex], "ldc.i4", subjectId, constantInstructionIndex);
        RequireInstructionOp(instructions[constantInstructionIndex + 1], "conv.i8", subjectId, constantInstructionIndex + 1);
        return GetRequiredOperandInt(instructions[constantInstructionIndex]);
    }

    private static string GetDeclaringTypeSubjectId(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException($"failed to extract declaring type from subject id '{subjectId}'");
        }

        return subjectId[..separatorIndex];
    }

    private static MetadataRegistrationEntry GetRequiredMetadataRegistration(
        IReadOnlyList<MetadataRegistrationEntry> registrations,
        string registrationKind,
        string subjectId,
        string description)
    {
        if (GetMetadataRegistrationLookup(registrations).TryGetRegistration(registrationKind, subjectId, out var registration))
        {
            return registration;
        }

        throw new InvalidOperationException(
            $"missing required '{registrationKind}' metadata registration for {description}");
    }

    private static MetadataRegistrationEntry GetRequiredMetadataRegistration(
        IReadOnlyList<MetadataRegistrationEntry> registrations,
        string registrationKind,
        Func<MetadataRegistrationEntry, bool> predicate,
        string description)
    {
        var registration = GetMetadataRegistrationsForKind(registrations, registrationKind)
            .FirstOrDefault(predicate);
        if (registration is null)
        {
            throw new InvalidOperationException(
                $"missing required '{registrationKind}' metadata registration for {description}");
        }

        return registration;
    }

    private static string GetRequiredRegistrationName(MetadataRegistrationEntry registration)
    {
        if (!string.IsNullOrWhiteSpace(registration.Name))
        {
            return registration.Name;
        }

        throw new InvalidOperationException(
            $"metadata registration '{registration.SubjectId}' is missing name metadata");
    }

    private static string GetRequiredRegistrationDisplayName(MetadataRegistrationEntry registration)
    {
        if (!string.IsNullOrWhiteSpace(registration.DisplayName))
        {
            return registration.DisplayName;
        }

        if (!string.IsNullOrWhiteSpace(registration.Name))
        {
            return registration.Name;
        }

        throw new InvalidOperationException(
            $"metadata registration '{registration.SubjectId}' is missing displayName metadata");
    }

    private static bool IsCompilerGeneratedMemberName(string? name)
    {
        return string.IsNullOrWhiteSpace(name) || name.StartsWith("<", StringComparison.Ordinal);
    }

    private static bool IsSpecialMethodName(string? name)
    {
        if (string.IsNullOrWhiteSpace(name))
        {
            return true;
        }

        return name.StartsWith(".", StringComparison.Ordinal) ||
               name.StartsWith("<", StringComparison.Ordinal) ||
               name.StartsWith("get_", StringComparison.Ordinal) ||
               name.StartsWith("set_", StringComparison.Ordinal) ||
               name.StartsWith("add_", StringComparison.Ordinal) ||
               name.StartsWith("remove_", StringComparison.Ordinal);
    }

    private static uint GetRequiredMetadataToken(
        MetadataRegistrationArtifact metadataRegistration,
        string registrationKind,
        string subjectId)
    {
        return GetMetadataRegistrationLookup(metadataRegistration.Registrations)
            .GetRequiredToken(registrationKind, subjectId);
    }

    private static string CreateTypeTokenLiteral(
        MetadataRegistrationArtifact metadataRegistration,
        string subjectId)
    {
        var hasConcreteRegistration = GetMetadataRegistrationLookup(metadataRegistration.Registrations)
            .HasConcreteType(subjectId);
        return hasConcreteRegistration
            ? FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", subjectId))
            : FormatCppTokenLiteral(CreatePseudoTypeToken(subjectId));
    }

    private static uint CreatePseudoTypeToken(string subjectId)
    {
        const uint typeTokenPrefix = 0x02000000u;
        uint hash = 2166136261u;

        foreach (var current in subjectId)
        {
            hash ^= current;
            hash *= 16777619u;
        }

        var rowIndex = hash & 0x00FFFFFFu;
        if (rowIndex == 0u)
        {
            rowIndex = 1u;
        }

        return typeTokenPrefix | rowIndex;
    }

    private static bool TryGetMetadataTokenPrefix(string registrationKind, out uint tokenPrefix)
    {
        switch (registrationKind)
        {
            case "type":
                tokenPrefix = 0x02000000u;
                return true;
            case "field":
                tokenPrefix = 0x04000000u;
                return true;
            case "method":
                tokenPrefix = 0x06000000u;
                return true;
            default:
                tokenPrefix = 0u;
                return false;
        }
    }

    private static uint GetMetadataTokenPrefix(string registrationKind)
    {
        return registrationKind switch
        {
            "type" => 0x02000000u,
            "field" => 0x04000000u,
            "method" => 0x06000000u,
            _ => throw new InvalidOperationException(
                $"native-reference emitter does not know how to derive metadata token for registration kind '{registrationKind}'"),
        };
    }

    private static string FormatCppTokenLiteral(uint token)
    {
        return $"0x{token:X8}u";
    }

    private static string ToCppStringLiteral(string value)
    {
        var builder = new StringBuilder();
        builder.Append('"');

        foreach (var current in value)
        {
            builder.Append(current switch
            {
                '\\' => "\\\\",
                '"' => "\\\"",
                '\r' => "\\r",
                '\n' => "\\n",
                '\t' => "\\t",
                _ => current.ToString(),
            });
        }

        builder.Append('"');
        return builder.ToString();
    }

    private static TypedIlMethodArtifact GetRequiredMethod(
        IReadOnlyList<TypedIlMethodArtifact> methods,
        string subjectId)
    {
        var methodLookup = GetMethodLookup(methods);
        if (methodLookup.TryGetValue(subjectId, out var method))
        {
            return method;
        }

        throw new InvalidOperationException($"missing required typed-il method for '{subjectId}'");
    }

    private static IReadOnlyDictionary<string, TypedIlMethodArtifact> GetMethodLookup(
        IReadOnlyList<TypedIlMethodArtifact> methods)
    {
        return MethodLookupCache.GetValue(
            methods,
            static items => items.ToDictionary(method => method.SubjectId, StringComparer.Ordinal));
    }

    private static IReadOnlyDictionary<string, CodeRegistrationEntry> GetCodeRegistrationLookup(
        IReadOnlyList<CodeRegistrationEntry> registrations)
    {
        return CodeRegistrationLookupCache.GetValue(
            registrations,
            static items => items.ToDictionary(registration => registration.SubjectId, StringComparer.Ordinal));
    }

    private static MetadataRegistrationLookup GetMetadataRegistrationLookup(
        IReadOnlyList<MetadataRegistrationEntry> registrations)
    {
        return MetadataRegistrationLookupCache.GetValue(
            registrations,
            static items => new MetadataRegistrationLookup(items));
    }

    private static IReadOnlyList<MetadataRegistrationEntry> GetMetadataRegistrationsForKind(
        IReadOnlyList<MetadataRegistrationEntry> registrations,
        string registrationKind)
    {
        return GetMetadataRegistrationLookup(registrations).GetRegistrationsForKind(registrationKind);
    }

    private static CodeRegistrationEntry GetRequiredRegistration(
        IReadOnlyList<CodeRegistrationEntry> registrations,
        string subjectId)
    {
        var registrationLookup = GetCodeRegistrationLookup(registrations);
        if (registrationLookup.TryGetValue(subjectId, out var registration))
        {
            return registration;
        }

        throw new InvalidOperationException($"missing required method pointer registration for '{subjectId}'");
    }

    private static T LoadRequiredJson<T>(string path)
    {
        if (!File.Exists(path))
        {
            throw new FileNotFoundException("required Stage 4 input artifact is missing", path);
        }

        var value = JsonSerializer.Deserialize<T>(File.ReadAllText(path), JsonOptions);
        if (value is null)
        {
            throw new InvalidOperationException($"failed to deserialize required JSON artifact: {path}");
        }

        return value;
    }
}


