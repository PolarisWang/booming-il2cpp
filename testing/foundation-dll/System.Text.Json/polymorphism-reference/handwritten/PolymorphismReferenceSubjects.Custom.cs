// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class PolymorphismReferenceSubjects
{
    // [0] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { _ = ReferenceHandler.IgnoreCycles.CreateResolver(); } catch { }    }

    // [3] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()
    [Fact]
    public static void CustomEntrySubject_3()
    {        try { _ = ReferenceHandler.IgnoreCycles.CreateResolver(); } catch { }    }

    // [4] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::.ctor:System.Void()
    [Fact]
    public static void CustomEntrySubject_4()
    {        try { new ReferenceHandler<byte>(); } catch { }    }

    // [5] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object)
    [Fact]
    public static void CustomEntrySubject_5()
    {        try { default(ReferenceResolver)!.AddReference("hello", null!); } catch { }    }

    // [6] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean&)
    [Fact]
    public static void CustomEntrySubject_6()
    {        try { _ = default(ReferenceResolver)!.GetReference(null!, out true); } catch { }    }

    // [7] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String)
    [Fact]
    public static void CustomEntrySubject_7()
    {        try { _ = default(ReferenceResolver)!.ResolveReference("hello"); } catch { }    }

}
