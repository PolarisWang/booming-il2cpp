using System.Text.Json.Nodes;

namespace Chaos.IL2CPP.Tools.FoundationPack.Analysis;

internal static class DependencyLayerCertificationReportBuilder
{
    private static readonly StringComparer NameComparer = StringComparer.Ordinal;

    public static IReadOnlyDictionary<string, JsonObject> Build(
        string assemblyPlanDirectory,
        string proofSummaryPath,
        string benchmarkSummaryPath,
        string taskId)
    {
        var normalizedAssemblyPlanDirectory = Path.GetFullPath(assemblyPlanDirectory);
        var normalizedProofSummaryPath = Path.GetFullPath(proofSummaryPath);
        var normalizedBenchmarkSummaryPath = Path.GetFullPath(benchmarkSummaryPath);

        var proofSummary = LoadRequiredObject(normalizedProofSummaryPath);
        var benchmarkSummary = LoadRequiredObject(normalizedBenchmarkSummaryPath);
        var selectedAssemblies = GetRequiredStringArray(proofSummary, "selectedAssemblies");
        var benchmarkAssemblies = new HashSet<string>(
            GetRequiredStringArray(benchmarkSummary, "selectedAssemblies"),
            NameComparer);

        foreach (var assemblyName in selectedAssemblies)
        {
            if (!benchmarkAssemblies.Contains(assemblyName))
            {
                throw new InvalidOperationException(
                    $"benchmark summary '{normalizedBenchmarkSummaryPath}' is missing selected assembly '{assemblyName}'.");
            }
        }

        var sourceTaskId = GetRequiredString(proofSummary, "taskId");
        var benchmarkSourceTaskId = GetRequiredString(benchmarkSummary, "taskId");
        var selectedAssemblySet = new HashSet<string>(selectedAssemblies, NameComparer);

        var gateResultsByGate = IndexByString(
            GetRequiredObjectArray(proofSummary, "gateResults"),
            "gate");
        var proofsById = IndexByString(GetRequiredObjectArray(proofSummary, "proofs"), "id");
        var proofsByAlias = IndexByString(GetRequiredObjectArray(proofSummary, "proofs"), "alias");
        var engineeringValidationsById = IndexByString(
            GetRequiredObjectArray(proofSummary, "engineeringValidations"),
            "id");
        var benchmarksById = IndexByString(GetRequiredObjectArray(benchmarkSummary, "benchmarks"), "id");
        var benchmarksByAlias = IndexByString(GetRequiredObjectArray(benchmarkSummary, "benchmarks"), "alias");

        var reports = new SortedDictionary<string, JsonObject>(NameComparer);
        foreach (var assemblyName in selectedAssemblies.OrderBy(static value => value, NameComparer))
        {
            var assemblyPlanPath = Path.Combine(normalizedAssemblyPlanDirectory, $"{assemblyName}.json");
            var assemblyPlan = LoadRequiredObject(assemblyPlanPath);
            reports[assemblyName] = BuildReport(
                taskId,
                sourceTaskId,
                benchmarkSourceTaskId,
                assemblyPlanPath,
                normalizedProofSummaryPath,
                normalizedBenchmarkSummaryPath,
                assemblyPlan,
                selectedAssemblySet,
                gateResultsByGate,
                proofsById,
                proofsByAlias,
                engineeringValidationsById,
                benchmarksById,
                benchmarksByAlias);
        }

        return reports;
    }

