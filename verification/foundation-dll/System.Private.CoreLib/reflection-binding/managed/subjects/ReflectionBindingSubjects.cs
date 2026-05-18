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
        try { typeof(byte).GetMethod("hello"); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)
    public static void Subject_1()
    {
        try { typeof(byte).GetMethod("hello", System.Reflection.BindingFlags.Default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])
    public static void Subject_2()
    {
        try { typeof(byte).GetMethod("hello", Array.Empty<System.Type>()); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)
    public static void Subject_3()
    {
        try { typeof(byte).GetField("hello"); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)
    public static void Subject_4()
    {
        try { typeof(byte).GetField("hello", System.Reflection.BindingFlags.Default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)
    public static void Subject_5()
    {
        try { typeof(byte).GetProperty("hello"); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)
    public static void Subject_6()
    {
        try { typeof(byte).GetProperty("hello", System.Reflection.BindingFlags.Default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [7] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])
    public static void Subject_7()
    {
        try { typeof(byte).GetProperty("hello", Array.Empty<System.Type>()); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [8] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)
    public static void Subject_8()
    {
        try { typeof(byte).GetEvent("hello"); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [9] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)
    public static void Subject_9()
    {
        try { typeof(byte).GetEvent("hello", System.Reflection.BindingFlags.Default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [10] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    public static void Subject_10()
    {
        // TODO: System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[]) could not be auto-generated
    }

    // [11] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    public static void Subject_11()
    {
        try { default(MethodInfo)!.Invoke(42, System.Reflection.BindingFlags.Default, null!, Array.Empty<System.Object>(), null!); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

}