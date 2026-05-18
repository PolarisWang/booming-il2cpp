import re

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'r', encoding='utf-8') as f:
    lines = f.readlines()

result = []
skip_continuation = False
brace_depth = 0
removed = 0

i = 0
while i < len(lines):
    line = lines[i]
    stripped = line.strip()

    # Detect start of LOG_DEBUG or LOG_DEBUG_M
    if 'CHAOS_IL2CPP_LOG_DEBUG' in stripped:
        removed += 1
        # Check if this is a multi-line macro (ends with comma)
        # or has arguments on the next line(s)
        if stripped.endswith(',,'):  # shouldn't happen but be safe
            pass
        # Count open/close parens to know if macro spans multiple lines
        open_parens = stripped.count('(') - stripped.count(')')
        if open_parens > 0:
            # Consume continuation lines until parens balance
            j = i + 1
            while j < len(lines) and open_parens > 0:
                cont = lines[j]
                removed += 1
                open_parens += cont.count('(') - cont.count(')')
                j += 1
            i = j
            continue
        i += 1
        continue

    # Remove COMPACT_DIAG printf lines
    if 'COMPACT_DIAG:' in stripped:
        removed += 1
        i += 1
        continue

    # Remove DBG fprintf(stderr) lines
    if stripped.startswith('fprintf(stderr, "DBG'):
        removed += 1
        i += 1
        continue

    # Remove standalone fflush(stdout) (from COMPACT_DIAG)
    if stripped == 'fflush(stdout);':
        removed += 1
        i += 1
        continue

    # Remove SWEEP diagnostic printf lines (not the SWEEP_DEADLOCK one)
    if stripped.startswith('printf("  SWEEP:') and 'DEADLOCK' not in stripped:
        removed += 1
        i += 1
        continue

    result.append(line)
    i += 1

print(f'Removed {removed} debug lines')
print(f'Before: {len(lines)} lines, After: {len(result)} lines')

with open('src/native/runtime-core/gc/gc_old_gen.cpp', 'w', encoding='utf-8') as f:
    f.writelines(result)
