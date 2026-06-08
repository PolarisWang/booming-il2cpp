"""Fix EmitExternalRuntimeTableDispatch signature and null check"""
import sys
sys.dont_write_bytecode = True

fpath = r"src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs"
with open(fpath, 'r', encoding='utf-8') as f:
    content = f.read()

# 1. Add instruction param to method signature
old_sig = ("private void EmitExternalRuntimeTableDispatch("
           "StringBuilder builder, InvocationTarget invocationTarget, "
           "string indentation, bool enforceInstanceNullCheck)")
new_sig = ("private void EmitExternalRuntimeTableDispatch("
           "StringBuilder builder, InvocationTarget invocationTarget, "
           "string indentation, bool enforceInstanceNullCheck, "
           "AotCoreIrInstructionArtifact? instruction = null)")
assert old_sig in content, "Signature not found!"
content = content.replace(old_sig, new_sig, 1)
print("1. Signature updated")

# 2. Update the null check to handle callvirt/call with no ParameterAbis
old_null = ["\t\tif (enforceInstanceNullCheck && invocationTarget.ParameterAbis.Count > 0)",
            "\t\t{",
            '\t\t    builder.AppendLine(indentation + "    if (chaos_arg_0 == 0)");',
            '\t\t    builder.AppendLine(indentation + "    {");',
            '\t\t    builder.AppendLine(indentation + "        ::chaos::il2cpp::runtime_core::RaiseNullReferenceException();");',
            '\t\t    builder.AppendLine(indentation + "    }");',
            "\t\t}"]

new_null_block = """\t\tif (enforceInstanceNullCheck && invocationTarget.ParameterAbis.Count > 0)
\t\t{
\t\t    builder.AppendLine(indentation + "    if (chaos_arg_0 == 0)");
\t\t    builder.AppendLine(indentation + "    {");
\t\t    builder.AppendLine(indentation + "        ::chaos::il2cpp::runtime_core::RaiseNullReferenceException();");
\t\t    builder.AppendLine(indentation + "    }");
\t\t}
\t\telse if (enforceInstanceNullCheck && instruction?.Op is "callvirt" or "call")
\t\t{
\t\t    // External runtime dispatch with no DirectNativeSymbol: the stub takes
\t\t    // void() args so ParameterAbis is empty.  Pop 'this' from eval stack
\t\t    // and check for null before calling the sentinel stub.
\t\t    var nullThisExpr = ConsumeEvalStackValueExpression();
\t\t    builder.AppendLine($"{{indentation}}    if (auto chaos_null_this = {nullThisExpr})");
\t\t    builder.AppendLine($"{{indentation}}    {{")
"""

old_null_str = "\n".join(old_null)
assert old_null_str in content, "Null check block not found!"
content = content.replace(old_null_str, new_null_block, 1)
print("2. Null check updated")

# 3. Add closing brace for the callvirt block before FormatAbiInvocationArgumentList
#    The dispatch code follows right after; wrap it in a block opened by the 'if' above
old_args = "\t\t\t\tstring args = FormatAbiInvocationArgumentList(invocationTarget.ParameterAbis);"
# Only add the closing brace the FIRST time this line appears (it's in the table-dispatch section)
lines = content.split('\n')
for i, line in enumerate(lines):
    stripped = line.strip()
    if stripped == 'string args = FormatAbiInvocationArgumentList(invocationTarget.ParameterAbis);':
        indent = line[:len(line) - len(line.lstrip())]
        lines[i] = indent + '}\n' + indent + stripped
        break
content = '\n'.join(lines)
print("3. Closing brace added")

with open(fpath, 'w', encoding='utf-8') as f:
    f.write(content)
print("Done!")
