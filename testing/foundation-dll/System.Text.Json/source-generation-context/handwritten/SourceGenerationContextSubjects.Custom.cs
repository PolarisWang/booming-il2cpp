// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class SourceGenerationContextSubjects
{
    // [0] System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Options:System.Text.Json.JsonSerializerOptions()
    public static void CustomEntrySubject_0()
    {        try { _ = default(JsonSerializerContext)!.Options; } catch { _exitCode = 1; }    }

    // [1] System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)
    public static void CustomEntrySubject_1()
    {        try { _ = default(JsonSerializerContext)!.GetTypeInfo(typeof(byte)); } catch { _exitCode = 1; }    }

}
