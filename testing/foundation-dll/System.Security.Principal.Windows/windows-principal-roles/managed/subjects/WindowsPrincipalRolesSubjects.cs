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
        try { if (((default(WindowsPrincipal)!.DeviceClaims).GetHashCode()) != ((default(WindowsPrincipal)!.DeviceClaims).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Identity:System.Security.Principal.IIdentity()
    public static void Subject_1()
    {
        try { if (((default(WindowsPrincipal)!.Identity).GetHashCode()) != ((default(WindowsPrincipal)!.Identity).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_UserClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Subject_2()
    {
        try { if (((default(WindowsPrincipal)!.UserClaims).GetHashCode()) != ((default(WindowsPrincipal)!.UserClaims).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.String)
    public static void Subject_3()
    {
        try { if (((default(WindowsPrincipal)!.IsInRole("hello")) ? 1 : 0) != ((default(WindowsPrincipal)!.IsInRole("hello")) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Security.Principal.WindowsBuiltInRole)
    public static void Subject_4()
    {
        try { if (((default(WindowsPrincipal)!.IsInRole(default(System.Security.Principal.SecurityIdentifier))) ? 1 : 0) != ((default(WindowsPrincipal)!.IsInRole(default(System.Security.Principal.SecurityIdentifier))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Int32)
    public static void Subject_5()
    {
        try { if (((default(WindowsPrincipal)!.IsInRole(42)) ? 1 : 0) != ((default(WindowsPrincipal)!.IsInRole(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Security.Principal.SecurityIdentifier)
    public static void Subject_6()
    {
        try { if (((default(WindowsPrincipal)!.IsInRole(default(System.Security.Principal.SecurityIdentifier))) ? 1 : 0) != ((default(WindowsPrincipal)!.IsInRole(default(System.Security.Principal.SecurityIdentifier))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::.ctor:System.Void(System.Security.Principal.WindowsIdentity)
    public static void Subject_7()
    {
        try { new WindowsPrincipal(null!); }
        catch { _exitCode = 1; }
    }

}