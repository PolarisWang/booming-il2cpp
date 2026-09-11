// ASYNC-P2-8 A4: regression tests for the IdentifyAsyncBoxPointerLocalSlots over-match
// that silently swallowed 4 codegen failures in the async-iterator fixture.
//
// WHY THESE TESTS EXIST
// ---------------------
// While measuring A4's target (YieldOne), the pipeline reported CodegenFailureCount=4
// with every failure recorded as InvalidOperationException — and NOTHING surfaced:
// BuildMethodSourceSafe catches every exception, writes to Console.Error (invisible to
// the test host), bumps a counter, and substitutes BuildAotUnreachableMethodStub.
// The build stayed green while 4 methods silently lost their real bodies.
//
// The root cause is IdentifyAsyncBoxPointerLocalSlots (ExceptionEmission.Helpers.cs).
// Its three patterns are heuristics over instruction shape, and its DEBUG-only
// self-validation turns an over-match into a thrown exception — which is then swallowed.
// Two independent defects:
//
//   (1) Pattern A (:98-118) treats ANY `newobj` on a `>d__` type as "async ENTRY" and
//       records the slot that receives the result.  In `GetAsyncEnumerator` the newobj
//       CONSTRUCTS the enumerator to RETURN it — there is no Start and no builder call
//       of any kind.  Slot 0 is the return value, not a durable box.
//
//   (2) Pattern A' (:131-169) records the slot of a `ldloca` whose forward callee
//       contains "AwaitUnsafeOnCompleted".  In a MoveNext, `AwaitUnsafeOnCompleted<TAwaiter,
//       TStateMachine>(!!0&, !!1&)` takes TWO by-ref args — the AWAITER (!!0&) and the
//       STATE MACHINE (!!1&).  The ldloca feeding the awaiter position matches the same
//       substring and gets recorded, so the AWAITER slot is mistaken for the box.
//
// The pre-existing unit tests (NativeAotPlannerHelperTests) all feed hand-written
// instruction lists and never drive the real pipeline, which is why neither defect was
// caught.  These tests use the real fixture and the real planner.
//
// COUNTEREXAMPLE (决策2=A): reverting the guards in IdentifyAsyncBoxPointerLocalSlots
// must turn `GetAsyncEnumerator_IsNotMistakenForAnAsyncEntry` and
// `MoveNext_DoesNotRecordTheAwaiterSlot` RED, and must push the pipeline-level
// `IteratorFixture_HasNoSwallowedCodegenFailures` back to CodegenFailureCount=4.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Pipeline;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

public sealed class AsyncBoxPointerDetectorRegressionTests
{
    private static readonly Type s_plannerType = typeof(NativeAotLoweringPlanner);
    private const BindingFlags s_static =
        BindingFlags.Static | BindingFlags.NonPublic | BindingFlags.Public;

    private static readonly string s_iteratorAssemblyPath = LocateIteratorAssemblyDll();

    private static HashSet<int> InvokeDetector(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        AotCoreIrMethodArtifact method)
    {
        var m = s_plannerType.GetMethod("IdentifyAsyncBoxPointerLocalSlots", s_static,
            new[] { typeof(IReadOnlyList<AotCoreIrInstructionArtifact>), typeof(AotCoreIrMethodArtifact) })!;
        return (HashSet<int>)m.Invoke(null, new object[] { instructions, method })!;
    }

    private static AotCoreIrMethodArtifact MethodWithSubject(string subjectId) =>
        new()
        {
            MethodId = "m1",
            SubjectId = subjectId,
            Signature = "void()",
            Identity = new ManagedMethodIdentityArtifact
            {
                AssemblyName = "AsyncIteratorTestAssembly",
                SubjectId = subjectId,
                DeclaringTypeSubjectId = "AsyncIteratorTestAssembly/AsyncIteratorMethods+<YieldOne>d__0",
                DefinitionSubjectId = subjectId,
                MethodId = "m1",
                Signature = "void()",
            },
            NativeSymbol = "chaos_probe_symbol",
            IsStatic = false,
            ReturnType = "System.Void",
            ReturnAbi = new AotCoreIrAbiSlotArtifact { CarrierKindCode = AotCoreIrAbiCarrierKind.Void },
            ParameterCount = 0,
            ParameterAbis = [],
            LocalCount = 8,
            ExceptionRegionCount = 0,
            ExceptionRegions = [],
            Instructions = [],
        };

