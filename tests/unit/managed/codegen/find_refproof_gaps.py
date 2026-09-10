"""Find ReferenceProof files that might be testable."""
import xml.etree.ElementTree as ET
import sys

tree = ET.parse(sys.argv[1])
root = tree.getroot()
pkg = root.find('.//package[@name="Chaos.IL2CPP.Generator"]')
target = 'CodeGen'

for cls in pkg.findall('.//class'):
    fn = cls.get('filename', '')
    fn_norm = fn.replace('\\', '/')
    if 'CodeGen' not in fn_norm or '/obj/' in fn_norm:
        continue
    if 'ReferenceProof/' not in fn_norm and 'NativeReferenceProofEmitter.cs' not in fn_norm and 'RuntimeSkeleton' not in fn_norm and 'NativeReferenceLoweringPlanner' not in fn_norm:
        continue
    lines = cls.findall('.//line')
    if not lines:
        continue
    covered = sum(1 for l in lines if int(l.get('hits', '0')) > 0)
    t = len(lines)
    if covered > 0 and t <= 50:
        short_fn = fn_norm.split('Chaos.IL2CPP.Generator/')[-1]
        print(f'{covered}/{t} = {covered/t*100:.0f}%  {short_fn}')
