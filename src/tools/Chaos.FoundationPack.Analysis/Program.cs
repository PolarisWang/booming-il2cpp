using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.PortableExecutable;
using System.Text.Json;

namespace Chaos.FoundationPack.Analysis;

internal static class Program
{
    private static readonly StringComparer NameComparer = StringComparer.Ordinal;
    private static readonly IReadOnlySet<string> SeedAssemblies =
        new HashSet<string>(new[] { "mscorlib", "netstandard", "System.Runtime", "System", "System.Core" }, NameComparer);

    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        WriteIndented = true,
    };

    public static int Main(string[] args)
    {
        if (args.Length > 0 && string.Equals(args[0], "--", StringComparison.Ordinal))
        {
            args = args[1..];
        }

        if (args.Length == 0)
        {
            Console.Error.WriteLine("usage: Chaos.FoundationPack.Analysis <phase1|phase2|phase3> [options]");
            return 1;
        }

        try
        {
            var command = args[0];
            var options = ParseOptions(args[1..]);
            return command switch
            {
                "phase1" => ExecutePhase1(options),
                "phase2" => ExecutePhase2(options),
                "phase3" => ExecutePhase3(options),
                _ => throw new ArgumentException($"unsupported command: {command}"),
            };
        }
        catch (Exception error)
        {
            Console.Error.WriteLine(error);
            return 1;
        }
    }

    private static Dictionary<string, string> ParseOptions(string[] args)
    {
        var options = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        for (var index = 0; index < args.Length; index++)
        {
            var current = args[index];
            if (!current.StartsWith("--", StringComparison.Ordinal))
            {
                throw new ArgumentException($"unexpected argument: {current}");
            }

            if (index + 1 >= args.Length)
            {
                throw new ArgumentException($"missing value for option: {current}");
            }

            options[current] = args[index + 1];
            index++;
        }

        return options;
    }

    private static int ExecutePhase1(IReadOnlyDictionary<string, string> options)
    {
        var catalogPath = GetRequiredOption(options, "--catalog");
        var outputDirectory = GetRequiredOption(options, "--output-dir");
        var taskId = GetRequiredOption(options, "--task-id");

        var outputRoot = Path.GetFullPath(outputDirectory);
        Directory.CreateDirectory(outputRoot);

        var totalStopwatch = Stopwatch.StartNew();
        var catalog = Phase0Catalog.Load(catalogPath);

        var classificationPayload = new Phase1ClassificationPayload
        {
            TaskId = taskId,
            SeedAssemblies = SeedAssemblies.OrderBy(static value => value, NameComparer).ToArray(),
        };
        var surfaceManifestPayload = new Phase1SurfaceManifestPayload { TaskId = taskId };
        var surfaceProofPayload = new Phase1SurfaceProofPayload { TaskId = taskId };
        var bindingProofPayload = new Phase1BindingProofPayload { TaskId = taskId };
        var benchmarkPayload = new Phase1BenchmarkPayload { TaskId = taskId };

        foreach (var source in catalog.Sources.OrderBy(static value => value.TargetFramework, NameComparer))
        {
            var analysis = AnalyzeTargetFramework(source);

            classificationPayload.TargetFrameworks[source.TargetFramework] = analysis.Classification;
            surfaceManifestPayload.TargetFrameworks[source.TargetFramework] = analysis.SurfaceManifest;
            surfaceProofPayload.TargetFrameworks[source.TargetFramework] = analysis.SurfaceProof;
            bindingProofPayload.TargetFrameworks[source.TargetFramework] = analysis.BindingProof;
            benchmarkPayload.TargetFrameworks[source.TargetFramework] = analysis.Benchmark;
        }

        benchmarkPayload.ElapsedMs = totalStopwatch.ElapsedMilliseconds;

        WriteJson(Path.Combine(outputRoot, "assembly-classification-v1-01.json"), classificationPayload);
        WriteJson(Path.Combine(outputRoot, "public-surface-manifest-v1-01.json"), surfaceManifestPayload);
        WriteJson(Path.Combine(outputRoot, "surface-proof-report-v1-01.json"), surfaceProofPayload);
        WriteJson(Path.Combine(outputRoot, "native-binding-proof-report-v1-01.json"), bindingProofPayload);
        WriteJson(Path.Combine(outputRoot, "manifest-benchmark-v1-01.json"), benchmarkPayload);
        return 0;
    }

    private static int ExecutePhase2(IReadOnlyDictionary<string, string> options)
    {
        var catalogPath = GetRequiredOption(options, "--catalog");
        var phase1Directory = GetRequiredOption(options, "--phase1-dir");
        var outputDirectory = GetRequiredOption(options, "--output-dir");
        var taskId = GetRequiredOption(options, "--task-id");

        var outputRoot = Path.GetFullPath(outputDirectory);
        Directory.CreateDirectory(outputRoot);

        var totalStopwatch = Stopwatch.StartNew();
        var catalog = Phase0Catalog.Load(catalogPath);
        var phase1Index = Phase1ContractLaneIndex.Load(phase1Directory);

        var semanticPayload = new Phase2SemanticIrPayload { TaskId = taskId };
        var helperPayload = new Phase2HelperContractsPayload { TaskId = taskId };
        var surfaceProofPayload = new Phase2SurfaceProofPayload { TaskId = taskId };
        var semanticProofPayload = new Phase2SemanticProofPayload { TaskId = taskId };
        var benchmarkPayload = new Phase2BenchmarkPayload { TaskId = taskId };

        foreach (var source in catalog.Sources.OrderBy(static value => value.TargetFramework, NameComparer))
        {
            var phase1CandidateAssemblyCount = phase1Index.GetCandidateAssemblyCount(source.TargetFramework);
            var analysis = AnalyzeCoreLibTargetFramework(source, phase1CandidateAssemblyCount);

            semanticPayload.TargetFrameworks[source.TargetFramework] = analysis.SemanticIr;
            helperPayload.TargetFrameworks[source.TargetFramework] = analysis.HelperContracts;
            surfaceProofPayload.TargetFrameworks[source.TargetFramework] = analysis.SurfaceProof;
            semanticProofPayload.TargetFrameworks[source.TargetFramework] = analysis.SemanticProof;
            benchmarkPayload.TargetFrameworks[source.TargetFramework] = analysis.Benchmark;
        }

        benchmarkPayload.ElapsedMs = totalStopwatch.ElapsedMilliseconds;

        WriteJson(Path.Combine(outputRoot, "assembly-semantic-ir-system-private-corelib-v1-01.json"), semanticPayload);
        WriteJson(Path.Combine(outputRoot, "runtime-helper-contracts-v1-01.json"), helperPayload);
        WriteJson(Path.Combine(outputRoot, "corelib-surface-proof-report-v1-01.json"), surfaceProofPayload);
        WriteJson(Path.Combine(outputRoot, "corelib-semantic-proof-report-v1-01.json"), semanticProofPayload);
        WriteJson(Path.Combine(outputRoot, "corelib-substrate-benchmark-v1-01.json"), benchmarkPayload);
        return 0;
    }

    private static int ExecutePhase3(IReadOnlyDictionary<string, string> options)
    {
        var phase2Directory = GetRequiredOption(options, "--phase2-dir");
        var outputDirectory = GetRequiredOption(options, "--output-dir");
        var taskId = GetRequiredOption(options, "--task-id");
        var registrySnapshotPath = options.TryGetValue("--registry-path", out var configuredRegistrySnapshotPath) &&
                                   !string.IsNullOrWhiteSpace(configuredRegistrySnapshotPath)
            ? configuredRegistrySnapshotPath
            : Path.Combine(Environment.CurrentDirectory, "artifacts", "tests", "registry", "current", "index.json");

        var outputRoot = Path.GetFullPath(outputDirectory);
        Directory.CreateDirectory(outputRoot);

        var totalStopwatch = Stopwatch.StartNew();
        var phase2Index = Phase2SubstrateIndex.Load(phase2Directory);
        var familyPlanPayload = new Phase3FamilyPlanPayload { TaskId = taskId };
        var proofMatrixPayload = new Phase3ProofMatrixPayload { TaskId = taskId };
        var benchmarkPayload = new Phase3BenchmarkPayload { TaskId = taskId };
        var executionEntryPayload = SemanticFamilyExecutionEntryBuilder.Build(
            phase2Index,
            taskId,
            registrySnapshotPath);

        foreach (var targetFramework in phase2Index.TargetFrameworks.Keys.OrderBy(static value => value, NameComparer))
        {
            var framework = phase2Index.TargetFrameworks[targetFramework];
            var stopwatch = Stopwatch.StartNew();
            var families = framework.MemberIdsByClassification
                .Where(static entry => entry.Value.Count > 0)
                .OrderBy(static entry => GetPhase3FamilyPriority(entry.Key))
                .ThenBy(static entry => entry.Key, NameComparer)
                .ToArray();

            var familyPlanFamilies = new SortedDictionary<string, Phase3FamilyPayload>(NameComparer);
            var proofMatrixFamilies = new SortedDictionary<string, Phase3ProofMatrixFamilyPayload>(NameComparer);
            var issues = new List<string>();

            foreach (var family in families)
            {
                var priority = GetPhase3FamilyPriority(family.Key);
                var orderedMembers = family.Value.OrderBy(static value => value, NameComparer).ToArray();
                familyPlanFamilies[family.Key] = new Phase3FamilyPayload
                {
                    Priority = priority,
                    MemberCount = orderedMembers.Length,
                    SampleMembers = orderedMembers.Take(20).ToArray(),
                };
                proofMatrixFamilies[family.Key] = new Phase3ProofMatrixFamilyPayload
                {
                    Priority = priority,
                    MemberCount = orderedMembers.Length,
                    RequiredProofs = GetRequiredProofsForFamily(family.Key),
                };
            }

            if (familyPlanFamilies.Count == 0)
            {
                issues.Add($"{targetFramework}:no-families-derived");
            }

            stopwatch.Stop();

            familyPlanPayload.TargetFrameworks[targetFramework] = new Phase3FamilyPlanFrameworkPayload
            {
                FamilyCount = familyPlanFamilies.Count,
                TotalPublicMemberCount = framework.TotalPublicMemberCount,
                Families = familyPlanFamilies,
            };
            proofMatrixPayload.TargetFrameworks[targetFramework] = new Phase3ProofMatrixFrameworkPayload
            {
                FamilyCount = proofMatrixFamilies.Count,
                Families = proofMatrixFamilies,
                IssueCount = issues.Count,
                Issues = issues,
            };
            benchmarkPayload.TargetFrameworks[targetFramework] = new Phase3BenchmarkFrameworkPayload
            {
                ElapsedMs = stopwatch.ElapsedMilliseconds,
                FamilyCount = familyPlanFamilies.Count,
                TotalPublicMemberCount = framework.TotalPublicMemberCount,
            };
        }

        benchmarkPayload.ElapsedMs = totalStopwatch.ElapsedMilliseconds;

        WriteJson(Path.Combine(outputRoot, "semantic-family-plan-v1-01.json"), familyPlanPayload);
        WriteJson(Path.Combine(outputRoot, "family-proof-matrix-v1-01.json"), proofMatrixPayload);
        WriteJson(Path.Combine(outputRoot, "family-priority-benchmark-v1-01.json"), benchmarkPayload);
        WriteJson(Path.Combine(outputRoot, "semantic-family-execution-entry-v1-01.json"), executionEntryPayload);
        return 0;
    }

    private static string GetRequiredOption(IReadOnlyDictionary<string, string> options, string optionName)
    {
        if (!options.TryGetValue(optionName, out var value) || string.IsNullOrWhiteSpace(value))
        {
            throw new ArgumentException($"missing required option: {optionName}");
        }

        return value;
    }

    private static void WriteJson<T>(string path, T payload)
    {
        File.WriteAllText(path, JsonSerializer.Serialize(payload, JsonOptions), new System.Text.UTF8Encoding(false));
    }

    private static Phase2TargetFrameworkAnalysisResult AnalyzeCoreLibTargetFramework(
        FrameworkCatalogSource source,
        int phase1CandidateAssemblyCount)
    {
        var stopwatch = Stopwatch.StartNew();
        var runtimePath = GetCoreLibRuntimePath(source.RuntimeRootPath);
        var refPath = GetOptionalCoreLibPath(source.RefRootPath);

        using var stream = File.OpenRead(runtimePath);
        using var peReader = new PEReader(stream);
        if (!peReader.HasMetadata)
        {
            throw new InvalidOperationException($"corelib runtime assembly missing metadata: {runtimePath}");
        }

        var metadataReader = peReader.GetMetadataReader();
        var publicTypes = new SortedDictionary<string, CorelibSemanticTypePayload>(NameComparer);
        var helperKindCounts = CreatePhase2ClassificationCounts();
        var membersByHelperKind = new SortedDictionary<string, List<string>>(NameComparer);
        var surfaceIssues = new List<string>();
        var semanticIssues = new List<string>();

        var publicTypeCount = 0;
        var publicMethodCount = 0;
        var publicPropertyCount = 0;
        var publicFieldCount = 0;
        var publicEventCount = 0;
        var publicMemberCount = 0;
        var classifiedPublicMemberCount = 0;

        foreach (var typeDefinitionHandle in metadataReader.TypeDefinitions)
        {
            var typeDefinition = metadataReader.GetTypeDefinition(typeDefinitionHandle);
            if (IsModuleType(metadataReader, typeDefinition) || !IsPublicType(typeDefinition.Attributes))
            {
                continue;
            }

            publicTypeCount++;
            var typeName = FullTypeName(metadataReader, typeDefinitionHandle);
            var methodPayloads = new List<SemanticMemberPayload>();
            var propertyPayloads = new List<SemanticMemberPayload>();
            var fieldPayloads = new List<SemanticMemberPayload>();
            var eventPayloads = new List<SemanticMemberPayload>();

            foreach (var methodHandle in typeDefinition.GetMethods())
            {
                var method = metadataReader.GetMethodDefinition(methodHandle);
                if (!IsPubliclyVisible(method.Attributes))
                {
                    continue;
                }

                var memberId = BuildMethodSignature(metadataReader, methodHandle);
                var classification = ClassifyMethod(metadataReader, methodHandle);
                methodPayloads.Add(new SemanticMemberPayload
                {
                    MemberId = memberId,
                    Kind = "method",
                    Classification = classification.Classification,
                    HasBody = classification.HasBody,
                });

                publicMethodCount++;
                publicMemberCount++;
                if (TrackPhase2Classification($"{typeName}::{memberId}", classification.Classification, helperKindCounts, membersByHelperKind))
                {
                    classifiedPublicMemberCount++;
                }
                else
                {
                    semanticIssues.Add($"{source.TargetFramework}:{typeName}::{memberId}:missing-classification");
                }
            }

            foreach (var propertyHandle in typeDefinition.GetProperties())
            {
                var property = metadataReader.GetPropertyDefinition(propertyHandle);
                var accessors = property.GetAccessors();
                if (!IsPublicProperty(metadataReader, accessors))
                {
                    continue;
                }

                var memberId = metadataReader.GetString(property.Name);
                var classification = ClassifyProperty(metadataReader, propertyHandle);
                propertyPayloads.Add(new SemanticMemberPayload
                {
                    MemberId = memberId,
                    Kind = "property",
                    Classification = classification.Classification,
                    HasBody = classification.HasBody,
                });

                publicPropertyCount++;
                publicMemberCount++;
                if (TrackPhase2Classification($"{typeName}::{memberId}", classification.Classification, helperKindCounts, membersByHelperKind))
                {
                    classifiedPublicMemberCount++;
                }
                else
                {
                    semanticIssues.Add($"{source.TargetFramework}:{typeName}::{memberId}:missing-classification");
                }
            }

            foreach (var fieldHandle in typeDefinition.GetFields())
            {
                var field = metadataReader.GetFieldDefinition(fieldHandle);
                if (!IsPubliclyVisible(field.Attributes))
                {
                    continue;
                }

                var memberId = metadataReader.GetString(field.Name);
                const string classification = "field-metadata";
                fieldPayloads.Add(new SemanticMemberPayload
                {
                    MemberId = memberId,
                    Kind = "field",
                    Classification = classification,
                    HasBody = false,
                });

                publicFieldCount++;
                publicMemberCount++;
                if (TrackPhase2Classification($"{typeName}::{memberId}", classification, helperKindCounts, membersByHelperKind))
                {
                    classifiedPublicMemberCount++;
                }
                else
                {
                    semanticIssues.Add($"{source.TargetFramework}:{typeName}::{memberId}:missing-classification");
                }
            }

            foreach (var eventHandle in typeDefinition.GetEvents())
            {
                var eventDefinition = metadataReader.GetEventDefinition(eventHandle);
                var accessors = eventDefinition.GetAccessors();
                if (!IsPublicEvent(metadataReader, accessors))
                {
                    continue;
                }

                var memberId = metadataReader.GetString(eventDefinition.Name);
                var classification = ClassifyEvent(metadataReader, eventHandle);
                eventPayloads.Add(new SemanticMemberPayload
                {
                    MemberId = memberId,
                    Kind = "event",
                    Classification = classification.Classification,
                    HasBody = classification.HasBody,
                });

                publicEventCount++;
                publicMemberCount++;
                if (TrackPhase2Classification($"{typeName}::{memberId}", classification.Classification, helperKindCounts, membersByHelperKind))
                {
                    classifiedPublicMemberCount++;
                }
                else
                {
                    semanticIssues.Add($"{source.TargetFramework}:{typeName}::{memberId}:missing-classification");
                }
            }

            publicTypes[typeName] = new CorelibSemanticTypePayload
            {
                PublicMethodCount = methodPayloads.Count,
                PublicPropertyCount = propertyPayloads.Count,
                PublicFieldCount = fieldPayloads.Count,
                PublicEventCount = eventPayloads.Count,
                PublicMethods = methodPayloads.OrderBy(static value => value.MemberId, NameComparer).ToArray(),
                PublicProperties = propertyPayloads.OrderBy(static value => value.MemberId, NameComparer).ToArray(),
                PublicFields = fieldPayloads.OrderBy(static value => value.MemberId, NameComparer).ToArray(),
                PublicEvents = eventPayloads.OrderBy(static value => value.MemberId, NameComparer).ToArray(),
            };
        }

        if (phase1CandidateAssemblyCount == 0)
        {
            semanticIssues.Add($"{source.TargetFramework}:phase1-candidate-assembly-count:0");
        }

        var helperMembersPayload = new SortedDictionary<string, string[]>(NameComparer);
        foreach (var entry in membersByHelperKind)
        {
            helperMembersPayload[entry.Key] = entry.Value.OrderBy(static value => value, NameComparer).ToArray();
        }

        var surfaceIssueCount = surfaceIssues.Count;
        var semanticIssueCount = semanticIssues.Count;
        var unclassifiedPublicMemberCount = publicMemberCount - classifiedPublicMemberCount;

        stopwatch.Stop();

        return new Phase2TargetFrameworkAnalysisResult(
            new Phase2SemanticFrameworkPayload
            {
                SelectedRuntimeVersion = source.RuntimeVersion,
                SourceKind = source.RuntimeSourceKind,
                AssemblyName = "System.Private.CoreLib",
                RefPath = refPath,
                RuntimePath = runtimePath,
                Phase1CandidateAssemblyCount = phase1CandidateAssemblyCount,
                PublicTypeCount = publicTypeCount,
                PublicMemberCount = publicMemberCount,
                PublicTypes = publicTypes,
            },
            new Phase2HelperContractsFrameworkPayload
            {
                AssemblyName = "System.Private.CoreLib",
                ClassifiedPublicMemberCount = classifiedPublicMemberCount,
                HelperKindCounts = helperKindCounts,
                MembersByHelperKind = helperMembersPayload,
            },
            new Phase2SurfaceProofFrameworkPayload
            {
                PublicTypeCount = publicTypeCount,
                PublicMethodCount = publicMethodCount,
                PublicPropertyCount = publicPropertyCount,
                PublicFieldCount = publicFieldCount,
                PublicEventCount = publicEventCount,
                UnresolvedPublicTypeCount = 0,
                IssueCount = surfaceIssueCount,
                Issues = surfaceIssues,
            },
            new Phase2SemanticProofFrameworkPayload
            {
                PublicMemberCount = publicMemberCount,
                ClassifiedPublicMemberCount = classifiedPublicMemberCount,
                UnclassifiedPublicMemberCount = unclassifiedPublicMemberCount,
                IssueCount = semanticIssueCount,
                Issues = semanticIssues,
            },
            new Phase2BenchmarkFrameworkPayload
            {
                ElapsedMs = stopwatch.ElapsedMilliseconds,
                PublicTypeCount = publicTypeCount,
                PublicMemberCount = publicMemberCount,
                Phase1CandidateAssemblyCount = phase1CandidateAssemblyCount,
            });
    }

    private static string GetCoreLibRuntimePath(string runtimeRootPath)
    {
        var runtimePath = Path.Combine(Path.GetFullPath(runtimeRootPath), "System.Private.CoreLib.dll");
        if (!File.Exists(runtimePath))
        {
            throw new FileNotFoundException("System.Private.CoreLib runtime assembly missing", runtimePath);
        }

        return runtimePath;
    }

    private static string? GetOptionalCoreLibPath(string rootPath)
    {
        var path = Path.Combine(Path.GetFullPath(rootPath), "System.Private.CoreLib.dll");
        return File.Exists(path) ? path : null;
    }

    private static SortedDictionary<string, int> CreatePhase2ClassificationCounts()
    {
        return new SortedDictionary<string, int>(NameComparer)
        {
            ["abstract"] = 0,
            ["composite"] = 0,
            ["field-metadata"] = 0,
            ["internal-call"] = 0,
            ["managed-cil"] = 0,
            ["metadata-only"] = 0,
            ["pinvoke"] = 0,
            ["runtime-special"] = 0,
        };
    }

    private static bool TrackPhase2Classification(
        string memberIdentity,
        string classification,
        SortedDictionary<string, int> helperKindCounts,
        SortedDictionary<string, List<string>> membersByHelperKind)
    {
        if (string.IsNullOrWhiteSpace(classification))
        {
            return false;
        }

        if (!helperKindCounts.ContainsKey(classification))
        {
            helperKindCounts[classification] = 0;
        }

        helperKindCounts[classification]++;
        if (!membersByHelperKind.TryGetValue(classification, out var members))
        {
            members = [];
            membersByHelperKind[classification] = members;
        }

        members.Add(memberIdentity);
        return true;
    }

    private static MemberClassificationResult ClassifyMethod(MetadataReader metadataReader, MethodDefinitionHandle methodHandle)
    {
        var method = metadataReader.GetMethodDefinition(methodHandle);
        var hasBody = method.RelativeVirtualAddress != 0;
        if ((method.Attributes & MethodAttributes.Abstract) != 0)
        {
            return new MemberClassificationResult("abstract", hasBody);
        }

        if ((method.Attributes & MethodAttributes.PinvokeImpl) != 0)
        {
            return new MemberClassificationResult("pinvoke", hasBody);
        }

        if ((method.ImplAttributes & MethodImplAttributes.InternalCall) != 0)
        {
            return new MemberClassificationResult("internal-call", hasBody);
        }

        if ((method.ImplAttributes & MethodImplAttributes.ForwardRef) != 0
            || (method.ImplAttributes & MethodImplAttributes.CodeTypeMask) == MethodImplAttributes.Native
            || (method.ImplAttributes & MethodImplAttributes.CodeTypeMask) == MethodImplAttributes.Runtime)
        {
            return new MemberClassificationResult("runtime-special", hasBody);
        }

        if (hasBody)
        {
            return new MemberClassificationResult("managed-cil", true);
        }

        return new MemberClassificationResult("metadata-only", false);
    }

    private static MemberClassificationResult ClassifyProperty(MetadataReader metadataReader, PropertyDefinitionHandle propertyHandle)
    {
        var property = metadataReader.GetPropertyDefinition(propertyHandle);
        var accessors = property.GetAccessors();
        var classifications = new HashSet<string>(NameComparer);
        var hasBody = false;

        if (IsVisibleAccessor(metadataReader, accessors.Getter))
        {
            var getterClassification = ClassifyMethod(metadataReader, accessors.Getter);
            classifications.Add(getterClassification.Classification);
            hasBody |= getterClassification.HasBody;
        }

        if (IsVisibleAccessor(metadataReader, accessors.Setter))
        {
            var setterClassification = ClassifyMethod(metadataReader, accessors.Setter);
            classifications.Add(setterClassification.Classification);
            hasBody |= setterClassification.HasBody;
        }

        return new MemberClassificationResult(CollapsePhase2Classification(classifications), hasBody);
    }

    private static MemberClassificationResult ClassifyEvent(MetadataReader metadataReader, EventDefinitionHandle eventHandle)
    {
        var eventDefinition = metadataReader.GetEventDefinition(eventHandle);
        var accessors = eventDefinition.GetAccessors();
        var classifications = new HashSet<string>(NameComparer);
        var hasBody = false;

        if (IsVisibleAccessor(metadataReader, accessors.Adder))
        {
            var adderClassification = ClassifyMethod(metadataReader, accessors.Adder);
            classifications.Add(adderClassification.Classification);
            hasBody |= adderClassification.HasBody;
        }

        if (IsVisibleAccessor(metadataReader, accessors.Remover))
        {
            var removerClassification = ClassifyMethod(metadataReader, accessors.Remover);
            classifications.Add(removerClassification.Classification);
            hasBody |= removerClassification.HasBody;
        }

        if (IsVisibleAccessor(metadataReader, accessors.Raiser))
        {
            var raiserClassification = ClassifyMethod(metadataReader, accessors.Raiser);
            classifications.Add(raiserClassification.Classification);
            hasBody |= raiserClassification.HasBody;
        }

        return new MemberClassificationResult(CollapsePhase2Classification(classifications), hasBody);
    }

    private static string CollapsePhase2Classification(IReadOnlyCollection<string> classifications)
    {
        if (classifications.Count == 0)
        {
            return "metadata-only";
        }

        return classifications.Count == 1
            ? classifications.Single()
            : "composite";
    }

    private static int GetPhase3FamilyPriority(string familyName)
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
            "field-metadata" => ["layout-proof", "conformance"],
            "abstract" => ["contract-proof"],
            "composite" => ["accessor-review", "native-proof", "conformance"],
            _ => ["native-proof", "conformance"],
        };
    }

    private static TargetFrameworkAnalysisResult AnalyzeTargetFramework(FrameworkCatalogSource source)
    {
        var stopwatch = Stopwatch.StartNew();
        var refAssemblies = LoadAssemblies(source.RefRootPath, MetadataPreference.Ref);
        var runtimeAssemblies = LoadAssemblies(source.RuntimeRootPath, MetadataPreference.Runtime);
        var analyses = MergeAssemblies(refAssemblies, runtimeAssemblies);
        ClassifyAssemblies(analyses);

        var candidateReasons = BuildCandidateReasons(analyses);
        var candidateNames = candidateReasons.Keys.OrderBy(static value => value, NameComparer).ToArray();
        var manifestAssemblies = new SortedDictionary<string, SurfaceManifestAssemblyPayload>(NameComparer);
        var classificationAssemblies = new SortedDictionary<string, CandidateAssemblyPayload>(NameComparer);

        var unresolvedForwardedTypeCount = 0;
        var publicTypeDefinitionCount = 0;
        var forwardedTypeCount = 0;
        var publicMemberCount = 0;
        var issueCount = 0;
        var issues = new List<string>();
        var bindingsCheckedCount = 0;
        var unresolvedBindingTargetCount = 0;
        var resolvedBindingTargetCount = 0;
        var missingRuntimeAssemblyCount = 0;
        var contractCompleteEligibleAssemblyCount = 0;

        foreach (var candidateName in candidateNames)
        {
            var analysis = analyses[candidateName];
            var metadata = analysis.PreferredMetadata;
            if (metadata is null)
            {
                issueCount++;
                issues.Add($"candidate assembly missing managed metadata: {candidateName}");
                continue;
            }

            var publicTypes = new SortedDictionary<string, PublicTypePayload>(NameComparer);
            foreach (var publicType in metadata.PublicTypes.Values.OrderBy(static value => value.TypeName, NameComparer))
            {
                publicTypes[publicType.TypeName] = new PublicTypePayload
                {
                    CanonicalOwnerAssembly = candidateName,
                    ResolutionStatus = "defined-in-candidate",
                    PublicMethodCount = publicType.PublicMethods.Count,
                    PublicPropertyCount = publicType.PublicProperties.Count,
                    PublicFieldCount = publicType.PublicFields.Count,
                    PublicEventCount = publicType.PublicEvents.Count,
                    PublicMethods = publicType.PublicMethods.ToArray(),
                    PublicProperties = publicType.PublicProperties.ToArray(),
                    PublicFields = publicType.PublicFields.ToArray(),
                    PublicEvents = publicType.PublicEvents.ToArray(),
                };
            }

            var forwardedTypes = new SortedDictionary<string, ForwardedTypePayload>(NameComparer);
            var bindingTargets = new HashSet<string>(NameComparer);
            var externalBindingTargets = new HashSet<string>(NameComparer);
            var assemblyUnresolvedForwarded = 0;
            foreach (var forwardedType in metadata.ForwardedTypes.Values.OrderBy(static value => value.TypeName, NameComparer))
            {
                var resolution = ResolveTypeOwner(candidateName, forwardedType.TypeName, analyses);
                if (!resolution.IsResolved)
                {
                    assemblyUnresolvedForwarded++;
                    unresolvedForwardedTypeCount++;
                    issueCount++;
                    issues.Add($"{source.TargetFramework}:{candidateName}:{forwardedType.TypeName}:{resolution.ResolutionStatus}");
                }

                if (!string.IsNullOrWhiteSpace(resolution.OwnerAssembly))
                {
                    if (resolution.RequiresRuntimeBindingValidation)
                    {
                        externalBindingTargets.Remove(resolution.OwnerAssembly);
                        bindingTargets.Add(resolution.OwnerAssembly);
                    }
                    else if (!bindingTargets.Contains(resolution.OwnerAssembly))
                    {
                        externalBindingTargets.Add(resolution.OwnerAssembly);
                    }
                }

                forwardedTypes[forwardedType.TypeName] = new ForwardedTypePayload
                {
                    ImmediateTargetAssembly = forwardedType.ImmediateTargetAssembly,
                    CanonicalOwnerAssembly = resolution.OwnerAssembly,
                    ResolutionStatus = resolution.ResolutionStatus,
                    ResolutionPath = resolution.ResolutionPath.ToArray(),
                };
            }

            if (publicTypes.Count > 0)
            {
                externalBindingTargets.Remove(candidateName);
                bindingTargets.Add(candidateName);
            }

            var assemblyUnresolvedBindings = 0;
            foreach (var bindingTarget in bindingTargets.OrderBy(static value => value, NameComparer))
            {
                bindingsCheckedCount++;
                if (analyses.TryGetValue(bindingTarget, out var bindingAnalysis) && bindingAnalysis.RuntimePath is not null)
                {
                    resolvedBindingTargetCount++;
                }
                else
                {
                    assemblyUnresolvedBindings++;
                    unresolvedBindingTargetCount++;
                    missingRuntimeAssemblyCount++;
                    issueCount++;
                    issues.Add($"{source.TargetFramework}:{candidateName}:missing-runtime-binding:{bindingTarget}");
                }
            }

            foreach (var bindingTarget in externalBindingTargets.OrderBy(static value => value, NameComparer))
            {
                bindingsCheckedCount++;
                resolvedBindingTargetCount++;
            }

            var contractCompleteEligible =
                assemblyUnresolvedForwarded == 0
                && assemblyUnresolvedBindings == 0
                && (string.Equals(analysis.Classification, "contract-facade", StringComparison.Ordinal)
                    || string.Equals(analysis.Classification, "compat-shim", StringComparison.Ordinal));
            if (contractCompleteEligible)
            {
                contractCompleteEligibleAssemblyCount++;
            }

            var currentPublicMemberCount = publicTypes.Values.Sum(static value =>
                value.PublicMethodCount + value.PublicPropertyCount + value.PublicFieldCount + value.PublicEventCount);
            publicTypeDefinitionCount += publicTypes.Count;
            forwardedTypeCount += forwardedTypes.Count;
            publicMemberCount += currentPublicMemberCount;

            classificationAssemblies[candidateName] = new CandidateAssemblyPayload
            {
                FileName = analysis.FileName,
                Classification = analysis.Classification,
                SourceKind = source.RefSourceKind,
                RefPath = analysis.RefPath,
                RuntimePath = analysis.RuntimePath,
                SeedReasons = candidateReasons[candidateName].OrderBy(static value => value, NameComparer).ToArray(),
                PublicTypeDefinitionCount = publicTypes.Count,
                ForwardedTypeCount = forwardedTypes.Count,
                PublicMemberCount = currentPublicMemberCount,
                BindingTargetAssemblyCount = bindingTargets.Count,
                ContractCompleteEligible = contractCompleteEligible,
            };

            manifestAssemblies[candidateName] = new SurfaceManifestAssemblyPayload
            {
                Classification = analysis.Classification,
                ContractCompleteEligible = contractCompleteEligible,
                PublicTypeDefinitionCount = publicTypes.Count,
                ForwardedTypeCount = forwardedTypes.Count,
                PublicMemberCount = currentPublicMemberCount,
                PublicTypes = publicTypes,
                ForwardedTypes = forwardedTypes,
            };
        }

        stopwatch.Stop();

        return new TargetFrameworkAnalysisResult(
            new ClassificationFrameworkPayload
            {
                SelectedVersion = source.RefVersion,
                SourceKind = source.RefSourceKind,
                CandidateAssemblyCount = classificationAssemblies.Count,
                CandidateAssemblies = classificationAssemblies,
            },
            new SurfaceManifestFrameworkPayload
            {
                SelectedVersion = source.RefVersion,
                Assemblies = manifestAssemblies,
            },
            new SurfaceProofFrameworkPayload
            {
                CandidateAssemblyCount = classificationAssemblies.Count,
                PublicTypeDefinitionCount = publicTypeDefinitionCount,
                ForwardedTypeCount = forwardedTypeCount,
                PublicMemberCount = publicMemberCount,
                UnresolvedForwardedTypeCount = unresolvedForwardedTypeCount,
                UnresolvedPublicTypeCount = 0,
                ContractCompleteEligibleAssemblyCount = contractCompleteEligibleAssemblyCount,
                IssueCount = issueCount,
                Issues = issues,
            },
            new BindingProofFrameworkPayload
            {
                BindingsCheckedCount = bindingsCheckedCount,
                ResolvedBindingTargetCount = resolvedBindingTargetCount,
                UnresolvedBindingTargetCount = unresolvedBindingTargetCount,
                MissingRuntimeAssemblyCount = missingRuntimeAssemblyCount,
                Issues = issues,
            },
            new BenchmarkFrameworkPayload
            {
                ElapsedMs = stopwatch.ElapsedMilliseconds,
                CandidateAssemblyCount = classificationAssemblies.Count,
                PublicTypeDefinitionCount = publicTypeDefinitionCount,
                ForwardedTypeCount = forwardedTypeCount,
                PublicMemberCount = publicMemberCount,
            });
    }

    private static Dictionary<string, AssemblyAnalysis> MergeAssemblies(
        IReadOnlyDictionary<string, LoadedAssembly> refAssemblies,
        IReadOnlyDictionary<string, LoadedAssembly> runtimeAssemblies)
    {
        var analyses = new Dictionary<string, AssemblyAnalysis>(NameComparer);
        foreach (var entry in refAssemblies)
        {
            analyses[entry.Key] = new AssemblyAnalysis(entry.Key, entry.Value.FileName)
            {
                RefPath = entry.Value.Path,
                RefMetadata = entry.Value.Metadata,
            };
        }

        foreach (var entry in runtimeAssemblies)
        {
            if (!analyses.TryGetValue(entry.Key, out var analysis))
            {
                analysis = new AssemblyAnalysis(entry.Key, entry.Value.FileName);
                analyses[entry.Key] = analysis;
            }

            analysis.RuntimePath = entry.Value.Path;
            analysis.RuntimeMetadata = entry.Value.Metadata;
        }

        return analyses;
    }

    private static IReadOnlyDictionary<string, LoadedAssembly> LoadAssemblies(string rootPath, MetadataPreference metadataPreference)
    {
        var root = Path.GetFullPath(rootPath);
        if (!Directory.Exists(root))
        {
            throw new DirectoryNotFoundException($"framework pack root missing: {root}");
        }

        var assemblies = new Dictionary<string, LoadedAssembly>(NameComparer);
        foreach (var filePath in Directory.EnumerateFiles(root, "*.dll", SearchOption.TopDirectoryOnly))
        {
            var loadedAssembly = TryLoadAssembly(filePath, metadataPreference);
            assemblies[loadedAssembly.AssemblyName] = loadedAssembly;
        }

        return new ReadOnlyDictionary<string, LoadedAssembly>(assemblies);
    }

    private static void ClassifyAssemblies(IReadOnlyDictionary<string, AssemblyAnalysis> analyses)
    {
        foreach (var analysis in analyses.Values)
        {
            var metadata = analysis.PreferredMetadata;
            if (metadata is null)
            {
                analysis.Classification = analysis.RuntimePath is not null ? "runtime-backed" : "unknown";
                continue;
            }

            if (SeedAssemblies.Contains(analysis.AssemblyName))
            {
                analysis.Classification = metadata.ForwardedTypes.Count > 0 ? "compat-shim" : "contract-facade";
                continue;
            }

            if (analysis.RefPath is null && analysis.RuntimePath is not null)
            {
                analysis.Classification = "runtime-backed";
                continue;
            }

            if (metadata.ForwardedTypes.Count > 0 && metadata.PublicTypes.Count == 0)
            {
                analysis.Classification = "contract-facade";
                continue;
            }

            if (metadata.ForwardedTypes.Count > 0)
            {
                analysis.Classification = "compat-shim";
                continue;
            }

            analysis.Classification = "implementation";
        }
    }

    private static Dictionary<string, HashSet<string>> BuildCandidateReasons(IReadOnlyDictionary<string, AssemblyAnalysis> analyses)
    {
        var reasons = new Dictionary<string, HashSet<string>>(NameComparer);
        var queue = new Queue<string>();

        foreach (var seedAssembly in SeedAssemblies)
        {
            if (!analyses.ContainsKey(seedAssembly))
            {
                continue;
            }

            queue.Enqueue(seedAssembly);
            reasons[seedAssembly] = new HashSet<string>(new[] { "seed" }, NameComparer);
        }

        while (queue.Count > 0)
        {
            var current = queue.Dequeue();
            var analysis = analyses[current];
            foreach (var closureTarget in GetClosureTargets(analysis))
            {
                if (!analyses.TryGetValue(closureTarget.AssemblyName, out var targetAnalysis))
                {
                    continue;
                }

                if (!ParticipatesInPhase1(targetAnalysis))
                {
                    continue;
                }

                if (!reasons.TryGetValue(closureTarget.AssemblyName, out var reasonSet))
                {
                    reasonSet = new HashSet<string>(NameComparer);
                    reasons[closureTarget.AssemblyName] = reasonSet;
                }

                var added = reasonSet.Add(closureTarget.Reason);
                if (added)
                {
                    queue.Enqueue(closureTarget.AssemblyName);
                }
            }
        }

        return reasons;
    }

    private static TypeOwnerResolution ResolveTypeOwner(
        string assemblyName,
        string typeName,
        IReadOnlyDictionary<string, AssemblyAnalysis> analyses)
    {
        var visited = new HashSet<string>(NameComparer);
        var path = new List<string>();
        return ResolveTypeOwnerCore(assemblyName, typeName, analyses, visited, path);
    }

    private static IEnumerable<ClosureTarget> GetClosureTargets(AssemblyAnalysis analysis)
    {
        var metadata = analysis.PreferredMetadata;
        if (metadata is null)
        {
            yield break;
        }

        foreach (var reference in metadata.AssemblyReferences.OrderBy(static value => value, NameComparer))
        {
            yield return new ClosureTarget(reference, "reference-closure");
        }

        foreach (var targetAssembly in metadata.ForwardedTypes.Values
                     .Select(static value => value.ImmediateTargetAssembly)
                     .Where(static value => !string.IsNullOrWhiteSpace(value))
                     .Distinct(NameComparer)
                     .OrderBy(static value => value, NameComparer))
        {
            yield return new ClosureTarget(targetAssembly, "forwarded-target-closure");
        }
    }

    private static bool ParticipatesInPhase1(AssemblyAnalysis analysis)
    {
        return string.Equals(analysis.Classification, "contract-facade", StringComparison.Ordinal)
               || string.Equals(analysis.Classification, "compat-shim", StringComparison.Ordinal);
    }

    private static TypeOwnerResolution ResolveTypeOwnerCore(
        string assemblyName,
        string typeName,
        IReadOnlyDictionary<string, AssemblyAnalysis> analyses,
        ISet<string> visited,
        List<string> path)
    {
        if (!visited.Add(assemblyName))
        {
            return new TypeOwnerResolution(false, null, "cycle-detected", path.ToArray(), true);
        }

        path.Add(assemblyName);
        if (!analyses.TryGetValue(assemblyName, out var analysis))
        {
            return path.Count > 1
                ? new TypeOwnerResolution(true, assemblyName, "resolved-external-owner-assembly", path.ToArray(), false)
                : new TypeOwnerResolution(false, null, "missing-assembly", path.ToArray(), true);
        }

        var metadata = analysis.PreferredMetadata;
        if (metadata is null)
        {
            return analysis.RuntimePath is not null
                ? new TypeOwnerResolution(true, assemblyName, "resolved-runtime-assembly-only", path.ToArray(), true)
                : new TypeOwnerResolution(false, null, "missing-metadata", path.ToArray(), true);
        }

        if (metadata.PublicTypes.ContainsKey(typeName))
        {
            var status = analysis.RefPath is null && analysis.RuntimePath is not null
                ? "resolved-runtime-definition"
                : "resolved-definition";
            return new TypeOwnerResolution(true, assemblyName, status, path.ToArray(), true);
        }

        if (metadata.DefinedTypes.Contains(typeName))
        {
            return new TypeOwnerResolution(true, assemblyName, "resolved-nonpublic-definition", path.ToArray(), true);
        }

        if (metadata.ForwardedTypes.TryGetValue(typeName, out var forwardedType))
        {
            return ResolveTypeOwnerCore(forwardedType.ImmediateTargetAssembly, typeName, analyses, visited, path);
        }

        return new TypeOwnerResolution(false, null, "missing-type", path.ToArray(), true);
    }

    private static LoadedAssembly TryLoadAssembly(string filePath, MetadataPreference metadataPreference)
    {
        using var stream = File.OpenRead(filePath);
        using var peReader = new PEReader(stream);
        if (!peReader.HasMetadata)
        {
            var nativeName = Path.GetFileNameWithoutExtension(filePath);
            return new LoadedAssembly(nativeName, Path.GetFileName(filePath), filePath, null, metadataPreference);
        }

        var metadataReader = peReader.GetMetadataReader();
        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);
        var metadata = AnalyzeManagedAssembly(metadataReader);
        return new LoadedAssembly(assemblyName, Path.GetFileName(filePath), filePath, metadata, metadataPreference);
    }

    private static ManagedAssemblyMetadata AnalyzeManagedAssembly(MetadataReader metadataReader)
    {
        var publicTypes = new Dictionary<string, PublicTypeAnalysis>(NameComparer);
        var definedTypes = new HashSet<string>(NameComparer);
        var forwardedTypes = new Dictionary<string, ForwardedTypeAnalysis>(NameComparer);
        var assemblyReferences = new HashSet<string>(NameComparer);
        var methodBodyCount = 0;

        foreach (var assemblyReferenceHandle in metadataReader.AssemblyReferences)
        {
            var assemblyReference = metadataReader.GetAssemblyReference(assemblyReferenceHandle);
            assemblyReferences.Add(metadataReader.GetString(assemblyReference.Name));
        }

        foreach (var typeDefinitionHandle in metadataReader.TypeDefinitions)
        {
            var typeDefinition = metadataReader.GetTypeDefinition(typeDefinitionHandle);
            if (IsModuleType(metadataReader, typeDefinition))
            {
                continue;
            }

            var typeName = FullTypeName(metadataReader, typeDefinitionHandle);
            definedTypes.Add(typeName);
            var typeMethods = new List<string>();
            var typeProperties = new List<string>();
            var typeFields = new List<string>();
            var typeEvents = new List<string>();

            foreach (var methodHandle in typeDefinition.GetMethods())
            {
                var method = metadataReader.GetMethodDefinition(methodHandle);
                if (method.RelativeVirtualAddress != 0)
                {
                    methodBodyCount++;
                }

                if (IsPubliclyVisible(method.Attributes))
                {
                    typeMethods.Add(BuildMethodSignature(metadataReader, methodHandle));
                }
            }

            foreach (var propertyHandle in typeDefinition.GetProperties())
            {
                var property = metadataReader.GetPropertyDefinition(propertyHandle);
                var accessors = property.GetAccessors();
                if (IsPublicProperty(metadataReader, accessors))
                {
                    typeProperties.Add(metadataReader.GetString(property.Name));
                }
            }

            foreach (var fieldHandle in typeDefinition.GetFields())
            {
                var field = metadataReader.GetFieldDefinition(fieldHandle);
                if (IsPubliclyVisible(field.Attributes))
                {
                    typeFields.Add(metadataReader.GetString(field.Name));
                }
            }

            foreach (var eventHandle in typeDefinition.GetEvents())
            {
                var eventDefinition = metadataReader.GetEventDefinition(eventHandle);
                var accessors = eventDefinition.GetAccessors();
                if (IsPublicEvent(metadataReader, accessors))
                {
                    typeEvents.Add(metadataReader.GetString(eventDefinition.Name));
                }
            }

            if (IsPublicType(typeDefinition.Attributes))
            {
                publicTypes[typeName] = new PublicTypeAnalysis(
                    typeName,
                    typeMethods.OrderBy(static value => value, NameComparer).ToArray(),
                    typeProperties.OrderBy(static value => value, NameComparer).ToArray(),
                    typeFields.OrderBy(static value => value, NameComparer).ToArray(),
                    typeEvents.OrderBy(static value => value, NameComparer).ToArray());
            }
        }

        foreach (var exportedTypeHandle in metadataReader.ExportedTypes)
        {
            var typeName = FullTypeName(metadataReader, exportedTypeHandle);
            var targetAssembly = ResolveExportedTypeTargetAssembly(metadataReader, exportedTypeHandle);
            if (string.IsNullOrWhiteSpace(targetAssembly))
            {
                continue;
            }

            forwardedTypes[typeName] = new ForwardedTypeAnalysis(typeName, targetAssembly);
        }

        return new ManagedAssemblyMetadata(
            new ReadOnlyDictionary<string, PublicTypeAnalysis>(publicTypes),
            definedTypes,
            new ReadOnlyDictionary<string, ForwardedTypeAnalysis>(forwardedTypes),
            assemblyReferences.OrderBy(static value => value, NameComparer).ToArray(),
            methodBodyCount);
    }

    private static bool IsModuleType(MetadataReader metadataReader, TypeDefinition typeDefinition)
    {
        return string.Equals(metadataReader.GetString(typeDefinition.Name), "<Module>", StringComparison.Ordinal);
    }

    private static bool IsPublicType(TypeAttributes attributes)
    {
        return (attributes & TypeAttributes.Public) != 0 || (attributes & TypeAttributes.NestedPublic) != 0;
    }

    private static bool IsPubliclyVisible(MethodAttributes attributes)
    {
        return (attributes & MethodAttributes.MemberAccessMask) switch
        {
            MethodAttributes.Public => true,
            MethodAttributes.Family => true,
            MethodAttributes.FamORAssem => true,
            _ => false,
        };
    }

    private static bool IsPubliclyVisible(FieldAttributes attributes)
    {
        return (attributes & FieldAttributes.FieldAccessMask) switch
        {
            FieldAttributes.Public => true,
            FieldAttributes.Family => true,
            FieldAttributes.FamORAssem => true,
            _ => false,
        };
    }

    private static bool IsPublicProperty(MetadataReader metadataReader, PropertyAccessors accessors)
    {
        return IsVisibleAccessor(metadataReader, accessors.Getter)
               || IsVisibleAccessor(metadataReader, accessors.Setter);
    }

    private static bool IsPublicEvent(MetadataReader metadataReader, EventAccessors accessors)
    {
        return IsVisibleAccessor(metadataReader, accessors.Adder)
               || IsVisibleAccessor(metadataReader, accessors.Remover)
               || IsVisibleAccessor(metadataReader, accessors.Raiser);
    }

    private static bool IsVisibleAccessor(MetadataReader metadataReader, MethodDefinitionHandle handle)
    {
        if (handle.IsNil)
        {
            return false;
        }

        return IsPubliclyVisible(metadataReader.GetMethodDefinition(handle).Attributes);
    }

    private static bool IsVisibleAccessor(MetadataReader metadataReader, MethodDefinitionHandleCollection handles)
    {
        foreach (var handle in handles)
        {
            if (IsVisibleAccessor(metadataReader, handle))
            {
                return true;
            }
        }

        return false;
    }

    private static string BuildMethodSignature(MetadataReader metadataReader, MethodDefinitionHandle methodHandle)
    {
        var method = metadataReader.GetMethodDefinition(methodHandle);
        var methodName = metadataReader.GetString(method.Name);
        var genericArity = method.GetGenericParameters().Count;
        var parameterCount = 0;
        foreach (var parameterHandle in method.GetParameters())
        {
            var parameter = metadataReader.GetParameter(parameterHandle);
            if (parameter.SequenceNumber > 0)
            {
                parameterCount++;
            }
        }

        return genericArity > 0
            ? $"{methodName}`{genericArity}/{parameterCount}"
            : $"{methodName}/{parameterCount}";
    }

    internal static string FullTypeName(MetadataReader metadataReader, TypeDefinitionHandle handle)
    {
        var typeDefinition = metadataReader.GetTypeDefinition(handle);
        var typeName = metadataReader.GetString(typeDefinition.Name);
        var namespaceName = metadataReader.GetString(typeDefinition.Namespace);
        var declaringTypeHandle = typeDefinition.GetDeclaringType();
        if (!declaringTypeHandle.IsNil)
        {
            return $"{FullTypeName(metadataReader, declaringTypeHandle)}+{typeName}";
        }

        return string.IsNullOrEmpty(namespaceName) ? typeName : $"{namespaceName}.{typeName}";
    }

    private static string FullTypeName(MetadataReader metadataReader, ExportedTypeHandle handle)
    {
        var exportedType = metadataReader.GetExportedType(handle);
        var typeName = metadataReader.GetString(exportedType.Name);
        var namespaceName = metadataReader.GetString(exportedType.Namespace);
        if (exportedType.Implementation.Kind == HandleKind.ExportedType)
        {
            return $"{FullTypeName(metadataReader, (ExportedTypeHandle)exportedType.Implementation)}+{typeName}";
        }

        return string.IsNullOrEmpty(namespaceName) ? typeName : $"{namespaceName}.{typeName}";
    }

    private static string ResolveExportedTypeTargetAssembly(MetadataReader metadataReader, ExportedTypeHandle handle)
    {
        var currentHandle = handle;
        while (true)
        {
            var exportedType = metadataReader.GetExportedType(currentHandle);
            if (exportedType.Implementation.Kind == HandleKind.AssemblyReference)
            {
                var assemblyReference = metadataReader.GetAssemblyReference((AssemblyReferenceHandle)exportedType.Implementation);
                return metadataReader.GetString(assemblyReference.Name);
            }

            if (exportedType.Implementation.Kind != HandleKind.ExportedType)
            {
                return string.Empty;
            }

            currentHandle = (ExportedTypeHandle)exportedType.Implementation;
        }
    }
}

