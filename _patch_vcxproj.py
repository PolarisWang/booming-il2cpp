"""Post-process .vcxproj to replace ExceptionHandling with /EHs /EHc-."""
import sys, re, shutil
from pathlib import Path

for vcxproj in Path(sys.argv[1]).rglob("*.vcxproj"):
    if "ALL_BUILD" in vcxproj.name or "CompilerId" in vcxproj.name or "VCTargets" in vcxproj.name:
        continue
    text = vcxproj.read_text(encoding="utf-8")

    # Remove all ExceptionHandling lines
    text = re.sub(r'\s*<ExceptionHandling>[^<]+</ExceptionHandling>\s*', '', text)

    # In ClCompile ItemDefinitionGroups, ensure /EHs /EHc- is in AdditionalOptions
    def add_ehs(m):
        block = m.group(0)
        if '/EHs' in block:
            return block  # already present
        # Add /EHs /EHc- to AdditionalOptions
        block = block.replace('/utf-8', '/utf-8 /EHs /EHc-')
        return block

    text = re.sub(
        r'<ClCompile>.*?</ClCompile>',
        add_ehs,
        text,
        flags=re.DOTALL
    )

    vcxproj.write_text(text, encoding="utf-8")
    print(f"  Patched: {vcxproj.name}")