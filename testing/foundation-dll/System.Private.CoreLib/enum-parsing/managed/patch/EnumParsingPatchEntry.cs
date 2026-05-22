// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/enum/parsing
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class EnumParsingPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)
    // [7] System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)
    // [8] System.Private.CoreLib/System.Enum::ToString:System.String()
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Private.CoreLib/System.Enum::ToString:System.String(System.String)
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)
    // [11] System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)
    // [12] System.Private.CoreLib/System.Enum::BoxToString:System.String()
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
                case 6: CustomEntryMethod6(); break;
                case 7: CustomEntryMethod7(); break;
                case 8: Subject_8(); break;
                case 9: Subject_9(); break;
                case 10: CustomEntryMethod10(); break;
                case 11: CustomEntryMethod11(); break;
                case 12: CustomEntryMethod12(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}