internal enum MetadataPreference : byte
{
    Ref = 0,
    Runtime = 1,
}

internal sealed class AssemblyAnalysis
{
    public AssemblyAnalysis(string assemblyName, string fileName)
    {
        AssemblyName = assemblyName;
        FileName = fileName;
    }

    public string AssemblyName { get; }

    public string FileName { get; }

    public string? RefPath { get; set; }

    public string? RuntimePath { get; set; }

    public ManagedAssemblyMetadata? RefMetadata { get; set; }

    public ManagedAssemblyMetadata? RuntimeMetadata { get; set; }

    public string Classification { get; set; } = "unknown";

    public ManagedAssemblyMetadata? PreferredMetadata => RefMetadata ?? RuntimeMetadata;
}

internal sealed record LoadedAssembly(
    string AssemblyName,
    string FileName,
    string Path,
    ManagedAssemblyMetadata? Metadata,
    MetadataPreference MetadataPreference);

internal sealed record ManagedAssemblyMetadata(
    IReadOnlyDictionary<string, PublicTypeAnalysis> PublicTypes,
    IReadOnlySet<string> DefinedTypes,
    IReadOnlyDictionary<string, ForwardedTypeAnalysis> ForwardedTypes,
    IReadOnlyList<string> AssemblyReferences,
    int MethodBodyCount);

