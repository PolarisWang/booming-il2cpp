"""Post-process generated native-aot.generated.cpp to add goto-based loop control
flow for methods whose structured IR recovery fails (br/blt emitted as NO-OP
comments).

Two loops need fixing:
1. ThreadedGcAllocation: wait-for-worker polling loop (counter < iterations/4)
2. b__0 (<ThreadedGcAllocation>b__0): GC allocation loop (i < iterations)
"""
import os
import re

FILE = r'D:/agent/booming-il2cpp/testing/project-test/runtime-self-test/codegen/RuntimeSelfTest/generated/native-aot.generated.cpp'

with open(FILE, 'rb') as f:
    data = f.read()

# Locate managed method comment boundaries
# Pattern: // Managed method: ... (preceded by \n, not \t)
fn_boundaries = {}
for m in re.finditer(rb'// Managed method: .+', data):
    fn_boundaries[m.start()] = m.group().decode('utf-8', errors='replace').strip()

marker_starts = sorted(fn_boundaries.keys())
print(f"Found {len(marker_starts)} managed method markers")

def find_marker_idx(marker_starts, substr):
    """Find the index in marker_starts whose marker name contains substr."""
    for i, pos in enumerate(marker_starts):
        name = fn_boundaries[pos]
        if substr in name:
            return i, pos
    return None, None

def get_section(marker_starts, start_idx):
    """Get byte range from a marker position to the next marker (or EOF)."""
    pos = marker_starts[start_idx]
    if start_idx + 1 < len(marker_starts):
        end = marker_starts[start_idx + 1]
    else:
        end = len(data)
    return pos, end

# Bytes patterns (file uses CRLF)
BR_PATTERN = b'\t// br (handled via structured EH branches)\r\n\t{'
BLT_PATTERN = b'\t// blt (structured EH branch)'

# =========================================================
# Fix 1: ThreadedGcAllocation wait loop
# =========================================================
tga_idx, tga_pos = find_marker_idx(marker_starts, 'ThreadedGcAllocation(System.Int32)')
assert tga_idx is not None, "ThreadedGcAllocation marker not found!"
tga_start, tga_end = get_section(marker_starts, tga_idx)

tga_section = data[tga_start:tga_end]
tga_br_cnt = tga_section.count(BR_PATTERN)
tga_blt_cnt = tga_section.count(BLT_PATTERN)
print(f"ThreadedGcAllocation: bytes [{tga_start}..{tga_end}) — {len(tga_section)} bytes, br={tga_br_cnt}, blt={tga_blt_cnt}")
assert tga_br_cnt == 1, f"ThreadedGcAllocation br count != 1: {tga_br_cnt}"
assert tga_blt_cnt == 1, f"ThreadedGcAllocation blt count != 1: {tga_blt_cnt}"

tga_new_br = b'\tchaos_wait_loop_br:\r\n\t{'
tga_new_blt = (
    b'\t// blt (replaced with goto check)\r\n'
    b'\tchaos_wait_loop_cond:\r\n'
    b'\t{\r\n'
    b'\t\tif (static_cast<CHAOS_IL2CPP_INT32>(_s0) < static_cast<CHAOS_IL2CPP_INT32>(_s1))\r\n'
    b'\t\t{\r\n'
    b'\t\t\tgoto chaos_wait_loop_br;\r\n'
    b'\t\t}\r\n'
    b'\t}'
)

tga_fixed = tga_section.replace(BR_PATTERN, tga_new_br, 1)
tga_fixed = tga_fixed.replace(BLT_PATTERN, tga_new_blt, 1)
print(f"  Fixed: {len(tga_fixed)} bytes (was {len(tga_section)})")

# =========================================================
# Fix 2: b__0 allocation loop
# =========================================================
b0_idx, b0_pos = find_marker_idx(marker_starts, '<ThreadedGcAllocation>b__0')
assert b0_idx is not None, "b__0 marker not found!"
b0_start, b0_end = get_section(marker_starts, b0_idx)

b0_section = data[b0_start:b0_end]
b0_br_cnt = b0_section.count(BR_PATTERN)
b0_blt_cnt = b0_section.count(BLT_PATTERN)
print(f"b__0: bytes [{b0_start}..{b0_end}) — {len(b0_section)} bytes, br={b0_br_cnt}, blt={b0_blt_cnt}")
assert b0_br_cnt == 1, f"b__0 br count != 1: {b0_br_cnt}"
assert b0_blt_cnt == 1, f"b__0 blt count != 1: {b0_blt_cnt}"

b0_new_br = b'\tchaos_b0_body:\r\n\t{'
b0_new_blt = (
    b'\t// blt (replaced with goto check)\r\n'
    b'\tchaos_b0_cond:\r\n'
    b'\t{\r\n'
    b'\t\tif (static_cast<CHAOS_IL2CPP_INT32>(_s0) < static_cast<CHAOS_IL2CPP_INT32>(_s1))\r\n'
    b'\t\t{\r\n'
    b'\t\t\tgoto chaos_b0_body;\r\n'
    b'\t\t}\r\n'
    b'\t}'
)

b0_fixed = b0_section.replace(BR_PATTERN, b0_new_br, 1)
b0_fixed = b0_fixed.replace(BLT_PATTERN, b0_new_blt, 1)
print(f"  Fixed: {len(b0_fixed)} bytes (was {len(b0_section)})")

# =========================================================
# Apply fixes (process end-to-start so positions stay valid)
# =========================================================
fixes = sorted([
    (tga_start, tga_section, tga_fixed),
    (b0_start, b0_section, b0_fixed),
], key=lambda x: -x[0])  # process higher offsets first

for start, old_sec, new_sec in fixes:
    assert data[start:start+len(old_sec)] == old_sec, \
        f"Section mismatch at {start}"
    data = data[:start] + new_sec + data[start + len(old_sec):]

with open(FILE, 'wb') as f:
    f.write(data)

print("\nDone! Wrote fixed file.")
print("  ThreadedGcAllocation: chaos_wait_loop_br + chaos_wait_loop_cond")
print("  b__0: chaos_b0_body + chaos_b0_cond")
