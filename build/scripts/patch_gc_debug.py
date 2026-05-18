import re

with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\gc\gc_old_gen.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# Target the EnumerateThreads lambda in Collect()
old = (
    '            [](threading::ManagedThread* thread) -> bool {\n'
    '                // Scan the shared young generation region directly.\n'
    '                // All threads share one young region; scan [begin, current).\n'
    '                Region* young_region = g_young_gen.region.load(std::memory_order_acquire);\n'
    '                if (young_region == nullptr) return true;\n'
    '                if (young_region->current > young_region->begin) {\n'
    '                    g_old_gen.ScanRangeForRoots(\n'
    '                        young_region->begin, young_region->current);\n'
    '                }\n'
    '                return true;'
)

new = (
    '            [](threading::ManagedThread* thread) -> bool {\n'
    '                fprintf(stderr, "DBG_ENUM: thread=%p is_running=%d\\n", (void*)thread, (int)thread->is_running); fflush(stderr);\n'
    '                // Scan the shared young generation region directly.\n'
    '                // All threads share one young region; scan [begin, current).\n'
    '                Region* young_region = g_young_gen.region.load(std::memory_order_acquire);\n'
    '                fprintf(stderr, "DBG_ENUM: region=%p\\n", (void*)young_region); fflush(stderr);\n'
    '                if (young_region == nullptr) return true;\n'
    '                fprintf(stderr, "DBG_ENUM: begin=%p current=%p\\n", (void*)young_region->begin, (void*)young_region->current); fflush(stderr);\n'
    '                if (young_region->current > young_region->begin) {\n'
    '                    g_old_gen.ScanRangeForRoots(\n'
    '                        young_region->begin, young_region->current);\n'
    '                }\n'
    '                return true;'
)

count = content.count(old)
print(f'Found {count} occurrences')
if count > 0:
    content = content.replace(old, new, 1)
    with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\gc\gc_old_gen.cpp', 'w', newline='') as f:
        f.write(content)
    print('Done - file updated')
else:
    # Find where EnumerateThreads is
    idx = content.find('EnumerateThreads')
    if idx >= 0:
        print(repr(content[idx-30:idx+400]))
