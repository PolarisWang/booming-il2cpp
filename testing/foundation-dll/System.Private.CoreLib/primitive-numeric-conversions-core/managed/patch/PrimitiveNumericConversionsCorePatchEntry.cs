// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/primitive/numeric/conversions/core
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class PrimitiveNumericConversionsCorePatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
                case 2: Subject_2(); break;
                case 3: Subject_3(); break;
                case 4: Subject_4(); break;
                case 5: Subject_5(); break;
                case 6: Subject_6(); break;
                case 7: Subject_7(); break;
                case 8: Subject_8(); break;
                case 9: Subject_9(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}