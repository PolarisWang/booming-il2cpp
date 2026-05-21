using System.Text.Json;
using System.Text.Json.Serialization;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

internal static class AssemblyFullClosureAuditEmitter
{
    internal sealed record GeneratedFile(string RelativePath, string Contents);

    private const string SummaryTemplateRelativePath = "Templates/AssemblyFullClosureAuditSummary.cpp.scriban";

    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
    };

    internal static IReadOnlyList<GeneratedFile> BuildGeneratedFiles(
        string assemblyName,
        string planKind,
        int translationUnitMethodCount,
        int translationUnitPageSize,
        IReadOnlyList<AuditTranslationUnitPageArtifact> translationUnitPages,
        IReadOnlyList<string> methodSubjectIds,
        string summaryRelativePath)
    {
        var generatedFiles = new List<GeneratedFile>
        {
            new(summaryRelativePath, BuildSummaryTranslationUnit(
                assemblyName,
                planKind,
                translationUnitMethodCount,
                translationUnitPageSize,
                translationUnitPages.Count)),
        };

        foreach (var page in translationUnitPages)
        {
            var pageItems = methodSubjectIds
                .Skip((page.PageNumber - 1) * translationUnitPageSize)
                .Take(page.MethodCount)
                .ToList();
            generatedFiles.Add(new GeneratedFile(
                page.Path,
                BuildPageManifest(
                    assemblyName,
                    planKind,
                    page.PageNumber,
                    pageItems)));
        }

        return generatedFiles;
    }

    internal static void ValidatePlan(
        string planKind,
        string assemblyName,
        string? translationUnitMode,
        IReadOnlyList<string>? translationUnitMethodSubjectIds,
        int? translationUnitPageSize,
        int? translationUnitPageCount,
        IReadOnlyList<AuditTranslationUnitPageArtifact>? translationUnitPages,
        int expectedPageSize)
    {
        RequireStringField(planKind, nameof(planKind));
        RequireStringField(assemblyName, nameof(assemblyName));

        if (!string.Equals(planKind, "assembly-full-closure-audit", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure audit emitter expected plan kind 'assembly-full-closure-audit', but found '{planKind}'");
        }

        if (!string.Equals(translationUnitMode, "audit-only", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure audit emitter expected translation-unit mode 'audit-only', but found '{translationUnitMode ?? "<null>"}'");
        }

        if (translationUnitMethodSubjectIds is null || translationUnitMethodSubjectIds.Count == 0)
        {
            throw new InvalidOperationException(
                "assembly-full-closure audit emitter requires at least one translation-unit method subject id");
        }

        if (translationUnitPageSize != expectedPageSize)
        {
            throw new InvalidOperationException(
                $"assembly-full-closure audit emitter expected translation-unit page size '{expectedPageSize}', but found '{translationUnitPageSize?.ToString() ?? "<null>"}'");
        }

        var expectedPageCount = GetPageCount(translationUnitMethodSubjectIds.Count, expectedPageSize);
        if (translationUnitPageCount != expectedPageCount)
        {
            throw new InvalidOperationException(
                $"assembly-full-closure audit emitter expected translation-unit page count '{expectedPageCount}', but found '{translationUnitPageCount?.ToString() ?? "<null>"}'");
        }

        if (translationUnitPages is null || translationUnitPages.Count != translationUnitPageCount)
        {
            throw new InvalidOperationException(
                "assembly-full-closure audit emitter requires translation-unit page metadata matching the declared page count");
        }
    }

    internal static int GetPageCount(int methodCount, int pageSize)
    {
        return methodCount == 0 ? 0 : (methodCount + pageSize - 1) / pageSize;
    }

    private static string BuildSummaryTranslationUnit(
        string assemblyName,
        string planKind,
        int translationUnitMethodCount,
        int translationUnitPageSize,
        int translationUnitPageCount)
    {
        var model = new ScriptObject
        {
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["plan_kind_literal"] = ToCppStringLiteral(planKind),
            ["translation_unit_method_count"] = translationUnitMethodCount,
            ["translation_unit_page_size"] = translationUnitPageSize,
            ["translation_unit_page_count"] = translationUnitPageCount,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(SummaryTemplateRelativePath),
            model);
    }

    private static string BuildPageManifest(
        string assemblyName,
        string planKind,
        int pageNumber,
        IReadOnlyList<string> methodSubjectIds)
    {
        var manifest = new AssemblyFullClosureAuditPageManifestArtifact
        {
            AssemblyName = assemblyName,
            PlanKind = planKind,
            PageNumber = pageNumber,
            MethodCount = methodSubjectIds.Count,
            MethodSubjectIds = methodSubjectIds,
        };
        return JsonSerializer.Serialize(manifest, JsonOptions) + Environment.NewLine;
    }

    private static void RequireStringField(string? value, string fieldName)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            throw new InvalidOperationException($"assembly-full-closure audit emitter requires '{fieldName}'");
        }
    }

    private static string ToCppStringLiteral(string value)
    {
        return $"\"{value.Replace("\\", "\\\\", StringComparison.Ordinal).Replace("\"", "\\\"", StringComparison.Ordinal)}\"";
    }
}
