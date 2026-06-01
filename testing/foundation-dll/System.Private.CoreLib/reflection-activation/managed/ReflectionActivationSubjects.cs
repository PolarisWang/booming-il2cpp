// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/activation
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

public static partial class ReflectionActivationSubjects
{
    // [0] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)
    [Fact]
    public static void Subject_0()
    {
        try { var __val = ((Activator.CreateInstance(typeof(byte))).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [1] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])
    [Fact]
    public static void Subject_1()
    {
        try { var __val = ((Activator.CreateInstance(typeof(byte), Array.Empty<System.Object>())).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [2] System.Private.CoreLib/System.Activator::CreateInstance:T()
    // [3] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((Activator.CreateInstance(typeof(byte), System.Reflection.BindingFlags.Default, default, Array.Empty<System.Object>(), default)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    [Fact]
    public static void Subject_4()
    {
        try { var __val = (int)(((Array)(Array.CreateInstance(typeof(byte), 42))).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    [Fact]
    public static void Subject_5()
    {
        try { var __val = (int)(((Array)(Array.CreateInstance(typeof(byte), 42, 42))).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])
    [Fact]
    public static void Subject_6()
    {
        try { var __val = (int)(((Array)(Array.CreateInstance(typeof(byte), Array.Empty<System.Int32>()))).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((typeof(byte).GetConstructors()[0].Invoke(new object[0])).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}