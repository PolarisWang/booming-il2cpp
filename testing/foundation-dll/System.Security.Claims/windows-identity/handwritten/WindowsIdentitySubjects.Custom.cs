// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class WindowsIdentitySubjects
{
    // [24] System.Security.Claims/System.Security.Claims.ClaimsPrincipal::get_Identity:System.Security.Principal.IIdentity()
    [Fact]
    public static void CustomEntrySubject_24()
    {        try { _ = ClaimsPrincipal.Identity; } catch { }    }

    // [25] System.Security.Claims/System.Security.Claims.ClaimsPrincipal::get_Claims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    [Fact]
    public static void CustomEntrySubject_25()
    {        try { _ = ClaimsPrincipal.Claims; } catch { }    }

    // [26] System.Security.Claims/System.Security.Claims.ClaimsPrincipal::AddIdentity:System.Void(System.Security.Claims.ClaimsIdentity)
    [Fact]
    public static void CustomEntrySubject_26()
    {        try { ClaimsPrincipal.AddIdentity(default); } catch { }    }

    // [27] System.Security.Claims/System.Security.Claims.ClaimsPrincipal::IsInRole:System.Boolean(System.String)
    [Fact]
    public static void CustomEntrySubject_27()
    {        try { _ = ClaimsPrincipal.IsInRole("hello"); } catch { }    }

    // [28] System.Security.Claims/System.Security.Claims.ClaimsPrincipal::HasClaim:System.Boolean(System.String,System.String)
    [Fact]
    public static void CustomEntrySubject_28()
    {        try { _ = ClaimsPrincipal.HasClaim("hello", "hello"); } catch { }    }

    // [31] System.Security.Claims/System.Security.Principal.GenericIdentity::get_Name:System.String()
    [Fact]
    public static void CustomEntrySubject_31()
    {        try { _ = GenericIdentity.Name; } catch { }    }

    // [32] System.Security.Claims/System.Security.Principal.GenericIdentity::get_AuthenticationType:System.String()
    [Fact]
    public static void CustomEntrySubject_32()
    {        try { _ = GenericIdentity.AuthenticationType; } catch { }    }

    // [33] System.Security.Claims/System.Security.Principal.GenericIdentity::get_IsAuthenticated:System.Boolean()
    [Fact]
    public static void CustomEntrySubject_33()
    {        try { _ = GenericIdentity.IsAuthenticated; } catch { }    }

    // [35] System.Security.Claims/System.Security.Principal.GenericPrincipal::get_Identity:System.Security.Principal.IIdentity()
    [Fact]
    public static void CustomEntrySubject_35()
    {        try { _ = GenericPrincipal.Identity; } catch { }    }

    // [36] System.Security.Claims/System.Security.Principal.GenericPrincipal::IsInRole:System.Boolean(System.String)
    [Fact]
    public static void CustomEntrySubject_36()
    {        try { _ = GenericPrincipal.IsInRole("hello"); } catch { }    }

}
