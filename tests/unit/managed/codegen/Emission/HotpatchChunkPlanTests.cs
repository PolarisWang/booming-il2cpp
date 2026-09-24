using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using Chaos.IL2CPP.Contracts;
using Chaos.IL2CPP.Generator.Tests.Infra;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Guards for chunking the hotpatch type/method index arrays.
///
/// <para>
/// The hotpatch arrays are index-coupled: <c>s_hotpatch_types[i].
/// first_method_index</c> is a <b>global</b> index into <c>s_hotpatch_methods</c>.
/// Chunking may therefore only change how an index is ADDRESSED, never what it
/// MEANS — a chunk that re-numbered from zero, or a cut placed inside a type's
/// method run, would make name lookup resolve to the wrong method with no
/// diagnostic at all.
/// </para>
///
/// <para>
/// These tests assert on the <b>emitted text</b> rather than on the planner's
/// own bookkeeping. That distinction is the point: an earlier revision of this
/// file tested a helper (<c>PlanHotpatchMethodChunks</c>) that was never called
/// from the emission path, so it passed while governing nothing. The
/// assertions below parse what codegen actually produced.
/// </para>
/// </summary>
public sealed class HotpatchChunkPlanTests
{
    private readonly PlannerFixture _fixture = new();

    /// <summary>
    /// Builds a model with enough hotpatchable methods to force chunking, then
    /// returns the emitted <c>hotpatch</c> payload section.
    /// </summary>
    private string EmitHotpatchTable(int methodCount)
    {
        var methods = new List<AotCoreIrMethodArtifact>(methodCount);
        for (int i = 0; i < methodCount; i++)
        {
            // Distinct declaring types so the chunker has type boundaries to cut
            // on — the split points are types, not methods.
            methods.Add(ModelFactory.CreateMethod(
                $"TestModule.Type{i / 8}::M{i}:System.Int32()",
                returnType: "System.Int32",
                returnAbi: ModelFactory.Int32Abi,
                instructions: new[]
                {
                    ModelFactory.Instruction("ldc.i4", ilOffset: 0, intOperand: i),
                    ModelFactory.Instruction("ret", ilOffset: 1),
                }));
        }

        var artifact = ModelFactory.CreateArtifact(methods.ToArray());
        var loweringPlan = ModelFactory.CreateDefaultPlan(methods[0].SubjectId);
        var manifest = ModelFactory.CreateDefaultManifest(
            inputAssemblyPath: PlannerFixture.StubAssemblyPath);

        var model = _fixture.RunPlanner(artifact, loweringPlan, manifest);
        var section = model.PayloadSections?.FirstOrDefault(s => s.Name == "hotpatch");
        Assert.True(section is not null,
            "the planner must expose a 'hotpatch' payload section; without it these "
            + "guards would assert on nothing");
        return section!.Content;
    }

    // ── Small modules keep the flat layout ──────────────────────────────

    /// <summary>
    /// Under budget the module must emit the ORIGINAL flat shape and leave the
    /// chunk fields implicit (null). That null-chunk path is what every
    /// pre-C3 module takes, so keeping it exercised matters.
    /// </summary>
    [Fact]
    public void SmallBlock_EmitsFlatLayoutWithoutChunks()
    {
        string text = EmitHotpatchTable(methodCount: 40);

        Assert.Contains("s_hotpatch_methods[", text);
        Assert.DoesNotContain("s_hotpatch_methods_0[", text);
        Assert.DoesNotContain("s_hotpatch_type_chunks", text);

        Assert.Contains(".type_entries           = s_hotpatch_types,", text);
        Assert.Contains(".method_entries         = s_hotpatch_methods,", text);
    }

    // ── Large modules chunk ─────────────────────────────────────────────

    [Fact]
    public void LargeBlock_IsChunked()
    {
        string text = EmitHotpatchTable(methodCount: 12_000);

        Assert.Contains("s_hotpatch_methods_0[", text);
        Assert.Contains("s_hotpatch_type_chunks", text);
        Assert.Contains("s_hotpatch_method_chunks", text);

        // Chunked modules must NOT also publish the flat pointers: the runtime
        // prefers the chunk list, and a populated flat pointer would invite a
        // caller to walk the array as if it were contiguous.
        Assert.Contains(".type_entries           = nullptr,", text);
        Assert.Contains(".method_entries         = nullptr,", text);
    }

