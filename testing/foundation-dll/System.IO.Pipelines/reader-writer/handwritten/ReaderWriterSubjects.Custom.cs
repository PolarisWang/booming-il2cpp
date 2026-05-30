// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class ReaderWriterSubjects
{
    // [0] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition)
    public static void CustomEntrySubject_0()
    {        try { default(PipeReader)!.AdvanceTo(default); } catch { _exitCode = 1; }    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition,System.SequencePosition)
    public static void CustomEntrySubject_1()
    {        try { default(PipeReader)!.AdvanceTo(default, default); } catch { _exitCode = 1; }    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AsStream:System.IO.Stream(System.Boolean)
    public static void CustomEntrySubject_2()
    {        try { _ = default(PipeReader)!.AsStream(true); } catch { _exitCode = 1; }    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CancelPendingRead:System.Void()
    public static void CustomEntrySubject_3()
    {        try { default(PipeReader)!.CancelPendingRead(); } catch { _exitCode = 1; }    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Complete:System.Void(System.Exception)
    public static void CustomEntrySubject_4()
    {        try { default(PipeReader)!.Complete(default); } catch { _exitCode = 1; }    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception)
    public static void CustomEntrySubject_5()
    {        try { _ = default(PipeReader)!.CompleteAsync(default); } catch { _exitCode = 1; }    }

    // [10] System.IO.Pipelines/System.IO.Pipelines.PipeReader::OnWriterCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object)
    public static void CustomEntrySubject_10()
    {        try { default(PipeReader)!.OnWriterCompleted(default, default, null!); } catch { _exitCode = 1; }    }

    // [11] System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Threading.CancellationToken)
    public static void CustomEntrySubject_11()
    {        try { _ = default(PipeReader)!.ReadAsync(default); } catch { _exitCode = 1; }    }

    // [12] System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAtLeastAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Int32,System.Threading.CancellationToken)
    public static void CustomEntrySubject_12()
    {        try { _ = default(PipeReader)!.ReadAtLeastAsync(42, default); } catch { _exitCode = 1; }    }

    // [13] System.IO.Pipelines/System.IO.Pipelines.PipeReader::TryRead:System.Boolean(System.IO.Pipelines.ReadResult&)
    public static void CustomEntrySubject_13()
    {        try { _ = default(PipeReader)!.TryRead(out default); } catch { _exitCode = 1; }    }

    // [14] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Advance:System.Void(System.Int32)
    public static void CustomEntrySubject_14()
    {        try { default(PipeWriter)!.Advance(42); } catch { _exitCode = 1; }    }

    // [15] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::AsStream:System.IO.Stream(System.Boolean)
    public static void CustomEntrySubject_15()
    {        try { _ = default(PipeWriter)!.AsStream(true); } catch { _exitCode = 1; }    }

    // [16] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CancelPendingFlush:System.Void()
    public static void CustomEntrySubject_16()
    {        try { default(PipeWriter)!.CancelPendingFlush(); } catch { _exitCode = 1; }    }

    // [17] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Complete:System.Void(System.Exception)
    public static void CustomEntrySubject_17()
    {        try { default(PipeWriter)!.Complete(default); } catch { _exitCode = 1; }    }

    // [18] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception)
    public static void CustomEntrySubject_18()
    {        try { _ = default(PipeWriter)!.CompleteAsync(default); } catch { _exitCode = 1; }    }

    // [20] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::FlushAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.Threading.CancellationToken)
    public static void CustomEntrySubject_20()
    {        try { _ = default(PipeWriter)!.FlushAsync(default); } catch { _exitCode = 1; }    }

    // [21] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_CanGetUnflushedBytes:System.Boolean()
    public static void CustomEntrySubject_21()
    {        try { _ = default(PipeWriter)!.CanGetUnflushedBytes; } catch { _exitCode = 1; }    }

    // [22] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_UnflushedBytes:System.Int64()
    public static void CustomEntrySubject_22()
    {        try { _ = default(PipeWriter)!.UnflushedBytes; } catch { _exitCode = 1; }    }

    // [23] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetMemory:System.Memory{System.Byte}(System.Int32)
    public static void CustomEntrySubject_23()
    {        try { _ = default(PipeWriter)!.GetMemory(42); } catch { _exitCode = 1; }    }

    // [24] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetSpan:System.Span{System.Byte}(System.Int32)
    public static void CustomEntrySubject_24()
    {        try { _ = default(PipeWriter)!.GetSpan(42); } catch { _exitCode = 1; }    }

    // [25] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::OnReaderCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object)
    public static void CustomEntrySubject_25()
    {        try { default(PipeWriter)!.OnReaderCompleted(default, default, null!); } catch { _exitCode = 1; }    }

    // [26] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::WriteAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.ReadOnlyMemory{System.Byte},System.Threading.CancellationToken)
    public static void CustomEntrySubject_26()
    {        try { _ = default(PipeWriter)!.WriteAsync(default(ReadOnlyMemory<byte>), default); } catch { _exitCode = 1; }    }

}
