using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Auto-generated skeleton for Convert.Char Conversions (family/System.Private.CoreLib/convert/char).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class ConvertChar
{
    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Boolean()
    {
    var result = Convert.ToChar(true);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Boolean() { _ = Convert.ToChar(true); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Boolean() { _ = Convert.ToChar(true); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Byte()
    {
    var result = Convert.ToChar((byte)42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Byte() { _ = Convert.ToChar((byte)42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Byte() { _ = Convert.ToChar((byte)42); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Char()
    {
    var result = Convert.ToChar('A');
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Char() { _ = Convert.ToChar('A'); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Char() { _ = Convert.ToChar('A'); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_DateTime()
    {
    var result = Convert.ToChar(DateTime.UtcNow);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_DateTime() { _ = Convert.ToChar(DateTime.UtcNow); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_DateTime() { _ = Convert.ToChar(DateTime.UtcNow); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Decimal()
    {
    var result = Convert.ToChar(42m);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Decimal() { _ = Convert.ToChar(42m); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Decimal() { _ = Convert.ToChar(42m); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Double()
    {
    var result = Convert.ToChar(42.0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Double() { _ = Convert.ToChar(42.0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Double() { _ = Convert.ToChar(42.0); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int16()
    {
    var result = Convert.ToChar((short)42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int16() { _ = Convert.ToChar((short)42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int16() { _ = Convert.ToChar((short)42); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int32()
    {
    var result = Convert.ToChar(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int32() { _ = Convert.ToChar(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int32() { _ = Convert.ToChar(42); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int64()
    {
    var result = Convert.ToChar(42L);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int64() { _ = Convert.ToChar(42L); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Int64() { _ = Convert.ToChar(42L); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object()
    {
    var result = Convert.ToChar(42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object() { _ = Convert.ToChar(42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object() { _ = Convert.ToChar(42); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object_System_IFormatProvider()
    {
    // TODO: needs-manual — ToChar with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object_System_IFormatProvider() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Object_System_IFormatProvider() { }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_SByte()
    {
    var result = Convert.ToChar((sbyte)42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_SByte() { _ = Convert.ToChar((sbyte)42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_SByte() { _ = Convert.ToChar((sbyte)42); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Single()
    {
    var result = Convert.ToChar(42.0f);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Single() { _ = Convert.ToChar(42.0f); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_Single() { _ = Convert.ToChar(42.0f); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String()
    {
    var result = Convert.ToChar("hello");
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String() { _ = Convert.ToChar("hello"); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String() { _ = Convert.ToChar("hello"); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String_System_IFormatProvider()
    {
    // TODO: needs-manual — ToChar with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String_System_IFormatProvider() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_String_System_IFormatProvider() { }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt16()
    {
    var result = Convert.ToChar((ushort)42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt16() { _ = Convert.ToChar((ushort)42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt16() { _ = Convert.ToChar((ushort)42); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt32()
    {
    var result = Convert.ToChar(42u);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt32() { _ = Convert.ToChar(42u); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt32() { _ = Convert.ToChar(42u); }

    // Convert.ToChar
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt64()
    {
    var result = Convert.ToChar(42uL);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt64() { _ = Convert.ToChar(42uL); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Convert_ToChar_System_Char_System_UInt64() { _ = Convert.ToChar(42uL); }
}
