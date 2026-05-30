// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class SourceGenerationContextSubjects
{
    // [0] System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Options:System.Text.Json.JsonSerializerOptions()
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { _ = default(JsonSerializerContext)!.Options; } catch { }    }

    // [1] System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { _ = default(JsonSerializerContext)!.GetTypeInfo(typeof(byte)); } catch { }    }

}
