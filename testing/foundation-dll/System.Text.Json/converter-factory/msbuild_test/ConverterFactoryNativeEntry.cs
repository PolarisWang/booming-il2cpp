// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/converter-factory
// Assembly: System.Text.Json
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Serialization;

public static partial class ConverterFactoryNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,System.Text.Json.JsonSerializerOptions)
    public static void Method0()
    {
        try { if ((int)(JsonConverterFactory.CreateConverter(typeof(byte), null!)) != (int)(JsonConverterFactory.CreateConverter(typeof(byte), null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_Type:System.Type()
    public static void Method1()
    {
        try { if (((JsonConverterFactory.Type).GetHashCode()) != ((JsonConverterFactory.Type).GetHashCode())) _exitCode = 1; }
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