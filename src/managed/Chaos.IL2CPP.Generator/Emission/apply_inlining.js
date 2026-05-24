var fs = require('fs');
var filePath = 'D:/agent/booming-il2cpp/src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs';
var content = fs.readFileSync(filePath, 'utf8');

// Edit 1: Insert inlining check after ResolveDirectInvocationTarget
var marker1 = '\t\tInvocationTarget invocationTarget = ResolveDirectInvocationTarget(instruction);';
var insert1 = '\t\t// IL-level inlining: expand small callee bodies directly at call site.\n' +
    '\t\tif (invocationTarget.TargetSymbol != null && TryInlineAtCallSite(builder, instruction, invocationTarget, indentation))\n' +
    '\t\t\treturn;\n';
var idx1 = content.indexOf(marker1);
if (idx1 >= 0) {
    content = content.substring(0, idx1 + marker1.length) + '\n' + insert1 + content.substring(idx1 + marker1.length);
    console.log('Edit 1 OK');
} else { console.log('Edit 1 FAILED'); }

// Edit 2: Insert TryInlineResolvedMethod + TryInlineAtCallSite before EmitLinearCall
var snippet =
    '\t/// <summary>\n' +
    '\t/// Try to inline a resolved callee method at the call site.\n' +
    '\t/// Used by both direct calls and devirtualized callvirt.\n' +
    '\t/// </summary>\n' +
    '\tprivate bool TryInlineResolvedMethod(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string calleeSubjectId, int paramCount, string indentation)\n' +
    '\t{\n' +
    '\t\tif (!_methodsBySubjectId.TryGetValue(calleeSubjectId, out var calleeMethod)) return false;\n' +
    '\t\tif (calleeMethod.ExceptionRegionCount > 0) return false;\n' +
    '\t\tif (calleeMethod.IsPInvoke) return false;\n' +
    '\t\tif (_currentMethodArtifact == null) return false;\n' +
    '\t\tif (_currentMethodNativeSymbol == null) return false;\n' +
    '\n' +
    '\t\t// Budget check via InliningPlanner\n' +
    '\t\tbool isRecursive = string.Equals(calleeMethod.NativeSymbol, _currentMethodNativeSymbol, StringComparison.Ordinal);\n' +
    '\t\tvar candidate = InliningPlanner.EvaluateInline(calleeMethod.Instructions.Count, _currentMethodArtifact.Instructions.Count, isRecursive);\n' +
    '\t\tif (!candidate.CanInline) return false;\n' +
    '\n' +
    '\t\t// Single-BB restriction: no branches, switch, leave, starg, ldarga, ldloca\n' +
    '\t\tforeach (var ci in calleeMethod.Instructions)\n' +
    '\t\t{\n' +
    '\t\t\tif (ci.Op is \"br\" or \"brfalse\" or \"brtrue\" or \"beq\" or \"bne.un\"\n' +
    '\t\t\t    or \"bge\" or \"bge.un\" or \"bgt\" or \"bgt.un\" or \"ble\" or \"ble.un\"\n' +
    '\t\t\t    or \"blt\" or \"blt.un\" or \"switch\" or \"leave\" or \"endfilter\"\n' +
    '\t\t\t    or \"starg\" or \"ldarga\" or \"ldloca\")\n' +
    '\t\t\t\treturn false;\n' +
    '\t\t}\n' +
    '\n' +
    '\t\t// ---- EMIT INLINE BODY ----\n' +
    '\t\tbuilder.AppendLine($"{indentation}{{");\n' +
    '\t\tbuilder.AppendLine($\"{indentation}    // Inlined: {calleeMethod.SubjectId}\");\n' +
    '\n' +
    '\t\t// Consume arguments from eval stack into local C++ variables\n' +
    '\t\tfor (int i = paramCount - 1; i >= 0; i--)\n' +
    '\t\t{\n' +
    '\t\t\tstring argExpr = ConsumeEvalStackValueExpression();\n' +
    '\t\t\tbuilder.AppendLine($\"{indentation}    auto chaos_inline_arg_{i} = {argExpr};\");\n' +
    '\t\t}\n' +
    '\n' +
    '\t\tint localOffset = _currentMethodArtifact.LocalCount;\n' +
    '\n' +
    '\t\tforeach (var calleeInstruction in calleeMethod.Instructions)\n' +
    '\t\t{\n' +
    '\t\t\tif (calleeInstruction.Op == \"ret\") continue;\n' +
    '\n' +
    '\t\t\tif (calleeInstruction.Op == \"ldarg\")\n' +
    '\t\t\t{\n' +
    '\t\t\t\tint argIndex = GetRequiredIntOperand(calleeInstruction);\n' +
    '\t\t\t\tEmitEvalStackPush(builder, indentation + \"    \", $\"chaos_inline_arg_{argIndex}\");\n' +
    '\t\t\t\tcontinue;\n' +
    '\t\t\t}\n' +
    '\n' +
    '\t\t\tif (calleeInstruction.Op == \"ldloc\")\n' +
    '\t\t\t{\n' +
    '\t\t\t\tint localIndex = GetRequiredIntOperand(calleeInstruction);\n' +
    '\t\t\t\tEmitEvalStackPush(builder, indentation + \"    \", $\"chaos_locals[{localOffset + localIndex}]\");\n' +
    '\t\t\t\tcontinue;\n' +
    '\t\t\t}\n' +
    '\n' +
    '\t\t\tif (calleeInstruction.Op == \"stloc\")\n' +
    '\t\t\t{\n' +
    '\t\t\t\tstring valueExpr = ConsumeEvalStackValueExpression();\n' +
    '\t\t\t\tint localIndex = GetRequiredIntOperand(calleeInstruction);\n' +
    '\t\t\t\tbuilder.AppendLine($\"{indentation}    chaos_locals[{localOffset + localIndex}] = {valueExpr};\");\n' +
    '\t\t\t\tcontinue;\n' +
    '\t\t\t}\n' +
    '\n' +
    '\t\t\tEmitInstruction(builder, calleeInstruction, indentation + \"    \");\n' +
    '\t\t}\n' +
    '\n' +
    '\t\tbuilder.AppendLine($"{indentation}}}");\n' +
    '\t\treturn true;\n' +
    '\t}\n' +
    '\n' +
    '\t/// <summary>Try to inline the callee method identified by the invocation target.</summary>\n' +
    '\tprivate bool TryInlineAtCallSite(StringBuilder builder, AotCoreIrInstructionArtifact instruction, InvocationTarget invocationTarget, string indentation)\n' +
    '\t{\n' +
    '\t\tstring? calleeSubjectId = instruction.Callee ?? instruction.TargetReference?.SubjectId;\n' +
    '\t\tif (calleeSubjectId == null) return false;\n' +
    '\t\treturn TryInlineResolvedMethod(builder, instruction, calleeSubjectId, invocationTarget.ParameterAbis.Count, indentation);\n' +
    '\t}\n';

