#!/usr/bin/env python3
"""Apply template fixes for --profile-range support."""
import sys
sys.path.insert(0, '.')

BS = chr(92)  # backslash character

with open('src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Templates/TestProject.RuntimeEntry.cpp.scriban', 'r', encoding='utf-8') as f:
    content = f.read()

# 1. Add C2712 pragma after initial comments, before includes
prag = f'// MSVC C2712 suppression: __try/__except is safe here\n#pragma warning(disable: 2712)\n\n'
idx = content.find('#include <cstdio>')
content = content[:idx] + prag + content[idx:]

# 2. Add EH macro override after chaos_runtime_host.h
eh = f'\n// EH macros: try/catch instead of __try/__except\n'
eh += f'#undef CHAOS_EH_TRY\n#undef CHAOS_EH_CATCH_BEGIN\n#undef CHAOS_EH_END\n'
eh += f'#define CHAOS_EH_TRY               try {{\n'
eh += f'#define CHAOS_EH_CATCH_BEGIN       }} catch (...) {{\n'
eh += f'#define CHAOS_EH_END               }}\n\n'
idx = content.find('#include "chaos_runtime_host.h"')
idx = content.find('BSn', idx)
content = content[:idx] + eh + content[idx:]

# 3. Add RunProfileRangeMode function before int main(
func = f'''// --profile-range: profile contiguous AOT method indices
static int RunProfileRangeMode(int start_idx, int end_idx) {{
    using namespace chaos::il2cpp::runtime_core;
    if (start_idx < 0) start_idx = 0;
    if (end_idx > kAotMethodCount) end_idx = kAotMethodCount;
    const int kCount = end_idx - start_idx;
    auto* entries = GetHotpatchEntries();
    ProfileStoreInit(kCount);
    for (int idx = start_idx; idx < end_idx; idx++) {{
        int64_t heap_before = chaos_gc_get_heap_size();
        GetThreadProfileData().heap_before = heap_before;
#if defined(_WIN32)
        __try {{
            ChaosDispatchMethodBenchDirect(entries, kAotMethodCount, idx);
        }} __except(EXCEPTION_EXECUTE_HANDLER) {{
        }}
#else
        try {{
            ChaosDispatchMethodBenchDirect(entries, kAotMethodCount, idx);
        }} catch (...) {{
        }}
#endif
        int64_t heap_after = chaos_gc_get_heap_size();
        GetThreadProfileData().heap_after = heap_after;
        FlushThreadProfileData(idx);
    }}
    ProfileStoreFinalize();
    ProfileEmitJson();
    return 0;
}}

'''
main_marker = 'int main(int argc, char* argv[]) {'
content = content.replace(main_marker, func + main_marker, 1)

# 4. Add CLI handler BEFORE the LAST printf("Unknown flag:)
# Find the LAST occurrence
last_idx = content.rfind('printf("Unknown flag:')
if last_idx >= 0:
    # Find the start of this line
    line_start = content.rfind('BSn', 0, last_idx) + 1
    line_end = content.find('BSn', last_idx) + 1
    original_line = content[line_start:line_end]

    # The \\n in the printf needs to be preserved EXACTLY as two chars in the template
    # Scriban will output them as-is (no escape processing outside {{}})
    cli = f'''    if (std::strcmp(argv[1], "--profile-range") == 0) {{
        if (argc < 4) {{ printf("Usage: entry.exe --profile-range <start> <end>{BS}n"); return 1; }}
        ret = RunProfileRangeMode(std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }}

'''
    content = content[:line_start] + cli + content[line_start:]

with open('src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Templates/TestProject.RuntimeEntry.cpp.scriban', 'w', encoding='utf-8') as f:
    f.write(content)

# Verify
refs = content.count('profile-range') + content.count('RunProfileRangeMode')
print(f'References: {refs} (expected >= 3)')
if refs >= 3:
    print('TEMPLATE UPDATED SUCCESSFULLY')
else:
    print('FAILED')
    sys.exit(1)
