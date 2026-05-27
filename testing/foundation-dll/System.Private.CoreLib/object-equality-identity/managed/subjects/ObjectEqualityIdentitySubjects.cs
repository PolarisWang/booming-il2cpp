// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/object/equality/identity
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class ObjectEqualityIdentitySubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)
    public static void Subject_0()
    {
        try { var _ = ((new object().Equals(null!)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)
    public static void Subject_1()
    {
        try { var _ = ((Object.Equals(null!, null!)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)
    public static void Subject_2()
    {
        try { var _ = ((Object.ReferenceEquals(null!, null!)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Object::GetHashCode:System.Int32()
    public static void Subject_3()
    {
        try { var _ = new object().GetHashCode(); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Object::ToString:System.String()
    public static void Subject_4()
    {
        try { var _ = ((new object().ToString()).Length); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Object::GetType:System.Type()
    public static void Subject_5()
    {
        try { var _ = ((new object().GetType()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    public static void Subject_6()
    {
        // needs-manual — MemberwiseClone requires manual implementation: System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    }

}