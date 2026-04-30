// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/activation
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Globalization;
using System.Reflection;

public static class SystemPrivateCoreLib_ReflectionActivationNativeEntry
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
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)
    static int Method0()
    {
        return (Activator.CreateInstance(typeof(byte))?.GetHashCode() ?? 0);
    }

    // [1] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])
    static int Method1()
    {
        return (Activator.CreateInstance(typeof(byte), Array.Empty<System.Object>())?.GetHashCode() ?? 0);
    }

    // [2] System.Private.CoreLib/System.Activator::CreateInstance:T()
    static int Method2()
    {
        return Activator.CreateInstance().GetHashCode();
    }

    // [3] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    static int Method3()
    {
        return (Activator.CreateInstance(typeof(byte), System.Reflection.BindingFlags.Default, null!, Array.Empty<System.Object>(), null!)?.GetHashCode() ?? 0);
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    static int Method4()
    {
        return Array.CreateInstance(typeof(byte), 42).GetHashCode();
    }

    // [5] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    static int Method5()
    {
        return Array.CreateInstance(typeof(byte), 42, 42).GetHashCode();
    }

    // [6] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])
    static int Method6()
    {
        return Array.CreateInstance(typeof(byte), Array.Empty<System.Int32>()).GetHashCode();
    }

    // [7] System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
    static int Method7()
    {
        return (typeof(object).GetConstructor(System.Type.EmptyTypes)!.Invoke(null)?.GetHashCode() ?? 0);
    }

}