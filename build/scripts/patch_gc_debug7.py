import re

with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\gc\gc_old_gen.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# Replace the markers inside the parallel-mark block
old = '            fprintf(stderr, "DBG: a1h_lock_mutex\\n"); fflush(stderr);\n            std::lock_guard<std::mutex> lock(mutex_);\n            fprintf(stderr, "DBG: a1i_reserve\\n"); fflush(stderr);\n            pages.reserve(static_cast<size_t>(page_count_));\n            fprintf(stderr, "DBG: page_list_walk_start\\n");'

new = '            fprintf(stderr, "DBG: a1h_lock_mutex\\n"); fflush(stderr);\n            std::lock_guard<std::mutex> lock(mutex_);\n            fprintf(stderr, "DBG: a1i_after_lock\\n"); fflush(stderr);\n            fprintf(stderr, "DBG: a1j_reserve_cap=%zu\\n", pages.capacity()); fflush(stderr);\n            pages.reserve(static_cast<size_t>(page_count_));\n            fprintf(stderr, "DBG: a1k_after_reserve_cap=%zu\\n", pages.capacity()); fflush(stderr);\n            fprintf(stderr, "DBG: page_list_walk_start\\n");'

count = content.count(old)
print(f'Found {count}')
if count == 1:
    content = content.replace(old, new, 1)

with open(r'D:\agent\booming-il2cpp\src\native\runtime-core\gc\gc_old_gen.cpp', 'w', encoding='utf-8', newline='') as f:
    f.write(content)
print('Done')
