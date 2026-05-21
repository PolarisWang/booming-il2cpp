// Auto-generated native-AOT entry point
// Family: family/System.Security.Principal.Windows/nt-account
// Assembly: System.Security.Principal.Windows
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Principal;

public static partial class NtAccountNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Security.Principal.Windows/System.Security.Principal.NTAccount::Equals:System.Boolean(System.Object)
    public static void Method0()
    {
        try { if (((NTAccount.Equals(42)) ? 1 : 0) != ((NTAccount.Equals(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.NTAccount::get_Value:System.String()
    public static void Method1()
    {
        try { if (((NTAccount.Value).Length) != ((NTAccount.Value).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.NTAccount::GetHashCode:System.Int32()
    public static void Method2()
    {
        try { if (NTAccount.GetHashCode() != NTAccount.GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.NTAccount::IsValidTargetType:System.Boolean(System.Type)
    public static void Method3()
    {
        try { if (((NTAccount.IsValidTargetType(typeof(byte))) ? 1 : 0) != ((NTAccount.IsValidTargetType(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.NTAccount::op_Equality:System.Boolean(System.Security.Principal.NTAccount,System.Security.Principal.NTAccount)
    public static void Method4()
    {
        try { if (((NTAccount.op_Equality(null!, null!)) ? 1 : 0) != ((NTAccount.op_Equality(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.NTAccount::op_Inequality:System.Boolean(System.Security.Principal.NTAccount,System.Security.Principal.NTAccount)
    public static void Method5()
    {
        try { if (((NTAccount.op_Inequality(null!, null!)) ? 1 : 0) != ((NTAccount.op_Inequality(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.NTAccount::ToString:System.String()
    public static void Method6()
    {
        try { if (((NTAccount.ToString()).Length) != ((NTAccount.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.NTAccount::Translate:System.Security.Principal.IdentityReference(System.Type)
    public static void Method7()
    {
        try { if ((int)(NTAccount.Translate(typeof(byte))) != (int)(NTAccount.Translate(typeof(byte)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Security.Principal.Windows/System.Security.Principal.NTAccount::.ctor:System.Void(System.String,System.String)
    public static void Method8()
    {
        try { new NTAccount("hello", "hello"); }
        catch { _exitCode = 1; }
    }

    // [9] System.Security.Principal.Windows/System.Security.Principal.NTAccount::.ctor:System.Void(System.String)
    public static void Method9()
    {
        try { new NTAccount("hello"); }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}