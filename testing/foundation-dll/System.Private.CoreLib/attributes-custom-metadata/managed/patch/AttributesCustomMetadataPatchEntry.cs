// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/attributes/custom/metadata
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class AttributesCustomMetadataPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.MemberInfo,System.Type)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Private.CoreLib/System.Attribute::GetCustomAttribute:System.Attribute(System.Reflection.Assembly,System.Type)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo,System.Type)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly,System.Type)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.MemberInfo)
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Private.CoreLib/System.Attribute::GetCustomAttributes:System.Attribute[](System.Reflection.Assembly)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.MemberInfo,System.Type)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Private.CoreLib/System.Attribute::IsDefined:System.Boolean(System.Reflection.Assembly,System.Type)
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Private.CoreLib/System.Attribute::get_TypeId:System.Object()
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}