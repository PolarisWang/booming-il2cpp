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
    SimpleLibrarySolutionManagedOutput = 4,

    /// <summary>
    /// SolutionCorePack managed output slice for the multi-project solution.
    /// </summary>
    MultiProjectSolutionManagedOutput = 5,

    /// <summary>
    /// SolutionCorePack managed output slice for the package-reference solution.
    /// </summary>
    PackageReferenceSolutionManagedOutput = 6,

    /// <summary>
    /// SolutionCorePack core runtime proof slice.
    /// </summary>
    CoreRuntimeFeaturesProof = 7,

    /// <summary>
    /// SolutionCorePack managed output slice for the explicit reference-assembly solution.
    /// </summary>
    ReferenceAssemblySolutionManagedOutput = 8,

    /// <summary>
    /// SolutionCorePack managed output slice for the repository-owned corelib reference solution.
    /// </summary>
    CoreLibReferenceSolutionManagedOutput = 9,

    /// <summary>
    /// SolutionCorePack managed output slice for the mixed reference closure solution.
    /// </summary>
    MixedReferenceClosureSolutionManagedOutput = 10,

    /// <summary>
    /// SolutionCorePack native proof slice for the canonical interface-dispatch proof.
    /// </summary>
    CoreRuntimeInterfaceDispatchProof = 11,
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

/// <summary>
/// Defines the top-level capability families carried by proof and benchmark entries.
/// </summary>
public enum ChaosCapabilityFamily : byte
{
    /// <summary>
    /// No explicit capability family is declared.
    /// </summary>
    None = 0,

    /// <summary>
    /// Primitive operators, conversions, and array basics.
    /// </summary>
    PrimitivesAndOps = 1,

    /// <summary>
    /// Generic instantiation, layout, and dispatch behavior.
    /// </summary>
    TypeSystemAndGenerics = 2,

    /// <summary>
    /// Async/await, task, and iterator state-machine support.
    /// </summary>
    AsyncAndIterators = 3,

    /// <summary>
    /// Delegate invocation, chaining, and closure capture behavior.
    /// </summary>
    DelegatesAndClosures = 4,

    /// <summary>
    /// Exception handling and control-flow behavior.
    /// </summary>
    ExceptionsAndControlFlow = 5,

    /// <summary>
    /// Reflection, metadata lookup, and linker metadata contracts.
    /// </summary>
    ReflectionAndMetadata = 6,

    /// <summary>
    /// Native interop and marshaling behavior.
    /// </summary>
    InteropAndMarshaling = 7,

    /// <summary>
    /// Threading, task scheduling, and synchronization behavior.
    /// </summary>
    ThreadingAndSynchronization = 8,

    /// <summary>
    /// Runtime helper, loader, and lifecycle services.
    /// </summary>
    RuntimeServices = 9,

    /// <summary>
    /// Unsafe, span, function-pointer, and low-level runtime support.
    /// </summary>
    UnsafeAndLowLevel = 10,

    /// <summary>
    /// Trace, crash, and diagnostics evidence flows.
    /// </summary>
    DiagnosticsAndTracing = 11,

    /// <summary>
    /// Linker closure and AOT instantiation coverage.
    /// </summary>
    LinkerAndAotClosure = 12,

    /// <summary>
    /// Hot-update package, patch, and rollback workflows.
    /// </summary>
    HotUpdateWorkflow = 13,

    /// <summary>
    /// Managed, native, and interpreter mixed execution flows.
    /// </summary>
    MixedExecution = 14,
}

/// <summary>
/// Defines the stable level-2 capability items carried by proof and benchmark entries.
/// </summary>
public enum ChaosCapabilityItem : ushort
{
    /// <summary>
    /// No explicit capability item is declared.
    /// </summary>
    None = 0,

    /// <summary>
    /// Arithmetic operators and numeric expression behavior.
    /// </summary>
    ArithmeticOps = 1,

    /// <summary>
    /// Bitwise operators and shift behavior.
    /// </summary>
    BitwiseOps = 2,

    /// <summary>
    /// Branching, comparison, and boolean control flow.
    /// </summary>
    ComparisonAndBranch = 3,

    /// <summary>
    /// Numeric conversion and cast behavior.
    /// </summary>
    NumericConversion = 4,

    /// <summary>
    /// Overflow and checked/unchecked numeric behavior.
    /// </summary>
    OverflowBehavior = 5,

    /// <summary>
    /// Array construction, indexing, and element access.
    /// </summary>
    ArrayBasics = 6,

    /// <summary>
    /// Array boxing and boxed element roundtrip behavior.
    /// </summary>
    ArrayBoxing = 7,

