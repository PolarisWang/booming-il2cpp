using System.Text.Json;

namespace Chaos.IL2CPP.Tools.FoundationPack.Analysis;

internal static class DependencyLayerArtifactsBuilder
{
    private static readonly StringComparer NameComparer = StringComparer.Ordinal;

    private static readonly string[] BaselineNativeProofAliases =
    [
        "aot-closure-validation-proof",
        "required-instantiation-closure-proof",
    ];

    private static readonly string[] BaselineBenchmarkAliases =
    [
        "required-instantiation-closure-bench",
    ];

    private static readonly string[] BaselineConformanceGateIds =
    [
        "engineering-validation/SolutionCorePack/managed-build",
        "engineering-validation/SolutionCorePack/managed-runtime-output",
        "engineering-validation/SolutionCorePack/native-executable-smoke",
        "engineering-validation/SolutionCorePack/project-graph",
    ];

    private static readonly string[] BaselineEngineeringWorkloadIds =
    [
        "engineering-workload/SolutionCorePack/codegen",
        "engineering-workload/SolutionCorePack/convert",
        "engineering-workload/SolutionCorePack/native-link",
    ];

    private static readonly string[] RequiredGates =
    [
        "collector",
        "registry",
        "workspace",
        "dependency-layer-proof",
        "benchmark",
    ];

    private static readonly string[] MonitorSignals =
    [
        "certified-assembly-count",
        "blocked-assembly-count",
        "layer-completion-rate",
        "layer-nativeization-throughput",
        "layer-compile-time-ms",
        "native-proof-pass-rate",
        "benchmark-regression-delta",
    ];

    public static DependencyLayerArtifactsBuildResult Build(
        string catalogPath,
        string phase1Directory,
        string phase2Directory,
        string phase3Directory,
        string taskId,
        string registrySnapshotPath)
    {
        var catalog = Phase0Catalog.Load(catalogPath);
        var phase1Index = DependencyPhase1ContractLaneIndex.Load(phase1Directory);
        var phase2Index = Phase2SubstrateIndex.Load(phase2Directory);
        var phase3Index = DependencyPhase3ExecutionIndex.Load(phase3Directory);
        var registry = DependencyRegistrySnapshotIndex.Load(registrySnapshotPath);

        var layerModelsByFramework = new SortedDictionary<string, DependencyFrameworkModel>(NameComparer);
        foreach (var source in catalog.Sources.OrderBy(static value => value.TargetFramework, NameComparer))
        {
            var phase1Framework = phase1Index.GetRequiredFramework(source.TargetFramework);
            var layerModel = BuildFrameworkModel(source, phase1Framework);
            layerModelsByFramework[source.TargetFramework] = layerModel;
        }

        var assemblyPlans = BuildAssemblyPlans(layerModelsByFramework, taskId, registry);

        var layerPlanPayload = new DependencyLayerPlanPayload
        {
            TaskId = taskId,
            InputAuthorities = new DependencyInputAuthoritiesPayload
            {
                Phase0CatalogPath = NormalizePath(Path.GetFullPath(catalogPath)),
                Phase1Directory = NormalizePath(Path.GetFullPath(phase1Directory)),
                Phase2Directory = NormalizePath(Path.GetFullPath(phase2Directory)),
                Phase3Directory = NormalizePath(Path.GetFullPath(phase3Directory)),
                RegistrySnapshotPath = NormalizePath(registry.RegistrySnapshotPath),
            },
            CertifiedBaseline = new DependencyCertifiedBaselinePayload
            {
                OwnerSubjectId = registry.OwnerSubjectId,
                Phase3BatchName = phase3Index.BatchName,
                Phase3SemanticFamilies = phase3Index.SelectedBatchFamilyNames.ToArray(),
            },
        };

        var proofLanePayload = new DependencyProofBenchmarkLanePayload
        {
            TaskId = taskId,
            OwnerSubjectId = registry.OwnerSubjectId,
            RegistrySnapshotPath = NormalizePath(registry.RegistrySnapshotPath),
            SubjectManifestPath = NormalizePath(registry.SubjectManifestPath),
            RequiredGates = RequiredGates.ToArray(),
        };

        foreach (var entry in layerModelsByFramework)
        {
            var targetFramework = entry.Key;
            var model = entry.Value;
            var phase2Framework = phase2Index.TargetFrameworks.TryGetValue(targetFramework, out var frameworkBaseline)
                ? frameworkBaseline
                : null;

            layerPlanPayload.CertifiedBaseline.TargetFrameworks[targetFramework] = new DependencyCertifiedBaselineFrameworkPayload
            {
                ContractCertifiedAssemblyCount = model.ContractCertifiedAssemblyCount,
                ContractCertifiedAssemblies = model.ContractCertifiedAssemblies.OrderBy(static value => value, NameComparer).ToArray(),
                CoreLibHelperKinds = phase2Framework is null
                    ? []
                    : phase2Framework.MemberIdsByClassification.Keys.OrderBy(static value => value, NameComparer).ToArray(),
                CoreLibHelperKindCount = phase2Framework?.MemberIdsByClassification.Count ?? 0,
            };

            var layerPlanFrameworkPayload = new DependencyLayerPlanFrameworkPayload
            {
                SelectedVersion = model.SelectedVersion,
                SourceKind = model.SourceKind,
                SharedSystemAssemblyCount = model.SharedSystemAssemblyCount,
                CandidateAssemblyCount = model.CandidateAssemblyCount,
                LayerCount = model.Layers.Count,
                FirstReadyLayerName = model.FirstReadyLayerName,
            };

            var proofLaneFrameworkPayload = new DependencyProofBenchmarkFrameworkPayload
            {
                LayerCount = model.Layers.Count,
            };

            foreach (var layer in model.Layers.Values.OrderBy(static value => value.Order))
            {
                var assemblyPlanPaths = layer.ReadinessStatus == "ready"
                    ? layer.AssemblyNames
                        .Where(assemblyPlans.ContainsKey)
                        .Select(static assemblyName => $"assembly-nativeization-plan/{assemblyName}.json")
                        .OrderBy(static value => value, NameComparer)
                        .ToArray()
                    : [];

                var layerPayload = new DependencyLayerPlanLayerPayload
                {
                    Order = layer.Order,
                    ReadinessStatus = layer.ReadinessStatus,
                    AssemblyCount = layer.AssemblyNames.Count,
                    ReadyAssemblyCount = layer.ReadinessStatus == "ready" ? layer.AssemblyNames.Count : 0,
                    BlockedAssemblyCount = layer.ReadinessStatus == "blocked" ? layer.AssemblyNames.Count : 0,
                    UpstreamLayerNames = layer.UpstreamLayerNames.OrderBy(static value => value, NameComparer).ToArray(),
                    CapabilityFamilies = layer.CapabilityFamilies.OrderBy(static value => value, NameComparer).ToArray(),
                    AssemblyPlanPaths = assemblyPlanPaths,
                };

                foreach (var assemblyName in layer.AssemblyNames.OrderBy(static value => value, NameComparer))
                {
                    var assembly = model.CandidateAssemblies[assemblyName];
                    layerPayload.Assemblies[assemblyName] = new DependencyLayerPlanAssemblyPayload
                    {
                        Classification = assembly.Classification,
                        Status = assembly.Status,
                        SurfaceContractCertified = assembly.SurfaceContractCertified,
                        PublicTypeDefinitionCount = assembly.PublicTypeDefinitionCount,
                        PublicMemberCount = assembly.PublicMemberCount,
                        RuntimeMethodBodyCount = assembly.RuntimeMethodBodyCount,
                        CapabilityFamilies = assembly.CapabilityFamilies.ToArray(),
                        DependencyAssemblies = assembly.DependencyAssemblies.ToArray(),
                        CandidateDependencyAssemblies = assembly.CandidateDependencyAssemblies.ToArray(),
                        CertifiedDependencyAssemblies = assembly.CertifiedDependencyAssemblies.ToArray(),
                        ExternalDependencyAssemblies = assembly.ExternalDependencyAssemblies.ToArray(),
                        BlockerReasons = assembly.BlockerReasons.ToArray(),
                    };
                }

                layerPlanFrameworkPayload.Layers[layer.LayerName] = layerPayload;

                var layerBundle = BuildLayerCommandBundle(layer, model.CandidateAssemblies, registry);
                proofLaneFrameworkPayload.Layers[layer.LayerName] = new DependencyProofBenchmarkLayerPayload
                {
                    Order = layer.Order,
                    ReadinessStatus = layer.ReadinessStatus,
                    AssemblyCount = layer.AssemblyNames.Count,
                    AssemblyNames = layer.AssemblyNames.OrderBy(static value => value, NameComparer).ToArray(),
                    CapabilityFamilies = layer.CapabilityFamilies.OrderBy(static value => value, NameComparer).ToArray(),
                    RequiredGates = RequiredGates.ToArray(),
                    MonitorSignals = MonitorSignals.ToArray(),
                    NativeProofs = layerBundle.NativeProofs,
                    Benchmarks = layerBundle.Benchmarks,
                    ConformanceGates = layerBundle.ConformanceGates,
                    EngineeringWorkloads = layerBundle.EngineeringWorkloads,
                };
            }

            layerPlanPayload.TargetFrameworks[targetFramework] = layerPlanFrameworkPayload;
            proofLanePayload.TargetFrameworks[targetFramework] = proofLaneFrameworkPayload;
        }

        return new DependencyLayerArtifactsBuildResult(layerPlanPayload, proofLanePayload, assemblyPlans);
    }

