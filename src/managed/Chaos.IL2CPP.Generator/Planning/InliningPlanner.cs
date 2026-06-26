using System;
using System.Collections.Generic;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Inlining budget model and hot/cold classification for generated C++ methods.
///
/// Provides two services:
///   1. Inline budget analysis — determines whether a callee is cheap enough
///      to inline at the call site based on IR instruction count and complexity.
///   2. Hot/cold classification — classifies methods as hot (frequently called),
///      warm (moderate), or cold (rare) so the emitter can group them into
///      separate linker sections (.text$hot / .text$cold) for better i-cache
///      locality.
///
/// Design principles:
///   - Budget is conservative: under-estimating inlining is safer than over-
///     estimating (avoids code bloat and compile-time explosions).
///   - Classification is based on call-count thresholds from tiering data,
///     or static heuristics when profiling data is unavailable.
///   - Recursive calls are always excluded from inlining.
/// </summary>
internal sealed class InliningPlanner
{
    // ── Budget thresholds ──────────────────────────────────────────────
    // Inline a callee only when:
    //   - Its IR instruction count ≤ kMaxInlineInstructionCount AND
    //   - Its IR count ≤ kMaxInlineCallerFraction * caller_instruction_count
    //
    // These are conservative defaults tuned for the interpreter IR size.
    // Typical managed methods compile to 5-20 IR instructions; raising
    // beyond 16 without profiling risks code-bloat regressions.

    /// <summary>Max callee IR instructions for inlining (hard cap).</summary>
    internal const int kMaxInlineInstructionCount = 32;

    /// <summary>
    /// Max callee size as a fraction of caller size (e.g., 0.50f means
    /// callee must be ≤ 50% of caller's IR instruction count).
    /// </summary>
    internal const float kMaxInlineCallerFraction = 0.50f;

    /// <summary>
    /// Max nesting depth for inlining. Depth 1 = top-level call site;
    /// depth 2 = inline inside an inline; etc. Prevents runaway
    /// recursive inlining and C++ code bloat.
    /// </summary>
    internal const int kMaxInlineDepth = 3;

    /// <summary>Max callee IR instructions for inlining when call site is inside a loop.</summary>
    internal const int kMaxInlineLoopInstructionCount = 48;

    // ── Hot/cold thresholds ────────────────────────────────────────────
    // Methods with estimated call count exceeding these thresholds are
    // classified into temperature bands for code-section assignment.

    /// <summary>
    /// Minimum estimated call count to classify a method as hot.
    /// Methods above this threshold get .text$hot section placement.
    /// </summary>
    internal const int kHotCallThreshold = 100;

    /// <summary>
    /// Maximum estimated call count for a method to be classified as cold.
    /// Methods at or below this threshold get .text$cold section placement
    /// or no special placement (default .text).
    /// </summary>
    internal const int kColdCallThreshold = 10;

    // ── Section names ──────────────────────────────────────────────────
    internal const string SectionHot = ".text$hot";
    internal const string SectionWarm = ".text$warm";
    internal const string SectionCold = ".text$cold";

    // ── Inline candidate result ────────────────────────────────────────
    internal sealed record InlineCandidate
    {
        /// <summary>true when the callee qualifies for inlining.</summary>
        public bool CanInline { get; init; }

        /// <summary>Reason for the decision (diagnostic only).</summary>
        public string Reason { get; init; } = "";
    }

    // ── Complexity analysis ────────────────────────────────────────────
    // Rough estimate of per-instruction "weight" for different opcode
    // families. A method with many call/throw/complex instructions is
    // less profitable to inline even if the raw count is low.

    internal static int EstimateInstructionWeight(string opCode)
    {
        return opCode switch
        {
            "call" or "callvirt" => 3,  // call site — indirect cost
            "calli" or "cpblk"
                or "initblk" => 4,  // expensive runtime helpers
            "throw" or "rethrow" => 5,  // exceptional path
            "switch" => 3,  // multi-branch
            "newobj" or "box"
                or "unbox" => 2,  // allocation path
            "ldelem" or "stelem"
                or "ldelema" => 2,  // bounds-checked access
            "ldfld" or "stfld"
                or "ldsfld" or "stsfld" => 1,  // simple field access
            _ => 1,  // default weight
        };
    }

