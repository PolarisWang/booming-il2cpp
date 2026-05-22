// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/garbage/collection
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;

public static partial class GarbageCollectionSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // Total number of subjects — the dispatch code reads this constant
    // to know how many test methods exist.  Increment when adding a new subject.
    public const int SubjectCount = 12;

    // [0] System.Private.CoreLib/System.GC::Collect:System.Void()
    // Just verify it doesn't crash.
    public static void Subject_0()
    {
        GC.Collect();
    }

    // [1] System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()
    // Just verify it doesn't crash.
    public static void Subject_1()
    {
        GC.WaitForPendingFinalizers();
    }

    // [2] System.Private.CoreLib/System.GC::KeepAlive:System.Void(System.Object)
    // Just verify it doesn't crash.
    public static void Subject_2()
    {
        GC.KeepAlive(42);
    }

    // [3] System.Private.CoreLib/System.GC::GetGeneration:System.Int32(System.Object)
    // Verify the call returns without error.  Generation guarantee is
    // GC-implementation-specific; just verify it's a non-negative int.
    public static void Subject_3()
    {
        int gen = GC.GetGeneration(42);
        if (gen < 0) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    // Verify the call returns an array with the correct length.
    public static void Subject_4()
    {
        Array arr = Array.CreateInstance(typeof(byte), 42);
        if (arr.Length != 42) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)
    // Verify the call returns without error.  Exact memory value depends on
    // GC internal state; just verify it's a positive number.
    public static void Subject_5()
    {
        long mem = GC.GetTotalMemory(true);
        if (mem <= 0) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.GC::GetGCMemoryInfo: System.GCMemoryInfo()
    // Verify the struct round-trips without crash.  Field values are
    // GC-implementation-specific; just verify the call works.
    public static void Subject_6()
    {
        var info = GC.GetGCMemoryInfo();
        if (info.GetHashCode() == 0) _exitCode = 1;  // weak: just verify no crash
    }

    // ── Additional GC managed API subjects ──────────────────────────

    // [7] System.Private.CoreLib/System.GC::Collect(System.Int32,System.GCCollectionMode)
    // Overload test: Collect with generation + mode.
    public static void Subject_7()
    {
        GC.Collect(0, GCCollectionMode.Default);
    }

    // [8] System.Private.CoreLib/System.GC::get_LatencyMode
    // Property getter: returns the current latency mode.
    public static void Subject_8()
    {
        var mode = GCLatencyMode.Interactive;
        // Just verify it returns a valid enum value
        int modeVal = (int)GC.LatencyMode;
        if (modeVal < 0 || modeVal > 3) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.GC::set_LatencyMode(System.GCLatencyMode)
    // Property setter: change latency mode and verify it sticks.
    public static void Subject_9()
    {
        GCLatencyMode original = GC.LatencyMode;
        GC.LatencyMode = GCLatencyMode.Interactive;
        GCLatencyMode after = GC.LatencyMode;
        if (after != GCLatencyMode.Interactive) _exitCode = 1;
        // Restore original so subsequent subjects run under default mode
        GC.LatencyMode = original;
    }

    // [10] System.Private.CoreLib/System.GC::CollectionCount(System.Int32)
    // Returns per-generation collection count.
    public static void Subject_10()
    {
        long count = GC.CollectionCount(0);
        // Collection count should be a non-negative integer
        if (count < 0) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.GC::GetTotalPauseDuration
    // Returns total accumulated GC pause duration as TimeSpan.
    public static void Subject_11()
    {
        var duration = GC.GetTotalPauseDuration();
        // Total pause should be non-negative
        if (duration.TotalMilliseconds < 0) _exitCode = 1;
    }

}
