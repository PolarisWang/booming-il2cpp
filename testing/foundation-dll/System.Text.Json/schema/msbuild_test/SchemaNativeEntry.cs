// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/schema
// Assembly: System.Text.Json
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Text.Json.Schema;
using System.Text.Json.Serialization.Metadata;

public static partial class SchemaNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::GetJsonSchemaAsNode:System.Text.Json.Nodes.JsonNode(System.Text.Json.JsonSerializerOptions,System.Type,System.Text.Json.Schema.JsonSchemaExporterOptions)
    public static void Method0()
    {
        try { if ((int)(JsonSchemaExporter.GetJsonSchemaAsNode(null!, typeof(byte), null!)) != (int)(JsonSchemaExporter.GetJsonSchemaAsNode(null!, typeof(byte), null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::GetJsonSchemaAsNode:System.Text.Json.Nodes.JsonNode(System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Text.Json.Schema.JsonSchemaExporterOptions)
    public static void Method1()
    {
        try { if ((int)(JsonSchemaExporter.GetJsonSchemaAsNode(null!, null!)) != (int)(JsonSchemaExporter.GetJsonSchemaAsNode(null!, null!))) _exitCode = 1; }
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