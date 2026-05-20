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
        if (GC.GetGeneration(42) != 0) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static void Subject_4()
    {
        if (((Array.CreateInstance(typeof(byte), 42)).GetHashCode()) != 35342034) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)
    public static void Subject_5()
    {
        if ((int)(GC.GetTotalMemory(true)) != 52816) _exitCode = 1;
    }

}