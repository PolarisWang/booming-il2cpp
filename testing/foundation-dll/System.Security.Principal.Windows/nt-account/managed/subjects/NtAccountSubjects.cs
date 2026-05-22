// Auto-generated native-AOT entry point
// Family: family/System.Security.Principal.Windows/nt/account
// Assembly: System.Security.Principal.Windows
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Principal;

public static partial class NtAccountSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Security.Principal.Windows/System.Security.Principal.NTAccount::Equals:System.Boolean(System.Object)
    public static void Subject_0()
    {
        try { if (((default(NTAccount)!.Equals(42)) ? 1 : 0) != ((default(NTAccount)!.Equals(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_Value:System.String()
    public static void Subject_1()
    {
        try { if (((default(NTAccount)!.Value).Length) != ((default(NTAccount)!.Value).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.NTAccount::GetHashCode:System.Int32()
    public static void Subject_2()
    {
        try { if (default(NTAccount)!.GetHashCode() != default(NTAccount)!.GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.NTAccount::IsValidTargetType:System.Boolean(System.Type)
    public static void Subject_3()
    {
        try { if (((default(NTAccount)!.IsValidTargetType(typeof(byte))) ? 1 : 0) != ((default(NTAccount)!.IsValidTargetType(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.NTAccount::op_Equality:System.Boolean(System.Security.Principal.NTAccount,System.Security.Principal.NTAccount)
    public static void Subject_4()
    {
        try { if (((default(NTAccount)! == default(NTAccount)!) ? 1 : 0) != ((default(NTAccount)! == default(NTAccount)!) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.NTAccount::op_Inequality:System.Boolean(System.Security.Principal.NTAccount,System.Security.Principal.NTAccount)
    public static void Subject_5()
    {
        try { if (((default(NTAccount)! != default(NTAccount)!) ? 1 : 0) != ((default(NTAccount)! != default(NTAccount)!) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.NTAccount::ToString:System.String()
    public static void Subject_6()
    {
        try { if (((default(NTAccount)!.ToString()).Length) != ((default(NTAccount)!.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.NTAccount::Translate:System.Security.Principal.IdentityReference(System.Type)
    public static void Subject_7()
    {
        try { if (((default(NTAccount)!.Translate(typeof(byte))).GetHashCode()) != ((default(NTAccount)!.Translate(typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Security.Principal.Windows/System.Security.Principal.NTAccount::.ctor:System.Void(System.String,System.String)
    public static void Subject_8()
    {
        try { new NTAccount("hello", "hello"); }
        catch { _exitCode = 1; }
    }

    // [9] System.Security.Principal.Windows/System.Security.Principal.NTAccount::.ctor:System.Void(System.String)
    public static void Subject_9()
    {
        try { new NTAccount("hello"); }
        catch { _exitCode = 1; }
    }

}