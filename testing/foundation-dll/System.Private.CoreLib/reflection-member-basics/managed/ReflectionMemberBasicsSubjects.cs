// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/member-basics
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionMemberBasicsSubjects
{
    // [0] System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()
    // [1] System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()
    // [2] System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()
    // [3] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    // [4] System.Private.CoreLib/System.Reflection.MethodInfo::GetParameters:System.Reflection.ParameterInfo[]()
    // [5] System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()
    // [6] System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
    [Fact]
    public static void Subject_6()
    {
        try { var __val = ((typeof(byte).GetConstructors()[0].Invoke(new object[0])).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [7] System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((typeof(byte).GetFields()[0].GetValue(null)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)
    [Fact]
    public static void Subject_8()
    {
        try { typeof(byte).GetFields()[0].SetValue(null, (byte)42); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()
    [Fact]
    public static void Subject_9()
    {
        try { var __val = ((0).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object)
    [Fact]
    public static void Subject_10()
    {
        try { var __val = ((typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].GetValue(null)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [11] System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object,System.Object[])
    [Fact]
    public static void Subject_11()
    {
        try { var __val = ((typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].GetValue(null, null)).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [12] System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object)
    [Fact]
    public static void Subject_12()
    {
        try { typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].SetValue(null, (byte)42); }
        catch { }
    }

    // [13] System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()
    [Fact]
    public static void Subject_13()
    {
        try { var __val = ((0).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

}