    private static DependencyFrameworkModel BuildFrameworkModel(
        FrameworkCatalogSource source,
        DependencyPhase1FrameworkIndex phase1Framework)
    {
        var refAssemblies = Program.LoadAssemblies(source.RefRootPath, MetadataPreference.Ref);
        var runtimeAssemblies = Program.LoadAssemblies(source.RuntimeRootPath, MetadataPreference.Runtime);
        var analyses = Program.MergeAssemblies(refAssemblies, runtimeAssemblies);
        Program.ClassifyAssemblies(analyses);

        var candidateAssemblies = new SortedDictionary<string, DependencyAssemblyModel>(NameComparer);
        var sharedSystemAssemblyCount = 0;

        foreach (var analysis in analyses.Values.OrderBy(static value => value.AssemblyName, NameComparer))
        {
            if (!analysis.AssemblyName.StartsWith("System", StringComparison.Ordinal))
            {
                continue;
            }

            if (analysis.RefPath is null || analysis.RuntimePath is null)
            {
                continue;
            }

            sharedSystemAssemblyCount++;

            var preferredMetadata = analysis.PreferredMetadata;
            if (preferredMetadata is null)
            {
                continue;
            }

            var runtimeMethodBodyCount = analysis.RuntimeMetadata?.MethodBodyCount ?? 0;
            if (runtimeMethodBodyCount == 0)
            {
                continue;
            }

            var publicTypeCount = preferredMetadata.PublicTypes.Count;
            var publicMemberCount = SumPublicMemberCount(preferredMetadata);
            if (publicTypeCount == 0 && publicMemberCount == 0)
            {
                continue;
            }

            var dependencyAssemblies = Program.GetClosureTargets(analysis)
                .Select(static value => value.AssemblyName)
                .Where(static value => !string.IsNullOrWhiteSpace(value))
                .Distinct(NameComparer)
                .OrderBy(static value => value, NameComparer)
                .ToArray();

            var capabilityFamilies = ClassifyCapabilityFamilies(analysis.AssemblyName);

            candidateAssemblies[analysis.AssemblyName] = new DependencyAssemblyModel(
                analysis.AssemblyName,
                analysis.Classification,
                phase1Framework.IsContractCompleteEligible(analysis.AssemblyName),
                publicTypeCount,
                publicMemberCount,
                runtimeMethodBodyCount,
                dependencyAssemblies,
                capabilityFamilies);
        }

        var candidateNames = new HashSet<string>(candidateAssemblies.Keys, NameComparer);
        foreach (var assembly in candidateAssemblies.Values)
        {
            assembly.CandidateDependencyAssemblies = assembly.DependencyAssemblies
                .Where(candidateNames.Contains)
                .OrderBy(static value => value, NameComparer)
                .ToArray();
            assembly.CertifiedDependencyAssemblies = assembly.DependencyAssemblies
                .Where(phase1Framework.IsContractCompleteEligible)
                .Append("System.Private.CoreLib")
                .Distinct(NameComparer)
                .OrderBy(static value => value, NameComparer)
                .ToArray();
            assembly.ExternalDependencyAssemblies = assembly.DependencyAssemblies
                .Where(value => !candidateNames.Contains(value))
                .Where(value => !phase1Framework.IsContractCompleteEligible(value))
                .Where(static value => !string.Equals(value, "System.Private.CoreLib", StringComparison.Ordinal))
                .OrderBy(static value => value, NameComparer)
                .ToArray();
        }

        var componentModels = BuildComponentModels(candidateAssemblies);
        var layers = BuildLayerModels(componentModels, candidateAssemblies);

        return new DependencyFrameworkModel(
            source.TargetFramework,
            source.RefVersion,
            source.RefSourceKind,
            sharedSystemAssemblyCount,
            candidateAssemblies,
            layers,
            phase1Framework.ContractCompleteAssemblies);
    }

