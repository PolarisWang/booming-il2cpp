// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/type
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionTypeSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    public static void CustomEntrySubject_0()
    {
    }

    // [1] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)
    public static void CustomEntrySubject_1()
    {
    }

    // [2] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)
    public static void CustomEntrySubject_2()
    {
    }

    // [3] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    public static void Subject_3()
    {
        try { _ = ((Type.GetTypeFromHandle(default(System.RuntimeTypeHandle))).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Type::get_Name:System.String()
    public static void Subject_4()
    {
        try { _ = ((typeof(byte).Name).Length); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Type::get_FullName:System.String()
    public static void Subject_5()
    {
        try { _ = ((typeof(byte).FullName).Length); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Type::get_Namespace:System.String()
    public static void Subject_6()
    {
        try { _ = ((typeof(byte).Namespace).Length); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()
    public static void Subject_7()
    {
        try { _ = ((typeof(byte).AssemblyQualifiedName).Length); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()
    public static void Subject_8()
    {
        try { _ = ((typeof(byte).TypeHandle).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()
    public static void Subject_9()
    {
        try { _ = ((typeof(byte).IsValueType) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()
    public static void Subject_10()
    {
        try { _ = ((typeof(byte).IsEnum) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()
    public static void Subject_11()
    {
        try { _ = ((typeof(byte).IsAbstract) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()
    public static void Subject_12()
    {
        try { _ = ((typeof(byte).IsSealed) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()
    public static void Subject_13()
    {
        try { _ = ((typeof(byte).IsInterface) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [14] System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()
    public static void Subject_14()
    {
        try { _ = ((typeof(byte).IsArray) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [15] System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()
    public static void Subject_15()
    {
        try { _ = ((typeof(byte).IsByRef) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [16] System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()
    public static void Subject_16()
    {
        try { _ = ((typeof(byte).IsPointer) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [17] System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()
    public static void Subject_17()
    {
        try { _ = ((typeof(byte).IsGenericType) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [18] System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()
    public static void Subject_18()
    {
        try { _ = ((typeof(byte).IsConstructedGenericType) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [19] System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()
    public static void Subject_19()
    {
        try { _ = ((typeof(byte).IsNested) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [20] System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()
    public static void Subject_20()
    {
        try { _ = ((typeof(byte).IsPublic) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [21] System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()
    public static void Subject_21()
    {
        try { _ = ((typeof(byte).IsNotPublic) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [22] System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()
    public static void Subject_22()
    {
        try { _ = ((typeof(byte).IsNestedPublic) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [23] System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()
    public static void Subject_23()
    {
        try { _ = ((typeof(byte).IsNestedPrivate) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [24] System.Private.CoreLib/System.Type::get_BaseType:System.Type()
    public static void Subject_24()
    {
        try { _ = ((typeof(byte).BaseType).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [25] System.Private.CoreLib/System.Type::get_DeclaringType:System.Type()
    public static void Subject_25()
    {
        try { _ = ((typeof(byte).DeclaringType).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [26] System.Private.CoreLib/System.Type::get_ReflectedType:System.Type()
    public static void Subject_26()
    {
        try { _ = ((typeof(byte).ReflectedType).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [27] System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()
    public static void Subject_27()
    {
        try { _ = ((typeof(byte).UnderlyingSystemType).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [28] System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()
    public static void Subject_28()
    {
        try { _ = ((typeof(byte).GetMethods()).Length); }
        catch { _exitCode = 1; }
    }

    // [29] System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)
    public static void Subject_29()
    {
        try { _ = ((typeof(byte).GetMethods(System.Reflection.BindingFlags.Default)).Length); }
        catch { _exitCode = 1; }
    }

    // [30] System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()
    public static void Subject_30()
    {
        try { _ = ((typeof(byte).GetFields()).Length); }
        catch { _exitCode = 1; }
    }

    // [31] System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)
    public static void Subject_31()
    {
        try { _ = ((typeof(byte).GetFields(System.Reflection.BindingFlags.Default)).Length); }
        catch { _exitCode = 1; }
    }

    // [32] System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()
    public static void Subject_32()
    {
        try { _ = ((typeof(byte).GetProperties()).Length); }
        catch { _exitCode = 1; }
    }

    // [33] System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)
    public static void Subject_33()
    {
        try { _ = ((typeof(byte).GetProperties(System.Reflection.BindingFlags.Default)).Length); }
        catch { _exitCode = 1; }
    }

    // [34] System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()
    public static void Subject_34()
    {
        try { _ = ((typeof(byte).GetEvents()).Length); }
        catch { _exitCode = 1; }
    }

    // [35] System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()
    public static void Subject_35()
    {
        try { _ = ((typeof(byte).GetConstructors()).Length); }
        catch { _exitCode = 1; }
    }

    // [36] System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)
    public static void Subject_36()
    {
        try { _ = ((typeof(byte).GetConstructors(System.Reflection.BindingFlags.Default)).Length); }
        catch { _exitCode = 1; }
    }

    // [37] System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()
    public static void Subject_37()
    {
        try { _ = ((typeof(byte).GetMembers()).Length); }
        catch { _exitCode = 1; }
    }

    // [38] System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()
    public static void Subject_38()
    {
        try { _ = ((typeof(byte).GetInterfaces()).Length); }
        catch { _exitCode = 1; }
    }

    // [39] System.Private.CoreLib/System.Type::GetElementType:System.Type()
    public static void Subject_39()
    {
        try { _ = ((typeof(byte).GetElementType()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [40] System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()
    public static void Subject_40()
    {
        try { _ = ((typeof(byte).GetNestedTypes()).Length); }
        catch { _exitCode = 1; }
    }

    // [41] System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)
    public static void Subject_41()
    {
        try { _ = ((typeof(byte).GetNestedType("hello")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [42] System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()
    public static void Subject_42()
    {
        try { _ = ((typeof(byte).Assembly).GetHashCode()); }
        catch { _exitCode = 1; }
    }

}