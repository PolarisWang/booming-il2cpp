using System;
using System.Collections.Generic;

namespace Chaos.TestFramework;

/// <summary>
/// Defines the shared proof categories for declared correctness tests.
/// </summary>
public enum ChaosUnitCategory : byte
{
    None = 0,
    RuntimeContract = 1,
    MetadataContract = 2,
    InteropContract = 3,
    HostContract = 4,
    HotUpdateContract = 5,
}

/// <summary>
/// Defines the shared runtime benchmark categories.
/// </summary>
public enum ChaosBenchmarkCategory : byte
{
    None = 0,
    RuntimeDispatch = 1,
    Startup = 2,
    Allocation = 3,
    HotUpdate = 4,
}

[Flags]
/// <summary>
/// Defines which runtime execution modes a benchmark supports.
/// </summary>
public enum ChaosExecutionMode : byte
{
    None = 0,
    Managed = 1 << 0,
    Native = 1 << 1,
    Interpreter = 1 << 2,
    All = Managed | Native | Interpreter,
}

/// <summary>
/// Defines the compact entry families used by retained subject launchers.
/// </summary>
public enum ChaosSubjectEntryKind : byte
{
    None = 0,
    Proof = 1,
    Benchmark = 2,
}

/// <summary>
/// Defines the shared retained-subject entry slices.
/// </summary>
public enum ChaosSubjectSlice : ushort
{
    None = 0,
    HotUpdateSkeletonProof = 1,
    MixedExecutionProof = 2,
    InterpreterLoweringProof = 3,
    SimpleLibrarySolutionManagedOutput = 4,
    MultiProjectSolutionManagedOutput = 5,
    PackageReferenceSolutionManagedOutput = 6,
    CoreRuntimeFeaturesProof = 7,
    ReferenceAssemblySolutionManagedOutput = 8,
    CoreLibReferenceSolutionManagedOutput = 9,
    MixedReferenceClosureSolutionManagedOutput = 10,
    CoreRuntimeInterfaceDispatchProof = 11,
}

[Flags]
/// <summary>
/// Defines the standard metrics that a benchmark may emit.
/// </summary>
public enum ChaosMetric : ushort
{
    None = 0,
    WallClockUs = 1 << 0,
    ManagedAllocBytes = 1 << 1,
    NativeAllocBytes = 1 << 2,
    PeakWorkingSet = 1 << 3,
}

/// <summary>
/// Defines the top-level capability families carried by proof and benchmark entries.
/// </summary>
public enum ChaosCapabilityFamily : byte
{
    None = 0,
    PrimitivesAndOps = 1,
    TypeSystemAndGenerics = 2,
    AsyncAndIterators = 3,
    DelegatesAndClosures = 4,
    ExceptionsAndControlFlow = 5,
    ReflectionAndMetadata = 6,
    InteropAndMarshaling = 7,
    ThreadingAndSynchronization = 8,
    RuntimeServices = 9,
    UnsafeAndLowLevel = 10,
    DiagnosticsAndTracing = 11,
    LinkerAndAotClosure = 12,
    HotUpdateWorkflow = 13,
    MixedExecution = 14,
}

/// <summary>
/// Defines the stable level-2 capability items carried by proof and benchmark entries.
/// </summary>
public enum ChaosCapabilityItem : ushort
{
    None = 0,
    ArithmeticOps = 1,
    BitwiseOps = 2,
    ComparisonAndBranch = 3,
    NumericConversion = 4,
    OverflowBehavior = 5,
    ArrayBasics = 6,
    ArrayBoxing = 7,
    GenericInstantiation = 8,
    GenericConstraint = 9,
    GenericVirtualDispatch = 10,
    GenericInterfaceDispatch = 11,
    GenericCollections = 12,
    GenericLayout = 13,
    AsyncAwait = 14,
    TaskAndValueTaskFlow = 15,
    IteratorStateMachine = 16,
    DelegateInvocation = 17,
    DelegateChaining = 18,
    LambdaClosureCapture = 19,
    EventCallbackFlow = 20,
    CustomAttributeLookup = 21,
    MemberMetadataLookup = 22,
    MetadataSupplement = 23,
    MetadataClosure = 24,
    LinkerPreserveContract = 25,
    LinkerStripping = 26,
    ExceptionBasic = 27,
    NestedException = 28,
    FinallyAndFilter = 29,
    CrossBoundaryException = 30,
    NativeCallInterop = 31,
    StringAndUtf8Marshaling = 32,
    StructMarshaling = 33,
    DelegateCallbackInterop = 34,
    Threading = 35,
    TaskScheduling = 36,
    MonitorAndLocking = 37,
    ThreadLocalState = 38,
    RuntimeHelpers = 39,
    GcSensitiveFlow = 40,
    ResourceLifecycle = 41,
    LoaderContract = 42,
    SpanAndMemory = 43,
    RefStructAndStackalloc = 44,
    UnsafePointer = 45,
    FunctionPointer = 46,
    RuntimeTraceOutput = 47,
    NativeSymbolEvidence = 48,
    CrashEvidence = 49,
    FailureDiagnostics = 50,
    RequiredInstantiationClosure = 51,
    GenericSharingBoundary = 52,
    AotClosureValidation = 53,
    PackageLoad = 54,
    SharedContractBinding = 55,
    MethodReplacement = 56,
    HotUpdateMetadataSupplement = 57,
    PatchIntegrity = 58,
    VersionRollback = 59,
    PatchCallbackFlow = 60,
    MixedBridgeDispatch = 61,
    InterpreterLowering = 62,
    InterpreterArithmetic = 63,
    MixedGenericFlow = 64,
    MixedExceptionFlow = 65,
    MixedDelegateFlow = 66,
    ObjectCastAndTypeCheck = 67,
    ArrayCovariance = 68,
}

