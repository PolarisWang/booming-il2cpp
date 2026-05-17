// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/garbage/collection
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

// GCMemoryInfoData struct — Sequential layout matching native GcMemoryInfoNative.
// Used by Subject_6 to verify chaos_gc_get_memory_info produces valid data.
[System.Runtime.InteropServices.StructLayout(
    System.Runtime.InteropServices.LayoutKind.Sequential)]
public struct GCMemoryInfoData
{
    public long highMemoryLoadThresholdBytes;
    public long memoryLoadBytes;
    public long totalAvailableMemoryBytes;
    public long heapSizeBytes;
    public long fragmentedBytes;
    public long totalCommittedBytes;
    public long promotedBytes;
    public int generation;
    public int finalizationPendingCount;
    public int compacted;
    public int concurrent;
}

public static partial class GarbageCollectionSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // P/Invoke to native chaos_gc_get_memory_info
    [System.Runtime.InteropServices.DllImport("__Internal",
        EntryPoint = "chaos_gc_get_memory_info")]
    static extern void GetNativeMemoryInfo(out GCMemoryInfoData info);

    // [0] System.Private.CoreLib/System.GC::Collect:System.Void()
    public static void Subject_0()
    {
        GC.Collect();
    }

    // [1] System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()
    public static void Subject_1()
    {
        GC.WaitForPendingFinalizers();
    }

    // [2] System.Private.CoreLib/System.GC::KeepAlive:System.Void(System.Object)
    public static void Subject_2()
    {
        GC.KeepAlive(42);
    }

    // [3] System.Private.CoreLib/System.GC::GetGeneration:System.Int32(System.Object)
    public static void Subject_3()
    {
        if (GC.GetGeneration(42) != GC.GetGeneration(42)) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static void Subject_4()
    {
        if (((Array.CreateInstance(typeof(byte), 42)).GetHashCode()) != ((Array.CreateInstance(typeof(byte), 42)).GetHashCode())) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)
    public static void Subject_5()
    {
        if ((int)(GC.GetTotalMemory(true)) != (int)(GC.GetTotalMemory(true))) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.GC::GetMemoryInfo → chaos_gc_get_memory_info
    public static void Subject_6()
    {
        GetNativeMemoryInfo(out var info);

        // Verify basic properties are populated.
        if (info.heapSizeBytes <= 0) _exitCode = 1;
        if (info.memoryLoadBytes <= 0) _exitCode = 2;
        if (info.totalAvailableMemoryBytes <= 0) _exitCode = 3;
        if (info.generation < 0) _exitCode = 4;
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
            case 4: Subject_4(); break;
            case 5: Subject_5(); break;
            case 6: Subject_6(); break;
        }
    }

}