internal sealed record PublicTypeAnalysis(
    string TypeName,
    IReadOnlyList<string> PublicMethods,
    IReadOnlyList<string> PublicProperties,
    IReadOnlyList<string> PublicFields,
    IReadOnlyList<string> PublicEvents);

internal sealed record ForwardedTypeAnalysis(string TypeName, string ImmediateTargetAssembly);

internal sealed record ClosureTarget(string AssemblyName, string Reason);

internal sealed record TypeOwnerResolution(
    bool IsResolved,
    string? OwnerAssembly,
    string ResolutionStatus,
    IReadOnlyList<string> ResolutionPath,
    bool RequiresRuntimeBindingValidation);

internal sealed record TargetFrameworkAnalysisResult(
    ClassificationFrameworkPayload Classification,
    SurfaceManifestFrameworkPayload SurfaceManifest,
    SurfaceProofFrameworkPayload SurfaceProof,
    BindingProofFrameworkPayload BindingProof,
    BenchmarkFrameworkPayload Benchmark);

internal sealed record Phase2TargetFrameworkAnalysisResult(
    Phase2SemanticFrameworkPayload SemanticIr,
    Phase2HelperContractsFrameworkPayload HelperContracts,
    Phase2SurfaceProofFrameworkPayload SurfaceProof,
    Phase2SemanticProofFrameworkPayload SemanticProof,
    Phase2BenchmarkFrameworkPayload Benchmark);

