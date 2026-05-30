// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class PolymorphismReferenceSubjects
{
    // [0] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()
    public static void CustomEntrySubject_0()
    {        try { _ = ReferenceHandler.IgnoreCycles.CreateResolver(); } catch { _exitCode = 1; }    }

    // [3] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()
    public static void CustomEntrySubject_3()
    {        try { _ = ReferenceHandler.IgnoreCycles.CreateResolver(); } catch { _exitCode = 1; }    }

    // [4] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::.ctor:System.Void()
    public static void CustomEntrySubject_4()
    {        try { new ReferenceHandler<byte>(); } catch { _exitCode = 1; }    }

    // [5] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object)
    public static void CustomEntrySubject_5()
    {        try { default(ReferenceResolver)!.AddReference("hello", null!); } catch { _exitCode = 1; }    }

    // [6] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean&)
    public static void CustomEntrySubject_6()
    {        try { _ = default(ReferenceResolver)!.GetReference(null!, out true); } catch { _exitCode = 1; }    }

    // [7] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String)
    public static void CustomEntrySubject_7()
    {        try { _ = default(ReferenceResolver)!.ResolveReference("hello"); } catch { _exitCode = 1; }    }

}
