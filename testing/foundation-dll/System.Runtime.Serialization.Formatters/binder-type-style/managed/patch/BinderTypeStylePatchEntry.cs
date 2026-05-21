// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/binder/type/style
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class BinderTypeStylePatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToName:System.Void(System.Type,System.String&,System.String&)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToType:System.Type(System.String,System.String)
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