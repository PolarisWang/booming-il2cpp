// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/source/generation/context
// Assembly: System.Text.Json
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Text.Json.Serialization.Metadata;

public static partial class SourceGenerationContextSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Options:System.Text.Json.JsonSerializerOptions()
    public static void Subject_0()
    {
        try { if (((default(JsonSerializerContext)!.Options).GetHashCode()) != ((default(JsonSerializerContext)!.Options).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)
    public static void Subject_1()
    {
        try { if (((default(JsonSerializerContext)!.GetTypeInfo(typeof(byte))).GetHashCode()) != ((default(JsonSerializerContext)!.GetTypeInfo(typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}