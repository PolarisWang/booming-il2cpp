import sys

with open(sys.argv[1], 'r', encoding='utf-8') as f:
    content = f.read()

count = 0

# 1. Patch EmitHotpatchResolvedInvocation
old1 = '\t\t\tstring rawExpr = ConsumeEvalStackValueExpression();\n\t\t\tif (rawArgumentIndices.Contains(i))'
# Insert GC-safe reload after ConsumeEvalStackValueExpression
new1_lines = [
    '\t\t\tstring rawExpr = ConsumeEvalStackValueExpression();',
    '\t\t\tif (_activeStructuredSlotContext is { } ctx)',
    '\t\t\t{',
    '\t\t\t\tstring? reload = ctx.GetChaosLocalReload(rawExpr);',
    '\t\t\t\tif (reload != null)',
    '\t\t\t\t\tbuilder.AppendLine($"{indentation}    {reload}");',
    '\t\t\t}',
    '\t\t\tif (rawArgumentIndices.Contains(i))',
]
new1 = '\n'.join(new1_lines)
c1 = content.count(old1)
if c1 > 0:
    content = content.replace(old1, new1, 1)
    count += 1
    print(f'Hompatch: found {c1} occurrence(s), replaced')
else:
    print('Hompatch: NOT FOUND')

# 2. Patch EmitLinearResolvedInvocation
old2 = '\t\t\tstring rawExpr = ConsumeEvalStackValueExpression();\n\t\t\tbool isRawArg = rawArgumentIndices.Contains(num);\n\t\t\tif (isRawArg)'
new2_lines = [
    '\t\t\tstring rawExpr = ConsumeEvalStackValueExpression();',
    '\t\t\tif (_activeStructuredSlotContext is { } ctx)',
    '\t\t\t{',
    '\t\t\t\tstring? reload = ctx.GetChaosLocalReload(rawExpr);',
    '\t\t\t\tif (reload != null)',
    '\t\t\t\t{',
    '\t\t\t\t\tbuilder.AppendLine($"{indentation}    {reload}");',
    '\t\t\t\t}',
    '\t\t\t}',
    '\t\t\tbool isRawArg = rawArgumentIndices.Contains(num);',
    '\t\t\tif (isRawArg)',
]
new2 = '\n'.join(new2_lines)
c2 = content.count(old2)
if c2 > 0:
    content = content.replace(old2, new2, 1)
    count += 1
    print(f'LinearResolved: found {c2} occurrence(s), replaced')
else:
    print('LinearResolved: NOT FOUND')

if count > 0:
    with open(sys.argv[1], 'w', encoding='utf-8') as f:
        f.write(content)
    print(f'OK: {count} patch(es) applied')
else:
    print('ERROR: no patches applied')
    sys.exit(1)
