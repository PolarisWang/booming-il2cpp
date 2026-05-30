// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class BinderTypeStyleSubjects
{
    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToName:System.Void(System.Type,System.String&,System.String&)
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { default(SerializationBinder)!.BindToName(typeof(byte), out "hello", out "hello"); } catch { }    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToType:System.Type(System.String,System.String)
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { _ = default(SerializationBinder)!.BindToType("hello", "hello"); } catch { }    }

}
