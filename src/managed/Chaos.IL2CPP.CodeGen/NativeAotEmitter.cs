using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

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
        IReadOnlyList<NativeAotGeneratedSource> generatedSources;
        IReadOnlyList<NativeAotGeneratedArtifactRef> generatedArtifacts;

        if (string.Equals(loweringPlan.PlanKind, "full-assembly-entry", StringComparison.Ordinal))
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

            ValidateLoweringPlan(loweringPlan, closureManifest!);
            var entryMethod = LoadEntryMethod(aotCoreIr!, loweringPlan.EntrySubjectId);
            var templateModel = new NativeAotLoweringPlanner().Create(
                loweringPlan,
                aotCoreIr!,
                entryMethod,
                closureManifest!,
                metadataRegistration!,
                supplementalMetadataTemplate!,
                fullAssemblyMode: true);
            generatedSources =
            [
                new NativeAotGeneratedSource
                {
                    RelativePath = NativeAotArtifactNames.GeneratedTranslationUnit,
                    Contents = BuildGeneratedTranslationUnit(templateModel),
                },
                new NativeAotGeneratedSource
                {
                    RelativePath = NativeAotArtifactNames.ShapeDispatchHeader,
                    Contents = templateModel.ShapeDispatchHeaderContent,
                },
            ];
            generatedArtifacts = generatedSources
                .Select(generatedSource => new NativeAotGeneratedArtifactRef
                {
                    Kind = generatedSource.RelativePath.EndsWith(".h", StringComparison.Ordinal) ? "shapeDispatchHeader" : "generatedTranslationUnit",
                    Path = generatedSource.RelativePath,
                })
                .ToList();
        }
        else
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

            ValidateLoweringPlan(loweringPlan, closureManifest!);
            var entryMethod = LoadEntryMethod(aotCoreIr!, loweringPlan.EntrySubjectId);
            var templateModel = new NativeAotLoweringPlanner().Create(
                loweringPlan,
                aotCoreIr!,
                entryMethod,
                closureManifest!,
                metadataRegistration!,
                supplementalMetadataTemplate!);
            generatedSources =
            [
                new NativeAotGeneratedSource
                {
                    RelativePath = NativeAotArtifactNames.GeneratedTranslationUnit,
                    Contents = BuildGeneratedTranslationUnit(templateModel),
                },
                new NativeAotGeneratedSource
                {
                    RelativePath = NativeAotArtifactNames.ShapeDispatchHeader,
                    Contents = templateModel.ShapeDispatchHeaderContent,
                },
            ];
            generatedArtifacts = generatedSources
                .Select(generatedSource => new NativeAotGeneratedArtifactRef
                {
                    Kind = generatedSource.RelativePath.EndsWith(".h", StringComparison.Ordinal) ? "shapeDispatchHeader" : "generatedTranslationUnit",
                    Path = generatedSource.RelativePath,
                })
                .ToList();
        }

        var codegenMetrics = NativeCodegenMetricsBuilder.Build(
            "native-aot",
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
                System.Console.WriteLine($"[NativeCodegenValidator] {generatedSource.RelativePath}: WARNING: {warning}");
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
            ManagedClosureRootPath = ManagedNaming.NormalizePathForManifest(managedClosureRoot, Environment.CurrentDirectory),
            PlanArtifactPath = NativeAotArtifactNames.LoweringPlan,
            TranslationUnitPageSize = loweringPlan.TranslationUnitPageSize,
            TranslationUnitPageCount = loweringPlan.TranslationUnitPageCount,
            TranslationUnitPages = loweringPlan.TranslationUnitPages,
            GeneratedArtifacts = generatedArtifacts,
        };

        return new NativeAotResult
        {
            OutputRootPath = request.OutputRootPath,
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
            ["generic_registration_code"] = ScribanTemplateRenderer.NormalizeIndentation(templateModel.GenericRegistrationCode),
            ["module_registration_code"] = ScribanTemplateRenderer.NormalizeIndentation(templateModel.ModuleRegistrationCode),
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
}
