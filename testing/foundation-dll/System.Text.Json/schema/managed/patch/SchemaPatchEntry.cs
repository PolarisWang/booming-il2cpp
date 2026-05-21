// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/schema
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class SchemaPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::GetJsonSchemaAsNode:System.Text.Json.Nodes.JsonNode(System.Text.Json.JsonSerializerOptions,System.Type,System.Text.Json.Schema.JsonSchemaExporterOptions)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::GetJsonSchemaAsNode:System.Text.Json.Nodes.JsonNode(System.Text.Json.Serialization.Metadata.JsonTypeInfo,System.Text.Json.Schema.JsonSchemaExporterOptions)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}