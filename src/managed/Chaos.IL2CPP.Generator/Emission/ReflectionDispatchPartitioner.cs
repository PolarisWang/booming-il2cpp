using System;
using System.Collections.Generic;
using System.Text;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Splits the two giant reflection dispatch functions on page 0 into a shell plus
/// budget-sized <c>_partN</c> functions, so each piece can be emitted as its own
/// payload translation unit.
///
/// <para>
/// <b>Why.</b> <c>chaos_reflection_get_parameters_managed</c> (10,258 branches,
/// 4.35 MB) and <c>chaos_reflection_resolve_method_handle</c> (9,438 STRCMPs
/// across 89 cases, 1.97 MB) are unpartitionable code: paging splits
/// <c>templateModel.Methods</c>, and these are not methods in that sense. They
/// therefore stay whole on page 0 no matter how the rest is partitioned, which is
/// what keeps page 0 at 14 MB after pps3-1.
/// </para>
///
/// <para>
/// <b>Invariant preserved.</b> Both functions are pure dispatchers whose branches
/// only read their inputs and <c>return</c> — no branch writes state another
/// branch reads. Splitting is therefore semantics-preserving as long as each
/// branch body is moved <i>verbatim</i> and stays in its original relative order.
/// </para>
///
/// <para>
/// <b>Order is load-bearing.</b> A single <c>case</c> in
/// <c>resolve_method_handle</c> contains up to 16 <c>STRCMP(name, "AreEqual")</c>
/// tests returning <i>different</i> tokens; the semantics are "first match wins".
/// Reordering or de-duplicating those tests silently changes which token is
/// returned — no compile error, no exception, just a wrong answer. Every helper
/// here appends in input order and never sorts or de-dupes.
/// </para>
/// </summary>
public static class ReflectionDispatchPartitioner
{
    /// <summary>
    /// Branch budget per part. Chosen so an emitted part lands around 0.3–0.6 MB
    /// — comfortably inside the payload partitioner's per-TU budget, with margin
    /// for a single branch to overshoot when it has many parameters.
    /// </summary>
    public const int DefaultBranchesPerPart = 1500;

    /// <summary>
    /// One split-out <c>_partN</c> function: its suffix, full text, and branch count.
    /// </summary>
    public sealed record Part(string NameSuffix, string Text, int BranchCount);

    /// <summary>
    /// Splits a flat <c>if (cond) { ... return x; }</c> chain into budget-sized parts.
    ///
    /// <para>
    /// <paramref name="prologue"/> (the null check and handle resolution) and
    /// <paramref name="epilogue"/> (the fallback path) both stay in the shell:
    /// the epilogue must run exactly once, so appending it to a part would run it
    /// once per part that misses.
    /// </para>
    ///
    /// <para>
    /// Each part returns its branch's value, or <c>0</c> when nothing matched. A
    /// branch's value here is a GC-allocated array value, which is never 0, so 0
    /// is an unambiguous "not mine" signal for the shell.
    /// </para>
    /// </summary>
    public static IReadOnlyList<Part> SplitFlatChain(
        string functionName,
        string returnType,
        string parameters,
        IReadOnlyList<string> branches,
        int branchesPerPart = DefaultBranchesPerPart,
        string? partParameters = null)
    {
        if (branchesPerPart <= 0)
            throw new ArgumentOutOfRangeException(nameof(branchesPerPart), branchesPerPart, "must be positive");

        // Parts receive the value(s) the branches would have read in the shell.
        // The shell names its parameters from the public signature, but the branch
        // bodies reference the shell's INTERNAL variable (e.g. `chaos_method_handle`),
        // so the part must expose that name as its parameter.
        partParameters ??= parameters;

        var parts = new List<Part>();
        int partIndex = 0;

        for (int start = 0; start < branches.Count; start += branchesPerPart)
        {
            int count = Math.Min(branchesPerPart, branches.Count - start);
            var sb = new StringBuilder();

            sb.Append("extern \"C\" ").Append(returnType).Append(' ')
              .Append(functionName).Append("_part").Append(partIndex)
              .Append('(').Append(partParameters).Append(") noexcept").Append('\n');
            sb.Append("{\n");
            for (int i = start; i < start + count; i++)
                sb.Append(branches[i]);
            // 0 == "no branch in this part matched"; the shell tries the next part.
            sb.Append("    return 0;\n");
            sb.Append("}\n\n");

            // BranchCount is the splitter's own accounting; it is validated against
            // the emitted text by the guard tests.
            parts.Add(new Part($"part{partIndex}", sb.ToString(), count));
            partIndex++;
        }

        return parts;
    }

