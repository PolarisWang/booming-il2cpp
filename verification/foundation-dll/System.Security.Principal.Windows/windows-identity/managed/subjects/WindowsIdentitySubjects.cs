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
        try { default(WindowsIdentity)!.Clone(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::Dispose:System.Void()
    public static void Subject_1()
    {
        try { default(WindowsIdentity)!.Dispose(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_AccessToken:Microsoft.Win32.SafeHandles.SafeAccessTokenHandle()
    public static void Subject_2()
    {
        try { _ = default(WindowsIdentity)!.AccessToken; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_AuthenticationType:System.String()
    public static void Subject_3()
    {
        try { _ = default(WindowsIdentity)!.AuthenticationType; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Claims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Subject_4()
    {
        try { _ = default(WindowsIdentity)!.Claims; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_DeviceClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Subject_5()
    {
        try { _ = default(WindowsIdentity)!.DeviceClaims; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Groups:System.Security.Principal.IdentityReferenceCollection()
    public static void Subject_6()
    {
        try { _ = default(WindowsIdentity)!.Groups; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_ImpersonationLevel:System.Security.Principal.TokenImpersonationLevel()
    public static void Subject_7()
    {
        try { _ = default(WindowsIdentity)!.ImpersonationLevel; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [8] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsAnonymous:System.Boolean()
    public static void Subject_8()
    {
        try { _ = default(WindowsIdentity)!.IsAnonymous; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [9] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsAuthenticated:System.Boolean()
    public static void Subject_9()
    {
        try { _ = default(WindowsIdentity)!.IsAuthenticated; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [10] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsGuest:System.Boolean()
    public static void Subject_10()
    {
        try { _ = default(WindowsIdentity)!.IsGuest; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [11] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_IsSystem:System.Boolean()
    public static void Subject_11()
    {
        try { _ = default(WindowsIdentity)!.IsSystem; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [12] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Name:System.String()
    public static void Subject_12()
    {
        try { _ = default(WindowsIdentity)!.Name; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [13] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Owner:System.Security.Principal.SecurityIdentifier()
    public static void Subject_13()
    {
        try { _ = default(WindowsIdentity)!.Owner; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [14] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_Token:System.IntPtr()
    public static void Subject_14()
    {
        try { _ = default(WindowsIdentity)!.Token; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [15] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_User:System.Security.Principal.SecurityIdentifier()
    public static void Subject_15()
    {
        try { _ = default(WindowsIdentity)!.User; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [16] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::get_UserClaims:System.Collections.Generic.IEnumerable{System.Security.Claims.Claim}()
    public static void Subject_16()
    {
        try { _ = default(WindowsIdentity)!.UserClaims; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [17] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetAnonymous:System.Security.Principal.WindowsIdentity()
    public static void Subject_17()
    {
        if (((WindowsIdentity.GetAnonymous()).GetHashCode()) != 7880838) _exitCode = 1;
    }

    // [18] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity()
    public static void Subject_18()
    {
        if (((WindowsIdentity.GetCurrent()).GetHashCode()) != 56793269) _exitCode = 1;
    }

    // [19] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity(System.Boolean)
    public static void Subject_19()
    {
        try { WindowsIdentity.GetCurrent(true); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [20] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::GetCurrent:System.Security.Principal.WindowsIdentity(System.Security.Principal.TokenAccessLevels)
    public static void Subject_20()
    {
        try { WindowsIdentity.GetCurrent(true); _exitCode = 1; }
        catch (NullReferenceException) { }
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
        try { new WindowsIdentity(IntPtr.Zero); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [26] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String)
    public static void Subject_26()
    {
        try { new WindowsIdentity(IntPtr.Zero, "hello"); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [27] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String,System.Security.Principal.WindowsAccountType)
    public static void Subject_27()
    {
        try { new WindowsIdentity(IntPtr.Zero, "hello", default); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [28] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.IntPtr,System.String,System.Security.Principal.WindowsAccountType,System.Boolean)
    public static void Subject_28()
    {
        try { new WindowsIdentity(IntPtr.Zero, "hello", default, true); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [29] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.String)
    public static void Subject_29()
    {
        try { new WindowsIdentity("hello"); _exitCode = 1; }
        catch (System.Security.SecurityException) { }
    }

    // [30] System.Security.Principal.Windows/System.Security.Principal.WindowsIdentity::.ctor:System.Void(System.Runtime.Serialization.SerializationInfo,System.Runtime.Serialization.StreamingContext)
    public static void Subject_30()
    {
        try { new WindowsIdentity(null!, default); _exitCode = 1; }
        catch (PlatformNotSupportedException) { }
    }

}