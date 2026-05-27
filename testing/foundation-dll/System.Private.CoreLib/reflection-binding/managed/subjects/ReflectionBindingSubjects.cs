// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/binding
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Reflection;

public static partial class ReflectionBindingSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String)
    public static void Subject_0()
    {
        try { var _ = ((typeof(byte).GetMethod("hello")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Reflection.BindingFlags)
    public static void Subject_1()
    {
        try { var _ = ((typeof(byte).GetMethod("hello", System.Reflection.BindingFlags.Default)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Type::GetMethod:System.Reflection.MethodInfo(System.String,System.Type[])
    public static void Subject_2()
    {
        try { var _ = ((typeof(byte).GetMethod("hello", Array.Empty<System.Type>())).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String)
    public static void Subject_3()
    {
        try { var _ = ((typeof(byte).GetField("hello")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Type::GetField:System.Reflection.FieldInfo(System.String,System.Reflection.BindingFlags)
    public static void Subject_4()
    {
        try { var _ = ((typeof(byte).GetField("hello", System.Reflection.BindingFlags.Default)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String)
    public static void Subject_5()
    {
        try { var _ = ((typeof(byte).GetProperty("hello")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Reflection.BindingFlags)
    public static void Subject_6()
    {
        try { var _ = ((typeof(byte).GetProperty("hello", System.Reflection.BindingFlags.Default)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Type::GetProperty:System.Reflection.PropertyInfo(System.String,System.Type[])
    public static void Subject_7()
    {
        try { var _ = ((typeof(byte).GetProperty("hello", Array.Empty<System.Type>())).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String)
    public static void Subject_8()
    {
        try { var _ = ((typeof(byte).GetEvent("hello")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Type::GetEvent:System.Reflection.EventInfo(System.String,System.Reflection.BindingFlags)
    public static void Subject_9()
    {
        try { var _ = ((typeof(byte).GetEvent("hello", System.Reflection.BindingFlags.Default)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    public static void Subject_10()
    {
        // needs-manual — Invoke with 2 params requires manual implementation: System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Object[])
    }

    // [11] System.Private.CoreLib/System.Reflection.MethodBase::Invoke:System.Object(System.Object,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    public static void Subject_11()
    {
        try { var _ = ((default(MethodInfo)!.Invoke(null!, System.Reflection.BindingFlags.Default, default, Array.Empty<System.Object>(), default)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

}