// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/boxing/unboxing-casts
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;

public static partial class BoxingUnboxingCastsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)
    public static void Subject_0()
    {
        try { _ = ((Convert.ChangeType(null!, typeof(byte))).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)
    public static void Subject_1()
    {
        try { _ = ((Convert.ChangeType(null!, typeof(byte), null)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
    public static void Subject_2()
    {
        try { _ = ((typeof(System.IConvertible).IsAssignableFrom(typeof(int))) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)
    public static void Subject_3()
    {
        try { _ = ((typeof(string).IsInstanceOfType("hello")) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
    public static void Subject_4()
    {
        try { _ = ((typeof(System.MemberAccessException).IsSubclassOf(typeof(System.Exception))) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)
    public static void Subject_5()
    {
        try { _ = ((typeof(System.IConvertible).IsAssignableTo(typeof(int))) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    public static void Subject_6()
    {
        // needs-manual — MemberwiseClone requires manual implementation: System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    }

    // [7] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
    public static void Subject_7()
    {
        try { _ = ((RuntimeHelpers.GetObjectValue(null!)).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static void Subject_8()
    {
        try { _ = (int)(((Array)(Array.CreateInstance(typeof(byte), 42))).Length); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    public static void Subject_9()
    {
        try { _ = (int)(((Array)(Array.CreateInstance(typeof(byte), 42, 42))).Length); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    public static void Subject_10()
    {
        try { _ = ((((ValueType)42).Equals(null!)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

}