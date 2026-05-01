// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/activation
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Globalization;
using System.Reflection;

public static class ReflectionActivationNativeEntry
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
        return (int)(Activator.CreateInstance(typeof(byte)));
    }

    // [1] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])
    static int Method1()
    {
        return (int)(Activator.CreateInstance(typeof(byte), Array.Empty<System.Object>()));
    }

    // [2] System.Private.CoreLib/System.Activator::CreateInstance:T()
    static int Method2()
    {
        return (int)(Activator.CreateInstance());
    }

    // [3] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    static int Method3()
    {
        return (int)(Activator.CreateInstance(typeof(byte), null!, null!, Array.Empty<System.Object>(), null!));
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    static int Method4()
    {
        return (int)(Array.CreateInstance(typeof(byte), 42));
    }

    // [5] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    static int Method5()
    {
        return (int)(Array.CreateInstance(typeof(byte), 42, 42));
    }

    // [6] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])
    static int Method6()
    {
        return (int)(Array.CreateInstance(typeof(byte), Array.Empty<System.Int32>()));
    }

    // [7] System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
    static int Method7()
    {
        // TODO: System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[]) could not be auto-generated
        return 0;
    }

}