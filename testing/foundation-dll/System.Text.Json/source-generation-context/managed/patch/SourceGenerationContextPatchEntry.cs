// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/source/generation/context
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class SourceGenerationContextPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::get_Options:System.Text.Json.JsonSerializerOptions()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Text.Json/System.Text.Json.Serialization.JsonSerializerContext::GetTypeInfo:System.Text.Json.Serialization.Metadata.JsonTypeInfo(System.Type)
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