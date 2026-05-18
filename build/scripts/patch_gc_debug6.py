import re

with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\gc\gc_old_gen.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# 1. After POH block close
old1 = '\n    // Phase 2: Mark transitive closure.\n'
new1 = '\n    fprintf(stderr, "DBG: a1d_after_POH_block\\n"); fflush(stderr);\n    // Phase 2: Mark transitive closure.\n'
count1 = content.count(old1)
print(f'Phase2: {count1}')
if count1 == 1:
    content = content.replace(old1, new1, 1)

# 2. Before LOH check
old2 = '    if (g_loh.SegmentCount() > 0) {\n'
new2 = '    fprintf(stderr, "DBG: a1e_loh_segcount=%d\\n", g_loh.SegmentCount()); fflush(stderr);\n    if (g_loh.SegmentCount() > 0) {\n'
count2 = content.count(old2)
print(f'LOH: {count2}')
if count2 == 1:
    content = content.replace(old2, new2, 1)

# 3. Before parallel mark vector creation
old3 = '    if (page_count_ >= kMinPagesForParallelMark) {\n        // Snapshot page list into array for parallel mark.\n        std::vector<OldGenPage*> pages;'
new3 = '    fprintf(stderr, "DBG: a1f_pcount=%d\\n", page_count_); fflush(stderr);\n    if (page_count_ >= kMinPagesForParallelMark) {\n        fprintf(stderr, "DBG: a1g_create_vec\\n"); fflush(stderr);\n        std::vector<OldGenPage*> pages;'
count3 = content.count(old3)
print(f'Parallel: {count3}')
if count3 == 1:
    content = content.replace(old3, new3, 1)
elif count3 > 1:
    print('  WARNING: multiple matches, skipping')

# 4. Before mutex lock
old4 = '        {\n            std::lock_guard<std::mutex> lock(mutex_);\n            pages.reserve(static_cast<size_t>(page_count_));\n            fprintf(stderr, "DBG: page_list_walk_start\\n");'
new4 = '        {\n            fprintf(stderr, "DBG: a1h_lock_mutex\\n"); fflush(stderr);\n            std::lock_guard<std::mutex> lock(mutex_);\n            fprintf(stderr, "DBG: a1i_reserve\\n"); fflush(stderr);\n            pages.reserve(static_cast<size_t>(page_count_));\n            fprintf(stderr, "DBG: page_list_walk_start\\n");'
count4 = content.count(old4)
print(f'Lock: {count4}')
if count4 == 1:
    content = content.replace(old4, new4, 1)

with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\gc\gc_old_gen.cpp', 'w', encoding='utf-8', newline='') as f:
    f.write(content)
print('Done')
