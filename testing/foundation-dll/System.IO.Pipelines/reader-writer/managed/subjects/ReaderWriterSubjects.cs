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
        try { default(PipeReader)!.AdvanceTo(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition,System.SequencePosition)
    public static void Subject_1()
    {
        try { default(PipeReader)!.AdvanceTo(default, default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AsStream:System.IO.Stream(System.Boolean)
    public static void Subject_2()
    {
        try { default(PipeReader)!.AsStream(true); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CancelPendingRead:System.Void()
    public static void Subject_3()
    {
        try { default(PipeReader)!.CancelPendingRead(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Complete:System.Void(System.Exception)
    public static void Subject_4()
    {
        try { default(PipeReader)!.Complete(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception)
    public static void Subject_5()
    {
        try { default(PipeReader)!.CompleteAsync(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CopyToAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Threading.CancellationToken)
    public static void Subject_6()
    {
        try { default(PipeReader)!.CopyToAsync(default(PipeWriter)!, default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [7] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Threading.CancellationToken)
    public static void Subject_7()
    {
        try { default(PipeReader)!.CopyToAsync(default(PipeWriter)!, default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [8] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Create:System.IO.Pipelines.PipeReader(System.IO.Stream,System.IO.Pipelines.StreamPipeReaderOptions)
    public static void Subject_8()
    {
        if (((PipeReader.Create(new MemoryStream(), default(StreamPipeReaderOptions)!)).GetHashCode()) != 7880838) _exitCode = 1;
    }

    // [9] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Create:System.IO.Pipelines.PipeReader(System.Buffers.ReadOnlySequence{System.Byte})
    public static void Subject_9()
    {
        if (((PipeReader.Create(default(ReadOnlySequence<byte>))).GetHashCode()) != 56793269) _exitCode = 1;
    }

    // [10] System.IO.Pipelines/System.IO.Pipelines.PipeReader::OnWriterCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object)
    public static void CustomEntrySubject_10()
    {
    }

    // [11] System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Threading.CancellationToken)
    public static void Subject_11()
    {
        try { default(PipeReader)!.ReadAsync(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [12] System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAtLeastAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Int32,System.Threading.CancellationToken)
    public static void Subject_12()
    {
        try { default(PipeReader)!.ReadAtLeastAsync(42, default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [13] System.IO.Pipelines/System.IO.Pipelines.PipeReader::TryRead:System.Boolean(System.IO.Pipelines.ReadResult&)
    public static void Subject_13()
    {
        try { default(PipeReader)!.TryRead(out _); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [14] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Advance:System.Void(System.Int32)
    public static void Subject_14()
    {
        try { default(PipeWriter)!.Advance(42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [15] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::AsStream:System.IO.Stream(System.Boolean)
    public static void Subject_15()
    {
        try { default(PipeWriter)!.AsStream(true); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [16] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CancelPendingFlush:System.Void()
    public static void Subject_16()
    {
        try { default(PipeWriter)!.CancelPendingFlush(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [17] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Complete:System.Void(System.Exception)
    public static void Subject_17()
    {
        try { default(PipeWriter)!.Complete(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [18] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception)
    public static void Subject_18()
    {
        try { default(PipeWriter)!.CompleteAsync(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [19] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Create:System.IO.Pipelines.PipeWriter(System.IO.Stream,System.IO.Pipelines.StreamPipeWriterOptions)
    public static void Subject_19()
    {
        if (((PipeWriter.Create(new MemoryStream(), default(StreamPipeWriterOptions)!)).GetHashCode()) != 115000) _exitCode = 1;
    }

    // [20] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::FlushAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.Threading.CancellationToken)
    public static void Subject_20()
    {
        try { default(PipeWriter)!.FlushAsync(default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [21] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_CanGetUnflushedBytes:System.Boolean()
    public static void Subject_21()
    {
        try { _ = default(PipeWriter)!.CanGetUnflushedBytes; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [22] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_UnflushedBytes:System.Int64()
    public static void Subject_22()
    {
        try { _ = default(PipeWriter)!.UnflushedBytes; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [23] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetMemory:System.Memory{System.Byte}(System.Int32)
    public static void Subject_23()
    {
        try { default(PipeWriter)!.GetMemory(42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [24] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetSpan:System.Span{System.Byte}(System.Int32)
    public static void Subject_24()
    {
        try { default(PipeWriter)!.GetSpan(42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [25] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::OnReaderCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object)
    public static void CustomEntrySubject_25()
    {
    }

    // [26] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::WriteAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.ReadOnlyMemory{System.Byte},System.Threading.CancellationToken)
    public static void Subject_26()
    {
        try { default(PipeWriter)!.WriteAsync(default(ReadOnlyMemory<byte>), default); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

}