internal sealed record MemberClassificationResult(string Classification, bool HasBody);

internal sealed class Phase0Catalog
{
    private Phase0Catalog(IReadOnlyList<FrameworkCatalogSource> sources)
    {
        Sources = sources;
    }

    public IReadOnlyList<FrameworkCatalogSource> Sources { get; }

    public static Phase0Catalog Load(string catalogPath)
    {
        using var document = JsonDocument.Parse(File.ReadAllText(catalogPath));
        var selectedSources = document.RootElement.GetProperty("selectedSources");
        var sources = new List<FrameworkCatalogSource>();
        foreach (var property in selectedSources.EnumerateObject())
        {
            var sourceElement = property.Value;
            var refElement = sourceElement.GetProperty("ref");
            var runtimeElement = sourceElement.GetProperty("runtime");
            sources.Add(new FrameworkCatalogSource(
                property.Name,
                sourceElement.GetProperty("targetFramework").GetString() ?? throw new InvalidOperationException("missing targetFramework"),
                refElement.GetProperty("version").GetString() ?? throw new InvalidOperationException("missing ref version"),
                refElement.GetProperty("sourceKind").GetString() ?? throw new InvalidOperationException("missing ref sourceKind"),
                refElement.GetProperty("rootPath").GetString() ?? throw new InvalidOperationException("missing ref rootPath"),
                refElement.GetProperty("present").GetBoolean(),
                runtimeElement.GetProperty("version").GetString() ?? throw new InvalidOperationException("missing runtime version"),
                runtimeElement.GetProperty("sourceKind").GetString() ?? throw new InvalidOperationException("missing runtime sourceKind"),
                runtimeElement.GetProperty("rootPath").GetString() ?? throw new InvalidOperationException("missing runtime rootPath"),
                runtimeElement.GetProperty("present").GetBoolean()));
        }

        return new Phase0Catalog(sources);
    }
}

