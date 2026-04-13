using System;
using System.Collections.Generic;

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
/// Defines which runtime execution modes a benchmark supports.
/// </summary>
public enum ChaosExecutionMode : byte
{
    /// <summary>
    /// No execution mode is declared.
    /// </summary>
    None = 0,

    /// <summary>
    /// Managed (.NET) execution.
    /// </summary>
    Managed = 1 << 0,

    /// <summary>
    /// Native AOT execution.
    /// </summary>
    Native = 1 << 1,

    /// <summary>
    /// Interpreter / hot-update execution.
    /// </summary>
    Interpreter = 1 << 2,

    /// <summary>
    /// All standard execution modes are supported.
    /// </summary>
    All = Managed | Native | Interpreter,
}

/// <summary>
/// Defines the compact entry families used by retained subject launchers.
/// </summary>
public enum ChaosSubjectEntryKind : byte
{
    /// <summary>
    /// No explicit entry was selected.
    /// </summary>
    None = 0,

    /// <summary>
    /// Correctness proof entry.
    /// </summary>
    Proof = 1,

    /// <summary>
    /// Benchmark workload entry.
    /// </summary>
    Benchmark = 2,
}

/// <summary>
/// Defines the shared retained-subject entry slices.
/// </summary>
public enum ChaosSubjectSlice : ushort
{
    /// <summary>
    /// No explicit slice was selected.
    /// </summary>
    None = 0,

    /// <summary>
    /// HotUpdateHostPack default skeleton proof slice.
    /// </summary>
    HotUpdateSkeletonProof = 1,

    /// <summary>
    /// MixedExecutionFeaturePack default mixed proof slice.
    /// </summary>
    MixedExecutionProof = 2,

    /// <summary>
    /// MixedExecutionFeaturePack interpreter lowering proof slice.
    /// </summary>
    InterpreterLoweringProof = 3,

    /// <summary>
    /// SolutionCorePack managed output slice for the simple library solution.
    /// </summary>
    SolutionSimpleLibManagedOutput = 4,

    /// <summary>
    /// SolutionCorePack managed output slice for the multi-project solution.
    /// </summary>
    SolutionMultiProjectManagedOutput = 5,

    /// <summary>
    /// SolutionCorePack managed output slice for the package-reference solution.
    /// </summary>
    SolutionPackageReferenceManagedOutput = 6,

    /// <summary>
    /// SolutionCorePack default mainline proof slice.
    /// </summary>
    SolutionMainlineProof = 7,
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
/// Represents a compact retained-subject entry selection.
/// </summary>
public readonly record struct ChaosSubjectEntrySelection(
    ChaosSubjectEntryKind EntryKind,
    ChaosSubjectSlice EntrySlice)
{
    /// <summary>
    /// Gets a value indicating whether no explicit entry was selected.
    /// </summary>
    public bool IsNone => EntryKind == ChaosSubjectEntryKind.None && EntrySlice == ChaosSubjectSlice.None;
}

/// <summary>
/// Parses compact retained-subject dispatch arguments.
/// </summary>
public static class ChaosSubjectEntryArguments
{
    /// <summary>
    /// Prefix for the compact entry-kind command-line argument.
    /// </summary>
    public const string EntryKindPrefix = "--chaos-entry-kind=";

    /// <summary>
    /// Prefix for the compact entry-slice command-line argument.
    /// </summary>
    public const string EntrySlicePrefix = "--chaos-entry-slice=";

