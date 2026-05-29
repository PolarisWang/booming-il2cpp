// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/object/equality/identity
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ObjectEqualityIdentityPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)
    public static void CustomEntryMethod0()
    {
    }

    // [1] System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)
    public static void CustomEntryMethod1()
    {
    }

    // [2] System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Private.CoreLib/System.Object::GetHashCode:System.Int32()
    public static void CustomEntryMethod3()
    {
    }

    // [4] System.Private.CoreLib/System.Object::ToString:System.String()
    public static void CustomEntryMethod4()
    {
    }

    // [5] System.Private.CoreLib/System.Object::GetType:System.Type()
    public static void CustomEntryMethod5()
    {
    }

    // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    public static void CustomEntryMethod6()
    {
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: CustomEntryMethod0(); break;
                case 1: CustomEntryMethod1(); break;
                case 2: Subject_2(); break;
                case 3: CustomEntryMethod3(); break;
                case 4: CustomEntryMethod4(); break;
                case 5: CustomEntryMethod5(); break;
                case 6: CustomEntryMethod6(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}