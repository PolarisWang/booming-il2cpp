// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/collections/generic/core
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class CollectionsGenericCorePatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)
    // [1] System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)
    // [3] System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)
    // [4] System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)
    // [5] System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)
    // [9] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)
    // [11] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)
    // [12] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)
    // [13] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)
    // [14] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)
    // [15] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)
    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: CustomEntryMethod0(); break;
                case 1: Subject_1(); break;
                case 2: CustomEntryMethod2(); break;
                case 3: CustomEntryMethod3(); break;
                case 4: CustomEntryMethod4(); break;
                case 5: Subject_5(); break;
                case 6: Subject_6(); break;
                case 7: Subject_7(); break;
                case 8: CustomEntryMethod8(); break;
                case 9: Subject_9(); break;
                case 10: CustomEntryMethod10(); break;
                case 11: CustomEntryMethod11(); break;
                case 12: CustomEntryMethod12(); break;
                case 13: CustomEntryMethod13(); break;
                case 14: CustomEntryMethod14(); break;
                case 15: CustomEntryMethod15(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}