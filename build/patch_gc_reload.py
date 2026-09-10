# Patch GC-safe reload in indirect dispatch path of EmitExternalRuntimeTableDispatch
import sys

with open(sys.argv[1], 'r', encoding='utf-8') as f:
    content = f.read()

# Line 3359-3360: 3 tabs indent
old = '\t\t\tstring rawExpr = ConsumeEvalStackValueExpression();\n\t\t\tif (invocationTarget.RawArgumentIndices.Contains(i))'

new = '\t\t\tstring rawExpr = ConsumeEvalStackValueExpression();\n\t\t\tif (_activeStructuredSlotContext is { } ctx)\n\t\t\t{\n\t\t\t\tstring? reload = ctx.GetChaosLocalReload(rawExpr);\n\t\t\t\tif (reload != null)\n\t\t\t\t\tbuilder.AppendLine($"{indentation}    {reload}");\n\t\t\t}\n\t\t\tif (invocationTarget.RawArgumentIndices.Contains(i))'

count = content.count(old)
if count == 0:
    print('ERROR: old string not found')
    # Debug: show what's around line 3359
    import re
    for m in re.finditer(r'string rawExpr = ConsumeEvalStackValueExpression', content):
        pos = m.start()
        # Check the context right after
        after = content[pos:pos+200]
        # Check if next non-comment line has RawArgumentIndices
        print(f'Found at {pos}: {repr(after[:80])}')
    sys.exit(1)

print(f'Found {count} occurrence(s), replacing first')
content = content.replace(old, new, 1)
with open(sys.argv[1], 'w', encoding='utf-8') as f:
    f.write(content)
print('OK: second site patched')
