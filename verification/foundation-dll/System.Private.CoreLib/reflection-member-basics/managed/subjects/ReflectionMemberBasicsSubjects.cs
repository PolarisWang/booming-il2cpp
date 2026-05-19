// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/member/basics
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Reflection;

public static partial class ReflectionMemberBasicsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()
    public static void Subject_0()
    {
        try { _ = default(MemberInfo)!.Name; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()
    public static void Subject_1()
    {
        if ((int)(typeof(byte).MemberType) != 32) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()
    public static void Subject_2()
    {
        try { _ = default(MemberInfo)!.DeclaringType; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    public static void Subject_3()
    {
        try { typeof(byte).GetMethods()[0].Invoke(null, new object[0]); _exitCode = 1; }
        catch (TargetException) { }
    }

    // [4] System.Private.CoreLib/System.Reflection.MethodInfo::GetParameters:System.Reflection.ParameterInfo[]()
    public static void Subject_4()
    {
        if (((typeof(byte).GetMethods()[0].GetParameters()).Length) != 1) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()
    public static void Subject_5()
    {
        if (((typeof(byte).GetMethods()[0].ReturnType).GetHashCode()) != 35342034) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
    public static void Subject_6()
    {
        try { typeof(byte).GetConstructors()[0].Invoke(new object[0]); _exitCode = 1; }
        catch (IndexOutOfRangeException) { }
    }

    // [7] System.Private.CoreLib/System.Reflection.FieldInfo::GetValue:System.Object(System.Object)
    public static void Subject_7()
    {
        if (((typeof(byte).GetFields()[0].GetValue(null)).GetHashCode()) != 255) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Reflection.FieldInfo::SetValue:System.Void(System.Object,System.Object)
    public static void Subject_8()
    {
        try { typeof(byte).GetFields()[0].SetValue(null, (byte)42); _exitCode = 1; }
        catch (FieldAccessException) { }
    }

    // [9] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()
    public static void Subject_9()
    {
        if (((typeof(byte).GetFields()[0].FieldType).GetHashCode()) != 56793269) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object)
    public static void Subject_10()
    {
        try { typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].GetValue(null); _exitCode = 1; }
        catch (IndexOutOfRangeException) { }
    }

    // [11] System.Private.CoreLib/System.Reflection.PropertyInfo::GetValue:System.Object(System.Object,System.Object[])
    public static void Subject_11()
    {
        try { typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].GetValue(null, null); _exitCode = 1; }
        catch (IndexOutOfRangeException) { }
    }

    // [12] System.Private.CoreLib/System.Reflection.PropertyInfo::SetValue:System.Void(System.Object,System.Object)
    public static void Subject_12()
    {
        try { typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].SetValue(null, (byte)42); _exitCode = 1; }
        catch (IndexOutOfRangeException) { }
    }

    // [13] System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()
    public static void Subject_13()
    {
        try { _ = typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].PropertyType; _exitCode = 1; }
        catch (IndexOutOfRangeException) { }
    }

}