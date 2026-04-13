using System.Text.Json;
using Chaos.IL2CPP.HotUpdate;

internal sealed record CompatibilityMatrixConfig
{
    public string SchemaVersion { get; init; } = "v1";

    public string SubjectId { get; init; } = "CompatibilityMatrixProof";

    public string ProjectPath { get; init; } = "subjects/CompatibilityMatrixProof/source/CompatibilityMatrixProof.csproj";

    public IReadOnlyList<string> AotVersions { get; init; } = [];

    public IReadOnlyList<CompatibilityPatchConfig> HotUpdateVersions { get; init; } = [];

    public IReadOnlyList<string> Platforms { get; init; } = [];

    public IReadOnlyList<CompatibilityAdditionalCaseConfig> AdditionalTestCases { get; init; } = [];
}

internal sealed record CompatibilityPatchConfig
{
    public string PatchId { get; init; } = string.Empty;

    public string TargetAotVersion { get; init; } = string.Empty;
}

internal sealed record CompatibilityAdditionalCaseConfig
{
    public string CaseId { get; init; } = string.Empty;

    public string AotVersion { get; init; } = string.Empty;

    public string PatchId { get; init; } = string.Empty;

    public string TargetAotVersion { get; init; } = string.Empty;

    public string Expected { get; init; } = "reject";

    public IReadOnlyList<string> Platforms { get; init; } = [];
}

internal sealed record CompatibilityCaseResult
{
    public required string CaseId { get; init; }

    public required string Source { get; init; }

    public required string Platform { get; init; }

    public required string AotVersion { get; init; }

    public required string PatchId { get; init; }

    public required string TargetAotVersion { get; init; }

    public required string Expected { get; init; }

    public required string Actual { get; init; }

    public required string Status { get; init; }

    public string? Error { get; init; }
}

internal sealed record CompatibilityStatusCounts
{
    public required int Total { get; init; }

    public required int Ok { get; init; }

    public required int Fail { get; init; }
}

internal sealed record CompatibilityPlatformResult
{
    public required string Platform { get; init; }

    public required string Status { get; init; }

    public required CompatibilityStatusCounts StatusCounts { get; init; }
}

internal sealed record CompatibilityMatrixDimensions
{
    public required int AotVersionCount { get; init; }

    public required int HotUpdateVersionCount { get; init; }

    public required int PlatformCount { get; init; }
}

internal sealed record CompatibilityMatrixReport
{
    public string ReportVersion { get; init; } = "v1";

    public required string SchemaVersion { get; init; }

    public required string SubjectId { get; init; }

    public required string Status { get; init; }

    public required CompatibilityMatrixDimensions MatrixDimensions { get; init; }

    public required CompatibilityStatusCounts StatusCounts { get; init; }

    public required IReadOnlyList<CompatibilityPlatformResult> PlatformResults { get; init; }

    public required IReadOnlyList<CompatibilityCaseResult> CaseResults { get; init; }
}