    /// <summary>
    /// 🔴 Chunk counts must sum to the declared totals, and the module's counts
    /// must describe the WHOLE logical array.
    ///
    /// If a chunk count were short, the tail of the method array becomes
    /// unreachable — lookups near the end resolve to nullptr and the method
    /// silently reports "not found" rather than failing loudly.
    /// </summary>
    [Fact]
    public void ChunkCounts_SumToDeclaredTotals()
    {
        string text = EmitHotpatchTable(methodCount: 12_000);

        var typeChunkCounts = Regex.Matches(text, @"s_hotpatch_types_\d+\[(\d+)\]")
            .Select(m => int.Parse(m.Groups[1].Value)).ToList();
        var methodChunkCounts = Regex.Matches(text, @"s_hotpatch_methods_\d+\[(\d+)\]")
            .Select(m => int.Parse(m.Groups[1].Value)).ToList();

        Assert.True(typeChunkCounts.Count > 1, "expected multiple type chunks");
        Assert.True(methodChunkCounts.Count > 1, "expected multiple method chunks");
        Assert.Equal(typeChunkCounts.Count, methodChunkCounts.Count);

        int declaredTypes = int.Parse(Regex.Match(text,
            @"\.type_entry_count\s+=\s+(\d+)u").Groups[1].Value);
        int declaredMethods = int.Parse(Regex.Match(text,
            @"\.method_entry_count\s+=\s+(\d+)u").Groups[1].Value);

        Assert.Equal(declaredTypes, typeChunkCounts.Sum());
        Assert.Equal(declaredMethods, methodChunkCounts.Sum());
    }

    /// <summary>
    /// 🔴 A chunk must never cut through a type's method run. Every type's
    /// <c>(first_method_index, method_count)</c> range has to sit wholly inside
    /// one chunk, or the runtime's global-index arithmetic resolves into the
    /// wrong chunk.
    ///
    /// Asserted structurally: each chunk's method count must equal the sum of
    /// the <c>method_count</c> values declared by the type entries in that same
    /// chunk.
    /// </summary>
    [Fact]
    public void Chunks_BreakOnlyOnTypeBoundaries()
    {
        string text = EmitHotpatchTable(methodCount: 12_000);

        var typeChunks = Regex.Matches(text,
            @"s_hotpatch_types_(\d+)\[(\d+)\] = \{(.*?)\n\};", RegexOptions.Singleline);
        var methodChunks = Regex.Matches(text,
            @"s_hotpatch_methods_(\d+)\[(\d+)\] = \{(.*?)\n\};", RegexOptions.Singleline);
        Assert.True(typeChunks.Count > 1, "expected multiple type chunks");

        foreach (Match tc in typeChunks)
        {
            int index = int.Parse(tc.Groups[1].Value);
            var mc = methodChunks.Cast<Match>()
                .FirstOrDefault(m => m.Groups[1].Value == tc.Groups[1].Value);
            Assert.True(mc is not null, $"no method chunk paired with type chunk {index}");

            // The type entries end with `, <method_count>u },` — sum those.
            long declaredInChunk = Regex.Matches(tc.Groups[3].Value, @",\s*(\d+)u\s*\}")
                .Select(m => long.Parse(m.Groups[1].Value))
                .Sum();

            int methodChunkSize = int.Parse(mc!.Groups[2].Value);
            Assert.Equal(methodChunkSize, declaredInChunk);
        }
    }

    /// <summary>
    /// Chunk arrays must have distinct symbol names, or the generated TU has
    /// duplicate definitions and fails to compile.
    /// </summary>
    [Fact]
    public void ChunkSymbolNames_AreDistinct()
    {
        string text = EmitHotpatchTable(methodCount: 12_000);

        foreach (var prefix in new[] { "s_hotpatch_types_", "s_hotpatch_methods_" })
        {
            var names = Regex.Matches(text, Regex.Escape(prefix) + @"(\d+)\[")
                .Select(m => m.Groups[1].Value).ToList();
            Assert.Equal(names.Count, names.Distinct(StringComparer.Ordinal).Count());
        }
    }

    /// <summary>
    /// Each chunk descriptor must carry the element size the runtime uses to
    /// compute offsets; a wrong size silently mis-addresses every entry after
    /// the first.
    /// </summary>
    [Fact]
    public void ChunkDescriptors_CarryElementSizes()
    {
        string text = EmitHotpatchTable(methodCount: 12_000);

        Assert.Contains("sizeof(HotpatchTypeEntryV0)", text);
        Assert.Contains("sizeof(HotpatchMethodEntryV0)", text);
    }
}
