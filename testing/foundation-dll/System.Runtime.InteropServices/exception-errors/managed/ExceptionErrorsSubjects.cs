// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/exception-errors
// Assembly: System.Runtime.InteropServices
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

public static partial class ExceptionErrorsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::ToString:System.String()
    public static void Subject_0()
    {
        try { if (((default(COMException)!.ToString()).Length) != ((default(COMException)!.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::.ctor:System.Void()
    public static void Subject_1()
    {
        try { new COMException(); }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::.ctor:System.Void(System.String)
    public static void Subject_2()
    {
        try { new COMException("hello"); }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::.ctor:System.Void(System.String,System.Exception)
    public static void Subject_3()
    {
        try { new COMException(null, default(int)); }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::.ctor:System.Void(System.String,System.Int32)
    public static void Subject_4()
    {
        try { new COMException(null, default(int)); }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::get_ErrorCode:System.Int32()
    public static void Subject_5()
    {
        try { if (default(ExternalException)!.ErrorCode != default(ExternalException)!.ErrorCode) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::ToString:System.String()
    public static void Subject_6()
    {
        try { if (((default(ExternalException)!.ToString()).Length) != ((default(ExternalException)!.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::.ctor:System.Void()
    public static void Subject_7()
    {
        try { new ExternalException(); }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::.ctor:System.Void(System.String)
    public static void Subject_8()
    {
        try { new ExternalException("hello"); }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::.ctor:System.Void(System.String,System.Exception)
    public static void Subject_9()
    {
        try { new ExternalException(null, default(int)); }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::.ctor:System.Void(System.String,System.Int32)
    public static void Subject_10()
    {
        try { new ExternalException(null, default(int)); }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::CanResume:System.Boolean()
    public static void Subject_11()
    {
        try { if (((default(SEHException)!.CanResume()) ? 1 : 0) != ((default(SEHException)!.CanResume()) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::.ctor:System.Void()
    public static void Subject_12()
    {
        try { new SEHException(); }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::.ctor:System.Void(System.String)
    public static void Subject_13()
    {
        try { new SEHException("hello"); }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::.ctor:System.Void(System.String,System.Exception)
    public static void Subject_14()
    {
        try { new SEHException("hello", default); }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.InteropServices/System.DllNotFoundException::.ctor:System.Void()
    public static void Subject_15()
    {
        try { new DllNotFoundException(); }
        catch { _exitCode = 1; }
    }

    // [16] System.Runtime.InteropServices/System.DllNotFoundException::.ctor:System.Void(System.String)
    public static void Subject_16()
    {
        try { new DllNotFoundException("hello"); }
        catch { _exitCode = 1; }
    }

    // [17] System.Runtime.InteropServices/System.DllNotFoundException::.ctor:System.Void(System.String,System.Exception)
    public static void Subject_17()
    {
        try { new DllNotFoundException("hello", default); }
        catch { _exitCode = 1; }
    }

}