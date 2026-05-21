// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/security/identifier
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class SecurityIdentifierPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::CompareTo:System.Int32(System.Security.Principal.SecurityIdentifier)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Equals:System.Boolean(System.Object)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Equals:System.Boolean(System.Security.Principal.SecurityIdentifier)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_AccountDomainSid:System.Security.Principal.SecurityIdentifier()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_BinaryLength:System.Int32()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::get_Value:System.String()
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::GetBinaryForm:System.Void(System.Byte{},System.Int32)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::GetHashCode:System.Int32()
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsAccountSid:System.Boolean()
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsEqualDomainSid:System.Boolean(System.Security.Principal.SecurityIdentifier)
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsValidTargetType:System.Boolean(System.Type)
    public static int Subject_10()
    {
        return unchecked((int)(0xB0000000u + 10));
    }

    // [11] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::IsWellKnown:System.Boolean(System.Security.Principal.WellKnownSidType)
    public static int Subject_11()
    {
        return unchecked((int)(0xB0000000u + 11));
    }

    // [12] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::op_Equality:System.Boolean(System.Security.Principal.SecurityIdentifier,System.Security.Principal.SecurityIdentifier)
    public static int Subject_12()
    {
        return unchecked((int)(0xB0000000u + 12));
    }

    // [13] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::op_Inequality:System.Boolean(System.Security.Principal.SecurityIdentifier,System.Security.Principal.SecurityIdentifier)
    public static int Subject_13()
    {
        return unchecked((int)(0xB0000000u + 13));
    }

    // [14] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::ToString:System.String()
    public static int Subject_14()
    {
        return unchecked((int)(0xB0000000u + 14));
    }

    // [15] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::Translate:System.Security.Principal.IdentityReference(System.Type)
    public static int Subject_15()
    {
        return unchecked((int)(0xB0000000u + 15));
    }

    // [16] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.String)
    public static int Subject_16()
    {
        return unchecked((int)(0xB0000000u + 16));
    }

    // [17] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.Byte{},System.Int32)
    public static int Subject_17()
    {
        return unchecked((int)(0xB0000000u + 17));
    }

    // [18] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.IntPtr)
    public static int Subject_18()
    {
        return unchecked((int)(0xB0000000u + 18));
    }

    // [19] System.Security.Principal.Windows/System.Security.Principal.SecurityIdentifier::.ctor:System.Void(System.Security.Principal.WellKnownSidType,System.Security.Principal.SecurityIdentifier)
    public static int Subject_19()
    {
        return unchecked((int)(0xB0000000u + 19));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
                case 2: Subject_2(); break;
                case 3: Subject_3(); break;
                case 4: Subject_4(); break;
                case 5: Subject_5(); break;
                case 6: Subject_6(); break;
                case 7: Subject_7(); break;
                case 8: Subject_8(); break;
                case 9: Subject_9(); break;
                case 10: Subject_10(); break;
                case 11: Subject_11(); break;
                case 12: Subject_12(); break;
                case 13: Subject_13(); break;
                case 14: Subject_14(); break;
                case 15: Subject_15(); break;
                case 16: Subject_16(); break;
                case 17: Subject_17(); break;
                case 18: Subject_18(); break;
                case 19: Subject_19(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}