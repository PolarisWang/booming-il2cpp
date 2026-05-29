// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/eh/stress
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class EhStressSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)
    public static void Subject_0()
    {
        try { _ = ((new int[1].GetValue(0)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)
    public static void Subject_1()
    {
        try { _ = Convert.ToInt32("42"); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)
    public static void Subject_2()
    {
        try { _ = int.Parse("42"); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)
}