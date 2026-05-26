#!/usr/bin/env python3
"""
Batch-update all verification families to use A1+A2 architecture:
  - CMakeLists.txt: include chaos_generated_module.cpp + generated include dir
  - runtime-entry.cpp: replace manual init with ChaosRuntimeHost + ChaosGeneratedModuleActivate

Run BEFORE pipeline_native_aot_runner.py to regen codegen for all families.
"""

import os
import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[5]
VERIFICATION = REPO_ROOT / "testing" / "foundation-dll" / "System.Private.CoreLib"

# Template for the ChaosRuntimeHost-based init block (replaces old manual init + FillExternalRuntimeStubs)
INIT_BLOCK = '''\
    ChaosRuntimeHost host;
    if (!host.Initialize("runtime-entry")) {
        return -1;
    }

    // Register the generated module (register_codegen + bootstrap_runtime).
    if (!ChaosGeneratedModuleActivate(&host)) {
        std::fprintf(stderr, "ChaosGeneratedModuleActivate failed\\n");
        return -1;
    }

'''

def family_slugs() -> list[str]:
    """Discover all families with native/runtime-entry.cpp."""
    slugs = []
    for d in sorted(VERIFICATION.iterdir()):
        if d.is_dir() and (d / "native" / "runtime-entry.cpp").exists():
            slugs.append(d.name)
    return slugs

def update_cmakelists(cmakelists: Path) -> bool:
    """Add chaos_generated_module.cpp support to CMakeLists.txt."""
    text = cmakelists.read_text(encoding="utf-8")
    changed = False

    # 1. Add CHAOS_MODULE_SOURCE glob after CHAOS_CODEGEN_CPP glob
    if "CHAOS_MODULE_SOURCE" not in text:
        text = text.replace(
            'file(GLOB CHAOS_CODEGEN_CPP "${CHAOS_CODEGEN_DIR}/*Subjects/generated/native-aot.generated.cpp")',
            'file(GLOB CHAOS_CODEGEN_CPP "${CHAOS_CODEGEN_DIR}/*Subjects/generated/native-aot.generated.cpp")\n'
            'file(GLOB CHAOS_MODULE_SOURCE "${CHAOS_CODEGEN_DIR}/*Subjects/generated/chaos_generated_module.cpp")',
        )
        changed = True

    # 2. Add ${CHAOS_MODULE_SOURCE} to CHAOS_ENTRY_SOURCES
    if "${CHAOS_MODULE_SOURCE}" not in text:
        text = text.replace(
            '${CHAOS_CODEGEN_CPP}',
            '${CHAOS_CODEGEN_CPP}\n    ${CHAOS_MODULE_SOURCE}',
        )
        changed = True

    # 3. Add generated include dirs
    if "CHAOS_GENERATED_DIRS" not in text:
        # Find the set() section and add GLOB for generated dirs before CHAOS_ENTRY_INCLUDES
        text = text.replace(
            'set(CHAOS_ENTRY_INCLUDES',
            'file(GLOB CHAOS_GENERATED_DIRS "${CHAOS_CODEGEN_DIR}/*/generated")\n'
            'set(CHAOS_ENTRY_INCLUDES',
        )
        changed = True

    # 4. Add CHAOS_GENERATED_DIRS to the includes list
    if "${CHAOS_GENERATED_DIRS}" not in text:
        # Add before the closing paren of CHAOS_ENTRY_INCLUDES
        text = text.replace(
            '    "${CHAOS_PROJECT_ROOT}/third_party/unordered_dense/include"\n)',
            '    "${CHAOS_PROJECT_ROOT}/third_party/unordered_dense/include"\n'
            '    ${CHAOS_GENERATED_DIRS}\n'
            ')',
        )
        changed = True

    if changed:
        cmakelists.write_text(text, encoding="utf-8")
        return True
    return False