internal sealed class Phase1ContractLaneIndex
{
    private readonly IReadOnlyDictionary<string, int> _candidateAssemblyCounts;

    private Phase1ContractLaneIndex(IReadOnlyDictionary<string, int> candidateAssemblyCounts)
    {
        _candidateAssemblyCounts = candidateAssemblyCounts;
    }

    public int GetCandidateAssemblyCount(string targetFramework)
    {
        return _candidateAssemblyCounts.TryGetValue(targetFramework, out var candidateAssemblyCount) ? candidateAssemblyCount : 0;
    }

    public static Phase1ContractLaneIndex Load(string phase1Directory)
    {
        var classificationPath = Path.Combine(Path.GetFullPath(phase1Directory), "assembly-classification-v1-01.json");
        if (!File.Exists(classificationPath))
        {
            throw new FileNotFoundException("phase1 assembly classification missing", classificationPath);
        }

        using var document = JsonDocument.Parse(File.ReadAllText(classificationPath));
        var targetFrameworks = document.RootElement.GetProperty("targetFrameworks");
        var candidateAssemblyCounts = new Dictionary<string, int>(StringComparer.Ordinal);

        foreach (var frameworkProperty in targetFrameworks.EnumerateObject())
        {
            candidateAssemblyCounts[frameworkProperty.Name] = frameworkProperty.Value.GetProperty("candidateAssemblyCount").GetInt32();
        }

        return new Phase1ContractLaneIndex(candidateAssemblyCounts);
    }
}

