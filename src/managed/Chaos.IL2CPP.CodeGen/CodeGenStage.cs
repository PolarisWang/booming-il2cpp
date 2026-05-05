using Chaos.IL2CPP.Contracts;
using System.Reflection;

namespace Chaos.IL2CPP.CodeGen;

public sealed class CodeGenStage
{
    private const int AuditTranslationUnitPageSize = 1024;

    public string Name => "CodeGen";

    public ManagedClosureResult Generate(
        ManagedClosureRequest request,
        LinkedWorldModel linkedWorld,
        MetadataWriterOutput metadataWriterOutput)
    {
        var lookups = BuildStageLookups(linkedWorld);
        var methodShapes = lookups.MethodShapes;
        var methodCapabilities = lookups.MethodCapabilities;
        var internalAssemblyNames = lookups.InternalAssemblyNames;
        var methodsBySubjectId = lookups.MethodsBySubjectId;
        var typedIl = new TypedIlIrArtifact
        {
            Methods = linkedWorld.Methods
                .Select(method => ToTypedIlMethodArtifact(
                    method,
                    methodShapes,
                    methodCapabilities,
                    internalAssemblyNames,
                    methodsBySubjectId))
                .ToList(),
        };
        var codeRegistration = new CodeRegistrationArtifact
        {
            Modules = linkedWorld.Assemblies
                .Select(assembly => new CodeRegistrationModule
                {
                    ModuleName = $"{assembly.Name}.dll",
                    Registrations = linkedWorld.Methods
                        .Where(method => string.Equals(method.AssemblyName, assembly.Name, StringComparison.Ordinal))
                        .Select((method, index) => new CodeRegistrationEntry
                        {
                            RegistrationKind = "methodPointer",
                            Slot = index,
                            Symbol = ManagedNaming.CreateMethodSymbol(method),
                            SubjectId = method.SubjectId,
                        })
                        .ToList(),
                })
                .ToList(),
            TypeCapabilities = BuildCodeRegistrationTypeCapabilities(metadataWriterOutput.MetadataRegistration),
        };
        var aotCoreIr = new AotCoreIrLowering().Create(linkedWorld, typedIl, codeRegistration);
        var genericInstantiationDemandGraph = linkedWorld.GenericInstantiationDemandGraph
            ?? new GenericInstantiationDemandGraphModel
            {
                Demands = [],
            };
        var genericCapabilityMatrix = new GenericCapabilityMatrixBuilder().Build(
            ResolveOwnerSubjectId(request.InputAssemblyPath, linkedWorld.Assembly.Name),
            linkedWorld.EntryPointSubjectId,
            genericInstantiationDemandGraph,
            aotCoreIr,
            metadataWriterOutput.SupplementalMetadataTemplate);
        NativeReferenceLoweringPlanArtifact nativeReferenceLoweringPlan;
        if (linkedWorld.FullAssemblyClosure && string.IsNullOrWhiteSpace(linkedWorld.EntryPointSubjectId))
        {
            nativeReferenceLoweringPlan = CreateAssemblyFullClosureNativeReferenceRuntimeSkeletonPlan(linkedWorld, codeRegistration);
        }
        else
        {
            var loweringPlanner = new NativeReferenceLoweringPlanner();
            try
            {
                nativeReferenceLoweringPlan = loweringPlanner.Create(
                    linkedWorld,
                    typedIl,
                    metadataWriterOutput.MetadataRegistration,
                    codeRegistration);
            }
            catch when (ShouldFallbackToGenericLoweringPlan(linkedWorld))
            {
                nativeReferenceLoweringPlan = CreateGenericLoweringPlan(linkedWorld, codeRegistration);
            }
        }

        var nativeAotLoweringPlan = CreateNativeAotLoweringPlan(
            linkedWorld,
            metadataWriterOutput.MetadataRegistration,
            codeRegistration);

        var closureManifest = new ManagedClosureManifestArtifact
        {
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            InputAssemblyPath = ManagedNaming.NormalizePathForManifest(request.InputAssemblyPath, Environment.CurrentDirectory),
            AdditionalAssemblyPaths = request.AdditionalAssemblyPaths?
                .Where(path => !string.IsNullOrWhiteSpace(path))
                .Select(path => ManagedNaming.NormalizePathForManifest(path, Environment.CurrentDirectory))
                .Distinct(StringComparer.OrdinalIgnoreCase)
                .ToList(),
            ResolvedAssemblies = BuildResolvedAssemblies(request, linkedWorld),
            FullAssemblyClosure = request.FullAssemblyClosure,
            InputModuleVersionId = linkedWorld.Assembly.ModuleVersionId.ToString(),
            Artifacts =
            [
                new ManagedClosureArtifactRef { Kind = "typedIlIr", Path = ManagedClosureArtifactNames.TypedIlIr },
                new ManagedClosureArtifactRef { Kind = "aotCoreIr", Path = ManagedClosureArtifactNames.AotCoreIr },
                new ManagedClosureArtifactRef { Kind = "aotManifest", Path = ManagedClosureArtifactNames.AotManifest },
                new ManagedClosureArtifactRef { Kind = "metadataRegistration", Path = ManagedClosureArtifactNames.MetadataRegistration },
                new ManagedClosureArtifactRef { Kind = "supplementalMetadataTemplate", Path = ManagedClosureArtifactNames.SupplementalMetadataTemplate },
                new ManagedClosureArtifactRef { Kind = "codeRegistration", Path = ManagedClosureArtifactNames.CodeRegistration },
                new ManagedClosureArtifactRef { Kind = "genericInstantiationDemandGraph", Path = ManagedClosureArtifactNames.GenericInstantiationDemandGraph },
                new ManagedClosureArtifactRef { Kind = "genericCapabilityMatrix", Path = ManagedClosureArtifactNames.GenericCapabilityMatrix },
                new ManagedClosureArtifactRef { Kind = "optimizationFacts", Path = ManagedClosureArtifactNames.OptimizationFacts },
                new ManagedClosureArtifactRef { Kind = "preserveDescriptor", Path = ManagedClosureArtifactNames.PreserveDescriptor },
                new ManagedClosureArtifactRef { Kind = "nativeReferenceLoweringPlan", Path = ManagedClosureArtifactNames.NativeReferenceLoweringPlan },
                new ManagedClosureArtifactRef { Kind = "nativeAotLoweringPlan", Path = ManagedClosureArtifactNames.NativeAotLoweringPlan },
            ],
        };

        return new ManagedClosureResult
        {
            OutputRootPath = request.OutputRootPath,
            TypedIlIr = typedIl,
            AotCoreIr = aotCoreIr,
            AotManifest = metadataWriterOutput.AotManifest,
            MetadataRegistration = metadataWriterOutput.MetadataRegistration,
            SupplementalMetadataTemplate = metadataWriterOutput.SupplementalMetadataTemplate,
            CodeRegistration = codeRegistration,
            GenericInstantiationDemandGraph = genericInstantiationDemandGraph,
            GenericCapabilityMatrix = genericCapabilityMatrix,
            OptimizationFacts = linkedWorld.OptimizationFacts,
            PreserveDescriptor = linkedWorld.PreserveDescriptor,
            NativeReferenceLoweringPlan = nativeReferenceLoweringPlan,
            NativeAotLoweringPlan = nativeAotLoweringPlan,
            ClosureManifest = closureManifest,
        };
    }

