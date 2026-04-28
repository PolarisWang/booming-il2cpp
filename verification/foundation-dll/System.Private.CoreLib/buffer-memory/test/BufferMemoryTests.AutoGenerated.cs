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
using Xunit;

// Auto-generated skeleton for Buffer.Memory Operations (family/System.Private.CoreLib/buffer/memory).
public partial class BufferMemoryTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — BlockCopy with 5 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Buffer_BlockCopy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_In()
    {
    // TODO: needs-manual — BlockCopy with 5 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Buffer::BulkMoveWithWriteBarrier:System.Void(System.Byte&,System.Byte&,System.UIntPtr)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Buffer_BulkMoveWithWriteBarrier_System_Void_System_Byte_System_Byte_System_UIntPtr()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [MethodSubjectId("System.Private.CoreLib/System.Buffer::ByteLength:System.Int32(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Buffer_ByteLength_System_Int32_System_Array()
    {
    var result = Buffer.ByteLength(Array.Empty<byte>());
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Buffer::GetByte:System.Byte(System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — GetByte with 2 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Buffer_GetByte_System_Byte_System_Array_System_Int32()
    {
    // TODO: needs-manual — GetByte with 2 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Byte*,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_System_Byte_System_Int32()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [MethodSubjectId("System.Private.CoreLib/System.Buffer::Memcpy:System.Void(System.Byte*,System.Int32,System.Byte[],System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Buffer_Memcpy_System_Void_System_Byte_System_Int32_System_Byte_System_Int32_System_Int32()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [MethodSubjectId("System.Private.CoreLib/System.Buffer::Memmove:System.Void(System.Byte&,System.Byte&,System.UIntPtr)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Buffer_Memmove_System_Void_System_Byte_System_Byte_System_UIntPtr()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [MethodSubjectId("System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.Int64,System.Int64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_System_Void_System_Int64_System_Int64()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [MethodSubjectId("System.Private.CoreLib/System.Buffer::MemoryCopy:System.Void(System.Void*,System.Void*,System.UInt64,System.UInt64)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Buffer_MemoryCopy_System_Void_System_Void_System_Void_System_UInt64_System_UInt64()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [MethodSubjectId("System.Private.CoreLib/System.Buffer::SetByte:System.Void(System.Array,System.Int32,System.Byte)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — SetByte with 3 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Buffer_SetByte_System_Void_System_Array_System_Int32_System_Byte()
    {
    // TODO: needs-manual — SetByte with 3 params requires manual implementation
    }}
