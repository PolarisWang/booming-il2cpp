#!/usr/bin/env python3
"""Inject --profile-range handler code into build.py."""
with open('verification/stages/build.py', 'r', encoding='utf-8') as f:
    lines = f.readlines()

fixup_idx = None
for i, line in enumerate(lines):
    if '# ── Post-build fixups ──' in line:
        fixup_idx = i
        break

if fixup_idx is not None:
    # The handler function and CLI code as a single string
    inject_code = r'''
    # ── Inject --profile-range handler into generated runtime-entry.cpp ──
    _rp = ctx.native_dir / "runtime-entry.cpp"
    if _rp.exists():
        _code = _rp.read_text(encoding="utf-8", errors="replace")
        _marker = '    if (std::strcmp(argv[1], "--microbench") == 0) { ret = RunMicrobenchMode(); goto shutdown; }'
        if _marker in _code and 'profile-range' not in _code:
            _prefix = (
                '\n// --profile-range: profile contiguous AOT method indices'
                '\nstatic int RunProfileRangeMode(int start_idx, int end_idx) {'
                '\n    using namespace chaos::il2cpp::runtime_core;'
                '\n    if (start_idx < 0) start_idx = 0;'
                '\n    if (end_idx > kAotMethodCount) end_idx = kAotMethodCount;'
                '\n    const int kCount = end_idx - start_idx;'
                '\n    auto* entries = GetHotpatchEntries();'
                '\n    ProfileStoreInit(kCount);'
                '\n    for (int idx = start_idx; idx < end_idx; idx++) {'
                '\n        int64_t heap_before = chaos_gc_get_heap_size();'
                '\n        GetThreadProfileData().heap_before = heap_before;'
                '\n        __try {'
                '\n            ChaosDispatchMethodBenchDirect(entries, kAotMethodCount, idx);'
                '\n        } __except(EXCEPTION_EXECUTE_HANDLER) {'
                '\n        }'
                '\n        int64_t heap_after = chaos_gc_get_heap_size();'
                '\n        GetThreadProfileData().heap_after = heap_after;'
                '\n        FlushThreadProfileData(idx);'
                '\n    }'
                '\n    ProfileStoreFinalize();'
                '\n    ProfileEmitJson();'
                '\n    return 0;'
                '\n}'
                '\n'
                '\nint main(int argc, char* argv[]) {'
            )
            _cli = (
                '\n    if (std::strcmp(argv[1], "--profile-range") == 0) {'
                '\n        if (argc < 4) { puts("Usage: entry.exe --profile-range <start> <end>"); return 1; }'
                '\n        ret = RunProfileRangeMode(std::atoi(argv[2]), std::atoi(argv[3]));'
                '\n        goto shutdown;'
                '\n    }'
                '\n'
            )
            _code = _code.replace('int main(int argc, char* argv[]) {', _prefix)
            _code = _code.replace(_marker, _cli + _marker)
            _rp.write_text(_code, encoding="utf-8")
            print(f'  [build] Injected --profile-range handler')
''' + '    # ── Post-build fixups ──\n'
    lines[fixup_idx] = inject_code
    with open('verification/stages/build.py', 'w', encoding='utf-8') as f:
        f.writelines(lines)
    print('Injection code added')
else:
    print('Post-build fixups line not found')
