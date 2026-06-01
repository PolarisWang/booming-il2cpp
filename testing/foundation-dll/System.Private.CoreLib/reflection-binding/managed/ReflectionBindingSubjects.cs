// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/binding
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionBindingSubjects
{
    // [0] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)
    [Fact]
    public static void Subject_0()
    {
        try { var __val = ((typeof(byte).GetMethod("hello")).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((typeof(byte).GetMethod("hello", System.Reflection.BindingFlags.Default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])
    [Fact]
    public static void Subject_2()
    {
        try { var __val = ((typeof(byte).GetMethod("hello", Array.Empty<System.Type>())).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [3] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((typeof(byte).GetField("hello")).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)
    [Fact]
    public static void Subject_4()
    {
        try { var __val = ((typeof(byte).GetField("hello", System.Reflection.BindingFlags.Default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)
    [Fact]
    public static void Subject_5()
    {
        try { var __val = ((typeof(byte).GetProperty("hello")).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)
    [Fact]
    public static void Subject_6()
    {
        try { var __val = ((typeof(byte).GetProperty("hello", System.Reflection.BindingFlags.Default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((typeof(byte).GetProperty("hello", Array.Empty<System.Type>())).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)
    [Fact]
    public static void Subject_8()
    {
        try { var __val = ((typeof(byte).GetEvent("hello")).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)
    [Fact]
    public static void Subject_9()
    {
        try { var __val = ((typeof(byte).GetEvent("hello", System.Reflection.BindingFlags.Default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    // [11] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    [Fact]
    public static void Subject_11()
    {
        try { var __val = ((default(MethodInfo)!.Invoke(null!, System.Reflection.BindingFlags.Default, default, Array.Empty<System.Object>(), default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}