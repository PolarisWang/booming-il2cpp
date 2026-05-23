// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/boxing/unboxing/casts
// Assembly: System.Private.CoreLib
// Variant: subjects

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
        try { if (((Convert.ChangeType(null!, typeof(byte))).GetHashCode()) != ((Convert.ChangeType(null!, typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)
    public static void Subject_1()
    {
        try { if (((Convert.ChangeType(null!, typeof(byte), null)).GetHashCode()) != ((Convert.ChangeType(null!, typeof(byte), null)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
    public static void Subject_2()
    {
        try { if (((typeof(byte).IsAssignableFrom(typeof(byte))) ? 1 : 0) != ((typeof(byte).IsAssignableFrom(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)
    public static void Subject_3()
    {
        try { if (((typeof(byte).IsInstanceOfType(null!)) ? 1 : 0) != ((typeof(byte).IsInstanceOfType(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
    public static void Subject_4()
    {
        try { if (((typeof(byte).IsSubclassOf(typeof(byte))) ? 1 : 0) != ((typeof(byte).IsSubclassOf(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)
    public static void Subject_5()
    {
        try { if (((typeof(byte).IsAssignableTo(typeof(byte))) ? 1 : 0) != ((typeof(byte).IsAssignableTo(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
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
        try { if (((RuntimeHelpers.GetObjectValue(null!)).GetHashCode()) != ((RuntimeHelpers.GetObjectValue(null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static void Subject_8()
    {
        try { if ((int)(((Array)(Array.CreateInstance(typeof(byte), 42))).Length) != (int)(((Array)(Array.CreateInstance(typeof(byte), 42))).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    public static void Subject_9()
    {
        try { if ((int)(((Array)(Array.CreateInstance(typeof(byte), 42, 42))).Length) != (int)(((Array)(Array.CreateInstance(typeof(byte), 42, 42))).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    public static void Subject_10()
    {
        try { if (((((ValueType)42).Equals(null!)) ? 1 : 0) != ((((ValueType)42).Equals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}