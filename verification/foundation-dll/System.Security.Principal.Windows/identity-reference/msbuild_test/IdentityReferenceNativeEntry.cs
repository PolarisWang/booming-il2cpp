// Auto-generated native-AOT entry point
// Family: family/System.Security.Principal.Windows/identity-reference
// Assembly: System.Security.Principal.Windows
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Principal;

public static partial class IdentityReferenceNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Equals:System.Boolean(System.Object)
    public static void Method0()
    {
        try { if (((default(IdentityReference)!.Equals(42)) ? 1 : 0) != ((default(IdentityReference)!.Equals(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::get_Value:System.String()
    public static void Method1()
    {
        try { if (((default(IdentityReference)!.Value).Length) != ((default(IdentityReference)!.Value).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::GetHashCode:System.Int32()
    public static void Method2()
    {
        try { if (default(IdentityReference)!.GetHashCode() != default(IdentityReference)!.GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::IsValidTargetType:System.Boolean(System.Type)
    public static void Method3()
    {
        try { if (((default(IdentityReference)!.IsValidTargetType(typeof(byte))) ? 1 : 0) != ((default(IdentityReference)!.IsValidTargetType(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::op_Equality:System.Boolean(System.Security.Principal.IdentityReference,System.Security.Principal.IdentityReference)
    public static void Method4()
    {
        try { if (((default(IdentityReference)!.op_Equality(null!, null!)) ? 1 : 0) != ((default(IdentityReference)!.op_Equality(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::op_Inequality:System.Boolean(System.Security.Principal.IdentityReference,System.Security.Principal.IdentityReference)
    public static void Method5()
    {
        try { if (((default(IdentityReference)!.op_Inequality(null!, null!)) ? 1 : 0) != ((default(IdentityReference)!.op_Inequality(null!, null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::ToString:System.String()
    public static void Method6()
    {
        try { if (((default(IdentityReference)!.ToString()).Length) != ((default(IdentityReference)!.ToString()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Security.Principal.Windows/System.Security.Principal.IdentityReference::Translate:System.Security.Principal.IdentityReference(System.Type)
    public static void Method7()
    {
        try { if ((int)(default(IdentityReference)!.Translate(typeof(byte))) != (int)(default(IdentityReference)!.Translate(typeof(byte)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Add:System.Void(System.Security.Principal.IdentityReference)
    public static void Method8()
    {
        try { default(IdentityReferenceCollection)!.Add(null!); }
        catch { _exitCode = 1; }
    }

    // [9] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Clear:System.Void()
    public static void Method9()
    {
        try { default(IdentityReferenceCollection)!.Clear(); }
        catch { _exitCode = 1; }
    }

    // [10] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Contains:System.Boolean(System.Security.Principal.IdentityReference)
    public static void Method10()
    {
        try { if (((default(IdentityReferenceCollection)!.Contains(null!)) ? 1 : 0) != ((default(IdentityReferenceCollection)!.Contains(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::CopyTo:System.Void(System.Security.Principal.IdentityReference{},System.Int32)
    public static void Method11()
    {
        try { default(IdentityReferenceCollection)!.CopyTo(null!, 42); }
        catch { _exitCode = 1; }
    }

    // [12] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Count:System.Int32()
    public static void Method12()
    {
        try { if (default(IdentityReferenceCollection)!.Count != default(IdentityReferenceCollection)!.Count) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Item:System.Security.Principal.IdentityReference(System.Int32)
    public static void Method13()
    {
        try { if ((int)(default(IdentityReferenceCollection)![42]) != (int)(default(IdentityReferenceCollection)![42])) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::GetEnumerator:System.Collections.Generic.IEnumerator{System.Security.Principal.IdentityReference}()
    public static void Method14()
    {
        try { if ((int)(default(IdentityReferenceCollection)!.GetEnumerator()) != (int)(default(IdentityReferenceCollection)!.GetEnumerator())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Remove:System.Boolean(System.Security.Principal.IdentityReference)
    public static void Method15()
    {
        try { if (((default(IdentityReferenceCollection)!.Remove(null!)) ? 1 : 0) != ((default(IdentityReferenceCollection)!.Remove(null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Item:System.Void(System.Int32,System.Security.Principal.IdentityReference)
    public static void Method16()
    {
        try { default(IdentityReferenceCollection)![42, null!]; }
        catch { _exitCode = 1; }
    }

    // [17] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Translate:System.Security.Principal.IdentityReferenceCollection(System.Type)
    public static void Method17()
    {
        try { if ((int)(default(IdentityReferenceCollection)!.Translate(typeof(byte))) != (int)(default(IdentityReferenceCollection)!.Translate(typeof(byte)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::Translate:System.Security.Principal.IdentityReferenceCollection(System.Type,System.Boolean)
    public static void Method18()
    {
        try { if ((int)(default(IdentityReferenceCollection)!.Translate(typeof(byte), true)) != (int)(default(IdentityReferenceCollection)!.Translate(typeof(byte), true))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::.ctor:System.Void()
    public static void Method19()
    {
        try { new IdentityReferenceCollection(); }
        catch { _exitCode = 1; }
    }

    // [20] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::.ctor:System.Void(System.Int32)
    public static void Method20()
    {
        try { new IdentityReferenceCollection(42); }
        catch { _exitCode = 1; }
    }

    // [21] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::get_Item:System.Security.Principal.IdentityReference()
    public static void Method21()
    {
        try { if ((int)(default(IdentityReferenceCollection)![]) != (int)(default(IdentityReferenceCollection)![])) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Security.Principal.Windows/System.Security.Principal.IdentityReferenceCollection::set_Item:System.Void(System.Security.Principal.IdentityReference)
    public static void Method22()
    {
        try { default(IdentityReferenceCollection)![null!]; }
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
                case 21: Method21(); break;
                case 22: Method22(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}