"""Clean diagnostic: replace const-auto-comma-throw pattern with direct throw in Subject_0.
Hypothesis: MSVC corrupts EH table when throw is in `const auto = (throw..., value)` pattern."""
import sys

path = r'D:\agent\booming-il2cpp\testing\foundation-dll\System.Private.CoreLib\convert-char\codegen\ConvertCharSubjects\generated\native-aot.generated.cpp'
with open(path, 'rb') as f:
    data = f.read()

# Find Subject_0 body
func_marker = b'ConvertCharSubjects_ConvertCharSubjects_Subject_0(void)\r\n{'
func_idx = data.rfind(func_marker)

# Work within Subject_0 scope
sub = data[func_idx:]

# The original const-auto-throw-comma pattern (EXACT bytes)
# {const auto chaos_inline_result = (throw chaos_managed_exception{{reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr)}}, static_cast<CHAOS_IL2CPP_UINT16>(0));
# _s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_inline_result);}
#
# Note: the {{ }} in C++ source is from scriban template: {{ → { and }} → }
# So in the rendered C++: {reinterpret_cast<...>(nullptr)}
# But the C++ source file contains {{ }} as literal characters in some codegen versions

# Let's search for the pattern in multiple ways
candidates = [
    b'const auto chaos_inline_result = (throw chaos_managed_exception',
    b'const auto chaos_inline_result =',
    b'chaos_inline_result',  # fallback
]

for pat in candidates:
    pos = sub.find(pat)
    if pos >= 0:
        print(f"Found pattern '{pat[:40]}' at offset {pos} in Subject_0")
        break

# Find the ENTIRE const auto line (search from pos to the semicolon)
# Line ends with );  — the `;` after `static_cast<CHAOS_IL2CPP_UINT16>(0))`
line_start = sub.rfind(b'\n', 0, pos) + 1  # after \n
semi_pos = sub.find(b');', pos)
if semi_pos >= 0:
    line_end = semi_pos + 2  # include the );
else:
    line_end = sub.find(b'\n', pos)

old_line = sub[line_start:line_end]
print(f"Found old line: {old_line[:100]}")

# The next line should be: _s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_inline_result);
next_line_start = line_end
# Skip whitespace
while next_line_start < len(sub) and sub[next_line_start:next_line_start+1] in (b' ', b'\t', b'\r', b'\n'):
    next_line_start += 1

next_semi = sub.find(b';', next_line_start)
next_line = sub[next_line_start:next_semi+1] if next_semi > 0 else b''
print(f"Next line: {next_line[:80]}")

# Replacement: just throw chaos_managed_exception; set _s0 = 0;
# Keep same indentation
indent = old_line[:len(old_line) - len(old_line.lstrip())]
new_block = (
    indent + b'throw chaos_managed_exception{{reinterpret_cast<CHAOS_IL2CPP_INTPTR>(nullptr)}};\r\n'
    + indent + b'_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);\r\n'
)

# Calculate absolute positions
abs_line_start = func_idx + line_start
abs_line_end = func_idx + next_line_start + len(next_line)  # include next_line
old_block = data[abs_line_start:abs_line_end]
print(f"Old block length: {len(old_block)}, new block length: {len(new_block)}")

data = data[:abs_line_start] + new_block + data[abs_line_end:]

# Add ENTER + CATCH diagnostics
sub2 = data[func_idx:]  # re-slice
# ENTER diag after opening brace
brace_pos = sub2.find(b'{') + 1
eol = sub2.find(b'\n', brace_pos)
# Check if ENTER already there from previous injection
if b'ENTER Subject_0' not in data:
    enter_diag = b'\tstd::fprintf(stderr, "ENTER Subject_0\\n");\r\n\tstd::fflush(stderr);\r\n'
    data = data[:func_idx+brace_pos] + enter_diag + data[func_idx+brace_pos:]

# CATCH diag inside the catch handler
sub3 = data[func_idx:]
catch_pos = sub3.find(b'catch (const chaos_managed_exception& chaos_exception)')
if catch_pos >= 0 and b'CATCH Subject_0' not in data:
    brace2 = sub3.find(b'{', catch_pos)
    eol2 = sub3.find(b'\n', brace2)
    catch_diag = b'\t\t\tstd::fprintf(stderr, "CATCH Subject_0\\n");\r\n\t\t\tstd::fflush(stderr);\r\n\t\t\t'
    data = data[:func_idx+brace2+1] + catch_diag + data[func_idx+brace2+1:]

# Add cstdio include if missing
if b'<cstdio>' not in data:
    data = data.replace(b'#include <cstdint>', b'#include <cstdint>\n#include <cstdio>', 1)

with open(path, 'wb') as f:
    f.write(data)

# Verify
assert b'ENTER Subject_0' in data, "Missing ENTER"
assert b'CATCH Subject_0' in data, "Missing CATCH"
assert b'throw chaos_managed_exception' in data, "Missing throw"
print("Injections OK - rebuilding...")
