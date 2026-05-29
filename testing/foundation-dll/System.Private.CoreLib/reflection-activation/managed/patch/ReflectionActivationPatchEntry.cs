// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/activation
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ReflectionActivationPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type)
    public static void CustomEntryMethod0()
    {
    }

    // [1] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Object[])
    public static void CustomEntryMethod1()
    {
    }

    // [2] System.Private.CoreLib/System.Activator::CreateInstance:T()
    public static void CustomEntryMethod2()
    {
    }

    // [3] System.Private.CoreLib/System.Activator::CreateInstance:System.Object(System.Type,System.Reflection.BindingFlags,System.Reflection.Binder,System.Object[],System.Globalization.CultureInfo)
    public static void CustomEntryMethod3()
    {
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32[])
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Private.CoreLib/System.Reflection.ConstructorInfo::Invoke:System.Object(System.Object[])
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
                case 0: CustomEntryMethod0(); break;
                case 1: CustomEntryMethod1(); break;
                case 2: CustomEntryMethod2(); break;
                case 3: CustomEntryMethod3(); break;
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