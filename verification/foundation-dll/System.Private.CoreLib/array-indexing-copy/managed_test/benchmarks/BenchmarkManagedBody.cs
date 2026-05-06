// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/array-indexing-copy
using System;

namespace Chaos.Benchmark.array_indexing_copy
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)
        public static void System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Array_System_Int32()
        {
            BenchmarkChecksum += (int)(Array.Copy(new byte[4], new byte[4], 4));
        }

        // System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32()
        {
            BenchmarkChecksum += (int)(Array.Copy(new byte[4], 0, new byte[4], 0, 4));
        }

        // System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_Array_Clear_System_Void_System_Array_System_Int32_System_Int32()
        {
            BenchmarkChecksum += (int)(Array.Clear(new byte[4], 0, 4));
        }

        // System.Private.CoreLib/System.Array::Resize:System.Void(T[]&,System.Int32)
        public static void System_Private_CoreLib_System_Array_Resize_System_Void_T_Ref_System_Int32()
        {
            BenchmarkChecksum += (int)(Array.Resize(Array.Empty<T>(), 42));
        }

        // System.Private.CoreLib/System.Array::Sort:System.Void(System.Array)
        public static void System_Private_CoreLib_System_Array_Sort_System_Void_System_Array()
        {
            BenchmarkChecksum += (int)(Array.Sort(Array.Empty<byte>()));
        }

        // System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Collections.IComparer)
        public static void System_Private_CoreLib_System_Array_Sort_System_Void_System_Array_System_Collections_IComparer()
        {
            BenchmarkChecksum += (int)(Array.Sort<byte>(new byte[1], (IComparer<byte>)null!));
        }

        // System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)
        public static void System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Object()
        {
            BenchmarkChecksum += Array.BinarySearch(new byte[4], (byte)42);
        }

        // System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Int32,System.Int32,System.Object)
        public static void System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Int32_System_Int32_System_Object()
        {
            BenchmarkChecksum += Array.BinarySearch(new byte[4], 0, 4, (byte)42);
        }

        // System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)
        public static void System_Private_CoreLib_System_Array_IndexOf_System_Int32_System_Array_System_Object()
        {
            BenchmarkChecksum += Array.IndexOf(new byte[4], (byte)42);
        }

        // System.Private.CoreLib/System.Array::LastIndexOf:System.Int32(System.Array,System.Object)
        public static void System_Private_CoreLib_System_Array_LastIndexOf_System_Int32_System_Array_System_Object()
        {
            BenchmarkChecksum += Array.LastIndexOf(new byte[4], (byte)42);
        }

        // System.Private.CoreLib/System.Array::Reverse:System.Void(System.Array)
        public static void System_Private_CoreLib_System_Array_Reverse_System_Void_System_Array()
        {
            BenchmarkChecksum += (int)(Array.Reverse(Array.Empty<byte>()));
        }

        // System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)
        public static void System_Private_CoreLib_System_Array_GetLength_System_Int32_System_Int32()
        {
            BenchmarkChecksum += new int[4, 4].GetLength(0);
        }

        // System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)
        public static void System_Private_CoreLib_System_Array_GetValue_System_Object_System_Int32()
        {
            BenchmarkChecksum += ((new int[1].GetValue(0)).GetHashCode());
        }

    }
}
