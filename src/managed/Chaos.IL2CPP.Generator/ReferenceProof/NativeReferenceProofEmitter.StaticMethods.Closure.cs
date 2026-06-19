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

}