    private static IReadOnlyDictionary<int, DependencyComponentModel> BuildComponentModels(
        IReadOnlyDictionary<string, DependencyAssemblyModel> assemblies)
    {
        var index = 0;
        var indices = new Dictionary<string, int>(NameComparer);
        var lowLinks = new Dictionary<string, int>(NameComparer);
        var stack = new Stack<string>();
        var onStack = new HashSet<string>(NameComparer);
        var components = new List<List<string>>();

        foreach (var assemblyName in assemblies.Keys.OrderBy(static value => value, NameComparer))
        {
            if (!indices.ContainsKey(assemblyName))
            {
                StrongConnect(assemblyName);
            }
        }

        var assemblyToComponent = new Dictionary<string, int>(NameComparer);
        for (var componentId = 0; componentId < components.Count; componentId++)
        {
            foreach (var assemblyName in components[componentId])
            {
                assemblyToComponent[assemblyName] = componentId;
            }
        }

        var componentModels = new Dictionary<int, DependencyComponentModel>();
        for (var componentId = 0; componentId < components.Count; componentId++)
        {
            var assemblyNames = components[componentId].OrderBy(static value => value, NameComparer).ToArray();
            componentModels[componentId] = new DependencyComponentModel(componentId, assemblyNames);
        }

        foreach (var component in componentModels.Values)
        {
            var dependencies = new HashSet<int>();
            foreach (var assemblyName in component.AssemblyNames)
            {
                foreach (var dependency in assemblies[assemblyName].CandidateDependencyAssemblies)
                {
                    var dependencyComponentId = assemblyToComponent[dependency];
                    if (dependencyComponentId != component.ComponentId)
                    {
                        dependencies.Add(dependencyComponentId);
                    }
                }
            }

            component.DependencyComponentIds = dependencies.OrderBy(static value => value).ToArray();
        }

        var layerMemo = new Dictionary<int, int>();
        foreach (var component in componentModels.Values)
        {
            component.LayerOrder = ComputeLayerOrder(component.ComponentId);
        }

        return componentModels;

        void StrongConnect(string assemblyName)
        {
            indices[assemblyName] = index;
            lowLinks[assemblyName] = index;
            index++;
            stack.Push(assemblyName);
            onStack.Add(assemblyName);

            foreach (var dependency in assemblies[assemblyName].CandidateDependencyAssemblies)
            {
                if (!indices.ContainsKey(dependency))
                {
                    StrongConnect(dependency);
                    lowLinks[assemblyName] = Math.Min(lowLinks[assemblyName], lowLinks[dependency]);
                    continue;
                }

                if (onStack.Contains(dependency))
                {
                    lowLinks[assemblyName] = Math.Min(lowLinks[assemblyName], indices[dependency]);
                }
            }

            if (lowLinks[assemblyName] != indices[assemblyName])
            {
                return;
            }

            var componentAssemblies = new List<string>();
            while (true)
            {
                var current = stack.Pop();
                onStack.Remove(current);
                componentAssemblies.Add(current);
                if (string.Equals(current, assemblyName, StringComparison.Ordinal))
                {
                    break;
                }
            }

            components.Add(componentAssemblies);
        }

        int ComputeLayerOrder(int componentId)
        {
            if (layerMemo.TryGetValue(componentId, out var cached))
            {
                return cached;
            }

            var component = componentModels[componentId];
            var order = component.DependencyComponentIds.Count == 0
                ? 1
                : component.DependencyComponentIds.Max(ComputeLayerOrder) + 1;
            layerMemo[componentId] = order;
            return order;
        }
    }

    private static SortedDictionary<string, DependencyLayerModel> BuildLayerModels(
        IReadOnlyDictionary<int, DependencyComponentModel> components,
        IReadOnlyDictionary<string, DependencyAssemblyModel> assemblies)
    {
        var layersByOrder = new SortedDictionary<int, DependencyLayerModel>();
        foreach (var component in components.Values.OrderBy(static value => value.LayerOrder).ThenBy(static value => value.ComponentId))
        {
            if (!layersByOrder.TryGetValue(component.LayerOrder, out var layer))
            {
                var layerName = $"core-bcl-layer-{component.LayerOrder:D2}";
                layer = new DependencyLayerModel(layerName, component.LayerOrder);
                layersByOrder[component.LayerOrder] = layer;
            }

            foreach (var assemblyName in component.AssemblyNames)
            {
                layer.AssemblyNames.Add(assemblyName);
            }

            foreach (var dependencyComponentId in component.DependencyComponentIds)
            {
                var upstreamOrder = components[dependencyComponentId].LayerOrder;
                if (upstreamOrder != component.LayerOrder)
                {
                    layer.UpstreamLayerNames.Add($"core-bcl-layer-{upstreamOrder:D2}");
                }
            }
        }

        foreach (var layer in layersByOrder.Values)
        {
            layer.ReadinessStatus = layer.Order == 1 ? "ready" : "blocked";
            foreach (var assemblyName in layer.AssemblyNames.OrderBy(static value => value, NameComparer))
            {
                var assembly = assemblies[assemblyName];
                assembly.LayerName = layer.LayerName;
                assembly.LayerOrder = layer.Order;
                assembly.Status = layer.ReadinessStatus == "ready" ? "ready" : "blocked";
                assembly.BlockerReasons = layer.Order == 1
                    ? []
                    : layer.UpstreamLayerNames.OrderBy(static value => value, NameComparer)
                        .Select(static value => $"waiting-on:{value}")
                        .ToArray();
                foreach (var capabilityFamily in assembly.CapabilityFamilies)
                {
                    layer.CapabilityFamilies.Add(capabilityFamily);
                }
            }
        }

        return new SortedDictionary<string, DependencyLayerModel>(
            layersByOrder.Values.ToDictionary(static value => value.LayerName, static value => value, NameComparer),
            NameComparer);
    }

