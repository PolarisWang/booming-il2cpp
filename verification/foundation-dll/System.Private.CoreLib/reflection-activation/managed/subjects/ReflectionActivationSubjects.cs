// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/activation
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Reflection;

public static partial class ReflectionActivationSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)
    public static void Subject_0()
    {
        if (((Activator.CreateInstance(typeof(byte))).GetHashCode()) != 0) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])
    public static void Subject_1()
    {
        if (((Activator.CreateInstance(typeof(byte), Array.Empty<System.Object>())).GetHashCode()) != 0) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Activator::CreateInstance:T()
    public static void Subject_2()
    {
        // TODO: System.Private.CoreLib/System.Activator::CreateInstance:T() could not be auto-generated
    }

    // [3] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    public static void Subject_3()
    {
        if (((Activator.CreateInstance(typeof(byte), System.Reflection.BindingFlags.Default, null!, Array.Empty<System.Object>(), null!)).GetHashCode()) != 0) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static void Subject_4()
    {
        if (((Array.CreateInstance(typeof(byte), 42)).GetHashCode()) != 35342034) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    public static void Subject_5()
    {
        if (((Array.CreateInstance(typeof(byte), 42, 42)).GetHashCode()) != 56793269) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])
    public static void Subject_6()
    {
        try { Array.CreateInstance(typeof(byte), Array.Empty<System.Int32>()); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [7] System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
    public static void Subject_7()
    {
        try { typeof(byte).GetConstructors()[0].Invoke(new object[0]); _exitCode = 1; }
        catch (IndexOutOfRangeException) { }
    }

}