    /// <summary>
    /// Pattern A must not fire for a method that merely CONSTRUCTS a state machine to
    /// return it.  `GetAsyncEnumerator` contains
    /// <code>newobj &lt;YieldOne&gt;d__0::.ctor(int32); stloc.0; ldloc.0; ret</code>
    /// — the stloc'd slot is the RETURN VALUE.  Treating it as a durable box pointer
    /// is a value/address flip, and the DEBUG self-validation correctly refuses to emit
    /// it — but the refusal is thrown and swallowed, so the method degrades to an
    /// AOT-unreachable stub with a green build.
    ///
    /// COUNTEREXAMPLE: remove the entry guard from Pattern A and this goes RED
    /// (the detector returns [0] instead of empty).
    /// </summary>
    [Fact]
    public void GetAsyncEnumerator_IsNotMistakenForAnAsyncEntry()
    {
        var method = MethodWithSubject(
            "AsyncIteratorTestAssembly/AsyncIteratorMethods+<YieldOne>d__0::"
            + "System.Collections.Generic.IAsyncEnumerable<System.Int32>.GetAsyncEnumerator:"
            + "System.Collections.Generic.IAsyncEnumerator<System.Int32>(System.Threading.CancellationToken)");

        // Faithful to the real IL (see AsyncIteratorTestAssembly/AsyncIteratorMethods.cs
        // -> <YieldOne>d__0::GetAsyncEnumerator): construct-and-return, no builder call.
        var instructions = new List<AotCoreIrInstructionArtifact>
        {
            new() { Op = "ldarg", IlOffset = 0, Operand = 0 },
            new() { Op = "ldc.i4", IlOffset = 1, Operand = -3 },
            new() { Op = "newobj", IlOffset = 2,
                TargetReference = new AotCoreIrReferenceArtifact
                {
                    Kind = AotCoreIrReferenceKind.Type,
                    AssemblyName = "AsyncIteratorTestAssembly",
                    SubjectId = "AsyncIteratorTestAssembly/AsyncIteratorMethods+<YieldOne>d__0",
                }},
            new() { Op = "stloc", IlOffset = 3, Operand = 0 },
            new() { Op = "ldloc", IlOffset = 4, Operand = 0 },
            new() { Op = "ret", IlOffset = 5 },
        };

        var result = InvokeDetector(instructions, method);

        Assert.Empty(result);
    }

    /// <summary>
    /// Pattern A' must record the STATE MACHINE argument of
    /// AwaitUnsafeOnCompleted&lt;TAwaiter, TStateMachine&gt;(!!0&amp;, !!1&amp;) —
    /// not the AWAITER argument, which is the first by-ref and matches the same
    /// "AwaitUnsafeOnCompleted" substring.
    ///
    /// Real shape (PlainAsync::MoveNext):
    /// <code>
    ///   ldloca.s 2                  // awaiter   (!!0&amp;)
    ///   ldloca.s 4                  // state machine (!!1&amp;)
    ///   call AwaitUnsafeOnCompleted&lt;YieldAwaiter, &lt;PlainAsync&gt;d__3&gt;(!!0&amp;, !!1&amp;)
    /// </code>
    /// The detector currently records slot 2 (the awaiter).  A YieldAwaiter is not a
    /// GC-tracked state machine; flipping it value/address corrupts the emitted code,
    /// and the DEBUG validation then throws — swallowed, stub emitted, green build.
    ///
    /// COUNTEREXAMPLE: remove the state-machine-position guard and this goes RED
    /// (the detector returns [4, 2] instead of just [4]).
    /// </summary>
    [Fact]
    public void MoveNext_DoesNotRecordTheAwaiterSlot()
    {
        var method = MethodWithSubject(
            "AsyncIteratorTestAssembly/AsyncIteratorMethods+<PlainAsync>d__3::MoveNext:System.Void()");

        var instructions = new List<AotCoreIrInstructionArtifact>
        {
            new() { Op = "ldarg", IlOffset = 0, Operand = 0 },
            new() { Op = "stloc", IlOffset = 1, Operand = 4 },   // slot 4 = this copy (Pattern B)
            new() { Op = "ldloca", IlOffset = 2, Operand = 2 },  // awaiter          (!!0&)
            new() { Op = "ldloca", IlOffset = 3, Operand = 4 },  // state machine    (!!1&)
            new() { Op = "call", IlOffset = 4,
                Callee = "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder`1"
                       + "::AwaitUnsafeOnCompleted" },
        };

        var result = InvokeDetector(instructions, method);

        Assert.Contains(4, result);      // the state-machine copy IS the box
        Assert.DoesNotContain(2, result); // the awaiter is NOT
    }

