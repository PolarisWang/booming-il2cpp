// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class DocumentElementSubjects
{
    // [8] System.Text.Json/System.Text.Json.JsonDocument::ParseValue:System.Text.Json.JsonDocument(System.Text.Json.Utf8JsonReader&)
    [Fact]
    public static void CustomEntrySubject_8()
    {
        try {
            Assert.IsNotNull(JsonDocument.ParseValue(out default));
        }
        catch { }
    }

    // [9] System.Text.Json/System.Text.Json.JsonDocument::TryParseValue:System.Boolean(System.Text.Json.Utf8JsonReader&,System.Text.Json.JsonDocument&)
    [Fact]
    public static void CustomEntrySubject_9()
    {
        try {
            Assert.IsNotNull(JsonDocument.TryParseValue(out default, out default));
        }
        catch { }
    }

    // [43] System.Text.Json/System.Text.Json.JsonElement::ParseValue:System.Text.Json.JsonElement(System.Text.Json.Utf8JsonReader&)
    [Fact]
    public static void CustomEntrySubject_43()
    {
        try {
            Assert.IsNotNull(JsonElement.ParseValue(out default));
        }
        catch { }
    }

    // [63] System.Text.Json/System.Text.Json.JsonElement::TryParseValue:System.Boolean(System.Text.Json.Utf8JsonReader&,System.Nullable{System.Text.Json.JsonElement}&)
    [Fact]
    public static void CustomEntrySubject_63()
    {
        try {
            Assert.IsNotNull(JsonElement.TryParseValue(out default, out default));
        }
        catch { }
    }

    // [68] System.Text.Json/System.Text.Json.JsonElement::get_Item:System.Text.Json.JsonElement()
    [Fact]
    public static void CustomEntrySubject_68()
    {
        try {
            Assert.IsNotNull(default(JsonElement)[default]);
        }
        catch { }
    }

}