    /// <summary>
    /// Generate per-assembly codegen results from a multi-assembly pipeline run.
    /// Each input assembly gets its own filtered AotCoreIr, CodeRegistration, and LoweringPlan.
    /// </summary>
        /// <summary>
    /// Filter a full pipeline result into per-assembly results.
    /// Each input assembly gets its own output root and filtered method set.
    /// </summary>
    public IReadOnlyList<ManagedClosureResult> FilterResultPerAssembly(
        ManagedClosureResult fullResult,
        IReadOnlyList<string> inputAssemblyPaths)
    {
        var results = new List<ManagedClosureResult>();

        foreach (var asmPath in inputAssemblyPaths)
        {
            var asmName = System.IO.Path.GetFileNameWithoutExtension(asmPath);
            var asmOutputRoot = System.IO.Path.Combine(fullResult.OutputRootPath, asmName);
            System.IO.Directory.CreateDirectory(asmOutputRoot);

            // Filter AotCoreIr methods to this assembly
            var assemblyMethods = fullResult.AotCoreIr?.Methods
                .Where(m => m.SubjectId.StartsWith(asmName + "/", System.StringComparison.Ordinal)
                         || m.SubjectId.StartsWith(asmName + ".", System.StringComparison.Ordinal))
                .ToList() ?? [];

            var filteredAotCoreIr = fullResult.AotCoreIr is null ? null : fullResult.AotCoreIr with
            {
                Methods = assemblyMethods,
            };

            results.Add(new ManagedClosureResult
            {
                OutputRootPath = asmOutputRoot,
                TypedIlIr = fullResult.TypedIlIr,
                AotCoreIr = filteredAotCoreIr,
                AotManifest = fullResult.AotManifest,
                MetadataRegistration = fullResult.MetadataRegistration,
                SupplementalMetadataTemplate = fullResult.SupplementalMetadataTemplate,
                CodeRegistration = fullResult.CodeRegistration,
                GenericInstantiationDemandGraph = fullResult.GenericInstantiationDemandGraph,
                GenericCapabilityMatrix = fullResult.GenericCapabilityMatrix,
                OptimizationFacts = fullResult.OptimizationFacts,
                PreserveDescriptor = fullResult.PreserveDescriptor,
                NativeReferenceLoweringPlan = fullResult.NativeReferenceLoweringPlan,
                NativeAotLoweringPlan = fullResult.NativeAotLoweringPlan,
                ClosureManifest = fullResult.ClosureManifest,
            });
        }

        return results;
    }



private static IReadOnlyList<ManagedClosureResolvedAssemblyRef> BuildResolvedAssemblies(
        ManagedClosureRequest request,
        LinkedWorldModel linkedWorld)
    {
        var resolvedPathsByAssemblyName = new Dictionary<string, string>(StringComparer.Ordinal);
        AddResolvedAssemblyPath(
            resolvedPathsByAssemblyName,
            linkedWorld.Assembly.Name,
            request.InputAssemblyPath);

        foreach (var additionalAssemblyPath in request.AdditionalAssemblyPaths ?? [])
        {
            if (string.IsNullOrWhiteSpace(additionalAssemblyPath))
            {
                continue;
            }

            if (TryReadAssemblyName(additionalAssemblyPath, out var additionalAssemblyName))
            {
                AddResolvedAssemblyPath(resolvedPathsByAssemblyName, additionalAssemblyName, additionalAssemblyPath);
            }
        }

        var probeDirectories = EnumerateAssemblyProbeDirectories(request)
            .ToList();
        var trustedPlatformAssemblies = BuildTrustedPlatformAssemblyPathsByName();

        foreach (var assemblyName in CollectClosureReferencedAssemblyNames(linkedWorld))
        {
            if (resolvedPathsByAssemblyName.ContainsKey(assemblyName))
            {
                continue;
            }

            if (TryResolveAssemblyPath(assemblyName, probeDirectories, trustedPlatformAssemblies, out var resolvedAssemblyPath))
            {
                AddResolvedAssemblyPath(resolvedPathsByAssemblyName, assemblyName, resolvedAssemblyPath);
            }
        }

        return resolvedPathsByAssemblyName
            .OrderBy(entry => entry.Key, StringComparer.Ordinal)
            .Select(entry => new ManagedClosureResolvedAssemblyRef
            {
                AssemblyName = entry.Key,
                Path = ManagedNaming.NormalizePathForManifest(entry.Value, Environment.CurrentDirectory),
            })
            .ToList();
    }