    /// <summary>
    /// Builds the shell that calls each part in order and falls through to the
    /// original epilogue when none matches.
    /// </summary>
    /// <param name="argumentList">
    /// Arguments forwarded to each part, naming the shell's own parameters
    /// (e.g. <c>chaos_method_handle</c>).
    /// </param>
    public static string BuildFlatChainShell(
        string functionName,
        string returnType,
        string parameters,
        string prologue,
        IReadOnlyList<Part> parts,
        string argumentList,
        string epilogue)
    {
        var sb = new StringBuilder();
        sb.Append("extern \"C\" ").Append(returnType).Append(' ')
          .Append(functionName).Append('(').Append(parameters).Append(") noexcept").Append('\n');
        sb.Append("{\n");
        sb.Append(prologue);

        foreach (var part in parts)
        {
            sb.Append("    {\n");
            sb.Append("        const auto chaos_part_value = ")
              .Append(functionName).Append('_').Append(part.NameSuffix)
              .Append('(').Append(argumentList).Append(");\n");
            sb.Append("        if (chaos_part_value != 0)\n");
            sb.Append("        {\n");
            sb.Append("            return chaos_part_value;\n");
            sb.Append("        }\n");
            sb.Append("    }\n");
        }

        sb.Append(epilogue);
        sb.Append("}\n\n");
        return sb.ToString();
    }

    /// <summary>
    /// Splits a <c>switch</c> body into budget-sized parts, treating each
    /// <c>case</c> block as an indivisible unit.
    ///
    /// <para>
    /// <b>Blocks are never cut.</b> A block is added to the current part until
    /// doing so would exceed <paramref name="branchesPerPart"/>, at which point a
    /// new part starts. A block larger than the budget gets a part of its own
    /// rather than being divided — dividing it is exactly the failure this
    /// signature exists to prevent (see the class remarks on duplicate STRCMPs).
    /// The overshoot is bounded by one block, not by the whole chain.
    /// </para>
    ///
    /// <para>
    /// Each part is a complete <c>switch</c> with its own <c>default: break;</c>
    /// and trailing <c>return 0;</c>, so a miss falls through to the shell's next
    /// part and finally to the shell's own <c>return 0;</c> — the same result the
    /// unsplit function produced.
    /// </para>
    /// </summary>
    /// <param name="caseBlocks">
    /// One entry per <c>case</c> group, in emission order, with its STRCMP count.
    /// </param>
    public static IReadOnlyList<Part> SplitCaseBlocks(
        string functionName,
        string returnType,
        string parameters,
        IReadOnlyList<(string CaseText, int BranchCount)> caseBlocks,
        int branchesPerPart = DefaultBranchesPerPart)
    {
        if (branchesPerPart <= 0)
            throw new ArgumentOutOfRangeException(nameof(branchesPerPart), branchesPerPart, "must be positive");

        var parts = new List<Part>();
        var current = new StringBuilder();
        int currentBranches = 0;
        int partIndex = 0;

        void FlushPart()
        {
            if (current.Length == 0) return;

            var sb = new StringBuilder();
            sb.Append("extern \"C\" ").Append(returnType).Append(' ')
              .Append(functionName).Append("_part").Append(partIndex)
              .Append('(').Append(parameters).Append(") noexcept").Append('\n');
            sb.Append("{\n");
            sb.Append("    switch (chaos_type_handle)\n");
            sb.Append("    {\n");
            sb.Append(current);
            // A miss must not terminate the search: the shell tries the next part.
            sb.Append("        default:\n");
            sb.Append("            break;\n");
            sb.Append("    }\n");
            sb.Append("    return 0;\n");
            sb.Append("}\n\n");

            parts.Add(new Part($"part{partIndex}", sb.ToString(), currentBranches));
            partIndex++;
            current.Clear();
            currentBranches = 0;
        }

        foreach (var (caseText, branchCount) in caseBlocks)
        {
            if (currentBranches > 0 && currentBranches + branchCount > branchesPerPart)
                FlushPart();

            current.Append(caseText);
            currentBranches += branchCount;
        }
        FlushPart();

        return parts;
    }
}