    private static JsonObject BuildReport(
        string taskId,
        string sourceTaskId,
        string benchmarkSourceTaskId,
        string assemblyPlanPath,
        string proofSummaryPath,
        string benchmarkSummaryPath,
        JsonObject assemblyPlan,
        IReadOnlySet<string> selectedAssemblies,
        IReadOnlyDictionary<string, JsonObject> gateResultsByGate,
        IReadOnlyDictionary<string, JsonObject> proofsById,
        IReadOnlyDictionary<string, JsonObject> proofsByAlias,
        IReadOnlyDictionary<string, JsonObject> engineeringValidationsById,
        IReadOnlyDictionary<string, JsonObject> benchmarksById,
        IReadOnlyDictionary<string, JsonObject> benchmarksByAlias)
    {
        var blockedReasons = new List<string>();
        var hasFailedEvidence = false;

        var requiredGates = GetRequiredStringArray(assemblyPlan, "requiredGates");
        var gateResults = new JsonArray();
        foreach (var gate in requiredGates)
        {
            if (!gateResultsByGate.TryGetValue(gate, out var gateResult))
            {
                blockedReasons.Add($"missing-gate:{gate}");
                continue;
            }

            gateResults.Add(gateResult.DeepClone());
            if (!string.Equals(GetOptionalString(gateResult, "status"), "ok", StringComparison.OrdinalIgnoreCase))
            {
                hasFailedEvidence = true;
            }
        }

        var nativeProofResults = new JsonArray();
        foreach (var proof in GetRequiredObjectArray(assemblyPlan, "nativeProofs"))
        {
            var proofId = GetRequiredString(proof, "id");
            var proofAlias = GetOptionalString(proof, "alias");
            var matchedProof = FindEvidence(proofsById, proofsByAlias, proofId, proofAlias);
            if (matchedProof is null)
            {
                blockedReasons.Add($"missing-native-proof:{proofId}");
                continue;
            }

            nativeProofResults.Add(BuildProofResult(proof, matchedProof));
            if (!string.Equals(GetOptionalString(matchedProof, "finalStatus"), "ok", StringComparison.OrdinalIgnoreCase))
            {
                hasFailedEvidence = true;
            }
        }

        var benchmarkResults = new JsonArray();
        foreach (var benchmark in GetRequiredObjectArray(assemblyPlan, "benchmarks"))
        {
            var benchmarkId = GetRequiredString(benchmark, "id");
            var benchmarkAlias = GetOptionalString(benchmark, "alias");
            var matchedBenchmark = FindEvidence(benchmarksById, benchmarksByAlias, benchmarkId, benchmarkAlias);
            if (matchedBenchmark is null)
            {
                blockedReasons.Add($"missing-benchmark:{benchmarkId}");
                continue;
            }

            benchmarkResults.Add(BuildBenchmarkResult(benchmark, matchedBenchmark));
            if (!string.Equals(GetOptionalString(matchedBenchmark, "status"), "ok", StringComparison.OrdinalIgnoreCase))
            {
                hasFailedEvidence = true;
            }
        }

        var engineeringValidationResults = new JsonArray();
        foreach (var gate in GetRequiredObjectArray(assemblyPlan, "conformanceGates"))
        {
            var gateId = GetRequiredString(gate, "id");
            if (!engineeringValidationsById.TryGetValue(gateId, out var matchedGate))
            {
                blockedReasons.Add($"missing-engineering-validation:{gateId}");
                continue;
            }

            engineeringValidationResults.Add(BuildEngineeringValidationResult(gate, matchedGate));
            if (!string.Equals(GetOptionalString(matchedGate, "finalStatus"), "ok", StringComparison.OrdinalIgnoreCase))
            {
                hasFailedEvidence = true;
            }
        }

        var engineeringWorkloadResults = new JsonArray();
        foreach (var workload in GetRequiredObjectArray(assemblyPlan, "engineeringWorkloads"))
        {
            engineeringWorkloadResults.Add(new JsonObject
            {
                ["id"] = GetRequiredString(workload, "id"),
                ["displayName"] = GetOptionalString(workload, "displayName"),
                ["canonicalCommand"] = GetOptionalString(workload, "canonicalCommand"),
                ["status"] = "not-required-for-final-status",
                ["evidenceAvailable"] = false,
            });
        }

        var targetFrameworksPayload = GetRequiredObject(assemblyPlan, "targetFrameworks");
        var targetFrameworks = targetFrameworksPayload
            .Select(static entry => entry.Key)
            .OrderBy(static value => value, NameComparer)
            .ToArray();
        var dependencyAssemblies = new SortedSet<string>(NameComparer);
        var candidateDependencyAssemblies = new SortedSet<string>(NameComparer);
        var certifiedDependencyAssemblies = new SortedSet<string>(NameComparer);
        var planBlockers = new List<string>();
        foreach (var frameworkPayload in targetFrameworksPayload)
        {
            var frameworkObject = AsRequiredObject(frameworkPayload.Value, $"targetFrameworks.{frameworkPayload.Key}");
            AddStringArray(dependencyAssemblies, frameworkObject["dependencyAssemblies"]);
            AddStringArray(candidateDependencyAssemblies, frameworkObject["candidateDependencyAssemblies"]);
            AddStringArray(certifiedDependencyAssemblies, frameworkObject["certifiedDependencyAssemblies"]);
            planBlockers.AddRange(GetOptionalStringArray(frameworkObject["blockerReasons"]));
        }

        foreach (var planBlocker in planBlockers.Distinct(NameComparer))
        {
            blockedReasons.Add($"plan-blocker:{planBlocker}");
        }

        foreach (var dependencyAssembly in candidateDependencyAssemblies)
        {
            if (!selectedAssemblies.Contains(dependencyAssembly) && !certifiedDependencyAssemblies.Contains(dependencyAssembly))
            {
                blockedReasons.Add($"uncertified-dependency:{dependencyAssembly}");
            }
        }

        var blockedReasonArray = new JsonArray(blockedReasons
            .Distinct(NameComparer)
            .OrderBy(static value => value, NameComparer)
            .Select(static value => (JsonNode?)JsonValue.Create(value))
            .ToArray());

        var benchmarkStatuses = benchmarkResults
            .OfType<JsonObject>()
            .Select(static item => GetOptionalString(item, "regressionStatus"))
            .Where(static item => !string.IsNullOrWhiteSpace(item))
            .ToArray();
        var benchmarkAcceptance = benchmarkStatuses.Length > 0 &&
                                  benchmarkStatuses.All(static item => string.Equals(item, "no-baseline", StringComparison.OrdinalIgnoreCase))
            ? "throughput-only"
            : "baseline-available";

        var hasProjectedEvidence = gateResults.Count > 0 ||
                                   nativeProofResults.Count > 0 ||
                                   benchmarkResults.Count > 0 ||
                                   engineeringValidationResults.Count > 0;

        var finalStatus = blockedReasons.Count > 0
            ? (hasFailedEvidence ? "failed" : "blocked")
            : (hasFailedEvidence ? "failed" : "ok");
        var reportState = finalStatus switch
        {
            "ok" => "ok",
            "failed" => "failed",
            _ when hasProjectedEvidence => "evidence-ready",
            _ => "blocked",
        };

        var noteArray = new JsonArray
        {
            $"source proof summary task: {sourceTaskId}",
            $"source benchmark summary task: {benchmarkSourceTaskId}",
            "engineering workloads are preserved for bookkeeping but do not gate finalStatus in the current acceptance path.",
        };

        var evidencePaths = new JsonObject
        {
            ["assemblyPlanPath"] = NormalizePath(assemblyPlanPath),
            ["proofSummaryPath"] = NormalizePath(proofSummaryPath),
            ["benchmarkSummaryPath"] = NormalizePath(benchmarkSummaryPath),
            ["proofRunSummaryPaths"] = BuildUniquePathArray(nativeProofResults, "summaryPath"),
            ["benchmarkRunSummaryPaths"] = BuildUniquePathArray(benchmarkResults, "summaryPath"),
            ["engineeringValidationSummaryPaths"] = BuildUniquePathArray(engineeringValidationResults, "summaryPath"),
        };

        return new JsonObject
        {
            ["schemaVersion"] = 1,
            ["taskId"] = taskId,
            ["sourceTaskId"] = sourceTaskId,
            ["assemblyName"] = GetRequiredString(assemblyPlan, "assemblyName"),
            ["ownerSubjectId"] = GetRequiredString(assemblyPlan, "ownerSubjectId"),
            ["targetFrameworks"] = new JsonArray(targetFrameworks.Select(static value => (JsonNode?)JsonValue.Create(value)).ToArray()),
            ["requiredGates"] = new JsonArray(requiredGates.Select(static value => (JsonNode?)JsonValue.Create(value)).ToArray()),
            ["dependencyAssemblies"] = new JsonArray(dependencyAssemblies.Select(static value => (JsonNode?)JsonValue.Create(value)).ToArray()),
            ["gateResults"] = gateResults,
            ["nativeProofResults"] = nativeProofResults,
            ["benchmarkResults"] = benchmarkResults,
            ["engineeringValidationResults"] = engineeringValidationResults,
            ["engineeringWorkloadResults"] = engineeringWorkloadResults,
            ["evidencePaths"] = evidencePaths,
            ["blockedReasons"] = blockedReasonArray,
            ["reportState"] = reportState,
            ["benchmarkAcceptance"] = benchmarkAcceptance,
            ["dependencyStatus"] = blockedReasons.Any(static value => value.StartsWith("uncertified-dependency:", StringComparison.Ordinal))
                ? "blocked"
                : "certified-or-in-scope",
            ["crossVersionStatus"] = targetFrameworks.Length >= 2 ? "aligned" : "single-target-framework",
            ["notes"] = noteArray,
            ["finalStatus"] = finalStatus,
        };
    }

