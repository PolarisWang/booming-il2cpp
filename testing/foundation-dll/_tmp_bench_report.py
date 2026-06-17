import json, glob, os, sys

# Count how many families have Net8 data vs not
with_data = []
no_data = []
for fp in sorted(glob.glob('testing/foundation-dll/System.*/_dll/reports/latest/comparison-summary.json')):
    nfp = fp.replace(os.sep, '/')
    fam = nfp.split('/')[2]
    with open(fp) as fh:
        d = json.load(fh)
    total = d.get('totalMethods', 0)
    with8 = d.get('methodsWithNet8', 0)
    aot_mean = d.get('aggregate', {}).get('chaosAotVsNet8Pct', {}).get('mean', 'N/A')
    jit_mean = d.get('aggregate', {}).get('chaosJitVsNet8Pct', {}).get('mean', 'N/A')
    if with8 > 0:
        with_data.append((fam, total, with8, aot_mean, jit_mean))
    else:
        no_data.append((fam, total))

print('='*110)
print('FAMILIES WITH EXISTING BENCHMARK DATA (AOT/JIT vs Net8)')
print('='*110)
print(f'{"Family":35s} {"Methods":>8s} {"Net8":>6s} {"AOT%":>10s} {"JIT%":>10s}')
print('-'*110)
for fam, total, with8, aot, jit in sorted(with_data, key=lambda x: abs(x[3]) if isinstance(x[3], (int,float)) else 9999, reverse=True):
    aot_str = f'{aot:>+8.1f}%' if isinstance(aot, (int,float)) else f'{str(aot):>8s}'
    jit_str = f'{jit:>+8.1f}%' if isinstance(jit, (int,float)) else f'{str(jit):>8s}'
    print(f'{fam:35s} {total:>8d} {with8:>6d} {aot_str:>10s} {jit_str:>10s}')

print()
print('='*110)
print('FAMILIES WITHOUT Net8 BENCHMARK DATA (need fresh run)')
print('='*110)
for fam, total in sorted(no_data, key=lambda x: -x[1]):
    print(f'{fam:35s} {total:>8d} methods')
