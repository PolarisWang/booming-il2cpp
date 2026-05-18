// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/type
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Reflection;

public static partial class ReflectionTypeSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    public static void Subject_0()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String) could not be auto-generated
    }

    // [1] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)
    public static void Subject_1()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean) could not be auto-generated
    }

    // [2] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)
    public static void Subject_2()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean) could not be auto-generated
    }

    // [3] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    public static void Subject_3()
    {
        try { Type.GetTypeFromHandle(default(System.RuntimeTypeHandle)); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Private.CoreLib/System.Type::get_Name:System.String()
    public static void Subject_4()
    {
        if (((typeof(byte).Name).Length) != 4) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Type::get_FullName:System.String()
    public static void Subject_5()
    {
        if (((typeof(byte).FullName).Length) != 11) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Type::get_Namespace:System.String()
    public static void Subject_6()
    {
        if (((typeof(byte).Namespace).Length) != 6) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()
    public static void Subject_7()
    {
        if (((typeof(byte).AssemblyQualifiedName).Length) != 103) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()
    public static void Subject_8()
    {
        if (((typeof(byte).TypeHandle).GetHashCode()) != 35342034) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()
    public static void Subject_9()
    {
        if (((typeof(byte).IsValueType) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()
    public static void Subject_10()
    {
        if (((typeof(byte).IsEnum) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [11] System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()
    public static void Subject_11()
    {
        if (((typeof(byte).IsAbstract) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [12] System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()
    public static void Subject_12()
    {
        if (((typeof(byte).IsSealed) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [13] System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()
    public static void Subject_13()
    {
        if (((typeof(byte).IsInterface) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [14] System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()
    public static void Subject_14()
    {
        if (((typeof(byte).IsArray) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [15] System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()
    public static void Subject_15()
    {
        if (((typeof(byte).IsByRef) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [16] System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()
    public static void Subject_16()
    {
        if (((typeof(byte).IsPointer) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [17] System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()
    public static void Subject_17()
    {
        if (((typeof(byte).IsGenericType) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [18] System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()
    public static void Subject_18()
    {
        if (((typeof(byte).IsConstructedGenericType) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [19] System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()
    public static void Subject_19()
    {
        if (((typeof(byte).IsNested) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [20] System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()
    public static void Subject_20()
    {
        if (((typeof(byte).IsPublic) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [21] System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()
    public static void Subject_21()
    {
        if (((typeof(byte).IsNotPublic) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [22] System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()
    public static void Subject_22()
    {
        if (((typeof(byte).IsNestedPublic) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [23] System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()
    public static void Subject_23()
    {
        if (((typeof(byte).IsNestedPrivate) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [24] System.Private.CoreLib/System.Type::get_BaseType:System.Type()
    public static void Subject_24()
    {
        if (((typeof(byte).BaseType).GetHashCode()) != 56793269) _exitCode = 1;
    }

    // [25] System.Private.CoreLib/System.Type::get_DeclaringType:System.Type()
    public static void Subject_25()
    {
        try { _ = typeof(byte).DeclaringType; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [26] System.Private.CoreLib/System.Type::get_ReflectedType:System.Type()
    public static void Subject_26()
    {
        try { _ = typeof(byte).ReflectedType; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [27] System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()
    public static void Subject_27()
    {
        if (((typeof(byte).UnderlyingSystemType).GetHashCode()) != 35342034) _exitCode = 1;
    }

    // [28] System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()
    public static void Subject_28()
    {
        if (((typeof(byte).GetMethods()).Length) != 47) _exitCode = 1;
    }

    // [29] System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)
    public static void Subject_29()
    {
        if (((typeof(byte).GetMethods(System.Reflection.BindingFlags.Default)).Length) != 0) _exitCode = 1;
    }

    // [30] System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()
    public static void Subject_30()
    {
        if (((typeof(byte).GetFields()).Length) != 2) _exitCode = 1;
    }

    // [31] System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)
    public static void Subject_31()
    {
        if (((typeof(byte).GetFields(System.Reflection.BindingFlags.Default)).Length) != 0) _exitCode = 1;
    }

    // [32] System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()
    public static void Subject_32()
    {
        if (((typeof(byte).GetProperties()).Length) != 0) _exitCode = 1;
    }

    // [33] System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)
    public static void Subject_33()
    {
        if (((typeof(byte).GetProperties(System.Reflection.BindingFlags.Default)).Length) != 0) _exitCode = 1;
    }

    // [34] System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()
    public static void Subject_34()
    {
        if (((typeof(byte).GetEvents()).Length) != 0) _exitCode = 1;
    }

    // [35] System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()
    public static void Subject_35()
    {
        if (((typeof(byte).GetConstructors()).Length) != 0) _exitCode = 1;
    }

    // [36] System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)
    public static void Subject_36()
    {
        if (((typeof(byte).GetConstructors(System.Reflection.BindingFlags.Default)).Length) != 0) _exitCode = 1;
    }

    // [37] System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()
    public static void Subject_37()
    {
        if (((typeof(byte).GetMembers()).Length) != 49) _exitCode = 1;
    }

    // [38] System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()
    public static void Subject_38()
    {
        if (((typeof(byte).GetInterfaces()).Length) != 33) _exitCode = 1;
    }

    // [39] System.Private.CoreLib/System.Type::GetElementType:System.Type()
    public static void Subject_39()
    {
        try { typeof(byte).GetElementType(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [40] System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()
    public static void Subject_40()
    {
        if (((typeof(byte).GetNestedTypes()).Length) != 0) _exitCode = 1;
    }

    // [41] System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)
    public static void Subject_41()
    {
        try { typeof(byte).GetNestedType("hello"); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [42] System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()
    public static void Subject_42()
    {
        if (((typeof(byte).Assembly).GetHashCode()) != 115000) _exitCode = 1;
    }

}