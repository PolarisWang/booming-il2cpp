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



    /// <summary>
    /// Estimates how many arguments a call instruction pops from the eval stack.
    /// Uses the callee signature when available; falls back to conservative default.
    /// </summary>



    /// <summary>
    /// For a <c>call</c>/<c>callvirt</c> to an enum static method, check if the
    /// type argument was produced by <c>ldtoken &lt;EnumType&gt;</c>. If so, record
    /// the call site for AOT-baked specialized code emission.
    /// </summary>



    // ── A2.6: TypeInfo* direct API pre-scan ─────────────────────────────

    /// <summary>
    /// Pre-scan all methods' IR instructions to detect
    /// <c>typeof(T).IsAssignableFrom(typeof(U))</c> patterns where T and U are
    /// AOT-known types.  Records fold entries that let the emitter bypass
    /// <c>ChaosReflectionGetTypeFromHandle</c> and emit
    /// <c>ChaosReflectionIsAssignableFromPtr(...)</c> directly.
    /// </summary>



    // ── A2.7: typeof(T) compile-time fold pre-scan ─────────────────────────

    /// <summary>
    /// Pre-scan all methods' IR instructions to detect
    /// <c>ldtoken &lt;const_type&gt; + call GetTypeFromHandle</c> patterns where the
    /// type is AOT-known.  Records fold entries that let the emitter bypass
    /// <c>ChaosReflectionGetTypeFromHandle</c> and emit a direct TypeInfo*
    /// pointer expression.
    /// </summary>



    /// <summary>
    /// all type arguments are <c>typeof()</c> constants (ldtoken + GetTypeFromHandle).
    /// If so, produce a <see cref="TypeHierarchyPtrFoldEntry"/> that the emitter
    /// uses to emit the <c>*Ptr</c> direct API call with pre-resolved symbols.
    /// </summary>



    /// <summary>
    /// Check if <c>instrs[idx]</c> is <c>call Type::GetTypeFromHandle</c> and
    /// <c>instrs[idx-1]</c> is <c>ldtoken</c>.  If so, extract the type SubjectId.
    /// </summary>



    /// <summary>
    /// Extract the method name from a callee SubjectId like
    /// <c>"System.Type::IsAssignableFrom"</c> or
    /// <c>"System.Private.CoreLib/System.Type::IsAssignableFrom:Boolean(System.Type)"</c>.
    /// Returns the method name only if the declaring type is <c>System.Type</c>.
    /// </summary>



    /// <summary>
    /// Check if a callee SubjectId represents <c>System.Type::GetTypeFromHandle</c>.
    /// </summary>



    /// <summary>
    /// Check if a type SubjectId is AOT-known (has a <c>chaos_mt_*</c> symbol).
    /// </summary>



    /// <summary>
    /// For Enum.Format with a non-enum type (e.g. byte) and a known constant value
    /// with D/X/G format specifier, bake the result string at compile time.
    /// Non-enum types are not in _enumTypeSubjectIds so the main bake logic skips them.
    /// </summary>

}
