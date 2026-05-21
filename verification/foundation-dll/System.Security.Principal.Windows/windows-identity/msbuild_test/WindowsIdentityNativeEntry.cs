// Auto-generated native-AOT entry point
// Family: family/System.Security.Principal.Windows/windows-identity
// Assembly: System.Security.Principal.Windows
// Variant: benchmark

using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Security.Claims;
using System.Security.Principal;
using System.Threading.Tasks;

public static partial class WindowsIdentityNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Clone:System.Security.Claims.ClaimsIdentity()
    public static void Method0()
    {
        try { if ((int)(WindowsIdentity.Clone()) != (int)(WindowsIdentity.Clone())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Dispose:System.Void()
    public static void Method1()
    {
        try { WindowsIdentity.Dispose(); }
        catch { _exitCode = 1; }
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_AccessToken:Microsoft.Win32.SafeHandles.SafeAccessTokenHandle()
    public static void Method2()
    {
        try { if ((int)(WindowsIdentity.AccessToken) != (int)(WindowsIdentity.AccessToken)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_AuthenticationType:System.String()
    public static void Method3()
    {
        try { if (((WindowsIdentity.AuthenticationType).Length) != ((WindowsIdentity.AuthenticationType).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Claims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Method4()
    {
        try { if ((int)(WindowsIdentity.Claims) != (int)(WindowsIdentity.Claims)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_DeviceClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Method5()
    {
        try { if ((int)(WindowsIdentity.DeviceClaims) != (int)(WindowsIdentity.DeviceClaims)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Groups:System.Security.Principal.IdentityReferenceCollection()
    public static void Method6()
    {
        try { if ((int)(WindowsIdentity.Groups) != (int)(WindowsIdentity.Groups)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_ImpersonationLevel:System.Security.Principal.TokenImpersonationLevel()
    public static void Method7()
    {
        try { if ((int)(WindowsIdentity.ImpersonationLevel) != (int)(WindowsIdentity.ImpersonationLevel)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsAnonymous:System.Boolean()
    public static void Method8()
    {
        try { if (((WindowsIdentity.IsAnonymous) ? 1 : 0) != ((WindowsIdentity.IsAnonymous) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsAuthenticated:System.Boolean()
    public static void Method9()
    {
        try { if (((WindowsIdentity.IsAuthenticated) ? 1 : 0) != ((WindowsIdentity.IsAuthenticated) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsGuest:System.Boolean()
    public static void Method10()
    {
        try { if (((WindowsIdentity.IsGuest) ? 1 : 0) != ((WindowsIdentity.IsGuest) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsSystem:System.Boolean()
    public static void Method11()
    {
        try { if (((WindowsIdentity.IsSystem) ? 1 : 0) != ((WindowsIdentity.IsSystem) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Name:System.String()
    public static void Method12()
    {
        try { if (((WindowsIdentity.Name).Length) != ((WindowsIdentity.Name).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Owner:System.Security.Principal.SecurityIdentifier()
    public static void Method13()
    {
        try { if ((int)(WindowsIdentity.Owner) != (int)(WindowsIdentity.Owner)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Token:System.IntPtr()
    public static void Method14()
    {
        try { if ((int)(WindowsIdentity.Token) != (int)(WindowsIdentity.Token)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_User:System.Security.Principal.SecurityIdentifier()
    public static void Method15()
    {
        try { if ((int)(WindowsIdentity.User) != (int)(WindowsIdentity.User)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_UserClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Method16()
    {
        try { if ((int)(WindowsIdentity.UserClaims) != (int)(WindowsIdentity.UserClaims)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetAnonymous:System.Security.Principal.WindowsIdentity()
    public static void Method17()
    {
        try { if ((int)(WindowsIdentity.GetAnonymous()) != (int)(WindowsIdentity.GetAnonymous())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity()
    public static void Method18()
    {
        try { if ((int)(WindowsIdentity.GetCurrent()) != (int)(WindowsIdentity.GetCurrent())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity(System.Boolean)
    public static void Method19()
    {
        try { if ((int)(WindowsIdentity.GetCurrent(true)) != (int)(WindowsIdentity.GetCurrent(true))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity(System.Security.Principal.TokenAccessLevels)
    public static void Method20()
    {
        try { if ((int)(WindowsIdentity.GetCurrent(null!)) != (int)(WindowsIdentity.GetCurrent(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonated:System.Void(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Action)

    // [22] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonated{T}:T(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Func{T})

    // [23] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonatedAsync:System.Threading.Tasks.Task(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Func{System.Threading.Tasks.Task})

    // [24] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonatedAsync{T}:System.Threading.Tasks.Task{T}(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Func{System.Threading.Tasks.Task{T}})

    // [25] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr)
    public static void Method25()
    {
        try { new WindowsIdentity(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [26] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String)
    public static void Method26()
    {
        try { new WindowsIdentity(IntPtr.Zero, "hello"); }
        catch { _exitCode = 1; }
    }

    // [27] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String,System.Security.Principal.WindowsAccountType)
    public static void Method27()
    {
        try { new WindowsIdentity(IntPtr.Zero, "hello", null!); }
        catch { _exitCode = 1; }
    }

    // [28] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String,System.Security.Principal.WindowsAccountType,System.Boolean)
    public static void Method28()
    {
        try { new WindowsIdentity(IntPtr.Zero, "hello", null!, true); }
        catch { _exitCode = 1; }
    }

    // [29] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.String)
    public static void Method29()
    {
        try { new WindowsIdentity("hello"); }
        catch { _exitCode = 1; }
    }

    // [30] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.Runtime.Serialization.SerializationInfo,System.Runtime.Serialization.StreamingContext)
    public static void Method30()
    {
        try { new WindowsIdentity(null!, null!); }
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
                case 8: Method8(); break;
                case 9: Method9(); break;
                case 10: Method10(); break;
                case 11: Method11(); break;
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
                case 16: Method16(); break;
                case 17: Method17(); break;
                case 18: Method18(); break;
                case 19: Method19(); break;
                case 20: Method20(); break;
                case 21: CustomEntryMethod21(); break;
                case 22: CustomEntryMethod22(); break;
                case 23: CustomEntryMethod23(); break;
                case 24: CustomEntryMethod24(); break;
                case 25: Method25(); break;
                case 26: Method26(); break;
                case 27: Method27(); break;
                case 28: Method28(); break;
                case 29: Method29(); break;
                case 30: Method30(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}