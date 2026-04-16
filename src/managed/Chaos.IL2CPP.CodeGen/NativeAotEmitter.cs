using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed class NativeAotEmitter
{
    private const string TranslationUnitTemplateRelativePath = "Templates/NativeAot.TranslationUnit.cpp.scriban";
    private const string ObjectModelTemplateRelativePath = "Templates/NativeAot.ObjectModel.cpp.scriban";
    private const string MethodTemplateRelativePath = "Templates/NativeAot.Method.cpp.scriban";

    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

    private static readonly Lazy<Template> TranslationUnitTemplate =
        new(() => LoadTemplate(TranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> ObjectModelTemplate =
        new(() => LoadTemplate(ObjectModelTemplateRelativePath));

    private static readonly Lazy<Template> MethodTemplate =
        new(() => LoadTemplate(MethodTemplateRelativePath));

    public NativeAotResult Generate(NativeAotRequest request)
    {
        var managedClosureRoot = Path.GetFullPath(request.ManagedClosureRootPath);
        var loweringPlanPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.NativeAotLoweringPlan);
        var aotCoreIrPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.AotCoreIr);
        var closureManifestPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.ClosureManifest);
        var loweringPlan = LoadRequiredJson<NativeAotLoweringPlanArtifact>(loweringPlanPath);
        var aotCoreIr = LoadRequiredJson<AotCoreIrArtifact>(aotCoreIrPath);
        var closureManifest = LoadRequiredJson<ManagedClosureManifestArtifact>(closureManifestPath);
        ValidateLoweringPlan(loweringPlan, closureManifest);

        var entryMethod = LoadEntryMethod(aotCoreIr, loweringPlan.EntrySubjectId);
        var templateModel = new NativeAotLoweringPlanner().Create(loweringPlan, aotCoreIr, entryMethod);
        var translationUnit = BuildGeneratedTranslationUnit(templateModel);
        var generatedSource = new NativeAotGeneratedSource
        {
            RelativePath = NativeAotArtifactNames.GeneratedTranslationUnit,
            Contents = translationUnit,
        };

        var manifest = new NativeAotManifestArtifact
        {
            AssemblyName = loweringPlan.AssemblyName,
            EntrySubjectId = loweringPlan.EntrySubjectId,
            ManagedClosureRootPath = ManagedNaming.NormalizePathForManifest(managedClosureRoot, Environment.CurrentDirectory),
            PlanArtifactPath = NativeAotArtifactNames.LoweringPlan,
            GeneratedArtifacts =
            [
                new NativeAotGeneratedArtifactRef
                {
                    Kind = "generatedTranslationUnit",
                    Path = generatedSource.RelativePath,
                },
            ],
        };

        return new NativeAotResult
        {
            OutputRootPath = request.OutputRootPath,
            LoweringPlan = loweringPlan,
            Manifest = manifest,
            GeneratedSources = [generatedSource],
        };
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

        if (!string.Equals(loweringPlan.WorkloadAbi, "int(void)", StringComparison.Ordinal))
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
        };
        return RenderTemplate(TranslationUnitTemplate.Value, model);
    }

    private static string BuildObjectModelSection(NativeAotTemplateModel templateModel)
    {
        var model = new ScriptObject
        {
            ["object_model_code"] = templateModel.ObjectModelCode,
        };
        return RenderTemplate(ObjectModelTemplate.Value, model);
    }

    private static string BuildMethodSection(NativeAotMethodTemplateModel methodModel)
    {
        var model = new ScriptObject
        {
            ["subject_id"] = methodModel.SubjectId,
            ["method_source"] = methodModel.MethodSource,
        };
        return RenderTemplate(MethodTemplate.Value, model);
    }

    private static string RenderTemplate(Template template, ScriptObject model)
    {
        var context = new TemplateContext();
        context.PushGlobal(model);

        try
        {
            return template.Render(context).TrimEnd();
        }
        finally
        {
            context.PopGlobal();
        }
    }

    private static Template LoadTemplate(string relativeTemplatePath)
    {
        var templatePath = Path.Combine(AppContext.BaseDirectory, relativeTemplatePath.Replace('/', Path.DirectorySeparatorChar));
        if (!File.Exists(templatePath))
        {
            throw new FileNotFoundException("required Scriban template is missing", templatePath);
        }

        var template = Template.Parse(File.ReadAllText(templatePath), templatePath);
        if (!template.HasErrors)
        {
            return template;
        }

        throw new InvalidOperationException(
            $"failed to parse Scriban template '{relativeTemplatePath}':{Environment.NewLine}{string.Join(Environment.NewLine, template.Messages)}");
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
}
