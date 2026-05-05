// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/collections/generic-core
using System;

namespace Chaos.Benchmark.collections_generic_core
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)
        public static void System_Private_CoreLib_System_Collections_Generic_List_1_Add_System_Void_T()
        {
            BenchmarkChecksum += (int)(new List<int>().Add(null!));
        }

        // System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()
        public static void System_Private_CoreLib_System_Collections_Generic_List_1_Clear_System_Void()
        {
            BenchmarkChecksum += (int)(new List<int>().Clear());
        }

        // System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)
        public static void System_Private_CoreLib_System_Collections_Generic_List_1_Contains_System_Boolean_T()
        {
            BenchmarkChecksum += ((new List<int>().Contains(null!)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)
        public static void System_Private_CoreLib_System_Collections_Generic_List_1_IndexOf_System_Int32_T()
        {
            BenchmarkChecksum += new List<int>().IndexOf(null!);
        }

        // System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)
        public static void System_Private_CoreLib_System_Collections_Generic_List_1_Remove_System_Boolean_T()
        {
            BenchmarkChecksum += ((new List<int>().Remove(null!)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)
        public static void System_Private_CoreLib_System_Collections_Generic_List_1_RemoveAt_System_Void_System_Int32()
        {
            BenchmarkChecksum += (int)(new List<int>().RemoveAt(42));
        }

        // System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()
        public static void System_Private_CoreLib_System_Collections_Generic_List_1_Sort_System_Void()
        {
            BenchmarkChecksum += (int)(new List<int>().Sort());
        }

        // System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()
        public static void System_Private_CoreLib_System_Collections_Generic_List_1_ToArray_T()
        {
            BenchmarkChecksum += ((new List<int>().ToArray()).Length);
        }

        // System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)
        public static void System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Add_System_Void_TKey_TValue()
        {
            BenchmarkChecksum += (int)(new Dictionary<string, int>().Add(null!, null!));
        }

        // System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()
        public static void System_Private_CoreLib_System_Collections_Generic_Dictionary_2_get_Count_System_Int32()
        {
            BenchmarkChecksum += new Dictionary<string, int>().Count;
        }

        // System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)
        public static void System_Private_CoreLib_System_Collections_Generic_Dictionary_2_TryGetValue_System_Boolean_TKey_TValue_Ref()
        {
            BenchmarkChecksum += ((new Dictionary<string, int>().TryGetValue(null!, null!)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)
        public static void System_Private_CoreLib_System_Collections_Generic_Dictionary_2_ContainsKey_System_Boolean_TKey()
        {
            BenchmarkChecksum += ((new Dictionary<string, int>().ContainsKey(null!)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)
        public static void System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Remove_System_Boolean_TKey()
        {
            BenchmarkChecksum += ((new Dictionary<string, int>().Remove(null!)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)
        public static void System_Private_CoreLib_System_Collections_Generic_HashSet_1_Add_System_Boolean_T()
        {
            BenchmarkChecksum += ((HashSet.Add(null!)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)
        public static void System_Private_CoreLib_System_Collections_Generic_HashSet_1_Contains_System_Boolean_T()
        {
            BenchmarkChecksum += ((HashSet.Contains(null!)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)
        public static void System_Private_CoreLib_System_Collections_Generic_HashSet_1_Remove_System_Boolean_T()
        {
            BenchmarkChecksum += ((HashSet.Remove(null!)) ? 1 : 0);
        }

    }
}
