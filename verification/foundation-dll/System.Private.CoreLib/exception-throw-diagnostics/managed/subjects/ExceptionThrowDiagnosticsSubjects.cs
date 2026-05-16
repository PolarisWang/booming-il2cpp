// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/exception/throw/diagnostics
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class ExceptionThrowDiagnosticsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)
    public static void Subject_0()
    {
        new Exception("hello");
    }

    // [1] System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)
    public static void Subject_1()
    {
        new Exception("hello", null!);
    }

    // [2] System.Private.CoreLib/System.Exception::get_Message:System.String()
    public static void Subject_2()
    {
        if (((new Exception().Message).Length) != ((new Exception().Message).Length)) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()
    public static void Subject_3()
    {
        if (((new Exception().InnerException).GetHashCode()) != ((new Exception().InnerException).GetHashCode())) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Exception::get_StackTrace:System.String()
    public static void Subject_4()
    {
        // TODO: System.Private.CoreLib/System.Exception::get_StackTrace:System.String() could not be auto-generated
    }

    // [5] System.Private.CoreLib/System.Exception::get_HResult:System.Int32()
    public static void Subject_5()
    {
        if (new Exception().HResult != new Exception().HResult) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Exception::ToString:System.String()
    public static void Subject_6()
    {
        if (((new Exception().ToString()).Length) != ((new Exception().ToString()).Length)) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()
    public static void Subject_7()
    {
        if (((new Exception().GetBaseException()).GetHashCode()) != ((new Exception().GetBaseException()).GetHashCode())) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)
    public static void Subject_8()
    {
        new ArgumentException("hello", "hello");
    }

    // [9] System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)
    public static void Subject_9()
    {
        new ArgumentNullException("hello");
    }

    // [10] System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)
    public static void Subject_10()
    {
        new ArgumentOutOfRangeException("hello");
    }

    // [11] System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)
    public static void Subject_11()
    {
        new InvalidOperationException("hello");
    }

    // [12] System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)
    public static void Subject_12()
    {
        new NotSupportedException("hello");
    }

    // [13] System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)
    public static void Subject_13()
    {
        new NotImplementedException("hello");
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
            case 4: Subject_4(); break;
            case 5: Subject_5(); break;
            case 6: Subject_6(); break;
            case 7: Subject_7(); break;
            case 8: Subject_8(); break;
            case 9: Subject_9(); break;
            case 10: Subject_10(); break;
            case 11: Subject_11(); break;
            case 12: Subject_12(); break;
            case 13: Subject_13(); break;
        }
    }

}