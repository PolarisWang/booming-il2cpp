// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/extensions/marshal
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ExtensionsMarshalPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Byte>:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Byte)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>()
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Collections.Immutable/System.Collections.Immutable.ImmutableArray::Create<Int32>:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Int32)
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}