// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/activation
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Reflection;

public static partial class ReflectionActivationSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)
    public static void Subject_0()
    {
        try { var _ = ((Activator.CreateInstance(typeof(byte))).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])
    public static void Subject_1()
    {
        try { var _ = ((Activator.CreateInstance(typeof(byte), Array.Empty<System.Object>())).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Activator::CreateInstance:T()
    public static void Subject_2()
    {
        // needs-manual — CreateInstance with 0 params requires manual implementation: System.Private.CoreLib/System.Activator::CreateInstance:T()
    }

    // [3] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    public static void Subject_3()
    {
        try { var _ = ((Activator.CreateInstance(typeof(byte), System.Reflection.BindingFlags.Default, default, Array.Empty<System.Object>(), default)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static void Subject_4()
    {
        try { var _ = (int)(((Array)(Array.CreateInstance(typeof(byte), 42))).Length); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    public static void Subject_5()
    {
        try { var _ = (int)(((Array)(Array.CreateInstance(typeof(byte), 42, 42))).Length); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])
    public static void Subject_6()
    {
        try { var _ = (int)(((Array)(Array.CreateInstance(typeof(byte), Array.Empty<System.Int32>()))).Length); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
    public static void Subject_7()
    {
        try { var _ = ((typeof(byte).GetConstructors()[0].Invoke(new object[0])).GetHashCode()); }
        catch { _exitCode = 1; }
    }

}