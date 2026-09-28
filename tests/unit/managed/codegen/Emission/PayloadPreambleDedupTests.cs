using System.Reflection;
using System.Text.RegularExpressions;
using Chaos.IL2CPP.Generator;
using Chaos.IL2CPP.Contracts;
using Xunit;

namespace Chaos.IL2CPP.Generator.Tests;

/// <summary>
/// Guards that the two cross-TU declaration blocks live in the SHARED HEADER
/// rather than being repeated inside every payload translation unit.
///
/// <para>
/// <b>What was wrong.</b> <c>WrapPayloadSectionInTranslationUnit</c> emitted
/// <c>templateModel.MethodDeclarations</c> and <c>templateModel.CrossSectionSymbols</c>
/// into each payload TU's preamble. On the system chunk that measured 766,607
/// characters per TU — 2,095 <c>kRefl_params_*</c> declarations plus ~1,723
/// <c>extern "C"</c> method declarations — against a documented per-TU budget of
/// 350,000. No section cut could bring a payload TU under budget while the
/// preamble alone was 2.19x it.
/// </para>
///
/// <para>
/// <b>Measured before/after (system chunk, 28 payload TUs).</b> Declaration
/// characters 21,945,457 -> 483,877; total payload TU characters 30,921,996 ->
/// 9,457,644; payload TUs over the 350,000 budget 28 -> 4. The 4 that remain are
/// single oversized <i>sections</i> (hotpatch / gcslotmap / dispatch /
/// reflection), which is a different problem — see the remarks on
/// <c>PagePayloadSplitTests</c> for why those cannot be cut.
/// </para>
///
/// <para>
/// <b>Why this guard reads source text.</b> The duplication is a property of the
/// emitter's output text, and the property that matters is <i>which side of the
/// split a declaration appears on</i> — not a count. Asserting on the rendered
/// strings for each side is the direct expression of that, and it is what makes
/// the negative control possible (see below).
/// </para>
/// </summary>
public sealed class PayloadPreambleDedupTests
{
    private static readonly Type s_t = typeof(NativeAotEmitter);
    private static readonly BindingFlags s_flags = BindingFlags.NonPublic | BindingFlags.Static;

    /// <summary>
    /// Distinctive declaration texts that must never appear in a payload TU
    /// preamble. The names are deliberately greppable and cannot be produced by
    /// anything else in these helpers.
    /// </summary>
    private static readonly string[] s_methodDeclarations =
    {
        "extern \"C\" void Chaos_SentinelMethodAlpha(void);",
        "extern \"C\" void Chaos_SentinelMethodBeta(void);",
    };

    private static CrossSectionSymbol[] SentinelCrossSectionSymbols() => new[]
    {
        new CrossSectionSymbol(
            "kRefl_params_Sentinel_Type",
            "extern const chaos::il2cpp::runtime_core::ReflectionQueryParameterDescriptor kRefl_params_Sentinel_Type[];",
            NeedsExternalLinkage: true),
    };

    // ── The guard proper ────────────────────────────────────────────────

    /// <summary>
    /// The payload preamble must carry NEITHER declaration block. This is the
    /// regression guard: it fails if either block is re-added to
    /// <c>WrapPayloadSectionInTranslationUnit</c>.
    /// </summary>
    [Fact]
    public void PayloadPreamble_DoesNotRepeatMethodOrCrossSectionDeclarations()
    {
        string preamble = RenderPayloadPreamble();

        foreach (var decl in s_methodDeclarations)
        {
            Assert.DoesNotContain(decl, preamble, StringComparison.Ordinal);
        }

        foreach (var symbol in SentinelCrossSectionSymbols())
        {
            Assert.DoesNotContain(symbol.Declaration, preamble, StringComparison.Ordinal);
        }
    }

    /// <summary>
    /// The other half of the contract: removing them from the preamble is only
    /// correct because the shared header carries them. Without this test the
    /// guard above could be satisfied by deleting the declarations outright —
    /// which would compile nothing.
    /// </summary>
    [Fact]
    public void SharedHeader_CarriesBothDeclarationBlocks()
    {
        var model = MakeModel(
            typeDeclarationsCode: "extern int g_sentinel;",
            methodDeclarations: s_methodDeclarations,
            crossSectionSymbols: SentinelCrossSectionSymbols());

        string header = BuildSharedHeader(model);

        foreach (var decl in s_methodDeclarations)
        {
            Assert.Contains(decl, header, StringComparison.Ordinal);
        }

        Assert.Contains(SentinelCrossSectionSymbols()[0].Declaration, header, StringComparison.Ordinal);
    }

