// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/converter/factory
// Assembly: System.Text.Json
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Serialization;

public static partial class ConverterFactorySubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,System.Text.Json.JsonSerializerOptions)
    public static void Subject_0()
    {
        try { if (((default(JsonConverterFactory)!.CreateConverter(typeof(byte), null!)).GetHashCode()) != ((default(JsonConverterFactory)!.CreateConverter(typeof(byte), null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_Type:System.Type()
    public static void Subject_1()
    {
        try { if (((default(JsonConverterFactory)!.Type).GetHashCode()) != ((default(JsonConverterFactory)!.Type).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}