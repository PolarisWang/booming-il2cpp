// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/exception/throw/diagnostics
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;

public static partial class ExceptionThrowDiagnosticsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)
    public static void Subject_0()
    {
        try { new Exception("hello"); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)
    public static void Subject_1()
    {
        try { new Exception("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Exception::get_Message:System.String()
    public static void Subject_2()
    {
        try { if (((new Exception().Message).Length) != ((new Exception().Message).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()
    public static void Subject_3()
    {
        try { if (((new Exception().InnerException).GetHashCode()) != ((new Exception().InnerException).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Exception::get_StackTrace:System.String()
    public static void Subject_4()
    {
        try { if (((((new Exception().StackTrace) ?? "")).Length) != ((((new Exception().StackTrace) ?? "")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Exception::get_HResult:System.Int32()
    public static void Subject_5()
    {
        try { if (new Exception().HResult != new Exception().HResult) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Exception::ToString:System.String()
    public static void Subject_6()
    {
        try { if (((new Exception().ToString()).Length) != ((new Exception().ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()
    public static void Subject_7()
    {
        try { if (((new Exception().GetBaseException()).GetHashCode()) != ((new Exception().GetBaseException()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)
    public static void Subject_8()
    {
        try { new ArgumentException("hello", "hello"); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)
    public static void Subject_9()
    {
        try { new ArgumentNullException("hello"); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)
    public static void Subject_10()
    {
        try { new ArgumentOutOfRangeException("hello"); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)
    public static void Subject_11()
    {
        try { new InvalidOperationException("hello"); }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)
    public static void Subject_12()
    {
        try { new NotSupportedException("hello"); }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)
    public static void Subject_13()
    {
        try { new NotImplementedException("hello"); }
        catch { _exitCode = 1; }
    }

}