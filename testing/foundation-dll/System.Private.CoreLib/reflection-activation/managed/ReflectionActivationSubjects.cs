// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/activation
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

public static partial class ReflectionActivationSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)
    public static void Subject_0()
    {
        // non-callable: System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)
    }

    // [1] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])
    public static void Subject_1()
    {
        // non-callable: System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])
    }

    // [2] System.Private.CoreLib/System.Activator::CreateInstance:T()
    public static void Subject_2()
    {
        // needs-manual — CreateInstance with 0 params requires manual implementation: System.Private.CoreLib/System.Activator::CreateInstance:T()
    }

    // [3] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    public static void Subject_3()
    {
        // non-callable: System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static void Subject_4()
    {
        try { _ = (int)(((Array)(Array.CreateInstance(typeof(byte), 42))).Length); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    public static void Subject_5()
    {
        try { _ = (int)(((Array)(Array.CreateInstance(typeof(byte), 42, 42))).Length); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])
    public static void Subject_6()
    {
        try { _ = (int)(((Array)(Array.CreateInstance(typeof(byte), Array.Empty<System.Int32>()))).Length); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
    public static void Subject_7()
    {
        try { _ = ((typeof(byte).GetConstructors()[0].Invoke(new object[0])).GetHashCode()); }
        catch { _exitCode = 1; }
    }

}