// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/exception/errors
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
        try { default(COMException)!.ToString(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::.ctor:System.Void()
    public static void Subject_1()
    {
        new COMException();
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::.ctor:System.Void(System.String)
    public static void Subject_2()
    {
        new COMException("hello");
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::.ctor:System.Void(System.String,System.Exception)
    public static void Subject_3()
    {
        new COMException(null, default(int));
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::.ctor:System.Void(System.String,System.Int32)
    public static void Subject_4()
    {
        new COMException(null, default(int));
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::get_ErrorCode:System.Int32()
    public static void Subject_5()
    {
        try { _ = default(ExternalException)!.ErrorCode; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::ToString:System.String()
    public static void Subject_6()
    {
        try { default(ExternalException)!.ToString(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::.ctor:System.Void()
    public static void Subject_7()
    {
        new ExternalException();
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::.ctor:System.Void(System.String)
    public static void Subject_8()
    {
        new ExternalException("hello");
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::.ctor:System.Void(System.String,System.Exception)
    public static void Subject_9()
    {
        new ExternalException(null, default(int));
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::.ctor:System.Void(System.String,System.Int32)
    public static void Subject_10()
    {
        new ExternalException(null, default(int));
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::CanResume:System.Boolean()
    public static void Subject_11()
    {
        try { default(SEHException)!.CanResume(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::.ctor:System.Void()
    public static void Subject_12()
    {
        new SEHException();
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::.ctor:System.Void(System.String)
    public static void Subject_13()
    {
        new SEHException("hello");
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::.ctor:System.Void(System.String,System.Exception)
    public static void Subject_14()
    {
        new SEHException("hello", default);
    }

    // [15] System.Runtime.InteropServices/System.DllNotFoundException::.ctor:System.Void()
    public static void Subject_15()
    {
        new DllNotFoundException();
    }

    // [16] System.Runtime.InteropServices/System.DllNotFoundException::.ctor:System.Void(System.String)
    public static void Subject_16()
    {
        new DllNotFoundException("hello");
    }

    // [17] System.Runtime.InteropServices/System.DllNotFoundException::.ctor:System.Void(System.String,System.Exception)
    public static void Subject_17()
    {
        new DllNotFoundException("hello", default);
    }

}