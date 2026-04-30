// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/exception/throw/diagnostics
// Assembly: System.Private.CoreLib
// Variant: semantic-patch

using System;

public static class ExceptionThrowDiagnosticsSemanticPatchEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            case 10: return Method10();
            case 11: return Method11();
            case 12: return Method12();
            case 13: return Method13();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)
    static int Method0()
    {
        new Exception("99");
        return (int)0;
    }

    // [1] System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)
    static int Method1()
    {
        new Exception("99", null!);
        return (int)0;
    }

    // [2] System.Private.CoreLib/System.Exception::get_Message:System.String()
    static int Method2()
    {
        return (new Exception("test").Message?.GetHashCode() ?? 0);
    }

    // [3] System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()
    static int Method3()
    {
        return new Exception("test").InnerException.GetHashCode();
    }

    // [4] System.Private.CoreLib/System.Exception::get_StackTrace:System.String()
    static int Method4()
    {
        // TODO: System.Private.CoreLib/System.Exception::get_StackTrace:System.String() could not be auto-generated for semantic-patch
        return 0;
    }

    // [5] System.Private.CoreLib/System.Exception::get_HResult:System.Int32()
    static int Method5()
    {
        return new Exception("test").HResult;
    }

    // [6] System.Private.CoreLib/System.Exception::ToString:System.String()
    static int Method6()
    {
        return (new Exception("test").ToString()?.GetHashCode() ?? 0);
    }

    // [7] System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()
    static int Method7()
    {
        return new Exception("test").GetBaseException().GetHashCode();
    }

    // [8] System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)
    static int Method8()
    {
        new ArgumentException("99", "99");
        return (int)0;
    }

    // [9] System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)
    static int Method9()
    {
        new ArgumentNullException("99");
        return (int)0;
    }

    // [10] System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)
    static int Method10()
    {
        new ArgumentOutOfRangeException("99");
        return (int)0;
    }

    // [11] System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)
    static int Method11()
    {
        new InvalidOperationException("99");
        return (int)0;
    }

    // [12] System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)
    static int Method12()
    {
        new NotSupportedException("99");
        return (int)0;
    }

    // [13] System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)
    static int Method13()
    {
        new NotImplementedException("99");
        return (int)0;
    }

}