    /// <summary>
    /// Pipeline-level acceptance for the A4 target: emitting the async-iterator fixture
    /// must not silently swallow any codegen failure.  This is the assertion that would
    /// have caught the defect in the first place — it does not care WHICH method failed
    /// or why, only that none did.
    ///
    /// COUNTEREXAMPLE: revert either detector guard and this goes RED
    /// (CodegenFailureCount returns to 4).
    /// </summary>
    [Fact]
    public void IteratorFixture_HasNoSwallowedCodegenFailures()
    {
        using var ctx = new TempCtx();
        var request = new ManagedClosureRequest(
            InputAssemblyPath: s_iteratorAssemblyPath,
            OutputRootPath: ctx.OutputRoot,
            EntryPointSubjectIdOverride: null,
            AdditionalAssemblyPaths: null,
            FullAssemblyClosure: true);
        var exec = new PipelinePlan().Execute(request);
        Assert.False(exec.IsFailure,
            $"pipeline failed: {exec.Error?.Code}: {exec.Error?.Message}");
        var result = exec.Value!;

        var anyMoveNext = result.AotCoreIr.Methods
            .First(m => (m.SubjectId ?? "").Contains("YieldOne")
                     && (m.SubjectId ?? "").Contains("::MoveNext"));

        var planner = new NativeAotLoweringPlanner();
        planner.Create(
            new NativeAotLoweringPlanArtifact
            {
                PlanKind = "full-assembly-entry",
                AssemblyName = "AsyncIteratorTestAssembly",
                EntrySubjectId = anyMoveNext.SubjectId,
                NativeEntryFunctionName = string.Empty,
                EntrySymbol = anyMoveNext.NativeSymbol,
                EntryMethodToken = "0x06000001",
                WorkloadAbi = "full-assembly",
            },
            result.AotCoreIr,
            anyMoveNext,
            new ManagedClosureManifestArtifact
            {
                AssemblyName = "AsyncIteratorTestAssembly",
                EntrySubjectId = anyMoveNext.SubjectId,
                InputAssemblyPath = s_iteratorAssemblyPath,
                InputModuleVersionId = result.ClosureManifest!.InputModuleVersionId,
                FullAssemblyClosure = true,
                Artifacts = [],
            },
            result.MetadataRegistration,
            result.SupplementalMetadataTemplate,
            fullAssemblyMode: true);

        Assert.True(planner.CodegenFailureCount == 0,
            $"codegen silently swallowed {planner.CodegenFailureCount} failure(s): "
            + string.Join(", ", planner.CodegenFailureByType.Select(kv => $"{kv.Key} x{kv.Value}")));
    }

    private static string LocateIteratorAssemblyDll()
    {
        var repoRoot = RepoRootLocator.FindFromBaseDirectory();
        string config = "Debug";
        var baseDir = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory);
        if (baseDir is not null)
        {
            var configDir = baseDir.Parent;
            var binDir = configDir?.Parent;
            if (binDir is not null &&
                string.Equals(binDir.Name, "bin", StringComparison.OrdinalIgnoreCase) &&
                configDir!.Name is "Debug" or "Release" or "RelWithDebInfo")
            {
                config = configDir.Name;
            }
        }
        var resolved = Path.Combine(
            repoRoot, "tests", "unit", "managed", "codegen",
            "AsyncIteratorTestAssembly", "bin", config, "net9.0",
            "AsyncIteratorTestAssembly.dll");
        if (!File.Exists(resolved))
            throw new FileNotFoundException($"fixture not at '{resolved}'", resolved);
        return resolved;
    }

    private sealed class TempCtx : IDisposable
    {
        public string OutputRoot { get; } = Path.Combine(
            Path.GetTempPath(), "ChaosA4BoxDetector_" + Guid.NewGuid().ToString("N"));
        public void Dispose()
        {
            try { if (Directory.Exists(OutputRoot)) Directory.Delete(OutputRoot, true); }
            catch { }
        }
    }
}
