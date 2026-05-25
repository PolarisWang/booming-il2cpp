// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/generics/stress
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class GenericsStressPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Int32,System.Int32,System.Collections.Generic.IComparer<System.Int32>)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}