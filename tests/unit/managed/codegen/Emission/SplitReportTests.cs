using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using Chaos.IL2CPP.Generator;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Step 1 of the unified-packer work: the split report must faithfully describe
/// the division it was given, and be reproducible.
///
/// <para>
/// These exercise <see cref="SplitReportBuilder"/> directly rather than driving
/// the emitter. The emitter path additionally requires
/// <c>templateModel.Methods.Count >= 500</c> to engage payload sectioning, which
/// a synthetic planner fixture does not produce — so a test that went through
/// the emitter would silently assert nothing. (That is exactly what the first
/// version of this file did: it passed the "sections exist" check and then
/// failed on "no report", because sectioning never engaged.)
/// </para>
/// </summary>
public sealed class SplitReportTests
{
    private static readonly SplitReportBuilder.TuRecord[] TwoTus =
    {
        new("payload.0", "native-aot.payload.page-0001.cpp", 300_000, 2, "observed-only",
            new[]
            {
                new SplitReportBuilder.UnitRecord("section", "modulereg", 180_000),
                new SplitReportBuilder.UnitRecord("section", "vtable0", 120_000),
            }),
        new("payload.1", "native-aot.payload.page-0002.cpp", 90_000, 1, "observed-only",
            new[]
            {
                new SplitReportBuilder.UnitRecord("section", "gcslotmap", 90_000),
            }),
    };

    private static readonly Dictionary<string, long> Sizes = new()
    {
        ["modulereg"] = 180_000,
        ["vtable0"] = 120_000,
        ["gcslotmap"] = 90_000,
    };

    [Fact]
    public void Report_ListsEveryTuWithItsComponentsAndSizes()
    {
        var (json, _) = SplitReportBuilder.Build(TwoTus, 350_000, Sizes);
        using var doc = JsonDocument.Parse(json);
        var root = doc.RootElement;

        Assert.Equal(2, root.GetProperty("tuCount").GetInt32());

        var tus = root.GetProperty("tuList").EnumerateArray().ToList();
        Assert.Equal(2, tus.Count);
        Assert.Equal("payload.0", tus[0].GetProperty("id").GetString());
        Assert.Equal(300_000, tus[0].GetProperty("accumulatedChars").GetInt64());
        // The TU records the file it corresponds to, which is what turns a
        // compiler message naming a file into the units that produced it.
        Assert.Equal("native-aot.payload.page-0001.cpp", tus[0].GetProperty("path").GetString());
    }

    [Fact]
    public void Report_IsDeterministic()
    {
        var (j1, l1) = SplitReportBuilder.Build(TwoTus, 350_000, Sizes);
        var (j2, l2) = SplitReportBuilder.Build(TwoTus, 350_000, Sizes);
        Assert.Equal(j1, j2);
        Assert.Equal(l1, l2);
    }

    /// <summary>
    /// A unit larger than the budget cannot be placed inside it — the pager
    /// never divides a section. Those are the only cases where the emitted TU
    /// legitimately exceeds the budget, and they must be called out rather than
    /// left for MSVC to discover.
    /// </summary>
    [Fact]
    public void Report_FlagsUnitsThatExceedTheBudget()
    {
        var sizes = new Dictionary<string, long>
        {
            ["big"] = 9_000_000,
            ["small"] = 1_000,
        };
        var (json, log) = SplitReportBuilder.Build(TwoTus, 350_000, sizes);

        using var doc = JsonDocument.Parse(json);
        var oversized = doc.RootElement.GetProperty("oversizedUnits").EnumerateArray().ToList();
        Assert.Single(oversized);
        Assert.Equal("big", oversized[0].GetProperty("name").GetString());
        Assert.Equal(9_000_000 - 350_000, oversized[0].GetProperty("overBudgetBy").GetInt64());

        Assert.Contains("EXCEEDING BUDGET", log, System.StringComparison.Ordinal);
        Assert.Contains("big", log, System.StringComparison.Ordinal);
    }

    /// <summary>
    /// Unit-level detail is opt-in: at system-chunk scale a TU carries hundreds
    /// of units and the full listing dwarfs the summary a reviewer needs.
    /// </summary>
    [Fact]
    public void Report_OmitsUnitDetailUnlessVerbose()
    {
        var (json, _) = SplitReportBuilder.Build(TwoTus, 350_000, Sizes);
        using var doc = JsonDocument.Parse(json);
        var firstTu = doc.RootElement.GetProperty("tuList").EnumerateArray().First();
        // Present but null by default: the field documents that detail exists.
        Assert.Equal(JsonValueKind.Null, firstTu.GetProperty("units").ValueKind);
    }

    [Fact]
    public void Log_ShowsOneLinePerTuWithSizeAndReason()
    {
        var (_, log) = SplitReportBuilder.Build(TwoTus, 350_000, Sizes);
        Assert.Contains("payload.0", log, System.StringComparison.Ordinal);
        Assert.Contains("300,000", log, System.StringComparison.Ordinal);
        Assert.Contains("[observed-only]", log, System.StringComparison.Ordinal);
    }
}