    private static SortedDictionary<string, DependencyAssemblyNativeizationPlanPayload> BuildAssemblyPlans(
        IReadOnlyDictionary<string, DependencyFrameworkModel> layerModelsByFramework,
        string taskId,
        DependencyRegistrySnapshotIndex registry)
    {
        var planAssemblies = new SortedDictionary<string, HashSet<string>>(NameComparer);

        foreach (var framework in layerModelsByFramework.Values)
        {
            var readyLayer = framework.Layers[framework.FirstReadyLayerName];
            foreach (var assemblyName in readyLayer.AssemblyNames)
            {
                if (!planAssemblies.TryGetValue(assemblyName, out var frameworks))
                {
                    frameworks = new HashSet<string>(NameComparer);
                    planAssemblies[assemblyName] = frameworks;
                }

                frameworks.Add(framework.TargetFramework);
            }
        }

        var plans = new SortedDictionary<string, DependencyAssemblyNativeizationPlanPayload>(NameComparer);
        foreach (var entry in planAssemblies)
        {
            var assemblyName = entry.Key;
            var frameworkNames = entry.Value.OrderBy(static value => value, NameComparer).ToArray();
            var capabilityFamilies = new HashSet<string>(NameComparer);

            foreach (var frameworkName in frameworkNames)
            {
                foreach (var capabilityFamily in layerModelsByFramework[frameworkName].CandidateAssemblies[assemblyName].CapabilityFamilies)
                {
                    capabilityFamilies.Add(capabilityFamily);
                }
            }

            var bundle = BuildCommandBundle(capabilityFamilies.OrderBy(static value => value, NameComparer), registry);
            var payload = new DependencyAssemblyNativeizationPlanPayload
            {
                TaskId = taskId,
                AssemblyName = assemblyName,
                SelectionReason = "first-ready-layer",
                OwnerSubjectId = registry.OwnerSubjectId,
                SubjectManifestPath = NormalizePath(registry.SubjectManifestPath),
                RequiredGates = RequiredGates.ToArray(),
                MonitorSignals = MonitorSignals.ToArray(),
                NativeProofs = bundle.NativeProofs,
                Benchmarks = bundle.Benchmarks,
                ConformanceGates = bundle.ConformanceGates,
                EngineeringWorkloads = bundle.EngineeringWorkloads,
            };

            foreach (var frameworkName in frameworkNames)
            {
                var framework = layerModelsByFramework[frameworkName];
                var assembly = framework.CandidateAssemblies[assemblyName];
                payload.TargetFrameworks[frameworkName] = new DependencyAssemblyNativeizationFrameworkPayload
                {
                    SelectedVersion = framework.SelectedVersion,
                    SourceKind = framework.SourceKind,
                    LayerName = framework.FirstReadyLayerName,
                    Status = assembly.Status,
                    Classification = assembly.Classification,
                    SurfaceContractCertified = assembly.SurfaceContractCertified,
                    PublicTypeDefinitionCount = assembly.PublicTypeDefinitionCount,
                    PublicMemberCount = assembly.PublicMemberCount,
                    RuntimeMethodBodyCount = assembly.RuntimeMethodBodyCount,
                    CapabilityFamilies = assembly.CapabilityFamilies.ToArray(),
                    DependencyAssemblies = assembly.DependencyAssemblies.ToArray(),
                    CandidateDependencyAssemblies = assembly.CandidateDependencyAssemblies.ToArray(),
                    CertifiedDependencyAssemblies = assembly.CertifiedDependencyAssemblies.ToArray(),
                    ExternalDependencyAssemblies = assembly.ExternalDependencyAssemblies.ToArray(),
                    BlockerReasons = assembly.BlockerReasons.ToArray(),
                };
            }

            plans[assemblyName] = payload;
        }

        return plans;
    }

    private static DependencyCommandBundle BuildLayerCommandBundle(
        DependencyLayerModel layer,
        IReadOnlyDictionary<string, DependencyAssemblyModel> assemblies,
        DependencyRegistrySnapshotIndex registry)
    {
        var capabilityFamilies = new HashSet<string>(NameComparer);
        foreach (var assemblyName in layer.AssemblyNames)
        {
            foreach (var capabilityFamily in assemblies[assemblyName].CapabilityFamilies)
            {
                capabilityFamilies.Add(capabilityFamily);
            }
        }

        return BuildCommandBundle(capabilityFamilies.OrderBy(static value => value, NameComparer), registry);
    }

    private static DependencyCommandBundle BuildCommandBundle(
        IEnumerable<string> capabilityFamilies,
        DependencyRegistrySnapshotIndex registry)
    {
        var nativeProofs = new SortedDictionary<string, SemanticFamilyCommandPayload>(NameComparer);
        var benchmarks = new SortedDictionary<string, SemanticFamilyCommandPayload>(NameComparer);
        var conformanceGates = new SortedDictionary<string, SemanticFamilyCommandPayload>(NameComparer);
        var engineeringWorkloads = new SortedDictionary<string, SemanticFamilyCommandPayload>(NameComparer);

        AddDeclaredUnitTestAliases(nativeProofs, registry, BaselineNativeProofAliases);
        AddDeclaredBenchmarkAliases(benchmarks, registry, BaselineBenchmarkAliases);
        AddEngineeringValidationIds(conformanceGates, registry, BaselineConformanceGateIds);
        AddEngineeringWorkloadIds(engineeringWorkloads, registry, BaselineEngineeringWorkloadIds);

        foreach (var capabilityFamily in capabilityFamilies)
        {
            switch (capabilityFamily)
            {
                case "collections-and-generics":
                    AddDeclaredUnitTestAliases(nativeProofs, registry, "generic-collection-proof", "generic-instantiation-proof", "dispatch-proof");
                    AddDeclaredBenchmarkAliases(benchmarks, registry, "generic-bench", "dispatch-bench", "generic-interface-dispatch-bench");
                    break;
                case "unsafe-and-low-level":
                    AddDeclaredUnitTestAliases(nativeProofs, registry, "span-memory-proof", "function-pointer-proof", "direct-callvirt-proof");
                    AddDeclaredBenchmarkAliases(benchmarks, registry, "span-memory-bench", "function-pointer-bench");
                    break;
                case "threading-and-async":
                    AddDeclaredUnitTestAliases(nativeProofs, registry, "threading-proof", "thread-local-state-proof", "task-valuetask-flow-proof", "task-scheduling-proof", "async-await-proof");
                    AddDeclaredBenchmarkAliases(benchmarks, registry, "task-valuetask-flow-bench", "task-scheduling-bench", "allocation-bench");
                    break;
                case "reflection-and-metadata":
                    AddDeclaredUnitTestAliases(nativeProofs, registry, "member-metadata-lookup-proof", "metadata-supplement-proof", "reflection-interop-closure-proof");
                    AddDeclaredBenchmarkAliases(benchmarks, registry, "member-metadata-lookup-bench");
                    break;
                case "io-and-networking":
                    AddDeclaredUnitTestAliases(nativeProofs, registry, "task-valuetask-flow-proof", "span-memory-proof");
                    AddDeclaredBenchmarkAliases(benchmarks, registry, "task-valuetask-flow-bench", "allocation-bench");
                    break;
                case "text-and-encoding":
                    AddDeclaredUnitTestAliases(nativeProofs, registry, "span-memory-proof", "conversion-ops-proof");
                    AddDeclaredBenchmarkAliases(benchmarks, registry, "span-memory-bench", "string-utf8-marshaling-bench");
                    break;
                case "runtime-services":
                    AddDeclaredUnitTestAliases(nativeProofs, registry, "abi-widening-proof", "direct-callvirt-proof");
                    AddDeclaredBenchmarkAliases(benchmarks, registry, "allocation-bench");
                    break;
            }
        }

        return new DependencyCommandBundle(
            nativeProofs.Values.ToArray(),
            benchmarks.Values.ToArray(),
            conformanceGates.Values.ToArray(),
            engineeringWorkloads.Values.ToArray());
    }

    private static void AddDeclaredUnitTestAliases(
        IDictionary<string, SemanticFamilyCommandPayload> destination,
        DependencyRegistrySnapshotIndex registry,
        params string[] aliases)
    {
        foreach (var alias in aliases)
        {
            if (registry.TryGetDeclaredUnitTestByAlias(alias, out var entry))
            {
                destination[entry.Id] = ToCommandPayload(entry);
            }
        }
    }

    private static void AddDeclaredBenchmarkAliases(
        IDictionary<string, SemanticFamilyCommandPayload> destination,
        DependencyRegistrySnapshotIndex registry,
        params string[] aliases)
    {
        foreach (var alias in aliases)
        {
            if (registry.TryGetDeclaredBenchmarkByAlias(alias, out var entry))
            {
                destination[entry.Id] = ToCommandPayload(entry);
            }
        }
    }

