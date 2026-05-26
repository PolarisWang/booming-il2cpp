"""Inject debug fprintf into Subject_0's catch handler in the generated file."""
import sys

path = sys.argv[1]
with open(path, 'rb') as f:
    data = f.read()

# Find the catch handler inside Subject_0
# Look for "catch (const chaos_managed_exception& chaos_exception)"
catch_marker = b'catch (const chaos_managed_exception& chaos_exception)'
idx = data.find(catch_marker)
assert idx >= 0, "catch marker not found"

# But there might be multiple catch handlers - one for each Subject method
# We need the one that's inside Subject_0
# Subject_0's function body starts at the LAST occurrence of Subject_0 definition
func_marker = b'ConvertCharSubjects_ConvertCharSubjects_Subject_0(void)\r\n{'
func_idx = data.rfind(func_marker)
assert func_idx >= 0, "Subject_0 function body not found"

# Now find the catch handler AFTER func_idx
sub_data = data[func_idx:]
catch_idx = sub_data.find(catch_marker)
assert catch_idx >= 0, "catch handler not found inside Subject_0"

# Find the opening brace of the catch block
brace_start = sub_data.find(b'{', catch_idx)
assert brace_start >= 0, "catch opening brace not found"

# Insert fprintf after the brace
rest = sub_data[brace_start:]
eol = rest.find(b'\n')
insert_pos = brace_start + eol + 1

bs = bytes([0x5C])  # backslash
n = b'n'

debug_line = (b'\t\t\tstd::fprintf(stderr, "CATCH Subject_0 object_value=%lld'
              + bs + n + b'", (long long)chaos_exception.object_value);\r\n'
              b'\t\t\tstd::fflush(stderr);\r\n\t\t\t')

new_sub_data = sub_data[:insert_pos] + debug_line + sub_data[insert_pos:]
new_data = data[:func_idx] + new_sub_data

with open(path, 'wb') as f:
    f.write(new_data)

assert b'CATCH Subject_0' in new_data, "CATCH not found in output!"
print(f"OK: injected catch debug at offset {func_idx + insert_pos}")
print(f"File now {len(new_data)} bytes (was {len(data)} bytes)")
