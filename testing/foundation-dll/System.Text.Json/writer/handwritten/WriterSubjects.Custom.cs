// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class WriterSubjects
{
    // [124] System.Text.Json/System.Text.Json.Utf8JsonWriter::set_BytesPending:System.Void(System.Int32)
    [Fact]
    public static void CustomEntrySubject_124()
    {
        try {
            default(Utf8JsonWriter)!.BytesPending = 42;
        }
        catch { }
    }

    // [125] System.Text.Json/System.Text.Json.Utf8JsonWriter::set_BytesCommitted:System.Void(System.Int64)
    [Fact]
    public static void CustomEntrySubject_125()
    {
        try {
            default(Utf8JsonWriter)!.BytesCommitted = 42L;
        }
        catch { }
    }

}