    /// <summary>
    /// Attempts to parse a compact retained-subject entry selection.
    /// </summary>
    public static bool TryParse(string[]? args, out ChaosSubjectEntrySelection selection)
    {
        byte? entryKind = null;
        ushort? entrySlice = null;

        foreach (var argument in args ?? Array.Empty<string>())
        {
            if (argument.StartsWith(EntryKindPrefix, StringComparison.Ordinal))
            {
                entryKind = ParseByte(argument, EntryKindPrefix, nameof(EntryKindPrefix));
                continue;
            }

            if (argument.StartsWith(EntrySlicePrefix, StringComparison.Ordinal))
            {
                entrySlice = ParseUInt16(argument, EntrySlicePrefix, nameof(EntrySlicePrefix));
            }
        }

        if (entryKind is null && entrySlice is null)
        {
            selection = default;
            return false;
        }

        if (entryKind is null || entrySlice is null)
        {
            throw new ArgumentException("compact subject entry selection requires both kind and slice arguments.", nameof(args));
        }

        selection = new ChaosSubjectEntrySelection(
            (ChaosSubjectEntryKind)entryKind.Value,
            (ChaosSubjectSlice)entrySlice.Value);
        return true;
    }

    private static byte ParseByte(string argument, string prefix, string argumentName)
    {
        var valueText = argument[prefix.Length..];
        if (!byte.TryParse(valueText, out var value))
        {
            throw new ArgumentException($"invalid compact subject entry kind: {valueText}", argumentName);
        }

        return value;
    }

    private static ushort ParseUInt16(string argument, string prefix, string argumentName)
    {
        var valueText = argument[prefix.Length..];
        if (!ushort.TryParse(valueText, out var value))
        {
            throw new ArgumentException($"invalid compact subject entry slice: {valueText}", argumentName);
        }

        return value;
    }
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
    /// Gets or sets the execution modes supported by this benchmark.
    /// </summary>
    public ChaosExecutionMode Modes { get; init; } = ChaosExecutionMode.All;

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

/// <summary>
/// Represents a proof assertion failure.
/// </summary>
public sealed class ChaosAssertionException : Exception
{
    /// <summary>
    /// Initializes a new assertion failure with a message.
    /// </summary>
    public ChaosAssertionException(string message)
        : base(message)
    {
    }
}

/// <summary>
/// Provides minimal proof assertions for retained subjects.
/// </summary>
public static class Assert
{
    /// <summary>
    /// Verifies that the condition is true.
    /// </summary>
    public static void True(bool condition, string? message = null)
    {
        if (!condition)
        {
            throw new ChaosAssertionException(message ?? "Expected condition to be true.");
        }
    }

    /// <summary>
    /// Verifies that the condition is false.
    /// </summary>
    public static void False(bool condition, string? message = null)
    {
        if (condition)
        {
            throw new ChaosAssertionException(message ?? "Expected condition to be false.");
        }
    }

    /// <summary>
    /// Verifies that the expected and actual values are equal.
    /// </summary>
    public static void Equal<T>(T expected, T actual, string? message = null)
    {
        if (EqualityComparer<T>.Default.Equals(expected, actual))
        {
            return;
        }

        throw new ChaosAssertionException(
            message ?? $"Expected '{FormatValue(expected)}' but got '{FormatValue(actual)}'.");
    }

    /// <summary>
    /// Verifies that the value is not null.
    /// </summary>
    public static void NotNull(object? value, string? message = null)
    {
        if (value is null)
        {
            throw new ChaosAssertionException(message ?? "Expected value to be non-null.");
        }
    }

    /// <summary>
    /// Unconditionally fails the current proof.
    /// </summary>
    public static void Fail(string? message = null)
    {
        throw new ChaosAssertionException(message ?? "Assertion failure.");
    }

    /// <summary>
    /// Verifies that the action throws the expected exception type.
    /// </summary>
    public static TException Throws<TException>(Action action, string? message = null)
        where TException : Exception
    {
        ArgumentNullException.ThrowIfNull(action);

        try
        {
            action();
        }
        catch (TException exception)
        {
            return exception;
        }
        catch (Exception exception)
        {
            throw new ChaosAssertionException(
                message
                ?? $"Expected exception '{typeof(TException).FullName}', but got '{exception.GetType().FullName}'.");
        }

        throw new ChaosAssertionException(
            message ?? $"Expected exception '{typeof(TException).FullName}'.");
    }

    private static string FormatValue<T>(T value)
    {
        return value is null ? "<null>" : value.ToString() ?? $"<{typeof(T).Name}>";
    }
}
