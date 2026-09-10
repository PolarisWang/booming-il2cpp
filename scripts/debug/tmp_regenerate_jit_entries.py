"""Regenerate runtime-entry.cpp with fixed JIT startup for failing families."""
import sys, shutil
from pathlib import Path

sys.path.insert(0, str(Path('testing/foundation-dll').resolve()))
# Patch __file__ for the imported module
import verification.orchestration.family_entrypoint as ep

jit_fail_families = [
    'delegate-core-invocation', 'exception-throw-diagnostics',
    'reflection-activation', 'reflection-assembly', 'reflection-binding',
    'reflection-field-property', 'reflection-generics', 'reflection-member-basics',
    'reflection-member-complete', 'reflection-module', 'reflection-parameters',
    'runtime-compiler-services', 'snapshot-prover', 'span-memory-buffers',
    'string-char-text-core', 'string-construction',
]

for slug in jit_fail_families:
    native_dir = Path(f'testing/foundation-dll/System.Private.CoreLib/{slug}/native')
    re_path = native_dir / 'runtime-entry.cpp'
    new_content = ep.generate_runtime_entry(is_jit=True)
    re_path.write_text(new_content, encoding='utf-8')

    pd_path = native_dir / 'runtime-patchdata.cpp'
    if not pd_path.exists():
        sentinel = (
            '// Sentinel: no .patchdata available (hotpatch dispatch disabled)\n'
            '#include <cstddef>\n#include <cstdint>\n'
            'extern const char* const kPatchDataHostClassName;\n'
            'extern const char* const kPatchDataHostClassName = "";\n'
            'extern const uint8_t kPatchData[];\n'
            'extern const uint8_t kPatchData[] = { 0x00 };\n'
            'extern const size_t kPatchDataSize;\n'
            'extern const size_t kPatchDataSize = 0u;\n'
        )
        pd_path.write_text(sentinel, encoding='utf-8')

    build_dir = native_dir / 'build'
    if build_dir.exists():
        shutil.rmtree(build_dir, ignore_errors=True)

    print(f'  Updated {slug}')

print(f'Done - all {len(jit_fail_families)} families regenerated')