def update_runtime_entry(entry_path: Path) -> bool:
    """Replace manual init with ChaosRuntimeHost in runtime-entry.cpp."""
    text = entry_path.read_text(encoding="utf-8")

    # Skip if already converted (ChaosRuntimeHost present)
    if "ChaosRuntimeHost" in text:
        return False

    # 1. Remove old extern declarations that ChaosRuntimeHost handles
    # kChaosExternalRuntimeFnTable, kChaosExternalRuntimeSubjects, kChaosExternalRuntimeCount
    text = re.sub(
        r'// kChaosExternalRuntimeFnTable is defined.*?'
        r'extern "C" void\* kChaosExternalRuntimeFnTable\[\];\n'
        r'extern "C" const char\* kChaosExternalRuntimeSubjects\[\];\n'
        r'extern "C" int32_t kChaosExternalRuntimeCount;\n',
        '', text, flags=re.DOTALL
    )

    # ChaosJitRegisterAll
    text = re.sub(
        r'// ChaosJitRegisterAll is defined.*?'
        r'extern "C" void ChaosJitRegisterAll\(\);\n',
        '', text, flags=re.DOTALL
    )

    # Codegen-emitted registration structs
    text = re.sub(
        r'// Codegen-emitted registration structs.*?'
        r'extern "C" const CodeRegistrationV0 chaos_codegen_code_registration;\n'
        r'extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration;\n'
        r'extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options;\n',
        '', text, flags=re.DOTALL
    )

    # 4. Remove FillExternalRuntimeStubs function definition (ChaosRuntimeHost handles this)
    fill_func_match = re.search(
        r'static void FillExternalRuntimeStubs\(\)\s*\{',
        text
    )
    if fill_func_match:
        # Find the matching closing brace at the function's indent level
        start = fill_func_match.start()
        brace_count = 0
        i = fill_func_match.end()
        # Skip to first '{' (should already be there)
        while i < len(text) and text[i] != '{':
            i += 1
        if i < len(text):
            brace_count = 1
            i += 1
            while i < len(text) and brace_count > 0:
                if text[i] == '{':
                    brace_count += 1
                elif text[i] == '}':
                    brace_count -= 1
                i += 1
            # Remove from 'static void' to after the closing '}'
            text = text[:fill_func_match.start()] + text[i:]

    # 5. Remove redundant blank lines created by removals
    text = re.sub(r'\n{3,}', '\n\n', text)

    # 6. Add #include for chaos_runtime_host.h after the existing includes
    # (insert before "// kAotMethodCount defined..." or wherever)
    text = re.sub(
        r'(#include "jit_registration\.h"\n)',
        r'\1#include "chaos_runtime_host.h"\n',
        text
    )

    # 4. Remove old init block in main() — from auto* bridge = ... to ChaosJitRegisterAll();
    # Match the entire init block including thread_attach, FillExternalRuntimeStubs, ChaosJitRegisterAll
    init_match = re.search(
        r'int main\(int argc, char\*\* argv\) \{.*?'
        r'auto\* bridge = chaos_codegen_get_bridge_v0\(\);\n'
        r'.*?'
        r'Register JIT methods',
        text, re.DOTALL
    )
    if init_match:
        # Find the exact end: "ChaosJitRegisterAll();" is the last line before RunMode
        end_match = re.search(r'ChaosJitRegisterAll\(\);\n\n', text[init_match.start():])
        if end_match:
            init_end = init_match.start() + end_match.end()
            rest = text[init_end:]
            text = text[:init_match.start()] + 'int main(int argc, char** argv) {\n' + INIT_BLOCK + rest
        else:
            print(f"    WARN: Could not find end of init block in {entry_path.name}")
            return False
    else:
        # Try simpler pattern: just find the init pattern without the comment
        init_match2 = re.search(
            r'int main\(int argc, char\*\* argv\) \{.*?'
            r'auto\* bridge = chaos_codegen_get_bridge_v0\(\)',
            text, re.DOTALL
        )
        if init_match2:
            end_match = re.search(r'ChaosJitRegisterAll\(\);\n\n', text[init_match2.start():])
            if end_match:
                init_end = init_match2.start() + end_match.end()
                rest = text[init_end:]
                text = text[:init_match2.start()] + 'int main(int argc, char** argv) {\n' + INIT_BLOCK + rest
            else:
                print(f"    WARN: ChaosJitRegisterAll not found after init in {entry_path.name}")
                return False
        else:
            print(f"    WARN: Could not find init pattern in {entry_path.name}")
            return False

    entry_path.write_text(text, encoding="utf-8")
    return True

def main():
    slugs = family_slugs()
    print(f"Found {len(slugs)} families with native/runtime-entry.cpp")

    cmake_updated = 0
    entry_updated = 0
    skipped = 0

    for slug in slugs:
        family_dir = VERIFICATION / slug
        cmakelists = family_dir / "native" / "CMakeLists.txt"
        entry_path = family_dir / "native" / "runtime-entry.cpp"
        patchdata = family_dir / "native" / "runtime-patchdata.cpp"

        print(f"  {slug}: ", end="", flush=True)

        changes = []

        # Update CMakeLists.txt
        if cmakelists.exists():
            if update_cmakelists(cmakelists):
                changes.append("CMakeLists.txt")
                cmake_updated += 1

        # Update runtime-entry.cpp
        if entry_path.exists():
            if update_runtime_entry(entry_path):
                changes.append("runtime-entry.cpp")
                entry_updated += 1
            else:
                skipped += 1

        if changes:
            print(f"updated ({', '.join(changes)})")
        else:
            print("already up-to-date or skipped")

    print(f"\nSummary:")
    print(f"  CMakeLists.txt updated: {cmake_updated}")
    print(f"  runtime-entry.cpp updated: {entry_updated}")
    print(f"  Skipped: {skipped}")

if __name__ == "__main__":
    main()
