// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/convert/char
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

public static class ConvertCharNativeEntry
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
            case 15: return Method15();
            case 16: return Method16();
            case 17: return Method17();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)
    static int Method0()
    {
        return (int)Convert.ToChar(true);
    }

    // [1] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)
    static int Method1()
    {
        return (int)Convert.ToChar((byte)42);
    }

    // [2] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)
    static int Method2()
    {
        return (int)Convert.ToChar('A');
    }

    // [3] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)
    static int Method3()
    {
        return (int)Convert.ToChar(DateTime.UtcNow);
    }

    // [4] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)
    static int Method4()
    {
        return (int)Convert.ToChar(42m);
    }

    // [5] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)
    static int Method5()
    {
        return (int)Convert.ToChar(42.0);
    }

    // [6] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)
    static int Method6()
    {
        return (int)Convert.ToChar((short)42);
    }

    // [7] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)
    static int Method7()
    {
        return (int)Convert.ToChar(42);
    }

    // [8] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)
    static int Method8()
    {
        return (int)Convert.ToChar(42L);
    }

    // [9] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)
    static int Method9()
    {
        return (int)Convert.ToChar(42);
    }

    // [10] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)
    static int Method10()
    {
        return (int)Convert.ToChar("A", System.Globalization.CultureInfo.InvariantCulture);
    }

    // [11] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)
    static int Method11()
    {
        return (int)Convert.ToChar((sbyte)42);
    }

    // [12] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)
    static int Method12()
    {
        return (int)Convert.ToChar(42.0f);
    }

    // [13] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)
    static int Method13()
    {
        return (int)Convert.ToChar("hello");
    }

    // [14] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)
    static int Method14()
    {
        return (int)Convert.ToChar("A", System.Globalization.CultureInfo.InvariantCulture);
    }

    // [15] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)
    static int Method15()
    {
        return (int)Convert.ToChar((ushort)42);
    }

    // [16] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)
    static int Method16()
    {
        return (int)Convert.ToChar(42u);
    }

    // [17] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)
    static int Method17()
    {
        return (int)Convert.ToChar(42uL);
    }

}