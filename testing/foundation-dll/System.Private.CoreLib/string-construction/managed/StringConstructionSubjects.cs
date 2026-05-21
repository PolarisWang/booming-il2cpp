// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/string/construction
// Assembly: System.Private.CoreLib
// Variant: subjects

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
        try { if (((String.IsNullOrEmpty("hello")) ? 1 : 0) != ((String.IsNullOrEmpty("hello")) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.String::IsNullOrWhiteSpace:System.Boolean(System.String)
    public static void Subject_4()
    {
        try { if (((String.IsNullOrWhiteSpace("hello")) ? 1 : 0) != ((String.IsNullOrWhiteSpace("hello")) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.String::Copy:System.String(System.String)
    public static void Subject_5()
    {
        try { if (((String.Copy("hello")).Length) != ((String.Copy("hello")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.String::Intern:System.String(System.String)
    public static void Subject_6()
    {
        try { if (((String.Intern("hello")).Length) != ((String.Intern("hello")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.String::IsInterned:System.String(System.String)
    public static void Subject_7()
    {
        try { if (((String.IsInterned("hello")).Length) != ((String.IsInterned("hello")).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}