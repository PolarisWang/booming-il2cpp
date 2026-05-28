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
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Environment::get_TickCount:System.Int32()
    public static void Subject_0()
    {
        try { _ = Environment.TickCount; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
    public static void Subject_1()
    {
        try { Thread.Sleep(42); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)
    public static void Subject_2()
    {
        try { _ = (int)(GC.GetTotalMemory(true)); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Runtime.InteropServices.Marshal::GetLastPInvokeError:System.Int32()
    public static void Subject_3()
    {
        try { _ = Marshal.GetLastPInvokeError(); }
        catch { _exitCode = 1; }
    }

}