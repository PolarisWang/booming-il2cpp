using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotEmitter
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
        IReadOnlyDictionary<string, ManagedMethodModel>? allManagedMethods = null,
        string? namespaceFilter = null)
    {
        ValidateLoweringPlan(loweringPlan, closureManifest);
        var entryMethod = LoadEntryMethod(aotCoreIr, loweringPlan.EntrySubjectId);

        bool isFullAssembly = string.Equals(loweringPlan.PlanKind, "full-assembly-entry", StringComparison.Ordinal);
        var planner = new NativeAotLoweringPlanner();
        planner.NamespaceFilter = namespaceFilter;

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
            generatedSources.Select(generatedSource => (generatedSource.RelativePath, generatedSource.Contents!)),
            planner.StructuredMethodCount,
            planner.StructuredExceptionBodyCount,
            planner.TotalMethodCount,
            planner.AotReachableMethodCount,
            planner.AotUnreachableMethodCount,
            planner.HotpatchEntryCount,
            planner.HotpatchEligibleMethodCount);

        // Validate generated C++ code against project coding conventions.
        var validator = new Validation.NativeCodegenValidator();
        foreach (var generatedSource in generatedSources)
        {
            var result = validator.ValidateContent(generatedSource.Contents!, generatedSource.RelativePath);
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

        // Load closure artifacts sequentially.  Parallel loading (Parallel.Invoke)
        // uses ThreadPool threads with limited stack (1 MB), which can trigger
        // stack overflows when combined with deep JSON deserialization trees.
        try
        {
            aotCoreIr = LoadRequiredJson<AotCoreIrArtifact>(aotCoreIrPath);
            closureManifest = LoadRequiredJson<ManagedClosureManifestArtifact>(closureManifestPath);
            metadataRegistration = LoadRequiredJson<MetadataRegistrationArtifact>(metadataRegistrationPath);
            supplementalMetadataTemplate = LoadRequiredJson<SupplementalMetadataTemplateArtifact>(supplementalMetadataTemplatePath);
        }
        catch (Exception ex)
        {
            loadException = ex;
        }

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
        // Per-assembly translation units inherit an empty includes list from
        // BuildPerAssemblyLoweringPlan.  Add headers and fixes needed for
        // compilation — avoid chaos_pch.h which causes PCH conflicts.
        if (includes.Count == 0 || (includes.Count == 1 && includes[0] == "\"chaos_pch.h\""))
        {
            includes.Clear();
            includes.Add("<chaos/native_types.h>");
            includes.Add("<interpreter_entry.h>");
            includes.Add("<core/gc_alloc_stubs.h>");
            includes.Add("<gc/gc_bgc_inline.h>");
            includes.Add("<gc/gc_layout.h>");
            includes.Add("<gc/gc_helpers.h>");
        }

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
        // Deduplicate inline constexpr type_id/mt symbols across the entire
        // page.  Flat-merge can produce duplicate definitions of common
        // System.Private.CoreLib types from each merged assembly, causing
        // C2374 redefinition errors.
        string _pageText = _sb.ToString();
        _sb.Clear();
        DeduplicateTypeIdMtSymbols(_pageText, _sb);
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
        // Cross-TU declarations needed by generated code when the PCH isn't used
        // (e.g. per-assembly TUs that don't inherit chaos_pch.h includes).
        sb.Append("#pragma warning(disable: 2362)\n");
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
        sb.Append("#pragma warning(disable: 4065 4244 4172 4789 4335)\n");
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

    /// <summary>
    /// Builds the shared header content emitted as native-aot.generated.header.h.
    /// Contains extern TypeInfoV0 declarations so every translation unit page has
    /// access to all type symbols without ODR violations from duplicate inline defs.
    /// When there are no type declarations (non-paged families), returns a minimal
    /// valid header so chaos_generated_module.h can include it unconditionally.
    /// </summary>

    /// <summary>
    /// Builds the generated sources list. If the lowering plan has page information,
    /// splits the methods across multiple translation unit pages. Otherwise produces
    /// a single translation unit (traditional behavior).
    /// </summary>

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


}
