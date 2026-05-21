// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/polymorphism/reference
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class PolymorphismReferencePatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_IgnoreCycles:System.Text.Json.Serialization.ReferenceHandler()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::get_Preserve:System.Text.Json.Serialization.ReferenceHandler()
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Text.Json/System.Text.Json.Serialization.ReferenceHandler`1::.ctor:System.Void()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean&)
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String)
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
                case 1: Subject_1(); break;
                case 2: Subject_2(); break;
                case 3: Subject_3(); break;
                case 4: Subject_4(); break;
                case 5: Subject_5(); break;
                case 6: Subject_6(); break;
                case 7: Subject_7(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}