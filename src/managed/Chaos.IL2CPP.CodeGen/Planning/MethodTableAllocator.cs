namespace Chaos.IL2CPP.CodeGen.Planning;

/// <summary>
/// Allocates sequential ranges in the global method table (65536 entries)
/// for cross-assembly method dispatch. Each assembly in a multi-assembly
/// pipeline gets a contiguous block of indices.
///
/// Usage:
///  var allocator = new MethodTableAllocator();
///  var range = allocator.AllocateRange(100); // (Start: 0, Count: 100)
///  var range2 = allocator.AllocateRange(50); // (Start: 100, Count: 50)
/// </summary>
internal sealed class MethodTableAllocator
{
    private const uint MaxMethodTableSize = 524288;
    private uint _nextIndex = 0;

    /// <summary>
    /// Allocate a range of method table indices for an assembly.
    /// </summary>
    public (uint StartIndex, uint Count) AllocateRange(uint methodCount)
    {
        var start = _nextIndex;
        _nextIndex += methodCount;

        if (_nextIndex > MaxMethodTableSize)
        {
            throw new InvalidOperationException(
                $"Method table exceeds maximum size of {MaxMethodTableSize}. " +
                $"Allocated {start + methodCount} entries. " +
                "Consider increasing the method table size or reducing the number of cross-assembly calls.");
        }

        return (start, methodCount);
    }

    /// <summary>
    /// Get the total number of allocated entries.
    /// </summary>
    public uint TotalAllocated => _nextIndex;

    /// <summary>
    /// Reset the allocator (for testing or re-initialization).
    /// </summary>
    public void Reset()
    {
        _nextIndex = 0;
    }
}
