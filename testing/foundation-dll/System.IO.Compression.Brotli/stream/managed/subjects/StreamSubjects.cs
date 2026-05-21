// Auto-generated native-AOT entry point
// Family: family/System.IO.Compression.Brotli/stream
// Assembly: System.IO.Compression.Brotli
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class StreamSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::BeginRead:System.IAsyncResult(System.Byte{},System.Int32,System.Int32,System.AsyncCallback,System.Object)
    public static void Subject_0()
    {
        try { default(BrotliStream)!.BeginRead(default, 42, 42, null!, 42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::BeginWrite:System.IAsyncResult(System.Byte{},System.Int32,System.Int32,System.AsyncCallback,System.Object)
    public static void Subject_1()
    {
        try { default(BrotliStream)!.BeginWrite(default, 42, 42, null!, 42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::DisposeAsync:System.Threading.Tasks.ValueTask()
    public static void Subject_2()
    {
        try { default(BrotliStream)!.DisposeAsync(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::EndRead:System.Int32(System.IAsyncResult)
    public static void Subject_3()
    {
        try { default(BrotliStream)!.EndRead(null!); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::EndWrite:System.Void(System.IAsyncResult)
    public static void Subject_4()
    {
        try { default(BrotliStream)!.EndWrite(null!); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Flush:System.Void()
    public static void Subject_5()
    {
        try { default(BrotliStream)!.Flush(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::FlushAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)
    public static void Subject_6()
    {
        try { default(BrotliStream)!.FlushAsync(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [7] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_BaseStream:System.IO.Stream()
    public static void Subject_7()
    {
        try { _ = default(BrotliStream)!.BaseStream; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [8] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanRead:System.Boolean()
    public static void Subject_8()
    {
        try { _ = default(BrotliStream)!.CanRead; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [9] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanSeek:System.Boolean()
    public static void Subject_9()
    {
        try { _ = default(BrotliStream)!.CanSeek; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [10] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanWrite:System.Boolean()
    public static void Subject_10()
    {
        try { _ = default(BrotliStream)!.CanWrite; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [11] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Length:System.Int64()
    public static void Subject_11()
    {
        try { _ = default(BrotliStream)!.Length; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [12] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Position:System.Int64()
    public static void Subject_12()
    {
        try { _ = default(BrotliStream)!.Position; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [13] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Read:System.Int32(System.Byte{},System.Int32,System.Int32)
    public static void Subject_13()
    {
        try { default(BrotliStream)!.Read(default, 42, 42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [14] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Read:System.Int32(System.Span{System.Byte})
    public static void Subject_14()
    {
        try { default(BrotliStream)!.Read(default(Span<byte>)); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [15] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadAsync:System.Threading.Tasks.Task{System.Int32}(System.Byte{},System.Int32,System.Int32,System.Threading.CancellationToken)
    public static void Subject_15()
    {
        try { default(BrotliStream)!.ReadAsync(default, 42, 42, default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [16] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadAsync:System.Threading.Tasks.ValueTask{System.Int32}(System.Memory{System.Byte},System.Threading.CancellationToken)
    public static void Subject_16()
    {
        try { default(BrotliStream)!.ReadAsync(default, default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [17] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadByte:System.Int32()
    public static void Subject_17()
    {
        try { default(BrotliStream)!.ReadByte(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [18] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)
    public static void Subject_18()
    {
        try { default(BrotliStream)!.Seek(42L, System.IO.SeekOrigin.Begin); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [19] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_Position:System.Void(System.Int64)
    public static void Subject_19()
    {
        try { _ = default(BrotliStream)!.Position = 42L; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [20] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::SetLength:System.Void(System.Int64)
    public static void Subject_20()
    {
        try { default(BrotliStream)!.SetLength(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [21] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Write:System.Void(System.Byte{},System.Int32,System.Int32)
    public static void Subject_21()
    {
        try { default(BrotliStream)!.Write(default, 42, 42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [22] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Write:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Subject_22()
    {
        try { default(BrotliStream)!.Write(default(ReadOnlySpan<byte>)); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [23] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteAsync:System.Threading.Tasks.Task(System.Byte{},System.Int32,System.Int32,System.Threading.CancellationToken)
    public static void Subject_23()
    {
        try { default(BrotliStream)!.WriteAsync(default, 42, 42, default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [24] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory{System.Byte},System.Threading.CancellationToken)
    public static void Subject_24()
    {
        try { default(BrotliStream)!.WriteAsync(default(ReadOnlyMemory<byte>), default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [25] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteByte:System.Void(System.Byte)
    public static void Subject_25()
    {
        try { default(BrotliStream)!.WriteByte((byte)42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [26] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionLevel)
    public static void Subject_26()
    {
        new BrotliStream(new MemoryStream(), CompressionLevel.Optimal);
    }

    // [27] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean)
    public static void Subject_27()
    {
        new BrotliStream(new MemoryStream(), CompressionLevel.Optimal, true);
    }

    // [28] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.BrotliCompressionOptions,System.Boolean)
    public static void Subject_28()
    {
        new BrotliStream(new MemoryStream(), CompressionLevel.Optimal, true);
    }

    // [29] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionMode)
    public static void Subject_29()
    {
        new BrotliStream(new MemoryStream(), CompressionLevel.Optimal);
    }

    // [30] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionMode,System.Boolean)
    public static void Subject_30()
    {
        new BrotliStream(new MemoryStream(), CompressionLevel.Optimal, true);
    }

}