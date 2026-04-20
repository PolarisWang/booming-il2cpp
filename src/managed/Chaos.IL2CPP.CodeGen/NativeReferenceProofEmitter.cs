using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed class NativeReferenceProofEmitter
{
    private const int AuditTranslationUnitPageSize = 1024;
    private const string ManagedAsyncAwaitIntMinimal = NativeReferenceProofCatalog.ManagedAsyncAwaitIntMinimal;
    private const string ManagedThreadingThreadStaticMonitorMinimal = NativeReferenceProofCatalog.ManagedThreadingThreadStaticMonitorMinimal;
    private const string ManagedInterfaceDispatchMessageMinimal = NativeReferenceProofCatalog.ManagedInterfaceDispatchMessageMinimal;
    private const string ManagedDispatchVirtualInstanceMessageMinimal = NativeReferenceProofCatalog.ManagedDispatchVirtualInstanceMessageMinimal;
    private const string ManagedObjectCapturedStateInstanceMessageMinimal = NativeReferenceProofCatalog.ManagedObjectCapturedStateInstanceMessageMinimal;
    private const string ManagedGenericStaticForwarderCapturedGetterMinimal = NativeReferenceProofCatalog.ManagedGenericStaticForwarderCapturedGetterMinimal;
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

    public NativeReferenceProofResult Generate(NativeReferenceProofRequest request)
    {
        var managedClosureRoot = Path.GetFullPath(request.ManagedClosureRootPath);
        var loweringPlanPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.NativeReferenceLoweringPlan);
        var loweringPlan = LoadRequiredJson<NativeReferenceLoweringPlanArtifact>(loweringPlanPath);
        IReadOnlyList<NativeReferenceGeneratedSource> generatedSources;
        if (string.Equals(loweringPlan.PlanKind, "assembly-full-closure-runtime-skeleton", StringComparison.Ordinal))
        {
            var typedIlPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.TypedIlIr);
            var metadataRegistrationPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.MetadataRegistration);
            var codeRegistrationPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.CodeRegistration);
            var typedIl = LoadRequiredJson<TypedIlIrArtifact>(typedIlPath);
            var metadataRegistration = LoadRequiredJson<MetadataRegistrationArtifact>(metadataRegistrationPath);
            var codeRegistration = LoadRequiredJson<CodeRegistrationArtifact>(codeRegistrationPath);
            generatedSources = BuildAssemblyFullClosureRuntimeSkeletonGeneratedSources(
                loweringPlan,
                typedIl,
                metadataRegistration,
                codeRegistration);
        }
        else if (string.Equals(loweringPlan.PlanKind, "assembly-full-closure-audit", StringComparison.Ordinal))
        {
            generatedSources = BuildAssemblyFullClosureAuditGeneratedSources(loweringPlan);
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
        }

        var manifest = new NativeReferenceProofManifestArtifact
        {
            AssemblyName = loweringPlan.AssemblyName,
            EntrySubjectId = loweringPlan.EntrySubjectId,
            ManagedClosureRootPath = ManagedNaming.NormalizePathForManifest(managedClosureRoot, Environment.CurrentDirectory),
            PlanArtifactPath = NativeReferenceArtifactNames.LoweringPlan,
            RuntimeExecutionKind = loweringPlan.RuntimeExecutionKind,
            PreferredAssemblyDispatchSubjectId = BuildPreferredAssemblyDispatchSubjectId(loweringPlan),
            TranslationUnitPageSize = loweringPlan.TranslationUnitPageSize,
            TranslationUnitPageCount = loweringPlan.TranslationUnitPageCount,
            TranslationUnitPages = loweringPlan.TranslationUnitPages,
            GeneratedArtifacts = generatedSources
                .Select(generatedSource => new NativeReferenceGeneratedArtifactRef
                {
                    Kind = "generatedTranslationUnit",
                    Path = generatedSource.RelativePath,
                })
                .ToList(),
        };

        return new NativeReferenceProofResult
        {
            OutputRootPath = request.OutputRootPath,
            LoweringPlan = loweringPlan,
            Manifest = manifest,
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

        var generatedSources = new List<NativeReferenceGeneratedSource>
        {
            new()
            {
                RelativePath = NativeReferenceArtifactNames.GeneratedTranslationUnit,
                Contents = BuildAssemblyFullClosureAuditSummaryTranslationUnit(loweringPlan),
            },
        };

        var methodSubjectIds = loweringPlan.TranslationUnitMethodSubjectIds ?? [];
        for (var pageIndex = 0; pageIndex * AuditTranslationUnitPageSize < methodSubjectIds.Count; pageIndex++)
        {
            var pageItems = methodSubjectIds
                .Skip(pageIndex * AuditTranslationUnitPageSize)
                .Take(AuditTranslationUnitPageSize)
                .ToList();
            generatedSources.Add(new NativeReferenceGeneratedSource
            {
                RelativePath = $"generated/audit/native-reference.methods.page-{pageIndex + 1:D4}.cpp",
                Contents = BuildAssemblyFullClosureAuditPageTranslationUnit(loweringPlan, pageIndex + 1, pageItems),
            });
        }

        return generatedSources;
    }

    private static IReadOnlyList<NativeReferenceGeneratedSource> BuildAssemblyFullClosureRuntimeSkeletonGeneratedSources(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        TypedIlIrArtifact typedIl,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationArtifact codeRegistration)
    {
        ValidateAssemblyFullClosureRuntimeSkeletonPlan(loweringPlan);

        var generatedSources = new List<NativeReferenceGeneratedSource>
        {
            new()
            {
                RelativePath = NativeReferenceArtifactNames.GeneratedTranslationUnit,
                Contents = BuildAssemblyFullClosureRuntimeSkeletonSummaryTranslationUnit(loweringPlan),
            },
        };

        foreach (var page in loweringPlan.TranslationUnitPages ?? [])
        {
            var pageItems = (loweringPlan.TranslationUnitMethodSubjectIds ?? [])
                .Skip((page.PageNumber - 1) * (loweringPlan.TranslationUnitPageSize ?? AuditTranslationUnitPageSize))
                .Take(page.MethodCount)
                .ToList();
            generatedSources.Add(new NativeReferenceGeneratedSource
            {
                RelativePath = page.Path,
                Contents = BuildAssemblyFullClosureRuntimeSkeletonPageTranslationUnit(
                    loweringPlan,
                    typedIl,
                    metadataRegistration,
                    codeRegistration,
                    page.PageNumber,
                    pageItems),
            });
        }

        return generatedSources;
    }

    private static string BuildAssemblyFullClosureAuditSummaryTranslationUnit(
        NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        ValidateAssemblyFullClosureAuditPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["plan_kind_literal"] = ToCppStringLiteral(loweringPlan.PlanKind),
            ["translation_unit_method_count"] = loweringPlan.TranslationUnitMethodCount ?? 0,
            ["translation_unit_page_size"] = loweringPlan.TranslationUnitPageSize ?? AuditTranslationUnitPageSize,
            ["translation_unit_page_count"] = loweringPlan.TranslationUnitPageCount
                ?? GetAuditPageCount(loweringPlan.TranslationUnitMethodSubjectIds?.Count ?? 0),
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetAssemblyFullClosureAuditSummaryTemplate(),
            model);
    }

    private static string BuildAssemblyFullClosureAuditPageTranslationUnit(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        int pageNumber,
        IReadOnlyList<string> pageItems)
    {
        var model = new ScriptObject
        {
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["page_number"] = pageNumber,
            ["page_item_count"] = pageItems.Count,
            ["method_subject_id_literals"] = pageItems.Select(ToCppStringLiteral).ToArray(),
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetAssemblyFullClosureAuditPageTemplate(),
            model);
    }

    private static string BuildAssemblyFullClosureRuntimeSkeletonSummaryTranslationUnit(
        NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        ValidateAssemblyFullClosureRuntimeSkeletonPlan(loweringPlan);
        var pages = loweringPlan.TranslationUnitPages ?? [];
        var pageDispatchDeclarations = pages
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
    const char* subject_id,
    void* managed_args);
