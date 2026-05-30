// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class ReaderWriterSubjects
{
    // [0] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition)
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { default(PipeReader)!.AdvanceTo(default); } catch { }    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AdvanceTo:System.Void(System.SequencePosition,System.SequencePosition)
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { default(PipeReader)!.AdvanceTo(default, default); } catch { }    }

    // [2] System.IO.Pipelines/System.IO.Pipelines.PipeReader::AsStream:System.IO.Stream(System.Boolean)
    [Fact]
    public static void CustomEntrySubject_2()
    {        try { _ = default(PipeReader)!.AsStream(true); } catch { }    }

    // [3] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CancelPendingRead:System.Void()
    [Fact]
    public static void CustomEntrySubject_3()
    {        try { default(PipeReader)!.CancelPendingRead(); } catch { }    }

    // [4] System.IO.Pipelines/System.IO.Pipelines.PipeReader::Complete:System.Void(System.Exception)
    [Fact]
    public static void CustomEntrySubject_4()
    {        try { default(PipeReader)!.Complete(default); } catch { }    }

    // [5] System.IO.Pipelines/System.IO.Pipelines.PipeReader::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception)
    [Fact]
    public static void CustomEntrySubject_5()
    {        try { _ = default(PipeReader)!.CompleteAsync(default); } catch { }    }

    // [10] System.IO.Pipelines/System.IO.Pipelines.PipeReader::OnWriterCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object)
    [Fact]
    public static void CustomEntrySubject_10()
    {        try { default(PipeReader)!.OnWriterCompleted(default, default, null!); } catch { }    }

    // [11] System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Threading.CancellationToken)
    [Fact]
    public static void CustomEntrySubject_11()
    {        try { _ = default(PipeReader)!.ReadAsync(default); } catch { }    }

    // [12] System.IO.Pipelines/System.IO.Pipelines.PipeReader::ReadAtLeastAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.ReadResult}(System.Int32,System.Threading.CancellationToken)
    [Fact]
    public static void CustomEntrySubject_12()
    {        try { _ = default(PipeReader)!.ReadAtLeastAsync(42, default); } catch { }    }

    // [13] System.IO.Pipelines/System.IO.Pipelines.PipeReader::TryRead:System.Boolean(System.IO.Pipelines.ReadResult&)
    [Fact]
    public static void CustomEntrySubject_13()
    {        try { _ = default(PipeReader)!.TryRead(out default); } catch { }    }

    // [14] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Advance:System.Void(System.Int32)
    [Fact]
    public static void CustomEntrySubject_14()
    {        try { default(PipeWriter)!.Advance(42); } catch { }    }

    // [15] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::AsStream:System.IO.Stream(System.Boolean)
    [Fact]
    public static void CustomEntrySubject_15()
    {        try { _ = default(PipeWriter)!.AsStream(true); } catch { }    }

    // [16] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CancelPendingFlush:System.Void()
    [Fact]
    public static void CustomEntrySubject_16()
    {        try { default(PipeWriter)!.CancelPendingFlush(); } catch { }    }

    // [17] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::Complete:System.Void(System.Exception)
    [Fact]
    public static void CustomEntrySubject_17()
    {        try { default(PipeWriter)!.Complete(default); } catch { }    }

    // [18] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::CompleteAsync:System.Threading.Tasks.ValueTask(System.Exception)
    [Fact]
    public static void CustomEntrySubject_18()
    {        try { _ = default(PipeWriter)!.CompleteAsync(default); } catch { }    }

    // [20] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::FlushAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.Threading.CancellationToken)
    [Fact]
    public static void CustomEntrySubject_20()
    {        try { _ = default(PipeWriter)!.FlushAsync(default); } catch { }    }

    // [21] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_CanGetUnflushedBytes:System.Boolean()
    [Fact]
    public static void CustomEntrySubject_21()
    {        try { _ = default(PipeWriter)!.CanGetUnflushedBytes; } catch { }    }

    // [22] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::get_UnflushedBytes:System.Int64()
    [Fact]
    public static void CustomEntrySubject_22()
    {        try { _ = default(PipeWriter)!.UnflushedBytes; } catch { }    }

    // [23] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetMemory:System.Memory{System.Byte}(System.Int32)
    [Fact]
    public static void CustomEntrySubject_23()
    {        try { _ = default(PipeWriter)!.GetMemory(42); } catch { }    }

    // [24] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::GetSpan:System.Span{System.Byte}(System.Int32)
    [Fact]
    public static void CustomEntrySubject_24()
    {        try { _ = default(PipeWriter)!.GetSpan(42); } catch { }    }

    // [25] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::OnReaderCompleted:System.Void(System.Action{System.Exception,System.Object},System.Object)
    [Fact]
    public static void CustomEntrySubject_25()
    {        try { default(PipeWriter)!.OnReaderCompleted(default, default, null!); } catch { }    }

    // [26] System.IO.Pipelines/System.IO.Pipelines.PipeWriter::WriteAsync:System.Threading.Tasks.ValueTask{System.IO.Pipelines.FlushResult}(System.ReadOnlyMemory{System.Byte},System.Threading.CancellationToken)
    [Fact]
    public static void CustomEntrySubject_26()
    {        try { _ = default(PipeWriter)!.WriteAsync(default(ReadOnlyMemory<byte>), default); } catch { }    }

}
