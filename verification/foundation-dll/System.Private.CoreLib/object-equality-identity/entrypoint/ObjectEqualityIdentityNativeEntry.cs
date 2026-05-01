// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/object/equality/identity
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

public static class ObjectEqualityIdentityNativeEntry
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
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)
    static int Method0()
    {
        return (int)(new object().Equals(42));
    }

    // [1] System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)
    static int Method1()
    {
        return (int)(Object.Equals(42, 42));
    }

    // [2] System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)
    static int Method2()
    {
        return (int)(Object.ReferenceEquals(42, 42));
    }

    // [3] System.Private.CoreLib/System.Object::GetHashCode:System.Int32()
    static int Method3()
    {
        return new object().GetHashCode();
    }

    // [4] System.Private.CoreLib/System.Object::ToString:System.String()
    static int Method4()
    {
        return (int)(new object().ToString());
    }

    // [5] System.Private.CoreLib/System.Object::GetType:System.Type()
    static int Method5()
    {
        return (int)(new object().GetType());
    }

    // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    static int Method6()
    {
        // TODO: System.Private.CoreLib/System.Object::MemberwiseClone:System.Object() could not be auto-generated
        return 0;
    }

}