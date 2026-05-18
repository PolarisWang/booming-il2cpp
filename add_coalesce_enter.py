# Add COALESCE_ENTER marker at the top of CoalescePage function
import sys

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'rb') as f:
    data = f.read()

bk = chr(92).encode('ascii')  # single backslash

# Find "void MarkSweepOldGen::CoalescePage(OldGenPage* page) {"
search = b'void MarkSweepOldGen::CoalescePage(OldGenPage* page) {'
idx = data.find(search)
if idx < 0:
    print("CoalescePage definition not found!")
    sys.exit(1)

# Find the opening brace
brace_idx = data.find(b'{', idx)
# Find the end of the line with the brace
line_end = data.find(b'\n', brace_idx)
line_content = data[idx:line_end]
print(f"Line: {repr(line_content)}")

# Get the indentation of the first statement (line 1170 usually)
# Find the next line after {
next_line_start = line_end + 1
if data[next_line_start:next_line_start+1] == b'\r':
    next_line_start += 1
next_line_content = data[next_line_start:data.find(b'\n', next_line_start)]
print(f"Next line: {repr(next_line_content)}")

# Get indentation of the next line
indent = next_line_content[:len(next_line_content) - len(next_line_content.lstrip())]
print(f"Indent: {repr(indent)}")

# Build marker
eol = b'\r\n' if b'\r\n' in data else b'\n'
marker_line = indent + b'fprintf(stderr, "COL: COALESCE_ENTER pg=%p\\n", (void*)page); fflush(stderr);' + eol

# Insert after the function opening brace and newline
insert_pos = next_line_start  # start of the next line
new_data = data[:insert_pos] + marker_line + data[insert_pos:]

# Verify
v_idx = new_data.find(b'COALESCE_ENTER')
if v_idx >= 0:
    chunk = new_data[v_idx:v_idx+80]
    print(f"Marker: {repr(chunk)}")

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'wb') as f:
    f.write(new_data)
print("Done")
