// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/garbage-collection
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class GarbageCollectionSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.GC::Collect:System.Void()
    public static void CustomEntrySubject_0()
    {
    }

    // [1] System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()
    public static void CustomEntrySubject_1()
    {
    }

    // [2] System.Private.CoreLib/System.GC::KeepAlive:System.Void(System.Object)
    public static void CustomEntrySubject_2()
    {
    }

    // [3] System.Private.CoreLib/System.GC::GetGeneration:System.Int32(System.Object)
    public static void CustomEntrySubject_3()
    {
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static void Subject_4()
    {
        try { _ = (int)(((Array)(Array.CreateInstance(typeof(byte), 42))).Length); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)
    public static void CustomEntrySubject_5()
    {
    }

    // [6] System.Private.CoreLib/System.GC::GetGCMemoryInfo: System.GCMemoryInfo()
    public static void CustomEntrySubject_6()
    {
    }

}