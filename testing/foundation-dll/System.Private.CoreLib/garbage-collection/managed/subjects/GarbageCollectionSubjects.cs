// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/garbage/collection
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class GarbageCollectionSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.GC::Collect:System.Void()
    public static void Subject_0()
    {
        try { GC.Collect(); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()
    public static void Subject_1()
    {
        try { GC.WaitForPendingFinalizers(); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.GC::KeepAlive:System.Void(System.Object)
    public static void Subject_2()
    {
        try { GC.KeepAlive(null!); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.GC::GetGeneration:System.Int32(System.Object)
    public static void Subject_3()
    {
        try { var _ = GC.GetGeneration(null!); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static void Subject_4()
    {
        try { var _ = (int)(((Array)(Array.CreateInstance(typeof(byte), 42))).Length); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)
    public static void Subject_5()
    {
        try { var _ = (int)(GC.GetTotalMemory(true)); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.GC::GetGCMemoryInfo: System.GCMemoryInfo()
    public static void Subject_6()
    {
        try { var _ = (long)((GC.GetGCMemoryInfo()).TotalCommittedBytes); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.GC::Collect(System.Int32,System.GCCollectionMode):System.Void()
    public static void Subject_7()
    {
        try { GC.Collect(42, default); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.GC::CollectionCount(System.Int32):System.Int64()
    public static void Subject_8()
    {
        try { var _ = (int)(GC.CollectionCount(42)); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.GC::GetTotalPauseDuration:System.TimeSpan()
    public static void Subject_9()
    {
        try { var _ = (long)((GC.GetTotalPauseDuration()).Ticks); }
        catch { _exitCode = 1; }
    }

}