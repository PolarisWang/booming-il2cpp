# Add MINIMAL sweep-phase markers using correct escaping
import sys

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'rb') as f:
    data = bytearray(f.read())

eol = b'\r\n'

# 1. SWEEP_ENTER at top of SweepPage function
target1 = b'    if (page == nullptr) return 0;'
idx1 = data.find(target1, 10000)  # skip earlier occurrences
if idx1 >= 0:
    marker1 = b'    fprintf(stderr, "COL: SWEEP_ENTER pg=%p\\n", (void*)page); fflush(stderr);\r\n'
    data[idx1:idx1] = marker1
    print(f"SWEEP_ENTER at {idx1}")
else:
    print("SWEEP_ENTER target not found")

# 2. SWEEP_RETURN before SweepPage's return reclaimed
target2 = b'    page->sweep_lock.store(false, std::memory_order_release);\r\n    return reclaimed;\r\n}'
idx2 = data.find(target2)
if idx2 >= 0:
    marker2 = b'    fprintf(stderr, "COL: SWEEP_RETURN reclaimed=%llu\\n", (unsigned long long)reclaimed); fflush(stderr);\r\n'
    data[idx2:idx2] = marker2
    print(f"SWEEP_RETURN at {idx2}")
else:
    print("SWEEP_RETURN target not found")

# 3. P4_ENTER before Phase 4 sweep section in Collect()
target3 = b'    // Phase 4: Sweep all pages (parallel when beneficial).'
idx3 = data.find(target3)
if idx3 >= 0:
    line_start = data.rfind(eol, 0, idx3) + 2
    marker3 = b'    fprintf(stderr, "COL: P4_ENTER\\n"); fflush(stderr);\r\n'
    data[line_start:line_start] = marker3
    print(f"P4_ENTER at {line_start}")
else:
    print("P4_ENTER target not found")

# 4. PARALLEL_SWEEP before parallel sweep dispatch
target4 = b'        // Parallel sweep: dispatch pages via atomic index.'
idx4 = data.find(target4)
if idx4 >= 0:
    line_start = data.rfind(eol, 0, idx4) + 2
    marker4 = b'        fprintf(stderr, "COL: PARALLEL_SWEEP\\n"); fflush(stderr);\r\n'
    data[line_start:line_start] = marker4
    print(f"PARALLEL_SWEEP at {line_start}")
else:
    print("PARALLEL_SWEEP target not found")

# 5. PARALLEL_SWEEP_DONE after parallel sweep
target5 = b'        total_reclaimed = parallel_reclaimed.load(std::memory_order_relaxed);'
idx5 = data.find(target5)
if idx5 >= 0:
    next_line = data.find(eol, idx5) + 2
    marker5 = b'        fprintf(stderr, "COL: PARALLEL_SWEEP_DONE reclaimed=%llu\\n", (unsigned long long)total_reclaimed); fflush(stderr);\r\n'
    data[next_line:next_line] = marker5
    print(f"PARALLEL_SWEEP_DONE at {next_line}")
else:
    print("PARALLEL_SWEEP_DONE target not found")

# 6. P4_SWEEP_DONE before Phase 4 free pages section
target6 = b'    // Phase 4: Free decommissioned pages'
idx6 = data.find(target6)
if idx6 >= 0:
    line_start = data.rfind(eol, 0, idx6) + 2
    marker6 = b'    fprintf(stderr, "COL: P4_SWEEP_DONE\\n"); fflush(stderr);\r\n'
    data[line_start:line_start] = marker6
    print(f"P4_SWEEP_DONE at {line_start}")
else:
    print("P4_SWEEP_DONE target not found")

# 7. COLLECT_DONE before GcFireEvent(COLLECT_DONE)
target7 = b'    GcFireEvent(GcEvent::COLLECT_DONE);'
idx7 = data.find(target7)
if idx7 >= 0:
    line_start = data.rfind(eol, 0, idx7) + 2
    marker7 = b'    fprintf(stderr, "COL: COLLECT_DONE\\n"); fflush(stderr);\r\n'
    data[line_start:line_start] = marker7
    print(f"COLLECT_DONE at {line_start}")
else:
    print("COLLECT_DONE target not found")

# 8. Apply the num_words cap fix
target8 = b'    CHAOS_IL2CPP_SIZE num_words = bm.WordCount();\r\n    CHAOS_IL2CPP_SIZE slot = 0;'
idx8 = data.find(target8)
if idx8 >= 0:
    replacement8 = (
        b'    CHAOS_IL2CPP_SIZE num_words = bm.WordCount();\r\n'
        b'    // Cap to actual payload slots (bitmap_bytes includes poison guard).\r\n'
        b'    CHAOS_IL2CPP_SIZE max_sweep_slots = page->payload_size / sizeof(void*);\r\n'
        b'    if (num_words * 64 > max_sweep_slots) num_words = (max_sweep_slots + 63) / 64;\r\n'
        b'    CHAOS_IL2CPP_SIZE slot = 0;'
    )
    data[idx8:idx8+len(target8)] = replacement8
    print(f"num_words cap at {idx8}")
else:
    print("num_words cap target not found")

# Verify ALL markers use correct \\n
for name, pattern in [
    ("SWEEP_ENTER", b"SWEEP_ENTER"),
    ("SWEEP_RETURN", b"SWEEP_RETURN"),
    ("P4_ENTER", b"P4_ENTER"),
    ("PARALLEL_SWEEP", b"PARALLEL_SWEEP"),
    ("PARALLEL_SWEEP_DONE", b"PARALLEL_SWEEP_DONE"),
    ("P4_SWEEP_DONE", b"P4_SWEEP_DONE"),
    ("COLLECT_DONE", b"COLLECT_DONE"),
]:
    idx = data.find(pattern)
    if idx >= 0:
        chunk = data[idx:idx+80]
        # Check: must have \\n before closing quote
        q1 = chunk.find(b'"')  # first quote (should be right before or after pattern)
        q2 = chunk.find(b'"', q1 + 1)  # second quote
        segment = chunk[q1+1:q2]
        if b'\\n' in segment:
            print(f"  OK: {name} has \\\\n")
        else:
            print(f"  WARN: {name} missing \\\\n: {repr(chunk[:40])}")
    else:
        print(f"  MISSING: {name}")

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'wb') as f:
    f.write(data)

print("Done")
