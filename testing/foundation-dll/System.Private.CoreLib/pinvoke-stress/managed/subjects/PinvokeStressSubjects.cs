// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/pinvoke/stress
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
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
        try { if (Environment.TickCount != Environment.TickCount) _exitCode = 1; }
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
        try { if ((int)(GC.GetTotalMemory(true)) != (int)(GC.GetTotalMemory(true))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Runtime.InteropServices.Marshal::GetLastPInvokeError:System.Int32()
    public static void Subject_3()
    {
        try { if (Marshal.GetLastPInvokeError() != Marshal.GetLastPInvokeError()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}