internal sealed class Phase2SubstrateIndex
{
    private Phase2SubstrateIndex(IReadOnlyDictionary<string, Phase2SubstrateFrameworkIndex> targetFrameworks)
    {
        TargetFrameworks = targetFrameworks;
    }

    public IReadOnlyDictionary<string, Phase2SubstrateFrameworkIndex> TargetFrameworks { get; }

    public static Phase2SubstrateIndex Load(string phase2Directory)
    {
        var phase2Root = Path.GetFullPath(phase2Directory);
        var semanticIrPath = Path.Combine(phase2Root, "assembly-semantic-ir-system-private-corelib-v1-01.json");
        var helperContractsPath = Path.Combine(phase2Root, "runtime-helper-contracts-v1-01.json");
        if (!File.Exists(semanticIrPath))
        {
            throw new FileNotFoundException("phase2 semantic ir missing", semanticIrPath);
        }

        if (!File.Exists(helperContractsPath))
        {
            throw new FileNotFoundException("phase2 helper contracts missing", helperContractsPath);
        }

        using var semanticDocument = JsonDocument.Parse(File.ReadAllText(semanticIrPath));
        using var helperDocument = JsonDocument.Parse(File.ReadAllText(helperContractsPath));
        var semanticFrameworks = semanticDocument.RootElement.GetProperty("targetFrameworks");
        var helperFrameworks = helperDocument.RootElement.GetProperty("targetFrameworks");
        var targetFrameworks = new Dictionary<string, Phase2SubstrateFrameworkIndex>(StringComparer.Ordinal);

        foreach (var frameworkProperty in semanticFrameworks.EnumerateObject())
        {
            if (!helperFrameworks.TryGetProperty(frameworkProperty.Name, out var helperFramework))
            {
                throw new InvalidOperationException($"phase2 helper contracts missing target framework: {frameworkProperty.Name}");
            }

            var totalPublicMemberCount = frameworkProperty.Value.GetProperty("publicMemberCount").GetInt32();
            var memberIdsByClassification = new Dictionary<string, List<string>>(StringComparer.Ordinal);
            var membersByHelperKind = helperFramework.GetProperty("membersByHelperKind");
            foreach (var classificationProperty in membersByHelperKind.EnumerateObject())
            {
                var memberIds = new List<string>();
                foreach (var memberElement in classificationProperty.Value.EnumerateArray())
                {
                    var memberId = memberElement.GetString();
                    if (!string.IsNullOrWhiteSpace(memberId))
                    {
                        memberIds.Add(memberId);
                    }
                }

                memberIdsByClassification[classificationProperty.Name] = memberIds;
            }

            targetFrameworks[frameworkProperty.Name] = new Phase2SubstrateFrameworkIndex(totalPublicMemberCount, memberIdsByClassification);
        }

        return new Phase2SubstrateIndex(targetFrameworks);
    }
}

