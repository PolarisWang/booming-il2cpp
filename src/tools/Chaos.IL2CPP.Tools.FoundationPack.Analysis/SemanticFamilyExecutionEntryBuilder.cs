using System.Text.Json;

namespace Chaos.IL2CPP.Tools.FoundationPack.Analysis;

internal static class SemanticFamilyExecutionEntryBuilder
{
    private static readonly StringComparer NameComparer = StringComparer.Ordinal;

    private static readonly SemanticFamilyBatchDefinition[] FirstBatchDefinitions =
    [
        new(
            "managed-cil",
            NativeProofSourceEntries:
            [
                "CoreRuntimeFeatures/ArithmeticOpsProofEntry::Run()",
                "CoreRuntimeFeatures/AsyncAwaitProofEntry::Run()",
                "CoreRuntimeFeatures/DelegateProofEntry::Run()",
                "CoreRuntimeFeatures/ExceptionProofEntry::Run()",
                "CoreRuntimeFeatures/InterfaceDispatchProofEntry::Run()",
                "CoreRuntimeFeatures/TaskAndValueTaskFlowProofEntry::Run()",
            ],
            BenchmarkSourceEntries:
            [
                "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
                "CoreRuntimeBenchmarks/DispatchBenchmarkEntry::RunWorkload()",
                "CoreRuntimeBenchmarks/TaskAndValueTaskFlowBenchmarkEntry::RunWorkload()",
            ],
            ConformanceGateIds:
            [
                "engineering-validation/SolutionCorePack/managed-build",
                "engineering-validation/SolutionCorePack/managed-runtime-output",
                "engineering-validation/SolutionCorePack/native-executable-smoke",
                "engineering-validation/SolutionCorePack/project-graph",
            ],
            EngineeringWorkloadIds:
            [
                "engineering-workload/SolutionCorePack/codegen",
                "engineering-workload/SolutionCorePack/convert",
                "engineering-workload/SolutionCorePack/native-link",
            ],
            CoverageNotes:
            [
                "Covers canonical managed method bodies that already have runnable SolutionCorePack proofs.",
                "Pairs native proof entries with managed/native benchmark workloads to keep semantic-family progress measurable.",
            ]),
        new(
            "internal-call",
            NativeProofSourceEntries:
            [
                "CoreRuntimeFeatures/RuntimeHelpersProofEntry::Run()",
                "CoreRuntimeFeatures/ThreadingProofEntry::Run()",
                "CoreRuntimeFeatures/MonitorAndLockingProofEntry::Run()",
                "CoreRuntimeFeatures/TaskSchedulingProofEntry::Run()",
            ],
            BenchmarkSourceEntries:
            [
                "CoreRuntimeBenchmarks/MonitorAndLockingBenchmarkEntry::RunWorkload()",
                "CoreRuntimeBenchmarks/TaskSchedulingBenchmarkEntry::RunWorkload()",
            ],
            ConformanceGateIds:
            [
                "engineering-validation/SolutionCorePack/native-executable-smoke",
                "engineering-validation/SolutionCorePack/project-graph",
            ],
            EngineeringWorkloadIds:
            [
                "engineering-workload/SolutionCorePack/convert",
                "engineering-workload/SolutionCorePack/native-link",
            ],
            CoverageNotes:
            [
                "Anchors runtime-backed helper obligations to concrete threading and runtime-helper proofs.",
                "Uses native smoke plus conversion/link workloads as the conformance floor before expanding deeper helper families.",
            ]),
        new(
            "field-metadata",
            NativeProofSourceEntries:
            [
                "CoreRuntimeFeatures/MemberMetadataLookupProofEntry::Run()",
                "CoreRuntimeFeatures/CustomAttributeLookupProofEntry::Run()",
                "CoreRuntimeFeatures/MetadataSupplementProofEntry::Run()",
                "CoreRuntimeFeatures/ReflectionInteropClosureEntry::Run()",
            ],
            BenchmarkSourceEntries:
            [
                "CoreRuntimeBenchmarks/MemberMetadataLookupBenchmarkEntry::RunWorkload()",
            ],
            ConformanceGateIds:
            [
                "engineering-validation/SolutionCorePack/native-executable-smoke",
                "engineering-validation/SolutionCorePack/project-graph",
            ],
            EngineeringWorkloadIds:
            [
                "engineering-workload/SolutionCorePack/codegen",
                "engineering-workload/SolutionCorePack/convert",
            ],
            CoverageNotes:
            [
                "Focuses the first metadata batch on lookup-heavy proofs that exercise field and member registration paths.",
                "Keeps codegen workload explicit because metadata families are sensitive to registration and supplemental output shape.",
            ]),
    ];