    private static JsonObject BuildProofResult(JsonObject planProof, JsonObject matchedProof)
    {
        return new JsonObject
        {
            ["id"] = GetRequiredString(planProof, "id"),
            ["alias"] = GetOptionalString(planProof, "alias"),
            ["canonicalCommand"] = GetOptionalString(planProof, "canonicalCommand"),
            ["runId"] = GetOptionalString(matchedProof, "runId"),
            ["finalStatus"] = GetOptionalString(matchedProof, "finalStatus"),
            ["exitCode"] = GetOptionalInt32(matchedProof, "exitCode"),
            ["summaryPath"] = GetOptionalString(matchedProof, "summaryPath"),
            ["eventsPath"] = GetOptionalString(matchedProof, "eventsPath"),
            ["consolePath"] = GetOptionalString(matchedProof, "consolePath"),
            ["subjectSummaryPath"] = GetOptionalString(matchedProof, "subjectSummaryPath"),
            ["evidenceAvailable"] = true,
        };
    }

    private static JsonObject BuildBenchmarkResult(JsonObject planBenchmark, JsonObject matchedBenchmark)
    {
        var payload = new JsonObject
        {
            ["id"] = GetRequiredString(planBenchmark, "id"),
            ["alias"] = GetOptionalString(planBenchmark, "alias"),
            ["canonicalCommand"] = GetOptionalString(planBenchmark, "canonicalCommand"),
            ["runId"] = GetOptionalString(matchedBenchmark, "runId"),
            ["status"] = GetOptionalString(matchedBenchmark, "status"),
            ["regressionStatus"] = GetOptionalString(matchedBenchmark, "regressionStatus"),
            ["summaryPath"] = GetOptionalString(matchedBenchmark, "summaryPath"),
            ["subjectSummaryPath"] = GetOptionalString(matchedBenchmark, "subjectSummaryPath"),
            ["perfSummaryPath"] = GetOptionalString(matchedBenchmark, "perfSummaryPath"),
            ["baselineComparePath"] = GetOptionalString(matchedBenchmark, "baselineComparePath"),
            ["samplesPath"] = GetOptionalString(matchedBenchmark, "samplesPath"),
            ["evidenceAvailable"] = true,
        };

        if (matchedBenchmark["metrics"] is JsonObject metrics)
        {
            payload["metrics"] = metrics.DeepClone();
        }

        return payload;
    }

