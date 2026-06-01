// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/pinvoke/stress
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;

public static partial class PinvokeStressSubjects
{
    // [0] System.Private.CoreLib/System.Environment::get_TickCount:System.Int32()
    // [1] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
    [Fact]
    public static void Subject_1()
    {
        try { Thread.Sleep(42); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)
    [Fact]
    public static void Subject_2()
    {
        try { var __val = (int)(GC.GetTotalMemory(true)); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Runtime.InteropServices.Marshal::GetLastPInvokeError:System.Int32()
    [Fact]
    public static void Subject_3()
    {
        try { var __val = Marshal.GetLastPInvokeError(); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}