""";
            })
            .ToArray();
        var pageDispatchCatalogEntries = pages
            .Select(page =>
            {
                var pageDispatchName = BuildAssemblyFullClosureRuntimeSkeletonPageDispatchFunctionName(page.PageNumber);
                return $"    {{ {page.PageNumber}, {ToCppStringLiteral(page.FirstMethodSubjectId ?? string.Empty)}, {ToCppStringLiteral(page.LastMethodSubjectId ?? string.Empty)}, &{pageDispatchName} }},";
            })
            .ToArray();
        var model = new ScriptObject
        {
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["plan_kind_literal"] = ToCppStringLiteral(loweringPlan.PlanKind),
            ["runtime_execution_kind_literal"] = ToCppStringLiteral(loweringPlan.RuntimeExecutionKind ?? "assembly-bound-native-reference-skeleton"),
            ["translation_unit_mode_literal"] = ToCppStringLiteral(loweringPlan.TranslationUnitMode ?? "runtime-skeleton"),
            ["translation_unit_method_count"] = loweringPlan.TranslationUnitMethodCount ?? 0,
            ["translation_unit_page_size"] = loweringPlan.TranslationUnitPageSize ?? AuditTranslationUnitPageSize,
            ["translation_unit_page_count"] = loweringPlan.TranslationUnitPageCount ?? 0,
            ["page_dispatch_declarations"] = pageDispatchDeclarations,
            ["page_dispatch_catalog_entries"] = pageDispatchCatalogEntries,
            ["native_entry_function_name"] = loweringPlan.NativeEntryFunctionName,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetAssemblyFullClosureRuntimeSkeletonSummaryTemplate(),
            model);
    }

    private static string BuildAssemblyFullClosureRuntimeSkeletonPageTranslationUnit(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        TypedIlIrArtifact typedIl,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationArtifact codeRegistration,
        int pageNumber,
        IReadOnlyList<string> pageItems)
    {
        var methodsBySubjectId = (typedIl.Methods ?? [])
            .ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        var methodPointers = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .Where(registration => string.Equals(registration.RegistrationKind, "methodPointer", StringComparison.Ordinal))
            .ToList();
        var methodStubNamesBySubjectId = pageItems
            .Select((subjectId, index) => new
            {
                subjectId,
                stubName = BuildAssemblyFullClosureRuntimeSkeletonMethodStubName(pageNumber, index + 1, subjectId),
            })
            .ToDictionary(item => item.subjectId, item => item.stubName, StringComparer.Ordinal);
        var methodStubDeclarations = methodStubNamesBySubjectId.Values
            .Select(BuildAssemblyFullClosureRuntimeSkeletonMethodStubDeclaration)
            .ToList();
        var methodStubDefinitions = new List<string>();
        var dispatchEntries = new List<string>();
        for (var index = 0; index < pageItems.Count; index++)
        {
            var subjectId = pageItems[index];
            var stubName = methodStubNamesBySubjectId[subjectId];
            methodStubDefinitions.Add(
                BuildAssemblyFullClosureRuntimeSkeletonMethodStub(
                    loweringPlan,
                    metadataRegistration,
                    methodPointers,
                    methodsBySubjectId,
                    methodStubNamesBySubjectId,
                    stubName,
                    subjectId));
            dispatchEntries.Add($"    {{ {ToCppStringLiteral(subjectId)}, &{stubName} }},");
        }
        var model = new ScriptObject
        {
            ["runtime_execution_kind_literal"] = ToCppStringLiteral(loweringPlan.RuntimeExecutionKind ?? "assembly-bound-native-reference-skeleton"),
            ["page_number"] = pageNumber,
            ["page_item_count"] = pageItems.Count,
            ["method_subject_id_literals"] = pageItems.Select(ToCppStringLiteral).ToArray(),
            ["method_stub_declarations"] = methodStubDeclarations.ToArray(),
            ["method_stub_definitions"] = methodStubDefinitions.ToArray(),
            ["dispatch_entries"] = dispatchEntries.ToArray(),
            ["page_dispatch_function_name"] = BuildAssemblyFullClosureRuntimeSkeletonPageDispatchFunctionName(pageNumber),
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetAssemblyFullClosureRuntimeSkeletonPageTemplate(),
            model);
    }

    private static string BuildAssemblyFullClosureRuntimeSkeletonMethodStubName(
        int pageNumber,
        int itemNumber,
        string subjectId)
    {
        var sanitized = new StringBuilder();
        foreach (var current in subjectId)
        {
            sanitized.Append(char.IsLetterOrDigit(current) ? current : '_');
        }

        return $"NativeReferenceStub_Page{pageNumber:D4}_Item{itemNumber:D4}_{sanitized.ToString().Trim('_')}";
    }

    private static string BuildAssemblyFullClosureRuntimeSkeletonPageDispatchFunctionName(int pageNumber)
    {
        return $"DispatchRuntimeSkeletonPage{pageNumber:D4}";
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

    private static string BuildAssemblyFullClosureRuntimeSkeletonMethodStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        IReadOnlyDictionary<string, string> methodStubNamesBySubjectId,
        string stubName,
        string subjectId)
    {
        if (TryBuildAssemblyBoundMarshalingUtf8ExportPlan(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodPointers,
                out var marshalingUtf8ExportPlan))
        {
            return BuildAssemblyBoundMarshalingUtf8ExportStub(marshalingUtf8ExportPlan, stubName);
        }

        if (TryBuildAssemblyBoundReflectionInteropClosurePlan(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodPointers,
                out var reflectionInteropClosurePlan))
        {
            return BuildAssemblyBoundReflectionInteropClosureStub(reflectionInteropClosurePlan, stubName);
        }

        if (TryBuildAssemblyBoundPInvokeDirectCallPlan(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodPointers,
                methodsBySubjectId.Values.ToList(),
                out var pinvokeDirectCallPlan))
        {
            return BuildAssemblyBoundPInvokeDllImportMinimalStub(pinvokeDirectCallPlan, stubName);
        }

        if (TryBuildAssemblyBoundArrayBoxingReferenceArrayPlan(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodPointers,
                methodsBySubjectId.Values.ToList(),
                out var arrayBoxingReferenceArrayPlan))
        {
            return BuildAssemblyBoundArrayBoxingReferenceArrayStub(arrayBoxingReferenceArrayPlan, stubName);
        }

        if (TryBuildAssemblyBoundInterfaceDispatchMessagePlan(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodPointers,
                methodsBySubjectId.Values.ToList(),
                out var interfaceDispatchMessagePlan))
        {
            return BuildAssemblyBoundInterfaceDispatchMessageStub(interfaceDispatchMessagePlan, stubName);
        }

        if (TryBuildAssemblyBoundThreadingThreadStaticMonitorPlan(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodPointers,
                out var threadingThreadStaticMonitorPlan))
        {
            return BuildAssemblyBoundThreadingThreadStaticMonitorStub(threadingThreadStaticMonitorPlan, stubName);
        }

        if (TryBuildAssemblyBoundStaticIntForwarderStub(
                subjectId,
                methodsBySubjectId,
                methodStubNamesBySubjectId,
                stubName,
                out var staticIntForwarderStub))
        {
            return staticIntForwarderStub;
        }

        if (TryBuildAssemblyBoundStaticStringForwarderStub(
                subjectId,
                methodsBySubjectId,
                stubName,
                out var staticStringForwarderStub))
        {
            return staticStringForwarderStub;
        }

        if (TryBuildAssemblyBoundStaticStringForwarderConsoleWriteLineStub(
                subjectId,
                methodsBySubjectId,
                methodStubNamesBySubjectId,
                stubName,
                out var staticStringForwarderConsoleWriteLineStub))
        {
            return staticStringForwarderConsoleWriteLineStub;
        }

        if (TryBuildAssemblyBoundStaticStringProducerForwarderConsoleWriteLineStub(
                subjectId,
                methodsBySubjectId,
                methodStubNamesBySubjectId,
                stubName,
                out var staticStringProducerForwarderConsoleWriteLineStub))
        {
            return staticStringProducerForwarderConsoleWriteLineStub;
        }

        if (TryBuildAssemblyBoundStaticStringProducerCtorGetterConsoleWriteLineStub(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodsBySubjectId,
                methodStubNamesBySubjectId,
                stubName,
                out var staticStringProducerCtorGetterConsoleWriteLineStub))
        {
            return staticStringProducerCtorGetterConsoleWriteLineStub;
        }

        if (TryBuildAssemblyBoundStaticStringProducerCtorRenderConsoleWriteLineStub(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodsBySubjectId,
                methodStubNamesBySubjectId,
                stubName,
                out var staticStringProducerCtorRenderConsoleWriteLineStub))
        {
            return staticStringProducerCtorRenderConsoleWriteLineStub;
        }

        if (TryBuildAssemblyBoundStaticStringProducerCtorInstanceCallForwarderConsoleWriteLineStub(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodsBySubjectId,
                methodStubNamesBySubjectId,
                stubName,
                out var staticStringProducerCtorInstanceCallForwarderConsoleWriteLineStub))
        {
            return staticStringProducerCtorInstanceCallForwarderConsoleWriteLineStub;
        }

        if (TryBuildAssemblyBoundStaticStringProducerForwarderCtorInstanceCallConsoleWriteLineStub(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodsBySubjectId,
                methodStubNamesBySubjectId,
                stubName,
                out var staticStringProducerForwarderCtorInstanceCallConsoleWriteLineStub))
        {
            return staticStringProducerForwarderCtorInstanceCallConsoleWriteLineStub;
        }

        if (TryBuildAssemblyBoundStaticLiteralStringReturnStub(
                subjectId,
                methodsBySubjectId,
                stubName,
                out var staticLiteralStringReturnStub))
        {
            return staticLiteralStringReturnStub;
        }

        if (TryBuildAssemblyBoundStaticStringProducerConsoleWriteLineStub(
                subjectId,
                methodsBySubjectId,
                methodStubNamesBySubjectId,
                stubName,
                out var staticStringProducerConsoleWriteLineStub))
        {
            return staticStringProducerConsoleWriteLineStub;
        }

        if (TryBuildAssemblyBoundConstructorFieldSetterStub(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodsBySubjectId,
                stubName,
                out var constructorFieldSetterStub))
        {
            return constructorFieldSetterStub;
        }

        if (TryBuildAssemblyBoundFieldBackedStringReturnStub(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodsBySubjectId,
                stubName,
                out var fieldBackedStringReturnStub))
        {
            return fieldBackedStringReturnStub;
        }

        if (TryBuildAssemblyBoundFieldGetterStringReturnStub(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodsBySubjectId,
                stubName,
                out var fieldGetterStringReturnStub))
        {
            return fieldGetterStringReturnStub;
        }

        if (TryBuildAssemblyBoundConsoleWriteLineStub(subjectId, methodsBySubjectId, stubName, out var consoleWriteLineStub))
        {
            return consoleWriteLineStub;
        }

        if (TryBuildAssemblyBoundDelegateClosedTargetRelayPlan(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodPointers,
                out var delegateClosedTargetRelayPlan))
        {
            return BuildAssemblyBoundDelegateClosedTargetRelayStub(delegateClosedTargetRelayPlan, stubName);
        }

        if (TryBuildAssemblyBoundExceptionThrowCatchFinallyPlan(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodPointers,
                out var exceptionThrowCatchFinallyPlan))
        {
            return BuildAssemblyBoundExceptionThrowCatchFinallyStub(exceptionThrowCatchFinallyPlan, stubName);
        }

        if (TryBuildAssemblyBoundNestedExceptionThrowCatchFinallyPlan(
                loweringPlan.AssemblyName,
                subjectId,
                metadataRegistration,
                methodPointers,
                out var nestedExceptionThrowCatchFinallyPlan))
        {
            return BuildAssemblyBoundNestedExceptionThrowCatchFinallyStub(nestedExceptionThrowCatchFinallyPlan, stubName);
        }

        var executableLoweringPlan = TryBuildAssemblyFullClosureExecutableLoweringPlan(
            loweringPlan.AssemblyName,
            subjectId,
            methodsBySubjectId.Values.ToList(),
            metadataRegistration,
            methodPointers);
        if (executableLoweringPlan is not null &&
            string.Equals(executableLoweringPlan.PlanKind, "staticCallCtorGetter", StringComparison.Ordinal))
        {
            return BuildAssemblyBoundStaticCallCtorGetterStub(executableLoweringPlan, stubName);
        }

        if (executableLoweringPlan is not null &&
            string.Equals(executableLoweringPlan.PlanKind, "constructorThenInstanceCall", StringComparison.Ordinal))
        {
            return BuildAssemblyBoundConstructorThenInstanceCallStub(executableLoweringPlan, stubName);
        }

        if (executableLoweringPlan is not null &&
            string.Equals(executableLoweringPlan.PlanKind, "delegateClosedTargetRelayMinimal", StringComparison.Ordinal))
        {
            return BuildAssemblyBoundDelegateClosedTargetRelayStub(executableLoweringPlan, stubName);
        }

        if (executableLoweringPlan is not null &&
            string.Equals(executableLoweringPlan.PlanKind, ExceptionThrowCatchFinallyMinimal, StringComparison.Ordinal))
        {
            return BuildAssemblyBoundExceptionThrowCatchFinallyStub(executableLoweringPlan, stubName);
        }

        if (executableLoweringPlan is not null &&
            string.Equals(executableLoweringPlan.PlanKind, InteropPInvokeDirectCallMinimal, StringComparison.Ordinal))
        {
            return BuildAssemblyBoundPInvokeDllImportMinimalStub(executableLoweringPlan, stubName);
        }

        if (executableLoweringPlan is not null &&
            string.Equals(executableLoweringPlan.PlanKind, "arrayBoxingReferenceArray", StringComparison.Ordinal))
        {
            return BuildAssemblyBoundArrayBoxingReferenceArrayStub(executableLoweringPlan, stubName);
        }

        if (executableLoweringPlan is not null &&
            string.Equals(executableLoweringPlan.PlanKind, "interfaceDispatchMessage", StringComparison.Ordinal))
        {
            return BuildAssemblyBoundInterfaceDispatchMessageStub(executableLoweringPlan, stubName);
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["subject_id"] = subjectId,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonReservedStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundStaticCallCtorGetterStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["captured_field_token"] = loweringPlan.CapturedFieldToken,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["echo_literal"] = loweringPlan.EchoLiteral,
            ["echo_literal_byte_count"] = loweringPlan.EchoLiteralByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonStaticCallCtorGetterStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundConstructorThenInstanceCallStub(
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
            ["concat_icall_literal"] = ToCppStringLiteral(concatIcall),
            ["string_concat_triple_icall_literal"] = ToCppStringLiteral(StringConcatTripleIcall),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["constructor_literal"] = loweringPlan.ConstructorLiteral,
            ["constructor_literal_byte_count"] = loweringPlan.ConstructorLiteralByteCount ?? 0,
            ["message_prefix_literal"] = loweringPlan.MessagePrefixLiteral,
            ["message_prefix_literal_byte_count"] = loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            ["message_suffix_literal"] = loweringPlan.MessageSuffixLiteral,
            ["message_suffix_literal_byte_count"] = loweringPlan.MessageSuffixLiteralByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonConstructorThenInstanceCallStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundDelegateClosedTargetRelayStub(
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
            NativeReferenceProofCatalog.GetRuntimeSkeletonDelegateClosedTargetRelayStubTemplate(),
            model);
    }

    private static string BuildAssemblyBoundArrayBoxingReferenceArrayStub(
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
            ["boxed_value_type_token"] = loweringPlan.BoxedValueTypeToken,
            ["captured_field_token"] = loweringPlan.CapturedFieldToken,
            ["concat_icall_literal"] = ToCppStringLiteral(concatIcall),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["boxed_int32_value"] = loweringPlan.BoxedInt32Value ?? 0,
            ["constructor_literal"] = loweringPlan.ConstructorLiteral,
            ["constructor_literal_byte_count"] = loweringPlan.ConstructorLiteralByteCount ?? 0,
            ["message_prefix_literal"] = loweringPlan.MessagePrefixLiteral,
            ["message_prefix_literal_byte_count"] = loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            ["message_suffix_literal"] = loweringPlan.MessageSuffixLiteral,
            ["message_suffix_literal_byte_count"] = loweringPlan.MessageSuffixLiteralByteCount ?? 0,
            ["use_triple_concat"] = string.Equals(concatIcall, StringConcatTripleIcall, StringComparison.Ordinal),
        };
        return ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonArrayBoxingReferenceArrayStubTemplate(),
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
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("DelegateProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var bannerTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration => string.Equals(registration.Name, "DelegateBanner", StringComparison.Ordinal),
            "delegate banner type");
        var bannerFieldRegistration = GetRequiredMetadataRegistration(
            registrations,
            "field",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, bannerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "_name", StringComparison.Ordinal),
            "delegate banner captured field");
        var constructorMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, bannerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, ".ctor", StringComparison.Ordinal) &&
                registration.ParameterCount == 1,
            "delegate banner constructor");
        var instanceMethodMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, bannerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "BuildMessage", StringComparison.Ordinal) &&
                registration.ParameterCount == 1,
            "delegate closed target");
        var staticTailTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration => string.Equals(registration.Name, "DelegateStaticTail", StringComparison.Ordinal),
            "delegate static tail type");
        var staticMethodMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, staticTailTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "AppendBang", StringComparison.Ordinal) &&
                registration.ParameterCount == 1,
            "delegate static tail");

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorMetadata.SubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodMetadata.SubjectId);
        var staticMethodRegistration = GetRequiredRegistration(methodPointers, staticMethodMetadata.SubjectId);

        const string constructorLiteral = "delegate proof";
        const string messagePrefixLiteral = "Delegate native proof: ";
        const string messageSuffixLiteral = ".";

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
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", bannerTypeRegistration.SubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", bannerFieldRegistration.SubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorMetadata.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodMetadata.SubjectId)),
            StaticMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", staticMethodMetadata.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            StringConcatPairIcall = StringConcatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };

        return true;
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
        RequireCapability(entryPointMethod, "requires-imported-call");
        RequireCapability(importMethod, "requires-imported-call");
        RequireMethodContract(importMethod, "imported-method", "no-canonical-body");
        var importMethodMetadata = GetRequiredMetadataRegistration(
            metadataRegistration.Registrations,
            "method",
            importMethodSubjectId,
            "pinvoke import method");

        if (importMethodMetadata.IsImported != true)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{importMethodSubjectId}' to carry imported method metadata");
        }

        if (string.IsNullOrWhiteSpace(importMethodMetadata.ImportModuleName) ||
            string.IsNullOrWhiteSpace(importMethodMetadata.ImportEntryPointName))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{importMethodSubjectId}' to carry importModuleName/importEntryPointName metadata");
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
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, declaringTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
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

        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var loadedFieldSubjectId = GetRequiredOperandString(instructions[2]);
        var messagePrefixLiteral = GetRequiredOperandString(instructions[0]);
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(method, instructions);
        var concatIcall = NormalizeStringConcatIcall(GetCapturedStateInstanceMessageConcatIcall(method, instructions));
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, declaringTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", loadedFieldSubjectId)),
            ["concat_icall_literal"] = ToCppStringLiteral(concatIcall),
            ["string_concat_triple_icall_literal"] = ToCppStringLiteral(StringConcatTripleIcall),
            ["message_prefix_literal"] = ToCppStringLiteral(messagePrefixLiteral),
            ["message_prefix_literal_byte_count"] = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            ["message_suffix_literal"] = ToCppStringLiteral(messageSuffixLiteral),
            ["message_suffix_literal_byte_count"] = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonFieldBackedStringReturnStubTemplate(),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundFieldGetterStringReturnStub(
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
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["reference_type_token"] = CreateTypeTokenLiteral(metadataRegistration, declaringTypeSubjectId),
            ["captured_field_token"] = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", loadedFieldSubjectId)),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            NativeReferenceProofCatalog.GetRuntimeSkeletonFieldGetterStringReturnStubTemplate(),
            model);
        return true;
    }

    private static string? BuildPreferredAssemblyDispatchSubjectId(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        if (!string.Equals(loweringPlan.RuntimeExecutionKind, "assembly-bound-native-reference-skeleton", StringComparison.Ordinal))
        {
            return null;
        }

        if (!string.IsNullOrWhiteSpace(loweringPlan.EntrySubjectId))
        {
            return loweringPlan.EntrySubjectId;
        }

        var methodSubjectIds = loweringPlan.TranslationUnitMethodSubjectIds ?? [];
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
        RequireStringField(loweringPlan.PlanKind, nameof(loweringPlan.PlanKind));
        RequireStringField(loweringPlan.AssemblyName, nameof(loweringPlan.AssemblyName));

        if (!string.Equals(loweringPlan.PlanKind, "assembly-full-closure-audit", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure audit emitter expected plan kind 'assembly-full-closure-audit', but found '{loweringPlan.PlanKind}'");
        }

        if (!string.Equals(loweringPlan.TranslationUnitMode, "audit-only", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure audit emitter expected translation-unit mode 'audit-only', but found '{loweringPlan.TranslationUnitMode ?? "<null>"}'");
        }

        if (loweringPlan.TranslationUnitMethodSubjectIds is null ||
            loweringPlan.TranslationUnitMethodSubjectIds.Count == 0)
        {
            throw new InvalidOperationException(
                "assembly-full-closure audit emitter requires at least one translation-unit method subject id");
        }

        if (loweringPlan.TranslationUnitPageSize != AuditTranslationUnitPageSize)
        {
            throw new InvalidOperationException(
                $"assembly-full-closure audit emitter expected translation-unit page size '{AuditTranslationUnitPageSize}', but found '{loweringPlan.TranslationUnitPageSize?.ToString() ?? "<null>"}'");
        }

        if (loweringPlan.TranslationUnitPageCount != GetAuditPageCount(loweringPlan.TranslationUnitMethodSubjectIds.Count))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure audit emitter expected translation-unit page count '{GetAuditPageCount(loweringPlan.TranslationUnitMethodSubjectIds.Count)}', but found '{loweringPlan.TranslationUnitPageCount?.ToString() ?? "<null>"}'");
        }

        if (loweringPlan.TranslationUnitPages is null ||
            loweringPlan.TranslationUnitPages.Count != loweringPlan.TranslationUnitPageCount)
        {
            throw new InvalidOperationException(
                "assembly-full-closure audit emitter requires translation-unit page metadata matching the declared page count");
        }
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
        RequireCapability(entryPointMethod, "requires-imported-call");
        RequireCapability(importMethod, "requires-imported-call");
        RequireMethodContract(importMethod, "imported-method", "no-canonical-body");
        var importMethodMetadata = GetRequiredMetadataRegistration(
            metadataRegistration.Registrations,
            "method",
            importMethodSubjectId,
            "pinvoke import method");

        if (importMethodMetadata.IsImported != true)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{importMethodSubjectId}' to carry imported method metadata");
        }

        if (string.IsNullOrWhiteSpace(importMethodMetadata.ImportModuleName) ||
            string.IsNullOrWhiteSpace(importMethodMetadata.ImportEntryPointName))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{importMethodSubjectId}' to carry importModuleName/importEntryPointName metadata");
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
        var closedTypeRegistration = registrations.FirstOrDefault(registration =>
            string.Equals(registration.RegistrationKind, "type", StringComparison.Ordinal) &&
            !string.IsNullOrWhiteSpace(registration.DefinitionSubjectId) &&
            !string.Equals(registration.SubjectId, registration.DefinitionSubjectId, StringComparison.Ordinal));
        if (closedTypeRegistration is null)
        {
            return false;
        }

        var propertyRegistration = registrations.FirstOrDefault(registration =>
            string.Equals(registration.RegistrationKind, "property", StringComparison.Ordinal) &&
            string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
            !IsCompilerGeneratedMemberName(registration.Name));
        var queryMethodRegistration = registrations.FirstOrDefault(registration =>
            string.Equals(registration.RegistrationKind, "method", StringComparison.Ordinal) &&
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
        AddIfNotNull(model, "message_prefix_literal", loweringPlan.MessagePrefixLiteral);
        AddIfNotNull(model, "message_prefix_literal_byte_count", loweringPlan.MessagePrefixLiteralByteCount);
        AddIfNotNull(model, "message_suffix_literal", loweringPlan.MessageSuffixLiteral);
        AddIfNotNull(model, "message_suffix_literal_byte_count", loweringPlan.MessageSuffixLiteralByteCount);
        AddIfNotNull(model, "echo_literal", loweringPlan.EchoLiteral);
        AddIfNotNull(model, "echo_literal_byte_count", loweringPlan.EchoLiteralByteCount);
        AddIfNotNull(model, "finally_literal", loweringPlan.FinallyLiteral);
        AddIfNotNull(model, "finally_literal_byte_count", loweringPlan.FinallyLiteralByteCount);
        AddIfNotNull(model, "boxed_value_type_token", loweringPlan.BoxedValueTypeToken);
        AddIfNotNull(model, "boxed_int32_value", loweringPlan.BoxedInt32Value);
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
            RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
            RequireInstructionOp(instructions[1], "ldarg", method.SubjectId, 1);
            RequireInstructionOp(instructions[2], "ldfld", method.SubjectId, 2);
            RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
            RequireInstructionOp(instructions[4], "ldstr", method.SubjectId, 4);
            RequireInstructionOp(instructions[5], "call", method.SubjectId, 5);
            RequireInstructionOp(instructions[6], "ret", method.SubjectId, 6);

            if (GetRequiredOperandInt(instructions[1]) != 0)
            {
                throw new InvalidOperationException(
                    $"native-reference emitter expects '{method.SubjectId}' to start with ldarg 0");
            }

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
        var parameterSeparatorIndex = subjectId.IndexOf('(', methodSeparatorIndex + 2);
        if (methodSeparatorIndex <= 0 || parameterSeparatorIndex <= methodSeparatorIndex + 2)
        {
            throw new InvalidOperationException($"failed to extract method name from subject id '{subjectId}'");
        }

        return subjectId[(methodSeparatorIndex + 2)..parameterSeparatorIndex];
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
        return GetRequiredMetadataRegistration(
            registrations,
            registrationKind,
            registration => string.Equals(registration.SubjectId, subjectId, StringComparison.Ordinal),
            description);
    }

    private static MetadataRegistrationEntry GetRequiredMetadataRegistration(
        IReadOnlyList<MetadataRegistrationEntry> registrations,
        string registrationKind,
        Func<MetadataRegistrationEntry, bool> predicate,
        string description)
    {
        var registration = registrations.FirstOrDefault(item =>
            string.Equals(item.RegistrationKind, registrationKind, StringComparison.Ordinal) &&
            predicate(item));
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
        uint rowIndex = 0u;

        foreach (var registration in metadataRegistration.Registrations)
        {
            if (!string.Equals(registration.RegistrationKind, registrationKind, StringComparison.Ordinal))
            {
                continue;
            }

            rowIndex++;
            if (string.Equals(registration.SubjectId, subjectId, StringComparison.Ordinal))
            {
                return GetMetadataTokenPrefix(registrationKind) | rowIndex;
            }
        }

        throw new InvalidOperationException(
            $"missing required '{registrationKind}' metadata registration for '{subjectId}'");
    }

    private static string CreateTypeTokenLiteral(
        MetadataRegistrationArtifact metadataRegistration,
        string subjectId)
    {
        var hasConcreteRegistration = metadataRegistration.Registrations.Any(item =>
            string.Equals(item.RegistrationKind, "type", StringComparison.Ordinal) &&
            string.Equals(item.SubjectId, subjectId, StringComparison.Ordinal));
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
        var method = methods.FirstOrDefault(item => string.Equals(item.SubjectId, subjectId, StringComparison.Ordinal));
        if (method is null)
        {
            throw new InvalidOperationException($"missing required typed-il method for '{subjectId}'");
        }

        return method;
    }

    private static CodeRegistrationEntry GetRequiredRegistration(
        IReadOnlyList<CodeRegistrationEntry> registrations,
        string subjectId)
    {
        var registration = registrations.FirstOrDefault(item => string.Equals(item.SubjectId, subjectId, StringComparison.Ordinal));
        if (registration is null)
        {
            throw new InvalidOperationException($"missing required method pointer registration for '{subjectId}'");
        }

        return registration;
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


