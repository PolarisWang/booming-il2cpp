// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/binding
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Globalization;
using System.Reflection;

public static class ReflectionBindingNativeEntry
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

    // [0] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)
    static int Method0()
    {
        return (int)(typeof(byte).GetMethod("hello"));
    }

    // [1] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)
    static int Method1()
    {
        return (int)(typeof(byte).GetMethod("hello", null!));
    }

    // [2] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])
    static int Method2()
    {
        return (int)(typeof(byte).GetMethod("hello", Array.Empty<System.Type>()));
    }

    // [3] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)
    static int Method3()
    {
        return (int)(typeof(byte).GetField("hello"));
    }

    // [4] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)
    static int Method4()
    {
        return (int)(typeof(byte).GetField("hello", null!));
    }

    // [5] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)
    static int Method5()
    {
        return (int)(typeof(byte).GetProperty("hello"));
    }

    // [6] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)
    static int Method6()
    {
        return (int)(typeof(byte).GetProperty("hello", null!));
    }

    // [7] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])
    static int Method7()
    {
        return (int)(typeof(byte).GetProperty("hello", Array.Empty<System.Type>()));
    }

    // [8] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)
    static int Method8()
    {
        return (int)(typeof(byte).GetEvent("hello"));
    }

    // [9] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)
    static int Method9()
    {
        return (int)(typeof(byte).GetEvent("hello", null!));
    }

    // [10] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    static int Method10()
    {
        // TODO: System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[]) could not be auto-generated
        return 0;
    }

    // [11] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    static int Method11()
    {
        return (int)(default(MethodInfo)!.Invoke(42, null!, null!, Array.Empty<System.Object>(), null!));
    }

}