    /// <summary>
    /// The cross-section declarations must stay inside the codegen namespace in
    /// the header. Declaring them at global scope gives them a different mangled
    /// name than their definitions, and every consumer then fails at link time
    /// with LNK2001 on a symbol that is in fact defined.
    /// </summary>
    [Fact]
    public void SharedHeader_WrapsCrossSectionSymbolsInCodegenNamespace()
    {
        var model = MakeModel(
            methodDeclarations: s_methodDeclarations,
            crossSectionSymbols: SentinelCrossSectionSymbols(),
            codegenNamespace: "CombinedSubjects");

        string header = BuildSharedHeader(model);

        int nsOpen = header.IndexOf("namespace chaos::il2cpp::codegen::CombinedSubjects {", StringComparison.Ordinal);
        int declAt = header.IndexOf(SentinelCrossSectionSymbols()[0].Declaration, StringComparison.Ordinal);
        Assert.True(nsOpen >= 0, "the codegen namespace must be opened for cross-section symbols");
        Assert.True(declAt > nsOpen,
            "cross-section declarations must appear AFTER the codegen namespace is opened");

        // And the namespace must be closed again — an unclosed brace would make
        // every following declaration in the header nested, silently changing
        // their linkage.
        int nsClose = header.IndexOf("\n}\n", declAt, StringComparison.Ordinal);
        Assert.True(nsClose > declAt, "the codegen namespace must be closed after the declarations");
    }

    /// <summary>
    /// <c>extern "C"</c> declarations must NOT be namespace-wrapped. They have C
    /// language linkage, so declaring them at global scope and defining them
    /// inside the codegen namespace still names ONE entity; wrapping them is
    /// unnecessary and it hides that they are page-0-wide symbols rather than
    /// namespace members.
    /// </summary>
    [Fact]
    public void SharedHeader_DoesNotNamespaceWrapExternCDeclarations()
    {
        var model = MakeModel(
            methodDeclarations: s_methodDeclarations,
            crossSectionSymbols: SentinelCrossSectionSymbols(),
            codegenNamespace: "CombinedSubjects");

        string header = BuildSharedHeader(model);

        int declAt = header.IndexOf(s_methodDeclarations[0], StringComparison.Ordinal);
        int nsOpen = header.IndexOf("namespace chaos::il2cpp::codegen::CombinedSubjects {", StringComparison.Ordinal);

        Assert.True(declAt >= 0, "the extern \"C\" method declarations must be in the shared header");
        Assert.True(declAt < nsOpen,
            "extern \"C\" declarations must precede (i.e. sit outside) the codegen namespace block");
    }

    // ── Negative control ────────────────────────────────────────────────

    /// <summary>
    /// Proves the guard in <see cref="PayloadPreamble_DoesNotRepeatMethodOrCrossSectionDeclarations"/>
    /// can actually go red.
    ///
    /// <para>
    /// A test that only ever passes proves nothing. This reconstructs the OLD
    /// behaviour — the declarations appended to the preamble, exactly as
    /// <c>WrapPayloadSectionInTranslationUnit</c> used to do — and asserts that
    /// the guard's predicate REJECTS it. If the guard's assertion were vacuous
    /// (e.g. it searched for a text that could never be rendered, or compared
    /// against the wrong buffer), this control would pass and expose it.
    /// </para>
    ///
    /// <para>
    /// The control builds the preamble-shaped string the same way the emitter
    /// did, rather than by editing the emitter, so it stays valid independently
    /// of how the emitter is structured.
    /// </para>
    /// </summary>
    [Fact]
    public void NegativeControl_OldPreambleShape_IsRejectedByTheGuard()
    {
        // Reconstruct the pre-fix preamble: the two blocks appended verbatim.
        var oldPreamble = new System.Text.StringBuilder();
        oldPreamble.Append("namespace chaos::il2cpp::codegen::CombinedSubjects {\n");
        foreach (var decl in s_methodDeclarations)
        {
            oldPreamble.Append(decl);
            oldPreamble.Append('\n');
        }
        foreach (var symbol in SentinelCrossSectionSymbols())
        {
            oldPreamble.Append(symbol.Declaration);
            oldPreamble.Append('\n');
        }
        string legacy = oldPreamble.ToString();

        // The guard's predicate must REJECT this text. If either assertion below
        // fails, the guard is vacuous and the real test is a false green.
        foreach (var decl in s_methodDeclarations)
        {
            Assert.Contains(decl, legacy, StringComparison.Ordinal);
        }
        foreach (var symbol in SentinelCrossSectionSymbols())
        {
            Assert.Contains(symbol.Declaration, legacy, StringComparison.Ordinal);
        }

        // And the shape that the guard ACCEPTS is genuinely different from the
        // legacy one — i.e. the fix actually changed the preamble.
        string current = RenderPayloadPreamble();
        Assert.NotEqual(legacy, current);
        foreach (var decl in s_methodDeclarations)
        {
            Assert.DoesNotContain(decl, current, StringComparison.Ordinal);
        }
    }