    /// <summary>
    /// Generic type and method instantiation behavior.
    /// </summary>
    GenericInstantiation = 8,

    /// <summary>
    /// Generic constraint validation and binding behavior.
    /// </summary>
    GenericConstraint = 9,

    /// <summary>
    /// Generic virtual method dispatch behavior.
    /// </summary>
    GenericVirtualDispatch = 10,

    /// <summary>
    /// Generic interface dispatch behavior.
    /// </summary>
    GenericInterfaceDispatch = 11,

    /// <summary>
    /// Generic collection construction and usage behavior.
    /// </summary>
    GenericCollections = 12,

    /// <summary>
    /// Generic layout and field arrangement behavior.
    /// </summary>
    GenericLayout = 13,

    /// <summary>
    /// Async/await state-machine behavior.
    /// </summary>
    AsyncAwait = 14,

    /// <summary>
    /// Task and ValueTask flow behavior.
    /// </summary>
    TaskAndValueTaskFlow = 15,

    /// <summary>
    /// Iterator state-machine lowering and execution behavior.
    /// </summary>
    IteratorStateMachine = 16,

    /// <summary>
    /// Delegate invocation behavior.
    /// </summary>
    DelegateInvocation = 17,

    /// <summary>
    /// Delegate chaining and multicast behavior.
    /// </summary>
    DelegateChaining = 18,

    /// <summary>
    /// Lambda closure capture behavior.
    /// </summary>
    LambdaClosureCapture = 19,

    /// <summary>
    /// Event subscription and callback flow behavior.
    /// </summary>
    EventCallbackFlow = 20,

    /// <summary>
    /// Custom attribute lookup behavior.
    /// </summary>
    CustomAttributeLookup = 21,

    /// <summary>
    /// Member metadata lookup behavior.
    /// </summary>
    MemberMetadataLookup = 22,

    /// <summary>
    /// Metadata supplement generation and consumption behavior.
    /// </summary>
    MetadataSupplement = 23,

    /// <summary>
    /// Metadata closure retention behavior.
    /// </summary>
    MetadataClosure = 24,

    /// <summary>
    /// Linker preserve contract behavior.
    /// </summary>
    LinkerPreserveContract = 25,

    /// <summary>
    /// Linker stripping detection behavior.
    /// </summary>
    LinkerStripping = 26,

    /// <summary>
    /// Basic exception throw/catch behavior.
    /// </summary>
    ExceptionBasic = 27,

    /// <summary>
    /// Nested exception behavior.
    /// </summary>
    NestedException = 28,

    /// <summary>
    /// Finally/filter control-flow behavior.
    /// </summary>
    FinallyAndFilter = 29,

    /// <summary>
    /// Cross-boundary exception propagation behavior.
    /// </summary>
    CrossBoundaryException = 30,

    /// <summary>
    /// Native call interop behavior.
    /// </summary>
    NativeCallInterop = 31,

    /// <summary>
    /// String and UTF8 marshaling behavior.
    /// </summary>
    StringAndUtf8Marshaling = 32,

    /// <summary>
    /// Struct marshaling behavior.
    /// </summary>
    StructMarshaling = 33,

    /// <summary>
    /// Delegate callback interop behavior.
    /// </summary>
    DelegateCallbackInterop = 34,

    /// <summary>
    /// Thread creation and execution behavior.
    /// </summary>
    Threading = 35,

    /// <summary>
    /// Task scheduling and continuation behavior.
    /// </summary>
    TaskScheduling = 36,

    /// <summary>
    /// Monitor and lock synchronization behavior.
    /// </summary>
    MonitorAndLocking = 37,

    /// <summary>
    /// Thread-local state behavior.
    /// </summary>
    ThreadLocalState = 38,

    /// <summary>
    /// Runtime helper intrinsic behavior.
    /// </summary>
    RuntimeHelpers = 39,

    /// <summary>
    /// GC-sensitive flow behavior.
    /// </summary>
    GcSensitiveFlow = 40,

    /// <summary>
    /// Resource lifecycle behavior.
    /// </summary>
    ResourceLifecycle = 41,

    /// <summary>
    /// Loader contract behavior.
    /// </summary>
    LoaderContract = 42,

    /// <summary>
    /// Span and memory slicing behavior.
    /// </summary>
    SpanAndMemory = 43,

    /// <summary>
    /// Ref struct and stackalloc behavior.
    /// </summary>
    RefStructAndStackalloc = 44,

    /// <summary>
    /// Unsafe pointer behavior.
    /// </summary>
    UnsafePointer = 45,

    /// <summary>
    /// Function pointer behavior.
    /// </summary>
    FunctionPointer = 46,

    /// <summary>
    /// Runtime trace output behavior.
    /// </summary>
    RuntimeTraceOutput = 47,

