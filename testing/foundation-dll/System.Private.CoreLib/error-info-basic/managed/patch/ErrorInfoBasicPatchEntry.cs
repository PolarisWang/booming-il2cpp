// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/error/info/basic
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ErrorInfoBasicPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::Run:System.Int32(System.Int32)
    public static void CustomEntryMethod0()
    {
    }

    // [1] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetExceptionForHR:System.Int32()
    public static void CustomEntryMethod1()
    {
    }

    // [2] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetHRForException:System.Int32()
    public static void CustomEntryMethod2()
    {
    }

    // [3] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestThrowExceptionForHR_S_OK:System.Int32()
    public static void CustomEntryMethod3()
    {
    }

    // [4] ErrorInfoBasicNativeEntry/ErrorInfoBasicNativeEntry::TestGetExceptionForHR_Zero:System.Int32()
    public static void CustomEntryMethod4()
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}