var marker2 = '\tprivate void EmitLinearCall(StringBuilder builder, AotCoreIrInstructionArtifact instruction, string indentation)';
var idx2 = content.indexOf(marker2);
if (idx2 >= 0) {
    content = content.substring(0, idx2) + snippet + content.substring(idx2);
    console.log('Edit 2 OK');
} else { console.log('Edit 2 FAILED'); }

// Edit 3: Insert inlining check in guard-based devirtualization path
var guardMarker = 'string guardStableIdExpr = GetNativeTypeIdSymbol(devirtHint.GuardTypeSubjectId);';
var guardIdx = content.indexOf(guardMarker);
if (guardIdx >= 0) {
    var voidCheck = '\t\t\t\tif (string.Equals(devirtRet, "void", StringComparison.Ordinal))';
    var voidIdx = content.indexOf(voidCheck, guardIdx);
    if (voidIdx >= 0) {
        var insert3 = '\t\t\t\t// Try inlining the devirtualized method\n' +
            '\t\t\t\tif (TryInlineResolvedMethod(builder, instruction, devirtHint.ImplementationMethodSubjectId, devirtParams.Count, indentation))\n' +
            '\t\t\t\t\treturn;\n';
        content = content.substring(0, voidIdx) + insert3 + content.substring(voidIdx);
        console.log('Edit 3 OK');
    } else { console.log('Edit 3 FAILED'); }
} else { console.log('Edit 3 FAILED: guardMarker'); }

// Edit 4: Insert inlining check in sealed/monomorphic path
var sealedMarker = '// Sealed/monomorphic: unconditional direct call';
var sealedIdx = content.indexOf(sealedMarker);
if (sealedIdx >= 0) {
    var voidCheck2 = '\t\t\t\tif (string.Equals(devirtRet, "void", StringComparison.Ordinal))';
    var voidIdx2 = content.indexOf(voidCheck2, sealedIdx);
    if (voidIdx2 >= 0) {
        var insert4 = '\t\t\t\t// Try inlining the devirtualized method (sealed/monomorphic path)\n' +
            '\t\t\t\tif (TryInlineResolvedMethod(builder, instruction, devirtHint.ImplementationMethodSubjectId, devirtParams.Count, indentation))\n' +
            '\t\t\t\t\treturn;\n';
        content = content.substring(0, voidIdx2) + insert4 + content.substring(voidIdx2);
        console.log('Edit 4 OK');
    } else { console.log('Edit 4 FAILED'); }
} else { console.log('Edit 4 FAILED: sealedMarker'); }

// Edit 5: Insert inlining check BEFORE hotpatch check (3 tabs)
var hotpatchCheck = '\t\t\tif (_nativeSymbolToDispatchSlot?.TryGetValue(devirtSymbol, out int devirtSlot) == true)';
var hpIdx = content.indexOf(hotpatchCheck);
if (hpIdx >= 0) {
    var insert5 = '\t\t\t// Try inlining before hotpatch — skip hotpatch entirely if inlined.\n' +
        '\t\t\tif (TryInlineResolvedMethod(builder, instruction, devirtHint.ImplementationMethodSubjectId, GetMethodAbiParameterSlots(devirtMethod).Count, indentation))\n' +
        '\t\t\t\treturn;\n\n';
    content = content.substring(0, hpIdx) + insert5 + content.substring(hpIdx);
    console.log('Edit 5 OK');
} else { console.log('Edit 5 FAILED: hotpatch check not found'); }

fs.writeFileSync(filePath, content, 'utf8');
console.log('All edits done');