    private static void AddEngineeringValidationIds(
        IDictionary<string, SemanticFamilyCommandPayload> destination,
        DependencyRegistrySnapshotIndex registry,
        params string[] ids)
    {
        foreach (var id in ids)
        {
            var entry = registry.GetRequiredEngineeringValidationById(id);
            destination[entry.Id] = ToCommandPayload(entry);
        }
    }

    private static void AddEngineeringWorkloadIds(
        IDictionary<string, SemanticFamilyCommandPayload> destination,
        DependencyRegistrySnapshotIndex registry,
        params string[] ids)
    {
        foreach (var id in ids)
        {
            var entry = registry.GetRequiredEngineeringWorkloadById(id);
            destination[entry.Id] = ToCommandPayload(entry);
        }
    }

    private static SemanticFamilyCommandPayload ToCommandPayload(DependencyRegistryCommandEntry entry)
    {
        return new SemanticFamilyCommandPayload
        {
            Id = entry.Id,
            Type = entry.Type,
            Alias = entry.Alias,
            DisplayName = entry.DisplayName,
            SourceEntry = entry.SourceEntry,
            CanonicalCommand = entry.CanonicalCommand,
            DefaultGoalId = entry.DefaultGoalId,
            DefaultMatrixId = entry.DefaultMatrixId,
            CapabilityFamilyLabel = entry.CapabilityFamilyLabel,
            CapabilityItemLabel = entry.CapabilityItemLabel,
            CategoryLabel = entry.CategoryLabel,
            PipelineIds = entry.PipelineIds.ToArray(),
            SupportedHosts = entry.SupportedHosts.ToArray(),
        };
    }

    private static string[] ClassifyCapabilityFamilies(string assemblyName)
    {
        var families = new HashSet<string>(NameComparer);

        if (assemblyName.StartsWith("System.Collections", StringComparison.Ordinal) ||
            assemblyName.StartsWith("System.Linq", StringComparison.Ordinal) ||
            assemblyName.StartsWith("System.ObjectModel", StringComparison.Ordinal))
        {
            families.Add("collections-and-generics");
        }

        if (assemblyName.StartsWith("System.Memory", StringComparison.Ordinal) ||
            assemblyName.StartsWith("System.Buffers", StringComparison.Ordinal) ||
            string.Equals(assemblyName, "System.Runtime.CompilerServices.Unsafe", StringComparison.Ordinal) ||
            assemblyName.StartsWith("System.Runtime.InteropServices", StringComparison.Ordinal))
        {
            families.Add("unsafe-and-low-level");
        }

        if (assemblyName.StartsWith("System.Threading", StringComparison.Ordinal) ||
            assemblyName.StartsWith("System.Threading.Tasks", StringComparison.Ordinal) ||
            string.Equals(assemblyName, "System.Runtime.Loader", StringComparison.Ordinal))
        {
            families.Add("threading-and-async");
        }

        if (assemblyName.StartsWith("System.Reflection", StringComparison.Ordinal) ||
            assemblyName.StartsWith("System.Runtime.Serialization", StringComparison.Ordinal) ||
            assemblyName.StartsWith("System.Xml", StringComparison.Ordinal))
        {
            families.Add("reflection-and-metadata");
        }

        if (assemblyName.StartsWith("System.IO", StringComparison.Ordinal) ||
            assemblyName.StartsWith("System.Net", StringComparison.Ordinal))
        {
            families.Add("io-and-networking");
        }

        if (assemblyName.StartsWith("System.Text", StringComparison.Ordinal) ||
            string.Equals(assemblyName, "System.Console", StringComparison.Ordinal))
        {
            families.Add("text-and-encoding");
        }

        if (assemblyName.StartsWith("System.Diagnostics", StringComparison.Ordinal) ||
            assemblyName.StartsWith("System.Security", StringComparison.Ordinal) ||
            assemblyName.StartsWith("System.ComponentModel", StringComparison.Ordinal) ||
            string.Equals(assemblyName, "System.Runtime", StringComparison.Ordinal) ||
            string.Equals(assemblyName, "System.Runtime.Extensions", StringComparison.Ordinal))
        {
            families.Add("runtime-services");
        }

        if (families.Count == 0)
        {
            families.Add("runtime-services");
        }

        return families.OrderBy(static value => value, NameComparer).ToArray();
    }

    private static int SumPublicMemberCount(ManagedAssemblyMetadata metadata)
    {
        return metadata.PublicTypes.Values.Sum(static value =>
            value.PublicMethods.Count
            + value.PublicProperties.Count
            + value.PublicFields.Count
            + value.PublicEvents.Count);
    }

    private static string NormalizePath(string path)
    {
        return path.Replace('\\', '/');
    }
}

internal sealed record DependencyLayerArtifactsBuildResult(
    DependencyLayerPlanPayload LayerPlan,
    DependencyProofBenchmarkLanePayload ProofBenchmarkLane,
    SortedDictionary<string, DependencyAssemblyNativeizationPlanPayload> AssemblyPlans);

internal sealed class DependencyFrameworkModel
{
    public DependencyFrameworkModel(
        string targetFramework,
        string selectedVersion,
        string sourceKind,
        int sharedSystemAssemblyCount,
        SortedDictionary<string, DependencyAssemblyModel> candidateAssemblies,
        SortedDictionary<string, DependencyLayerModel> layers,
        IReadOnlyCollection<string> contractCertifiedAssemblies)
    {
        TargetFramework = targetFramework;
        SelectedVersion = selectedVersion;
        SourceKind = sourceKind;
        SharedSystemAssemblyCount = sharedSystemAssemblyCount;
        CandidateAssemblies = candidateAssemblies;
        Layers = layers;
        ContractCertifiedAssemblies = contractCertifiedAssemblies;
    }

    public string TargetFramework { get; }

    public string SelectedVersion { get; }

    public string SourceKind { get; }

    public int SharedSystemAssemblyCount { get; }

    public int CandidateAssemblyCount => CandidateAssemblies.Count;

    public SortedDictionary<string, DependencyAssemblyModel> CandidateAssemblies { get; }

    public SortedDictionary<string, DependencyLayerModel> Layers { get; }

    public IReadOnlyCollection<string> ContractCertifiedAssemblies { get; }

    public int ContractCertifiedAssemblyCount => ContractCertifiedAssemblies.Count;

    public string FirstReadyLayerName => Layers.Values.OrderBy(static value => value.Order).First(static value => value.ReadinessStatus == "ready").LayerName;
}

