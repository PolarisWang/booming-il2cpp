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
        try { default(SecurityIdentifier)!.CompareTo(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Equals:System.Boolean(System.Object)
    public static void Subject_1()
    {
        try { default(SecurityIdentifier)!.Equals(42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Equals:System.Boolean(System.Security.Principal.SecurityIdentifier)
    public static void Subject_2()
    {
        try { default(SecurityIdentifier)!.Equals(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_AccountDomainSid:System.Security.Principal.SecurityIdentifier()
    public static void Subject_3()
    {
        try { _ = default(SecurityIdentifier)!.AccountDomainSid; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_BinaryLength:System.Int32()
    public static void Subject_4()
    {
        try { _ = default(SecurityIdentifier)!.BinaryLength; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Value:System.String()
    public static void Subject_5()
    {
        try { _ = default(SecurityIdentifier)!.Value; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::GetBinaryForm:System.Void(System.Byte{},System.Int32)
    public static void Subject_6()
    {
        try { default(SecurityIdentifier)!.GetBinaryForm(default, 42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::GetHashCode:System.Int32()
    public static void Subject_7()
    {
        try { default(SecurityIdentifier)!.GetHashCode(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [8] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsAccountSid:System.Boolean()
    public static void Subject_8()
    {
        try { default(SecurityIdentifier)!.IsAccountSid(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [9] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsEqualDomainSid:System.Boolean(System.Security.Principal.SecurityIdentifier)
    public static void Subject_9()
    {
        try { default(SecurityIdentifier)!.IsEqualDomainSid(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [10] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsValidTargetType:System.Boolean(System.Type)
    public static void Subject_10()
    {
        try { default(SecurityIdentifier)!.IsValidTargetType(typeof(byte)); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [11] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsWellKnown:System.Boolean(System.Security.Principal.WellKnownSidType)
    public static void Subject_11()
    {
        try { default(SecurityIdentifier)!.IsWellKnown(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [12] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::op_Equality:System.Boolean(System.Security.Principal.SecurityIdentifier,System.Security.Principal.SecurityIdentifier)
    public static void Subject_12()
    {
        if (((default(SecurityIdentifier)! == default(SecurityIdentifier)!) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [13] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::op_Inequality:System.Boolean(System.Security.Principal.SecurityIdentifier,System.Security.Principal.SecurityIdentifier)
    public static void Subject_13()
    {
        if (((default(SecurityIdentifier)! != default(SecurityIdentifier)!) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [14] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::ToString:System.String()
    public static void Subject_14()
    {
        try { default(SecurityIdentifier)!.ToString(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [15] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Translate:System.Security.Principal.IdentityReference(System.Type)
    public static void Subject_15()
    {
        try { default(SecurityIdentifier)!.Translate(typeof(byte)); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [16] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.String)
    public static void Subject_16()
    {
        try { new SecurityIdentifier("hello"); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [17] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.Byte{},System.Int32)
    public static void Subject_17()
    {
        new SecurityIdentifier(System.Security.Principal.WellKnownSidType.WorldSid, null);
    }

    // [18] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.IntPtr)
    public static void Subject_18()
    {
        try { new SecurityIdentifier(IntPtr.Zero); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [19] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.Security.Principal.WellKnownSidType,System.Security.Principal.SecurityIdentifier)
    public static void Subject_19()
    {
        new SecurityIdentifier(System.Security.Principal.WellKnownSidType.WorldSid, null);
    }

}