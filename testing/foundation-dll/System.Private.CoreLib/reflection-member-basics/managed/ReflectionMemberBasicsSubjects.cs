// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/member-basics
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

public static partial class ReflectionMemberBasicsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()
    public static void Subject_0()
    {
        try { if (((default(MemberInfo)!.Name).Length) != ((default(MemberInfo)!.Name).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()
    public static void Subject_1()
    {
        try { if ((int)(default(MemberInfo)!.MemberType) != (int)(default(MemberInfo)!.MemberType)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()
    public static void Subject_2()
    {
        try { if (((default(MemberInfo)!.DeclaringType).GetHashCode()) != ((default(MemberInfo)!.DeclaringType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    public static void Subject_3()
    {
        // TODO: System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[]) could not be auto-generated
    }

    // [4] System.Private.CoreLib/System.Reflection.MethodInfo::GetParameters:System.Reflection.ParameterInfo[]()
    public static void Subject_4()
    {
        // TODO: System.Private.CoreLib/System.Reflection.MethodInfo::GetParameters:System.Reflection.ParameterInfo[]() could not be auto-generated
    }

    // [5] System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()
    public static void Subject_5()
    {
        try { if (((default(MethodInfo)!.ReturnType).GetHashCode()) != ((default(MethodInfo)!.ReturnType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
    public static void Subject_6()
    {
        try { if (((typeof(byte).GetConstructors()[0].Invoke(new object[0])).GetHashCode()) != ((typeof(byte).GetConstructors()[0].Invoke(new object[0])).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)
    public static void Subject_7()
    {
        try { if (((typeof(byte).GetFields()[0].GetValue(null)).GetHashCode()) != ((typeof(byte).GetFields()[0].GetValue(null)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)
    public static void Subject_8()
    {
        try { typeof(byte).GetFields()[0].SetValue(null, (byte)42); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()
    public static void Subject_9()
    {
        try { if (((0).GetHashCode()) != ((0).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object)
    public static void Subject_10()
    {
        try { if (((typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].GetValue(null)).GetHashCode()) != ((typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].GetValue(null)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object,System.Object[])
    public static void Subject_11()
    {
        try { if (((typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].GetValue(null, null)).GetHashCode()) != ((typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].GetValue(null, null)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object)
    public static void Subject_12()
    {
        try { typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].SetValue(null, (byte)42); }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()
    public static void Subject_13()
    {
        try { if (((0).GetHashCode()) != ((0).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}