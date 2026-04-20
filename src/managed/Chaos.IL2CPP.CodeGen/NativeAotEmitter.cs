using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed class NativeAotEmitter
{
    private const int AuditTranslationUnitPageSize = 1024;
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

    public NativeAotResult Generate(NativeAotRequest request)
    {
        var managedClosureRoot = Path.GetFullPath(request.ManagedClosureRootPath);
        var loweringPlanPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.NativeAotLoweringPlan);
        var loweringPlan = LoadRequiredJson<NativeAotLoweringPlanArtifact>(loweringPlanPath);
        IReadOnlyList<NativeAotGeneratedSource> generatedSources;

        if (string.Equals(loweringPlan.PlanKind, "assembly-full-closure-audit", StringComparison.Ordinal))
        {
            generatedSources = BuildAssemblyFullClosureAuditGeneratedSources(loweringPlan);
        }
        else
        {
            var aotCoreIrPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.AotCoreIr);
            var closureManifestPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.ClosureManifest);
            var metadataRegistrationPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.MetadataRegistration);
            var supplementalMetadataTemplatePath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.SupplementalMetadataTemplate);
            var aotCoreIr = LoadRequiredJson<AotCoreIrArtifact>(aotCoreIrPath);
            var closureManifest = LoadRequiredJson<ManagedClosureManifestArtifact>(closureManifestPath);
            var metadataRegistration = LoadRequiredJson<MetadataRegistrationArtifact>(metadataRegistrationPath);
            var supplementalMetadataTemplate = LoadRequiredJson<SupplementalMetadataTemplateArtifact>(supplementalMetadataTemplatePath);
            ValidateLoweringPlan(loweringPlan, closureManifest);

            var entryMethod = LoadEntryMethod(aotCoreIr, loweringPlan.EntrySubjectId);
            var templateModel = new NativeAotLoweringPlanner().Create(
                loweringPlan,
                aotCoreIr,
                entryMethod,
                closureManifest,
                metadataRegistration,
                supplementalMetadataTemplate);
            generatedSources =
            [
                new NativeAotGeneratedSource
                {
                    RelativePath = NativeAotArtifactNames.GeneratedTranslationUnit,
                    Contents = BuildGeneratedTranslationUnit(templateModel),
                },
            ];
        }

        var manifest = new NativeAotManifestArtifact
        {
            AssemblyName = loweringPlan.AssemblyName,
            EntrySubjectId = loweringPlan.EntrySubjectId,
            ManagedClosureRootPath = ManagedNaming.NormalizePathForManifest(managedClosureRoot, Environment.CurrentDirectory),
            PlanArtifactPath = NativeAotArtifactNames.LoweringPlan,
            TranslationUnitPageSize = loweringPlan.TranslationUnitPageSize,
            TranslationUnitPageCount = loweringPlan.TranslationUnitPageCount,
            TranslationUnitPages = loweringPlan.TranslationUnitPages,
            GeneratedArtifacts = generatedSources
                .Select(generatedSource => new NativeAotGeneratedArtifactRef
                {
                    Kind = "generatedTranslationUnit",
                    Path = generatedSource.RelativePath,
                })
                .ToList(),
        };

        return new NativeAotResult
        {
            OutputRootPath = request.OutputRootPath,
            LoweringPlan = loweringPlan,
            Manifest = manifest,
            GeneratedSources = generatedSources,
        };
    }

    private static IReadOnlyList<NativeAotGeneratedSource> BuildAssemblyFullClosureAuditGeneratedSources(
        NativeAotLoweringPlanArtifact loweringPlan)
    {
        ValidateAssemblyFullClosureAuditPlan(loweringPlan);

        var generatedSources = new List<NativeAotGeneratedSource>
        {
            new()
            {
                RelativePath = NativeAotArtifactNames.GeneratedTranslationUnit,
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
            generatedSources.Add(new NativeAotGeneratedSource
            {
                RelativePath = $"generated/audit/native-aot.methods.page-{pageIndex + 1:D4}.cpp",
                Contents = BuildAssemblyFullClosureAuditPageTranslationUnit(loweringPlan, pageIndex + 1, pageItems),
            });
        }

        return generatedSources;
    }

    private static string BuildAssemblyFullClosureAuditSummaryTranslationUnit(
        NativeAotLoweringPlanArtifact loweringPlan)
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
            NativeAotTemplateCatalog.GetAssemblyFullClosureAuditSummaryTemplate(),
            model);
    }

    private static string BuildAssemblyFullClosureAuditPageTranslationUnit(
        NativeAotLoweringPlanArtifact loweringPlan,
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
            NativeAotTemplateCatalog.GetAssemblyFullClosureAuditPageTemplate(),
            model);
    }

    private static void ValidateLoweringPlan(
        NativeAotLoweringPlanArtifact loweringPlan,
        ManagedClosureManifestArtifact closureManifest)
    {
        RequireStringField(loweringPlan.PlanKind, nameof(loweringPlan.PlanKind));
        RequireStringField(loweringPlan.AssemblyName, nameof(loweringPlan.AssemblyName));
        RequireStringField(loweringPlan.EntrySubjectId, nameof(loweringPlan.EntrySubjectId));
        RequireStringField(loweringPlan.NativeEntryFunctionName, nameof(loweringPlan.NativeEntryFunctionName));
        RequireStringField(loweringPlan.EntrySymbol, nameof(loweringPlan.EntrySymbol));
        RequireStringField(loweringPlan.EntryMethodToken, nameof(loweringPlan.EntryMethodToken));
        RequireStringField(loweringPlan.WorkloadAbi, nameof(loweringPlan.WorkloadAbi));

        if (!string.Equals(loweringPlan.PlanKind, "generic-managed-entry", StringComparison.Ordinal))
        {
            throw new NotSupportedException(
                $"unsupported native-aot lowering plan kind '{loweringPlan.PlanKind}'");
        }

        if (!string.Equals(loweringPlan.WorkloadAbi, "int(int32)", StringComparison.Ordinal))
        {
            throw new NotSupportedException(
                $"unsupported native-aot workload ABI '{loweringPlan.WorkloadAbi}'");
        }

        if (!string.Equals(loweringPlan.EntrySubjectId, closureManifest.EntrySubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-aot plan entry '{loweringPlan.EntrySubjectId}' does not match closure entry '{closureManifest.EntrySubjectId}'");
        }
    }

    private static void ValidateAssemblyFullClosureAuditPlan(NativeAotLoweringPlanArtifact loweringPlan)
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

    private static AotCoreIrMethodArtifact LoadEntryMethod(
        AotCoreIrArtifact aotCoreIr,
        string entrySubjectId)
    {
        var entryMethod = aotCoreIr.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, entrySubjectId, StringComparison.Ordinal));
        if (entryMethod is null)
        {
            throw new InvalidOperationException(
                $"aot-core-ir entry method '{entrySubjectId}' is missing from generated closure artifacts");
        }

        return entryMethod;
    }

    private static string BuildGeneratedTranslationUnit(NativeAotTemplateModel templateModel)
    {
        var objectModelSection = BuildObjectModelSection(templateModel);
        var methodSections = templateModel.Methods
            .Select(BuildMethodSection)
            .ToArray();
        var model = new ScriptObject
        {
            ["includes"] = templateModel.Includes,
            ["object_model_section"] = objectModelSection,
            ["method_declarations"] = templateModel.MethodDeclarations,
            ["method_sections"] = methodSections,
            ["entry_subject_id"] = templateModel.EntrySubjectId,
            ["entry_symbol"] = templateModel.EntrySymbol,
            ["entry_native_symbol"] = templateModel.EntryNativeSymbol,
            ["native_entry_function_name"] = templateModel.NativeEntryFunctionName,
            ["entry_bridge_arguments"] = templateModel.EntryBridgeArguments,
        };
        return ScribanTemplateRenderer.RenderTemplate(NativeAotTemplateCatalog.GetTranslationUnitTemplate(), model);
    }

    private static string BuildObjectModelSection(NativeAotTemplateModel templateModel)
    {
        var model = new ScriptObject
        {
            ["object_model_code"] = templateModel.ObjectModelCode,
        };
        return ScribanTemplateRenderer.RenderTemplate(NativeAotTemplateCatalog.GetObjectModelTemplate(), model);
    }

    private static string BuildMethodSection(NativeAotMethodTemplateModel methodModel)
    {
        var model = new ScriptObject
        {
            ["subject_id"] = methodModel.SubjectId,
            ["method_source"] = methodModel.MethodSource,
        };
        return ScribanTemplateRenderer.RenderTemplate(NativeAotTemplateCatalog.GetMethodTemplate(), model);
    }

    private static T LoadRequiredJson<T>(string path)
    {
        if (!File.Exists(path))
        {
            throw new FileNotFoundException($"required native-aot lowering plan is missing: {path}", path);
        }

        var value = JsonSerializer.Deserialize<T>(File.ReadAllText(path), JsonOptions);
        if (value is null)
        {
            throw new InvalidOperationException($"failed to deserialize native-aot lowering plan: {path}");
        }

        return value;
    }

    private static void RequireStringField(string? value, string fieldName)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            throw new InvalidOperationException(
                $"native-aot lowering plan requires non-empty field '{fieldName}'");
        }
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

    private static int GetAuditPageCount(int totalMethodCount)
    {
        if (totalMethodCount <= 0)
        {
            return 0;
        }

        return (totalMethodCount + AuditTranslationUnitPageSize - 1) / AuditTranslationUnitPageSize;
    }
}
