using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{

    private static IReadOnlyList<AotCoreIrInstructionArtifact> GetInstructionsInRange(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        int offset, int length)
    {
        int end = offset + length;
        var result = new List<AotCoreIrInstructionArtifact>();
        foreach (var instr in instructions)
        {
            if (instr.IlOffset >= offset && instr.IlOffset < end)
                result.Add(instr);
        }
        return result;
    }



    private static IReadOnlyList<AotCoreIrInstructionArtifact> GetInstructionsAfter(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        int offset)
    {
        var result = new List<AotCoreIrInstructionArtifact>();
        foreach (var instr in instructions)
        {
            if (instr.IlOffset >= offset)
                result.Add(instr);
        }
        return result;
    }



    private static void StripTrailingLeaveInstructions(List<AotCoreIrInstructionArtifact> instructions)
    {
        while (instructions.Count > 0 &&
               string.Equals(instructions[^1].Op, "leave", StringComparison.Ordinal))
        {
            instructions.RemoveAt(instructions.Count - 1);
        }
    }



    /// <summary>
    /// Validates that a partition's instruction sequence would not underflow the eval stack
    /// during structured IR emission. Returns false if any instruction would pop from an
    /// empty stack (signaling that the partition is not self-contained for structured IR).
    /// </summary>
    /// <param name="instructions">The instruction partition to validate.</param>
    /// <param name="initialDepth">Starting stack depth. Use 0 for most partitions; use 1
    /// for catch/filter handler bodies (runtime pushes the exception object).</param>
    private static bool ValidatePartitionStackBalance(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        int initialDepth)
    {
        int depth = initialDepth;
        foreach (var instr in instructions)
        {
            string op = instr.Op;
            int pops = GetStackPopCount(instr);
            if (depth < pops)
                return false;
            depth -= pops;
            depth += GetStackPushCount(instr);
        }
        return depth >= 0;
    }



    /// <summary>
    /// Returns the number of values an instruction pops from the eval stack.
    /// </summary>
    private static int GetStackPopCount(AotCoreIrInstructionArtifact instr)
    {
        string op = instr.Op;
        switch (op)
        {
            // Pure pops (pop 1)
            case "pop":
            case "stloc":
            case "starg":
            case "initobj":
            case "stsfld":
            case "throw":
            case "brfalse":
            case "brtrue":
            case "endfilter":
                return 1;

            // Pop 2 (instance stores, conditional branches)
            case "stfld":
            case "stobj":
            case "stind.i4":
            case "stind.i1":
            case "stind.i2":
            case "stind.i8":
            case "stind.r4":
            case "stind.r8":
            case "stind.ref":
            case "stind.i":
            case "beq":
            case "bgt":
            case "blt":
            case "bge":
            case "ble":
            case "bne.un":
            case "bge.un":
                return 2;

            // Pop 3
            case "stelem":
            case "stelem.i":
            case "stelem.ref":
            case "stelem.i1":
            case "stelem.i2":
            case "stelem.i4":
            case "stelem.i8":
            case "stelem.r4":
            case "stelem.r8":
            case "cpblk":
                return 3;

            // Pop 2, push 1 (net -1)
            case "cgt.un":
            case "ceq":
            case "cgt":
            case "clt":
            case "add":
            case "sub":
            case "mul":
            case "div":
            case "div.un":
            case "rem":
            case "rem.un":
            case "shl":
            case "shr":
            case "shr.un":
            case "and":
            case "or":
            case "xor":
            case "add.ovf":
            case "sub.ovf":
            case "mul.ovf":
            case "add.ovf.un":
            case "sub.ovf.un":
            case "mul.ovf.un":
            case "ldelem":
            case "ldelem.i":
            case "ldelem.ref":
            case "ldelema":
                return 2;

            // Pop 1, push 1 (net 0)
            case "ldfld":
            case "ldflda":
            case "ldind.i4":
            case "ldind.u1":
            case "ldind.i1":
            case "ldind.u2":
            case "ldind.i2":
            case "ldind.u4":
            case "ldind.i8":
            case "ldind.r4":
            case "ldind.r8":
            case "ldind.ref":
            case "ldind.i":
            case "box":
            case "unbox":
            case "unbox.any":
            case "castclass":
            case "isinst":
            case "ldobj":
            case "ldlen":
            case "localloc":
            case "conv.i4":
            case "conv.i1":
            case "conv.i2":
            case "conv.i8":
            case "conv.u8":
            case "conv.r4":
            case "conv.r8":
            case "conv.u":
            case "conv.u1":
            case "conv.u2":
            case "conv.u4":
            case "conv.r.un":
            case "ckfinite":
            case "conv.ovf.i1":
            case "conv.ovf.u1":
            case "conv.ovf.i2":
            case "conv.ovf.u2":
            case "conv.ovf.i4":
            case "conv.ovf.u4":
            case "conv.ovf.i8":
            case "conv.ovf.u8":
            case "conv.ovf.i":
            case "conv.ovf.u":
            case "conv.ovf.i8.un":
            case "conv.ovf.u8.un":
            case "conv.ovf.i.un":
            case "conv.ovf.u.un":
            case "conv.ovf.i1.un":
            case "conv.ovf.i2.un":
            case "conv.ovf.i4.un":
            case "conv.ovf.u1.un":
            case "conv.ovf.u2.un":
            case "conv.ovf.u4.un":
            case "not":
            case "neg":
            case "mkrefany":
            case "ldvirtftn":
            case "refanyval":
            case "refanytype":
                return 1;

            // Pop for call and newobj: depends on parameter count
            case "call":
            case "callvirt":
            case "calli":
            case "newobj":
                {
                    int? paramCount = instr.TargetParameterCount;
                    if (paramCount.HasValue)
                    {
                        // newobj: `this` is not on the IL stack
                        return op == "newobj" ? paramCount.Value : paramCount.Value;
                    }
                    string? callee = instr.Callee ?? instr.TargetReference?.SubjectId;
                    if (!string.IsNullOrEmpty(callee))
                    {
                        int inferred = InferParameterCountFromSubjectId(callee);
                        return inferred;
                    }
                    return 0;
                }

            // jmp: special — forwards arguments directly
            case "jmp":
                return 0;

            // ret, br, leave, switch, endfinally: pop count handled below
            case "ret":
                return 0; // ret's pop is return-value dependent, validated separately

            case "switch":
                return 1;

            case "br":
            case "leave":
            case "endfinally":
                return 0;

            // Unknown opcode: conservative, assume pops 0
            default:
                return 0;
        }
    }



    /// <summary>
    /// Returns the number of values an instruction pushes onto the eval stack.
    /// </summary>
    private static int GetStackPushCount(AotCoreIrInstructionArtifact instr)
    {
        string op = instr.Op;
        switch (op)
        {
            // Pure pushes (+1)
            case "ldc.i4":
            case "ldc.i8":
            case "ldc.r4":
            case "ldc.r8":
            case "ldarg":
            case "ldstr":
            case "ldtoken":
            case "ldarga":
            case "ldnull":
            case "ldloc":
            case "ldloca":
            case "ldsfld":
            case "ldsflda":
            case "ldftn":
            case "newarr":
            case "sizeof":
            case "dup":
            case "arglist":
                return 1;

            // Pop 2, push 1 (= net -1)
            case "cgt.un":
            case "ceq":
            case "cgt":
            case "clt":
            case "add":
            case "sub":
            case "mul":
            case "div":
            case "div.un":
            case "rem":
            case "rem.un":
            case "shl":
            case "shr":
            case "shr.un":
            case "and":
            case "or":
            case "xor":
            case "add.ovf":
            case "sub.ovf":
            case "mul.ovf":
            case "add.ovf.un":
            case "sub.ovf.un":
            case "mul.ovf.un":
            case "ldelem":
            case "ldelem.i":
            case "ldelem.ref":
            case "ldelema":
                return 1;

            // Pop 1, push 1 (= net 0)
            case "ldfld":
            case "ldflda":
            case "ldind.i4":
            case "ldind.u1":
            case "ldind.i1":
            case "ldind.u2":
            case "ldind.i2":
            case "ldind.u4":
            case "ldind.i8":
            case "ldind.r4":
            case "ldind.r8":
            case "ldind.ref":
            case "ldind.i":
            case "box":
            case "unbox":
            case "unbox.any":
            case "castclass":
            case "isinst":
            case "ldobj":
            case "ldlen":
            case "localloc":
            case "conv.i4":
            case "conv.i1":
            case "conv.i2":
            case "conv.i8":
            case "conv.u8":
            case "conv.r4":
            case "conv.r8":
            case "conv.u":
            case "conv.u1":
            case "conv.u2":
            case "conv.u4":
            case "conv.r.un":
            case "ckfinite":
            case "conv.ovf.i1":
            case "conv.ovf.u1":
            case "conv.ovf.i2":
            case "conv.ovf.u2":
            case "conv.ovf.i4":
            case "conv.ovf.u4":
            case "conv.ovf.i8":
            case "conv.ovf.u8":
            case "conv.ovf.i":
            case "conv.ovf.u":
            case "conv.ovf.i8.un":
            case "conv.ovf.u8.un":
            case "conv.ovf.i.un":
            case "conv.ovf.u.un":
            case "conv.ovf.i1.un":
            case "conv.ovf.i2.un":
            case "conv.ovf.i4.un":
            case "conv.ovf.u1.un":
            case "conv.ovf.u2.un":
            case "conv.ovf.u4.un":
            case "not":
            case "neg":
            case "mkrefany":
            case "ldvirtftn":
            case "refanyval":
            case "refanytype":
                return 1;

            // Pop 2, push 2 (= net 0, box + typeHandle)
            // Actually this is pop 1 push 2 for mkrefany, refanyval is pop 2 push 1
            // Already handled above.

            // Pure pops (push 0)
            case "pop":
            case "stloc":
            case "starg":
            case "initobj":
            case "stsfld":
            case "throw":
            case "brfalse":
            case "brtrue":
            case "stfld":
            case "stobj":
            case "stind.i4":
            case "stind.i1":
            case "stind.i2":
            case "stind.i8":
            case "stind.r4":
            case "stind.r8":
            case "stind.ref":
            case "stind.i":
            case "beq":
            case "bgt":
            case "blt":
            case "bge":
            case "ble":
            case "bne.un":
            case "bge.un":
            case "stelem":
            case "stelem.i":
            case "stelem.ref":
            case "stelem.i1":
            case "stelem.i2":
            case "stelem.i4":
            case "stelem.i8":
            case "stelem.r4":
            case "stelem.r8":
            case "cpblk":
            case "endfilter":
            case "jmp":
                return 0;

            // ret, br, leave, switch, endfinally
            case "ret":
                return 0;
            case "switch":
                return 0;
            case "br":
            case "leave":
            case "endfinally":
                return 0;

            // call/callvirt/calli/newobj: push 0 or 1 depending on return type
            case "call":
            case "callvirt":
            case "calli":
            case "newobj":
                {
                    string? callee = instr.Callee ?? instr.TargetReference?.SubjectId;
                    string? retType = instr.TargetReturnType;
                    if (string.IsNullOrEmpty(retType) && !string.IsNullOrEmpty(callee))
                        retType = InferReturnTypeFromSubjectId(callee);
                    if (!string.IsNullOrEmpty(retType) && retType != "System.Void")
                        return 1;
                    return op == "newobj" ? 1 : 0;
                }

            default:
                // Unknown opcode: conservative, assume pushes 1
                return 1;
        }
    }



    // ── TryCreateFinallyHandlerEmissionPlan (kept from original) ──

    private static bool TryCreateFinallyHandlerEmissionPlan(
        FinallyHandlerShape handlerShape,
        out FinallyHandlerEmissionPlan? emissionPlan)
    {
        emissionPlan = null;
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions = handlerShape.Instructions;
        if (instructions.Count == 0) return false;

        if (!string.Equals(instructions[instructions.Count - 1].Op, "endfinally", StringComparison.Ordinal))
            return false;

        var body = instructions.Take(instructions.Count - 1).ToArray();
        if (body.Length == 0)
        {
            emissionPlan = new FinallyHandlerEmissionPlan(null, Array.Empty<AotCoreIrInstructionArtifact>());
            return true;
        }

        // Look for a single brtrue/brfalse guard at the end of the body
        var guards = body
            .Select((instruction, index) => (instruction, index))
            .Where(entry => string.Equals(entry.instruction.Op, "brtrue", StringComparison.Ordinal) ||
                            string.Equals(entry.instruction.Op, "brfalse", StringComparison.Ordinal))
            .ToArray();

        if (guards.Length == 0)
        {
            emissionPlan = new FinallyHandlerEmissionPlan(null, body);
            return true;
        }

        if (guards.Length != 1)
            return false;

        var (guardInst, guardIdx) = guards[0];
        int guardTarget = GetRequiredIntOperand(guardInst);
        int endFinallyOffset = GetRequiredIlOffset(instructions[instructions.Count - 1]);
        bool guardTargetsEndFinally = guardTarget == endFinallyOffset;

        if (!guardTargetsEndFinally)
        {
            // Accept forward-branch guards (brtrue/brfalse targeting an offset within the
            // handler body rather than endfinally). This handles the common lock-expansion
            // pattern: finally { if (obj != null) Monitor.Exit(obj); }
            //   → ldloc obj; brfalse.s skip; ldloc obj; call Monitor.Exit; skip: endfinally
            // Verify that the guard targets a forward offset AFTER the guard instruction.
            int guardInstrOffset = GetRequiredIlOffset(instructions[guardIdx]);
            if (guardTarget <= guardInstrOffset || guardTarget >= endFinallyOffset)
                return false;
        }

        var condInstructions = body.Take(guardIdx).ToArray();
        var bodyInstructions = body.Skip(guardIdx + 1).ToArray();
        if (condInstructions.Length == 0)
            return false;

        emissionPlan = new FinallyHandlerEmissionPlan(
            new FinallyHandlerGuardShape(condInstructions, string.Equals(guardInst.Op, "brtrue", StringComparison.Ordinal)),
            bodyInstructions,
            guardTargetsEndFinally);
        return true;
    }

}
