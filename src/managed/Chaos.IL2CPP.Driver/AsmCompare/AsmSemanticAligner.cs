using System.Linq;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Driver;

/// <summary>
/// Phase 2: Semantic alignment engine that maps JIT basic blocks to AOT IR blocks
/// using IL offset as anchor points, with semantic labels (entry, guard-null,
/// range-check, call, return, gc-poll, box, unbox).
/// </summary>
internal static class AsmSemanticAligner
{
    public sealed record AlignedBlock
    {
        public int IlOffsetStart { get; set; }
        public int IlOffsetEnd { get; set; }
        public string Label { get; set; } = "";
        public string? SemanticTag { get; set; }
        public List<string> JitInstructions { get; init; } = [];
        public List<string> AotInstructions { get; init; } = [];
        public List<string> IlInstructions { get; init; } = [];
    }

    /// <summary>
    /// Align JIT asm blocks with AOT IR blocks using IL offset anchors.
    /// Returns a list of aligned blocks with IL context and semantic tags.
    /// </summary>
    public static IReadOnlyList<AlignedBlock> Align(
        JitAsmCapture.JitCaptureResult jitResult,
        AotCoreIrMethodArtifact aotMethod)
    {
        var blocks = new List<AlignedBlock>();
        var ilInstructions = jitResult.IlBody?.Instructions;

        if (ilInstructions is null || ilInstructions.Count == 0)
        {
            blocks.Add(new AlignedBlock
            {
                IlOffsetStart = 0,
                Label = "(no IL data)",
                SemanticTag = "unknown",
                AotInstructions = aotMethod.Instructions
                    .Select(i => FormatAotIrLine(i))
                    .ToList(),
            });
            return blocks;
        }

        // Group AOT IR by IL offset
        var aotByOffset = new SortedDictionary<int, List<AotCoreIrInstructionArtifact>>();
        foreach (var instr in aotMethod.Instructions)
        {
            if (!aotByOffset.ContainsKey(instr.IlOffset))
                aotByOffset[instr.IlOffset] = [];
            aotByOffset[instr.IlOffset].Add(instr);
        }

        // Group JIT instructions by IL offset ranges
        // Since JIT doesn't expose IL offsets, we approximate:
        // Each IL instruction maps to some contiguous JIT instructions.
        // We split JIT instructions proportionally across IL instructions.
        var jitInstructions = jitResult.Instructions;
        int jitPerIl = jitInstructions is not null
            ? Math.Max(1, jitInstructions.Count / ilInstructions.Count)
            : 0;

        // Build blocks from IL instructions
        for (int i = 0; i < ilInstructions.Count; i++)
        {
            var il = ilInstructions[i];
            int offset = il.Offset;

            // Determine block boundaries: look for branch targets (labels)
            // IL branch instructions mark potential block boundaries
            bool isBranchTarget = IsBranchTarget(il, ilInstructions);
            bool isBranchOpcode = il.OpCode.StartsWith("br") ||
                                  il.OpCode.StartsWith("b.") ||
                                  il.OpCode == "leave" ||
                                  il.OpCode == "switch" ||
                                  il.OpCode == "ret";

            // Detect semantic tags from opcode patterns
            string? semanticTag = DetectSemanticTag(il, i, ilInstructions);

            // Gather corresponding JIT instructions
            var jitLines = new List<string>();
            if (jitInstructions is not null)
            {
                int start = i * jitPerIl;
                int end = Math.Min(start + jitPerIl, jitInstructions.Count);
                for (int j = start; j < end && j < jitInstructions.Count; j++)
                {
                    var ji = jitInstructions[j];
                    var hex = string.Join(" ", ji.RawBytes.Take(8).Select(b => $"{b:x2}"));
                    if (ji.RawBytes.Length > 8) hex += "...";
                    jitLines.Add($"{ji.Mnemonic,-10} {ji.Operands,-35} ; {hex}");
                }
            }

            // Gather AOT IR for this offset
            var aotLines = new List<string>();
            if (aotByOffset.TryGetValue(offset, out var aotInstrs))
            {
                foreach (var aot in aotInstrs)
                    aotLines.Add(FormatAotIrLine(aot));
            }

            // Determine label
            string label = $"IL_{offset:x4}";

            // Merge with previous block if both are low-value
            if (blocks.Count > 0 &&
                string.IsNullOrEmpty(semanticTag) &&
                string.IsNullOrEmpty(blocks[^1].SemanticTag) &&
                !isBranchTarget && !isBranchOpcode)
            {
                // Extend previous block instead of creating many tiny fragments
                var prev = blocks[^1];
                prev.IlOffsetEnd = offset;
                prev.JitInstructions.AddRange(jitLines);
                prev.AotInstructions.AddRange(aotLines);
                prev.IlInstructions.Add($"{il.OpCode} {il.Operand}".Trim());
                continue;
            }

            blocks.Add(new AlignedBlock
            {
                IlOffsetStart = offset,
                IlOffsetEnd = offset,
                Label = label,
                SemanticTag = semanticTag,
                JitInstructions = jitLines,
                AotInstructions = aotLines,
                IlInstructions = [$"{il.OpCode} {il.Operand}".Trim()],
            });
        }

        return blocks;
    }

