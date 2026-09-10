"""Fix the else-if block in EmitExternalRuntimeTableDispatch"""
import sys
sys.dont_write_bytecode = True

fpath = r"src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs"
with open(fpath, 'r', encoding='utf-8') as f:
    content = f.read()

# Find the exact text block to replace
import re

# Pattern: the else-if body between "var nullThisExpr" and "string args"
old_pattern = (
    r'var nullThisExpr = ConsumeEvalStackValueExpression\(\);\n'
    r'\t\t    builder\.AppendLine\(\$"\{\{indentation\}\}    if \(auto chaos_null_this = \{\s*nullThisExpr\s*\})"\);\n'
    r'\t\t    builder\.AppendLine\(\$"\{\{indentation\}\}    \{\{"\);\n'
    r'\t\t\n'
    r'\t\t}'
)

new_text = (
    '\t\t    var nullThisExpr = ConsumeEvalStackValueExpression();\n'
    '\t\t    builder.AppendLine(indentation + "    if (" + nullThisExpr + " == 0)");\n'
    '\t\t    builder.AppendLine(indentation + "    {");\n'
    '\t\t    builder.AppendLine(indentation + "        ::chaos::il2cpp::runtime_core::RaiseNullReferenceException();");\n'
    '\t\t    builder.AppendLine(indentation + "    }");\n'
    '\t\t}'
)

if re.search(old_pattern, content):
    content = re.sub(old_pattern, new_text, content, count=1)
    with open(fpath, 'w', encoding='utf-8') as f:
        f.write(content)
    print("Fixed!")
else:
    # Debug: show what's around ConsumeEvalStackValueExpression
    idx = content.find('ConsumeEvalStackValueExpression();')
    chunk = content[idx-50:idx+300]
    print(f"Pattern not matched.\nContext:\n{repr(chunk)}")
    sys.exit(1)
