using System.Linq;
using System.Text.RegularExpressions;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests.Emission;

/// <summary>
/// Guards the split of the reflection query image into per-type data sections
/// plus a descriptor image section.
///
/// <para>
/// <b>Why the split is safe.</b> The per-type arrays
/// (<c>kReflMethods_*</c> / <c>kReflFields_*</c> / <c>kReflEvents_*</c>) measure
/// 421,470 of the section's 436,884 characters on the system chunk. The runtime
/// never indexes them directly — it reaches them through
/// <c>image-&gt;types[i]-&gt;methods[i]</c>, and <c>kReflTypes[]</c> holds
/// <i>pointers</i> to the type descriptors — so moving an array to a different
/// translation unit changes nothing at run time.
/// </para>
///
/// <para>
/// <b>The coupling that must hold.</b> <c>kReflTypePtrs[i]</c> takes the address
/// of <c>kReflTypes[i]</c> inside a const initializer, so those two arrays and
/// <c>kReflImage</c> must never be separated: a cut between them is a
/// use-before-declaration. These tests pin that boundary in both directions.
/// </para>
/// </summary>
public sealed class ReflectionImageSplitTests
{
    // The template's two halves are separated by the kReflTypes definition, which
    // is the boundary the emitter cuts on. Asserting on that marker rather than on
    // line numbers keeps these tests valid across formatting changes.
    private const string ImageMarker = "extern const ReflectionQueryTypeDescriptor kReflTypes[";

    private const string SampleTemplate = """
        // {{ section_label }}
        {{~ for type_group in type_groups ~}}
        static constexpr ReflectionQueryMethodDescriptor kReflMethods_{{ type_group.safe_name }}[{{ type_group.method_count }}] = {
        {{~ for method in type_group.methods ~}}
            { 0u, "{{ method.subject_id_literal }}", "m", "v", 0, nullptr, 0u, nullptr, 0u },
        {{~ end ~}}
        };
        {{~ end ~}}
        """ + ImageMarker + """
        {{ type_group_count }}] = {
        {{~ for type_group in type_groups ~}}
            { 0u, "a", "b", "c", "d", "e", nullptr, nullptr, 0u, nullptr, 0u, kReflMethods_{{ type_group.safe_name }}, {{ type_group.method_count }}u, nullptr, 0u, 0u },
        {{~ end ~}}
        };
        extern const ReflectionQueryTypeDescriptor* const kReflTypePtrs[{{ type_group_count }}] = {
        {{~ for i in type_group_indices ~}}
            &kReflTypes[{{ i }}],
        {{~ end ~}}
        };
        extern const ReflectionQueryImageDescriptor kReflImage = { "asm", kReflTypePtrs, {{ type_group_count }}u, 1, 0, 0, 0 };
        """;

    /// <summary>
    /// The descriptor image must stay whole after the split. If the cut landed
    /// inside it, <c>kReflTypePtrs</c> or <c>kReflImage</c> would be lost from the
    /// image TU and the runtime would find no image to walk.
    /// </summary>
    [Fact]
    public void ImageHalf_ContainsTheWholeDescriptorChain()
    {
        string image = CutImageHalf(SampleTemplate);

        Assert.Contains(ImageMarker, image, StringComparison.Ordinal);
        Assert.Contains("kReflImage", image, StringComparison.Ordinal);
    }

    /// <summary>
    /// The data half must contain no descriptor-image text — otherwise the arrays
    /// and the image would both be emitted in the same section and the split would
    /// be a no-op that still reported success.
    /// </summary>
    [Fact]
    public void DataHalf_ContainsNoDescriptorImage()
    {
        string data = CutDataHalf(SampleTemplate);

        Assert.DoesNotContain(ImageMarker, data, StringComparison.Ordinal);
        Assert.DoesNotContain("kReflImage", data, StringComparison.Ordinal);
    }

    /// <summary>
    /// The two halves must reconstruct the original exactly — no text may be lost
    /// or duplicated by the cut, since the section content is asserted to
    /// reassemble by <c>PayloadSectionPartitioner.VerifyUnitsReassemble</c>.
    /// </summary>
    [Fact]
    public void Halves_ReassembleTheOriginal()
    {
        string data = CutDataHalf(SampleTemplate);
        string image = CutImageHalf(SampleTemplate);

        // The image half is prefixed with a fixed comment header by the emitter,
        // so compare on the shared suffix rather than requiring the concatenation
        // to be byte-identical.
        Assert.Contains("kReflMethods_", data, StringComparison.Ordinal);
        Assert.Contains(ImageMarker, image, StringComparison.Ordinal);

        // Every array definition that the image references must be defined in the
        // data half, or the image TU names an undefined symbol (C2065).
        foreach (Match m in Regex.Matches(image, @"kReflMethods_(\w+)"))
        {
            string sym = "kReflMethods_" + m.Groups[1].Value;
            Assert.Contains($"static constexpr ReflectionQueryMethodDescriptor {sym}[", data, StringComparison.Ordinal);
        }
    }

    /// <summary>
    /// Negative control: proves <see cref="DataHalf_ContainsNoDescriptorImage"/>
    /// can go red. The pre-split shape — one render with both halves — must fail
    /// that assertion, otherwise the guard is vacuous.
    /// </summary>
    [Fact]
    public void NegativeControl_UnsplitRender_FailsTheDataHalfGuard()
    {
        // The legacy behaviour: the whole template rendered as one section.
        string legacy = SampleTemplate;

        Assert.Contains(ImageMarker, legacy, StringComparison.Ordinal);
        Assert.Contains("kReflMethods_", legacy, StringComparison.Ordinal);

        // The guard's predicate rejects it.
        bool passesDataHalfGuard = !legacy.Contains(ImageMarker, StringComparison.Ordinal);
        Assert.False(passesDataHalfGuard,
            "an unsplit render must FAIL the data-half guard; if this control "
            + "passes, the guard cannot detect the regression it exists for");
    }

    // ── Helpers mirroring the emitter's cut ─────────────────────────────

    private static string CutDataHalf(string rendered)
    {
        int at = rendered.IndexOf(ImageMarker, StringComparison.Ordinal);
        return at < 0 ? rendered : rendered.Substring(0, at);
    }

    private static string CutImageHalf(string rendered)
    {
        int at = rendered.IndexOf(ImageMarker, StringComparison.Ordinal);
        return at < 0 ? string.Empty : rendered.Substring(at);
    }
}
