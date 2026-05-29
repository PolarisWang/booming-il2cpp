// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/object/equality-identity
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;

public static partial class ObjectEqualityIdentitySubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)
    public static void CustomEntrySubject_0()
    {
    }

    // [1] System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)
    public static void CustomEntrySubject_1()
    {
    }

    // [2] System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)
    public static void Subject_2()
    {
        try { _ = ((Object.ReferenceEquals(null!, null!)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Object::GetHashCode:System.Int32()
    public static void CustomEntrySubject_3()
    {
    }

    // [4] System.Private.CoreLib/System.Object::ToString:System.String()
    public static void CustomEntrySubject_4()
    {
    }

    // [5] System.Private.CoreLib/System.Object::GetType:System.Type()
    public static void CustomEntrySubject_5()
    {
    }

    // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    public static void CustomEntrySubject_6()
    {
    }

}