    private static bool IsBranchTarget(JitAsmCapture.IlInstruction il,
        IReadOnlyList<JitAsmCapture.IlInstruction> allInstructions)
    {
        // Check if any instruction branches to this offset
        foreach (var instr in allInstructions)
        {
            if (string.IsNullOrEmpty(instr.Operand))
                continue;

            // Parse branch operand format "IL_xxxx"
            if (instr.Operand.StartsWith("IL_"))
            {
                var targetStr = instr.Operand;
                // Parse the hex after IL_
                if (int.TryParse(targetStr[3..], System.Globalization.NumberStyles.HexNumber, null, out int target))
                {
                    if (target == il.Offset)
                        return true;
                }
            }
        }
        return false;
    }

    private static string? DetectSemanticTag(JitAsmCapture.IlInstruction il, int index,
        IReadOnlyList<JitAsmCapture.IlInstruction> allInstructions)
    {
        // Entry point
        if (index == 0)
            return "entry";

        // Return
        if (il.OpCode == "ret")
            return "return";

        // Null check (ldnull + brtrue/brfalse pattern)
        if (il.OpCode == "ldnull" && index + 1 < allInstructions.Count)
        {
            var next = allInstructions[index + 1];
            if (next.OpCode is "brtrue" or "brfalse")
                return "guard-null";
        }

        // Call
        if (il.OpCode is "call" or "callvirt" or "newobj")
            return "call";

        // Box
        if (il.OpCode == "box")
            return "box";

        // Unbox
        if (il.OpCode is "unbox" or "unbox.any")
            return "unbox";

        // Bounds check (ldelem/stelem often paired with range checks)
        if (il.OpCode is "ldelem" or "ldelem.i4" or "ldelem.i8" or "ldelem.ref" or "ldelem.u1" or "ldelem.u2" or
            "stelem" or "stelem.i4" or "stelem.ref" or "stelem.i1")
            return "range-check";

        // Array length
        if (il.OpCode == "ldlen")
            return "array-length";

        // Switch
        if (il.OpCode == "switch")
            return "dispatch";

        return null;
    }

    private static string FormatAotIrLine(AotCoreIrInstructionArtifact instr)
    {
        var op = instr.Op;

        if (!string.IsNullOrEmpty(instr.Callee))
            return $"{op,-25} {instr.Callee}";

        if (instr.Operand is not null)
        {
            var val = instr.Operand.ToString() ?? "";
            if (val.Length > 50) val = val[..47] + "...";
            return $"{op,-25} {val}";
        }

        if (instr.RuntimeServiceKind is not null)
            return $"{op,-25} [svc:{instr.RuntimeServiceKind}]";

        if (!string.IsNullOrEmpty(instr.TargetSymbol))
            return $"{op,-25} [{instr.TargetSymbol}]";

        return $"{op,-25}";
    }

    // ── Native-vs-native alignment (Phase 3) ────────────────────────────────

