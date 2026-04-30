// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/generics
// Assembly: System.Private.CoreLib
// Variant: semantic-patch

using System;

public static class ReflectionGenericsSemanticPatchEntry
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
        return typeof(int).GetGenericTypeDefinition().GetHashCode();
    }

    // [1] System.Private.CoreLib/System.Type::MakeGenericType:System.Type(System.Type[])
    static int Method1()
    {
        return typeof(int).MakeGenericType(Array.Empty<System.Type>()).GetHashCode();
    }

    // [2] System.Private.CoreLib/System.Type::GetGenericArguments:System.Type[]()
    static int Method2()
    {
        return typeof(int).GetGenericArguments().GetHashCode();
    }

    // [3] System.Private.CoreLib/System.Type::GetGenericParameterConstraints:System.Type[]()
    static int Method3()
    {
        return typeof(int).GetGenericParameterConstraints().GetHashCode();
    }

    // [4] System.Private.CoreLib/System.Type::get_GenericParameterPosition:System.Int32()
    static int Method4()
    {
        return typeof(int).GenericParameterPosition;
    }

    // [5] System.Private.CoreLib/System.Type::ContainsGenericParameters:System.Boolean()
    static int Method5()
    {
        return (typeof(byte).ContainsGenericParameters ? 1 : 0);
    }

    // [6] System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()
    static int Method6()
    {
        return (typeof(int).IsGenericType ? 1 : 0);
    }

    // [7] System.Private.CoreLib/System.Type::get_IsGenericTypeDefinition:System.Boolean()
    static int Method7()
    {
        return (typeof(int).IsGenericTypeDefinition ? 1 : 0);
    }

    // [8] System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()
    static int Method8()
    {
        return (typeof(int).IsConstructedGenericType ? 1 : 0);
    }

    // [9] System.Private.CoreLib/System.Type::MakeArrayType:System.Type()
    static int Method9()
    {
        return typeof(int).MakeArrayType().GetHashCode();
    }

    // [10] System.Private.CoreLib/System.Type::MakeByRefType:System.Type()
    static int Method10()
    {
        return typeof(int).MakeByRefType().GetHashCode();
    }

    // [11] System.Private.CoreLib/System.Type::MakePointerType:System.Type()
    static int Method11()
    {
        return typeof(int).MakePointerType().GetHashCode();
    }

}