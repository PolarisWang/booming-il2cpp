using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Standalone C++ function body emitter for bridge-compiled methods.
/// Generates real function bodies using compile-time eval-stack tracking
/// and ExternalRuntime table dispatch, avoiding C2733 symbol duplication.
/// Supports 40+ IL opcodes for framework method compilation.
/// </summary>
public static class BridgeMethodBodyEmitter
{
    private static readonly HashSet<string> LoadArgOps = new(StringComparer.Ordinal)
        { "ldarg.0", "ldarg.1", "ldarg.2", "ldarg.3", "ldarg.s", "ldarg" };
    private static readonly HashSet<string> LoadLocalOps = new(StringComparer.Ordinal)
        { "ldloc.0", "ldloc.1", "ldloc.2", "ldloc.3", "ldloc.s", "ldloc" };
    private static readonly HashSet<string> StoreLocalOps = new(StringComparer.Ordinal)
        { "stloc.0", "stloc.1", "stloc.2", "stloc.3", "stloc.s", "stloc" };
    private static readonly HashSet<string> CallOps = new(StringComparer.Ordinal)
        { "call", "callvirt", "newobj", "calli" };
    private static readonly HashSet<string> BranchOps = new(StringComparer.Ordinal)
        { "br", "br.s", "brfalse", "brfalse.s", "brtrue", "brtrue.s",
          "beq", "bne.un", "bge", "bgt", "ble", "blt",
          "bge.un", "bgt.un", "ble.un", "blt.un" };

    private static string MapAbiType(AotCoreIrAbiSlotArtifact abi) => abi.CarrierKindCode switch
    {
        AotCoreIrAbiCarrierKind.Void => "void",
        AotCoreIrAbiCarrierKind.Int8 or AotCoreIrAbiCarrierKind.UInt8 or AotCoreIrAbiCarrierKind.Int16
            or AotCoreIrAbiCarrierKind.UInt16 or AotCoreIrAbiCarrierKind.Int32 => "CHAOS_IL2CPP_INT32",
        AotCoreIrAbiCarrierKind.Int64 => "CHAOS_IL2CPP_INT64",
        AotCoreIrAbiCarrierKind.Float32 => "CHAOS_IL2CPP_FLOAT32",
        AotCoreIrAbiCarrierKind.Float64 => "CHAOS_IL2CPP_FLOAT64",
        _ => "CHAOS_IL2CPP_INTPTR",
    };

    /// <summary>Extract declaring type SubjectId from a method SubjectId.</summary>
    private static string? GetTypeSubjectId(string methodSubjectId)
    {
        // Format: "Assembly/Type::MethodName:ReturnType(Params)"
        var sep = methodSubjectId.IndexOf("::", StringComparison.Ordinal);
        return sep > 0 ? methodSubjectId[..sep] : null;
    }

    /// <summary>Extract declaring type SubjectId from a field SubjectId.</summary>
    private static string? GetFieldTypeSubjectId(string fieldSubjectId)
    {
        // Format: "Assembly/Type::FieldName:FieldType"
        var sep = fieldSubjectId.IndexOf("::", StringComparison.Ordinal);
        return sep > 0 ? fieldSubjectId[..sep] : null;
    }

    /// <summary>Deterministic uint32 hash from a SubjectId for runtime type lookup.</summary>
    private static uint HashSubjectId(string sid)
    {
        uint h = 2166136261;
        foreach (char c in sid)
            h = (h ^ (byte)c) * 16777619;
        return h;
    }

    public static string EmitMethodBody(AotCoreIrMethodArtifact method, int methodIndex,
        IReadOnlyDictionary<string, string>? redirectMap)
    {
        var sb = new StringBuilder();
        var ind = "    ";
        var retType = MapAbiType(method.ReturnAbi);
        var isVoid = retType == "void";
        var argCount = method.ParameterAbis.Count;

        // ── Function signature ──
        sb.AppendLine($"extern \"C\" {retType} {method.NativeSymbol}(");
        var sigParams = new List<string>();
        for (int i = 0; i < argCount; i++)
            sigParams.Add($"{MapAbiType(method.ParameterAbis[i])} chaos_fn_arg_{i}");
        if (sigParams.Count == 0) sigParams.Add("void");
        sb.AppendLine($"    {string.Join(",\n    ", sigParams)}");
        sb.AppendLine(") {");

        // ── Arg copy ──
        for (int i = 0; i < argCount; i++)
        {
            var pt = MapAbiType(method.ParameterAbis[i]);
            if (pt == "CHAOS_IL2CPP_INTPTR")
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR chaos_arg_{i} = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_{i});");
            else
                sb.AppendLine($"{ind}{pt} chaos_arg_{i} = chaos_fn_arg_{i};");
        }

