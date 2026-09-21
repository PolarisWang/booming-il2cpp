using Chaos.IL2CPP.Generator;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Tests for the hotpatch method-index chunk plan.
///
/// <para>
/// The hotpatch block is a single coherent unit whose three index arrays are
/// coupled: <c>s_hotpatch_types[i].first_method_index</c> indexes into
/// <c>s_hotpatch_methods</c> globally. Chunking therefore has to preserve the
/// global index space exactly — a chunk that re-indexes from zero would make
/// name lookup silently resolve to the wrong method.
/// </para>
/// </summary>
public sealed class HotpatchChunkPlanTests
{
    private const int Budget = 350_000;

    [Fact]
    public void SmallBlock_IsNotChunked()
    {
        // 100 methods x ~100 chars = 10K chars, far under budget.
        var plan = NativeAotLoweringPlanner.PlanHotpatchMethodChunks(
            totalMethodCount: 100, estimatedCharsPerMethod: 100, budgetChars: Budget);

        Assert.Null(plan);
    }

    [Fact]
    public void LargeBlock_IsChunked()
    {
        // 9000 x 256 = 2.3M chars, the measured shape that motivated this.
        var plan = NativeAotLoweringPlanner.PlanHotpatchMethodChunks(
            totalMethodCount: 9_000, estimatedCharsPerMethod: 256, budgetChars: Budget);

        Assert.NotNull(plan);
        Assert.True(plan!.Value.ChunkNames.Count > 1,
            "a block far over budget must be split into multiple chunks");
    }

    /// <summary>
    /// The invariant that makes chunking safe: every method index 0..N-1 is
    /// covered exactly once, and the chunks tile the global index space
    /// contiguously starting at 0.
    /// </summary>
    [Fact]
    public void Chunks_TileTheGlobalIndexSpaceExactly()
    {
        const int total = 9_000;
        var plan = NativeAotLoweringPlanner.PlanHotpatchMethodChunks(
            totalMethodCount: total, estimatedCharsPerMethod: 256, budgetChars: Budget);
        Assert.NotNull(plan);

        var (_, offsets, counts) = plan!.Value;
        Assert.Equal(offsets.Count, counts.Count);

        // Contiguous from 0, no gaps, no overlaps, ends exactly at total.
        int cursor = 0;
        for (int i = 0; i < offsets.Count; i++)
        {
            Assert.Equal(cursor, offsets[i]);
            Assert.True(counts[i] > 0, $"chunk {i} is empty");
            cursor += counts[i];
        }
        Assert.Equal(total, cursor);
    }

    /// <summary>
    /// Each chunk must individually fit the budget, or chunking has not solved
    /// the problem it exists for.
    /// </summary>
    [Fact]
    public void EachChunk_FitsBudget()
    {
        var plan = NativeAotLoweringPlanner.PlanHotpatchMethodChunks(
            totalMethodCount: 9_000, estimatedCharsPerMethod: 256, budgetChars: Budget);
        Assert.NotNull(plan);

        var (_, _, counts) = plan!.Value;
        foreach (int count in counts)
        {
            Assert.True((long)count * 256 <= Budget,
                $"a chunk of {count} methods (~{count * 256:N0} chars) exceeds the {Budget:N0} budget");
        }
    }

    /// <summary>
    /// Chunk names must be distinct, or the emitted arrays collide
    /// (duplicate definition).
    /// </summary>
    [Fact]
    public void ChunkNames_AreDistinct()
    {
        var plan = NativeAotLoweringPlanner.PlanHotpatchMethodChunks(
            totalMethodCount: 50_000, estimatedCharsPerMethod: 256, budgetChars: Budget);
        Assert.NotNull(plan);

        var names = plan!.Value.ChunkNames;
        Assert.Equal(names.Count, names.Distinct(StringComparer.Ordinal).Count());
    }

    [Theory]
    [InlineData(0, 256)]
    [InlineData(100, 0)]
    [InlineData(-1, 256)]
    public void DegenerateInputs_ReturnNull(int total, int perMethod)
    {
        Assert.Null(NativeAotLoweringPlanner.PlanHotpatchMethodChunks(
            total, perMethod, Budget));
    }
}
