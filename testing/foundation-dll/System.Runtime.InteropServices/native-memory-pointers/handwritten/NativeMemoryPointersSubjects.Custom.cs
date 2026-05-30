// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class NativeMemoryPointersSubjects
{
    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int32{},System.Int32,System.IntPtr,System.Int32)
    [Fact]
    public static void CustomEntrySubject_8()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { }    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Char{},System.Int32,System.IntPtr,System.Int32)
    [Fact]
    public static void CustomEntrySubject_9()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { }    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int16{},System.Int32,System.IntPtr,System.Int32)
    [Fact]
    public static void CustomEntrySubject_10()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { }    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int64{},System.Int32,System.IntPtr,System.Int32)
    [Fact]
    public static void CustomEntrySubject_11()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { }    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Single{},System.Int32,System.IntPtr,System.Int32)
    [Fact]
    public static void CustomEntrySubject_12()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { }    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Double{},System.Int32,System.IntPtr,System.Int32)
    [Fact]
    public static void CustomEntrySubject_13()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { }    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Byte{},System.Int32,System.IntPtr,System.Int32)
    [Fact]
    public static void CustomEntrySubject_14()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { }    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr{},System.Int32,System.IntPtr,System.Int32)
    [Fact]
    public static void CustomEntrySubject_15()
    {        try { Marshal.Copy(default, 42, IntPtr.Zero, 42); } catch { }    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int32{},System.Int32,System.Int32)
    [Fact]
    public static void CustomEntrySubject_16()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { }    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Char{},System.Int32,System.Int32)
    [Fact]
    public static void CustomEntrySubject_17()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { }    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int16{},System.Int32,System.Int32)
    [Fact]
    public static void CustomEntrySubject_18()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { }    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int64{},System.Int32,System.Int32)
    [Fact]
    public static void CustomEntrySubject_19()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { }    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Single{},System.Int32,System.Int32)
    [Fact]
    public static void CustomEntrySubject_20()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { }    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Double{},System.Int32,System.Int32)
    [Fact]
    public static void CustomEntrySubject_21()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { }    }

    // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Byte{},System.Int32,System.Int32)
    [Fact]
    public static void CustomEntrySubject_22()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { }    }

    // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.IntPtr{},System.Int32,System.Int32)
    [Fact]
    public static void CustomEntrySubject_23()
    {        try { Marshal.Copy(IntPtr.Zero, default, 42, 42); } catch { }    }

    // [92] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::QueryInterface:System.Int32(System.IntPtr,System.Guid&,System.IntPtr&)
    [Fact]
    public static void CustomEntrySubject_92()
    {        try { _ = Marshal.QueryInterface(IntPtr.Zero, out Guid.Empty, out IntPtr.Zero); } catch { }    }

    // [136] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.Guid&,System.IntPtr)
    [Fact]
    public static void CustomEntrySubject_136()
    {        try { Marshal.ThrowExceptionForHR(42, out Guid.Empty, IntPtr.Zero); } catch { }    }

    // [163] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Dispose:System.Void()
    [Fact]
    public static void CustomEntrySubject_163()
    {        try { default(UnmanagedMemoryAccessor)!.Dispose(); } catch { }    }

    // [164] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanRead:System.Boolean()
    [Fact]
    public static void CustomEntrySubject_164()
    {        try { _ = default(UnmanagedMemoryAccessor)!.CanRead; } catch { }    }

    // [165] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanWrite:System.Boolean()
    [Fact]
    public static void CustomEntrySubject_165()
    {        try { _ = default(UnmanagedMemoryAccessor)!.CanWrite; } catch { }    }

    // [166] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_Capacity:System.Int64()
    [Fact]
    public static void CustomEntrySubject_166()
    {        try { _ = default(UnmanagedMemoryAccessor)!.Capacity; } catch { }    }

    // [167] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Read{T}:System.Void(System.Int64,T&)
    [Fact]
    public static void CustomEntrySubject_167()
    {        try { default(UnmanagedMemoryAccessor)!.Read{T}(42L, out default); } catch { }    }

    // [168] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadArray{T}:System.Int32(System.Int64,T{},System.Int32,System.Int32)
    [Fact]
    public static void CustomEntrySubject_168()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadArray{T}(42L, default, 42, 42); } catch { }    }

    // [169] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadBoolean:System.Boolean(System.Int64)
    [Fact]
    public static void CustomEntrySubject_169()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadBoolean(42L); } catch { }    }

    // [170] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadByte:System.Byte(System.Int64)
    [Fact]
    public static void CustomEntrySubject_170()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadByte(42L); } catch { }    }

    // [171] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadChar:System.Char(System.Int64)
    [Fact]
    public static void CustomEntrySubject_171()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadChar(42L); } catch { }    }

    // [172] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDecimal:System.Decimal(System.Int64)
    [Fact]
    public static void CustomEntrySubject_172()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadDecimal(42L); } catch { }    }

    // [173] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDouble:System.Double(System.Int64)
    [Fact]
    public static void CustomEntrySubject_173()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadDouble(42L); } catch { }    }

    // [174] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt16:System.Int16(System.Int64)
    [Fact]
    public static void CustomEntrySubject_174()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadInt16(42L); } catch { }    }

    // [175] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt32:System.Int32(System.Int64)
    [Fact]
    public static void CustomEntrySubject_175()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadInt32(42L); } catch { }    }

    // [176] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt64:System.Int64(System.Int64)
    [Fact]
    public static void CustomEntrySubject_176()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadInt64(42L); } catch { }    }

    // [177] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSByte:System.SByte(System.Int64)
    [Fact]
    public static void CustomEntrySubject_177()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadSByte(42L); } catch { }    }

    // [178] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSingle:System.Single(System.Int64)
    [Fact]
    public static void CustomEntrySubject_178()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadSingle(42L); } catch { }    }

    // [179] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt16:System.UInt16(System.Int64)
    [Fact]
    public static void CustomEntrySubject_179()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadUInt16(42L); } catch { }    }

    // [180] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt32:System.UInt32(System.Int64)
    [Fact]
    public static void CustomEntrySubject_180()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadUInt32(42L); } catch { }    }

    // [181] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt64:System.UInt64(System.Int64)
    [Fact]
    public static void CustomEntrySubject_181()
    {        try { _ = default(UnmanagedMemoryAccessor)!.ReadUInt64(42L); } catch { }    }

    // [182] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Boolean)
    [Fact]
    public static void CustomEntrySubject_182()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, true); } catch { }    }

    // [183] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Byte)
    [Fact]
    public static void CustomEntrySubject_183()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, (byte)42); } catch { }    }

    // [184] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Char)
    [Fact]
    public static void CustomEntrySubject_184()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, 'A'); } catch { }    }

    // [185] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int16)
    [Fact]
    public static void CustomEntrySubject_185()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, (short)42); } catch { }    }

    // [186] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int32)
    [Fact]
    public static void CustomEntrySubject_186()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42); } catch { }    }

    // [187] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int64)
    [Fact]
    public static void CustomEntrySubject_187()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42L); } catch { }    }

    // [188] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Decimal)
    [Fact]
    public static void CustomEntrySubject_188()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42m); } catch { }    }

    // [189] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Single)
    [Fact]
    public static void CustomEntrySubject_189()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42.0f); } catch { }    }

    // [190] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Double)
    [Fact]
    public static void CustomEntrySubject_190()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42.0); } catch { }    }

    // [191] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.SByte)
    [Fact]
    public static void CustomEntrySubject_191()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, (sbyte)42); } catch { }    }

    // [192] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt16)
    [Fact]
    public static void CustomEntrySubject_192()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, (ushort)42); } catch { }    }

    // [193] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt32)
    [Fact]
    public static void CustomEntrySubject_193()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42u); } catch { }    }

    // [194] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt64)
    [Fact]
    public static void CustomEntrySubject_194()
    {        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42uL); } catch { }    }

    // [195] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write{T}:System.Void(System.Int64,T&)
    [Fact]
    public static void CustomEntrySubject_195()
    {        try { default(UnmanagedMemoryAccessor)!.Write{T}(42L, out default); } catch { }    }

    // [196] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::WriteArray{T}:System.Void(System.Int64,T{},System.Int32,System.Int32)
    [Fact]
    public static void CustomEntrySubject_196()
    {        try { default(UnmanagedMemoryAccessor)!.WriteArray{T}(42L, default, 42, 42); } catch { }    }

}