    private static JsonObject BuildEngineeringValidationResult(JsonObject planGate, JsonObject matchedGate)
    {
        return new JsonObject
        {
            ["id"] = GetRequiredString(planGate, "id"),
            ["displayName"] = GetOptionalString(planGate, "displayName"),
            ["canonicalCommand"] = GetOptionalString(planGate, "canonicalCommand"),
            ["kind"] = GetOptionalString(matchedGate, "kind"),
            ["runId"] = GetOptionalString(matchedGate, "runId"),
            ["finalStatus"] = GetOptionalString(matchedGate, "finalStatus"),
            ["exitCode"] = GetOptionalInt32(matchedGate, "exitCode"),
            ["summaryPath"] = GetOptionalString(matchedGate, "summaryPath"),
            ["eventsPath"] = GetOptionalString(matchedGate, "eventsPath"),
            ["consolePath"] = GetOptionalString(matchedGate, "consolePath"),
            ["subjectSummaryPath"] = GetOptionalString(matchedGate, "subjectSummaryPath"),
            ["evidenceAvailable"] = true,
        };
    }

    private static JsonObject? FindEvidence(
        IReadOnlyDictionary<string, JsonObject> byId,
        IReadOnlyDictionary<string, JsonObject> byAlias,
        string id,
        string? alias)
    {
        if (byId.TryGetValue(id, out var matchedById))
        {
            return matchedById;
        }

        if (!string.IsNullOrWhiteSpace(alias) && byAlias.TryGetValue(alias, out var matchedByAlias))
        {
            return matchedByAlias;
        }

        return null;
    }

