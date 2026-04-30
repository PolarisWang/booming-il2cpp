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
        return (new Nullable<int>(42).HasValue ? 1 : 0);
    }

    // [1] System.Private.CoreLib/System.Nullable`1::get_Value:T()
    static int Method1()
    {
        return new Nullable<int>(42).Value.GetHashCode();
    }

    // [2] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()
    static int Method2()
    {
        return new Nullable<int>(42).GetValueOrDefault().GetHashCode();
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
        return (new Nullable<int>(42).Equals(42) ? 1 : 0);
    }

    // [5] System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()
    static int Method5()
    {
        return new Nullable<int>(42).GetHashCode();
    }

    // [6] System.Private.CoreLib/System.Nullable`1::ToString:System.String()
    static int Method6()
    {
        return (new Nullable<int>(42).ToString()?.GetHashCode() ?? 0);
    }

    // [7] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    static int Method7()
    {
        return (((ValueType)42).Equals(42) ? 1 : 0);
    }

    // [8] System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()
    static int Method8()
    {
        return ((ValueType)42).GetHashCode();
    }

    // [9] System.Private.CoreLib/System.ValueType::ToString:System.String()
    static int Method9()
    {
        return (((ValueType)42).ToString()?.GetHashCode() ?? 0);
    }

}