internal sealed class DependencyAssemblyModel
{
    public DependencyAssemblyModel(
        string assemblyName,
        string classification,
        bool surfaceContractCertified,
        int publicTypeDefinitionCount,
        int publicMemberCount,
        int runtimeMethodBodyCount,
        IReadOnlyList<string> dependencyAssemblies,
        IReadOnlyList<string> capabilityFamilies)
    {
        AssemblyName = assemblyName;
        Classification = classification;
        SurfaceContractCertified = surfaceContractCertified;
        PublicTypeDefinitionCount = publicTypeDefinitionCount;
        PublicMemberCount = publicMemberCount;
        RuntimeMethodBodyCount = runtimeMethodBodyCount;
        DependencyAssemblies = dependencyAssemblies;
        CapabilityFamilies = capabilityFamilies;
    }

    public string AssemblyName { get; }

    public string Classification { get; }

    public bool SurfaceContractCertified { get; }

    public int PublicTypeDefinitionCount { get; }

    public int PublicMemberCount { get; }

    public int RuntimeMethodBodyCount { get; }

    public IReadOnlyList<string> DependencyAssemblies { get; }

    public IReadOnlyList<string> CapabilityFamilies { get; }

    public string LayerName { get; set; } = string.Empty;

    public int LayerOrder { get; set; }

    public string Status { get; set; } = "blocked";

    public string[] CandidateDependencyAssemblies { get; set; } = [];

    public string[] CertifiedDependencyAssemblies { get; set; } = [];

    public string[] ExternalDependencyAssemblies { get; set; } = [];

    public string[] BlockerReasons { get; set; } = [];
}

internal sealed class DependencyLayerModel
{
    public DependencyLayerModel(string layerName, int order)
    {
        LayerName = layerName;
        Order = order;
    }

    public string LayerName { get; }

    public int Order { get; }

    public string ReadinessStatus { get; set; } = "blocked";

    public HashSet<string> AssemblyNames { get; } = new(StringComparer.Ordinal);

    public HashSet<string> UpstreamLayerNames { get; } = new(StringComparer.Ordinal);

    public HashSet<string> CapabilityFamilies { get; } = new(StringComparer.Ordinal);
}

internal sealed class DependencyComponentModel
{
    public DependencyComponentModel(int componentId, IReadOnlyList<string> assemblyNames)
    {
        ComponentId = componentId;
        AssemblyNames = assemblyNames;
    }

    public int ComponentId { get; }

    public IReadOnlyList<string> AssemblyNames { get; }

    public int LayerOrder { get; set; }

    public IReadOnlyList<int> DependencyComponentIds { get; set; } = [];
}

internal sealed record DependencyCommandBundle(
    SemanticFamilyCommandPayload[] NativeProofs,
    SemanticFamilyCommandPayload[] Benchmarks,
    SemanticFamilyCommandPayload[] ConformanceGates,
    SemanticFamilyCommandPayload[] EngineeringWorkloads);

internal sealed class DependencyPhase1ContractLaneIndex
{
    private DependencyPhase1ContractLaneIndex(IReadOnlyDictionary<string, DependencyPhase1FrameworkIndex> targetFrameworks)
    {
        TargetFrameworks = targetFrameworks;
    }

    public IReadOnlyDictionary<string, DependencyPhase1FrameworkIndex> TargetFrameworks { get; }

    public DependencyPhase1FrameworkIndex GetRequiredFramework(string targetFramework)
    {
        return TargetFrameworks.TryGetValue(targetFramework, out var framework)
            ? framework
            : throw new InvalidOperationException($"phase1 framework missing: {targetFramework}");
    }

    public static DependencyPhase1ContractLaneIndex Load(string phase1Directory)
    {
        var classificationPath = Path.Combine(Path.GetFullPath(phase1Directory), "assembly-classification-v1-01.json");
        using var document = JsonDocument.Parse(File.ReadAllText(classificationPath));
        var targetFrameworks = new Dictionary<string, DependencyPhase1FrameworkIndex>(StringComparer.Ordinal);

        foreach (var frameworkProperty in document.RootElement.GetProperty("targetFrameworks").EnumerateObject())
        {
            var contractCompleteAssemblies = new HashSet<string>(StringComparer.Ordinal);
            foreach (var assemblyProperty in frameworkProperty.Value.GetProperty("candidateAssemblies").EnumerateObject())
            {
                if (assemblyProperty.Value.TryGetProperty("contractCompleteEligible", out var eligibleProperty) &&
                    eligibleProperty.ValueKind == JsonValueKind.True)
                {
                    contractCompleteAssemblies.Add(assemblyProperty.Name);
                }
            }

            targetFrameworks[frameworkProperty.Name] = new DependencyPhase1FrameworkIndex(contractCompleteAssemblies);
        }

        return new DependencyPhase1ContractLaneIndex(targetFrameworks);
    }
}

internal sealed class DependencyPhase1FrameworkIndex
{
    public DependencyPhase1FrameworkIndex(IReadOnlyCollection<string> contractCompleteAssemblies)
    {
        ContractCompleteAssemblies = contractCompleteAssemblies;
    }

    public IReadOnlyCollection<string> ContractCompleteAssemblies { get; }

    public bool IsContractCompleteEligible(string assemblyName)
    {
        return ContractCompleteAssemblies.Contains(assemblyName, StringComparer.Ordinal);
    }
}

internal sealed class DependencyPhase3ExecutionIndex
{
    private DependencyPhase3ExecutionIndex(string batchName, IReadOnlyList<string> selectedBatchFamilyNames)
    {
        BatchName = batchName;
        SelectedBatchFamilyNames = selectedBatchFamilyNames;
    }

    public string BatchName { get; }

    public IReadOnlyList<string> SelectedBatchFamilyNames { get; }

    public static DependencyPhase3ExecutionIndex Load(string phase3Directory)
    {
        var executionEntryPath = Path.Combine(Path.GetFullPath(phase3Directory), "semantic-family-execution-entry-v1-01.json");
        using var document = JsonDocument.Parse(File.ReadAllText(executionEntryPath));
        var root = document.RootElement;
        var selectedBatchFamilyNames = root.GetProperty("selectedBatchFamilyNames")
            .EnumerateArray()
            .Where(static value => value.ValueKind == JsonValueKind.String)
            .Select(static value => value.GetString())
            .Where(static value => !string.IsNullOrWhiteSpace(value))
            .Cast<string>()
            .ToArray();

        return new DependencyPhase3ExecutionIndex(
            root.GetProperty("batchName").GetString() ?? throw new InvalidOperationException("phase3 batchName missing"),
            selectedBatchFamilyNames);
    }
}

internal sealed record DependencyRegistryCommandEntry(
    string Id,
    string Type,
    string? ManifestPath,
    string? Alias,
    string? DisplayName,
    string? SourceEntry,
    string CanonicalCommand,
    string? DefaultGoalId,
    string? DefaultMatrixId,
    string? CapabilityFamilyLabel,
    string? CapabilityItemLabel,
    string? CategoryLabel,
    IReadOnlyList<string> PipelineIds,
    IReadOnlyList<string> SupportedHosts);

