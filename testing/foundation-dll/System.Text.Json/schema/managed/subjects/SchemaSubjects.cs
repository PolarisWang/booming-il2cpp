// Auto-generated native-AOT entry point
// Family: family/System.Text.Json/schema
// Assembly: System.Text.Json
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Text.Json.Schema;
using System.Text.Json.Serialization.Metadata;

public static partial class SchemaSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::GetJsonSchemaAsNode:System.Text.Json.Nodes.JsonNode(System.Text.Json.JsonSerializerOptions,System.Type,System.Text.Json.Schema.JsonSchemaExporterOptions)
    public static void Subject_0()
    {
        try { JsonSchemaExporter.GetJsonSchemaAsNode(null!, typeof(byte), default); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [1] System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::GetJsonSchemaAsNode:System.Text.Json.Nodes.JsonNode(System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Text.Json.Schema.JsonSchemaExporterOptions)
    public static void Subject_1()
    {
        try { JsonSchemaExporter.GetJsonSchemaAsNode(null!, default); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

}