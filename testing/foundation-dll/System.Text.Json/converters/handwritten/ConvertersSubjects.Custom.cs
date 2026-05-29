// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class ConvertersSubjects
{
    // [5] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::Read:T(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_5()
    {        try { _ = default(JsonConverter)!.Read(out default, typeof(byte), null!); } catch { _exitCode = 1; }    }

    // [6] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::ReadAsPropertyName:T(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void CustomEntrySubject_6()
    {        try { _ = default(JsonConverter)!.ReadAsPropertyName(out default, typeof(byte), null!); } catch { _exitCode = 1; }    }

}
