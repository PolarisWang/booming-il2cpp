// Hand-written native-AOT entry point for Primitive Numeric Conversions Core verification
// Family: family/System.Private.CoreLib/primitive/numeric-conversions-core
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

public static class PrimitiveNumericConversionsCoreNativeEntry
{
    // [0] byte to int: widening (zero-extend)
    public static int TestByteToInt()
    {
        byte b = 200;
        int x = b;
        return x != 200 ? 1 : 0;
    }

    // [1] int to long: widening (sign-extend)
    public static int TestIntToLong()
    {
        int i = -1;
        long l = i;
        return l != -1L ? 1 : 0;
    }

    // [2] int to short: narrowing (truncate)
    public static int TestIntToShort()
    {
        int i = 0x0000FFFF;
        short s = (short)i;
        return s != -1 ? 1 : 0;
    }

    // [3] int to byte: narrowing (truncate)
    public static int TestIntToByte()
    {
        int i = 256 + 42;
        byte b = (byte)i;
        return b != 42 ? 1 : 0;
    }

    // [4] float to double: widening
    public static int TestFloatToDouble()
    {
        float f = 3.14f;
        double d = f;
        // Verify by casting back: (float)d should round-trip to original
        float f2 = (float)d;
        return f2 != 3.14f ? 1 : 0;
    }

    // [5] double to float: narrowing
    public static int TestDoubleToFloat()
    {
        double d = 2.71828;
        float f = (float)d;
        // Verify by widening back: (double)f should round-trip to same float
        double d2 = f;
        float f2 = (float)d2;
        return f2 != 2.71828f ? 1 : 0;
    }

    // [6] int to double: type conversion
    public static int TestIntToDouble()
    {
        int i = 42;
        double d = i;
        int i2 = (int)d;
        return i2 != 42 ? 1 : 0;
    }

    // [7] double to int: type conversion (truncate)
    public static int TestDoubleToInt()
    {
        double d = 3.99;
        int i = (int)d;
        return i != 3 ? 1 : 0;
    }

    // [8] uint to long: widening (zero-extend, not sign-extend)
    public static int TestUintToLong()
    {
        uint u = 0x80000000;
        long l = u;
        return l != 2147483648L ? 1 : 0;
    }

    // [9] long to int: narrowing (truncate)
    public static int TestLongToInt()
    {
        long l = 0x100000001;
        int i = (int)l;
        return i != 1 ? 1 : 0;
    }
}
