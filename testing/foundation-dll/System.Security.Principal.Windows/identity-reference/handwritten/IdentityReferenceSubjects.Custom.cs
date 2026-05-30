// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class IdentityReferenceSubjects
{
    // [0] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Equals:System.Boolean(System.Object)
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { _ = default(IdentityReference)!.Equals(null!); } catch { }    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Value:System.String()
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { _ = default(IdentityReference)!.Value; } catch { }    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::GetHashCode:System.Int32()
    [Fact]
    public static void CustomEntrySubject_2()
    {        try { _ = default(IdentityReference)!.GetHashCode(); } catch { }    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::IsValidTargetType:System.Boolean(System.Type)
    [Fact]
    public static void CustomEntrySubject_3()
    {        try { _ = default(IdentityReference)!.IsValidTargetType(typeof(byte)); } catch { }    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::ToString:System.String()
    [Fact]
    public static void CustomEntrySubject_6()
    {        try { _ = default(IdentityReference)!.ToString(); } catch { }    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Translate:System.Security.Principal.IdentityReference(System.Type)
    [Fact]
    public static void CustomEntrySubject_7()
    {        try { _ = default(IdentityReference)!.Translate(typeof(byte)); } catch { }    }

    // [22] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Item:System.Void(System.Security.Principal.IdentityReference)
    [Fact]
    public static void CustomEntrySubject_22()
    {        try { new IdentityReferenceCollection()[] = default; } catch { }    }

}
