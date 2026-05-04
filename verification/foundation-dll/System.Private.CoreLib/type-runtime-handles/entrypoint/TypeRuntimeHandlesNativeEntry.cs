// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/type/runtime/handles
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Reflection;

public static class TypeRuntimeHandlesNativeEntry
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
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    static int Method0()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String) could not be auto-generated
        return 0;
    }

    // [1] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)
    static int Method1()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean) could not be auto-generated
        return 0;
    }

    // [2] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)
    static int Method2()
    {
        // TODO: System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean) could not be auto-generated
        return 0;
    }

    // [3] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    static int Method3()
    {
        return ((Type.GetTypeFromHandle(default(System.RuntimeTypeHandle))).GetHashCode());
    }

    // [4] System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()
    static int Method4()
    {
        return ((typeof(byte).IsValueType) ? 1 : 0);
    }

    // [5] System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()
    static int Method5()
    {
        return ((typeof(byte).IsEnum) ? 1 : 0);
    }

    // [6] System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()
    static int Method6()
    {
        return ((typeof(byte).IsArray) ? 1 : 0);
    }

    // [7] System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()
    static int Method7()
    {
        return ((typeof(byte).TypeHandle).GetHashCode());
    }

    // [8] System.Private.CoreLib/System.Type::get_FullName:System.String()
    static int Method8()
    {
        return ((typeof(byte).FullName).Length);
    }

    // [9] System.Private.CoreLib/System.Type::get_Name:System.String()
    static int Method9()
    {
        return ((typeof(byte).Name).Length);
    }

    // [10] System.Private.CoreLib/System.Type::get_BaseType:System.Type()
    static int Method10()
    {
        return ((typeof(byte).BaseType).GetHashCode());
    }

    // [11] System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()
    static int Method11()
    {
        return ((typeof(byte).GetMethods()).Length);
    }

    // [12] System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()
    static int Method12()
    {
        return ((typeof(byte).GetFields()).Length);
    }

}