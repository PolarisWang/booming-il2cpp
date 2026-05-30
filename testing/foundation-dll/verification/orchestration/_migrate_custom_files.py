"""
Batch migrate handwritten Custom.cs files from _exitCode to [Fact] + Assert pattern.
Pattern: catch { _exitCode = 1; } -> catch { } + [Fact] annotation + using

Usage: python _migrate_custom_files.py [--apply]
  --apply: actually apply changes (default: dry-run)
"""

import re
import shutil
import sys
from pathlib import Path


def find_repo_root() -> Path:
    """Find repo root by looking for .git or CLAUDE.md."""
    p = Path(__file__).resolve()
    for _ in range(10):
        if (p / ".git").is_dir() or (p / "CLAUDE.md").exists():
            return p
        p = p.parent
    raise RuntimeError("Cannot find repo root")


REPO_ROOT = find_repo_root()
FOUNDATION_DLL = REPO_ROOT / "testing" / "foundation-dll"


def find_handwritten_files() -> list[Path]:
    """Find all handwritten Custom.cs files with _exitCode."""
    result = []
    for p in sorted(FOUNDATION_DLL.rglob("handwritten/*.Custom.cs")):
        if "_exitCode" in p.read_text(encoding="utf-8"):
            result.append(p)
    return result


def add_using(text: str, using_stmt: str = "using Chaos.TestFramework;") -> str:
    """Add using statement if not present."""
    if using_stmt in text:
        return text
    lines = text.split("\n")
    last_using = -1
    for i, line in enumerate(lines):
        stripped = line.strip()
        if stripped.startswith("using ") and stripped.endswith(";"):
            last_using = i
    if last_using >= 0:
        lines.insert(last_using + 1, using_stmt)
    else:
        lines.insert(0, using_stmt)
    return "\n".join(lines)


def add_fact_annotations(text: str) -> str:
    """Add [Fact] attribute before each public static void CustomEntrySubject_N method."""
    return re.sub(
        r'^([ \t]*)public static void (CustomEntrySubject_\d+)\s*\(\)',
        r'\1[Fact]\n\1public static void \2()',
        text,
        flags=re.MULTILINE,
    )


def remove_exitcode_from_catch(text: str) -> str:
    """Replace catch { _exitCode = 1; } with catch { }."""
    text = re.sub(r'catch\s*\{\s*_exitCode\s*=\s*1\s*;\s*\}', 'catch { }', text)
    return text


def find_subjects_copy(handwritten_path: Path) -> Path | None:
    """Find the corresponding managed/subjects/Custom.cs copy."""
    try:
        rel = handwritten_path.relative_to(FOUNDATION_DLL)
        # rel = <assembly>/<family>/handwritten/<file>
        parts = rel.parts
        if len(parts) < 4:
            return None
        assembly, family = parts[0], parts[1]
        fname = parts[-1]
        subjects_path = FOUNDATION_DLL / assembly / family / "managed" / "subjects" / fname
        return subjects_path if subjects_path.exists() else None
    except ValueError:
        return None


def migrate_text(text: str) -> str:
    """Apply all migrations to text. Returns modified text."""
    text = remove_exitcode_from_catch(text)
    text = add_fact_annotations(text)
    text = add_using(text)
    return text


def migrate_file(path: Path, dry_run: bool = True) -> bool:
    """Migrate a single file. Returns True if changed."""
    original = path.read_text(encoding="utf-8")
    modified = migrate_text(original)
    if modified == original:
        return False
    if not dry_run:
        path.write_text(modified, encoding="utf-8")
    return True


def main():
    dry_run = "--apply" not in sys.argv

    files = find_handwritten_files()
    print(f"Found {len(files)} handwritten files with _exitCode")

    changed_hw = 0
    changed_subj = 0

    for hw_path in files:
        if migrate_file(hw_path, dry_run=dry_run):
            changed_hw += 1
            rel = hw_path.relative_to(REPO_ROOT)
            print(f"  [handwritten] {rel}")

        subj_path = find_subjects_copy(hw_path)
        if subj_path:
            if migrate_file(subj_path, dry_run=dry_run):
                changed_subj += 1
                rel = subj_path.relative_to(REPO_ROOT)
                print(f"  [subjects]   {rel}")

    action = "Dry-run" if dry_run else "Applied"
    print(f"\n{action}: {changed_hw} handwritten + {changed_subj} subjects files changed")
    return 0


if __name__ == "__main__":
    exit(main())
