// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class ReaderSubjects
{
    // [0] System.Text.Json/System.Text.Json.Utf8JsonReader::CopyString:System.Int32(System.Span{System.Byte})
    public static void CustomEntrySubject_0()
    {
        try {
            Assert.IsNotNull(default(Utf8JsonReader).CopyString(default(Span<byte>)));
        }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Utf8JsonReader::CopyString:System.Int32(System.Span{System.Char})
    public static void CustomEntrySubject_1()
    {
        try {
            Assert.IsNotNull(default(Utf8JsonReader).CopyString(default(Span<char>)));
        }
        catch { _exitCode = 1; }
    }

    // [56] System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueSpan:System.Void(System.ReadOnlySpan{System.Byte})
    public static void CustomEntrySubject_56()
    {
        try {
            default(Utf8JsonReader).ValueSpan = default(ReadOnlySpan<byte>);
        }
        catch { _exitCode = 1; }
    }

    // [57] System.Text.Json/System.Text.Json.Utf8JsonReader::set_TokenStartIndex:System.Void(System.Int64)
    public static void CustomEntrySubject_57()
    {
        try {
            default(Utf8JsonReader).TokenStartIndex = 42L;
        }
        catch { _exitCode = 1; }
    }

    // [58] System.Text.Json/System.Text.Json.Utf8JsonReader::set_HasValueSequence:System.Void(System.Boolean)
    public static void CustomEntrySubject_58()
    {
        try {
            default(Utf8JsonReader).HasValueSequence = true;
        }
        catch { _exitCode = 1; }
    }

    // [59] System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueIsEscaped:System.Void(System.Boolean)
    public static void CustomEntrySubject_59()
    {
        try {
            default(Utf8JsonReader).ValueIsEscaped = true;
        }
        catch { _exitCode = 1; }
    }

    // [60] System.Text.Json/System.Text.Json.Utf8JsonReader::set_ValueSequence:System.Void(System.Buffers.ReadOnlySequence{System.Byte})
    public static void CustomEntrySubject_60()
    {
        try {
            default(Utf8JsonReader).ValueSequence = default(ReadOnlySequence<byte>);
        }
        catch { _exitCode = 1; }
    }

}
