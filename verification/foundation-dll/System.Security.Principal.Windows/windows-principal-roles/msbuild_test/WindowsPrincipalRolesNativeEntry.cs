// Auto-generated native-AOT entry point
// Family: family/System.Security.Principal.Windows/windows-principal-roles
// Assembly: System.Security.Principal.Windows
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Principal;

public static partial class WindowsPrincipalRolesNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_DeviceClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Method0()
    {
        try { if ((int)(WindowsPrincipal.DeviceClaims) != (int)(WindowsPrincipal.DeviceClaims)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_Identity:System.Security.Principal.IIdentity()
    public static void Method1()
    {
        try { if ((int)(WindowsPrincipal.Identity) != (int)(WindowsPrincipal.Identity)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::get_UserClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Method2()
    {
        try { if ((int)(WindowsPrincipal.UserClaims) != (int)(WindowsPrincipal.UserClaims)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.String)
    public static void Method3()
    {
        try { if (((WindowsPrincipal.IsInRole("hello")) ? 1 : 0) != ((WindowsPrincipal.IsInRole("hello")) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Security.Principal.WindowsBuiltInRole)
    public static void Method4()
    {
        try { if (((WindowsPrincipal.IsInRole(null!)) ? 1 : 0) != ((WindowsPrincipal.IsInRole(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Int32)
    public static void Method5()
    {
        try { if (((WindowsPrincipal.IsInRole(42)) ? 1 : 0) != ((WindowsPrincipal.IsInRole(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::IsInRole:System.Boolean(System.Security.Principal.SecurityIdentifier)
    public static void Method6()
    {
        try { if (((WindowsPrincipal.IsInRole(null!)) ? 1 : 0) != ((WindowsPrincipal.IsInRole(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.WindowsPrincipal::.ctor:System.Void(System.Security.Principal.WindowsIdentity)
    public static void Method7()
    {
        try { new WindowsPrincipal(null!); }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}