/// <summary>
/// Defines the canonical solution archetypes carried by retained subjects.
/// </summary>
public enum ChaosSolutionArchetype : byte
{
    None = 0,
    SimpleLibrarySolution = 1,
    MultiProjectSolution = 2,
    PackageReferenceSolution = 3,
    ReferenceAssemblySolution = 4,
    CoreLibReferenceSolution = 5,
    MixedReferenceClosureSolution = 6,
    SkeletonPatchSolution = 7,
    FullProjectHotUpdateSolution = 8,
    VersionCompatibilitySolution = 9,
    ReferenceAssemblyPatchSolution = 10,
    MixedBridgeSolution = 11,
}

[Flags]
/// <summary>
/// Defines hot-update capability points carried by a declared entry.
/// </summary>
public enum ChaosHotUpdateCapability : ushort
{
    None = 0,
    PackageLoad = 1 << 0,
    SharedContractBinding = 1 << 1,
    MethodReplacement = 1 << 2,
    MetadataSupplement = 1 << 3,
    PatchIntegrity = 1 << 4,
    VersionRollback = 1 << 5,
    PatchCallbackFlow = 1 << 6,
}

[Flags]
/// <summary>
/// Defines the runtime features required by a declared entry.
/// </summary>
public enum ChaosRuntimeFeature : uint
{
    None = 0,
    GenericSharing = 1 << 0,
    Reflection = 1 << 1,
    Delegate = 1 << 2,
    ExceptionFlow = 1 << 3,
    NativeInterop = 1 << 4,
    HotUpdate = 1 << 5,
    AsyncStateMachine = 1 << 6,
    Threading = 1 << 7,
    Synchronization = 1 << 8,
    SpanMemory = 1 << 9,
    UnsafePointer = 1 << 10,
    FunctionPointer = 1 << 11,
    MetadataSupplement = 1 << 12,
    GenericVirtualDispatch = 1 << 13,
    RefStructStackalloc = 1 << 14,
}

[Flags]
/// <summary>
/// Defines the evidence kinds expected from a proof entry.
/// </summary>
public enum ChaosEvidenceKind : ushort
{
    None = 0,
    Stdout = 1 << 0,
    FileArtifact = 1 << 1,
    Metadata = 1 << 2,
    NativeSymbol = 1 << 3,
}

/// <summary>
/// Represents a compact retained-subject entry selection.
/// </summary>
public readonly record struct ChaosSubjectEntrySelection(
    ChaosSubjectEntryKind EntryKind,
    ChaosSubjectSlice EntrySlice)
{
    public bool IsNone => EntryKind == ChaosSubjectEntryKind.None && EntrySlice == ChaosSubjectSlice.None;
}

/// <summary>
/// Parses compact retained-subject dispatch arguments.
/// </summary>
public static class ChaosSubjectEntryArguments
{
    public const string EntryKindPrefix = "--chaos-entry-kind=";
    public const string EntrySlicePrefix = "--chaos-entry-slice=";

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

// =========================================================================
//  Class-level attributes
// =========================================================================

/// <summary>
/// Marks a test class as belonging to a specific capability family from the
/// foundation-dll capability-family-ledger. The familyId is a string matching
/// the ledger's familyId (e.g. "System.Private.CoreLib/convert/char").
///
/// Place this on the test class. The framework discovers it automatically
/// and maps test methods to the appropriate verification gates.
/// </summary>
[AttributeUsage(AttributeTargets.Class, AllowMultiple = false)]
public sealed class CapabilityTestAttribute : Attribute
{
    /// <param name="familyId">
    /// Ledger family ID, e.g. "System.Private.CoreLib/convert/char".
    /// Must match a familyId in capability-family-ledger.json.
    /// </param>
    public CapabilityTestAttribute(string familyId)
    {
        FamilyId = familyId;
    }

    /// <summary>Ledger family ID.</summary>
    public string FamilyId { get; }

    // ----- Gate selection -----

    /// <summary>
    /// Whether to run tests in native mode for native-proof verification.
    /// Default: true.
    /// </summary>
    public bool IncludeNativeProof { get; init; } = true;

    /// <summary>
    /// Whether to collect benchmark metrics for this family.
    /// Default: false (opt-in).
    /// </summary>
    public bool IncludeBenchmark { get; init; }

