// Auto-generated benchmark managed bodies
// Family: family/System.Private.CoreLib/guid/random/hashcode
using System;

namespace Chaos.Benchmark.guid_random_hashcode
{
    public static class BenchmarkManagedBody
    {
        internal static long BenchmarkChecksum;

        // System.Private.CoreLib/System.Guid::.ctor:System.Void(System.String)
        public static void System_Private_CoreLib_System_Guid_ctor_System_Void_System_String()
        {
            BenchmarkChecksum += (int)(new Guid("hello"));
        }

        // System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[])
        public static void System_Private_CoreLib_System_Guid_ctor_System_Void_System_Byte()
        {
            BenchmarkChecksum += (int)(new Guid(new byte[] { 1, 2, 3 }));
        }

        // System.Private.CoreLib/System.Guid::NewGuid:System.Guid()
        public static void System_Private_CoreLib_System_Guid_NewGuid_System_Guid()
        {
            BenchmarkChecksum += ((Guid.NewGuid()).GetHashCode());
        }

        // System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)
        public static void System_Private_CoreLib_System_Guid_Parse_System_Guid_System_String()
        {
            BenchmarkChecksum += ((Guid.Parse("hello")).GetHashCode());
        }

        // System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)
        public static void System_Private_CoreLib_System_Guid_TryParse_System_Boolean_System_String_System_Guid_Ref()
        {
            BenchmarkChecksum += ((Guid.TryParse("hello", Guid.Empty)) ? 1 : 0);
        }

        // System.Private.CoreLib/System.Guid::ToString:System.String()
        public static void System_Private_CoreLib_System_Guid_ToString_System_String()
        {
            BenchmarkChecksum += ((Guid.NewGuid().ToString()).Length);
        }

        // System.Private.CoreLib/System.Random::.ctor:System.Void()
        public static void System_Private_CoreLib_System_Random_ctor_System_Void()
        {
            BenchmarkChecksum += (int)(new Random());
        }

        // System.Private.CoreLib/System.Random::Next:System.Int32()
        public static void System_Private_CoreLib_System_Random_Next_System_Int32()
        {
            BenchmarkChecksum += new Random().Next();
        }

        // System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)
        public static void System_Private_CoreLib_System_Random_Next_System_Int32_System_Int32()
        {
            BenchmarkChecksum += new Random().Next(42);
        }

        // System.Private.CoreLib/System.Random::NextDouble:System.Double()
        public static void System_Private_CoreLib_System_Random_NextDouble_System_Double()
        {
            BenchmarkChecksum += (int)(new Random().NextDouble());
        }

        // System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])
        public static void System_Private_CoreLib_System_Random_NextBytes_System_Void_System_Byte()
        {
            BenchmarkChecksum += (int)(new Random().NextBytes(new byte[] { 1, 2, 3 }));
        }

        // System.Private.CoreLib/System.HashCode::Add:System.Void(System.Object)
        public static void System_Private_CoreLib_System_HashCode_Add_System_Void_System_Object()
        {
            BenchmarkChecksum += (int)(default(HashCode).Add(42));
        }

        // System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()
        public static void System_Private_CoreLib_System_HashCode_ToHashCode_System_Int32()
        {
            BenchmarkChecksum += default(HashCode).ToHashCode();
        }

        // System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)
        public static void System_Private_CoreLib_System_HashCode_Combine_System_Int32_System_Int32_System_Int32()
        {
            BenchmarkChecksum += HashCode.Combine(42, 42);
        }

    }
}
