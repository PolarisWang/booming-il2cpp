// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/type/runtime/handles
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class TypeRuntimeHandlesPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetType:System.Type(System.String)
    public static void CustomEntryMethod0()
    {
    }

    // [1] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)
    public static void CustomEntryMethod1()
    {
    }

    // [2] System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)
    public static void CustomEntryMethod2()
    {
    }

    // [3] System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Private.CoreLib/System.Type::get_FullName:System.String()
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Private.CoreLib/System.Type::get_Name:System.String()
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.Private.CoreLib/System.Type::get_BaseType:System.Type()
    public static int Subject_10()
    {
        return unchecked((int)(0xB0000000u + 10));
    }

    // [11] System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()
    public static int Subject_11()
    {
        return unchecked((int)(0xB0000000u + 11));
    }

    // [12] System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()
    public static int Subject_12()
    {
        return unchecked((int)(0xB0000000u + 12));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: CustomEntryMethod0(); break;
                case 1: CustomEntryMethod1(); break;
                case 2: CustomEntryMethod2(); break;
                case 3: Subject_3(); break;
                case 4: Subject_4(); break;
                case 5: Subject_5(); break;
                case 6: Subject_6(); break;
                case 7: Subject_7(); break;
                case 8: Subject_8(); break;
                case 9: Subject_9(); break;
                case 10: Subject_10(); break;
                case 11: Subject_11(); break;
                case 12: Subject_12(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}