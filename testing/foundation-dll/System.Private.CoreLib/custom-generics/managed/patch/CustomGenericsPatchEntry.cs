// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/custom/generics
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class CustomGenericsPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/CustomGenerics.Box`1::Get:T()
    public static void CustomEntryMethod0()
    {
    }

    // [1] System.Private.CoreLib/CustomGenerics.Box`1::Set:(T)
    public static void CustomEntryMethod1()
    {
    }

    // [2] System.Private.CoreLib/CustomGenerics.Pair`2::Deconstruct:(T1&,T2&)
    public static void CustomEntryMethod2()
    {
    }

    // [3] System.Private.CoreLib/CustomGenerics.Util::Echo:T(T)
    public static void CustomEntryMethod3()
    {
    }

    // [4] System.Private.CoreLib/CustomGenerics.Util::Equal:bool(T,T)
    public static void CustomEntryMethod4()
    {
    }

    // [5] System.Private.CoreLib/CustomGenerics.Wrapper`1::GetRefType:System.Type()
    public static void CustomEntryMethod5()
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
                case 2: CustomEntryMethod2(); break;
                case 3: CustomEntryMethod3(); break;
                case 4: CustomEntryMethod4(); break;
                case 5: CustomEntryMethod5(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}