internal sealed record Phase2SubstrateFrameworkIndex(
    int TotalPublicMemberCount,
    IReadOnlyDictionary<string, List<string>> MemberIdsByClassification);

internal sealed record FrameworkCatalogSource(
    string CatalogKey,
    string TargetFramework,
    string RefVersion,
    string RefSourceKind,
    string RefRootPath,
    bool RefPresent,
    string RuntimeVersion,
    string RuntimeSourceKind,
    string RuntimeRootPath,
    bool RuntimePresent);

internal sealed class Phase1ClassificationPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public string[] SeedAssemblies { get; set; } = [];

    public SortedDictionary<string, ClassificationFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class ClassificationFrameworkPayload
{
    public string SelectedVersion { get; set; } = string.Empty;

    public string SourceKind { get; set; } = string.Empty;

    public int CandidateAssemblyCount { get; set; }

    public SortedDictionary<string, CandidateAssemblyPayload> CandidateAssemblies { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class CandidateAssemblyPayload
{
    public string FileName { get; set; } = string.Empty;

    public string Classification { get; set; } = string.Empty;

    public string SourceKind { get; set; } = string.Empty;

    public string? RefPath { get; set; }

    public string? RuntimePath { get; set; }

    public string[] SeedReasons { get; set; } = [];

    public int PublicTypeDefinitionCount { get; set; }

    public int ForwardedTypeCount { get; set; }

    public int PublicMemberCount { get; set; }

    public int BindingTargetAssemblyCount { get; set; }

    public bool ContractCompleteEligible { get; set; }
}

internal sealed class Phase1SurfaceManifestPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public SortedDictionary<string, SurfaceManifestFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class SurfaceManifestFrameworkPayload
{
    public string SelectedVersion { get; set; } = string.Empty;

    public SortedDictionary<string, SurfaceManifestAssemblyPayload> Assemblies { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class SurfaceManifestAssemblyPayload
{
    public string Classification { get; set; } = string.Empty;

    public bool ContractCompleteEligible { get; set; }

    public int PublicTypeDefinitionCount { get; set; }

    public int ForwardedTypeCount { get; set; }

    public int PublicMemberCount { get; set; }

    public SortedDictionary<string, PublicTypePayload> PublicTypes { get; set; } = new(StringComparer.Ordinal);

    public SortedDictionary<string, ForwardedTypePayload> ForwardedTypes { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class PublicTypePayload
{
    public string CanonicalOwnerAssembly { get; set; } = string.Empty;

    public string ResolutionStatus { get; set; } = string.Empty;

    public int PublicMethodCount { get; set; }

    public int PublicPropertyCount { get; set; }

    public int PublicFieldCount { get; set; }

    public int PublicEventCount { get; set; }

    public string[] PublicMethods { get; set; } = [];

    public string[] PublicProperties { get; set; } = [];

    public string[] PublicFields { get; set; } = [];

    public string[] PublicEvents { get; set; } = [];
}

internal sealed class ForwardedTypePayload
{
    public string ImmediateTargetAssembly { get; set; } = string.Empty;

    public string? CanonicalOwnerAssembly { get; set; }

    public string ResolutionStatus { get; set; } = string.Empty;

    public string[] ResolutionPath { get; set; } = [];
}

internal sealed class Phase1SurfaceProofPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public SortedDictionary<string, SurfaceProofFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class SurfaceProofFrameworkPayload
{
    public int CandidateAssemblyCount { get; set; }

    public int PublicTypeDefinitionCount { get; set; }

    public int ForwardedTypeCount { get; set; }

    public int PublicMemberCount { get; set; }

    public int UnresolvedForwardedTypeCount { get; set; }

    public int UnresolvedPublicTypeCount { get; set; }

    public int ContractCompleteEligibleAssemblyCount { get; set; }

    public int IssueCount { get; set; }

    public List<string> Issues { get; set; } = [];
}

internal sealed class Phase1BindingProofPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public SortedDictionary<string, BindingProofFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class BindingProofFrameworkPayload
{
    public int BindingsCheckedCount { get; set; }

    public int ResolvedBindingTargetCount { get; set; }

    public int UnresolvedBindingTargetCount { get; set; }

    public int MissingRuntimeAssemblyCount { get; set; }

    public List<string> Issues { get; set; } = [];
}

internal sealed class Phase1BenchmarkPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public long ElapsedMs { get; set; }

    public SortedDictionary<string, BenchmarkFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class BenchmarkFrameworkPayload
{
    public long ElapsedMs { get; set; }

    public int CandidateAssemblyCount { get; set; }

    public int PublicTypeDefinitionCount { get; set; }

    public int ForwardedTypeCount { get; set; }

    public int PublicMemberCount { get; set; }
}

internal sealed class Phase2SemanticIrPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public SortedDictionary<string, Phase2SemanticFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class Phase2SemanticFrameworkPayload
{
    public string SelectedRuntimeVersion { get; set; } = string.Empty;

    public string SourceKind { get; set; } = string.Empty;

    public string AssemblyName { get; set; } = string.Empty;

    public string? RefPath { get; set; }

    public string RuntimePath { get; set; } = string.Empty;

    public int Phase1CandidateAssemblyCount { get; set; }

    public int PublicTypeCount { get; set; }

    public int PublicMemberCount { get; set; }

    public SortedDictionary<string, CorelibSemanticTypePayload> PublicTypes { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class CorelibSemanticTypePayload
{
    public int PublicMethodCount { get; set; }

    public int PublicPropertyCount { get; set; }

    public int PublicFieldCount { get; set; }

    public int PublicEventCount { get; set; }

    public SemanticMemberPayload[] PublicMethods { get; set; } = [];

    public SemanticMemberPayload[] PublicProperties { get; set; } = [];

    public SemanticMemberPayload[] PublicFields { get; set; } = [];

    public SemanticMemberPayload[] PublicEvents { get; set; } = [];
}

internal sealed class SemanticMemberPayload
{
    public string MemberId { get; set; } = string.Empty;

    public string Kind { get; set; } = string.Empty;

    public string Classification { get; set; } = string.Empty;

    public bool HasBody { get; set; }
}

internal sealed class Phase2HelperContractsPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public SortedDictionary<string, Phase2HelperContractsFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class Phase2HelperContractsFrameworkPayload
{
    public string AssemblyName { get; set; } = string.Empty;

    public int ClassifiedPublicMemberCount { get; set; }

    public SortedDictionary<string, int> HelperKindCounts { get; set; } = new(StringComparer.Ordinal);

    public SortedDictionary<string, string[]> MembersByHelperKind { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class Phase2SurfaceProofPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public SortedDictionary<string, Phase2SurfaceProofFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class Phase2SurfaceProofFrameworkPayload
{
    public int PublicTypeCount { get; set; }

    public int PublicMethodCount { get; set; }

    public int PublicPropertyCount { get; set; }

    public int PublicFieldCount { get; set; }

    public int PublicEventCount { get; set; }

    public int UnresolvedPublicTypeCount { get; set; }

    public int IssueCount { get; set; }

    public List<string> Issues { get; set; } = [];
}

internal sealed class Phase2SemanticProofPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public SortedDictionary<string, Phase2SemanticProofFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class Phase2SemanticProofFrameworkPayload
{
    public int PublicMemberCount { get; set; }

    public int ClassifiedPublicMemberCount { get; set; }

    public int UnclassifiedPublicMemberCount { get; set; }

    public int IssueCount { get; set; }

    public List<string> Issues { get; set; } = [];
}

internal sealed class Phase2BenchmarkPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public long ElapsedMs { get; set; }

    public SortedDictionary<string, Phase2BenchmarkFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class Phase2BenchmarkFrameworkPayload
{
    public long ElapsedMs { get; set; }

    public int PublicTypeCount { get; set; }

    public int PublicMemberCount { get; set; }

    public int Phase1CandidateAssemblyCount { get; set; }
}

internal sealed class Phase3FamilyPlanPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public SortedDictionary<string, Phase3FamilyPlanFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class Phase3FamilyPlanFrameworkPayload
{
    public int FamilyCount { get; set; }

    public int TotalPublicMemberCount { get; set; }

    public SortedDictionary<string, Phase3FamilyPayload> Families { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class Phase3FamilyPayload
{
    public int Priority { get; set; }

    public int MemberCount { get; set; }

    public string[] SampleMembers { get; set; } = [];
}

internal sealed class Phase3ProofMatrixPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public SortedDictionary<string, Phase3ProofMatrixFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class Phase3ProofMatrixFrameworkPayload
{
    public int FamilyCount { get; set; }

    public SortedDictionary<string, Phase3ProofMatrixFamilyPayload> Families { get; set; } = new(StringComparer.Ordinal);

    public int IssueCount { get; set; }

    public List<string> Issues { get; set; } = [];
}

internal sealed class Phase3ProofMatrixFamilyPayload
{
    public int Priority { get; set; }

    public int MemberCount { get; set; }

    public string[] RequiredProofs { get; set; } = [];
}

internal sealed class Phase3BenchmarkPayload
{
    public int SchemaVersion { get; set; } = 1;

    public string TaskId { get; set; } = string.Empty;

    public DateTimeOffset GeneratedAt { get; set; } = DateTimeOffset.Now;

    public long ElapsedMs { get; set; }

    public SortedDictionary<string, Phase3BenchmarkFrameworkPayload> TargetFrameworks { get; set; } = new(StringComparer.Ordinal);
}

internal sealed class Phase3BenchmarkFrameworkPayload
{
    public long ElapsedMs { get; set; }

    public int FamilyCount { get; set; }

    public int TotalPublicMemberCount { get; set; }
}
