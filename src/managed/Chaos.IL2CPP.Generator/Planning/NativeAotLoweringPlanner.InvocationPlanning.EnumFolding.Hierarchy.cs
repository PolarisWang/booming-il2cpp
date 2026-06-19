using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{



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
