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
        try { if (((default(BrotliStream)!.BeginRead(default, 42, 42, null!, 42)).GetHashCode()) != ((default(BrotliStream)!.BeginRead(default, 42, 42, null!, 42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::BeginWrite:System.IAsyncResult(System.Byte{},System.Int32,System.Int32,System.AsyncCallback,System.Object)
    public static void Subject_1()
    {
        try { if (((default(BrotliStream)!.BeginWrite(default, 42, 42, null!, 42)).GetHashCode()) != ((default(BrotliStream)!.BeginWrite(default, 42, 42, null!, 42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::DisposeAsync:System.Threading.Tasks.ValueTask()
    public static void Subject_2()
    {
        try { if (((default(BrotliStream)!.DisposeAsync()).GetHashCode()) != ((default(BrotliStream)!.DisposeAsync()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::EndRead:System.Int32(System.IAsyncResult)
    public static void Subject_3()
    {
        try { if (default(BrotliStream)!.EndRead(null!) != default(BrotliStream)!.EndRead(null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::EndWrite:System.Void(System.IAsyncResult)
    public static void Subject_4()
    {
        try { default(BrotliStream)!.EndWrite(null!); }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Flush:System.Void()
    public static void Subject_5()
    {
        try { default(BrotliStream)!.Flush(); }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::FlushAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)
    public static void Subject_6()
    {
        try { if (((default(BrotliStream)!.FlushAsync(default)).GetHashCode()) != ((default(BrotliStream)!.FlushAsync(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_BaseStream:System.IO.Stream()
    public static void Subject_7()
    {
        try { if (((default(BrotliStream)!.BaseStream).GetHashCode()) != ((default(BrotliStream)!.BaseStream).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanRead:System.Boolean()
    public static void Subject_8()
    {
        try { if (((default(BrotliStream)!.CanRead) ? 1 : 0) != ((default(BrotliStream)!.CanRead) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanSeek:System.Boolean()
    public static void Subject_9()
    {
        try { if (((default(BrotliStream)!.CanSeek) ? 1 : 0) != ((default(BrotliStream)!.CanSeek) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanWrite:System.Boolean()
    public static void Subject_10()
    {
        try { if (((default(BrotliStream)!.CanWrite) ? 1 : 0) != ((default(BrotliStream)!.CanWrite) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Length:System.Int64()
    public static void Subject_11()
    {
        try { if ((int)(default(BrotliStream)!.Length) != (int)(default(BrotliStream)!.Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Position:System.Int64()
    public static void Subject_12()
    {
        try { if ((int)(default(BrotliStream)!.Position) != (int)(default(BrotliStream)!.Position)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Read:System.Int32(System.Byte{},System.Int32,System.Int32)
    public static void Subject_13()
    {
        try { if (default(BrotliStream)!.Read(default, 42, 42) != default(BrotliStream)!.Read(default, 42, 42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Read:System.Int32(System.Span{System.Byte})
    public static void Subject_14()
    {
        try { if (default(BrotliStream)!.Read(default(Span<byte>)) != default(BrotliStream)!.Read(default(Span<byte>))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadAsync:System.Threading.Tasks.Task{System.Int32}(System.Byte{},System.Int32,System.Int32,System.Threading.CancellationToken)
    public static void Subject_15()
    {
        try { if (((default(BrotliStream)!.ReadAsync(default, 42, 42, default)).GetHashCode()) != ((default(BrotliStream)!.ReadAsync(default, 42, 42, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadAsync:System.Threading.Tasks.ValueTask{System.Int32}(System.Memory{System.Byte},System.Threading.CancellationToken)
    public static void Subject_16()
    {
        try { if (((default(BrotliStream)!.ReadAsync(default, default)).GetHashCode()) != ((default(BrotliStream)!.ReadAsync(default, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadByte:System.Int32()
    public static void Subject_17()
    {
        try { if (default(BrotliStream)!.ReadByte() != default(BrotliStream)!.ReadByte()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)
    public static void Subject_18()
    {
        try { if ((int)(default(BrotliStream)!.Seek(42L, System.IO.SeekOrigin.Begin)) != (int)(default(BrotliStream)!.Seek(42L, System.IO.SeekOrigin.Begin))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_Position:System.Void(System.Int64)
    public static void Subject_19()
    {
        try { default(BrotliStream)!.Position = 42L; }
        catch { _exitCode = 1; }
    }

    // [20] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::SetLength:System.Void(System.Int64)
    public static void Subject_20()
    {
        try { default(BrotliStream)!.SetLength(42L); }
        catch { _exitCode = 1; }
    }

    // [21] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Write:System.Void(System.Byte{},System.Int32,System.Int32)
    public static void Subject_21()
    {
        try { default(BrotliStream)!.Write(default, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [22] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Write:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Subject_22()
    {
        try { default(BrotliStream)!.Write(default(ReadOnlySpan<byte>)); }
        catch { _exitCode = 1; }
    }

    // [23] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteAsync:System.Threading.Tasks.Task(System.Byte{},System.Int32,System.Int32,System.Threading.CancellationToken)
    public static void Subject_23()
    {
        try { if (((default(BrotliStream)!.WriteAsync(default, 42, 42, default)).GetHashCode()) != ((default(BrotliStream)!.WriteAsync(default, 42, 42, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory{System.Byte},System.Threading.CancellationToken)
    public static void Subject_24()
    {
        try { if (((default(BrotliStream)!.WriteAsync(default(ReadOnlyMemory<byte>), default)).GetHashCode()) != ((default(BrotliStream)!.WriteAsync(default(ReadOnlyMemory<byte>), default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteByte:System.Void(System.Byte)
    public static void Subject_25()
    {
        try { default(BrotliStream)!.WriteByte((byte)42); }
        catch { _exitCode = 1; }
    }

    // [26] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionLevel)
    public static void Subject_26()
    {
        try { new BrotliStream(new MemoryStream(), CompressionLevel.Optimal); }
        catch { _exitCode = 1; }
    }

    // [27] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean)
    public static void Subject_27()
    {
        try { new BrotliStream(new MemoryStream(), CompressionLevel.Optimal, true); }
        catch { _exitCode = 1; }
    }

    // [28] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.BrotliCompressionOptions,System.Boolean)
    public static void Subject_28()
    {
        try { new BrotliStream(new MemoryStream(), CompressionLevel.Optimal, true); }
        catch { _exitCode = 1; }
    }

    // [29] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionMode)
    public static void Subject_29()
    {
        try { new BrotliStream(new MemoryStream(), CompressionLevel.Optimal); }
        catch { _exitCode = 1; }
    }

    // [30] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionMode,System.Boolean)
    public static void Subject_30()
    {
        try { new BrotliStream(new MemoryStream(), CompressionLevel.Optimal, true); }
        catch { _exitCode = 1; }
    }

}