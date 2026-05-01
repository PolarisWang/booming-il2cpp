// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/generic/nullable/value
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

public static class GenericNullableValueNativeEntry
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
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()
    static int Method0()
    {
        return (int)(default(Nullable<int>).HasValue);
    }

    // [1] System.Private.CoreLib/System.Nullable`1::get_Value:T()
    static int Method1()
    {
        // TODO: System.Private.CoreLib/System.Nullable`1::get_Value:T() could not be auto-generated
        return 0;
    }

    // [2] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()
    static int Method2()
    {
        return (int)(default(Nullable<int>).GetValueOrDefault());
    }

    // [3] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)
    static int Method3()
    {
        // TODO: System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T) could not be auto-generated
        return 0;
    }

    // [4] System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)
    static int Method4()
    {
        return (int)(default(Nullable<int>).Equals(42));
    }

    // [5] System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()
    static int Method5()
    {
        return default(Nullable<int>).GetHashCode();
    }

    // [6] System.Private.CoreLib/System.Nullable`1::ToString:System.String()
    static int Method6()
    {
        return (int)(default(Nullable<int>).ToString());
    }

    // [7] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    static int Method7()
    {
        return (int)(((ValueType)42).Equals(42));
    }

    // [8] System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()
    static int Method8()
    {
        return ((ValueType)42).GetHashCode();
    }

    // [9] System.Private.CoreLib/System.ValueType::ToString:System.String()
    static int Method9()
    {
        return (int)(((ValueType)42).ToString());
    }

}