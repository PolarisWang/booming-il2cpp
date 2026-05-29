// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/binding
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ReflectionBindingPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)
    public static void CustomEntryMethod0()
    {
    }

    // [1] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)
    public static void CustomEntryMethod1()
    {
    }

    // [2] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])
    public static void CustomEntryMethod2()
    {
    }

    // [3] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)
    public static void CustomEntryMethod3()
    {
    }

    // [4] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)
    public static void CustomEntryMethod4()
    {
    }

    // [5] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)
    public static void CustomEntryMethod5()
    {
    }

    // [6] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)
    public static void CustomEntryMethod6()
    {
    }

    // [7] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])
    public static void CustomEntryMethod7()
    {
    }

    // [8] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)
    public static void CustomEntryMethod8()
    {
    }

    // [9] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)
    public static void CustomEntryMethod9()
    {
    }

    // [10] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    public static void CustomEntryMethod10()
    {
    }

    // [11] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    public static void CustomEntryMethod11()
    {
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
                case 3: CustomEntryMethod3(); break;
                case 4: CustomEntryMethod4(); break;
                case 5: CustomEntryMethod5(); break;
                case 6: CustomEntryMethod6(); break;
                case 7: CustomEntryMethod7(); break;
                case 8: CustomEntryMethod8(); break;
                case 9: CustomEntryMethod9(); break;
                case 10: CustomEntryMethod10(); break;
                case 11: CustomEntryMethod11(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}