    /// <summary>
    /// Native symbol evidence behavior.
    /// </summary>
    NativeSymbolEvidence = 48,

    /// <summary>
    /// Crash evidence behavior.
    /// </summary>
    CrashEvidence = 49,

    /// <summary>
    /// Failure diagnostics behavior.
    /// </summary>
    FailureDiagnostics = 50,

    /// <summary>
    /// Required instantiation closure behavior.
    /// </summary>
    RequiredInstantiationClosure = 51,

    /// <summary>
    /// Generic sharing boundary behavior.
    /// </summary>
    GenericSharingBoundary = 52,

    /// <summary>
    /// AOT closure validation behavior.
    /// </summary>
    AotClosureValidation = 53,

    /// <summary>
    /// Hot-update package load behavior.
    /// </summary>
    PackageLoad = 54,

    /// <summary>
    /// Hot-update shared contract binding behavior.
    /// </summary>
    SharedContractBinding = 55,

    /// <summary>
    /// Hot-update method replacement behavior.
    /// </summary>
    MethodReplacement = 56,

    /// <summary>
    /// Hot-update metadata supplement behavior.
    /// </summary>
    HotUpdateMetadataSupplement = 57,

    /// <summary>
    /// Patch integrity behavior.
    /// </summary>
    PatchIntegrity = 58,

    /// <summary>
    /// Version rollback behavior.
    /// </summary>
    VersionRollback = 59,

    /// <summary>
    /// Patch callback flow behavior.
    /// </summary>
    PatchCallbackFlow = 60,

    /// <summary>
    /// Mixed managed/native/interpreter dispatch behavior.
    /// </summary>
    MixedBridgeDispatch = 61,

    /// <summary>
    /// Interpreter lowering behavior.
    /// </summary>
    InterpreterLowering = 62,

    /// <summary>
    /// Interpreter arithmetic behavior.
    /// </summary>
    InterpreterArithmetic = 63,

    /// <summary>
    /// Mixed generic flow behavior.
    /// </summary>
    MixedGenericFlow = 64,

    /// <summary>
    /// Mixed exception flow behavior.
    /// </summary>
    MixedExceptionFlow = 65,

    /// <summary>
    /// Mixed delegate flow behavior.
    /// </summary>
    MixedDelegateFlow = 66,

    /// <summary>
    /// Object castclass/isinst exact-type behavior.
    /// </summary>
    ObjectCastAndTypeCheck = 67,

    /// <summary>
    /// Reference-array covariance cast and compatibility behavior.
    /// </summary>
    ArrayCovariance = 68,
}

/// <summary>
/// Defines the canonical solution archetypes carried by retained subjects.
/// </summary>
public enum ChaosSolutionArchetype : byte
{
    /// <summary>
    /// No explicit solution archetype is declared.
    /// </summary>
    None = 0,

    /// <summary>
    /// App plus library through project references.
    /// </summary>
    SimpleLibrarySolution = 1,

    /// <summary>
    /// App plus transitive middle/core project graph.
    /// </summary>
    MultiProjectSolution = 2,

    /// <summary>
    /// App plus external package reference closure.
    /// </summary>
    PackageReferenceSolution = 3,

    /// <summary>
    /// App plus explicit repository-owned DLL references.
    /// </summary>
    ReferenceAssemblySolution = 4,

    /// <summary>
    /// App plus repository-owned corelib reference bundle.
    /// </summary>
    CoreLibReferenceSolution = 5,

    /// <summary>
    /// Mixed project, package, and reference dependency closure.
    /// </summary>
    MixedReferenceClosureSolution = 6,

    /// <summary>
    /// Minimal host plus patch hot-update skeleton.
    /// </summary>
    SkeletonPatchSolution = 7,

    /// <summary>
    /// Full-project hot-update graph with host, shared contracts, and patch modules.
    /// </summary>
    FullProjectHotUpdateSolution = 8,

    /// <summary>
    /// Host plus versioned patch compatibility validation graph.
    /// </summary>
    VersionCompatibilitySolution = 9,

    /// <summary>
    /// Patch graph that consumes repository-owned reference assemblies.
    /// </summary>
    ReferenceAssemblyPatchSolution = 10,

    /// <summary>
    /// Managed, interpreter, and AOT mixed bridge graph.
    /// </summary>
    MixedBridgeSolution = 11,
}

[Flags]
/// <summary>
/// Defines hot-update capability points carried by a declared entry.
/// </summary>
public enum ChaosHotUpdateCapability : ushort
{
    /// <summary>
    /// No additional hot-update capability is declared.
    /// </summary>
    None = 0,

