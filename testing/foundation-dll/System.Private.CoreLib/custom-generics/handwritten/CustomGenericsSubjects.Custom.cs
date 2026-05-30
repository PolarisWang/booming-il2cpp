// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class CustomGenericsSubjects
{
    // [0] System.Private.CoreLib/CustomGenerics.Box`1::Get:T()
    public static void CustomEntrySubject_0()
    {        try { _ = Box.Get(); } catch { _exitCode = 1; }    }

    // [1] System.Private.CoreLib/CustomGenerics.Box`1::Set:(T)
    public static void CustomEntrySubject_1()
    {        try { Box.Set(default); } catch { _exitCode = 1; }    }

    // [2] System.Private.CoreLib/CustomGenerics.Pair`2::Deconstruct:(T1&,T2&)
    public static void CustomEntrySubject_2()
    {        try { Pair.Deconstruct(out default, out default); } catch { _exitCode = 1; }    }

    // [3] System.Private.CoreLib/CustomGenerics.Util::Echo:T(T)
    public static void CustomEntrySubject_3()
    {        try { _ = Util.Echo(default); } catch { _exitCode = 1; }    }

    // [4] System.Private.CoreLib/CustomGenerics.Util::Equal:bool(T,T)
    public static void CustomEntrySubject_4()
    {        try { _ = Util.Equal(default, default); } catch { _exitCode = 1; }    }

    // [5] System.Private.CoreLib/CustomGenerics.Wrapper`1::GetRefType:System.Type()
    public static void CustomEntrySubject_5()
    {        try { _ = Wrapper.GetRefType(); } catch { _exitCode = 1; }    }

}
