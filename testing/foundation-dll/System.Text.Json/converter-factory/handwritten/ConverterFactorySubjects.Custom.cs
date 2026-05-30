// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class ConverterFactorySubjects
{
    // [0] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,System.Text.Json.JsonSerializerOptions)
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { _ = default(JsonConverterFactory)!.CreateConverter(typeof(byte), null!); } catch { }    }

    // [1] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_Type:System.Type()
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { _ = default(JsonConverterFactory)!.Type; } catch { }    }

}
