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
        if (((new Exception().Message).Length) != 48) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()
    public static void Subject_3()
    {
        try { _ = new Exception().InnerException; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Private.CoreLib/System.Exception::get_StackTrace:System.String()
    public static void Subject_4()
    {
        // TODO: System.Private.CoreLib/System.Exception::get_StackTrace:System.String() could not be auto-generated
    }

    // [5] System.Private.CoreLib/System.Exception::get_HResult:System.Int32()
    public static void Subject_5()
    {
        if (new Exception().HResult != -2146233088) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Exception::ToString:System.String()
    public static void Subject_6()
    {
        if (((new Exception().ToString()).Length) != 66) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()
    public static void Subject_7()
    {
        if (((new Exception().GetBaseException()).GetHashCode()) != 35342034) _exitCode = 1;
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

}