    /// <summary>
    /// Patch package load or activation flow.
    /// </summary>
    PackageLoad = 1 << 0,

    /// <summary>
    /// Shared contract binding across host and patch modules.
    /// </summary>
    SharedContractBinding = 1 << 1,

    /// <summary>
    /// Method replacement or redirect support.
    /// </summary>
    MethodReplacement = 1 << 2,

    /// <summary>
    /// Metadata supplement generation or consumption.
    /// </summary>
    MetadataSupplement = 1 << 3,

    /// <summary>
    /// Patch package integrity and consistency checks.
    /// </summary>
    PatchIntegrity = 1 << 4,

    /// <summary>
    /// Version rollback and compatibility band support.
    /// </summary>
    VersionRollback = 1 << 5,

    /// <summary>
    /// Callback flow across host and patch boundaries.
    /// </summary>
    PatchCallbackFlow = 1 << 6,
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

    /// <summary>
    /// Async/await state-machine lowering and execution.
    /// </summary>
    AsyncStateMachine = 1 << 6,

    /// <summary>
    /// Thread creation, scheduling, or thread-local flow.
    /// </summary>
    Threading = 1 << 7,

    /// <summary>
    /// Locks, monitors, or synchronization primitives.
    /// </summary>
    Synchronization = 1 << 8,

    /// <summary>
    /// Span, memory, or slicing-oriented runtime support.
    /// </summary>
    SpanMemory = 1 << 9,

    /// <summary>
    /// Unsafe pointers, fixed buffers, or low-level memory operations.
    /// </summary>
    UnsafePointer = 1 << 10,

    /// <summary>
    /// Function-pointer invocation and bridging support.
    /// </summary>
    FunctionPointer = 1 << 11,

    /// <summary>
    /// Supplemental metadata generation or consumption requirements.
    /// </summary>
    MetadataSupplement = 1 << 12,

    /// <summary>
    /// Generic virtual or interface dispatch support.
    /// </summary>
    GenericVirtualDispatch = 1 << 13,

    /// <summary>
    /// Ref struct, stackalloc, or stack-only flow support.
    /// </summary>
    RefStructStackalloc = 1 << 14,
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
    /// Gets or sets the top-level capability family covered by this entry.
    /// </summary>
    public ChaosCapabilityFamily CapabilityFamily { get; init; }

    /// <summary>
    /// Gets or sets the stable level-2 capability item covered by this entry.
    /// </summary>
    public ChaosCapabilityItem Capability { get; init; }

    /// <summary>
    /// Gets or sets the runtime features required by this entry.
    /// </summary>
    public ChaosRuntimeFeature Requires { get; init; }

    /// <summary>
    /// Gets or sets the canonical solution archetype carried by this entry.
    /// </summary>
    public ChaosSolutionArchetype Archetype { get; init; }

    /// <summary>
    /// Gets or sets the hot-update capability points carried by this entry.
    /// </summary>
    public ChaosHotUpdateCapability HotUpdateCapability { get; init; }

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
    /// Gets or sets the top-level capability family covered by this entry.
    /// </summary>
    public ChaosCapabilityFamily CapabilityFamily { get; init; }

    /// <summary>
    /// Gets or sets the stable level-2 capability item covered by this entry.
    /// </summary>
    public ChaosCapabilityItem Capability { get; init; }

    /// <summary>
    /// Gets or sets the runtime features required by this entry.
    /// </summary>
    public ChaosRuntimeFeature Requires { get; init; }

    /// <summary>
    /// Gets or sets the canonical solution archetype carried by this entry.
    /// </summary>
    public ChaosSolutionArchetype Archetype { get; init; }

    /// <summary>
    /// Gets or sets the hot-update capability points carried by this entry.
    /// </summary>
    public ChaosHotUpdateCapability HotUpdateCapability { get; init; }

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
/// Captures compact assertion state for AOT-friendly proof execution.
/// </summary>
public static class ChaosAssertState
{
    /// <summary>
    /// Gets the current compact exit code.
    /// </summary>
    public static int ExitCode;

    /// <summary>
    /// Resets the compact assertion state.
    /// </summary>
    public static void Reset()
    {
        ExitCode = 0;
    }

    /// <summary>
    /// Returns the current compact exit code.
    /// </summary>
    public static int Complete()
    {
        return ExitCode;
    }

    /// <summary>
    /// Records a proof failure in compact form.
    /// </summary>
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
    public static void Equal(int expected, int actual, string? message = null)
    {
        _ = message;
        if (expected == actual)
        {
            return;
        }

        ChaosAssertState.RecordFailure();
    }

    /// <summary>
    /// Verifies that the expected and actual values are equal.
    /// </summary>
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

}
