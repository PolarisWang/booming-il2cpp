// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/generics
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

public static class ReflectionGenericsNativeEntry
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
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Type::GetGenericTypeDefinition:System.Type()
    static int Method0()
    {
        return (int)(typeof(byte).GetGenericTypeDefinition());
    }

    // [1] System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])
    static int Method1()
    {
        return (int)(typeof(byte).MakeGenericType(Array.Empty<System.Type>()));
    }

    // [2] System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()
    static int Method2()
    {
        return (int)(typeof(byte).GetGenericArguments());
    }

    // [3] System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]()
    static int Method3()
    {
        return (int)(typeof(byte).GetGenericParameterConstraints());
    }

    // [4] System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32()
    static int Method4()
    {
        return typeof(byte).GenericParameterPosition;
    }

    // [5] System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()
    static int Method5()
    {
        return (int)(typeof(byte).ContainsGenericParameters());
    }

    // [6] System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()
    static int Method6()
    {
        return (int)(typeof(byte).IsGenericType);
    }

    // [7] System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean()
    static int Method7()
    {
        return (int)(typeof(byte).IsGenericTypeDefinition);
    }

    // [8] System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()
    static int Method8()
    {
        return (int)(typeof(byte).IsConstructedGenericType);
    }

    // [9] System.Private.CoreLib/System.Type::MakeArrayType:System.Type()
    static int Method9()
    {
        return (int)(typeof(byte).MakeArrayType());
    }

    // [10] System.Private.CoreLib/System.Type::MakeByRefType:System.Type()
    static int Method10()
    {
        return (int)(typeof(byte).MakeByRefType());
    }

    // [11] System.Private.CoreLib/System.Type::MakePointerType:System.Type()
    static int Method11()
    {
        return (int)(typeof(byte).MakePointerType());
    }

}