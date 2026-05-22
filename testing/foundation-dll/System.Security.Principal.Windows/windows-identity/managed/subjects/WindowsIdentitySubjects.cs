// Auto-generated native-AOT entry point
// Family: family/System.Security.Principal.Windows/windows/identity
// Assembly: System.Security.Principal.Windows
// Variant: subjects

using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Security.Claims;
using System.Security.Principal;
using System.Threading.Tasks;

public static partial class WindowsIdentitySubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Clone:System.Security.Claims.ClaimsIdentity()
    public static void Subject_0()
    {
        try { if (((default(WindowsIdentity)!.Clone()).GetHashCode()) != ((default(WindowsIdentity)!.Clone()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Dispose:System.Void()
    public static void Subject_1()
    {
        try { default(WindowsIdentity)!.Dispose(); }
        catch { _exitCode = 1; }
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_AccessToken:Microsoft.Win32.SafeHandles.SafeAccessTokenHandle()
    public static void Subject_2()
    {
        try { if (((default(WindowsIdentity)!.AccessToken).GetHashCode()) != ((default(WindowsIdentity)!.AccessToken).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_AuthenticationType:System.String()
    public static void Subject_3()
    {
        try { if (((default(WindowsIdentity)!.AuthenticationType).Length) != ((default(WindowsIdentity)!.AuthenticationType).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Claims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Subject_4()
    {
        try { if (((default(WindowsIdentity)!.Claims).GetHashCode()) != ((default(WindowsIdentity)!.Claims).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_DeviceClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Subject_5()
    {
        try { if (((default(WindowsIdentity)!.DeviceClaims).GetHashCode()) != ((default(WindowsIdentity)!.DeviceClaims).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Groups:System.Security.Principal.IdentityReferenceCollection()
    public static void Subject_6()
    {
        try { if (((default(WindowsIdentity)!.Groups).GetHashCode()) != ((default(WindowsIdentity)!.Groups).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_ImpersonationLevel:System.Security.Principal.TokenImpersonationLevel()
    public static void Subject_7()
    {
        try { if (((default(WindowsIdentity)!.ImpersonationLevel).GetHashCode()) != ((default(WindowsIdentity)!.ImpersonationLevel).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsAnonymous:System.Boolean()
    public static void Subject_8()
    {
        try { if (((default(WindowsIdentity)!.IsAnonymous) ? 1 : 0) != ((default(WindowsIdentity)!.IsAnonymous) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsAuthenticated:System.Boolean()
    public static void Subject_9()
    {
        try { if (((default(WindowsIdentity)!.IsAuthenticated) ? 1 : 0) != ((default(WindowsIdentity)!.IsAuthenticated) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsGuest:System.Boolean()
    public static void Subject_10()
    {
        try { if (((default(WindowsIdentity)!.IsGuest) ? 1 : 0) != ((default(WindowsIdentity)!.IsGuest) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsSystem:System.Boolean()
    public static void Subject_11()
    {
        try { if (((default(WindowsIdentity)!.IsSystem) ? 1 : 0) != ((default(WindowsIdentity)!.IsSystem) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Name:System.String()
    public static void Subject_12()
    {
        try { if (((default(WindowsIdentity)!.Name).Length) != ((default(WindowsIdentity)!.Name).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Owner:System.Security.Principal.SecurityIdentifier()
    public static void Subject_13()
    {
        try { if (((default(WindowsIdentity)!.Owner).GetHashCode()) != ((default(WindowsIdentity)!.Owner).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Token:System.IntPtr()
    public static void Subject_14()
    {
        try { if ((int)(default(WindowsIdentity)!.Token) != (int)(default(WindowsIdentity)!.Token)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_User:System.Security.Principal.SecurityIdentifier()
    public static void Subject_15()
    {
        try { if (((default(WindowsIdentity)!.User).GetHashCode()) != ((default(WindowsIdentity)!.User).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_UserClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Subject_16()
    {
        try { if (((default(WindowsIdentity)!.UserClaims).GetHashCode()) != ((default(WindowsIdentity)!.UserClaims).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetAnonymous:System.Security.Principal.WindowsIdentity()
    public static void Subject_17()
    {
        try { if (((WindowsIdentity.GetAnonymous()).GetHashCode()) != ((WindowsIdentity.GetAnonymous()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity()
    public static void Subject_18()
    {
        try { if (((WindowsIdentity.GetCurrent()).GetHashCode()) != ((WindowsIdentity.GetCurrent()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity(System.Boolean)
    public static void Subject_19()
    {
        try { if (((WindowsIdentity.GetCurrent(true)).GetHashCode()) != ((WindowsIdentity.GetCurrent(true)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity(System.Security.Principal.TokenAccessLevels)
    public static void Subject_20()
    {
        try { if (((WindowsIdentity.GetCurrent(true)).GetHashCode()) != ((WindowsIdentity.GetCurrent(true)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonated:System.Void(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Action)
    public static void CustomEntrySubject_21()
    {
    }

    // [22] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonated{T}:T(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Func{T})
    public static void CustomEntrySubject_22()
    {
    }

    // [23] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonatedAsync:System.Threading.Tasks.Task(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Func{System.Threading.Tasks.Task})
    public static void CustomEntrySubject_23()
    {
    }

    // [24] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::RunImpersonatedAsync{T}:System.Threading.Tasks.Task{T}(Microsoft.Win32.SafeHandles.SafeAccessTokenHandle,System.Func{System.Threading.Tasks.Task{T}})
    public static void CustomEntrySubject_24()
    {
    }

    // [25] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr)
    public static void Subject_25()
    {
        try { new WindowsIdentity(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [26] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String)
    public static void Subject_26()
    {
        try { new WindowsIdentity(IntPtr.Zero, "hello"); }
        catch { _exitCode = 1; }
    }

    // [27] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String,System.Security.Principal.WindowsAccountType)
    public static void Subject_27()
    {
        try { new WindowsIdentity(IntPtr.Zero, "hello", default); }
        catch { _exitCode = 1; }
    }

    // [28] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String,System.Security.Principal.WindowsAccountType,System.Boolean)
    public static void Subject_28()
    {
        try { new WindowsIdentity(IntPtr.Zero, "hello", default, true); }
        catch { _exitCode = 1; }
    }

    // [29] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.String)
    public static void Subject_29()
    {
        try { new WindowsIdentity("hello"); }
        catch { _exitCode = 1; }
    }

    // [30] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.Runtime.Serialization.SerializationInfo,System.Runtime.Serialization.StreamingContext)
    public static void Subject_30()
    {
        try { new WindowsIdentity(null!, default); }
        catch { _exitCode = 1; }
    }

}