    private static JsonArray BuildUniquePathArray(JsonArray results, string fieldName)
    {
        var values = results
            .OfType<JsonObject>()
            .Select(item => GetOptionalString(item, fieldName))
            .Where(static value => !string.IsNullOrWhiteSpace(value))
            .Distinct(NameComparer)
            .OrderBy(static value => value, NameComparer)
            .Select(static value => (JsonNode?)JsonValue.Create(value))
            .ToArray();
        return new JsonArray(values);
    }

    private static IReadOnlyDictionary<string, JsonObject> IndexByString(IEnumerable<JsonObject> items, string fieldName)
    {
        var index = new Dictionary<string, JsonObject>(NameComparer);
        foreach (var item in items)
        {
            var key = GetOptionalString(item, fieldName);
            if (!string.IsNullOrWhiteSpace(key))
            {
                index[key] = item;
            }
        }

        return index;
    }

    private static void AddStringArray(ISet<string> destination, JsonNode? node)
    {
        foreach (var value in GetOptionalStringArray(node))
        {
            destination.Add(value);
        }
    }

    private static JsonObject LoadRequiredObject(string path)
    {
        if (!File.Exists(path))
        {
            throw new FileNotFoundException($"required json file missing: {path}", path);
        }

        var node = JsonNode.Parse(File.ReadAllText(path));
        return AsRequiredObject(node, path);
    }

    private static JsonObject GetRequiredObject(JsonObject parent, string propertyName)
    {
        if (parent[propertyName] is JsonObject child)
        {
            return child;
        }

        throw new InvalidOperationException($"json object property '{propertyName}' is required.");
    }

    private static JsonObject AsRequiredObject(JsonNode? node, string label)
    {
        if (node is JsonObject child)
        {
            return child;
        }

        throw new InvalidOperationException($"json object '{label}' is required.");
    }

    private static IReadOnlyList<JsonObject> GetRequiredObjectArray(JsonObject parent, string propertyName)
    {
        if (parent[propertyName] is not JsonArray array)
        {
            throw new InvalidOperationException($"json array property '{propertyName}' is required.");
        }

        return array
            .Select((node, index) => AsRequiredObject(node, $"{propertyName}[{index}]"))
            .ToArray();
    }

    private static string GetRequiredString(JsonObject parent, string propertyName)
    {
        var value = GetOptionalString(parent, propertyName);
        if (string.IsNullOrWhiteSpace(value))
        {
            throw new InvalidOperationException($"string property '{propertyName}' is required.");
        }

        return value;
    }

    private static IReadOnlyList<string> GetRequiredStringArray(JsonObject parent, string propertyName)
    {
        if (parent[propertyName] is not JsonArray array)
        {
            throw new InvalidOperationException($"json array property '{propertyName}' is required.");
        }

        return GetOptionalStringArray(array);
    }

    private static string? GetOptionalString(JsonObject parent, string propertyName)
    {
        return parent[propertyName]?.GetValue<string>();
    }

    private static int? GetOptionalInt32(JsonObject parent, string propertyName)
    {
        return parent[propertyName] is null ? null : parent[propertyName]!.GetValue<int>();
    }

    private static IReadOnlyList<string> GetOptionalStringArray(JsonNode? node)
    {
        if (node is not JsonArray array)
        {
            return [];
        }

        return array
            .Where(static value => value is not null)
            .Select(static value => value!.GetValue<string>())
            .Where(static value => !string.IsNullOrWhiteSpace(value))
            .Distinct(NameComparer)
            .OrderBy(static value => value, NameComparer)
            .ToArray();
    }

    private static string NormalizePath(string path)
    {
        return path.Replace('\\', '/');
    }
}
