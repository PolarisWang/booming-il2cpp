import json

p = 'testing/foundation-dll/System.Runtime.Serialization.Formatters/_dll/reports/latest/comparison-summary.json'
with open(p) as f:
    data = json.load(f)

chunks = data.get('perChunk', [])
print('Assembly:', data.get('assemblyName', '?'))
print('Total methods:', data.get('totalMethods', '?'))
print()

all_methods = []
for chunk in chunks:
    cname = chunk.get('slug', '?')
    for m in chunk.get('methods', []):
        chaos = m.get('chaosAotMs', 0) or 0
        net8 = m.get('net8Ms', 0) or 0
        sid = m.get('methodSubjectId', '?')
        err = m.get('net8Error', False)
        all_methods.append((chaos, net8, sid, cname, err))

all_methods.sort(key=lambda x: x[0], reverse=True)

print('Top 15 slowest methods (by chaosAotMs):')
for chaos, net8, sid, cname, err in all_methods[:15]:
    ratio = str(round(chaos/net8, 2)) + 'x' if net8 > 0 else 'N/A'
    err_str = ' [NET8_ERR]' if err else ''
    print(f'  {chaos:.4f}ms  net8={net8:.4f}ms  ratio={ratio}{err_str}')
    print(f'    {sid[:120]}')

print()
print('Total methods:', len(all_methods))
with_baseline = sum(1 for _, net8, _, _, _ in all_methods if net8 > 0)
print('With net8 baseline:', with_baseline)
avg_chaos = sum(m[0] for m in all_methods) / len(all_methods)
print('Avg chaosAotMs:', round(avg_chaos, 4))
