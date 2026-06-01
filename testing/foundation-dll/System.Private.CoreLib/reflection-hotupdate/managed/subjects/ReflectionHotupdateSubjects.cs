// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/hotupdate
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionHotupdateSubjects
{
    // [0] System.Private.CoreLib/System.Reflection.Assembly::GetModules:System.Reflection.Module[]()
    [Fact]
    public static void Subject_0()
    {
        try { var __val = ((typeof(byte).Assembly.GetModules()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Reflection.Assembly::GetTypes:System.Type[]()
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((typeof(byte).Assembly.GetTypes()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((Type.GetTypeFromHandle(default(System.RuntimeTypeHandle))).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Reflection.Assembly::GetType:System.Type(System.String)
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((typeof(byte).Assembly.GetType("hello")).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}