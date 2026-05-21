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
        try { GC.KeepAlive(42); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.GC::GetGeneration:System.Int32(System.Object)
    public static void Subject_3()
    {
        try { if (GC.GetGeneration(42) != GC.GetGeneration(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static void Subject_4()
    {
        try { if (((Array.CreateInstance(typeof(byte), 42)).GetHashCode()) != ((Array.CreateInstance(typeof(byte), 42)).GetHashCode())) _exitCode = 1; }
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
        try
        {
            var info = GC.GetGCMemoryInfo();
            if (info.HighMemoryLoadThresholdBytes <= 0) _exitCode = 1;
            if (info.GenerationInfo.Length != 5) _exitCode = 1;
        }
        catch { _exitCode = 1; }
    }

}