// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/generic/custom
// Assembly: System.Private.CoreLib
// Variant: subjects

using CustomGenerics;
using System;
using System.Collections.Generic;
using System.Linq;

public static partial class GenericCustomSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/CustomGenerics.Box`1::Get:T()
    public static void Subject_0()
    {
        try { if (((Box.Get()).GetHashCode()) != ((Box.Get()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/CustomGenerics.Box`1::Set:(T)
    public static void Subject_1()
    {
        // non-callable: System.Private.CoreLib/CustomGenerics.Box`1::Set:(T)
    }

    // [2] System.Private.CoreLib/CustomGenerics.Pair`2::Deconstruct:(T1&,T2&)
    public static void Subject_2()
    {
            T1 refLocal_0 = default;
            T2 refLocal_1 = default;
        try { Pair.Deconstruct(out refLocal_0, out refLocal_1); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/CustomGenerics.Util::Echo:T(T)
    public static void Subject_3()
    {
        // non-callable: System.Private.CoreLib/CustomGenerics.Util::Echo:T(T)
    }

    // [4] System.Private.CoreLib/CustomGenerics.Util::Equal:bool(T,T)
    public static void Subject_4()
    {
        // non-callable: System.Private.CoreLib/CustomGenerics.Util::Equal:bool(T,T)
    }

    // [5] System.Private.CoreLib/CustomGenerics.Wrapper`1::GetRefType:System.Type()
    public static void Subject_5()
    {
        try { if (((Wrapper.GetRefType()).GetHashCode()) != ((Wrapper.GetRefType()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}