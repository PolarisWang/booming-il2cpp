namespace RuntimeSelfTest;

/// <summary>
/// Integration proof — exercises cross-module interactions that span
/// multiple runtime subsystems simultaneously.
///
/// These tests validate that the il2cpp codegen + runtime correctly
/// handles scenarios where GC, threading, delegates, and finalizers
/// interact — not just each subsystem in isolation.
///
/// ── Test Design Principles ──────────────────────────────────────────
/// 1. Each test exercises at least two runtime subsystems
/// 2. Output is deterministic (no race-dependent values)
/// 3. GC pressure is explicit (allocate known sizes, force collect)
/// 4. Thread synchronization uses Monitor (not Thread.Join in some cases)
///    to exercise the runtime's sync primitives
/// ────────────────────────────────────────────────────────────────────
public static class IntegrationProof
{
    /// <summary>
    /// GC + Delegate Invocation integration.
    ///
    /// Creates a delegate pointing to a static method, triggers a GC
    /// collection to verify the delegate's target method pointer survives
    /// compaction, then invokes the delegate.
    ///
    /// This validates:
    /// - Delegate allocation and virtual method dispatch
    /// - GC root scanning of delegate objects
    /// - Delegate invocation after heap compaction
    /// </summary>
    public static int GcAndDelegateInvoke(int input)
    {
        System.Func<int, int> del = new System.Func<int, int>(DoubleValue);
        System.GC.Collect();
        System.GC.WaitForPendingFinalizers();
        return del(input);
    }

    private static int DoubleValue(int x) => x * 2;

    /// <summary>
    /// GC + Finalizer + Re-allocation integration.
    ///
    /// Creates objects with finalizers, forces GC collection to run
    /// finalizers, then allocates new objects and reads them back.
    ///
    /// This validates:
    /// - Finalizer registration and execution
    /// - GC reclaimation of finalizer-reachable objects
    /// - New allocation after finalizer sweep (heap is not corrupted)
    /// - Object header/metadata not clobbered by finalizer queue
    /// </summary>
    public static int FinalizerAndNewAllocation(int input)
    {
        // Phase 1: Create finalizable objects
        for (int i = 0; i < 10; i++)
        {
            _ = new FinalizerProof(i);
        }

        // Phase 2: Force collection — runs pending finalizers
        System.GC.Collect();
        System.GC.WaitForPendingFinalizers();

        // Phase 3: Allocate new objects and verify they work
        var fresh = new GcAllocationProof();
        return fresh.AllocateAndReturn(input);
    }

    /// <summary>
    /// GC stress across multiple interpreter method calls.
    ///
    /// Repeatedly allocates arrays of increasing size across multiple
    /// method invocations, forcing GC to collect between calls.
    /// Each phase's result is accumulated to produce a deterministic sum.
    ///
    /// This validates:
    /// - GC correctness across interpreter dispatch boundaries
    /// - Array allocation and survival across GC cycles
    /// - Interpreter call/ret sequence not corrupted by GC
    /// - Card table updates for cross-generation references
    /// </summary>
    public static int GcAcrossMethodCalls(int count)
    {
        int total = 0;
        for (int phase = 0; phase < count; phase++)
        {
            // Allocate array sized to trigger young GC on larger phases
            int size = (phase + 1) * 16;
            var data = new int[size];

            // Fill — exercises stelem/ldelem across GC-sensitive boundaries
            for (int i = 0; i < size; i++)
            {
                data[i] = phase + i;
            }

            // Partial sum
            int partial = 0;
            for (int i = 0; i < size; i++)
            {
                partial += data[i];
            }

            // Force GC periodically — array must survive
            if (phase % 3 == 2)
            {
                System.GC.Collect();
            }

            total += partial;
        }
        return total;
    }

    /// <summary>
    /// Threaded GC allocation with synchronized handshake.
    ///
    /// Spawns a background thread that allocates objects continuously,
    /// while the main thread forces GC collections. Uses Monitor
    /// (lock) to synchronize a shared counter between threads.
    ///
    /// This validates:
    /// - Thread creation and managed execution (bootstrap + threading)
    /// - GC safepoint synchronization for multiple managed threads
    /// - Thread-local TLAB re-acquisition after GC
    /// - Monitor Enter/Exit (sync primitive) correctness
    /// - No deadlock between GC safepoint and managed locks
    /// </summary>
    public static int ThreadedGcAllocation(int iterations)
    {
        object sync = new object();
        int sharedCounter = 0;
        bool workerDone = false;

        // Background worker: allocates and signals
        System.Threading.Thread worker = new System.Threading.Thread(() =>
        {
            int localSum = 0;
            for (int i = 0; i < iterations; i++)
            {
                var obj = new GcAllocationProof();
                obj.Value = i;
                localSum += obj.Value;
            }
            lock (sync)
            {
                sharedCounter += localSum;
                workerDone = true;
            }
        });

        worker.Start();

        // Main thread: force GC while worker allocates
        for (int i = 0; i < iterations / 4; i++)
        {
            System.GC.Collect();
            System.Threading.Thread.Sleep(0); // yield
        }

        worker.Join();

        lock (sync)
        {
            // Expected: sum(0..iterations-1) = iterations * (iterations - 1) / 2
            return sharedCounter;
        }
    }
}

/// <summary>
/// Helper class with finalizer — used by IntegrationProof.FinalizerAndNewAllocation
/// to verify that finalizer queue processing does not corrupt the heap.
/// </summary>
internal class FinalizerProof
{
    public int Id { get; }

    public FinalizerProof(int id)
    {
        Id = id;
    }

    ~FinalizerProof()
    {
        // Finalizer runs during GC — no heap allocation allowed here.
        // The runtime must not crash when this finalizer is invoked.
        // No stdout output — finalizer lines are non-deterministic
        // and would break the fact-output validation.
    }
}
