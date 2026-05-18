using System.Runtime.InteropServices;

namespace Chaos.IL2CPP.HotUpdate;

public enum BridgeDispatchStyle
{
    ManagedArgs = 0,
    ByRefInt32 = 1,
    PointerInt32 = 2,
    ReadOnlySpanInt32 = 3,
}

public enum BridgeCarrierKind
{
    Void = 0,
    Int32 = 1,
    Int64 = 7,
    Float = 8,
    Double = 9,
    String = 2,
    Object = 3,
    ByRefInt32 = 4,
    PointerInt32 = 5,
    ReadOnlySpanInt32 = 6,
}

public sealed record BridgeCarrierSchema
{
    public required BridgeCarrierKind ReturnKind { get; init; }

    public IReadOnlyList<BridgeCarrierKind> ParameterKinds { get; init; } = [];

    public required string WriteBackRule { get; init; }

    public required string PinningRule { get; init; }

    public required string LifetimeRule { get; init; }
}

public sealed class HotUpdateByRefInt32Carrier
{
    public HotUpdateByRefInt32Carrier(int value)
    {
        Value = value;
    }

    public int Value { get; set; }
}

public sealed class HotUpdatePinnedInt32Carrier : IDisposable
{
    private readonly GCHandle _handle;
    private bool _disposed;

    private HotUpdatePinnedInt32Carrier(int[] buffer)
    {
        Buffer = buffer;
        _handle = GCHandle.Alloc(Buffer, GCHandleType.Pinned);
    }

    public int[] Buffer { get; }

    public IntPtr Pointer
    {
        get
        {
            ObjectDisposedException.ThrowIf(_disposed, this);
            return _handle.AddrOfPinnedObject();
        }
    }

    public static HotUpdatePinnedInt32Carrier Create(int value)
    {
        return new HotUpdatePinnedInt32Carrier([value]);
    }

    public void Dispose()
    {
        if (_disposed)
        {
            return;
        }

        if (_handle.IsAllocated)
        {
            _handle.Free();
        }

        _disposed = true;
    }
}

public sealed class HotUpdateReadOnlySpanInt32Carrier
{
    public HotUpdateReadOnlySpanInt32Carrier(int[] buffer, int offset, int length)
    {
        ArgumentNullException.ThrowIfNull(buffer);

        if ((uint)offset > (uint)buffer.Length)
        {
            throw new ArgumentOutOfRangeException(nameof(offset));
        }

        if ((uint)length > (uint)(buffer.Length - offset))
        {
            throw new ArgumentOutOfRangeException(nameof(length));
        }

        Buffer = buffer;
        Offset = offset;
        Length = length;
    }

    public int[] Buffer { get; }

    public int Offset { get; }

    public int Length { get; }

    public ReadOnlySpan<int> AsSpan()
    {
        return new ReadOnlySpan<int>(Buffer, Offset, Length);
    }
}

public delegate void ByRefInt32BridgeTarget(ref int value);

public delegate int PointerInt32BridgeTarget(IntPtr address);

public delegate int ReadOnlySpanInt32BridgeTarget(ReadOnlySpan<int> values);
