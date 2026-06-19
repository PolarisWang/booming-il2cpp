using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{



    /// <summary>
    /// Pre-scan for <c>ldtoken &lt;EnumType&gt; + call Enum::Parse/Format/IsDefined</c>
    /// patterns where the enum type is known at codegen time. Records the call-site
    /// IlOffset so <c>EmitLinearCallTarget</c> can emit specialized (AOT-baked) C++
    /// strcmp-based code instead of routing through generic runtime helpers.
    /// </summary>
    private void BuildEnumAotBakeTable(IReadOnlyList<AotCoreIrMethodArtifact> methodsForLowering)
    {
        _enumAotBakeMap.Clear();
        _enumAotBakeSkipIlOffsets.Clear();

        // Collect field entries — prefer reflection metadata, fall back to PE metadata scan
        var fieldEntries = _reflectionMemberSupport.FieldEntries.Count > 0
            ? _reflectionMemberSupport.FieldEntries
            : CollectEnumFieldEntriesFromMetadata();
        if (fieldEntries.Count == 0)
            return;

        // Build enum type → { fieldName → constantValue } map
        var enumFieldsByType = new Dictionary<string, Dictionary<string, long>>(StringComparer.Ordinal);
        foreach (var field in fieldEntries)
        {
            if (!_enumTypeSubjectIds.Contains(field.DeclaringTypeSubjectId))
                continue;
            if (!field.ConstantValue.HasValue)
                continue;
            if (!enumFieldsByType.TryGetValue(field.DeclaringTypeSubjectId, out var fields))
                enumFieldsByType[field.DeclaringTypeSubjectId] = fields = new(StringComparer.Ordinal);
            fields[field.FieldName] = field.ConstantValue.Value;
        }
        Console.Error.WriteLine($"[AOT Bake] scanning {methodsForLowering.Count} methods, {enumFieldsByType.Count} enum types with {fieldEntries.Count} fields");
        if (enumFieldsByType.Count == 0)
            return;

        foreach (var method in methodsForLowering)
        {
            var instrs = method.Instructions;
            if (instrs.Count == 0) continue;
            string? methodId = method.NativeSymbol;

            var producers = new int[2048];
            int depth = 0;

            for (int i = 0; i < instrs.Count; i++)
            {
                var instr = instrs[i];

                switch (instr.Op)
                {
                    case "ldtoken":
                        if (depth >= producers.Length) return;
                        producers[depth++] = i;
                        break;

                    case "ldstr":
                    case "ldc.i4":
                    case "ldc.i8":
                    case "ldc.r4":
                    case "ldc.r8":
                    case "ldarg":
                    case "ldloc":
                    case "ldnull":
                    case "ldfld":
                    case "ldsfld":
                    case "ldlen":
                    case "ldelema":
                    case "ldarga":
                    case "ldloca":
                        if (depth >= producers.Length) return;
                        producers[depth++] = i;
                        break;

                    case "stloc":
                    case "starg":
                    case "pop":
                    case "stsfld":
                    case "stfld":
                    case "stind.i":
                    case "stind.i8":
                    case "stind.i4":
                    case "stind.r4":
                    case "stind.r8":
                    case "stind.ref":
                    case "stobj":
                        if (depth > 0) depth--;
                        break;

                    case "box":
                        // box reinterprets the value; producer unchanged
                        break;

                    case "dup":
                        if (depth > 0 && depth < producers.Length)
                        {
                            producers[depth] = producers[depth - 1];
                            depth++;
                        }
                        break;

                    case "call":
                    case "callvirt":
                        TryRecordEnumAotBake(instr, instrs, producers, depth, enumFieldsByType, methodId, i);
                        // Conservative: pop N args (determined by callee), push 1 result
                        int popCount = EstimateCallPopCount(instr, depth);
                        if (depth >= popCount)
                            depth -= popCount - 1;  // pop N, push 1
                        else
                            depth = 1;  // underflow guard
                        break;

                    case "newobj":
                        if (depth > 0) depth--;
                        depth++;
                        break;

                    case "ret":
                        depth = 0;
                        break;

                    default:
                        if (instr.Op.StartsWith("br") || instr.Op == "switch")
                        {
                            if (instr.Op != "br" && depth > 0) depth--;
                        }
                        else if (instr.Op.StartsWith("st") && depth > 0)
                        {
                            depth--;
                        }
                        else if (instr.Op.StartsWith("ld"))
                        {
                            depth++;
                        }
                        else if (instr.Op is "add" or "sub" or "mul" or "div" or "rem"
                            or "and" or "or" or "xor" or "shl" or "shr"
                            or "ceq" or "cgt" or "clt")
                        {
                            if (depth > 1) depth--;
                        }
                        else if (instr.Op is "conv.i4" or "conv.i8" or "conv.r4" or "conv.r8"
                            or "conv.u4" or "conv.u8" or "conv.u2" or "conv.u1"
                            or "conv.i2" or "conv.i1" or "conv.r.un")
                        {
                            // type conversion: pop 1, push 1 — depth unchanged
                        }
                        break;
                }
            }
        }

        // Populate _enumAotBakeSkipIlOffsets from entries with SkipIlOffsets
        foreach (var kv in _enumAotBakeMap)
        {
            var entry = kv.Value;
            if (entry.SkipIlOffsets is { Length: > 0 })
            {
                var methodId = kv.Key.MethodId;
                if (!_enumAotBakeSkipIlOffsets.TryGetValue(methodId, out var set))
                {
                    set = new HashSet<int>();
                    _enumAotBakeSkipIlOffsets[methodId] = set;
                }
                foreach (var offset in entry.SkipIlOffsets)
                {
                    if (offset > 0)
                        set.Add(offset);
                }
            }
        }
    }





    /// <summary>
    /// Estimates how many arguments a call instruction pops from the eval stack.
    /// Uses the callee signature when available; falls back to conservative default.
    /// </summary>
    private static int EstimateCallPopCount(AotCoreIrInstructionArtifact instr, int currentDepth)
    {
        var callee = instr.Callee;
        if (callee == null) return 1;

        bool isInstance = instr.Op == "callvirt";
        int count = isInstance ? 1 : 0;  // instance methods have 'this' as first arg

        // Count commas in the parameter list portion of the callee signature
        // Format: "Namespace.Type::MethodName:ReturnType(Param1,Param2)"
        int parenStart = callee.IndexOf('(');
        int parenEnd = callee.LastIndexOf(')');
        if (parenStart > 0 && parenEnd > parenStart)
        {
            var paramStr = callee.Substring(parenStart + 1, parenEnd - parenStart - 1);
            if (paramStr.Length > 0)
                count += paramStr.Count(c => c == ',') + 1;
        }
        else
        {
            // Conservative default for unknown signatures
            count += 2;
        }
        return Math.Min(count, currentDepth);
    }





    /// <summary>
    /// For a <c>call</c>/<c>callvirt</c> to an enum static method, check if the
    /// type argument was produced by <c>ldtoken &lt;EnumType&gt;</c>. If so, record
    /// the call site for AOT-baked specialized code emission.
    /// </summary>
    private void TryRecordEnumAotBake(
        AotCoreIrInstructionArtifact callInstr,
        IReadOnlyList<AotCoreIrInstructionArtifact> instrs,
        int[] producers,
        int depth,
        Dictionary<string, Dictionary<string, long>> enumFieldsByType,
        string? methodId,
        int callIndex)
    {
        var callee = callInstr.Callee;
        if (callee == null) return;

        // Only handle static System.Enum methods
        if (callInstr.Op == "callvirt") return;
        if (!callee.StartsWith("System.Private.CoreLib/System.Enum::", StringComparison.Ordinal))
        {
            return;
        }


        // Focus on static methods where the first argument is Type:
        //   Parse(Type, String), Format(Type, Object, String),
        //   GetName(Type, Object), IsDefined(Type, Object),
        //   GetNames(Type), GetValues(Type)
        bool isGetName = callee.Contains("::GetName:", StringComparison.Ordinal);
        bool isFormat = callee.Contains("::Format:", StringComparison.Ordinal);
        bool isParse = callee.Contains("::Parse:", StringComparison.Ordinal);
        bool isIsDefined = callee.Contains("::IsDefined:", StringComparison.Ordinal);
        bool isTryParse = callee.Contains("::TryParse:", StringComparison.Ordinal);
        if (!isGetName && !isFormat && !isParse && !isIsDefined && !isTryParse) return;

        int paramCount = EstimateCallPopCount(callInstr, depth);
        // After the call, the first arg is always at eval stack position (depth - 1).
        // This works because the call pops paramCount args and pushes 1 result,
        // so depth_after = depth_before - paramCount + 1.
        // The first arg is at (depth - paramCount) where depth is the eval stack
        // depth BEFORE the call. producers is not updated for call results,
        // so after "ldtoken → call GetTypeFromHandle", producers[typeArgDepth]
        // still holds the original ldtoken (stale-but-useful).
        int typeArgDepth = depth - paramCount;
        if (typeArgDepth < 0) return;

        int typeProducerIdx = producers[typeArgDepth];
        if (typeProducerIdx < 0) return;

        var typeProducer = instrs[typeProducerIdx];

        if (typeProducer.Op != "ldtoken" || typeProducer.TargetReference?.SubjectId == null)
        {
            return;
        }

        string enumTypeId = typeProducer.TargetReference.SubjectId;
        if (!enumFieldsByType.TryGetValue(enumTypeId, out var fields))
        {
            // Not an enum type (e.g. byte, int): only Format with D/X/G can be baked,
            // since the result is a numeric string independent of enum field names.
            if (isFormat && paramCount == 3)
                TryRecordNonEnumFormatBake(callee, methodId, callIndex, instrs, typeProducerIdx, producers, depth, enumTypeId);
            return;
        }

        var bakeKey = (methodId ?? "", callInstr.IlOffset);

        // Scan for dead instructions between the ldtoken and this call:
        //   - call Type::GetTypeFromHandle (present for all — expensive metadata call)
        //   - box instruction (present for GetName/Format/IsDefined — GC allocation)
        // These are skipped during emission via _enumAotBakeSkipIlOffsets DCE.
        int[]? skipOffsets = null;
        bool isGetNameOrFormatOrIsDefined = isGetName || isFormat || isIsDefined;
        for (int si = typeProducerIdx + 1; si < callIndex; si++)
        {
            var scout = instrs[si];
            if (scout.Op == "call" && scout.Callee != null &&
                scout.Callee.Contains("::GetTypeFromHandle:", StringComparison.Ordinal))
            {
                (skipOffsets ??= new int[2])[0] = scout.IlOffset;
            }
            else if (scout.Op == "box" && isGetNameOrFormatOrIsDefined)
            {
                (skipOffsets ??= new int[2])[1] = scout.IlOffset;
            }
        }
        // Compact: remove trailing zero if box not found but GetTypeFromHandle was
        if (skipOffsets != null && skipOffsets[1] == 0 && skipOffsets[0] != 0)
            skipOffsets = new[] { skipOffsets[0] };

        // Check the value/name/format arguments for compile-time constants
        int valueArgDepth = typeArgDepth + 1; // arg after Type
        if (valueArgDepth >= depth) return;

        int valueProducerIdx = producers[valueArgDepth];
        if (valueProducerIdx < 0) return;

        var valueProducer = instrs[valueProducerIdx];

        // ── Enum.GetName(ldtoken<EnumType>, ldc.i4 value) → string ─────
        if (isGetName && valueProducer.Op is "ldc.i4" or "ldc.i8")
        {
            long constValue = Convert.ToInt64(valueProducer.Operand);
            string? fieldName = null;
            foreach (var kv in fields)
            {
                if (kv.Value == constValue) { fieldName = kv.Key; break; }
            }
            if (fieldName != null)
            {
                _enumAotBakeMap[bakeKey] = new EnumAotBakeEntry(
                    enumTypeId, callee, ConstantStr: fieldName, ConstantInt: null, ArgCount: 2, SkipIlOffsets: skipOffsets);
            }
            return;
        }

        // ── Enum.Parse(ldtoken<EnumType>, ldstr name) → boxed int64 ────
        bool isParse2 = isParse && paramCount == 2;  // Parse(Type, string)
        bool isParse3 = isParse && paramCount == 3;  // Parse(Type, string, bool)
        if ((isParse2 || isParse3) && valueProducer.Op == "ldstr")
        {
            string nameStr = valueProducer.Operand?.ToString() ?? "";
            long? constValue = null;
            foreach (var kv in fields)
            {
                if (string.Equals(kv.Key, nameStr, StringComparison.Ordinal))
                {
                    constValue = kv.Value;
                    break;
                }
            }
            // Case-insensitive fallback for Parse(Type, string, bool ignoreCase)
            if (constValue == null && isParse3)
            {
                foreach (var kv in fields)
                {
                    if (string.Equals(kv.Key, nameStr, StringComparison.OrdinalIgnoreCase))
                    {
                        constValue = kv.Value;
                        break;
                    }
                }
            }
            if (constValue != null)
            {
                _enumAotBakeMap[bakeKey] = new EnumAotBakeEntry(
                    enumTypeId, callee, ConstantStr: null, ConstantInt: constValue, ArgCount: paramCount, SkipIlOffsets: skipOffsets);
            }
            return;
        }

        // ── Enum.TryParse(ldtoken<EnumType>, ldstr name, [bool ignoreCase,] out Object) ──
        // Returns bool (1/0), writes boxed value through out parameter.
        bool isTryParse3 = isTryParse && paramCount == 3;  // TryParse(Type, string, out)
        bool isTryParse4 = isTryParse && paramCount == 4;  // TryParse(Type, string, bool, out)
        if ((isTryParse3 || isTryParse4) && valueProducer.Op == "ldstr")
        {
            string nameStr = valueProducer.Operand?.ToString() ?? "";
            long? constValue = null;
            // Case-sensitive match first
            foreach (var kv in fields)
            {
                if (string.Equals(kv.Key, nameStr, StringComparison.Ordinal))
                {
                    constValue = kv.Value;
                    break;
                }
            }
            // Case-insensitive fallback for TryParse(Type, string, bool, out)
            if (constValue == null && isTryParse4)
            {
                foreach (var kv in fields)
                {
                    if (string.Equals(kv.Key, nameStr, StringComparison.OrdinalIgnoreCase))
                    {
                        constValue = kv.Value;
                        break;
                    }
                }
            }
            if (constValue != null)
            {
                _enumAotBakeMap[bakeKey] = new EnumAotBakeEntry(
                    enumTypeId, callee, ConstantStr: null, ConstantInt: constValue, ArgCount: paramCount, SkipIlOffsets: skipOffsets);
            }
            return;
        }

        // ── Enum.Format(ldtoken<EnumType>, ldc.i4 value, ldstr format) → string ──
        int fmtArgDepth = typeArgDepth + 2; // Enum.Format(Type, Object, String)
        if (isFormat && fmtArgDepth < depth &&
            valueProducer.Op is "ldc.i4" or "ldc.i8")
        {
            // Check the format string (3rd arg)
            int formatProducerIdx = producers[fmtArgDepth];
            if (formatProducerIdx < 0) return;
            var formatProducer = instrs[formatProducerIdx];
            if (formatProducer.Op != "ldstr") return;

            long constValue = Convert.ToInt64(valueProducer.Operand);
            string formatStr = formatProducer.Operand?.ToString() ?? "G";
            string? result = null;

            if (string.Equals(formatStr, "G", StringComparison.OrdinalIgnoreCase) ||
                string.Equals(formatStr, "g", StringComparison.Ordinal))
            {
                foreach (var kv in fields)
                {
                    if (kv.Value == constValue) { result = kv.Key; break; }
                }
            }
            else if (string.Equals(formatStr, "D", StringComparison.OrdinalIgnoreCase) ||
                     string.Equals(formatStr, "d", StringComparison.Ordinal))
            {
                result = constValue.ToString();
            }
            else if (string.Equals(formatStr, "X", StringComparison.OrdinalIgnoreCase) ||
                     string.Equals(formatStr, "x", StringComparison.Ordinal))
            {
                result = constValue.ToString("X");
            }
            if (result != null)
            {
                _enumAotBakeMap[bakeKey] = new EnumAotBakeEntry(
                    enumTypeId, callee, ConstantStr: result, ConstantInt: null, ArgCount: 3, SkipIlOffsets: skipOffsets);
            }
            return;
        }

        // ── Enum.IsDefined(ldtoken<EnumType>, ldc.i4 value) → bool ─────
        if (isIsDefined && valueProducer.Op is "ldc.i4" or "ldc.i8")
        {
            long constValue = Convert.ToInt64(valueProducer.Operand);
            bool defined = false;
            foreach (var kv in fields)
            {
                if (kv.Value == constValue) { defined = true; break; }
            }
            _enumAotBakeMap[bakeKey] = new EnumAotBakeEntry(
                enumTypeId, callee, ConstantStr: null, ConstantInt: defined ? 1L : 0L, ArgCount: 2, SkipIlOffsets: skipOffsets);
        }
    }

}
