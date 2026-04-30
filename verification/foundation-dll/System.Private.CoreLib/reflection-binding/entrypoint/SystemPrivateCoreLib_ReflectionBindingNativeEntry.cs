// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/binding
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Globalization;
using System.Reflection;

public static class SystemPrivateCoreLib_ReflectionBindingNativeEntry
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
        return typeof(byte).GetMethod("hello").GetHashCode();
    }

    // [1] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)
    static int Method1()
    {
        return typeof(byte).GetMethod("hello", System.Reflection.BindingFlags.Default).GetHashCode();
    }

    // [2] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])
    static int Method2()
    {
        return typeof(byte).GetMethod("hello", Array.Empty<System.Type>()).GetHashCode();
    }

    // [3] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)
    static int Method3()
    {
        return typeof(byte).GetField("hello").GetHashCode();
    }

    // [4] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)
    static int Method4()
    {
        return typeof(byte).GetField("hello", System.Reflection.BindingFlags.Default).GetHashCode();
    }

    // [5] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)
    static int Method5()
    {
        return typeof(byte).GetProperty("hello").GetHashCode();
    }

    // [6] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)
    static int Method6()
    {
        return typeof(byte).GetProperty("hello", System.Reflection.BindingFlags.Default).GetHashCode();
    }

    // [7] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])
    static int Method7()
    {
        return typeof(byte).GetProperty("hello", Array.Empty<System.Type>()).GetHashCode();
    }

    // [8] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)
    static int Method8()
    {
        return typeof(byte).GetEvent("hello").GetHashCode();
    }

    // [9] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)
    static int Method9()
    {
        return typeof(byte).GetEvent("hello", System.Reflection.BindingFlags.Default).GetHashCode();
    }

    // [10] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    static int Method10()
    {
        return (typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.Invoke("hello", null)?.GetHashCode() ?? 0);
    }

    // [11] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    static int Method11()
    {
        return (typeof(string).GetMethod("ToString", System.Type.EmptyTypes)!.Invoke(42, System.Reflection.BindingFlags.Default, null!, Array.Empty<System.Object>(), null!)?.GetHashCode() ?? 0);
    }

}