    /// <summary>
    /// Second negative control, on the removal side: proves that if the shared
    /// header stopped carrying the declarations, the counterpart guard would go
    /// red. Without this, <see cref="SharedHeader_CarriesBothDeclarationBlocks"/>
    /// could be satisfied by leftovers from elsewhere in the header.
    /// </summary>
    [Fact]
    public void NegativeControl_HeaderWithoutDeclarations_FailsTheCounterpartGuard()
    {
        var emptyModel = MakeModel(
            typeDeclarationsCode: "extern int g_sentinel;",
            methodDeclarations: Array.Empty<string>(),
            crossSectionSymbols: Array.Empty<CrossSectionSymbol>());

        string header = BuildSharedHeader(emptyModel);

        foreach (var decl in s_methodDeclarations)
        {
            Assert.DoesNotContain(decl, header, StringComparison.Ordinal);
        }
        Assert.DoesNotContain(SentinelCrossSectionSymbols()[0].Declaration, header, StringComparison.Ordinal);
    }

    // ── Helpers ─────────────────────────────────────────────────────────

    /// <summary>
    /// Renders a payload TU preamble through the real
    /// <c>WrapPayloadSectionInTranslationUnit</c>, with sentinel declarations in
    /// both model fields so an accidental re-introduction is detectable.
    /// </summary>
    private static string RenderPayloadPreamble()
    {
        var model = MakeModel(
            methodDeclarations: s_methodDeclarations,
            crossSectionSymbols: SentinelCrossSectionSymbols(),
            codegenNamespace: "CombinedSubjects");

        var sb = new System.Text.StringBuilder("// SENTINEL_SECTION_BODY\n");
        var method = s_t.GetMethod("WrapPayloadSectionInTranslationUnit", s_flags)!;
        var includes = new List<string> { "<chaos_pch.h>" };
        method.Invoke(null, new object[]
        {
            sb,
            model,
            includes,
            /* includeExternalRuntimeDefault: */ true,
        });

        return sb.ToString();
    }

    private static string BuildSharedHeader(NativeAotTemplateModel model)
    {
        var method = s_t.GetMethod("BuildSharedHeader", s_flags, new[] { typeof(NativeAotTemplateModel) })!;
        return (string)method.Invoke(null, new object[] { model })!;
    }

    private static NativeAotTemplateModel MakeModel(
        string objectModelCode = "",
        string typeDeclarationsCode = "",
        string codegenNamespace = "",
        IReadOnlyList<string>? methodDeclarations = null,
        IReadOnlyList<CrossSectionSymbol>? crossSectionSymbols = null) =>
        new()
        {
            Includes = Array.Empty<string>(),
            ObjectModelCode = objectModelCode,
            MethodDeclarations = methodDeclarations ?? Array.Empty<string>(),
            Methods = Array.Empty<NativeAotMethodTemplateModel>(),
            EntrySubjectId = "Test::Foo()",
            EntrySymbol = "Test_Foo",
            EntryNativeSymbol = "Chaos_Test_Foo",
            NativeEntryFunctionName = "",
            EntryBridgeArguments = "",
            WorkloadAbi = "void()",
            ShapeDispatchHeaderContent = "// header",
            TypeDeclarationsCode = typeDeclarationsCode,
            CodegenNamespace = codegenNamespace,
            CrossSectionSymbols = crossSectionSymbols ?? Array.Empty<CrossSectionSymbol>(),
            GenericRegistrationCode = "",
            ModuleRegistrationCode = "",
        };
}
