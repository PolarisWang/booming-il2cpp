// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/type
// Assembly: System.Private.CoreLib
// Variant: semantic-patch

using System;
using System.Reflection;

public static class ReflectionTypeSemanticPatchEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            case 10: return Method10();
            case 11: return Method11();
            case 12: return Method12();
            case 13: return Method13();
            case 14: return Method14();
            case 15: return Method15();
            case 16: return Method16();
            case 17: return Method17();
            case 18: return Method18();
            case 19: return Method19();
            case 20: return Method20();
            case 21: return Method21();
            case 22: return Method22();
            case 23: return Method23();
            case 24: return Method24();
            case 25: return Method25();
            case 26: return Method26();
            case 27: return Method27();
            case 28: return Method28();
            case 29: return Method29();
            case 30: return Method30();
            case 31: return Method31();
            case 32: return Method32();
            case 33: return Method33();
            case 34: return Method34();
            case 35: return Method35();
            case 36: return Method36();
            case 37: return Method37();
            case 38: return Method38();
            case 39: return Method39();
            case 40: return Method40();
            case 41: return Method41();
            case 42: return Method42();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    static int Method0()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String) could not be auto-generated for semantic-patch
        return 0;
    }

    // [1] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)
    static int Method1()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean) could not be auto-generated for semantic-patch
        return 0;
    }

    // [2] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)
    static int Method2()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean) could not be auto-generated for semantic-patch
        return 0;
    }

    // [3] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    static int Method3()
    {
        return Type.GetTypeFromHandle(typeof(int).TypeHandle).GetHashCode();
    }

    // [4] System.Private.CoreLib/System.Type::get_Name:System.String()
    static int Method4()
    {
        return (typeof(int).Name?.GetHashCode() ?? 0);
    }

    // [5] System.Private.CoreLib/System.Type::get_FullName:System.String()
    static int Method5()
    {
        return (typeof(int).FullName?.GetHashCode() ?? 0);
    }

    // [6] System.Private.CoreLib/System.Type::get_Namespace:System.String()
    static int Method6()
    {
        return (typeof(int).Namespace?.GetHashCode() ?? 0);
    }

    // [7] System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()
    static int Method7()
    {
        return (typeof(int).AssemblyQualifiedName?.GetHashCode() ?? 0);
    }

    // [8] System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()
    static int Method8()
    {
        return typeof(int).TypeHandle.GetHashCode();
    }

    // [9] System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()
    static int Method9()
    {
        return (typeof(int).IsValueType ? 1 : 0);
    }

    // [10] System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()
    static int Method10()
    {
        return (typeof(int).IsEnum ? 1 : 0);
    }

    // [11] System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()
    static int Method11()
    {
        return (typeof(int).IsAbstract ? 1 : 0);
    }

    // [12] System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()
    static int Method12()
    {
        return (typeof(int).IsSealed ? 1 : 0);
    }

    // [13] System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()
    static int Method13()
    {
        return (typeof(int).IsInterface ? 1 : 0);
    }

    // [14] System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()
    static int Method14()
    {
        return (typeof(int).IsArray ? 1 : 0);
    }

    // [15] System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()
    static int Method15()
    {
        return (typeof(int).IsByRef ? 1 : 0);
    }

    // [16] System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()
    static int Method16()
    {
        return (typeof(int).IsPointer ? 1 : 0);
    }

    // [17] System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()
    static int Method17()
    {
        return (typeof(int).IsGenericType ? 1 : 0);
    }

    // [18] System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()
    static int Method18()
    {
        return (typeof(int).IsConstructedGenericType ? 1 : 0);
    }

    // [19] System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()
    static int Method19()
    {
        return (typeof(int).IsNested ? 1 : 0);
    }

    // [20] System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()
    static int Method20()
    {
        return (typeof(int).IsPublic ? 1 : 0);
    }

    // [21] System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()
    static int Method21()
    {
        return (typeof(int).IsNotPublic ? 1 : 0);
    }

    // [22] System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()
    static int Method22()
    {
        return (typeof(int).IsNestedPublic ? 1 : 0);
    }

    // [23] System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()
    static int Method23()
    {
        return (typeof(int).IsNestedPrivate ? 1 : 0);
    }

    // [24] System.Private.CoreLib/System.Type::get_BaseType:System.Type()
    static int Method24()
    {
        return typeof(int).BaseType.GetHashCode();
    }

    // [25] System.Private.CoreLib/System.Type::get_DeclaringType:System.Type()
    static int Method25()
    {
        return typeof(int).DeclaringType.GetHashCode();
    }

    // [26] System.Private.CoreLib/System.Type::get_ReflectedType:System.Type()
    static int Method26()
    {
        return typeof(int).ReflectedType.GetHashCode();
    }

    // [27] System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()
    static int Method27()
    {
        return typeof(int).UnderlyingSystemType.GetHashCode();
    }

    // [28] System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()
    static int Method28()
    {
        return typeof(int).GetMethods().GetHashCode();
    }

    // [29] System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)
    static int Method29()
    {
        return typeof(int).GetMethods(System.Reflection.BindingFlags.Instance).GetHashCode();
    }

    // [30] System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()
    static int Method30()
    {
        return typeof(int).GetFields().GetHashCode();
    }

    // [31] System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)
    static int Method31()
    {
        return typeof(int).GetFields(System.Reflection.BindingFlags.Instance).GetHashCode();
    }

    // [32] System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()
    static int Method32()
    {
        return typeof(int).GetProperties().GetHashCode();
    }

    // [33] System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)
    static int Method33()
    {
        return typeof(int).GetProperties(System.Reflection.BindingFlags.Instance).GetHashCode();
    }

    // [34] System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()
    static int Method34()
    {
        return typeof(int).GetEvents().GetHashCode();
    }

    // [35] System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()
    static int Method35()
    {
        return typeof(int).GetConstructors().GetHashCode();
    }

    // [36] System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)
    static int Method36()
    {
        return typeof(int).GetConstructors(System.Reflection.BindingFlags.Instance).GetHashCode();
    }

    // [37] System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()
    static int Method37()
    {
        return typeof(int).GetMembers().GetHashCode();
    }

    // [38] System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()
    static int Method38()
    {
        return typeof(int).GetInterfaces().GetHashCode();
    }

    // [39] System.Private.CoreLib/System.Type::GetElementType:System.Type()
    static int Method39()
    {
        return typeof(int).GetElementType().GetHashCode();
    }

    // [40] System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()
    static int Method40()
    {
        return typeof(int).GetNestedTypes().GetHashCode();
    }

    // [41] System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)
    static int Method41()
    {
        return typeof(int).GetNestedType("99").GetHashCode();
    }

    // [42] System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()
    static int Method42()
    {
        return typeof(int).Assembly.GetHashCode();
    }

}