    private static IEnumerable<string> CollectClosureReferencedAssemblyNames(LinkedWorldModel linkedWorld)
    {
        var assemblyNames = new HashSet<string>(StringComparer.Ordinal)
        {
            linkedWorld.Assembly.Name,
        };

        foreach (var assembly in linkedWorld.Assemblies)
        {
            if (!string.IsNullOrWhiteSpace(assembly.Name))
            {
                assemblyNames.Add(assembly.Name);
            }
        }

        foreach (var type in linkedWorld.Types)
        {
            if (!string.IsNullOrWhiteSpace(type.AssemblyName))
            {
                assemblyNames.Add(type.AssemblyName);
            }

            TryAddAssemblyNameFromSubjectId(assemblyNames, type.SubjectId);
            TryAddAssemblyNameFromSubjectId(assemblyNames, type.DefinitionSubjectId);
            TryAddAssemblyNameFromSubjectId(assemblyNames, type.BaseTypeSubjectId);
            foreach (var implementedInterfaceSubjectId in type.ImplementedInterfaceSubjectIds ?? [])
            {
                TryAddAssemblyNameFromSubjectId(assemblyNames, implementedInterfaceSubjectId);
            }
        }

        foreach (var field in linkedWorld.Fields)
        {
            if (!string.IsNullOrWhiteSpace(field.AssemblyName))
            {
                assemblyNames.Add(field.AssemblyName);
            }

            TryAddAssemblyNameFromSubjectId(assemblyNames, field.SubjectId);
            TryAddAssemblyNameFromSubjectId(assemblyNames, field.DefinitionSubjectId);
            TryAddAssemblyNameFromSubjectId(assemblyNames, field.DeclaringTypeSubjectId);
        }

        foreach (var property in linkedWorld.Properties)
        {
            if (!string.IsNullOrWhiteSpace(property.AssemblyName))
            {
                assemblyNames.Add(property.AssemblyName);
            }

            TryAddAssemblyNameFromSubjectId(assemblyNames, property.SubjectId);
            TryAddAssemblyNameFromSubjectId(assemblyNames, property.DefinitionSubjectId);
            TryAddAssemblyNameFromSubjectId(assemblyNames, property.DeclaringTypeSubjectId);
        }

        foreach (var method in linkedWorld.Methods)
        {
            if (!string.IsNullOrWhiteSpace(method.AssemblyName))
            {
                assemblyNames.Add(method.AssemblyName);
            }

            TryAddAssemblyNameFromSubjectId(assemblyNames, method.SubjectId);
            TryAddAssemblyNameFromSubjectId(assemblyNames, method.DefinitionSubjectId);
            TryAddAssemblyNameFromSubjectId(assemblyNames, method.DeclaringTypeSubjectId);
            TryAddAssemblyNameFromSubjectId(assemblyNames, method.Import?.ModuleName);

            foreach (var exceptionRegion in method.Body.ExceptionRegions)
            {
                TryAddAssemblyNameFromSubjectId(assemblyNames, exceptionRegion.CatchTypeSubjectId);
            }

            foreach (var block in method.Body.Blocks)
            {
                foreach (var instruction in block.Instructions)
                {
                    if (!string.IsNullOrWhiteSpace(instruction.Reference?.AssemblyName))
                    {
                        assemblyNames.Add(instruction.Reference.AssemblyName);
                    }

                    TryAddAssemblyNameFromSubjectId(assemblyNames, instruction.Reference?.SubjectId);
                    TryAddAssemblyNameFromSubjectId(assemblyNames, instruction.Callee);
                }
            }
        }

        foreach (var dependency in linkedWorld.Dependencies)
        {
            if (!string.IsNullOrWhiteSpace(dependency.AssemblyName))
            {
                assemblyNames.Add(dependency.AssemblyName);
            }

            TryAddAssemblyNameFromSubjectId(assemblyNames, dependency.SubjectId);
        }

        return assemblyNames
            .Where(assemblyName => !string.IsNullOrWhiteSpace(assemblyName))
            .OrderBy(assemblyName => assemblyName, StringComparer.Ordinal);
    }

