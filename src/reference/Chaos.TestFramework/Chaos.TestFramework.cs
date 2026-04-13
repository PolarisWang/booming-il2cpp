using System;

namespace Chaos.TestFramework;

/// <summary>
/// Defines the shared proof categories for declared correctness tests.
/// </summary>
public enum ChaosUnitCategory : byte
{
    /// <summary>
    /// Runtime-observable behavior and output contracts.
    /// </summary>
    RuntimeContract = 1,

    /// <summary>
    /// Metadata, registration, or supplement contracts.
    /// </summary>
    MetadataContract = 2,

    /// <summary>
    /// Interop contracts across managed and native boundaries.
    /// </summary>
    InteropContract = 3,

    /// <summary>
    /// Host lifecycle, embedding, or engine-facing contracts.
    /// </summary>
    HostContract = 4,

    /// <summary>
    /// Hot-update load, rollback, or patch consistency contracts.
    /// </summary>
    HotUpdateContract = 5,
}

/// <summary>
/// Defines the shared runtime benchmark categories.
/// </summary>
public enum ChaosBenchmarkCategory : byte
{
    /// <summary>
    /// Runtime dispatch, scheduling, or call-path benchmarks.
    /// </summary>
    RuntimeDispatch = 1,

    /// <summary>
    /// Startup, cold-start, or initialization benchmarks.
    /// </summary>
    Startup = 2,

    /// <summary>
    /// Allocation, garbage-collection pressure, or churn benchmarks.
    /// </summary>
    Allocation = 3,

    /// <summary>
    /// Hot-update load, switch, or roundtrip benchmarks.
    /// </summary>
    HotUpdate = 4,
}

[Flags]
/// <summary>
/// Defines the standard metrics that a benchmark may emit.
/// </summary>
public enum ChaosMetric : ushort
{
    /// <summary>
    /// No metric requirements are declared.
    /// </summary>
    None = 0,

    /// <summary>
    /// Wall-clock time in microseconds.
    /// </summary>
    WallClockUs = 1 << 0,

    /// <summary>
    /// Managed allocation bytes.
    /// </summary>
    ManagedAllocBytes = 1 << 1,

    /// <summary>
    /// Native allocation bytes.
    /// </summary>
    NativeAllocBytes = 1 << 2,

    /// <summary>
    /// Peak working-set or resident memory.
    /// </summary>
    PeakWorkingSet = 1 << 3,
}

[Flags]
/// <summary>
/// Defines the runtime features required by a declared entry.
/// </summary>
public enum ChaosRuntimeFeature : uint
{
    /// <summary>
    /// No additional runtime feature is required.
    /// </summary>
    None = 0,

    /// <summary>
    /// Generic sharing or generic instantiation support.
    /// </summary>
    GenericSharing = 1 << 0,

    /// <summary>
    /// Reflection or metadata query support.
    /// </summary>
    Reflection = 1 << 1,

    /// <summary>
    /// Delegate, function-pointer, or callback support.
    /// </summary>
    Delegate = 1 << 2,

    /// <summary>
    /// Exception flow, propagation, or bridging support.
    /// </summary>
    ExceptionFlow = 1 << 3,

    /// <summary>
    /// Native interop, P/Invoke, or ABI-boundary support.
    /// </summary>
    NativeInterop = 1 << 4,

    /// <summary>
    /// Hot-update loading, switching, or rollback support.
    /// </summary>
    HotUpdate = 1 << 5,
}

[Flags]
/// <summary>
/// Defines the evidence kinds expected from a proof entry.
/// </summary>
public enum ChaosEvidenceKind : ushort
{
    /// <summary>
    /// No additional evidence is required.
    /// </summary>
    None = 0,

    /// <summary>
    /// Standard-output text evidence.
    /// </summary>
    Stdout = 1 << 0,

    /// <summary>
    /// File, snapshot, or generated artifact evidence.
    /// </summary>
    FileArtifact = 1 << 1,

    /// <summary>
    /// Metadata, manifest, or registration evidence.
    /// </summary>
    Metadata = 1 << 2,

    /// <summary>
    /// Native symbol, export-table, or linker-result evidence.
    /// </summary>
    NativeSymbol = 1 << 3,
}

/// <summary>
/// Marks a declared proof entry inside a subject solution.
/// </summary>
[AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
public sealed class ChaosUnitTestAttribute : Attribute
{
    /// <summary>
    /// Initializes a declared proof entry with its primary category.
    /// </summary>
    public ChaosUnitTestAttribute(ChaosUnitCategory category)
    {
        Category = category;
    }

    /// <summary>
    /// Gets the shared proof category.
    /// </summary>
    public ChaosUnitCategory Category { get; }

    /// <summary>
    /// Gets or sets the optional short alias used by selectors and reports.
    /// </summary>
    public string? Alias { get; init; }

    /// <summary>
    /// Gets or sets the runtime features required by this entry.
    /// </summary>
    public ChaosRuntimeFeature Requires { get; init; }

    /// <summary>
    /// Gets or sets the expected evidence kinds for this entry.
    /// </summary>
    public ChaosEvidenceKind Evidence { get; init; }

    /// <summary>
    /// Gets or sets the local execution priority.
    /// </summary>
    public byte Priority { get; init; }
}

/// <summary>
/// Marks a declared benchmark entry inside a subject solution.
/// </summary>
[AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
public sealed class ChaosBenchmarkAttribute : Attribute
{
    /// <summary>
    /// Initializes a declared benchmark entry with its category and metric mask.
    /// </summary>
    public ChaosBenchmarkAttribute(ChaosBenchmarkCategory category, ChaosMetric metrics)
    {
        Category = category;
        Metrics = metrics;
    }

    /// <summary>
    /// Gets the shared benchmark category.
    /// </summary>
    public ChaosBenchmarkCategory Category { get; }

    /// <summary>
    /// Gets the metric mask emitted by this benchmark.
    /// </summary>
    public ChaosMetric Metrics { get; }

    /// <summary>
    /// Gets or sets the optional short alias used by selectors and reports.
    /// </summary>
    public string? Alias { get; init; }

    /// <summary>
    /// Gets or sets the runtime features required by this entry.
    /// </summary>
    public ChaosRuntimeFeature Requires { get; init; }

    /// <summary>
    /// Gets or sets the benchmark warmup count.
    /// </summary>
    public byte WarmupCount { get; init; }

    /// <summary>
    /// Gets or sets the benchmark iteration count.
    /// </summary>
    public ushort IterationCount { get; init; }

    /// <summary>
    /// Gets or sets the benchmark invocation count.
    /// </summary>
    public ushort InvocationCount { get; init; }
}