    /// <summary>
    /// Align JIT native blocks with AOT native blocks using basic block detection.
    /// Blocks are detected by splitting at ret/jmp/jcc boundaries and aligned by
    /// position (1st JIT block ↔ 1st AOT block, etc.).
    /// </summary>
    public static IReadOnlyList<AlignedBlock> AlignNativeVsNative(
        JitAsmCapture.JitCaptureResult jitResult,
        JitAsmCapture.JitCaptureResult aotResult)
    {
        var jitBlocks = DetectNativeBlocks(jitResult);
        var aotBlocks = DetectNativeBlocks(aotResult);

        var blocks = new List<AlignedBlock>();
        int maxBlocks = Math.Max(jitBlocks.Count, aotBlocks.Count);

        for (int i = 0; i < maxBlocks; i++)
        {
            blocks.Add(new AlignedBlock
            {
                IlOffsetStart = i,
                Label = $"Block_{i}",
                JitInstructions = i < jitBlocks.Count ? jitBlocks[i] : [],
                AotInstructions = i < aotBlocks.Count ? aotBlocks[i] : [],
            });
        }

        return blocks;
    }

    private static List<List<string>> DetectNativeBlocks(JitAsmCapture.JitCaptureResult result)
    {
        if (result.Instructions is null || result.Instructions.Count == 0)
            return [];

        var blocks = new List<List<string>>();
        var current = new List<string>();

        foreach (var instr in result.Instructions)
        {
            var formatted = FormatNativeInstr(instr);
            current.Add(formatted);

            // Block-ending instructions
            if (instr.Mnemonic is "ret" or "retn" or "retf")
            {
                blocks.Add(current);
                current = [];
            }
            else if (instr.Mnemonic == "jmp")
            {
                blocks.Add(current);
                current = [];
            }
            else if (instr.Mnemonic.Length <= 3 && instr.Mnemonic.StartsWith('j') &&
                     instr.Mnemonic is not "jo" and not "jn" and not "jp" and not "js")
            {
                // conditional jump: current block ends, next instruction starts new block
                blocks.Add(current);
                current = [];
            }
        }

        if (current.Count > 0)
            blocks.Add(current);

        if (blocks.Count == 0)
            blocks.Add([]);

        return blocks;
    }

    private static string FormatNativeInstr(JitAsmCapture.DecodedInstruction instr)
    {
        var hex = string.Join(" ", instr.RawBytes.Take(8).Select(b => $"{b:x2}"));
        if (instr.RawBytes.Length > 8) hex += "...";
        return $"{instr.Mnemonic,-10} {instr.Operands,-35}; {hex}";
    }

    /// <summary>
    /// Render aligned native blocks in side-by-side JIT vs AOT format.
    /// </summary>
    public static string RenderNativeBlocks(IReadOnlyList<AlignedBlock> blocks)
    {
        var sb = new System.Text.StringBuilder();

        foreach (var block in blocks)
        {
            sb.AppendLine($"  {block.Label}");
            sb.AppendLine($"    {"J",-4} {"AOT",-4}");
            sb.AppendLine($"    {"-",-4} {"----",-4}");

            int maxLines = Math.Max(block.JitInstructions.Count, block.AotInstructions.Count);
            for (int i = 0; i < maxLines; i++)
            {
                var jit = i < block.JitInstructions.Count ? block.JitInstructions[i] : "";
                var aot = i < block.AotInstructions.Count ? block.AotInstructions[i] : "";
                sb.AppendLine($"    {jit,-50} {aot}");
            }

            sb.AppendLine();
        }

        return sb.ToString();
    }

    /// <summary>
    /// Render aligned blocks as text (used from SIDE-BY-SIDE for Phase 2).
    /// </summary>
    public static string RenderBlocks(IReadOnlyList<AlignedBlock> blocks)
    {
        var sb = new System.Text.StringBuilder();

        foreach (var block in blocks)
        {
            // Block header with semantic tag
            var tag = !string.IsNullOrEmpty(block.SemanticTag)
                ? $" [{block.SemanticTag}]"
                : "";
            sb.AppendLine($"  {block.Label}{tag}");

            // IL instructions
            foreach (var il in block.IlInstructions)
                sb.AppendLine($"    IL:      {il}");

            // AOT instructions
            foreach (var aot in block.AotInstructions)
                sb.AppendLine($"    AOT IR:  {aot}");

            // JIT instructions
            foreach (var jit in block.JitInstructions)
                sb.AppendLine($"    JIT asm: {jit}");

            sb.AppendLine();
        }

        return sb.ToString();
    }
}