internal sealed class DependencyRegistrySnapshotIndex
{
    private readonly IReadOnlyDictionary<string, DependencyRegistryCommandEntry> _declaredUnitTestsByAlias;
    private readonly IReadOnlyDictionary<string, DependencyRegistryCommandEntry> _declaredBenchmarksByAlias;
    private readonly IReadOnlyDictionary<string, DependencyRegistryCommandEntry> _engineeringValidationsById;
    private readonly IReadOnlyDictionary<string, DependencyRegistryCommandEntry> _engineeringWorkloadsById;

    private DependencyRegistrySnapshotIndex(
        string registrySnapshotPath,
        string ownerSubjectId,
        string subjectManifestPath,
        IReadOnlyDictionary<string, DependencyRegistryCommandEntry> declaredUnitTestsByAlias,
        IReadOnlyDictionary<string, DependencyRegistryCommandEntry> declaredBenchmarksByAlias,
        IReadOnlyDictionary<string, DependencyRegistryCommandEntry> engineeringValidationsById,
        IReadOnlyDictionary<string, DependencyRegistryCommandEntry> engineeringWorkloadsById)
    {
        RegistrySnapshotPath = registrySnapshotPath;
        OwnerSubjectId = ownerSubjectId;
        SubjectManifestPath = subjectManifestPath;
        _declaredUnitTestsByAlias = declaredUnitTestsByAlias;
        _declaredBenchmarksByAlias = declaredBenchmarksByAlias;
        _engineeringValidationsById = engineeringValidationsById;
        _engineeringWorkloadsById = engineeringWorkloadsById;
    }

    public string RegistrySnapshotPath { get; }

    public string OwnerSubjectId { get; }

    public string SubjectManifestPath { get; }

    public bool TryGetDeclaredUnitTestByAlias(string alias, out DependencyRegistryCommandEntry entry)
    {
        return _declaredUnitTestsByAlias.TryGetValue(alias, out entry!);
    }

    public bool TryGetDeclaredBenchmarkByAlias(string alias, out DependencyRegistryCommandEntry entry)
    {
        return _declaredBenchmarksByAlias.TryGetValue(alias, out entry!);
    }

    public DependencyRegistryCommandEntry GetRequiredEngineeringValidationById(string id)
    {
        return GetRequiredEntry(_engineeringValidationsById, id, "engineering validation id");
    }

    public DependencyRegistryCommandEntry GetRequiredEngineeringWorkloadById(string id)
    {
        return GetRequiredEntry(_engineeringWorkloadsById, id, "engineering workload id");
    }

    public static DependencyRegistrySnapshotIndex Load(string registrySnapshotPath)
    {
        var fullPath = Path.GetFullPath(registrySnapshotPath);
        using var document = JsonDocument.Parse(File.ReadAllText(fullPath));
        var root = document.RootElement;

        var declaredUnitTestsByAlias = new Dictionary<string, DependencyRegistryCommandEntry>(StringComparer.Ordinal);
        var declaredBenchmarksByAlias = new Dictionary<string, DependencyRegistryCommandEntry>(StringComparer.Ordinal);
        var engineeringValidationsById = new Dictionary<string, DependencyRegistryCommandEntry>(StringComparer.Ordinal);
        var engineeringWorkloadsById = new Dictionary<string, DependencyRegistryCommandEntry>(StringComparer.Ordinal);

        foreach (var element in root.GetProperty("declaredUnitTests").EnumerateArray())
        {
            if (!string.Equals(GetOptionalString(element, "subjectId"), "SolutionCorePack", StringComparison.Ordinal))
            {
                continue;
            }

            var alias = GetOptionalString(element, "alias");
            if (string.IsNullOrWhiteSpace(alias))
            {
                continue;
            }

            declaredUnitTestsByAlias[alias] = CreateCommandEntry(element);
        }

        foreach (var element in root.GetProperty("declaredBenchmarks").EnumerateArray())
        {
            if (!string.Equals(GetOptionalString(element, "subjectId"), "SolutionCorePack", StringComparison.Ordinal))
            {
                continue;
            }

            var alias = GetOptionalString(element, "alias");
            if (string.IsNullOrWhiteSpace(alias))
            {
                continue;
            }

            declaredBenchmarksByAlias[alias] = CreateCommandEntry(element);
        }

        foreach (var element in root.GetProperty("engineeringValidations").EnumerateArray())
        {
            var entry = CreateCommandEntry(element);
            engineeringValidationsById[entry.Id] = entry;
        }

        foreach (var element in root.GetProperty("engineeringWorkloads").EnumerateArray())
        {
            var entry = CreateCommandEntry(element);
            engineeringWorkloadsById[entry.Id] = entry;
        }

        var subjectManifestPath = declaredUnitTestsByAlias.Values
            .Select(static value => value.ManifestPath)
            .FirstOrDefault(static value => !string.IsNullOrWhiteSpace(value));
        if (string.IsNullOrWhiteSpace(subjectManifestPath))
        {
            throw new InvalidOperationException("failed to resolve SolutionCorePack subject manifest path from registry snapshot");
        }

        return new DependencyRegistrySnapshotIndex(
            fullPath,
            "SolutionCorePack",
            subjectManifestPath!,
            declaredUnitTestsByAlias,
            declaredBenchmarksByAlias,
            engineeringValidationsById,
            engineeringWorkloadsById);
    }

    private static DependencyRegistryCommandEntry CreateCommandEntry(JsonElement element)
    {
        return new DependencyRegistryCommandEntry(
            GetRequiredString(element, "id"),
            GetRequiredString(element, "type"),
            GetOptionalString(element, "manifestPath"),
            GetOptionalString(element, "alias"),
            GetOptionalString(element, "displayName"),
            GetOptionalString(element, "sourceEntry"),
            GetRequiredString(element, "canonicalCommand"),
            GetOptionalString(element, "defaultGoalId"),
            GetOptionalString(element, "defaultMatrixId"),
            GetOptionalString(element, "capabilityFamilyLabel"),
            GetOptionalString(element, "capabilityItemLabel"),
            GetOptionalString(element, "categoryLabel"),
            GetOptionalStringArray(element, "pipelineIds"),
            GetOptionalStringArray(element, "supportedHosts"));
    }

    private static DependencyRegistryCommandEntry GetRequiredEntry(
        IReadOnlyDictionary<string, DependencyRegistryCommandEntry> index,
        string key,
        string description)
    {
        if (index.TryGetValue(key, out var entry))
        {
            return entry;
        }

        throw new InvalidOperationException($"registry snapshot missing required {description}: {key}");
    }

    private static string GetRequiredString(JsonElement element, string propertyName)
    {
        if (!element.TryGetProperty(propertyName, out var property) || property.ValueKind != JsonValueKind.String)
        {
            throw new InvalidOperationException($"registry entry missing required string property '{propertyName}'");
        }

        return property.GetString() ?? throw new InvalidOperationException($"registry property '{propertyName}' is null");
    }

    private static string? GetOptionalString(JsonElement element, string propertyName)
    {
        if (!element.TryGetProperty(propertyName, out var property) || property.ValueKind != JsonValueKind.String)
        {
            return null;
        }

        return property.GetString();
    }

