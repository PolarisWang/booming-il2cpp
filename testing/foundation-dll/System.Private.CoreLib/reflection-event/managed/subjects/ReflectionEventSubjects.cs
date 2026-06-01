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
    // [0] System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()
    [Fact]
    public static void Subject_0()
    {
        try { var __val = ((typeof(byte).GetEvents()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[](System.Reflection.BindingFlags)
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((typeof(byte).GetEvents(System.Reflection.BindingFlags.Default)).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((typeof(byte).GetEvent("hello")).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}