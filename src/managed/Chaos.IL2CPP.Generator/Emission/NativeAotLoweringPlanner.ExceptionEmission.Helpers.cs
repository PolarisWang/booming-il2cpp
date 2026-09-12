using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.CompilerServices;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

// _state is ThreadLocal<PerMethodState> with a factory initializer (see ExceptionEmission.cs L21),
// so it is never null at runtime.  The nullable analyzer cannot prove this across partial class
// files, so suppress CS8602 (Dereference of possibly null reference) for _state.Value! accesses.
#pragma warning disable CS8602

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{

    private void ResetArrayCheckCache()
    {
        _state.Value!.LastCheckedArrayExpr = null;
        _state.Value!.LastCheckedIndexExpr = null;
    }



    private bool TrySkipArrayChecks(string arrayExpr, string indexExpr)
    {
        if (_state.Value!.ActiveStructuredSlotContext is null)
            return false;
        if (arrayExpr == _state.Value!.LastCheckedArrayExpr && indexExpr == _state.Value!.LastCheckedIndexExpr)
            return true;
        _state.Value!.LastCheckedArrayExpr = arrayExpr;
        _state.Value!.LastCheckedIndexExpr = indexExpr;
        return false;
    }



    private static HashSet<int> IdentifyStructLocalSlots(IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
    {
        var structLocals = new HashSet<int>();
        for (int i = 1; i < instructions.Count; i++)
        {
            if (!string.Equals(instructions[i].Op, "initobj", StringComparison.Ordinal))
                continue;
            // initobj only operates on value types per IL spec, so the
            // targetRef typeShape check is redundant. Explicitly omitted
            // because generic value types (e.g. Vector128<T>) may carry
            // an incorrect typeShape (ReferenceType=1) in the AOT core IR.
            if (!string.Equals(instructions[i - 1].Op, "ldloca", StringComparison.Ordinal))
                continue;
            structLocals.Add(GetRequiredIntOperand(instructions[i - 1]));
        }
        return structLocals;
    }

    /// <summary>
    /// Pre-scan to identify local slots that hold a GC-heap pointer to a boxed async
    /// state machine (&gt;d__).  These slots contain a durable GC heap address.  The
    /// codegen's <c>ldloca</c> for such slots must emit the <b>value</b> of the slot
    /// (which already IS the address of the struct) rather than <c>&amp;chaos_locals[N]</c>
    /// (the address of the C++ stack slot holding the pointer), because the pointer
    /// must survive cross-thread continuation resumption.
    /// <para/>
    /// Three detection patterns:
    /// <list type="bullet">
    ///   <item><b>Entry methods, boxing form</b>: a <c>newobj</c> on a type whose
    ///   SubjectId contains <c>&gt;d__</c>, whose result is <c>stloc</c>'d.  Roslyn emits
    ///   this when the state machine escapes the method (e.g. it is captured).</item>
    ///   <item><b>Entry methods, non-boxing form</b>: no <c>newobj</c> at all — the
    ///   compiler declares the state machine as a plain <c>valuetype</c> local and
    ///   passes <c>ldloca V</c> straight to
    ///   <c>AsyncTaskMethodBuilder::Start&lt;T&gt;(ref T)</c>.  This is the common
    ///   Release shape.  The slot still must be a durable GC box: <c>Start</c>
    ///   drives the first MoveNext, which may suspend (Task.Yield) and register the
    ///   address with the thread pool as a resume target.  Treating it as a stack
    ///   slot would hand the pool a pointer that dies with the entry frame.</item>
    ///   <item><b>MoveNext methods</b> (the state machine proper): an <c>ldarg.0 + stloc</c>
    ///   pair at the method entry — the <c>stloc</c> slot receives the <c>this</c> pointer
    ///   (the box address) and propagates it to <c>ldfld/stfld</c> call sites.</item>
    /// </list>
    /// </summary>
    private static HashSet<int> IdentifyAsyncBoxPointerLocalSlots(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        AotCoreIrMethodArtifact method)
    {
        var result = new HashSet<int>();
        if (instructions.Count == 0) return result;

        // Pattern A: async ENTRY — newobj on >d__ type, then stloc.
        bool isEntry = false;
        for (int i = 0; i < instructions.Count; i++)
        {
            if (instructions[i].Op != "newobj") continue;
            if (instructions[i].TargetReference is not { SubjectId: { } sid }) continue;
            if (!sid.Contains(">d__", StringComparison.Ordinal)) continue;
            isEntry = true;
            // Find the stloc that consumes the newobj result.
            int boxSlot = -1;
            int boxStlocIndex = -1;
            for (int j = i + 1; j < instructions.Count; j++)
            {
                if (instructions[j].Op is "stloc" or "stloc.s")
                {
                    boxSlot = GetRequiredIntOperand(instructions[j]);
                    boxStlocIndex = j;
                    break;
                }
                if (instructions[j].Op is "dup" or "call" or "callvirt" or "nop") continue;
                break; // Unknown intervening instruction — stop.
            }
            // Not every `newobj` on a `>d__` type is an *escaping entry* box. Roslyn also
            // emits this shape in the state machine's own factory methods — most notably
            // `IAsyncEnumerable<T>::GetAsyncEnumerator`, whose whole body is
            // `newobj <T>d__0::.ctor(int32); stloc.0; ldloc.0; ret`. There the box is the
            // RETURN VALUE: freshly constructed, never passed by-ref to a builder, and it
            // must be emitted as an ordinary local slot (&chaos_locals[N]) or the caller
            // receives a dangling address.
            //
            // The discriminator is consumption, not construction: an entry box is handed to
            // an async builder (Start / AwaitUnsafeOnCompleted / SetResult ...) or is
            // `this`-propagated in MoveNext. A factory box is merely returned. Follow the
            // value forward from the stloc through ldloca/ldloc/ldfld until it is either
            // given to a builder call or consumed some other way.
            if (boxSlot >= 0 && IsConsumedByAsyncBuilderCall(instructions, boxStlocIndex))
                result.Add(boxSlot);
            break; // only one async state machine per method
        }

        // Pattern A': async ENTRY, non-boxing form.  Roslyn's common Release shape
        // declares the state machine as a plain valuetype local and never emits a
        // newobj — the entry is then just a chain of `ldloca V` feeding
        // AsyncTaskMethodBuilder::Start<T>(ref T) / AwaitUnsafeOnCompleted<T>(..., ref T).
        // Pattern A above only matches the boxing form, so without this branch such an
        // entry falls through entirely and `V` is emitted as a raw `&chaos_locals[V]`
        // stack slot.  Start() drives MoveNext synchronously and a Task.Yield inside it
        // parks that address on the thread pool as the resume target — so the stack slot
        // becomes a dangling pointer the moment the entry returns.  Any local that is
        // ldloca'd into a generic async-builder Start/AwaitUnsafeOnCompleted call is the
        // state-machine slot; treat it as the durable GC box.
        if (!isEntry)
        {
            // Which ldloca is the state machine? For a call, the generic-argument list in the
            // postfix names the FIRST by-ref position as TAwaiter and the LAST as TStateMachine:
            //   AsyncTaskMethodBuilder::AwaitUnsafeOnCompleted<TAwaiter, TStateMachine>(!!0&, !!1&)
            //   AsyncTaskMethodBuilder::Start<TStateMachine>(!!0&)
            // Argument pushes are left-to-right, so the state machine is the LAST `ldloca` pushed
            // before the call — not the first.  Recording the first would capture the AWAITER's
            // slot (a YieldAwaiter is not a GC-tracked state machine; a value/address flip there
            // corrupts the emitted code).
            int pendingSlot = -1;
            for (int i = 0; i < instructions.Count; i++)
            {
                var op = instructions[i].Op;
                if (op == "ldloca")
                {
                    pendingSlot = GetRequiredIntOperand(instructions[i]);
                    continue;
                }
                if (op is "call" or "callvirt")
                {
                    var callee = instructions[i].Callee;
                    if (pendingSlot >= 0 && callee is not null
                        && (callee.Contains("AsyncTaskMethodBuilder", StringComparison.Ordinal)
                            || callee.Contains("AsyncValueTaskMethodBuilder", StringComparison.Ordinal)
                            || callee.Contains("AsyncVoidMethodBuilder", StringComparison.Ordinal))
                        && (callee.Contains("::Start", StringComparison.Ordinal)
                            || callee.Contains("AwaitUnsafeOnCompleted", StringComparison.Ordinal)))
                    {
                        isEntry = true;
                        result.Add(pendingSlot);
                    }
                    pendingSlot = -1;
                    continue;
                }
                // Any op that itself pushes a value establishes a NEW top-of-stack, so an
                // earlier ldloca is no longer the last argument pushed.  `ldfld`/`unbox`/
                // `isinst` derivations deliberately keep the slot alive: the builder may
                // legitimately receive an address derived from the state-machine local.
                if (PushesAValue(op)) pendingSlot = -1;
            }
        }

        // Pattern B: async MoveNext — `this` (ldarg.0) copied into a local via an adjacent
        // stloc, to be later ldloca'd and passed as `ref state_machine` to
        // builder.AwaitUnsafeOnCompleted / builder.Start.  Those locals must be treated as
        // the durable box pointer.  Scan the WHOLE method (the copy may occur mid-body, not
        // just at index 0), matching any `ldarg.0` immediately followed by `stloc V`.
        if (!isEntry && method.SubjectId is not null && IsAsyncStateMachineMoveNext(method.SubjectId))
        {
            for (int i = 0; i < instructions.Count - 1; i++)
            {
                if (instructions[i].Op != "ldarg" || GetRequiredIntOperand(instructions[i]) != 0)
                    continue;
                if (instructions[i + 1].Op is "stloc" or "stloc.s")
                    result.Add(GetRequiredIntOperand(instructions[i + 1]));
            }
        }

        // ── Self-validation (DEBUG only) ──────────────────────────────────────
        // For each identified slot, verify it is consumed as a by-ref async builder
        // argument (ldloca → call/callvirt to builder.Start/AwaitUnsafeOnCompleted).
        // This catches false positives where the heuristic marks a non-async-box slot:
        // mis-emission would cause silent data corruption (value/address flip).
#if DEBUG
        if (result.Count > 0)
        {
            // Build a map: slot → set of subsequent call/callvirt callees that ldloca it.
            var slotConsumers = new Dictionary<int, HashSet<string>>();
            for (int i = 0; i < instructions.Count; i++)
            {
                if (instructions[i].Op != "ldloca") continue;
                int slot = GetRequiredIntOperand(instructions[i]);
                if (!result.Contains(slot)) continue;
                // Scan forward from this ldloca to find the nearest call/callvirt
                // that consumes the pushed address.  ldloca pushes a single value;
                // any intervening push or pop would break the contract.
                for (int j = i + 1; j < instructions.Count; j++)
                {
                    var op = instructions[j].Op;
                    if (op is "call" or "callvirt")
                    {
                        if (!string.IsNullOrEmpty(instructions[j].Callee))
                        {
                            if (!slotConsumers.ContainsKey(slot))
                                slotConsumers[slot] = new HashSet<string>();
                            slotConsumers[slot].Add(instructions[j].Callee!);
                        }
                        break; // ldloca's single value is consumed by this call
                    }
                    // Ops that consume one eval-stack value and produce one:
                    // still forwarding the same address reference.
                    if (op is "ldfld" or "ldflda" or "unbox" or "unbox.any"
                        or "isinst" or "castclass") continue;
                    // Ops that consume without pushing a reference (dup is a
                    // copy so we need to track both forks — but tracking every
                    // dup fork is unbounded; flag it as a potential loss).
                    if (op is "dup") continue; // conservatively not a loss
                    // Any other op consumed the value: stop scanning.
                    break;
                }
            }
            // Every identified slot must be consumed by an async builder call.
            foreach (int slot in result)
            {
                bool hasBuilderRef = slotConsumers.TryGetValue(slot, out var callees)
                    && callees!.Any(c => c.Contains("AsyncTaskMethodBuilder")
                        || c.Contains("AsyncValueTaskMethodBuilder")
                        || c.Contains("AsyncVoidMethodBuilder")
                        || c.Contains("AsyncPromise")
                        || c.Contains("Start")
                        || c.Contains("AwaitUnsafeOnCompleted"));
                if (!hasBuilderRef)
                {
                    // A slot identified as a durable async box pointer must actually be
                    // ldloca/ldloc-forwarded into an async builder call.  If not, the
                    // value/address flip in EmitInstruction's ldloca branch would emit a
                    // corrupt pointer with no other compile-time defence — fail LOUDLY here
                    // (development build) instead of silently emitting wrong C++.
                    throw new InvalidOperationException(
                        "IdentifyAsyncBoxPointerLocalSlots: slot " + slot
                        + " was identified as a durable async state-machine box pointer but is "
                        + "not consumed by an async builder call (Start/AwaitUnsafeOnCompleted). "
                        + "Consumer callees: [" + string.Join(", ", slotConsumers.TryGetValue(slot, out var cs) ? cs : new HashSet<string>()) + "]. "
                        + "A value/address flip at ldloca would corrupt the emitted code.");
                }
            }
        }
#endif
        return result;
    }

    /// <summary>
    /// True when <paramref name="valueIndex"/> (a local write) is followed, before the value is
    /// consumed or the method returns, by an async builder call. This distinguishes the two
    /// `newobj` shapes Roslyn emits for a `>d__` type:
    /// <list type="bullet">
    ///   <item><b>Escaping entry box</b> — the box is handed to a builder (directly, or via a
    ///   MoveNext call that reads it from the local). Durable heap address; emit by value.</item>
    ///   <item><b>Factory construction</b> — <c>GetAsyncEnumerator</c> builds the enumerator
    ///   only to return it (<c>newobj; stloc.0; ldloc.0; ret</c>). The box belongs to the
    ///   caller; emit it as an ordinary local slot.</item>
    /// </list>
    /// </summary>
    private static bool IsConsumedByAsyncBuilderCall(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions, int valueIndex)
    {
        for (int j = valueIndex + 1; j < instructions.Count; j++)
        {
            var op = instructions[j].Op;
            // The value is consumed / replaced without reaching a builder.
            if (op is "stloc" or "stloc.s" or "stfld" or "stind.i4" or "stind.ref"
                or "stobj" or "ret" or "throw" or "endfinally" or "endfilter")
                return false;
            if (op is "call" or "callvirt")
            {
                var callee = instructions[j].Callee;
                if (IsAsyncBuilderCallSpelling(callee)) return true;
            }
            // `ldloc` of the constructed local re-pushes it; keep scanning.
            if (op is "ldloc" or "ldloc.s" or "ldfld" or "castclass" or "isinst" or "nop")
                continue;
        }
        return false;
    }

    /// <summary>
    /// True when the callee spelling names an async builder entry point that takes a state
    /// machine (by reference, or as a MoveNext receiver).
    /// </summary>
    private static bool IsAsyncBuilderCallSpelling(string? callee)
    {
        if (string.IsNullOrEmpty(callee)) return false;
        return callee.Contains("AsyncTaskMethodBuilder", StringComparison.Ordinal)
            || callee.Contains("AsyncValueTaskMethodBuilder", StringComparison.Ordinal)
            || callee.Contains("AsyncVoidMethodBuilder", StringComparison.Ordinal)
            || callee.Contains("AsyncIteratorMethodBuilder", StringComparison.Ordinal)
            || callee.Contains("ManualResetValueTaskSourceCore", StringComparison.Ordinal);
    }

    /// <summary>
    /// True when <paramref name="op"/> pushes a value, which supersedes any earlier
    /// <c>ldloca</c> as the "last argument pushed" for Pattern A'.
    /// <para/>
    /// Deliberately excludes <c>ldfld</c>/<c>unbox</c>/<c>isinst</c>/<c>castclass</c>: those
    /// derive a value from the state-machine address (e.g. <c>ldloca V; ldflda F</c>) and the
    /// builder may legitimately receive that derived address. Excluding them can only miss a
    /// slot, never mis-record one.
    /// </summary>
    private static bool PushesAValue(string op) => op is
        "ldarg" or "ldarga" or "ldloc" or "ldloca" or "ldnull" or "ldstr" or "ldflda"
        or "ldc.i4" or "ldc.i4.s" or "ldc.i4.m1" or "ldc.i4.0" or "ldc.i4.1" or "ldc.i4.2"
        or "ldc.i4.3" or "ldc.i4.4" or "ldc.i4.5" or "ldc.i4.6" or "ldc.i4.7" or "ldc.i4.8"
        or "ldc.i8" or "ldc.r4" or "ldc.r8" or "ldtoken" or "newobj" or "newarr"
        or "call" or "callvirt" or "dup" or "box" or "box.any" or "ldobj"
        or "add" or "sub" or "mul" or "div" or "rem" or "and" or "or" or "xor"
        or "shl" or "shr" or "neg" or "not" or "ceq" or "cgt" or "clt"
        or "ldind.i4" or "ldind.ref";

    /// <summary>
    /// Pre-scan the instruction list to identify which local slots hold
    /// Float32 or Float64 values. Simulates the eval stack type stack
    /// linearly across all instructions, recording the SlotType stored
    /// into each local by stloc. Falls back to NativeInt for ambiguous
    /// cases (locals stored with multiple types, or unresolvable types).
    /// </summary>
    private static Dictionary<int, SlotType> IdentifyFloatLocalSlots(IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
    {
        var result = new Dictionary<int, SlotType>();
        var typeStack = new Stack<SlotType>();

        // Iterate all instructions linearly, simulating the eval stack type flow.
        // This captures the type at each stloc, which tells us what type
        // ldloc should restore when loading the same local.
        for (int i = 0; i < instructions.Count; i++)
        {
            var op = instructions[i].Op;
            switch (op)
            {
                // Push float types
                case "ldc.r8":
                    typeStack.Push(SlotType.Float64);
                    break;
                case "ldc.r4":
                    typeStack.Push(SlotType.Float32);
                    break;

                // stloc: pop type and record for this local slot
                case "stloc":
                    if (typeStack.Count > 0)
                    {
                        int slot = GetRequiredIntOperand(instructions[i]);
                        SlotType storedType = typeStack.Pop();
                        // Only record if the type is float (NativeInt is the default, no need to track)
                        if (storedType is SlotType.Float32 or SlotType.Float64)
                            result[slot] = storedType;
                    }
                    break;

                // ldloc: push conservative type (we don't know yet; stloc above will capture)
                case "ldloc":
                    typeStack.Push(SlotType.NativeInt);
                    break;

                // Binary arithmetic: pop 2, push float if either operand is float
                case "add":
                case "sub":
                case "mul":
                case "div":
                case "rem":
                case "add.ovf":
                case "add.ovf.un":
                case "sub.ovf":
                case "sub.ovf.un":
                case "mul.ovf":
                case "mul.ovf.un":
                case "div.un":
                case "rem.un":
                    if (typeStack.Count >= 2)
                    {
                        SlotType rightType = typeStack.Pop();
                        SlotType leftType = typeStack.Pop();
                        typeStack.Push(
                            rightType is SlotType.Float32 or SlotType.Float64 ||
                            leftType is SlotType.Float32 or SlotType.Float64
                                ? SlotType.Float64
                                : SlotType.NativeInt);
                    }
                    break;

                // Conv: in-place type change
                case "conv.r8":
                    if (typeStack.Count > 0)
                    {
                        typeStack.Pop();
                        typeStack.Push(SlotType.Float64);
                    }
                    break;
                case "conv.r4":
                case "conv.r.un":
                    if (typeStack.Count > 0)
                    {
                        typeStack.Pop();
                        typeStack.Push(SlotType.Float32);
                    }
                    break;
                case "conv.i4":
                case "conv.i8":
                case "conv.u4":
                case "conv.u8":
                case "conv.i":
                case "conv.u":
                    if (typeStack.Count > 0)
                    {
                        typeStack.Pop();
                        typeStack.Push(SlotType.NativeInt);
                    }
                    break;

                // ldelem: element type determined by opcode
                case "ldelem.r8":
                    typeStack.Push(SlotType.Float64);
                    break;
                case "ldelem.r4":
                    typeStack.Push(SlotType.Float32);
                    break;

                // ceq/cgt/clt: always produce integer (NativeInt)
                case "ceq":
                case "cgt":
                case "clt":
                case "cgt.un":
                case "clt.un":
                    if (typeStack.Count >= 2)
                    {
                        typeStack.Pop();
                        typeStack.Pop();
                    }
                    typeStack.Push(SlotType.NativeInt);
                    break;

                // Instructions that push one value (ldc.i4, ldnull, ldarg, etc.)
                case "ldc.i4":
                case "ldc.i8":
                case "ldnull":
                case "ldarg":
                case "ldarga":
                case "ldloca":
                case "ldsfld":
                case "ldfld":
                case "ldlen":
                case "ldind.i4":
                case "ldind.i8":
                case "ldind.i":
                case "ldind.u4":
                case "ldind.u8":
                case "ldind.ref":
                case "ldobj":
                    typeStack.Push(SlotType.NativeInt);
                    break;

                // Instructions with no stack effect, or unknown: keep stack as-is
                case "nop":
                case "br":
                case "leave":
                case "endfinally":
                case "brtrue.s":
                case "brfalse.s":
                case "brtrue":
                case "brfalse":
                case "beq.s":
                case "bge.s":
                case "bgt.s":
                case "ble.s":
                case "blt.s":
                case "beq":
                case "bge":
                case "bgt":
                case "ble":
                case "blt":
                case "bne.un":
                case "bge.un":
                case "bgt.un":
                case "ble.un":
                case "blt.un":
                case "switch":
                    break;

                // pop: remove one value
                case "pop":
                    if (typeStack.Count > 0)
                        typeStack.Pop();
                    break;

                // dup: duplicate top
                case "dup":
                    if (typeStack.Count > 0)
                        typeStack.Push(typeStack.Peek());
                    break;

                // Default: assume unknown ops push one value (conservative)
                default:
                    typeStack.Push(SlotType.NativeInt);
                    break;
            }
        }

        return result;
    }



    /// <summary>
    /// Pre-scan the instruction list to identify which local slots hold
    /// Int64 values. Simulates the eval stack type stack linearly, recording
    /// the slot for each stloc of an Int64-typed value. This enables ldloc
    /// to push SlotType.Int64, so that EmitLinearBinaryArithmetic emits
    /// 64-bit arithmetic instead of int32 truncation.
    /// </summary>
    private static HashSet<int> IdentifyInt64LocalSlots(IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
    {
        var int64Locals = new HashSet<int>();
        var typeStack = new Stack<SlotType>();

        for (int i = 0; i < instructions.Count; i++)
        {
            var op = instructions[i].Op;
            switch (op)
            {
                // Int64 producers
                case "conv.i8":
                case "conv.u8":
                case "conv.ovf.i8":
                case "conv.ovf.u8":
                case "conv.ovf.i8.un":
                case "conv.ovf.u8.un":
                case "ldc.i8":
                case "ldind.i8":
                case "ldelem.i8":
                    typeStack.Push(SlotType.Int64);
                    break;
                case "ldelem.i":
                    typeStack.Push(SlotType.NativeInt);
                    break;

                // stloc: pop type and record for this local slot
                case "stloc":
                    if (typeStack.Count > 0)
                    {
                        int slot = GetRequiredIntOperand(instructions[i]);
                        SlotType storedType = typeStack.Pop();
                        if (storedType == SlotType.Int64)
                            int64Locals.Add(slot);
                    }
                    break;

                // ldloc: conservative — push NativeInt (overridden by int64Locals in emission)
                case "ldloc":
                    typeStack.Push(SlotType.NativeInt);
                    break;

                // Binary arithmetic: Int64 + any → Int64 (promotion)
                case "add":
                case "sub":
                case "mul":
                case "div":
                case "rem":
                case "add.ovf":
                case "add.ovf.un":
                case "sub.ovf":
                case "sub.ovf.un":
                case "mul.ovf":
                case "mul.ovf.un":
                case "div.un":
                case "rem.un":
                    if (typeStack.Count >= 2)
                    {
                        SlotType rightType = typeStack.Pop();
                        SlotType leftType = typeStack.Pop();
                        bool isInt64Result = (rightType == SlotType.Int64 || leftType == SlotType.Int64);
                        bool isFloatResult = rightType is SlotType.Float32 or SlotType.Float64 ||
                                             leftType is SlotType.Float32 or SlotType.Float64;
                        if (isFloatResult)
                            typeStack.Push(SlotType.Float64);
                        else if (isInt64Result)
                            typeStack.Push(SlotType.Int64);
                        else
                            typeStack.Push(SlotType.NativeInt);
                    }
                    break;

                // Conv narrowing: Int64 → NativeInt
                case "conv.i4":
                case "conv.u4":
                case "conv.i2":
                case "conv.u2":
                case "conv.i1":
                case "conv.u1":
                case "conv.i":
                case "conv.u":
                    if (typeStack.Count > 0)
                    {
                        typeStack.Pop();
                        typeStack.Push(SlotType.NativeInt);
                    }
                    break;

                // Conv float: Int64 → Float64
                case "conv.r4":
                case "conv.r.un":
                    if (typeStack.Count > 0)
                    {
                        typeStack.Pop();
                        typeStack.Push(SlotType.Float32);
                    }
                    break;
                case "conv.r8":
                    if (typeStack.Count > 0)
                    {
                        typeStack.Pop();
                        typeStack.Push(SlotType.Float64);
                    }
                    break;

                // ldelem: element type determined by opcode
                case "ldelem.r8":
                    typeStack.Push(SlotType.Float64);
                    break;
                case "ldelem.r4":
                    typeStack.Push(SlotType.Float32);
                    break;

                // ceq/cgt/clt: always produce integer (NativeInt)
                case "ceq":
                case "cgt":
                case "clt":
                case "cgt.un":
                case "clt.un":
                    if (typeStack.Count >= 2)
                    {
                        typeStack.Pop();
                        typeStack.Pop();
                    }
                    typeStack.Push(SlotType.NativeInt);
                    break;

                // Instructions that push one value (default to NativeInt)
                case "ldc.i4":
                case "ldnull":
                case "ldarg":
                case "ldarga":
                case "ldloca":
                case "ldsfld":
                case "ldfld":
                case "ldlen":
                case "ldind.i4":
                case "ldind.i":
                case "ldind.u4":
                case "ldind.ref":
                case "ldobj":
                case "ldelem.i4":
                case "ldelem.u4":
                case "ldelem.i2":
                case "ldelem.u2":
                case "ldelem.i1":
                case "ldelem.u1":
                case "ldelem.ref":
                case "newarr":
                case "box":
                case "isinst":
                case "castclass":
                    typeStack.Push(SlotType.NativeInt);
                    break;

                // Instructions with no stack effect
                case "nop":
                case "br":
                case "leave":
                case "endfinally":
                case "brtrue.s":
                case "brfalse.s":
                case "brtrue":
                case "brfalse":
                case "beq.s":
                case "bge.s":
                case "bgt.s":
                case "ble.s":
                case "blt.s":
                case "beq":
                case "bge":
                case "bgt":
                case "ble":
                case "blt":
                case "bne.un":
                case "bge.un":
                case "bgt.un":
                case "ble.un":
                case "blt.un":
                case "switch":
                    break;

                // pop: remove one value
                case "pop":
                    if (typeStack.Count > 0)
                        typeStack.Pop();
                    break;

                // dup: duplicate top
                case "dup":
                    if (typeStack.Count > 0)
                        typeStack.Push(typeStack.Peek());
                    break;

                // Default: assume unknown ops produce NativeInt
                default:
                    typeStack.Push(SlotType.NativeInt);
                    break;
            }
        }

        return int64Locals;
    }



    private SlotType PeekSlotType()
        => _state.Value!.ActiveStructuredSlotContext is not null && _state.Value!.StructuredSlotTypes.Count > 0
            ? _state.Value!.StructuredSlotTypes.Peek()
            : SlotType.NativeInt;



    private void PushSlotType(SlotType type)
    {
        if (_state.Value!.ActiveStructuredSlotContext is not null)
            _state.Value!.StructuredSlotTypes.Push(type);
    }



    private SlotType ConsumeSlotType()
    {
        if (_state.Value!.ActiveStructuredSlotContext is not null && _state.Value!.StructuredSlotTypes.Count > 0)
            return _state.Value!.StructuredSlotTypes.Pop();
        return SlotType.NativeInt;
    }



    private void UpdateSlotType(SlotType type)
    {
        if (_state.Value!.ActiveStructuredSlotContext is not null && _state.Value!.StructuredSlotTypes.Count > 0)
        {
            _state.Value!.StructuredSlotTypes.Pop();
            _state.Value!.StructuredSlotTypes.Push(type);
        }
    }



    private string AllocateLinearScratchName(string prefix)
        => "chaos_" + prefix + "_" + (_state.Value!.LinearScratchCounter++).ToString(CultureInfo.InvariantCulture);




    private static IReadOnlyList<AotCoreIrInstructionArtifact> FilterRedundantStoreReloadPairs(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        IReadOnlySet<int>? branchTargetOffsets = null,
        IReadOnlySet<int>? externallyReferencedLocals = null)
    {
        var result = new List<AotCoreIrInstructionArtifact>(instructions.Count);

        // Pre-scan: count ldloc occurrences per local slot.
        // If a local is read by ldloc instructions outside a stloc+ldloc pair,
        // we must preserve the stloc so chaos_locals[N] is populated for those reads.
        var ldlocCountBySlot = new Dictionary<int, int>();
        for (int i = 0; i < instructions.Count; i++)
        {
            if (instructions[i].Op is "ldloc")
            {
                int slot = GetRequiredIntOperand(instructions[i]);
                ldlocCountBySlot.TryGetValue(slot, out int count);
                ldlocCountBySlot[slot] = count + 1;
            }
        }

        int idx = 0;
        while (idx < instructions.Count)
        {
            if (idx + 1 < instructions.Count &&
                instructions[idx].Op is "stloc" &&
                instructions[idx + 1].Op is "ldloc" &&
                GetRequiredIntOperand(instructions[idx]) == GetRequiredIntOperand(instructions[idx + 1]))
            {
                // Don't skip the ldloc if it's a branch target ¡ª the label must be preserved.
                if (branchTargetOffsets is not null &&
                    branchTargetOffsets.Contains(GetRequiredIlOffset(instructions[idx + 1])))
                {
                    result.Add(instructions[idx]);
                    idx++;
                    continue;
                }

                int slot = GetRequiredIntOperand(instructions[idx]);

                // If the local slot is referenced by other instruction lists (e.g., then-body or
                // else-body of an if-then-else), we must preserve the stloc so chaos_locals[N] is
                // populated for those external references.
                if (externallyReferencedLocals is not null && externallyReferencedLocals.Contains(slot))
                {
                    // Keep stloc (other instruction lists read chaos_locals[slot]).
                    // Skip ldloc since the value remains on the eval stack.
                    result.Add(instructions[idx]);
                    idx++;
                    continue;
                }

                // If any OTHER instruction reads from this local slot in the current list,
                // we must keep the stloc so chaos_locals[N] is populated.
                if (ldlocCountBySlot.TryGetValue(slot, out int totalLdloc) && totalLdloc > 1)
                {
                    // Keep stloc (other instructions read chaos_locals[slot]).
                    // Skip ldloc since the value remains on the eval stack.
                    result.Add(instructions[idx]);
                    idx++;
                    continue;
                }

                idx += 2;  // Skip BOTH stloc and ldloc
                continue;
            }
            result.Add(instructions[idx]);
            idx++;
        }
        return result;
    }



    /// <summary>
    /// Emit a pre-filtered instruction list with next-instruction lookahead.
    /// Unlike EmitInstructionSequence, this does NOT call
    /// FilterRedundantStoreReloadPairs — the caller is responsible for
    /// any filtering.  Used by the structured IR path which already
    /// filters before each loop.
    /// </summary>



    /// <summary>
    /// Resolve the effective next instruction for box elimination lookahead.
    /// When <paramref name="nextInstruction"/> is provided (linear path), use it directly.
    /// When null (structured IR path), fall back to the lookahead context fields.
    /// </summary>



    /// <summary>
    /// Two-step lookahead for patterns spanning 3 instructions (box + ldnull + call).
    /// Only used in structured IR path; linear path uses nextInstruction directly.
    /// </summary>



    // ©¤©¤ Shared throw/rethrow emission helpers ©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤
    // Consolidated from three duplicate implementations:
    //   EmitInstruction (structured EH linear)
    //   EmitFlatGotoBody (flat goto fallback)
    //   EmitIRBlockTerminator (structured IR)




    /// <summary>
    /// Scan backward from current instruction to find the target method for Assert.Throws.
    /// Looks for the pattern: ldnull + ldftn <method> + newobj Action::.ctor.
    /// Returns (symbol, isStatic) — isStatic indicates whether the target method is
    /// static (no implicit 'this' argument) vs. an instance method on a DisplayClass.
    /// </summary>



    /// <summary>
    /// Try to inline a resolved callee method at the call site.
    /// Used by both direct calls and devirtualized callvirt.
    /// Supports simple multi-BB callees (up to 3 basic blocks) with branch
    /// target remapping and unified exit via goto chaos_inline_end{N}.
    /// </summary>



    /// <summary>Try to inline the callee method identified by the invocation target.</summary>

}
