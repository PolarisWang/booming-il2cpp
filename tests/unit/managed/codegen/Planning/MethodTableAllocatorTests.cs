using Chaos.IL2CPP.Generator.Planning;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Planning;

public sealed class MethodTableAllocatorTests
{
    [Fact]
    public void AllocateRange_FirstAllocation_StartsAtZero()
    {
        var allocator = new MethodTableAllocator();
        var range = allocator.AllocateRange(100);

        Assert.Equal(0u, range.StartIndex);
        Assert.Equal(100u, range.Count);
    }

    [Fact]
    public void AllocateRange_SequentialAllocations_Contiguous()
    {
        var allocator = new MethodTableAllocator();

        var first = allocator.AllocateRange(50);
        var second = allocator.AllocateRange(30);

        Assert.Equal(0u, first.StartIndex);
        Assert.Equal(50u, first.Count);
        Assert.Equal(50u, second.StartIndex);
        Assert.Equal(30u, second.Count);
    }

    [Fact]
    public void AllocateRange_ZeroCount_AdvancesNextIndex()
    {
        var allocator = new MethodTableAllocator();

        var first = allocator.AllocateRange(0);
        var second = allocator.AllocateRange(10);

        Assert.Equal(0u, first.StartIndex);
        Assert.Equal(0u, first.Count);
        Assert.Equal(0u, second.StartIndex);
        Assert.Equal(10u, second.Count);
    }

    [Fact]
    public void AllocateRange_MultipleAllocations_Accumulates()
    {
        var allocator = new MethodTableAllocator();

        allocator.AllocateRange(100);
        allocator.AllocateRange(200);
        allocator.AllocateRange(50);
        var last = allocator.AllocateRange(1);

        Assert.Equal(350u, last.StartIndex);
        Assert.Equal(1u, last.Count);
    }

    [Fact]
    public void AllocateRange_ExceedsMaxSize_Throws()
    {
        var allocator = new MethodTableAllocator();

        // Max is 524288, allocate 524288 first, then 1 more
        allocator.AllocateRange(524288);

        var ex = Assert.Throws<InvalidOperationException>(() => allocator.AllocateRange(1));
        Assert.Contains("Method table exceeds maximum size", ex.Message);
    }

    [Fact]
    public void AllocateRange_AllocateUpToMax_DoesNotThrow()
    {
        var allocator = new MethodTableAllocator();
        var range = allocator.AllocateRange(524288);

        Assert.Equal(0u, range.StartIndex);
        Assert.Equal(524288u, range.Count);
    }
}