    /// <summary>
    /// Evaluate whether <paramref name="calleeInstructionCount"/> IR
    /// instructions should be inlined into a caller with
    /// <paramref name="callerInstructionCount"/> instructions.
    /// </summary>
    internal static InlineCandidate EvaluateInline(
        int calleeInstructionCount,
        int callerInstructionCount,
        bool isRecursive,
        int calleeEstimatedComplexity = -1,
        bool isInLoop = false)
    {
        if (isRecursive)
        {
            return new InlineCandidate
            {
                CanInline = false,
                Reason = "recursive call — excluded from inlining"
            };
        }

        int effectiveMax = isInLoop ? kMaxInlineLoopInstructionCount : kMaxInlineInstructionCount;
        if (calleeInstructionCount > effectiveMax)
        {
            return new InlineCandidate
            {
                CanInline = false,
                Reason = $"callee size ({calleeInstructionCount}) exceeds max inline count ({effectiveMax})"
            };
        }

        float fraction = (float)calleeInstructionCount / Math.Max(callerInstructionCount, 1);
        if (fraction > kMaxInlineCallerFraction)
        {
            return new InlineCandidate
            {
                CanInline = false,
                Reason = $"callee/caller ratio ({fraction:F2}) exceeds max ({kMaxInlineCallerFraction})"
            };
        }

        return new InlineCandidate
        {
            CanInline = true,
            Reason = "within budget"
        };
    }

    // ── Hot/cold classification ────────────────────────────────────────

    /// <summary>
    /// Section name for a method given its estimated call count.
    /// </summary>
    internal static string ClassifySection(int estimatedCallCount)
    {
        if (estimatedCallCount >= kHotCallThreshold)
            return SectionHot;
        if (estimatedCallCount <= kColdCallThreshold)
            return SectionCold;
        return SectionWarm;
    }

    /// <summary>
    /// Classify a method as hot based on static heuristics (e.g., it's
    /// an entry point, a frequently-used accessor, or explicitly marked).
    /// </summary>
    internal static string ClassifySectionByStaticHeuristic(
        string subjectId,
        bool isEntryPoint,
        int instructionCount)
    {
        // Entry points are always hot.
        if (isEntryPoint)
            return SectionHot;

        // Very small methods (getters, simple calcs) are warm+.
        if (instructionCount <= 5)
            return SectionWarm;

        // Default: cold.
        return SectionCold;
    }

    /// <summary>
    /// Get the MSVC `#pragma code_seg` directive for a section name.
    /// Returns empty string for the default .text section.
    /// </summary>
    internal static string GetSectionPragma(string sectionName)
    {
        if (string.IsNullOrEmpty(sectionName) || sectionName == ".text")
            return "";

        // MSVC: #pragma code_seg(".text$hot")
        // GCC/Clang: __attribute__((hot)) — we emit both for compatibility.
        // The MSVC pragma is linker-section-scoped; __attribute__ is hint-only.
        return $"#pragma code_seg(\"{sectionName}\")";
    }

    /// <summary>
    /// Get a C++ `__declspec` or `__attribute__` for the section.
    /// MSVC uses `__declspec(allocate(".text$hot"))` for data;
    /// for code, `#pragma code_seg` is the standard approach.
    /// This returns the attribute form for GCC/Clang compatibility.
    /// </summary>
    internal static string GetSectionAttribute(string sectionName)
    {
        // GCC/Clang: __attribute__((hot))/((cold)) — optimization hints for
        // the compiler to place hot/cold code in separate sections.
        // MSVC does not support __attribute__ — it uses #pragma code_seg
        // (handled via CMakeLists.txt) and emits C4235 for __attribute__.
        // Wrap in #ifndef _MSC_VER so the generated C++ compiles on all platforms.
        string attr = sectionName switch
        {
            SectionHot => "__attribute__((hot))",
            SectionCold => "__attribute__((cold))",
            _ => "",
        };
        return string.IsNullOrEmpty(attr) ? "" :
            $"#ifndef _MSC_VER\n {attr}\n#endif";
    }
}
