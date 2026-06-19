using System.Collections.Generic;
using System.Linq;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private static string ClassifyIrreducibleReason(ControlFlowGraph cfg)
    {
        int unnaturalBackedges = 0;
        int crossNestedLoopPairs = 0;
        int switchWithLoopBack = 0;
        var blocks = cfg.Blocks;
        var headers = cfg.LoopHeaders;
        // Count unnatural backedges
        for (int i = 0; i < blocks.Count; i++)
        {
            var block = blocks[i];
            if (block.ConditionalTarget.HasValue)
            {
                if (cfg.OffsetToBlockIndex.TryGetValue(block.ConditionalTarget.Value, out var tgt) && tgt < i)
                {
                    if (!headers.ContainsKey(tgt) || !headers[tgt].LatchIndices.Contains(i))
                        unnaturalBackedges++;
                }
            }
            if (block.BranchTarget.HasValue)
            {
                if (cfg.OffsetToBlockIndex.TryGetValue(block.BranchTarget.Value, out var tgt) && tgt < i)
                {
                    if (!headers.ContainsKey(tgt) || !headers[tgt].LatchIndices.Contains(i))
                        unnaturalBackedges++;
                }
            }
            if (block.SwitchTargets.Count > 0)
            {
                foreach (var t in block.SwitchTargets)
                {
                    if (cfg.OffsetToBlockIndex.TryGetValue(t, out var tgt) && tgt < i)
                    {
                        if (!headers.ContainsKey(tgt))
                            switchWithLoopBack++;
                    }
                }
            }
        }
        // Count cross-nested loop pairs
        foreach (var kvp1 in headers)
        {
            foreach (var kvp2 in headers)
            {
                if (kvp1.Key >= kvp2.Key) continue;
                if (kvp1.Value.BodyIndices.Contains(kvp2.Key) &&
                    kvp2.Value.BodyIndices.Contains(kvp1.Key))
                    crossNestedLoopPairs++;
            }
        }
        // Determine primary reason
        if (crossNestedLoopPairs > 0 && unnaturalBackedges > 0)
            return $"multi-loop-cross-nested+{unnaturalBackedges}unat+{crossNestedLoopPairs}xnested";
        if (unnaturalBackedges > 1)
            return $"multi-entry-loop+{unnaturalBackedges}unat";
        if (switchWithLoopBack > 0)
            return $"switch-loop-back+{switchWithLoopBack}sw";
        if (unnaturalBackedges == 1)
            return $"single-unnatural-backedge";
        return $"unknown-irreducible-b{blocks}l{headers.Count}";
    }
    // ════════════════════════════════════════════════════════════════════════════
    // Report summary (call at end of codegen session)
    // ════════════════════════════════════════════════════════════════════════════
    internal static void LogPhase1Summary()
    {
        long total = s_totalMethodCount;
        long structured = s_structuredMethodCount;
        long exceptionBody = s_exceptionBodyCount;
        long flat = s_irreducibleCount;
        System.Console.Error.WriteLine("");
        System.Console.Error.WriteLine("╔══════════════════════════════════════════════════╗");
        System.Console.Error.WriteLine("║  Phase 1 — StructuredIR Coverage Summary        ║");
        System.Console.Error.WriteLine("╠══════════════════════════════════════════════════╣");
        System.Console.Error.WriteLine($"║  Total methods:          {total,8}                ║");
        System.Console.Error.WriteLine($"║  StructuredIR:           {structured,8} ({Pct(structured, total),6})       ║");
        System.Console.Error.WriteLine($"║  Exception-body:         {exceptionBody,8} ({Pct(exceptionBody, total),6})       ║");
        System.Console.Error.WriteLine($"║  Flat goto (total):      {flat,8} ({Pct(flat, total),6})       ║");
        System.Console.Error.WriteLine("╠══════════════════════════════════════════════════╣");
        System.Console.Error.WriteLine("║  Irreducible reasons:                            ║");
        foreach (var kvp in s_irreducibleReasons.OrderByDescending(k => k.Value))
        {
            System.Console.Error.WriteLine($"║    {kvp.Key,-36} {kvp.Value,4} ({Pct(kvp.Value, flat),6}) ║");
        }
        System.Console.Error.WriteLine("╚══════════════════════════════════════════════════╝");
        System.Console.Error.WriteLine("");
    }
    private static string Pct(long part, long total)
    {
        if (total == 0) return " 0.0%";
        return (part * 100.0 / total).ToString("F1") + "%";
    }
    internal static void LogMultipleCatchShape() =>
        System.Console.Error.WriteLine("[MultipleCatch] Detected multi-catch EH shape");
    // ════════════════════════════════════════════════════════════════════════════
    // Loop array bounds check elimination — induction variable detection
    // ════════════════════════════════════════════════════════════════════════════
    /// <summary>
    /// Scans instructions for the induction variable increment pattern
    /// ldloc X; ldc.i4 1; add; stloc X. Returns the local slot number if found.
    /// </summary>
    private static int? DetectInductionVariableSlot(IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
    {
        for (int i = 0; i < instructions.Count - 3; i++)
        {
            if (instructions[i].Op == "ldloc" &&
                instructions[i + 1].Op == "ldc.i4" &&
                instructions[i + 2].Op == "add" &&
                instructions[i + 3].Op == "stloc")
            {
                int ldlocSlot = GetRequiredIntOperand(instructions[i]);
                int constVal = GetRequiredIntOperand(instructions[i + 1]);
                int stlocSlot = GetRequiredIntOperand(instructions[i + 3]);
                if (constVal == 1 && ldlocSlot == stlocSlot)
                    return ldlocSlot;
            }
            // Also detect decrement pattern: ldloc X, ldc.i4 1, sub, stloc X
            if (instructions[i].Op == "ldloc" &&
                instructions[i + 1].Op == "ldc.i4" &&
                instructions[i + 2].Op == "sub" &&
                instructions[i + 3].Op == "stloc")
            {
                int ldlocSlot = GetRequiredIntOperand(instructions[i]);
                int constVal = GetRequiredIntOperand(instructions[i + 1]);
                int stlocSlot = GetRequiredIntOperand(instructions[i + 3]);
                if (constVal == 1 && ldlocSlot == stlocSlot)
                    return ldlocSlot;
            }
        }
        return null;
    }
    /// <summary>
    /// Scans instructions for the accumulator promotion pattern:
    /// A local slot that is read exactly once (first access) and written exactly
    /// once (last access) in the loop body. This is a loop-carried dependency
    /// (e.g. sum += array[i]) that can be promoted to a C++ local variable
    /// to eliminate chaos_locals[N] memory round-trips per iteration.
    /// Only 64-bit integer accumulators are promoted (int32 IVs are already
    /// handled by D2 IV hoisting in the do-while emitter).
    /// Returns a map: slot number → accumulator variable name (_acc_N).
    /// </summary>
    private static Dictionary<int, string>? DetectAccumulatorSlots(IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
    {
        var readCount = new Dictionary<int, int>();
        var writeCount = new Dictionary<int, int>();
        foreach (var instr in instructions)
        {
            if (instr.Op == "ldloc" && TryGetIntOperand(instr, out int ldSlot))
                readCount[ldSlot] = readCount.GetValueOrDefault(ldSlot) + 1;
            else if (instr.Op == "stloc" && TryGetIntOperand(instr, out int stSlot))
                writeCount[stSlot] = writeCount.GetValueOrDefault(stSlot) + 1;
        }
        var result = new Dictionary<int, string>();
        foreach (int slot in readCount.Keys)
        {
            // Criteria: read exactly once, written exactly once.
            // This means the slot value is loaded from chaos_locals at loop entry
            // and stored back at loop exit — a loop-carried dependency pattern
            // that can be promoted to a C++ local variable.
            if (readCount.TryGetValue(slot, out int rc) && rc == 1 &&
                writeCount.TryGetValue(slot, out int wc) && wc == 1)
            {
                result[slot] = $"_acc_{slot}";
            }
        }
        return result.Count > 0 ? result : null;
    }
    /// <summary>
    /// Tries to extract an integer operand from an IR instruction.
    /// Handles both direct int and JSON-deserialized JsonElement operands.
    /// </summary>
    private static bool TryGetIntOperand(AotCoreIrInstructionArtifact instruction, out int value)
    {
        object? operand = instruction.Operand;
        if (operand is int intVal)
        {
            value = intVal;
            return true;
        }
        if (operand is System.Text.Json.JsonElement { ValueKind: System.Text.Json.JsonValueKind.Number } je && je.TryGetInt32(out int jeVal))
        {
            value = jeVal;
            return true;
        }
        value = 0;
        return false;
    }
    /// <summary>
    /// Collects all local slots written (stloc) within an IR tree.
    /// </summary>
    private static void CollectWrittenSlots(StructuredIRNode node, HashSet<int> written)
    {
        switch (node)
        {
            case IRBlock block:
                foreach (var instr in block.BodyInstructions)
                    if (instr.Op == "stloc")
                        written.Add(GetRequiredIntOperand(instr));
                if (block.Terminator?.Op == "stloc")
                    written.Add(GetRequiredIntOperand(block.Terminator));
                break;
            case IRSequence seq:
                foreach (var n in seq.Nodes)
                    CollectWrittenSlots(n, written);
                break;
            case IRIfThenElse ite:
                CollectWrittenSlots(ite.ThenBody, written);
                if (ite.ElseBody != null)
                    CollectWrittenSlots(ite.ElseBody, written);
                break;
            case IRWhileLoop w:
                CollectWrittenSlots(w.Body, written);
                break;
            case IRDoWhileLoop dw:
                CollectWrittenSlots(dw.Body, written);
                break;
            case IRBreak: case IRContinue: case IRReturn: case IRThrow:
                break;
            case IRExceptionRegion er:
                CollectWrittenSlots(er.TryBody, written);
                CollectWrittenSlots(er.HandlerBody, written);
                break;
            case IRSwitch sw:
                foreach (var caseBody in sw.CaseBodies.Values)
                    CollectWrittenSlots(caseBody, written);
                if (sw.DefaultBody != null)
                    CollectWrittenSlots(sw.DefaultBody, written);
                break;
            case IRPcDispatch pc:
                foreach (var pcCase in pc.Cases)
                    foreach (var instr in pcCase.Instructions)
                        if (instr.Op == "stloc")
                            written.Add(GetRequiredIntOperand(instr));
                break;
        }
    }
    /// <summary>
    /// Scans a flat instruction list for array accesses (ldelem/stelem) and identifies
    /// loop-invariant chaos_locals slots used as array sources.
    /// Returns a map: slot number → base pointer variable name.
    /// </summary>
    private Dictionary<int, string> DetectInvariantArraySlots(
        List<AotCoreIrInstructionArtifact> instructions,
        HashSet<int> writtenSlots)
    {
        // Simulate eval stack: track whether each stack entry represents a chaos_locals[N] value
        // null = computed/unknown expression, string = "chaos_locals[N]" or similar
        var stack = new List<string?>();
        var result = new Dictionary<int, string>();
        foreach (var instr in instructions)
        {
            string op = instr.Op;
            // Determine pop/push counts
            int popCount = EstimatePopCount(op);
            int pushCount = EstimatePushCount(op);
            // === Before popping: check if this instruction is an array access ===
            if (op.StartsWith("ldelem", StringComparison.Ordinal) || op == "ldelem")
            {
                // ldelem: stack has [..., array, index] → array is 2nd from top
                if (stack.Count >= 2)
                {
                    string? arraySource = stack[stack.Count - 2];
                    if (arraySource != null && arraySource.StartsWith("chaos_locals[", StringComparison.Ordinal))
                    {
                        int slot = int.Parse(arraySource.AsSpan(13, arraySource.Length - 14));
                        if (!writtenSlots.Contains(slot))
                        {
                            string basePtr = $"_arr_base_{slot}";
                            result.TryAdd(slot, basePtr);
                        }
                    }
                }
            }
            else if (op.StartsWith("stelem", StringComparison.Ordinal) || op == "stelem")
            {
                // stelem: stack has [..., array, index, value] → array is 3rd from top
                if (stack.Count >= 3)
                {
                    string? arraySource = stack[stack.Count - 3];
                    if (arraySource != null && arraySource.StartsWith("chaos_locals[", StringComparison.Ordinal))
                    {
                        int slot = int.Parse(arraySource.AsSpan(13, arraySource.Length - 14));
                        if (!writtenSlots.Contains(slot))
                        {
                            string basePtr = $"_arr_base_{slot}";
                            result.TryAdd(slot, basePtr);
                        }
                    }
                }
            }
            // === Pop from stack ===
            for (int i = 0; i < popCount && stack.Count > 0; i++)
                stack.RemoveAt(stack.Count - 1);
            // === Push to stack ===
            string? pushedValue = null;
            if (op == "ldloc")
                pushedValue = $"chaos_locals[{GetRequiredIntOperand(instr)}]";
            for (int i = 0; i < pushCount; i++)
                stack.Add(pushedValue);
        }
        return result;
    }
    /// <summary>
    /// Collects all instruction artifacts within an IR tree (flattened).
    /// </summary>
    private static void CollectInstructions(StructuredIRNode node, List<AotCoreIrInstructionArtifact> instructions)
    {
        switch (node)
        {
            case IRBlock block:
                instructions.AddRange(block.BodyInstructions);
                if (block.Terminator != null)
                    instructions.Add(block.Terminator);
                break;
            case IRSequence seq:
                foreach (var n in seq.Nodes)
                    CollectInstructions(n, instructions);
                break;
            case IRIfThenElse ite:
                CollectInstructions(ite.ThenBody, instructions);
                if (ite.ElseBody != null)
                    CollectInstructions(ite.ElseBody, instructions);
                break;
            case IRWhileLoop w:
                CollectInstructions(w.Body, instructions);
                break;
            case IRDoWhileLoop dw:
                CollectInstructions(dw.Body, instructions);
                break;
            case IRBreak: case IRContinue: case IRReturn: case IRThrow:
                break;
            case IRExceptionRegion er:
                CollectInstructions(er.TryBody, instructions);
                CollectInstructions(er.HandlerBody, instructions);
                break;
            case IRSwitch sw:
                foreach (var caseBody in sw.CaseBodies.Values)
                    CollectInstructions(caseBody, instructions);
                if (sw.DefaultBody != null)
                    CollectInstructions(sw.DefaultBody, instructions);
                break;
            case IRPcDispatch pc:
                foreach (var pcCase in pc.Cases)
                    instructions.AddRange(pcCase.Instructions);
                break;
        }
    }
    /// <summary>
    /// Abstract eval stack interpreter that tracks which local slots are on
    /// the stack.  When a stelem/ldelem/ldelema instruction is found where the
    /// index operand is the induction variable slot and the array operand is
    /// a loop-invariant slot, the instruction's IlOffset is added to skipOffsets.
    /// </summary>
    private static void PreScanBlockArrayAccesses(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        int ivSlot,
        HashSet<int> writtenSlots,
        HashSet<int> skipOffsets)
    {
        // Abstract stack: each entry is a local slot number if the value
        // came directly from ldloc, or null if the value is derived/unknown.
        var stack = new List<int?>(instructions.Count);
        foreach (var instr in instructions)
        {
            switch (instr.Op)
            {
                case "ldloc":
                    stack.Add(GetRequiredIntOperand(instr));
                    break;
                case "ldc.i4": case "ldc.i8": case "ldc.r4": case "ldc.r8":
                case "ldarg": case "ldnull": case "ldstr": case "ldtoken":
                case "ldarga": case "ldloca": case "ldsflda":
                case "ldflda":
                case "newarr": case "sizeof":
                case "dup": case "arglist":
                case "ldftn": case "ldvirtftn":
                    stack.Add(null);
                    break;
                case "stloc":
                    if (stack.Count > 0) stack.RemoveAt(stack.Count - 1);
                    break;
                case "starg":
                    if (stack.Count > 0) stack.RemoveAt(stack.Count - 1);
                    break;
                case "stelem": case "stelem.i": case "stelem.ref":
                    if (stack.Count >= 3)
                    {
                        // Pop: value (bottom), index (middle), array (top of pops)
                        stack.RemoveAt(stack.Count - 1); // value
                        int? idxSlot = stack[^1];
                        int? arrSlot = stack[^2];
                        stack.RemoveRange(stack.Count - 2, 2); // index + array
                        if (idxSlot == ivSlot && arrSlot.HasValue && !writtenSlots.Contains(arrSlot.Value))
                            skipOffsets.Add(instr.IlOffset);
                    }
                    break;
                case "ldelem": case "ldelem.i": case "ldelem.ref": case "ldelema":
                    if (stack.Count >= 2)
                    {
                        int? idxSlot = stack[^1];
                        int? arrSlot = stack[^2];
                        stack.RemoveRange(stack.Count - 2, 2);
                        stack.Add(null); // result value
                        if (idxSlot == ivSlot && arrSlot.HasValue && !writtenSlots.Contains(arrSlot.Value))
                            skipOffsets.Add(instr.IlOffset);
                    }
                    break;
                // Binary: pop 2, push null (derived)
                case "add": case "sub": case "mul": case "div": case "div.un":
                case "rem": case "rem.un":
                case "and": case "or": case "xor":
                case "shl": case "shr": case "shr.un":
                case "ceq": case "cgt": case "clt": case "cgt.un":
                case "add.ovf": case "sub.ovf": case "mul.ovf":
                case "add.ovf.un": case "sub.ovf.un": case "mul.ovf.un":
                    if (stack.Count >= 2)
                    {
                        stack.RemoveRange(stack.Count - 2, 2);
                        stack.Add(null);
                    }
                    break;
                // Unary: pop 1, push null (derived)
                case "neg": case "not":
                case "conv.i1": case "conv.i2": case "conv.i4": case "conv.i8":
                case "conv.u1": case "conv.u2": case "conv.u4": case "conv.u8":
                case "conv.u": case "conv.r4": case "conv.r8": case "conv.r.un":
                case "conv.ovf.i1": case "conv.ovf.u1": case "conv.ovf.i2":
                case "conv.ovf.u2": case "conv.ovf.i4": case "conv.ovf.u4":
                case "conv.ovf.i8": case "conv.ovf.u8":
                case "conv.ovf.i": case "conv.ovf.u":
                case "ckfinite":
                case "ldind.i1": case "ldind.u1": case "ldind.i2": case "ldind.u2":
                case "ldind.i4": case "ldind.u4": case "ldind.i8":
                case "ldind.ref": case "ldind.r4": case "ldind.r8": case "ldind.i":
                case "ldobj": case "ldlen": case "localloc":
                case "box": case "unbox": case "unbox.any":
                case "castclass": case "isinst":
                case "ldfld":
                    if (stack.Count >= 1)
                    {
                        stack.RemoveAt(stack.Count - 1);
                        stack.Add(null);
                    }
                    break;
                // Pop 1 only
                case "pop": case "stsfld": case "stfld":
                case "initobj": case "throw":
                case "brtrue": case "brfalse":
                    if (stack.Count >= 1) stack.RemoveAt(stack.Count - 1);
                    break;
                // Pop 2 only
                case "beq": case "bne.un": case "bge": case "bgt":
                case "ble": case "blt":
                case "bge.un": case "bgt.un": case "ble.un": case "blt.un":
                case "stobj":
                case "stind.i4": case "stind.i1": case "stind.i2":
                case "stind.i8": case "stind.r4": case "stind.r8":
                case "stind.ref": case "stind.i":
                    if (stack.Count >= 2) stack.RemoveRange(stack.Count - 2, 2);
                    break;
                // Pop 3 (cpblk)
                case "cpblk":
                    if (stack.Count >= 3) stack.RemoveRange(stack.Count - 3, 3);
                    break;
                // Call: pop N args, push 0/1 result
                case "call": case "callvirt": case "calli":
                    int? paramCount = instr.TargetParameterCount;
                    if (paramCount.HasValue && stack.Count >= paramCount.Value)
                        stack.RemoveRange(stack.Count - paramCount.Value, paramCount.Value);
                    string? retType = instr.TargetReturnType;
                    if (!string.IsNullOrEmpty(retType) && retType != "System.Void")
                        stack.Add(null);
                    break;
                case "newobj":
                    int? ctorParamCount = instr.TargetParameterCount;
                    if (ctorParamCount.HasValue && stack.Count >= ctorParamCount.Value)
                        stack.RemoveRange(stack.Count - ctorParamCount.Value, ctorParamCount.Value);
                    stack.Add(null);
                    break;
                // ret: reset stack
                case "ret":
                    stack.Clear();
                    break;
                // br/leave: non-fallthrough, reset
                case "br": case "leave":
                    stack.Clear();
                    break;
                case "jmp":
                    break;
                case "switch":
                    if (stack.Count >= 1) stack.RemoveAt(stack.Count - 1);
                    break;
                case "mkrefany":
                    if (stack.Count >= 1) { stack.RemoveAt(stack.Count - 1); stack.Add(null); stack.Add(null); }
                    break;
                case "refanyval": case "refanytype":
                    if (stack.Count >= 2) { stack.RemoveRange(stack.Count - 2, 2); stack.Add(null); }
                    break;
            }
        }
    }
    /// <summary>
    /// Walk an IR tree and collect all array access offsets that can skip
    /// bounds checks because they use ivSlot as the index with a loop-invariant
    /// array (not in writtenSlots).
    /// </summary>
    private HashSet<int>? PreScanLoopArraySkips(StructuredIRNode body, int ivSlot, HashSet<int> writtenSlots)
    {
        var skipOffsets = new HashSet<int>();
        PreScanNodeArrayAccesses(body, ivSlot, writtenSlots, skipOffsets);
        return skipOffsets.Count > 0 ? skipOffsets : null;
    }
    private static void PreScanNodeArrayAccesses(
        StructuredIRNode node,
        int ivSlot,
        HashSet<int> writtenSlots,
        HashSet<int> skipOffsets)
    {
        switch (node)
        {
            case IRBlock block:
                PreScanBlockArrayAccesses(block.BodyInstructions, ivSlot, writtenSlots, skipOffsets);
                if (block.Terminator != null)
                    PreScanBlockArrayAccesses(new[] { block.Terminator }, ivSlot, writtenSlots, skipOffsets);
                break;
            case IRSequence seq:
                foreach (var n in seq.Nodes)
                    PreScanNodeArrayAccesses(n, ivSlot, writtenSlots, skipOffsets);
                break;
            case IRIfThenElse ite:
                PreScanNodeArrayAccesses(ite.ThenBody, ivSlot, writtenSlots, skipOffsets);
                if (ite.ElseBody != null)
                    PreScanNodeArrayAccesses(ite.ElseBody, ivSlot, writtenSlots, skipOffsets);
                break;
            case IRWhileLoop w:
                PreScanNodeArrayAccesses(w.Body, ivSlot, writtenSlots, skipOffsets);
                break;
            case IRDoWhileLoop dw:
                PreScanNodeArrayAccesses(dw.Body, ivSlot, writtenSlots, skipOffsets);
                break;
            case IRBreak: case IRContinue: case IRReturn: case IRThrow:
                break;
            case IRExceptionRegion er:
                PreScanNodeArrayAccesses(er.TryBody, ivSlot, writtenSlots, skipOffsets);
                PreScanNodeArrayAccesses(er.HandlerBody, ivSlot, writtenSlots, skipOffsets);
                break;
            case IRSwitch sw:
                foreach (var caseBody in sw.CaseBodies.Values)
                    PreScanNodeArrayAccesses(caseBody, ivSlot, writtenSlots, skipOffsets);
                if (sw.DefaultBody != null)
                    PreScanNodeArrayAccesses(sw.DefaultBody, ivSlot, writtenSlots, skipOffsets);
                break;
            case IRPcDispatch pc:
                foreach (var pcCase in pc.Cases)
                    PreScanBlockArrayAccesses(pcCase.Instructions, ivSlot, writtenSlots, skipOffsets);
                break;
        }
    }
}