    private static string[] GetOptionalStringArray(JsonElement element, string propertyName)
    {
        if (!element.TryGetProperty(propertyName, out var property) || property.ValueKind != JsonValueKind.Array)
        {
            return [];
        }

        return property.EnumerateArray()
            .Where(static item => item.ValueKind == JsonValueKind.String)
            .Select(static item => item.GetString())
            .Where(static value => !string.IsNullOrWhiteSpace(value))
            .Cast<string>()
            .ToArray();
    }
}

internal sealed class DependencyLayerPlanPayload
{
    public int SchemaVersion { get; set; } = 1;
    public string TaskId { get; set; } = string.Empty;
    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;
    public DependencyInputAuthoritiesPayload InputAuthorities { get; set; } = new();
    public DependencyCertifiedBaselinePayload CertifiedBaseline { get; set; } = new();
    public SortedDictionary<string, DependencyLayerPlanFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class DependencyInputAuthoritiesPayload
{
    public string Phase0CatalogPath { get; set; } = string.Empty;
    public string Phase1Directory { get; set; } = string.Empty;
    public string Phase2Directory { get; set; } = string.Empty;
    public string Phase3Directory { get; set; } = string.Empty;
    public string RegistrySnapshotPath { get; set; } = string.Empty;
}

internal sealed class DependencyCertifiedBaselinePayload
{
    public string OwnerSubjectId { get; set; } = string.Empty;
    public string Phase3BatchName { get; set; } = string.Empty;
    public string[] Phase3SemanticFamilies { get; set; } = [];
    public SortedDictionary<string, DependencyCertifiedBaselineFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class DependencyCertifiedBaselineFrameworkPayload
{
    public int ContractCertifiedAssemblyCount { get; set; }
    public string[] ContractCertifiedAssemblies { get; set; } = [];
    public int CoreLibHelperKindCount { get; set; }
    public string[] CoreLibHelperKinds { get; set; } = [];
}

internal sealed class DependencyLayerPlanFrameworkPayload
{
    public string SelectedVersion { get; set; } = string.Empty;
    public string SourceKind { get; set; } = string.Empty;
    public int SharedSystemAssemblyCount { get; set; }
    public int CandidateAssemblyCount { get; set; }
    public int LayerCount { get; set; }
    public string FirstReadyLayerName { get; set; } = string.Empty;
    public SortedDictionary<string, DependencyLayerPlanLayerPayload> Layers { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class DependencyLayerPlanLayerPayload
{
    public int Order { get; set; }
    public string ReadinessStatus { get; set; } = string.Empty;
    public int AssemblyCount { get; set; }
    public int ReadyAssemblyCount { get; set; }
    public int BlockedAssemblyCount { get; set; }
    public string[] UpstreamLayerNames { get; set; } = [];
    public string[] CapabilityFamilies { get; set; } = [];
    public string[] AssemblyPlanPaths { get; set; } = [];
    public SortedDictionary<string, DependencyLayerPlanAssemblyPayload> Assemblies { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class DependencyLayerPlanAssemblyPayload
{
    public string Classification { get; set; } = string.Empty;
    public string Status { get; set; } = string.Empty;
    public bool SurfaceContractCertified { get; set; }
    public int PublicTypeDefinitionCount { get; set; }
    public int PublicMemberCount { get; set; }
    public int RuntimeMethodBodyCount { get; set; }
    public string[] CapabilityFamilies { get; set; } = [];
    public string[] DependencyAssemblies { get; set; } = [];
    public string[] CandidateDependencyAssemblies { get; set; } = [];
    public string[] CertifiedDependencyAssemblies { get; set; } = [];
    public string[] ExternalDependencyAssemblies { get; set; } = [];
    public string[] BlockerReasons { get; set; } = [];
}

internal sealed class DependencyProofBenchmarkLanePayload
{
    public int SchemaVersion { get; set; } = 1;
    public string TaskId { get; set; } = string.Empty;
    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;
    public string OwnerSubjectId { get; set; } = string.Empty;
    public string RegistrySnapshotPath { get; set; } = string.Empty;
    public string SubjectManifestPath { get; set; } = string.Empty;
    public string[] RequiredGates { get; set; } = [];
    public SortedDictionary<string, DependencyProofBenchmarkFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class DependencyProofBenchmarkFrameworkPayload
{
    public int LayerCount { get; set; }
    public SortedDictionary<string, DependencyProofBenchmarkLayerPayload> Layers { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class DependencyProofBenchmarkLayerPayload
{
    public int Order { get; set; }
    public string ReadinessStatus { get; set; } = string.Empty;
    public int AssemblyCount { get; set; }
    public string[] AssemblyNames { get; set; } = [];
    public string[] CapabilityFamilies { get; set; } = [];
    public string[] RequiredGates { get; set; } = [];
    public string[] MonitorSignals { get; set; } = [];
    public SemanticFamilyCommandPayload[] NativeProofs { get; set; } = [];
    public SemanticFamilyCommandPayload[] Benchmarks { get; set; } = [];
    public SemanticFamilyCommandPayload[] ConformanceGates { get; set; } = [];
    public SemanticFamilyCommandPayload[] EngineeringWorkloads { get; set; } = [];
}

internal sealed class DependencyAssemblyNativeizationPlanPayload
{
    public int SchemaVersion { get; set; } = 1;
    public string TaskId { get; set; } = string.Empty;
    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;
    public string AssemblyName { get; set; } = string.Empty;
    public string SelectionReason { get; set; } = string.Empty;
    public string OwnerSubjectId { get; set; } = string.Empty;
    public string SubjectManifestPath { get; set; } = string.Empty;
    public string[] RequiredGates { get; set; } = [];
    public string[] MonitorSignals { get; set; } = [];
    public SemanticFamilyCommandPayload[] NativeProofs { get; set; } = [];
    public SemanticFamilyCommandPayload[] Benchmarks { get; set; } = [];
    public SemanticFamilyCommandPayload[] ConformanceGates { get; set; } = [];
    public SemanticFamilyCommandPayload[] EngineeringWorkloads { get; set; } = [];
    public SortedDictionary<string, DependencyAssemblyNativeizationFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class DependencyAssemblyNativeizationFrameworkPayload
{
    public string SelectedVersion { get; set; } = string.Empty;
    public string SourceKind { get; set; } = string.Empty;
    public string LayerName { get; set; } = string.Empty;
    public string Status { get; set; } = string.Empty;
    public string Classification { get; set; } = string.Empty;
    public bool SurfaceContractCertified { get; set; }
    public int PublicTypeDefinitionCount { get; set; }
    public int PublicMemberCount { get; set; }
    public int RuntimeMethodBodyCount { get; set; }
    public string[] CapabilityFamilies { get; set; } = [];
    public string[] DependencyAssemblies { get; set; } = [];
    public string[] CandidateDependencyAssemblies { get; set; } = [];
    public string[] CertifiedDependencyAssemblies { get; set; } = [];
    public string[] ExternalDependencyAssemblies { get; set; } = [];
    public string[] BlockerReasons { get; set; } = [];
}
