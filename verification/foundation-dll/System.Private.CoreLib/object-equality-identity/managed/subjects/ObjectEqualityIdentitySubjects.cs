// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/object/equality/identity
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;

public static partial class ObjectEqualityIdentitySubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)
    public static void Subject_0()
    {
        if (((new object().Equals(42)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)
    public static void Subject_1()
    {
        if (((Object.Equals(42, 42)) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)
    public static void Subject_2()
    {
        if (((Object.ReferenceEquals(42, 42)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Object::GetHashCode:System.Int32()
    public static void Subject_3()
    {
        if (new object().GetHashCode() != 35342034) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Object::ToString:System.String()
    public static void Subject_4()
    {
        if (((new object().ToString()).Length) != 13) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Object::GetType:System.Type()
    public static void Subject_5()
    {
        if (((new object().GetType()).GetHashCode()) != 56793269) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    public static void Subject_6()
    {
        // TODO: System.Private.CoreLib/System.Object::MemberwiseClone:System.Object() could not be auto-generated
    }

}