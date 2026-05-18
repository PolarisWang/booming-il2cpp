"""Inject debug fprintf into Subject_0 in the generated file."""
import sys

path = sys.argv[1]
with open(path, 'rb') as f:
    data = f.read()

# Find the LAST occurrence (actual function body, not declaration)
marker = b'ConvertCharSubjects_ConvertCharSubjects_Subject_0(void)'
idx = data.rfind(marker)
assert idx > 0, "Subject_0 not found!"

brace = data.find(b'{', idx)
rest = data[brace:]
eol = rest.find(b'\n')
insert_pos = brace + eol + 1

# Build debug line byte by byte to avoid ALL escaping issues
debug_line = bytearray()
debug_line.extend(b'\tstd::fprintf(stderr, "ENTER Subject_0')
debug_line.append(0x5C)  # backslash
debug_line.append(0x6E)  # 'n'
debug_line.extend(b'");\r\n\tstd::fflush(stderr);\r\n')

new_data = data[:insert_pos] + bytes(debug_line) + data[insert_pos:]

# Also add cstdio include
if b'<cstdio>' not in new_data:
    old_include = b'#include <cstdint>'
    new_data = new_data.replace(old_include, old_include + b'\n#include <cstdio>', 1)

with open(path, 'wb') as f:
    f.write(new_data)

# Verify
verify_idx = new_data.find(b'ENTER Subject_0')
ctx = new_data[verify_idx-2:verify_idx+30]
print(f"Verification hex: {ctx.hex(' ')}")
assert b'ENTER Subject_0\\n"' in new_data, "BACKSLASH-N NOT PRESENT!"
print("OK: backslash-n confirmed in output")
