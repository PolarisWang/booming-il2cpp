using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Text.RegularExpressions;
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
        CodegenMode mode = CodegenMode.Aot,
        List<string>? subjectMethods = null,
        string? goldProfilePath = null,
        IReadOnlyDictionary<string, ManagedMethodModel>? allManagedMethods = null)
    {
        ValidateLoweringPlan(loweringPlan, closureManifest);
        var entryMethod = LoadEntryMethod(aotCoreIr, loweringPlan.EntrySubjectId);

        bool isFullAssembly = string.Equals(loweringPlan.PlanKind, "full-assembly-entry", StringComparison.Ordinal);
        var planner = new NativeAotLoweringPlanner();

        // Gold Direct Link: load PGO profile for hot method direct calls
        if (!string.IsNullOrEmpty(goldProfilePath))
        {
            planner.LoadGoldDirectCallProfile(goldProfilePath);
        }

        // In JIT mode, set the .jdata output path so AotCoreIr JSON is written
        // to a binary file instead of embedded C++ string literals.
        if (mode.HasFlag(CodegenMode.Jit))
        {
            var jitDataPath = Path.Combine(outputRootPath, "aot-core-ir.jdata");
            planner.SetJitDataOutputPath(jitDataPath);
        }

        var templateModel = planner.Create(
            loweringPlan,
            aotCoreIr,
            entryMethod,
            closureManifest,
            metadataRegistration,
            supplementalMetadataTemplate,
            fullAssemblyMode: isFullAssembly,
            mode: mode,
            subjectMethods: subjectMethods,
            allManagedMethods: allManagedMethods);

        // In JIT mode, set the .jdata output path so AotCoreIr JSON is written
        // to a binary file instead of embedded C++ string literals.
        if (mode.HasFlag(CodegenMode.Jit))
        {
            var jitDataPath = Path.Combine(outputRootPath, "aot-core-ir.jdata");
            planner.SetJitDataOutputPath(jitDataPath);
        }

        var (generatedSources, generatedArtifacts) = BuildGeneratedSources(
            templateModel, loweringPlan);

        var codegenMetrics = NativeCodegenMetricsBuilder.Build(
            "native-aot",
            loweringPlan.PlanKind,
            generatedSources.Select(generatedSource => (generatedSource.RelativePath, generatedSource.Contents)),
            planner.StructuredMethodCount,
            planner.StructuredExceptionBodyCount,
            planner.TotalMethodCount,
            planner.AotReachableMethodCount,
            planner?.AotUnreachableMethodCount ?? 0,
            planner.HotpatchEntryCount,
            planner.HotpatchEligibleMethodCount);

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
        var methodSections = pageMethods
            .Select(BuildMethodSection)
            .ToArray();

        var includes = new List<string>(templateModel.Includes);

        // ── Direct StringBuilder path for large ObjectModelCode ────────
        // When ObjectModelCode exceeds ~200 KB (e.g. 50+ MB in subject mode),
        // bypass Scriban entirely to avoid OOM from intermediary string copies.
        // The Scriban translation unit template is a simple interleaving of
        // sections with fixed C++ boilerplate — trivial to replicate with
        // StringBuilder, and avoids the 2-3x memory multiplier from Scriban's
        // TemplateContext + string copies of 80+ MB of content.
        int objectModelLength = templateModel.ObjectModelCodeBuilder?.Length ?? templateModel.ObjectModelCode?.Length ?? 0;
        if (includeObjectModel && objectModelLength > 200_000)
        {
            return BuildGeneratedPageDirect(templateModel, methodSections, includes, includeRegistration);
        }

        var objectModelSection = BuildObjectModelSection(templateModel);

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
            ["global_declarations"] = includeRegistration
                ? templateModel.GlobalDeclarations + templateModel.EntryFunctionCode
                : "",
            ["workload_abi"] = templateModel.WorkloadAbi,
        };
        var _rendered = ScribanTemplateRenderer.RenderTemplate(NativeAotTemplateCatalog.GetTranslationUnitTemplate(), model);
        var _sb = new StringBuilder(_rendered);
        AddExternalRuntimeStubs(_sb);
        FixFallbackZeroArgCalls(_sb);
        return _sb.ToString();
    }

    /// <summary>
    /// Shared skeleton for all page builders: includes, boilerplate, namespace,
    /// and method declarations.  Returns a pre-sized StringBuilder ready for
    /// page-specific content (object model, registration, method sections).
    /// </summary>
    private static StringBuilder BuildGeneratedPageSkeleton(
        NativeAotTemplateModel templateModel,
        List<string> includes,
        bool includeRegistration,
        string? perPageTypeDeclarations = null,
        IReadOnlyList<string>? perPageIncludes = null)
    {
        int objectModelLength = templateModel.ObjectModelCodeBuilder?.Length
                             ?? templateModel.ObjectModelCode?.Length
                             ?? 0;
        int estimatedSize = objectModelLength
                          + (templateModel.ModuleRegistrationCode?.Length ?? 0)
                          + (templateModel.GenericRegistrationCode?.Length ?? 0)
                          + (templateModel.GlobalDeclarations?.Length ?? 0)
                          + (includeRegistration ? templateModel.EntryFunctionCode?.Length ?? 0 : 0)
                          + (templateModel.MethodDeclarations?.Sum(static d => d.Length + 1) ?? 0)
                          + 51200;
        var sb = new StringBuilder(Math.Max(estimatedSize, 8192));

        // Includes — use per-page filtered includes when available, else full set
        var effectiveIncludes = perPageIncludes ?? includes;
        foreach (var include in effectiveIncludes)
        {
            sb.Append("#include ");
            sb.Append(include);
            sb.Append('\n');
        }

        // Use per-page type declarations when available (optimization: only declare
        // types referenced by methods on this page). Fall back to shared header when
        // per-page declarations are not provided.
        if (perPageTypeDeclarations is { Length: > 0 })
        {
            sb.Append(perPageTypeDeclarations);
        }
        else
        {
            sb.Append("#include \"native-aot.generated.header.h\"");
        }
        sb.Append("\n\n// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)\n");
        sb.Append("extern \"C\" void InterpreterEntryDirect(\n");
        sb.Append("    CHAOS_IL2CPP_UINTPTR method_key,\n");
        sb.Append("    void*     args_buf,\n");
        sb.Append("    void*     ret_buf) noexcept;\n");
        sb.Append("\n#pragma warning(push)\n");
        sb.Append("#pragma warning(disable: 4065 4244)\n");
        sb.Append("#ifdef __GNUC__\n");
        sb.Append("#pragma GCC diagnostic push\n");
        sb.Append("#pragma GCC diagnostic ignored \"-Wunused-variable\"\n");
        sb.Append("#endif\n");
        sb.Append("\nnamespace chaos::il2cpp::codegen::");
        sb.Append(templateModel.CodegenNamespace);
        sb.Append(" {\n\n");
        sb.Append("// Bring runtime_core and jit declarations into scope for unqualified lookup\n");
        sb.Append("using namespace chaos::il2cpp::runtime_core;\n");
        sb.Append("using namespace chaos::il2cpp::jit;\n\n");

        // Method declarations — included only on page 0 (first page).
        // Including them on every page multiplies memory by the page count,
        // causing OOM when there are 1000+ pages for subject-mode chunks.
        if (includeRegistration && templateModel.MethodDeclarations is { Count: > 0 })
        {
            foreach (var decl in templateModel.MethodDeclarations)
            {
                sb.Append(decl);
                sb.Append('\n');
            }
            sb.Append('\n');
        }

        return sb;
    }

    /// <summary>
    /// Build a complete translation unit page into a StringBuilder.  Used when the
    /// ObjectModelCodeBuilder is available, avoiding the 2x memory overhead of
    /// calling ToString() on a multi-GB StringBuilder — the page StringBuilder is
    /// stored in <see cref="NativeAotGeneratedSource.ContentsBuilder"/> and written
    /// to disk via <c>GetChunks()</c> without ever creating a single large string.
    /// </summary>
    private static StringBuilder BuildGeneratedPageToBuilder(
        NativeAotTemplateModel templateModel,
        string[] methodSections,
        List<string> includes,
        bool includeRegistration,
        bool includeObjectModel,
        string? perPageTypeDeclarations = null,
        IReadOnlyList<string>? perPageIncludes = null)
    {
        var sb = BuildGeneratedPageSkeleton(templateModel, includes, includeRegistration,
            perPageTypeDeclarations, perPageIncludes);

        // Object model code — the bulk of page 0 content
        if (includeObjectModel)
        {
            if (templateModel.ObjectModelCodeBuilder is { } omBuilder)
            {
                foreach (var chunk in omBuilder.GetChunks())
                    sb.Append(chunk.Span);
            }
            else
            {
                sb.Append(templateModel.ObjectModelCode);
            }
            sb.Append('\n');
        }

        // Module registration + generic registration (page 0 only)
        if (includeRegistration)
        {
            sb.Append(templateModel.ModuleRegistrationCode);
            sb.Append('\n');
            sb.Append(templateModel.GenericRegistrationCode);
            sb.Append('\n');
        }

        // Method sections
        foreach (var section in methodSections)
        {
            sb.Append(section);
            sb.Append("\n\n");
        }

        // Close namespace
        sb.Append("\n}  // namespace chaos::il2cpp::codegen::");
        sb.Append(templateModel.CodegenNamespace);
        sb.Append('\n');
        sb.Append("#ifdef __GNUC__\n");
        sb.Append("#pragma GCC diagnostic pop\n");
        sb.Append("#endif\n");
        sb.Append("#pragma warning(pop)\n\n");

        // Global declarations + entry function code (outside namespace)
        // Emitted only on the first page to avoid multiple-definition linker errors.
        if (includeRegistration)
        {
            sb.Append(templateModel.GlobalDeclarations);
            sb.Append(templateModel.EntryFunctionCode);
        }

	AddExternalRuntimeStubs(sb);
	FixFallbackZeroArgCalls(sb);
        return sb;
    }

    /// <summary>
    /// Direct StringBuilder version of BuildGeneratedPage, used when the ObjectModelCode
    /// is very large (e.g. 50+ MB for subject-mode coverage chunks). Builds the same
    /// content as the Scriban template NativeAot.TranslationUnit.cpp.scriban but without
    /// the memory overhead of Scriban's TemplateContext, string copies, and regex-based
    /// NormalizeIndentation. This avoids OutOfMemoryException when processing hundreds
    /// of methods with large object model/registration sections.
    /// </summary>
    private static string BuildGeneratedPageDirect(
        NativeAotTemplateModel templateModel,
        string[] methodSections,
        List<string> includes,
        bool includeRegistration)
    {
        var builder = BuildGeneratedPageToBuilder(
            templateModel, methodSections, includes,
            includeRegistration, includeObjectModel: true);
        return builder.ToString().TrimEnd();
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
    /// When there are no type declarations (non-paged families), returns a minimal
    /// valid header so chaos_generated_module.h can include it unconditionally.
    /// </summary>
    private static string BuildSharedHeader(NativeAotTemplateModel templateModel)
    {
        var code = templateModel.TypeDeclarationsCode;
        if (string.IsNullOrEmpty(code))
        {
            return "// Auto-generated by chaos-il2cpp convert-to-cpp\n#pragma once\n";
        }
        return code;
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
        int? pageSize = loweringPlan.TranslationUnitPageSize;

        const int autoPageSize = 150;
        // Auto-paging: when the plan doesn't specify pages but the method count
        // exceeds the threshold, create synthetic pages to split the output into
        // multiple translation units. This prevents oversized generated files
        // for test subjects with many methods (e.g. coverage subjects).
        // Note: the planner may set NativeAotPageSize to an artificially high value
        // (999999) to effectively disable paging for non-subject codegen.  For
        // pipeline subjects (e.g. 49086 methods in a numerics chunk), this value
        // is never exceeded, so no paging is set up.  We force auto-paging here
        // regardless of the planner's pageSize setting when the method count
        // clearly exceeds the auto-page threshold, preventing OOM during Scriban
        // template rendering of a single 200+ MB translation unit.
        if (templateModel.Methods.Count > autoPageSize && pages is not { Count: > 0 })
        {
            int totalMethods = templateModel.Methods.Count;
            int pageCount = (totalMethods + autoPageSize - 1) / autoPageSize;
            var autoPages = new List<AuditTranslationUnitPageArtifact>(pageCount);
            for (int i = 0; i < pageCount; i++)
            {
                string pageSuffix = i == 0 ? "" : $".page{i + 1}";
                autoPages.Add(new AuditTranslationUnitPageArtifact
                {
                    PageNumber = i + 1,
                    MethodCount = Math.Min(autoPageSize, totalMethods - i * autoPageSize),
                    Path = $"native-aot.generated{pageSuffix}.cpp",
                });
            }
            pages = autoPages;
            pageSize = autoPageSize;
        }

        if (pages is { Count: > 0 } && pageSize is > 0)
        {
            // Paged output: split methods across multiple translation units.
            // Uses estimated output size (based on MethodSource.Length) rather than
            // method count to keep each generated .cpp file within reasonable limits.
            var allMethods = templateModel.Methods;
            int totalMethods = allMethods.Count;

            // ── Estimate page 0 overhead ──────────────────────────────
            // Page 0 carries object model (TypeInfoV0 inline defs), method
            // declarations, module registration, and generic registration.
            // Each entry is optional — use empty string length when null.
            int objectModelLength = templateModel.ObjectModelCodeBuilder?.Length
                                 ?? templateModel.ObjectModelCode?.Length
                                 ?? 0;
            int page0Overhead = objectModelLength
                              + (templateModel.MethodDeclarations?.Sum(d => d.Length) ?? 0)
                              + (templateModel.ModuleRegistrationCode?.Length ?? 0)
                              + (templateModel.GenericRegistrationCode?.Length ?? 0)
                              + 500; // Scriban template structural overhead

            // Threshold: ~350 KB of C++ source per page (~50 pages for 3788 methods).
            // Keeps generated files under ~50K lines / ~2 MB to avoid MSVC C1060
            // (compiler out of heap space) which triggers at ~15 MB / 448K lines.
            const int sizeThresholdChars = 350_000;
            const int perMethodOverhead = 200;
            const int perPageOverhead = 500;

            // Emit shared header with extern TypeInfoV0 declarations + shape dispatch header.
            // Always emitted (even empty) so chaos_generated_module.h can include it
            // unconditionally for chaos_valuetype_* forward declaration visibility.
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

            // ── Size-based partitioning ────────────────────────────
            // Accumulate estimated method sizes into pages. Each page gets at
            // least one method (no empty pages). Page 0 accounts for overhead
            // from object model / registration code.
            int methodIdx = 0;
            int actualPageIdx = 0;
            while (methodIdx < totalMethods)
            {
                bool isFirstPage = (actualPageIdx == 0);
                int accumulated = isFirstPage ? page0Overhead : perPageOverhead;
                int pageStart = methodIdx;

                while (methodIdx < totalMethods)
                {
                    int methodSize = (allMethods[methodIdx].MethodSource?.Length ?? 0) + perMethodOverhead;
                    if (accumulated + methodSize > sizeThresholdChars && methodIdx > pageStart)
                        break;
                    accumulated += methodSize;
                    methodIdx++;
                }

                // Ensure at least one method on this page
                if (methodIdx == pageStart)
                    methodIdx = pageStart + 1;

                var pageMethods = allMethods
                    .Skip(pageStart)
                    .Take(methodIdx - pageStart)
                    .ToArray();

                // Pick page artifact or create on-the-fly for overflow pages
                AuditTranslationUnitPageArtifact page;
                if (actualPageIdx < pages.Count)
                {
                    page = pages[actualPageIdx];
                }
                else
                {
                    // Overflow page beyond the originally planned page count.
                    // Use native-aot.page-NNNN.cpp naming to match cmake glob
                    // (native-aot.page-*.cpp) and keep the lowering plan convention.
                    page = new AuditTranslationUnitPageArtifact
                    {
                        PageNumber = actualPageIdx + 1,
                        MethodCount = methodIdx - pageStart,
                        Path = $"native-aot.page-{actualPageIdx + 1:D4}.cpp",
                    };
                }

                // Only page 0 includes inline TypeInfoV0 definitions (includeObjectModel=true).
                // Subsequent pages rely on the shared header (native-aot.generated.header.h)
                // for forward declarations and extern references to type symbols, avoiding
                // multi-GB bloat from duplicating TypeInfoV0 definitions across hundreds of pages.

                // Normalize page path to flat (no directory prefix) so cmake
                // glob patterns match regardless of what the lowering plan uses.
                var pageFileName = Path.GetFileName(page.Path.AsSpan());
                string pageRelativePath = pageFileName.Length > 0 ? pageFileName.ToString() : page.Path;

                // Per-page type declarations and includes (for paging optimization)
                int pageNumber = page.PageNumber;
                string? pageTypeDecl = templateModel.PerPageTypeDeclarations?.GetValueOrDefault(pageNumber);
                IReadOnlyList<string>? pageIncludes = templateModel.PerPageIncludes?.GetValueOrDefault(pageNumber);

                // For page 0 with an ObjectModelCodeBuilder (large content), build
                // directly into a StringBuilder and store as ContentsBuilder to avoid
                // a 3+ GB ToString() copy on the Large Object Heap.
                if (isFirstPage && templateModel.ObjectModelCodeBuilder is { } omBuilder)
                {
                    var methodSections = pageMethods
                        .Select(BuildMethodSection)
                        .ToArray();
                    var includes = new List<string>(pageIncludes ?? templateModel.Includes);
                    // Page 0 includes inline TypeInfoV0 → no shared header needed
                    var pageBuilder = BuildGeneratedPageToBuilder(
                        templateModel, methodSections, includes,
                        includeRegistration: true,
                        includeObjectModel: true,
                        perPageTypeDeclarations: pageTypeDecl,
                        perPageIncludes: pageIncludes);
                    sources.Add(new NativeAotGeneratedSource
                    {
                        RelativePath = pageRelativePath,
                        Contents = "",
                        ContentsBuilder = pageBuilder,
                    });
                }
                else
                {
                    string content = BuildGeneratedPage(
                        templateModel, pageMethods,
                        includeRegistration: isFirstPage,
                        includeObjectModel: isFirstPage);
                    sources.Add(new NativeAotGeneratedSource
                    {
                        RelativePath = pageRelativePath,
                        Contents = content,
                    });
                }
                artifacts.Add(new NativeAotGeneratedArtifactRef
                {
                    Kind = "generatedTranslationUnit",
                    Path = page.Path,
                });

                actualPageIdx++;
            }
        }
        else
        {
            // Single translation unit (traditional behavior)
            // Emit shared header with extern TypeInfoV0 declarations so that
            // chaos_generated_module.h can include it unconditionally.
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

    /// <summary>
    /// Scan generated page content for calls to chaoternal_runtime_* functions
	/// <summary>
	/// Scan generated page content for calls to chaoternal_runtime_* functions
	/// that lack declarations, and prepend static inline stub declarations.
	/// </summary>
	/// <summary>
	/// Replace ChaosExternalRuntimeFallback() zero-arg calls with ChaosExternalRuntimeFallbackDefault().
	/// Catch-all external runtime helpers for void-returning methods (constructors) may emit
	/// ChaosExternalRuntimeFallback() without the required subject_id argument.
	/// </summary>
	private static void FixFallbackZeroArgCalls(StringBuilder sb)
	{
		string text = sb.ToString();
		if (!text.Contains("ChaosExternalRuntimeFallback();"))
			return;
		sb.Replace("ChaosExternalRuntimeFallback();", "ChaosExternalRuntimeFallbackDefault();");
		// Prepend extern declaration so ChaosExternalRuntimeFallbackDefault is visible
		int anchor = text.LastIndexOf("#include");
		anchor = anchor >= 0 ? text.IndexOf('\n', anchor) + 1 : 0;
		sb.Insert(anchor,
			"extern \"C\" CHAOS_IL2CPP_INTPTR ChaosExternalRuntimeFallbackDefault() noexcept;\n");
	}

	private static void AddExternalRuntimeStubs(StringBuilder sb)
	{
		string text = sb.ToString();
		var callRx = new Regex(@"\b(chaos_external_runtime_\w+)\(");
		// Match declarations/definitions: extern "C" / static inline / void return-type
		var declRx = new Regex(@"(?:extern|static inline|\bvoid)\b.*\bchaos_external_runtime_\w+\s*\(", RegexOptions.Multiline);
		var calls = callRx.Matches(text);
		if (calls.Count == 0) return;
		var missing = new HashSet<string>();
		foreach (Match m in calls) missing.Add(m.Groups[1].Value);
		foreach (Match m in declRx.Matches(text))
		{
			string sv = m.Value;
			// Skip declarations with empty parens "()" — they are stubs with
			// wrong arg count and need to be replaced by corrected declarations.
			int nextIdx = m.Index + m.Length;
			if (nextIdx < text.Length && text[nextIdx] == ')') continue;
			foreach (var s in missing.ToList())
				if (sv.Contains(s)) missing.Remove(s);
		}
		if (missing.Count == 0) return;
		var stub = new StringBuilder();
		stub.AppendLine("// ── External runtime stubs (post-emission) ──");
		foreach (var sym in missing.OrderBy(s => s))
		{
			int argCount = 0;
			// Count arguments from first call site
			var callMatch = System.Text.RegularExpressions.Regex.Match(text,
				System.Text.RegularExpressions.Regex.Escape(sym) + "\\(");
			if (callMatch.Success)
			{
				int pos = callMatch.Index + callMatch.Length;
				int depth = 0;
				bool inString = false;
				for (int i = pos; i < text.Length; i++)
				{
					char c = text[i];
					if (c == '"') inString = !inString;
					else if (!inString)
					{
						if (c == '(') depth++;
						else if (c == ')')
						{
							if (depth == 0) { argCount = 1; break; }
							depth--;
						}
						else if (c == ',' && depth == 0) argCount++;
					}
				}
				argCount++;
			}
			stub.Append("extern CHAOS_IL2CPP_INTPTR ");
			stub.Append(sym);
			stub.Append('(');
			for (int i = 0; i < argCount; i++)
			{
				if (i > 0) stub.Append(", ");
				stub.Append("CHAOS_IL2CPP_INTPTR");
			}
			stub.AppendLine(") noexcept;");
		}
		stub.AppendLine();
		string genSrc = sb.ToString();
		int anchor = genSrc.LastIndexOf("#include");
		if (anchor < 0) anchor = 0;
		else {
			int nl = genSrc.IndexOf('\n', anchor);
			if (nl >= 0) anchor = nl + 1;
		}
		sb.Insert(anchor, stub.ToString());

		// Fix existing () declarations: replace with corrected arg counts
		string postText = sb.ToString();
		foreach (var sym in missing.OrderBy(s => s))
		{
			int argCount = 0;
			var callMatch = System.Text.RegularExpressions.Regex.Match(text,
				System.Text.RegularExpressions.Regex.Escape(sym) + "\\(");
			while (callMatch.Success)
			{
				int ls = text.LastIndexOf((char)10, callMatch.Index);
				if (ls < 0) ls = 0;
				string lp = text.Substring(ls, callMatch.Index - ls).TrimStart();
				if (!lp.StartsWith("extern") && !lp.StartsWith("static"))
					break;
				callMatch = callMatch.NextMatch();
			}
			if (callMatch.Success)
			{
				int pos = callMatch.Index + callMatch.Length;
				int depth = 0;
				bool inString = false;
				for (int i = pos; i < text.Length; i++)
				{
					char c = text[i];
					if (c == '"') inString = !inString;
					else if (!inString)
					{
						if (c == '(') depth++;
						else if (c == ')')
						{
							if (depth == 0) { argCount = 1; break; }
							depth--;
						}
						else if (c == ',' && depth == 0) argCount++;
					}
				}
				argCount++;
			}
			string wrongDecl = "extern CHAOS_IL2CPP_INTPTR " + sym + "() noexcept;";
			string wrongDeclC = "extern \"C\" CHAOS_IL2CPP_INTPTR " + sym + "() noexcept;";
			string correctDecl = "extern CHAOS_IL2CPP_INTPTR " + sym + "(";
			for (int i = 0; i < argCount; i++)
			{
				if (i > 0) correctDecl += ", ";
				correctDecl += "CHAOS_IL2CPP_INTPTR";
			}
			correctDecl += ") noexcept;";
			if (postText.Contains(wrongDecl))
				sb.Replace(wrongDecl, correctDecl);
			if (postText.Contains(wrongDeclC))
				sb.Replace(wrongDeclC, correctDecl);
		}

	}


}
