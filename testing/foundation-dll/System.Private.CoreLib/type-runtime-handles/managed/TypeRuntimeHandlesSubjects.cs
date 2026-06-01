// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/type/runtime-handles
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class TypeRuntimeHandlesSubjects
{
    // [0] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    // [1] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)
    // [2] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)
    // [3] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    [Fact]
    public static void Subject_3()
    {
        try { var __val = ((Type.GetTypeFromHandle(default(System.RuntimeTypeHandle))).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [4] System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()
    [Fact]
    public static void Subject_4()
    {
        try { var __val = ((typeof(byte).IsValueType) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [5] System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()
    [Fact]
    public static void Subject_5()
    {
        try { var __val = ((typeof(byte).IsEnum) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [6] System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()
    [Fact]
    public static void Subject_6()
    {
        try { var __val = ((typeof(byte).IsArray) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((typeof(byte).TypeHandle).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.Type::get_FullName:System.String()
    [Fact]
    public static void Subject_8()
    {
        try { var __val = ((typeof(byte).FullName).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Type::get_Name:System.String()
    [Fact]
    public static void Subject_9()
    {
        try { var __val = ((typeof(byte).Name).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.Type::get_BaseType:System.Type()
    [Fact]
    public static void Subject_10()
    {
        try { var __val = ((typeof(byte).BaseType).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [11] System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()
    [Fact]
    public static void Subject_11()
    {
        try { var __val = ((typeof(byte).GetMethods()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [12] System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()
    [Fact]
    public static void Subject_12()
    {
        try { var __val = ((typeof(byte).GetFields()).Length); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}