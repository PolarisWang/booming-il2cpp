"""
Task 2a: Catalog DRY violation fix.

Phase 1: Remove 94 repetitive GetRuntimeSkeleton*StubTemplate() methods from Catalog.cs
         (also GetAssemblyFullClosureRuntimeSkeletonSummaryTemplate/PageTemplate).

Phase 2: Replace all call sites with direct LoadTemplate calls.
"""
import pathlib
import re

# Note: Catalog.cs uses 4-space indentation throughout.
INDENT = '    '  # one level of indentation

REPO = pathlib.Path(__file__).resolve().parent.parent
CATALOG = REPO / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "ReferenceProof" / "NativeReferenceProofCatalog.cs"
EMITTER_DIR = REPO / "src" / "managed" / "Chaos.IL2CPP.CodeGen"

def main():
    method_to_const = {}
    catalog_text = CATALOG.read_text(encoding="utf-8")

    # Pattern matches GetRuntimeSkeleton*StubTemplate() methods with standard 3-line body
    # The naming convention: method name = "GetRuntimeSkeleton<X>StubTemplate",
    # const name = "RuntimeSkeleton<X>StubTemplateRelativePath" (same <X> suffix)
    method_pattern = re.compile(
        re.escape(INDENT) + r'public static Template GetRuntimeSkeleton(\w+)StubTemplate\(\)\n'
        + re.escape(INDENT) + r'\{\n'
        + re.escape(INDENT * 2) + r'return ScribanTemplateRenderer\.LoadTemplate\(RuntimeSkeleton\1StubTemplateRelativePath\);\n'
        + re.escape(INDENT) + r'\}\n'
    )

    matches = list(method_pattern.finditer(catalog_text))
    for m in matches:
        suffix = m.group(1)
        method_name = f"GetRuntimeSkeleton{suffix}StubTemplate"
        const_name = f"RuntimeSkeleton{suffix}StubTemplateRelativePath"
        method_to_const[method_name] = const_name

    print(f"Found {len(matches)} repetitive GetRuntimeSkeleton*StubTemplate methods")

    # Also include the 2 summary/page templates
    method_to_const["GetAssemblyFullClosureRuntimeSkeletonSummaryTemplate"] = \
        "AssemblyFullClosureRuntimeSkeletonSummaryTemplateRelativePath"
    method_to_const["GetAssemblyFullClosureRuntimeSkeletonPageTemplate"] = \
        "AssemblyFullClosureRuntimeSkeletonPageTemplateRelativePath"

    # Count call sites BEFORE removal
    total_calls_before = 0
    for cs_file in sorted(EMITTER_DIR.rglob("*.cs")):
        text = cs_file.read_text(encoding="utf-8")
        for method_name in method_to_const:
            total_calls_before += text.count(f"NativeReferenceProofCatalog.{method_name}()")
    print(f"Total call sites before: {total_calls_before}")

    # ---- Phase 1: Remove methods from Catalog.cs ----
    lines = catalog_text.split('\n')
    new_lines = []
    i = 0
    while i < len(lines):
        line = lines[i]

        # Check method start: "    public static Template GetRuntimeSkeleton<X>StubTemplate()"
        m = re.match(
            re.escape(INDENT) + r'public static Template (GetRuntimeSkeleton\w+StubTemplate)\(\)$',
            line
        )
        if m and m.group(1) in method_to_const:
            # Verify 3-line body
            if (i + 3 < len(lines)
                and lines[i+1] == INDENT + '{'
                and lines[i+2].startswith(INDENT * 2 + 'return ScribanTemplateRenderer.LoadTemplate(')
                and lines[i+3] == INDENT + '}'):
                i += 4
                continue

        # Check summary/page template methods
        m2 = re.match(
            re.escape(INDENT) + r'public static Template (GetAssemblyFullClosureRuntimeSkeleton(?:Summary|Page)Template)\(\)$',
            line
        )
        if m2 and m2.group(1) in method_to_const:
            if (i + 3 < len(lines)
                and lines[i+1] == INDENT + '{'
                and lines[i+2].startswith(INDENT * 2 + 'return ScribanTemplateRenderer.LoadTemplate(')
                and lines[i+3] == INDENT + '}'):
                i += 4
                continue

        new_lines.append(line)
        i += 1

    new_catalog = '\n'.join(new_lines)
    new_catalog = re.sub(r'\n{4,}', '\n\n\n', new_catalog)

    CATALOG.write_text(new_catalog, encoding="utf-8")
    print(f"Removed {len(method_to_const)} methods from Catalog.cs")

    # ---- Phase 2: Update all call sites ----
    total_replacements = 0
    changed_files = 0
    for cs_file in sorted(EMITTER_DIR.rglob("*.cs")):
        if "NativeReferenceProofCatalog.cs" in str(cs_file):
            continue

        original_text = cs_file.read_text(encoding="utf-8")

        any_match = False
        for method_name in method_to_const:
            if f"NativeReferenceProofCatalog.{method_name}()" in original_text:
                any_match = True
                break
        if not any_match:
            continue

        new_text = original_text
        for method_name, const_name in method_to_const.items():
            old = f"NativeReferenceProofCatalog.{method_name}()"
            new = f"ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.{const_name})"
            new_text = new_text.replace(old, new)

        # Count actual changes
        changes = 0
        for method_name in method_to_const:
            old = f"NativeReferenceProofCatalog.{method_name}()"
            changes += original_text.count(old)

        if changes > 0:
            cs_file.write_text(new_text, encoding="utf-8")
            print(f"  {cs_file.relative_to(EMITTER_DIR)}: {changes} replacements")
            total_replacements += changes
            changed_files += 1

    print(f"\nTotal: {total_replacements} replacements across {changed_files} files")

if __name__ == "__main__":
    main()
