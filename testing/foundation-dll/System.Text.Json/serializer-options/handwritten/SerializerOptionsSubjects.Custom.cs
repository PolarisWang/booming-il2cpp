// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class SerializerOptionsSubjects
{
    // [6] System.Text.Json/System.Text.Json.JsonSerializerOptions::AddContext{TContext}:System.Void()
    [Fact]
    public static void CustomEntrySubject_6()
    {
        try {
            default(JsonSerializerOptions)!.AddContext<TContext>();
        }
        catch { }
    }

    // [71] System.Text.Json/System.Text.Json.JsonSerializerOptions::TryGetTypeInfo:System.Boolean(System.Type,System.Text.Json.Serialization.Metadata.JsonTypeInfo&)
    [Fact]
    public static void CustomEntrySubject_71()
    {
        try {
            Assert.IsNotNull(default(JsonSerializerOptions)!.TryGetTypeInfo(typeof(byte), out null!));
        }
        catch { }
    }

}
