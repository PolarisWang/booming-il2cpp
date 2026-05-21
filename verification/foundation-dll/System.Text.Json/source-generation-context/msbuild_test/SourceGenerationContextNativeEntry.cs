// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/source-generation-context
// Assembly: System.Text.Json
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Text.Json.Serialization.Metadata;

public static partial class SourceGenerationContextNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Options:System.Text.Json.JsonSerializerOptions()
    public static void Method0()
    {
        try { if ((int)(JsonSerializerContext.Options) != (int)(JsonSerializerContext.Options)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)
    public static void Method1()
    {
        try { if ((int)(JsonSerializerContext.GetTypeInfo(typeof(byte))) != (int)(JsonSerializerContext.GetTypeInfo(typeof(byte)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}