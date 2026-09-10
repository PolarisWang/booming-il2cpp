import xml.etree.ElementTree as ET
import sys

tree = ET.parse(sys.argv[1])
root = tree.getroot()
pkg = root.find('.//package[@name="Chaos.IL2CPP.Generator"]')
target = 'CodeGen'

obj_lines = 0
obj_covered = 0
src_lines = 0
src_covered = 0

for cls in pkg.findall('.//class'):
    fn = cls.get('filename', '')
    if target not in fn:
        continue
    fn_norm = fn.replace('\\', '/')
    is_obj = '/obj/' in fn_norm
    for line in cls.findall('.//line'):
        hits = int(line.get('hits', '0'))
        if is_obj:
            obj_lines += 1
            if hits > 0:
                obj_covered += 1
        else:
            src_lines += 1
            if hits > 0:
                src_covered += 1

total_l = obj_lines + src_lines
total_c = obj_covered + src_covered

print(f'obj/: {obj_covered}/{obj_lines} = {obj_covered/obj_lines*100:.1f}%')
print(f'src/: {src_covered}/{src_lines} = {src_covered/src_lines*100:.1f}%')
print(f'Total: {total_c}/{total_l} = {total_c/total_l*100:.1f}%')
