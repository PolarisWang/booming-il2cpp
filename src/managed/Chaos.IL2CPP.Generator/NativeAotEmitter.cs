using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed class NativeAotEmitter
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

    public NativeAotResult Generate(NativeAotRequest request)
    {
        var managedClosureRoot = Path.GetFullPath(request.ManagedClosureRootPath);
        var loweringPlanPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.NativeAotLoweringPlan);
        var loweringPlan = LoadRequiredJson<NativeAotLoweringPlanArtifact>(loweringPlanPath);

        var (aotCoreIr, closureManifest, metadataRegistration, supplementalMetadataTemplate) =
            LoadClosureArtifacts(managedClosureRoot);

        return GenerateFromArtifacts(
            loweringPlan, aotCoreIr, closureManifest,
            metadataRegistration, supplementalMetadataTemplate,
            request.OutputRootPath, request.Mode);
    }

    /// <summary>
    /// Generate NativeAot C++ output directly from in-memory artifacts (no JSON round-trip).
    /// Used by FullAssemblyEmitter to avoid serialization overhead when artifacts are
    /// already available from the pipeline.
    /// </summary>
    public NativeAotResult GenerateFromArtifacts(
        NativeAotLoweringPlanArtifact loweringPlan,
        AotCoreIrArtifact aotCoreIr,
        ManagedClosureManifestArtifact closureManifest,
        MetadataRegistrationArtifact metadataRegistration,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate,
        string outputRootPath,
        CodegenMode mode = CodegenMode.Aot)
    {
        ValidateLoweringPlan(loweringPlan, closureManifest);
        var entryMethod = LoadEntryMethod(aotCoreIr, loweringPlan.EntrySubjectId);

        bool isFullAssembly = string.Equals(loweringPlan.PlanKind, "full-assembly-entry", StringComparison.Ordinal);
        var planner = new NativeAotLoweringPlanner();
        var templateModel = planner.Create(
            loweringPlan,
            aotCoreIr,
            entryMethod,
            closureManifest,
            metadataRegistration,
            supplementalMetadataTemplate,
            fullAssemblyMode: isFullAssembly,
            mode: mode);

        var (generatedSources, generatedArtifacts) = BuildGeneratedSources(
            templateModel, loweringPlan);

        var codegenMetrics = NativeCodegenMetricsBuilder.Build(
            "native-aot",
            loweringPlan.PlanKind,
            generatedSources.Select(generatedSource => (generatedSource.RelativePath, generatedSource.Contents)),
            planner.StructuredMethodCount,
            planner.StructuredExceptionBodyCount,
            planner.FlatFallbackCount,
            planner.TotalMethodCount,
            planner.AotReachableMethodCount,
            planner?.AotUnreachableMethodCount ?? 0);

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
                new NativeAotGeneratedArtifactRef
                {
                    Kind = "codegenMetrics",
                    Path = NativeAotArtifactNames.CodegenMetrics,
                },
            ])
            .ToList();

        var manifest = new NativeAotManifestArtifact
        {
            AssemblyName = loweringPlan.AssemblyName,
            EntrySubjectId = loweringPlan.EntrySubjectId,
            ManagedClosureRootPath = ManagedNaming.NormalizePathForManifest(outputRootPath, Environment.CurrentDirectory),
            PlanArtifactPath = NativeAotArtifactNames.LoweringPlan,
            TranslationUnitPageSize = loweringPlan.TranslationUnitPageSize,
            TranslationUnitPageCount = loweringPlan.TranslationUnitPageCount,
            TranslationUnitPages = loweringPlan.TranslationUnitPages,
            GeneratedArtifacts = generatedArtifacts,
        };

        return new NativeAotResult
        {
            OutputRootPath = outputRootPath,
            LoweringPlan = loweringPlan,
            Manifest = manifest,
            CodegenMetrics = codegenMetrics,
            GeneratedSources = generatedSources,
        };
    }

    private static void ValidateLoweringPlan(
        NativeAotLoweringPlanArtifact loweringPlan,
        ManagedClosureManifestArtifact closureManifest)
    {
        RequireStringField(loweringPlan.PlanKind, nameof(loweringPlan.PlanKind));
        RequireStringField(loweringPlan.AssemblyName, nameof(loweringPlan.AssemblyName));
        RequireStringField(loweringPlan.EntrySubjectId, nameof(loweringPlan.EntrySubjectId));
        RequireStringField(loweringPlan.EntrySymbol, nameof(loweringPlan.EntrySymbol));
        RequireStringField(loweringPlan.EntryMethodToken, nameof(loweringPlan.EntryMethodToken));
        RequireStringField(loweringPlan.WorkloadAbi, nameof(loweringPlan.WorkloadAbi));

        if (string.Equals(loweringPlan.PlanKind, "full-assembly-entry", StringComparison.Ordinal))
        {
            // Full assembly mode: NativeEntryFunctionName is intentionally empty,
            // WorkloadAbi is "full-assembly", and entry is synthetic (not in closureManifest).
            return;
        }

        RequireStringField(loweringPlan.NativeEntryFunctionName, nameof(loweringPlan.NativeEntryFunctionName));

        if (!string.Equals(loweringPlan.PlanKind, "generic-managed-entry", StringComparison.Ordinal))
        {
            throw new NotSupportedException(
                $"unsupported native-aot lowering plan kind '{loweringPlan.PlanKind}'");
        }

        if (loweringPlan.WorkloadAbi is not ("int(int32)" or "void(int32)" or "int()" or "void()"))
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

    private static (AotCoreIrArtifact AotCoreIr, ManagedClosureManifestArtifact ClosureManifest,
        MetadataRegistrationArtifact MetadataRegistration, SupplementalMetadataTemplateArtifact SupplementalMetadataTemplate)
        LoadClosureArtifacts(string managedClosureRoot)
    {
        var aotCoreIrPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.AotCoreIr);
        var closureManifestPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.ClosureManifest);
        var metadataRegistrationPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.MetadataRegistration);
        var supplementalMetadataTemplatePath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.SupplementalMetadataTemplate);

        AotCoreIrArtifact? aotCoreIr = null;
        ManagedClosureManifestArtifact? closureManifest = null;
        MetadataRegistrationArtifact? metadataRegistration = null;
        SupplementalMetadataTemplateArtifact? supplementalMetadataTemplate = null;
        Exception? loadException = null;
        var loadLock = new object();

        System.Threading.Tasks.Parallel.Invoke(
            () => { try { aotCoreIr = LoadRequiredJson<AotCoreIrArtifact>(aotCoreIrPath); } catch (Exception ex) { lock (loadLock) { loadException ??= ex; } } },
            () => { try { closureManifest = LoadRequiredJson<ManagedClosureManifestArtifact>(closureManifestPath); } catch (Exception ex) { lock (loadLock) { loadException ??= ex; } } },
            () => { try { metadataRegistration = LoadRequiredJson<MetadataRegistrationArtifact>(metadataRegistrationPath); } catch (Exception ex) { lock (loadLock) { loadException ??= ex; } } },
            () => { try { supplementalMetadataTemplate = LoadRequiredJson<SupplementalMetadataTemplateArtifact>(supplementalMetadataTemplatePath); } catch (Exception ex) { lock (loadLock) { loadException ??= ex; } } });

        if (loadException is not null)
        {
            throw new InvalidOperationException("failed to load one or more required closure artifacts", loadException);
        }

        return (aotCoreIr!, closureManifest!, metadataRegistration!, supplementalMetadataTemplate!);
    }

    private static string BuildGeneratedTranslationUnit(NativeAotTemplateModel templateModel)
    {
        return BuildGeneratedPage(templateModel, templateModel.Methods, includeRegistration: true, includeObjectModel: true);
    }

    /// <summary>
    /// Builds content for a single translation unit page containing a subset of methods.
    /// </summary>
    /// <param name="templateModel">Full template model from the planner.</param>
    /// <param name="pageMethods">Methods to include in this page.</param>
    /// <param name="includeRegistration">True to include module/generic registration code
    /// (only in the first page, to avoid duplicate symbol definitions).</param>
    /// <param name="includeObjectModel">True to include TypeInfoV0 inline definitions
    /// (only in the first page, to avoid duplicate definitions; other pages reference
    /// the shared header for extern declarations).</param>
    private static string BuildGeneratedPage(
        NativeAotTemplateModel templateModel,
        IReadOnlyList<NativeAotMethodTemplateModel> pageMethods,
        bool includeRegistration,
        bool includeObjectModel)
    {
        var objectModelSection = BuildObjectModelSection(templateModel);
        var methodSections = pageMethods
            .Select(BuildMethodSection)
            .ToArray();

        // When TypeInfoV0 inline definitions are excluded from this page, add
        // the shared header include so extern declarations are available.
        IReadOnlyList<string> includes = templateModel.Includes;
        if (!includeObjectModel && !string.IsNullOrEmpty(templateModel.TypeDeclarationsCode))
        {
            var extended = new List<string>(includes);
            extended.Add("\"generated/native-aot.generated.header.h\"");
            includes = extended;
        }

        var model = new ScriptObject
        {
            ["includes"] = includes,
            ["object_model_section"] = includeObjectModel ? objectModelSection : "",
            ["method_declarations"] = templateModel.MethodDeclarations,
            ["method_sections"] = methodSections,
            ["entry_subject_id"] = templateModel.EntrySubjectId,
            ["entry_symbol"] = templateModel.EntrySymbol,
            ["entry_native_symbol"] = templateModel.EntryNativeSymbol,
            ["native_entry_function_name"] = templateModel.NativeEntryFunctionName,
            ["entry_bridge_arguments"] = templateModel.EntryBridgeArguments,
            ["generic_registration_code"] = includeRegistration
                ? ScribanTemplateRenderer.NormalizeIndentation(templateModel.GenericRegistrationCode)
                : "",
            ["module_registration_code"] = includeRegistration
                ? ScribanTemplateRenderer.NormalizeIndentation(templateModel.ModuleRegistrationCode)
                : "",
            [NativeAotTemplateCatalog.TranslationUnitNamespaceProperty] = templateModel.CodegenNamespace,
            ["global_declarations"] = templateModel.GlobalDeclarations,
            ["workload_abi"] = templateModel.WorkloadAbi,
        };
        return ScribanTemplateRenderer.RenderTemplate(NativeAotTemplateCatalog.GetTranslationUnitTemplate(), model);
    }

    private static string BuildObjectModelSection(NativeAotTemplateModel templateModel)
    {
        var model = new ScriptObject
        {
            ["object_model_code"] = ScribanTemplateRenderer.NormalizeIndentation(templateModel.ObjectModelCode),
        };
        return ScribanTemplateRenderer.RenderTemplate(NativeAotTemplateCatalog.GetObjectModelTemplate(), model);
    }

    /// <summary>
    /// Builds the shared header content emitted as native-aot.generated.header.h.
    /// Contains extern TypeInfoV0 declarations so every translation unit page has
    /// access to all type symbols without ODR violations from duplicate inline defs.
    /// </summary>
    private static string BuildSharedHeader(NativeAotTemplateModel templateModel)
    {
        return templateModel.TypeDeclarationsCode;
    }

    /// <summary>
    /// Builds the generated sources list. If the lowering plan has page information,
    /// splits the methods across multiple translation unit pages. Otherwise produces
    /// a single translation unit (traditional behavior).
    /// </summary>
    private static (IReadOnlyList<NativeAotGeneratedSource> Sources, IReadOnlyList<NativeAotGeneratedArtifactRef> Artifacts)
        BuildGeneratedSources(NativeAotTemplateModel templateModel, NativeAotLoweringPlanArtifact loweringPlan)
    {
        var sources = new List<NativeAotGeneratedSource>();
        var artifacts = new List<NativeAotGeneratedArtifactRef>();

        // Always include the shape dispatch header
        sources.Add(new NativeAotGeneratedSource
        {
            RelativePath = NativeAotArtifactNames.ShapeDispatchHeader,
            Contents = templateModel.ShapeDispatchHeaderContent,
        });
        artifacts.Add(new NativeAotGeneratedArtifactRef
        {
            Kind = "shapeDispatchHeader",
            Path = NativeAotArtifactNames.ShapeDispatchHeader,
        });

        // Always emit enum metadata header — the generated C++ unconditionally
        // includes it (#include "enum_metadata.generated.h"). Families with no
        // enum types produce an empty file, which is valid C++.
        sources.Add(new NativeAotGeneratedSource
        {
            RelativePath = NativeAotArtifactNames.EnumMetadataHeader,
            Contents = templateModel.EnumMetadataHeaderContent,
        });
        artifacts.Add(new NativeAotGeneratedArtifactRef
        {
            Kind = "enumMetadataHeader",
            Path = NativeAotArtifactNames.EnumMetadataHeader,
        });

        var pages = loweringPlan.TranslationUnitPages;
        if (pages is { Count: > 0 } && loweringPlan.TranslationUnitPageSize is > 0)
        {
            // Paged output: split methods across multiple translation units
            int pageSize = loweringPlan.TranslationUnitPageSize!.Value;
            var allMethods = templateModel.Methods;

            // Emit shared header with extern TypeInfoV0 declarations + shape dispatch header
            bool hasTypeDeclarations = !string.IsNullOrEmpty(templateModel.TypeDeclarationsCode);
            if (hasTypeDeclarations)
            {
                string headerContent = BuildSharedHeader(templateModel);
                sources.Add(new NativeAotGeneratedSource
                {
                    RelativePath = NativeAotArtifactNames.GeneratedHeader,
                    Contents = headerContent,
                });
                artifacts.Add(new NativeAotGeneratedArtifactRef
                {
                    Kind = "generatedHeader",
                    Path = NativeAotArtifactNames.GeneratedHeader,
                });
            }

            for (int i = 0; i < pages.Count; i++)
            {
                var page = pages[i];
                var pageMethods = allMethods
                    .Skip(i * pageSize)
                    .Take(pageSize)
                    .ToArray();

                string content = BuildGeneratedPage(
                    templateModel, pageMethods,
                    includeRegistration: i == 0,
                    includeObjectModel: i == 0 || !hasTypeDeclarations);

                sources.Add(new NativeAotGeneratedSource
                {
                    RelativePath = page.Path,
                    Contents = content,
                });
                artifacts.Add(new NativeAotGeneratedArtifactRef
                {
                    Kind = "generatedTranslationUnit",
                    Path = page.Path,
                });
            }
        }
        else
        {
            // Single translation unit (traditional behavior)
            string content = BuildGeneratedTranslationUnit(templateModel);
            sources.Add(new NativeAotGeneratedSource
            {
                RelativePath = NativeAotArtifactNames.GeneratedTranslationUnit,
                Contents = content,
            });
            artifacts.Add(new NativeAotGeneratedArtifactRef
            {
                Kind = "generatedTranslationUnit",
                Path = NativeAotArtifactNames.GeneratedTranslationUnit,
            });
        }

        // A1 + A2: Emit typed dispatch table header and dispatch wiring source.
        // These are always emitted when the planner produced non-empty content.
        if (!string.IsNullOrEmpty(templateModel.GeneratedModuleHeaderContent))
        {
            sources.Add(new NativeAotGeneratedSource
            {
                RelativePath = NativeAotArtifactNames.GeneratedModuleHeader,
                Contents = templateModel.GeneratedModuleHeaderContent,
            });
            artifacts.Add(new NativeAotGeneratedArtifactRef
            {
                Kind = "generatedModuleHeader",
                Path = NativeAotArtifactNames.GeneratedModuleHeader,
            });
        }
        if (!string.IsNullOrEmpty(templateModel.GeneratedModuleSourceContent))
        {
            sources.Add(new NativeAotGeneratedSource
            {
                RelativePath = NativeAotArtifactNames.GeneratedModuleSource,
                Contents = templateModel.GeneratedModuleSourceContent,
            });
            artifacts.Add(new NativeAotGeneratedArtifactRef
            {
                Kind = "generatedModuleSource",
                Path = NativeAotArtifactNames.GeneratedModuleSource,
            });
        }
        if (!string.IsNullOrEmpty(templateModel.ManifestJson))
        {
            sources.Add(new NativeAotGeneratedSource
            {
                // Use MethodsManifest (not Manifest) to avoid being overwritten by
                // the artifact manifest (NativeAotManifestArtifact) written at
                // NativeAotArtifactNames.Manifest in ConvertToCppHandler.EmitNativeAot().
                RelativePath = NativeAotArtifactNames.MethodsManifest,
                Contents = templateModel.ManifestJson,
            });
            // No artifact ref — this is consumed internally by the verification orchestrator.
        }

        return (sources, artifacts);
    }

    private static string BuildMethodSection(NativeAotMethodTemplateModel methodModel)
    {
        var model = new ScriptObject
        {
            ["subject_id"] = methodModel.SubjectId,
            ["method_source"] = ScribanTemplateRenderer.NormalizeIndentation(methodModel.MethodSource),
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
        var builder = new StringBuilder(Math.Max(value.Length * 2 + 2, 256));
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
}
