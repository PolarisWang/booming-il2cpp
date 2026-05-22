// Auto-generated native-AOT entry point
// Family: family/System.Security.Principal.Windows/security/identifier
// Assembly: System.Security.Principal.Windows
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Principal;

public static partial class SecurityIdentifierSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::CompareTo:System.Int32(System.Security.Principal.SecurityIdentifier)
    public static void Subject_0()
    {
        try { if (default(SecurityIdentifier)!.CompareTo(default) != default(SecurityIdentifier)!.CompareTo(default)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Equals:System.Boolean(System.Object)
    public static void Subject_1()
    {
        try { if (((default(SecurityIdentifier)!.Equals(42)) ? 1 : 0) != ((default(SecurityIdentifier)!.Equals(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Equals:System.Boolean(System.Security.Principal.SecurityIdentifier)
    public static void Subject_2()
    {
        try { if (((default(SecurityIdentifier)!.Equals(default)) ? 1 : 0) != ((default(SecurityIdentifier)!.Equals(default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_AccountDomainSid:System.Security.Principal.SecurityIdentifier()
    public static void Subject_3()
    {
        try { if (((default(SecurityIdentifier)!.AccountDomainSid).GetHashCode()) != ((default(SecurityIdentifier)!.AccountDomainSid).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_BinaryLength:System.Int32()
    public static void Subject_4()
    {
        try { if (default(SecurityIdentifier)!.BinaryLength != default(SecurityIdentifier)!.BinaryLength) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Value:System.String()
    public static void Subject_5()
    {
        try { if (((default(SecurityIdentifier)!.Value).Length) != ((default(SecurityIdentifier)!.Value).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::GetBinaryForm:System.Void(System.Byte{},System.Int32)
    public static void Subject_6()
    {
        try { default(SecurityIdentifier)!.GetBinaryForm(default, 42); }
        catch { _exitCode = 1; }
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::GetHashCode:System.Int32()
    public static void Subject_7()
    {
        try { if (default(SecurityIdentifier)!.GetHashCode() != default(SecurityIdentifier)!.GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsAccountSid:System.Boolean()
    public static void Subject_8()
    {
        try { if (((default(SecurityIdentifier)!.IsAccountSid()) ? 1 : 0) != ((default(SecurityIdentifier)!.IsAccountSid()) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsEqualDomainSid:System.Boolean(System.Security.Principal.SecurityIdentifier)
    public static void Subject_9()
    {
        try { if (((default(SecurityIdentifier)!.IsEqualDomainSid(default)) ? 1 : 0) != ((default(SecurityIdentifier)!.IsEqualDomainSid(default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsValidTargetType:System.Boolean(System.Type)
    public static void Subject_10()
    {
        try { if (((default(SecurityIdentifier)!.IsValidTargetType(typeof(byte))) ? 1 : 0) != ((default(SecurityIdentifier)!.IsValidTargetType(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsWellKnown:System.Boolean(System.Security.Principal.WellKnownSidType)
    public static void Subject_11()
    {
        try { if (((default(SecurityIdentifier)!.IsWellKnown(default)) ? 1 : 0) != ((default(SecurityIdentifier)!.IsWellKnown(default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::op_Equality:System.Boolean(System.Security.Principal.SecurityIdentifier,System.Security.Principal.SecurityIdentifier)
    public static void Subject_12()
    {
        try { if (((default(SecurityIdentifier)! == default(SecurityIdentifier)!) ? 1 : 0) != ((default(SecurityIdentifier)! == default(SecurityIdentifier)!) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::op_Inequality:System.Boolean(System.Security.Principal.SecurityIdentifier,System.Security.Principal.SecurityIdentifier)
    public static void Subject_13()
    {
        try { if (((default(SecurityIdentifier)! != default(SecurityIdentifier)!) ? 1 : 0) != ((default(SecurityIdentifier)! != default(SecurityIdentifier)!) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::ToString:System.String()
    public static void Subject_14()
    {
        try { if (((default(SecurityIdentifier)!.ToString()).Length) != ((default(SecurityIdentifier)!.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Translate:System.Security.Principal.IdentityReference(System.Type)
    public static void Subject_15()
    {
        try { if (((default(SecurityIdentifier)!.Translate(typeof(byte))).GetHashCode()) != ((default(SecurityIdentifier)!.Translate(typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.String)
    public static void Subject_16()
    {
        try { new SecurityIdentifier("hello"); }
        catch { _exitCode = 1; }
    }

    // [17] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.Byte{},System.Int32)
    public static void Subject_17()
    {
        try { new SecurityIdentifier(System.Security.Principal.WellKnownSidType.WorldSid, null); }
        catch { _exitCode = 1; }
    }

    // [18] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.IntPtr)
    public static void Subject_18()
    {
        try { new SecurityIdentifier(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [19] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.Security.Principal.WellKnownSidType,System.Security.Principal.SecurityIdentifier)
    public static void Subject_19()
    {
        try { new SecurityIdentifier(System.Security.Principal.WellKnownSidType.WorldSid, null); }
        catch { _exitCode = 1; }
    }

}