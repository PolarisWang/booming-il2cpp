// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/binding
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ReflectionBindingPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    public static int Subject_10()
    {
        return unchecked((int)(0xB0000000u + 10));
    }

    // [11] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    public static int Subject_11()
    {
        return unchecked((int)(0xB0000000u + 11));
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
                case 10: Subject_10(); break;
                case 11: Subject_11(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}