    /// <summary>
    /// Whether to run hot-update tests for this family.
    /// Default: false (opt-in).
    /// </summary>
    public bool IncludeHotUpdate { get; init; }

    // ----- Execution modes -----

    /// <summary>
    /// Supported execution modes for the test methods in this class.
    /// </summary>
    public ChaosExecutionMode ExecutionModes { get; init; } = ChaosExecutionMode.All;
}

// =========================================================================
//  Method-level attributes
// =========================================================================

/// <summary>
/// Marks a declared proof entry inside a subject solution.
/// </summary>
[AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
public sealed class ChaosUnitTestAttribute : Attribute
{
    public ChaosUnitTestAttribute(ChaosUnitCategory category)
    {
        Category = category;
    }

    public ChaosUnitCategory Category { get; }
    public string? Alias { get; init; }
    public ChaosCapabilityFamily CapabilityFamily { get; init; }
    public ChaosCapabilityItem Capability { get; init; }
    public ChaosRuntimeFeature Requires { get; init; }
    public ChaosSolutionArchetype Archetype { get; init; }
    public ChaosHotUpdateCapability HotUpdateCapability { get; init; }
    public ChaosEvidenceKind Evidence { get; init; }
    public byte Priority { get; init; }
}

/// <summary>
/// Marks a declared benchmark entry inside a subject solution.
/// </summary>
[AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
public class BenchmarkAttribute : Attribute
{
    public BenchmarkAttribute(ChaosBenchmarkCategory category, ChaosMetric metrics)
    {
        Category = category;
        Metrics = metrics;
    }

    public ChaosBenchmarkCategory Category { get; }
    public ChaosMetric Metrics { get; }
    public string? Alias { get; init; }
    public ChaosCapabilityFamily CapabilityFamily { get; init; }
    public ChaosCapabilityItem Capability { get; init; }
    public ChaosRuntimeFeature Requires { get; init; }
    public ChaosSolutionArchetype Archetype { get; init; }
    public ChaosHotUpdateCapability HotUpdateCapability { get; init; }
    public ChaosExecutionMode Modes { get; init; } = ChaosExecutionMode.All;
    public byte WarmupCount { get; init; }
    public ushort IterationCount { get; init; }
    public ushort InvocationCount { get; init; }

    // ----- Strongly typed benchmark thresholds -----

    /// <summary>P99 latency upper bound (ms). 0 or negative = no limit.</summary>
    public int P99Ms { get; init; }

    /// <summary>Minimum throughput (ops/sec). 0 or negative = no limit.</summary>
    public int MinOpsPerSec { get; init; }

    /// <summary>Maximum allocation (bytes). 0 or negative = no limit.</summary>
    public int MaxAllocBytes { get; init; }
}

/// <summary>
/// Legacy alias for <see cref="BenchmarkAttribute"/>. Use BenchmarkAttribute instead.
/// </summary>
[Obsolete("Use BenchmarkAttribute instead.")]
[AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
public sealed class ChaosBenchmarkAttribute : BenchmarkAttribute
{
    public ChaosBenchmarkAttribute(ChaosBenchmarkCategory category, ChaosMetric metrics)
        : base(category, metrics)
    {
    }
}

/// <summary>
/// Represents a proof assertion failure.
/// </summary>
public sealed class ChaosAssertionException : Exception
{
    public ChaosAssertionException(string message)
        : base(message)
    {
    }
}

/// <summary>
/// Captures compact assertion state for AOT-friendly proof execution.
/// </summary>
public static class ChaosAssertState
{
    public static int ExitCode;

    public static void Reset()
    {
        ExitCode = 0;
    }

    public static int Complete()
    {
        return ExitCode;
    }

    public static void RecordFailure()
    {
        ExitCode = 1;
    }
}

/// <summary>
/// Provides minimal proof assertions for retained subjects.
/// </summary>
public static class Assert
{
    public static void True(bool condition, string? message = null)
    {
        if (!condition)
        {
            throw new ChaosAssertionException(message ?? "Expected condition to be true.");
        }
    }

    public static void False(bool condition, string? message = null)
    {
        if (condition)
        {
            throw new ChaosAssertionException(message ?? "Expected condition to be false.");
        }
    }

    public static void Equal(int expected, int actual, string? message = null)
    {
        _ = message;
        if (expected == actual)
        {
            return;
        }
        ChaosAssertState.RecordFailure();
    }

    public static void Equal<T>(T expected, T actual, string? message = null)
    {
        object? expectedObject = expected;
        object? actualObject = actual;
        if (expectedObject is null)
        {
            if (actualObject is null)
            {
                return;
            }
        }
        else if (expectedObject.Equals(actualObject))
        {
            return;
        }
        throw new ChaosAssertionException(message ?? "Expected values to be equal.");
    }

    public static void NotNull(object? value, string? message = null)
    {
        if (value is null)
        {
            throw new ChaosAssertionException(message ?? "Expected value to be non-null.");
        }
    }

    public static void Fail(string? message = null)
    {
        throw new ChaosAssertionException(message ?? "Assertion failure.");
    }

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
}
