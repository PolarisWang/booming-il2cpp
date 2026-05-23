// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/event
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

public static partial class ReflectionEventSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()
    public static void Subject_0()
    {
        try { if (((typeof(byte).GetEvents()).Length) != ((typeof(byte).GetEvents()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[](System.Reflection.BindingFlags)
    public static void Subject_1()
    {
        try { if (((typeof(byte).GetEvents(System.Reflection.BindingFlags.Default)).Length) != ((typeof(byte).GetEvents(System.Reflection.BindingFlags.Default)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)
    public static void Subject_2()
    {
        try { if (((typeof(byte).GetEvent("hello")).GetHashCode()) != ((typeof(byte).GetEvent("hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}