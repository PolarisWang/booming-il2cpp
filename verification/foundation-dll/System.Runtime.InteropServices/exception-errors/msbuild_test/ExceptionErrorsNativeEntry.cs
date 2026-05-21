// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/exception-errors
// Assembly: System.Runtime.InteropServices
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

public static partial class ExceptionErrorsNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::ToString:System.String()
    public static void Method0()
    {
        try { if (((COMException.ToString()).Length) != ((COMException.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::.ctor:System.Void()
    public static void Method1()
    {
        try { new COMException(); }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::.ctor:System.Void(System.String)
    public static void Method2()
    {
        try { new COMException("hello"); }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::.ctor:System.Void(System.String,System.Exception)
    public static void Method3()
    {
        try { new COMException("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::.ctor:System.Void(System.String,System.Int32)
    public static void Method4()
    {
        try { new COMException("hello", 42); }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::get_ErrorCode:System.Int32()
    public static void Method5()
    {
        try { if (ExternalException.ErrorCode != ExternalException.ErrorCode) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::ToString:System.String()
    public static void Method6()
    {
        try { if (((ExternalException.ToString()).Length) != ((ExternalException.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::.ctor:System.Void()
    public static void Method7()
    {
        try { new ExternalException(); }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::.ctor:System.Void(System.String)
    public static void Method8()
    {
        try { new ExternalException("hello"); }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::.ctor:System.Void(System.String,System.Exception)
    public static void Method9()
    {
        try { new ExternalException("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.ExternalException::.ctor:System.Void(System.String,System.Int32)
    public static void Method10()
    {
        try { new ExternalException("hello", 42); }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::CanResume:System.Boolean()
    public static void Method11()
    {
        try { if (((SEHException.CanResume()) ? 1 : 0) != ((SEHException.CanResume()) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::.ctor:System.Void()
    public static void Method12()
    {
        try { new SEHException(); }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::.ctor:System.Void(System.String)
    public static void Method13()
    {
        try { new SEHException("hello"); }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::.ctor:System.Void(System.String,System.Exception)
    public static void Method14()
    {
        try { new SEHException("hello", null!); }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.InteropServices/System.DllNotFoundException::.ctor:System.Void()
    public static void Method15()
    {
        try { new DllNotFoundException(); }
        catch { _exitCode = 1; }
    }

    // [16] System.Runtime.InteropServices/System.DllNotFoundException::.ctor:System.Void(System.String)
    public static void Method16()
    {
        try { new DllNotFoundException("hello"); }
        catch { _exitCode = 1; }
    }

    // [17] System.Runtime.InteropServices/System.DllNotFoundException::.ctor:System.Void(System.String,System.Exception)
    public static void Method17()
    {
        try { new DllNotFoundException("hello", null!); }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
                case 8: Method8(); break;
                case 9: Method9(); break;
                case 10: Method10(); break;
                case 11: Method11(); break;
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
                case 16: Method16(); break;
                case 17: Method17(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}