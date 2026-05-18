# Apply ONLY the SweepPage num_words cap fix, nothing else
import sys

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'rb') as f:
    data = bytearray(f.read())

eol = b'\r\n'

# Find the exact target in the file
target = b'    CHAOS_IL2CPP_SIZE num_words = bm.WordCount();' + eol + b'    CHAOS_IL2CPP_SIZE slot = 0;'

idx = data.find(target)
if idx < 0:
    print("Target pattern not found!")
    # Try to find partial match for debugging
    partial = b'num_words = bm.WordCount();'
    pidx = data.find(partial)
    if pidx >= 0:
        print(f"Found partial at {pidx}")
        print(repr(data[pidx-20:pidx+60]))
    sys.exit(1)

replacement = (
    b'    CHAOS_IL2CPP_SIZE num_words = bm.WordCount();' + eol +
    b'    // Cap to actual payload slots (bitmap_bytes includes poison guard).' + eol +
    b'    CHAOS_IL2CPP_SIZE max_sweep_slots = page->payload_size / sizeof(void*);' + eol +
    b'    if (num_words * 64 > max_sweep_slots) num_words = (max_sweep_slots + 63) / 64;' + eol +
    b'    CHAOS_IL2CPP_SIZE slot = 0;'
)

data = data[:idx] + replacement + data[idx+len(target):]

# Verify num_words cap is present
if b'max_sweep_slots' in data:
    print("num_words cap fix applied successfully")
else:
    print("ERROR: fix not found in output!")
    sys.exit(1)

# Count diagnostic COL: lines to verify nothing else changed
col_count = data.count(b'COL:')
print(f"COL: markers in file: {col_count}")
orig_col = b'fprintf(stderr, "COL:'
col_markers = []
start = 0
while True:
    idx2 = data.find(orig_col, start)
    if idx2 < 0:
        break
    end = data.find(eol, idx2)
    col_markers.append(data[idx2:end].decode('utf-8', errors='replace'))
    start = end + 2
print(f"Found {len(col_markers)} COL markers (these are pre-existing, not from my changes)")
for m in col_markers:
    print(f"  {m}")

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'wb') as f:
    f.write(data)
print("Done")
