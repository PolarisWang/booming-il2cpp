// Auto-generated native-AOT entry point
// Family: family/System.IO.Compression.Brotli/stream
// Assembly: System.IO.Compression.Brotli
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class StreamNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::BeginRead:System.IAsyncResult(System.Byte{},System.Int32,System.Int32,System.AsyncCallback,System.Object)
    public static void Method0()
    {
        try { if ((int)(BrotliStream.BeginRead(null!, 42, 42, null!, 42)) != (int)(BrotliStream.BeginRead(null!, 42, 42, null!, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::BeginWrite:System.IAsyncResult(System.Byte{},System.Int32,System.Int32,System.AsyncCallback,System.Object)
    public static void Method1()
    {
        try { if ((int)(BrotliStream.BeginWrite(null!, 42, 42, null!, 42)) != (int)(BrotliStream.BeginWrite(null!, 42, 42, null!, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::DisposeAsync:System.Threading.Tasks.ValueTask()
    public static void Method2()
    {
        try { if ((int)(BrotliStream.DisposeAsync()) != (int)(BrotliStream.DisposeAsync())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::EndRead:System.Int32(System.IAsyncResult)
    public static void Method3()
    {
        try { if (BrotliStream.EndRead(null!) != BrotliStream.EndRead(null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::EndWrite:System.Void(System.IAsyncResult)
    public static void Method4()
    {
        try { BrotliStream.EndWrite(null!); }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Flush:System.Void()
    public static void Method5()
    {
        try { BrotliStream.Flush(); }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::FlushAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)
    public static void Method6()
    {
        try { if (((BrotliStream.FlushAsync(null!)).GetHashCode()) != ((BrotliStream.FlushAsync(null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_BaseStream:System.IO.Stream()
    public static void Method7()
    {
        try { if (((BrotliStream.BaseStream).GetHashCode()) != ((BrotliStream.BaseStream).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanRead:System.Boolean()
    public static void Method8()
    {
        try { if (((BrotliStream.CanRead) ? 1 : 0) != ((BrotliStream.CanRead) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanSeek:System.Boolean()
    public static void Method9()
    {
        try { if (((BrotliStream.CanSeek) ? 1 : 0) != ((BrotliStream.CanSeek) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_CanWrite:System.Boolean()
    public static void Method10()
    {
        try { if (((BrotliStream.CanWrite) ? 1 : 0) != ((BrotliStream.CanWrite) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Length:System.Int64()
    public static void Method11()
    {
        try { if ((int)(BrotliStream.Length) != (int)(BrotliStream.Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::get_Position:System.Int64()
    public static void Method12()
    {
        try { if ((int)(BrotliStream.Position) != (int)(BrotliStream.Position)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Read:System.Int32(System.Byte{},System.Int32,System.Int32)
    public static void Method13()
    {
        try { if (BrotliStream.Read(null!, 42, 42) != BrotliStream.Read(null!, 42, 42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Read:System.Int32(System.Span{System.Byte})
    public static void Method14()
    {
        try { if (BrotliStream.Read(null!) != BrotliStream.Read(null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadAsync:System.Threading.Tasks.Task{System.Int32}(System.Byte{},System.Int32,System.Int32,System.Threading.CancellationToken)
    public static void Method15()
    {
        try { if (((BrotliStream.ReadAsync(null!, 42, 42, null!)).GetHashCode()) != ((BrotliStream.ReadAsync(null!, 42, 42, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadAsync:System.Threading.Tasks.ValueTask{System.Int32}(System.Memory{System.Byte},System.Threading.CancellationToken)
    public static void Method16()
    {
        try { if ((int)(BrotliStream.ReadAsync(null!, null!)) != (int)(BrotliStream.ReadAsync(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::ReadByte:System.Int32()
    public static void Method17()
    {
        try { if (BrotliStream.ReadByte() != BrotliStream.ReadByte()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)
    public static void Method18()
    {
        try { if ((int)(BrotliStream.Seek(42L, System.IO.SeekOrigin.Begin)) != (int)(BrotliStream.Seek(42L, System.IO.SeekOrigin.Begin))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::set_Position:System.Void(System.Int64)
    public static void Method19()
    {
        try { BrotliStream.Position = 42L; }
        catch { _exitCode = 1; }
    }

    // [20] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::SetLength:System.Void(System.Int64)
    public static void Method20()
    {
        try { BrotliStream.SetLength(42L); }
        catch { _exitCode = 1; }
    }

    // [21] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Write:System.Void(System.Byte{},System.Int32,System.Int32)
    public static void Method21()
    {
        try { BrotliStream.Write(null!, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [22] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::Write:System.Void(System.ReadOnlySpan{System.Byte})
    public static void Method22()
    {
        try { BrotliStream.Write(null!); }
        catch { _exitCode = 1; }
    }

    // [23] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteAsync:System.Threading.Tasks.Task(System.Byte{},System.Int32,System.Int32,System.Threading.CancellationToken)
    public static void Method23()
    {
        try { if (((BrotliStream.WriteAsync(null!, 42, 42, null!)).GetHashCode()) != ((BrotliStream.WriteAsync(null!, 42, 42, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory{System.Byte},System.Threading.CancellationToken)
    public static void Method24()
    {
        try { if ((int)(BrotliStream.WriteAsync(null!, null!)) != (int)(BrotliStream.WriteAsync(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::WriteByte:System.Void(System.Byte)
    public static void Method25()
    {
        try { BrotliStream.WriteByte((byte)42); }
        catch { _exitCode = 1; }
    }

    // [26] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionLevel)
    public static void Method26()
    {
        try { new BrotliStream(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [27] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionLevel,System.Boolean)
    public static void Method27()
    {
        try { new BrotliStream(null!, null!, true); }
        catch { _exitCode = 1; }
    }

    // [28] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.BrotliCompressionOptions,System.Boolean)
    public static void Method28()
    {
        try { new BrotliStream(null!, null!, true); }
        catch { _exitCode = 1; }
    }

    // [29] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionMode)
    public static void Method29()
    {
        try { new BrotliStream(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [30] System.IO.Compression.Brotli/System.IO.Compression.BrotliStream::.ctor:System.Void(System.IO.Stream,System.IO.Compression.CompressionMode,System.Boolean)
    public static void Method30()
    {
        try { new BrotliStream(null!, null!, true); }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
                case 8: Method8(); break;
                case 9: Method9(); break;
                case 10: Method10(); break;
                case 11: Method11(); break;
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
                case 16: Method16(); break;
                case 17: Method17(); break;
                case 18: Method18(); break;
                case 19: Method19(); break;
                case 20: Method20(); break;
                case 21: Method21(); break;
                case 22: Method22(); break;
                case 23: Method23(); break;
                case 24: Method24(); break;
                case 25: Method25(); break;
                case 26: Method26(); break;
                case 27: Method27(); break;
                case 28: Method28(); break;
                case 29: Method29(); break;
                case 30: Method30(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}