    private static IEnumerable<string> EnumerateAssemblyProbeDirectories(ManagedClosureRequest request)
    {
        var directories = new HashSet<string>(StringComparer.OrdinalIgnoreCase);

        static void AddDirectory(HashSet<string> target, string? path)
        {
            if (string.IsNullOrWhiteSpace(path))
            {
                return;
            }

            var directory = Path.GetDirectoryName(Path.GetFullPath(path));
            if (!string.IsNullOrWhiteSpace(directory))
            {
                target.Add(directory);
            }
        }

        AddDirectory(directories, request.InputAssemblyPath);
        foreach (var additionalAssemblyPath in request.AdditionalAssemblyPaths ?? [])
        {
            AddDirectory(directories, additionalAssemblyPath);
        }

        return directories;
    }

    private static IReadOnlyDictionary<string, string> BuildTrustedPlatformAssemblyPathsByName()
    {
        var trustedPlatformAssemblies = new Dictionary<string, string>(StringComparer.Ordinal);
        if (AppContext.GetData("TRUSTED_PLATFORM_ASSEMBLIES") is not string trustedPlatformAssembliesValue ||
            string.IsNullOrWhiteSpace(trustedPlatformAssembliesValue))
        {
            return trustedPlatformAssemblies;
        }

        foreach (var trustedPlatformAssemblyPath in trustedPlatformAssembliesValue.Split(Path.PathSeparator, StringSplitOptions.RemoveEmptyEntries))
        {
            var assemblyName = Path.GetFileNameWithoutExtension(trustedPlatformAssemblyPath);
            if (!string.IsNullOrWhiteSpace(assemblyName) &&
                !trustedPlatformAssemblies.ContainsKey(assemblyName))
            {
                trustedPlatformAssemblies[assemblyName] = Path.GetFullPath(trustedPlatformAssemblyPath);
            }
        }

        return trustedPlatformAssemblies;
    }

    private static bool TryResolveAssemblyPath(
        string assemblyName,
        IReadOnlyList<string> probeDirectories,
        IReadOnlyDictionary<string, string> trustedPlatformAssemblies,
        out string resolvedAssemblyPath)
    {
        foreach (var probeDirectory in probeDirectories)
        {
            foreach (var extension in new[] { ".dll", ".exe" })
            {
                var candidatePath = Path.Combine(probeDirectory, assemblyName + extension);
                if (File.Exists(candidatePath))
                {
                    resolvedAssemblyPath = Path.GetFullPath(candidatePath);
                    return true;
                }
            }
        }

        if (trustedPlatformAssemblies.TryGetValue(assemblyName, out var trustedPlatformAssemblyPath))
        {
            resolvedAssemblyPath = trustedPlatformAssemblyPath;
            return true;
        }

        resolvedAssemblyPath = string.Empty;
        return false;
    }

    private static bool TryReadAssemblyName(string assemblyPath, out string assemblyName)
    {
        assemblyName = string.Empty;
        try
        {
            assemblyName = AssemblyName.GetAssemblyName(assemblyPath).Name ?? string.Empty;
            return !string.IsNullOrWhiteSpace(assemblyName);
        }
        catch (Exception) when (
            File.Exists(assemblyPath) &&
            (assemblyPath.EndsWith(".dll", StringComparison.OrdinalIgnoreCase) ||
             assemblyPath.EndsWith(".exe", StringComparison.OrdinalIgnoreCase)))
        {
            return false;
        }
    }

