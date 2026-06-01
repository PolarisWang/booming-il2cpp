// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/exception/throw-diagnostics
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class ExceptionThrowDiagnosticsSubjects
{
    // [0] System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)
    [Fact]
    public static void Subject_0()
    {
        try { new Exception("hello"); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)
    [Fact]
    public static void Subject_1()
    {
        try { new Exception("hello", default); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Exception::get_Message:System.String()
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((new Exception().Message).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((new Exception().InnerException).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Exception::get_StackTrace:System.String()
    [Fact]
    public static void Subject_4()
    {
        try { var __val = ((((new Exception().StackTrace) ?? "")).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Exception::get_HResult:System.Int32()
    [Fact]
    public static void Subject_5()
    {
        try { var __val = new Exception().HResult; Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.Exception::ToString:System.String()
    [Fact]
    public static void Subject_6()
    {
        try { var __val = ((new Exception().ToString()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((new Exception().GetBaseException()).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)
    [Fact]
    public static void Subject_8()
    {
        try { new ArgumentException("hello", "hello"); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)
    [Fact]
    public static void Subject_9()
    {
        try { new ArgumentNullException("hello"); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)
    [Fact]
    public static void Subject_10()
    {
        try { new ArgumentOutOfRangeException("hello"); }
        catch { }
    }

    // [11] System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)
    [Fact]
    public static void Subject_11()
    {
        try { new InvalidOperationException("hello"); }
        catch { }
    }

    // [12] System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)
    [Fact]
    public static void Subject_12()
    {
        try { new NotSupportedException("hello"); }
        catch { }
    }

    // [13] System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)
    [Fact]
    public static void Subject_13()
    {
        try { new NotImplementedException("hello"); }
        catch { }
    }

}