        // ── Locals ──
        int localCount = EstimateMaxLocalIndex(method);
        for (int i = 0; i < localCount; i++)
            sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR chaos_local_{i} = 0;");

        // ── Eval stack ──
        var stack = new List<string>();
        var labelDefs = CollectBranchTargets(method.Instructions);
        int tempCounter = 0;
        string Tmp() => $"chaos_tmp_{tempCounter++}";
        string TmpArr() => $"chaos_tmp_arr_{tempCounter++}";

        sb.AppendLine();

        for (int i = 0; i < method.Instructions.Count; i++)
        {
            var inst = method.Instructions[i];
            var op = inst.Op;

            if (labelDefs.Contains(i))
                sb.AppendLine($"\nL{inst.IlOffset}:\n{ind};");

            // ── ldarg ──
            if (LoadArgOps.Contains(op))
            {
                int idx = op switch
                {
                    "ldarg.0" => 0, "ldarg.1" => 1, "ldarg.2" => 2, "ldarg.3" => 3,
                    _ when inst.Operand is int iv => iv, _ => 0,
                };
                stack.Add($"chaos_arg_{idx}");
            }
            // ── ldloc ──
            else if (LoadLocalOps.Contains(op))
            {
                int idx = op switch
                {
                    "ldloc.0" => 0, "ldloc.1" => 1, "ldloc.2" => 2, "ldloc.3" => 3,
                    _ when inst.Operand is int iv => iv, _ => 0,
                };
                stack.Add($"chaos_local_{idx}");
            }
            // ── stloc ──
            else if (StoreLocalOps.Contains(op))
            {
                int idx = op switch
                {
                    "stloc.0" => 0, "stloc.1" => 1, "stloc.2" => 2, "stloc.3" => 3,
                    _ when inst.Operand is int iv => iv, _ => 0,
                };
                sb.AppendLine($"{ind}chaos_local_{idx} = {Pop(stack)};");
            }
            // ── ldnull ──
            else if (op == "ldnull") stack.Add("static_cast<CHAOS_IL2CPP_INTPTR>(0)");
            // ── ldc.i4 ──
            else if (op.StartsWith("ldc.i4"))
            {
                int val = op switch
                {
                    "ldc.i4.0" => 0, "ldc.i4.1" => 1, "ldc.i4.2" => 2, "ldc.i4.3" => 3,
                    "ldc.i4.4" => 4, "ldc.i4.5" => 5, "ldc.i4.6" => 6, "ldc.i4.7" => 7,
                    "ldc.i4.8" => 8, "ldc.i4.m1" => -1,
                    _ when inst.Operand is int iv => iv,
                    _ when inst.Operand is sbyte sv => sv, _ => 0,
                };
                stack.Add($"static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>({val}))");
            }
            // ── ldc.i8 ──
            else if (op == "ldc.i8")
            {
                var val = inst.Operand is long lv ? lv : (inst.Operand is int iv ? (long)iv : 0L);
                stack.Add($"static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT64>({val}LL))");
            }
            // ── ldc.r4/r8 ──
            else if (op == "ldc.r4")
                stack.Add($"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(_mm_set_ss({inst.Operand ?? 0f}f))");
            else if (op == "ldc.r8")
                stack.Add($"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(_mm_set_sd({inst.Operand ?? 0.0}))");
            // ── ldstr ──
            else if (op == "ldstr")
            {
                var sv = inst.Operand as string ?? "";
                stack.Add($"reinterpret_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_STRING_ID({Escape(sv)}))");
            }
            // ── dup ──
            else if (op == "dup") { if (stack.Count > 0) stack.Add(stack[^1]); }
            // ── pop ──
            else if (op == "pop") Pop(stack);
            // ── Arithmetic ──
            else if (op is "add" or "sub" or "mul" or "div" or "rem" or "and" or "or" or "xor")
            {
                var cop = op switch
                {
                    "add" => "+", "sub" => "-", "mul" => "*", "div" => "/", "rem" => "%",
                    "and" => "&", "or" => "|", "xor" => "^", _ => "+",
                };
                var b = Pop(stack); var a = Pop(stack);
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = static_cast<CHAOS_IL2CPP_INTPTR>(" +
                    $"static_cast<CHAOS_IL2CPP_INT32>({a}) {cop} static_cast<CHAOS_IL2CPP_INT32>({b}));");
                stack.Add(tmp);
            }
            // ── neg ──
            else if (op == "neg")
            {
                var v = Pop(stack);
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = static_cast<CHAOS_IL2CPP_INTPTR>(-static_cast<CHAOS_IL2CPP_INT32>({v}));");
                stack.Add(tmp);
            }
            // ── not ──
            else if (op == "not")
            {
                var v = Pop(stack);
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = static_cast<CHAOS_IL2CPP_INTPTR>(~static_cast<CHAOS_IL2CPP_INT32>({v}));");
                stack.Add(tmp);
            }
            // ── Comparisons ──
            else if (op is "ceq" or "cgt" or "clt" or "cgt.un" or "clt.un")
            {
                var cop = op switch
                {
                    "ceq" => "==", "cgt" => ">", "clt" => "<",
                    "cgt.un" => ">", "clt.un" => "<", _ => "==",
                };
                var b = Pop(stack); var a = Pop(stack);
                var tmp = Tmp();
                // Unsigned comparison for .un variants (treat as uint32)
                var castA = op is "cgt.un" or "clt.un"
                    ? $"static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>({a}))"
                    : $"static_cast<CHAOS_IL2CPP_INT32>({a})";
                var castB = op is "cgt.un" or "clt.un"
                    ? $"static_cast<CHAOS_IL2CPP_UINT32>(static_cast<CHAOS_IL2CPP_INT32>({b}))"
                    : $"static_cast<CHAOS_IL2CPP_INT32>({b})";
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = static_cast<CHAOS_IL2CPP_INTPTR>({castA} {cop} {castB} ? 1 : 0);");
                stack.Add(tmp);
            }
            // ── Conversions ──
            else if (op is "conv.i4" or "conv.u4" or "conv.i" or "conv.u" or "conv.i1" or "conv.i2" or "conv.u1" or "conv.u2")
                { /* truncation to 8/16/32-bit: already INTPTR-width, no code needed */ }
            else if (op is "conv.i8" or "conv.u8")
                { /* INTPTR already holds 64-bit on x64 */ }
            else if (op is "conv.r4" or "conv.r8" or "conv.r.un")
                { /* float on eval stack already in correct format */ }
            else if (op is "conv.ovf.i" or "conv.ovf.u" or "conv.ovf.i1" or "conv.ovf.i2" or "conv.ovf.u1" or "conv.ovf.u2"
                     or "conv.ovf.i4" or "conv.ovf.u4" or "conv.ovf.i8" or "conv.ovf.u8")
                { /* overflow-checking conversions — no overflow check in stub */ }
            // ── Shifts ──
            else if (op is "shl" or "shr" or "shr.un")
            {
                var cop = op switch { "shl" => "<<", "shr" => ">>", _ => ">>" };
                var b = Pop(stack); var a = Pop(stack);
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>({a}) {cop} static_cast<CHAOS_IL2CPP_INT32>({b}));");
                stack.Add(tmp);
            }
            // ── Box ──
            else if (op == "box")
            {
                var v = Pop(stack);
                var typeId = inst.Callee ?? inst.Reference?.SubjectId ?? "";
                var tmp = Tmp();
                var typeHash = typeId.Length > 0 ? HashSubjectId(typeId) : 0u;
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(" +
                    $"chaos::il2cpp::runtime_core::BoxValueObject({typeHash}u, reinterpret_cast<void*>(&{v})));");
                stack.Add(tmp);
            }
            // ── Unbox ──
            else if (op == "unbox" || op == "unbox.any")
            {
                var v = Pop(stack);
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>({v});");
                stack.Add(tmp);
            }
            // ── ldfld ──
            else if (op == "ldfld")
            {
                var obj = Pop(stack);
                var fieldId = inst.Reference?.SubjectId ?? inst.Callee ?? "";
                var tmp = Tmp();
                var fieldHash = fieldId.Length > 0 ? $"_fld_{HashSubjectId(fieldId):X4}" : "";
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>({obj}){fieldHash};");
                stack.Add(tmp);
            }
            // ── ldsfld ──
            else if (op == "ldsfld")
            {
                var fieldId = inst.Reference?.SubjectId ?? inst.Callee ?? "";
                var fieldHash = fieldId.Length > 0 ? $"_sfld_{HashSubjectId(fieldId):X4}" : "";
                var tmp = Tmp();
                sb.AppendLine($"{ind}extern CHAOS_IL2CPP_INTPTR {fieldHash};");
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = {fieldHash};");
                stack.Add(tmp);
            }
            // ── stfld ──
            else if (op == "stfld")
            {
                var val = Pop(stack); var obj = Pop(stack);
                var fieldId = inst.Reference?.SubjectId ?? inst.Callee ?? "";
                var fieldHash = fieldId.Length > 0 ? $"_fld_{HashSubjectId(fieldId):X4}" : "";
                sb.AppendLine($"{ind}*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>({obj}){fieldHash} = {val};");
            }
            // ── stsfld ──
            else if (op == "stsfld")
            {
                var val = Pop(stack);
                var fieldId = inst.Callee ?? "";
                var fieldHash = fieldId.Length > 0 ? $"_sfld_{HashSubjectId(fieldId):X4}" : "";
                sb.AppendLine($"{ind}extern CHAOS_IL2CPP_INTPTR {fieldHash};");
                sb.AppendLine($"{ind}{fieldHash} = {val};");
            }
            // ── ldflda ──
            else if (op == "ldflda")
            {
                var obj = Pop(stack);
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = {obj}; /* ldflda: using object addr */");
                stack.Add(tmp);
            }
            // ── ldloca ──
            else if (op is "ldloca" or "ldloca.s")
            {
                int idx = inst.Operand is int iv ? iv : 0;
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_local_{idx});");
                stack.Add(tmp);
            }
            // ── ldarga ──
            else if (op is "ldarga" or "ldarga.s")
            {
                int idx = inst.Operand is int iv ? iv : 0;
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_arg_{idx});");
                stack.Add(tmp);
            }
            // ── initobj ──
            else if (op == "initobj")
            {
                var addr = Pop(stack);
                sb.AppendLine($"{ind}std::memset(reinterpret_cast<void*>({addr}), 0, sizeof(CHAOS_IL2CPP_INTPTR));");
            }
            // ── ldlen ──
            else if (op == "ldlen")
            {
                var arr = Pop(stack);
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = static_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_ARRAY_LENGTH({arr}));");
                stack.Add(tmp);
            }
            // ── ldftn / ldvirtftn ──
            else if (op is "ldftn" or "ldvirtftn")
            {
                var callee = inst.Callee ?? "";
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ResolveBridge({Escape(callee)}));");
                stack.Add(tmp);
            }
            // ── newarr ──
            else if (op == "newarr")
            {
                var len = Pop(stack);
                var tmp = Tmp();
                var typeId = inst.Callee ?? inst.Reference?.SubjectId ?? "";
                var typeHash = typeId.Length > 0 ? HashSubjectId(typeId) : 0u;
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(" +
                    $"chaos::il2cpp::runtime_core::ArrayNew({typeHash}u, static_cast<CHAOS_IL2CPP_INT32>({len})));");
                stack.Add(tmp);
            }
            // ── isinst (type check) ──
            else if (op == "isinst")
            {
                var obj = Pop(stack);
                var typeId = inst.Reference?.SubjectId ?? "";
                var tmp = Tmp();
                if (typeId.Length > 0)
                {
                    var typeHash = HashSubjectId(typeId);
                    sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(" +
                        $"chaos::il2cpp::runtime_core::chaos_isinst(reinterpret_cast<CHAOS_IL2CPP_OBJECT_TYPE*>({obj}), {typeHash}u));");
                }
                else
                {
                    sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = {obj}; /* isinst: no type info */");
                }
                stack.Add(tmp);
            }
            // ── castclass ──
            else if (op == "castclass")
            {
                var obj = Pop(stack);
                var typeId = inst.Reference?.SubjectId ?? "";
                var tmp = Tmp();
                if (typeId.Length > 0)
                {
                    var typeHash = HashSubjectId(typeId);
                    sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(" +
                        $"chaos::il2cpp::runtime_core::chaos_castclass(reinterpret_cast<CHAOS_IL2CPP_OBJECT_TYPE*>({obj}), {typeHash}u));");
                }
                else
                {
                    sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = {obj}; /* castclass: no type info */");
                }
                stack.Add(tmp);
            }
            // ── ldelem / stelem ──
            else if (op is "ldelem" or "ldelem.i4" or "ldelem.i8" or "ldelem.r4"
                     or "ldelem.r8" or "ldelem.ref" or "ldelem.u4" or "ldelem.u8")
            {
                var idx = Pop(stack); var arr = Pop(stack);
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>({arr})[static_cast<CHAOS_IL2CPP_INT32>({idx}) + 1]; /* skip header */");
                stack.Add(tmp);
            }
            else if (op is "stelem" or "stelem.i4" or "stelem.i8" or "stelem.ref" or "stelem.r4" or "stelem.r8")
            {
                var val = Pop(stack); var idx = Pop(stack); var arr = Pop(stack);
                sb.AppendLine($"{ind}reinterpret_cast<CHAOS_IL2CPP_INTPTR*>({arr})[static_cast<CHAOS_IL2CPP_INT32>({idx}) + 1] = {val};");
            }
            // ── Throw / rethrow ──
            else if (op is "throw" or "rethrow")
            {
                var ex = op == "throw" ? Pop(stack) : "0";
                sb.AppendLine($"{ind}chaos::il2cpp::runtime_core::chaos_raise_exception(reinterpret_cast<CHAOS_IL2CPP_OBJECT_TYPE*>({ex}));");
            }
            // ── endfinally / leave / nop / volatile / readonly. ──
            else if (op is "endfinally" or "leave" or "leave.s" or "nop" or "break"
                     or "volatile." or "readonly." or "unaligned." or "tail." or "constrained.")
                { /* no-op */ }
            // ── Calls ──
            else if (CallOps.Contains(op))
                EmitCall(sb, inst, stack, ind, redirectMap, Tmp);
            // ── Branch ──
            else if (BranchOps.Contains(op))
                EmitBranch(sb, inst, stack, ind);
            // ── switch ──
            else if (op == "switch")
            {
                var val = Pop(stack);
                var targets = inst.Operand as int[];
                if (targets != null && targets.Length > 0)
                {
                    for (int t = 0; t < targets.Length; t++)
                        sb.AppendLine($"{ind}if (static_cast<CHAOS_IL2CPP_INT32>({val}) == {t}) goto L{targets[t]};");
                }
            }
            // ── sizeof ──
            else if (op == "sizeof")
                stack.Add("static_cast<CHAOS_IL2CPP_INTPTR>(sizeof(CHAOS_IL2CPP_INTPTR))");
            // ── cpblk / initblk ──
            else if (op == "cpblk")
            {
                var count = Pop(stack); var src = Pop(stack); var dst = Pop(stack);
                sb.AppendLine($"{ind}std::memcpy(reinterpret_cast<void*>({dst}), reinterpret_cast<const void*>({src}), static_cast<size_t>({count}));");
            }
            else if (op == "initblk")
            {
                var count = Pop(stack); var val = Pop(stack); var addr = Pop(stack);
                sb.AppendLine($"{ind}std::memset(reinterpret_cast<void*>({addr}), static_cast<int>({val}), static_cast<size_t>({count}));");
            }
            // ── ckfinite ──
            else if (op == "ckfinite")
            {
                var v = Pop(stack);
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = {v}; /* ckfinite: no check */");
                stack.Add(tmp);
            }
            // ── localloc (stack-allocate bytes) ──
            else if (op == "localloc")
            {
                var size = Pop(stack);
                var tmp = Tmp();
                sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(_alloca(static_cast<size_t>({size})));");
                stack.Add(tmp);
            }
            // ── endfilter / arglist / mkrefany / refanyval / refanytype ──
            else if (op is "endfilter" or "arglist" or "mkrefany" or "refanyval" or "refanytype")
                { /* rare — no-op stub */ }
            else if (op == "ret")
            {
                if (!isVoid && stack.Count > 0)
                    sb.AppendLine($"{ind}return static_cast<{retType}>({Pop(stack)});");
                else if (!isVoid)
                    sb.AppendLine($"{ind}return 0;");
                else
                    sb.AppendLine($"{ind}return;");
            }
            else
            {
                sb.AppendLine($"{ind}// UNSUPPORTED: {op}");
            }
        }

        sb.AppendLine("}");
        return sb.ToString();
    }

    private static void EmitCall(StringBuilder sb, AotCoreIrInstructionArtifact inst,
        List<string> stack, string ind, IReadOnlyDictionary<string, string>? redirectMap,
        Func<string> Tmp)
    {
        var callee = inst.Callee ?? "";
        var ts = inst.TargetSymbol ?? "";
        var isNewObj = inst.Op == "newobj";
        var isCalli = inst.Op == "calli";

        // Pop call args (IL pushes left-to-right, pop right-to-left)
        var paramCount = inst.TargetParameterCount ?? 0;
        var args = new List<string>();
        for (int i = 0; i < paramCount; i++)
            args.Insert(0, Pop(stack));

        // calli: the last "arg" is the function pointer (call target)
        string? fnPtr = null;
        if (isCalli)
        {
            fnPtr = args.Count > 0 ? args[^1] : "0";
            if (args.Count > 0) args.RemoveAt(args.Count - 1);
        }

        // newobj: allocate object then call constructor
        if (isNewObj)
        {
            var tmp = Tmp();
            sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(" +
                "chaos::il2cpp::runtime_core::ObjectNew(0));");

            if (!string.IsNullOrEmpty(callee) || !string.IsNullOrEmpty(ts))
            {
                // Call constructor on the new object
                sb.AppendLine($"{ind}{{");
                sb.AppendLine($"{ind}    auto _this = {tmp};");
                if (inst.DispatchKindCode == HybridDispatchKind.Direct && !string.IsNullOrEmpty(ts))
                {
                    var allArgs = string.Join(", ", new[] { "_this" }.Concat(args));
                    sb.AppendLine($"{ind}    extern \"C\" void {ts}(CHAOS_IL2CPP_INTPTR); {ts}({allArgs});");
                }
                else
                {
                    var escaped = Escape(callee);
                    sb.AppendLine($"{ind}    static int _idx = -2;");
                    sb.AppendLine($"{ind}    if (_idx == -2) {{");
                    sb.AppendLine($"{ind}        for (int _i = 0; _i < kChaosExternalRuntimeCount; _i++)");
                    sb.AppendLine($"{ind}            if (std::strcmp(kChaosExternalRuntimeSubjects[_i], {escaped}) == 0)");
                    sb.AppendLine($"{ind}            {{ _idx = _i; break; }}");
                    sb.AppendLine($"{ind}    }}");
                    sb.AppendLine($"{ind}    if (_idx >= 0 && kChaosExternalRuntimeFnTable[_idx]) {{");
                    sb.AppendLine($"{ind}        reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[_idx])(_this);");
                    sb.AppendLine($"{ind}    }}");
                }
                sb.AppendLine($"{ind}}}");
            }
            stack.Add(tmp);
            return;
        }

        // Normal call
        if (inst.DispatchKindCode == HybridDispatchKind.Direct && !string.IsNullOrEmpty(ts))
        {
            // Direct dispatch to chaos_bridge_ symbol
            var allArgs = string.Join(", ", args);
            var tmp = Tmp();
            sb.AppendLine($"{ind}{{ extern \"C\" void {ts}(); {ts}({allArgs}); }}");
            sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = 0;");
            stack.Add(tmp);
        }
        else if (!string.IsNullOrEmpty(callee))
        {
            // ExternalRuntime dispatch
            var escaped = Escape(callee);
            var tmp = Tmp();
            sb.AppendLine($"{ind}{{");
            sb.AppendLine($"{ind}    static int _idx = -2;");
            sb.AppendLine($"{ind}    if (_idx == -2) {{");
            sb.AppendLine($"{ind}        _idx = -1;");
            sb.AppendLine($"{ind}        for (int _i = 0; _i < kChaosExternalRuntimeCount; _i++)");
            sb.AppendLine($"{ind}            if (std::strcmp(kChaosExternalRuntimeSubjects[_i], {escaped}) == 0)");
            sb.AppendLine($"{ind}            {{ _idx = _i; break; }}");
            sb.AppendLine($"{ind}    }}");
            sb.AppendLine($"{ind}    if (_idx >= 0 && kChaosExternalRuntimeFnTable[_idx]) {{");
            sb.AppendLine($"{ind}        auto _fn = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[_idx]);");
            var callArgs = args.Count > 0 ? string.Join(", ", args) : "0";
            sb.AppendLine($"{ind}        _fn({callArgs});");
            sb.AppendLine($"{ind}    }}");
            sb.AppendLine($"{ind}}}");
            sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = 0;");
            stack.Add(tmp);
        }
        else if (isCalli && fnPtr != null)
        {
            // calli: call through function pointer
            var allArgs = string.Join(", ", args);
            var tmp = Tmp();
            sb.AppendLine($"{ind}auto {tmp} = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)({allArgs})>({fnPtr});");
            sb.AppendLine($"{ind}if ({tmp}) {tmp}({allArgs});");
            sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp}_result = 0;");
            stack.Add($"{tmp}_result");
        }
        else
        {
            var tmp = Tmp();
            sb.AppendLine($"{ind}CHAOS_IL2CPP_INTPTR {tmp} = 0;");
            stack.Add(tmp);
        }
    }

    private static void EmitBranch(StringBuilder sb, AotCoreIrInstructionArtifact inst,
        List<string> stack, string ind)
    {
        var op = inst.Op;
        var target = inst.Operand is int ti ? ti : 0;

        if (op is "br" or "br.s")
        {
            sb.AppendLine($"{ind}goto L{target};");
        }
        else if (op is "brfalse" or "brfalse.s")
        {
            sb.AppendLine($"{ind}if ({Pop(stack)} == 0) goto L{target};");
        }
        else if (op is "brtrue" or "brtrue.s")
        {
            sb.AppendLine($"{ind}if ({Pop(stack)} != 0) goto L{target};");
        }
        else if (op is "beq" or "bne.un" or "bge" or "bgt" or "ble" or "blt"
                 or "bge.un" or "bgt.un" or "ble.un" or "blt.un")
        {
            var b = Pop(stack); var a = Pop(stack);
            var cop = op switch
            {
                "beq" => "==", "bne.un" => "!=",
                "bge" or "bge.un" => ">=", "bgt" or "bgt.un" => ">",
                "ble" or "ble.un" => "<=", "blt" or "blt.un" => "<",
                _ => "==",
            };
            sb.AppendLine($"{ind}if ({a} {cop} {b}) goto L{target};");
        }
    }

    private static string Pop(List<string> stack)
    {
        if (stack.Count == 0) return "0";
        var last = stack[^1];
        stack.RemoveAt(stack.Count - 1);
        return last;
    }

    private static HashSet<int> CollectBranchTargets(IReadOnlyList<AotCoreIrInstructionArtifact> insts)
    {
        var targets = new HashSet<int>();
        foreach (var inst in insts)
        {
            if (inst.Operand is int off)
            {
                for (int j = 0; j < insts.Count; j++)
                {
                    if (insts[j].IlOffset == off || insts[j].IlOffset > off)
                    { targets.Add(j); break; }
                }
            }
        }
        return targets;
    }

    private static int EstimateMaxLocalIndex(AotCoreIrMethodArtifact method)
    {
        int max = -1;
        foreach (var inst in method.Instructions)
        {
            int idx = inst.Op switch
            {
                "ldloc.0" or "stloc.0" => 0, "ldloc.1" or "stloc.1" => 1,
                "ldloc.2" or "stloc.2" => 2, "ldloc.3" or "stloc.3" => 3,
                _ when (inst.Op is "ldloc" or "stloc" or "ldloc.s" or "stloc.s") && inst.Operand is int iv => iv,
                _ => -1,
            };
            if (idx > max) max = idx;
        }
        return max + 1;
    }

    private static string Escape(string s)
    {
        var sb = new StringBuilder(s.Length + 2);
        sb.Append('"');
        foreach (char c in s)
        {
            if (c == '\\') sb.Append("\\\\");
            else if (c == '"') sb.Append("\\\"");
            else if (c == '\n') sb.Append("\\n");
            else if (c == '\r') sb.Append("\\r");
            else if (c == '\t') sb.Append("\\t");
            else sb.Append(c);
        }
        sb.Append('"');
        return sb.ToString();
    }
}
