// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/converters
// Assembly: System.Text.Json
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Serialization;

public static partial class ConvertersNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)
    public static void Method0()
    {
        try { if (((JsonConverter.CanConvert(typeof(byte))) ? 1 : 0) != ((JsonConverter.CanConvert(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Serialization.JsonConverter::get_Type:System.Type()
    public static void Method1()
    {
        try { if (((JsonConverter.Type).GetHashCode()) != ((JsonConverter.Type).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::CanConvert:System.Boolean(System.Type)
    public static void Method2()
    {
        try { if (((JsonConverter.CanConvert(typeof(byte))) ? 1 : 0) != ((JsonConverter.CanConvert(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::get_HandleNull:System.Boolean()
    public static void Method3()
    {
        try { if (((JsonConverter.HandleNull) ? 1 : 0) != ((JsonConverter.HandleNull) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::get_Type:System.Type()
    public static void Method4()
    {
        try { if (((JsonConverter.Type).GetHashCode()) != ((JsonConverter.Type).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::Read:T(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void Method5()
    {
    var refLocal_0 = null!;
        try { if ((int)(JsonConverter.Read(ref refLocal_0, typeof(byte), null!)) != (int)(JsonConverter.Read(ref refLocal_0, typeof(byte), null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::ReadAsPropertyName:T(System.Text.Json.Utf8JsonReader&,System.Type,System.Text.Json.JsonSerializerOptions)
    public static void Method6()
    {
    var refLocal_0 = null!;
        try { if ((int)(JsonConverter.ReadAsPropertyName(ref refLocal_0, typeof(byte), null!)) != (int)(JsonConverter.ReadAsPropertyName(ref refLocal_0, typeof(byte), null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::Write:System.Void(System.Text.Json.Utf8JsonWriter,T,System.Text.Json.JsonSerializerOptions)
    public static void Method7()
    {
        // TODO: System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::Write:System.Void(System.Text.Json.Utf8JsonWriter,T,System.Text.Json.JsonSerializerOptions) could not be auto-generated
    }

    // [8] System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::WriteAsPropertyName:System.Void(System.Text.Json.Utf8JsonWriter,T,System.Text.Json.JsonSerializerOptions)
    public static void Method8()
    {
        // TODO: System.Text.Json/System.Text.Json.Serialization.JsonConverter`1::WriteAsPropertyName:System.Void(System.Text.Json.Utf8JsonWriter,T,System.Text.Json.JsonSerializerOptions) could not be auto-generated
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
                case 8: Method8(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}