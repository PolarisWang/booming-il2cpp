"""Fix the else-if block using string replace"""
import sys
sys.dont_write_bytecode = True

fpath = r"src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs"
with open(fpath, 'r', encoding='utf-8') as f:
    content = f.read()

old = 'var nullThisExpr = ConsumeEvalStackValueExpression();'
new = '''\t\t    var nullThisExpr = ConsumeEvalStackValueExpression();
\t\t    builder.AppendLine(indentation + "    if (" + nullThisExpr + " == 0)");
\t\t    builder.AppendLine(indentation + "    {");
\t\t    builder.AppendLine(indentation + "        ::chaos::il2cpp::runtime_core::RaiseNullReferenceException();");
\t\t    builder.AppendLine(indentation + "    }");'''

# Find the old code and the following garbage lines to remove
old_start = content.find(old)
if old_start < 0:
    print("Old code not found!")
    sys.exit(1)

# Find the end of the block - look for '\t\t}\n\t\tstring args'
block_end = content.find('\t\tstring args', old_start)
if block_end < 0:
    print("Block end not found!")
    sys.exit(1)

# The block to replace: from 'var nullThisExpr' up to and including '\t\t}\n'
old_block = content[old_start:block_end]
print(f"Old block ({len(old_block)} chars): {repr(old_block[:100])}...")

# The new block: the proper null check + the closing brace
new_block = new + '\n\t\t}'

content = content[:old_start] + new_block + content[block_end:]
with open(fpath, 'w', encoding='utf-8') as f:
    f.write(content)
print("Fixed!")
