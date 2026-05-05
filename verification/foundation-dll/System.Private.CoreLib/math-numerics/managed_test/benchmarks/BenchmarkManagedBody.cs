// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/math/numerics
using System;

namespace Chaos.Benchmark.math_numerics
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)
        public static void System_Private_CoreLib_System_Math_Abs_System_Int32_System_Int32()
        {
            BenchmarkChecksum += Math.Abs(42);
        }

        // System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)
        public static void System_Private_CoreLib_System_Math_Abs_System_Double_System_Double()
        {
            BenchmarkChecksum += (int)(Math.Abs(42.0));
        }

        // System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)
        public static void System_Private_CoreLib_System_Math_Ceiling_System_Double_System_Double()
        {
            BenchmarkChecksum += (int)(Math.Ceiling(42.0));
        }

        // System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)
        public static void System_Private_CoreLib_System_Math_Floor_System_Double_System_Double()
        {
            BenchmarkChecksum += (int)(Math.Floor(42.0));
        }

        // System.Private.CoreLib/System.Math::Round:System.Double(System.Double)
        public static void System_Private_CoreLib_System_Math_Round_System_Double_System_Double()
        {
            BenchmarkChecksum += (int)(Math.Round(42.0));
        }

        // System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)
        public static void System_Private_CoreLib_System_Math_Round_System_Double_System_Double_System_Int32()
        {
            BenchmarkChecksum += (int)(Math.Round(42.0));
        }

        // System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_Math_Max_System_Int32_System_Int32_System_Int32()
        {
            BenchmarkChecksum += Math.Max(42, 42);
        }

        // System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)
        public static void System_Private_CoreLib_System_Math_Max_System_Double_System_Double_System_Double()
        {
            BenchmarkChecksum += (int)(Math.Max(42.0, 42.0));
        }

        // System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_Math_Min_System_Int32_System_Int32_System_Int32()
        {
            BenchmarkChecksum += Math.Min(42, 42);
        }

        // System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)
        public static void System_Private_CoreLib_System_Math_Sqrt_System_Double_System_Double()
        {
            BenchmarkChecksum += (int)(Math.Sqrt(42.0));
        }

        // System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)
        public static void System_Private_CoreLib_System_Math_Pow_System_Double_System_Double_System_Double()
        {
            BenchmarkChecksum += (int)(Math.Pow(42.0, 42.0));
        }

        // System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)
        public static void System_Private_CoreLib_System_Math_Sin_System_Double_System_Double()
        {
            BenchmarkChecksum += (int)(Math.Sin(42.0));
        }

        // System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)
        public static void System_Private_CoreLib_System_Math_Cos_System_Double_System_Double()
        {
            BenchmarkChecksum += (int)(Math.Cos(42.0));
        }

        // System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_Math_BigMul_System_Int64_System_Int32_System_Int32()
        {
            BenchmarkChecksum += (int)(Math.BigMul(42, 42));
        }

        // System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)
        public static void System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Int32()
        {
            BenchmarkChecksum += ((BitConverter.GetBytes(42)).Length);
        }

        // System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)
        public static void System_Private_CoreLib_System_BitConverter_GetBytes_System_Byte_System_Double()
        {
            BenchmarkChecksum += ((BitConverter.GetBytes(42.0)).Length);
        }

        // System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)
        public static void System_Private_CoreLib_System_BitConverter_ToInt32_System_Int32_System_Byte_System_Int32()
        {
            BenchmarkChecksum += BitConverter.ToInt32(new byte[] { 1, 2, 3 }, 42);
        }

        // System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)
        public static void System_Private_CoreLib_System_BitConverter_ToDouble_System_Double_System_Byte_System_Int32()
        {
            BenchmarkChecksum += (int)(BitConverter.ToDouble(new byte[] { 1, 2, 3 }, 42));
        }

    }
}
