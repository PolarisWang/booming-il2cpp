// Auto-generated native-AOT entry point
// Family: family/System.Security.Principal.Windows/security-identifier
// Assembly: System.Security.Principal.Windows
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Principal;

public static partial class SecurityIdentifierNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::CompareTo:System.Int32(System.Security.Principal.SecurityIdentifier)
    public static void Method0()
    {
        try { if (SecurityIdentifier.CompareTo(null!) != SecurityIdentifier.CompareTo(null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Equals:System.Boolean(System.Object)
    public static void Method1()
    {
        try { if (((SecurityIdentifier.Equals(42)) ? 1 : 0) != ((SecurityIdentifier.Equals(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Equals:System.Boolean(System.Security.Principal.SecurityIdentifier)
    public static void Method2()
    {
        try { if (((SecurityIdentifier.Equals(null!)) ? 1 : 0) != ((SecurityIdentifier.Equals(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_AccountDomainSid:System.Security.Principal.SecurityIdentifier()
    public static void Method3()
    {
        try { if ((int)(SecurityIdentifier.AccountDomainSid) != (int)(SecurityIdentifier.AccountDomainSid)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_BinaryLength:System.Int32()
    public static void Method4()
    {
        try { if (SecurityIdentifier.BinaryLength != SecurityIdentifier.BinaryLength) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Value:System.String()
    public static void Method5()
    {
        try { if (((SecurityIdentifier.Value).Length) != ((SecurityIdentifier.Value).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::GetBinaryForm:System.Void(System.Byte{},System.Int32)
    public static void Method6()
    {
        try { SecurityIdentifier.GetBinaryForm(null!, 42); }
        catch { _exitCode = 1; }
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::GetHashCode:System.Int32()
    public static void Method7()
    {
        try { if (SecurityIdentifier.GetHashCode() != SecurityIdentifier.GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsAccountSid:System.Boolean()
    public static void Method8()
    {
        try { if (((SecurityIdentifier.IsAccountSid()) ? 1 : 0) != ((SecurityIdentifier.IsAccountSid()) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsEqualDomainSid:System.Boolean(System.Security.Principal.SecurityIdentifier)
    public static void Method9()
    {
        try { if (((SecurityIdentifier.IsEqualDomainSid(null!)) ? 1 : 0) != ((SecurityIdentifier.IsEqualDomainSid(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsValidTargetType:System.Boolean(System.Type)
    public static void Method10()
    {
        try { if (((SecurityIdentifier.IsValidTargetType(typeof(byte))) ? 1 : 0) != ((SecurityIdentifier.IsValidTargetType(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsWellKnown:System.Boolean(System.Security.Principal.WellKnownSidType)
    public static void Method11()
    {
        try { if (((SecurityIdentifier.IsWellKnown(null!)) ? 1 : 0) != ((SecurityIdentifier.IsWellKnown(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::op_Equality:System.Boolean(System.Security.Principal.SecurityIdentifier,System.Security.Principal.SecurityIdentifier)
    public static void Method12()
    {
        try { if (((SecurityIdentifier.op_Equality(null!, null!)) ? 1 : 0) != ((SecurityIdentifier.op_Equality(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::op_Inequality:System.Boolean(System.Security.Principal.SecurityIdentifier,System.Security.Principal.SecurityIdentifier)
    public static void Method13()
    {
        try { if (((SecurityIdentifier.op_Inequality(null!, null!)) ? 1 : 0) != ((SecurityIdentifier.op_Inequality(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::ToString:System.String()
    public static void Method14()
    {
        try { if (((SecurityIdentifier.ToString()).Length) != ((SecurityIdentifier.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Translate:System.Security.Principal.IdentityReference(System.Type)
    public static void Method15()
    {
        try { if ((int)(SecurityIdentifier.Translate(typeof(byte))) != (int)(SecurityIdentifier.Translate(typeof(byte)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.String)
    public static void Method16()
    {
        try { new SecurityIdentifier("hello"); }
        catch { _exitCode = 1; }
    }

    // [17] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.Byte{},System.Int32)
    public static void Method17()
    {
        try { new SecurityIdentifier(null!, 42); }
        catch { _exitCode = 1; }
    }

    // [18] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.IntPtr)
    public static void Method18()
    {
        try { new SecurityIdentifier(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [19] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.Security.Principal.WellKnownSidType,System.Security.Principal.SecurityIdentifier)
    public static void Method19()
    {
        try { new SecurityIdentifier(null!, null!); }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}