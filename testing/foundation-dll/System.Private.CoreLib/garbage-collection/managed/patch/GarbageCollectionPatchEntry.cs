// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/garbage/collection
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class GarbageCollectionPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.GC::Collect:System.Void()
    public static void CustomEntryMethod0()
    {
    }

    // [1] System.Private.CoreLib/System.GC::WaitForPendingFinalizers:System.Void()
    public static void CustomEntryMethod1()
    {
    }

    // [2] System.Private.CoreLib/System.GC::KeepAlive:System.Void(System.Object)
    public static void CustomEntryMethod2()
    {
    }

    // [3] System.Private.CoreLib/System.GC::GetGeneration:System.Int32(System.Object)
    public static void CustomEntryMethod3()
    {
    }

    // [4] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Private.CoreLib/System.GC::GetTotalMemory:System.Int64(System.Boolean)
    public static void CustomEntryMethod5()
    {
    }

    // [6] System.Private.CoreLib/System.GC::GetGCMemoryInfo: System.GCMemoryInfo()
    public static void CustomEntryMethod6()
    {
    }

    // [7] System.Private.CoreLib/System.GC::Collect(System.Int32,System.GCCollectionMode):System.Void()
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Private.CoreLib/System.GC::CollectionCount(System.Int32):System.Int64()
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Private.CoreLib/System.GC::GetTotalPauseDuration:System.TimeSpan()
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
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
                case 5: CustomEntryMethod5(); break;
                case 6: CustomEntryMethod6(); break;
                case 7: Subject_7(); break;
                case 8: Subject_8(); break;
                case 9: Subject_9(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}