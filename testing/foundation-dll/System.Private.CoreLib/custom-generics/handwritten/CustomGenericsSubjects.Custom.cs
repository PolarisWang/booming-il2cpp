// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class CustomGenericsSubjects
{
    // [0] System.Private.CoreLib/CustomGenerics.Box`1::Get:T()
    [Fact]
    public static void CustomEntrySubject_0()
    {
        _ = Box.Get();
    }

    // [1] System.Private.CoreLib/CustomGenerics.Box`1::Set:(T)
    [Fact]
    public static void CustomEntrySubject_1()
    {
        Box.Set(default);
    }

    // [2] System.Private.CoreLib/CustomGenerics.Pair`2::Deconstruct:(T1&,T2&)
    [Fact]
    public static void CustomEntrySubject_2()
    {
        Pair.Deconstruct(out default, out default);
    }

    // [3] System.Private.CoreLib/CustomGenerics.Util::Echo:T(T)
    [Fact]
    public static void CustomEntrySubject_3()
    {
        _ = Util.Echo(default);
    }

    // [4] System.Private.CoreLib/CustomGenerics.Util::Equal:bool(T,T)
    [Fact]
    public static void CustomEntrySubject_4()
    {
        _ = Util.Equal(default, default);
    }

    // [5] System.Private.CoreLib/CustomGenerics.Wrapper`1::GetRefType:System.Type()
    [Fact]
    public static void CustomEntrySubject_5()
    {
        _ = Wrapper.GetRefType();
    }

}
