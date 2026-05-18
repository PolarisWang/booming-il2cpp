# Fix: add COALESCE_DONE marker with correct line endings
import sys

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'rb') as f:
    data = f.read()

bk = chr(92).encode('ascii')  # single backslash

# Find CoalescePage in the sweep lambda (~line 2115)
idx = data.find(b'                CoalescePage(pg);')
if idx < 0:
    print("CoalescePage pattern not found!")
    sys.exit(1)

# Read the full line
line_start = data.rfind(b'\n', 0, idx) + 1
line_end = data.find(b'\n', idx)
line_content = data[line_start:line_end]
print(f"Line: {repr(line_content)}")

# Build marker line with same indentation
indent = line_content[:len(line_content) - len(line_content.lstrip())]
marker_line = indent + b'fprintf(stderr, "COL: COALESCE_DONE pg=%p\\n", (void*)pg); fflush(stderr);'

# Insert after CoalescePage(pg);
insert_pos = idx + len(b'                CoalescePage(pg);')
new_data = data[:insert_pos] + b'\n' + marker_line + data[insert_pos:]

# Verify no embedded newlines in the marker
v_idx = new_data.find(b'COALESCE_DONE')
if v_idx >= 0:
    chunk = new_data[v_idx:v_idx+80]
    print(f"Marker bytes: {repr(chunk)}")
    # The \\n should be backslash (0x5C) followed by n (0x6E)
    if b'\\n' in chunk:
        print("OK: found \\n escape")
    else:
        print("WARNING: \\n escape not found!")

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'wb') as f:
    f.write(new_data)
print("Done - added COALESCE_DONE marker")
