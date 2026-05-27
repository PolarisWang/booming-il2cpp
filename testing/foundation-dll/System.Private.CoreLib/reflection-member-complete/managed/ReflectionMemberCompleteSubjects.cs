// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/member-complete
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

public static partial class ReflectionMemberCompleteSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()
    public static void Subject_0()
    {
        try { var _ = ((default(MemberInfo)!.Name).Length); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()
    public static void Subject_1()
    {
        try { var _ = ((default(MemberInfo)!.MemberType).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()
    public static void Subject_2()
    {
        try { var _ = ((default(MemberInfo)!.DeclaringType).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()
    public static void Subject_3()
    {
        try { var _ = ((default(MemberInfo)!.ReflectedType).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()
    public static void Subject_4()
    {
        try { var _ = default(MemberInfo)!.MetadataToken; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()
    public static void Subject_5()
    {
        try { var _ = ((default(MethodInfo)!.ReturnType).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()
    public static void Subject_6()
    {
        try { var _ = ((default(MethodInfo)!.GetBaseDefinition()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()
    public static void Subject_7()
    {
        try { var _ = ((default(MethodInfo)!.MethodHandle).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()
    public static void Subject_8()
    {
        try { var _ = ((default(MethodInfo)!.CallingConvention).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()
    public static void Subject_9()
    {
        try { var _ = ((default(MethodInfo)!.IsPublic) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()
    public static void Subject_10()
    {
        try { var _ = ((default(MethodInfo)!.IsStatic) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()
    public static void Subject_11()
    {
        try { var _ = ((default(MethodInfo)!.IsVirtual) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()
    public static void Subject_12()
    {
        try { var _ = ((0).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()
    public static void Subject_13()
    {
        try { var _ = ((default(FieldInfo)!.IsStatic) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()
    public static void Subject_14()
    {
        try { var _ = ((default(FieldInfo)!.IsInitOnly) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [15] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()
    public static void Subject_15()
    {
        try { var _ = ((default(FieldInfo)!.IsLiteral) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [16] System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()
    public static void Subject_16()
    {
        try { var _ = ((0).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [17] System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()
    public static void Subject_17()
    {
        try { var _ = ((default(PropertyInfo)!.CanRead) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [18] System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()
    public static void Subject_18()
    {
        try { var _ = ((default(PropertyInfo)!.CanWrite) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [19] System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()
    public static void Subject_19()
    {
        try { var _ = ((default(PropertyInfo)!.GetIndexParameters()).Length); }
        catch { _exitCode = 1; }
    }

    // [20] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()
    public static void Subject_20()
    {
        try { var _ = ((default(FieldInfo)!.FieldHandle).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [21] System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions()
    public static void Subject_21()
    {
        try { var _ = ((default(ConstructorInfo)!.CallingConvention).GetHashCode()); }
        catch { _exitCode = 1; }
    }

}