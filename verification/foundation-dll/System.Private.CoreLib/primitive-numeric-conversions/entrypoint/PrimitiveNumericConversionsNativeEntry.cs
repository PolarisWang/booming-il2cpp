// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/primitive/numeric/conversions
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

public static class PrimitiveNumericConversionsNativeEntry
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
            case 12: return Method12();
            case 13: return Method13();
            case 14: return Method14();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Convert::ToBoolean:System.Boolean(System.String)
    static int Method0()
    {
        return (Convert.ToBoolean("true") ? 1 : 0);
    }

    // [1] System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)
    static int Method1()
    {
        return (int)Convert.ToByte("99");
    }

    // [2] System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)
    static int Method2()
    {
        return (int)Convert.ToInt16("99");
    }

    // [3] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)
    static int Method3()
    {
        return Convert.ToInt32("99");
    }

    // [4] System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)
    static int Method4()
    {
        return unchecked((int)Convert.ToInt64("99"));
    }

    // [5] System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)
    static int Method5()
    {
        return unchecked((int)Convert.ToSingle("99.5"));
    }

    // [6] System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)
    static int Method6()
    {
        return unchecked((int)Convert.ToDouble("99.5"));
    }

    // [7] System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)
    static int Method7()
    {
        return unchecked((int)Convert.ToDecimal("99.5"));
    }

    // [8] System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)
    static int Method8()
    {
        return (Convert.ToString(42)?.GetHashCode() ?? 0);
    }

    // [9] System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)
    static int Method9()
    {
        return (Convert.ToString(42.0)?.GetHashCode() ?? 0);
    }

    // [10] System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)
    static int Method10()
    {
        return unchecked((int)Convert.ToDecimal("99.5"));
    }

    // [11] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)
    static int Method11()
    {
        return Convert.ToInt32("99");
    }

    // [12] System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)
    static int Method12()
    {
        return int.Parse("99");
    }

    // [13] System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)
    static int Method13()
    {
        return unchecked((int)long.Parse("99"));
    }

    // [14] System.Private.CoreLib/System.Double::Parse:System.Double(System.String)
    static int Method14()
    {
        return unchecked((int)double.Parse("99.5"));
    }

}