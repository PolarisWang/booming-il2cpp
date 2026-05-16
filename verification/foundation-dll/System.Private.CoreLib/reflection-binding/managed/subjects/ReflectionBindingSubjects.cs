// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/binding
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Globalization;
using System.Reflection;

public static partial class ReflectionBindingSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)
    public static void Subject_0()
    {
        if (((typeof(byte).GetMethod("hello")).GetHashCode()) != ((typeof(byte).GetMethod("hello")).GetHashCode())) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)
    public static void Subject_1()
    {
        if (((typeof(byte).GetMethod("hello", System.Reflection.BindingFlags.Default)).GetHashCode()) != ((typeof(byte).GetMethod("hello", System.Reflection.BindingFlags.Default)).GetHashCode())) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])
    public static void Subject_2()
    {
        if (((typeof(byte).GetMethod("hello", Array.Empty<System.Type>())).GetHashCode()) != ((typeof(byte).GetMethod("hello", Array.Empty<System.Type>())).GetHashCode())) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)
    public static void Subject_3()
    {
        if (((typeof(byte).GetField("hello")).GetHashCode()) != ((typeof(byte).GetField("hello")).GetHashCode())) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)
    public static void Subject_4()
    {
        if (((typeof(byte).GetField("hello", System.Reflection.BindingFlags.Default)).GetHashCode()) != ((typeof(byte).GetField("hello", System.Reflection.BindingFlags.Default)).GetHashCode())) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)
    public static void Subject_5()
    {
        if (((typeof(byte).GetProperty("hello")).GetHashCode()) != ((typeof(byte).GetProperty("hello")).GetHashCode())) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)
    public static void Subject_6()
    {
        if (((typeof(byte).GetProperty("hello", System.Reflection.BindingFlags.Default)).GetHashCode()) != ((typeof(byte).GetProperty("hello", System.Reflection.BindingFlags.Default)).GetHashCode())) _exitCode = 1;
    }

    // [7] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])
    public static void Subject_7()
    {
        if (((typeof(byte).GetProperty("hello", Array.Empty<System.Type>())).GetHashCode()) != ((typeof(byte).GetProperty("hello", Array.Empty<System.Type>())).GetHashCode())) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)
    public static void Subject_8()
    {
        if (((typeof(byte).GetEvent("hello")).GetHashCode()) != ((typeof(byte).GetEvent("hello")).GetHashCode())) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)
    public static void Subject_9()
    {
        if (((typeof(byte).GetEvent("hello", System.Reflection.BindingFlags.Default)).GetHashCode()) != ((typeof(byte).GetEvent("hello", System.Reflection.BindingFlags.Default)).GetHashCode())) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    public static void Subject_10()
    {
        // TODO: System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[]) could not be auto-generated
    }

    // [11] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    public static void Subject_11()
    {
        if (((default(MethodInfo)!.Invoke(42, System.Reflection.BindingFlags.Default, null!, Array.Empty<System.Object>(), null!)).GetHashCode()) != ((default(MethodInfo)!.Invoke(42, System.Reflection.BindingFlags.Default, null!, Array.Empty<System.Object>(), null!)).GetHashCode())) _exitCode = 1;
    }

    public static void Run(int entryIndex)
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

}