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
        try { if (GC.GetGeneration(null!) != GC.GetGeneration(null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static void Subject_4()
    {
        try { if ((int)(((Array)(Array.CreateInstance(typeof(byte), 42))).Length) != (int)(((Array)(Array.CreateInstance(typeof(byte), 42))).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)
    public static void Subject_5()
    {
        try { if ((int)(GC.GetTotalMemory(true)) != (int)(GC.GetTotalMemory(true))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.GC::GetGCMemoryInfo: System.GCMemoryInfo()
    public static void Subject_6()
    {
        try { if ((long)((GC.GetGCMemoryInfo()).TotalCommittedBytes) != (long)((GC.GetGCMemoryInfo()).TotalCommittedBytes)) _exitCode = 1; }
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
        try { if ((int)(GC.CollectionCount(42)) != (int)(GC.CollectionCount(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.GC::GetTotalPauseDuration:System.TimeSpan()
    public static void Subject_9()
    {
        try { if ((long)((GC.GetTotalPauseDuration()).Ticks) != (long)((GC.GetTotalPauseDuration()).Ticks)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}