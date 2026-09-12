using System.Reflection;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Chaos.IL2CPP.Tools.ApiSurfaceScanner;

/// <summary>
/// Models for the reflection API coverage matrix.
/// Tier values encode the three-档 judgement defined in
/// docs/dev/in-progress/reflection-production-readiness/design-v1-01.md §3.1.
/// </summary>
public static class Tier
{
    /// <summary>Native has a real implementation (not a residual stub).</summary>
    public const string Real = "real";

    /// <summary>Explicitly unsupported — must throw NotSupportedException.</summary>
    public const string NotSupported = "not-supported";

    /// <summary>Not meaningful under AOT (e.g. Reflection.Emit). Excluded from denominator.</summary>
    public const string NotApplicable = "not-applicable";

    /// <summary>Not yet judged — a todo item for Phase 2.</summary>
    public const string Unclassified = "unclassified";

    /// <summary>
    /// Semantically reachable from existing metadata, but the accessor is NOT
    /// yet implemented in native code. Distinct from <see cref="Real"/> so the
    /// matrix never claims a working implementation it does not have.
    ///
    /// Phase 3 promotes these to <c>real</c> only after the accessor exists and
    /// a real assertion passes (roadmap P3-1).
    /// </summary>
    public const string RealPlanned = "real-planned";

    /// <summary>
    /// Pure enum-constant surface (e.g. BindingFlags, TypeAttributes). These are
    /// metadata value definitions, not reflection operations, and are excluded
    /// from the denominator. Counted separately as <see cref="ApiEntry.MemberKind"/> = "enum".
    /// </summary>
    public const string EnumSurface = "enum-surface";

    /// <summary>
    /// Pure data-carrier types: exception classes and attribute classes. These
    /// are ordinary managed types whose behaviour is already covered by the
    /// general BCL translation path — they hold no reflection *operation*
    /// semantics. Their constructors/properties are ordinary object members.
    /// Excluded from the reflection-operation denominator.
    /// </summary>
    public const string DataCarrier = "data-carrier";
}

/// <summary>
/// A single reflection API entry in the coverage matrix.
/// </summary>
public sealed class ApiEntry
{
    [JsonPropertyName("namespace")] public string Namespace { get; set; } = "";
    [JsonPropertyName("typeName")] public string TypeName { get; set; } = "";
    [JsonPropertyName("memberName")] public string MemberName { get; set; } = "";
    [JsonPropertyName("memberKind")] public string MemberKind { get; set; } = "";
    [JsonPropertyName("signature")] public string Signature { get; set; } = "";
    [JsonPropertyName("isStatic")] public bool IsStatic { get; set; }
    [JsonPropertyName("isPublic")] public bool IsPublic { get; set; }
    [JsonPropertyName("tier")] public string TierValue { get; set; } = Tier.Unclassified;

    /// <summary>fact = semantic only; final = semantic + performance. Per design §3.2.</summary>
    [JsonPropertyName("realKind")] public string RealKind { get; set; } = "none";

    [JsonPropertyName("evidence")] public Evidence Evidence { get; set; } = new();
    [JsonPropertyName("riskRefs")] public List<string> RiskRefs { get; set; } = new();
    [JsonPropertyName("rationale")] public string? Rationale { get; set; }
}

public sealed class Evidence
{
    [JsonPropertyName("managedTest")] public string? ManagedTest { get; set; }
    [JsonPropertyName("nativeImpl")] public string? NativeImpl { get; set; }
    [JsonPropertyName("pipelineStatus")] public string? PipelineStatus { get; set; }
}

public sealed class MatrixSummary
{
    [JsonPropertyName("generatedFrom")] public List<string> GeneratedFrom { get; set; } = new();
    [JsonPropertyName("targetFramework")] public string TargetFramework { get; set; } = "";
    [JsonPropertyName("totalApis")] public int TotalApis { get; set; }
    [JsonPropertyName("byTier")] public Dictionary<string, int> ByTier { get; set; } = new();
    [JsonPropertyName("byType")] public Dictionary<string, int> ByType { get; set; } = new();
    [JsonPropertyName("denominator")] public int Denominator { get; set; }
}

public sealed class CoverageMatrix
{
    [JsonPropertyName("$schema")] public string Schema { get; set; } = "reflection-api-coverage-matrix/v1";
    [JsonPropertyName("generatedAt")] public string GeneratedAt { get; set; } = "";
    [JsonPropertyName("summary")] public MatrixSummary Summary { get; set; } = new();
    [JsonPropertyName("entries")] public List<ApiEntry> Entries { get; set; } = new();
}
