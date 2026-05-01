// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/boxing/unboxing/casts
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Runtime.CompilerServices;

public static class BoxingUnboxingCastsNativeEntry
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
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)
    static int Method0()
    {
        return (int)(Convert.ChangeType(42, typeof(byte)));
    }

    // [1] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)
    static int Method1()
    {
        return (int)(Convert.ChangeType(42, typeof(byte), null!));
    }

    // [2] System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
    static int Method2()
    {
        return (int)(typeof(byte).IsAssignableFrom(typeof(byte)));
    }

    // [3] System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)
    static int Method3()
    {
        return (int)(typeof(byte).IsInstanceOfType(42));
    }

    // [4] System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
    static int Method4()
    {
        return (int)(typeof(byte).IsSubclassOf(typeof(byte)));
    }

    // [5] System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)
    static int Method5()
    {
        return (int)(typeof(byte).IsAssignableTo(typeof(byte)));
    }

    // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    static int Method6()
    {
        // TODO: System.Private.CoreLib/System.Object::MemberwiseClone:System.Object() could not be auto-generated
        return 0;
    }

    // [7] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
    static int Method7()
    {
        return (int)(RuntimeHelpers.GetObjectValue(42));
    }

    // [8] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    static int Method8()
    {
        return (int)(Array.CreateInstance(typeof(byte), 42));
    }

    // [9] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    static int Method9()
    {
        return (int)(Array.CreateInstance(typeof(byte), 42, 42));
    }

    // [10] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    static int Method10()
    {
        return (int)(((ValueType)42).Equals(42));
    }

}