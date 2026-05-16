using System.Text;
using System.Text.Json;
using System.Runtime.CompilerServices;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceProofEmitter
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

    private sealed record MetadataTypeIdentity(
        string AssemblyName,
        string NamespaceName,
        string TypeName,
        string DisplayName,
        string SubjectId);

    private sealed record ExternalMetadataTokenResolution(
        string AssemblyName,
        uint TypeToken,
        uint MethodToken);

    private sealed class AssemblyMetadataTokenResolver
    {
        private readonly IReadOnlyDictionary<string, string> assemblyPathsByName;
        private readonly Dictionary<string, AssemblyMetadataCache> cacheByAssemblyName = new(StringComparer.Ordinal);
        private readonly object cacheGate = new();

        public AssemblyMetadataTokenResolver(IReadOnlyDictionary<string, string> assemblyPathsByName)
        {
            this.assemblyPathsByName = assemblyPathsByName;
        }

        public bool TryResolveMethodAndDeclaringTypeToken(
            string assemblyName,
            string methodSubjectId,
            out ExternalMetadataTokenResolution resolution)
        {
            resolution = null!;
            if (!assemblyPathsByName.TryGetValue(assemblyName, out var assemblyPath))
            {
                return false;
            }

            AssemblyMetadataCache cache;
            lock (cacheGate)
            {
                if (!cacheByAssemblyName.TryGetValue(assemblyName, out cache!))
                {
                    if (!TryCreateAssemblyMetadataCache(assemblyName, assemblyPath, out cache))
                    {
                        return false;
                    }

                    cacheByAssemblyName.Add(assemblyName, cache);
                }
            }

            return cache.TryResolveMethodAndDeclaringTypeToken(methodSubjectId, out resolution);
        }
    }

    private sealed class AssemblyMetadataCache
    {
        private readonly string assemblyName;
        private readonly FileStream metadataStream;
        private readonly PEReader peReader;
        private readonly MetadataReader metadataReader;
        private readonly Dictionary<string, TypeDefinitionHandle> typeDefinitionHandlesBySubjectId;
        private readonly Dictionary<string, ExternalMetadataTokenResolution> resolutionsByMethodSubjectId =
            new(StringComparer.Ordinal);
        private readonly object resolutionGate = new();

        public AssemblyMetadataCache(
            string assemblyName,
            FileStream metadataStream,
            PEReader peReader,
            MetadataReader metadataReader,
            Dictionary<string, TypeDefinitionHandle> typeDefinitionHandlesBySubjectId)
        {
            this.assemblyName = assemblyName;
            this.metadataStream = metadataStream;
            this.peReader = peReader;
            this.metadataReader = metadataReader;
            this.typeDefinitionHandlesBySubjectId = typeDefinitionHandlesBySubjectId;
        }

        public bool TryResolveMethodAndDeclaringTypeToken(
            string methodSubjectId,
            out ExternalMetadataTokenResolution resolution)
        {
            lock (resolutionGate)
            {
                if (resolutionsByMethodSubjectId.TryGetValue(methodSubjectId, out resolution!))
                {
                    return true;
                }

                var declaringTypeSubjectId = GetDeclaringTypeSubjectId(methodSubjectId);
                if (!typeDefinitionHandlesBySubjectId.TryGetValue(declaringTypeSubjectId, out var typeDefinitionHandle))
                {
                    resolution = null!;
                    return false;
                }

                var metadataMethodName = GetMetadataMethodName(methodSubjectId);
                var parameterCount = GetMethodParameterTypesFromSubjectId(methodSubjectId).Count;
                var typeDefinition = metadataReader.GetTypeDefinition(typeDefinitionHandle);
                foreach (var candidateHandle in typeDefinition.GetMethods())
                {
                    var candidateDefinition = metadataReader.GetMethodDefinition(candidateHandle);
                    if (!string.Equals(
                            metadataReader.GetString(candidateDefinition.Name),
                            metadataMethodName,
                            StringComparison.Ordinal) ||
                        GetMethodParameterCount(metadataReader, candidateDefinition) != parameterCount)
                    {
                        continue;
                    }

                    resolution = new ExternalMetadataTokenResolution(
                        assemblyName,
                        unchecked((uint)MetadataTokens.GetToken(typeDefinitionHandle)),
                        unchecked((uint)MetadataTokens.GetToken(candidateHandle)));
                    resolutionsByMethodSubjectId[methodSubjectId] = resolution;
                    return true;
                }

                resolution = null!;
                return false;
            }
        }
    }

    private sealed record RuntimeSkeletonStubBuildContext(
        NativeReferenceLoweringPlanArtifact LoweringPlan,
        ManagedClosureManifestArtifact ClosureManifest,
        AssemblyMetadataTokenResolver ExternalMetadataTokenResolver,
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

    private delegate RuntimeSkeletonFamilyHandlerResult RuntimeSkeletonConvertLikeFamilyHandler(
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

    private static readonly RuntimeSkeletonConvertLikeFamilyHandler[] RuntimeSkeletonConvertLikeFamilyHandlers =
    [
        TryBuildRuntimeSkeletonConvertLikeIntForwarderHandler,
        TryBuildRuntimeSkeletonConvertLikeBoolIdentityForwarderHandler,
        TryBuildRuntimeSkeletonConvertLikeBoolProducerForwarderHandler,
        TryBuildRuntimeSkeletonConvertLikeStringProviderPassthroughHandler,
        TryBuildRuntimeSkeletonConvertLikeStringCharProviderHandler,
        TryBuildRuntimeSkeletonConvertLikeObjectCharProviderHandler,
        TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeStringInstanceCallHandler,
        TryBuildRuntimeSkeletonConvertLikeStaticStringCallForwarderHandler,
        TryBuildRuntimeSkeletonConvertLikePrimitiveHandler,
        TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeCharInvalidCastHandler,
        TryBuildRuntimeSkeletonConvertLikeBoxedIConvertibleCharInvalidCastHandler,
        TryBuildRuntimeSkeletonConvertLikeCheckedPrimitiveHandler,
        TryBuildRuntimeSkeletonConvertLikeByteForwarderHandler,
        TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeInstanceCallHandler,
        TryBuildRuntimeSkeletonConvertLikeStaticValueTypeCallForwarderHandler,
        TryBuildRuntimeSkeletonConvertLikeInstanceValueTypeCallForwarderHandler,
        TryBuildRuntimeSkeletonConvertLikeValueTypeIdentityForwarderHandler,
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

    private const string RuntimeSkeletonConvertStringCharProviderCoreLibSubjectId =
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)";

    private const string RuntimeSkeletonConvertStringProviderPassthroughCoreLibSubjectId =
        "System.Private.CoreLib/System.Convert::ToString:System.String(System.String,System.IFormatProvider)";

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
        var closureManifestPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.ClosureManifest);
        var loweringPlan = LoadRequiredJson<NativeReferenceLoweringPlanArtifact>(loweringPlanPath);
        var closureManifest = LoadRequiredJson<ManagedClosureManifestArtifact>(closureManifestPath);
        var externalMetadataTokenResolver = new AssemblyMetadataTokenResolver(BuildAssemblyPathsByName(closureManifest));
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
                closureManifest,
                externalMetadataTokenResolver,
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

        // Validate generated C++ code against project coding conventions.
        var validator = new Validation.NativeCodegenValidator();
        foreach (var generatedSource in generatedSources)
        {
            var result = validator.ValidateContent(generatedSource.Contents, generatedSource.RelativePath);
            if (!result.IsValid)
            {
                foreach (var error in result.Errors)
                {
                    System.Console.Error.WriteLine($"[NativeCodegenValidator] {generatedSource.RelativePath}: ERROR: {error}");
                }
            }
            foreach (var warning in result.Warnings)
            {
                System.Console.Error.WriteLine($"[NativeCodegenValidator] {generatedSource.RelativePath}: WARNING: {warning}");
            }
        }

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
        ManagedClosureManifestArtifact closureManifest,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
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
                    closureManifest,
                    externalMetadataTokenResolver,
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
                    emittedMethodCount,
                    codeRegistration),
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
        int emittedMethodCount,
        CodeRegistrationArtifact codeRegistration)
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
CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL {pageDispatchName}(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    CHAOS_IL2CPP_UINT32 method_slot,
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
        var typeCapabilityEntries = codeRegistration.TypeCapabilities
            .Select(capability =>
                $"    {{ {FormatCppTokenLiteral(capability.TypeToken)}, {{ sizeof(RuntimeTypeCapabilityInfoV0), {capability.CapabilityBits}u, {capability.ValueSizeBytes}u, {capability.VectorWidthBytes}u, {capability.VectorLaneCount}u, {capability.VectorLaneKind}u, {capability.ScalarKind}u }} }},")
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
            ["has_type_capability_entries"] = typeCapabilityEntries.Length > 0,
            ["type_capability_entries"] = typeCapabilityEntries,
            ["type_capability_entry_count"] = typeCapabilityEntries.Length,
            ["native_entry_function_name"] = loweringPlan.NativeEntryFunctionName,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.AssemblyFullClosureRuntimeSkeletonSummaryTemplateRelativePath),
            model);
    }

    private static RuntimeSkeletonPageEmission BuildAssemblyFullClosureRuntimeSkeletonPageTranslationUnit(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        ManagedClosureManifestArtifact closureManifest,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
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
                closureManifest,
                externalMetadataTokenResolver,
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
                stubDefinition = BuildAssemblyFullClosureRuntimeSkeletonFallbackStubDefinition(stubName);
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
                ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.AssemblyFullClosureRuntimeSkeletonPageTemplateRelativePath),
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
        var canonicalSubjectIds = BuildCanonicalSubjectIdLookup(loweringPlan.TranslationUnitMethodSubjectIds ?? []);
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
            ["uncoveredMethodSubjectIds"] = unsupportedMethods
                .Select(method => ResolveCanonicalSubjectId(canonicalSubjectIds, method.SubjectId))
                .ToArray(),
            ["uncoveredMethods"] = unsupportedMethods.Select(method => new Dictionary<string, object?>
            {
                ["subjectId"] = method.SubjectId,
                ["canonicalSubjectId"] = ResolveCanonicalSubjectId(canonicalSubjectIds, method.SubjectId),
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
        return $@"CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL {stubName}(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);";
    }

    private static string BuildAssemblyFullClosureRuntimeSkeletonFallbackStubDefinition(string stubName)
    {
        return $@"CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL {stubName}(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args)
{{
    (void)bridge;
    (void)code_registration;
    (void)metadata_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    (void)managed_args;
    return CHAOS_BRIDGE_STATUS_NOT_SUPPORTED;
}}";
    }

    private static string? TryBuildAssemblyFullClosureRuntimeSkeletonMethodStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        ManagedClosureManifestArtifact closureManifest,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        string subjectId)
    {
        if (subjectId.Contains("/System.Number::", StringComparison.Ordinal) &&
            TryBuildAssemblyBoundNumberManagedInvokeCore(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodsBySubjectId,
                stubName,
                out var numberManagedInvokeStub))
        {
            return numberManagedInvokeStub;
        }

        if (subjectId.Contains("/System.Enum::", StringComparison.Ordinal) &&
            TryBuildAssemblyBoundEnumManagedInvokeCore(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodPointers,
                methodsBySubjectId,
                pageSupportBuilder,
                stubName,
                out var enumManagedInvokeStub))
        {
            return enumManagedInvokeStub;
        }

        var buildContext = new RuntimeSkeletonStubBuildContext(
            loweringPlan,
            closureManifest,
            externalMetadataTokenResolver,
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
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertFamilyCore(buildContext);

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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonInteropKernel32PlatformCapabilityFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonInteropKernel32PlatformCapabilityFamilyCore(buildContext);

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
        return TryBuildRuntimeSkeletonConvertLikeHandlers(buildContext, RuntimeSkeletonFamilyHandlerResult.NoMatch);
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonThreadingAtomicKernelFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonThreadingAtomicKernelFamilyCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonThreadingTaskManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonThreadingTaskManagedInvokeFamilyCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonThreadingSyncManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonThreadingSyncManagedInvokeFamilyCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonThreadingThreadManagedInvokeFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonThreadingThreadManagedInvokeFamilyCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeHandlers(
        RuntimeSkeletonStubBuildContext buildContext,
        RuntimeSkeletonFamilyHandlerResult missResult)
    {
        foreach (var convertLikeFamilyHandler in RuntimeSkeletonConvertLikeFamilyHandlers)
        {
            var result = convertLikeFamilyHandler(buildContext);
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
        if (buildContext.SubjectId.Contains("/System.String::", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonTaskContinuationFamilyHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonTaskContinuationFamilyCore(buildContext);

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

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeIntForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeIntForwarderCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoolIdentityForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoolIdentityForwarderCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoolProducerForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoolProducerForwarderCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikePrimitiveHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikePrimitiveCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStringCharProviderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeStringCharProviderCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStringProviderPassthroughHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeStringProviderPassthroughCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeObjectCharProviderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeObjectCharProviderCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeStringInstanceCallHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeStringInstanceCallCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeInstanceCallHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeInstanceCallCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStaticStringCallForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeStaticStringCallForwarderCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeStaticValueTypeCallForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeStaticValueTypeCallForwarderCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeInstanceValueTypeCallForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeInstanceValueTypeCallForwarderCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeValueTypeIdentityForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeValueTypeIdentityForwarderCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeCharInvalidCastHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoxedValueTypeCharInvalidCastCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeBoxedIConvertibleCharInvalidCastHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeBoxedIConvertibleCharInvalidCastCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeCheckedPrimitiveHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeCheckedPrimitiveCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConvertLikeByteForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConvertLikeByteForwarderCore(buildContext);

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
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringReturnForwarderCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringForwarderHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringForwarderCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringLiteralAppendHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringLiteralAppendCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringForwarderConsoleWriteLineCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticLiteralStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticLiteralStringReturnCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticResourceKeyStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticResourceKeyStringReturnCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerConsoleWriteLineCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableCore(buildContext);

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableCore(buildContext);

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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticCallCtorGetterStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonConstructorThenInstanceCallStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonDelegateClosedTargetRelayStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonArrayBoxingReferenceArrayStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonArrayCopyReferenceArrayStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonArrayClearReferenceArrayStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonArrayReverseReferenceArrayStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonInterfaceDispatchMessageStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonThreadingThreadStaticMonitorStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonExceptionThrowCatchFinallyStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonNestedExceptionThrowCatchFinallyStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonMarshalingUtf8ExportStubTemplateRelativePath),
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
            ["parameter_attributes"] = loweringPlan.ParameterAttributes ?? 0,
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonReflectionInteropClosureStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonPInvokeDirectCallStubTemplateRelativePath),
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
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.MarshalingUtf8ExportMinimal),
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
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.ReflectionInteropClosureMinimal),
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
            ParameterAttributes = parameterRegistration.ParameterAttributes,
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
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.InteropPInvokeDirectCallMinimal),
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
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.ManagedThreadingThreadStaticMonitorMinimal),
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
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.ExceptionThrowCatchFinallyMinimal),
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
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.NestedExceptionThrowCatchFinallyMinimal),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonAsyncTaskFactoryStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonAsyncGetResultStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonAsyncStateMachineNoOpStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticVoidObjectSinkNoOpStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticExceptionThrowLiteralStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticExceptionThrowStringProducerStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticExceptionCatchStringReturnStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticIntForwarderStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticIntForwarderStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticBoolIdentityForwarderStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticBoolProducerForwarderStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticPrimitiveConvertStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundConvertRuntimeHelperStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundConvertRuntimeHelperCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundStringManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundStringManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundUnsafeManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundUnsafeManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundMemoryExtensionsManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundMemoryExtensionsManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundCollectionsManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundCollectionsManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundSpanHelpersKernelStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundSpanHelpersKernelCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundValueTypeKernelStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundValueTypeKernelCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundVectorKernelStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundVectorKernelCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundArrayAndMemoryMarshalStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundArrayAndMemoryMarshalCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundHalfManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundHalfManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundFloatingScalarManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundFloatingScalarManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundPrimitiveScalarManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundPrimitiveScalarManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundWideNumericManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundWideNumericManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundCalendarStructManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundCalendarStructManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundIdentityStructManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundIdentityStructManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundCompanionManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundCompanionManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundDateTimeSemanticEngineStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundDateTimeSemanticEngineCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundGlobalizationDateTimeSupportStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundGlobalizationDateTimeSupportCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundNumberManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundNumberManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundDecimalManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundDecimalManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundCharManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundCharManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodsBySubjectId,
            stubName,
            out stub);

    private static bool TryBuildAssemblyBoundEnumManagedInvokeStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName,
        out string stub) =>
        TryBuildAssemblyBoundEnumManagedInvokeCore(
            assemblyName,
            subjectId,
            metadataRegistration,
            methodPointers,
            methodsBySubjectId,
            pageSupportBuilder,
            stubName,
            out stub);

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

        if (!TryResolveRuntimeSkeletonCheckedPrimitiveConvertShape(
                method.SubjectId,
                method.Parameters[0].Type,
                outputManagedType,
                instructions,
                out var inputCppType,
                out var overflowConditionExpression,
                out var throwSubjectId))
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticCheckedPrimitiveConvertStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticValueTypeIdentityForwarderStub(
        string subjectId,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
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
            !string.Equals(GetMethodReturnType(method.SubjectId), outputManagedType, StringComparison.Ordinal) ||
            !string.Equals(method.Parameters[0].Type, outputManagedType, StringComparison.Ordinal))
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

        if (instructions.Count != 2 ||
            !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[0]) != 0 ||
            !string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonValueTypeByValueInputCppType(outputManagedType, out var outputCppType, out _))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["output_cpp_type"] = outputCppType,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticValueTypeIdentityForwarderStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticBoxedIConvertibleCharInvalidCastStub(
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

        if (!TryResolveRuntimeSkeletonBoxedIConvertibleCharInvalidCastShape(
                method,
                methodsBySubjectId,
                out var inputCppType,
                out var outputCppType,
                out var exceptionTypeSubjectId,
                out var sourceTypeName,
                out var targetTypeName))
        {
            return false;
        }

        var exceptionMessage = $"InvalidCast_FromTo: {sourceTypeName} -> {targetTypeName}";
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["input_cpp_type"] = inputCppType,
            ["output_cpp_type"] = outputCppType,
            ["exception_type_token"] = CreateTypeTokenLiteral(metadataRegistration, exceptionTypeSubjectId),
            ["source_type_name_literal"] = ToCppStringLiteral(sourceTypeName),
            ["target_type_name_literal"] = ToCppStringLiteral(targetTypeName),
            ["exception_message_literal"] = ToCppStringLiteral(exceptionMessage),
            ["exception_message_byte_count"] = Encoding.UTF8.GetByteCount(exceptionMessage),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticBoxedIConvertibleCharInvalidCastStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticBoxedValueTypeCharInvalidCastStub(
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

        if (!TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastShape(
                method,
                methodsBySubjectId,
                out var inputCppType,
                out var outputCppType,
                out var boxedValueTypeSubjectId,
                out var exceptionTypeSubjectId,
                out var sourceTypeName,
                out var targetTypeName,
                out var inputSize))
        {
            return false;
        }

        var exceptionMessage = $"InvalidCast_FromTo: {sourceTypeName} -> {targetTypeName}";
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["input_cpp_type"] = inputCppType,
            ["output_cpp_type"] = outputCppType,
            ["boxed_value_type_token"] = CreateTypeTokenLiteral(metadataRegistration, boxedValueTypeSubjectId),
            ["exception_type_token"] = CreateTypeTokenLiteral(metadataRegistration, exceptionTypeSubjectId),
            ["source_type_name_literal"] = ToCppStringLiteral(sourceTypeName),
            ["target_type_name_literal"] = ToCppStringLiteral(targetTypeName),
            ["exception_message_literal"] = ToCppStringLiteral(exceptionMessage),
            ["exception_message_byte_count"] = Encoding.UTF8.GetByteCount(exceptionMessage),
            ["input_size"] = inputSize,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticBoxedValueTypeCharInvalidCastStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringCharProviderStub(
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

        if (!TryResolveRuntimeSkeletonStringCharProviderShape(
                method,
                out var targetMethodSubjectId))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId)),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringCharProviderStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringProviderPassthroughStub(
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

        if (!TryResolveRuntimeSkeletonStringProviderPassthroughShape(method))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProviderPassthroughStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticObjectCharProviderStub(
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

        if (!TryResolveRuntimeSkeletonObjectCharProviderShape(
                method,
                out var targetMethodSubjectId))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId)),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticObjectCharProviderStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticBoxedValueTypeStringInstanceCallStub(
        string assemblyName,
        string subjectId,
        ManagedClosureManifestArtifact closureManifest,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
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

        if (!TryResolveRuntimeSkeletonBoxedValueTypeStringInstanceCallShape(
                method,
                methodsBySubjectId,
                out var inputCppType,
                out var boxedValueTypeSubjectId,
                out var targetMethodSubjectId,
                out var targetAssemblyName,
                out var inputSize,
                out var argCount))
        {
            return false;
        }

        string boxedValueTypeTokenLiteral;
        string targetMethodTokenLiteral;
        string targetAssemblyNameLiteral;
        if (string.Equals(targetAssemblyName, assemblyName, StringComparison.Ordinal))
        {
            boxedValueTypeTokenLiteral = CreateTypeTokenLiteral(metadataRegistration, boxedValueTypeSubjectId);
            targetMethodTokenLiteral = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId));
            targetAssemblyNameLiteral = ToCppStringLiteral(targetAssemblyName);
        }
        else if (externalMetadataTokenResolver.TryResolveMethodAndDeclaringTypeToken(
                     targetAssemblyName,
                     targetMethodSubjectId,
                     out var externalResolution))
        {
            boxedValueTypeTokenLiteral = FormatCppTokenLiteral(externalResolution.TypeToken);
            targetMethodTokenLiteral = FormatCppTokenLiteral(externalResolution.MethodToken);
            targetAssemblyNameLiteral = ToCppStringLiteral(externalResolution.AssemblyName);
        }
        else
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_assembly_name_literal"] = targetAssemblyNameLiteral,
            ["input_cpp_type"] = inputCppType,
            ["boxed_value_type_token"] = boxedValueTypeTokenLiteral,
            ["target_method_token"] = targetMethodTokenLiteral,
            ["input_size"] = inputSize,
            ["argc"] = argCount,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticBoxedValueTypeStringInstanceCallStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticBoxedValueTypeInstanceCallStub(
        string assemblyName,
        string subjectId,
        ManagedClosureManifestArtifact closureManifest,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        string outputManagedType,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonBoxedValueTypeInstanceCallShape(
                method,
                methodsBySubjectId,
                outputManagedType,
                out var inputCppType,
                out var outputCppType,
                out var boxedValueTypeSubjectId,
                out var targetMethodSubjectId,
                out var targetAssemblyName,
                out var inputSize,
                out var argCount))
        {
            return false;
        }

        string boxedValueTypeTokenLiteral;
        string targetMethodTokenLiteral;
        string targetAssemblyNameLiteral;
        if (string.Equals(targetAssemblyName, assemblyName, StringComparison.Ordinal))
        {
            boxedValueTypeTokenLiteral = CreateTypeTokenLiteral(metadataRegistration, boxedValueTypeSubjectId);
            targetMethodTokenLiteral = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId));
            targetAssemblyNameLiteral = ToCppStringLiteral(targetAssemblyName);
        }
        else if (externalMetadataTokenResolver.TryResolveMethodAndDeclaringTypeToken(
                     targetAssemblyName,
                     targetMethodSubjectId,
                     out var externalResolution))
        {
            boxedValueTypeTokenLiteral = FormatCppTokenLiteral(externalResolution.TypeToken);
            targetMethodTokenLiteral = FormatCppTokenLiteral(externalResolution.MethodToken);
            targetAssemblyNameLiteral = ToCppStringLiteral(externalResolution.AssemblyName);
        }
        else
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_assembly_name_literal"] = targetAssemblyNameLiteral,
            ["input_cpp_type"] = inputCppType,
            ["output_cpp_type"] = outputCppType,
            ["boxed_value_type_token"] = boxedValueTypeTokenLiteral,
            ["target_method_token"] = targetMethodTokenLiteral,
            ["input_size"] = inputSize,
            ["argc"] = argCount,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticBoxedValueTypeInstanceCallStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticStringCallForwarderStub(
        string assemblyName,
        string subjectId,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
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
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal) ||
            method.Parameters.Count < 1 ||
            method.Parameters.Count > 2)
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

        int callIndex;
        if (method.Parameters.Count == 1)
        {
            if (instructions.Count != 3 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            callIndex = 1;
        }
        else
        {
            if (instructions.Count != 4 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 1 ||
                !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            callIndex = 2;
        }

        var targetMethodSubjectId = GetRequiredInstructionCallee(
            instructions[callIndex], method.SubjectId, callIndex);
        if (string.Equals(targetMethodSubjectId, subjectId, StringComparison.Ordinal))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.String", StringComparison.Ordinal))
        {
            return false;
        }

        // Only match System.Convert methods themselves or methods that forward to System.Convert::ToString
        if (!subjectId.Contains("/System.Convert::", StringComparison.Ordinal) &&
            !targetMethodSubjectId.Contains("/System.Convert::", StringComparison.Ordinal))
        {
            return false;
        }

        string targetAssemblyName;
        string targetMethodTokenLiteral;
        var targetMethodAssemblyName = GetAssemblyNameFromSubjectId(targetMethodSubjectId);
        try
        {
            if (string.Equals(targetMethodAssemblyName, assemblyName, StringComparison.Ordinal))
            {
                targetAssemblyName = assemblyName;
                targetMethodTokenLiteral = FormatCppTokenLiteral(
                    GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId));
            }
            else if (externalMetadataTokenResolver.TryResolveMethodAndDeclaringTypeToken(
                         targetMethodAssemblyName,
                         targetMethodSubjectId,
                         out var externalResolution))
            {
                targetAssemblyName = externalResolution.AssemblyName;
                targetMethodTokenLiteral = FormatCppTokenLiteral(externalResolution.MethodToken);
            }
            else
            {
                return false;
            }
        }
        catch
        {
            return false;
        }

        var argFieldDeclarations = new List<string>(method.Parameters.Count);
        var argRefs = new List<string>(method.Parameters.Count);
        foreach (var parameter in method.Parameters)
        {
            var argIndex = argFieldDeclarations.Count;
            if (TryResolveRuntimeSkeletonValueTypeByValueInputCppType(parameter.Type, out var cppType, out _))
            {
                argFieldDeclarations.Add($"{cppType} arg{argIndex};");
            }
            else
            {
                argFieldDeclarations.Add($"void* arg{argIndex};");
            }

            argRefs.Add($"const_cast<void*>(reinterpret_cast<const void*>(&request->arg{argIndex}))");
        }

        var paramCount = method.Parameters.Count;
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_assembly_name_literal"] = ToCppStringLiteral(targetAssemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["arg_field_declarations"] = string.Join("\n    ", argFieldDeclarations),
            ["argc"] = paramCount,
            ["arg_refs"] = string.Join(",\n        ", argRefs),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringCallForwarderStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticValueTypeCallForwarderStub(
        string assemblyName,
        string subjectId,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        string outputManagedType,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonStaticValueTypeCallShape(
                method,
                outputManagedType,
                out var targetMethodSubjectId,
                out var targetAssemblyName,
                out var argFieldDeclarations,
                out var argRefs,
                out var argCount))
        {
            return false;
        }

        string targetMethodTokenLiteral;
        string resolvedTargetAssemblyName;
        try
        {
            if (string.Equals(targetAssemblyName, assemblyName, StringComparison.Ordinal))
            {
                resolvedTargetAssemblyName = assemblyName;
                targetMethodTokenLiteral = FormatCppTokenLiteral(
                    GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId));
            }
            else if (externalMetadataTokenResolver.TryResolveMethodAndDeclaringTypeToken(
                         targetAssemblyName,
                         targetMethodSubjectId,
                         out var externalResolution))
            {
                resolvedTargetAssemblyName = externalResolution.AssemblyName;
                targetMethodTokenLiteral = FormatCppTokenLiteral(externalResolution.MethodToken);
            }
            else
            {
                return false;
            }
        }
        catch
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonValueTypeByValueInputCppType(outputManagedType, out var outputCppType, out _))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_assembly_name_literal"] = ToCppStringLiteral(resolvedTargetAssemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["arg_field_declarations"] = string.Join("\n    ", argFieldDeclarations),
            ["argc"] = argCount,
            ["arg_refs"] = string.Join(",\n        ", argRefs),
            ["output_cpp_type"] = outputCppType,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticValueTypeCallForwarderStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundInstanceValueTypeCallForwarderStub(
        string assemblyName,
        string subjectId,
        AssemblyMetadataTokenResolver externalMetadataTokenResolver,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        string outputManagedType,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonInstanceValueTypeCallShape(
                method,
                outputManagedType,
                out var targetMethodSubjectId,
                out var targetAssemblyName,
                out var hasNullArg,
                out var argFieldDeclarations,
                out var argRefs,
                out var argCount))
        {
            return false;
        }

        string targetMethodTokenLiteral;
        string resolvedTargetAssemblyName;
        try
        {
            if (string.Equals(targetAssemblyName, assemblyName, StringComparison.Ordinal))
            {
                resolvedTargetAssemblyName = assemblyName;
                targetMethodTokenLiteral = FormatCppTokenLiteral(
                    GetRequiredMetadataToken(metadataRegistration, "method", targetMethodSubjectId));
            }
            else if (externalMetadataTokenResolver.TryResolveMethodAndDeclaringTypeToken(
                         targetAssemblyName,
                         targetMethodSubjectId,
                         out var externalResolution))
            {
                resolvedTargetAssemblyName = externalResolution.AssemblyName;
                targetMethodTokenLiteral = FormatCppTokenLiteral(externalResolution.MethodToken);
            }
            else
            {
                return false;
            }
        }
        catch
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonValueTypeByValueInputCppType(outputManagedType, out var outputCppType, out _))
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["target_assembly_name_literal"] = ToCppStringLiteral(resolvedTargetAssemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["arg_field_declarations"] = string.Join("\n    ", argFieldDeclarations),
            ["argc"] = argCount,
            ["arg_refs"] = string.Join(",\n        ", argRefs),
            ["output_cpp_type"] = outputCppType,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonInstanceValueTypeCallForwarderStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticByteForwarderStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringForwarderStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringLiteralAppendStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringReturnForwarderStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringForwarderConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerForwarderConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerCtorGetterConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerCtorRenderConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerForwarderCtorInstanceCallForwarderForwarderForwarderConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticLiteralStringReturnStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticLiteralStringReturnStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticStringProducerConsoleWriteLineStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonConstructorFieldSetterStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonFieldBackedStringReturnStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonFieldArgumentStringReturnStubTemplateRelativePath),
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
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonFieldGetterStringReturnStubTemplateRelativePath),
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

    private static IReadOnlyDictionary<string, string> BuildAssemblyPathsByName(ManagedClosureManifestArtifact closureManifest)
    {
        var pathsByAssemblyName = new Dictionary<string, string>(StringComparer.Ordinal);
        foreach (var resolvedAssembly in closureManifest.ResolvedAssemblies ?? [])
        {
            if (!string.IsNullOrWhiteSpace(resolvedAssembly.AssemblyName) &&
                !string.IsNullOrWhiteSpace(resolvedAssembly.Path))
            {
                pathsByAssemblyName[resolvedAssembly.AssemblyName] = Path.GetFullPath(resolvedAssembly.Path);
            }
        }

        if (pathsByAssemblyName.Count > 0)
        {
            return pathsByAssemblyName;
        }

        pathsByAssemblyName[closureManifest.AssemblyName] = Path.GetFullPath(closureManifest.InputAssemblyPath);

        foreach (var additionalAssemblyPath in closureManifest.AdditionalAssemblyPaths ?? [])
        {
            var normalizedPath = Path.GetFullPath(additionalAssemblyPath);
            var assemblyName = Path.GetFileNameWithoutExtension(normalizedPath);
            if (!string.IsNullOrWhiteSpace(assemblyName))
            {
                pathsByAssemblyName[assemblyName] = normalizedPath;
            }
        }

        return pathsByAssemblyName;
    }

    private static bool TryCreateAssemblyMetadataCache(
        string assemblyName,
        string assemblyPath,
        out AssemblyMetadataCache cache)
    {
        cache = null!;
        var stream = File.OpenRead(assemblyPath);
        var peReader = new PEReader(stream);
        if (!peReader.HasMetadata)
        {
            peReader.Dispose();
            stream.Dispose();
            return false;
        }

        var metadataReader = peReader.GetMetadataReader();
        var typeDefinitionHandlesBySubjectId = new Dictionary<string, TypeDefinitionHandle>(StringComparer.Ordinal);
        foreach (var typeHandle in metadataReader.TypeDefinitions)
        {
            if (!TryResolveTypeDefinitionIdentity(metadataReader, assemblyName, typeHandle, out var typeIdentity))
            {
                continue;
            }

            typeDefinitionHandlesBySubjectId[typeIdentity.SubjectId] = typeHandle;
        }

        cache = new AssemblyMetadataCache(
            assemblyName,
            stream,
            peReader,
            metadataReader,
            typeDefinitionHandlesBySubjectId);
        return true;
    }

    private static bool TryResolveTypeDefinitionIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        TypeDefinitionHandle handle,
        out MetadataTypeIdentity typeIdentity)
    {
        typeIdentity = default!;
        var typeDefinition = metadataReader.GetTypeDefinition(handle);
        var namespaceName = metadataReader.GetString(typeDefinition.Namespace);
        var typeName = metadataReader.GetString(typeDefinition.Name);
        var declaringTypeHandle = typeDefinition.GetDeclaringType();
        if (!declaringTypeHandle.IsNil &&
            TryResolveTypeDefinitionIdentity(metadataReader, currentAssemblyName, declaringTypeHandle, out var declaringTypeIdentity))
        {
            var nestedTypeName = $"{declaringTypeIdentity.TypeName}+{typeName}";
            var nestedDisplayName = string.IsNullOrEmpty(namespaceName)
                ? nestedTypeName
                : $"{namespaceName}.{nestedTypeName}";
            typeIdentity = new MetadataTypeIdentity(
                currentAssemblyName,
                namespaceName,
                nestedTypeName,
                nestedDisplayName,
                $"{currentAssemblyName}/{nestedDisplayName}");
            return true;
        }

        var displayName = string.IsNullOrEmpty(namespaceName)
            ? typeName
            : $"{namespaceName}.{typeName}";
        typeIdentity = new MetadataTypeIdentity(
            currentAssemblyName,
            namespaceName,
            typeName,
            displayName,
            $"{currentAssemblyName}/{displayName}");
        return true;
    }

    private static int GetMethodParameterCount(MetadataReader metadataReader, MethodDefinition methodDefinition)
    {
        return methodDefinition.GetParameters()
            .Select(parameterHandle => metadataReader.GetParameter(parameterHandle))
            .Count(parameter => parameter.SequenceNumber > 0);
    }

    private static string GetMetadataMethodName(string methodSubjectId)
    {
        var methodName = GetMethodName(methodSubjectId);
        var genericArgumentIndex = methodName.IndexOf('<');
        return genericArgumentIndex >= 0
            ? methodName[..genericArgumentIndex]
            : methodName;
    }

    private static string GetAssemblyNameFromSubjectId(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf('/');
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException($"failed to extract assembly name from subject id '{subjectId}'");
        }

        return subjectId[..separatorIndex];
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
        var builder = new StringBuilder(value.Length + 2);
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

    private static bool TryRenderValueTypeManagedInvokeStub(
        Template template,
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        string stubName,
        string contractId,
        string thisFieldDeclaration,
        string thisValidationStatement,
        string thisArgumentExpression,
        IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> argumentShapes,
        string argumentValidationStatements,
        int argumentCount,
        int argumentStorageSize,
        RuntimeSkeletonManagedInvokeReturnContract returnShape,
        out string stub)
    {
        stub = string.Empty;
        string targetMethodTokenLiteral;
        try
        {
            targetMethodTokenLiteral = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", subjectId));
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = contractId,
            ["target_assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["this_field_declaration"] = thisFieldDeclaration,
            ["this_validation_statement"] = thisValidationStatement,
            ["this_argument_expression"] = thisArgumentExpression,
            ["arg_field_declarations"] = string.Join("\n    ", argumentShapes.Select(shape => shape.FieldDeclaration)),
            ["arg_validation_statements"] = argumentValidationStatements,
            ["arg_refs"] = string.Join(",\n        ", argumentShapes.Select(shape => shape.ArgReferenceExpression)),
            ["argc"] = argumentCount,
            ["arg_storage_size"] = argumentStorageSize,
            ["return_managed_type"] = returnShape.ManagedType,
            ["return_field_declarations"] = returnShape.FieldDeclarations,
            ["return_value_validation_statement"] = returnShape.ValidationStatement,
            ["return_value_declaration"] = returnShape.ReturnValueDeclaration,
            ["return_value_argument"] = returnShape.ReturnValueArgument,
            ["return_value_size"] = returnShape.ReturnValueSizeExpression,
            ["return_value_is_indirect"] = returnShape.ReturnValueIsIndirectExpression,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(template, model);
        return true;
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

    private static IReadOnlyDictionary<string, string> BuildCanonicalSubjectIdLookup(IReadOnlyList<string> subjectIds)
    {
        return subjectIds
            .Where(subjectId => !string.IsNullOrWhiteSpace(subjectId))
            .Distinct(StringComparer.Ordinal)
            .ToDictionary(
                subjectId => subjectId,
                ManagedNaming.CanonicalizeSubjectId,
                StringComparer.Ordinal);
    }

    private static string ResolveCanonicalSubjectId(
        IReadOnlyDictionary<string, string> canonicalSubjectIds,
        string subjectId)
    {
        return canonicalSubjectIds.TryGetValue(subjectId, out var canonicalSubjectId)
            ? canonicalSubjectId
            : ManagedNaming.CanonicalizeSubjectId(subjectId);
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


