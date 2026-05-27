// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/string/construction
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class StringConstructionSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.String::.ctor:System.Void(System.Char[])
    public static void Subject_0()
    {
        try { new String(Array.Empty<System.Char>()); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.String::.ctor:System.Void(System.Char,System.Int32)
    public static void Subject_1()
    {
        try { new String('A', 42); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.String::.ctor:System.Void(System.Char[],System.Int32,System.Int32)
    public static void Subject_2()
    {
        try { new String(Array.Empty<System.Char>(), 42, 42); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.String::IsNullOrEmpty:System.Boolean(System.String)
    public static void Subject_3()
    {
        try { var _ = ((String.IsNullOrEmpty("hello")) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.String::IsNullOrWhiteSpace:System.Boolean(System.String)
    public static void Subject_4()
    {
        try { var _ = ((String.IsNullOrWhiteSpace("hello")) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.String::Copy:System.String(System.String)
    public static void Subject_5()
    {
        try { var _ = ((String.Copy("hello")).Length); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.String::Intern:System.String(System.String)
    public static void Subject_6()
    {
        try { var _ = ((String.Intern("hello")).Length); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.String::IsInterned:System.String(System.String)
    public static void Subject_7()
    {
        try { var _ = ((String.IsInterned("hello")).Length); }
        catch { _exitCode = 1; }
    }

}