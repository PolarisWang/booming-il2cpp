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

    private const string RuntimeSkeletonConvertStringCharProviderCoreLibSubjectId =
        "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)";

    private const string RuntimeSkeletonConvertStringProviderPassthroughCoreLibSubjectId =
        "System.Private.CoreLib/System.Convert::ToString:System.String(System.String,System.IFormatProvider)";

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
}

