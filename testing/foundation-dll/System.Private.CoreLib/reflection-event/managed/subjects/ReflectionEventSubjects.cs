// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/event
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionEventSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()
    public static void Subject_0()
    {
        try { _ = ((typeof(byte).GetEvents()).Length); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[](System.Reflection.BindingFlags)
    public static void Subject_1()
    {
        try { _ = ((typeof(byte).GetEvents(System.Reflection.BindingFlags.Default)).Length); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)
    public static void Subject_2()
    {
        try { _ = ((typeof(byte).GetEvent("hello")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

}