// Auto-generated native-AOT entry point
// Family: family/System.IO.Pipelines/reader/writer
// Assembly: System.IO.Pipelines
// Variant: subjects

using System;
using System.Buffers;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipelines;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class ReaderWriterSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition)
    public static void Subject_0()
    {
        try { default(PipeReader)!.AdvanceTo(default); }
        catch { _exitCode = 1; }
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition,System.SequencePosition)
    public static void Subject_1()
    {
        try { default(PipeReader)!.AdvanceTo(default, default); }
        catch { _exitCode = 1; }
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AsStream:System.IO.Stream(System.Boolean)
    public static void Subject_2()
    {
        try { if (((default(PipeReader)!.AsStream(true)).GetHashCode()) != ((default(PipeReader)!.AsStream(true)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CancelPendingRead:System.Void()
    public static void Subject_3()
    {
        try { default(PipeReader)!.CancelPendingRead(); }
        catch { _exitCode = 1; }
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Complete:System.Void(System.Exception)
    public static void Subject_4()
    {
        try { default(PipeReader)!.Complete(default); }
        catch { _exitCode = 1; }
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception)
    public static void Subject_5()
    {
        try { if (((default(PipeReader)!.CompleteAsync(default)).GetHashCode()) != ((default(PipeReader)!.CompleteAsync(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CopyToAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Threading.CancellationToken)
    public static void Subject_6()
    {
        try { if (((default(PipeReader)!.CopyToAsync(default(PipeWriter)!, default)).GetHashCode()) != ((default(PipeReader)!.CopyToAsync(default(PipeWriter)!, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Threading.CancellationToken)
    public static void Subject_7()
    {
        try { if (((default(PipeReader)!.CopyToAsync(default(PipeWriter)!, default)).GetHashCode()) != ((default(PipeReader)!.CopyToAsync(default(PipeWriter)!, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Create:System.IO.Pipelines.PipeReader(System.IO.Stream,System.IO.Pipelines.StreamPipeReaderOptions)
    public static void Subject_8()
    {
        try { if (((PipeReader.Create(new MemoryStream(), default(StreamPipeReaderOptions)!)).GetHashCode()) != ((PipeReader.Create(new MemoryStream(), default(StreamPipeReaderOptions)!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Create:System.IO.Pipelines.PipeReader(System.Buffers.ReadOnlySequence{System.Byte})
    public static void Subject_9()
    {
        try { if (((PipeReader.Create(default(ReadOnlySequence<byte>))).GetHashCode()) != ((PipeReader.Create(default(ReadOnlySequence<byte>))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [10] System.IO.Pipelines/System.IO.Pipelines.PipeReader::OnWriterCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object)
    public static void CustomEntrySubject_10()
    {
    }

    // [11] System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Threading.CancellationToken)
    public static void Subject_11()
    {
        try { if (((default(PipeReader)!.ReadAsync(default)).GetHashCode()) != ((default(PipeReader)!.ReadAsync(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAtLeastAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Int32,System.Threading.CancellationToken)
    public static void Subject_12()
    {
        try { if (((default(PipeReader)!.ReadAtLeastAsync(42, default)).GetHashCode()) != ((default(PipeReader)!.ReadAtLeastAsync(42, default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.IO.Pipelines/System.IO.Pipelines.PipeReader::TryRead:System.Boolean(System.IO.Pipelines.ReadResult&)
    public static void Subject_13()
    {
        try { if (((default(PipeReader)!.TryRead(out _)) ? 1 : 0) != ((default(PipeReader)!.TryRead(out _)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Advance:System.Void(System.Int32)
    public static void Subject_14()
    {
        try { default(PipeWriter)!.Advance(42); }
        catch { _exitCode = 1; }
    }

    // [15] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::AsStream:System.IO.Stream(System.Boolean)
    public static void Subject_15()
    {
        try { if (((default(PipeWriter)!.AsStream(true)).GetHashCode()) != ((default(PipeWriter)!.AsStream(true)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CancelPendingFlush:System.Void()
    public static void Subject_16()
    {
        try { default(PipeWriter)!.CancelPendingFlush(); }
        catch { _exitCode = 1; }
    }

    // [17] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Complete:System.Void(System.Exception)
    public static void Subject_17()
    {
        try { default(PipeWriter)!.Complete(default); }
        catch { _exitCode = 1; }
    }

    // [18] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception)
    public static void Subject_18()
    {
        try { if (((default(PipeWriter)!.CompleteAsync(default)).GetHashCode()) != ((default(PipeWriter)!.CompleteAsync(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [19] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Create:System.IO.Pipelines.PipeWriter(System.IO.Stream,System.IO.Pipelines.StreamPipeWriterOptions)
    public static void Subject_19()
    {
        try { if (((PipeWriter.Create(new MemoryStream(), default(StreamPipeWriterOptions)!)).GetHashCode()) != ((PipeWriter.Create(new MemoryStream(), default(StreamPipeWriterOptions)!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::FlushAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.Threading.CancellationToken)
    public static void Subject_20()
    {
        try { if (((default(PipeWriter)!.FlushAsync(default)).GetHashCode()) != ((default(PipeWriter)!.FlushAsync(default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_CanGetUnflushedBytes:System.Boolean()
    public static void Subject_21()
    {
        try { if (((default(PipeWriter)!.CanGetUnflushedBytes) ? 1 : 0) != ((default(PipeWriter)!.CanGetUnflushedBytes) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_UnflushedBytes:System.Int64()
    public static void Subject_22()
    {
        try { if ((int)(default(PipeWriter)!.UnflushedBytes) != (int)(default(PipeWriter)!.UnflushedBytes)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetMemory:System.Memory{System.Byte}(System.Int32)
    public static void Subject_23()
    {
        try { if (((default(PipeWriter)!.GetMemory(42)).GetHashCode()) != ((default(PipeWriter)!.GetMemory(42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetSpan:System.Span{System.Byte}(System.Int32)
    public static void Subject_24()
    {
        try { if (((default(PipeWriter)!.GetSpan(42)).GetHashCode()) != ((default(PipeWriter)!.GetSpan(42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::OnReaderCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object)
    public static void CustomEntrySubject_25()
    {
    }

    // [26] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::WriteAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.ReadOnlyMemory{System.Byte},System.Threading.CancellationToken)
    public static void Subject_26()
    {
        try { if (((default(PipeWriter)!.WriteAsync(default(ReadOnlyMemory<byte>), default)).GetHashCode()) != ((default(PipeWriter)!.WriteAsync(default(ReadOnlyMemory<byte>), default)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}