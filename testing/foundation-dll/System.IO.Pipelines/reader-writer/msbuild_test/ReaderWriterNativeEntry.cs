// Auto-generated native-AOT entry point
// Family: family/System.IO.Pipelines/reader-writer
// Assembly: System.IO.Pipelines
// Variant: benchmark

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipelines;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class ReaderWriterNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition)
    public static void Method0()
    {
        try { PipeReader.AdvanceTo(null!); }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition,System.SequencePosition)
    public static void Method1()
    {
        try { PipeReader.AdvanceTo(null!, null!); }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AsStream:System.IO.Stream(System.Boolean)
    public static void Method2()
    {
        try { if (((PipeReader.AsStream(true)).GetHashCode()) != ((PipeReader.AsStream(true)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CancelPendingRead:System.Void()
    public static void Method3()
    {
        try { PipeReader.CancelPendingRead(); }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Complete:System.Void(System.Exception)
    public static void Method4()
    {
        try { PipeReader.Complete(null!); }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception)
    public static void Method5()
    {
        try { if ((int)(PipeReader.CompleteAsync(null!)) != (int)(PipeReader.CompleteAsync(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CopyToAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Threading.CancellationToken)
    public static void Method6()
    {
        try { if (((PipeReader.CopyToAsync(null!, null!)).GetHashCode()) != ((PipeReader.CopyToAsync(null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Threading.CancellationToken)
    public static void Method7()
    {
        try { if (((PipeReader.CopyToAsync(null!, null!)).GetHashCode()) != ((PipeReader.CopyToAsync(null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Create:System.IO.Pipelines.PipeReader(System.IO.Stream,System.IO.Pipelines.StreamPipeReaderOptions)
    public static void Method8()
    {
        try { if ((int)(PipeReader.Create(null!, null!)) != (int)(PipeReader.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Create:System.IO.Pipelines.PipeReader(System.Buffers.ReadOnlySequence{System.Byte})
    public static void Method9()
    {
        try { if ((int)(PipeReader.Create(null!)) != (int)(PipeReader.Create(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.IO.Pipelines/System.IO.Pipelines.PipeReader::OnWriterCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object)

    // [11] System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Threading.CancellationToken)
    public static void Method11()
    {
        try { if ((int)(PipeReader.ReadAsync(null!)) != (int)(PipeReader.ReadAsync(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAtLeastAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Int32,System.Threading.CancellationToken)
    public static void Method12()
    {
        try { if ((int)(PipeReader.ReadAtLeastAsync(42, null!)) != (int)(PipeReader.ReadAtLeastAsync(42, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.IO.Pipelines/System.IO.Pipelines.PipeReader::TryRead:System.Boolean(System.IO.Pipelines.ReadResult&)
    public static void Method13()
    {
    var refLocal_0 = null!;
        try { if (((PipeReader.TryRead(ref refLocal_0)) ? 1 : 0) != ((PipeReader.TryRead(ref refLocal_0)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Advance:System.Void(System.Int32)
    public static void Method14()
    {
        try { PipeWriter.Advance(42); }
        catch { _exitCode = 1; }
    }

    // [15] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::AsStream:System.IO.Stream(System.Boolean)
    public static void Method15()
    {
        try { if (((PipeWriter.AsStream(true)).GetHashCode()) != ((PipeWriter.AsStream(true)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CancelPendingFlush:System.Void()
    public static void Method16()
    {
        try { PipeWriter.CancelPendingFlush(); }
        catch { _exitCode = 1; }
    }

    // [17] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Complete:System.Void(System.Exception)
    public static void Method17()
    {
        try { PipeWriter.Complete(null!); }
        catch { _exitCode = 1; }
    }

    // [18] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception)
    public static void Method18()
    {
        try { if ((int)(PipeWriter.CompleteAsync(null!)) != (int)(PipeWriter.CompleteAsync(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Create:System.IO.Pipelines.PipeWriter(System.IO.Stream,System.IO.Pipelines.StreamPipeWriterOptions)
    public static void Method19()
    {
        try { if ((int)(PipeWriter.Create(null!, null!)) != (int)(PipeWriter.Create(null!, null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::FlushAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.Threading.CancellationToken)
    public static void Method20()
    {
        try { if ((int)(PipeWriter.FlushAsync(null!)) != (int)(PipeWriter.FlushAsync(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_CanGetUnflushedBytes:System.Boolean()
    public static void Method21()
    {
        try { if (((PipeWriter.CanGetUnflushedBytes) ? 1 : 0) != ((PipeWriter.CanGetUnflushedBytes) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_UnflushedBytes:System.Int64()
    public static void Method22()
    {
        try { if ((int)(PipeWriter.UnflushedBytes) != (int)(PipeWriter.UnflushedBytes)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetMemory:System.Memory{System.Byte}(System.Int32)
    public static void Method23()
    {
        try { if ((int)(PipeWriter.GetMemory(42)) != (int)(PipeWriter.GetMemory(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetSpan:System.Span{System.Byte}(System.Int32)
    public static void Method24()
    {
        try { if ((int)(PipeWriter.GetSpan(42)) != (int)(PipeWriter.GetSpan(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::OnReaderCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object)

    // [26] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::WriteAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.ReadOnlyMemory{System.Byte},System.Threading.CancellationToken)
    public static void Method26()
    {
        try { if ((int)(PipeWriter.WriteAsync(null!, null!)) != (int)(PipeWriter.WriteAsync(null!, null!))) _exitCode = 1; }
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
                case 10: CustomEntryMethod10(); break;
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
                case 25: CustomEntryMethod25(); break;
                case 26: Method26(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}