using Chaos.IL2CPP.Contracts;
using System.Reflection;

namespace Chaos.IL2CPP.Generator;

public sealed partial class CodeGenStage
{
    private const int AuditTranslationUnitPageSize = 1024;
    private const int NativeAotPageSize = 999999;

    public string Name => "CodeGen";

    public PipelineResult<ManagedClosureResult> Generate(
        ManagedClosureRequest request,
        LinkedWorldModel linkedWorld,
        MetadataWriterOutput metadataWriterOutput)
    {
        try
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

            // Build generic capability matrix BEFORE adding bridge methods
            // (bridge methods would produce conflicting generic authority observations).
            var genericCapabilityMatrix = new GenericCapabilityMatrixBuilder().Build(
                ResolveOwnerSubjectId(request.InputAssemblyPath, linkedWorld.Assembly.Name),
                linkedWorld.EntryPointSubjectId,
                genericInstantiationDemandGraph,
                aotCoreIr,
                metadataWriterOutput.SupplementalMetadataTemplate);

            // Phase L2: Bridge method AOT compilation & integration.
            // DISABLED (LCAC Phase 1): Cross-assembly calls use Demeter Table.
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

            return PipelineResult<ManagedClosureResult>.Ok(new ManagedClosureResult
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
                // All managed methods from the original linked world (incl. BCL /
                // referenced-assembly methods that AotCoreIr lowering skipped).
                // This feeds TryBuildExternalRuntimeAotIrJson so the interpreter
                // can execute external-runtime methods instead of return-0.
                AllManagedMethods = BuildAllManagedMethods(linkedWorld),
            });
        }
        catch (Exception ex)
        {
            return PipelineResult<ManagedClosureResult>.Fail("CODEGEN_GENERATE_FAILED",
                $"Code generation failed: {ex.Message}", ex);
        }
    }

    /// <summary>
    /// Build a dictionary of ALL managed methods from the linked world keyed by
    /// SubjectId, including methods that AotCoreIrLowering skipped (BCL methods,
    /// missing-shape methods, etc.). This dictionary is consumed by the Planner
    /// (TryBuildExternalRuntimeAotIrJson) to generate interpreter-executable AOT
    /// Core IR JSON for external-runtime methods so they don't return 0.
    /// </summary>
    private static IReadOnlyDictionary<string, ManagedMethodModel>? BuildAllManagedMethods(
        LinkedWorldModel linkedWorld)
    {
        var methods = linkedWorld.Methods;
        if (methods == null || methods.Count == 0)
            return null;

        var dict = new Dictionary<string, ManagedMethodModel>(methods.Count, StringComparer.Ordinal);
        foreach (var m in methods)
        {
            // TryAdd (not indexer assignment) so a SubjectId collision keeps the
            // first occurrence instead of silently overwriting it with a later
            // method of the same id. This mirrors _methodsBySubjectId construction
            // (NativeAotLoweringPlanner.Methods.cs) — a silently dropped method
            // would otherwise vanish from AllManagedMethods and the interpreter
            // would fall back to returning 0 for it.
            if (!string.IsNullOrEmpty(m.SubjectId))
                dict.TryAdd(m.SubjectId, m);
        }
        return dict;
    }

    /// <summary>
    /// Generate per-assembly codegen results from a multi-assembly pipeline run.
    /// Each input assembly gets its own filtered AotCoreIr, CodeRegistration, and LoweringPlan.
    ///
    /// G11 enhancements:
    /// - CrossAssemblySymbolRegistry detects and disambiguates symbol conflicts
    /// - AssemblyExportRegistry tracks cross-assembly call exports
    /// - Global MethodTable slot allocation via MethodTableAllocator
    /// - Per-assembly filtered CodeRegistration (only the assembly's own methods)
    /// - Per-assembly NativeAotLoweringPlan with unique entry symbol
    /// </summary>
    public IReadOnlyList<ManagedClosureResult> FilterResultPerAssembly(
        ManagedClosureResult fullResult,
        IReadOnlyList<string> inputAssemblyPaths)
    {
        // Flat merge mode: when additional assemblies are provided, return ALL
        // methods in a single result instead of splitting into per-assembly files.
        // This avoids symbol conflicts (hotpatch table, code registration, type IDs)
        // when multiple assemblies are compiled into a single executable (entry.exe).
        if (inputAssemblyPaths.Count > 1)
            return new[] { fullResult };

        // ── Phase 1: Global coordination ──

        // Build per-assembly method groups
        var assemblyMethodMap = new Dictionary<string, List<AotCoreIrMethodArtifact>>(StringComparer.Ordinal);
        foreach (var method in fullResult.AotCoreIr.Methods)
        {
            var asmName = ExtractAssemblyNameFromSubjectId(method.SubjectId);
            if (!assemblyMethodMap.TryGetValue(asmName, out var list))
            {
                list = [];
                assemblyMethodMap[asmName] = list;
            }
            list.Add(method);
        }

        // Build symbol registry: register all method symbols per assembly
        var symbolRegistry = new CrossAssemblySymbolRegistry();
        foreach (var (asmName, methods) in assemblyMethodMap)
        {
            var symbols = methods.Select(m => m.NativeSymbol);
            symbolRegistry.RegisterAssembly(asmName, symbols);
        }
        symbolRegistry.ResolveConflicts();

        // Build export registry: detect cross-assembly calls
        var exportRegistry = new AssemblyExportRegistry();
        foreach (var method in fullResult.AotCoreIr.Methods)
        {
            var callerAssembly = ExtractAssemblyNameFromSubjectId(method.SubjectId);
            foreach (var instruction in method.Instructions)
            {
                if (string.IsNullOrEmpty(instruction.Callee))
                    continue;
                exportRegistry.RegisterCall(callerAssembly, instruction.Callee);
            }
        }

        // Allocate global method table slots per assembly
        var methodTableAllocator = new Chaos.IL2CPP.Generator.Planning.MethodTableAllocator();
        var assemblySlotInfo = new Dictionary<string, (uint StartIndex, uint Count)>(StringComparer.Ordinal);
        foreach (var (asmName, methods) in assemblyMethodMap)
        {
            var range = methodTableAllocator.AllocateRange((uint)methods.Count);
            assemblySlotInfo[asmName] = range;
        }

        // ── Phase 2: Per-assembly result generation ──

        var results = new List<ManagedClosureResult>();

        foreach (var asmPath in inputAssemblyPaths)
        {
            var asmName = System.IO.Path.GetFileNameWithoutExtension(asmPath);
            var asmOutputRoot = System.IO.Path.Combine(fullResult.OutputRootPath, asmName);
            System.IO.Directory.CreateDirectory(asmOutputRoot);

            // Filter AotCoreIr methods to this assembly
            var assemblyMethods = assemblyMethodMap.TryGetValue(asmName, out var methods)
                ? methods
                : [];

            var filteredAotCoreIr = fullResult.AotCoreIr with
            {
                Methods = assemblyMethods,
            };

            // Filter CodeRegistration to only this assembly's module entries
            var filteredCodeRegistration = FilterCodeRegistrationPerAssembly(
                fullResult.CodeRegistration, asmName);

            // Build per-assembly NativeAotLoweringPlan
            var filteredLoweringPlan = BuildPerAssemblyLoweringPlan(
                fullResult.NativeAotLoweringPlan, asmName, assemblyMethods);

            results.Add(new ManagedClosureResult
            {
                OutputRootPath = asmOutputRoot,
                TypedIlIr = fullResult.TypedIlIr,
                AotCoreIr = filteredAotCoreIr,
                AotManifest = fullResult.AotManifest,
                MetadataRegistration = fullResult.MetadataRegistration,
                SupplementalMetadataTemplate = fullResult.SupplementalMetadataTemplate,
                CodeRegistration = filteredCodeRegistration,
                GenericInstantiationDemandGraph = fullResult.GenericInstantiationDemandGraph,
                GenericCapabilityMatrix = fullResult.GenericCapabilityMatrix,
                OptimizationFacts = fullResult.OptimizationFacts,
                PreserveDescriptor = fullResult.PreserveDescriptor,
                NativeReferenceLoweringPlan = fullResult.NativeReferenceLoweringPlan,
                NativeAotLoweringPlan = filteredLoweringPlan,
                ClosureManifest = fullResult.ClosureManifest,
                CrossAssemblyExportHeader = new AssemblyExportHeaderGenerator(
                    exportRegistry, symbolRegistry, asmName).GenerateExportHeader(),
            });
        }

        return results;
    }

    /// <summary>
    /// Filter CodeRegistration to only include the module for the given assembly.
    /// Keeps all TypeCapabilities (they are shared metadata).
    /// </summary>
    private static CodeRegistrationArtifact FilterCodeRegistrationPerAssembly(
        CodeRegistrationArtifact fullCodeRegistration,
        string assemblyName)
    {
        var moduleName = $"{assemblyName}.dll";
        var filteredModules = fullCodeRegistration.Modules
            .Where(m => string.Equals(m.ModuleName, moduleName, StringComparison.OrdinalIgnoreCase))
            .Select(m => new CodeRegistrationModule
            {
                ModuleName = m.ModuleName,
                Registrations = m.Registrations
                    .Select((entry, idx) => new CodeRegistrationEntry
                    {
                        RegistrationKind = entry.RegistrationKind,
                        Slot = idx, // Re-index slots sequentially per assembly
                        Symbol = entry.Symbol,
                        SubjectId = entry.SubjectId,
                    })
                    .ToList(),
            })
            .ToList();

        return fullCodeRegistration with
        {
            Modules = filteredModules,
            TypeCapabilities = fullCodeRegistration.TypeCapabilities,
        };
    }

    /// <summary>
    /// Build a per-assembly NativeAotLoweringPlan based on the original plan
    /// but with the assembly-specific entry symbol and method set.
    /// </summary>
    private static NativeAotLoweringPlanArtifact BuildPerAssemblyLoweringPlan(
        NativeAotLoweringPlanArtifact originalPlan,
        string assemblyName,
        IReadOnlyList<AotCoreIrMethodArtifact> assemblyMethods)
    {
        var firstSymbol = assemblyMethods.Count > 0
            ? assemblyMethods[0].NativeSymbol
            : $"{assemblyName}_empty";

        return new NativeAotLoweringPlanArtifact
        {
            PlanKind = originalPlan.PlanKind,
            AssemblyName = assemblyName,
            EntrySubjectId = assemblyMethods.Count > 0
                ? assemblyMethods[0].SubjectId
                : originalPlan.EntrySubjectId,
            NativeEntryFunctionName = originalPlan.NativeEntryFunctionName,
            EntrySymbol = firstSymbol,
            EntryMethodToken = originalPlan.EntryMethodToken,
            WorkloadAbi = originalPlan.WorkloadAbi,
            TranslationUnitPageSize = originalPlan.TranslationUnitPageSize,
            TranslationUnitPageCount = originalPlan.TranslationUnitPageCount,
            TranslationUnitPages = originalPlan.TranslationUnitPages,
            Includes = originalPlan.Includes,
        };
    }

    /// <summary>
    /// Extract the assembly name prefix from a SubjectId.
    /// SubjectId format: "AssemblyName/Namespace.TypeName::MethodName:..."
    /// </summary>
    private static string ExtractAssemblyNameFromSubjectId(string subjectId)
    {
        if (string.IsNullOrEmpty(subjectId))
            return string.Empty;

        var slashIndex = subjectId.IndexOf('/');
        return slashIndex > 0 ? subjectId[..slashIndex] : subjectId;
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

        return entryPointMethod.IsStatic;
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
            return CreateAssemblyFullClosureNativeAotPlan(linkedWorld, codeRegistration);
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
            WorkloadAbi = DeriveWorkloadAbi(linkedWorld.EntryPointSubjectId),
        };
    }

    private static string DeriveWorkloadAbi(string entrySubjectId)
    {
        // SubjectId format: AssemblyName/FullTypeName::MethodName:ReturnType(ParamTypes)
        // Examples:
        //   ...::Main:System.Int32()        → "int()"
        //   ...::Run:System.Int32(Int32)    → "int(int32)"
        //   ...::Run:System.Void(Int32)     → "void(int32)"

        // Find the return type: after last ':' and before '('
        var colonIndex = entrySubjectId.LastIndexOf(':');
        if (colonIndex < 0)
            return "int(int32)";

        var parenIndex = entrySubjectId.IndexOf('(', colonIndex);
        if (parenIndex < 0)
            return "int(int32)";

        var returnType = entrySubjectId.Substring(colonIndex + 1, parenIndex - colonIndex - 1);
        var isVoid = string.Equals(returnType, "System.Void", StringComparison.Ordinal);

        // Parse parameter types
        var paramsPart = entrySubjectId.Substring(parenIndex + 1);
        var closeParen = paramsPart.IndexOf(')');
        paramsPart = closeParen >= 0 ? paramsPart.Substring(0, closeParen) : paramsPart;

        var paramTypes = string.IsNullOrWhiteSpace(paramsPart)
            ? Array.Empty<string>()
            : paramsPart.Split(',');

        var abiParams = string.Join(",", paramTypes.Select(p =>
        {
            p = p.Trim();
            if (string.Equals(p, "System.Int32", StringComparison.Ordinal) || p.Contains("Int32"))
                return "int32";
            return "int32"; // default fallback
        }));

        if (string.IsNullOrEmpty(abiParams))
            return isVoid ? "void()" : "int()";

        return isVoid ? $"void({abiParams})" : $"int({abiParams})";
    }

    private sealed record StageLookups(
        IReadOnlyDictionary<string, MethodShapeModel> MethodShapes,
        IReadOnlyDictionary<string, IReadOnlyList<string>> MethodCapabilities,
        IReadOnlySet<string> InternalAssemblyNames,
        IReadOnlyDictionary<string, ManagedMethodModel> MethodsBySubjectId);
}