    private static void AddResolvedAssemblyPath(
        IDictionary<string, string> resolvedPathsByAssemblyName,
        string assemblyName,
        string assemblyPath)
    {
        if (string.IsNullOrWhiteSpace(assemblyName) || string.IsNullOrWhiteSpace(assemblyPath))
        {
            return;
        }

        resolvedPathsByAssemblyName[assemblyName] = Path.GetFullPath(assemblyPath);
    }

    private static void TryAddAssemblyNameFromSubjectId(
        ISet<string> assemblyNames,
        string? subjectId)
    {
        if (string.IsNullOrWhiteSpace(subjectId))
        {
            return;
        }

        var separatorIndex = subjectId.IndexOf('/');
        if (separatorIndex > 0)
        {
            assemblyNames.Add(subjectId[..separatorIndex]);
        }
    }

    private static string ResolveOwnerSubjectId(string inputAssemblyPath, string fallbackOwnerSubjectId)
    {
        if (!string.IsNullOrWhiteSpace(inputAssemblyPath))
        {
            var normalizedPath = inputAssemblyPath
                .Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar)
                .Split(Path.DirectorySeparatorChar, StringSplitOptions.RemoveEmptyEntries);
            for (var index = 0; index < normalizedPath.Length - 1; index++)
            {
                if (string.Equals(normalizedPath[index], "subjects", StringComparison.OrdinalIgnoreCase))
                {
                    return normalizedPath[index + 1];
                }
            }
        }