    public static SemanticFamilyExecutionEntryPayload Build(
        Phase2SubstrateIndex phase2Index,
        string taskId,
        string registrySnapshotPath)
    {
        var registry = RegistrySnapshotIndex.Load(registrySnapshotPath);
        var targetFrameworks = new SortedDictionary<string, SemanticFamilyExecutionFrameworkPayload>(NameComparer);

        foreach (var targetFramework in phase2Index.TargetFrameworks.Keys.OrderBy(static value => value, NameComparer))
        {
            var framework = phase2Index.TargetFrameworks[targetFramework];
            var selectedFamilies = new SortedDictionary<string, SemanticFamilyExecutionFamilyPayload>(NameComparer);
            var issues = new List<string>();
            var selectedMemberCount = 0;

            foreach (var definition in FirstBatchDefinitions)
            {
                if (!framework.MemberIdsByClassification.TryGetValue(definition.FamilyName, out var members) || members.Count == 0)
                {
                    issues.Add($"{targetFramework}:{definition.FamilyName}:missing-phase2-members");
                    continue;
                }

                selectedFamilies[definition.FamilyName] = new SemanticFamilyExecutionFamilyPayload
                {
                    Priority = GetSemanticFamilyPriority(definition.FamilyName),
                    MemberCount = members.Count,
                    RequiredProofs = GetRequiredProofsForFamily(definition.FamilyName),
                    CoverageNotes = definition.CoverageNotes.ToArray(),
                    NativeProofs = definition.NativeProofSourceEntries
                        .Select(registry.GetRequiredDeclaredUnitTestBySourceEntry)
                        .Select(ToCommandPayload)
                        .ToArray(),
                    ConformanceGates = definition.ConformanceGateIds
                        .Select(registry.GetRequiredEngineeringValidationById)
                        .Select(ToCommandPayload)
                        .ToArray(),
                    Benchmarks = definition.BenchmarkSourceEntries
                        .Select(registry.GetRequiredDeclaredBenchmarkBySourceEntry)
                        .Select(ToCommandPayload)
                        .ToArray(),
                    EngineeringWorkloads = definition.EngineeringWorkloadIds
                        .Select(registry.GetRequiredEngineeringWorkloadById)
                        .Select(ToCommandPayload)
                        .ToArray(),
                };
                selectedMemberCount += members.Count;
            }

            targetFrameworks[targetFramework] = new SemanticFamilyExecutionFrameworkPayload
            {
                SelectedFamilyCount = selectedFamilies.Count,
                SelectedMemberCount = selectedMemberCount,
                SelectedFamilies = selectedFamilies,
                IssueCount = issues.Count,
                Issues = issues,
            };
        }

        return new SemanticFamilyExecutionEntryPayload
        {
            TaskId = taskId,
            BatchName = "corelib-semantic-family-batch-01",
            OwnerSubjectId = registry.OwnerSubjectId,
            RegistrySnapshotPath = NormalizePath(registry.RegistrySnapshotPath),
            SubjectManifestPath = NormalizePath(registry.SubjectManifestPath),
            SelectionPolicy = "priority-and-existing-proof-coverage",
            SelectedBatchFamilyNames = FirstBatchDefinitions.Select(static definition => definition.FamilyName).ToArray(),
            TargetFrameworks = targetFrameworks,
        };
    }

    private static SemanticFamilyCommandPayload ToCommandPayload(RegistryCommandEntry entry)
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

    private static int GetSemanticFamilyPriority(string familyName)
    {
        return familyName switch
        {
            "managed-cil" => 1,
            "internal-call" => 2,
            "runtime-special" => 3,
            "pinvoke" => 4,
            "field-metadata" => 5,
            "abstract" => 6,
            "composite" => 7,
            "metadata-only" => 8,
            _ => 9,
        };
    }

    private static string[] GetRequiredProofsForFamily(string familyName)
    {
        return familyName switch
        {
            "managed-cil" => ["native-proof", "conformance", "benchmark"],
            "internal-call" => ["helper-contract-proof", "native-proof", "conformance", "benchmark"],
            "runtime-special" => ["helper-contract-proof", "native-proof", "conformance"],
            "pinvoke" => ["interop-proof", "native-proof", "conformance"],
            "field-metadata" => ["layout-proof", "conformance", "benchmark"],
            "abstract" => ["contract-proof"],
            "composite" => ["accessor-review", "native-proof", "conformance"],
            _ => ["native-proof", "conformance"],
        };
    }

