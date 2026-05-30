// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/member/complete
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionMemberCompleteSubjects
{
    // [0] System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()
    // [1] System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()
    // [2] System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()
    // [3] System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()
    // [4] System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()
    // [5] System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()
    // [6] System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()
    // [7] System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()
    [Fact]
    public static void Subject_7()
    {
        try { var __val = ((default(MethodInfo)!.MethodHandle).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [8] System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()
    [Fact]
    public static void Subject_8()
    {
        try { var __val = ((default(MethodInfo)!.CallingConvention).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [9] System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()
    [Fact]
    public static void Subject_9()
    {
        try { var __val = ((default(MethodInfo)!.IsPublic) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [10] System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()
    [Fact]
    public static void Subject_10()
    {
        try { var __val = ((default(MethodInfo)!.IsStatic) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [11] System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()
    [Fact]
    public static void Subject_11()
    {
        try { var __val = ((default(MethodInfo)!.IsVirtual) ? 1 : 0); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [12] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()
    [Fact]
    public static void Subject_12()
    {
        try { var __val = ((0).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [13] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()
    // [14] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()
    // [15] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()
    // [16] System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()
    [Fact]
    public static void Subject_16()
    {
        try { var __val = ((0).GetHashCode()); Assert.IsNotNull(__val.GetHashCode()); }
        catch { }
    }

    // [17] System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()
    // [18] System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()
    // [19] System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()
    // [20] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()
    // [21] System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions()
}