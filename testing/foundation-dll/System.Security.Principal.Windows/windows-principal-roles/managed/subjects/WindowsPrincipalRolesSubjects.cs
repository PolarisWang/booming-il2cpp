// Auto-generated native-AOT entry point
// Family: family/System.Security.Principal.Windows/windows/principal/roles
// Assembly: System.Security.Principal.Windows
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Principal;

public static partial class WindowsPrincipalRolesSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_DeviceClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Subject_0()
    {
        try { _ = default(WindowsPrincipal)!.DeviceClaims; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Identity:System.Security.Principal.IIdentity()
    public static void Subject_1()
    {
        try { _ = default(WindowsPrincipal)!.Identity; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_UserClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Subject_2()
    {
        try { _ = default(WindowsPrincipal)!.UserClaims; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.String)
    public static void Subject_3()
    {
        try { default(WindowsPrincipal)!.IsInRole("hello"); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Security.Principal.WindowsBuiltInRole)
    public static void Subject_4()
    {
        try { default(WindowsPrincipal)!.IsInRole(default(System.Security.Principal.SecurityIdentifier)); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Int32)
    public static void Subject_5()
    {
        try { default(WindowsPrincipal)!.IsInRole(42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Security.Principal.SecurityIdentifier)
    public static void Subject_6()
    {
        try { default(WindowsPrincipal)!.IsInRole(default(System.Security.Principal.SecurityIdentifier)); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::.ctor:System.Void(System.Security.Principal.WindowsIdentity)
    public static void Subject_7()
    {
        try { new WindowsPrincipal(null!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

}