        return fallbackOwnerSubjectId;
    }

    private static bool ShouldFallbackToGenericLoweringPlan(LinkedWorldModel linkedWorld)
    {
        if (linkedWorld.Assemblies.Count > 1 ||
            linkedWorld.PreserveDescriptor.Entries.Count > 0 ||
            linkedWorld.Dependencies.Any(dependency =>
                string.Equals(dependency.Reason, "external-call", StringComparison.Ordinal)))
        {
            return true;
        }

        var entryPointMethod = linkedWorld.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal));
        if (entryPointMethod is null)
        {
            return false;
        }

        return entryPointMethod.IsStatic &&
               entryPointMethod.Parameters.Count == 0 &&
               !string.Equals(entryPointMethod.Name, "Run", StringComparison.Ordinal);
    }

    private static NativeReferenceLoweringPlanArtifact CreateGenericLoweringPlan(
        LinkedWorldModel linkedWorld,
        CodeRegistrationArtifact codeRegistration)
    {
        var entrySymbol = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .FirstOrDefault(registration => string.Equals(registration.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal))
            ?.Symbol
            ?? "analysis_only_entry";

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "generic-analysis-only",
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entrySymbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = "0u",
            ConsoleWriteLineStringIcall = "System.Console/System.Console::WriteLine(System.String)",
        };
    }

    private static NativeReferenceLoweringPlanArtifact CreateAssemblyFullClosureNativeReferenceRuntimeSkeletonPlan(
        LinkedWorldModel linkedWorld,
        CodeRegistrationArtifact codeRegistration)
    {
        var canonicalSubjectIds = BuildCanonicalSubjectIdLookup(linkedWorld.CanonicalSubjects.Subjects);
        var methodSubjectIds = linkedWorld.Methods
            .OrderBy(method => ResolveCanonicalSubjectId(canonicalSubjectIds, method.SubjectId), StringComparer.Ordinal)
            .ThenBy(method => method.SubjectId, StringComparer.Ordinal)
            .Select(method => method.SubjectId)
            .Distinct(StringComparer.Ordinal)
            .ToList();
        var firstMethodSymbol = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .FirstOrDefault(registration => string.Equals(registration.RegistrationKind, "methodPointer", StringComparison.Ordinal))
            ?.Symbol
            ?? $"{linkedWorld.Assembly.Name}_assembly_audit";

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "assembly-full-closure-runtime-skeleton",
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReferenceAssembly",
            EntrySymbol = firstMethodSymbol,
            RuntimeExecutionKind = "assembly-bound-native-reference-skeleton",
            TranslationUnitMode = "runtime-skeleton",
            TranslationUnitMethodSubjectIds = methodSubjectIds,
            TranslationUnitMethodCount = methodSubjectIds.Count,
            TranslationUnitPageSize = AuditTranslationUnitPageSize,
            TranslationUnitPageCount = GetAuditPageCount(methodSubjectIds.Count),
            TranslationUnitPages = BuildAuditTranslationUnitPages(methodSubjectIds, "generated/runtime/native-reference.runtime-skeleton"),
            AuditStatus = "runtime-skeleton",
            AuditMessage = "assembly-bound full-closure native-reference runtime skeleton emits covered executable methods only; uncovered methods are reported separately",
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = "0u",
            ConsoleWriteLineStringIcall = "System.Console/System.Console::WriteLine(System.String)",
        };
    }

    private static NativeAotLoweringPlanArtifact CreateNativeAotLoweringPlan(
        LinkedWorldModel linkedWorld,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationArtifact codeRegistration)
    {
        if (linkedWorld.FullAssemblyClosure && string.IsNullOrWhiteSpace(linkedWorld.EntryPointSubjectId))
        {
            return CreateAssemblyFullClosureNativeAotAuditPlan(linkedWorld, codeRegistration);
        }

        var entrySymbol = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .FirstOrDefault(registration => string.Equals(registration.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal))
            ?.Symbol
            ?? "analysis_only_entry";
        var entryMethodToken = metadataRegistration.Registrations
            .FirstOrDefault(registration =>
                string.Equals(registration.RegistrationKind, "method", StringComparison.Ordinal) &&
                string.Equals(registration.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal))
            ?.SubjectId is null
                ? "0u"
                : FormatCppTokenLiteral(metadataRegistration, linkedWorld.EntryPointSubjectId);

        return new NativeAotLoweringPlanArtifact
        {
            PlanKind = "generic-managed-entry",
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            NativeEntryFunctionName = "RunNativeAot",
            EntrySymbol = entrySymbol,
            EntryMethodToken = entryMethodToken,
            WorkloadAbi = "int(int32)",
        };
    }

    private static NativeAotLoweringPlanArtifact CreateAssemblyFullClosureNativeAotAuditPlan(
        LinkedWorldModel linkedWorld,
        CodeRegistrationArtifact codeRegistration)
    {
        var canonicalSubjectIds = BuildCanonicalSubjectIdLookup(linkedWorld.CanonicalSubjects.Subjects);
        var methodSubjectIds = linkedWorld.Methods
            .OrderBy(method => ResolveCanonicalSubjectId(canonicalSubjectIds, method.SubjectId), StringComparer.Ordinal)
            .ThenBy(method => method.SubjectId, StringComparer.Ordinal)
            .Select(method => method.SubjectId)
            .Distinct(StringComparer.Ordinal)
            .ToList();
        var firstMethodSymbol = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .FirstOrDefault(registration => string.Equals(registration.RegistrationKind, "methodPointer", StringComparison.Ordinal))
            ?.Symbol
            ?? $"{linkedWorld.Assembly.Name}_assembly_audit";

        return new NativeAotLoweringPlanArtifact
        {
            PlanKind = "assembly-full-closure-audit",
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            NativeEntryFunctionName = "RunNativeAotAudit",
            EntrySymbol = firstMethodSymbol,
            EntryMethodToken = "0u",
            WorkloadAbi = "audit-only",
            TranslationUnitMode = "audit-only",
            TranslationUnitMethodSubjectIds = methodSubjectIds,
            TranslationUnitMethodCount = methodSubjectIds.Count,
            TranslationUnitPageSize = AuditTranslationUnitPageSize,
            TranslationUnitPageCount = GetAuditPageCount(methodSubjectIds.Count),
            TranslationUnitPages = BuildAuditTranslationUnitPages(methodSubjectIds, "generated/audit/native-aot.audit", ".json"),
            AuditStatus = "not-yet-emittable",
            AuditMessage = "assembly-bound full-closure native-aot emission is not implemented",
        };
    }

    private static IReadOnlyDictionary<string, string> BuildCanonicalSubjectIdLookup(
        IReadOnlyList<CanonicalSubjectModel> canonicalSubjects)
    {
        return canonicalSubjects
            .GroupBy(subject => subject.SubjectId, StringComparer.Ordinal)
            .ToDictionary(
                group => group.Key,
                group => group.Last().CanonicalSubjectId,
                StringComparer.Ordinal);
    }

    private static string ResolveCanonicalSubjectId(
        IReadOnlyDictionary<string, string> canonicalSubjectIds,
        string subjectId)
    {
        return canonicalSubjectIds.TryGetValue(subjectId, out var canonicalSubjectId)
            ? canonicalSubjectId
            : ManagedNaming.CanonicalizeSubjectId(subjectId);
    }

    private static IReadOnlyList<AuditTranslationUnitPageArtifact> BuildAuditTranslationUnitPages(
        IReadOnlyList<string> methodSubjectIds,
        string pathPrefix,
        string extension = ".cpp")
    {
        var pages = new List<AuditTranslationUnitPageArtifact>();
        for (var pageIndex = 0; pageIndex * AuditTranslationUnitPageSize < methodSubjectIds.Count; pageIndex++)
        {
            var pageItems = methodSubjectIds
                .Skip(pageIndex * AuditTranslationUnitPageSize)
                .Take(AuditTranslationUnitPageSize)
                .ToList();
            pages.Add(new AuditTranslationUnitPageArtifact
            {
                PageNumber = pageIndex + 1,
                MethodCount = pageItems.Count,
                Path = $"{pathPrefix}.page-{pageIndex + 1:D4}{extension}",
                FirstMethodSubjectId = pageItems.FirstOrDefault(),
                LastMethodSubjectId = pageItems.LastOrDefault(),
            });
        }

        return pages;
    }

    private static int GetAuditPageCount(int totalMethodCount)
    {
        if (totalMethodCount <= 0)
        {
            return 0;
        }

        return (totalMethodCount + AuditTranslationUnitPageSize - 1) / AuditTranslationUnitPageSize;
    }

    private static string FormatCppTokenLiteral(
        MetadataRegistrationArtifact metadataRegistration,
        string subjectId)
    {
        var token = metadataRegistration.Registrations
            .FirstOrDefault(registration =>
                string.Equals(registration.RegistrationKind, "method", StringComparison.Ordinal) &&
                string.Equals(registration.SubjectId, subjectId, StringComparison.Ordinal))
            ?.DefinitionSubjectId;

        _ = token; // metadataRegistration is read for validation only; token literal is always "0u" for now
        return "0u";
    }

    private static TypedIlMethodArtifact ToTypedIlMethodArtifact(
        ManagedMethodModel method,
        IReadOnlyDictionary<string, MethodShapeModel> methodShapes,
        IReadOnlyDictionary<string, IReadOnlyList<string>> methodCapabilities,
        IReadOnlySet<string> internalAssemblyNames,
        IReadOnlyDictionary<string, ManagedMethodModel> methodsBySubjectId)
    {
        var methodShape = GetRequiredMethodShape(methodShapes, method.SubjectId);
        var capabilities = GetRequiredCapabilities(methodCapabilities, method.SubjectId);

        return new TypedIlMethodArtifact
        {
            MethodId = ManagedNaming.CreateMethodId(method),
            SubjectId = method.SubjectId,
            Signature = method.Signature,
            Identity = ManagedMethodIdentityResolver.Create(method),
            MethodRole = methodShape.MethodRole,
            BodyAvailability = methodShape.BodyAvailability,
            BodyAvailabilityCode = methodShape.BodyAvailabilityCode,
            Capabilities = capabilities,
            Parameters = method.Parameters.Select(parameter => new TypedIlParameterArtifact
            {
                Name = parameter.Name,
                Type = parameter.Type,
            }).ToList(),
            Blocks = method.Body.Blocks.Select(block => new TypedIlBlockArtifact
            {
                BlockId = block.BlockId,
                Instructions = block.Instructions
                    .Select(instruction => ToTypedIlInstructionArtifact(
                        method,
                        internalAssemblyNames,
                        methodsBySubjectId,
                        instruction))
                    .ToList(),
            }).ToList(),
        };
    }

    private static MethodShapeModel GetRequiredMethodShape(
        IReadOnlyDictionary<string, MethodShapeModel> methodShapes,
        string subjectId)
    {
        if (methodShapes.TryGetValue(subjectId, out var methodShape))
        {
            return methodShape;
        }

        throw new InvalidOperationException(
            $"missing semantic method shape for '{subjectId}' during typed-il generation");
    }

    private static IReadOnlyList<string> GetRequiredCapabilities(
        IReadOnlyDictionary<string, IReadOnlyList<string>> methodCapabilities,
        string subjectId)
    {
        if (methodCapabilities.TryGetValue(subjectId, out var capabilities))
        {
            return capabilities;
        }

        throw new InvalidOperationException(
            $"missing method capability bundle for '{subjectId}' during typed-il generation");
    }

    private static TypedIlInstructionArtifact ToTypedIlInstructionArtifact(
        ManagedMethodModel method,
        IReadOnlySet<string> internalAssemblyNames,
        IReadOnlyDictionary<string, ManagedMethodModel> methodsBySubjectId,
        ManagedInstructionModel instruction)
    {
        return new TypedIlInstructionArtifact
        {
            Op = instruction.Op,
            Operand = instruction.Operand,
            ResultType = instruction.ResultType,
            Callee = instruction.Callee,
            CallSiteSignature = instruction.CallSiteSignature,
            Reference = instruction.Reference,
            DispatchKindCode = HybridDispatchResolver.ResolveInstruction(
                method.AssemblyName,
                internalAssemblyNames,
                instruction,
                methodsBySubjectId),
        };
    }

    private sealed record StageLookups(
        IReadOnlyDictionary<string, MethodShapeModel> MethodShapes,
        IReadOnlyDictionary<string, IReadOnlyList<string>> MethodCapabilities,
        IReadOnlySet<string> InternalAssemblyNames,
        IReadOnlyDictionary<string, ManagedMethodModel> MethodsBySubjectId);

    private static StageLookups BuildStageLookups(LinkedWorldModel linkedWorld)
    {
        var methodShapes = new Dictionary<string, MethodShapeModel>(linkedWorld.SemanticShapes.Methods.Count, StringComparer.Ordinal);
        var methodCapabilities = new Dictionary<string, IReadOnlyList<string>>(linkedWorld.CapabilityBundles.Methods.Count, StringComparer.Ordinal);
        var internalAssemblyNames = new HashSet<string>(linkedWorld.Assemblies.Count, StringComparer.Ordinal);
        var methodsBySubjectId = new Dictionary<string, ManagedMethodModel>(linkedWorld.Methods.Count, StringComparer.Ordinal);

        foreach (var method in linkedWorld.Methods)
        {
            methodsBySubjectId[method.SubjectId] = method;
        }

        foreach (var shape in linkedWorld.SemanticShapes.Methods)
        {
            methodShapes[shape.SubjectId] = shape;
        }

        foreach (var bundle in linkedWorld.CapabilityBundles.Methods)
        {
            methodCapabilities[bundle.SubjectId] = bundle.Capabilities;
        }

        foreach (var assembly in linkedWorld.Assemblies)
        {
            internalAssemblyNames.Add(assembly.Name);
        }

        return new StageLookups(methodShapes, methodCapabilities, internalAssemblyNames, methodsBySubjectId);
    }

    private static IReadOnlyList<CodeRegistrationTypeCapabilityEntry> BuildCodeRegistrationTypeCapabilities(
        MetadataRegistrationArtifact metadataRegistration)
    {
        static CodeRegistrationTypeCapabilityEntry? TryCreate(MetadataRegistrationEntry registration)
        {
            if (!string.Equals(registration.RegistrationKind, "type", StringComparison.Ordinal) ||
                string.IsNullOrWhiteSpace(registration.SubjectId) ||
                string.IsNullOrWhiteSpace(registration.DisplayName) ||
                string.IsNullOrWhiteSpace(registration.DefinitionSubjectId))
            {
                return null;
            }

            var displayName = registration.DisplayName!;
            uint valueSizeBytes;
            uint vectorLaneKind;
            uint scalarKind;
            switch (displayName)
            {
                case "System.Byte":
                    valueSizeBytes = 1u;
                    vectorLaneKind = 1u;
                    scalarKind = 2u;
                    break;
                case "System.SByte":
                    valueSizeBytes = 1u;
                    vectorLaneKind = 1u;
                    scalarKind = 1u;
                    break;
                case "System.Int16":
                    valueSizeBytes = 2u;
                    vectorLaneKind = 1u;
                    scalarKind = 1u;
                    break;
                case "System.UInt16":
                    valueSizeBytes = 2u;
                    vectorLaneKind = 1u;
                    scalarKind = 2u;
                    break;
                case "System.Int32":
                    valueSizeBytes = 4u;
                    vectorLaneKind = 1u;
                    scalarKind = 1u;
                    break;
                case "System.UInt32":
                    valueSizeBytes = 4u;
                    vectorLaneKind = 1u;
                    scalarKind = 2u;
                    break;
                case "System.Int64":
                    valueSizeBytes = 8u;
                    vectorLaneKind = 1u;
                    scalarKind = 1u;
                    break;
                case "System.UInt64":
                    valueSizeBytes = 8u;
                    vectorLaneKind = 1u;
                    scalarKind = 2u;
                    break;
                case "System.IntPtr":
                    valueSizeBytes = (uint)IntPtr.Size;
                    vectorLaneKind = 1u;
                    scalarKind = 3u;
                    break;
                case "System.UIntPtr":
                    valueSizeBytes = (uint)IntPtr.Size;
                    vectorLaneKind = 1u;
                    scalarKind = 4u;
                    break;
                case "System.Single":
                    valueSizeBytes = 4u;
                    vectorLaneKind = 2u;
                    scalarKind = 5u;
                    break;
                case "System.Double":
                    valueSizeBytes = 8u;
                    vectorLaneKind = 2u;
                    scalarKind = 5u;
                    break;
                default:
                    return null;
            }

            if (!uint.TryParse(registration.DefinitionSubjectId, out var typeToken))
            {
                var tokenText = registration.DefinitionSubjectId;
                if (tokenText.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
                {
                    typeToken = Convert.ToUInt32(tokenText, 16);
                }
                else
                {
                    return null;
                }
            }

            return new CodeRegistrationTypeCapabilityEntry
            {
                SubjectId = registration.SubjectId,
                TypeToken = typeToken,
                CapabilityBits = 1u,
                ValueSizeBytes = valueSizeBytes,
                VectorWidthBytes = 16u,
                VectorLaneCount = valueSizeBytes == 0u ? 0u : (16u / valueSizeBytes),
                VectorLaneKind = vectorLaneKind,
                ScalarKind = scalarKind,
            };
        }

        return metadataRegistration.Registrations
            .Select(TryCreate)
            .Where(static item => item is not null)
            .Cast<CodeRegistrationTypeCapabilityEntry>()
            .GroupBy(static item => item.SubjectId, StringComparer.Ordinal)
            .Select(static group => group.First())
            .ToList();
    }
}