internal static class Program
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        WriteIndented = false,
    };

    private static int Main(string[] args)
    {
        try
        {
            var config = LoadConfig(args);
            var report = BuildReport(config);
            System.Console.WriteLine(JsonSerializer.Serialize(report, JsonOptions));
            return string.Equals(report.Status, "ok", StringComparison.Ordinal) ? 0 : 1;
        }
        catch (Exception exception)
        {
            System.Console.Error.WriteLine(exception.Message);
            return 2;
        }
    }

    private static CompatibilityMatrixConfig LoadConfig(string[] args)
    {
        if (args.Length == 0)
        {
            return CreateDefaultConfig();
        }

        if (args.Length == 2 && string.Equals(args[0], "--config", StringComparison.Ordinal))
        {
            var configPath = args[1];
            if (!File.Exists(configPath))
            {
                throw new FileNotFoundException($"compatibility matrix config not found: {configPath}");
            }

            var config = JsonSerializer.Deserialize<CompatibilityMatrixConfig>(
                File.ReadAllText(configPath),
                JsonOptions);
            if (config is null)
            {
                throw new InvalidOperationException($"compatibility matrix config is empty: {configPath}");
            }

            ValidateConfig(config);
            return config;
        }

        throw new InvalidOperationException("usage: CompatibilityMatrixProof [--config <path>]");
    }

    private static CompatibilityMatrixConfig CreateDefaultConfig()
    {
        return new CompatibilityMatrixConfig
        {
            AotVersions = ["1.0.0"],
            HotUpdateVersions =
            [
                new CompatibilityPatchConfig
                {
                    PatchId = "patch-001",
                    TargetAotVersion = "1.0.0",
                },
            ],
            Platforms = ["windows-x64"],
            AdditionalTestCases = [],
        };
    }

    private static void ValidateConfig(CompatibilityMatrixConfig config)
    {
        if (!string.Equals(config.SchemaVersion, "v1", StringComparison.Ordinal))
        {
            throw new InvalidOperationException($"unsupported compatibility matrix schemaVersion: {config.SchemaVersion}");
        }

        if (string.IsNullOrWhiteSpace(config.SubjectId))
        {
            throw new InvalidOperationException("compatibility matrix subjectId must not be empty");
        }

        if (string.IsNullOrWhiteSpace(config.ProjectPath))
        {
            throw new InvalidOperationException("compatibility matrix projectPath must not be empty");
        }

        if (config.AotVersions.Count == 0)
        {
            throw new InvalidOperationException("compatibility matrix must declare at least one AOT version");
        }

        if (config.HotUpdateVersions.Count == 0)
        {
            throw new InvalidOperationException("compatibility matrix must declare at least one hot update version");
        }

        if (config.Platforms.Count == 0)
        {
            throw new InvalidOperationException("compatibility matrix must declare at least one platform");
        }

        foreach (var aotVersion in config.AotVersions)
        {
            if (string.IsNullOrWhiteSpace(aotVersion))
            {
                throw new InvalidOperationException("compatibility matrix AOT versions must not contain empty values");
            }
        }

        foreach (var patch in config.HotUpdateVersions)
        {
            if (string.IsNullOrWhiteSpace(patch.PatchId) || string.IsNullOrWhiteSpace(patch.TargetAotVersion))
            {
                throw new InvalidOperationException("compatibility matrix hot update versions require patchId and targetAotVersion");
            }
        }

        foreach (var platform in config.Platforms)
        {
            if (string.IsNullOrWhiteSpace(platform))
            {
                throw new InvalidOperationException("compatibility matrix platforms must not contain empty values");
            }
        }

        foreach (var additionalCase in config.AdditionalTestCases)
        {
            if (string.IsNullOrWhiteSpace(additionalCase.CaseId))
            {
                throw new InvalidOperationException("additional compatibility cases require caseId");
            }

            if (string.IsNullOrWhiteSpace(additionalCase.AotVersion) ||
                string.IsNullOrWhiteSpace(additionalCase.PatchId) ||
                string.IsNullOrWhiteSpace(additionalCase.TargetAotVersion))
            {
                throw new InvalidOperationException(
                    $"additional compatibility case '{additionalCase.CaseId}' is missing required fields");
            }

            if (!string.Equals(additionalCase.Expected, "ok", StringComparison.Ordinal) &&
                !string.Equals(additionalCase.Expected, "reject", StringComparison.Ordinal))
            {
                throw new InvalidOperationException(
                    $"additional compatibility case '{additionalCase.CaseId}' must use expected='ok' or 'reject'");
            }
        }
    }

    private static CompatibilityMatrixReport BuildReport(CompatibilityMatrixConfig config)
    {
        var caseResults = new List<CompatibilityCaseResult>();

        foreach (var aotVersion in config.AotVersions)
        {
            foreach (var patch in config.HotUpdateVersions)
            {
                foreach (var platform in config.Platforms)
                {
                    caseResults.Add(
                        EvaluateCase(
                            caseId: $"matrix-{aotVersion}-{patch.PatchId}-{platform}",
                            source: "matrix",
                            platform: platform,
                            aotVersion: aotVersion,
                            patchId: patch.PatchId,
                            targetAotVersion: patch.TargetAotVersion,
                            expected: "ok"));
                }
            }
        }

        foreach (var additionalCase in config.AdditionalTestCases)
        {
            var platforms = additionalCase.Platforms.Count == 0
                ? config.Platforms
                : additionalCase.Platforms;
            foreach (var platform in platforms)
            {
                caseResults.Add(
                    EvaluateCase(
                        caseId: $"{additionalCase.CaseId}-{platform}",
                        source: "additional",
                        platform: platform,
                        aotVersion: additionalCase.AotVersion,
                        patchId: additionalCase.PatchId,
                        targetAotVersion: additionalCase.TargetAotVersion,
                        expected: additionalCase.Expected));
            }
        }

        var totalCounts = BuildCounts(caseResults);
        var platformResults = config.Platforms
            .Select(
                platform =>
                {
                    var platformCases = caseResults.Where(caseResult => string.Equals(caseResult.Platform, platform, StringComparison.Ordinal)).ToList();
                    return new CompatibilityPlatformResult
                    {
                        Platform = platform,
                        Status = ResolveStatus(platformCases),
                        StatusCounts = BuildCounts(platformCases),
                    };
                })
            .ToList();

        return new CompatibilityMatrixReport
        {
            SchemaVersion = config.SchemaVersion,
            SubjectId = config.SubjectId,
            Status = ResolveStatus(caseResults),
            MatrixDimensions = new CompatibilityMatrixDimensions
            {
                AotVersionCount = config.AotVersions.Count,
                HotUpdateVersionCount = config.HotUpdateVersions.Count,
                PlatformCount = config.Platforms.Count,
            },
            StatusCounts = totalCounts,
            PlatformResults = platformResults,
            CaseResults = caseResults,
        };
    }

    private static CompatibilityStatusCounts BuildCounts(IReadOnlyCollection<CompatibilityCaseResult> caseResults)
    {
        return new CompatibilityStatusCounts
        {
            Total = caseResults.Count,
            Ok = caseResults.Count(caseResult => string.Equals(caseResult.Status, "ok", StringComparison.Ordinal)),
            Fail = caseResults.Count(caseResult => string.Equals(caseResult.Status, "fail", StringComparison.Ordinal)),
        };
    }

    private static string ResolveStatus(IReadOnlyCollection<CompatibilityCaseResult> caseResults)
    {
        return caseResults.Any(caseResult => string.Equals(caseResult.Status, "fail", StringComparison.Ordinal))
            ? "fail"
            : "ok";
    }

    private static CompatibilityCaseResult EvaluateCase(
        string caseId,
        string source,
        string platform,
        string aotVersion,
        string patchId,
        string targetAotVersion,
        string expected)
    {
        var package = new LoadedHotUpdatePackage
        {
            RootPath = Path.Combine("CompatibilityMatrixProof", platform, patchId),
            Manifest = new HotUpdatePackage
            {
                PackageId = $"com.example.compatibility.{patchId}",
                TargetAotVersion = targetAotVersion,
                Assemblies = [],
                SupplementalMetadata = "metadata-supplement.bin",
                Signature = "signed-proof",
            },
            LoadedAssemblies = new Dictionary<string, LoadedAssemblyImage>(StringComparer.Ordinal),
        };

        try
        {
            PackageValidator.ValidateCompatibleTargetAotVersion(package, aotVersion);
            return new CompatibilityCaseResult
            {
                CaseId = caseId,
                Source = source,
                Platform = platform,
                AotVersion = aotVersion,
                PatchId = patchId,
                TargetAotVersion = targetAotVersion,
                Expected = expected,
                Actual = "ok",
                Status = string.Equals(expected, "ok", StringComparison.Ordinal) ? "ok" : "fail",
                Error = null,
            };
        }
        catch (Exception exception)
        {
            return new CompatibilityCaseResult
            {
                CaseId = caseId,
                Source = source,
                Platform = platform,
                AotVersion = aotVersion,
                PatchId = patchId,
                TargetAotVersion = targetAotVersion,
                Expected = expected,
                Actual = "reject",
                Status = string.Equals(expected, "reject", StringComparison.Ordinal) ? "ok" : "fail",
                Error = exception.Message,
            };
        }
    }
}