    private static string NormalizePath(string path)
    {
        return path.Replace('\\', '/');
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

    private sealed record SemanticFamilyBatchDefinition(
        string FamilyName,
        IReadOnlyList<string> NativeProofSourceEntries,
        IReadOnlyList<string> BenchmarkSourceEntries,
        IReadOnlyList<string> ConformanceGateIds,
        IReadOnlyList<string> EngineeringWorkloadIds,
        IReadOnlyList<string> CoverageNotes);

    private sealed record RegistryCommandEntry(
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

    private sealed class RegistrySnapshotIndex
    {
        private readonly IReadOnlyDictionary<string, RegistryCommandEntry> _declaredUnitTestsBySourceEntry;
        private readonly IReadOnlyDictionary<string, RegistryCommandEntry> _declaredBenchmarksBySourceEntry;
        private readonly IReadOnlyDictionary<string, RegistryCommandEntry> _engineeringValidationsById;
        private readonly IReadOnlyDictionary<string, RegistryCommandEntry> _engineeringWorkloadsById;

        private RegistrySnapshotIndex(
            string registrySnapshotPath,
            string ownerSubjectId,
            string subjectManifestPath,
            IReadOnlyDictionary<string, RegistryCommandEntry> declaredUnitTestsBySourceEntry,
            IReadOnlyDictionary<string, RegistryCommandEntry> declaredBenchmarksBySourceEntry,
            IReadOnlyDictionary<string, RegistryCommandEntry> engineeringValidationsById,
            IReadOnlyDictionary<string, RegistryCommandEntry> engineeringWorkloadsById)
        {
            RegistrySnapshotPath = registrySnapshotPath;
            OwnerSubjectId = ownerSubjectId;
            SubjectManifestPath = subjectManifestPath;
            _declaredUnitTestsBySourceEntry = declaredUnitTestsBySourceEntry;
            _declaredBenchmarksBySourceEntry = declaredBenchmarksBySourceEntry;
            _engineeringValidationsById = engineeringValidationsById;
            _engineeringWorkloadsById = engineeringWorkloadsById;
        }

        public string RegistrySnapshotPath { get; }

        public string OwnerSubjectId { get; }

        public string SubjectManifestPath { get; }

        public RegistryCommandEntry GetRequiredDeclaredUnitTestBySourceEntry(string sourceEntry)
        {
            return GetRequiredEntry(_declaredUnitTestsBySourceEntry, sourceEntry, "declared unit test sourceEntry");
        }

        public RegistryCommandEntry GetRequiredDeclaredBenchmarkBySourceEntry(string sourceEntry)
        {
            return GetRequiredEntry(_declaredBenchmarksBySourceEntry, sourceEntry, "declared benchmark sourceEntry");
        }

        public RegistryCommandEntry GetRequiredEngineeringValidationById(string id)
        {
            return GetRequiredEntry(_engineeringValidationsById, id, "engineering validation id");
        }

        public RegistryCommandEntry GetRequiredEngineeringWorkloadById(string id)
        {
            return GetRequiredEntry(_engineeringWorkloadsById, id, "engineering workload id");
        }

        public static RegistrySnapshotIndex Load(string registrySnapshotPath)
        {
            var fullPath = Path.GetFullPath(registrySnapshotPath);
            if (!File.Exists(fullPath))
            {
                throw new FileNotFoundException("registry snapshot missing", fullPath);
            }

            using var document = JsonDocument.Parse(File.ReadAllText(fullPath));
            var root = document.RootElement;

            var declaredUnitTestsBySourceEntry = new Dictionary<string, RegistryCommandEntry>(NameComparer);
            var declaredBenchmarksBySourceEntry = new Dictionary<string, RegistryCommandEntry>(NameComparer);
            var engineeringValidationsById = new Dictionary<string, RegistryCommandEntry>(NameComparer);
            var engineeringWorkloadsById = new Dictionary<string, RegistryCommandEntry>(NameComparer);

            foreach (var element in root.GetProperty("declaredUnitTests").EnumerateArray())
            {
                if (!string.Equals(GetOptionalString(element, "subjectId"), "SolutionCorePack", StringComparison.Ordinal) ||
                    !string.Equals(GetOptionalString(element, "assemblyName"), "CoreRuntimeFeatures", StringComparison.Ordinal))
                {
                    continue;
                }

                var sourceEntry = GetOptionalString(element, "sourceEntry");
                if (string.IsNullOrWhiteSpace(sourceEntry))
                {
                    continue;
                }

                declaredUnitTestsBySourceEntry[sourceEntry] = CreateCommandEntry(element);
            }

            foreach (var element in root.GetProperty("declaredBenchmarks").EnumerateArray())
            {
                if (!string.Equals(GetOptionalString(element, "subjectId"), "SolutionCorePack", StringComparison.Ordinal) ||
                    !string.Equals(GetOptionalString(element, "assemblyName"), "CoreRuntimeBenchmarks", StringComparison.Ordinal))
                {
                    continue;
                }

                var sourceEntry = GetOptionalString(element, "sourceEntry");
                if (string.IsNullOrWhiteSpace(sourceEntry))
                {
                    continue;
                }

                declaredBenchmarksBySourceEntry[sourceEntry] = CreateCommandEntry(element);
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

            var subjectManifestPath = declaredUnitTestsBySourceEntry.Values
                .Select(static entry => entry.ManifestPath)
                .FirstOrDefault(static value => !string.IsNullOrWhiteSpace(value));
            if (string.IsNullOrWhiteSpace(subjectManifestPath))
            {
                throw new InvalidOperationException("failed to resolve SolutionCorePack subject manifest path from registry snapshot");
            }

            return new RegistrySnapshotIndex(
                fullPath,
                "SolutionCorePack",
                subjectManifestPath!,
                declaredUnitTestsBySourceEntry,
                declaredBenchmarksBySourceEntry,
                engineeringValidationsById,
                engineeringWorkloadsById);
        }

        private static RegistryCommandEntry CreateCommandEntry(JsonElement element)
        {
            return new RegistryCommandEntry(
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

        private static RegistryCommandEntry GetRequiredEntry(
            IReadOnlyDictionary<string, RegistryCommandEntry> index,
            string key,
            string description)
        {
            if (index.TryGetValue(key, out var entry))
            {
                return entry;
            }

            throw new InvalidOperationException($"registry snapshot missing required {description}: {key}");
        }
    }
}

internal sealed class SemanticFamilyExecutionEntryPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public string BatchName { get; set; } = string.Empty;

    public string OwnerSubjectId { get; set; } = string.Empty;

    public string RegistrySnapshotPath { get; set; } = string.Empty;

    public string SubjectManifestPath { get; set; } = string.Empty;

    public string SelectionPolicy { get; set; } = string.Empty;

    public string[] SelectedBatchFamilyNames { get; set; } = [];

    public SortedDictionary<string, SemanticFamilyExecutionFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class SemanticFamilyExecutionFrameworkPayload
{
    public int SelectedFamilyCount { get; set; }

    public int SelectedMemberCount { get; set; }

    public SortedDictionary<string, SemanticFamilyExecutionFamilyPayload> SelectedFamilies { get; set; } = new(StringComparer.Ordinal);

    public int IssueCount { get; set; }

    public List<string> Issues { get; set; } = [];
}

internal sealed class SemanticFamilyExecutionFamilyPayload
{
    public int Priority { get; set; }

    public int MemberCount { get; set; }

    public string[] RequiredProofs { get; set; } = [];

    public string[] CoverageNotes { get; set; } = [];

    public SemanticFamilyCommandPayload[] NativeProofs { get; set; } = [];

    public SemanticFamilyCommandPayload[] ConformanceGates { get; set; } = [];

    public SemanticFamilyCommandPayload[] Benchmarks { get; set; } = [];

    public SemanticFamilyCommandPayload[] EngineeringWorkloads { get; set; } = [];
}

internal sealed class SemanticFamilyCommandPayload
{
    public string Id { get; set; } = string.Empty;

    public string Type { get; set; } = string.Empty;

    public string? Alias { get; set; }

    public string? DisplayName { get; set; }

    public string? SourceEntry { get; set; }

    public string CanonicalCommand { get; set; } = string.Empty;

    public string? DefaultGoalId { get; set; }

    public string? DefaultMatrixId { get; set; }

    public string? CapabilityFamilyLabel { get; set; }

    public string? CapabilityItemLabel { get; set; }

    public string? CategoryLabel { get; set; }

    public string[] PipelineIds { get; set; } = [];

    public string[] SupportedHosts { get; set; } = [];
}
