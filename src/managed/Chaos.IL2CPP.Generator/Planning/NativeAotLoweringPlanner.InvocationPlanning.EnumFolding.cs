using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{

    /// <summary>
    /// Pre-scan all methods' IR instructions to detect foldable
    /// <c>ldsfld &lt;literal_enum_field&gt;</c> → <c>call Enum::ToString()</c> patterns.
    /// Populates <see cref="_enumToStringFoldMap"/> with call-site IlOffset → field name.
    /// </summary>
    private void BuildEnumToStringFoldTable(IReadOnlyList<AotCoreIrMethodArtifact> methodsForLowering)
    {
        _enumToStringFoldMap.Clear();

        if (_reflectionMemberSupport.FieldEntries.Count == 0)
            return;

        // Build per-type field name index for enum types
        var enumFieldNamesByType = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);
        foreach (var field in _reflectionMemberSupport.FieldEntries)
        {
            if (!_enumTypeSubjectIds.Contains(field.DeclaringTypeSubjectId))
                continue;
            if (!enumFieldNamesByType.TryGetValue(field.DeclaringTypeSubjectId, out var names))
            {
                names = new HashSet<string>(StringComparer.Ordinal);
                enumFieldNamesByType[field.DeclaringTypeSubjectId] = names;
            }
            names.Add(field.FieldName);
        }
        if (enumFieldNamesByType.Count == 0)
            return;

        foreach (var method in methodsForLowering)
        {
            var instrs = method.Instructions;
            if (instrs.Count == 0) continue;

            // Simulated eval stack: track which instruction index produced each slot
            // Some methods (particularly large SIMD/vectorized code) can have very
            // deep eval stacks.  Use a generous fixed-size buffer; skip methods whose
            // eval stack exceeds it (these are unlikely to be simple enum→ToString).
            var producers = new int[2048];
            int depth = 0;

            for (int i = 0; i < instrs.Count; i++)
            {
                var instr = instrs[i];

                switch (instr.Op)
                {
                    case "ldsfld":
                        if (depth >= producers.Length) return;
                        producers[depth++] = i;
                        break;

                    case "ldc.i4":
                    case "ldc.i8":
                        if (depth >= producers.Length) return;
                        producers[depth++] = i;
                        break;

                    case "ldarg":
                    case "ldloc":
                    case "ldnull":
                    case "ldstr":
                    case "ldfld":
                    case "ldtoken":
                    case "ldelema":
                    case "ldlen":
                        depth++;
                        break;

                    case "stloc":
                    case "pop":
                    case "stsfld":
                    case "stfld":
                    case "stind.i":
                    case "stind.i8":
                    case "stind.i4":
                    case "stind.ref":
                        if (depth > 0) depth--;
                        break;

                    case "box":
                        // box reinterprets the value type as a reference; the underlying
                        // value (and therefore the producer instruction) is unchanged.
                        break;

                    case "dup":
                        if (depth > 0)
                        {
                            producers[depth] = producers[depth - 1];
                            depth++;
                        }
                        break;

                    case "call":
                    case "callvirt":
                        TryFoldEnumToStringCall(instr, i, instrs, producers, depth, enumFieldNamesByType);
                        // Conservative depth: pop 'this' (for instance) or 0, push result
                        if (depth > 0) depth--;
                        depth++;
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
                        else if (instr.Op.StartsWith("st"))
                        {
                            if (depth > 0) depth--;
                        }
                        else if (instr.Op.StartsWith("ld"))
                        {
                            depth++;
                        }
                        else if (instr.Op is "add" or "sub" or "mul" or "div" or "rem"
                            or "and" or "or" or "xor" or "shl" or "shr"
                            or "ceq" or "cgt" or "clt")
                        {
                            if (depth > 1) depth--; // pop 2, push 1
                        }
                        break;
                }
            }
        }
    }



    /// <summary>
    /// For a <c>call</c>/<c>callvirt</c> instruction, check if it targets
    /// <c>System.Enum::ToString()</c> and the <c>this</c> argument was produced by
    /// an <c>ldsfld</c> of a literal enum field. If so, record the fold in
    /// <see cref="_enumToStringFoldMap"/>.
    /// </summary>
    private void TryFoldEnumToStringCall(
        AotCoreIrInstructionArtifact callInstr,
        int callIndex,
        IReadOnlyList<AotCoreIrInstructionArtifact> instrs,
        int[] producers,
        int depth,
        Dictionary<string, HashSet<string>> enumFieldNamesByType)
    {
        var callee = callInstr.Callee;
        if (callee == null || !callee.Contains("::ToString:System.String()", StringComparison.Ordinal))
            return;

        int thisDepth = depth - 1;
        if (thisDepth < 0) return;

        int producerIdx = producers[thisDepth];
        if (producerIdx < 0 || producerIdx >= callIndex) return;

        var producer = instrs[producerIdx];

        // Case 1: ldsfld EnumType::FieldName → callvirt ToString (static field ref)
        // e.g. DayOfWeek.Monday.ToString() where producer = ldsfld DayOfWeek::Monday
        if (producer.Op == "ldsfld")
        {
            var fieldSubjectId = producer.TargetReference?.SubjectId;
            if (fieldSubjectId == null) return;

            var colonIdx = fieldSubjectId.LastIndexOf("::", StringComparison.Ordinal);
            if (colonIdx <= 0) return;

            var declaringType = fieldSubjectId.Substring(0, colonIdx);
            var fieldName = fieldSubjectId.Substring(colonIdx + 2);

            if (enumFieldNamesByType.TryGetValue(declaringType, out var validNames) &&
                validNames.Contains(fieldName))
            {
                _enumToStringFoldMap[callInstr.IlOffset] = fieldName;
            }
            return;
        }

        // Case 2: ldc.i4 value + box EnumType → callvirt ToString (BoxToString)
        // e.g. ((Enum)DayOfWeek.Monday).ToString() — producer is ldc.i4(1),
        // and a preceding box instruction carries the enum type.
        if (producer.Op is "ldc.i4" or "ldc.i8")
        {
            long constValue = Convert.ToInt64(producer.Operand);

            // Scan backward from callIndex for a box <EnumType> instruction
            for (int j = callIndex - 1; j > producerIdx; j--)
            {
                if (instrs[j].Op != "box") continue;
                var boxType = instrs[j].TargetReference?.SubjectId;
                if (boxType == null || !enumFieldNamesByType.ContainsKey(boxType))
                    continue;

                // Look up field name by constant value
                foreach (var field in _reflectionMemberSupport.FieldEntries)
                {
                    if (field.DeclaringTypeSubjectId == boxType &&
                        field.ConstantValue.HasValue &&
                        field.ConstantValue.Value == constValue)
                    {
                        _enumToStringFoldMap[callInstr.IlOffset] = field.FieldName;
                        return;
                    }
                }
            }
            return;
        }
    }



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



    // ── A2.6: TypeInfo* direct API pre-scan ─────────────────────────────

    /// <summary>
    /// Pre-scan all methods' IR instructions to detect
    /// <c>typeof(T).IsAssignableFrom(typeof(U))</c> patterns where T and U are
    /// AOT-known types.  Records fold entries that let the emitter bypass
    /// <c>ChaosReflectionGetTypeFromHandle</c> and emit
    /// <c>ChaosReflectionIsAssignableFromPtr(...)</c> directly.
    /// </summary>
    private void BuildTypeHierarchyPtrFoldTable(IReadOnlyList<AotCoreIrMethodArtifact> methodsForLowering)
    {
        _typeHierarchyPtrFoldMap.Clear();
        _typeHierarchyPtrSkipIlOffsets.Clear();
        _preTryFoldInitializers = null;
        if (_allEmittedTypeSubjectIds is not { Count: > 0 })
            return;
        int totalCalls = 0, matchedCalls = 0;

        foreach (var method in methodsForLowering)
        {
            var instrs = method.Instructions;
            if (instrs is null) continue;

            for (int i = 0; i < instrs.Count; i++)
            {
                var instr = instrs[i];
                if (instr.Op is not ("call" or "callvirt"))
                    continue;

                var callee = instr.Callee;
                if (string.IsNullOrEmpty(callee)) continue;

                totalCalls++;

                if (TryFoldTypeHierarchyPtrCall(instrs, i, callee, out var entry))
                {
                    _typeHierarchyPtrFoldMap[(method.NativeSymbol, instr.IlOffset)] = entry;
                    // Populate skip offsets map for DCE in EmitInstruction
                    if (entry.SkipIlOffsets.Length > 0)
                    {
                        if (!_typeHierarchyPtrSkipIlOffsets.TryGetValue(method.NativeSymbol, out var set))
                        {
                            set = new HashSet<int>();
                            _typeHierarchyPtrSkipIlOffsets[method.NativeSymbol] = set;
                        }
                        foreach (var offset in entry.SkipIlOffsets)
                            set.Add(offset);
                    }
                    matchedCalls++;
                }
            }
        }
    }



    // ── A2.7: typeof(T) compile-time fold pre-scan ─────────────────────────

    /// <summary>
    /// Pre-scan all methods' IR instructions to detect
    /// <c>ldtoken &lt;const_type&gt; + call GetTypeFromHandle</c> patterns where the
    /// type is AOT-known.  Records fold entries that let the emitter bypass
    /// <c>ChaosReflectionGetTypeFromHandle</c> and emit a direct TypeInfo*
    /// pointer expression.
    /// </summary>
    private void BuildTypeOfFoldTable(IReadOnlyList<AotCoreIrMethodArtifact> methodsForLowering)
    {
        _typeOfFoldMap.Clear();
        _typeOfSkipIlOffsets.Clear();
        if (_allEmittedTypeSubjectIds is not { Count: > 0 })
            return;
        int totalCalls = 0, matchedCalls = 0;

        foreach (var method in methodsForLowering)
        {
            var instrs = method.Instructions;
            if (instrs is null) continue;

            for (int i = 1; i < instrs.Count; i++)
            {
                var instr = instrs[i];
                if (instr.Op is not ("call" or "callvirt"))
                    continue;

                if (!IsGetTypeFromHandle(instr.Callee))
                    continue;

                totalCalls++;

                // Check preceding instruction: must be ldtoken <type>
                var prev = instrs[i - 1];
                if (prev.Op != "ldtoken")
                    continue;

                string? subjectId = prev.TargetReference?.SubjectId;
                if (string.IsNullOrEmpty(subjectId))
                    continue;

                if (prev.TargetReference?.Kind != AotCoreIrReferenceKind.Type)
                    continue;

                if (!IsTypeAotKnown(subjectId))
                    continue;

                matchedCalls++;

                var typeInfoExpr = $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>({GetNativeTypeInfoSymbol(subjectId)})";

                var entry = new TypeOfFoldEntry(
                    typeInfoExpr,
                    new[] { prev.IlOffset }); // skip ltoken

                _typeOfFoldMap[(method.NativeSymbol, instr.IlOffset)] = entry;

                // Populate skip offsets for DCE of the ltoken instruction
                if (!_typeOfSkipIlOffsets.TryGetValue(method.NativeSymbol, out var set))
                {
                    set = new HashSet<int>();
                    _typeOfSkipIlOffsets[method.NativeSymbol] = set;
                }
                set.Add(prev.IlOffset);
            }
        }
    }



    /// <summary>
    /// all type arguments are <c>typeof()</c> constants (ldtoken + GetTypeFromHandle).
    /// If so, produce a <see cref="TypeHierarchyPtrFoldEntry"/> that the emitter
    /// uses to emit the <c>*Ptr</c> direct API call with pre-resolved symbols.
    /// </summary>
    private bool TryFoldTypeHierarchyPtrCall(
        IReadOnlyList<AotCoreIrInstructionArtifact> instrs,
        int callIndex,
        string callee,
        out TypeHierarchyPtrFoldEntry entry)
    {
        entry = default;

        var methodName = ExtractTypeHierarchyMethodName(callee);
        if (methodName is null)
            return false;

        if (!TypeHierarchyPtrOptimizationMap.TryGetValue(methodName, out var ptrFuncName))
            return false;

        switch (methodName)
        {
            case "IsAssignableFrom":
            case "IsSubclassOf":
            case "IsAssignableTo":
                if (callIndex < 4)
                    return false;

                if (!TryGetLdTokenSubjectId(instrs, callIndex - 1, out var subjectId2))
                    return false;
                if (!TryGetLdTokenSubjectId(instrs, callIndex - 3, out var subjectId1))
                    return false;

                if (!IsTypeAotKnown(subjectId1!) || !IsTypeAotKnown(subjectId2!))
                    return false;

                // Collect IlOffsets of dead instructions for DCE
                var skipOffsets2 = new[]
                {
                    instrs[callIndex - 4].IlOffset, // ltoken type1
                    instrs[callIndex - 3].IlOffset, // call GetTypeFromHandle(type1)
                    instrs[callIndex - 2].IlOffset, // ltoken type2
                    instrs[callIndex - 1].IlOffset, // call GetTypeFromHandle(type2)
                };

                entry = new TypeHierarchyPtrFoldEntry(
                    ptrFuncName,
                    GetNativeTypeInfoSymbol(subjectId1),
                    GetNativeTypeInfoSymbol(subjectId2),
                    skipOffsets2);
                return true;

            case "IsInstanceOfType":
                if (callIndex < 3) return false;

                if (!TryGetLdTokenSubjectId(instrs, callIndex - 2, out var typeSubjectId)) return false;
                if (!IsTypeAotKnown(typeSubjectId!)) return false;

                // SAFETY: if the object argument is a ldstr (produces CHAOS_IL2CPP_STRING_ID,
                // a bit-63 tagged integer), we cannot fold because
                // ChaosReflectionIsInstanceOfTypePtr calls chaos_object_get_type_info on the
                // object pointer, which would segfault on a tagged integer.  The non-optimized
                // virtual dispatch path handles StringId correctly through the managed
                // Type::IsInstanceOfType implementation.
                if (callIndex >= 1 && instrs[callIndex - 1].Op == "ldstr")
                    return false;

                // Collect IlOffsets of dead instructions (ltoken + GetTypeFromHandle for the type arg)
                var skipOffsets1 = new[]
                {
                    instrs[callIndex - 2].IlOffset, // ltoken type
                    instrs[callIndex - 1].IlOffset, // call GetTypeFromHandle(type)
                };

                entry = new TypeHierarchyPtrFoldEntry(
                    ptrFuncName,
                    GetNativeTypeInfoSymbol(typeSubjectId),
                    TypeExpr2: null,
                    skipOffsets1);
                return true;
        }

        return false;
    }



    /// <summary>
    /// Check if <c>instrs[idx]</c> is <c>call Type::GetTypeFromHandle</c> and
    /// <c>instrs[idx-1]</c> is <c>ldtoken</c>.  If so, extract the type SubjectId.
    /// </summary>
    private static bool TryGetLdTokenSubjectId(
        IReadOnlyList<AotCoreIrInstructionArtifact> instrs,
        int idx,
        out string? subjectId)
    {
        subjectId = null;
        if (idx < 1)
            return false;

        // Check: call Type::GetTypeFromHandle
        var callInstr = instrs[idx];
        if (callInstr.OpCode is not (InstructionOpCode.Call or InstructionOpCode.CallVirt))
            return false;

        if (!IsGetTypeFromHandle(callInstr.Callee))
            return false;

        // Check: ltoken <type>
        var ldtokenInstr = instrs[idx - 1];
        if (ldtokenInstr.Op != "ldtoken")
            return false;

        subjectId = ldtokenInstr.TargetReference?.SubjectId;
        if (string.IsNullOrEmpty(subjectId) && ldtokenInstr.Operand is string directId)
            subjectId = directId;

        return !string.IsNullOrEmpty(subjectId);
    }



    /// <summary>
    /// Extract the method name from a callee SubjectId like
    /// <c>"System.Type::IsAssignableFrom"</c> or
    /// <c>"System.Private.CoreLib/System.Type::IsAssignableFrom:Boolean(System.Type)"</c>.
    /// Returns the method name only if the declaring type is <c>System.Type</c>.
    /// </summary>
    private static string? ExtractTypeHierarchyMethodName(string callee)
    {
        if (string.IsNullOrEmpty(callee))
            return null;

        var doubleColon = callee.LastIndexOf("::", StringComparison.Ordinal);
        if (doubleColon < 0)
            return null;

        // Check declaring type is System.Type
        var typePart = callee.Substring(0, doubleColon);
        var slashIdx = typePart.LastIndexOf('/');
        var typeName = slashIdx >= 0 ? typePart.Substring(slashIdx + 1) : typePart;
        if (typeName != "System.Type")
            return null;

        // Get method name (strip optional ":retType(params)" suffix)
        var rest = callee.Substring(doubleColon + 2);
        var colonIdx = rest.IndexOf(':');
        var methodName = colonIdx >= 0 ? rest.Substring(0, colonIdx) : rest;

        return methodName;
    }



    /// <summary>
    /// Check if a callee SubjectId represents <c>System.Type::GetTypeFromHandle</c>.
    /// </summary>
    private static bool IsGetTypeFromHandle(string? callee)
    {
        if (string.IsNullOrEmpty(callee))
            return false;

        var doubleColon = callee.LastIndexOf("::", StringComparison.Ordinal);
        if (doubleColon < 0)
            return false;

        // Check declaring type is System.Type
        var typePart = callee.Substring(0, doubleColon);
        var slashIdx = typePart.LastIndexOf('/');
        var typeName = slashIdx >= 0 ? typePart.Substring(slashIdx + 1) : typePart;
        if (typeName != "System.Type")
            return false;

        // Get method name
        var rest = callee.Substring(doubleColon + 2);
        var colonIdx = rest.IndexOf(':');
        var methodName = colonIdx >= 0 ? rest.Substring(0, colonIdx) : rest;

        return methodName == "GetTypeFromHandle";
    }



    /// <summary>
    /// Check if a type SubjectId is AOT-known (has a <c>chaos_mt_*</c> symbol).
    /// </summary>
    private bool IsTypeAotKnown(string subjectId)
    {
        return _allEmittedTypeSubjectIds?.Contains(subjectId) == true;
    }



    /// <summary>
    /// For Enum.Format with a non-enum type (e.g. byte) and a known constant value
    /// with D/X/G format specifier, bake the result string at compile time.
    /// Non-enum types are not in _enumTypeSubjectIds so the main bake logic skips them.
    /// </summary>
    private void TryRecordNonEnumFormatBake(
        string callee, string? methodId, int callIndex,
        IReadOnlyList<AotCoreIrInstructionArtifact> instrs,
        int typeProducerIdx, int[] producers, int depth, string enumTypeId)
    {
        // Stack layout before Format(Type, Object, String) call:
        //   stack[depth-3] = Type   (0th arg)
        //   stack[depth-2] = Object (1st arg = value)
        //   stack[depth-1] = String (2nd arg = format)
        int valueArgDepthNonEnum = depth - 2;
        int formatArgDepth = depth - 1;
        if (valueArgDepthNonEnum < 0 || formatArgDepth < 0) return;

        int valueProdIdx = producers[valueArgDepthNonEnum];
        int formatProdIdx = producers[formatArgDepth];
        if (valueProdIdx < 0 || formatProdIdx < 0) return;

        var valueProducer = instrs[valueProdIdx];
        var formatProducer = instrs[formatProdIdx];
        if ((valueProducer.Op is not "ldc.i4" and not "ldc.i8") || formatProducer.Op != "ldstr")
            return;

        string formatStr = formatProducer.Operand?.ToString() ?? "G";
        long constValue = Convert.ToInt64(valueProducer.Operand);
        string? result = null;

        if (string.Equals(formatStr, "G", StringComparison.OrdinalIgnoreCase) ||
            string.Equals(formatStr, "D", StringComparison.OrdinalIgnoreCase))
        {
            result = constValue.ToString();
        }
        else if (string.Equals(formatStr, "X", StringComparison.OrdinalIgnoreCase))
        {
            result = constValue.ToString("X");
        }

        if (result == null) return;

        var bakeKey = (methodId ?? "", instrs[callIndex].IlOffset);
        var skipOffsets = CollectEnumAotBakeSkipOffsets(instrs, typeProducerIdx, valueProdIdx, callIndex);
        _enumAotBakeMap[bakeKey] = new EnumAotBakeEntry(
            enumTypeId, callee, ConstantStr: result, ConstantInt: null, ArgCount: 3, SkipIlOffsets: skipOffsets);
        PopulateEnumAotBakeSkipIlOffsets(methodId, skipOffsets);
    }



    private static int[] CollectEnumAotBakeSkipOffsets(
        IReadOnlyList<AotCoreIrInstructionArtifact> instrs,
        int typeProducerIdx, int valueProdIdx, int callIndex)
    {
        // Collect IL offsets of the producers that get folded (type + value)
        // and the Format call itself.
        var offsets = new List<int>();
        if (typeProducerIdx >= 0 && typeProducerIdx < instrs.Count)
            offsets.Add(instrs[typeProducerIdx].IlOffset);
        if (valueProdIdx >= 0 && valueProdIdx < instrs.Count)
            offsets.Add(instrs[valueProdIdx].IlOffset);
        if (callIndex >= 0 && callIndex < instrs.Count)
            offsets.Add(instrs[callIndex].IlOffset);
        return offsets.ToArray();
    }



    private void PopulateEnumAotBakeSkipIlOffsets(string? methodId, int[] skipOffsets)
    {
        if (string.IsNullOrEmpty(methodId) || skipOffsets.Length == 0) return;
        if (!_enumAotBakeSkipIlOffsets.TryGetValue(methodId, out var set))
        {
            set = new HashSet<int>();
            _enumAotBakeSkipIlOffsets[methodId] = set;
        }
        foreach (var offset in skipOffsets)
            set.Add(offset);
    }

}
