from __future__ import annotations

import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SKILLS_ROOT = REPO_ROOT / ".codex" / "skills"
FRONT_MATTER_RE = re.compile(r"^---\r?\n(.*?)\r?\n---", re.S)


class SkillFrontMatterTests(unittest.TestCase):
    def test_skill_markdown_files_are_utf8_without_bom_and_have_required_metadata(self) -> None:
        failures: list[str] = []

        for skill_file in sorted(SKILLS_ROOT.rglob("SKILL.md")):
            raw = skill_file.read_bytes()
            rel = skill_file.relative_to(REPO_ROOT)

            if raw.startswith(b"\xef\xbb\xbf"):
                failures.append(f"{rel}: UTF-8 BOM present")
                continue

            try:
                text = raw.decode("utf-8")
            except UnicodeDecodeError as exc:
                failures.append(f"{rel}: invalid UTF-8 ({exc})")
                continue

            match = FRONT_MATTER_RE.match(text)
            if match is None:
                failures.append(f"{rel}: missing YAML front matter")
                continue

            metadata = match.group(1)
            if re.search(r"(?m)^name:\s*.+$", metadata) is None:
                failures.append(f"{rel}: missing front matter name")
            if re.search(r"(?m)^description:\s*.+$", metadata) is None:
                failures.append(f"{rel}: missing front matter description")

        if failures:
            self.fail("\n".join(failures))


if __name__ == "__main__":
    unittest.main()
